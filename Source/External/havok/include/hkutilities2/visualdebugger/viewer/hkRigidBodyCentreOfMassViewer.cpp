/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkRigidBodyCentreOfMassViewer.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/entity/hkRigidBody.h>

#include <hkutilities2/visualdebugger/hkDebugDisplay.h>

#include <hkutilities2/visualdebugger/hkColor.h>

int hkRigidBodyCentreOfMassViewer::m_tag = 0;

void HK_CALL hkRigidBodyCentreOfMassViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "CentreOfMass", create );
}


hkRigidBodyCentreOfMassViewer::hkRigidBodyCentreOfMassViewer( hkWorld* world, hkDisplayHandler* displayHandler )
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

void hkRigidBodyCentreOfMassViewer::entityAddedCallback( hkEntity* entity )
{
//	if(entity->getType() == HK_RIGID_BODY)
//	{
		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entity);

		m_entitiesCreated.pushBack(rigidBody);
//	}
}

void hkRigidBodyCentreOfMassViewer::entityRemovedCallback( hkEntity* entity )
{
//	if( entity->getType() == HK_RIGID_BODY )
//	{
		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entity);

		// remove the id from the list of 'owned' created entities
		const int index = m_entitiesCreated.indexOf(rigidBody);
		HK_ASSERT2(index != -1, "Trying to remove body which hkRigidBodyCentreOfMassViewer does not think has been added!");
		if(index >= 0)
		{
			m_entitiesCreated.removeAt(index);
		}
//	}

}



void hkRigidBodyCentreOfMassViewer::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{
	for(int i = 0; i < m_entitiesCreated.getSize(); i++)
	{
		const hkReal mass = m_entitiesCreated[i]->getMass();
		if(mass != 0.0f)
		{
			hkVector4 centreOfMass = m_entitiesCreated[i]->getCenterOfMassInWorld();

			hkVector4 xAxis, yAxis, zAxis;

			xAxis.setAdd4(centreOfMass, m_entitiesCreated[i]->getTransform().getRotation().getColumn(0));
			yAxis.setAdd4(centreOfMass, m_entitiesCreated[i]->getTransform().getRotation().getColumn(1));
			zAxis.setAdd4(centreOfMass, m_entitiesCreated[i]->getTransform().getRotation().getColumn(2));
	
			m_displayHandler->displayLine(centreOfMass, xAxis, hkColor::RED, m_tag);
			m_displayHandler->displayLine(centreOfMass, yAxis, hkColor::GREEN, m_tag);
			m_displayHandler->displayLine(centreOfMass, zAxis, hkColor::BLUE, m_tag);
		}

	}

}


void hkRigidBodyCentreOfMassViewer::worldDeletedCallback( hkWorld* world )
{
	world->removeWorldDeletionListener( this );
	world->removeEntityListener( this );
	world->removeWorldPostSimulationListener( this );

	m_world = HK_NULL;
	this->removeReference();
}

hkRigidBodyCentreOfMassViewer::~hkRigidBodyCentreOfMassViewer()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeEntityListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}

	m_displayHandler->removeReference();
}


hkReferencedObject* HK_CALL hkRigidBodyCentreOfMassViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkRigidBodyCentreOfMassViewer(world, displayHandler);
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
