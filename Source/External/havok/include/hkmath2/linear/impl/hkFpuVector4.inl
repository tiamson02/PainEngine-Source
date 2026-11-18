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
	return reinterpret_cast<hkQuadReal&>(x);
}

inline const hkQuadReal& hkVector4::quad() const
{
	return reinterpret_cast<const hkQuadReal&>(x);
}


/* construct, assign, zero */

inline hkVector4::hkVector4(hkReal a, hkReal b, hkReal c, hkReal d)
	: x(a), y(b), z(c), w(d)
{
}

inline hkVector4::hkVector4(const hkQuadReal& q)
	: x(q.x), y(q.y), z(q.z), w(q.w)
{
}


inline hkVector4::hkVector4( const Havok::Vector3& v)
{
	const hkReal *r = reinterpret_cast<const hkReal*>( &v );
	x = r[0];
	y = r[1];
	z = r[2];
	w = 0.0f;
}

inline void hkVector4::operator= (const hkVector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

inline hkVector4::hkVector4( const hkVector4& v)
{
	*this = v;
}

inline void hkVector4::set(hkReal a, hkReal b, hkReal c, hkReal d)
{
	x = a;
	y = b;
	z = c;
	w = d;
}

inline void hkVector4::setAll(hkReal a)
{
	x = a;
	y = a;
	z = a;
	w = a;
}

inline void hkVector4::setAll3(hkReal a)
{
	x = a;
	y = a;
	z = a;
	w = a;
}

inline void hkVector4::setZero4()
{
	x = y = z = w = 0;
}

inline void hkVector4::zeroElement( int i )
{
	(*this)(i) = 0.0f;
}


/* vector methods */

inline void hkVector4::add4(const hkVector4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
}
inline void hkVector4::setAdd4(const hkVector4& v0, const hkVector4& v1)
{
	x = v0.x + v1.x;
	y = v0.y + v1.y;
	z = v0.z + v1.z;
	w = v0.w + v1.w;
}

inline void hkVector4::sub4(const hkVector4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}
inline void hkVector4::setSub4(const hkVector4& v0, const hkVector4& v1)
{
	x = v0.x - v1.x;
	y = v0.y - v1.y;
	z = v0.z - v1.z;
	w = v0.w - v1.w;
}

inline void hkVector4::mul4(const hkVector4& a)
{
	x *= a.x;
	y *= a.y;
	z *= a.z;
	w *= a.w;
}
inline void hkVector4::setMul4(const hkVector4& a, const hkVector4& b)
{
	x = a.x * b.x;
	y = a.y * b.y;
	z = a.z * b.z;
	w = a.w * b.w;
}

inline void hkVector4::mul4(hkSimdRealParameter rs)
{  
	hkReal s = hkReal(rs);
	x *= s;
	y *= s;
	z *= s;
	w *= s;
}
inline void hkVector4::setMul4(hkSimdRealParameter rs, const hkVector4& a)
{
	hkReal r = hkReal(rs);
	x = r * a.x;
	y = r * a.y;
	z = r * a.z;
	w = r * a.w;
}
inline void hkVector4::addMul4(hkSimdRealParameter rs, const hkVector4& a)
{
	hkReal r = hkReal(rs);
	x += r * a.x;
	y += r * a.y;
	z += r * a.z;
	w += r * a.w;
}

inline void hkVector4::setCross(const hkVector4& v1, const hkVector4& v2)
{
	const hkReal x = v1(1)*v2(2) - v1(2)*v2(1);
	const hkReal y = v1(2)*v2(0) - v1(0)*v2(2);
	const hkReal z = v1(0)*v2(1) - v1(1)*v2(0);
	this->set( x, y, z , 0.0f );
}

inline void hkVector4::setInterpolate4(const hkVector4& v0, const hkVector4& v1, hkSimdRealParameter rt)
{
	hkReal t = hkReal(rt);
	hkReal s = 1.0f - t;
	hkVector4& d = *this;
	d(0) = s * v0(0) + t * v1(0);
	d(1) = s * v0(1) + t * v1(1);
	d(2) = s * v0(2) + t * v1(2);
	d(3) = s * v0(3) + t * v1(3);
}

inline int hkVector4::compareLessThan4(const hkVector4& a) const
{
	return
		((x<a.x) * HK_VECTOR3_COMPARE_MASK_X) |
		((y<a.y) * HK_VECTOR3_COMPARE_MASK_Y) |
		((z<a.z) * HK_VECTOR3_COMPARE_MASK_Z) |
		((w<a.w) * HK_VECTOR3_COMPARE_MASK_W);
}

inline int hkVector4::compareLessThanEqual4(const hkVector4& a) const
{
	return
		((x<=a.x) * HK_VECTOR3_COMPARE_MASK_X) |
		((y<=a.y) * HK_VECTOR3_COMPARE_MASK_Y) |
		((z<=a.z) * HK_VECTOR3_COMPARE_MASK_Z) |
		((w<=a.w) * HK_VECTOR3_COMPARE_MASK_W);
}

inline int hkVector4::compareLessThanZero4() const
{
	return
		((x<0) * HK_VECTOR3_COMPARE_MASK_X) |
		((y<0) * HK_VECTOR3_COMPARE_MASK_Y) |
		((z<0) * HK_VECTOR3_COMPARE_MASK_Z) |
		((w<0) * HK_VECTOR3_COMPARE_MASK_W);
}

inline void hkVector4::setNeg4(const hkVector4& v)
{
	x = -v.x;
	y = -v.y;
	z = -v.z;
	w = -v.w;
}

inline void hkVector4::setAbs4(const hkVector4& v)
{
	x = hkMath::fabs(v.x);
	y = hkMath::fabs(v.y);
	z = hkMath::fabs(v.z);
	w = hkMath::fabs(v.w);
}

inline void hkVector4::setMin4(const hkVector4& a, const hkVector4& b)
{
	x = a.x < b.x ? a.x : b.x;
	y = a.y < b.y ? a.y : b.y;
	z = a.z < b.z ? a.z : b.z;
	w = a.w < b.w ? a.w : b.w;
}

inline void hkVector4::setMax4(const hkVector4& a, const hkVector4& b)
{
	x = a.x > b.x ? a.x : b.x;
	y = a.y > b.y ? a.y : b.y;
	z = a.z > b.z ? a.z : b.z;
	w = a.w > b.w ? a.w : b.w;
}

inline void hkVector4::_setMul3( const hkMatrix3& r, const hkVector4& v )
{
	hkReal v0 = v(0);
	hkReal v1 = v(1);
	hkReal v2 = v(2);
	hkVector4& t = *this;
	t(0) = r(0,0)*v0 + r(0,1)*v1 + r(0,2)*v2;
	t(1) = r(1,0)*v0 + r(1,1)*v1 + r(1,2)*v2;
	t(2) = r(2,0)*v0 + r(2,1)*v1 + r(2,2)*v2;
	t(3) = 0;
}

inline void hkVector4::_setRotatedDir(const hkRotation& r, const hkVector4& v)
{
	_setMul3(r,v);	
}

inline void hkVector4::_setRotatedInverseDir(const hkRotation& r, const hkVector4& v)
{
	hkReal v0 = v(0);
	hkReal v1 = v(1);
	hkReal v2 = v(2);
	hkVector4& t = *this;
	t(0) = r(0,0)*v0 + r(1,0)*v1 + r(2,0)*v2;
	t(1) = r(0,1)*v0 + r(1,1)*v1 + r(2,1)*v2;
	t(2) = r(0,2)*v0 + r(1,2)*v1 + r(2,2)*v2;
	t(3) = 0;
}

inline void hkVector4::_setTransformedPos(const hkTransform& t, const hkVector4& v)
{
	hkReal v0 = v(0);
	hkReal v1 = v(1);
	hkReal v2 = v(2);
	hkVector4& d = *this;
	d(0) = t(0,0)*v0 + t(0,1)*v1 + t(0,2)*v2 + t(0,3);
	d(1) = t(1,0)*v0 + t(1,1)*v1 + t(1,2)*v2 + t(1,3);
	d(2) = t(2,0)*v0 + t(2,1)*v1 + t(2,2)*v2 + t(2,3);
	d(3) = 0;
}

inline void hkVector4::_setTransformedInversePos(const hkTransform& t, const hkVector4& v)
{
	hkReal v0 = v(0) - t(0,3);
	hkReal v1 = v(1) - t(1,3);
	hkReal v2 = v(2) - t(2,3);

	hkVector4& d = *this;
	d(0) = t(0,0)*v0 + t(1,0)*v1 + t(2,0)*v2;
	d(1) = t(0,1)*v0 + t(1,1)*v1 + t(2,1)*v2;
	d(2) = t(0,2)*v0 + t(1,2)*v1 + t(2,2)*v2;
}

/* matrix3, rotation, quaternion, transform */

inline void hkVector4::setRotatedDir(const hkQuaternion& q, const hkVector4& v)
{
	// (2*qr*qr-1) * v  +  (2*qi.v) * qi  +  (2*qr) * (qi x v)
	hkReal scale[3];
	scale[0] = (2.0f * q.m_vec(3) * q.m_vec(3)) - 1.0f;
	scale[1] = 2.0f * ( q.m_vec(0)*v(0) + q.m_vec(1)*v(1) + q.m_vec(2)*v(2) );
	scale[2] = 2.0f * q.m_vec(3);

	hkVector4 imagCrossV;
	imagCrossV.setCross( q.getImag(), v);

	setMul4( scale[0], v);
	addMul4( scale[1], q.getImag() );
	addMul4( scale[2], imagCrossV  );
}

inline void hkVector4::setRotatedInverseDir(const hkQuaternion& q, const hkVector4& v)
{
	// as above with q.imag = -q.imag, and much cancelling
	hkReal scale[3];
	scale[0] = (2.0f * q.m_vec(3) * q.m_vec(3)) - 1.0f;
	scale[1] = 2.0f * ( q.m_vec(0)*v(0) + q.m_vec(1)*v(1) + q.m_vec(2)*v(2) );
	scale[2] = 2.0f * -q.m_vec(3);

	hkVector4 imagCrossV;
	imagCrossV.setCross( q.getImag(), v);

	setMul4( scale[0], v);
	addMul4( scale[1], q.getImag() );
	addMul4( scale[2], imagCrossV  );
}


/* length and distance */

inline hkSimdReal hkVector4::dot3(const hkVector4& a) const
{
	return (x * a.x) + ( y * a.y ) + ( z * a.z );
}

inline hkReal hkVector4::dot3fpu(const hkVector4& a) const
{
	return (x * a.x) + ( y * a.y ) + ( z * a.z );
}

inline hkSimdReal hkVector4::dot4(const hkVector4& a) const
{
	return (x * a.x) + ( y * a.y ) + ( z * a.z ) + ( w * a.w );
}

inline hkSimdReal hkVector4::horizontalAdd3() const
{
	return x+y+z;
}

inline hkSimdReal hkVector4::horizontalAdd4() const
{
	return x+y+z+w;
}

inline hkSimdReal hkVector4::length3() const
{
	return hkMath::sqrt((x * x) + ( y * y ) + ( z * z ));
}

inline hkSimdReal hkVector4::lengthSquared3() const
{
	return this->dot3(*this);
}

inline hkSimdReal hkVector4::lengthInverse3() const
{
	hkReal l2 = (x*x) + (y*y) + (z*z);
	return l2 ? hkMath::sqrtInverse(l2) : 0;
}

inline hkSimdReal hkVector4::length4() const
{
	return hkMath::sqrt( (x * x) + (y * y) + (z * z) + (w * w) );
}

inline hkSimdReal hkVector4::lengthSquared4() const
{
	return this->dot4(*this);
}

inline hkSimdReal hkVector4::lengthInverse4() const
{
	hkReal l2 = (x*x) + (y*y) + (z*z) + (w*w);
	return l2 ? hkMath::sqrtInverse(l2) : 0;
}


inline void hkVector4::normalize3()
{
	HK_ASSERT2( length3() > hkMath::HK_REAL_EPSILON,"length of hkVector4 is too small for normalize3 (degenerate vector)");
	this->mul4( this->lengthInverse3() );
}

inline hkSimdReal hkVector4::normalizeWithLength3()
{
	hkReal len = this->length3();
	HK_ASSERT2(len > hkMath::HK_REAL_EPSILON,"length of hkVector4 is too small for normalizeWithLength3 (degenerate vector)");

	this->mul4( (1.0f)/len );
	return len;
}

inline void hkVector4::normalize4()
{
	HK_ASSERT2( length4() > hkMath::HK_REAL_EPSILON,"length of hkVector4 is too small for normalize4 (degenerate vector)");

	this->mul4( this->lengthInverse4() );
}

inline hkSimdReal hkVector4::normalizeWithLength4()
{
	
	hkReal len = this->length4();
	HK_ASSERT2( len > hkMath::HK_REAL_EPSILON,"length of hkVector4 is too small for normalizeWithLength4 (degenerate vector)");
	this->mul4( (1.0f)/len );
	return len;
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
	return (&x)[i];
}

inline const hkReal& hkVector4::operator() (int i) const
{
	HK_ASSERT(i>=0 && i<4);
	return (&x)[i];
}

inline hkSimdReal hkVector4::getSimdAt(int i) const
{
	HK_ASSERT(i>=0 && i<4);
	return (&x)[i];
}

inline void hkVector4::broadcast(int i)
{
	HK_ASSERT2(i>=0 && i<4, "index error in broadcast");
	hkReal r = (&x)[i];
	x = r;
	y = r;
	z = r;
	w = r;
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

inline const hkVector4 operator* (hkSimdReal s, const hkVector4& v)
{
	return hkVector4(s*v(0), s*v(1), s*v(2));
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
