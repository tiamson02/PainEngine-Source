/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_COLLIDE2_COLLISION_FILTER_SET_H
#define HK_COLLIDE2_COLLISION_FILTER_SET_H

#include <hkcollide2/dispatch/hkCollisionFilter.h>

	/// This class allows you to set a collision filter that is composed of multiple other collision filters.
class hkCollisionFilterSet : public hkCollisionFilter
{
	public:

			/// Destructor removes all references to collision filters
		~hkCollisionFilterSet();

			/// Get the collision filter
		inline const hkArray<hkCollisionFilter*>& getCollisionFilters() const;

			/// adds a new collision filter: Attention: this does not start reevaluating the collideable pairs
		void addCollisionFilter( hkCollisionFilter* filter);

			/// adds a new collision filter: Attention: this does not start reevaluating the collideable pairs
		void removeCollisionFilter( hkCollisionFilter* filter);

			/// Returns true if the specified objects can collide.
		virtual bool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b ) const;


	private:

		hkInplaceArray<hkCollisionFilter*,3> m_collisionFilters;

};

#include <hkcollide2/dispatch/hkCollisionFilterSet.inl>

#endif // HK_COLLIDE2_COLLISION_FILTER_SET_H

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
