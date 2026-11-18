/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#if HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED 
#	if defined(HK_COMPILER_HAS_INTRINSICS_PS2) 
#		include <mmintrin.h>
#		include <vu0intrin.h>
 		typedef __v4sf hkQuadReal;
#	else
		struct hkQuadReal
		{
			hkUint128 m_quad;
		};
#	endif // !INTRINSICS

extern "C"
{
#include <math.h>
#include <float.h>
}


	union hkQuadRealUnion
	{
		hkReal r[4];
		hkQuadReal q;
	};
		
	class hkSimdReal
	{
		public:

			hkSimdReal()
			{
			}

			hkSimdReal(const hkQuadReal& x)
				: m_real(x)
			{
			}

			hkSimdReal(hkReal x)
			{
#	if defined(HK_COMPILER_HAS_INTRINSICS_PS2)
				_vaddf(m_real, vu0_field_X, vu0_vf0, x);
#	else
				asm( "	qmtc2 %1, %0 " : "=j"(this->m_real):  "r"(x)  );
//				hkQuadRealUnion u;	u.r[0] = x;		m_real = u.q;
#	endif
			}

			operator hkReal() const
			{
				//hkQuadRealUnion u;	u.q = m_real;	return u.r[0];
				float ret;		
				asm( "	qmfc2 %0, %1 " : "=r"(ret):  "j"(this->m_real)  );		
/*
	hkQuadRealUnion a;
	hkQuadRealUnion b;
    __asm__ __volatile__("\n\
	lqc2    vf4,0x0(%1)\n\
	lqc2    vf5,0x0(%2)\n\
	vmul.xyz vf5,vf4,vf5\n\
	vaddy.x vf5,vf5,vf5\n\
	vaddz.x vf5,vf5,vf5\n\
	qmfc2   $2 ,vf5\n\
	mtc1    $2,%0\n\
	": "=f" (ret) :"r" (a.q) ,"r" (b.q) :"$2", "memory" );
*/				
				return ret;
			}

			inline const hkQuadReal& quad() const
			{
				return m_real;
			}

		private:

			hkQuadReal m_real;
	};

	typedef const hkSimdReal& hkSimdRealParameter;

	inline const hkSimdReal HK_CALL operator* (hkSimdReal v0, hkSimdReal v1)
	{
		hkQuadReal ret;
#	if defined(HK_COMPILER_HAS_INTRINSICS_PS2)
		_vmul(ret, vu0_field_X, v0.quad(), v1.quad());
#	else
		asm("vmul.x %0, %1, %2" : "=j"(ret) : "j"(v0.quad()), "j"(v1.quad())  );
#	endif
		return ret;
	}

	inline const hkSimdReal HK_CALL operator- (hkSimdReal v0, hkSimdReal v1)
	{
		hkQuadReal ret;
#	if defined(HK_COMPILER_HAS_INTRINSICS_PS2)
		_vsub(ret, vu0_field_X, v0.quad(), v1.quad());
#	else
		asm("vsub.x %0, %1, %2" : "=j"(ret) : "j"(v0.quad()), "j"(v1.quad())  );
#	endif
		return ret;
	}

	inline const hkSimdReal HK_CALL operator+ (hkSimdReal v0, hkSimdReal v1)
	{
		hkQuadReal ret;
#	if defined(HK_COMPILER_HAS_INTRINSICS_PS2)
		_vadd(ret, vu0_field_X, v0.quad(), v1.quad());
#	else
		asm("vadd.x %0, %1, %2" : "=j"(ret) : "j"(v0.quad()), "j"(v1.quad())  );
#	endif
		return ret;
	}

	inline const hkSimdReal HK_CALL operator/ (hkSimdReal v0, hkSimdReal v1)
	{
		hkQuadReal ret;
	//#if defined(HK_COMPILER_HAS_INTRINSICS_PS2)
	//	_vdivf(ret, vu0_field_X, s1.quad(), r);
	//#else
		asm(
			"vdiv Q, %1x, %2x \n"
			"vwaitq \n"
			"vaddq.x %0, vf0, Q"
			: "=j"(ret) :
			 "j"(v0.quad()), "j"(v1.quad())  );
	//#endif
		return ret;
	}


#else // HK_CONFIG_SIMD_ENABLED

typedef hkReal hkSimdReal;
typedef hkReal hkSimdRealParameter;

struct hkQuadReal
{
	hkReal HK_ALIGNED_VARIABLE(x, 16);
	hkReal y;
	hkReal z;
	hkReal w;
};

#endif // HK_CONFIG_SIMD_ENABLED

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

