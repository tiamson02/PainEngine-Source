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


inline hkVector4::hkVector4(hkReal x, hkReal y, hkReal z, hkReal w)
{
	/*
	hkQuadReal ans;
	_vaddf(ans, vu0_field_X, vu0_vf0, x);
	_vaddf(ans, vu0_field_Y, vu0_vf0, y);
	_vaddf(ans, vu0_field_Z, vu0_vf0, z);
	_vmulf(ans, vu0_field_W, vu0_vf0, w);
	*/
	hkQuadRealUnion u;
	u.r[0] = x;
	u.r[1] = y;
	u.r[2] = z;
	u.r[3] = w; 
	m_quad = u.q;
}

inline hkVector4::hkVector4(const hkQuadReal& q)
{
	m_quad = q;
}

inline void hkVector4::operator= (const hkVector4& v)
{
	m_quad = v.m_quad;
}

inline hkVector4::hkVector4( const Havok::Vector3& v)
{
	m_quad = reinterpret_cast<const hkQuadReal&>( v );
	//_vmove(m_quad, vu0_field_W, vu0_vf0);
}

inline hkVector4::hkVector4(const hkVector4& v)
{
	m_quad = v.m_quad;
}

inline void hkVector4::set(hkReal x, hkReal y, hkReal z, hkReal w)
{
	hkQuadRealUnion u;
	u.r[0] = x;
	u.r[1] = y;
	u.r[2] = z;
	u.r[3] = w;
	m_quad = u.q;
}

inline void hkVector4::setAll(hkReal a)
{

	hkQuadRealUnion u;
	u.r[0] = a;
	u.r[1] = a;
	u.r[2] = a;
	u.r[3] = a;
	m_quad = u.q;
}

inline void hkVector4::setAll3(hkReal a)
{

	hkQuadRealUnion u;
	u.r[0] = a;
	u.r[1] = a;
	u.r[2] = a;
	u.r[3] = a;
	m_quad = u.q;
}

inline void hkVector4::setZero4()
{
	_vsub(m_quad, vu0_field_XYZW, vu0_vf0, vu0_vf0);
}

inline void hkVector4::zeroElement( int i )
{
	HK_ASSERT(i>=0 && i<4);
	if ( i == 0 )
	{
		_vsub(m_quad, vu0_field_X, vu0_vf0, vu0_vf0);
	}
	else if ( i == 1 )
	{
		_vsub(m_quad, vu0_field_Y, vu0_vf0, vu0_vf0);
	}
	else if ( i == 2 )
	{
		_vsub(m_quad, vu0_field_Z, vu0_vf0, vu0_vf0);
	}
	else //if ( i == 3 )
	{
		_vsub(m_quad, vu0_field_W, vu0_vf0, vu0_vf0);
	}
}

/* vector methods */


inline void hkVector4::add4(const hkVector4& a)
{
	_vadd(m_quad, vu0_field_XYZW, m_quad, a.m_quad);
}
inline void hkVector4::setAdd4(const hkVector4& x, const hkVector4& y)
{
	_vadd(m_quad, vu0_field_XYZW, x.m_quad, y.m_quad);
}

inline void hkVector4::sub4(const hkVector4& a)
{
	_vsub(m_quad, vu0_field_XYZW, m_quad, a.m_quad);
}
inline void hkVector4::setSub4(const hkVector4& x, const hkVector4& y)
{
	_vsub(m_quad, vu0_field_XYZW, x.m_quad, y.m_quad);
}

inline void hkVector4::mul4(const hkVector4& v)
{
	_vmul(m_quad, vu0_field_XYZW, m_quad, v.m_quad);
}
inline void hkVector4::setMul4(const hkVector4& x, const hkVector4& y)
{
	_vmul(m_quad, vu0_field_XYZW, x.m_quad, y.m_quad);
}

inline void hkVector4::mul4(hkSimdRealParameter s)
{
	_vmulbc(m_quad, vu0_field_XYZW, m_quad, s.quad(), vu0_field_X);
}
inline void hkVector4::setMul4(hkSimdRealParameter r, const hkVector4& x)
{
	_vmulbc(m_quad, vu0_field_XYZW, x.m_quad, r.quad(), vu0_field_X);
}

