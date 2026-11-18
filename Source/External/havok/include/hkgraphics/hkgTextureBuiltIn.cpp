/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkgraphics/hkGraphics.h> // precompiled
#include <hkgraphics/hkgTexture.h>
#include <hkbase/stream/hkStringStream.h>

/*
typedef unsigned short HKG_TEXTURE_DEFAULTS	
#define HKG_TEXTURE_HAVOKLOGO	1
#define HKG_TEXTURE_FONT		2
#define HKG_TEXTURE_POINTER		4		
#define HKG_TEXTURE_CHECKER		8
*/

// Font
extern const unsigned char hkgFontPNGData[];
extern const unsigned int  hkgFontPNGSize;

// Havok Logo
extern const unsigned char hkgLogoPNGData[];
extern const unsigned int  hkgLogoPNGSize;

// Mouse pointer
extern const unsigned char hkgPointerPNGData[];
extern const unsigned int  hkgPointerPNGSize;


bool hkgTexture::loadBuiltIn( HKG_TEXTURE_DEFAULTS d )
{
	// all built in textures are stored in tga format
	switch (d)
	{
		case HKG_TEXTURE_FONT:
			{
				hkIstringStream s( (const char*)hkgFontPNGData, hkgFontPNGSize, false);
			//	hkIfstream s ("font.tga");
				return loadFromPNG( s );
				
			}
			
		case HKG_TEXTURE_HAVOKLOGO:
			{
				hkIstringStream s( (const char*)hkgLogoPNGData, hkgLogoPNGSize, false);
			//	hkIfstream s ("font.tga");
				return loadFromPNG( s );
			}

	
		case HKG_TEXTURE_POINTER:
			{
				hkIstringStream s( (const char*)hkgPointerPNGData, hkgPointerPNGSize, false);
			//	hkIfstream s ("pointer2.png");
				return loadFromPNG( s );
			}
		
		case HKG_TEXTURE_CHECKER:
			{
				// a procedural texture
				unsigned char* data = allocateSurface(64, 64, false, false, HK_NULL);
				unsigned int index = 0;
				for (unsigned int w = 0; w<64; ++w)
				{
					for (unsigned int h = 0; h<64; ++h)
					{
						unsigned char c = ((w/8)+(h/8))%2 ? 0xff : 0x00; 
						data[index++] = c;
						data[index++] = c;
						data[index++] = c;
					}
				}
				return true;
			}

		default: break;
	}

	return false;
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
