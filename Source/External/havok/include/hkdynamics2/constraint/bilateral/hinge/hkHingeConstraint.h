/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_HINGE_CONSTRAINT_H
#define HK_DYNAMICS2_HINGE_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/hinge/hkHingeConstructionInfo.h>


/// Basic hinge constraint
class hkHingeConstraint : public hkRigidConstraint
{
	public:

			/// Creates a new hinge constraint using the specified construction info.
        hkHingeConstraint(const hkHingeConstructionInfo& info);

			/// Sets all construction data except the bodies.
		inline void setData(const hkHingeConstructionInfo& info);

			/// Gets the construction info for the constraint
		inline void getConstructionInfo(hkHingeConstructionInfo& info);

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

			/// Check consistency of constraint members
		virtual bool isValid() const;

			/// Get type from this constraint	
		virtual int getType() const;

		enum 
		{
			SOLVER_RESULT_ANG_0 = 0,		// angular constraint 0
			SOLVER_RESULT_ANG_1 = 1,		// angular constraint 1
			SOLVER_RESULT_LIN_0 = 2,		// linear constraint defined by m_constrainedAxis1B
			SOLVER_RESULT_LIN_1 = 3,		// linear constraint defined by m_constrainedAxis2B
			SOLVER_RESULT_LIN_2 = 4,		// linear constraint defined by m_constrainedAxis2B
			SOLVER_RESULT_MAX = 5
		};		

	public:	

		// The pivot in A and B space
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		// For the angular constraints
		// Mutually perp vectors to the free axis in A space

		hkVector4 m_perpToAxleInA1;
		hkVector4 m_perpToAxleInA2;

		// Free axis in B space
		hkVector4 m_axleInB;

		/// Magnitude of force in each constrained each DOF 
		hkSolverResults m_results[SOLVER_RESULT_MAX];

	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#include <hkdynamics2/constraint/bilateral/hinge/hkHingeConstraint.inl>

#endif //HK_DYNAMICS2_HINGE_CONSTRAINT_H
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
