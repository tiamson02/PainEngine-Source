/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>



/// Get rest length of the spring
inline hkReal hkStiffSpringConstraint::getSpringLength()
{
	return m_springLength;
}

///////////////////////////////////////////////////////////////////////

/// Set spring rest length
inline void hkStiffSpringConstraint::setSpringLength(hkReal length)
{
	m_springLength = length;
}

////////////////////////////////////////////////////////////////////////

/// Sets data for constraint except the rigid bodies.
/// Should be used after constructor is called with the body pair
inline void hkStiffSpringConstraint::setData(const hkStiffSpringConstructionInfo& info)
{
	m_pivotInA = info.m_pivotInA;
	m_pivotInB = info.m_pivotInB;
	m_springLength = info.m_springLength;
}

////////////////////////////////////////////////////////////////////////

inline void hkStiffSpringConstraint::getConstructionInfo(hkStiffSpringConstructionInfo& info)
{
	info.m_bodyA = static_cast<hkRigidBody*>(getEntityA());
	info.m_bodyB = static_cast<hkRigidBody*>(getEntityB());
	info.m_pivotInA = m_pivotInA;
	info.m_pivotInB = m_pivotInB;
	info.m_springLength = m_springLength;
}

////////////////////////////////////////////////////////////////////////

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