inline void hkVector4::addMul4(hkSimdRealParameter r, const hkVector4& x)
{
	hkQuadReal acc;
	_vmulbc(acc, vu0_field_XYZW, m_quad, vu0_vf0, vu0_field_W);
	_vmaddbc(m_quad, vu0_field_XYZW, acc, x.m_quad, r.quad(), vu0_field_X);
}

inline void hkVector4::setCross(const hkVector4& x, const hkVector4& y)
{
	hkQuadReal acc;
	_vopmula(acc, x.m_quad, y.m_quad);
	_vopmsub(m_quad, acc, y.m_quad, x.m_quad);
}

inline void hkVector4::setInterpolate4( const hkVector4& a, const hkVector4& b, hkSimdRealParameter t)
{
	hkQuadReal s_w;
	_vsubbc(s_w, vu0_field_W, vu0_vf0, t.quad(), vu0_field_X); // note using w
	hkQuadReal acc;
	_vmulbc(acc, vu0_field_XYZW, b.m_quad, t.quad(), vu0_field_X);
	_vmaddbc(m_quad, vu0_field_XYZW, acc, a.m_quad, s_w, vu0_field_W);
}

inline void hkVector4::setNeg4(const hkVector4& x)
{

	_vmulf(m_quad, vu0_field_XYZW, x.m_quad, -1.0f);
}

inline int hkVector4::compareLessThan4(const hkVector4& a) const
{
	const hkVector4& v = *this;
	return
		((v(0)<a(0)) * HK_VECTOR3_COMPARE_MASK_X) |
		((v(1)<a(1)) * HK_VECTOR3_COMPARE_MASK_Y) |
		((v(2)<a(2)) * HK_VECTOR3_COMPARE_MASK_Z) |
		((v(3)<a(3)) * HK_VECTOR3_COMPARE_MASK_W);


	/* 
	register int mask;
	vu0_vf0 = _vsub(vu0_field_XYZW, m_quad, a.quad());
	__asm__ __volatile__ (
		"cfc2 %0, $vi17\n"
		"andi %0, %0, 0xf0\n"
		"sra %0, %0, 4\n"
			: "=r"(mask) );
	return mask;
	*/
}

inline int hkVector4::compareLessThanZero4() const
{
	const hkVector4& v = *this;
	return
		((v(0)<0) * HK_VECTOR3_COMPARE_MASK_X) |
		((v(1)<0) * HK_VECTOR3_COMPARE_MASK_Y) |
		((v(2)<0) * HK_VECTOR3_COMPARE_MASK_Z) |
		((v(3)<0) * HK_VECTOR3_COMPARE_MASK_W);


	/*
	vu0_vf0 = _vsub( vu0_field_XYZW, m_quad, _vsub(vu0_field_XYZW, vu0_vf0, vu0_vf0) );
	register int mask;
	__asm__ __volatile__ (
		"cfc2 %0, $vi17\n"
		"andi %0, %0, 0xf0\n"
		"sra %0, %0, 4\n"
			: "=r"(mask) );
	return mask;
	*/
}

inline int hkVector4::compareLessThanEqual4(const hkVector4 &a) const
{

	const hkVector4& v = *this;
	return
		((v(0)<=a(0)) * HK_VECTOR3_COMPARE_MASK_X) |
		((v(1)<=a(1)) * HK_VECTOR3_COMPARE_MASK_Y) |
		((v(2)<=a(2)) * HK_VECTOR3_COMPARE_MASK_Z) |
		((v(3)<=a(3)) * HK_VECTOR3_COMPARE_MASK_W);
}

inline void hkVector4::setAbs4(const hkVector4& x)
{
	_vabs(m_quad, vu0_field_XYZW, x.m_quad);
}

inline void hkVector4::setMin4(const hkVector4& a, const hkVector4& b)
{
	_vmini(m_quad, vu0_field_XYZW, a.quad(), b.quad());
}

inline void hkVector4::setMax4(const hkVector4& a, const hkVector4& b)
{
	_vmax(m_quad, vu0_field_XYZW, a.quad(), b.quad());
}


/* matrix3, rotation, quaternion, transform */


