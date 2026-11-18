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
	m_quad = _mm_set_ps(d,c,b,a);
}

inline hkVector4::hkVector4(const hkQuadReal& q)
{
	m_quad = q;
}

inline void hkVector4::operator= (const hkVector4& v)
{
	m_quad = v.m_quad;
}

inline hkVector4::hkVector4(const hkVector4& v)
{
	m_quad = v.m_quad;
}

inline hkVector4::hkVector4( const Havok::Vector3& v)
{
	const hkReal *r = reinterpret_cast<const hkReal*>( &v );
	(*this)(0) = r[0];
	(*this)(1) = r[1];
	(*this)(2) = r[2];
	(*this)(3) = 0.0f;
}

inline void hkVector4::set(hkReal a, hkReal b, hkReal c, hkReal d)
{
	m_quad = _mm_set_ps(d,c,b,a);
}

inline void hkVector4::setAll(hkReal a)
{
	m_quad = _mm_set_ps1(a);
}

inline void hkVector4::setAll3(hkReal a)
{
	m_quad = _mm_set_ps1(a);
}

inline void hkVector4::setZero4()
{
	m_quad = _mm_setzero_ps();
}

inline void hkVector4::zeroElement( int i )
{
	static const hkUint32 HK_ALIGNED_VARIABLE( cx[4], 16) = { 0x00000000, 0xffffffff, 0xffffffff, 0xffffffff };
	static const hkUint32 HK_ALIGNED_VARIABLE( cy[4], 16) = { 0xffffffff, 0x00000000, 0xffffffff, 0xffffffff };
	static const hkUint32 HK_ALIGNED_VARIABLE( cz[4], 16) = { 0xffffffff, 0xffffffff, 0x00000000, 0xffffffff };
	static const hkUint32 HK_ALIGNED_VARIABLE( cw[4], 16) = { 0xffffffff, 0xffffffff, 0xffffffff, 0x00000000 };
	if ( i == 0 )
	{
		m_quad = _mm_and_ps( m_quad, *(hkQuadReal*)&cx );
	}
	else	if ( i == 1 )
	{
		m_quad = _mm_and_ps( m_quad, *(hkQuadReal*)&cy );
	}
	else	if ( i == 2 )
	{
		m_quad = _mm_and_ps( m_quad, *(hkQuadReal*)&cz );
	}
	else	if ( i == 3 )
	{
		m_quad = _mm_and_ps( m_quad, *(hkQuadReal*)&cw );
	}
}


/* vector methods */

inline void hkVector4::add4(const hkVector4& v)
{
	m_quad = _mm_add_ps( m_quad, v.m_quad );
}

inline void hkVector4::setAdd4(const hkVector4& v0, const hkVector4& v1)
{
	m_quad = _mm_add_ps(v0.m_quad, v1.m_quad);
}

inline void hkVector4::sub4(const hkVector4& v)
{
	m_quad = _mm_sub_ps( m_quad, v.m_quad );
}

inline void hkVector4::setSub4(const hkVector4& v0, const hkVector4& v1)
{
	m_quad = _mm_sub_ps(v0.m_quad, v1.m_quad);
}

inline void hkVector4::mul4(const hkVector4& v)
{              
	m_quad = _mm_mul_ps( m_quad, v.m_quad );
}

inline void hkVector4::setMul4(const hkVector4& a, const hkVector4& b)
{
	m_quad = _mm_mul_ps( a.m_quad, b.m_quad );
}

inline void hkVector4::mul4(hkSimdRealParameter s)
{              
	m_quad = _mm_mul_ps( s.broadcast(), m_quad );
}

inline void hkVector4::setMul4(hkSimdRealParameter r, const hkVector4& v1)
{
	m_quad = _mm_mul_ps( r.broadcast(), v1.m_quad);
}

inline void hkVector4::addMul4(hkSimdRealParameter r, const hkVector4& v1)
{
	m_quad = _mm_add_ps( m_quad,
			_mm_mul_ps( r.broadcast(), v1.m_quad) );
}

inline void hkVector4::setCross(const hkVector4& v0, const hkVector4& v1)
{
	hkQuadReal cross0;
	cross0 = _mm_mul_ps(
			_mm_shuffle_ps(v0.m_quad, v0.m_quad, _MM_SHUFFLE(3,0,2,1)),
			_mm_shuffle_ps(v1.m_quad, v1.m_quad, _MM_SHUFFLE(3,1,0,2)) );
	hkQuadReal cross1;
	cross1 = _mm_mul_ps(
			_mm_shuffle_ps(v0.m_quad, v0.m_quad, _MM_SHUFFLE(3,1,0,2)),
			_mm_shuffle_ps(v1.m_quad, v1.m_quad, _MM_SHUFFLE(3,0,2,1)) );
	m_quad = _mm_sub_ps(cross0, cross1);
}

