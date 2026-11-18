/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// INLINES FROM HAVOKVEHICLE

#include <hkvehicle2/chassis/hkChassisComponent.h>
#include <hkvehicle2/wheels/hkWheelsComponent.h>

inline const ChassisComponent* HavokVehicle::getChassisComponent() const
{
	return m_chassis;
}

inline const WheelsComponent* HavokVehicle::getWheelsComponent() const
{
	return m_wheels;
}

inline WheelsComponent* HavokVehicle::getWheelsComponent() 
{
	return m_wheels;
}

inline const DriverInputComponent* HavokVehicle::getDriverInputComponent() const
{
	return m_driverInput;
}

inline const SteeringComponent*	HavokVehicle::getSteeringComponent() const
{
	return m_steering;
}

inline const EngineComponent* HavokVehicle::getEngineComponent() const
{
	return m_engine;
}

inline const TransmissionComponent*	HavokVehicle::getTransmissionComponent() const
{
	return m_transmission;
}

inline const BrakeComponent* HavokVehicle::getBrakeComponent() const
{
	return m_brake;
}

inline const SuspensionComponent* HavokVehicle::getSuspensionComponent() const
{
	return m_suspension;
}

inline const AerodynamicsComponent*	HavokVehicle::getAerodynamicsComponent() const
{
	return m_aerodynamics;
}




/*
inline BodyInfo &HavokVehicle::getChassisBodyInfoRef()
{
	return m_chassis->getChassisBodyInfoRef();
}
*/
inline WheelInfo &HavokVehicle::getWheelBodyInfoRef(int wheel)
{
	return m_wheels->getWheelInfoRef(wheel);
}


inline const hkReal	HavokVehicle::getCurrentTime() const
{
	return m_currentTime;
}


inline const class hkRigidBody* HavokVehicle::getChassisMotionState() const
{
	return m_chassisMotionState;
}

inline class hkRigidBody* HavokVehicle::getChassisMotionState()
{
	return m_chassisMotionState;
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
