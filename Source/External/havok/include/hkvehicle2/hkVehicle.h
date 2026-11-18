/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKVEHICLE
#define INC_HKVEHICLE

// *************** havok  ***************

#include <hkmath2/hkMath.h>
#include <hkdynamics2/common/hkStepInfo.h>
#include <hkdynamics2/entity/hkRigidBody.h>

// ************* Runtime info ************
#include <hkvehicle2/wheels/hkWheelInfo.h>
#include <hkvehicle2/wheels/hkWheelInfo.h>

// *************** camera ***************
#include <hkvehicle2/camera/hkVehicleCameraBP.h>
#include <hkvehicle2/camera/hkVehicleCamera.h> 

// ******** component blueprints ********
#include <hkvehicle2/aerodynamics/hkAeroDynamicsComponentBP.h>
#include <hkvehicle2/brake/hkBrakeComponentBP.h>
#include <hkvehicle2/driverinput/hkDriverInputComponentBP.h>
#include <hkvehicle2/engine/hkEngineComponentBP.h>
#include <hkvehicle2/steering/hkSteeringComponentBP.h>
#include <hkvehicle2/suspension/hkSuspensionComponentBP.h>
#include <hkvehicle2/transmission/hkTransmissionComponentBP.h>
#include <hkvehicle2/wheels/hkWheelsComponentBP.h>
#include <hkvehicle2/chassis/hkChassisComponentBP.h>

// ************** components ************
#include <hkvehicle2/vehicle/hkVehicleComponent.h>
#include <hkvehicle2/aerodynamics/hkAeroDynamicsComponent.h>
#include <hkvehicle2/brake/hkBrakeComponent.h>
#include <hkvehicle2/driverinput/hkDriverInputComponent.h>
#include <hkvehicle2/engine/hkEngineComponent.h>
#include <hkvehicle2/steering/hkSteeringComponent.h>
#include <hkvehicle2/suspension/hkSuspensionComponent.h>
#include <hkvehicle2/transmission/hkTransmissionComponent.h>
#include <hkvehicle2/wheels/hkWheelsComponent.h>
#include <hkvehicle2/chassis/hkChassisComponent.h>


// *************** Vehicle ****************
#include <hkvehicle2/vehicle/hkHavokVehicleBP.h>
#include <hkvehicle2/vehicle/hkHavokVehicle.h>

#endif /* HK_STANDARD_VEHICLE_H */

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
