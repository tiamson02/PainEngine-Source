/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled header

//hkg
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgLightManager.h>
#include <hkgraphics/hkgMaterialManager.h>
//dx8
#include <hkgraphicsdx8/hkgCameraDX8.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>
#include <hkgraphicsdx8/hkgDisplayObjectDX8.h>
#include <hkgraphicsdx8/hkgFaceSetDX8.h>
#include <hkgraphicsdx8/hkgFontDX8.h>
#include <hkgraphicsdx8/hkgLightDX8.h>
#include <hkgraphicsdx8/hkgMaterialDX8.h>
#include <hkgraphicsdx8/hkgTextureDX8.h>
#include <hkgraphicsdx8/hkgVertexSetDX8.h>
#include <hkgraphicsdx8/hkgViewportDX8.h>
//win32
#include <hkgraphicsdx8/pc/hkgWindowDX8PC.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d8.lib")
#pragma comment(lib,"dinput8.lib")

extern void hkgSystemDx8PcInit();
void hkgSystemDx8PcInit()
{
	//hkg
	hkgDisplayWorld::create = hkgDisplayWorld::createDisplayWorld;
	hkgGeometry::create = hkgGeometry::createGeometry;
	hkgLightManager::create = hkgLightManager::createLightManager;
	hkgMaterialFaceSet::create = hkgMaterialFaceSet::createMaterialFaceSet;
	hkgMaterialManager::create = hkgMaterialManager::createMaterialManager;
	//ogl
	hkgCamera::create = hkgCameraDX8::createCameraDX8;
	hkgDisplayContext::create = hkgDisplayContextDX8::createDisplayContextDX8;
	hkgDisplayContext::s_maxNumLights = 8;
	hkgDisplayObject::create = hkgDisplayObjectDX8::createDisplayObjectDX8; 
	hkgFaceSet::create = hkgFaceSetDX8::createFaceSetDX8;
	hkgFont::create = hkgFontDX8::createFontDX8;
	hkgLight::create = hkgLightDX8::createLightDX8;
	hkgMaterial::create = hkgMaterialDX8::createMaterialDX8;
	hkgTexture::create = hkgTextureDX8::createTextureDX8;
	hkgVertexSet::create = hkgVertexSetDX8::createVertexSetDX8;
	hkgViewport::create = hkgViewportDX8::createViewportDX8;
	//win32
	hkgWindow::create = hkgWindowDX8PC::createWindowDX8PC;
}

extern void hkgSystemDx8PcQuit();
void hkgSystemDx8PcQuit()
{
}

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
