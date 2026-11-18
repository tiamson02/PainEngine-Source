/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphicsdx8/hkgraphicsdx8.h> // precompiled
#include <hkgraphicsdx8/hkgTextureDX8.h>
#include <hkgraphicsdx8/hkgDisplayContextDX8.h>

#ifdef _XBOX
#include <XGraphics.h>
#endif

hkgTextureDX8::~hkgTextureDX8()
{
	if (m_texture)
		m_texture->Release();
}

bool hkgTextureDX8::realize()
{
	if (m_texture)
		m_texture->Release();

	D3DFORMAT format; 
	if (m_bpp >= 24 )
		format = D3DFMT_A8R8G8B8;
	else 	
	{
		m_texture = HK_NULL;
		return false;
	}

	HRESULT hr = m_device->CreateTexture( m_width, m_height, 1, NULL, format, D3DPOOL_MANAGED, &m_texture);

	if (hr != D3D_OK)
		return false;

	D3DLOCKED_RECT lockedRect;

	m_texture->LockRect( 0, &lockedRect, NULL, NULL);

	unsigned char* bits = (unsigned char*)( lockedRect.pBits );

	unsigned int index = 0;
	for (unsigned int h = 0; h < m_height; ++h)
	{
		unsigned int offset = h*lockedRect.Pitch;

		for (unsigned int w=0; w < m_width; ++w)
		{
			const unsigned char r = m_data[index++];
			const unsigned char g = m_data[index++];
			const unsigned char b = m_data[index++];

			bits[offset++] = b;
			bits[offset++] = g;
			bits[offset++] = r;
			
			if (m_hasAlpha)
			{
				const unsigned char a = m_data[index++];
				bits[offset++] = a; //A
			}
			else
				bits[offset++] = 0xff; 
		}
	}

#ifdef _XBOX
	swizzle(bits);
#endif

	m_texture->UnlockRect( 0 );
	return true;
}

void hkgTextureDX8::bind() const
{
	m_device->SetTexture( 0, m_texture);
}

void hkgTextureDX8::free()
{
	m_texture->Release();
	m_texture = NULL;
}



void hkgTextureDX8::swizzle(unsigned char* bits)
{
#ifdef _XBOX
	
	unsigned int textureSize = m_width*m_height*4;
    unsigned char* pSrcBits = hkAllocate<unsigned char>( textureSize, HK_MEMORY_CLASS_DISPLAY);

	hkString::memCpy( pSrcBits, bits, textureSize );
    
    XGSwizzleRect( pSrcBits, 0, NULL, bits,
                  m_width, m_height, 
                  NULL, 4);

    hkDeallocate<unsigned char>( pSrcBits );

#endif
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
