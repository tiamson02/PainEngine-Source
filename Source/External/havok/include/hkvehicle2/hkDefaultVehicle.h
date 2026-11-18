/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_HKDEFAULTVEHICLE
#define INC_HKDEFAULTVEHICLE

#include <hkvehicle2/hkVehicle.h>

// *************** camera ***************
#include <hkvehicle2/camera/hkDefaultVehicleCameraBP.h>
#include <hkvehicle2/camera/hkDefaultVehicleCamera.h> 

// ********  default component blueprints ********
#include <hkvehicle2/aerodynamics/hkDefaultAeroDynamicsBP.h>
#include <hkvehicle2/brake/hkDefaultBrakeBP.h>
#include <hkvehicle2/driverinput/hkDefaultAnalogDriverInputBP.h>
#include <hkvehicle2/engine/hkDefaultEngineBP.h>
#include <hkvehicle2/steering/hkDefaultSteeringBP.h>
#include <hkvehicle2/suspension/hkDefaultSuspensionBP.h>
#include <hkvehicle2/transmission/hkDefaultTransmissionBP.h>
#include <hkvehicle2/wheels/hkDefaultWheelsBP.h>
#include <hkvehicle2/chassis/hkDefaultChassisBP.h>

// ************** default components ************
#include <hkvehicle2/aerodynamics/hkDefaultAeroDynamics.h>
#include <hkvehicle2/brake/hkDefaultBrake.h>
#include <hkvehicle2/driverinput/hkDefaultAnalogDriverInput.h>
#include <hkvehicle2/engine/hkDefaultEngine.h>
#include <hkvehicle2/steering/hkDefaultSteering.h>
#include <hkvehicle2/suspension/hkDefaultSuspension.h>
#include <hkvehicle2/transmission/hkDefaultTransmission.h>
#include <hkvehicle2/wheels/hkDefaultWheels.h>
#include <hkvehicle2/chassis/hkDefaultChassis.h>

// *************** Default (Raycast) Vehicle ****************
#include <hkvehicle2internal/hkVehicleFrictionSolver.h>

#include <hkvehicle2/raycastvehicle/hkExternalVehicleController.h>
#include <hkvehicle2/raycastvehicle/hkRaycastVehicleBP.h>
#include <hkvehicle2/raycastvehicle/hkRaycastVehicle.h>

// ***************** External Controllers ******************
#include <hkvehicle2/raycastvehicle/hkAngularVelocityDamperBP.h>
#include <hkvehicle2/raycastvehicle/hkAngularVelocityDamper.h>

// *************** Framework *************
#include <hkvehicle2/framework/hkVehicleFrameworkBP.h>
#include <hkvehicle2/framework/hkSkidmarksInfo.h>
#include <hkvehicle2/framework/hkVehicleFramework.h>

#endif //INC_HKDEFAULTVEHICLE

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
