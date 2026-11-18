/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_MOTORACTION_H
#define HK_MOTORACTION_H

#include <hkdynamics2/action/hkUnaryAction.h>

class hkRigidBody;

	/// This action applies torque to the rigid body passed as a parameter until it 
	/// reaches the desired angular velocity. The motor acts on 'body' spinning about 
	/// 'axis' at 'spin_rate' [radians/sec]. 'gain' specifies the rate at  which the 
	/// desired velocity is attained [(angular - current) * gain].
class hkMotorAction: public hkUnaryAction
{
	public:
			/// Creates a new hkMotorAction.
		inline hkMotorAction(hkRigidBody* body, const hkVector4& axis, hkReal spinRate, hkReal gain = 2.0f);

			/// Apply the action to the rigid body. Called every simulation step.
		void applyAction(const hkStepInfo& stepInfo);

			/// Gets the spin rate (angular velocity) the action is trying to reach.
		inline hkReal getSpinRate();

			/// Sets the new desired spin rate the motor action will try to reach.
		inline void setSpin(hkReal new_rate);

			/// Gets the current gain.
		inline hkReal getGain();

			/// Sets the a new gain rate for the action.
		inline void setGain(hkReal new_rate);

			/// Gets the axis around which the object should spin, in local space.
		inline const hkVector4& getAxis();

			/// Sets the axis around which the object should spin, in local space.
		inline void setAxis(hkVector4& axis);

			/// Activate/deactivate
		inline void setActivation(bool b);

	private:
		
		hkVector4 m_axis;

		
		hkReal m_spinRate;
		
		
		hkReal m_gain;

		
		bool m_active;
};

#include <hkutilities2/actions/motor/hkMotorAction.inl>

#endif // HK_MOTOR_ACTION_H

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
