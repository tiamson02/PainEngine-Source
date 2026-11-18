/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h>
#include <hkgraphics/hkgWindow.h>
#include <hkgraphics/hkgFont.h>
#include <hkgraphics/hkgTexture.h>
#include <hkgraphics/hkgCamera.h>
#include <hkgraphics/hkgDisplayObject.h>
#include <hkgraphics/hkgFaceSet.h>
#include <hkgraphics/hkgVertexSet.h>
#include <hkgraphics/hkgDisplayContext.h>
#include <hkgraphics/hkgLight.h>
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgMaterialManager.h>
#include <hkgraphics/hkgLightManager.h>
#include <hkgraphics/hkgMaterialFaceSet.h>


class hkgWindowNull : public hkgWindow
{
	public:

		static hkgWindow* createWindowNull()
		{
			return new hkgWindowNull;
		}

		bool clearBuffers() { return true; }
		bool swapBuffers() { return true; }
		int peekMessages(bool) { return HKG_WINDOW_MSG_CONTINUE; }
};

class hkgDisplayContextNull : public hkgDisplayContext
{
	public:

		static hkgDisplayContext* createDisplayContextNull(hkgWindow* owner)
		{
			return new hkgDisplayContextNull(owner);
		}	

		void setLightingState(bool) {}
		void setTexture2DState(bool) {}
		void setBlendState(bool) {}
		void setDepthReadState(bool) {}
		void setDepthWriteState(bool) {}
		void setCullFaceState(bool) {}
		void setWireframeState(bool) {}
		void setLightState(int,bool) {}
		void pushMatrix() {}
		void popMatrix() {}
		void beginGroup(unsigned short) {}
		void setCurrentColor3(float []) {}
		void setCurrentColor4(float []) {}
		void setCurrentColorPacked(unsigned int) {}
		void setCurrentTextureCoord(float []) {}
		void setCurrentNormal(float []) {}
		void setCurrentPosition(float []) {}
		void endGroup(void) {}
		void flush(void) {}
		void multMatrix(float []) {}

	protected:

		hkgDisplayContextNull(hkgWindow* owner)
			: hkgDisplayContext(owner)
		{
		}		
};

class hkgViewportNull : public hkgViewport
{
	public:
		
		static hkgViewport* createViewportNull(hkgWindow* owner)
		{
			return new hkgViewportNull(owner);
		}	
	
	protected:

		hkgViewportNull(hkgWindow* owner)
			: hkgViewport(owner)
		{
		}
};


class hkgCameraNull : public hkgCamera
{
	public:
		
		static hkgCamera* createCameraNull()
		{
			return new hkgCameraNull();
		}	

		void setAsCurrent(class hkgDisplayContext *) {}
		
	protected:
		
		hkgCameraNull()
			: hkgCamera(hkgCamera::DEFAULT_FROM, hkgCamera::DEFAULT_TO, hkgCamera::DEFAULT_UP, hkgCamera::DEFAULT_FOV)
		{
		}
};

class hkgFontNull : public hkgFont
{
	public:
		
		static hkgFont* createFontNull()
		{
			return new hkgFontNull();
		}	
		
		void render(class hkgDisplayContext *,const char *,int,int,float []) {}
		
	protected:
		
		hkgFontNull()
		{
		}
};

class hkgTextureNull : public hkgTexture
{
	public:
		
		static hkgTexture* createTextureNull(hkgDisplayContext* context)
		{
			return new hkgTextureNull(context);
		}	
		
		
	protected:
		
		hkgTextureNull(hkgDisplayContext* context)
		{
		}
};

class hkgMaterialNull : public hkgMaterial
{
	public:
		
		static hkgMaterial* createMaterialNull()
		{
			return new hkgMaterialNull;
		}	
		
		
	protected:
		
		hkgMaterialNull()
		{
		}
};

class hkgLightNull : public hkgLight
{
	public:
		
		static hkgLight* createLightNull()
		{
			return new hkgLightNull;
		}	
		
		void updateState(class hkgDisplayContext *) {}
		
	protected:
		
		hkgLightNull()
		{
		}
};

class hkgDisplayObjectNull : public hkgDisplayObject
{
	public:
		
		static hkgDisplayObject* createDisplayObjectNull()
		{
			return new hkgDisplayObjectNull;
		}	
		
	protected:
		
		hkgDisplayObjectNull()
		{
		}
};

class hkgVertexSetNull : public hkgVertexSet
{
	public:
		
		static hkgVertexSet* createVertexSetNull(hkgFaceSet* owner)
		{
			return new hkgVertexSetNull(owner);
		}	
		
	protected:
		
		hkgVertexSetNull(hkgFaceSet* owner)
			: hkgVertexSet(owner)
		{
		}

		void setNumVerts(int,bool,bool) {}

		unsigned char *lock(int,int) { return 0; }

		void unlock(int,int) {}

		void setStreams(class hkgDisplayContext *) {}

		void addPosInfo(const float *,int) {}

		void addNormalInfo(const float *,int) {}

		void addUVInfo(const float *,int) {}

		void addColorInfo(const float *,int) {}

		float* getPosInfo(int) { return 0; }

		float* getNormalInfo(int) { return 0; }

		float* getUVInfo(int) { return 0; }

		float* getColorInfo(int) { return 0; }
};


extern void hkgSystemNullInit();
void hkgSystemNullInit()
{
	hkgWindow::create = hkgWindowNull::createWindowNull;
	hkgDisplayContext::s_maxNumLights = 0;
	hkgDisplayContext::create = hkgDisplayContextNull::createDisplayContextNull;
	hkgViewport::create = hkgViewportNull::createViewportNull;
	hkgCamera::create = hkgCameraNull::createCameraNull;
	hkgDisplayWorld::create = hkgDisplayWorld::createDisplayWorld;
	hkgFont::create = hkgFontNull::createFontNull;
	hkgTexture::create = hkgTextureNull::createTextureNull;
	hkgMaterialManager::create = hkgMaterialManager::createMaterialManager;
	hkgMaterial::create = hkgMaterialNull::createMaterialNull;
	hkgLightManager::create = hkgLightManager::createLightManager;
	hkgLight::create = hkgLightNull::createLightNull;
	hkgDisplayObject::create = hkgDisplayObjectNull::createDisplayObjectNull;
	hkgGeometry::create = hkgGeometry::createGeometry;
	hkgMaterialFaceSet::create = hkgMaterialFaceSet::createMaterialFaceSet;
	hkgFaceSet::create = hkgFaceSet::createFaceSet;
	hkgVertexSet::create = hkgVertexSetNull::createVertexSetNull;

	hkBaseSystem::init();
}

extern void hkgSystemNullQuit();
void hkgSystemNullQuit()
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
