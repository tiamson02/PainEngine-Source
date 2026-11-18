/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_1D_ANGULAR_FOLLOW_COM
#define HK_1D_ANGULAR_FOLLOW_COM

#include <hkutilities2/cameras/hkCamera.h>

#include <hkutilities2/cameras/1dangularfollowcam/hk1dAngularFollowCamCi.h>


/// The hk1dAngularFollowCam attaches a camera to a vehicle to aid rendering.
class hk1dAngularFollowCam : public hkCamera
{
	public:

			/// Constructor.
		hk1dAngularFollowCam (const hk1dAngularFollowCamConstructionInfo &bp);

			/// Destructor.
		virtual ~hk1dAngularFollowCam();

			/// Reset all values except pointer values.
		void reinitialize( const hk1dAngularFollowCamConstructionInfo &bp );

			/// Immediately jump to the ideal yaw angle.
		virtual void resetCamera( const hkTransform& trans, const hkVector4& linearVelocity, const hkVector4& angularVelocity);

		virtual void calculateCamera ( const CameraInput &in, CameraOutput &out );
		
	private:


		hkReal m_cameraYawAngle;

		hkReal m_yawCorrection;
		hkReal m_yawSignCorrection;

		hkVector4 m_upDirWS;
		hkVector4 m_rigidBodyForwardDir;


		hkVector4 m_flat0DirWS;  // an orthogonal to m_upDirWS
		hkVector4 m_flat1DirWS;	 // an orthogonal to m_upDirWS and m_flat0DirWS

		hk1dAngularFollowCamConstructionInfo::CameraSet m_set[2];

	private:
			/// Internal methods for calculating camera position
		HK_FORCE_INLINE hkReal calcYawAngle(const hkReal factor1, const hkTransform& trans, const hkVector4& linearVelocity);
		HK_FORCE_INLINE hkReal calcVelocityFactor(const hkVector4& bodyVelocity);
};



#endif //HK_1D_ANGULAR_FOLLOW_COM

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
