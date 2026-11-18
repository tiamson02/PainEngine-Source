/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkdemoframework2/hkDemoFramework.h>
#include <hkdemoframework2/hkDefaultGame.h>
#include <hkdemoframework2/hkTextDisplay.h>
#include <hkutilities2/mousespring/hkMouseSpringAction.h>

#include <hkdynamics2/hkDynamics2.h>
#include <hkdynamics2/world/hkWorld.h>
#include <hkbase/stream/hkStreambufFactory.h>
#include <hkbase/debugutil/hkMonitorBank.h>
#include <hkbase/stopwatch/hkStopwatch.h>
#include <hkbase/stream/hkStringStream.h>

#include <hkgraphics/hkGraphics.h>
#include <hkgraphics/hkgPad.h>
#include <hkgraphics/hkgWindow.h>
#include <hkgraphics/hkgCamera.h>
#include <hkgraphics/hkgDisplayObject.h>
#include <hkgraphics/hkgDisplayWorld.h>

#include <hkgraphicsbridge2/hkgDisplayHandler.h>
#include <hkutilities2/visualdebugger/viewer/hkBroadphaseViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkConstraintViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkContactPointViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkDebugDisplayViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkRigidBodyCentreOfMassViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkRigidBodyInertiaViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkShapeDisplayViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkSimulationIslandViewer.h>
#include <hkutilities2/visualdebugger/viewer/hkStatisticsViewer.h>

#include <hkutilities2/visualdebugger/hkDisplayCallbackUtil.h>
#include <hkutilities2/visualdebugger/hkVisualDebugger.h>

#include <hkdynamics2/entity/hkRigidBody.h>


hkDefaultGame::hkDefaultGame(hkGameEnvironment* env)
	:	m_env(env),
		m_world(HK_NULL),
		m_timestep(0.016f),
		// m_error
		m_mouseActive(false),
		m_mouseSpring(HK_NULL),
		m_mouseSpringMaxRelativeForce(1000.0f),
		m_mousePosDepth(0),
		m_path(env->m_path),
		// lastProgress
		// progressname
		m_lastProgress( HK_NULL )
{
	if(env->m_parameters != HK_NULL)
	{
		m_parameters = *env->m_parameters;
	}
}

hkDefaultGame::~hkDefaultGame()
{
	if(m_mouseActive)
	{
		mouseUp();
	}

	if(m_world)
	{
		m_world->removeReference();
	}
	if ( m_lastProgress )
	{
		delete m_lastProgress;
	}
}

hkGame::Result hkDefaultGame::stepGame()
{
	if(m_world)
	{
		m_world->stepDeltaTime(m_timestep);
	}
	return GAME_OK;
}

void hkDefaultGame::setupGraphics(hkWorld* world)
{
	if(world==HK_NULL)
	{
		world = m_world;
	}

	if(world)
	{
		// Add viewers to the demo display.

		// Uncomment these to use them.
	//	hkBroadphaseViewer::create(world, m_env->m_displayHandler);
	//	hkConstraintViewer::create(world, m_env->m_displayHandler);
	//	hkContactPointViewer::create(world, m_env->m_displayHandler);
	//	hkRigidBodyCentreOfMassViewer::create(world, m_env->m_displayHandler);
	//	hkRigidBodyInertiaViewer::create(world, m_env->m_displayHandler);
	//	hkSimulationIslandViewer::create(world, m_env->m_displayHandler);
	//	hkStatisticsViewer::create(world, m_env->m_displayHandler);

		// These are the two "default" viewers which display shapes
		// and debug lines.
		hkShapeDisplayViewer::create(world, m_env->m_displayHandler);
		hkDebugDisplayViewer::create(world, m_env->m_displayHandler);

		// Visual Debugger
		setupVisualDebugger(world, m_env->m_options->m_debugger);
	}
}

void hkDefaultGame::setupDefaultCameras(
			hkGameEnvironment* env, const hkVector4& from, const hkVector4& to, const hkVector4& up, const hkReal farPlane)
{
	hkgWindow* w = env->m_window;
	for(int i = 0; i < w->getNumViewports(); ++i)
	{
		hkgCamera* c = w->getViewport(i)->getCamera();

			// set up camera
		c->setFrom(&from(0));
		c->setTo(&to(0));
		c->setUp(&up(0));
		c->setFar(farPlane);
		c->setNear(farPlane / 2000.0f);
		c->orthogonalize();
		c->computeModelView();
		c->computeProjection();
	}
}

