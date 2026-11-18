/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkutilities2/visualdebugger/hkColor.h>
#include <hkmath2/hkMath.h>

int HK_CALL hkColor::rgbFromChars(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	unsigned color =  (unsigned(alpha) * (256 * 256 * 256)) 
					+ (unsigned(red) * (256 * 256))
					+ (unsigned(green) * 256)
					+ unsigned(blue);
	return int(color);
}

int HK_CALL hkColor::rgbFromFloats(const hkReal red, const hkReal green, const hkReal blue, const hkReal alpha)
{
	HK_ASSERT2( (alpha >= 0.0f) && (alpha <= 1.0f), "Color component is out of range!" );
	HK_ASSERT2( (red >= 0.0f) && (red <= 1.0f), "Color component is out of range!" );
	HK_ASSERT2( (green >= 0.0f) && (green <= 1.0f), "Color component is out of range!" );
	HK_ASSERT2( (blue >= 0.0f) && (blue <= 1.0f), "Color component is out of range!" );

	unsigned char cAlpha = (unsigned char)(hkMath::hkFloatToInt(alpha * 255));
	unsigned char cRed   = (unsigned char)(hkMath::hkFloatToInt(red * 255));
	unsigned char cGreen = (unsigned char)(hkMath::hkFloatToInt(green * 255));
	unsigned char cBlue	 = (unsigned char)(hkMath::hkFloatToInt(blue * 255));

	return rgbFromChars(cRed, cGreen, cBlue, cAlpha);
}


int HK_CALL hkColor::getRandomColor()
{
		float r = hkMath::randRange(0.0f, 1.0f);
		float g = hkMath::randRange(0.0f, 1.0f);
		float b = hkMath::randRange(0.0f, 1.0f);
		float a = 1.0f;

		return rgbFromFloats(r,g,b,a);
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