namespace hkMath
{
	const hkReal HK_REAL_DIG		= FLT_DIG;			/* # of decimal digits of precision */
	const hkReal HK_REAL_EPSILON	= FLT_EPSILON;		/* smallest such that 1.0+FLT_EPSILON != 1.0 */
	const hkReal HK_REAL_MANT_DIG	= FLT_MANT_DIG;		/* # of bits in mantissa */
	const hkReal HK_REAL_MAX		= FLT_MAX;			/* max value */
	const hkReal HK_REAL_MAX_10_EXP	= FLT_MAX_10_EXP;	/* max decimal exponent */
	const hkReal HK_REAL_MAX_EXP	= FLT_MAX_EXP;		/* max binary exponent */
	const hkReal HK_REAL_MIN		= FLT_MIN;			/* min positive value */
	const hkReal HK_REAL_MIN_10_EXP	= FLT_MIN_10_EXP;	/* min decimal exponent */
	const hkReal HK_REAL_MIN_EXP	= FLT_MIN_EXP;		/* min binary exponent */
	const hkReal HK_REAL_RADIX		= FLT_RADIX;		/* exponent radix */

	const hkReal HK_REAL_PI			= 3.14159265358979f;

	inline hkReal HK_CALL sqrt(hkReal r)
	{
		HK_ASSERT(r>=0);
		hkReal res;
		__asm__ ("sqrt.s  %0, %1"
				: "=f" (res)
				: "f"  (r) );
		return res;
	}
	inline hkSimdReal HK_CALL sqrt(hkSimdRealParameter r)
	{
		HK_ASSERT(r>=0);
		hkReal q;
		q = sqrt( hkReal(r) );
		return q;
	
		//_vsqrt(q, r.quad(), vu0_field_X);
		//_vwaitq();
	}

	inline hkReal HK_CALL sqrtInverse(hkReal r, hkReal denominator=1.0f)
	{
		HK_ASSERT(r>=0);
		hkReal res;
		__asm__ ("rsqrt.s  %0, %1, %2"
				: "=f" (res)
				: "f"  (denominator), "f" (r) );
		return res;
	}

	inline hkSimdReal HK_CALL sqrtInverse(hkSimdReal r)
	{
		hkReal q;
		q = sqrtInverse( hkReal(r) );
	
		//_vrsqrt( q, vu0_vf0, vu0_field_W, r.quad(), vu0_field_X );
		//_vwaitq();
		return q;
	}

	inline hkReal HK_CALL fabs(hkReal r)
	{
		hkReal res;
		__asm__ ("abs.s  %0, %1"
				: "=f" (res)
				: "f"  (r) );
		return res;
	}

	inline hkReal HK_CALL sin(hkReal r)
	{
		return sinf(r);
	}
	inline hkReal HK_CALL asin(hkReal r)
	{
		HK_ASSERT( hkMath::fabs(r) < 1.001f );
		if( hkMath::fabs(r) > 1.0f )
		{
			r = ( r>0 ) ? 1.0f : -1.0f;
		}
		return ::asinf(r);
	}

	inline hkReal HK_CALL cos(hkReal r)
	{
		return cosf(r);
	}
	inline hkReal HK_CALL acos(hkReal r)
	{
		HK_ASSERT( hkMath::fabs(r) < 1.001f );
		if( hkMath::fabs(r) > 1.0f )
		{
			r = ( r>0 ) ? 1.0f : -1.0f;
		}
		return ::acosf(r);
	}

	hkReal HK_CALL rand01();
	hkReal HK_CALL atan2fApproximation( hkReal x, hkReal y );

	hkReal HK_CALL hkFloor(hkReal r);
	int HK_CALL hkFloatToInt(hkReal r);
	int HK_CALL hkFloorToInt(hkReal r);

	inline hkReal HK_CALL randRange(hkReal min, hkReal max)
	{
		return min + rand01() * (max-min);
	}

	/*
	inline hkReal HK_CALL tan(hkReal r)
	{
		return tanf(r);
	}
	inline hkReal HK_CALL atan(hkReal r)
	{
		return atanf(r);
	}


	inline hkReal HK_CALL pow( hkReal x, hkReal p)
	{
		return powf(x,p);
	}
	inline hkReal HK_CALL exp( hkReal x)
	{
		return expf(x);
	}

	inline hkReal HK_CALL floor(hkReal x)
	{
		return floorf(x);
	}
	inline hkReal HK_CALL ceil(hkReal x)
	{
		return ceilf(x);
	}
	*/

	inline int HK_CALL isNegative(hkSimdReal r0)
	{
		return (r0<0.0f)? HK_VECTOR3_COMPARE_MASK_X : 0;
	}


	inline hkReal HK_CALL clamp( hkReal x, hkReal mn, hkReal mx)
	{
		HK_ASSERT(mn<=mx);
		return (x<mn ? mn : (x>mx ? mx : x)) ;
	}

	inline bool HK_CALL equal(hkReal x, hkReal y, hkReal tolerance2=1e-5f)
	{
		return hkMath::fabs(x-y) < tolerance2;
	}

	inline bool HK_CALL isFinite(hkReal r)
	{
		return r==r;
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
