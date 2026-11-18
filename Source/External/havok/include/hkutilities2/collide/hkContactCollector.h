/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_CONTACT_COLLECTOR
#define HK_CONTACT_COLLECTOR

#include <hkmath2/hkMath.h>
#include <hkdynamics2/collide/hkCollisionListener.h>
#include <hkdynamics2/entity/hkEntityListener.h>

class hkRigidBody;

	/// This class collects all contact points for a specified entity.\n
	/// For this class to work successfully, hkRigidBody::m_processContactCallbackSkip should be set to 0.
class hkContactCollector : private hkCollisionListener, private hkEntityListener
{
	
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CDINFO, hkContactCollector);

	public:
		/*
		** public classes
		*/
		struct ContactPoint
		{
			hkVector4	m_normal;
			hkVector4	m_position;
			hkRigidBody* m_bodyA;
			hkRigidBody* m_bodyB;
			hkReal		m_distance;
		};

	public:

		virtual ~hkContactCollector();
		
			/// Gets all contact points
		const hkArray<ContactPoint>& getContactPoints(){ return m_contactPoints; }

			/// Flips contact points (normal and bodies), so that bodyA is always refBody
		void flipContactPoints( hkRigidBody* refBody );

			/// Resets all contact points
		void reset();

			/// Adds the contact collector as a listener to an entity. This also stores a reference to the entity
			/// to allow it to remove itself as a listener on destruction.
		void addToEntity( hkEntity* entity );

			/// Removes the contact collector from the entity. 
		void removeFromEntity( hkEntity* entity );

	protected:
			// the hkCollisionListener interface implementation
		virtual void contactPointAddedCallback(	  hkContactPointAddedEvent& event);

			// the hkCollisionListener interface implementation
		virtual void contactPointRemovedCallback( hkContactPointRemovedEvent& event);

			// the hkCollisionListener interface implementation
		virtual void contactProcessCallback( hkContactProcessEvent& event);

			// hkEntityListener interface implementation
		virtual void entityDeletedCallback( hkEntity* entity );



	protected:

		hkArray<ContactPoint> m_contactPoints;

		hkInplaceArray<hkEntity*, 1> m_entities;
};


#endif // HK_CONTACT_COLLECTOR

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
