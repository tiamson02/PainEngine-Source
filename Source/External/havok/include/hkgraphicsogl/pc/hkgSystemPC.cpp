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
//win32
#include <hkgraphicsogl/pc/hkgWindowPC.h>
#include <hkbase/error/hkDefaultError.h>

#pragma comment(lib,"opengl32.lib")

class hkWindowsError : public hkDefaultError
{
	public:

		virtual void assertMessage(const char* what, const char* description, const char* file, int line)
		{
			popupMessage(what, description, file, line);
		}
		
		virtual void errorMessage(const char* what, const char* description, const char* file, int line)
		{
			popupMessage(what, description, file, line);
		}

	protected:

		virtual void popupMessage(const char* what, const char* description, const char* file, int line)
		{
			if( isDisabled(file, line) == false )
			{
				hkInplaceStringStreambuf<> buf;
				hkOstream os(&buf);
				os	<< what << "\n\n" << description 
					<< "\n\nin " << file << ":" << line
					<< "\n\nAbort to debug\nRetry to continue anyway\nIgnore to always ignore this assert";
				int dowhat = MessageBox(0, buf.getString(), what, MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_APPLMODAL | MB_TOPMOST);
				switch( dowhat )
				{
					case IDABORT:
						__asm { int 3 }
						break;
					case IDRETRY:
						break;
					case IDIGNORE:
						disableAssert(file, line);
						break;
				}
			}
		}
};

extern void hkgSystemOglPcInit();
void hkgSystemOglPcInit()
{
	hkError::replaceInstance( new hkWindowsError() );
	
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
	//win32
	hkgWindow::create = hkgWindowPC::createWindowPC;

	hkBaseSystem::init();
}

extern void hkgSystemOglPcQuit();
void hkgSystemOglPcQuit()
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
