/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/world/hkWorld.h>
#include <hkutilities2/simulation/hkVariableTimestepper.h>


/// seems to be ok for timesteps between 15 fps and 750 fps
/// if you fall below the min substep count, the time per substep will change, which will
/// result in a noticable change to the system stiffness.  This is better than the alternative
/// where the system can totaly fail if you fall bellow 2 substeps.
/// If you do fall bellow 2 substeps you're game has far to much CPU time to spare, put in some
/// gratuitous piles of crates or something to slow it down.
/// This scheme will result noticable jitter.  A better scheme that keeps jitter down is to 
/// always step at n*timePerSubstep.  Accumulate requestedTimeStep - n*timePerSubstep and do an
/// extra substep when that accumulated ammount > timePerSubstep.  Even more solid would be if 
/// you did the same, but with full timesteps rather than substeps.
hkVariableTimestepper::hkVariableTimestepper( hkReal timePerSubstep, hkReal minSubstepCount ) :
	m_timePerSubstep( timePerSubstep ), m_minSubstepCount( minSubstepCount )
{
	
}


int hkVariableTimestepper::step( hkWorld* world, hkReal timestep )
{

	// the idea here is to try to keep the substeps a constant size, so we vary the number of substeps
	// Scale tau and damping to account for fractional substeps, in the end the substep size will vary
	// when stepDeltaTime is called, but with tau/damping scaling the effect should be the same as if
	// we took substeps of a size that remains constant over frames

	// take ceil to make sure we always scale tau down, which is safer
	hkReal numSubsteps = ceilf(timestep / m_timePerSubstep);

	hkReal timePerSubstep = m_timePerSubstep;
	
	if( numSubsteps < m_minSubstepCount )
	{
		numSubsteps = m_minSubstepCount;
		timePerSubstep = timestep/m_minSubstepCount;
	}

	HK_ASSERT2( (timestep / (numSubsteps*timePerSubstep)) <= 1.01f , "increasing tau is dangerous!" );

	// scale Tau and Damping to interpolate stiffness factors
	hkReal originalTau = world->m_dynamicsStepInfo.m_solverInfo.m_tau;
	hkReal scaledTau = originalTau + ( timestep / (numSubsteps*timePerSubstep) - 1.0f );

	hkReal originalDamping = world->m_dynamicsStepInfo.m_solverInfo.m_damping;
	hkReal scaledDamping = originalDamping + ( timestep / (numSubsteps*timePerSubstep) - 1.0f );

	world->m_dynamicsStepInfo.m_solverInfo.m_tau = scaledTau;
	world->m_dynamicsStepInfo.m_solverInfo.m_damping = scaledDamping;
	
	world->m_dynamicsStepInfo.m_solverInfo.m_numSteps = hkMath::hkFloatToInt( numSubsteps );
	world->m_dynamicsStepInfo.m_solverInfo.m_invNumSteps = 1.0f/world->m_dynamicsStepInfo.m_solverInfo.m_numSteps;

	// step it
	world->stepDeltaTime(timestep);

	world->m_dynamicsStepInfo.m_solverInfo.m_tau = originalTau;
	world->m_dynamicsStepInfo.m_solverInfo.m_damping = originalDamping;

	return world->m_dynamicsStepInfo.m_solverInfo.m_numSteps;
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
