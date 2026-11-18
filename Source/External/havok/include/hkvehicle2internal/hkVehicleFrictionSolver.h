/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK2VEHICLEFRICTIONSOLVER
#define HK2VEHICLEFRICTIONSOLVER

#include <hkmath2/hkMath.h>

enum { HK_VEHICLE_FRICTION_N_AXIS = 2 };

	//
	//	Step info
	//

	/// Info about the time for our vehicle
struct hkVehicleStepInfo
{
	float m_deltaTime;
	float m_invDeltaTime;
};



	//
	//	Const Vehicle Info
	//
/// The class hkVehicleFrictionDescription is used by vehicle dynamics as a const description of how to simulate the friction of a vehicle
/// this class can be initialized using hkVehicleFrictionDescriptionInitValues()
struct hkVehicleFrictionDescription
{

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ACTION, hkVehicleFrictionDescription)

		// a helper class to fill in the values;
		// all values should be given in chassis local space.
	struct ConstructionInfo
	{
			/// An approximation of the final wheel position of a standing car.
			/// This information is used to calculate some tables for the internal implicit integrator for
			/// the friction clipping.
			/// Note: If in the final game the wheels change position, this internal table can get slightly incorrect.
			/// However the effect is minimal (<1% in normal situations). If you are worried and you want to check the correctness
			/// of this function, you can call hkVehicleFrictionDescriptionInitValues every step with the correct wheel positions.
			/// (On ps2, this could be rather slow).
		hkVector4 m_wheelPosition[HK_VEHICLE_FRICTION_N_AXIS];
		hkVector4 m_chassisCenterOfMass;
		hkVector4 m_directionRight;
		hkVector4 m_directionFront;
		hkVector4 m_directionUp;

		hkMatrix3 m_chassisFrictionInertiaInv;
		hkReal m_chassisMassInv;

			/// This is the inertia of the wheel around the axle
		hkReal m_wheelAxleAngularInertia[HK_VEHICLE_FRICTION_N_AXIS];
			/// This is the wheels radius
		hkReal m_wheelRadius[HK_VEHICLE_FRICTION_N_AXIS];

			/// Equalizes the friction between front and backwheels: If set to 0, realistic behavior,
			/// if set to 1.0, the system redistrubutes the wheel down forces to be the down forces of a 
			/// non moving, in its resting position car. Values between 0 and 1 interpolate between those values
		hkReal m_frictionEqualizer;
	};


	//
	//	Internal use only
	//
	enum { hkFricionEllipseLineSegments = 16 };

	/// Used by the vehicle friction solver as a const description for a certain type of vehicle.
	struct AxisDescription
	{

		//
		//	For friction clipping (internal optimization)
		//
		float	m_frictionCircleYtab[hkFricionEllipseLineSegments];	
		float	m_xStep;
		float	m_xStart;

			// the rotation inertia of the wheel
		hkReal	m_wheelSurfaceInertia;
		hkReal	m_wheelSurfaceInertiaInv;

		hkReal  m_wheelChassisMassRatio; 

		hkReal  m_wheelRadius;
		hkReal  m_wheelRadiusInv;

			// the final wheeldown force will be calculated using the following formulas:
			// wheelDownForce = m_wheelDownForceFactor * wheelDownForce + m_wheelDownForceSumFactor * ( sumOfAllWheelDownForces)
		hkReal  m_wheelDownForceFactor;
		hkReal  m_wheelDownForceSumFactor;
	};

	hkReal m_extraAngularImpulseFactor;
	hkReal m_extraTorqueFactor;
	hkReal m_wheelDistance;

	hkReal m_chassisMassInv;

	AxisDescription m_axleDescr[HK_VEHICLE_FRICTION_N_AXIS];
};

	/// This class is used for persistant and export data of the vehicle friction solver
struct hkVehicleFrictionStatus
{

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONSTRAINT, hkVehicleFrictionStatus)

		struct  AxisStatus
		{

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONSTRAINT, AxisStatus)

			//
			//	Export data
			//

			hkReal  m_forward_slip_velocity;	// [m/sec]
			hkReal  m_skid_energy_density;		// [N]
			hkReal	m_side_force;				// [N]
			
			AxisStatus()
			{
				m_delayed_forward_impulse = 0.0f;			// 
				m_forward_slip_velocity = 0.0f;				// the velocity between a skidding tire and ground
				m_skid_energy_density = 0.0f;				// the energy destroyed in one physical timestep by a skidding tire
			}

			//
			// For internal use only
			//
			hkReal m_delayed_forward_impulse;	// the to the next PSI delayed forward impulse
		};
		
		AxisStatus m_axis[ HK_VEHICLE_FRICTION_N_AXIS ];
};




/// The class hkVehicleFrictionSolver is used is an input and output to the vehicle dynamics.
/// It holds all values which change every frame
/// See RaycastVehicle for more details...
struct hkVehicleFrictionSolverAxleParams
{

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ACTION, hkVehicleFrictionSolverAxleParams);

		//
		//	Geometry of the car
		//
		hkVector4 m_contactPointWS;
		hkVector4 m_constraintNormalWS;
		hkVector4 m_forwardDirWS;


		//
		// Friction Info
		//
		hkReal	m_frictionCoefficient;
		hkReal	m_viscosityFrictionCoefficient;
		hkReal	m_wheelDownForce;
		hkReal	m_forwardForce;

		//
		//	Handbrake Info
		//
		bool	m_wheelFixed;

			/// Initialises all data members to default values.
		inline void initialize();
};



/// The class hkVehicleFrictionSolverParams is used by vehicle dynamics.
/// See RaycastVehicle for more details...
struct hkVehicleFrictionSolverParams
{

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ACTION, hkVehicleFrictionSolverParams)
		//
		// dynamic section
		//

		hkVehicleFrictionSolverAxleParams m_axleParams[HK_VEHICLE_FRICTION_N_AXIS];

		hkReal m_fastYawSteeringAngle;
		hkReal m_fastYawDriverInputAngle;

		hkReal		m_chassisMassInv;

		hkMatrix3	m_chassisInertiaInv;
		hkVector4	m_chassisLinearVelocity;
		hkVector4	m_chassisAngularVelocity;
		hkVector4   m_chassisMassCenter;
};

inline void hkVehicleFrictionSolverAxleParams::initialize()
{
	
	m_contactPointWS.setZero4();
	m_constraintNormalWS.setZero4();
	m_forwardDirWS.setZero4();
	m_frictionCoefficient = 0.0f;
	m_viscosityFrictionCoefficient = 0.0f;
	m_wheelDownForce = 0.0f;
	m_forwardForce = 0.0f;
	m_wheelFixed = false;
}


extern "C"
{
	/// setup internal cache information hkVehicleFrictionDescription for fast solving of the vehicle 
	void HK_CALL hkVehicleFrictionDescriptionInitValues( const hkVehicleFrictionDescription::ConstructionInfo& ci, hkVehicleFrictionDescription& out);

	/// solve the vehicle friction
	void HK_CALL hkVehicleFrictionApplyVehicleFriction(	const hkVehicleStepInfo& stepInfo,
												const hkVehicleFrictionDescription& descr,
												hkVehicleFrictionSolverParams &params,
												hkVehicleFrictionStatus &status);
}


#endif //HK2VEHICLEFRICTIONSOLVER

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
