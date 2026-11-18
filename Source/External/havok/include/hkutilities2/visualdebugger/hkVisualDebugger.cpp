/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkSockServer.h>
#include <hkbase/stream/hkSockStreambuf.h>
#include <hkbase/stream/hkStreambufFactory.h>

#include <hkdynamics2/world/hkWorld.h>

#include <hkutilities2/visualdebugger/serialize/hkSerializingDisplayHandler.h>
#include <hkutilities2/visualdebugger/serialize/hkServerDeserializer.h>

#include <hkutilities2/visualdebugger/hkDisplayHandler.h>

#include <hkutilities2/visualdebugger/hkServerPluginHandler.h>
#include <hkutilities2/visualdebugger/hkMousePickingHandler.h>

#include <hkutilities2/visualdebugger/hkViewerFactory.h>

#include <hkutilities2/visualdebugger/hkViewerRegisterUtil.h>

#include <hkutilities2/visualdebugger/hkVisualDebugger.h>
#include <hkutilities2/visualdebugger/hkDisplayCallbackUtil.h>

#include <hkutilities2/visualdebugger/hkVersionHandler.h>

hkVisualDebugger* HK_CALL hkVisualDebugger::create(hkWorld* world, hkKeyboard* keyboard)
{
	return new hkVisualDebugger(world, keyboard);
}

void hkVisualDebugger::setKeyboard(hkKeyboard* keyboard)
{
	m_keyboard = keyboard;
}

hkVisualDebugger::hkVisualDebugger(hkWorld* world, hkKeyboard* keyboard)
	: m_world(world), m_server(HK_NULL), m_keyboard(keyboard), m_amTimingFrame(false), m_frameTimer("Frame Timer")
{
	HK_VISUAL_DEBUGGER_INFO(1, "Visual Debugger instance has been created");

	hkViewerRegisterUtil::registerAllViewers();

	world->addWorldDeletionListener(this);
	world->addWorldPostSimulationListener(this);
	addViewer("Shapes");
	addViewer("DebugDisplay");

	HK_ADD_STEP_DISPLAY_LISTENER(this);
}

void hkVisualDebugger::serve(int listenPort)
{
	if(!m_server)
	{
		m_server = hkSockServer::create(listenPort);
		if(m_server)
		{
			HK_VISUAL_DEBUGGER_INFO(1, "Server created and will poll for new client(s) on port " << listenPort << " every frame");
		}
		else
		{
			HK_VISUAL_DEBUGGER_INFO(0, "Server could not be created, please check that you platform supports sockets with the hkBase library");
		}
	}
	else
	{
		HK_VISUAL_DEBUGGER_INFO(0, "Server has already been created, only one server allowed per visual debugger instance");
	}
}

	/// Instructs the visual debugger to start capturing the output of all
	/// of the default viewers to the specified filename.
void hkVisualDebugger::capture(const char* captureFilename)
{
	hkStreambuf* stream = hkStreambufFactory::getInstance().open(captureFilename, hkStreambuf::OUTPUT);

	if(stream)
	{
		if(stream->isOk())
		{
			HK_VISUAL_DEBUGGER_INFO(1, "Capturing simulation state to \'" << captureFilename << "\'");

			// create display handler for the network
			hkDisplayHandler* dh = new hkSerializingDisplayHandler(stream);

			// send version information
			hkVersionHandler::sendVersionInformation(stream);

			// send a step so the the client doesn't get to parse commands it doesn't understand
			dh->step(16.0f);

			for(int i = 0; i < m_requiredViewers.getSize(); i++)
			{
				hkViewerFactory::getInstance().createViewer(m_requiredViewers[i]->cString(), m_world, dh);
			}

			Client* newClient = m_clients.expandBy(1);
			newClient->m_stream = stream;
			newClient->m_dh = dh;
			newClient->m_sph = HK_NULL;
			newClient->m_sd = HK_NULL;
		}
		else
		{
			HK_VISUAL_DEBUGGER_INFO(0, "Capture file \'" << captureFilename << "\' could not be opened for writing" );
			stream->removeReference();
		}
	}
}


void HK_CALL hkVisualDebugger::step(hkReal frameTimeInMs)
{
	HK_STEP_DISPLAY(frameTimeInMs);
}

