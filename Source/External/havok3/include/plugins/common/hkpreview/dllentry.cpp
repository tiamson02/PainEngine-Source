/* 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <windows.h>
#include <hkpreview/hkPreview.h>
#include <hkserialize/hkSerialize.h>


HINSTANCE g_hDLLModule;

bool g_dllInUse = false;

// Initializes predefined properties to defaults
void initGlobalProperties ()
{
	setGlobalProperty("hideWarnings", 0);
	setGlobalProperty("useDirectX", 0);
	setGlobalProperty("exporterType", 0);
	setGlobalProperty("simulationFrequency", 60);
	setGlobalProperty("csConversion", 0);
	setGlobalProperty("dllVersion", HKPREVIEW2_DLL_VERSION);
}

BOOL APIENTRY DllMain( HINSTANCE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
					 )
{

	g_hDLLModule = hModule;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			initGlobalProperties();
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

int getDllVersion()
{
	return HKPREVIEW2_DLL_VERSION;
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
