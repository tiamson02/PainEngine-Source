/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_COLLIDE2_BROAD_PHASE_H
#define HK_COLLIDE2_BROAD_PHASE_H

#include <hkbase/hkBase.h>

class hkBroadPhaseHandle;
class hkBroadPhaseHandlePair;
class hkAabb;
class hkPrimitiveCastCallback;

/// The job of the broadphase is to quickly find pairs of AABBs that are intersecting, and thus to identify
/// pairs of objects that require narrowphase collison detection. Objects that can be
/// processed by the broadphase must have an hkBroadPhaseHandle, which is used as an id for each object by the broadphase. The broadphase actually keeps
/// an internal pointer to each hkBroadPhaseHandle, so do not forget this - for instance, do not copy handles.
class hkBroadPhase: public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BROAD_PHASE)

			/// Adds an object to the broadphase. The new overlapping pairs are reported in pairsOut.
		virtual void addObject( hkBroadPhaseHandle* object, const hkAabb& aabb, hkArray<hkBroadPhaseHandlePair>& pairsOut  ) = 0;

		virtual void addObjectBatch( hkArray<hkBroadPhaseHandle*>& addObjectList, hkArray< hkAabb >& addAabbList, hkArray<hkBroadPhaseHandlePair>& newPairs ) = 0;

			/// Removes an object from the broadphase. The removed overlapping pairs are reported in pairsOut.
		virtual void removeObject( hkBroadPhaseHandle* object, hkArray<hkBroadPhaseHandlePair>& pairsOut ) = 0;

		virtual void removeObjectBatch( hkArray<hkBroadPhaseHandle*>& removeObjectList, hkArray<hkBroadPhaseHandlePair>& delPairsOut ) = 0;

			/// Changes the position of an object and reports the changes. New overlaps are reported in addedPairs, removed pairs are reported in removedPairs. 
			/// Note that a pair can be in both lists. 
		virtual void updateAabbs( hkBroadPhaseHandle* objects[], const hkAabb* aabbs, int numObjects, hkArray<hkBroadPhaseHandlePair>& addedPairs, hkArray<hkBroadPhaseHandlePair>& removedPairs ) = 0;

			/// Optimises internal memory layout. This can speed up the normal operation by up to 20%.
			/// This should be done after all static objects and before all dynamics objects
			/// are added to the scene.
		virtual void defragment() = 0;

			/// Gets all AABBs (for debug purposes).
		virtual void getAllAabbs( hkArray<hkAabb>& allAabbs ) = 0;

			/// Gets AABB from hkBroadPhaseHandle
		virtual void getAabb(hkBroadPhaseHandle* object, hkAabb& aabb) const = 0;

			/// Finds all intersections between the input object and the rest. 
		virtual void querySingleAabb( const hkAabb& aabb, hkArray<hkBroadPhaseHandlePair>& pairs_out) const = 0;
		
			/// Queries the broadphase for overlapping AABBs along the ray 'from-to'.
		virtual	void castPrimitive( const hkVector4& from, const hkVector4& to,	 hkPrimitiveCastCallback& raycallback)    const = 0;

		virtual void shortCastPrimitive( const hkVector4& from, const hkVector4& to, const hkVector4& halfExtents , hkPrimitiveCastCallback& raycallback	)    const = 0;

		static hkBroadPhase* HK_CALL createDefaultBroadPhase( const hkVector4& worldMin, const hkVector4& worldMax );
};

#endif // HK_COLLIDE2_BROAD_PHASE_H

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
