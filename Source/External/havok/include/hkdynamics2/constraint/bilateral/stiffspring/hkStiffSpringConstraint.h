/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_STIFF_SPRING_CONSTRAINT_H
#define HK_DYNAMICS2_STIFF_SPRING_CONSTRAINT_H

#include <hkconstraintsolver2/solve/hkSolverResults.h>
#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/stiffspring/hkStiffSpringConstructionInfo.h>


/// A stiff spring constraint. It holds the constrained bodies apart at a specified distance, 
/// as if they were attached at each end of an invisible rod.
class hkStiffSpringConstraint : public hkRigidConstraint
{
	public:
			/// Creates a new stiff spring using the specified construction info
		hkStiffSpringConstraint(const hkStiffSpringConstructionInfo& info);

			/// Sets all data except the bodies
		inline void setData(const hkStiffSpringConstructionInfo& info);

			/// Gets the construction info for the constraint
		inline void getConstructionInfo(hkStiffSpringConstructionInfo& info);

			/// Provides access to solver results.
	    virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results
		virtual int getNumSolverResults() const;

			/// Gets the length of the stiff spring.
	    inline hkReal getSpringLength();

			/// Sets the length of the stiff spring.
        inline void setSpringLength( hkReal length);

				/// Check consistency of constraint
		bool isValid() const;

			/// Get type from this constraint	
 		virtual int getType() const;
		

	public:

		enum 
		{
			SOLVER_RESULT_LIN_0 = 0,		// linear constraint 
			SOLVER_RESULT_MAX = 1
		};

		/// Pivot in body space of each entity
		hkVector4 m_pivotInA;
		hkVector4 m_pivotInB;

		/// The rest length
		hkReal m_springLength;
		
		/// Result is the magnitude of the force along the spring. 
		hkSolverResults m_results[1];


	public:

		// Internal functions

		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

///////////////////////////////////////////////////////////////

#include <hkdynamics2/constraint/bilateral/stiffspring/hkStiffSpringConstraint.inl>

#endif // HK_DYNAMICS2_STIFF_SPRING_CONSTRAINT_H

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
