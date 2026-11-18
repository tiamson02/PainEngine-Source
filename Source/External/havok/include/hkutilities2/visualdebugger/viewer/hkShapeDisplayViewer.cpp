/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkShapeDisplayViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkShapeDisplayBuilder.h>
#include <hkgeometry2/display/hkDisplayGeometry.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/entity/hkRigidBody.h>

#include <hkutilities2/visualdebugger/hkVisualDebuggerDebugOutput.h>
#include <hkutilities2/visualdebugger/hkColor.h>
static const unsigned int HK_DEFAULT_OBJECT_COLOR = hkColor::rgbFromChars( 240, 240, 240, 255 );
static const unsigned int HK_DEFAULT_FIXED_OBJECT_COLOR = hkColor::rgbFromChars( 120, 120, 120, 255 );

int hkShapeDisplayViewer::m_tag = 0;

void HK_CALL hkShapeDisplayViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "Shapes", create );
}


hkShapeDisplayViewer::hkShapeDisplayViewer( hkWorld* world, hkDisplayHandler* displayHandler )
	: m_displayHandler(displayHandler)
{
	m_world = world;  // when this was in the initializer list it crashed code warrior on ps2

	m_displayHandler->addReference();
	world->addWorldDeletionListener( this );
	world->addEntityListener( this );
	world->addWorldPostSimulationListener( this );

	// get all the active entities from the active simulation islands
	{
		const hkArray<hkSimulationIsland*>& activeIslands = world->getActiveSimulationIslands();

		for(int i = 0; i < activeIslands.getSize(); i++)
		{
			const hkArray<hkEntity*>& activeEntities = activeIslands[i]->getEntities();
			for(int j = 0; j < activeEntities.getSize(); j++)
			{
				entityAddedCallback( activeEntities[j] );
			}
		}
	}

	// get all the inactive entities from the inactive simulation islands
	{
		const hkArray<hkSimulationIsland*>& inactiveIslands = world->getInactiveSimulationIslands();

		for(int i = 0; i < inactiveIslands.getSize(); i++)
		{
			const hkArray<hkEntity*>& activeEntities = inactiveIslands[i]->getEntities();
			for(int j = 0; j < activeEntities.getSize(); j++)
			{
				entityAddedCallback( activeEntities[j] );
			}
		}
	}


	// get all the fixed bodies in the world
	{
		const hkArray<hkEntity*>& fixedEntities = world->getFixedIsland()->getEntities();
		for(int j = 0; j < fixedEntities.getSize(); j++)
		{
			entityAddedCallback( fixedEntities[j] );
		}
	}

}

void hkShapeDisplayViewer::entityAddedCallback( hkEntity* entity )
{
	if(entity->getCollidable().getShape() == HK_NULL)
	{
		return;
	}

	{
	
		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entity);

		// create an array of display geometries from the collidable - use default display settings
		hkArray<hkDisplayGeometry*> displayGeometries;
		{
			hkShapeDisplayBuilder::hkShapeDisplayBuilderEnvironment env;
			hkShapeDisplayBuilder shapeBuilder(env);
			shapeBuilder.buildDisplayGeometries( rigidBody->getCollidable().getShape(), displayGeometries);

			for(int i = (displayGeometries.getSize() - 1); i >= 0; i--)
			{
				if( (displayGeometries[i]->getType() == HK_DISPLAY_CONVEX) &&
					(displayGeometries[i]->getGeometry() == HK_NULL) )
				{
					HK_VISUAL_DEBUGGER_INFO(0, "Unable to build display geometry from hkShape geometry data");
					displayGeometries.removeAt(i);
				}
			}
		}

		// send the display geometeries off to the display handler
		{
			int id = (int)&rigidBody->getCollidable();

			m_entitiesCreated.pushBack( id );
			m_displayHandler->addGeometry( displayGeometries, rigidBody->getTransform(), id, m_tag );

#if 1 // toggle to reintroduce random colors
			if( rigidBody->isFixed() )
			{
				m_displayHandler->setGeometryColor( HK_DEFAULT_FIXED_OBJECT_COLOR, id, m_tag );
			}
			else
			{
				m_displayHandler->setGeometryColor( HK_DEFAULT_OBJECT_COLOR, id, m_tag );
			}
#endif
		}

		// delete intermediate display geometries - we could cache these for duplication - TODO
		{
			for( int i = 0; i < displayGeometries.getSize(); ++i )
			{
				delete displayGeometries[i];
			}
		}
	}
}

