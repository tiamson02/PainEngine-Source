/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RAGDOLL_LIMITS_H
#define HK_DYNAMICS2_RAGDOLL_LIMITS_H

#include <hkdynamics/constraint/hkConstraintData.h>

class hkRigidBody;

extern const hkClass hkRagdollLimitsDataClass;

	/// It is a version of a hkRagdollConstraint with removed linear constraints.
	/// This constraint is meant to be used with hkConstraintChains.  
	/// (that correspond to the ball-and-socket part). Once a chain has been defined,
	/// this constraint can be simply overlayed on top of it to impose 
	/// additional angular constraints.
	/// When this constraint is used together with hkPoweredChainData, 
	/// care should be taken not to set the target in the powered chain data to lay
	/// outside the limits defined here.
class hkRagdollLimitsData : public hkConstraintData
{
	public:

		HK_DECLARE_REFLECTION();

		hkRagdollLimitsData();

			/// Setup a ragdoll constraint based on information in world space
			/// Use the set angle limit methods to add in the extra data.
		void setInWorldSpace(const hkTransform& bodyATransform, const hkTransform& bodyBTransform, 
						const hkVector4& twistAxisW, const hkVector4& planeAxisW);

			/// Setup a ragdoll constraint based on information in body space.
			/// Use the set angle limit methods to add in the extra data.
		void setInBodySpace( const hkVector4& planeAxisA, const hkVector4& planeAxisB,
						const hkVector4& twistAxisA, const hkVector4& twistAxisB);


		void buildLimits( const hkVector4& twistA, const hkVector4& twistB, const hkVector4& planeB, const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

			/// Gets the constraint frame. column 0 = twist axis, 1 = plane, 2 = twist cross plane
		void getConstraintFrameA( hkMatrix3& constraintFrameA ) const;

			/// Gets the constraint frame. column 0 = twist axis, 1 = plane, 2 = twist cross plane
		void getConstraintFrameB( hkMatrix3& constraintFrameB ) const;

			///Sets the minimum twist limit.
		inline void setTwistMinAngularLimit(hkReal rad);

			///Sets the maximum twist limit.
		inline void setTwistMaxAngularLimit(hkReal rad);

			/// Sets the maximum plane limit.
		inline void setPlaneMaxAngularLimit(hkReal rad);

			/// Sets the minimum plane limit.
		inline void setPlaneMinAngularLimit(hkReal rad);

			/// Sets the half angle used to define the cone of possible movement.
		inline void setConeAngularLimit(hkReal rad);

			/// Sets the angle used to define the cone of possible movement in terms of
			/// a min and max angle which it bisects and then calls setConeAngularLimit() 
			/// and also adjusts the twist axis in body B accordingly
		inline void setAsymmetricConeAngle(hkReal cone_min, hkReal cone_max );

			/// Sets the friction value. Set this before adding to the system.
			/// Note that this value is an absolute torque value and is therefore dependent on the masses of constrained bodies
			/// and not limited between 0.0f and 1.0f. 
			/// If trying to stiffen up ragdoll constraints, try setting this value sufficiently high so that constraints are completely stiff and then
			/// reduce until the desired behaviour has been achieved.
		void setMaxFrictionTorque(hkReal tmag);

			/// sets the m_angularLimitsTauFactor. This is a factor in the range between 0 and 1
			/// which controls the stiffness of the angular limits. If you slowly increase
			/// this value from 0 to 1 for a newly created constraint,
			/// you can nicely blend in the limits.
		inline void setAngularLimitsTauFactor( hkReal mag );
			
			/// get the m_angularLimitsTauFactor;
 		inline hkReal getAngularLimitsTauFactor(  );

			/// Gets the minimum twist limit.
		inline hkReal getTwistMinAngularLimit() const;

			/// Gets the maximum twist limit.
		inline hkReal getTwistMaxAngularLimit() const;
	   
			/// Gets the maximum plane limit.
		inline hkReal getPlaneMaxAngularLimit() const;

			/// Gets the minimum plane limit.
		inline hkReal getPlaneMinAngularLimit() const;

			/// Gets the cone angle limit.
		inline hkReal getConeAngularLimit() const;

			/// Gets the friction value.
		inline hkReal getMaxFrictionTorque() const;

			/// Check consistency of constraint
		hkBool isValid() const;
 
			/// Get type from this constraint	
		virtual int getType() const;

	protected:

		hkRagdollLimitsData( hkRigidBody* rbA, hkRigidBody* rbB );


	public:

		enum 
		{
			SOLVER_RESULT_TWIST = 0,			// angular limits defined around m_freeAxisA 
			SOLVER_RESULT_CONE  = 1,	    	// angular constraint defined by m_constrainedAxis1A
			SOLVER_RESULT_PLANE = 2,	    	// angular constraint defined by m_constrainedAxis2A

			SOLVER_RESULT_MAX = 4 // keep in sync with solver res array below
		};

		struct Runtime
		{
			class hkSolverResults m_solverResults[4/*VC6 doesn't like the scoping for SOLVER_RESULT_MAX*/];
		};

		static inline Runtime* HK_CALL getRuntime( hkConstraintRuntime* runtime ) { return reinterpret_cast<Runtime*>(runtime); }

			// hkConstraintData interface implementation
		virtual void getRuntimeInfo( hkBool wantRuntime, hkConstraintData::RuntimeInfo& infoOut ) const;

	public:

		struct ConstraintBasisA
		{
			HK_DECLARE_REFLECTION();
			
				/// Local space plane axis in body A.
			hkVector4 m_planeAxis;
				/// Local space twist axis in body A.
			hkVector4 m_twistAxis;
		};

		struct ConstraintBasisB
		{
			HK_DECLARE_REFLECTION();
			
				/// Local space plane axis in body B.
			hkVector4 m_planeAxis;
				/// Local space twist axis in body B.
			hkVector4 m_twistAxis;
		};

		struct ConstraintBasisA m_basisA;
		struct ConstraintBasisB m_basisB;

		// Limit on cone angle
		hkReal m_coneMinAngle;

		// Limits on planar cones
		hkReal m_planeMinAngle;
		hkReal m_planeMaxAngle;

			// Limits on twist around twist axis
			// stored as radians
		hkReal m_twistMinAngle;
		hkReal m_twistMaxAngle;

			// Max angular friction impulse
		hkReal m_maxFrictionTorque;

			/// a factor to tau used for angular limits
			/// defaults to 0.8
		hkReal m_angularLimitsTauFactor;

	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

	protected:

			// For Internal Experimental Usage
	        /// Gets the current twist cone angle
		inline hkReal getTwistConeAngle(const hkRigidBody* rbA,const hkRigidBody* rbB);
		
		    /// Gets the current plane cone angle 
		inline hkReal getConeAnglePlanes(const hkRigidBody* rbA,const hkRigidBody* rbB);

	public:

		hkRagdollLimitsData(hkFinishLoadedObjectFlag f);
};

//////////////////////////////////

#include <hkinternal/dynamics/constraint/chain/ragdolllimits/hkRagdollLimitsData.inl>

#endif // HK_DYNAMICS2_RAGDOLL_LIMITS_H

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