inline void hkVector4::setInterpolate4(const hkVector4& v0, const hkVector4& v1, hkSimdRealParameter _t)
{
	hkQuadReal t = _t.broadcast();
	hkQuadReal s = _mm_sub_ps( _mm_set_ps(1,1,1,1), t);
	m_quad = _mm_add_ps(
			_mm_mul_ps(s, v0.m_quad),
			_mm_mul_ps(t, v1.m_quad) );
}

inline int hkVector4::compareLessThan4(const hkVector4 &a) const
{
	return _mm_movemask_ps( _mm_cmplt_ps(m_quad, a.quad()) );
}

inline int hkVector4::compareLessThanEqual4(const hkVector4 &a) const
{
	return _mm_movemask_ps( _mm_cmple_ps(m_quad, a.quad()) );
}

inline int hkVector4::compareLessThanZero4() const
{
	return _mm_movemask_ps( m_quad );
}

inline void hkVector4::setNeg4(const hkVector4& v)
{
	static const hkUint32 HK_ALIGNED_VARIABLE(negateMask[4], 16) = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
	m_quad = _mm_xor_ps(v.m_quad, *(hkQuadReal*)&negateMask);
}

inline void hkVector4::setAbs4(const hkVector4& v)
{
	static const hkUint32 HK_ALIGNED_VARIABLE(absMask[4], 16) = { 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff };
	m_quad = _mm_and_ps(v.m_quad, *(hkQuadReal*)&absMask);
}

inline void hkVector4::setMin4(const hkVector4& a, const hkVector4& b)
{
	m_quad = _mm_min_ps(a.quad(), b.quad());
}

inline void hkVector4::setMax4(const hkVector4& a, const hkVector4& b)
{
	m_quad = _mm_max_ps(a.quad(), b.quad());
}

/* matrix3, rotation, quaternion, transform */

inline void hkVector4::_setMul3(const hkMatrix3& a, const hkVector4& b )
{
	const hkQuadReal b0 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(0,0,0,0));
	const hkQuadReal r0 = _mm_mul_ps( a.getColumn(0).m_quad, b0 );
	const hkQuadReal b1 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(1,1,1,1));
	const hkQuadReal r1 = _mm_mul_ps( a.getColumn(1).m_quad, b1 );
	const hkQuadReal b2 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(2,2,2,2));	
	const hkQuadReal r2 = _mm_mul_ps( a.getColumn(2).m_quad, b2 );

	m_quad = _mm_add_ps( _mm_add_ps(r0, r1), r2 );
}

inline void hkVector4::_setRotatedDir (const hkRotation& r, const hkVector4& v)
{
	_setMul3(r,v);
}

inline void hkVector4::_setRotatedInverseDir(const hkRotation& r, const hkVector4& b)
{



	// - horizontal add is 15+a few more shuffles.
	hkQuadReal c0 = r.getColumn(0).m_quad;
	hkQuadReal c1 = r.getColumn(1).m_quad;
	hkQuadReal c2 = r.getColumn(2).m_quad;
	hkQuadReal c3 = r.getColumn(2).m_quad; // c3 doesn't exist, fake it
	_MM_TRANSPOSE4_PS(c0,c1,c2,c3);

	const hkQuadReal b0 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(0,0,0,0));
	const hkQuadReal r0 = _mm_mul_ps( c0, b0 );
	const hkQuadReal b1 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(1,1,1,1));
	const hkQuadReal r1 = _mm_mul_ps( c1, b1 );
	const hkQuadReal b2 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(2,2,2,2));
	const hkQuadReal r2 = _mm_mul_ps( c2, b2 );

	m_quad = _mm_add_ps( _mm_add_ps(r0, r1), r2 );
}

inline void hkVector4::_setTransformedPos(const hkTransform& t, const hkVector4& b)
{
	const hkRotation r = t.getRotation();
	hkQuadReal b0 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(0,0,0,0));
	hkQuadReal r0 = _mm_mul_ps( r.getColumn(0).m_quad, b0 );
	hkQuadReal b1 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(1,1,1,1) );
	hkQuadReal r1 = _mm_mul_ps( r.getColumn(1).m_quad, b1 );
	hkQuadReal b2 = _mm_shuffle_ps( b.m_quad, b.m_quad, _MM_SHUFFLE(2,2,2,2));
	hkQuadReal r2 = _mm_mul_ps( r.getColumn(2).m_quad, b2 );
	m_quad = _mm_add_ps( _mm_add_ps(r0, r1), _mm_add_ps(r2, t.getTranslation().m_quad ) );
}


