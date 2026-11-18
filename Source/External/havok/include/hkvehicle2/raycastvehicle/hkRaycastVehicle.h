/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_RAYCASTVEHICLE_H
#define HK_RAYCASTVEHICLE_H


#include <hkvehicle2/vehicle/hkHavokVehicle.h>
#include <hkvehicle2/raycastvehicle/hkRaycastVehicleBP.h>
#include <hkvehicle2internal/hkVehicleFrictionSolver.h>

/// A "RayCast" vehicle, which uses the intersections of rays fired from the vehicle wheels to determine
/// contact points for the vehicle.
class RaycastVehicle : public HavokVehicle
{
public:
	/// Constructor.
	RaycastVehicle (const hkRaycastVehicleBP &bp,class hkWorld* world);

	/// Reset all values except pointer values.
	void reinitialize( const hkRaycastVehicleBP &bp );

	/// (Overriden from HavokVehicle) Destructor.
	~RaycastVehicle();

	/*
	** Specific of RaycastVehicle
	** ------------------------------
	*/

	/// Returns a pointer to the rigid body associated with the chassis.
	//inline RigidBody *getChassisRigidBody() const;

	/// Retrieves the number of ExternalVehicleControllers.
	inline int getNumExternalVehicleControllers() const;

	/// Retrieves the id-th ExternalVehicleController.
	inline ExternalVehicleController *getExternalVehicleController(int id) const;


protected:

	// Overrided functions from HavokVehicle
	virtual void updateBodiesInfo(const hkStepInfo& stepInfo);
	
	void doSimulationStep(const hkStepInfo& stepInfo);

private:


	void reinitializeInternal( const hkRaycastVehicleBP &bp );
		    //: reset all values except pointer values


private:

	// For the raycaster
	class	hkWorld*	m_world;
	class hkAabbPhantom*	m_phantom;

	// For the friction: const data
	hkVehicleFrictionDescription m_frictionDescription;

	// For the friction: export data
	hkVehicleFrictionStatus m_frictionStatus;

	// the inertia used for normal operation
	hkVector4		m_chassisInertiaInvDiag;

	// the inertia used for friction calculation
	hkVector4		m_chassisFrictionInertiaInvDiag;

	// External controllers
	int				m_numExternalVehicleControllers;
	ExternalVehicleController** m_externalVehicleControllers;


	float			m_extraTorqueFactor;
	
};

#include <hkvehicle2/raycastvehicle/hkRaycastVehicle.inl>



#endif

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
