/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkpreview/hkPreview.h>
#include <hkpreview/convertToXml.h>

#include <hkpreview/createWorld.h>
#include <hkpreview/properties.h>

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStreambufFactory.h>
#include <hkbase/stream/hkStreamWriter.h>
#include <hkbase/stream/hkIstream.h>
#include <hkbase/basesystem/hkBaseSystem.h>
#include <hkbase/memory/impl/hkPoolMemory.h>
#include <hkbase/memory/hkScratchpad.h>
#include <hkbase/error/hkDefaultError.h>

#include <hkexport2/exportworld.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportaction.h>

#include <hkserialize/packfile/binary/hkBinaryPackfileWriter.h>
#include <hkserialize/packfile/xml/hkXmlPackfileWriter.h>
#include <hkserialize/util/hkStructureLayout.h>
#include <hkserialize/util/hkRootLevelContainer.h>
#include <hkserialize/util/hkBuiltinTypeRegistry.h>
#include <hkserialize/util/hkLoader.h>



#include <hkdynamics/world/hkWorld.h>
#include <hkdynamics/world/hkPhysicsSystem.h>

#include <hkutilities/serialize/hkPhysicsData.h>
#include <hkutilities/serialize/hkHavokSnapshot.h>

#include <hkpreview/RemoveFixedRigidBodyListener.h>

#include <windows.h>    // includes common dialog functionality
#include <commdlg.h>    // includes common dialog functionality
extern "C" int printf(const char* fmt, ...);

extern HINSTANCE g_hDLLModule;



struct NamedRules
{
	const char* m_name;
	const hkStructureLayout::LayoutRules* m_rule;
};

// Always keep these 2 in sync
static char* g_fileFilters = 
"Havok XML (*.xml)\0*.xml\0"\
"Havok Win32 (MSVC) Binary (*.hkx)\0*.hkx\0"\
"Havok Xbox (MSVC) Binary (*.hkx)\0*.hkx\0"\
"Havok PS2 (CW) Binary (*.hkx)\0*.hkx\0"\
"Havok PS2 (SN 3.1) Binary (*.hkx)\0*.hkx\0"\
"Havok PS2 (GCC 3.2) Binary (*.hkx)\0*.hkx\0"\
"Havok Linux (GCC 3.3) Binary (*.hkx)\0*.hkx\0"\
"Havok Linux (GCC 2.95) Binary (*.hkx)\0*.hkx\0"\
"Havok NGC (SN 3.9.3) Binary (*.hkx)\0*.hkx\0"\
"Havok NGC (CW) Binary (*.hkx)\0*.hkx\0"\
"Havok AMD64 (MSVC) Binary (*.hkx)\0*.hkx\0"\
"Havok Xbox 360 (MSVC) Binary (*.hkx)\0*.hkx\0"\
"Havok PS3 (GCC) Binary (*.hkx)\0*.hkx\0"\
"\0";

static NamedRules g_knownRules[] =
{
	{"xml",				HK_NULL	},
	{"win32:msvc",		&hkStructureLayout::MsvcWin32LayoutRules}, 
	{"xbox:msvc",		&hkStructureLayout::MsvcXboxLayoutRules}, 
	{"ps2:cw",			&hkStructureLayout::CwPs2LayoutRules}, 
	{"ps2:sn31",		&hkStructureLayout::Sn31Ps2LayoutRules}, 
	{"ps2:gcc32",		&hkStructureLayout::Gcc32Ps2LayoutRules}, 
	{"linux:gcc33",		&hkStructureLayout::Gcc33LinuxLayoutRules}, 
	{"linux:gcc295",	&hkStructureLayout::Gcc295LinuxLayoutRules}, 
	{"ngc:sn393",		&hkStructureLayout::Sn393NgcLayoutRules}, 
	{"ngc:cw",			&hkStructureLayout::CwNgcLayoutRules}, 
	{"amd64:msvc",		&hkStructureLayout::MsvcAmd64LayoutRules}, 
	{"xbox360:msvc",	&hkStructureLayout::Xbox360LayoutRules},
	{"ps3:gcc",			&hkStructureLayout::GccPs3LayoutRules}
};

// We keep track of the last format used, which we use as default
static int g_lastFormatUsed = 1; // XML at the beginning

