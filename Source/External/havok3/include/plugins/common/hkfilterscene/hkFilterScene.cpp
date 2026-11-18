/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkfilterscene/hkFilterScene.h>

#include <hkbase/basesystem/hkBaseSystem.h>
#include <hkbase/memory/hkScratchpad.h>
#include <hkbase/stream/hkStreambufFactory.h>
#include <hkserialize/util/hkBuiltinTypeRegistry.h>

#include <hkfiltermanager/hkFilterManagerInterface.h>

// Filters we want to expose in this dll:
#include <hkfilterscene/hkFilterSceneRemoveScale.h>
#include <hkfilterscene/hkFilterSceneTransform.h>
#include <hkfilterscene/hkFilterScenePlatformWriter.h>
#include <hkfilterscene/hkFilterSceneAlterMesh.h>

#define NUM_FILTERS 4
hkFilterDesc* g_filterDescs[NUM_FILTERS] = 
{
	&g_SceneRemoveScale,
	&g_SceneTransformDesc,
	&g_PlatformWriterDesc,
	&g_SceneAlterMeshDesc
};

// DLL main
HINSTANCE hInstance;
static BOOL CommonControlsInitialized = FALSE;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	switch ( fdwReason )
	{
		case DLL_PROCESS_ATTACH:
		{
			hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

			if (!CommonControlsInitialized) {

				CommonControlsInitialized = TRUE;
				InitCommonControls();			// Initialize Win32 controls

			}
			break;
		}

		case DLL_PROCESS_DETACH:
		{
			if (CommonControlsInitialized)
			{
				CommonControlsInitialized = FALSE;
			}
		}

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}		
	return TRUE;
}


// Require desc retrieval funcs:
__declspec( dllexport ) int GetNumFilters()
{
	return NUM_FILTERS;
}

__declspec( dllexport ) const hkFilterDesc* GetFilterDesc(int index)
{
	if ((index >= 0) && (index < NUM_FILTERS))
		return g_filterDescs[index];
	return HK_NULL;
}

__declspec( dllexport ) unsigned GetFilterVersion()
{
	return HK_FILTER_VERSION;
}

extern hkBool hkBaseSystemIsInitialized;
__declspec( dllexport ) void InitSystem( HINSTANCE managerModuleHandle, const hkFilterBaseSystemInfo* ptrs )
{
	if (ptrs->m_memory) ptrs->m_memory->addReference();
	hkMemory::replaceInstance(ptrs->m_memory);

	if (ptrs->m_threadMemory) ptrs->m_threadMemory->addReference();
	hkThreadMemory::replaceInstance(ptrs->m_threadMemory);

	if (ptrs->m_streamBufFactory) ptrs->m_streamBufFactory->addReference();
	hkStreambufFactory::replaceInstance(ptrs->m_streamBufFactory);

	if (ptrs->m_error) ptrs->m_error->addReference();
	hkError::replaceInstance(ptrs->m_error);

	if (ptrs->m_scratchPad) ptrs->m_scratchPad->addReference();
	hkScratchpad::replaceInstance(ptrs->m_scratchPad);

	hkBaseSystem::initSingletons();

	hkBaseSystemIsInitialized = true;
}

__declspec( dllexport ) void InitThread( hkFilterThreadData* ptrs )
{
	hkBaseSystem::initThread(ptrs->m_threadMemory);
	// can add any other per thread init here
}

__declspec( dllexport ) void QuitSystem()
{
	hkBaseSystem::quit();
}

// EXP-370
__declspec( dllexport ) hkMemory* GetAndReplaceMemoryManager (hkMemory *newMemory)
{
	hkMemory& curMem = hkMemory::getInstance();
	curMem.addReference(); // Add ref
	newMemory->addReference(); // as replace instance doesn't ref but does deref after
	hkMemory::replaceInstance(newMemory); 

	return &curMem;
}
// No type information used in this dll itself. Gets given the type info.
const hkTypeInfo* const hkBuiltinTypeRegistry::StaticLinkedTypeInfos[] = { HK_NULL };
const hkClass* const hkBuiltinTypeRegistry::StaticLinkedClasses[] = { HK_NULL };


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
