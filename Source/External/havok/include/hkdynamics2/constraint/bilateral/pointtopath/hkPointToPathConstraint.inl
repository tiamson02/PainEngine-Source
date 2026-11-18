/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// inline functions for hkPointToPathConstructionInfo and hkPointToPathConstraint.

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkdynamics2/constraint/bilateral/pointtopath/hkParametricCurve.h>



//////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstraint::setMaxFrictionForce(hkReal fmag)
{
	m_maxFrictionForce = fmag;
}

////////////////////////////////////////////////////////////////////////

inline hkReal hkPointToPathConstraint::getMaxFrictionForce()
{
	return m_maxFrictionForce;
}

////////////////////////////////////////////////////////////////////////

inline hkParametricCurve* hkPointToPathConstraint::getPath()
{
	return m_path;
}

////////////////////////////////////////////////////////////////////////

inline const hkTransform& hkPointToPathConstraint::getConstraintToLocalTransform(int i) const
{
	HK_ASSERT((i >=0) && ( i <= 2));
	return m_transform_OS_KS[i];
}

////////////////////////////////////////////////////////////////////////

inline void hkPointToPathConstraint::getConstructionInfo(hkPointToPathConstructionInfo& info)
{
	info.m_bodyA = static_cast<hkRigidBody*>(getEntityA());
	info.m_bodyB = static_cast<hkRigidBody*>(getEntityB());
	info.m_transform_OS_KS[0] = m_transform_OS_KS[0];
	info.m_transform_OS_KS[1] = m_transform_OS_KS[1];
	info.m_path = m_path;
	info.m_angularConstrainedDOF = m_angularConstrainedDOF;
	info.m_maxFrictionForce = m_maxFrictionForce;
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
