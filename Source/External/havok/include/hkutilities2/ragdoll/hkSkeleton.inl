/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline const hkHierarchy*	hkSkeleton::getHierarchy() const
{
	return m_hierarchy;
}

inline void hkSkeleton::setBone( int i, const hkBone& bone )
{
	HK_ASSERT( i < m_pose.getSize() );

	m_pose[i] = bone;
}

inline const hkBone& hkSkeleton::getBone( int i ) const
{
	HK_ASSERT( i < m_pose.getSize() );

	return m_pose[i];
}
	

inline void hkSkeleton::setPose( const hkArray<hkBone>& pose )
{
	HK_ASSERT( pose.getSize() == m_pose.getSize() );

	m_pose = pose;
}

inline const hkArray<hkBone>& hkSkeleton::getPose() const
{
	return m_pose;
}

inline hkArray<hkBone>& hkSkeleton::getPose()
{
	return m_pose;
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
