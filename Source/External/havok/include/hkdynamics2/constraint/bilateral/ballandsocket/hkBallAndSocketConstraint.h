/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_BALL_AND_SOCKET_CONSTRAINT_H
#define HK_DYNAMICS2_BALL_AND_SOCKET_CONSTRAINT_H

#include <hkconstraintsolver2/solve/hkSolverResults.h>
#include <hkdynamics2/constraint/hkRigidConstraint.h>
#include <hkdynamics2/constraint/bilateral/ballandsocket/hkBallAndSocketConstructionInfo.h>

	/// The ball-and-socket or point-to-point constraint.
class hkBallAndSocketConstraint : public hkRigidConstraint
{
	public:

			/// Creates a new constraint that constrains two bodies by a specified point.
			/// The bodies and pivot point are specified in the hkBallAndSocketConstructionInfo.
        hkBallAndSocketConstraint(const hkBallAndSocketConstructionInfo& info);
 
			/// Sets all data except the bodies.
		void setData(const hkBallAndSocketConstructionInfo& info);

			/// Gets the construction info for the constraint.
		void getConstructionInfo(hkBallAndSocketConstructionInfo& info);

			/// Provides access to solver results.
		virtual hkSolverResults* getSolverResults();
		
			/// Provides number of solver results.
		virtual int getNumSolverResults() const;

			/// Check consistency of constraint members.
		virtual bool isValid() const;

			/// Get type from this constraint.
		virtual int getType() const;

	public:

		enum 
		{
			SOLVER_RESULT_LIN_0 = 0,		// linear constraint 
			SOLVER_RESULT_LIN_1 = 1,		// linear constraint 
			SOLVER_RESULT_LIN_2 = 2,		// linear constraint 
			SOLVER_RESULT_MAX = 3
		};

			/// Pivot in body A space.
		hkVector4 m_pivotInA;

			/// Pivot in body B space.
		hkVector4 m_pivotInB;

			/// Magnitude of force in each constrained each DOF.
		hkSolverResults m_results[3];


	public:
			//
			// Internal functions.
			//
		virtual void buildJacobian( const hkConstraintQueryIn &in, hkConstraintQueryOut &out );

		virtual void getConstraintInfo( hkConstraintInfo& info ) const;
};

#endif // HK_DYNAMICS2_BALL_AND_SOCKET_H

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
