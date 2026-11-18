/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_EXPORT2_GROUP_COLLISION_FILTER_H
#define HK_EXPORT2_GROUP_COLLISION_FILTER_H

#include <hkcollide2/dispatch/hkCollisionFilter.h>

/// The group collision filter. There are 32 possible collision groups, and
/// each entity can be assigned to any number of groups 
/// (one group is defined by one bit in a 32 bit integer).
/// You can then disable or enable collisions between these groups.
/// Objects that have not been assigned to a collision group have a collision group value of 0.
/// Collision group 32 is reserved for use as a disable all collision filter, any bodies in that group
/// will not collide with anything
class hkGroupCollisionFilter : public hkCollisionFilter
{
	public:

		hkGroupCollisionFilter();


				/// Enables collisions between the specified collision groups. If one of the parameters passed to this function is 0,
				/// the m_noGroupCollisionEnabled flag is set to true.
				/// This is a flag that indicates whether objects that are not assigned to any group can collide with objects that are group members.
				/// By default, it is set to true.
		void enableCollisionGroups(hkUint32 groupBitsA, hkUint32 groupBitsB);

				/// Disables collisions between the specified collision groups. If one of the parameters passed to this function is 0,
				/// the m_noGroupCollisionEnabled flag is set to false.
				
		void disableCollisionGroups(hkUint32 groupBitsA, hkUint32 groupBitsB);

				/// Returns true if the specified groups A and B have collisions enabled. 
				/// The special cases of (0,X) or (X,0) always return the m_noGroupCollisionEnabled value.
		bool isCollisionEnabled(hkUint32 groupBitsA, hkUint32 groupBitsB) const;


			/// hkCollisionFilter interface implementation which returns true if the objects are enabled to collide, based on their collision groups. 
		/// The implementation calls isCollisionEnabled( a->getOwner()->getCollisionFilterInfo() ... )
		virtual bool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const;

	private:

		hkUint32    m_collisionGroups[32];
};




#endif // HK_DYNAMICS2_GROUP_COLLISION_FILTER_H

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
