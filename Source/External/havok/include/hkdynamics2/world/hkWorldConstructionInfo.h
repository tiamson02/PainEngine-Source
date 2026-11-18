/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_WORLD_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_WORLD_CONSTRUCTION_INFO_H

#include <hkcollide2/algorithms/boundingvolume/hkAabb.h>

/// used to specify the speed/stiffness balance of the core solver
enum hkSolverType
{
	HK_SOLVER_TYPE_2ITERS_SOFT,
	HK_SOLVER_TYPE_2ITERS_MEDIUM,
	HK_SOLVER_TYPE_2ITERS_HARD,
	HK_SOLVER_TYPE_4ITERS_SOFT,
	HK_SOLVER_TYPE_4ITERS_MEDIUM,
	HK_SOLVER_TYPE_4ITERS_HARD,
	HK_SOLVER_TYPE_8ITERS_SOFT,
	HK_SOLVER_TYPE_8ITERS_MEDIUM,
	HK_SOLVER_TYPE_8ITERS_HARD
};

/// This structure contains all the information needed to construct a hkWorld object.
struct hkWorldConstructionInfo
{	

		/// Creates an hkWorldConstructionInfo and initializes its data to default values.
	inline hkWorldConstructionInfo();

		/// Sets the broad phase size to be a cube centered on the origin of side sideLength. See also m_broadPhaseWorldSize.
	inline void setBroadPhaseWorldSize(hkReal sideLength);

		/// Set the parameters of the world's solver. Each solver type provides a different speed/stiffness balance for the simulation.
		/// The default solver type is HK_SOLVER_TYPE_4ITERS_MEDIUM, which is fast and moderately soft.
	inline void setupSolverInfo( enum hkSolverType st);

	/*
	**	public members
	*/
	
		/// The gravity for the world - this is initialised to -9.8 in the y direction
	hkVector4 m_gravity;

		/// Defines the area dealt with by the collision detection broadphase. The size specified cuboid in world units - this cuboid is the broadphase world. 
		/// There is a significant performance cost for objects that leave this area. The default size is a cube of side 1000.
	hkAabb m_broadPhaseWorldSize;

		/// The parameters of the world's solver. Default sets of values can be automtically set using the
		/// setupSolverInfo() method by passing the required hkSolverType. See Documentation for details on customising these values.
	hkReal	m_solverTau;
	hkReal	m_solverDamp;
	int		m_solverIterations;

		/// The collision tolerance. This is used to create and keep contact points even for non penetrating objects. This dramatically increases the stability of the system.
		/// The default collision tolerance is 0.1.
	hkReal	m_collisionTolerance;

		/// This tolerance is used by the convex-convex manifold to reject points which are too close together
		/// The default value is .05, i.e. contact points less than 5 cm apart are considered to be equivalent
		/// If you are simulating an object with a side length of less than 5 cm you need to reduce this tolerance
	hkReal m_convexManifoldWeldTolerance;

		/// This tolerance is used by the convex-convex manifold to remove points when the original points of
		/// contact from both objects have drifted too far apart along the contact plane.  The default value 
		/// is 0.4f which means that points that have slid apart by 40cm will be removed from the manifold.
		/// Note that if the time step is large this tolerance may need to be increased in order to avoid throwing
		/// out contact points that are necessary to maintain a proper manifold.
	hkReal m_convexManifoldDriftTolerance;

		/// Tims are an internal collision detection optimization
	bool		m_useTims;

		/// This is an internal parameter used for backstepping. The default value is -.03. This means that if backstepping
		/// is enabled between a pair of objects, they are allowed to penetrate by 3 cm ( if units are metres), before
		/// the system backsteps them. Increasing this means less backstepping will happen, but lessens the effectiveness
		/// of backstepping for small objects. This must be a negative value.
	hkReal m_backstepAllowPenetrationDepth;

		/// This is an internal parameter used for backstepping. The default value is -.3. 
		/// It should not need to altered by the user. This value is used to
		/// increase the allowed penetration during backstepping. Each step that a collision pair needs to be backstepped,
		/// the penetration threshold is increased by this multiple of m_backstepAllowPenetrationDepth. 
		/// This allows the system to better recover from fast collisions, if backstepping is enabled.
	hkReal m_backstepIncrementalPenetration;


		/// This is an internal parameter used for backstepping. 
		/// This is the minimum timestep that the system will try to step objects. The default is .001
	hkReal m_minimumDivisibleDeltaTime;
};

#include <hkdynamics2/world/hkWorldConstructionInfo.inl>

#endif // HK_DYNAMICS2_WORLD_CONSTRUCTION_INFO_H

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
