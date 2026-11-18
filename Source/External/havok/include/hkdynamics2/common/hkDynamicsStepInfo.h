/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_STEP_INFO_H
#define HK_DYNAMICS2_STEP_INFO_H

#include <hkcollide2/dispatch/hkCollisionEnvironment.h>
#include <hkdynamics2/common/hkStepInfo.h>
#include <hkconstraintsolver2/solve/hkSolverInfo.h>
#include <hkconstraintsolver2/constraint/hkConstraintQueryIn.h>

class hkSecondaryScratchPad;

/// A structure used by the hkWorld that groups together solver and step time information. 
struct hkDynamicsStepInfo: public hkCollisionEnvironment
{
	hkStepInfo   m_stepInfo;
	hkSolverInfo m_solverInfo;
	hkConstraintQueryIn m_constraintQueryIn;
	mutable hkSecondaryScratchPad* m_scratchPad;
};



#endif //HK_DYNAMICS2_STEP_INFO_H

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
