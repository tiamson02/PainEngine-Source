/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_DISPLAY_CONTEXT_OGL_H
#define HK_GRAPHICS_DISPLAY_CONTEXT_OGL_H

#include <hkgraphics/hkgDisplayContext.h>

typedef unsigned short HKG_VERTEX_STREAMS;
#define HKG_OGL_VTX_NONE   (0)
#define HKG_OGL_VTX_POS    (1<<0)
#define HKG_OGL_VTX_NORMAL (1<<1)
#define HKG_OGL_VTX_COLOR  (1<<2)
#define HKG_OGL_VTX_UV     (1<<3)
#define HKG_OGL_VTX_ALL    (0xff)



class hkgDisplayContextOGL : public hkgDisplayContext
{
public:

	virtual void setLightingState(bool on);
	virtual void setTexture2DState(bool on);
	virtual void setBlendState(bool on);
	virtual void setDepthReadState(bool on);
	virtual void setDepthWriteState(bool on);
	virtual void setCullFaceState(bool on);
	virtual void setWireframeState(bool on);

	virtual void setLightState(int light, bool on);
	
	// The following do not alter the actual state but assume
	// that you have altered the state yourself
	inline HKG_VERTEX_STREAMS getStreamState() const;
	inline void setStreamState( HKG_VERTEX_STREAMS s ); 

	virtual void popMatrix();
	virtual void pushMatrix();
	virtual void multMatrix( float m[16]);
	virtual void beginGroup(HKG_IMM_GROUP type);
	virtual void  setCurrentColor3( float c[3]);
	virtual void  setCurrentColor4( float c[4]);
	virtual void  setCurrentColorPacked( unsigned int c);
	virtual void  setCurrentTextureCoord(float uv[2]);
	virtual void  setCurrentNormal(float n[3]);
	virtual void  setCurrentPosition(float p[3]);
	virtual void endGroup();
	virtual void flush();

	static hkgDisplayContext* createDisplayContextOGL(hkgWindow* owner) { return new hkgDisplayContextOGL(owner); }

protected:

	hkgDisplayContextOGL(hkgWindow* owner); 
	virtual ~hkgDisplayContextOGL();
	
	HKG_VERTEX_STREAMS m_enabledVertexStreams;	
};

#include <hkgraphicsogl/hkgDisplayContextOGL.inl>

#endif //HK_GRAPHICS_DISPLAY_CONTEXT_OGL_H
	

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
