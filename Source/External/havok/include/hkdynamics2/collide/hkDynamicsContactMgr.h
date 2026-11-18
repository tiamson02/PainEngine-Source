/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_DYNAMICS_CONTACT_MGR_H
#define HK_DYNAMICS2_DYNAMICS_CONTACT_MGR_H

#include <hkcollide2/contact/hkContactMgr.h>

typedef void* hkContactPointId;
class hkContactPoint;
class hkContactPointProperties;
class hkVector4;


///An interface to access contact point information.
class hkDynamicsContactMgr: public hkContactMgr
{
	public:
			/// Gets the properties attached to the contact point (if available)
		virtual hkContactPointProperties* getContactPointProperties( hkContactPointId id ){ return HK_NULL;}

			/// Gets the dynamics contact point (if available)
		virtual hkContactPoint* getContactPoint( hkContactPointId id ){ return HK_NULL;}

			/// get all the collision contact points from the contact constraints
		virtual void getContactPoints( hkArray<hkCollisionContactPoint>& points, int maxNum ){}

			/// get the ids of all contact points in this collision
		virtual void getAllContactPointIds( hkArray<hkContactPointId>& contactPointIds ) {}
};

#endif // HK_DYNAMICS2_DYNAMICS_CONTACT_MGR_H

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
