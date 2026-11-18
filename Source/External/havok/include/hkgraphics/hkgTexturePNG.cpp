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

// PNG inlcudes
#define ALL_STATIC 1
#define PNG_FLOATING_POINT_SUPPORTED 1
#define PNG_USER_MEM_SUPPORTED 1
#include <hkgraphics/png/png.h>

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

#undef memcpy

static void myReadFunc(png_structp png_ptr, png_bytep data, png_size_t length)
{
	hkIstream* stream = (hkIstream*)(png_ptr->io_ptr);
	HK_ASSERT( stream != NULL );
	unsigned int check = stream->read( (char*)data, length );

	if (check != length)
	{
		png_error(png_ptr, "read error from the Havok stream!");
	}
}

static void myWriteFunc(png_structp png_ptr, png_bytep data, png_size_t length)
{
	hkOstream* stream = (hkOstream*)(png_ptr->io_ptr);
	HK_ASSERT( stream != NULL );
	unsigned int check = stream->write( (char*)data, length );

	if (check != length)
	{
		png_error(png_ptr, "write error to the Havok stream!");
	}
}

static void myFlushFunc(png_structp png_ptr)
{
	hkOstream* stream = (hkOstream*)(png_ptr->io_ptr);
	HK_ASSERT( stream != NULL );
	stream->flush();
}

static void myError(png_structp png_ptr, png_const_charp errorstring)
{
	hkcerr << errorstring << hkendl;
}
static png_voidp myMalloc(png_structp png_ptr, png_size_t sz)
{
	return hkAllocate<char>( sz, HK_MEMORY_CLASS_DISPLAY);
}
static void myFree(png_structp png_ptr, png_voidp p)
{
	hkDeallocate<char>( static_cast<char*>(p));
}


#if 0
bool hkgTexture::loadFromPNG(hkIstream& stream) 
{
	// cleanup if we have any current data
	clear();
	m_width  = 1;
	m_height = 1;
	m_bpp  = 24;
	m_hasAlpha = 0;
	static unsigned char foobar[1024];
	m_data = foobar;
	return true;
}

#else

bool hkgTexture::loadFromPNG(hkIstream& stream) 
{
	// cleanup if we have any current data
	clear();

	unsigned char header[8];
	png_structp png;
	png_infop   info;
	png_infop   endinfo;
	png_bytep   data;
	png_bytep  *row_p;
	
	png_uint_32 width, height;
	
	int depth, color;
	png_uint_32 i;

	// 
	// Check signature
	//

	stream.read((char*)header, 8);

//	if (!png_sig_cmp(header,0, 8)) return 0;

	// create info holders

	png = png_create_read_struct_2(
			PNG_LIBPNG_VER_STRING,
			HK_NULL, myError, myError,
			HK_NULL, myMalloc, myFree );
	info = png_create_info_struct(png);
	endinfo = png_create_info_struct(png);

	// disaster recovery
	if (setjmp(png->jmpbuf))
	{
		png_destroy_read_struct(&png, &info, &endinfo);
		return false;
	}

	// set my read func for streams
	png_set_read_fn(png, (void *)&stream, myReadFunc);
	png_set_sig_bytes(png, 8);

#ifdef HK_ARCH_PPC
	png_set_swap(png); // endian swap on PowerPC platform
#endif

	// read the info and interpret
	png_read_info(png, info);
	png_get_IHDR(png, info, &width, &height, &depth, &color, HK_NULL, HK_NULL, HK_NULL);

	// get the important data
	m_width  = width;
	m_height = height;
	m_hasAlpha = (color & PNG_COLOR_MASK_ALPHA) != 0;
	m_bpp  = depth * (color== PNG_COLOR_TYPE_PALETTE? 1 : ( (color & PNG_COLOR_MASK_ALPHA)? 4 : 3 ));

	// gray --> rgb conversion

	if (color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	// read info (rarely used) 
	png_read_update_info(png, info);

	//
	// setup data pointers
	//

	unsigned int dataSize = png_get_rowbytes(png, info)*height;
	data = (png_bytep) hkAlignedAllocate<unsigned char>(32, dataSize , HK_MEMORY_CLASS_DISPLAY);
	row_p = (png_bytep *) hkAllocate<unsigned char>( sizeof(png_bytep)*height, HK_MEMORY_CLASS_DISPLAY);

	for (i = 0; i < height; i++) 
	{
		row_p[height - 1 - i] = &data[png_get_rowbytes(png, info)*i];
	}

	//
	// read the rows (all at once)
	//

	png_read_image(png, row_p);

	hkDeallocate<unsigned char>( reinterpret_cast<unsigned char*>( row_p ) );

	// read the clut if there is one

	if (color == PNG_COLOR_TYPE_PALETTE)
	{
		png_colorp pal;
		int cols;

		png_get_PLTE(png, info, &pal, &cols);

		if (cols != 1<<8)
		{
			png_error(png, "Color depth not recognized. Only 256 color palettes supported.");
			return false;
		}
		
		unsigned int palByteSize = cols * (m_hasAlpha? 4 : 3);
		m_clut = hkAlignedAllocate<unsigned char>(32, palByteSize, HK_MEMORY_CLASS_DISPLAY );
		hkString::memCpy( m_clut, pal, palByteSize );

		if (m_hasAlpha)
		{
			unsigned char* clutPtr = m_clut + 3; // point to first alpah val 
			for (int i = 0; (i < cols) && !m_isTransparent; ++i)
			{
				m_isTransparent |= (*clutPtr != 0xff);
				clutPtr += 4;
			}
		}	
	}
	else if (m_hasAlpha) // RGBA raw data
	{
		// check if is transparent:
		unsigned char* dataPtr = data + 3; // point to first alpah val 
		for (unsigned int i = 0; (i < (m_width*m_height)) && !m_isTransparent; ++i)
		{
			m_isTransparent |= (*dataPtr != 0xff);
			dataPtr += 4;
		}
	}


	// keep the data for actual display engine conversion
	m_data = data; 

	// clean up and return 
	png_read_end(png, endinfo);
	png_destroy_read_struct(&png, &info, &endinfo);

	return true;
}

#endif



/* Generate a compiler error if there is an old png.h in the search path. */
typedef version_1_2_1 your_png_h_is_not_version_1_2_1;


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