void hkVisualDebugger::postSimulationCallback( hkWorld* world, const hkStepInfo& stepInfo )
{
	// see if there is a new client trying to connect
	if(m_server)
	{
		hkStreambuf* stream = m_server->pollForNewClient();

		if(stream)
		{
			HK_VISUAL_DEBUGGER_INFO(1, "A new network client has been received (host name not availibe at present)");

			// create display handler for the network
			hkDisplayHandler* dh = new hkSerializingDisplayHandler(stream);
			hkServerPluginHandler* sph = new hkServerPluginHandler(stream, world, dh);
			hkMousePickingHandler* mph = new hkMousePickingHandler(world);
			hkServerDeserializer* sd = new hkServerDeserializer(stream, sph, mph, m_keyboard);

			Client* newClient = m_clients.expandBy(1);
			newClient->m_stream = stream;
			newClient->m_dh = dh;
			newClient->m_sph = sph;
			newClient->m_sd = sd;

			// send version information
			hkVersionHandler::sendVersionInformation(stream);
			
			// send a step so the the client doesn't get to parse commands it doesn't understand
			dh->step(16.0f);

			// send a list of all the viewers over to the client
			sph->registerAllViewers();

			for(int i = 0; i < m_requiredViewers.getSize(); i++)
			{
				sph->selectViewerByName(m_requiredViewers[i]->cString());
			}
		}
	}

	// check existing clients (both network and file based clients)
	{
		for(int i = (m_clients.getSize() - 1); i >= 0; i--)
		{
			Client& client = m_clients[i];
			if(!client.m_stream->isOk())
			{
				HK_VISUAL_DEBUGGER_INFO(1, "Client has died, cleaning up (host name not available at present)");

				if(client.m_sd)
				{
					client.m_sd->removeReference();
				}

				if(client.m_sph)
				{
					client.m_sph->dereferenceAllViewers();
					client.m_sph->removeReference();
				}

				client.m_dh->removeReference();
				client.m_stream->removeReference();

				m_clients.removeAt(i);
			}
		}
	}
}

void hkVisualDebugger::stepDisplayCallback(hkReal frameTimeInMs)
{
	// time the frame
	hkReal frameTime = 16.0f; // (this is an approximation for the first frame)
	{
		if(m_amTimingFrame)
		{
			m_frameTimer.stop();
			frameTime = m_frameTimer.getElapsedSeconds() * 1000.0f;
		}

		m_frameTimer.reset();
		m_frameTimer.start();
		m_amTimingFrame = true;

		if(hkMath::fabs(frameTimeInMs) > hkMath::HK_REAL_EPSILON)
		{
			frameTime = frameTimeInMs;
		}
	}

	// step all clients
	for(int i = 0; i < m_clients.getSize(); i++)
	{
		Client& client = m_clients[i];
		if(client.m_stream->isOk())
		{
			// Step the display
			client.m_dh->step(frameTime);

			// Recieve commands from the client
			if(client.m_sd)
			{
				bool success = client.m_sd->consumeCommands();
			}

		}
	}
}

void hkVisualDebugger::addViewer(const char* viewerName)
{
	hkString* s = new hkString(viewerName);
	m_requiredViewers.pushBack(s);
}

void hkVisualDebugger::removeViewer(const char* viewerName)
{
	bool viewerRemoved = false;

	for(int i = 0; i < m_requiredViewers.getSize(); i++)
	{
		if(hkString::strCmp(m_requiredViewers[i]->cString(), viewerName) == 0)
		{
			delete m_requiredViewers[i];
			m_requiredViewers.removeAt(i);
			viewerRemoved = true;
		}
	}

	if(!viewerRemoved)
	{
		HK_VISUAL_DEBUGGER_INFO(0, "The viewer '" << viewerName << "', cannot not be removed from the default viewers list as it cannot be found!");
	}

}

// Cleanup management
hkVisualDebugger::~hkVisualDebugger()
{
	HK_REMOVE_STEP_DISPLAY_LISTENER(this);

	if(m_world)
	{
		m_world->removeWorldDeletionListener( this );
		m_world->removeWorldPostSimulationListener( this );
	}

	// cleanup existing clients
	{
		for(int i = (m_clients.getSize() - 1); i >= 0; i--)
		{
			Client& client = m_clients[i];

			// Send a step command to make sure that this frame gets processed
			client.m_dh->step(0.16f);

			if(client.m_sd)
			{
				client.m_sd->removeReference();
			}

			if(client.m_sph)
			{
				client.m_sph->removeReference();
			}

			client.m_dh->removeReference();	// this will not be dereferenced by the world...
			client.m_stream->flush();
			client.m_stream->removeReference();

			m_clients.removeAt(i);
		}
	}

	// clean up the list of viewers
	{
		for(int i = 0; i < m_requiredViewers.getSize(); i++)
		{
			delete m_requiredViewers[i];
		}
		m_requiredViewers.clear();
	}

	if(m_server)
	{
		delete m_server;
	}

	HK_VISUAL_DEBUGGER_INFO(1, "Server deleted");
}

void hkVisualDebugger::worldDeletedCallback( hkWorld* world )
{
	m_world->removeWorldDeletionListener( this );
	m_world->removeWorldPostSimulationListener( this );
	m_world = HK_NULL;
	delete this;
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
