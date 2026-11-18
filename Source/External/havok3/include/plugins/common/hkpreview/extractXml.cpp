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
#include <hkbase/basesystem/hkBaseSystem.h>
#include <hkbase/error/hkDefaultError.h>
#include <hkbase/stream/impl/hkArrayStreamWriter.h>
#include <hkbase/memory/impl/hkPoolMemory.h>

#include <hkexport2/exportworld.h>
#include <hkexport2/exportrigidbody.h>
#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportaction.h>

#include <hkserialize/util/hkStructureLayout.h>
#include <hkserialize/packfile/binary/hkBinaryPackfileWriter.h>
#include <hkserialize/packfile/xml/hkXmlPackfileWriter.h>
#include <hkserialize/util/hkVtableClassRegistry.h>

#include <hkdynamics/world/hkWorld.h>
#include <hkdynamics/world/hkPhysicsSystem.h>

#include <hkutilities/serialize/hkPhysicsData.h>
#include <hkutilities/serialize/hkHavokSnapshot.h>

#include <hkbase/keycode.cxx>

#include <hkpreview/RemoveFixedRigidBodyListener.h>

#include <string.h>
extern "C" int printf(const char* fmt, ...);

extern bool g_dllInUse;

static ExportWorld* g_exportWorld = HK_NULL;
static hkSerializerSession* g_currentSession = HK_NULL;
static char* g_stackBuffer = HK_NULL;

static void printfFunc(const char* str, void* vargs)
{
	printf(str, vargs);
}

// Version 2.3+ : Methods to retrieve XML for havok objects from HKE
bool beginXmlExtraction (const char* hkeFileName)
{
	if (g_dllInUse)
	{
		return false;
	}

	g_dllInUse = true;
	hkBaseSystem::init( new hkPoolMemory(), HK_NULL, (hkErrorReportFunction)(printfFunc) );

	// We now initialize the stack area to 1Mb (fast temporary memory to be used by the engine).
	int stackSize = 0x100000;
	char* g_stackBuffer = hkAllocate<char>( stackSize, HK_MEMORY_CLASS_BASE);
	hkThreadMemory::getInstance().setStackArea( g_stackBuffer, stackSize);

	g_exportWorld = createWorldFromHke (hkeFileName);

	if (!g_exportWorld)
	{
		hkBaseSystem::quit();
		g_dllInUse = false;

		return false;
	}


	// We leave the DLL in use (g_dllInUse = true) so no other calls to the DLL can interfere
	return true;

}

bool extractXmlFromObjects (const char** objectNames, int numObjects, char** xmlOut)
{
	if (!g_exportWorld)
	{
		return false;
	}
	hkArray<char> buf;
	hkArrayStreamWriter sw(&buf, hkArrayStreamWriter::ARRAY_BORROW );

	RemoveFixedRigidBodyListener removeFixedRigidBodyListener;

	hkXmlPackfileWriter pw;
	hkPhysicsSystem* system = createSystemFromNamedObjects( g_exportWorld, objectNames, numObjects );
	pw.setContents(system, hkPhysicsSystemClass, &removeFixedRigidBodyListener );
	hkPackfileWriter::Options o;
	if ( pw.save( &sw, o ) != HK_SUCCESS )
	{
		return false;
	}

	buf.pushBack(0);

	// Now, we want to ignore the "types" section and only return the "data" section
	const char* dataSection = ::strstr(buf.begin(),"<hksection name=\"__data__\">");

	// Check; in case we fail we return everything
	if (dataSection)
	{
		*xmlOut = _strdup(dataSection);
	}
	else
	{
		*xmlOut = _strdup(buf.begin());
	}

	return true;
}

bool endXmlExtraction ()
{
	if (!g_exportWorld)
	{
		return false;
	}

	delete g_exportWorld;
	g_exportWorld = HK_NULL;

	// Deallocate stack area
	hkThreadMemory::getInstance().setStackArea(0, 0);
	hkDeallocate(g_stackBuffer);

	// Unlock the DLL
	hkBaseSystem::quit();
	g_dllInUse = false;

	return true;
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
