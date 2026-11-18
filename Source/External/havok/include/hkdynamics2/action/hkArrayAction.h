/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_ARRAY_ACTION_H
#define HK_DYNAMICS2_ARRAY_ACTION_H

#include <hkdynamics2/action/hkAction.h>

///You can use this as a base class for hkActions that operate on a large number of entities.
/// In addition to the hkAction interface, this class provides some useful basic functionality for your 
/// actions, such as an array to store the entity references and a callback that removes the action from the simulation
/// if one of the entities is removed.
class hkArrayAction : public hkAction
{

	public:
	
		///Creates a new hkArrayAction that operates on the specified entities.
		hkArrayAction(const hkArray<hkEntity*>& bodies);

		///Adds an entity to the action.
		void addEntity(hkEntity* entity);

		///Removes an entity from the action.
		void removeEntity(hkEntity* entity);
	
		///Gets the action's entities.
		virtual void getEntities( hkArray<hkEntity*>& entitiesOut );

		///Removes the action from the hkWorld if any of its entities are removed from the simulation.
		virtual void entityRemovedCallback(hkEntity* entity);

	protected:
		~hkArrayAction();

		///The action's entities.
		hkArray<hkEntity*> m_bodies;
};


#endif // HK_DYNAMICS2_ARRAY_ACTION_H

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
