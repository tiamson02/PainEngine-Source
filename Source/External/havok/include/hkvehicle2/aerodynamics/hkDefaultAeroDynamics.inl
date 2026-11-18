/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/*
** GET Methods
*/
const hkReal &DefaultAerodynamics::getAirDensity() const
{
	return m_airDensity;
}

const hkReal &DefaultAerodynamics::getFrontalArea() const
{
	return m_frontalArea;
}

const hkReal &DefaultAerodynamics::getDragCoefficient() const
{
	return m_dragCoefficient;
}

const hkReal &DefaultAerodynamics::getLiftCoefficient() const
{
	return m_liftCoefficient;
}

const hkVector4 &DefaultAerodynamics::getExtraGravity() const
{
	return m_extraGravityws;
}

/*
** SET Methods
*/
void DefaultAerodynamics::setAirDensity(const hkReal &air_density)
{
	m_airDensity = air_density;
}

void DefaultAerodynamics::setFrontalArea(const hkReal &frontal_area)
{
	m_frontalArea = frontal_area;
}

void DefaultAerodynamics::setDragCoefficient(const hkReal &drag_coefficient)
{
	m_dragCoefficient = drag_coefficient;
}

void DefaultAerodynamics::setLiftCoefficient(const hkReal &lift_coefficient)
{
	m_liftCoefficient = lift_coefficient;
}

void DefaultAerodynamics::setExtraGravity(const hkVector4 &extra_gravityws)
{
	m_extraGravityws = extra_gravityws;
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
