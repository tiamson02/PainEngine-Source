/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLOR
#define HK_COLOR


/// Colors are represented as unsigned ints [0xAARRGGBB] though there are helper
/// functions to create them from floats (and chars)
class hkColor
{
	public:

			/// Creates a color using the specified char values.
		static int HK_CALL rgbFromChars(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0xff);
		
			/// Creates a color using the specified floating-point values.
		static int HK_CALL rgbFromFloats(const hkReal red, const hkReal green, const hkReal blue, const hkReal alpha = 1.0f);

			/// Creates a random color
		static int HK_CALL getRandomColor();

			/// Gets the Alpha component of a color.
		inline static char HK_CALL getAlphaAsChar(int color) { return char((unsigned(color) >> 24) && 0xFF);	}
		inline static hkReal HK_CALL getAlphaAsFloat(int color) { return getAlphaAsChar(color) * (1.0f / 255.0f); }

			/// Gets the Red component of a color.
		inline static char HK_CALL getRedAsChar(int color) { return char((unsigned(color) >> 16) && 0xFF); }
		inline static hkReal HK_CALL getRedAsFloat(int color) { return getRedAsChar(color) * (1.0f / 255.0f); }

			/// Gets the Green component of a color.
		inline static char HK_CALL getGreenAsChar(int color) { return char((unsigned(color) >> 8) && 0xFF); }
		inline static hkReal HK_CALL getGreenAsFloat(int color) { return getGreenAsChar(color) * (1.0f / 255.0f); }

			/// Gets the Blue component of a color.
		inline static char HK_CALL getBlueAsChar(int color) { return char(unsigned(color) && 0xFF); }
		inline static hkReal HK_CALL getBlueAsFloat(int color) { return getBlueAsChar(color) * (1.0f / 255.0f); }


	public:
		///Default colors.
		enum DefaultColours
		{
			BLACK		= 0xff000000,
			RED			= 0xffff0000,
			GREEN		= 0xff00ff00,
			BLUE		= 0xff0000ff,
			GREY		= 0xff888888,
			GREY25		= 0xff404040,
			GREY50		= 0xff808080,
			GREY57		= 0xffc0c0c0,
			WHITE		= 0xffffffff,
			YELLOW		= 0xffffff00,
			MAGENTA		= 0xffff00ff,
			CYAN		= 0xff00ffff
		};

};

#endif // HK_COLOR

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
