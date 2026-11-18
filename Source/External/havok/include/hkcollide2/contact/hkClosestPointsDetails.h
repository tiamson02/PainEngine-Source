/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_CLOSEST_POINTS_DETAILS_H
#define HK_COLLIDE2_CLOSEST_POINTS_DETAILS_H

#include <hkmath2/hkMath.h>

class hkCollidable;

	/// A class to receive callbacks when hkCollisionAgent::getClosestPoints() is called. The user can derive from this class
	/// to store any necessary information with the callback.  The default implementation is hkSimpleClosestPointDetails, which
	/// maintains a list of the contact points returned from getClosestPoints().
class hkClosestPointsDetails : public hkBaseObject 
{

	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CONTACT);
		
		virtual ~hkClosestPointsDetails();
		
		struct ClosestPointEvent
		{
			hkVector4 m_pointInWorld;
			hkVector4 m_normalInWorld;
			const hkCollidable* m_collidableA;
			const hkCollidable* m_collidableB; 
			hkReal m_distance;
		};


			/// callback, pointInWorld and normalInWorld, from collidable A's perspective and the shortest distance between a and b
		virtual	void addClosestPointCallback( const ClosestPointEvent& event ) = 0;

};

#endif //HK_COLLIDE2_CLOSEST_POINTS_DETAILS_H

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
