/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RIGIDBODY_DEACTIVATOR_TYPES_H
#define HK_DYNAMICS2_RIGIDBODY_DEACTIVATOR_TYPES_H


/// A list of all deactivator types. 
/// The deactivator type of a hkRigidBody determines which mechanism Havok will use to classify the body as deactivated.
enum hkRigidBodyDeactivatorType
{
		/// Setting the deactivator type to HK_NO_RIGIDBODY_DEACTIVATOR will force the rigid body to never deactivate.
	HK_RIGIDBODY_DEACTIVATOR_NEVER = 0,

		/// Setting the deactivator type to HK_SPATIAL_RIGIDBODY_DEACTIVATOR sets Havok to use a spatial deactivation scheme.
		/// This makes use of high and low frequencies of positional motion to determine when deactivation should occur.
	HK_RIGIDBODY_DEACTIVATOR_SPATIAL,

		//
		//	The end of the list
		//
	HK_RIGIDBODY_DEACTIVATOR_MAX_ID
};

#endif // HK_DYNAMICS2_RIGIDBODY_DEACTIVATOR_TYPES_H

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
