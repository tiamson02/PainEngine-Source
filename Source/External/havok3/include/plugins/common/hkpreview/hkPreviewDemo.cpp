/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkpreview/hkPreviewDemo.h>
#include <hkdemoframework/hkTextDisplay.h>
#include <hkutilities/mousespring/hkMouseSpringAction.h>

#include <hkbase/stream/hkStreambufFactory.h>
#include <hkbase/stream/hkStreamReader.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkbase/class/hkClass.h>

#include <hkgraphics/hkGraphics.h>
#include <hkgraphics/hkgPad.h>
#include <hkgraphics/hkgWindow.h>
#include <hkgraphics/hkgCamera.h>
#include <hkgraphics/hkgDisplayObject.h>
#include <hkgraphics/hkgDisplayWorld.h>
#include <hkgraphics/hkgLightManager.h>

#include <hkgraphicsbridge/hkgDisplayHandler.h>

//#include <hkutilities/visualdebugger/viewer/hkBroadphaseViewer.h>
//#include <hkutilities/visualdebugger/viewer/hkConstraintViewer.h>
//#include <hkutilities/visualdebugger/viewer/hkContactPointViewer.h>
//#include <hkutilities/visualdebugger/viewer/hkRigidBodyCentreOfMassViewer.h>
//#include <hkutilities/visualdebugger/viewer/hkRigidBodyInertiaViewer.h>
//#include <hkutilities/visualdebugger/viewer/hkSimulationIslandViewer.h>

#include <hkutilities/visualdebugger/viewer/hkShapeDisplayViewer.h>
#include <hkutilities/visualdebugger/viewer/hkConvexRadiusViewer.h>
#include <hkutilities/visualdebugger/viewer/hkPhantomDisplayViewer.h>
#include <hkutilities/visualdebugger/hkPhysicsContext.h>

#include <hkvisualize/process/hkDebugDisplayProcess.h>

#include <hkvisualize/hkProcess.h>
#include <hkvisualize/hkProcessRegisterUtil.h>
#include <hkvisualize/hkVisualDebugger.h>
#include <hkvisualize/hkProcessFactory.h>


#include <hkdynamics/hkDynamics.h>
#include <hkdynamics/world/hkWorld.h>
#include <hkdynamics/entity/hkRigidBody.h>

extern const hkClass hkWorldCinfoClass;
extern const hkClass hkRigidBodyCinfoClass;

const char* DemoOrder[] = {""};

hkPreviewDemo::hkPreviewDemo(hkDemoEnvironment* env)
: hkDefaultDemo	(env),
m_mouseSpring(HK_NULL),
m_mouseSpringMaxRelativeForce(1000.0f),
m_world(HK_NULL),
m_physicsViewersContext(HK_NULL)
{
	// Disable warnings that commonly occur in the demos
	hkError::getInstance().setEnabled(0x6e8d163b, false); // hkMoppUtility.cpp:18
	hkError::getInstance().setEnabled(0x34df5494, false); //hkGeometryUtility.cpp:26

	m_physicsViewersContext = new hkPhysicsContext;
	hkPhysicsContext::registerAllPhysicsProcesses(); // all physics only ones
}

hkPreviewDemo::~hkPreviewDemo()
{
	if(m_world)
	{
		m_world->removeReference();
	}

	if (m_physicsViewersContext)
	{
		m_physicsViewersContext->removeReference();
		m_physicsViewersContext = HK_NULL;
	}
}

hkDemo::Result hkPreviewDemo::stepDemo()
{
	if(m_world)
	{
		m_world->stepDeltaTime(m_timestep);
	}
	return DEMO_OK;
}

void hkPreviewDemo::setupContexts(hkArray<hkProcessContext*>& contexts)
{
	if (m_world && (m_physicsViewersContext->findWorld(m_world) < 0) ) 	
		m_physicsViewersContext->addWorld(m_world);

	contexts.pushBack( m_physicsViewersContext );

	// Add viewers to the demo display.
	// Uncomment these to use them.
	//	m_debugViewerNames.pushBack( hkBroadphaseViewer::getName()  );
	//	m_debugViewerNames.pushBack( hkConstraintViewer::getName()  );
	//	m_debugViewerNames.pushBack( hkContactPointViewer::getName()  );
	//	m_debugViewerNames.pushBack( hkRigidBodyCentreOfMassViewer::getName()  );
	//	m_debugViewerNames.pushBack( hkRigidBodyInertiaViewer::getName()  );
	//	m_debugViewerNames.pushBack( hkSimulationIslandViewer::getName()  );
	//	m_debugViewerNames.pushBack( hkStatisticsProcess::getName()  );

	// These are the three "default" viewers which display shapes
	// phantoms and debug lines.    
	m_debugViewerNames.pushBack( hkShapeDisplayViewer::getName() );
	
	m_debugViewerNames.pushBack( hkPhantomDisplayViewer::getName() );
	
	m_debugViewerNames.pushBack( hkConvexRadiusViewer::getName() );

	m_debugViewerNames.pushBack( hkDebugDisplayProcess::getName() );

}

hkBool hkPreviewDemo::objectPicked( const hkgDisplayObject* displayObject, const hkVector4& worldPosition )
{
	HK_ASSERT(0x65b2643b, m_env->m_displayHandler);

	hkgDisplayHandler* dhandler = m_env->m_displayHandler;

	if( displayObject )
	{
		hkUlong id = dhandler->getDisplayObjectId( displayObject );
		// HACK!  We know the id is actually the address of the Collidable

		// Check if id is valid
		if ( id!=0  )
		{
			if ( (id & 0x3) == 0x3 )
				id &= ~0x3;
			else if ( (id % 4) != 0)
				return false;

			hkCollidable* col = reinterpret_cast<hkCollidable*>( id );
			if( col->getOwner() )
			{
				hkRigidBody* rb = hkGetRigidBody(col);

				if( rb && !rb->isFixed() )
				{
					hkVector4 positionAinA;
					positionAinA.setTransformedInversePos( rb->getTransform(), worldPosition );

					const hkReal springDamping = 0.5f;
					const hkReal springElasticity = 0.3f;
					const hkReal objectDamping = 0.95f;
					m_mouseSpring = new hkMouseSpringAction( positionAinA, worldPosition, springDamping, springElasticity, objectDamping, rb );
					m_mouseSpring->setMaxRelativeForce(m_mouseSpringMaxRelativeForce);
					m_world->addAction( m_mouseSpring );
					return true;
				}
			}
		}
	}
	return false;
}

void hkPreviewDemo::objectDragged( const hkVector4& newWorldSpacePoint )
{
	// has mousespring been deleted when entity was deleted?
	if( m_mouseSpring != HK_NULL )
	{
		m_mouseSpring->setMousePosition( newWorldSpacePoint );
	}
}

void hkPreviewDemo::objectReleased()
{
	if( m_mouseSpring != HK_NULL)
	{
		if ( m_mouseSpring->getWorld() )
		{
			m_world->removeAction( m_mouseSpring );
			m_mouseSpring->getEntity()->activate();
		}
		m_mouseSpring->removeReference();
		m_mouseSpring = HK_NULL;
	}
}

/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
