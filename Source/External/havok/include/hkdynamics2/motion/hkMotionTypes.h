/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_MOTIONTYPES_H
#define HK_DYNAMICS2_MOTIONTYPES_H

/*! \file hkMotionTypes.h
	
*/



/// A list of all motion types. 
/// The motion type of a hkRigidBody determines what happens when the rigid body is simulated.
enum hkMotionType
{
		/// Simulation is performed using a sphere inertia tensor. (A multiple of the Identity matrix).
		/// The highest value of the diagonal of the rigid body's inertia tensor is used as the spherial inertia.
	HK_SPHERE_INERTIA_MOTION = 0,

		/// This is the same as HK_SPHERE_INERTIA_MOTION, except that simulation of the rigid body is "softened",
		/// but producing more stable results in large constrained systems
	HK_STABILIZED_SPHERE_INERTIA_MOTION,

		/// Simulation is performed using a box inertia tensor. 
		/// The non-diagonal elements of the inertia tensor are set to zero.
		/// This is slower than the HK_SPHERE_INERTIA motions, however it can produce more accurate results, especially
		/// for long thin objects
	HK_BOX_INERTIA_MOTION,

		/// This is the same as HK_BOX_INERTIA_MOTION, except that simulation of the rigid body is "softened",
		/// but producing more stable results in large constrained systems
	HK_STABILIZED_BOX_INERTIA_MOTION,

		/// Simulation is not performed as a normal rigid body. During a simulation step, the velocity of the rigid body is used to
		/// calculate the new position of the rigid body, however the velocity is NOT updated. The user can keyframe a rigid body
		/// by setting the velocity of the rigid body to produce the desired keyframe positions.  The hkKeyFrameUtility 
		/// class can be used to simply apply keyframes in this way. The velocity of a
		/// keyframed rigid body is NOT changed by the application of impulses or forces. 
		/// The keyframed rigid body has an infinite mass when viewed by the rest of the system. 
	HK_KEYFRAMED_MOTION,

		/// Fixed rigid bodies are treated in a special way by the system. They have the same effect as a rigid body with a motion of 
		/// type HK_KEYFRAMED_MOTION and velocity 0, however they much faster to use, incurring no simulation overhead, except in collision
		/// with moving bodies.
	HK_FIXED_MOTION, 	

		//
		//	The end of the list
		//
	HK_MOTION_MAX_ID
};



#endif // HK_DYNAMICS2_MOTIONTYPES_H

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
