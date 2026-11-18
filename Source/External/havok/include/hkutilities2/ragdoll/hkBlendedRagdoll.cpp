/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/ragdoll/hkBlendedRagdoll.h>

#include <hkutilities2/ragdoll/hkSkeleton.h>
#include <hkutilities2/ragdoll/hkHierarchy.h>
#include <hkdynamics2/entity/hkRigidBody.h>

hkBlendedRagdoll::hkBlendedRagdoll( hkBlendedRagdollConstructionInfo& info )
	:	hkRagdoll( info )
{
	m_gains.setSize( getRigidBodies().getSize() );
	for( int i = 0; i < m_gains.getSize(); i++ )
	{
		m_gains[i] = info.m_gains[i];
	}
}

///[hkBlendedRagdoll_text]
/// The rigid bodies within a blended ragdoll are updated based on the bones that
/// have been specified in the skeleton.  The assumtion is that the skeleton
/// passed in has the same number of bones as there are ragdoll bodies and
/// that the indices of the skeleton bones match those of the physical bodies
/// stored within this blended ragdoll.
///
/// We iterate through the bones and for each bone we: 
/// - Use hierarchy gain to calculate the desired position and rotation for bone
/// - Use velocity gain to drive the rigid body towards this position.
///

// Fix for Havok 2.1.0 and VC6
#if defined(_MSC_VER) 
	#if _MSC_VER <= 1200
		#pragma optimize("", off)
	#endif
#endif

///[CODE applyKeyframe_code]<
static void applyKeyframe(	hkRigidBody*		rb, 
							const hkVector4&	newPosition, 
							const hkQuaternion&	newRotation, 
							hkReal				invDeltaTime, 
							hkReal				velocityGain = 1.0f, 
							hkReal				linearCap = hkMath::HK_REAL_MAX,
							hkReal				angularCap = hkMath::HK_REAL_MAX )
{
	/// Calculate centre of mass in world space since velocities are 
	/// specified with reference to this.
	hkVector4 newCenterOfMassInWorld;
	newCenterOfMassInWorld.setRotatedDir( newRotation, rb->getCenterOfMassLocal() );
	newCenterOfMassInWorld.add4( newPosition );

	/// Calculate new linear velocity required to get from
	/// current position to desired keyframe position, taking into
	/// account old velocity
	hkVector4 nlv = newCenterOfMassInWorld;
	nlv.sub4( rb->getCenterOfMassInWorld() );
	nlv.mul4( invDeltaTime );
		
	/// cap new linear velocity
	{
		const hkReal magnitude = nlv.length3();
		if( magnitude > linearCap )
		{
			nlv.mul4( linearCap / magnitude );
		}
	}

	/// Calculate new angular velocity 
	hkQuaternion inverseOldRotation;
	inverseOldRotation.setInverse( rb->getRotation() );
	hkQuaternion dR; 
	dR.setMul( newRotation, inverseOldRotation );
	dR.normalize();
	hkVector4 nav = hkVector4::getZero();
	const hkReal angle = dR.getAngle();
	if( 1e-3f < angle )
	{
		dR.getAxis( nav );
		nav.setMul4( angle * invDeltaTime, nav );		
	}	
	
	/// cap new angular velocity
	{
		const hkReal magnitude = nav.length3();
		if( magnitude > angularCap )
		{
			nav.mul4( angularCap / magnitude );
		}
	}

	///[CODE applyVelocityGain_code]<
	/// Use velocity gain to interpolate between old velocity 
	/// and new velocity.  This effectively determines how much
	/// effect the keyframe driving has versus the existing 
	/// physical motion.
	hkVector4 lv; lv.setInterpolate4( rb->getLinearVelocity(), nlv, velocityGain );
	hkVector4 av; av.setInterpolate4( rb->getAngularVelocity(), nav, velocityGain );
	rb->setLinearVelocity( lv );
	rb->setAngularVelocity( av );
	///[CODE applyVelocityGain_code]>
}
///[CODE applyKeyframe_code]>

