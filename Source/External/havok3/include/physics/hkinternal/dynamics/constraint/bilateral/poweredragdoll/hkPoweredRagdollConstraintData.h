/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_POWERED_RAGDOLL_CONSTRAINT_H
#define HK_DYNAMICS2_POWERED_RAGDOLL_CONSTRAINT_H

#include <hkinternal/dynamics/constraint/bilateral/ragdoll/hkRagdollConstraintData.h>

class hkConstraintMotor;

extern const hkClass hkPoweredRagdollConstraintDataClass;

/// Ragdoll constraint with motors to drive the 3 angles.  If you are using a motor with a
/// target position to set, set it to 0.  During the hkConstraintMotor::motor() callback the 
/// difference between the current angle the desired angle will be passed in.  This will result
/// in your motor driving the difference in the angle to zero.
/// If the motor is enabled, all friction will be disabled, however once you disable the
/// motor, friction will be enabled again
class hkPoweredRagdollConstraintData : public hkRagdollConstraintData
{
	public:

		HK_DECLARE_REFLECTION();

			/// The constructor for the powered ragdoll constraint.
		hkPoweredRagdollConstraintData();

			/// Setup a ragdoll constraint based on information in world space
			/// Use the set motors and set angle limit methods to add in the extra data.
			/// Will set the target frame B to match the current setup frame for B
		void setInWorldSpace(const hkTransform& bodyATransform, const hkTransform& bodyBTransform, 
					const hkVector4& pivot, const hkVector4& twistAxisW, const hkVector4& planeAxisW);

			/// Setup a powered ragdoll constraint based on information in body space.
			/// Use the set motors and set angle limit methods to add in the extra data.
			/// Will set the target frame B to match the current setup frame for B
		void setInBodySpace( const hkVector4& pivotA, const hkVector4& pivotB,
					const hkVector4& planeAxisA, const hkVector4& planeAxisB,
					const hkVector4& twistAxisA, const hkVector4& twistAxisB);
		
			/// Will dereference any motors held.
		~hkPoweredRagdollConstraintData();

			/// Given the relative attached ( A ) to reference ( B ) frame, create the target constraint basis to achieve this relative orientation.
		void setTargetFromRelativeFrame( const hkRotation& bRa );

			/// Returns target frame
		void getTarget(hkMatrix3& targetFrameAinB );

			/// Return the motor used to drive the twist axis angle.
		hkConstraintMotor* getTwistMotor() const;

			/// Set the motor used to drive the twist axis angle.
			/// The measured angle passed in to the motor will be a relative angle between the current
			/// deflection around the twist axis and the desired deflection.  i.e. the target angle
			/// that the motor should be trying to achieve is zero.
			/// Increments reference of new motor, decrements counter of replaced motor ( if any, can be null )
		void setTwistMotor( hkConstraintMotor* motor );

			/// Return the motor used to drive the cone axis angle.
		hkConstraintMotor* getConeMotor() const;

			/// Set the motor used to drive the cone axis angle.
			/// The measured angle passed in to the motor will be a relative angle between the current
			/// deflection around the cone axis and the desired deflection.  i.e. the target angle
			/// that the motor should be trying to achieve is zero.
			/// Increments reference of new motor, decrements counter of replaced motor ( if any, can be null ).
		void setConeMotor( hkConstraintMotor* motor );

			/// Return the motor used to drive the plane axis angle.
		hkConstraintMotor* getPlaneMotor() const;

			/// Set the motor used to drive the plane axis angle.
			/// The measured angle passed in to the motor will be a relative angle between the current
			/// deflection around the plane axis and the desired deflection.  i.e. the target angle
			/// that the motor should be trying to achieve is zero.
			/// Increments reference of new motor, decrements counter of replaced motor ( if any, can be null ).
		void setPlaneMotor( hkConstraintMotor* motor );

			/// are the motors on or off?
		hkBool getMotorsActive() const;

			/// turn the motors on or off
		void setMotorsActive( hkConstraintInstance* instance, hkBool onOff );

			/// Get type from this constraint.
		virtual int getType() const;


	public:
			// WARNING:  this function will be deprecated, use setTargetFromRelativeFrame instead.
			// A matrix holding the target constraint space of the attached body ( A ) in the
			// reference bodies ( B ) space.  The motors will work to make the actual constraint
			// space of A line up with this target. 
			// Twist should line up with col0 and plane with col1, col2 should be twist cross plane.
		void setTarget( const hkMatrix3& relativeBasisAinB );

	public:

		struct Runtime
		{
			class hkSolverResults m_solverResults[9/*VC6 doesn't like the scoping for SOLVER_RESULT_MAX*/];

				// Target angles for the individiual motors
			hkReal m_prevTargetAngles[3];

				// Tells whether the previous target angles have been initialized
			hkBool m_initialized;
		};

		static inline Runtime* HK_CALL getRuntime( hkConstraintRuntime* runtime ) { return reinterpret_cast<Runtime*>(runtime); }

			// hkConstraintData interface implementation
		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;


			/// The frame the constraint will try to match
		hkMatrix3 m_targetFrameAinB; 

			/// flag saying whether the motor is active or not
		hkBool m_motorsActive;

	protected:
		
			// reference counted so protected access
		hkConstraintMotor* m_twistMotor;
		hkConstraintMotor* m_coneMotor;
		hkConstraintMotor* m_planeMotor;
	
	public:

			// hkConstraintData interface implemenationInternal functions
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

			// hkConstraintData interface implemenationInternal functions
		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

	public:

		hkPoweredRagdollConstraintData(hkFinishLoadedObjectFlag f) : hkRagdollConstraintData(f) {}

};

#endif // HK_DYNAMICS2_POWERED_RAGDOLL_CONSTRAINT_H

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
