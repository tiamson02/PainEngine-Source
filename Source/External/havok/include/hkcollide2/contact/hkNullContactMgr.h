/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_NULL_CONTACT_MGR_H
#define HK_COLLIDE2_NULL_CONTACT_MGR_H

#include <hkcollide2/contact/hkContactMgr.h>

class hkCollidable;
struct hkCollisionEnvironment;
class hkCollisionContactPoint;
struct hkCollisionResult;


	/// An hkContactMgr that doesn't add any contact points.
class hkNullContactMgr: public hkContactMgr
{
	public:

			///No contact points are added in this implementation.
		hkContactPointId addContactPoint(	const hkCollidable& a, const hkCollidable& b, const hkCollisionEnvironment& env, hkCollisionContactPoint& cp)
		{ return HK_NULL; }

		void removeContactPoint( hkContactPointId cpId, const hkCollisionEnvironment& env)
		{}

		void processContact( const hkCollidable& a, const hkCollidable& b, const hkCollisionEnvironment& env, hkCollisionResult& collisionResult )
		{}


		void getContactPoints( hkArray<hkCollisionContactPoint>& points, int maxNum )
		{}

};

#endif // HK_COLLIDE2_NULL_CONTACT_MGR_H

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