inline void hkVector4::_setTransformedInversePos(const hkTransform& a, const hkVector4& b)
{
	hkVector4 t0; t0.setSub4( b, a.getTranslation() );
	_setRotatedInverseDir(a.getRotation(), t0);
}



inline void hkVector4::setRotatedDir(const hkQuaternion& r, const hkVector4& v)
{
	hkVector4 temp(v);
	hkReal qreal = r.getReal();
	this->setMul4( (2.0f*qreal*qreal-1.0f), temp );

	hkVector4 imag = r.getImag();
	imag(3) = 0;
	hkReal idott = imag.dot3(temp);
	this->addMul4( 2.0f*idott, imag ); 

	hkVector4 ixt;
	ixt.setCross( imag, temp );
	this->addMul4( 2.0f*qreal, ixt );
}

inline void hkVector4::setRotatedInverseDir(const hkQuaternion& r, const hkVector4& v)
{
	hkVector4 temp(v);
	hkReal qreal = r.getReal();
	this->setMul4( (2.0f*qreal*qreal-1.0f), temp );
	hkVector4 imag = r.getImag();
	imag(3) = 0;
	hkReal idott = imag.dot3(temp);
	this->addMul4( 2.0f*idott, imag ); 
	hkVector4 ixt;
	ixt.setCross( imag, temp );
	this->addMul4( (2.0f * (-qreal)), ixt );
}

inline hkSimdReal hkVector4::dot3(const hkVector4& a) const
{
	hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	hkQuadReal xySum = _mm_add_ss( _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(1,1,1,1)), x2);
	return _mm_add_ss( _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(2,2,2,2)), xySum);
}

inline hkReal hkVector4::dot3fpu(const hkVector4& a) const
{
	const hkVector4& t = *this;
	return (t(0) * a(0)) + ( t(1) * a(1)) + ( t(2) * a(2) );
}

inline hkSimdReal hkVector4::dot4(const hkVector4& a) const
{
	hkQuadReal x2 = _mm_mul_ps(m_quad,a.m_quad);
	hkQuadReal sum0 = _mm_add_ps( _mm_shuffle_ps(x2,x2,_MM_SHUFFLE(1,0,3,2)), x2); // w+z w+z x+y x+y
	hkQuadReal sum1 = _mm_shuffle_ps(sum0,sum0, _MM_SHUFFLE(2,3,0,1)); // x+y x+y w+z w+z
	return _mm_add_ps( sum0, sum1 ); // x+y+w+z all 4
}

inline hkSimdReal hkVector4::horizontalAdd3() const
{
	hkQuadReal xySum = _mm_add_ss( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,1,1,1)), m_quad);
	return _mm_add_ss( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(2,2,2,2)), xySum);
}

inline hkSimdReal hkVector4::horizontalAdd4() const
{
	hkQuadReal sum0 = _mm_add_ps( _mm_shuffle_ps(m_quad,m_quad,_MM_SHUFFLE(1,0,3,2)), m_quad); // w+z w+z x+y x+y
	hkQuadReal sum1 = _mm_shuffle_ps(sum0,sum0, _MM_SHUFFLE(2,3,0,1)); // x+y x+y w+z w+z
	return _mm_add_ps( sum0, sum1 ); // x+y+w+z all 4
}

inline hkSimdReal hkVector4::length3() const
{
	return _mm_sqrt_ss( this->dot3(*this).quad() );
}

inline hkSimdReal hkVector4::lengthSquared3() const
{
	return this->dot3(*this);
}

inline hkSimdReal hkVector4::lengthInverse3() const
{
	const hkQuadReal half  = _mm_set_ss(0.5f);
	const hkQuadReal three = _mm_set_ss(3.0f);

	hkQuadReal len2 = this->dot3(*this).quad();
	hkQuadReal approx = _mm_rsqrt_ss(len2);
	return _mm_mul_ss(
			_mm_mul_ss(half, approx),
			_mm_sub_ss(three, _mm_mul_ss( _mm_mul_ss(len2, approx), approx ) ) );
}

inline hkSimdReal hkVector4::length4() const
{
	return _mm_sqrt_ss( this->dot4(*this).quad() );
}

inline hkSimdReal hkVector4::lengthSquared4() const
{
	return this->dot4(*this);
}

inline hkSimdReal hkVector4::lengthInverse4() const
{
	const hkQuadReal half  = _mm_set_ss(0.5f);
	const hkQuadReal three = _mm_set_ss(3.0f);

	hkQuadReal len2 = this->dot4(*this).quad();
	hkQuadReal approx = _mm_rsqrt_ss(len2);
	return _mm_mul_ss(
			_mm_mul_ss(half, approx),
			_mm_sub_ss(three, _mm_mul_ss( _mm_mul_ss(len2, approx), approx ) ) );
}

