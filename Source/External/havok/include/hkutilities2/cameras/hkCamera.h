/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CAMERA_H
#define HK_CAMERA_H

#include <hkmath2/hkMath.h>

class hkRigidBody;


/// The hkCamera attaches a camera to a vehicle to aid rendering.
class hkCamera : public hkReferencedObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CAMERA);

		/// Constructor.
	hkCamera (){}

		/// Destructor.
	virtual ~hkCamera(){}

	// Input for the camera calculations
	struct CameraInput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CAMERA, CameraInput);
		hkVector4		m_linearVelocity;
		hkVector4		m_angularVelocity;
		hkTransform		m_fromTrans;
		hkReal			m_deltaTime;
	};

	// Output of the camera calculations
	struct CameraOutput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CAMERA, CameraInput);
		hkVector4  m_positionWS;
		hkVector4  m_lookAtWS;
		hkVector4  m_upDirWS;
		hkReal m_fov;
		hkReal m_pad[3];
	};

		/// Virtual, resets the camera to a stable position.
	virtual void resetCamera( const hkTransform& trans, const hkVector4& linearVelocity, const hkVector4& angularVelocity){}
		/// Virtual, calculates the camera configuration for the
		/// current time.
	virtual void calculateCamera ( const CameraInput &in, CameraOutput &out ) = 0;

};


#endif //HK_CAMERA_H

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
