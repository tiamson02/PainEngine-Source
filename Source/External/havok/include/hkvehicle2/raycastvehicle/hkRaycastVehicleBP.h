/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKRAYCASTVEHICLEBP
#define INC_HKRAYCASTVEHICLEBP



#include <hkvehicle2/vehicle/hkHavokVehicleBP.h>

/// RaycastVehicle BluePrint
class hkRaycastVehicleBP : public hkHavokVehicleBP
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, hkRaycastVehicleBP);

		/// Constructor.
		hkRaycastVehicleBP (int num_external_controllers = 0);

		/// Destructor.
		~hkRaycastVehicleBP();

		/// The number of external vehicle controllers for this vehicle.
		const int m_numExternalVehicleControllers;

		/// The rigid body of the chassis.
		//RigidBody *m_chassisRigidBody;
	

		/// The list of external vehicle controllers.
		class ExternalVehicleController** m_externalVehicleControllers;

		// FRICTION SOLVING PARAMETERS

		/// Specifies how the effect of dynamic load distribution is averaged with static load distribution.\n 
		/// A value of 0.0f doesn't do any averaging (dynamic load distribution is fully taken into account).\n
		/// A value of 1.0f doesn't take dynamic load distribution, and the car behaves always as if it was in static load distribution.\n
		/// Values between 0.0f and 1.0f give a certain amount of both.
		hkReal m_frictionEqualizer;

		/// Scales the torques applied by the simulation at the roll axis by the given factor.
		hkReal m_torqueRollFactor;

		/// Scales the torques applied by the simulation at the pitch axis by the given factor.
		hkReal m_torquePitchFactor;

		/// Scales the torques applied by the simulation at the yaw axis by the given factor.
		hkReal m_torqueYawFactor;

		/// An extra torque which is applied to the car when steering is pointing left or right.
	    hkReal m_extraTorqueFactor;
		// Note: some coordinate systems require negative values.

		/// A (non physical based), very very important parameter.
		/// This enables an extra angular impulse when the steering is changed.\n
		/// A value of 0.0f means no extra angular impulse.\n
		/// A value of 1.0f means that you can perfectly steer the car on ice
		/// (however the car may slide).\n
		/// So it adds a lot of extra control to the car
		/// which allows for very good control in extreme situations.
		/// Note: some coordinate systems require negative values.
		//hkReal m_extraAngularImpulseFactor;
	

		/// The rotation inertia in yaw direction for a car of the mass 1.0f kilo
		hkReal m_chassisUnitInertiaYaw;

		/// The rotation inertia in roll direction for a car of the mass 1.0f kilo
		hkReal m_chassisUnitInertiaRoll;

		/// The rotation inertia in pitch direction for a car of the mass 1.0f kilo
		hkReal m_chassisUnitInertiaPitch;

};



#endif //INC_HKRAYCASTVEHICLEBP
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
