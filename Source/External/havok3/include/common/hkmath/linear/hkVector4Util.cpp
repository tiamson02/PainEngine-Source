/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <hkmath/hkMath.h>
#include <hkmath/linear/hkVector4Util.h>

#ifdef HK_PS2
HK_ALIGN16( const float hkConvertMagicVector[] ) = { 65536.0f, 65536.0f, 65536.0f, 65536.0f };
#endif

hkUint32 hkVector4Util::m_reservedRegisters = 0;

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_ARCH_PS2)
		extern const hkUint128 hkQuadSignMask;
#		if !defined(HK_COMPILER_MWERKS)
			const hkUint128 hkQuadSignMask = 0x80000000800000008000000080000000ULL; // top bit ones	
#		elif defined(HK_COMPILER_MWERKS)
			const hkUint128 hkQuadSignMask = 0x80000000800000008000000080000000; // top bit ones	
#		endif
#	endif
#endif
 
hkReal hkVector4Util::getFloatToInt16FloorCorrection()
{
	hkReal l = 10.0f;
	hkReal h = 11.0f;
	hkVector4 one; one.setAll( 1.0f );

	for (int i = 0; i < 23; i++ )
	{
		hkReal m = (l+h) * 0.5f;
		hkVector4 p; p.setAll( m );

		hkIntUnion64 out;
		convertToUint16( p, hkVector4::getZero(), one, out );

		if ( out.u16[0] < 11 )
		{
			l = m;
		}
		else
		{
			h = m;
		}
	}
	return (l+h) * 0.5f - 11;
}

hkReal hkVector4Util::getFloatToInt32FloorCorrection()
{
	hkReal l = 10.0f;
	hkReal h = 11.0f;
	hkVector4 one; one.setAll( 1.0f );

	for (int i = 0; i < 23; i++ )
	{
		hkReal m = (l+h) * 0.5f;
		hkVector4 p; p.setAll( m );

		hkUint32 out[4];
		convertToUint32( p, hkVector4::getZero(), one, out );

		if ( out[0] < 11 )
		{
			l = m;
		}
		else
		{
			h = m;
		}
	}
	return (l+h) * 0.5f - 11;
}

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
