/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// The default values are
/// m_mass = -1 (invalid), m_linearDamping = 0, m_angularDamping = .05, 
/// m_friction = 0.5, m_restitution = 0.4
/// m_position = (0,0,0), m_rotation = Identity
/// m_inertiaTensor = (invalid), m_centerOfMass = (0,0,0)
/// m_motionType = HK_MOTION_MAX_ID (invalid)
/// m_rigidBodyDeactivatorType = HK_SPATIAL_RIGIDBODY_DEACTIVATOR
inline hkRigidBodyConstructionInfo::hkRigidBodyConstructionInfo() 
: 	m_mass(-1),	//  Default is invalid, to ensure user sets this if required.
	m_linearDamping(0.00f),
	m_angularDamping(0.05f),
	m_friction(0.5f),
	m_restitution(0.4f),
	m_motionType(HK_MOTION_MAX_ID),
	m_rigidBodyDeactivatorType(HK_RIGIDBODY_DEACTIVATOR_SPATIAL)
{
	m_position.setZero4();
	m_rotation.setIdentity();
	m_inertiaTensor.setDiagonal(-1.0f, -1.0f, -1.0f);	// Default is invalid, to ensure user sets this if required.
	m_centerOfMass.setZero4();
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