void hkShapeDisplayViewer::entityRemovedCallback( hkEntity* entity )
{
	if( entity->getCollidable().getShape() == HK_NULL )
	{
		return;
	}

	//if( entity->getType() == HK_RIGID_BODY )
	{
		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entity);
		int id = (int)&rigidBody->getCollidable();

		// remove the gemoetry from the displayHandler
		m_displayHandler->removeGeometry(id, m_tag);

		// remove the id from the list of 'owned' created entities
		const int index = m_entitiesCreated.indexOf(id);
		if(index >= 0)
		{
			m_entitiesCreated.removeAt(index);
		}
	}
}



void hkShapeDisplayViewer::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{
	// update the transform for all active entities (in all the active simulation islands)
	{
		const hkArray<hkSimulationIsland*>& activeIslands = world->getActiveSimulationIslands();

		for(int i = 0; i < activeIslands.getSize(); i++)
		{
			const hkArray<hkEntity*>& activeEntities = activeIslands[i]->getEntities();
			for(int j = 0; j < activeEntities.getSize(); j++)
			{
				hkRigidBody* rigidBody = static_cast<hkRigidBody*>(activeEntities[j]);
				m_displayHandler->updateGeometry(rigidBody->getTransform(), (int)&rigidBody->getCollidable(), m_tag);
			}
		}
	}
	// update the transform for all active entities (in all the active simulation islands)
	{
		const hkArray<hkSimulationIsland*>& inactiveIslands = world->getInactiveSimulationIslands();

		for(int i = 0; i < inactiveIslands.getSize(); i++)
		{
			const hkArray<hkEntity*>& inactiveEntities = inactiveIslands[i]->getEntities();
			for(int j = 0; j < inactiveEntities.getSize(); j++)
			{
				hkRigidBody* rigidBody = static_cast<hkRigidBody*>(inactiveEntities[j]);
				m_displayHandler->updateGeometry(rigidBody->getTransform(), (int)&rigidBody->getCollidable(), m_tag);
			}
		}
	}

//#define  KINEMATIC_OBJECTS
#ifdef KINEMATIC_OBJECTS

	{
		hkSimulationIsland* fixedIsland = world->getFixedIsland();

		hkArray<hkEntity*>& activeEntities = fixedIsland->getEntities();
		for(int j = 0; j < activeEntities.getSize(); j++)
		{
			hkRigidBody* rigidBody = static_cast<hkRigidBody*>(activeEntities[j]);
			m_displayHandler->updateGeometry(rigidBody->getTransform(), &rigidBody->getCollidable());
		}
	}
#endif 

}

void hkShapeDisplayViewer::removeAllGeometries()
{
	for(int i = m_entitiesCreated.getSize() - 1; i >= 0 ; i--)
	{
		m_displayHandler->removeGeometry(m_entitiesCreated[i], m_tag);
		m_entitiesCreated.popBack();
	}
}

void hkShapeDisplayViewer::worldDeletedCallback( hkWorld* world )
{
	world->removeWorldDeletionListener( this );
	world->removeEntityListener( this );
	world->removeWorldPostSimulationListener( this );

	m_world = HK_NULL;
	this->removeReference();
}

hkShapeDisplayViewer::~hkShapeDisplayViewer()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeEntityListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}

	removeAllGeometries();
	m_displayHandler->removeReference();
}


hkReferencedObject* HK_CALL hkShapeDisplayViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkShapeDisplayViewer(world, displayHandler);
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
