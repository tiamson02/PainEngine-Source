/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>

////////////////////////////////////////////////////////////////////
// Construction initialization methods
////////////////////////////////////////////////////////////////

inline hkStiffSpringConstructionInfo::hkStiffSpringConstructionInfo()
{
	m_bodyA = HK_NULL;
	m_bodyB = HK_NULL;
}

/////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			bodyA's pivot point, specified in world space.
/// \param pivotB			bodyB's pivot point, specified in world space.
inline void hkStiffSpringConstructionInfo::setInWorldSpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
													  const hkVector4& pivotA,
													  const hkVector4& pivotB) 
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	m_pivotInA.setTransformedInversePos(bodyA->getTransform(),pivotA);
	m_pivotInB.setTransformedInversePos(bodyB->getTransform(),pivotB);

	hkVector4 sepDist;

	sepDist.setSub4( pivotA, pivotB );

	m_springLength = sepDist.length3();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// \param bodyA			The first rigid body.
/// \param bodyB			The second rigid body.
/// \param pivotA			bodyA's pivot point, specified in bodyA's space.
/// \param pivotB			bodyB's pivot point, specified in bodyB's space.
inline void hkStiffSpringConstructionInfo::setInBodySpace(hkRigidBody* bodyA,hkRigidBody* bodyB,
													 const hkVector4& pivotA,const hkVector4& pivotB)
{
	m_bodyA = bodyA;
	m_bodyB = bodyB;
	m_pivotInA = pivotA;
	m_pivotInB = pivotB;	

    hkVector4 pivotAW, pivotBW; 
    pivotAW.setTransformedPos(bodyA->getTransform(),pivotA); 
    pivotBW.setTransformedPos(bodyB->getTransform(),pivotB); 
        
	hkVector4 sepDist; sepDist.setSub4( pivotAW, pivotBW ); 

	m_springLength = sepDist.length3();
}

///////////////////////////////////////////////////////////////////////

/// Set rest length of the spring
inline void hkStiffSpringConstructionInfo::setSpringLength(hkReal length)
{
	m_springLength = length;
}

///////////////////////////////////////////////////////////////////////
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
