/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>

//////////////////////////////////////////////////////////////////////////////

inline hkBallAndSocketConstructionInfo::hkBallAndSocketConstructionInfo()
{
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;
}

///////////////////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivot			The constraint pivot point, specified in world space.
inline void hkBallAndSocketConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,
														hkRigidBody* bodyB,
														const hkVector4& pivot)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;	
	m_pivotInA.setTransformedInversePos( bodyA->getTransform(), pivot );
	m_pivotInB.setTransformedInversePos( bodyB->getTransform(), pivot );
}

////////////////////////////////////////////////////////////////////////////////
	
/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			The constraint pivot point, specified in bodyA's space.
/// \param pivotB			The constraint pivot point, specified in bodyB's space.
inline void hkBallAndSocketConstructionInfo::setInBodySpace(hkRigidBody* bodyA,
													   hkRigidBody* bodyB,
													   const hkVector4& pivotA,
													   const hkVector4& pivotB)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
    m_pivotInA = pivotA;
    m_pivotInB = pivotB;
}

////////////////////////////////////////////////////////////////////////////////

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
