/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_WHEEL_CONSTRAINT_H
#define HK_DYNAMICS2_WHEEL_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/wheel/hkWheelConstructionInfo.h>

/// A wheel constraint.  The constraint's body A is the wheel, while its body B is the chassis.
class hkWheelConstraint : public hkRigidConstraint
{
	public: 

			/// Creates a new wheel constraint using the provided construction info.
		hkWheelConstraint( const hkWheelConstructionInfo& info );

		    /// Sets the steering angle.
		void setSteeringAngle( hkReal angle );

			/// Sets construction info except for bodies
		void setData( const hkWheelConstructionInfo& info );

			/// Gets the construction info for the constraint
		void getConstructionInfo(hkWheelConstructionInfo& info);

			/// Check consistency of constraint members
		virtual bool isValid() const;

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

			/// Get type from this constraint	
		virtual int getType() const;

	
	public:

		enum 
		{
			SOLVER_RESULT_ANG_0 = 0,		// angular constraint 0
			SOLVER_RESULT_ANG_1 = 1,		// angular constraint 1
			SOLVER_RESULT_LIMIT = 2,        // linear limit 2
			SOLVER_RESULT_LIN_0 = 3,		// linear constraint 
			SOLVER_RESULT_LIN_1 = 4,		// linear constraint 
			SOLVER_RESULT_TAU_AND_DAMP = 5,	// tau and damping
			SOLVER_RESULT_MAX = 6
		};

		/// Linear limits along suspension axis
		hkReal m_suspensionMinLimit;
		hkReal m_suspensionMaxLimit;

		//// Stiffness and Damping along suspension axis
		hkReal m_suspensionStrength;
		hkReal m_suspensionDamping;
		
		//// Pivot in body space of each entity
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		//// Steering axis in B space
		hkVector4 m_steeringAxisInB;

		//// Perp to steering axis in B space
		hkVector4 m_perpToSteeringAxisInB;

		//// The axis of free rotation in A space -  the car axle
		hkVector4 m_axleInA;

		/// The initial axis of m_axleInA in B space
		hkVector4 m_referenceAxleInB;
		
		/// Suspension axis in B space
		hkVector4 m_suspensionAxisInB;

		/// Mutually perp vectors to m_suspensionAxisInB in B space
		hkVector4 m_perpToSuspensionAxisInB1;
		hkVector4 m_perpToSuspensionAxisInB2;

		hkSolverResults m_results[6];

	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#endif //HK_DYNAMICS2_WHEEL_CONSTRAINT_H
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
