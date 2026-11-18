/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_LIMITED_HINGE_CONSTRAINT_H
#define HK_LIMITED_HINGE_CONSTRAINT_H

#include <hkdynamics/constraint/hkConstraintData.h>

extern const hkClass hkLimitedHingeConstraintDataClass;

/// Basic hinge constraint
class hkLimitedHingeConstraintData : public hkConstraintData
{
	public:

		HK_DECLARE_REFLECTION();

		hkLimitedHingeConstraintData();

			/// Set the data for a Limited Hinge constraint with information given in body space.
			/// \param pivotA The constraint pivot point, specified in bodyA space.
			/// \param pivotB The constraint pivot point, specified in bodyB space.
			/// \param axisA The hinge axis, specified in bodyA space.
			/// \param axisB The hinge axis, specified in bodyB space.
			/// \param axisAPerp Axis perpendicular to the hinge axis, specified in bodyA space.
			/// \param axisBPerp Axis perpendicular to the hinge axis, specified in bodyB space.
		void setInBodySpace(const hkVector4& pivotA, const hkVector4& pivotB, 
								const hkVector4& axisA, const hkVector4& axisB, 
								const hkVector4& axisAPerp, const hkVector4& axisBPerp);

			/// Set the data for a Limited Hinge constraint with information given in world space.
			/// \param bodyA The first rigid body transform.
			/// \param bodyB The second rigid body transform.
			/// \param pivot The pivot point, specified in world space.
			/// \param axis	The hinge axis, specified in world space.
		void setInWorldSpace(const hkTransform& bodyATransform, const hkTransform& bodyBTransform, 
								const hkVector4& pivot, const hkVector4& axis);
			
			/// Check consistency of constraint members
		virtual hkBool isValid() const;
	
		/* Methods to set and get angle limits and friction */

			/// Sets the friction value. Set this before adding to the system.
			/// Note that this value is an absolute torque value and is therefore dependent on the masses of constrained bodies
			/// and not limited between 0.0f and 1.0f. 
			/// If trying to stiffen up hinge constraints, try setting this value sufficiently high so that constraints are completely stiff and then
			/// reduce until the desired behaviour has been achieved.
		inline void setMaxFrictionTorque(hkReal tmag);

			/// Gets the friction value.
		inline hkReal getMaxFrictionTorque() const;

			/// Sets the maximum angular limit.
		inline void setMaxAngularLimit(hkReal rad);

			/// Sets the minimum angular limit.
		inline void setMinAngularLimit(hkReal rad);

			/// Gets the maximum angular limit.
		inline hkReal getMaxAngularLimit() const;

			/// Gets the minimum angular limit.
		inline hkReal getMinAngularLimit() const;

			/// sets the m_angularLimitsTauFactor. This is a factor in the range between 0 and 1
			/// which controls the stiffness of the angular limits. If you slowly increase
			/// this value from 0 to 1 for a newly created constraint,
			/// you can nicely blend in the limits.
		inline void setAngularLimitsTauFactor( hkReal mag );
			
			/// get the m_angularLimitsTauFactor;
 		inline hkReal getAngularLimitsTauFactor(  );

			/// Get type from this constraint	
		virtual int getType() const;

		enum 
		{
			SOLVER_RESULT_ANG_AXIS_1 = 0,		// angular constraint defined by m_constrainedAxis1A
			SOLVER_RESULT_ANG_AXIS_2 = 1,		// angular constraint defined by m_constrainedAxis2A
			SOLVER_RESULT_LIMITS = 2,			// angular limits defined around m_freeAxisA 
			SOLVER_RESULT_LIN_0 = 3,			// linear constraint defined by (1,0,0) in object space A
			SOLVER_RESULT_LIN_1 = 4,			// linear constraint defined by (0,1,0) in object space A
			SOLVER_RESULT_LIN_2 = 5,			// linear constraint defined by (0,0,1) in object space A

			SOLVER_RESULT_FRICTION = 6,			// the angular friction value
			SOLVER_RESULT_MAX = 7				// keep this in sync with the hkSolverResult array below
		};

		struct Runtime
		{
			class hkSolverResults m_solverResults[7/*VC6 doesn't like the scoping for SOLVER_RESULT_MAX*/];
			// used by limits
			hkReal m_lastAngle; 
		};

		static inline Runtime* HK_CALL getRuntime( hkConstraintRuntime* runtime ) { return reinterpret_cast<Runtime*>(runtime); }


	public:

		struct ConstraintBasisA
		{
			HK_DECLARE_REFLECTION();
			
			/// Pivot in body A space. 	
			hkVector4 m_pivot;

				/// Axle in body A space. 	
			hkVector4 m_axle;

				/// First of two mutually perpendicular vectors to the axis in A space. 	
			hkVector4 m_perpToAxle1;

				/// Second of two mutually perpendicular vectors to the axis in A space. 	
			hkVector4 m_perpToAxle2;
		};

		struct ConstraintBasisB
		{
			HK_DECLARE_REFLECTION();
			
			/// Pivot in body B space. 	
			hkVector4 m_pivot;

				/// Axle in body B space. 	
			hkVector4 m_axle;

				/// Vector perp to free axis in B space 	
			hkVector4 m_perp2FreeAxis;
		};

			/// Angular limits. Min value in radians. 	
		hkReal m_minAngle;

			/// Angular limits. Max value in radians. 	
		hkReal m_maxAngle;

			/// Max torque applied due to friction. 	
		hkReal m_maxFrictionTorque;

			/// A stiffness factor [0..1] for the angular limits
			/// Defaults to 1.0
		hkReal m_angularLimitsTauFactor; //+default(1.0) +absmin(0) +absmax(1)

		struct ConstraintBasisA m_basisA;
		struct ConstraintBasisB m_basisB;


	public:

		// Internal functions

			/// Constrained axis are in A's space and a perpendicular is in B's space.
			/// The error term is found by checking the perpendicular's projection on one of the axis in 
			/// a mutual space. Ahis works because the axis are in A's frame and the perpendicular is in B's.
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;

	public:

		hkLimitedHingeConstraintData(hkFinishLoadedObjectFlag f) : hkConstraintData(f) {}

};

#include <hkdynamics/constraint/bilateral/limitedhinge/hkLimitedHingeConstraintData.inl>

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
