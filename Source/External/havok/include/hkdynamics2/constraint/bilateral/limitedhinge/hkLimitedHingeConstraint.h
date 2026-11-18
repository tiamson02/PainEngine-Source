/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_LIMITED_HINGE_CONSTRAINT_H
#define HK_LIMITED_HINGE_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>

#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstructionInfo.h>


/// Basic hinge constraint
class hkLimitedHingeConstraint : public hkRigidConstraint
{
	public:

			/// Creates a new hkLimitedHingeConstraint using the specified construction info.
		hkLimitedHingeConstraint(const hkLimitedHingeConstructionInfo& info);

			/// Sets all data except the bodies
	    void setData(const hkLimitedHingeConstructionInfo& info);

			/// Gets the construction info for the constraint
		inline void getConstructionInfo(hkLimitedHingeConstructionInfo& info);
		
			/// Check consistency of constraint members
		virtual bool isValid() const;

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

        // Methods to set and get angle limits and friction

			/// Sets the friction value.
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
		SOLVER_RESULT_MAX = 7
	};

    public:

		// Pivot in body space of each entity
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		// Vectors perp 2 free axis in A space
		hkVector4 m_perpToAxleInA1;
		hkVector4 m_perpToAxleInA2;

		// Free axis in A space
		hkVector4 m_axleInA;

		// Free axis in B space
		hkVector4 m_axleInB;
	
		// Perp to free axis in B space
		hkVector4 m_perp2FreeAxisInB;

		/// Magnitude of force  in each constrained each DOF (friction and limits share one DOF
		hkSolverResults m_results[SOLVER_RESULT_MAX];

		// Angular limits
		hkReal m_minAngle;
		hkReal m_maxAngle;

		// Max angular friction impulse
		hkReal m_maxFrictionTorque;

		// used by limits
		hkReal m_lastAngle; 

	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#include <hkdynamics2/constraint/bilateral/limitedhinge/hkLimitedHingeConstraint.inl>

#endif

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
