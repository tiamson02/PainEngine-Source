/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK2VEHICLEFRICTIONSOLVER
#define HK2VEHICLEFRICTIONSOLVER

#include <hkmath/hkMath.h>
#include <hkvehicle/friction/hkVehicleFriction.h>
#include <hkconstraintsolver/accumulator/hkRigidAccumulator.h>

/// The class hkVehicleFrictionSolver is used is an input and output to the vehicle dynamics.
/// It holds all values which change every frame.
/// See hkRaycastVehicle for more details.
struct hkVehicleFrictionSolverAxleParams
{

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ACTION, hkVehicleFrictionSolverAxleParams);

		//
		//	Geometry of the car
		//
		hkVector4 m_contactPointWs;
		hkVector4 m_constraintNormalWs;
		hkVector4 m_forwardDirWs;

		//
		//	Ground Object
		//
		class hkRigidAccumulator* m_groundObject;
		class hkRigidAccumulator* m_groundObjectAtLastIntegration;

		//
		// Friction Info: 
		//  formula:   finalFriction = min( m_maxFrictionCoefficient, m_frictionCoefficient + slipVelocity * m_viscosityFrictionCoefficient );
		//             maxForce = finalFriction * m_wheelDownForce
		//
		hkReal	m_frictionCoefficient;
		hkReal	m_viscosityFrictionCoefficient;
		hkReal  m_maxFrictionCoefficient;
		hkReal	m_wheelDownForce;
		hkReal	m_forwardForce;

		//
		//	Handbrake Info
		//
		hkUint8	m_wheelFixed;

			/// Initialises all data members to default values.
		inline void initialize();
};



/// The class hkVehicleFrictionSolverParams is used by vehicle dynamics.
/// See hkRaycastVehicle for more details.
struct hkVehicleFrictionSolverParams
{

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ACTION, hkVehicleFrictionSolverParams);
		//
		// dynamic section
		//

		hkVehicleFrictionSolverAxleParams m_axleParams[2];/*HK_VEHICLE_FRICTION_N_AXIS*/

		hkReal m_maxVelocityForPositionalFriction;

		class hkRigidAccumulator m_chassis;
		class hkRigidAccumulator m_chassisAtLastIntegration;
};

inline void hkVehicleFrictionSolverAxleParams::initialize()
{
	m_contactPointWs.setZero4();
	m_constraintNormalWs.setZero4();
	m_forwardDirWs.setZero4();
	m_frictionCoefficient = 0.0f;
	m_viscosityFrictionCoefficient = 0.0f;
	m_maxFrictionCoefficient = 0.0f;
	m_wheelDownForce = 0.0f;
	m_forwardForce = 0.0f;
	m_wheelFixed = false;
	m_groundObject = HK_NULL;
	m_groundObjectAtLastIntegration = HK_NULL;
}


extern "C"
{
		/// Setup internal cache information hkVehicleFrictionDescription for fast solving of the vehicle.
	void hkVehicleFrictionDescriptionInitValues( const hkVehicleFrictionDescription::Cinfo& ci, hkVehicleFrictionDescription& out);

		/// Solve the vehicle friction.
	void hkVehicleFrictionApplyVehicleFriction(	const hkVehicleStepInfo& stepInfo,
												const hkVehicleFrictionDescription& descr,
												hkVehicleFrictionSolverParams &params,
												hkVehicleFrictionStatus &status);
}




#endif //HK2VEHICLEFRICTIONSOLVER

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
