/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CONTACT_MGR_H
#define HK_COLLIDE2_CONTACT_MGR_H

class hkCollidable;
struct hkCollisionEnvironment;
struct hkCollisionResult;

class hkCollisionContactPoint;

typedef void* hkContactPointId;

#define HK_INVALID_CONTACT_POINT  ( hkContactPointId(-1) )

#include <hkbase/hkBase.h>

	/// The hkContactMgr is an interface used by a collision agent to report its results. 
class hkContactMgr: public hkBaseObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONTACT)

			/// Allocates a new contact point, typically a contact constraint.
		virtual hkContactPointId addContactPoint(const 	hkCollidable& a, const hkCollidable& b, const hkCollisionEnvironment& env, hkCollisionContactPoint& cp ) = 0;

			/// Frees a contact point. 
		virtual void removeContactPoint( hkContactPointId cpId, const hkCollisionEnvironment& env) = 0;

			/// Processes all the contact information.
		virtual void processContact(const  hkCollidable& a, const hkCollidable& b, const hkCollisionEnvironment& env, hkCollisionResult& collisionResult ) = 0;

			/// get all the collision contact points from the contact constraints
			/// may not fill out the m_contactPointId field
		virtual void getContactPoints( hkArray<hkCollisionContactPoint>& points, int maxNum ) = 0;

};



#endif // HK_COLLIDE2_CONTACT_MGR_H

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