class ProgressStreambuf : public hkStreambuf
{
	public:

		ProgressStreambuf( hkStreambuf* s, hkDefaultGame* g, const char* fname)
			: m_child(s), m_game(g)
		{
			//child already has 1 ref // m_child->addReference();
			m_game->progressBegin(fname);
			m_fileSize = m_child->getFileSize();
			m_bytesRead = 0;
		}

		~ProgressStreambuf()
		{
			m_game->progressEnd();
			m_child->removeReference();
		}

		virtual int sysWrite(const char* buf, int nbytes) { HK_ASSERT(0);	return 0; }

		virtual int sysRead(char* buf, int nbytes)
		{
			m_bytesRead += nbytes;
			m_game->progressSet(hkReal(m_bytesRead) / m_fileSize);
			return m_child->read(buf, nbytes);
		}

		virtual void sysFlush() { HK_ASSERT(0);	}
		//{			m_child->flush();		}

		virtual hkResult sysSeek(long offset, SeekWhence whence) { HK_ASSERT(0);	return HK_FAILURE; }
		//{		return m_child->seek(offset, whence);		}

		virtual int sysTell() { HK_ASSERT(0); return 0;	}
		//{			return m_child->tell();		}

		virtual int sysGetFileSize()
		{
			return m_child->getFileSize();
		}

		int m_fileSize;
		int m_bytesRead;
		hkStreambuf* m_child;
		hkDefaultGame* m_game;
};

void hkDefaultGame::progressBegin(const char* name)
{
	if ( ! m_lastProgress )
	{
		m_lastProgress = new hkStopwatch();
	}
	m_lastProgress->start();
	m_progressName = name;
}

void hkDefaultGame::progressEnd()
{
	m_lastProgress->stop();
	delete m_lastProgress;
	m_lastProgress = HK_NULL;
}

void hkDefaultGame::progressSet(hkReal percent)
{
	if( m_lastProgress->getElapsedSeconds() >= (1.0f / 60.f) )
	{
		hkString bar = m_progressName;
		{
			const int NDOTS = 50;
			char spaces[NDOTS+4];
			spaces[0] = '\n';
			spaces[1] = '[';
			hkString::memSet(spaces+2, ' ', NDOTS);
			spaces[NDOTS+2] = ']';
			spaces[NDOTS+3] = 0;
			int end = (percent < 1) ? int(percent*NDOTS) : NDOTS;
			for(int i = 0; i < end; ++i)
			{
				spaces[2+i] = '#';
			}
			bar += spaces;
		}

		hkgWindow* window = m_env->m_window;
		window->clearBuffers();

		for(int i = 0; i < window->getNumViewports(); ++i )
		{
			window->getViewport(i)->setAsCurrent( window->getContext() );
			m_env->m_textDisplay->outputText(bar.cString(), 20, 20);
			m_env->m_textDisplay->displayText(window);
		}
		window->swapBuffers();

		m_lastProgress->reset();
		m_lastProgress->start();
	}
}



hkStreambuf* hkDefaultGame::openStream(const char* name, hkStreambuf::OpenMode mode, bool wantprogress)
{
	hkString path = m_path;
	path += "/Resources/";
	path += name;
	hkStreambuf* sb = hkStreambufFactory::getInstance().open(path.cString(), mode);
	if( sb->isOk() )
	{
		int fsize = sb->getFileSize();
		if( wantprogress && mode == hkStreambuf::INPUT)
		{
			return new ProgressStreambuf(sb, this, name);
		}
		return sb;
	}
	else
	{
		m_error = "Couldn't open `";
		m_error += name;
		m_error += "'";
		sb->removeReference();
		return HK_NULL;
	}
}



