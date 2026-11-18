/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics/hkDynamics.h>
#include <hkdynamics/constraint/motor/position/hkPositionConstraintMotor.h>
#include <hkbase/class/hkTypeInfo.h>

HK_REFLECTION_DEFINE_VIRTUAL(hkPositionConstraintMotor);


hkPositionConstraintMotor::hkPositionConstraintMotor( hkReal currentPosition )
: hkLimitedForceConstraintMotor()
{
	m_maxForce = 1e6f;
	m_tau = 0.8f;
	m_damping = 1.0f;
	m_constantRecoveryVelocity = 1.0f;
	m_proportionalRecoveryVelocity = 2.0f;
}

void hkPositionConstraintMotor::calcMotorData( const hkConstraintMotorInput& input, hkConstraintMotorOutput& output )
{
//	HK_ASSERT2( 0xf0e512da, input.m_motorRuntimeData, "This type of constraint does not support the hkPositionConstraintMotor" );
	{
		output.m_maxForce = m_maxForce; // * m_masterWeight;
		output.m_minForce = -output.m_maxForce;
	}
	
	//hkReal desiredPosition = input.m_targetPosition;

	//
	//hkReal delta = desiredPosition - input.m_motorRuntimeData[0];
	const hkReal delta = input.m_deltaTarget;
	const hkReal error = input.m_positionError;

	//hkReal error = input.m_motorRuntimeData[0] - input.m_currentPosition;
	//input.m_motorRuntimeData[0] = desiredPosition;

	output.m_tau = m_tau; // * m_masterWeight;
	output.m_damping = m_damping;

	hkReal recovery = error * m_proportionalRecoveryVelocity * input.m_stepInfo->m_frameDeltaTime;
	{
		hkReal restError = error - recovery;
		hkReal linspeed = m_constantRecoveryVelocity * input.m_stepInfo->m_frameDeltaTime;
		if ( hkMath::fabs( restError ) > linspeed )
		{
			recovery += (restError > 0) ? linspeed : -linspeed;
		}
		else
		{
			recovery = error;
		}
	}

	// now recovery should not overshoot rhs
	{
		hkReal ferror = hkMath::fabs( error );
		recovery = hkMath::clamp( recovery, -ferror - input.m_lastResults.m_internalSolverData,
											 ferror - input.m_lastResults.m_internalSolverData );

	}
	output.m_targetVelocity = ( delta + recovery ) * input.m_stepInfo->m_frameInvDeltaTime;
	output.m_targetPosition = input.m_lastResults.m_internalSolverData;
}

hkConstraintMotor* hkPositionConstraintMotor::clone() const
{
	hkPositionConstraintMotor* pcm = new hkPositionConstraintMotor( *this );
	return pcm;
}

hkConstraintMotor::Type hkPositionConstraintMotor::getType()
{
	return hkConstraintMotor::TYPE_POSITION;
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
