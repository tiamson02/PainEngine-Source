/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_PRISMATIC_CONSTRAINT_H
#define HK_DYNAMICS2_PRISMATIC_CONSTRAINT_H

#include <hkdynamics/constraint/hkConstraintData.h>

class hkRigidBody;
class hkConstraintMotor;
extern const hkClass hkPrismaticConstraintDataClass;

/// prismatic constraint.  free to move along an axis.
class hkPrismaticConstraintData : public hkConstraintData
{
	public:

		HK_DECLARE_REFLECTION();

		hkPrismaticConstraintData();

		~hkPrismaticConstraintData();

			/// Construct a prismatic constraint with a pivot and axis in world space.
			/// \param bodyATransform	The attached body transform.
			/// \param bodyBTransform	The reference body transform.
			/// \param pivot			The pivot point, specified in world space.
			/// \param axis				The axis, specified in world space.
		void setInWorldSpace(const hkTransform& bodyATransform, const hkTransform& bodyBTransform,
							const hkVector4& pivot,const hkVector4& axis);

			/// Construct a prismatic constraint with a pivot and axis in the two body spaces.
			/// \param pivotA			The pivot point, specified in bodyA space.
			/// \param pivotB			The pivot point, specified in bodyB space.
			/// \param axisA			The axis, specified in bodyA space.
			/// \param axisB			The axis, specified in bodyB space.
		void setInBodySpace( const hkVector4& pivotA, const hkVector4& pivotB,
							const hkVector4& axisA, const hkVector4& axisB,
							const hkVector4& axisAPerp, const hkVector4& axisBPerp);


		// Methods to set and get limits and friction

			///Sets the maximum limit for the attached body's movement along the axis.	
		inline void setMaxLinearLimit(hkReal mmax);

			///Sets the minimum limit for the attached body's movement along the axis.	
		inline void setMinLinearLimit(hkReal mmin);

			///Sets the friction value.
		inline void setMaxFrictionForce(hkReal mag);	
		
			///Gets the maximum limit for the attached body's movement along the axis.	
		inline hkReal getMaxLinearLimit() const;

			///Gets the minimum limit for the attached body's movement along the axis.	
		inline hkReal getMinLinearLimit() const;

			///Gets the friction value.
		inline hkReal getMaxFrictionForce() const;

			/// Set the motor.  Setting this to null will disable any motor computations.
			/// increments reference of new motor, decrements counter of replaced motor ( if any )
		void setMotor( hkConstraintMotor* motor );

			/// get the motor. 
		inline hkConstraintMotor* getMotor() const;

			/// Sets the target position for the motor. Only used by motors which use positions
		inline void setMotorTargetPosition( hkReal position );

			/// Gets the target angle for the motor
		inline hkReal getMotorTargetPosition();


			/// Check consistency of constraint members
		hkBool isValid() const;

			/// Get type from this constraint	
		virtual int getType() const;

		enum 
		{
			SOLVER_RESULT_MOTOR = 0,		// the motor
			SOLVER_RESULT_FRICTION = 0,		// the friction

			SOLVER_RESULT_ANG_0 = 1,		// angular constraint 0
			SOLVER_RESULT_ANG_1 = 2,		// angular constraint 1
			SOLVER_RESULT_ANG_2 = 3,		// angular constraint 2
			SOLVER_RESULT_LIN_0 = 4,		// linear constraint defined by m_constrainedAxis1B
			SOLVER_RESULT_LIN_1 = 5,		// linear constraint defined by m_constrainedAxis2B

			SOLVER_RESULT_LIMIT  = 6,		// for limits 
			SOLVER_RESULT_MAX = 7 // keep in sync with solver res array below
		};

		struct Runtime
		{
			class hkSolverResults m_solverResults[7/*VC6 doesn't like the scoping for SOLVER_RESULT_MAX*/];
			hkReal m_prevTargetPosition;
			hkBool m_initialized;
		};

		static inline Runtime* HK_CALL getRuntime( hkConstraintRuntime* runtime ) { return reinterpret_cast<Runtime*>(runtime); }


	public:
			/// hkPrismaticConstraintData::ConstraintBasisA stores information that defines the constraint.
		struct ConstraintBasisA
		{
			HK_DECLARE_REFLECTION();
			
			/// Pivot in body A space. 
			hkVector4 m_pivot;

				/// Shaft (free axis) represented in body A space.
			hkVector4 m_shaft;

				/// Offset rotation between attached (B) and reference body (A)
			hkRotation m_BtoAoffsetRotation;
		};

			/// hkPrismaticConstraintData::ConstraintBasisB stores more information that defines the constraint.
		struct ConstraintBasisB
		{
			HK_DECLARE_REFLECTION();
			
			/// Pivot in body B space.
			hkVector4 m_pivot;

				/// Shaft (free axis) represented in body B space.
			hkVector4 m_shaft;

				/// Arbitrary vector perpendicular to the shaft (in body B space).
			hkVector4 m_perpToShaft;
		};

			/// An optional motor
		hkConstraintMotor* m_motor;

			/// Constraint information (1 of 2)
		struct ConstraintBasisA m_basisA;
			/// Constraint information (2 of 2)
		struct ConstraintBasisB m_basisB;

			/// Linear limits along free axis that the bodies can move with respect to each other.
		hkReal m_minLimit;

			/// Linear limits along free axis that the bodies can move with respect to each other.
		hkReal m_maxLimit;

			/// Max linear friction impulse along free axis
		hkReal m_maxFrictionForce;

			/// The target position of the motor
		hkReal m_motorTargetPosition;
	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;

	public:

		hkPrismaticConstraintData(hkFinishLoadedObjectFlag f) : hkConstraintData(f) {}

};

#include <hkdynamics/constraint/bilateral/prismatic/hkPrismaticConstraintData.inl>

#endif // HK_DYNAMICS2_PRISMATIC_CONSTRAINT_H

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
