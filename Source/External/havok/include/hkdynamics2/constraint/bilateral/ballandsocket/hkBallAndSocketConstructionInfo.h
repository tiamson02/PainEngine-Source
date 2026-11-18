/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_BALLANDSOCKET_CONSTRUCTION_INFO_H
#define HK_DYNAMICS2_BALLANDSOCKET_CONSTRUCTION_INFO_H

class hkRigidBody;

	/// The information needed to construct a ball-and-socket constraint.
	/// All vector members are specified in body space.
class hkBallAndSocketConstructionInfo  
{
	public:
			/// Constructor sets m_bodyA and m_bodyB to HK_NULL.
		hkBallAndSocketConstructionInfo();
	
			/// Sets the construction information with world space information.
		inline void setInWorldSpace(hkRigidBody* bodyA, hkRigidBody* bodyB, 
									const hkVector4& pivot);

			/// Sets the construction information with body space information.
		inline void setInBodySpace(hkRigidBody* bodyA, hkRigidBody* bodyB,
									const hkVector4& pivotA, const hkVector4& pivotB);

			/// The first constrained body.
		hkRigidBody* m_bodyA;

			/// The second constrained body.
		hkRigidBody* m_bodyB;

			/// Pivot in body A space.
		hkVector4 m_pivotInA;

			/// Pivot in body B space.
		hkVector4 m_pivotInB;
};

#include <hkdynamics2/constraint/bilateral/ballandsocket/hkBallAndSocketConstructionInfo.inl>

#endif	// HK_DYNAMICS2_BALLANDSOCKET_CONSTRUCTION_INFO_H

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
