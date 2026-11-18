/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_RAYCASTCALLBACK
#define HK_RAYCASTCALLBACK

#include <hkcollide2/broadphase/hkPrimitiveCastCallback.h>

#include <hkcollide2/shape/hkShape.h> //for rayresults structure

class hkCollidable;

/// hkRayCastCallback will perform a raycast on a collidable. The broadphase calls this callback for collidables that are not during the broadphase 'castPrimitive' method.
class hkRayCastCallback : public hkPrimitiveCastCallback
{
	public:
	
		hkRayCastCallback(const hkVector4& fromWorld,const hkVector4& toWorld , const hkShape::hkRayResults& initResults = hkShape::hkRayResults () );

		//if returns true (meaning the ray hist the collidable) also update the fraction between 0 and 1 where it hits
		virtual	hkReal	castPrimitive(const hkBroadPhaseHandle* broadphaseHandle);
		/// Get the raycast results of the callback.
		const hkShape::hkRayResults& getRayResults() const;
		/// Get the raycast results of the callback.
		hkCollidable* getHitObject();

	protected:

		hkShape::hkRayResults m_rayResults;

		hkVector4	m_fromWorld;

		hkVector4	m_toWorld;

		class hkCollidable*	m_hitObject;

};

#endif //HK_RAYCASTCALLBACK

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
