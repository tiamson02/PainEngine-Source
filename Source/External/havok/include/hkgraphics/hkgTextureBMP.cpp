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


#ifdef HK_PLATFORM_WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static void * readBMP(hkIstream& s, BITMAPINFO	**info)
{
	void			*bits;		
	long			bitsize, infosize;

	BITMAPFILEHEADER	header;		/* File header */

	if ( s.read((char*)&header, sizeof(BITMAPFILEHEADER)) < sizeof(BITMAPFILEHEADER))
		return HK_NULL;
	
	if ( header.bfType != (unsigned short)('MB') )	/* Check for BM reversed... */
		return HK_NULL;
	
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	
	*info = (BITMAPINFO *)hkAllocate<unsigned char>( infosize, HK_MEMORY_CLASS_DISPLAY );

	if ((long) s.read( (char*)(*info), infosize ) < infosize)
	{
		hkDeallocate<unsigned char>( reinterpret_cast<unsigned char*>(*info) );
		return HK_NULL;
	}


	bitsize = (*info)->bmiHeader.biSizeImage;
	if (bitsize == 0)
	{
		bitsize =	((*info)->bmiHeader.biWidth *
					(*info)->bmiHeader.biBitCount + 7) / 8 *
  					hkg_abs((*info)->bmiHeader.biHeight);
	}

	bits = hkAllocate< unsigned char >( bitsize, HK_MEMORY_CLASS_DISPLAY ) ;
	
	if ((long)s.read( (char*)bits, bitsize ) < bitsize)
	{
		hkDeallocate<unsigned char>( reinterpret_cast<unsigned char*>(*info) );
	    hkDeallocate<unsigned char>( reinterpret_cast<unsigned char*>(bits)  );
		return HK_NULL;	
	}

	return bits;
}


static unsigned char* convertToRGB(BITMAPINFO* info, void* bits)
{
  int		i, j,			
  			bitsize,		
			width;			

  unsigned char * newbits = HK_NULL;
  unsigned char * from, *to;		
	
  if (( info->bmiHeader.biWidth % 2 != 0) || (info->bmiHeader.biHeight % 2 != 0) )
  {
	  HK_WARN("Warning: Non power of 2 texture.\n");
  }

  width   = 3 * info->bmiHeader.biWidth;
  width   = (width + 3) & ~3;	
  bitsize = width * info->bmiHeader.biHeight;
  
 
  switch (info->bmiHeader.biCompression)
  {
    case BI_RGB :
		if (info->bmiHeader.biBitCount == 24)
		{
  			newbits = hkAlignedAllocate<unsigned char>(32, bitsize, HK_MEMORY_CLASS_DISPLAY);
			
			for (i = 0; i < info->bmiHeader.biHeight; i ++)
				for (j = 0, from = ((unsigned char*)bits) + i * width,
						to = newbits + i * width;
						j < info->bmiHeader.biWidth;
						j ++, from += 3, to += 3)
				{
				  to[0] = from[2];
				  to[1] = from[1];
				  to[2] = from[0];
				};
		};
	break;
    case BI_RLE4 :
    case BI_RLE8 :
    case BI_BITFIELDS :
        break;
  }

  return newbits;
}

static unsigned char* convertToRGB(BITMAP& bm)	
{
  int		i, j,			/* Looping vars */
  			bitsize,		/* Total size of bitmap */
			newWidth;			/* Aligned width of bitmap */

  unsigned char	*newbits = HK_NULL;		/* New RGB bits */
  unsigned char	*from, *to;		/* RGB looping vars */
	
  // Allocate memory for the RGB bitmap...
  newWidth = 3 * bm.bmWidth;
  newWidth = (newWidth + 3) & ~3;	
  bitsize = newWidth * bm.bmHeight;
  
  if ( (bm.bmWidth % 2 != 0) || (bm.bmHeight % 2 != 0) )
  {
	  HK_WARN("Warning: Non power of 2 texture");
  }

  // Copy the original bitmap to the new array, converting as necessary.
  if (bm.bmBitsPixel == 24)
  {
	  newbits = hkAlignedAllocate<unsigned char>(32, bitsize, HK_MEMORY_CLASS_DISPLAY );

      for (i = 0; i <  bm.bmHeight; i ++)
		for (j = 0, from = ((unsigned char*)bm.bmBits) + i * bm.bmWidthBytes,
					to = newbits + i * newWidth;
				j < bm.bmWidth;
				j ++, from += 3, to += 3)
        {
          to[0] = from[2];
          to[1] = from[1];
          to[2] = from[0];
        };
  }
  // else unsupported for the moment

  return newbits;
}

#endif // PLATFORM_WIN32




// only will work in windows
bool hkgTexture::loadFromDIB(void* win32DIB)
{
	// cleanup if we have any current data
	clear();

#ifdef HK_PLATFORM_WIN32
	
	BITMAPINFO	*info = (BITMAPINFO*) win32DIB;
	void		*bits = info->bmiColors;

	if ( info->bmiHeader.biCompression != BI_RGB ||
	     info->bmiHeader.biBitCount != 24) 
	   return false;

	m_data = convertToRGB(info, bits);
	
	if (m_data == HK_NULL)
		return false;

	m_width  = info->bmiHeader.biWidth;
	m_height = info->bmiHeader.biHeight;
	m_clut = HK_NULL;
	m_bpp = 24;
	m_hasAlpha = false;
	m_isTransparent = false;

	return true;

#else
	return false;
#endif
}


bool hkgTexture::loadFromBMP(const char* filename, void* hinstance)
{
	// cleanup if we have any current data
	clear();

#ifdef HK_PLATFORM_WIN32

	if (hinstance)
	{
		// find the filename without the directory addons
		int len = hkString::strLen( filename );
		int j = len - 1;
		while ( j != 0 && 
			(*(filename + j) != '\\') && (*(filename + j) != '/') )
			--j;

		if ( j!= 0)
			j++; // skip '\'

		HBITMAP bm = (HBITMAP)LoadImage( (HINSTANCE)hinstance, (filename + j),
                                      IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
		if (bm)
		{
			// Get the bitmap structure (to extract width, height, and bpp)
			BITMAP bitmap;
			GetObject( bm, sizeof(BITMAP), &bitmap );

			m_data = convertToRGB(bitmap);

			if (m_data == HK_NULL)
				return false;

			m_width = bitmap.bmWidth;
			m_height = bitmap.bmHeight;
			m_bpp = 24;
		
			DeleteObject(bm);
		}
	}

	if (m_data == HK_NULL)
	{
		hkIfstream s( filename );
		return loadFromBMP( s );
	}
	else
		return true;

#else

	return false;

#endif

}

bool hkgTexture::loadFromBMP(hkIstream& s)
{
	clear();

#ifdef HK_PLATFORM_WIN32

	BITMAPINFO* info;
	unsigned char* bits = reinterpret_cast<unsigned char*>( readBMP(s, &info) );
		
	if (bits == NULL)
		return false;

	m_data = convertToRGB(info, bits);
	
	if (m_data == NULL)
	{
		hkDeallocate<unsigned char>( reinterpret_cast<unsigned char*>(info) );
		hkDeallocate<unsigned char>( bits );
		return false;
	}

	m_width = info->bmiHeader.biWidth;
	m_height = info->bmiHeader.biHeight;
	m_bpp = 24;

	hkDeallocate<unsigned char>( reinterpret_cast<unsigned char*>(info) );
	hkDeallocate<unsigned char>( bits );

	return true;

#else

	return false;

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
