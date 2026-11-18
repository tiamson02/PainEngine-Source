/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdynamics2/entity/hkRigidBody.h>
#include <hkmath2/linear/hkVector4Util.h>

/////////////////////////////////////////////////////////////////

inline void hkPrismaticConstraint::setMaxLinearLimit(hkReal max)
{
	m_maxLimit = max;
}

//////////////////////////////////////////////////////////////////

inline void hkPrismaticConstraint::setMinLinearLimit(hkReal min)
{
	m_minLimit = min;
}

//////////////////////////////////////////////////////////////////

inline void hkPrismaticConstraint::setMaxFrictionForce(hkReal fmag)
{
	m_maxFrictionForce = fmag;	
}

//////////////////////////////////////////////////////////////////		

inline hkReal hkPrismaticConstraint::getMaxLinearLimit() const
{
	return m_maxLimit;
}

//////////////////////////////////////////////////////////////////

inline hkReal hkPrismaticConstraint::getMinLinearLimit() const
{
	return m_minLimit;
}

//////////////////////////////////////////////////////////////////

inline hkReal hkPrismaticConstraint::getMaxFrictionForce() const
{
	return m_maxFrictionForce;
}

//////////////////////////////////////////////////////////////////

inline void hkPrismaticConstraint::setData(const hkPrismaticConstructionInfo& info)
{
	m_pivotInA = info.m_pivotInA;
	m_pivotInB = info.m_pivotInB;
	m_shaftInA = info.m_shaftInA;
	m_shaftInB = info.m_shaftInB;
	m_minLimit = info.m_minLimit;
	m_maxLimit = info.m_maxLimit;
	m_perpToShaftInB1 = info.m_perpToShaftInB1;
	m_perpToShaftInB2 = info.m_perpToShaftInB2;
	m_BtoAoffsetRotation = info.m_BtoAoffsetRotation;
    m_maxFrictionForce = info.m_maxFrictionForce;
	
	if(!isValid())
	{
		HK_ASSERT("Members of prismatic constraint inconsistent.");
	}
}

////////////////////////////////////////////////////////////////////

inline void hkPrismaticConstraint::getConstructionInfo(hkPrismaticConstructionInfo& info)
{
	info.m_bodyA = static_cast<hkRigidBody*>(getEntityA());
	info.m_bodyB = static_cast<hkRigidBody*>(getEntityB());
	info.m_pivotInA = m_pivotInA;
	info.m_pivotInB = m_pivotInB;
	info.m_shaftInA = m_shaftInA;
	info.m_shaftInB = m_shaftInB;
	info.m_minLimit = m_minLimit;
	info.m_maxLimit = m_maxLimit;
	info.m_perpToShaftInB1 = m_perpToShaftInB1;
	info.m_perpToShaftInB2 = m_perpToShaftInB2;
	info.m_BtoAoffsetRotation = m_BtoAoffsetRotation;
    info.m_maxFrictionForce = m_maxFrictionForce;
}

////////////////////////////////////////////////////////////////////


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
