/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/lazyadd/hkLazyAddToWorld.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/entity/hkEntity.h>
#include <hkdynamics2/action/hkAction.h>
#include <hkdynamics2/constraint/hkConstraint.h>

hkLazyAddToWorld::hkLazyAddToWorld(hkWorld* world)
:	m_world(world)
{
	m_world->addReference();
}

hkLazyAddToWorld::~hkLazyAddToWorld()
{
	m_world->removeReference();
	
	HK_ASSERT2(m_entities.getSize() == 0, "Cannot delete hkLazyAddToWorld before committing all lazy hkEntities.");
	HK_ASSERT2(m_actions.getSize() == 0, "Cannot delete hkLazyAddToWorld before committing all lazy hkActions.");
	HK_ASSERT2(m_constraints.getSize() == 0, "Cannot delete hkLazyAddToWorld before committing all lazy hkConstraints.");
}
		
int hkLazyAddToWorld::commitAll()
{
	int totalCommitted = 0;

	// Commit lazy hkEntities.
	int numCommitted = commitAllEntity();
	if(numCommitted == -1)
	{
		return -1;
	}
	totalCommitted += numCommitted;

	// Commit lazy hkActions.
	numCommitted = commitAllAction();
	if(numCommitted == -1)
	{
		return -1;
	}
	totalCommitted += numCommitted;

	// Commit lazy hkConstraints.
	numCommitted = commitAllConstraint();
	if(numCommitted == -1)
	{
		return -1;
	}
	totalCommitted += numCommitted;

	return totalCommitted;
}

int hkLazyAddToWorld::commitAllEntity()
{
	// Initialise to zero.
	int numCommitted = 0;

	//
	// Iterate through the m_entities hkArray, committing each hkEntity.
	//
	while (m_entities.getSize() > 0)
	{
		// Check if it's OK to add this hkAction to m_world.
		bool committOk = isValid(m_entities[0]);

		HK_ASSERT2(committOk, "Cannot commit invalid hkEntity.");

		if (committOk)
		{
			// Add entity to hkWorld.
			m_world->addEntity(m_entities[0]);

			//
			// Remove entity from wait arrays. 
			//
			m_entities[0]->removeReference();
			m_entities.removeAt(0);
			numCommitted++;
		}
		else
		{
			// If any hkEntitiess fail to commit, return failure value (-1) immediately.
			return -1;
		}
	}

	// Return the number of hkEntities committed.
	return numCommitted;
}

int hkLazyAddToWorld::commitAllAction()
{
	// Initialise to zero.
	int numCommitted = 0;

	//
	// Iterate through the m_actions hkArray, committing each hkAction.
	//
	while (m_actions.getSize() > 0)
	{
		// Check if it's OK to add this hkAction to m_world.
		bool committOk = isValid(m_actions[0]);

		HK_ASSERT2(committOk, "Cannot commit invalid hkAction.");

		if (committOk)
		{
			// Add action to hkWorld.
			m_world->addAction(m_actions[0]);

			m_actions[0]->removeReference();
			m_actions.removeAt(0);
			numCommitted++;
		}
		else
		{
			// If any hkActions fail to commit, return failure value (-1) immediately.
			return -1;
		}
	}

	// Return the number of hkActions committed.
	return numCommitted;
}

int hkLazyAddToWorld::commitAllConstraint()
{
	// Initialise to zero.
	int numCommitted = 0;

	//
	// Iterate through the m_constraints hkArray, committing each hkConstraint.
	//
	while (m_constraints.getSize() > 0)
	{
		// Check if it's OK to add this hkConstraint to m_world.
		bool committOk = isValid(m_constraints[0]);

		HK_ASSERT2(committOk, "Cannot commit invalid hkConstraint.");

		if (committOk)
		{
			// Add constraint to hkWorld.
			m_world->addConstraint(m_constraints[0]);

			m_constraints[0]->removeReference();
			m_constraints.removeAt(0);
			numCommitted++;
		}
		else
		{
			// If any hkConstraints fail to commit, return failure value (-1) immediately.
			return -1;
		}
	}

	// Return the number of hkActions committed.
	return numCommitted;
}
		
int hkLazyAddToWorld::addEntity(hkEntity* entity)
{
	bool addOk = entity->getOwner() == HK_NULL;

	HK_ASSERT2(addOk, "Cannot add an hkEntity that is already added to an hkWorld.");

	if (addOk)
	{
		entity->addReference();
		m_entities.pushBack(entity);
		return m_entities.getSize();
	}
	else
	{
		return -1;	
	}
}

int hkLazyAddToWorld::addAction(hkAction* action)
{
	bool addOk = action->getOwner() == HK_NULL;

	HK_ASSERT2(addOk, "Cannot add an hkAction that is already added to an hkWorld.");

	if (addOk)
	{
		action->addReference();
		m_actions.pushBack(action);
		return m_actions.getSize();
	}
	else
	{
		return -1;	
	}
}

int hkLazyAddToWorld::addConstraint( hkConstraint* constraint )
{
	bool addOk = constraint->getOwner() == HK_NULL;

	HK_ASSERT2(addOk, "Cannot add an hkConstraint that is already added to an hkWorld.");

	if (addOk)
	{
		constraint->addReference();
		m_constraints.pushBack(constraint);
		return m_constraints.getSize();
	}
	else
	{
		return -1;	
	}
}

bool hkLazyAddToWorld::isValid(hkEntity* entity)
{
	// If entity's hkCollidable has an hkShape it's valid.
	return ( entity->getCollidable().getShape() != HK_NULL );
}

bool hkLazyAddToWorld::isValid(hkAction* action)
{
	hkArray<hkEntity*> entities;

	action->getEntities(entities);

	//
	// If the action's entities are not HK_NULL it's valid.
	//
	for (int i = 0; i < entities.getSize(); ++i)
	{
		if (entities[i] == HK_NULL)
		{
			return false;	
		}
	}
	return true;
}

bool hkLazyAddToWorld::isValid(hkConstraint* constraint)
{
	// If constraint's entities are not HK_NULL, it is valid.
	return (constraint->getEntityA() != HK_NULL 
			&& constraint->getEntityB() != HK_NULL);
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
