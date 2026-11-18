/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_UTILITIES2_MASS_CHANGER_UTIL
#define HK_UTILITIES2_MASS_CHANGER_UTIL


#include <hkmath2/hkMath.h>
#include <hkdynamics2/collide/hkCollisionListener.h>

#include <hkdynamics2/entity/hkEntity.h>
#include <hkdynamics2/entity/hkEntityListener.h>

class hkRigidBody;

/// A listener class used to change the masses of objects during collision
class hkCollisionMassChangerUtil: public hkBaseObject, private hkCollisionListener, private hkEntityListener
{
	public:	
			/// Adds the mass changer util as a listener to bodyA
			/// if a contact point is added between bodyA and bodyB, the relative mass of the collision is changed.
			/// you specify this using the invMassA for the apparant inverse mass of body A in the collision, and
			/// invMassB for the apparant inverse mass of body B in the collision.
			/// By default invMassA is 0, and invMassB is 1, i.e. bodyA will appear to be immovable from body B's perspective
		hkCollisionMassChangerUtil( hkRigidBody* bodyA, hkRigidBody* bodyB, float inverseMassA = 0, float inverseMassB = 1 );

		~hkCollisionMassChangerUtil();

	protected:

			// The hkCollisionListener interface implementation
		void contactPointAddedCallback(	hkContactPointAddedEvent& event );

			// The hkCollisionListener interface implementation
		void contactPointRemovedCallback( hkContactPointRemovedEvent& event ){}

			// The hkCollisionListener interface implementation
		void contactProcessCallback( hkContactProcessEvent& event );

			// The hkCollisionListener interface implementation
		void entityDeletedCallback( hkEntity* entity );


	private:

		hkRigidBody* m_bodyA;
		hkRigidBody* m_bodyB;
		hkReal m_inverseMassA;
		hkReal m_inverseMassB;
};

#endif		// HK_UTILITIES2_SURFACE_VELOCITY_UTIL

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