inline void hkVector4::_setMul3(const hkMatrix3& m, const hkVector4& v)
{
	hkQuadReal acc;
	_vmulbc ( acc,    vu0_field_XYZW,      m.getColumn(0).m_quad, v.m_quad, vu0_field_X);
	_vmaddbc( acc,    vu0_field_XYZW, acc, m.getColumn(1).m_quad, v.m_quad, vu0_field_Y);
	_vmaddbc( m_quad, vu0_field_XYZW, acc, m.getColumn(2).m_quad, v.m_quad, vu0_field_Z);
}

inline void hkVector4::_setRotatedDir(const hkRotation& t, const hkVector4& v)
{
	_setMul3(t,v);
}
 

inline void hkVector4::_setRotatedInverseDir(const hkRotation& r, const hkVector4& v)
{
	hkQuadReal res;
	_vsub ( res, vu0_field_XYZW, vu0_vf0, vu0_vf0 );

	hkQuadReal dp0;
	_vmul ( dp0, vu0_field_XYZ,      r.getColumn(0).m_quad, v.m_quad);
	_vaddbc(dp0, vu0_field_X, dp0, dp0, vu0_field_Y);
	_vaddbc(res, vu0_field_X, dp0, dp0, vu0_field_Z);

	hkQuadReal dp1;
	_vmul ( dp1, vu0_field_XYZ,      r.getColumn(1).m_quad, v.m_quad);
	_vaddbc(dp1, vu0_field_Y, dp1, dp1, vu0_field_X);
	_vaddbc(res, vu0_field_Y, dp1, dp1, vu0_field_Z);

	hkQuadReal dp2;
	_vmul ( dp2, vu0_field_XYZ,      r.getColumn(2).m_quad, v.m_quad);
	_vaddbc(dp2, vu0_field_Z, dp2, dp2, vu0_field_X);
	_vaddbc(res, vu0_field_Z, dp2, dp2, vu0_field_Y);

	m_quad = res;
}

inline void hkVector4::_setTransformedPos(const hkTransform& t, const hkVector4& v)
{
	hkQuadReal acc;
	_vmulbc ( acc,    vu0_field_XYZW,      t.getRotation().getColumn(0).m_quad, v.m_quad, vu0_field_X);
	_vmaddbc( acc,    vu0_field_XYZW, acc, t.getRotation().getColumn(1).m_quad, v.m_quad, vu0_field_Y);
	_vmaddbc( acc,    vu0_field_XYZW, acc, t.getRotation().getColumn(2).m_quad, v.m_quad, vu0_field_Z);
	_vmaddbc( m_quad, vu0_field_XYZW, acc, t.getTranslation().m_quad, vu0_vf0, vu0_field_W);
}

inline void hkVector4::_setTransformedInversePos(const hkTransform& t, const hkVector4& v)
{
	hkVector4 trans;
	trans.setSub4( v, t.getTranslation() );
	_setRotatedInverseDir( t.getRotation(), trans );
}

inline void hkVector4::setRotatedDir(const hkQuaternion& q, const hkVector4& v)
{
//!me optimize ( see Ps2AsmVector4 )
	// (2*qr*qr-1) * v  +  (2*qi.v) * qi  +  (2*qr) * (qi x v)
	hkQuadRealUnion u;
	u.r[0] = q(3)*q(3) - 0.5f;
	u.r[1] = q(0)*v(0) + q(1)*v(1) + q(2)*v(2);
	u.r[2] = q(3);
	u.r[3] = 0;
	hkQuadReal scales;
   	_vadd(scales, vu0_field_XYZ, u.q, u.q);

	hkVector4 ixt;
	ixt.setCross( q.getImag(), v);

	hkQuadReal acc;
	_vmulbc ( acc,    vu0_field_XYZ,      v.m_quad,           scales, vu0_field_X);
	_vmaddbc( acc,    vu0_field_XYZ, acc, q.getImag().m_quad, scales, vu0_field_Y);
	_vmaddbc( m_quad, vu0_field_XYZ, acc, ixt.m_quad,         scales, vu0_field_Z);
}

