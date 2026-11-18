/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RIGID_BODY_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_RIGID_BODY_CONSTRUCTION_INFO_H

#include <hkdynamics2/entity/hkEntityConstructionInfo.h>
#include <hkdynamics2/motion/hkMotionTypes.h>
#include <hkdynamics2/entity/hkRigidBodyDeactivatorTypes.h>

/// A struct containing all the information needed to construct a rigid body. 
struct hkRigidBodyConstructionInfo : public hkEntityConstructionInfo
{
		/// Creates a new hkRigidBodyConstructionInfo and initializes its data with suitable default values. 
	inline hkRigidBodyConstructionInfo( );


		/// The initial position of the rigid body.
	hkVector4 m_position;

		/// The initial rotation of the rigid body.
	hkQuaternion m_rotation;
	
		/// The inertia tensor of the rigid body.
		/// Use the hkInertiaTensorComputer class to set the inertia to suitable values.
	hkMatrix3 m_inertiaTensor;

		/// The center of mass in the local space of the rigid body.
	hkVector4 m_centerOfMass;

		/// The mass of the rigid body.
	hkReal m_mass;

		/// The initial linear damping of the rigid body.
	hkReal m_linearDamping;

		/// The initial angular damping of the rigid body.
	hkReal m_angularDamping;

		/// The initial friction of the rigid body.
	hkReal m_friction;

		/// The initial restitution of the rigid body.
	hkReal m_restitution;

		/// The initial motion type of the rigid body.
	hkMotionType m_motionType;
	
		/// The initial deactivator type of the rigid body.
	hkRigidBodyDeactivatorType	m_rigidBodyDeactivatorType;

};

#include <hkdynamics2/entity/hkRigidBodyConstructionInfo.inl>

#endif // HK_DYNAMICS2_RIGID_BODY_CONSTRUCTION_INFO_H

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
