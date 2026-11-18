/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CONSTRAINTSOLVER2_SOLVER_RESULTS_H
#define HK_CONSTRAINTSOLVER2_SOLVER_RESULTS_H

#include <hkmath2/hkMath.h>

/// Constraint solver results. This class is used internally by the system, however it may be used to query the results of the constraint
/// solver for this constraint on the last solve() call.
class hkSolverResults
{
	public:
			/// Get the impulse applied by the solver on the last timestep. This structure may be returned prior to the application of the
			/// constraint solver. For the case of a contact constraint it is initialised to a very crude guess at the expected initial impulse.
		inline hkReal getImpulseApplied();

	public:
			// internal use only
		inline hkSolverResults();

			// internal use only
		inline void init();

	public:
			// internal use only
		float m_impulseApplied;

			// internal use only
		float m_internalSolverData;


};

#include <hkconstraintsolver2/solve/hkSolverResults.inl>


#endif // HK_CONSTRAINTSOLVER2_SOLVER_RESULTS_H

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
