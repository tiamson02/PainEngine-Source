/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#include <float.h>

typedef hkReal hkSimdReal;
typedef hkReal hkSimdRealParameter;

struct hkQuadReal
{
	hkReal HK_ALIGNED_VARIABLE(x, 16);
	hkReal y;
	hkReal z;
	hkReal w;
};

enum hkVector4CompareMask
{
	HK_VECTOR3_COMPARE_MASK_NONE	= 0,
	HK_VECTOR3_COMPARE_MASK_W		= 1,
	HK_VECTOR3_COMPARE_MASK_Z		= 2,
	HK_VECTOR3_COMPARE_MASK_ZW		= 3,

	HK_VECTOR3_COMPARE_MASK_Y		= 4,
	HK_VECTOR3_COMPARE_MASK_YW		= 5,
	HK_VECTOR3_COMPARE_MASK_YZ		= 6,
	HK_VECTOR3_COMPARE_MASK_YZW		= 7,

	HK_VECTOR3_COMPARE_MASK_X		= 8,
	HK_VECTOR3_COMPARE_MASK_XW		= 9,
	HK_VECTOR3_COMPARE_MASK_XZ		= 10,
	HK_VECTOR3_COMPARE_MASK_XZW		= 11,

	HK_VECTOR3_COMPARE_MASK_XY		= 12,
	HK_VECTOR3_COMPARE_MASK_XYW		= 13,
	HK_VECTOR3_COMPARE_MASK_XYZ		= 14,
	HK_VECTOR3_COMPARE_MASK_XYZW	= 15
};
#define HK_SIMD_COMPARE_MASK_X 8



#if defined (HK_PLATFORM_WIN32) || defined (HK_PLATFORM_XBOX)
// have to include math header seperately on Win32 and XBox
// as this header has a templated function prototype 
#	include <cmath>
#else
extern "C"
{
#	include <math.h>
}
#endif

#if !defined (HK_PLATFORM_GC) 

namespace hkMath
{
#if defined (HK_PLATFORM_PS2)
	inline hkReal HK_CALL sqrt(hkReal x)		{ __asm__ ("sqrt.s %0, %0" : "+f" (x) : ); return x; }
	inline hkReal HK_CALL fabs(hkReal x)		{ __asm__ ("abs.s  %0, %0" : "+f" (x) : ); return x; }
#else
	inline hkReal HK_CALL sqrt(hkReal r) { return ::sqrtf(r); }
	inline hkReal HK_CALL fabs(hkReal r) { return ::fabsf(r); }
#endif
	inline hkReal HK_CALL sin (hkReal r) { return ::sinf(r); }
	inline hkReal HK_CALL cos (hkReal r) { return ::cosf(r); }
	inline hkReal HK_CALL acos(hkReal r)
	{
		HK_ASSERT( hkMath::fabs(r) < 1.001f );
		if( hkMath::fabs(r) > 1.0f )
		{
			r = ( r>0 )	? 1.0f : -1.0f;
		}
		return ::acosf(r);
	}
	inline hkReal HK_CALL asin(hkReal r)
	{
		HK_ASSERT( hkMath::fabs(r) < 1.001f );
		if( hkMath::fabs(r) > 1.0f )
		{
			r = ( r>0 )	? 1.0f : -1.0f;
		}
		return ::asinf(r);
	}
	inline hkReal HK_CALL sqrtInverse(hkReal r) { return 1.0f / sqrt(r); }
	inline hkReal HK_CALL floor(hkReal r) { return ::floorf(r); }
}

