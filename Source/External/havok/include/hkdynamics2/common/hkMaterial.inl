/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkMaterial::hkMaterial() 
:	m_friction( 0.5f ),	
	m_restitution( 0.4f )	
{
}

inline hkReal hkMaterial::getFriction() const 
{ 
	return m_friction; 
}

inline hkReal hkMaterial::getRestitution() const 
{
	return m_restitution; 
}
	
inline void hkMaterial::setFriction( hkReal newFriction )
{ 
	m_friction = newFriction; 
}

inline void hkMaterial::setRestitution( hkReal newRestitution ) 
{ 
	m_restitution = newRestitution; 
}


inline hkReal HK_CALL hkMaterial::getCombinedFriction( hkReal frictionA, hkReal frictionB )
{
	return hkMath::sqrt( frictionA * frictionB );
}

inline hkReal HK_CALL hkMaterial::getCombinedRestitution( hkReal restitutionA, hkReal restitutionB )
{
	return hkMath::sqrt( restitutionA * restitutionB );
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
