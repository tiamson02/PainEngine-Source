/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkContactPointViewer.h>

#include <hkdynamics2/world/hkWorld.h>

#include <hkcollide2/contact/hkCollisionResults.h>

//#define HK_DISABLE_DEBUG_DISPLAY
#include <hkutilities2/visualdebugger/hkDebugDisplay.h>
#include <hkutilities2/visualdebugger/viewer/hkCollideDebugUtil.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

int hkContactPointViewer::m_tag = 0;

hkContactPointViewer::hkContactPointViewer(hkWorld* world, hkDisplayHandler* displayHandler)
	: m_world(world), m_displayHandler(displayHandler)
{
	m_displayHandler->addReference();
	world->addCollisionListener( this );
	world->addWorldDeletionListener( this );
}

void HK_CALL hkContactPointViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "ContactPoints", create );
}

void hkContactPointViewer::contactPointAddedCallback( hkContactPointAddedEvent& event)
{
/*
	// This version displays new contact points as big "stars"
	const hkVector4& p = event.m_contactPoint->getPosition();

	for (int i=0; i < 20; i++ )
	{
		hkVector4 dir( hkMath::sin( i * 1.0f ), hkMath::cos( i * 1.0f ), hkMath::sin(i * 5.0f ) );

		hkVector4 pdir; pdir.setAdd4(p, dir);
		m_displayHandler->displayLine(p, pdir, hkColor::RED);
	}
*/


	hkVector4 pos2; pos2.setAdd4( event.m_contactPoint->getPosition(), event.m_contactPoint->getNormal() );

	m_displayHandler->displayLine(event.m_contactPoint->getPosition(), pos2, hkColor::RED, m_tag);

}

void hkContactPointViewer::contactPointRemovedCallback( hkContactPointRemovedEvent& event)
{

}

void hkContactPointViewer::contactProcessCallback( hkContactProcessEvent& event)
{
	hkCollisionResult& result = event.m_collisionResult;

	for (int i = 0; i < result.m_contactPoints.getSize(); i++ )
	{
		hkVector4 pos = result.m_contactPoints[i].m_position;

		hkVector4 normal = result.m_contactPoints[i].m_normal;
		//normal.mul4( result.m_contactPoints[i].m_distance );
		hkVector4 pos2; pos2.setAdd4( pos, normal );

		m_displayHandler->displayLine(pos, pos2, hkColor::GREEN, m_tag);
	}
}


	/// Called, when the world gets deleted
void hkContactPointViewer::worldDeletedCallback( hkWorld* world )
{
	m_world->removeCollisionListener( this );
	m_world->removeWorldDeletionListener( this );
	m_world = HK_NULL;
	this->removeReference();
}

hkContactPointViewer::~hkContactPointViewer()
{
	if(m_world)
	{
		m_world->removeCollisionListener( this );
		m_world->removeWorldDeletionListener( this );
	}
	m_displayHandler->removeReference();
}


hkReferencedObject* HK_CALL hkContactPointViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkContactPointViewer(world, displayHandler);
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
