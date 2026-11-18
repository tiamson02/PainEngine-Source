/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKHAVOKVEHICLEBP
#define INC_HKHAVOKVEHICLEBP



/// Havok vehicle BluePrint. Minimum construction info for any HavokVehicle, passed in a struct.
/// See the Vehicle Programmers' Reference for more information
class hkHavokVehicleBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkHavokVehicleBP);

		/*
		** Components
		*/

		/// The Chassis component for the vehicle
		ChassisComponent*		m_chassis;
		
		/// The Wheels component for this vehicle
		WheelsComponent*		m_wheels;

		/// The DriverInput component for the vehicle
		DriverInputComponent*	m_driverInput;

		/// The Steering component for the vehicle
		SteeringComponent*		m_steering;

		/// The Engine component for the vehicle
		EngineComponent*		m_engine;

		/// The Transmission component for the vehicle
		TransmissionComponent*	m_transmission;

		/// The Brake component for the vehicle
		BrakeComponent*			m_brake;

		/// The Aerodynamics component for the vehicle
		AerodynamicsComponent*	m_aerodynamics;

		/// The Suspension component for the vehicle
		SuspensionComponent*	m_suspension;

		class	hkRigidBody*	m_chassisMotionState;

};



#endif //INC_HKHAVOKVEHICLEBP

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
