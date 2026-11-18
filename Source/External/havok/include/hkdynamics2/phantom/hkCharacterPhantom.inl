/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


inline const hkArray<hkSimpleClosestPointsDetails*>& hkCharacterPhantom::getCollisionDetails() const
{
	return m_collisionDetails;
}

inline const hkTransform& hkCharacterPhantom::getTransform() const
{
	return m_motionState.m_sceneGraphTransform1;
}

inline void hkCharacterPhantom::setTransform( const hkTransform& transform)
{
	m_motionState.m_rootPosition0 = transform.getTranslation();
	m_motionState.m_rootPosition1 = transform.getTranslation();
	m_motionState.m_rootPosition0(3) = 1e30f;
	m_motionState.m_rootPosition1 (3) = 1e30f;
	m_motionState.m_sceneGraphTransform1 = transform;
}

inline void hkCharacterPhantom::setTranslation( const hkVector4& translation )
{
	m_motionState.m_rootPosition0 = translation;
	m_motionState.m_rootPosition1 = translation;
	m_motionState.m_rootPosition0(3) = 1e30f;
	m_motionState.m_rootPosition1 (3) = 1e30f;
	m_motionState.m_sceneGraphTransform1.setTranslation( translation );
}

inline void hkCharacterPhantom::setRotation( const hkQuaternion& rotation )
{
	m_motionState.m_sceneGraphTransform1.setRotation( rotation );
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
