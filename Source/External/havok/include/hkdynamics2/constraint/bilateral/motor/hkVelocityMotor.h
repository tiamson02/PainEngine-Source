/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_VELOCITY_MOTOR_H
#define HK_DYNAMICS2_VELOCITY_MOTOR_H

#include <hkdynamics2/constraint/bilateral/motor/hkMotorController.h>

/// simply sets a desired target velocity
class hkVelocityMotor : public hkMotorController
{
	public:

		hkVelocityMotor();

			/// The infomation needed for an external solver to achieve a desired output velocity
			///  target while not exceeding the specified force cap is supplied.
		virtual void motor( const hkMotorControllerInput& input, hkMotorControllerOutput& output );


			/// the velocity target
		void setVelocityTarget( hkReal vTarget );

			/// the velocity target
		hkReal getVelocityTarget();

			/// max force applied to achieve the velocity target
		void setMaxForce( hkReal f );

			/// max force applied to achieve the velocity target
		hkReal getMaxForce();

	private:

		hkReal m_velocityTarget;
		hkReal m_maxForce;
};

#include <hkdynamics2/constraint/bilateral/motor/hkVelocityMotor.inl>

#endif // HK_DYNAMICS2_VELOCITY_MOTOR_H

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
