/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_SPRING_DAMPER_MOTOR_H
#define HK_DYNAMICS2_SPRING_DAMPER_MOTOR_H

#include <hkdynamics2/constraint/bilateral/motor/hkMotorController.h>

/// a damped spring that drives the input system to the target position
class hkSpringDamperMotor : public hkMotorController
{
	public:

		hkSpringDamperMotor();
			
			/// The infomation needed for an external solver to achieve a desired output velocity target
			///  while not exceeding the specified force cap is supplied.
		virtual void motor( const hkMotorControllerInput& input, hkMotorControllerOutput& output );

			/// set the rest position of the spring
		inline void setTargetPosition( hkReal x );

			/// get the target rest position of the spring
		inline hkReal getTargetPosition();

			/// set the 1.0f/ks spring constant.  ks can be quite large.
		inline void setInvSpringConstant( hkReal kInv );

			/// returns the current inverse spring constant
		inline hkReal getInvSpringConstant();

			/// max force applied to achieve the target
		inline void setMaxForce( hkReal f );

			/// max force applied to achieve the target
		inline hkReal getMaxForce();

			/// set the spring damping constant.  A good rule of thumb is to set the damping = sqrt( ks )
			/// where ks is the spring constant you would use for a spring attached to a mass of 1kg
			/// Then scale both ks and your base damping by the mass you are actually dealing with.
		inline void setDamping( hkReal d );

			/// max force applied to achieve the target
		inline hkReal getDamping();


	private:
			// spring rest position. 
		hkReal				m_targetPosition;

			// hooke's law constant for target position
		hkReal				m_invSpringConstant;

			/// max force that can be applied
		hkReal				m_maxForce;

			// spring damping coeff
		hkReal				m_damping;

};

#include <hkdynamics2/constraint/bilateral/motor/hkSpringDamperMotor.inl>

#endif // HK_DYNAMICS2_SPRING_DAMPER_MOTOR_H

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
