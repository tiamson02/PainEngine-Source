/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_AABB_PHANTOM_H
#define HK_DYNAMICS2_AABB_PHANTOM_H

#include <hkdynamics2/phantom/hkPhantom.h>
#include <hkcollide2/dispatch/hkCollisionFilter.h>

class hkAabb;
struct hkAabbPhantomConstructionInfo;
class hkTypedBroadPhaseHandler;
class hkBroadPhase;
class hkCollidable;
class hkWorld;


/// A phantom is a pinned entity that maintains a list of other entities whose AABBs overlap with it (including other phantoms).
/// The phantom itself is just an AABB and has no physical presence in the simulation. You can use phantoms to create areas where events occur if the area is entered.
/// Phantoms are very fast as they use only broadphase collision detection.
class hkAabbPhantom : public hkPhantom
{
	public:
			/// Creates a new hkAabbPhantom using the information in the construction info parameter.
			/// To add a phantom to the world, you use hkWorld::addEntity( myPhantom ).
		hkAabbPhantom( const hkPhantomConstructionInfo& info );

		~hkAabbPhantom();
		
			///Gets the list of collidables that are currently overlapping with the phantom.
		inline hkArray<hkCollidable*>& getOverlappingCollidables();

			/// Casts a ray (specified by fromWorld and toWorld) and returns the first hit object that overlaps with the phantom. 
			/// The hitNormalWorld and mindist values are filled in for the result.
		hkCollidable* castRay( const hkVector4& fromWorld, const hkVector4& toWorld, hkVector4& hitNormalWorld, float& mindist );
		
	public:

			/// Callback implementation 
		virtual void addOverlappingCollidable( hkCollidable& collidable );

			/// Callback implementation 
		virtual void removeOverlappingCollidable( hkCollidable& collidable );


	protected:

		inline bool addFilteredOverlappingCollidable( hkCollidable& collidable );

		inline bool removeFilteredOverlappingCollidable( hkCollidable& collidable );


		hkArray<hkCollidable*> m_overlappingCollidables;
		
		hkCollisionFilter*	m_collisionFilter;
};

#include <hkdynamics2/phantom/hkAabbPhantom.inl>

#endif	// HK_DYNAMICS2_AABB_PHANTOM_H

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
