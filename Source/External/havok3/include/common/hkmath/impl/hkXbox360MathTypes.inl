/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/fwd/hkcmath.h>
#include <hkbase/fwd/hkcfloat.h>

#include <ppcintrinsics.h>
#include <VectorIntrinsics.h>
typedef __vector4 hkQuadReal;

union hkQuadRealUnion
{
	hkReal r[4];
	hkQuadReal q;
};

extern const hkQuadReal hkQuadReal1111;
extern const hkQuadReal hkQuadReal0000;

#define HK_QUADREAL_CONSTANT(a,b,c,d) {a,b,c,d}

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


		hkSimdReal(const hkReal& x)
		{
			m_real = __lvlx(&x, 0);
			m_real = __vspltw( m_real, 0 );
		}

		void set( const hkQuadReal& x ){ m_real = x; }
		void set( const hkReal& x )
		{
			m_real = __lvlx(&x, 0);
			m_real = __vspltw( m_real, 0 );
		}

		operator hkReal() const
		{
			float x;
			__stvewx( m_real, &x, 0);
			return x;
		}

		inline hkQuadReal& getQuad()
		{
			return m_real;
		}

		inline const hkQuadReal& getQuad() const
		{
			return m_real;
		}

		inline static hkSimdReal HK_CALL create( hkReal a)
		{
			hkSimdReal sr; sr.set(a);
			return sr;
		}
	private:

		hkQuadReal m_real;
};

#define HK_SIMD_REAL(a) hkSimdReal::create(a)


typedef const hkSimdReal& hkSimdRealParameter;

inline const hkSimdReal HK_CALL operator* (hkSimdReal v0, hkSimdReal v1)
{
	hkSimdReal res; res.getQuad() = __vmulfp( v0.getQuad(), v1.getQuad() );
	return res;
}

inline const hkSimdReal HK_CALL operator- (hkSimdReal v0, hkSimdReal v1)
{
	hkSimdReal res; res.getQuad() = __vsubfp( v0.getQuad(), v1.getQuad() );
	return res;
}

inline const hkSimdReal HK_CALL operator+ (hkSimdReal v0, hkSimdReal v1)
{
	hkSimdReal res; res.getQuad() = __vaddfp( v0.getQuad(), v1.getQuad() );
	return res;
}

inline const hkSimdReal HK_CALL operator/ (hkSimdReal v0, hkSimdReal v1)
{

	__vector4 estimate = __vrefp( v1.getQuad() );
	//One round of Newton-Raphson refinement
	__vector4 v1Recip = __vmaddfp( __vnmsubfp( estimate, v1.getQuad(), hkQuadReal1111 ), estimate, estimate );

	hkSimdReal res; res.getQuad() = __vmulfp( v0.getQuad(), v1Recip );
	return res;
}

inline const hkSimdReal HK_CALL operator- (hkSimdReal v0)
{
	hkSimdReal res; res.getQuad() = __vsubfp( hkQuadReal0000, v0.getQuad() );
	return res;
}

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
#	define HK_MATH_sqrt
	inline hkReal HK_CALL sqrt(hkReal r)
	{
		return __fsqrts(r);
	}

	inline hkSimdReal HK_CALL sqrt(hkSimdRealParameter r)
	{
		hkReal x = r;
		hkSimdReal res; res.set(__fsqrts( x ));
		return res;
	}

#	define HK_MATH_sqrtInverse
	inline hkReal HK_CALL sqrtInverse( hkReal r )
	{
		return ( 1.0f / __fsqrts(r) );
	}

	inline hkSimdReal HK_CALL sqrtInverse(hkSimdReal r)
	{
		hkReal x = r;
		hkSimdReal res; res.set( 1.0f / __fsqrts( x ) );
	}

#	define HK_MATH_min2
	inline hkReal HK_CALL min2( hkReal x, hkReal y)
	{
		return (hkReal)__fsel( x - y , y , x);
	}

#	define HK_MATH_max2
	inline hkReal HK_CALL max2( hkReal x, hkReal y)
	{
		return (hkReal)__fsel( x - y , x , y);
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

#	define HK_MATH_prefetch128
	inline void prefetch128( void* p )
	{
		__dcbt(0, p);
	}

#	define HK_MATH_fselectGreaterEqualZero
	inline hkReal fselectGreaterEqualZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{
		return (hkReal)__fsel(testVar, ifTrue, ifFalse);
	}

#	define HK_MATH_fselectGreaterZero
	inline hkReal fselectGreaterZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{ 
		return (hkReal)__fsel(-testVar, ifTrue, ifFalse);
	}

#	define HK_MATH_fselectEqualZero
	inline hkReal fselectEqualZero( hkReal testVar, hkReal ifTrue, hkReal ifFalse)
	{ 
		return (hkReal)__fsel( -testVar, __fsel(testVar, ifTrue, ifFalse), ifFalse );
	}
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
