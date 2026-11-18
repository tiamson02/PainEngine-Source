/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_RESPONSE_MODIFIER_H
#define HK_DYNAMICS2_RESPONSE_MODIFIER_H

class hkDynamicsContactMgr;
class hkRigidBody;

/// This class contains a number of functions to allow altering the behavior of collision response
/// NB: you may NOT modify the response of a contact point twice (ie call 2 of these functions on one constraint)
/// NB: any functions that receive a pointer as an argument expect the object to remain extant for the lifetime
/// of the constraint. DO NOT delete an object passed into this modifier until the constraint has been removed
/// (unless you are not going to call simulate() again)

class hkResponseModifier
{
	public:

			/// scales the inverse mass of each object by the relevant scaling factor
			/// NOTE: ONLY USE THIS FUNCTION WITH THE DEFAULT CONTACT MANAGER
		static void HK_CALL setInvMassScalingForContact( hkDynamicsContactMgr* manager, hkRigidBody* bodyA, hkRigidBody* bodyB, float factorA, float factorB );

			/// sets the surfaceVelocity
			/// NOTE: ONLY USE THIS FUNCTION WITH THE DEFAULT CONTACT MANAGER
		static void HK_CALL setSurfaceVelocity( hkDynamicsContactMgr* manager, hkRigidBody* body, hkVector4* velWorld );

			/// sets the surface to have low viscosity. Effectively disables static friction.
			/// NOTE: ONLY USE THIS FUNCTION WITH THE DEFAULT CONTACT MANAGER
		static void HK_CALL setLowSurfaceViscosity( hkDynamicsContactMgr* manager );

};

#endif		// HK_DYNAMICS2_RESPONSE_MODIFIER_H

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
