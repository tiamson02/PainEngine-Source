/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkVector4& hkTransform::getColumn(int x)
{
	return m_rotation.getColumn(x);
}

inline const hkVector4& hkTransform::getColumn(int x) const
{
	return m_rotation.getColumn(x);
}

inline hkVector4& hkTransform::getTranslation()
{
	return m_translation;
}

inline const hkVector4& hkTransform::getTranslation() const
{
	return m_translation;
}

inline void hkTransform::setTranslation(const hkVector4& t)
{
	m_translation = t;
}

inline hkRotation& hkTransform::getRotation()
{
	return m_rotation;
}

inline const hkRotation& hkTransform::getRotation() const
{
	return m_rotation;
}

inline void hkTransform::setRotation(const hkRotation& rotation)
{	
	m_rotation = rotation;
}

inline void hkTransform::setRotation(const hkQuaternion& quatRotation)
{
	m_rotation.set(quatRotation);
}

inline hkTransform::hkTransform(const hkTransform& t)
{
	m_rotation = t.m_rotation;
	m_translation = t.m_translation;
}

inline hkTransform::hkTransform(const hkQuaternion& q, const hkVector4& t)
	:	m_rotation(q),
		m_translation(t)
{
}


inline hkTransform::hkTransform(const hkRotation& r, const hkVector4& t)
	:	m_rotation(r),
		m_translation(t)
{
}

inline hkReal& hkTransform::operator() (int r, int c)
{
	return m_rotation(r,c);
}

inline const hkReal& hkTransform::operator() (int r, int c) const
{
	return m_rotation(r,c);
}

inline void hkTransform::set(const hkQuaternion& q, const hkVector4& t)
{
	m_rotation.set(q);
	m_translation = t;
}

const hkTransform& HK_CALL hkTransform::getIdentity()
{
	extern hkReal hkTransformIdentity[];
	return reinterpret_cast<const hkTransform&>(hkTransformIdentity);
}

inline void hkTransform::setIdentity()
{
	m_rotation.setIdentity();
	m_translation.setZero4();
}

inline bool hkTransform::isApproximatelyEqual( const hkTransform& t, hkReal zero ) const
{
	return		m_rotation.isApproximatelyEqual( t.getRotation(), zero )
		&&	m_translation.equals3( t.getTranslation(), zero );
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
