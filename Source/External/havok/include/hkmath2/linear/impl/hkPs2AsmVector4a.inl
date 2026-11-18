/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


/* quad, here for inlining */

inline hkQuadReal& hkVector4::quad()
{
	return m_quad;
}

inline const hkQuadReal& hkVector4::quad() const
{
	return m_quad;
}


/* construct, assign, zero */

inline hkVector4::hkVector4(hkReal a, hkReal b, hkReal c, hkReal d)
{
	hkQuadRealUnion u;
	u.r[0] = a;
	u.r[1] = b;
	u.r[2] = c;
	u.r[3] = d; 
	m_quad = u.q;
}


inline hkVector4::hkVector4(const hkQuadReal& q)
	: m_quad(q)
{
}


inline hkVector4::hkVector4( const Havok::Vector3& v)
{
	m_quad = reinterpret_cast<const hkQuadReal&>( v );
}


inline void hkVector4::operator= (const hkVector4& v)
{
	m_quad = v.m_quad;
}

inline hkVector4::hkVector4( const hkVector4& v)
{
	m_quad = v.m_quad;
}

inline void hkVector4::set(hkReal a, hkReal b, hkReal c, hkReal d)
{
	hkQuadRealUnion u;
	u.r[0] = a;
	u.r[1] = b;
	u.r[2] = c;
	u.r[3] = d;
	m_quad = u.q;
}

inline void hkVector4::setAll(hkReal a)
{
	asm("qmtc2 %1, %0  \n"			\
		"vaddax ACC, vf0, %0 \n"	\
		"vmsubw %0, vf0, vf0" : "=j"(m_quad) : "r" (a) );
}

inline void hkVector4::setAll3(hkReal a)
{
	asm("qmtc2 %1, %0  \n"			\
		"vaddx %0, vf0, %0" : "=j"(m_quad) : "r" (a) );
}

inline void hkVector4::setZero4()
{
	asm(
"		vsub.xyzw %0, vf0, vf0"
	: "=j"(m_quad) );
 
	
}

inline void hkVector4::zeroElement( int i )
{
	HK_ASSERT(i>=0 && i<4);
	if ( i == 0 )
	{
		asm ("vmulx.x %0, %0, vf0" : "+j"(m_quad) );	
	}
	else if ( i == 1 )
	{
		asm("vmulx.y %0, %0, vf0" : "+j"(m_quad) );
	}
	else if ( i == 2 )
	{
		asm("vmulx.z %0, %0, vf0" : "+j"(m_quad) );
	}
	else //if ( i == 3 )
	{
		asm("vmulx.w %0, %0, vf0" : "+j"(m_quad) );
	}
}


/* vector methods */

inline void hkVector4::add4(const hkVector4& v)
{
	asm("vadd.xyzw %0, %0, %1" : "+j"(m_quad) : "j"(v.m_quad) );
}

inline void hkVector4::setAdd4(const hkVector4& v0, const hkVector4& v1)
{
	asm("vadd.xyzw %0, %1, %2" : "=j"(m_quad) : "j"(v0.m_quad), "j"(v1.m_quad) );
}

inline void hkVector4::sub4(const hkVector4& v)
{
	asm("vsub.xyzw %0, %0, %1" : "+j"(m_quad) : "j"(v.m_quad) );
}

inline void hkVector4::setSub4(const hkVector4& v0, const hkVector4& v1)
{
	asm("vsub.xyzw %0, %1, %2" : "=j"(m_quad) : "j"(v0.m_quad), "j"(v1.m_quad) );
}

inline void hkVector4::mul4(const hkVector4& v)
{
	asm("vmul.xyzw %0, %0, %1" : "+j"(m_quad) : "j"(v.m_quad) );
}

inline void hkVector4::setMul4(const hkVector4& v0, const hkVector4& v1)
{
	asm("vmul.xyzw %0, %1, %2" : "=j"(m_quad) : "j"(v0.m_quad), "j"(v1.m_quad)  );
}

inline void hkVector4::mul4(hkSimdRealParameter s)
{  
	asm("vmulx.xyzw %0, %0, %1" : "+j"(m_quad) : "j"(s.quad()) );
}

inline void hkVector4::setMul4(hkSimdRealParameter s, const hkVector4& v)
{
	asm("vmulx.xyzw %0, %2, %1" : "=j"(m_quad) : "j"(s.quad()), "j"(v.m_quad) );
}
 
inline void hkVector4::addMul4(hkSimdRealParameter s, const hkVector4& v)
{
	asm(
"		vmulaw.xyzw ACC, %0, vf0	\n"
"		vmaddx.xyzw %0, %2, %1	"
	: "+j"(m_quad) : "j"(s.quad()), "j"(v.m_quad) );
}

