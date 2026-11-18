/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/simulation/hkHalfSteppingUtility.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/entity/hkRigidBody.h>

hkHalfSteppingUtility::hkHalfSteppingUtility( hkWorld* world )
{
	m_integrate = true;
	m_world = world;

	const hkArray<hkSimulationIsland*>& activeIslands = world->getActiveSimulationIslands();

	for( int i = 0; i < activeIslands.getSize(); i++ )
	{
		const hkArray<hkEntity*>& activeEntities = activeIslands[i]->getEntities();

		int x = m_activeEntities.getSize();
		m_activeEntities.setSize( x + activeEntities.getSize() );

		for(int j = 0; j < activeEntities.getSize(); j++)
		{
			hkRigidBody* rigidBody = static_cast<hkRigidBody*>(activeEntities[j]);
			
			// add property to rigid body which allows the half stepper to find it's details quickly
			hkPropertyValue p; p.asInt = x;
			rigidBody->addProperty(HK_PROPERTY_HALF_STEPPER_INDEX, p);

			// store position and orientation information for rigid body
			m_activeEntities[x].m_rigidBody = rigidBody;
			m_activeEntities[x].m_position = rigidBody->getPosition();
			m_activeEntities[x].m_rotation = rigidBody->getRotation();

			x++;
		}
	}

	m_world->addIslandActivationListener( this );
	m_world->addWorldDeletionListener( this );
	m_world->addEntityListener( this );
}

hkHalfSteppingUtility::~hkHalfSteppingUtility()
{
	if(m_world)
	{
		m_world->removeIslandActivationListener( this );
		m_world->removeWorldDeletionListener( this );
		m_world->removeEntityListener( this );
	}
}



void hkHalfSteppingUtility::syncRigidBody( hkRigidBody* rb )
{
	// only sync if the the rigid body is active else it won't be known to the halfstepper
	if( rb->isActive() )
	{
		HK_ASSERT2(rb->hasProperty(HK_PROPERTY_HALF_STEPPER_INDEX), "Trying to sync an active hkRigidBody that is not known to the half stepper! The half stepper should know about all active hkEntities in the hkWorld.");
		const int x = rb->getProperty(HK_PROPERTY_HALF_STEPPER_INDEX).asInt;

		m_activeEntities[x].m_position = rb->getPosition();
		m_activeEntities[x].m_rotation = rb->getRotation();
	}
}



void hkHalfSteppingUtility::stepDeltaTime( hkReal deltaTime )
{
	hkStepInfo stepInfo;
	stepInfo.m_deltaTime = deltaTime * 2.0f;
	stepInfo.m_invDeltaTime = 1.0f / stepInfo.m_deltaTime;

	if( m_integrate )
	{
#ifdef HK_DEBUG
		// verify rigid body positions are the same as the half stepper positions
		{
			const hkArray<hkSimulationIsland*>& activeIslands = m_world->getActiveSimulationIslands();

			// interpolate all active entities
			for( int i = 0; i < activeIslands.getSize(); i++ )
			{
				const hkArray<hkEntity*>& activeEntities = activeIslands[i]->getEntities();
				for(int j = 0; j < activeEntities.getSize(); j++)
				{
					const hkRigidBody* entity = static_cast<hkRigidBody*>(activeEntities[j]);
					HK_ASSERT2(entity->hasProperty(HK_PROPERTY_HALF_STEPPER_INDEX), "Half stepper should know about all active hkEntities in the hkWorld.");
					const int x = entity->getProperty(HK_PROPERTY_HALF_STEPPER_INDEX).asInt;
					
					HK_ASSERT2(m_activeEntities[x].m_position.equals4(entity->getPosition()), "Half stepper position not in sync.  Make sure to call syncRigidBody if calling hkRigidBody::setPosition etc..." );
					HK_ASSERT2(m_activeEntities[x].m_rotation.m_vec.equals4(entity->getRotation().m_vec), "Half stepper position not in sync.  Make sure to call syncRigidBody if calling hkRigidBody::setPosition etc..." );
				}
			}
		}
#endif

		m_world->integrate( stepInfo );
		interpolateHalf();
	}
	else
	{
		updateToStoredPositions();
		m_world->collide( stepInfo );
	}
	m_integrate = !m_integrate;
}

