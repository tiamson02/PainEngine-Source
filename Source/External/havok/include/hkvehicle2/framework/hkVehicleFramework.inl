/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline const HavokVehicle* VehicleFramework::getCurrentVehicle() const
{
	return m_vehicle;
}
	
inline void VehicleFramework::setCurrentVehicle(const HavokVehicle *vehicle)
{
	m_vehicle = vehicle;
}

inline hkReal VehicleFramework::getRPM() const
{
	const hkReal rpm = m_vehicle->getEngineComponent()->getEngineRPM();

	return rpm;
}

inline hkReal VehicleFramework::getKMPH() const
{
	const hkReal vel_ms = m_vehicle->getChassisMotionState()->getLinearVelocity().length3();
	const hkReal vel_kmh = vel_ms / 1000.0f * 3600.0f;

	return vel_kmh;
}

inline hkReal VehicleFramework::getMPH() const
{
	const hkReal vel_ms = m_vehicle->getChassisMotionState()->getLinearVelocity().length3();
	const hkReal vel_mph = vel_ms / 1609.3f * 3600.0f;

	return vel_mph;
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
