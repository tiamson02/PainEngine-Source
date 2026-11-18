/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_LIMITED_FORCE_CONSTRAINT_MOTOR_H
#define HK_DYNAMICS2_LIMITED_FORCE_CONSTRAINT_MOTOR_H

#include <hkdynamics/constraint/motor/hkConstraintMotor.h>

extern const hkClass hkLimitedForceConstraintMotorClass;

	/// takes 1DOF state as input, and calculates the data, which goes into 
	/// the constraint solver
class hkLimitedForceConstraintMotor : public hkConstraintMotor
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT );
		HK_DECLARE_REFLECTION();

			/// Constructor
		inline hkLimitedForceConstraintMotor(){}

			/// Gets the type of the motor.
		virtual Type getType() = 0;

	public:
	
		hkLimitedForceConstraintMotor(hkFinishLoadedObjectFlag f) : hkConstraintMotor(f) {}

			/// Minimum force exerted by the motor.
		hkReal m_minForce;

			/// Maximum force exerted by the motor.
		hkReal m_maxForce;
}; 

#endif // define HK_DYNAMICS2_LIMITED_FORCE_CONSTRAINT_MOTOR_H

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