#elif defined (HK_PLATFORM_GC)
//TODO: this may all be unecessary (cmath should have the defs)
//but if it isn't, then more care should be taken in the implementation
namespace hkMath
{
#if defined(HK_COMPILER_MWERKS)
	inline hkReal HK_CALL fabs(hkReal r) { return static_cast<hkReal>((float)__fabs((double)r)); }
#elif defined(HK_COMPILER_GCC)
	inline hkReal HK_CALL fabs(hkReal r) { return static_cast<hkReal>((float)::fabsf((double)r)); }
#else
#	error unknown compiler
#endif
	inline hkReal HK_CALL sin (hkReal r) { return static_cast<hkReal>(::sin(r)); }
	inline hkReal HK_CALL cos (hkReal r) { return static_cast<hkReal>(::cos(r)); }
	inline hkReal HK_CALL acos(hkReal r) { return static_cast<hkReal>(::acos(r)); }
	inline hkReal HK_CALL asin(hkReal r) { return static_cast<hkReal>(::asin(r)); }
/*
	inline hkReal HK_CALL sqrtInverse(hkReal r) 
	{
		register long double val = (long double) r;
		register long double ret;
#if defined(HK_COMPILER_MWERKS)
		 asm {
			 frsqrte ret, val
		 }
#elif defined(HK_COMPILER_GCC)
		 asm("frsqrte %0, %1": "=f"(ret): "f"(val));
#else
#	error unknown compiler
#endif
		 return static_cast<hkReal>(ret);
	}
	//inline hkReal HK_CALL sqrt(hkReal r) { return static_cast<hkReal>(::sqrt(r)); }
	inline hkReal HK_CALL sqrt(hkReal r)
	{ 
		return static_cast<hkReal>(1.0f/sqrtInverse(r)); 
	}
*/
	inline hkReal HK_CALL sqrt(hkReal r)
	{
		return static_cast<hkReal>(::sqrt(r));
	}
	inline hkReal HK_CALL sqrtInverse(hkReal r)
	{ 
		return static_cast<hkReal>(1.0f/sqrt(r)); 
	}
	
	inline hkReal HK_CALL floor(hkReal r) { return static_cast<hkReal>(::floorf(r)); }

}

#endif



namespace hkMath
{
	const hkReal HK_REAL_DIG			= FLT_DIG;			/* # of decimal digits of precision */
	const hkReal HK_REAL_EPSILON		= FLT_EPSILON;		/* smallest such that 1.0+FLT_EPSILON != 1.0 */
	const hkReal HK_REAL_MANT_DIG		= FLT_MANT_DIG;		/* # of bits in mantissa */
	const hkReal HK_REAL_MAX			= FLT_MAX;			/* max value */
	const hkReal HK_REAL_MAX_10_EXP	= FLT_MAX_10_EXP;	/* max decimal exponent */
	const hkReal HK_REAL_MAX_EXP		= FLT_MAX_EXP;		/* max binary exponent */
	const hkReal HK_REAL_MIN			= FLT_MIN;			/* min positive value */
	const hkReal HK_REAL_MIN_10_EXP	= FLT_MIN_10_EXP;	/* min decimal exponent */
	const hkReal HK_REAL_MIN_EXP		= FLT_MIN_EXP;		/* min binary exponent */
	const hkReal HK_REAL_RADIX			= FLT_RADIX;		/* exponent radix */

	const hkReal HK_REAL_PI			= 3.14159265358979f;

	inline int HK_CALL isNegative(hkReal r0)
	{
		return (r0<0)? HK_VECTOR3_COMPARE_MASK_X : 0;
	}

	inline bool HK_CALL equal(hkReal x, hkReal y, hkReal tolerance2=1e-5f)
	{
		return hkMath::fabs(x-y) < tolerance2;
	}

	template <typename T>
	inline T HK_CALL max2( T x, T y)
	{
		return x > y ? x : y;
	}
	template <typename T>
	inline T HK_CALL min2( T x, T y)
	{
		return x < y ? x : y;
	}

	inline bool HK_CALL isFinite(hkReal r)
	{
		return r==r;
	}

	hkReal HK_CALL rand01();
	hkReal HK_CALL atan2fApproximation( hkReal x, hkReal y );

	hkReal HK_CALL hkFloor(hkReal r);
	int HK_CALL hkFloatToInt(hkReal r);
	int HK_CALL hkFloorToInt(hkReal r);

	inline hkReal HK_CALL randRange(hkReal minv, hkReal maxv)
	{
		return minv + rand01() * (maxv-minv);
	}
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
