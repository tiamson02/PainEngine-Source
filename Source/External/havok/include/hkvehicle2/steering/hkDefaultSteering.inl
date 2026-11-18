/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// SET Methods.
//

inline void DefaultSteering::setMaxSteeringAngle(const hkReal &angle)
{
	m_maxSteeringAngle = angle;
}

inline void DefaultSteering::setMaxSpeedFullSteeringAngle(const hkReal &speed)
{
	m_maxSpeedFullSteeringAngle = speed;
}

inline void DefaultSteering::setWheelDoesSteer(int wheel, const bool &does_steer)
{
	m_wheelsDoesSteer [wheel] = does_steer;
}

//
// GET Methods
//

inline const hkReal &DefaultSteering::getMaxSteeringAngle() const
{
	return m_maxSteeringAngle;
}

inline const hkReal &DefaultSteering::getMaxSpeedFullSteeringAngle() const
{
	return m_maxSpeedFullSteeringAngle;
}

inline const bool &DefaultSteering::getWheelDoesSteer(int wheel) const
{
	return m_wheelsDoesSteer [wheel];
}

inline hkReal DefaultSteering::calcMainSteeringAngle() const
{
	const DriverInputComponent *input = m_vehicle->getDriverInputComponent();
	
	const hkReal input_value = input->getSteeringWheelInput();
	
	const hkReal unclipped_steering = input_value * m_maxSteeringAngle;
	
	// Calculate the velocity of the car.
	const ChassisComponent *chassis_comp = m_vehicle->getChassisComponent();
	const hkTransform &car_transform = m_vehicle->getChassisMotionState()->getTransform();
	const hkVector4& forward_cs = chassis_comp->getForwardDirCS();
	
	//const hkVector4 forward_ws = car_transform.getTransformedDir(forward_cs);
	hkVector4 forward_ws;
	forward_ws.setRotatedDir(car_transform.getRotation(),forward_cs);
	
	const hkReal chassis_lin_vel = m_vehicle->getChassisMotionState()->getLinearVelocity().dot3(forward_ws);
	
	if (chassis_lin_vel < m_maxSpeedFullSteeringAngle)
	{
		return unclipped_steering;
	}
	
	// else
	// Clip steering angle.
	const hkReal s_factor = m_maxSpeedFullSteeringAngle / chassis_lin_vel;
	const hkReal clipped_steering = unclipped_steering  * s_factor * s_factor;
	
	return clipped_steering;
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