inline void hkVector4::setCross(const hkVector4& v1, const hkVector4& v2)
{
	asm( 
"		vopmula.xyz	ACC, %1, %2		# vf1 = cross(vf1, vf2)	\n"
"		vopmsub.xyz %0,  %2, %1	"
	: "=j"(m_quad) : "j"(v1.m_quad), "j"(v2.m_quad) );
}

inline void hkVector4::setInterpolate4(const hkVector4& v0, const hkVector4& v1, hkSimdRealParameter t)
{
	// this = (1.0f-t)*v0 + t*v1 == v0 - t*v0 + t*v1

	asm(
"		vmulaw.xyzw 	ACC, %2, vf0	# v0		 	\n"
"		vmsubax.xyzw 	ACC, %2, %1		# - t*v0		\n"
"		vmaddx.xyzw 	%0, %3, %1		# + t*v1		" 
	: "=j"(m_quad) : "j"(t.quad()), "j"(v0.m_quad), "j"(v1.m_quad) );
}

inline int hkVector4::compareLessThan4(const hkVector4& a) const
{

	register int mask;

	asm (
"		vsub.xyzw   vf0, %1, %2	# %1 = this - a		\n"	
"		vnop					\n"
"		vnop					\n"
"		vnop					\n"
"		vnop					\n"
"		cfc2 %0, $vi17		\n"	
"		andi %0, %0, 0xf0	\n"	
"		sra %0, %0, 4		"
: "=r"(mask) : "j"( m_quad ), "j"( a.m_quad ) );
	return mask;
}

inline int hkVector4::compareLessThanEqual4(const hkVector4& a) const
{
	register int mask;
	register int temp;

	asm (
"		vsub.xyzw   vf0, %2, %3	# %2 = this - a		\n"	
"		vnop					\n"
"		vnop					\n"
"		vnop					\n"
"		vnop					\n"
"		cfc2 %1, $vi17		\n"
"		sra  %0, %1, 4		\n"
"		or   %0, %0, %1		\n"
"		andi %0, %0, 0x0f	"
: "=r"(mask), "=r"(temp) : "j"( m_quad ), "j"( a.m_quad ) );	

	return mask;
}

inline int hkVector4::compareLessThanZero4() const
{
	return compareLessThan4( hkVector4::getZero() );
}


inline void hkVector4::setNeg4(const hkVector4& v)
{

	// vf0 = (0,0,0,1) so zero the acc by vf0 * vf0(x)
	// then our result uses multiply subract operator
	// result = 0 - result*1   where 1 is from vf0.w 
	asm(
"		vmulax.xyzw ACC, vf0, vf0	# zero ACC				\n"
"		vmsubw.xyzw %0,	 %1, vf0    # 0 - result = -result	"
		: "=j"(m_quad) : "j"(v.m_quad) );

}


inline void hkVector4::setAbs4(const hkVector4& v)
{
	asm( "vabs.xyzw %0, %1" : "=j"(m_quad) : "j"(v.m_quad) );
}

inline void hkVector4::setMin4(const hkVector4& v0, const hkVector4& v1)
{
	asm( "vmini.xyzw %0, %1, %2" : "=j"(m_quad) : "j"(v0.m_quad), "j"(v1.m_quad) );
}

inline void hkVector4::setMax4(const hkVector4& v0, const hkVector4& v1)
{
	asm( "vmax.xyzw %0, %1, %2" : "=j"(m_quad) : "j"(v0.m_quad), "j"(v1.m_quad) );
}



/* length and distance */

inline hkSimdReal hkVector4::dot3(const hkVector4& a) const
{
	hkQuadReal result;
	register hkQuadReal t0;
	asm(
"		vmul.xyz 	%0, %2, %3														\n"
"		vaddw.x 	%1, vf0, vf0	# tricky: make %1 = ( 1,0,0,0 )					\n"
"		vadday.x 	ACC, %0, %0 	# now y*y + x*x is in ACC.x						\n"
"		vmaddz.x 	%0, %1, %0 		# result.x = ACC.x + %3.x * %0.z = yy+xx+1*zz 	"
	: "=&j"(result), "=&j"(t0) : "j"( m_quad ), "j"( a.m_quad ) );

	return result; 
}

