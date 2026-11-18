/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_FPS_CAMERA_CI
#define HK_FPS_CAMERA_CI


	/// All the parameters needed to construct a hkFpsCamera
class hkFpsCameraConstructionInfo
{
	public:
		
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CAMERA, hkFpsCameraConstructionInfo);

		hkFpsCameraConstructionInfo() {}

		struct CameraSet
		{

			/// The resting position of the camera in unrotated space. Default: ( 0.0f, 12.0f, 5.5f)
			hkVector4 m_positionUS;

			/// The position in unrotated space. Default: (0, 0, 0)
			hkVector4 m_lookAtUS;

			/// The field of view (f.o.v.) in radians. Default: 1.0
			hkReal m_fov;

			/// m_velocity [m/sec] defines the velocity for which this parameter set gets a weight of 1.0f. Default: 0.0
			/// Normally the hkReal parameters are interpolated from two sets of
			/// parameter sets. The m_velocity parameter of set 1 defines the velocity
			/// for which set 1 gets a weight of 1.0f. At velocity 0 parameter set 1
			/// gets a weight of 1.0f and set 0 a weight of 1.0f.
			hkReal m_velocity;
			

			/// This parameter defines the magnitude of the influence of the velocity for the camera direction. Default 0.01
			/// Normally the camera direction is based on the direction the car. 
			/// However in some situations the camera should also use the velocity
			/// direction. So this parameter is a factor to the current velocity
			/// to be added to the direction of the car to calculate the ideal
			/// camera direction.
			hkReal m_speedInfluenceOnCameraDirection;
		
			/// The angular speed the camera follows the car. Default 4.0
			/// m_angularRelaxation is used to make the yaw angle of the camera follow
			/// the direction of the car. Values from 1.0f to 10.0f are a good choice
			hkReal m_angularRelaxation;

			/// Construct with default values
			CameraSet();
		};

	public:


		/// m_oldFrom is the current eye position in world space.
	hkVector4			m_oldFrom;
	
		/// oldTo is the lookat position in world space.
	hkVector4			m_oldTo;
	
		/// describes the up vector of the camera.
	hkVector4			m_upDir;

		/// used to smooth the motion of the camera by interpolating the new and the old position with this factor.
	hkReal				m_heightInterpolate;
	
	hkReal				m_viewingHeight;

	int					m_smoothingIndex;

	
};

#endif /* HK_FPS_CAMERA_CI */

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