inline void hkVector4::normalize3()
{
	hkQuadReal lenInv = lengthInverse3().broadcast();
	m_quad = _mm_mul_ps(lenInv, m_quad);
}

inline hkSimdReal hkVector4::normalizeWithLength3()
{
	const hkQuadReal half  = _mm_set_ss(0.5f);
	const hkQuadReal three = _mm_set_ss(3.0f);

	hkQuadReal len2 = this->dot3(*this).quad();
	hkQuadReal approx = _mm_rsqrt_ss(len2);
	hkQuadReal lenInv = _mm_mul_ss(
			_mm_mul_ss(half, approx),
			_mm_sub_ss(three, _mm_mul_ss( _mm_mul_ss(len2, approx), approx ) ) );
	lenInv = _mm_shuffle_ps(lenInv, lenInv, _MM_SHUFFLE(0,0,0,0) );
	m_quad = _mm_mul_ps(lenInv, m_quad);
	return _mm_mul_ss(len2, lenInv); // quicker to return x^2/x
}

inline void hkVector4::normalize4()
{
	hkQuadReal lenInv = lengthInverse4().broadcast();
	m_quad = _mm_mul_ps(lenInv, m_quad);
}

inline hkSimdReal hkVector4::normalizeWithLength4()
{
	const hkQuadReal half  = _mm_set_ss(0.5f);
	const hkQuadReal three = _mm_set_ss(3.0f);

	hkQuadReal len2 = this->dot4(*this).quad();
	hkQuadReal approx = _mm_rsqrt_ss(len2);
	hkQuadReal lenInv = _mm_mul_ss(
			_mm_mul_ss(half, approx),
			_mm_sub_ss(three, _mm_mul_ss( _mm_mul_ss(len2, approx), approx ) ) );
	lenInv = _mm_shuffle_ps(lenInv, lenInv, _MM_SHUFFLE(0,0,0,0) );
	m_quad = _mm_mul_ps(lenInv, m_quad);
	return _mm_mul_ss(len2, lenInv); // quicker to return x^2/x
}


inline void hkVector4::fastNormalize3()
{
	hkQuadReal len2 = this->dot3(*this).quad();
	hkSimdReal approx = _mm_rsqrt_ss(len2);
	m_quad = _mm_mul_ps(m_quad, approx.broadcast());
}

inline hkSimdReal hkVector4::fastNormalizeWithLength3()
{
	hkQuadReal len2 = this->dot3(*this).quad();
	hkQuadReal approx = _mm_rsqrt_ss(len2);
	approx = _mm_shuffle_ps(approx, approx, _MM_SHUFFLE(0,0,0,0) );
	m_quad = _mm_mul_ps(m_quad, approx);
	return _mm_mul_ss(len2, approx);
}

/* operator () */

inline hkReal& hkVector4::operator() (int a)
{
	return reinterpret_cast<hkReal*>(&m_quad)[a];
}
inline const hkReal& hkVector4::operator() (int a) const
{
	return reinterpret_cast<const hkReal*>(&m_quad)[a];
}

inline hkSimdReal hkVector4::getSimdAt(int i) const
{
	HK_ASSERT2(i>=0 && i<4, "index out of bounds for element access");
	switch(i)
	{
		case 0:
			return _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(0,0,0,0));
		case 1:
			return _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(1,1,1,1));
		case 2:
			return _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(2,2,2,2));
		case 3:
		default:
			return _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(3,3,3,3));
	}
}

inline void hkVector4::broadcast(int i)
{
	HK_ASSERT2(i>=0 && i<4, "index out of bounds for broadcast");
	if( i == 0 )
	{
		m_quad = _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(0,0,0,0));
	}
	else if( i == 1 )
	{
		m_quad = _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(1,1,1,1));
	}
	else if( i == 2 )
	{
		m_quad = _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(2,2,2,2));
	}
	else if( i == 3 )
	{
		m_quad = _mm_shuffle_ps(m_quad, m_quad, _MM_SHUFFLE(3,3,3,3));
	}
}

/* conveneince operators */

/*
inline const hkVector4 operator+ (const hkVector4& a, const hkVector4& b)
{
	return _mm_add_ps(a.quad(), b.quad());
}

inline const hkVector4 operator- (const hkVector4& a, const hkVector4& b)
{
	return _mm_sub_ps(a.quad(), b.quad());
}

inline const hkVector4 operator* (hkSimdRealParameter s, const hkVector4& v)
{
	return _mm_mul_ps(s.broadcast(), v.quad());
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