inline hkReal hkVector4::dot3fpu(const hkVector4& v) const
{
	register float d;
	register float t0;
	register float t1;
	register float t2;
	register float t3;
	__asm__ __volatile__ ("
	.set noreorder
		lwc1	%0, 0x0(%5)	# t0 = x
		lwc1	%1, 0x0(%6)	# t1 = v.x
		lwc1	%2, 0x4(%5)	# t2 = y
		mula.s	%0, %1		# A = t0 * t1
		lwc1	%3, 0x4(%6)	# t3 = v.y
		lwc1	%0, 0x8(%5)	# t0 = z
		madda.s	%2, %3		# A += t2 * t3
		lwc1	%1, 0x8(%6)	# t1 = v.z
		madd.s	%4, %0, %1	# d = A + t0 * t1
	.set reorder
	" : "=&f"(t0), "=&f"(t1), "=&f"(t2), "=&f"(t3), "=f"(d) : "r"(this), "r"(&v) );
	return d;
}

inline hkSimdReal hkVector4::dot4(const hkVector4& a) const
{
	hkQuadReal result;
	register hkQuadReal temp;
	asm( 
"		vmul.xyzw	%0,   %2,   %3										\n"
"		vaddw.x 	%1x,  vf0x, vf0w	# %1 = ( 1,0,0,0 )				\n"
"		vadday.x	ACCx, %0x,  %0y		# ACC.x = x*x + y*y				\n"
"		vmaddaz.x	ACCx, %1x,  %0z		# ACC.x = ACC.x + 1*z*z			\n"
"		vmaddw.x	%0x,  %1x,  %0w		# result.x = ACC.x + 1*w*w		"
	: "=&j"(result), "=&j"(temp): "j"( m_quad ), "j"( a.m_quad ) );
	
	return result;
}

inline hkSimdReal hkVector4::horizontalAdd3() const
{

	hkQuadReal result;
	asm( 
"		vaddy.x  	%0, %1, %1 	\n"
"		vaddz.x  	%0, %0, %1 	"
	: "=&j"(result) : "j"( m_quad ) );
	
	return result;
}

inline hkSimdReal hkVector4::horizontalAdd4() const
{
	hkQuadReal result;
	register hkQuadReal temp; 
	asm( 
"		vaddz.w		%1, %2, %2 	\n"
"		vaddy.x		%0, %2, %2 	\n"
"		vaddw.x		%0, %0, %1	"
	: "=&j"(result), "=&j"(temp) : "j"( m_quad ) );
	
	return result;
}

#define ME_NO_SQRT 0
/*
	Excerpt from /usr/local/sce/1st_read/notice.txt
   The above problem can be avoided by not locating the divide 
    instructions in the following points:

       1. Two steps of the branch destination instructions
       2. Three steps following the branch instruction (including
	  the branch delay slot)
       3. One step instruction immediately after the SYNC.P 
       4. The first two steps of the VU0 micro code
*/

inline hkSimdReal hkVector4::length3() const
{
//!me there is a bug in the fdiv unit.  See /usr/local/sce/1st_read/notice.txt
//!me I don't fully understand it, but this is conservative.  
//!me Trying to avoid fdiv use being: "2 steps of the branch and 3 steps after branch"
//!me that's why dot3/4 is cut and pasted several times here.
#if ME_NO_SQRT
	hkSimdReal l3 = this->dot3(*this);
	return hkMath::sqrt( l3 );
#else
	register hkQuadReal dots;
	asm("vmul.xyz %0, %1, %1" : "=j"(dots) : "j"(m_quad) ); // x2,y2,z2,?
	register hkQuadReal temp0;
	asm("vaddw.x %0, vf0, vf0" : "=j"(temp0) ); // 1,?,?,?
	asm("vadday.x ACC, %0, %0\n"
		"vmaddz.x %0, %1, %0" : "+j"(dots) : "j"(temp0) ); // x2+y2+z2,?,?,?
	hkQuadReal result;
	asm("vsqrt Q, %1x\n"
		"vwaitq\n"
		"vaddq.x %0, vf0, Q" : "=j"(result) : "j"(dots) ); // len3,?,?,?
	return result;
#endif
}


inline hkSimdReal hkVector4::lengthSquared3() const
{
	return this->dot3(*this);
}


inline hkSimdReal hkVector4::lengthInverse3() const
{
#if ME_NO_SQRT
	hkSimdReal l3 = this->dot3(*this);
	return hkMath::sqrtInverse(l3);
#else
	hkQuadReal result;
	register hkQuadReal t0;
	
	// rsqrt( dot3) 
	asm(
"		vmul.xyz 	%0, %2, %2														\n"
"		vaddw.x 	%1, vf0, vf0	# tricky: make %3 = ( 1,0,0,0 )					\n"
"		vadday.x 	ACC, %0, %0 	# now y*y + x*x is in ACC.x						\n"
"		vmaddz.x 	%0, %1, %0 		# result.x = ACC.x + %3.x * %0.z = yy+xx+1*zz 	\n"
"		vrsqrt		Q, vf00w, %0x													\n"
"		vwaitq																		\n"
"		vaddq.x		%0, vf0, Q														\n"
"		vnop																		\n"
"		vnop																		"
	: "+j"(result), "+j"(t0) : "j"( m_quad ) );	

	return result;
#endif
}


inline hkSimdReal hkVector4::length4() const
{
#if ME_NO_SQRT
	hkSimdReal l2 = this->dot4(*this);
	return hkMath::sqrt( l2 );
#else
	hkQuadReal result;
	register hkQuadReal temp;

	// sqrt( dot4 ) 
	asm( 
"		vmul.xyzw	%0,   %2,   %2										\n"
"		vaddw.x 	%1x,  vf0x, vf0w	# %1 = ( 1,0,0,0 )				\n"
"		vadday.x	ACCx, %0x,  %0y		# ACC.x = x*x + y*y				\n"
"		vmaddaz.x	ACCx, %1x,  %0z		# ACC.x = ACC.x + 1*z*z			\n"
"		vmaddw.x	%0x,  %1x,  %0w		# result.x = ACC.x + 1*w*w		\n"
"		vsqrt		Q, %0x												\n"
"		vwaitq															\n"
"		vaddq.x		%0, vf0, Q											\n"
"		vnop															\n"
"		vnop															"
	: "+j"(result), "+j"(temp): "j"( m_quad ) );
	
	return result;
#endif
}


inline hkSimdReal hkVector4::lengthSquared4() const
{
	return this->dot4(*this);
}


inline hkSimdReal hkVector4::lengthInverse4() const
{
#if ME_NO_SQRT
	hkSimdReal l2 = this->dot4(*this);
	return hkMath::sqrtInverse(l2);
#else
	hkQuadReal result;
	register hkQuadReal temp;

	// rsqrt( dot4 ) 
	asm( 
"		vmul.xyzw	%0,   %2,   %2										\n"
"		vaddw.x 	%1x,  vf0x, vf0w	# %1 = ( 1,0,0,0 )				\n"
"		vadday.x	ACCx, %0x,  %0y		# ACC.x = x*x + y*y				\n"
"		vmaddaz.x	ACCx, %1x,  %0z		# ACC.x = ACC.x + 1*z*z			\n"
"		vmaddw.x	%0x,  %1x,  %0w		# result.x = ACC.x + 1*w*w		\n"
"		vrsqrt		Q, vf00w, %0x										\n"
"		vwaitq															\n"
"		vaddq.x		%0, vf0, Q											\n"
"		vnop															\n"
"		vnop															"
	: "+j"(result), "+j"(temp): "j"( m_quad ) );
	
	return result;
#endif
}


inline void hkVector4::normalize3()
{
#if ME_NO_SQRT
	this->mul4( this->lengthInverse3() );
#else
	register hkQuadReal t0, t1;
	
	// this*rsqrt( dot3) 
	asm(
"		vmul.xyz 	%1, %0, %0														\n"
"		vaddw.x 	%2, vf0, vf0	# tricky: make %3 = ( 1,0,0,0 )					\n"
"		vadday.x 	ACC, %1, %1 	# now y*y + x*x is in ACC.x						\n"
"		vmaddz.x 	%1, %2, %1 		# result.x = ACC.x + %3.x * %0.z = yy+xx+1*zz 	\n"
"		vrsqrt		Q, vf00w, %1x													\n"
"		vwaitq																		\n"
"		vmulq.xyz	%0, %0, Q														\n"
"		vnop																		\n"
"		vnop																		"
	: "+j"( m_quad ), "=&j"(t0), "=&j"(t1) );	

#endif
}

 
inline hkSimdReal hkVector4::normalizeWithLength3()
{
#if ME_NO_SQRT

	hkReal len = this->length3();
	hkReal invLen = 1.0f / len;
	this->mul4( invLen );
	return len;

#else
	hkQuadReal result;
	register hkQuadReal t0,t1;

	asm(
"		vmul.xyz 	%2, %0, %0														\n"
"		vaddw.x 	%3, vf0, vf0	# tricky: make %3 = ( 1,0,0,0 )					\n"
"		vadday.x 	ACC, %2, %2 	# now y*y + x*x is in ACC.x						\n"
"		vmaddz.x 	%2, %3, %2 		# result.x = ACC.x + %3.x * %0.z = yy+xx+1*zz 	\n"
"		vrsqrt		Q, vf0w, %2x													\n"
"		vwaitq																		\n"
"		vmulq.xyz	%0, %0, Q														\n"
"		vmulq.x		%1x, %2x, Q														\n"
"		vnop																		"
	: "+j"( m_quad ), "=&j"(result), "=&j"(t0) , "=&j"(t1) );	

	return result;

#endif
}


inline void hkVector4::normalize4()
{
#if ME_NO_SQRT
	this->mul4( this->lengthInverse4() );
#else

	register hkQuadReal t0, t1;
	
	// this*rsqrt( dot4) 
	asm(
"		vmul.xyzw	%1,   %0,   %0										\n"
"		vaddw.x 	%2x,  vf0x, vf0w	# %1 = ( 1,0,0,0 )				\n"
"		vadday.x	ACCx, %1x,  %1y		# ACC.x = x*x + y*y				\n"
"		vmaddaz.x	ACCx, %2x,  %1z		# ACC.x = ACC.x + 1*z*z			\n"
"		vmaddw.x	%1x,  %2x,  %1w		# result.x = ACC.x + 1*w*w		\n"
"		vrsqrt		Q, vf00w, %1x										\n"
"		vwaitq															\n"
"		vmulq.xyzw	%0, %0, Q											\n"
"		vnop															\n"
"		vnop															"
	: "+j"( m_quad ), "=&j"(t0), "=&j"(t1) );	

#endif
}


inline hkSimdReal hkVector4::normalizeWithLength4()
{
#if ME_NO_SQRT
	hkReal len = this->length4();
	this->mul4( hkSimdReal((1.0f)/len) );
	return len;
#else
	hkQuadReal result;
	register hkQuadReal t0,t1;

	asm(
"		vmul.xyzw 	%2, %0, %0														\n"
"		vaddw.x 	%3, vf0, vf0	#												\n"
"		vadday.x 	ACCx, %2x, %2y 	#												\n"
"		vmaddaz.x	ACCx, %3x, %2z  #												\n"
"		vmaddw.x 	%2x,  %3x, %2w	#											 	\n"
"		vrsqrt		Q, vf0w, %2x													\n"
"		vwaitq																		\n"
"		vmulq.xyzw	%0, %0, Q														\n"
"		vmulq.x		%1x, %2x, Q														\n"
"		vnop																		"
	: "+j"( m_quad ), "=&j"(result), "=&j"(t0) , "=&j"(t1) );	

	return result;
#endif
}


inline void hkVector4::fastNormalize3()
{
	normalize3();
}
inline hkSimdReal hkVector4::fastNormalizeWithLength3()
{
	return normalizeWithLength3();
}


/* operator (),[] */

inline hkReal& hkVector4::operator() (int i)
{
	HK_ASSERT(i>=0 && i<4);
	return ((hkReal*)(&m_quad))[i];
}

inline const hkReal& hkVector4::operator() (int i) const
{
	HK_ASSERT(i>=0 && i<4);
	return ((hkReal*)(&m_quad))[i];
}

inline hkSimdReal hkVector4::getSimdAt(int i) const
{
	HK_ASSERT(i>=0 && i<4);
	return ((hkReal*)(&m_quad))[i];
}

inline void hkVector4::broadcast(int i)
{
	HK_ASSERT2(i>=0 && i<4, "index error in broadcast");

	if ( i == 0 )
	{
		asm("vaddax ACC, vf0, %0 \n"	\
			"vmsubw %0, vf0, vf0" : "+j"(m_quad) );
	}
	else if ( i == 1 )
	{
		asm("vadday ACC, vf0, %0 \n"	\
			"vmsubw %0, vf0, vf0" : "+j"(m_quad) );
	}
	else if ( i == 2 )
	{
		asm("vaddaz ACC, vf0, %0 \n"	\
			"vmsubw %0, vf0, vf0" : "+j"(m_quad) );
	}
	else //if ( i == 3 )
	{
		asm("vaddaw ACC, vf0, %0 \n"	\
			"vmsubw %0, vf0, vf0" : "+j"(m_quad) );
	}

}

// convenience operators
/*
inline const hkVector4 operator+ (const hkVector4& a, const hkVector4& b)
{
	return hkVector4(a(0)+b(0), a(1)+b(1), a(2)+b(2));
}
inline const hkVector4 operator- (const hkVector4& a, const hkVector4& b)
{
	return hkVector4(a(0)-b(0), a(1)-b(1), a(2)-b(2));
}
inline const hkVector4 operator* (hkSimdRealParameter s, const hkVector4& v)
{
	return hkVector4((hkReal)s*v(0), (hkReal)s*v(1), (hkReal)s*v(2));
}
*/

#undef ME_NO_SQRT

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
