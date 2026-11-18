/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_RAGDOLL_H
#define HK_RAGDOLL_H

#include <hkmath2/hkMath.h>
#include <hkutilities2/ragdoll/hkSkeleton.h>

class hkRigidBody;
class hkConstraint;

/// The information needed to construct an hkRagdoll.
struct hkRagdollSystemConstructionInfo
{
	hkUint32		m_numRigidBodies;
	hkUint32		m_numConstraints;
	hkRigidBody**	m_rigidbodies;
	hkBone*			m_offsets;
	hkConstraint**	m_constraints;
};

/// An hkRagdoll is a container for all aspects of a constrained system.
/// The current main use is for constrained biped characters.
/// The class acts as a controller, updating the contrained system from 
/// a skeleton structure. This control mechanism may be achieved via the 
/// rigid bodies, constraints or both. This base class implements 
/// a simple control approach whereby the rigid body transforms or 
/// velocities are set based on the supplied skeleton pose.
///
class hkRagdoll : public hkBaseObject
{
	public:

		/// Create a ragdoll
		hkRagdoll( const hkRagdollSystemConstructionInfo& info );

		virtual ~hkRagdoll( );

		/// Setup rigid body transforms to match skeleton bones.
		virtual void setPose( const hkSkeleton& skeleton );
		
		/// Setup skeleton bones to match rigid body transforms.
		virtual void getPose( hkSkeleton& skeleton ) const;

		/// Update rigid body velocities to reach skeleton bone transforms over specified time period.
		virtual void updatePose( const hkSkeleton& skeleton, hkReal invDeltaTime );

		/// Access the rigid bodies
		inline const hkArray<hkRigidBody*>&	getRigidBodies() const;

		/// Access the constraints
		inline const hkArray<hkConstraint*>& getConstraints() const;

		/// Access the offsets
		/// Note : offset array size may be zero if rigid body pivots match skeleton bones.
		inline const hkArray<hkBone>& getOffsets() const;

	private:

		hkArray<hkRigidBody*>	m_rigidbodies;

		hkArray<hkConstraint*>	m_constraints;

		hkArray<hkBone>			m_offsets;

};

#include <hkutilities2/ragdoll/hkRagdoll.inl>

#endif // HK_RAGDOLL_H
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