///[CODE updatePose_code]<
void hkBlendedRagdoll::updatePose( const hkSkeleton& skeleton, hkReal invDeltaTime )
{
	/// Get the local transforms for each bone in skeleton.
	const hkArray<hkBone>&	poseInLocal = skeleton.getPose();
	
	/// We also need the world transforms for each bone in skeleton.
	/// These represent the bone joint points, i.e. there may be an 
	/// offset between the rigid body space and bone local space.
	hkInplaceArray<hkBone,15> poseInWorld; 
	skeleton.getPoseInWorld( poseInWorld );

	HK_ASSERT2( poseInWorld.getSize() == getRigidBodies().getSize(), "Pose does not match number of bodies in Ragdoll" );
	
	/// Check whether offsets have been specified.
	const bool applyOffsets = ( getOffsets().getSize() != 0 );

	hkInplaceArray<hkReal,15> accumulatedVelocityGains; 
	accumulatedVelocityGains.setSize( poseInWorld.getSize() );

	for( int i = 0; i < poseInWorld.getSize(); i++ )
	{
		/// Get the current rigid body 
		hkRigidBody* rb = getRigidBodies()[i];
	
		/// Get skeleton bone position in local and in world
		const hkBone&	boneInLocal = poseInLocal[i];
		hkBone			boneInWorld = poseInWorld[i];

		/// Get the skeleton gain values
		const hkRagdollGainInfo& rg = getGains()[i];
		const hkReal velocityGain	= rg.m_velocityGain;
		const hkReal hierarchyGain	= rg.m_hierarchyGain;
		const hkReal linearCap		= rg.m_maxLinearVelocity;
		const hkReal angularCap		= rg.m_maxAngularVelocity;

		accumulatedVelocityGains[i] = velocityGain;

		//===================================================================
		// Calculate desired position/rotation for bone using hierarchy gain
		//===================================================================
		
		/// Get the skeleton parent of the current skeleton bone?
		int parentIndex = skeleton.getHierarchy()->getParents()[i];

		hkRigidBody* parentRB = HK_NULL;

		/// If there is a skeleton parent bone for the current bone
		/// then we need to apply the hierarchy gain.
		/// Note: root bone wouldn't have a parent.
		if( (-1 != parentIndex) ) 
		{
			/// modify velocity gain based on parents gain
			accumulatedVelocityGains[i] *= accumulatedVelocityGains[parentIndex];

			/// Get the rigid body that represents this parent bone
			parentRB = getRigidBodies()[parentIndex];

			/// Get the parent rigid body's position and rotation
			hkBone parentRBBoneInWorld;
			parentRBBoneInWorld.m_position = parentRB->getPosition();
			parentRBBoneInWorld.m_rotation = parentRB->getRotation();

			/// Apply offsets if needed...
			if( applyOffsets )
			{
				hkVector4		positionOffset; 
				hkQuaternion	rotationOffset; 

				/// Offset goes from skeleton -> rigid body
				/// Calculate rotation offset from parent Rigid Body to skeleton parent bone
				/// which is the inverse of the stored offset.
				rotationOffset.setInverse( getOffsets()[parentIndex].m_rotation );
				parentRBBoneInWorld.m_rotation.mul( rotationOffset );

				/// Calculate position offset from parent rigid body -> skeleton parent bone.
				positionOffset.setRotatedDir( parentRBBoneInWorld.m_rotation, getOffsets()[parentIndex].m_position );
				parentRBBoneInWorld.m_position.sub4( positionOffset );
			}

			/// Interpolate between the skeleton and physics parents based on
			/// the hierarchical gain.
			{
				/// Get skeleton parent bone in world space
				hkBone parentBoneInWorld = poseInWorld[parentIndex];

				///[CODE applyHierarchyGain_code]<
				/// Blend between using the parent transform from the skeleton hierarchy
				/// and the corresponding parent rigid body transform using the hierarchy gain.
				hkVector4 p;	
				p.setInterpolate4( parentRBBoneInWorld.m_position, parentBoneInWorld.m_position, hierarchyGain );
				
				hkQuaternion q;	
				q.setSlerp(	parentRBBoneInWorld.m_rotation,
							parentBoneInWorld.m_rotation, 
							hierarchyGain );
				///[CODE applyHierarchyGain_code]>

				/// Set bone's position and rotation applying existing local transform
				/// so that this represents desired position and rotation for the rigid body
				/// NOTE: This represents the pos/rot of the bone pivot point, the
				/// offset for the current bone hasn't been factored in yet
				boneInWorld.m_rotation.setMul( q, boneInLocal.m_rotation );
				boneInWorld.m_position.setRotatedDir( q, boneInLocal.m_position );
				boneInWorld.m_position.add4( p );
			}
		}

		//=========================================================
		// Calculate desired velocity for bone using velocity gain
		//=========================================================

		/// Factor in the rigid body/bone offset into calculated desired pos/rot
		if( applyOffsets )
		{
			hkVector4		positionOffset; 
			hkQuaternion	rotationOffset; 

			positionOffset.setRotatedDir( boneInWorld.m_rotation, getOffsets()[i].m_position );
			boneInWorld.m_position.add4( positionOffset );

			rotationOffset = getOffsets()[i].m_rotation;
			boneInWorld.m_rotation.mul( rotationOffset );
		}

		/// Calculate new velocities to reach desired transform using 
		/// velocity gain factor. The accumulated velocity gain for this 
		/// bone could be passed instead.
		applyKeyframe(	rb, 
						boneInWorld.m_position, 
						boneInWorld.m_rotation, 
						invDeltaTime, 
						velocityGain, 
						linearCap, 
						angularCap );
	}
}

#if defined(_MSC_VER) 
	#if _MSC_VER <= 1200
		#pragma optimize("", on)		
	#endif
#endif

///[CODE updatePose_code]>


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
