/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
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

			/// Scales the inverse mass of each object by the relevant scaling factor.
			/// NOTE: ONLY USE THIS FUNCTION WITH THE DEFAULT CONTACT MANAGER.
			/// Note: setting a factor to 0.0f makes that object "fixed" for the purpose of resolving this contact constraint.
		static void HK_CALL setInvMassScalingForContact( hkDynamicsContactMgr* manager, hkRigidBody* bodyA, hkRigidBody* bodyB, float factorA, float factorB );

			/// Scales the force applied by a contact. 
			///
			/// Using small values, e.g. less than 0.05f, yields noticeably softer contact response and allows object to interpenetrate.
			/// NOTE: ONLY USE THIS FUNCTION WITH THE DEFAULT CONTACT MANAGER.
		static void HK_CALL setImpulseScalingForContact( hkDynamicsContactMgr* manager, hkRigidBody* bodyA, hkRigidBody* bodyB, float usedImpulseFraction );

			/// sets the surfaceVelocity
			/// NOTE: ONLY USE THIS FUNCTION WITH THE DEFAULT CONTACT MANAGER
		static void HK_CALL setSurfaceVelocity( hkDynamicsContactMgr* manager, hkRigidBody* body, hkVector4* velWorld );

			/// removes the surfaceVelocity
			/// NOTE: ONLY USE THIS FUNCTION AFTER CALLING setSurfaceVelocity
		static void HK_CALL clearSurfaceVelocity( hkDynamicsContactMgr* manager, hkRigidBody* body, hkVector4* velWorld );

			/// sets the surface to have low viscosity. Effectively disables static friction.
			/// NOTE: ONLY USE THIS FUNCTION WITH THE DEFAULT CONTACT MANAGER
		static void HK_CALL setLowSurfaceViscosity( hkDynamicsContactMgr* manager );

};

#endif		// HK_DYNAMICS2_RESPONSE_MODIFIER_H

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
