/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkRigidBodyInertiaViewer.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/entity/hkRigidBody.h>

#include <hkutilities2/visualdebugger/hkDebugDisplay.h>
#include <hkutilities2/visualdebugger/viewer/hkCollideDebugUtil.h>

#include <hkutilities2/visualdebugger/hkColor.h>

static const int HK_DEFAULT_INERTIA_TENSOR_COLOR = hkColor::MAGENTA;

int hkRigidBodyInertiaViewer::m_tag = 0;

void HK_CALL hkRigidBodyInertiaViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "Tensor", create );
}


hkRigidBodyInertiaViewer::hkRigidBodyInertiaViewer( hkWorld* world, hkDisplayHandler* displayHandler )
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

void hkRigidBodyInertiaViewer::entityAddedCallback( hkEntity* entity )
{
//	if(entity->getType() == HK_RIGID_BODY)
//	{
		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entity);

		m_entitiesCreated.pushBack(rigidBody);
//	}
}

void hkRigidBodyInertiaViewer::entityRemovedCallback( hkEntity* entity )
{
//	if( entity->getType() == HK_RIGID_BODY )
//	{
		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entity);

		// remove the id from the list of 'owned' created entities
		const int index = m_entitiesCreated.indexOf(rigidBody);
		HK_ASSERT2(index != -1, "Trying to remove body which hkRigidBodyInertiaViewer does not think has been added!");
		if(index >= 0)
		{
			m_entitiesCreated.removeAt(index);
		}
//	}

}



void hkRigidBodyInertiaViewer::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{

	if(m_entitiesCreated.getSize() > m_displayBoxes.getSize() )
	{
		m_displayBoxes.setSize( m_entitiesCreated.getSize() );
	}

	for(int i = 0; i < m_entitiesCreated.getSize(); i++)
	{
		const hkReal mass = m_entitiesCreated[i]->getMass();
		if(mass != 0.0f)
		{
			// Want the inertia tensor to be displayed as a "box". This requires "diagonalising" the IT,
			// but for the moment we assume the IT is already diagonalised...

			// Box IT has elements given by:
			/*	hkReal alpha = halfSize(0) * 2.0f;
				hkReal beta = halfSize(1) * 2.0f;
				hkReal gamma = halfSize(2) * 2.0f;

				hkReal ixx = 1.0f / 6.0f;
				hkReal iyy = 1.0f / 6.0f;
				hkReal izz = 1.0f / 6.0f;


				hkReal ixxP = ( (beta*beta) / 12 )
								+ (   (gamma*gamma) / 12 ) * mass;

				hkReal iyyP = ( (alpha*alpha) / 12)
								+ ( (gamma*gamma) / 12 ) * mass;

				hkReal izzP = ( (alpha*alpha) / 12 )
								+ (  (beta*beta) / 12 ) * mass;

			*/
			// So ixxP/mass * 12 = (beta*beta) + (gamma*gamma);
			//    iyyP/mass * 12 = (alpha*alpha) + (gamma*gamma);
			//    izzP/mass * 12 = (alpha*alpha) + (beta*beta);

			// Therefore ixxP/mass * 12 - iyyP/mass * 12 + izzP/mass * 12 = 2 (beta*beta) etc.

			hkMatrix3 m;
			m_entitiesCreated[i]->getInertiaLocal(m);

			hkReal betaSqrd = (m(0,0) - m(1,1) + m(2,2))/mass * 6;
			hkReal beta = ((betaSqrd < 0.0f) ? 0.0f : hkMath::sqrt( betaSqrd ));	// Safety check for zeroed elements!
			hkReal gammaSqrd = m(0,0) / mass * 12 - betaSqrd;
			hkReal gamma = ((gammaSqrd < 0.0f) ? 0.0f : hkMath::sqrt( gammaSqrd ));	// Safety check for zeroed elements!
			hkReal alphaSqrd = m(2,2) / mass * 12 - betaSqrd;
			hkReal alpha = ((alphaSqrd < 0.0f) ? 0.0f : hkMath::sqrt( alphaSqrd ));	// Safety check for zeroed elements!

			hkReal scale = 1.01f;

			hkVector4 halfExtents(scale * alpha * 0.5f, scale * beta * 0.5f, scale * gamma * 0.5f);
		
			// We need to construct the transform to the *centre* of the OBB/AABB, since the drawObbLines() method
			// uses a (halfExtents vector4) and (transform). 
			hkTransform t = m_entitiesCreated[i]->getTransform();
			
				// calc transform as t + (vector to centre of OBB/AABB in world space)
			hkVector4 centre = m_entitiesCreated[i]->getCenterOfMassLocal();			
			hkVector4 offset;
			offset.setRotatedDir(t.getRotation(), centre);
			offset.add4(t.getTranslation());
			t.setTranslation(offset);
		
			
			m_displayBoxes[i].setParameters(halfExtents, t);	
			
			//hkCollideDebugUtil::drawObbLines(t, halfExtents, HK_DEFAULT_INERTIA_TENSOR_COLOR, m_displayHandler);
		}
	}

	hkArray<hkDisplayGeometry*> geometries;
	geometries.setSize( m_displayBoxes.getSize() );

	for(int j = m_displayBoxes.getSize() - 1; j >= 0; j--)
	{
		geometries[j] = &m_displayBoxes[j];	
	}
	
	m_displayHandler->displayGeometry(geometries, HK_DEFAULT_INERTIA_TENSOR_COLOR, m_tag);
}


void hkRigidBodyInertiaViewer::worldDeletedCallback( hkWorld* world )
{
	world->removeWorldDeletionListener( this );
	world->removeEntityListener( this );
	world->removeWorldPostSimulationListener( this );

	m_world = HK_NULL;
	this->removeReference();
}

hkRigidBodyInertiaViewer::~hkRigidBodyInertiaViewer()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeEntityListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}

	m_displayHandler->removeReference();
}


hkReferencedObject* HK_CALL hkRigidBodyInertiaViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkRigidBodyInertiaViewer(world, displayHandler);
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
