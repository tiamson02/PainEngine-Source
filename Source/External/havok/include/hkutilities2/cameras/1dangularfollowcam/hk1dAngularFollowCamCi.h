/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_1D_ANGULAR_FOLLOW_COM_CI
#define HK_1D_ANGULAR_FOLLOW_COM_CI


	/// All the parameters needed to construct a hk1dAngularFollowCam
class hk1dAngularFollowCamConstructionInfo 
{
	public:
		
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CAMERA, hk1dAngularFollowCamConstructionInfo);

		hk1dAngularFollowCamConstructionInfo();

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

		/// An extra angular add-on to the yaw angle (needed if the internal angle calculation has an offset)
		hkReal m_yawCorrection;

		/// This member can be set to -1.0f (instead of 1.0f) to reverse the camara yaw angle
		hkReal m_yawSignCorrection;

		/// A normalized vector pointing up in world space.
		hkVector4 m_upDirWS;

		/// A normalized vector pointing forward in the local space of the rigid body
		hkVector4 m_rigidBodyForwardDir;

		/// Two parameters sets for the car values.
		/// Depending on the velocity of the car, the parameters actually 
		/// used are calculated by interpolating between these two sets
		/// using the following formula:<br>
		/// hkReal f1 = min (1.0f, current_velocity / set[1].m_velocity);<br>
		/// hkReal f0 = 1.0f - f1;<br>
		/// set = f0 * m_set[0] + f1 * m_set[1]
		CameraSet m_set[2];
		

};

#endif /* HK_1D_ANGULAR_FOLLOW_COM_CI */

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
