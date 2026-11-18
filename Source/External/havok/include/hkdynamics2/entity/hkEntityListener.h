/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_ENTITY_LISTENER_H
#define HK_DYNAMICS2_ENTITY_LISTENER_H

#include <hkbase/hkBase.h>

class hkEntity;

/// Any class that is interested in events from an entity inherits from this class. For example, hkConstraints and hkActions are entity listeners.
class hkEntityListener
{
	public:

		virtual ~hkEntityListener() {}

			/// Called when an entity is deleted. hkEntityListener subclasses <b>must</b> implement this function.
		virtual void entityDeletedCallback( hkEntity* entity ) {}

			/// Called when an entity is removed from the hkWorld.
		virtual void entityRemovedCallback( hkEntity* entity ) {}

			/// Called when an entity is added to the hkWorld.
		virtual void entityAddedCallback( hkEntity* entity ) {}


};


#endif // HK_DYNAMICS2_ENTITY_LISTENER_H

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
