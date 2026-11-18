/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <cmath>
#include <xmmintrin.h>
#include <float.h>

typedef __m128 hkQuadReal;

union hkQuadRealUnion
{
	hkReal r[4];
	hkQuadReal q;
};

class hkSimdReal
{
	public:

		hkSimdReal(const hkQuadReal& x)
			: m_real(x)
		{
		}

		hkSimdReal(hkReal x)
		{
			m_real = _mm_set_ss(x);
		}

		operator hkReal() const
		{
			hkReal s;
			_mm_store_ss(&s, m_real);
			return s;
		}

		inline hkQuadReal broadcast() const
		{
			return _mm_shuffle_ps(m_real, m_real, 0);
		}

		inline const hkQuadReal& quad() const
		{
			return m_real;
		}

	private:

		hkQuadReal m_real;
};

typedef const hkSimdReal& hkSimdRealParameter;

inline hkSimdReal HK_CALL operator* (hkSimdRealParameter r, hkSimdRealParameter s)
{
	return _mm_mul_ss(r.quad(),s.quad());
}

inline hkSimdReal HK_CALL operator- (hkSimdRealParameter r, hkSimdRealParameter s)
{
	return _mm_sub_ss(r.quad(),s.quad());
}

inline hkSimdReal HK_CALL operator+ (hkSimdRealParameter r, hkSimdRealParameter s)
{
	return _mm_add_ss(r.quad(),s.quad());
}

inline hkSimdReal HK_CALL operator/ (hkSimdRealParameter r, hkSimdRealParameter s)
{
	return _mm_div_ss(r.quad(),s.quad());
}



/*
inline hkSimdReal HK_CALL operator- (const hkSimdReal r,const  hkSimdReal s)
{
	return _mm_sub_ss(r.quad(),s.quad());
}*/

enum hkVector4CompareMask
{
	HK_VECTOR3_COMPARE_MASK_NONE	= 0,
	HK_VECTOR3_COMPARE_MASK_X		= 1,
	HK_VECTOR3_COMPARE_MASK_Y		= 2,
	HK_VECTOR3_COMPARE_MASK_XY		= 3,

	HK_VECTOR3_COMPARE_MASK_Z		= 4,
	HK_VECTOR3_COMPARE_MASK_XZ		= 5,
	HK_VECTOR3_COMPARE_MASK_YZ		= 6,
	HK_VECTOR3_COMPARE_MASK_XYZ		= 7,

	HK_VECTOR3_COMPARE_MASK_W		= 8,
	HK_VECTOR3_COMPARE_MASK_XW		= 9,
	HK_VECTOR3_COMPARE_MASK_YW		= 10,
	HK_VECTOR3_COMPARE_MASK_XYW		= 11,

	HK_VECTOR3_COMPARE_MASK_ZW		= 12,
	HK_VECTOR3_COMPARE_MASK_XZW		= 13,
	HK_VECTOR3_COMPARE_MASK_YZW		= 14,
	HK_VECTOR3_COMPARE_MASK_XYZW	= 15
};

#define HK_SIMD_COMPARE_MASK_X 1

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

	inline int HK_CALL isNegative(const hkSimdReal& r0)
	{
		return _mm_movemask_ps(r0.quad()) & HK_VECTOR3_COMPARE_MASK_X;
	}

	inline hkSimdReal HK_CALL sqrt(hkSimdRealParameter r)
	{
		return _mm_sqrt_ss(r.quad());
	}
	inline hkReal HK_CALL sqrt(hkReal r)
	{
        return ::sqrtf(r);
	}

	inline hkReal HK_CALL fabs(hkReal r) { return ::fabsf(r); }
	inline hkReal HK_CALL sin (hkReal r) { return ::sinf(r); }
	inline hkReal HK_CALL cos (hkReal r) { return ::cosf(r); }
	inline hkReal HK_CALL acos(hkReal r) { return ::acosf(r); }
	inline hkReal HK_CALL asin(hkReal r) { return ::asinf(r); }
	inline hkReal HK_CALL sqrtInverse(hkReal r) { return 1.0f/::sqrtf(r); }
	inline bool HK_CALL isFinite(hkReal r) { return r==r; }

	hkReal HK_CALL rand01();
	hkReal HK_CALL atan2fApproximation( hkReal x, hkReal y );

	hkReal HK_CALL hkFloor(hkReal r);
	int HK_CALL hkFloatToInt(hkReal r);
	int HK_CALL hkFloorToInt(hkReal r);

	inline hkReal HK_CALL randRange(hkReal min, hkReal max)
	{
		return min + rand01() * (max-min);
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
