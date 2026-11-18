/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_DISPLAY_CONTEXT_DX8_H
#define HK_GRAPHICS_DISPLAY_CONTEXT_DX8_H

#include <hkgraphics/hkgDisplayContext.h>
#include <hkgraphicsdx8/hkgWindowDx8.h>

class hkgDisplayContextDX8 : public hkgDisplayContext
{
public:

	static hkgDisplayContext* createDisplayContextDX8(hkgWindow* owner)
	{
		return new hkgDisplayContextDX8(owner);
	}
	
	virtual void setLightingState(bool on);
	virtual void setTexture2DState(bool on);
	virtual void setBlendState(bool on);
	virtual void setDepthReadState(bool on);
	virtual void setDepthWriteState(bool on);
	virtual void setCullFaceState(bool on);
	virtual void setWireframeState(bool on);

	virtual void setLightState(int light, bool on);

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

	inline LPDIRECT3DDEVICE8 getDevice();
	inline const LPDIRECT3DDEVICE8 getDevice() const;

	void initD3DState(); //needs to be called not only on ctor, but also after device::Reset()

protected:

	hkgDisplayContextDX8(hkgWindow* owner); 
	virtual ~hkgDisplayContextDX8();
	
	struct VertInfo
	{
		float x,y,z;
		float nx, ny, nz; 
		DWORD c;
		float tu, tv;
	};

	HKG_IMM_GROUP		m_immMode;
	hkArray<VertInfo>	m_immVerts; 
	bool				m_immGroup;

	float				m_curNorm[3];
	float				m_curUV[2];
	DWORD				m_curColor;

	LPDIRECT3DDEVICE8	m_device;

};

#include <hkgraphicsdx8/hkgDisplayContextDX8.inl>

#endif //HK_GRAPHICS_DISPLAY_CONTEXT_DX8_H
	
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
