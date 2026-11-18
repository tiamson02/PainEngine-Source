/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>

/// This is a very basic motor to test actions.  An appropriate torque is applied to the
/// identified rigid body at each step.
///
/// \param body The body the action is applied to.
/// \param axis The axis of rotation of the body.
/// \param spinRate The desired angular velocity we want to reach [rad/s].
/// \param gain Specified how fast do we want to reach the desired angular velocity range[>0] default == 2.0f.
inline hkMotorAction::hkMotorAction(hkRigidBody* body, const hkVector4& axis, hkReal spinRate, hkReal gain) :
	hkUnaryAction(body), m_axis(axis), m_spinRate(spinRate), m_gain(gain), m_active(true)
{
	m_axis.normalize4();
}

/// \param new_rate The new desired spin rate
inline void hkMotorAction::setSpin(hkReal new_rate)
{
	m_spinRate = new_rate;
}

/// The gain is the rate at which the action will try to reach the desired angular velocity.
///
/// \param new_gain The new gain. Range [>0].
inline void hkMotorAction::setGain(hkReal new_gain)
{
	m_gain = new_gain;
}

/// This axis is specified on construction.
inline const hkVector4& hkMotorAction::getAxis()
{
	return m_axis;
}

inline void hkMotorAction::setAxis(hkVector4& axis)
{
	m_axis = axis;
}

/// This value is set on construction and can be changed by calling setSpinRate().
inline hkReal hkMotorAction::getSpinRate()
{
	return m_spinRate;
}

/// This is the gain that the action is currently using to reach the desired spin speed.
/// The gain is set on construction and can be modified by the function setGain().
inline hkReal hkMotorAction::getGain()
{
	return m_gain;
}

inline void hkMotorAction::setActivation(bool b)
{
	m_active = b;
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
