/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/ragdoll/hkSkeleton.h>
#include <hkutilities2/ragdoll/hkHierarchy.h>


hkSkeleton::hkSkeleton( const hkHierarchy* hierarchy, const hkArray<hkBone>& pose )
: m_hierarchy(hierarchy)
{
	HK_ASSERT( hierarchy->getParents().getSize() == pose.getSize() );

	m_hierarchy->addReference();
	m_pose = pose;
}

hkSkeleton::~hkSkeleton()
{
	m_hierarchy->removeReference();
	m_hierarchy = HK_NULL;
}



void hkSkeleton::setPoseInWorld( const hkArray<hkBone>& poseInWorld )
{
	HK_ASSERT( poseInWorld.getSize() == m_pose.getSize() );
	HK_ASSERT( m_hierarchy->getParents().getSize() == m_pose.getSize() );

	for( int i = 0; i < m_pose.getSize(); i++ )
	{
		const hkBone&	boneInWorld = poseInWorld[i];
		hkBone&			boneInLocal = m_pose[i];

		const hkInt16 parentId = m_hierarchy->getParents()[ i ];
		if( -1 != parentId )
		{
			// assumes parent world transform has already been set
			HK_ASSERT( parentId < i );

			const hkBone& parentBoneInWorld = poseInWorld[parentId];
			
			boneInLocal.m_position.setSub4( boneInWorld.m_position, parentBoneInWorld.m_position );
			hkQuaternion q;	q.setInverse( parentBoneInWorld.m_rotation );
			boneInLocal.m_position.setRotatedDir( q, boneInLocal.m_position );
			boneInLocal.m_rotation.setMul( q, boneInWorld.m_rotation );
		}
		else
		{
			boneInLocal.m_position = boneInWorld.m_position;
			boneInLocal.m_rotation = boneInWorld.m_rotation;
		}
	}
}

void hkSkeleton::getPoseInWorld( hkArray<hkBone>& poseInWorld ) const
{
	HK_ASSERT( m_hierarchy->getParents().getSize() == m_pose.getSize() );

	poseInWorld.setSize( m_pose.getSize() );

	for( int i = 0; i < m_pose.getSize(); i++ )
	{
		const hkBone&	boneInLocal = m_pose[i];
		hkBone&			boneInWorld = poseInWorld[i];

		const hkInt16 parentId = m_hierarchy->getParents()[ i ];
		if( -1 != parentId )
		{
			// assumes parent world transform has already been set
			HK_ASSERT( parentId < i );

			const hkBone& parentBoneInWorld = poseInWorld[parentId];
			
			boneInWorld.m_rotation.setMul( parentBoneInWorld.m_rotation, boneInLocal.m_rotation );
			boneInWorld.m_position.setRotatedDir( parentBoneInWorld.m_rotation, boneInLocal.m_position );
			boneInWorld.m_position.add4( parentBoneInWorld.m_position );
		}
		else
		{
			boneInWorld.m_position = boneInLocal.m_position;
			boneInWorld.m_rotation = boneInLocal.m_rotation;
		}
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
