/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/viewer/hkConstraintViewer.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>

#include <hkdynamics2/world/hkSimulationIsland.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkdynamics2/entity/hkRigidBody.h>

#include <hkutilities2/visualdebugger/hkDebugDisplay.h>
#include <hkutilities2/visualdebugger/hkColor.h>
#include <hkdynamics2/constraint/hkConstraint.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkHingeDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkLimitedHingeDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkRagdollDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPrismaticDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkWheelDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkStiffSpringDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkBallSocketDrawer.h>
#include <hkutilities2/visualdebugger/viewer/constraint/hkPointToPathDrawer.h>

int hkConstraintViewer::m_tag = 0;

void HK_CALL hkConstraintViewer::registerViewer()
{
	m_tag = hkViewerFactory::getInstance().registerViewer( "Constraint", create );
}


hkConstraintViewer::hkConstraintViewer( hkWorld* world, hkDisplayHandler* displayHandler )
	: m_displayHandler(displayHandler)
{
	m_world = world;  // when this was in the initializer list it crashed code warrior on ps2

	m_displayHandler->addReference();
	world->addWorldDeletionListener( this );
	world->addConstraintListener( this );
	world->addWorldPostSimulationListener( this );
}

void hkConstraintViewer::constraintAddedCallback( hkConstraint* constraint )
{

}

void hkConstraintViewer::constraintRemovedCallback( hkConstraint* constraint )
{
	int index = m_selectedConstraints.indexOf(constraint);
	
	if(index >= 0)
	{
		m_selectedConstraints.removeAt(index);
	}
}


void hkConstraintViewer::addConstraint(hkConstraint* constraint)
{
	m_selectedConstraints.pushBack(constraint);
	constraint->addReference();
}

void hkConstraintViewer::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{

	if( m_selectedConstraints.getSize() )
	{
		for(int j = 0; j < m_selectedConstraints.getSize(); j++)
		{
			draw(m_selectedConstraints[j], m_displayHandler);
		}
	}
	else
	{
		const hkArray<hkSimulationIsland*>& activeIslands = m_world->getActiveSimulationIslands();
		{
			for ( int i = 0; i < activeIslands.getSize(); ++i )
			{
				const hkArray<hkConstraint*>& constraints = activeIslands[i]->getConstraints();		
				
				for(int j = 0; j < constraints.getSize(); j++)
				{
					draw(constraints[j], m_displayHandler);
				}
			}
		}

		const hkArray<hkSimulationIsland*>& inactiveIslands = m_world->getInactiveSimulationIslands();
		{
			for ( int i = 0; i < inactiveIslands.getSize(); ++i )
			{
				const hkArray<hkConstraint*>& constraints = inactiveIslands[i]->getConstraints();		
	    		
				for(int j = 0; j < constraints.getSize(); j++)
				{
					draw(constraints[j], m_displayHandler);
				}
			}
		}
	}
}


void hkConstraintViewer::draw(hkConstraint* constraint, hkDisplayHandler* displayHandler)
{
	int type = constraint->getType();

	HK_ASSERT2(displayHandler,"displayHandler is NULL");

	switch(type)
	{
		case HK_CONSTRAINT_TYPE_BALLANDSOCKET:
		{
			hkBallSocketDrawer ballSocketDrawer;
			ballSocketDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

		case HK_CONSTRAINT_TYPE_HINGE:
		{
			hkHingeDrawer hingeDrawer;
			hingeDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

		case HK_CONSTRAINT_TYPE_LIMITEDHINGE:
		{
			hkLimitedHingeDrawer limitedHingeDrawer;
			limitedHingeDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

		case HK_CONSTRAINT_TYPE_PRISMATIC:
		{
			hkPrismaticDrawer prismaticDrawer;
			prismaticDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

		case HK_CONSTRAINT_TYPE_STIFFSPRING:
		{
			hkStiffSpringDrawer stiffSpringDrawer;
			stiffSpringDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

		case HK_CONSTRAINT_TYPE_WHEEL:
		{
			hkWheelDrawer wheelDrawer;
			wheelDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

		case HK_CONSTRAINT_TYPE_POWEREDHINGE:
		{
			hkLimitedHingeDrawer poweredHingeDrawer;
			poweredHingeDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

		case HK_CONSTRAINT_TYPE_POINTTOPATH:
		{
			hkPointToPathDrawer pToPDrawer;
			pToPDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;


		case HK_CONSTRAINT_TYPE_RAGDOLL:
		case HK_CONSTRAINT_TYPE_POWEREDRAGDOLL:
		{
			hkRagdollDrawer ragdollDrawer;
			ragdollDrawer.drawConstraint(constraint, displayHandler, m_tag);
		}
		break;

	}

}

void hkConstraintViewer::worldDeletedCallback( hkWorld* world )
{
	world->removeWorldDeletionListener( this );
	world->removeConstraintListener( this );
	world->removeWorldPostSimulationListener( this );

	m_world = HK_NULL;
	this->removeReference();
}


hkConstraintViewer::~hkConstraintViewer()
{
	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeConstraintListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}
	
	for(int j = 0; j < m_selectedConstraints.getSize(); j++)
	{
		m_selectedConstraints[j]->removeReference();
	}

	m_displayHandler->removeReference();
}


hkReferencedObject* HK_CALL hkConstraintViewer::create(hkWorld* world, hkDisplayHandler* displayHandler)
{
	return new hkConstraintViewer(world, displayHandler);
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
