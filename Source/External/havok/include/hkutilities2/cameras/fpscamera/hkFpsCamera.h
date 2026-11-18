/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_FPS_CAMERA_H
#define HK_FPS_CAMERA_H

#include <hkutilities2/cameras/hkCamera.h>
#include <hkutilities2/cameras/fpscamera/hkFpsCameraCi.h>

/// The hkCamera attaches a first person shooter camera to a character to aid rendering.
class hkFpsCamera : public hkCamera 
{
public:

		/// Constructor.
	hkFpsCamera (const class hkFpsCameraConstructionInfo& fpsci);

		/// Destructor.
	virtual ~hkFpsCamera();

		/// Virtual, resets the camera to a stable position.
	virtual void resetCameraToBody( const hkRigidBody* body);

		/// Virtual, calculates the camera configuration for the
		/// current time.
	virtual void calculateCamera ( const CameraInput &in, CameraOutput &out );

	void	getFullOrientation( hkQuaternion& fullOrientatonOut);

		/// get the look at point in world space
	hkVector4 getLookAtWs(const hkTransform& trans);

		/// the up/down angle of the camera
	hkReal						m_angleY;

		/// the direction of the camera
	hkReal						m_angleX;

	hkVector4					m_oldFrom;

	hkVector4					m_oldTo;

	int							m_smoothingIndex;

	hkVector4					m_upDir;

	// used to smooth the motion of the camera by interpolating the new and the old position with this factor.
	hkReal						m_heightInterpolate;

	hkReal						m_viewingHeight;

	//cached rotation
	hkQuaternion				m_fullOrientaton;

};


#endif //HK_FPS_CAMERA_H

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
