/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_PRISMATIC_CONSTRAINT_H
#define HK_DYNAMICS2_PRISMATIC_CONSTRAINT_H

#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/prismatic/hkPrismaticConstructionInfo.h>

class hkRigidBody;

/// prismatic constraint.  free to move along an axis.
class hkPrismaticConstraint : public hkRigidConstraint
{
	public:
				
			/// Creates a new prismatic constraint using the provided construction info.
		hkPrismaticConstraint(const hkPrismaticConstructionInfo& info);

		/// Sets all data except the bodies
		inline void setData(const hkPrismaticConstructionInfo& info);
	
			/// Gets the construction info for the constraint
		inline void getConstructionInfo(hkPrismaticConstructionInfo& info);

		/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
		/// Provides number of solver results
		virtual int getNumSolverResults() const;

        // Methods to set and get limits and friction

			///Sets the maximum limit for the attached body's movement along the axis.	
	    inline void setMaxLinearLimit(hkReal max);

			///Sets the minimum limit for the attached body's movement along the axis.	
        inline void setMinLinearLimit(hkReal min);

			///Sets the friction value.
        inline void setMaxFrictionForce(hkReal mag);	
		
			///Gets the maximum limit for the attached body's movement along the axis.	
	    inline hkReal getMaxLinearLimit() const;

			///Gets the minimum limit for the attached body's movement along the axis.	
        inline hkReal getMinLinearLimit() const;

			///Gets the friction value.
        inline hkReal getMaxFrictionForce() const;

			/// Check consistency of constraint members
		bool isValid() const;

			/// Get type from this constraint	
		virtual int getType() const;


		enum 
		{
			SOLVER_RESULT_ANG_0 = 0,		// angular constraint 0
			SOLVER_RESULT_ANG_1 = 1,		// angular constraint 1
			SOLVER_RESULT_ANG_2 = 2,		// angular constraint 2
			SOLVER_RESULT_LIN_0 = 3,		// linear constraint defined by m_constrainedAxis1B
			SOLVER_RESULT_LIN_1 = 4,		// linear constraint defined by m_constrainedAxis2B

			SOLVER_RESULT_LIMIT = 5,		// the limits
			SOLVER_RESULT_FRICTION = 6,		// the friction
			SOLVER_RESULT_MAX = 7
		};

	public:
		
		/// Pivot in body space of each entity
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		/// Shaft represented in body space of each entity
		hkVector4 m_shaftInA;
		hkVector4 m_shaftInB;

		/// Vectors mutually perp to free axis in B space
		hkVector4 m_perpToShaftInB1;
		hkVector4 m_perpToShaftInB2;

		/// Offset rotation between attached and reference body
		hkRotation m_BtoAoffsetRotation;

		/// Linear limits along free axis
		hkReal m_minLimit;
		hkReal m_maxLimit;

		/// Max linear friction impulse along free axis
        hkReal m_maxFrictionForce;
		
		///  Magnitude of impulse in each constrained each DOF (friction and limit both have one result
	    hkSolverResults m_results[SOLVER_RESULT_MAX];

	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#include <hkdynamics2/constraint/bilateral/prismatic/hkPrismaticConstraint.inl>

#endif // HK_DYNAMICS2_PRISMATIC_CONSTRAINT_H

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