inline void hkVector4::setRotatedInverseDir(const hkQuaternion& q, const hkVector4& v)
{
	// as above with q.imag = -q.imag, and much cancelling
	hkQuadRealUnion u;
	u.r[0] = q(3) * q(3) - 0.5f;
	u.r[1] = q(0)*v(0) + q(1)*v(1) + q(2)*v(2);
	u.r[2] = -q(3);
	u.r[3] = 0;
	hkQuadReal scales;
   	_vadd(scales, vu0_field_XYZ, u.q, u.q);

	hkVector4 ixt;
	ixt.setCross( q.getImag(), v);

	hkQuadReal acc;
	_vmulbc ( acc,    vu0_field_XYZ,      v.m_quad,           scales, vu0_field_X);
	_vmaddbc( acc,    vu0_field_XYZ, acc, q.getImag().m_quad, scales, vu0_field_Y);
	_vmaddbc( m_quad, vu0_field_XYZ, acc, ixt.m_quad,         scales, vu0_field_Z);
}

inline hkSimdReal hkVector4::dot3(const hkVector4& x) const
{
/*
	hkQuadReal ret;
	_vmul  (ret, vu0_field_XYZ, m_quad, x.m_quad);
	_vaddbc(ret, vu0_field_X, ret, ret, vu0_field_Y);
	_vaddbc(ret, vu0_field_X, ret, ret, vu0_field_Z);	
*/	

	register hkQuadReal squared, t0, acc;
	hkQuadReal ret;
		
	_vmul	(squared, vu0_field_XYZ, m_quad, x.m_quad);
	_vaddbc	(t0, vu0_field_X, vu0_vf0, vu0_vf0, vu0_field_W );
	_vaddbc(acc, vu0_field_X, squared, squared, vu0_field_Y );
	_vmaddbc(ret, vu0_field_X, acc, t0, squared, vu0_field_Z );

	return ret;
}

inline hkReal hkVector4::dot3fpu(const hkVector4& a) const
{
	const hkVector4& t = *this;
	return (t(0) * a(0)) + ( t(1) * a(1) ) + ( t(2) * a(2) );
}

inline hkSimdReal hkVector4::dot4(const hkVector4& x) const
{
/*
	hkQuadReal t0, t1, t2, ret;
   	_vmul(t0, vu0_field_XYZW, m_quad, x.m_quad);
	_vaddbc(t1, vu0_field_X, t0, t0, vu0_field_Y);
	_vaddbc(t2, vu0_field_X, t1, t0, vu0_field_Z);
	_vaddbc(ret, vu0_field_X, t2, t0, vu0_field_W);
*/	
	
	hkQuadReal t0, ret;
   	_vmul(ret, vu0_field_XYZW, m_quad, x.m_quad);
	_vaddbc(t0, vu0_field_W, ret, ret, vu0_field_Z);
	_vaddbc(ret, vu0_field_X, ret, ret, vu0_field_Y);
	_vaddbc(ret, vu0_field_X, ret, t0, vu0_field_W);	
	
	return ret;
}

inline hkSimdReal hkVector4::horizontalAdd3() const
{
	hkQuadReal t0, ret;
	_vaddbc(t0,  vu0_field_X, m_quad, m_quad, vu0_field_Y);
	_vaddbc(ret, vu0_field_X, t0,     m_quad, vu0_field_Z);
	return ret;
}

inline hkSimdReal hkVector4::horizontalAdd4() const
{
	hkQuadReal t0, t1, ret;
	_vaddbc(t0,  vu0_field_X, m_quad, m_quad, vu0_field_Y);
	_vaddbc(t1,  vu0_field_X, m_quad, t0,     vu0_field_Z);
	_vaddbc(ret, vu0_field_X, m_quad, t1,     vu0_field_W);
	return ret;
}


inline hkSimdReal hkVector4::length3() const
{
	hkSimdReal l3 = this->dot3(*this);
	return hkMath::sqrt( l3 );
}

inline hkSimdReal hkVector4::lengthSquared3() const
{
	return this->dot3(*this);
}

inline hkSimdReal hkVector4::lengthInverse3() const
{
	hkSimdReal l3 = this->dot3(*this);
	return hkMath::sqrtInverse( l3 );
}

inline hkSimdReal hkVector4::length4() const
{
	hkSimdReal l2 = this->dot4(*this);
	return hkMath::sqrt( l2 );
}

inline hkSimdReal hkVector4::lengthSquared4() const
{
	return this->dot4(*this);
}

inline hkSimdReal hkVector4::lengthInverse4() const
{
	hkSimdReal l2 = this->dot4(*this);
	return hkMath::sqrtInverse( l2 );
}

