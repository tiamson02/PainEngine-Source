/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_DX8_PRECOMPILE_H
#define HK_GRAPHICS_DX8_PRECOMPILE_H


#if defined(_XBOX)

#	include <xtl.h>

#else 

#	define STRICT 1
#	define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#	define _WIN32_WINNT 0x0400	  // nt4
#	define _WIN32_WINDOWS 0x0500 // win 98 (not right, but has to be over 0x0400
#	define DIRECT3D_VERSION         0x0800
#	define DIRECTINPUT_VERSION		 0x0800

#	include <windows.h>
#	include <d3d8.h>

#endif

#include <hkgraphics/hkGraphics.h> // the source header for the hkg precompiles

#endif //HK_GRAPHICS_OGL_PRECOMPILE_H

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
