/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/constraint/bilateral/motor/hkSpringDamperMotor.h>


hkSpringDamperMotor::hkSpringDamperMotor() :
	m_targetPosition( 0 ), m_invSpringConstant( 0 ), m_maxForce( 0 ), m_damping( 0 )

{
}


void hkSpringDamperMotor::motor( const hkMotorControllerInput& input, hkMotorControllerOutput& output )
{

	const hkReal x0 = input.m_x - m_targetPosition;

	const hkReal invK = m_invSpringConstant;

/*/
	// explicit spring formula.  nasty, prone to explosions when stiff
	hkReal f = -x0 * 1.0f/invK - m_damping*input.m_v;

	// we do an euler step, so target velocity is what it should be after spring impulse is applied
	output.m_velocityTargetOut = input.m_v + f*input.m_dt/input.m_mass;

	output.m_forceCapOut = hkMath::fabs( f );
	if( output.m_forceCapOut > m_maxForce )
	{
		output.m_forceCapOut = m_maxForce;
	}
/*/

	// implicit integration formula for a spring solved for change in velocity
	// stable when stiff, but will introduce stiffness into the system, which could cause
	// explosions
	hkReal top = -x0 - input.m_v*invK*m_damping;
	hkReal bottom = input.m_dt + input.m_dtInv*invK*input.m_mass + m_damping*invK;
	output.m_velocityTargetOut = top/bottom;

	// f = m a
	// a = dv/dt
	// f = m dv/dt

	output.m_forceCapOut = hkMath::fabs( input.m_mass*output.m_velocityTargetOut*input.m_dtInv );
	if( output.m_forceCapOut > m_maxForce )
	{
		output.m_forceCapOut = m_maxForce;
	}

	// we do an euler step, so target velocity is what it should be after spring impulse is applied
	output.m_velocityTargetOut += input.m_v;
	
	//*/
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
