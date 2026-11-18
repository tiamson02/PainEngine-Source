/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GRAPHICS_TEXTURE_H
#define HK_GRAPHICS_TEXTURE_H

typedef unsigned short HKG_TEXTURE_DEFAULTS;
#define HKG_TEXTURE_HAVOKLOGO	1
#define HKG_TEXTURE_FONT		2
#define HKG_TEXTURE_POINTER		4		
#define HKG_TEXTURE_CHECKER		8

class hkIstream;
/// The graphics library texture class. The class knows how to load itself from different
/// file formats across multiple platforms, can handle alpha channels and plalettes, but most
/// importantly it can swizzle and load itself into hardware using the realize(), bind() and free()
/// calls.
class hkgTexture : public hkgReferencedObject
{
		friend class hkgDisplayContext; 

	public:

			/// Create a blank, platform specific, texture object in the current context as given.
		static hkgTexture* (*create)(hkgDisplayContext* context); 
		
			/// Load a texture from file. Will parse the filename to find the file type,
			/// and use an Ifstream to load it from file before passing the stream on to 
			/// the specific loadFrom*() functions. 
			/// It can load PNG, uncompessed TGA, or BMP(only under Win32, and will try the
			/// HINSTANCE resources too if it is of BMP type). The hinstance is only required
			/// in Win32 and then only if you want the resource check.
		bool loadFromFile(const char* filename, void * hinstance = HK_NULL); 
		
			/// Load a PNG using libpng, which is built into this library, It will support
			/// most common PNG types, but compressed 32 bit with no pallete is recommended.
		bool loadFromPNG(hkIstream& s);

			/// Load an uncompressed 24bit or 32bit Targa file. 
		bool loadFromTGA(hkIstream& s);

			/// Load a Windows BMP. As this method will use Windows functions, it is 
			/// only implimented on Windows at the moment. In future a library such as FreeImage 
			/// may be used for true cross platform support, but PNG is supported on all 
			/// platforms as is and is the recommened format.
		bool loadFromBMP(hkIstream& s);

			/// Load from a Windows Device Independant Bitmap (DIB). Only will work in Windows
			/// and will assume the void* is a BITMAPINFO*
		bool loadFromDIB(void* win32DIB); 
		
			/// Load a built in texture. Built-in means that the library has all the texture
			/// information already for the image. These are all either programatic (such as the 
			/// checker pattern) or stored in a binary resource, as a compressed PNG (the logo, pointer
			/// and default font).
			/// The 4 current built-ins are:
			///   HKG_TEXTURE_HAVOKLOGO
			///   HKG_TEXTURE_FONT
			///   HKG_TEXTURE_POINTER
			///   HKG_TEXTURE_CHECKER
		bool loadBuiltIn( HKG_TEXTURE_DEFAULTS d );

			/// Has the texture got any pixel data yet? It is a simple check for m_data != HK_NULL.
		inline bool hasData() const;

			/// Will wipe and deallocate all held data. This is not the same as free() which 
			/// frees platform specific information. This clear() will remove all the data and	
			/// clut data.
		virtual void clear(); 

			/// Get the const pointer to the internal data array. May be HK_NULL if it has yet to be allocated.
			/// You can work out the dimensions by the width, height and bpp. Once a texture has been realized(),
			/// the structure may change (swizzled for instance on GameCube) to reduce memory overheads.
		inline const unsigned char* getDataPointer() const;

			/// Get the const pointer to the Color Lookup Table. This is not fully implimented on all platforms yet so it
			/// is safest to stick with a HK_NULL clut for the time being.
		inline const unsigned char* getClutPointer() const;

			/// Get the pointer to the internal data array. May be HK_NULL if it has yet to be allocated.
			/// You can work out the dimensions by the width, height and bpp. Once a texture has been realized(),
			/// the structure may change (swizzled for instance on GameCube) to reduce memory overheads.
		inline unsigned char* getDataPointer();

			/// Get the pointer to the Color Lookup Table. This is not fully implimented on all platforms yet so it
			/// is safest to stick with a HK_NULL clut for the time being.
		inline unsigned char* getClutPointer();

			/// Allocate the data (texture surface) with the given width and height. with optional alpha channel and a clut. 
			/// You can optional supply a data pointer to copy the image from. This source must be at least
			/// the same size as the destination. A lot of platforms restrict width and height to power of 2 in size,
			/// with max size of 2048x2048. This is naturally too big for the PS2 for instance that has
			/// limited resources for textures.
		unsigned char* allocateSurface( unsigned int width, unsigned int height, bool alpha, bool usesClut, const unsigned char* sourceData = HK_NULL );

			/// Allocate the Color Lookup Table (CLUT) with numEntries and optional alpha channel. You can
			/// supply a source CLUT to copy the data from, but it must be the same dimension (or higher) than
			/// this one. A lot of platforms require a 256 entry CLUT.
		unsigned char* allocateClut( unsigned int numEntries, bool alpha, const unsigned char* sourceData = HK_NULL );
	
			/// Get the width, in pixels, of the texture. Usually a power of 2.
		inline unsigned int getWidth() const;

			/// Get the height, in pixels, of the texture. Usually a power of 2.
		inline unsigned int getHeight() const;
			
			/// Get the bits per pixel of the image (8,24,32).
		inline unsigned int getBPP() const;

			/// Is the texture transparent? This would imply that it has an alpha channel and values
			/// in that channel are less than max alpha (< 255)
		inline bool isTransparent() const;

			/// Does the texture have an alpha channel? This does not imply that the alpha channel
			/// has an particlar values though.
		inline bool hasAlphaChannel() const;

			/// Add an alpha channel to the image. This will have to reallocate the data if there is not alpha
			/// in the image already (if there is, the function just returns). You can init the alpha
			/// to the given value if desired.
		void addAlphaChannel(unsigned char value = 0xff);

			/// Get the void* user data that can be used to tag the texture with game or conversion information.
		inline void* getUserData() const;

			/// Set the void* user data that can be used to tag the texture with game or conversion information.
		inline void setUserData(void* d);

			/// Platform specific: Realize the texture to the hardware, On PS2 this will just use the
			/// texture in m_data directly, but on GameCube and DirectX platforms (PC & Xbox) the
			/// textures are swizzled to a specific form and so a copy is created. On GameCube (only) this
			/// will result in the actual m_data being swizzled to reduce memory overhead of keeping
			/// the unswizzled version aswell. On PC and Xbox, DirectX manages this extra overhead, and on PS2
			/// there is no overhead.
		virtual bool realize();	

			/// Free all platform specific resources. If you free a texture, m_data will still be valid
			/// on all platforms except GameCube where the m_data pointer will become HK_NULL as it is 
			/// swizzled anyway, so can't be re-realized with special treatment, This will change in future
			/// to be more like the other platforms. 
		virtual void free();	

	protected:

		inline hkgTexture();
		virtual ~hkgTexture();

		virtual void bind() const;		

		// call loadFromFile instead
		bool loadFromBMP(const char* filename, void * hinstance);

		bool m_hasAlpha;		// has an alpha channel
		bool m_isTransparent;  // some of the alpha values are non max values.

		unsigned char* m_clut; // 256 entry clut. Will have be 3 or 4 bytes depending on m_hasAlpha;
		unsigned char* m_data; // 8 bit clut, 24 bit  rgb or 32 bit rgba

		unsigned int m_width;  // in pixels
		unsigned int m_height; // in pixels
		unsigned short m_bpp;  // bits per pixel;

		void* m_userData;		// whatever you like

};

#include <hkgraphics/hkgTexture.inl>

#endif

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
