/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_GROUP_FILTER_H
#define HK_DYNAMICS2_GROUP_FILTER_H

#include <hkcollide2/dispatch/hkCollisionFilter.h>

/// The group collision filter.
/// These filter assumes that each entity is assigned to:
///		- one of 32 collision layers
///		- and one of 65000 system groups.
///
/// Two entities A and B collide if:
///        They do not share the same system group (except group 0)
///  AND   the collisions between the layers of the entities are enabled (based on a 32*32 bitfield).
///
///	Example:
///        Keyframed objects should not collide with static geometry. Therefore 
///	       we can put all static geometry in one and all the keyframed object
///        another layer. Than collision between these layers are disabled.
///	Example:
///			All elements of a ragdoll should not collide with the ragdoll.
///			So we should create a ragdoll system group, which gets assigned
///         to all ragdoll parts

class hkGroupFilter : public hkCollisionFilter
{
	public:
			/// constructor, by default enables all collisions between all layer
		hkGroupFilter();


			/// enable the collision between layer layerA and layerB
		void enableCollisionsBetween(int layerA, int layerB);

			/// Disables collisions between the layers A and B
		void disableCollisionsBetween(int layerA, int layerB);

		
			/// Enables collisions between the specified layers
			/// layerBitsA and layerBitsB are bitfields, one bit for every layer.
			/// e.g. to enable collisions between one layer and all other layers,
			/// call enableCollisionsUsingBitfield( 1<< myLayer, 0xffffffff)
		void enableCollisionsUsingBitfield(hkUint32 layerBitsA, hkUint32 layerBitsB);


			/// Disables collisions between the specified collision layers. 
			/// See enableCollisionsUsingBitfield for how to use bitfields
		void disableCollisionsUsingBitfield(hkUint32 layerBitsA, hkUint32 layerBitsB);

			/// Calculate the collision filter info to be used in entities and hkEntityConstructionInfo
		static inline hkUint32 calcFilterInfo( int layer, int systemGroup = 0);


			/// Creates a new unique identifier for system groups (maximum 65k)
		inline int getNewSystemGroup();


			/// hkCollisionFilter interface implementation which returns true if the objects are enabled to collide
			/// The implementation calls isCollisionEnabled( a->getOwner()->getCollisionFilterInfo() ... )
		virtual bool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const;

	protected:

		int		    m_nextFreeSystemGroup;
		hkUint32    m_collisionLookupTable[32];
};

#include <hkdynamics2/collide/hkGroupFilter.inl>



#endif // HK_DYNAMICS2_GROUP_FILTER_H

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