bool selectFile(char* bufferOut, int bufSize, int& formatRule)
{
	if (bufSize<1)
	{
		return false;
	}

	if (formatRule<0)
	{
		formatRule = g_lastFormatUsed;
	}

	// The buffer needs to be initialized
	bufferOut[0]=0;

	OPENFILENAME ofname;

	ofname.lStructSize=sizeof(ofname);
	ofname.hwndOwner=NULL;
	ofname.hInstance=g_hDLLModule;
	ofname.lpstrFilter=g_fileFilters;
	ofname.lpstrCustomFilter=NULL;
	ofname.nMaxCustFilter=0;
	ofname.nFilterIndex=formatRule+1;
	ofname.lpstrFile=bufferOut;
	ofname.nMaxFile=bufSize;
	ofname.lpstrFileTitle=NULL;
	ofname.nMaxFileTitle=0;
	ofname.lpstrInitialDir=NULL;
	ofname.lpstrTitle="Save as Havok XML/Binary";
	ofname.Flags=OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofname.lpstrDefExt="xml";
	ofname.lCustData=0;

	if (GetSaveFileName(&ofname))
	{
		formatRule = ofname.nFilterIndex-1;
		return true;
	}
	else 
	{
		//DWORD err=CommDlgExtendedError();  // for debugging purposes
		return false;
	}

}



