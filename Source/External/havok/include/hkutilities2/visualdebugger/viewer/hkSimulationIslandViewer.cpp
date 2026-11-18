/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkSimulationIslandViewer.h>

#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/entity/hkEntity.h>

//#define HK_DISABLE_DEBUG_DISPLAY
#include <hkutilities2/visualdebugger/viewer/hkCollideDebugUtil.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

int hkSimulationIslandViewer::m_tag = 0;

hkSimulationIslandViewer::hkSimulationIslandViewer(hkWorld* world, hkDisplayHandler* displayHandler)
	:	m_showActiveIslands(true),
		m_showInactiveIslands(true),
		m_world(world),
		m_displayHandler(displayHandler)
{
	m_displayHandler->addReference();
	world->addWorldDeletionListener( this );
	world->addWorldPostSimulationListener( this );
}

void HK_CALL hkSimulationIslandViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "Islands", create );
}

void hkSimulationIslandViewer::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{
	//	hkprintf("island display...\n");
	if (m_showActiveIslands)
	{
		const hkArray<hkSimulationIsland*>& islands = world->getActiveSimulationIslands();
		if(islands.getSize() > m_activeIslandDisplayGeometries.getSize())
		{
			m_activeIslandDisplayGeometries.setSize(islands.getSize());
		}

		hkArray<hkDisplayGeometry*> displayGeometries;
		displayGeometries.setSize(islands.getSize());

		hkArray<hkAabb> islandAabbs;
		islandAabbs.setSize(islands.getSize());

		for ( int i = 0; i < islands.getSize(); ++i )
		{
			const hkArray<hkEntity*>& entities = islands[i]->getEntities();

			islandAabbs[i].m_max.setAll(-hkMath::HK_REAL_MAX);
			islandAabbs[i].m_min.setAll(hkMath::HK_REAL_MAX);

			// Create one aabb about all the entities.
			hkAabb aabb;
			hkReal halfTolerance = world->getCollisionEnvironment()->m_tolerance * 0.5f;
			hkVector4 minExtent;
			hkVector4 maxExtent;	
			maxExtent.setAll(-hkMath::HK_REAL_MAX);
		    minExtent.setAll(hkMath::HK_REAL_MAX);

			for ( int j = 0; j < entities.getSize(); ++j )
			{
				hkCollidable& c = entities[j]->getCollidable();

				// hkCollidable may not have an hkShape.
				if (c.getShape() != HK_NULL)
				{
					c.getShape()->getAabb( *c.getTransform(), halfTolerance, aabb );

					maxExtent.setMax4( maxExtent, aabb.m_max );
					minExtent.setMin4( minExtent, aabb.m_min );
				}
			}

			m_activeIslandDisplayGeometries[i].setExtents(minExtent, maxExtent);
			displayGeometries[i] = &m_activeIslandDisplayGeometries[i];
		}

		m_displayHandler->displayGeometry(displayGeometries, hkColor::BLUE, m_tag);
	}

	if (m_showInactiveIslands)
	{
		const hkArray<hkSimulationIsland*>& islands = world->getInactiveSimulationIslands();
	
		if(islands.getSize() > m_inactiveIslandDisplayGeometries.getSize())
		{
			m_inactiveIslandDisplayGeometries.setSize(islands.getSize());
		}

		hkArray<hkDisplayGeometry*> displayGeometries;
		displayGeometries.setSize(islands.getSize());

		for ( int i = 0; i < islands.getSize(); ++i )
		{
			const hkArray<hkEntity*>& entities = islands[i]->getEntities();

			// Create one aabb about all the entities.
			hkAabb aabb;
			hkReal halfTolerance = world->getCollisionEnvironment()->m_tolerance * 0.5f;
			hkVector4 minExtent;
			hkVector4 maxExtent;	
			maxExtent.setAll(-hkMath::HK_REAL_MAX);
		    minExtent.setAll(hkMath::HK_REAL_MAX);

			for ( int j = 0; j < entities.getSize(); ++j )
			{
				hkCollidable& c = entities[j]->getCollidable();

				// hkCollidable may not have an hkShape.
				if (c.getShape() != HK_NULL)
				{
					c.getShape()->getAabb( *c.getTransform(), halfTolerance, aabb );

					maxExtent.setMax4( maxExtent, aabb.m_max );
					minExtent.setMin4( minExtent, aabb.m_min );
				}
			}

			m_inactiveIslandDisplayGeometries[i].setExtents(minExtent, maxExtent);
			displayGeometries[i] = &m_inactiveIslandDisplayGeometries[i];
		}

		m_displayHandler->displayGeometry(displayGeometries, hkColor::GREEN, m_tag);

	}
}

/// Called when hkWorld is deleted.
void hkSimulationIslandViewer::worldDeletedCallback( hkWorld* world )
{
	world->removeWorldDeletionListener( this );
	world->removeWorldPostSimulationListener( this );
	m_world = HK_NULL;
	this->removeReference();
}

hkSimulationIslandViewer::~hkSimulationIslandViewer()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}
	m_displayHandler->removeReference();
}

hkReferencedObject* HK_CALL hkSimulationIslandViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkSimulationIslandViewer(world, displayHandler);
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