void hkHalfSteppingUtility::interpolateHalf()
{
	const hkArray<hkSimulationIsland*>& activeIslands = m_world->getActiveSimulationIslands();

	// interpolate all active entities
	for( int i = 0; i < activeIslands.getSize(); i++ )
	{
		const hkArray<hkEntity*>& activeEntities = activeIslands[i]->getEntities();
		for(int j = 0; j < activeEntities.getSize(); j++)
		{
			hkRigidBody* entity = static_cast<hkRigidBody*>(activeEntities[j]);
			HK_ASSERT2(entity->hasProperty(HK_PROPERTY_HALF_STEPPER_INDEX), "Half stepper should know about all active hkEntities in the hkWorld.");
			const int x = entity->getProperty(HK_PROPERTY_HALF_STEPPER_INDEX).asInt;

			// interpolate the entity
			HK_ASSERT2(entity->isActive(), "The half stepper should not be trying to step an hkRigidBody that is not active!");

			{
				hkVector4 diff;
				diff.setSub4(entity->getPosition(), m_activeEntities[x].m_position);
				diff.mul4(0.5f);
				m_activeEntities[x].m_position = entity->getPosition();
				hkVector4 newPos; newPos.setSub4( entity->getPosition(), diff);
				entity->setPosition(newPos);
			}

			{
				hkQuaternion diff;
				diff.m_vec.setSub4(entity->getRotation().m_vec, m_activeEntities[x].m_rotation.m_vec);
				diff.m_vec.mul4(0.5f);
				m_activeEntities[x].m_rotation = entity->getRotation();
				hkQuaternion newRot; newRot.m_vec.setSub4(entity->getRotation().m_vec, diff.m_vec);
				newRot.m_vec.normalize4();
				entity->setRotation(newRot);
			}
		}
	}
}

void hkHalfSteppingUtility::updateToStoredPositions()
{
	const hkArray<hkSimulationIsland*>& activeIslands = m_world->getActiveSimulationIslands();

	// update positions of all active entities
	for( int i = 0; i < activeIslands.getSize(); i++ )
	{
		const hkArray<hkEntity*>& activeEntities = activeIslands[i]->getEntities();
		for(int j = 0; j < activeEntities.getSize(); j++)
		{
			hkRigidBody* entity = static_cast<hkRigidBody*>(activeEntities[j]);
			HK_ASSERT2(entity->hasProperty(HK_PROPERTY_HALF_STEPPER_INDEX), "Half stepper should know about all active hkEntities in the hkWorld.");
			const int x = entity->getProperty(HK_PROPERTY_HALF_STEPPER_INDEX).asInt;

			entity->setPosition(m_activeEntities[x].m_position);
			entity->setRotation(m_activeEntities[x].m_rotation);
		}
	}
}


void hkHalfSteppingUtility::islandActivatedCallback( hkSimulationIsland* island )
{
	const hkArray<hkEntity*>& entities = island->getEntities();
	int x = m_activeEntities.getSize();
	m_activeEntities.setSize( x + entities.getSize() );

	for(int i = 0; i < entities.getSize(); ++i)
	{
		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entities[i]);
		// add property to rigid body which allows the half stepper to find it's details quickly
		hkPropertyValue p; p.asInt = x;
		rigidBody->addProperty(HK_PROPERTY_HALF_STEPPER_INDEX, p);

		// store position and orientation information for rigid body
		m_activeEntities[x].m_rigidBody = rigidBody;
		m_activeEntities[x].m_position = rigidBody->getPosition();
		m_activeEntities[x].m_rotation = rigidBody->getRotation();

		x++;
	}
}

