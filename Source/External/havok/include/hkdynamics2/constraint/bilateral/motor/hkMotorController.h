/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_MOTOR_CONTROLLER_H
#define HK_DYNAMICS2_MOTOR_CONTROLLER_H

#include <hkbase/baseobject/hkReferencedObject.h>
#include <hkmath2/hkMath.h>

/// The inputs for the hkMotorController::motor method
class hkMotorControllerInput
{
	public:

			/// the current position of the single degree of freedom being driven by the motor
		hkReal m_x;

			/// the current relative velocity of the single degree of freedom being driven by the motor
		hkReal m_v;

			/// the effective mass that the motor will be pushing
		hkReal m_mass;

			/// the timestep
		hkReal m_dt;

			/// 1.0/dt
		hkReal m_dtInv;

			/// the impulse that the solver applied last frame to try to achieve the velocity target 
			/// requested by the motor
		hkReal m_previousImpulse;
};

/// The outputs for the hkMotorController::motor method
class hkMotorControllerOutput
{
	public:

			/// the velocity target that the motor wants the solver to try to attain during this frame
		hkReal m_velocityTargetOut;

			/// the maximum force that the solver can apply to achieve the requested velocity target
		hkReal m_forceCapOut;
};

/// takes 1DOF state as input, applys a control law and produces outputs that will move system
/// towards a desired goal state.
class hkMotorController : public hkReferencedObject
{
	public:

			/// The infomation needed for an external solver to achieve a desired output velocity
			/// target while not exceeding the specified force cap is supplied.  the duration of
			/// the pending time step and the effective impulse applied by this motor last
			/// step and effective mass at the constraint are included as inputs
		virtual void motor( const hkMotorControllerInput& input, hkMotorControllerOutput& output ) = 0;
		
}; 


#endif // HK_DYNAMICS2_MOTOR_CONTROLLER_H

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
