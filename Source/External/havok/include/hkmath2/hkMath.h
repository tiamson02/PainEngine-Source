/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_MATH2_MATH_H
#define HK_MATH2_MATH_H

#include <hkbase/hkBase.h>

#define	HK_ARCH_PS2
#define	HK_COMPILER_HAS_INTRINSICS_PS2

#define inline HK_FORCE_INLINE

#include <hkbase/config/hkConfigSimd.h>

typedef float hkReal;

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <hkmath2/impl/hkSseMathTypes.inl>
#	elif defined(HK_ARCH_PS2) // ps2 intrinsics or asm blocks
#		include <hkmath2/impl/hkPs2MathTypes.inl>
#	else
#		undef HK_CONFIG_SIMD
#		define HK_CONFIG_SIMD HK_CONFIG_SIMD_DISABLED
#		include <hkmath2/impl/hkFpuMathTypes.inl>
#	endif
#else
#	include <hkmath2/impl/hkFpuMathTypes.inl>
#endif


enum hkCanonicalAxis 
{
	HK_AXIS_X = 0,
	HK_AXIS_Y = 1,
	HK_AXIS_Z = 2
};

union hkIntUnion64
{
	hkInt64 i64;
	hkUint64 u64;
	hkInt32 i32[2];
	hkUint32 u32[2];
	hkInt16 i16[4];
	hkUint16 u16[4];
	hkInt8 i8[8];
	hkUint8 u8[8];
};

// declarations

class hkVector4;
class hkQuaternion;
class hkMatrix3;
class hkRotation;
class hkTransform;

#include <hkmath2/linear/hkVector4.h>
#include <hkmath2/linear/hkQuaternion.h>
#include <hkmath2/linear/hkMatrix3.h>
#include <hkmath2/linear/hkRotation.h>
#include <hkmath2/linear/hkTransform.h>


// platform implementations

#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
#	if defined(HK_COMPILER_HAS_INTRINSICS_IA32)
#		include <hkmath2/linear/impl/hkSseVector4.inl>
#	elif defined(HK_COMPILER_HAS_INTRINSICS_PS2)
#		include <hkmath2/linear/impl/hkPs2Vector4.inl>
#	elif defined(HK_PS2)
		// to work around inlining issues, Ps2AsmVector4 is split into a and b parts
#		include <hkmath2/linear/impl/hkPs2AsmVector4a.inl>
#	else
#		error Dont know how to do simd on this platform
#	endif
#else
#	include <hkmath2/linear/impl/hkFpuVector4.inl>
#endif // HK_CONFIG_SIMD

#include <hkmath2/linear/impl/hkFpuMatrix3.inl>
#include <hkmath2/linear/impl/hkFpuRotation.inl>
#include <hkmath2/linear/impl/hkFpuQuaternion.inl>

// common implementations after inline definitions
#include <hkmath2/linear/hkVector4.inl>
#include <hkmath2/linear/hkTransform.inl>

// to work around inlining issues, Ps2AsmVector4 is split
#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED && !defined(HK_COMPILER_HAS_INTRINSICS_PS2) && defined(HK_PS2)
#	include <hkmath2/linear/impl/hkPs2AsmVector4b.inl>
#endif

// streaming: Note: include this if you need this
//#include <hkmath2/linear/hkMathStream.h>

#undef inline 

#endif // HK_MATH2_MATH_H

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