void hkDefaultGame::mouseDown()
{
	HK_ASSERT(m_mouseActive == false);
	// see if the user is picking - only if
	//   there is a display world!
	//   and we're not already picking
	hkgDisplayWorld* dw = m_env->m_displayWorld;
	if( dw )
	{
		hkgWindow* w = m_env->m_window;
		hkgViewport* v = w->getCurrentViewport();
		hkgCamera* c = v->getCamera();
		int vx, vy;
		v->getLowerLeftCoord(vx, vy);

		hkgViewportPickData pd;

		const int x = w->getMouse().getPosX() - vx;
		const int y = w->getMouse().getPosY() - vy;

		if (v->pick( x, y, dw, pd ))
		{
   			const int objectNum = pd.m_objectIndex;
			const hkVector4 mousePosWorldSpace( pd.m_worldPos[0], pd.m_worldPos[1], pd.m_worldPos[2]);

			float result[3];
			c->project( pd.m_worldPos[0], pd.m_worldPos[1], pd.m_worldPos[2], v->getWidth(), v->getHeight(), result );
			m_mousePosDepth = result[2];	// remember the z value

			//hkprintf( "GM: Try pick at [%d %d (%f)] ", x, y, m_mousePosDepth );
			//hkprintf( "World mouse pos: [%f %f %f]\n", pd.m_worldPos[0], pd.m_worldPos[1], pd.m_worldPos[2]);

			// find out if there is a (the first one) game that has a rigid body attached
			// to this display object
			const hkgDisplayObject* dobject = dw->getDisplayObject( objectNum );
			m_mouseActive = objectPicked( dobject, mousePosWorldSpace );
		}
		else
		{
			m_mouseActive = objectPicked( HK_NULL, hkVector4::getZero() );
		}
	}
}

void hkDefaultGame::mouseUp()
{
	if(m_mouseActive)
	{
		objectReleased();
		m_mouseActive = false;
	}
}

void hkDefaultGame::mouseDrag()
{
	if(m_mouseActive == false)
		return;

	// find the new world space point
	hkgWindow* w = m_env->m_window;
	hkgViewport* v = w->getCurrentViewport();
	hkgCamera* c = v->getCamera();

	int vx, vy;
	v->getLowerLeftCoord(vx, vy);

	const int x = w->getMouse().getPosX() - vx;
	const int y = w->getMouse().getPosY() - vy;

	hkVector4 mousePosWorldSpace;	mousePosWorldSpace.setZero4();

	c->unProject( (x - vx), (y - vy), m_mousePosDepth, v->getWidth(), v->getHeight(), &mousePosWorldSpace(0) );

	objectDragged( mousePosWorldSpace );
}




bool hkDefaultGame::objectPicked( const hkgDisplayObject* displayObject, const hkVector4& worldPosition )
{
	HK_ASSERT(m_env->m_displayHandler);

	hkgDisplayHandler* dhandler = m_env->m_displayHandler;

	if( displayObject )
	{
		const hkgDisplayHandler::IdDisplayObjectLink* doLink = dhandler->getDisplayObjectId( displayObject );
		// HACK!  We know the id is actually the address of the Collidable

		// Check if doLink->m_id is a valid address.
		if ( (doLink != HK_NULL) && (static_cast<hkUint32>(doLink->m_id) % 4 == 0) )
		{
			hkCollidable* col = reinterpret_cast<hkCollidable*>(doLink->m_id);
 			if( col->getOwner() )
			{
				hkRigidBody* rb = reinterpret_cast<hkRigidBody*>(col->getOwner());

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

void hkDefaultGame::objectDragged( const hkVector4& newWorldSpacePoint )
{
	// has mousespring been deleted when entity was deleted?
	if( m_mouseSpring != HK_NULL )
	{
		m_mouseSpring->setMousePosition( newWorldSpacePoint );
	}

}


void hkDefaultGame::objectReleased()
{
	if( m_mouseSpring != HK_NULL)
	{
		if ( m_mouseSpring->getOwner() )
		{
			m_world->removeAction( m_mouseSpring );
			m_mouseSpring->getRigidBody()->forceActivate();
		}
		m_mouseSpring->removeReference();
		m_mouseSpring = HK_NULL;
	}
}


//
// Visual Debugger Utility Functions
//

/// Call this function to setup the visualdebugger
/// The default is to run a network server
hkVisualDebugger* HK_CALL setupVisualDebugger(hkWorld* world, bool runServer, const char* captureFile)
{
	// Create it
	hkVisualDebugger* vdb = HK_NULL;
	if(runServer || captureFile)
	{
		vdb = hkVisualDebugger::create(world);
	}

	// Add/Remove viewers from the default list
	//vdb->addViewer("Islands");
	//vdb->addViewer("Broadphase");
	//vdb->addViewer("Statistics");
	//vdb->addViewer("Tensor");
	//vdb->addViewer("CentreOfMass");
	//vdb->addViewer("ContactPoints");

	// Start the server if it has been requested
	if(runServer)
	{
		vdb->serve();
	}

	// Capture the simulation information to a file
	if(captureFile)
	{
		vdb->capture(captureFile);
	}

	return vdb;
}

/// Steps all clients of all  instances of the visual debugger.
/// This function must be called in order for the display and
/// the visual debugger to function correctly
void HK_CALL stepVisualDebugger()
{
	hkVisualDebugger::step();
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
