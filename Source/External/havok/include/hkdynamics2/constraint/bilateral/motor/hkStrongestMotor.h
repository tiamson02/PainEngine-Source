/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_STRONGEST_MOTOR_H
#define HK_DYNAMICS2_STRONGEST_MOTOR_H

#include <hkdynamics2/constraint/bilateral/motor/hkMotorController.h>

/// applys the strongest force for a motor that only considers the current state in its calculations 
/// while remaining stable.  Essentially as strong as a constraint.
class hkStrongestMotor : public hkMotorController
{
	public:

		hkStrongestMotor();
			
			/// The infomation needed for an external solver to achieve a desired output velocity target while not 
			/// exceeding the specified force cap is supplied.
		virtual void motor( const hkMotorControllerInput& input, hkMotorControllerOutput& output );

			/// set the target we are driving towards
		void setTargetPosition( hkReal x );

			/// get the target we are driving towards
		hkReal getTargetPosition();

			/// max force applied to achieve the target
		void setMaxForce( hkReal f );

			/// max force applied to achieve the target
		hkReal getMaxForce();


	private:
			// position target. 
		hkReal				m_targetPosition;

			/// max force that can be applied
		hkReal				m_maxForce;

};

#include <hkdynamics2/constraint/bilateral/motor/hkStrongestMotor.inl>

#endif // HK_DYNAMICS2_STRONGEST_MOTOR_H

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
