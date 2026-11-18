/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_COLLIDE2_COLLISION_ENVIRONMENT_H
#define HK_COLLIDE2_COLLISION_ENVIRONMENT_H

class hkCollisionDispatcher;
class hkManifold;
class hkContactMgr;
class hkCollisionFilter;

///An hkCollisionEnvironment is a struct containing an hkCollisionDispatcher and an hkContactMgr. A hkWorld has an hkCollisionEnvironment member.
struct hkCollisionEnvironment
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkCollisionEnvironment);


		/// The dispatcher. This is used to create collision agents between colliding shapes.
	hkCollisionDispatcher* m_dispatcher;

		/// A callback class to enable / disable collisions
	hkCollisionFilter* m_filter;

		/// The contact manager. This is called by collision agents to report contact points. 
	mutable hkContactMgr* m_contactMgr;

		/// The collision tolerance - collisions will only be reported if closer than this distance
	hkReal m_tolerance;

		/// This tolerance is used by the convex-convex manifold to reject points which are too close together
		/// The default value is .05, i.e. contact points less than 5 cm apart are considered to be equivalent
		/// If you are simulating an object with a side length of less than 5 cm you need to reduce this tolerance
	hkReal m_convexManifoldWeldTolerance;

		/// This tolerance is used by the convex-convex manifold to remove points when the original points of
		/// contact from both objects have drifted too far apart along the contact plane.  The default value 
		/// is 0.4f which means that points that have slid apart by 40cm will be removed from the manifold.
		/// Note that if the time step is large this tolerance may need to be increased in order to avoid throwing
		/// out contact points that are necessary to maintain a proper manifold.
	hkReal m_convexManifoldDriftTolerance;

		/// An internal optimization flag
	bool m_useTims;
};


#endif // HK_COLLIDE2_COLLISION_ENVIRONMENT_H

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
