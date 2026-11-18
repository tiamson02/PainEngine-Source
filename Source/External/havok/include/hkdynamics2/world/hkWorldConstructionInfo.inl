/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



inline void hkWorldConstructionInfo::setBroadPhaseWorldSize(hkReal sideLength)
{
	m_broadPhaseWorldSize.m_min.setAll( -0.5f * sideLength );
	m_broadPhaseWorldSize.m_max.setAll(  0.5f * sideLength );
}

// Old solver settings.
/*
inline void hkWorldConstructionInfo::setupSolverInfo( enum hkSolverType st)
{
	switch ( st )
	{
	case HK_SOLVER_TYPE_0S:		m_solverTau = 0.5f;
								m_solverDamp = 0.4f;
								m_solverIterations = 2;
								break;
	case HK_SOLVER_TYPE_0M:		m_solverTau = 0.75f;
								m_solverDamp = 0.6f;
								m_solverIterations = 2;
								break;
	case HK_SOLVER_TYPE_0H:		m_solverTau = 0.9f;
								m_solverDamp = 0.8f;
								m_solverIterations = 2;
								break;
	case HK_SOLVER_TYPE_1S:		m_solverTau = 0.5f;
								m_solverDamp = 0.4f;
								m_solverIterations = 4;
								break;
	case HK_SOLVER_TYPE_1M:		m_solverTau = 0.75f;
								m_solverDamp = 0.6f;
								m_solverIterations = 4;
								break;
	case HK_SOLVER_TYPE_1H:		m_solverTau = 0.9f;
								m_solverDamp = 0.8f;
								m_solverIterations = 4;
								break;
	case HK_SOLVER_TYPE_1U:		m_solverTau = 1.4f;
								m_solverDamp = 1.0f;
								m_solverIterations = 4;
								break;
	case HK_SOLVER_TYPE_2S:		m_solverTau = 0.5f;
								m_solverDamp = 0.4f;
								m_solverIterations = 8;
								break;
	case HK_SOLVER_TYPE_2M:		m_solverTau = 0.75f;
								m_solverDamp = 0.6f;
								m_solverIterations = 8;
								break;
	case HK_SOLVER_TYPE_2H:		m_solverTau = 0.9f;
								m_solverDamp = 0.8f;
								m_solverIterations = 8;
								break;
	case HK_SOLVER_TYPE_2U:		m_solverTau = 1.4f;
								m_solverDamp = 1.0f;
								m_solverIterations = 8;
								break;

	default:
		HK_ASSERT2(0, "Unknown solver type" );

	}

}
*/

// New solver settings.
inline void hkWorldConstructionInfo::setupSolverInfo( enum hkSolverType st)
{
	switch ( st )
	{
	case HK_SOLVER_TYPE_2ITERS_SOFT:
								m_solverTau = 0.3f;
								m_solverDamp = 0.9f;
								m_solverIterations = 2;
								break;
	case HK_SOLVER_TYPE_2ITERS_MEDIUM:
								m_solverTau = 0.6f;
								m_solverDamp = 1.0f;
								m_solverIterations = 2;
								break;
	case HK_SOLVER_TYPE_2ITERS_HARD:
								m_solverTau = 0.9f;
								m_solverDamp = 1.1f;
								m_solverIterations = 2;
								break;
	case HK_SOLVER_TYPE_4ITERS_SOFT:
								m_solverTau = 0.3f;
								m_solverDamp = 0.9f;
								m_solverIterations = 4;
								break;
	case HK_SOLVER_TYPE_4ITERS_MEDIUM:
								m_solverTau = 0.6f;
								m_solverDamp = 1.0f;
								m_solverIterations = 4;
								break;
	case HK_SOLVER_TYPE_4ITERS_HARD:
								m_solverTau = 0.9f;
								m_solverDamp = 1.1f;
								m_solverIterations = 4;
								break;
	case HK_SOLVER_TYPE_8ITERS_SOFT:
								m_solverTau = 0.3f;
								m_solverDamp = 0.9f;
								m_solverIterations = 8;
								break;
	case HK_SOLVER_TYPE_8ITERS_MEDIUM:
								m_solverTau = 0.6f;
								m_solverDamp = 1.0f;
								m_solverIterations = 8;
								break;
	case HK_SOLVER_TYPE_8ITERS_HARD:
								m_solverTau = 0.9f;
								m_solverDamp = 1.1f;
								m_solverIterations = 8;
								break;

	default:
		HK_ASSERT2(0, "Unknown solver type" );

	}

}

inline hkWorldConstructionInfo::hkWorldConstructionInfo()
:	m_collisionTolerance( 0.1f),
	m_convexManifoldWeldTolerance(0.05f),
	m_convexManifoldDriftTolerance(0.2f),
	m_useTims(true),
	m_backstepAllowPenetrationDepth(-.03f),
	m_backstepIncrementalPenetration( .3f),
	m_minimumDivisibleDeltaTime(.001f)
{
	m_broadPhaseWorldSize.m_min.setAll( -500 );
	m_broadPhaseWorldSize.m_max.setAll(  500 );
	m_gravity.set(0, -9.8f, 0);
	setupSolverInfo( HK_SOLVER_TYPE_4ITERS_MEDIUM );
}
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
