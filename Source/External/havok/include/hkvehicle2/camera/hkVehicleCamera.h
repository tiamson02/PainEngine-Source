/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_VEHICLE_CAMERA_H
#define INC_VEHICLE_CAMERA_H



/// The VehicleCamera attaches a camera to a vehicle to aid rendering.
class VehicleCamera : public hkBaseObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE);

		/// Constructor.
	VehicleCamera (const hkVehicleCameraBP &bp);

		/// Destructor.
	virtual ~VehicleCamera();

	// Input for the camera calculations
	struct VehicleCameraInput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, VehicleCameraInput);
		const class HavokVehicle* m_vehicle;
		hkReal m_deltaTime;
	};

	// Output of the camera calculations
	struct VehicleCameraOutput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VEHICLE, VehicleCameraInput);
		hkVector4  m_positionWS;
		hkVector4  m_lookAtWS;
		hkVector4  m_upDirWS;
		hkReal m_fov;
		hkReal m_pad[3];
	};

		/// Virtual, resets the camera to a stable position.
	virtual void resetCamera( const HavokVehicle * vehicle ) = 0;

		/// Virtual, calculates the camera configuration for the
		/// current time.
	virtual void calculateCamera ( const VehicleCameraInput &in, VehicleCameraOutput &out ) = 0;

protected:

	    /// Reset all values except pointer values.
	void reinitializeInternal( const hkVehicleCameraBP &bp );

};



#endif //INC_VEHICLE_CAMERA_H

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
