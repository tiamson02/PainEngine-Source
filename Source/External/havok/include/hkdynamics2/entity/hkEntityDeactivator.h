/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#ifndef HK_DYNAMICS2_ENTITY_DEACTIVATOR_H
#define HK_DYNAMICS2_ENTITY_DEACTIVATOR_H

#include <hkbase/hkBase.h>

class hkEntity;

/// An hkEntityDeactivator is created for every entity. This tells the system when it is safe to deactivate the entity.
class hkEntityDeactivator : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEACTIVATOR)

		///High frequency deactivation is used to quickly deactivate entities that have come to a definite halt. 
		virtual bool shouldDeactivateHighFrequency( const hkEntity* entity ) = 0;
		///Low frequency deactivation checks less frequently if the object needs to be deactivated. This makes it less likely that the system will deactivate an object that might quickly need to move again.
		virtual bool shouldDeactivateLowFrequency( const hkEntity* entity ) = 0;

	protected:

		hkEntityDeactivator() {}
};






#endif // HK_DYNAMICS2_ENTITY_DEACTIVATOR_H

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
