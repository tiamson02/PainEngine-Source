/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline /*non-const*/ WheelInfo& WheelsComponent::getWheelInfoRef(int wheel)
{
	return m_wheelsInfo[wheel];
}


void WheelsComponent::postUpdateWheels()
{

}

/*
** GET MEthods
*/

inline const int WheelsComponent::getNumWheels() const
{
	return m_numWheels;
}

inline const int WheelsComponent::getNumAxles() const
{
	return m_numAxles;
}

inline const int WheelsComponent::getNumWheelsOnAxle(int axle) const
{
	return m_numWheelsPerAxle[axle];
}

inline const WheelInfo &WheelsComponent::getWheelInfo(int wheel) const
{
	return m_wheelsInfo[wheel];
}

inline const int WheelsComponent::getWheelAxle(int wheel) const
{
	return m_wheelsAxle[wheel];
}

inline const hkReal &WheelsComponent::getWheelRadius( int wheel) const
{
	return m_wheelsRadius[wheel];
}

inline const hkReal &WheelsComponent::getWheelWidth( int wheel) const
{
	return m_wheelsWidth[wheel];
}

inline const hkReal &WheelsComponent::getWheelFriction( int wheel) const
{
	return m_wheelsFriction[wheel];
}

inline const hkReal &WheelsComponent::getWheelViscosityFriction( int wheel) const
{
	return m_wheelsViscosityFriction[wheel];
}

/*
** Set Methods
*/

inline void WheelsComponent::setWheelRadius ( int wheel, const hkReal &radius)
{
	m_wheelsRadius[wheel] = radius;
}

inline void WheelsComponent::setWheelWidth ( int wheel, const hkReal &width)
{
	m_wheelsWidth[wheel] = width;
}

inline void WheelsComponent::setWheelFriction ( int wheel, const hkReal &friction)
{
	m_wheelsFriction[wheel] = friction;
}

inline void WheelsComponent::setWheelViscosityFriction ( int wheel, const hkReal &v_friction)
{
	m_wheelsViscosityFriction[wheel] = v_friction;
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
