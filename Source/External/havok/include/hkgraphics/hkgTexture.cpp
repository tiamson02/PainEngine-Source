/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgTexture.h>
#include <hkbase/stream/hkIstream.h>

hkgTexture* (*hkgTexture::create)(hkgDisplayContext* context);

hkgTexture::~hkgTexture()
{
	clear();
}

unsigned char* hkgTexture::allocateSurface( unsigned int width, unsigned int height, bool alpha, bool usesClut, const unsigned char* sourceData )
{
	m_width = width;
	m_height = height;
	m_bpp = usesClut? 8 : (alpha? 24 : 32);
	m_hasAlpha = alpha;
	m_isTransparent = alpha; 

	if (m_data)
		hkAlignedDeallocate<unsigned char>(m_data );

	unsigned int size = width * height * (m_bpp == 8? 1 : (alpha? 4 : 3));
	m_data = hkAlignedAllocate<unsigned char>(32, size , HK_MEMORY_CLASS_DISPLAY);

	if ( sourceData )
		hkString::memCpy( m_data, sourceData, size);
	else
		hkString::memSet( m_data, 0, size);

	return m_data;
}

unsigned char* hkgTexture::allocateClut( unsigned int numEntries, bool alpha, const unsigned char* sourceData )
{
	if (m_clut)
		hkAlignedDeallocate<unsigned char>( m_clut );

	unsigned int size = numEntries * ( alpha? 4 : 3 );
	m_clut = hkAlignedAllocate<unsigned char>( 32, size, HK_MEMORY_CLASS_DISPLAY );

	m_hasAlpha = alpha;
	m_isTransparent = alpha; 

	if ( sourceData )
		hkString::memCpy( m_clut, sourceData, size);
	else
		hkString::memSet( m_clut, 0, size);

	return m_clut;
}

// will wipe all held data
void hkgTexture::clear()
{
	if (m_clut)
	{
		hkAlignedDeallocate<unsigned char>( m_clut );
		m_clut = HK_NULL;
	}

	if (m_data)
	{
		hkAlignedDeallocate<unsigned char>( m_data );
		m_data = HK_NULL;
	}

	m_hasAlpha = false;
	m_isTransparent = false;
	m_width = 0;
	m_height = 0;
	m_bpp = 0;
}


// PNG, uncompessed TGA, or BMP(only under Win32)
bool hkgTexture::loadFromFile(const char* filename, void * hinstance)
{
	clear();

	HK_ASSERT (filename != HK_NULL);
		
	int flen = hkString::strLen(filename);
	if (flen < 4) // no possible extension
		return false;

	const char* ext = filename + flen - 3;

	if ( hkString::strCasecmp(ext, "bmp") == 0 )
	{
		return loadFromBMP( filename, hinstance ); // will only work on Win32, will return false on all others
	}

	hkIfstream s(filename);

	if (s.isOk())
	{
		// figure out proper conversion
		if ( hkString::strCasecmp(ext, "png") == 0 )
		{
			return loadFromPNG( s );
		}
		else if ( hkString::strCasecmp(ext, "tga") == 0 )
		{
			return loadFromTGA( s );
		}
	}

	return false;
}

bool hkgTexture::realize()
{
	return false;
}

void hkgTexture::bind() const
{
	// do nothing
}

void hkgTexture::free()
{
	// do nothing
}


void hkgTexture::addAlphaChannel(unsigned char value)
{
	if ( (m_data == HK_NULL) || (m_hasAlpha) || (m_clut))
		return;
		
	unsigned char* t = hkAlignedAllocate<unsigned char>( 32, m_width * m_height * 4, HK_MEMORY_CLASS_DISPLAY );

	unsigned int fpos = 0;
	unsigned int tpos = 0;
	for (unsigned int r = 0; r < m_height; ++r)
	{
		for (unsigned int c = 0; c < m_width; ++c)
		{
			t[tpos++] = m_data[fpos++];
			t[tpos++] = m_data[fpos++];
			t[tpos++] = m_data[fpos++];
			t[tpos++] = value;
		}
	}
	
	m_hasAlpha = true;
	m_isTransparent = (value != 0xff);
	
	hkAlignedDeallocate<unsigned char>( m_data );
	m_data = t;	 
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
