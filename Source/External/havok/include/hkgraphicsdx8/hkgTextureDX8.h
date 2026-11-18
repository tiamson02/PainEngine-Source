/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_TEXTURE_DX8_H
#define HK_GRAPHICS_TEXTURE_DX8_H

#include <hkgraphics/hkgTexture.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

class hkgTextureDX8 : public hkgTexture
{
public:

	static hkgTexture* createTextureDX8(hkgDisplayContext* context)
	{
		return new hkgTextureDX8(context);
	}	

	virtual bool realize();	
	virtual void free();		

protected:
	
	virtual void bind() const;		
	
	LPDIRECT3DTEXTURE8	m_texture;
	LPDIRECT3DDEVICE8	m_device;

	inline hkgTextureDX8(hkgDisplayContext* context);
	virtual ~hkgTextureDX8();
	
	void swizzle(unsigned char*); // Xbox needs this

};

#include <hkgraphicsdx8/hkgTextureDX8.inl>

#endif // HK_GRAPHICS_TEXTURE_DX8_H
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
