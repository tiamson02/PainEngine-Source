/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_EXPORT2_DISABLE_ALL_COLLISION_FILTER_H
#define HK_EXPORT2_DISABLE_ALL_COLLISION_FILTER_H

#include <hkcollide2/dispatch/hkCollisionFilter.h>
#include <hkdynamics2/entity/hkEntityListener.h>

	/// This class is subclassed to implement special code for disabling or enabling collisions between objects.
class hkDisableAllCollisionFilter : public hkCollisionFilter, private hkEntityListener
{
	public:
			/// Returns true if the specified objects can collide, overrides from hkCollisionFilter
		virtual bool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b ) const;

		~hkDisableAllCollisionFilter();

		//adds entity if the pointer is valid and not already in list and returns true, else returns false
		bool addEntityToFilter( hkEntity* new_entity);
		//removes the entity from the stored array if it exists and returns true, if the entity is not in the array it returns false
		bool removeEntityFromFilter( hkEntity* new_entity);

		/// hkEntityListener implementation - removes any references to removed entities.
		virtual void entityRemovedCallback(hkEntity* entity);

		virtual void entityDeletedCallback(hkEntity* entity);

		int getNumStoredEntities() {return m_disabledEntities.getSize();}

	private:

		hkArray<hkEntity*> m_disabledEntities;

};


#endif // HK_EXPORT2_DISABLE_ALL_COLLISION_FILTER_H

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
