/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_PAIRWISE_COLLISION_FILTER_H
#define HK_PAIRWISE_COLLISION_FILTER_H

#include <hkcollide2/dispatch/hkCollisionFilter.h>
#include <hkdynamics2/entity/hkEntityListener.h>
#include <hkbase/htl/hkMap.h>

/// A simple filter to disable collisions on a pairwise basis. 
/// Note that this is currently not very efficient with large numbers of pairs, as it is implemented using a list rather than a map.
class hkPairwiseCollisionFilter : public hkCollisionFilter, private hkEntityListener
{
	public:

		~hkPairwiseCollisionFilter();

			/// Disables collisions between this pair of collidables.
		void disableCollisionPair(hkEntity* a, hkEntity* b);

			/// Enables collisions between this pair of collidables.
		void enableCollisionPair(hkEntity* a, hkEntity* b);

			/// hkCollisionFilter interface implementation.
			/// This implementation checks if the pair is in the disabled list.
		virtual bool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const;

			/// hkEntityListener implementation - removes any references to removed entities.
		virtual void entityRemovedCallback(hkEntity* entity);

			/// hkEntityListener implementation.
		virtual void entityDeletedCallback(hkEntity* entity);
		
			/// gives the number of disabled pairs in the filter
		virtual int  getNumDisabledPairs() const;

	public:

		struct CollisionPair
		{
			inline CollisionPair(hkEntity* a, hkEntity* b);
			
			hkEntity* m_a;
			hkEntity* m_b;
		};

	private:
		hkMap<CollisionPair, int> m_disabledPairs;
};

inline bool operator == (const hkPairwiseCollisionFilter::CollisionPair& a, const hkPairwiseCollisionFilter::CollisionPair& b);


#include <hkexport2/customelements/hkPairwiseCollisionFilter.inl>


#endif // HK_PAIRWISE_COLLISION_FILTER_H

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