void hkHalfSteppingUtility::islandDeactivatedCallback( hkSimulationIsland* island )
{
	const hkArray<hkEntity*>& entities = island->getEntities();

	for(int i = 0; i < entities.getSize(); i++)
	{
		// body to remove
		hkRigidBody* entity = static_cast<hkRigidBody*>(entities[i]);
		HK_ASSERT2(entity->hasProperty(HK_PROPERTY_HALF_STEPPER_INDEX), "The half stepper should know about all active hkEntities in the hkWorld.");
		const int x = entity->getProperty(HK_PROPERTY_HALF_STEPPER_INDEX).asInt;

		// last body (in half stepper array)
		const int z = m_activeEntities.getSize() - 1;
		hkRigidBody* lastEntity = m_activeEntities[z].m_rigidBody;

		// swap last body into position of body to be removed...
		{
			// add property to rigid body which allows the half stepper to find it's details quickly
			hkPropertyValue p; p.asInt = x;
			lastEntity->addProperty(HK_PROPERTY_HALF_STEPPER_INDEX, p);

			// store position and orientation information for rigid body
			m_activeEntities[x].m_rigidBody = lastEntity;
			m_activeEntities[x].m_position = m_activeEntities[z].m_position;
			m_activeEntities[x].m_rotation = m_activeEntities[z].m_rotation;
		}

		// remove property from the entity
		entity->removeProperty(HK_PROPERTY_HALF_STEPPER_INDEX);

		m_activeEntities.setSize(z);
	}
}

void hkHalfSteppingUtility::worldDeletedCallback( hkWorld* world )
{
	m_world->removeIslandActivationListener( this );
	m_world->removeWorldDeletionListener( this );
	m_world->removeEntityListener( this );
	m_world = HK_NULL;
}

void hkHalfSteppingUtility::entityAddedCallback( hkEntity* entity )
{
	// entities are active when added, so add the entity to our list

	if( entity->isActive() )
	{
		const int x = m_activeEntities.getSize();
		m_activeEntities.setSize(x + 1);

		hkRigidBody* rigidBody = static_cast<hkRigidBody*>(entity);

		// swap last body into position of body to be removed...
		{
			// add property to rigid body which allows the half stepper to find it's details quickly
			hkPropertyValue p; p.asInt = x;
			rigidBody->addProperty(HK_PROPERTY_HALF_STEPPER_INDEX, p);

			// store position and orientation information for rigid body
			m_activeEntities[x].m_rigidBody = rigidBody;
			m_activeEntities[x].m_position = rigidBody->getPosition();
			m_activeEntities[x].m_rotation = rigidBody->getRotation();
		}
	}

}

void hkHalfSteppingUtility::entityRemovedCallback( hkEntity* entity )
{
	if( entity->isActive() )
	{
		// body to remove
		HK_ASSERT2(entity->hasProperty(HK_PROPERTY_HALF_STEPPER_INDEX), "The half stepper should know about all active hkEntities in the hkWorld.");
		const int x = entity->getProperty(HK_PROPERTY_HALF_STEPPER_INDEX).asInt;

		// last body (in half stepper array)
		const int z = m_activeEntities.getSize() - 1;
		hkRigidBody* lastEntity = m_activeEntities[z].m_rigidBody;

		// swap last body into position of body to be removed...
		{
			// add property to rigid body which allows the half stepper to find it's details quickly
			hkPropertyValue p; p.asInt = x;
			lastEntity->addProperty(HK_PROPERTY_HALF_STEPPER_INDEX, p);

			// store position and orientation information for rigid body
			m_activeEntities[x].m_rigidBody = lastEntity;
			m_activeEntities[x].m_position = m_activeEntities[z].m_position;
			m_activeEntities[x].m_rotation = m_activeEntities[z].m_rotation;
		}

		// remove property from the entity
		entity->removeProperty(HK_PROPERTY_HALF_STEPPER_INDEX);

		m_activeEntities.setSize(z);
	}
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
