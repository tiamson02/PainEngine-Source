/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RAGDOLL_CONSTRAINT_H
#define HK_DYNAMICS2_RAGDOLL_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/ragdoll/hkRagdollConstructionInfo.h>

/*
// sample setup code

		hkVector4 twistAxis( 0,0,1 );
		hkVector4 planeAxis( 1,0,0 );
		hkRagdollConstraint* rag = new hkRagdollConstraint( slider, fixedBody, pivot, twistAxis, planeAxis );
		rag->m_friction = 0.1f;
		rag->m_coneMin = hkMath::cos( hkMath::REAL_PI/6.0f );
		rag->m_planeMin = -hkMath::cos( hkMath::REAL_PI*0.45f );  
		rag->m_planeMax = hkMath::cos( hkMath::REAL_PI*0.45f );  
		rag->m_twistMin = -hkMath::cos( hkMath::REAL_PI/6.0f );  
		rag->m_twistMax = hkMath::cos( hkMath::REAL_PI/6.0f );  
*/


/// The ragdoll constraint. You can use this constraint to to realistically simulate the behavior of body joints, such as hips, shoulders, and knees.
class hkRagdollConstraint : public hkRigidConstraint
{
	public:
			///  Creates a new ragdoll constraint using the provided construction info.
		hkRagdollConstraint(const hkRagdollConstructionInfo& info);
		
			/// Sets all data except the bodies
	    inline void setData(const hkRagdollConstructionInfo& info);
		
			/// Gets the construction info for the constraint
		inline void getConstructionInfo(hkRagdollConstructionInfo& info);

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

		void buildConeLimit( const hkVector4& twist, const hkVector4& twistRef, hkReal min, hkReal max, const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		inline void buildLimits( const hkVector4& twist, const hkVector4& twistRef, const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

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
		inline void setConeMinAngularLimit(hkReal rad);

			/// Sets the friction value.
        inline void setMaxFrictionTorque(hkReal tmag);

			/// Gets the minimum twist limit.
        inline hkReal getTwistMinAngularLimit() const;

			/// Gets the maximum twist limit.
	    inline hkReal getTwistMaxAngularLimit() const;
	   
			/// Gets the maximum plane limit.
        inline hkReal getPlaneMaxAngularLimit() const;

			/// Gets the minimum plane limit.
        inline hkReal getMinPlaneAngularLimit() const;

			/// Gets the cone angle limit.
	    inline hkReal getMinConeAngularLimit() const;

			/// Gets the friction value.
        inline hkReal getMaxFrictionTorque() const;

			/// Check consistency of constraint
		bool isValid() const;
 
			/// Get type from this constraint	
		virtual int getType() const;

	protected:

		hkRagdollConstraint( hkRigidBody* rbA, hkRigidBody* rbB );

	public:

		enum 
		{
			SOLVER_RESULT_ANG_CONE_0 = 0,		// angular constraint defined by m_constrainedAxis1A
			SOLVER_RESULT_ANG_CONE_1 = 1,		// angular constraint defined by m_constrainedAxis2A
			SOLVER_RESULT_TWIST  = 2,			// angular limits defined around m_freeAxisA 
			SOLVER_RESULT_LIN_0 = 3,			// linear constraint defined by (1,0,0) in object space A
			SOLVER_RESULT_LIN_1 = 4,			// linear constraint defined by (0,1,0) in object space A
			SOLVER_RESULT_LIN_2 = 5,			// linear constraint defined by (0,0,1) in object space A

			SOLVER_RESULT_FRICTION_0 = 6,			// the angular friction value
			SOLVER_RESULT_FRICTION_1 = 7,			// the angular friction value
			SOLVER_RESULT_FRICTION_2 = 8,			// the angular friction value
			SOLVER_RESULT_MAX = 9
		};

	public:

		// Pivots in body space of each entity
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		// Axis/Plane normal in each body space
		hkVector4 m_planeAxisInA;
		hkVector4 m_planeAxisInB;

		// Twist axis in A space
		hkVector4 m_twistAxisInA;

		// Twist axis in B space
		hkVector4 m_twistAxisInB;

		// Limit on cone angle
		hkReal m_coneMinAngle;

		// Limits on planar cones
		hkReal m_planeMinAngle;
		hkReal m_planeMaxAngle;

		// Limits on twist around twist axis
		hkReal m_twistMinAngle;
		hkReal m_twistMaxAngle;

		// Max angular friction impulse
		hkReal m_maxFrictionTorque;

		// Magnitude of force in each constrained each DOF 
		hkSolverResults m_results[SOLVER_RESULT_MAX];

	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;

	private:

			// For Internal Experimental Usage

	        /// Gets the current twist cone angle
		inline hkReal getTwistConeAngle(const hkRigidBody* rbA,const hkRigidBody* rbB);
		
		    /// Gets the current plane cone angle 
		inline hkReal getConeAnglePlanes(const hkRigidBody* rbA,const hkRigidBody* rbB);
	
			/// Gets the twist axes in world space
		inline void getTwistAxisWS(const hkRigidBody* rbA,const hkRigidBody* rbB,hkVector4& twistAWS,hkVector4& twistBWS) const;
		
			/// Gets the plane axis in world space
		inline void getPlaneAxisRefWS(const hkRigidBody* rbB,hkVector4& planeAxisWS) const;
		
};




//////////////////////////////////

#include <hkdynamics2/constraint/bilateral/ragdoll/hkRagdollConstraint.inl>

#endif // HK_DYNAMICS2_RAGDOLL_H

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
