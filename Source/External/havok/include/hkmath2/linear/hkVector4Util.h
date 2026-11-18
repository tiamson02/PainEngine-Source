/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_MATH2_VECTOR3_UTIL_H
#define HK_MATH2_VECTOR3_UTIL_H

namespace hkVector4Util
{
		//!me if these aren't used anywhere but in JSD, should be moved to be just local functions in GJK
	HK_FORCE_INLINE void HK_CALL assignSubDot(hkReal& result, const hkVector4& a, const hkVector4& b, const hkVector4& c);
		
	HK_FORCE_INLINE void HK_CALL pairAssignSubDots(hkReal& result0, hkReal& result1, const hkVector4& a, const hkVector4& b0, const hkVector4& b1, const hkVector4& c);

	HK_FORCE_INLINE void HK_CALL tripleAssignSubDots(hkReal& result0, hkReal& result1, hkReal& result2, const hkVector4& a, const hkVector4& b0, const hkVector4& b1, const hkVector4& b2);

		/// Result = (b-a) cross (c-a)
	HK_FORCE_INLINE void HK_CALL setNormalOfTriangle(hkVector4& result, const hkVector4 &a, const hkVector4& b,const hkVector4& c);

		/// Multiplies sign bits from signsSource onto inout leaving the mantissa and exponent untouched.
	HK_FORCE_INLINE void HK_CALL mulSigns4( hkVector4& inout, const hkVector4& signsSource);

		/// 
	HK_FORCE_INLINE void HK_CALL convertToUint16( hkIntUnion64& out, const hkVector4& in, const hkVector4& offset, const hkVector4& scale );

		/// 
	HK_FORCE_INLINE void HK_CALL convertToUint16WithClip( hkIntUnion64& out, const hkVector4& in, const hkVector4& offset, const hkVector4& scale, const hkVector4& min, const hkVector4& max);

		/// Finds a vector that is perpendicular to a line segment.
	HK_FORCE_INLINE void HK_CALL calculatePerpendicularVector(const hkVector4& vectorIn, hkVector4& biVectorOut);

		/// Transforms an array of points.
	HK_FORCE_INLINE void HK_CALL transformPoints( const hkTransform& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// Rotate an array of points.
	HK_FORCE_INLINE void HK_CALL rotatePoints( const hkMatrix3& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// Inverte rotate an array of points.
	HK_FORCE_INLINE void HK_CALL rotateInversePoints( const hkRotation& t, const hkVector4* vectorsIn, int numVectors, hkVector4* vectorsOut );

		/// Resets the fpu after using MMX instructions on x86. No-op for other architectures.
	HK_FORCE_INLINE void HK_CALL exitMmx();

		/// Returns the squared distance from p to the line segment ab 
	HK_FORCE_INLINE hkSimdReal HK_CALL distToLineSquared( const hkVector4& a, const hkVector4& b, const hkVector4& p );

		/// check for free registers for computers with lot's of registers (e.g. PS2)
	HK_FORCE_INLINE void HK_CALL checkRegisters( hkUint32 registerMask );

	/// reserve registers for computers with lot's of registers (e.g. PS2)
	HK_FORCE_INLINE void HK_CALL reserveRegisters( hkUint32 registerMask );

		/// release these registers
	HK_FORCE_INLINE void HK_CALL releaseRegisters( hkUint32 registerMask );

	extern hkUint32 m_reservedRegisters;
}

#include <hkmath2/linear/hkVector4Util.inl>

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <hkmath2/linear/impl/hkSseVector4Util.inl>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_PS2)
#		include <hkmath2/linear/impl/hkPs2Vector4Util.inl>
#	elif defined(HK_ARCH_PS2)
#		include <hkmath2/linear/impl/hkPs2AsmVector4Util.inl>
#	else
#		error Dont know how to do simd on this platform
#	endif
#else
#	include <hkmath2/linear/impl/hkFpuVector4Util.inl>
#endif // HK_CONFIG_SIMD

#endif // HK_MATH2_VECTOR3_UTIL_H

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
