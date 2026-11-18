/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsogl/hkgraphicsogl.h> // precompiled header
//hkg
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgLightManager.h>
#include <hkgraphics/hkgMaterialManager.h>
//ogl
#include <hkgraphicsogl/hkgCameraOGL.h>
#include <hkgraphicsogl/hkgDisplayContextOGL.h>
#include <hkgraphicsogl/hkgDisplayObjectOGL.h>
#include <hkgraphicsogl/hkgFaceSetOGL.h>
#include <hkgraphicsogl/hkgFontOGL.h>
#include <hkgraphicsogl/hkgLightOGL.h>
#include <hkgraphicsogl/hkgMaterialOGL.h>
#include <hkgraphicsogl/hkgTextureOGL.h>
#include <hkgraphicsogl/hkgVertexSetOGL.h>
#include <hkgraphicsogl/hkgViewportOGL.h>
//ps2
#include <hkgraphicsogl/ps2/hkgWindowPS2.h>

extern void hkgSystemOglPs2Init();
void hkgSystemOglPs2Init()
{
	//hkg
	hkgDisplayWorld::create = hkgDisplayWorld::createDisplayWorld;
	hkgGeometry::create = hkgGeometry::createGeometry;
	hkgLightManager::create = hkgLightManager::createLightManager;
	hkgMaterialFaceSet::create = hkgMaterialFaceSet::createMaterialFaceSet;
	hkgMaterialManager::create = hkgMaterialManager::createMaterialManager;
	//ogl
	hkgCamera::create = hkgCameraOGL::createCameraOGL;
	hkgDisplayContext::create = hkgDisplayContextOGL::createDisplayContextOGL;
	hkgDisplayContext::s_maxNumLights = 8;
	hkgDisplayObject::create = hkgDisplayObjectOGL::createDisplayObjectOGL; 
	hkgFaceSet::create = hkgFaceSetOGL::createFaceSetOGL;
	hkgFont::create = hkgFontOGL::createFontOGL;
	hkgLight::create = hkgLightOGL::createLightOGL;
	hkgMaterial::create = hkgMaterialOGL::createMaterialOGL;
	hkgTexture::create = hkgTextureOGL::createTextureOGL;
	hkgVertexSet::create = hkgVertexSetOGL::createVertexSetOGL;
	hkgViewport::create = hkgViewportOGL::createViewportOGL;
	//ps2gl
	hkgWindow::create = hkgWindowPS2::createWindowPS2;

	hkBaseSystem::init();
}

extern void hkgSystemOglPs2Quit();
void hkgSystemOglPs2Quit()
{
	hkBaseSystem::quit();
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
