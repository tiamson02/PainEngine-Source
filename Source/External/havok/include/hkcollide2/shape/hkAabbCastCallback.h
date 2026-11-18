/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_AABBCASTRAYCALLBACK
#define HK_AABBCASTRAYCALLBACK

#include <hkcollide2/shape/hkRayCastCallback.h> //for rayresults structure

class hkCollidable;

	/// hkAabbCastCallback performs an aabb cast. The broadphase can call this callback the during the 'castPrimitive' method.
class hkAabbCastCallback : public hkRayCastCallback
{
	public:
	
		/// constructor will take aabb fromWorld, aabb toWorld and the aabb halfextents
		hkAabbCastCallback (const hkVector4& fromWorld,const hkVector4& toWorld, const hkVector4& halfExtents);

		/// castPrimitive will perform a aabbcast on the collidable (collidable is passed as a broadphaseHandle)
		virtual	hkReal	castPrimitive(const hkBroadPhaseHandle* broadphaseHandle);
		
	protected:

		hkVector4	m_halfExtents;

};

#endif //HK_AABBCASTRAYCALLBACK
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
