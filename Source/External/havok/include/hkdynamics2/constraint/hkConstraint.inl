/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline hkConstraint::hkConstraint()
{
//	HK_ASSERT2 ( (int(this) & 0xf) == 0, "Address '" << (void*)this <<"' of hkConstraint is not 16 byte aligned" );
	m_owner = HK_NULL;
	m_bodies[0] = m_bodies[1] = HK_NULL;
	m_userData = HK_NULL;
}

inline hkEntity* hkConstraint::getEntityA() const
{
	return m_bodies[0];
}



inline hkEntity* hkConstraint::getEntityB() const
{ 
	return m_bodies[1];
}



inline hkConstraintOwner* hkConstraint::getOwner()
{
	return m_owner;
}

inline void hkConstraint::setOwner( hkConstraintOwner* owner )
{
	m_owner = owner;
}

inline void* hkConstraint::getUserData() const
{
	return m_userData;
}

inline void hkConstraint::setUserData( void* data )
{
	m_userData = data;
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