inline void hkVector4::normalize3()
{
	hkSimdReal l2 = this->dot3(*this);
	hkSimdReal scale = hkMath::sqrtInverse( l2 );
	this->setMul4(scale, m_quad);
}
inline hkSimdReal hkVector4::normalizeWithLength3()
{
	hkSimdReal l2 = this->dot3(*this);
	hkSimdReal scale = hkMath::sqrtInverse( l2 );
	this->setMul4(scale, m_quad);
	hkQuadReal ret;
	_vmul(ret, vu0_field_X, l2.quad(), scale.quad());
	return ret;
}

inline void hkVector4::normalize4()
{
	hkSimdReal l2 = this->dot4(*this);
	hkSimdReal scale = hkMath::sqrtInverse( l2 );
	this->setMul4(scale, m_quad);
}
inline hkSimdReal hkVector4::normalizeWithLength4()
{
	hkSimdReal l2 = this->dot4(*this);
	hkSimdReal scale = hkMath::sqrtInverse( l2 );
	this->setMul4(scale, m_quad);
	hkQuadReal ret;
	_vmul(ret, vu0_field_X, l2.quad(), scale.quad());
	return ret;
}

inline void hkVector4::fastNormalize3()
{
	normalize3();
}
inline hkSimdReal hkVector4::fastNormalizeWithLength3()
{
	return normalizeWithLength3();
}



/* operator () */

inline hkReal& hkVector4::operator() (int i)
{
	HK_ASSERT(i>=0 && i<4);
	return reinterpret_cast<hkReal*>(&m_quad)[i];
}

inline const hkReal& hkVector4::operator() (int i) const
{
	HK_ASSERT(i>=0 && i<4);
	return reinterpret_cast<const hkReal*>(&m_quad)[i];
}

inline hkSimdReal hkVector4::getSimdAt(int i) const
{
	// this looks ugly but is needed when this function is out of line (debug mode)
	// when inlined in release it compiles to one statement
	HK_ASSERT(i>=0 && i<4);
	if( i == 0 )
	{
		hkQuadReal ret;
		_vaddbc(ret, vu0_field_X, vu0_vf0, m_quad, vu0_field_X );
		return ret;
	}
	else if( i == 1 )
	{
		hkQuadReal ret;
		_vaddbc(ret, vu0_field_X, vu0_vf0, m_quad, vu0_field_Y );
		return ret;
	}
	else if( i == 2 )
	{
		hkQuadReal ret;
		_vaddbc(ret, vu0_field_X, vu0_vf0, m_quad, vu0_field_Z );
		return ret;
	}
	else //if( i == 3 )
	{
		hkQuadReal ret;
		_vaddbc(ret, vu0_field_X, vu0_vf0, m_quad, vu0_field_W );
		return ret;
	}
}

inline void hkVector4::broadcast(int i)
{
	HK_ASSERT(i>=0 && i<4);
	if( i == 0 )
	{
		_vaddbc(m_quad, vu0_field_XYZW, getZero().m_quad, m_quad, vu0_field_X );
	}
	else if( i == 1 )
	{
		_vaddbc(m_quad, vu0_field_XYZW, getZero().m_quad, m_quad, vu0_field_Y );
	}
	else if( i == 2 )
	{
		_vaddbc(m_quad, vu0_field_XYZW, getZero().m_quad, m_quad, vu0_field_Z );
	}
	else //if( i == 3 )
	{
		_vaddbc(m_quad, vu0_field_XYZW, getZero().m_quad, m_quad, vu0_field_W );
	}
}
/*
inline const hkVector4 operator+ (const hkVector4& a, const hkVector4& b)
{
	hkQuadReal ret;
	_vadd(ret, vu0_field_XYZW, a.quad(), b.quad());
	return ret;
}
inline const hkVector4 operator- (const hkVector4& a, const hkVector4& b)
{
	hkQuadReal ret;
	_vsub(ret, vu0_field_XYZW, a.quad(), b.quad());
	return ret;
}
inline const hkVector4 operator* (hkSimdRealParameter s, const hkVector4& v)
{
	hkQuadReal ret;
	_vmulbc(ret, vu0_field_XYZW, v.quad(), s.quad(), vu0_field_X);
	return ret;
}

*/
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
