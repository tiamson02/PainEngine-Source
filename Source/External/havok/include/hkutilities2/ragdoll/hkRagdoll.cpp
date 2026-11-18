/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <hkutilities2/ragdoll/hkRagdoll.h>
#include <hkdynamics2/constraint/hkConstraint.h>
#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkutilities2/keyframe/hkKeyFrameUtility.h>

hkRagdoll::hkRagdoll( const hkRagdollSystemConstructionInfo& info )
{
	int i;

	// Copy the array of constraints
	m_constraints.setSize( info.m_numConstraints );
	for( i = 0; i < m_constraints.getSize(); i++ )
	{
		m_constraints[i] = info.m_constraints[i];
		m_constraints[i]->addReference();
	}

	// Copy the array of rigidbodies
	m_rigidbodies.setSize( info.m_numRigidBodies );
	for( i = 0; i < m_rigidbodies.getSize(); i++ )
	{
		m_rigidbodies[i] = info.m_rigidbodies[i];
		m_rigidbodies[i]->addReference();
	}
	
	// Copy the offsets if present
	if( HK_NULL != info.m_offsets )
	{
		m_offsets.setSize( info.m_numRigidBodies );
		for( i = 0; i < m_offsets.getSize(); i++ )
		{
			m_offsets[i] = info.m_offsets[i];
		}
	}
}

hkRagdoll::~hkRagdoll( )
{
	int i;

	for( i = 0; i < m_constraints.getSize(); i++ )
	{
		m_constraints[i]->removeReference();
	}

	for( i = 0; i < m_rigidbodies.getSize(); i++ )
	{
		m_rigidbodies[i]->removeReference();
	}
}

void hkRagdoll::setPose( const hkSkeleton& skeleton )
{
	hkInplaceArray<hkBone, 15> poseInWorld;

	skeleton.getPoseInWorld( poseInWorld );

	HK_ASSERT2( poseInWorld.getSize() == m_rigidbodies.getSize(), "Pose does not match number of bodies in Ragdoll" );
	
	const bool applyOffsets = ( m_offsets.getSize() != 0 );

	for( int i = 0; i < poseInWorld.getSize(); i++ )
	{
		hkRigidBody* rb = m_rigidbodies[i];

		hkBone& boneInWorld = poseInWorld[i];

		if( applyOffsets )
		{
			hkVector4		positionOffset; 
			hkQuaternion	rotationOffset; 

			positionOffset.setRotatedDir( boneInWorld.m_rotation, m_offsets[i].m_position );
			boneInWorld.m_position.add4( positionOffset );

			rotationOffset = m_offsets[i].m_rotation;
			boneInWorld.m_rotation.mul( rotationOffset );
		}

		rb->setPosition( boneInWorld.m_position );
		rb->setRotation( boneInWorld.m_rotation );

		rb->setLinearVelocity( hkVector4::getZero() );
		rb->setAngularVelocity( hkVector4::getZero() );
	}
}
		
void hkRagdoll::getPose( hkSkeleton& skeleton ) const
{
	hkInplaceArray<hkBone, 15> poseInWorld;
	poseInWorld.setSize( skeleton.getPose().getSize() );

	HK_ASSERT2( poseInWorld.getSize() == m_rigidbodies.getSize(), "Pose does not match number of bodies in Ragdoll" );
	
	const bool applyOffsets = ( m_offsets.getSize() != 0 );

	for( int i = 0; i < poseInWorld.getSize(); i++ )
	{
		hkRigidBody* rb = m_rigidbodies[i];

		hkBone& boneInWorld = poseInWorld[i];
		boneInWorld.m_position = rb->getPosition();
		boneInWorld.m_rotation = rb->getRotation();
	
		// Inverse Offset
		if( applyOffsets )
		{
			hkVector4		positionOffset; 
			hkQuaternion	rotationOffset; 

			rotationOffset.setInverse( m_offsets[i].m_rotation );
			boneInWorld.m_rotation.mul( rotationOffset );

			positionOffset.setRotatedDir( boneInWorld.m_rotation, m_offsets[i].m_position );
			boneInWorld.m_position.sub4( positionOffset );
		}
	}

	skeleton.setPoseInWorld( poseInWorld );
}

void hkRagdoll::updatePose( const hkSkeleton& skeleton, hkReal invDeltaTime )
{
	hkInplaceArray<hkBone, 15> poseInWorld;
	skeleton.getPoseInWorld( poseInWorld );

	HK_ASSERT2( poseInWorld.getSize() == m_rigidbodies.getSize(), "Pose does not match number of bodies in Ragdoll" );
	
	const bool applyOffsets = ( m_offsets.getSize() != 0 );

	for( int i = 0; i < poseInWorld.getSize(); i++ )
	{
		hkRigidBody* rb = m_rigidbodies[i];

		hkBone boneInWorld = poseInWorld[i];

		if( applyOffsets )
		{
			hkVector4		positionOffset; 
			hkQuaternion	rotationOffset; 

			positionOffset.setRotatedDir( boneInWorld.m_rotation, m_offsets[i].m_position );
			boneInWorld.m_position.add4( positionOffset );

			rotationOffset = m_offsets[i].m_rotation;
			boneInWorld.m_rotation.mul( rotationOffset );
		}

		hkKeyFrameUtility::applyHardKeyFrame( boneInWorld.m_position, boneInWorld.m_rotation, invDeltaTime, rb );

		rb->forceActivate();
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
