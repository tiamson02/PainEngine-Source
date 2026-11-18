/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

/// This listener is part of a set of utility functions and classes
/// to provide fracture functionality. This class listens for a collision
/// event of a body that is to be fractured and takes appropriate action -
/// to be documented further once I know what it really does :)

#ifndef HK_DYNAMICS2_FRACTURE_LISTENER_H
#define HK_DYNAMICS2_FRACTURE_LISTENER_H

#include <hkdynamics2/collide/hkCollisionListener.h>

typedef void* hkContactPointId;

class hkRigidBody;
class hkWorld;

class hkFractureListener : private hkCollisionListener
{
    private:
 
        hkWorld* m_world;

	public:


		hkFractureListener(hkRigidBody* compoundRigidBody,hkWorld* theWorld);
        
		~hkFractureListener();

		/// Called, after a contact point was added 
		/// Here we decide how to break the body into multiple bodies
		/// reset the states, and add new bodies to the world.
		void contactPointAddedCallback(	hkContactPointAddedEvent& event);

		/// Called before a contact point gets removed: NA for fracture
		void contactPointRemovedCallback( hkContactPointRemovedEvent& event ){}

		/// Called, just before the collisionResult is passed to the constraint system: NA for fracture 
		void contactProcessCallback( hkContactProcessEvent& event ) {}

        void entityDeletedCallback( hkEntity* entity );
     
		void entityRemovedCallback( hkEntity* entity );

};

#endif // HK_DYNAMICS2_FRACTURE_LISTENER_H

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
