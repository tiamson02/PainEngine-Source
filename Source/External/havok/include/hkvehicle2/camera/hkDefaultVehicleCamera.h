/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef INC_DEFAULT_VEHICLE_CAMERA_H
#define INC_DEFAULT_VEHICLE_CAMERA_H



/// Default implementation of VehicleCamera, see hkDefaultVehicleCameraBP for parameter details
class DefaultVehicleCamera: public VehicleCamera
{
	public:
		/// Constructor.
		DefaultVehicleCamera( const hkDefaultVehicleCameraBP& bp);

			/// Reset all values except pointer values.
		void reinitialize( const hkDefaultVehicleCameraBP& bp);

		/// Immediately jump to the ideal yaw angle.
		void resetCamera( const HavokVehicle* vehicle);

		/// Overriden from Vehicle camera, actually calculates the new position of the camera.
		void calculateCamera ( const VehicleCameraInput& in, VehicleCameraOutput& out);

	private:

		hkReal m_cameraYawAngle;

		hkReal m_yawCorrection;
		hkReal m_yawSignCorrection;

		hkVector4 m_upDirWS;
		hkVector4 m_forwardDirWS;

		hkVector4 m_flat0DirWS;  // an orthogonal to m_upDirWS
		hkVector4 m_flat1DirWS;	 // an orthogonal to m_upDirWS and m_flat0DirWS

		hkDefaultVehicleCameraBP::hkCameraSet m_set[2];

	private:

		    /// Reset all values except pointer values.
		void reinitializeInternal( const hkDefaultVehicleCameraBP& bp );

			/// Internal methods for calculating camera position
		HK_FORCE_INLINE hkReal calcYawAngle(const hkReal factor1, const HavokVehicle* vehicle);
		HK_FORCE_INLINE hkReal calcVelocityFactor(const hkVector4& chassisVelocity);
};



#endif //INC_DEFAULT_VEHICLE_CAMERA_H

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