bool saveWorldToFile (ExportWorld* exportWorld, const char* fileName, int formatRule)
{
	if (!exportWorld) return false;

	// Get the filename
	char buffer[1000];

	if (!fileName || formatRule < 0)
	{
		if (selectFile(buffer,sizeof(buffer), formatRule))
		{
			fileName = buffer;
		}
		else
		{
			return false;
		}
	}

	// We open the file for writing.
	hkOstream output(fileName);
	if( output.isOk() == false )
	{
		HK_WARN(0x74807fd1, "Unable to open " << fileName << ", skipping");
		return false;
	}

	hkPhysicsData data;

	hkWorldCinfo* worldCinfo = new hkWorldCinfo();
	exportWorld->GetHKWorld()->getCinfo(*worldCinfo);
	data.setWorldCinfo(worldCinfo);
	worldCinfo->removeReference();

	hkPhysicsSystem* sys = exportWorld->GetHKWorld()->getWorldAsOneSystem();

	// Split up the systems into one system per ragdoll, etc
	hkPhysicsData::SplitPhysicsSystemsOutput newSystems;
	hkPhysicsData::splitPhysicsSystems(sys, newSystems);

	delete sys;

	if (newSystems.m_unconstrainedFixedBodies)
	{
		data.addPhysicsSystem(newSystems.m_unconstrainedFixedBodies);
		newSystems.m_unconstrainedFixedBodies->removeReference();
	}
	if (newSystems.m_unconstrainedKeyframedBodies)
	{
		data.addPhysicsSystem(newSystems.m_unconstrainedKeyframedBodies);
		newSystems.m_unconstrainedKeyframedBodies->removeReference();
	}
	if (newSystems.m_unconstrainedMovingBodies)
	{
		data.addPhysicsSystem(newSystems.m_unconstrainedMovingBodies);
		newSystems.m_unconstrainedMovingBodies->removeReference();
	}
	if (newSystems.m_phantoms)
	{
		data.addPhysicsSystem(newSystems.m_phantoms);
		newSystems.m_phantoms->removeReference();
	}
	for (int i = 0; i < newSystems.m_constrainedSystems.getSize(); ++i )
	{
		data.addPhysicsSystem(newSystems.m_constrainedSystems[i]);
		newSystems.m_constrainedSystems[i]->removeReference();
	}

	g_lastFormatUsed = formatRule;

	// Add this to our root level container object
	hkRootLevelContainer::NamedVariant physicsData;
	physicsData.m_name = "PreviewSaveWorld";
	physicsData.m_variant.m_class = &hkPhysicsDataClass;
	physicsData.m_variant.m_object = &data;

	hkRootLevelContainer container;
	container.m_numNamedVariants = 1;
	container.m_namedVariants = &physicsData;

	if (formatRule > 0)
	{
		RemoveFixedRigidBodyListener removeFixedRigidBodyListener;

		hkBinaryPackfileWriter pw;
		pw.setContents(&container, hkRootLevelContainerClass, &removeFixedRigidBodyListener );

		hkBinaryPackfileWriter::Options o;
		o.m_userTag = 0x0;
		o.m_layout = *g_knownRules[formatRule].m_rule;
		if ( pw.save( output.getStreamWriter(), o ) == HK_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{		
		RemoveFixedRigidBodyListener removeFixedRigidBodyListener;

		hkXmlPackfileWriter pw;
		pw.setContents(&container, hkRootLevelContainerClass, &removeFixedRigidBodyListener);
		hkPackfileWriter::Options o;
		if ( pw.save( output.getStreamWriter(), o ) == HK_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}


}


hkPhysicsSystem* createSystemFromNamedObjects(ExportWorld* exportWorld, const char** objectNames, int numObjects )
{
	if (numObjects<1 || !objectNames)
	{
		return HK_NULL;
	}

	hkPhysicsSystem* system = new hkPhysicsSystem();
	{
		for (int it=0; it<numObjects; it++)
		{
			const char* objectName = objectNames[it];

			const ExportRigidBody* exportRigidBody = exportWorld->GetRigidBody(objectName);
			if (exportRigidBody)
			{
				system->addRigidBody( exportRigidBody->GetHKRigidBody());
			}

			const ExportConstraint* exportConstraint = exportWorld->GetConstraint(objectName);
			if (exportConstraint)
			{
				system->addConstraint( exportConstraint->GetHKConstraint());
			}
		}
	}
	return system;
}

bool saveObjectsToFile (ExportWorld* exportWorld, const char* fileName, int formatRule, const char** objectNames, int numObjects )
{
	if (!exportWorld || (numObjects<1) || !objectNames)
	{
		return false;
	}

	char buffer[1000];

	if (!fileName || formatRule < 0)
	{
		if (selectFile(buffer,sizeof(buffer), formatRule))
		{
			fileName = buffer; 
		}
		else
		{
			return false;
		}
	}

	// Open the file
	hkOstream output(fileName);
	if( output.isOk() == false )
	{
		HK_WARN(0x74807fd1, "Unable to open " << fileName << ", skipping");
		return false;
	}

	hkPhysicsData data;
	hkPhysicsSystem* system = createSystemFromNamedObjects(exportWorld, objectNames, numObjects);
	data.addPhysicsSystem(system);
	system->removeReference();


	g_lastFormatUsed = formatRule;
	// Add this to our root level container object
	hkRootLevelContainer::NamedVariant physicsData;
	physicsData.m_name = "PreviewSaveObject";
	physicsData.m_variant.m_class = &hkPhysicsDataClass;
	physicsData.m_variant.m_object = &data;

	hkRootLevelContainer container;
	container.m_numNamedVariants = 1;
	container.m_namedVariants = &physicsData;

	if (formatRule > 0)
	{
		RemoveFixedRigidBodyListener removeFixedRigidBodyListener;

		hkBinaryPackfileWriter pw;
		pw.setContents(&container, hkRootLevelContainerClass, &removeFixedRigidBodyListener);

		hkBinaryPackfileWriter::Options o;
		o.m_userTag = 0x0;
		o.m_layout = *g_knownRules[formatRule].m_rule;
		if ( pw.save( output.getStreamWriter(), o ) == HK_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{		
		RemoveFixedRigidBodyListener removeFixedRigidBodyListener;

		hkXmlPackfileWriter pw;
		pw.setContents(&container, hkRootLevelContainerClass, &removeFixedRigidBodyListener);
		hkPackfileWriter::Options o;
		if ( pw.save( output.getStreamWriter(), o ) == HK_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

// Given a format string, returns the layout rule to follow; -1 if unknown
static int formatStrToRule(const char* format)
{
	if (!format)
	{
		return -1;
	}

	const int numKnownRules = sizeof(g_knownRules) / sizeof(NamedRules);
	for (int i=0; i<numKnownRules; i++)
	{
		if (0 == _stricoll(g_knownRules[i].m_name, format))
		{
			return i;
		}
	}

	return -1;
}


// DLL FUNCTIONS
extern bool g_dllInUse;

static void printfFunc(const char* str, void* vargs)
{
	printf(str, vargs);
}

bool convertHke (const char* hkeFileName, const char* outFileName , const char* format)
{
	if (g_dllInUse)
	{
		return false;
	}

	const int formatRule = formatStrToRule(format);

	g_dllInUse = true;

	hkBaseSystem::init( new hkPoolMemory(), HK_NULL, (hkErrorReportFunction)(printfFunc) );

	// We now initialize the stack area to 1Mb (fast temporary memory to be used by the engine).
	int stackSize = 0x100000;
	char* stackBuffer = hkAllocate<char>( stackSize, HK_MEMORY_CLASS_BASE);
	hkThreadMemory::getInstance().setStackArea( stackBuffer, stackSize);

	{
		ExportWorld* exportWorld = createWorldFromHke( hkeFileName );

		saveWorldToFile (exportWorld, outFileName, formatRule);

		delete exportWorld;
	}

	// Deallocate stack area
	hkThreadMemory::getInstance().setStackArea(0, 0);
	hkDeallocate(stackBuffer);

	hkBaseSystem::quit();
	g_dllInUse = false;

	return true;
}


bool convertHkeSelected (const char* hkeFileName, const char* outFileName, const char* format, const char** objectNames, int numObjects )
{
	if (g_dllInUse)
	{
		return false;
	}

	const int formatRule = formatStrToRule(format);

	g_dllInUse = true;
	hkBaseSystem::init( new hkPoolMemory(), HK_NULL, (hkErrorReportFunction)(printfFunc) );

	// We now initialize the stack area to 1Mb (fast temporary memory to be used by the engine).
	int stackSize = 0x100000;
	char* stackBuffer = hkAllocate<char>( stackSize, HK_MEMORY_CLASS_BASE);
	hkThreadMemory::getInstance().setStackArea( stackBuffer, stackSize);

	{
		ExportWorld* exportWorld = createWorldFromHke( hkeFileName );

		saveObjectsToFile(exportWorld, outFileName, formatRule, objectNames, numObjects);

		delete exportWorld;
	}

	// Deallocate stack area
	hkThreadMemory::getInstance().setStackArea(0, 0);
	hkDeallocate(stackBuffer);

	hkBaseSystem::quit();
	g_dllInUse = false;

	return true;
}

// DLL Deprecated functions; they forward to new functions
bool convertHkeToXml (const char* hkeFileName, const char* xmlFileName)
{
	return convertHke (hkeFileName, xmlFileName, "xml");
}

bool convertHkeToXmlSelected (const char* hkeFileName, const char* xmlFileName, const char** objectNames, int numObjects )
{
	return convertHkeSelected (hkeFileName, xmlFileName, "xml", objectNames, numObjects);
}

int getNumTypeInfoLists()
{
	return 1; // just the master built in one
}

void getTypeInfoList(int l, const hkTypeInfo* const *& infos, const hkClass* const *& classes)
{
	switch (l)
	{
	case 0: 

		// List of classes which are added to the default registry.
		// See classes.h and classes.cpp to see how to set this up.
		infos = hkBuiltinTypeRegistry::StaticLinkedTypeInfos;
		classes = hkBuiltinTypeRegistry::StaticLinkedClasses;
		break;

	default: infos = NULL; classes = NULL;
	}
}


void setBaseSystemPointers( hkMemory* memory, hkStreambufFactory* streamBufFactory, 
						   hkError* error, hkScratchpad* scratchPad )
{
	HK_ASSERT2(0xabba9f72, 0,"Mismatched hkpreview.dll version - Ensure you have installed up-to-date versions of the Havok Content Tools");
}

void getBaseSystemPointers( hkMemory** memory, hkStreambufFactory** streamBufFactory, 
						   hkError** error, hkScratchpad** scratchPad )
{
	HK_ASSERT2(0xabba9f72, 0, "Mismatched hkpreview.dll version - Ensure you have installed up-to-date versions of the Havok Content Tools");
}

void setBaseSystemPointers_v32(hkMemory* memory, hkThreadMemory* threadMemory, hkStreambufFactory* streamBufFactory, 
			hkError* error, hkScratchpad* scratchPad )
{

	// Cannot call setBaseSystemPointers since we need to initialize threadMemory before singletons

	extern hkBool hkBaseSystemIsInitialized;
	// cleanup what we have had so far so we can set in new ones cleanly
	if (hkBaseSystemIsInitialized)
	{
		hkBaseSystem::quit();
	}

	// set new ones
	if (memory) memory->addReference();
	hkMemory::replaceInstance(memory);

	if (threadMemory) threadMemory->addReference();
	hkThreadMemory::replaceInstance(threadMemory);

	if (streamBufFactory) streamBufFactory->addReference();
	hkStreambufFactory::replaceInstance(streamBufFactory);

	if (error) error->addReference();
	hkError::replaceInstance(error);

	if (scratchPad) scratchPad->addReference();
	hkScratchpad::replaceInstance(scratchPad);

	// init (if we can)
	if (memory)
	{
		hkBaseSystemIsInitialized = true;
		hkBaseSystem::initSingletons();
	}

}

void getBaseSystemPointers_v32(hkMemory** memory, hkThreadMemory** threadMemory, hkStreambufFactory** streamBufFactory, 
			hkError** error, hkScratchpad** scratchPad )
{
	*memory = &hkMemory::getInstance();
	*streamBufFactory = &hkStreambufFactory::getInstance();
	*error = &hkError::getInstance();
	*scratchPad = &hkScratchpad::getInstance();
	*threadMemory = hkThreadMemory::getInstancePtr(); // we cannot use getInstance() as it asserts if NULL (it usually is)
}

void initThreadData(hkThreadMemory* threadMemory)
{
	hkBaseSystem::initThread(threadMemory);
}


/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
