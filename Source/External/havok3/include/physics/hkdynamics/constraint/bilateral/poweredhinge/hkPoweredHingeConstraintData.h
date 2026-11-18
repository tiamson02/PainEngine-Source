/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_POWERED_HINGE_CONSTRAINT_H
#define HK_DYNAMICS2_POWERED_HINGE_CONSTRAINT_H

#include <hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.h>
#include <hkdynamics/constraint/motor/hkConstraintMotor.h>

extern const hkClass hkPoweredHingeConstraintDataClass;

/// Same as limited hinge, but powered, as long as the motor is enabled friction is disabled
class hkPoweredHingeConstraintData : public hkLimitedHingeConstraintData
{
	public:
				
		HK_DECLARE_REFLECTION();

		hkPoweredHingeConstraintData();

		// void setInBodySpace( ... ) same as hkLimitedHingeConstraintData
		// void setInWorldSpace( ... ) same as hkLimitedHingeConstraintData

		~hkPoweredHingeConstraintData();

			/// Set whether to ignore limits
		inline void setIgnoreLimits(hkBool state);
        

			/// Get type from this constraint	
		virtual int getType() const;

			/// get the motor. default is HK_NULL
		inline hkConstraintMotor* getMotor() const;

			/// is this motor on or off?
		inline hkBool getMotorActive() const;

			/// turn the motor on or off
		void setMotorActive( hkConstraintInstance* constraint, hkBool onOff );

			/// Set the motor.  Setting this to null will disable any motor computations.
			/// The angle passed to the hkConstraintMotor::motor() callback will be the relative angle  
			/// between the attached and reference body.  You need to set the desired target angle of 
			/// your motor each step.
			/// increments reference of new motor, decrements counter of replaced motor ( if any )
		void setMotor( hkConstraintMotor* motor );

			/// Sets the target angle for the motor. Only used by motors which use positions
		inline void setMotorTargetAngle( hkReal angle );

			/// Gets the target angle for the motor
		inline hkReal getMotorTargetAngle();

		enum 
		{
			SOLVER_RESULT_MOTOR = 0,		// the motor
			SOLVER_RESULT_FRICTION = 0,		// or friction 

			SOLVER_RESULT_ANG_0 = 1,		// angular constraint 0
			SOLVER_RESULT_ANG_1 = 2,		// angular constraint 1

			SOLVER_RESULT_LIN_0 = 3,		// linear constraint defined by m_constrainedAxis1B
			SOLVER_RESULT_LIN_1 = 4,		// linear constraint defined by m_constrainedAxis2B
			SOLVER_RESULT_LIN_2 = 5,		// linear constraint defined by m_constrainedAxis2B
			SOLVER_RESULT_LIMIT = 6,		// limits
			SOLVER_RESULT_MAX = 7
		};

			/// Runtime data for this constraint
		struct Runtime
		{
				// To tell whether the previous angle has been initialized.
			hkBool m_initialized;
	
				// The previous target angle
			hkReal m_previousTargetAngle;

			class hkSolverResults m_solverResults[7/*VC6 doesn't like the scoping for SOLVER_RESULT_MAX*/];
			

				// used by limits
			hkReal m_lastAngle; 

				// record number of times 0 has been passed ( -ve if passed backwards )
				// We use a real to avoid unnecessary int to float conversion
			hkReal m_rotations; 

				/// Returns current angle position
			inline hkReal getCurrentPos() const;
		};

		static inline Runtime* HK_CALL getRuntime( hkConstraintRuntime* runtime ) { return reinterpret_cast<Runtime*>(runtime); }

			// hkConstraintData interface implementation
		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;

	public:
			/// A flag saying whether the motor is active
		hkBool m_motorActive;

			/// Ignore the limits option
		hkBool m_ignoreLimits;

			// The targetAngle for the motor
		hkReal m_targetAngle;
	protected:

			// Motor, which is reference counted so protected access.
		hkConstraintMotor* m_motor;


	public:

			/// Constrained axis are in A's space, and a perpendicular is in B's space.
			/// Error term is found by checking the perpendicular's projection on one of the axis in 
			/// a mutual space. This works because the axis are in A's frame and the perpendicular is in B's.
		void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		void getConstraintInfo( hkConstraintInfo& info ) const;

	public:

		hkPoweredHingeConstraintData(hkFinishLoadedObjectFlag f) : hkLimitedHingeConstraintData(f) {}

};

#include <hkdynamics/constraint/bilateral/poweredhinge/hkPoweredHingeConstraintData.inl>

#endif

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
