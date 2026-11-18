/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// construction

inline const hkVector4& hkQuaternion::getImag() const
{
	return m_vec;
}

inline void hkQuaternion::setImag(const hkVector4& i)
{

	m_vec(0) = i(0);
	m_vec(1) = i(1);
	m_vec(2) = i(2);
}


inline hkReal hkQuaternion::getReal() const
{
	return m_vec(3);
}
inline void hkQuaternion::setReal(hkReal r)
{
	m_vec(3) = r;
}

inline hkQuaternion::hkQuaternion(const hkRotation& r)
{
	set(r);
}

inline hkQuaternion::hkQuaternion(hkReal x, hkReal y, hkReal z, hkReal w)
{
	m_vec.set(x,y,z,w);
}

inline hkQuaternion::hkQuaternion(const hkVector4& axis, hkReal angle)
{
	setAxisAngle(axis,angle);
}

inline void hkQuaternion::operator= (const hkQuaternion& q)
{
	m_vec = q.m_vec;
}

inline void hkQuaternion::setIdentity()
{
	m_vec.set(0,0,0,1);
}

inline void hkQuaternion::normalize()
{
	m_vec.normalize4();	
}



inline void hkQuaternion::setMul(hkSimdRealParameter r, const hkQuaternion& q)
{
	m_vec.setMul4(r,q.m_vec);
}

inline void hkQuaternion::addMul(hkSimdRealParameter r, const hkQuaternion& q)
{
	m_vec.addMul4(r,q.m_vec);
}

inline void hkQuaternion::setMul(const hkQuaternion& q0, const hkQuaternion& q1)
{
	m_vec.setCross(q0.getImag(), q1.getImag());
	m_vec.addMul4(q0.getReal(), q1.getImag());
	m_vec.addMul4(q1.getReal(), q0.getImag());


	m_vec(3) = q0.getReal() * q1.getReal() - hkReal( q0.m_vec.dot3( q1.m_vec ) );
}

		
inline void hkQuaternion::mul(const hkQuaternion& q)
{
	hkVector4 vec;
	vec.setCross(getImag(), q.getImag());
	vec.addMul4(getReal(), q.getImag());
	vec.addMul4(q.getReal(), getImag());
//!me <todo> optimize	vec(3) = hkSimdReal(getReal()) * hkSimdReal(q.getReal()) - getImag().dot3( q.getImag() );
	vec(3) = getReal() * q.getReal() - hkReal(getImag().dot3( q.getImag() ));	
	m_vec = vec;
}

inline void hkQuaternion::setMulInverse(const hkQuaternion& q0, const hkQuaternion& q1)
{
	m_vec.setCross(q1.getImag(), q0.getImag());
	m_vec.addMul4(-q0.getReal(), q1.getImag());
	m_vec.addMul4(q1.getReal(), q0.getImag());


	m_vec(3) = q0.getImag().dot4(q1.getImag());
}




void hkQuaternion::setInverse( const hkQuaternion &q )
{
	m_vec.setNeg4( q.getImag() );
	setReal( q.getReal() );
}

const hkReal& hkQuaternion::operator()(int i) const
{
	return m_vec(i);
}

// This method extracts the angle of rotation, always returning it as a real in
// the range [0,PI].
/// NOTE - The standard mathemetical convention is followed, with the angle taken to be 
/// such that Quaternion((0,0,1), PI/2,) will rotate the X vector (1,0,0) to the Y vector (0,0,1).
/// If you are converting to/from a system where such a quaternion would rotate X to -Y, 
/// then you will need to flip the angle or axis to convert successfully.\n
/// hkQuaternion q((1,0,0), -0.7);
/// hkReal ang = q.getAngle() ;
/// // ang is now 0.7 radians (and a call to getAxis() will return (-1,0,0) )
inline hkReal hkQuaternion::getAngle() const
{
	hkReal angle = hkMath::fabs( m_vec(3) );
	angle = hkMath::acos(angle);
	angle *= 2.0f;
	return(angle);
}

	
// This method extracts the axis of rotation, returning it as a Vector4 assuming
// the angle is in the range [0,PI].
/// NOTE - The standard mathemetical convention is followed, with the angle taken to be 
/// such that Quaternion((0,0,1), PI/2) will rotate the X vector (1,0,0) to the Y vector (0,0,1).
/// If you are converting to/from a system where such a quaternion would rotate X to -Y, 
/// then you will need to flip the angle or axis to convert successfully.\n
/// NOTE - getAxis() returns a valid value ONLY if the angle of the rotation
/// is nonzero, since the axis is undefined for a rotation of zero degrees
/// and it is not possible to return it even if the Quaterinon was created
/// from an angle and axis. It was decided that returning an arbitray
/// axis would be confusing, so it is up to the user to check that the
/// stored axis is nonzero before attempting to get the axis.
/// Otherwise, a division by zero will occur.
/// eg. hkQuaternion((1,0,0), 1.5 PI) will return:
///		0.5 PI as angle
///		(-1,0,0) as axis.
inline void hkQuaternion::getAxis(hkVector4 &axis) const
{
	HK_ASSERT2( m_vec.length3() > hkMath::HK_REAL_EPSILON, "Cannot extract axis from a Quaternion representing (within numerical tolerance) the Identity rotation (or Quaternion may not be normalized).");
	axis = m_vec;
	axis.normalize3();
	if(m_vec(3) < 0.0f)
	{
		axis.setNeg4(axis);
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
