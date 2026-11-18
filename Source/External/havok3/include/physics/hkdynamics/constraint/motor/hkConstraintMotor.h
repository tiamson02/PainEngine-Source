/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_CONSTRAINT_MOTOR_H
#define HK_DYNAMICS2_CONSTRAINT_MOTOR_H

#include <hkmath/hkMath.h>
#include <hkconstraintsolver/solve/hkSolverResults.h>
#include <hkconstraintsolver/constraint/bilateral/hk1dBilateralConstraint.h>

extern const hkClass hkConstraintMotorClass;

/// The inputs for the hkConstraintMotor::calcMotorData method
class hkConstraintMotorInput: public hk1dBilateralConstraintStatus
{
	public:
			/// delta time information about the solver (
		const class hkConstraintQueryStepInfo* m_stepInfo;

			/// information from the last step
		class hkSolverResults m_lastResults;

			/// new target minus old target
		hkReal m_deltaTarget;

			/// old target minus current position
		hkReal m_positionError;

};

	/// The outputs for the hkConstraintMotor::calcMotorData method
class hkConstraintMotorOutput: public hk1dConstraintMotorInfo
{
};

	/// takes 1DOF state as input, and calculates the data, which goes into 
	/// the constraint solver
class hkConstraintMotor : public hkReferencedObject
{
	public:

		enum Type
		{
			TYPE_INVALID,
			TYPE_POSITION,
			TYPE_VELOCITY,
			TYPE_SPRING_DAMPER,
			TYPE_MAX
		};

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_CONSTRAINT );
		HK_DECLARE_REFLECTION();

			/// Constructor
		inline hkConstraintMotor(){}

			/// Calculate the desired position and the forces we can apply to get there according
			/// to the implemented force law.
		virtual void calcMotorData( const hkConstraintMotorInput& input, hkConstraintMotorOutput& output ) = 0;

			/// Clone the motor. As constraint data can be shared, but motors can't (normally, as they 
			/// have per instance targets in them) we use a motor clone method to create new ones if required.
		virtual hkConstraintMotor* clone() const = 0;

			/// Gets the type of the motor.
		virtual Type getType() = 0;

	public:
	
		hkConstraintMotor(hkFinishLoadedObjectFlag f) {}

}; 

#endif // HK_DYNAMICS2_MOTOR_CONTROLLER_H

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
