/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics/hkDynamics.h>
#include <hkdynamics/constraint/motor/springdamper/hkSpringDamperConstraintMotor.h>
#include <hkbase/class/hkTypeInfo.h>

HK_REFLECTION_DEFINE_VIRTUAL(hkSpringDamperConstraintMotor);

hkSpringDamperConstraintMotor::hkSpringDamperConstraintMotor()
: hkLimitedForceConstraintMotor()
{
	m_springConstant = 0.0f;
	m_springDamping = 0.0f;
	m_maxForce = 1e6f;
	m_minForce = -1e6f;
}

// Construct a motor with the given properties.
hkSpringDamperConstraintMotor::hkSpringDamperConstraintMotor( hkReal springConstant, hkReal springDamping, hkReal maxForce )
: hkLimitedForceConstraintMotor()
{
	m_springConstant = springConstant;
	m_springDamping = springDamping;
	m_maxForce = maxForce;
	m_minForce = -maxForce;
}

void hkSpringDamperConstraintMotor::setMaxForce( hkReal f )
{
	m_maxForce = f;
	m_minForce = -f;
}


void hkSpringDamperConstraintMotor::calcMotorData( const hkConstraintMotorInput& input, hkConstraintMotorOutput& output )
{
	output.m_targetVelocity = 0.0f;
	output.m_targetPosition = input.m_deltaTarget + input.m_positionError;
	hkReal invMass = 1.0f / input.m_virtualMass;
	hkReal dt = input.m_stepInfo->m_substepDeltaTime;

	hkReal tau = m_springConstant * dt * dt * invMass ;
	output.m_tau = hkMath::clamp( tau, 0.0f, 1.0f );

	hkReal damp = m_springDamping * dt * invMass;
	output.m_damping = hkMath::clamp( damp, 0.0f, 1.0f );

	output.m_maxForce = m_maxForce;
	output.m_minForce = m_minForce;
}

hkConstraintMotor* hkSpringDamperConstraintMotor::clone() const
{
	hkSpringDamperConstraintMotor* sdcm = new hkSpringDamperConstraintMotor( *this );
	return sdcm;
}

hkConstraintMotor::Type hkSpringDamperConstraintMotor::getType()
{
	return hkConstraintMotor::TYPE_SPRING_DAMPER;
}

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
