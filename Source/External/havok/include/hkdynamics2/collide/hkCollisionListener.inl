/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkContactPointAddedEvent::hkContactPointAddedEvent( 	hkContactPointId contactPointId, hkDynamicsContactMgr& contactMgr,
					const hkCollidable& ca,	const hkCollidable& cb,
					hkContactPoint* ccp, hkContactPointProperties* dcp, hkReal projectedVelocity):
	m_contactPointId(contactPointId), 	m_contactMgr(contactMgr),
	m_collidableA(ca),		m_collidableB(cb),
	m_contactPoint(ccp),	m_contactPointProperties(dcp),
	m_projectedVelocity( projectedVelocity )
{
}

hkContactPointRemovedEvent::hkContactPointRemovedEvent( 	hkContactPointId contactPointId, hkDynamicsContactMgr& contactMgr,	hkEntity* ca,	hkEntity* cb):
	m_contactPointId(contactPointId), 	m_contactMgr(contactMgr),
	m_entityA(ca),		m_entityB(cb)
{
}

hkContactProcessEvent::hkContactProcessEvent( hkDynamicsContactMgr& contactMgr,	const hkCollidable& ca,	const hkCollidable& cb, hkCollisionResult& cr):
	m_contactMgr(contactMgr),
	m_collidableA(ca),		m_collidableB(cb),
	m_collisionResult( cr )
{
}

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
