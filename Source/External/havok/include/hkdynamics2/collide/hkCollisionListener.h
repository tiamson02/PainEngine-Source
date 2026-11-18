/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_DYNAMICS2_COLLISION_LISTENER_H
#define HK_DYNAMICS2_COLLISION_LISTENER_H

#include <hkmath2/basetypes/hkContactPoint.h>
#include <hkconstraintsolver2/constraint/contact/hkContactPointProperties.h>

typedef void* hkContactPointId;

class hkCollidable;
struct hkCollisionEnvironment;


struct hkCollisionResult;
class hkEntity;

class hkDynamicsContactMgr;

enum hkContactPointAdditionStatus
{
	HK_CONTACT_POINT_VALID = 0,
	HK_CONTACT_POINT_INVALID
};

///This collision event is passed to a hkCollisionListener's contactPointAddedCallback() after a contact point is added.
struct hkContactPointAddedEvent
{
	///Identifies the contact point.
	const hkContactPointId m_contactPointId;
	///The contact manager.
	hkDynamicsContactMgr& m_contactMgr;
	///The first entity's hkCollidable.
	const hkCollidable& m_collidableA;
	///The second entity's hkCollidable.
	const hkCollidable& m_collidableB;
	///The entity that fired this callback.
	hkEntity* m_callbackFiredFrom; 

	///The contact point.
	hkContactPoint* m_contactPoint;
	///The contact point properties, including friction and restitution.
	hkContactPointProperties*  m_contactPointProperties;

	/// Whether the contact point will actually be added to the constraint solver or not. 
	/// By default this is HK_CONTACT_POINT_VALID. If you set this to HK_CONTACT_POINT_INVALID,
	/// there will be no further action taken for this contact point, and no process or remove 
	/// events will be fired for the contact point.
	hkContactPointAdditionStatus m_status;

	///The relative velocity of the objects at the point of contact projected onto the collision normal.
	hkReal		m_projectedVelocity;        
	///Creates a new hkContactPointAddedEvent.
	inline hkContactPointAddedEvent( 	hkContactPointId contactPointId, hkDynamicsContactMgr& contactMgr,	const hkCollidable& a,	const hkCollidable& b,	hkContactPoint* cp, hkContactPointProperties* cpp, hkReal projectedVelocity);
};

///This collision event is passed to a hkCollisionListener's contactPointRemovedCallback() before a contact point is removed.
struct hkContactPointRemovedEvent
{
	///Identifies the contact point.
	const hkContactPointId m_contactPointId;
	///The contact manager.
	hkDynamicsContactMgr& m_contactMgr;
	///The first entity.
	hkEntity*		m_entityA;
	///The second entity.
	hkEntity*		m_entityB;
	///The entity that fired this callback.
        hkEntity* m_callbackFiredFrom;
	///Creates a new hkContactPointRemovedEvent.
	inline hkContactPointRemovedEvent( 	hkContactPointId contactPointId, hkDynamicsContactMgr& contactMgr,	hkEntity* a,	hkEntity* b);
};

///This collision event is passed to a hkCollisionListener's contactProcessCallback() just before the hkCollisionResult is passed to the constraint system.
struct hkContactProcessEvent
{
	///The contact manager.
	hkDynamicsContactMgr& m_contactMgr;
	///The first entity's hkCollidable.
	const hkCollidable& m_collidableA;
	///The second entity's hkCollidable.
	const hkCollidable& m_collidableB;
	///The collision result.
	hkCollisionResult& m_collisionResult;
    ///The entity that fired this callback.
	hkEntity* m_callbackFiredFrom;

	///Creates a new hkContactProcessEvent.
	inline hkContactProcessEvent( 	hkDynamicsContactMgr& contactMgr,	const hkCollidable& a,	const hkCollidable& b,	hkCollisionResult& cr);
};

/// The base interface for listeners that respond to collision events.
class hkCollisionListener
{
	public:
			/// Called after a contact point is added. You can use this callback to override the friction and restitution values in the contact point's hkContactPointProperties.
		virtual void contactPointAddedCallback(	hkContactPointAddedEvent& event) = 0;

			/// Called before a contact point gets removed.
		virtual void contactPointRemovedCallback( hkContactPointRemovedEvent& event ) = 0;

			/// Called just before the collisionResult is passed to the constraint system. This callback allows you to change any information in the result. 
		virtual void contactProcessCallback( hkContactProcessEvent& event ) {}
};

#include <hkdynamics2/collide/hkCollisionListener.inl>


#endif // HK_DYNAMICS2_COLLISION_LISTENER_H


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
