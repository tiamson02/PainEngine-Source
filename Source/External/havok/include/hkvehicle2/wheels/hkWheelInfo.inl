/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// INLINES FROM WHEELINFO

// Constructor

inline WheelInfo::WheelInfo() 
{
	m_groundBody=0;
	m_isInContact=false;
	m_contactPointWS.setZero4();
	m_contactNormalWS.setZero4();
	m_spinVelocity = 0.0f;
	m_spinAngle    = 0.0f;
	m_skidEnergyDensity=0.0f;
	m_forwardSlipVelocity = 0.0f;
}

inline const bool& WheelInfo::getIsInContact() const
{
	return m_isInContact;
}

inline const hkVector4& WheelInfo::getContactPointWS() const
{
	return m_contactPointWS;
}

inline const hkVector4& WheelInfo::getContactNormalWS()	const
{
	return m_contactNormalWS;
}

inline const hkReal& WheelInfo::getContactFriction() const
{
	return m_contactFriction;
}

inline const hkReal& WheelInfo::getSpinAngle() const
{
	return m_spinAngle;
}

inline const hkReal& WheelInfo::getSpinVelocity() const
{
	return m_spinVelocity;
}

inline const hkReal& WheelInfo::getSkidEnergyDensity() const
{
	return m_skidEnergyDensity;
}

inline const hkReal& WheelInfo::getSideForce() const
{
	return m_sideForce;
}

inline const hkReal& WheelInfo::getForwardSlipVelocity() const
{
	return m_forwardSlipVelocity;
}

// Non-const methods, only accesible by Vehicle
inline void WheelInfo::setIsInContact(const bool &is_in_contact)
{
	m_isInContact=is_in_contact;
}

inline void WheelInfo::setContactPointWS(const hkVector4 &contact_point_ws)
{
	m_contactPointWS=contact_point_ws;
}

inline void WheelInfo::setContactNormalWS(const hkVector4 &contact_normal_ws)
{
	m_contactNormalWS=contact_normal_ws;
}

inline void WheelInfo::setContactFriction(const hkReal &contact_friction)
{
	m_contactFriction = contact_friction;
}

inline void WheelInfo::setSpinAngle(const hkReal &spin_angle)
{
	m_spinAngle = spin_angle;
}

inline void WheelInfo::setSpinVelocity(const hkReal &spin_velocity)
{
	m_spinVelocity = spin_velocity;
}

inline void WheelInfo::setSkidEnergyDensity(const hkReal &skid_energy_density)
{
	m_skidEnergyDensity = skid_energy_density;
}

inline void WheelInfo::setSideForce(const hkReal &side_force)
{
	m_sideForce = side_force;
}

inline void WheelInfo::setForwardSlipVelocity(const hkReal &forward_slip)
{
	m_forwardSlipVelocity = forward_slip;
}


inline void WheelInfo::setGroundBody(hkRigidBody* groundbody)
{
	m_groundBody = groundbody;
}

inline hkRigidBody* WheelInfo::getGroundBody()
{
	return m_groundBody;
}


inline const hkReal WheelInfo::getMass() const
{
	return m_mass;
}

inline void WheelInfo::setMass(const hkReal mass)
{
	m_mass = mass;
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
