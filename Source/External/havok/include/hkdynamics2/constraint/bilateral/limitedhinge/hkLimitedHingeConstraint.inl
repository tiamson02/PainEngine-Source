/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


//////////////////////////////////////////////////////////////////////////
// hkLimitedHinge set and get methods

//////////////////////////////////////////////////////////////////////////

inline void hkLimitedHingeConstraint::setMaxAngularLimit(hkReal rad)
{
	m_maxAngle = rad;
}

//////////////////////////////////////////////////////////////////////////

inline void hkLimitedHingeConstraint::setMinAngularLimit(hkReal rad)
{
	m_minAngle = rad;
}

//////////////////////////////////////////////////////////////////////////

inline void hkLimitedHingeConstraint::setMaxFrictionTorque(hkReal tmag)
{
	m_maxFrictionTorque = tmag;
}

/////////////////////////////////////////////
inline hkReal hkLimitedHingeConstraint::getMaxFrictionTorque() const
{
	return m_maxFrictionTorque;
}

/////////////////////////////////////////////
inline hkReal hkLimitedHingeConstraint::getMaxAngularLimit() const
{
	return m_maxAngle;
}

/////////////////////////////////////////////
inline hkReal hkLimitedHingeConstraint::getMinAngularLimit() const
{
	return m_minAngle;
}

/////////////////////////////////////////////

/// Sets data for constraint except the rigid bodies.
inline void hkLimitedHingeConstraint::setData(const hkLimitedHingeConstructionInfo& info)
{

	m_minAngle = info.m_minAngle;
	m_maxAngle = info.m_maxAngle;
	
	m_maxFrictionTorque = info.m_maxFrictionTorque;

	m_pivotInA = info.m_pivotInA;
	m_pivotInB = info.m_pivotInB;

	m_perpToAxleInA1 = info.m_perpToAxleInA1;
	m_perpToAxleInA2 = info.m_perpToAxleInA2;

 	m_axleInA = info.m_axleInA;
	m_axleInB = info.m_axleInB;  
	m_perp2FreeAxisInB = info.m_perp2FreeAxisInB;
	m_lastAngle = 0;

	if(!isValid())
	{
		HK_ASSERT("Members of limited constraint inconsistent.");
	}
}

/////////////////////////////////////////////

inline void hkLimitedHingeConstraint::getConstructionInfo(hkLimitedHingeConstructionInfo& info)
{

	info.m_bodyA = static_cast<hkRigidBody*>(getEntityA());
	info.m_bodyB = static_cast<hkRigidBody*>(getEntityB());
	
	info.m_minAngle = m_minAngle;
	info.m_maxAngle = m_maxAngle;
	
	info.m_maxFrictionTorque = m_maxFrictionTorque;

	info.m_pivotInA = m_pivotInA;
	info.m_pivotInB = m_pivotInB;

	info.m_perpToAxleInA1 = m_perpToAxleInA1;
	info.m_perpToAxleInA2 = m_perpToAxleInA2;

 	info.m_axleInA = m_axleInA;
	info.m_axleInB = m_axleInB;  
	info.m_perp2FreeAxisInB = m_perp2FreeAxisInB;
}

/////////////////////////////////////////////



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
