/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

// Class definition
#include <hkutilities2/visualdebugger/hkServerPluginHandler.h>
#include <hkutilities2/visualdebugger/hkDisplayHandler.h>
#include <hkutilities2/visualdebugger/hkViewerFactory.h>
#include <hkutilities2/visualdebugger/hkVisualDebuggerDebugOutput.h>

// from dynamics
#include <hkdynamics2/world/hkWorld.h>

// from base
#include <hkbase/stream/hkOArchive.h>

hkServerPluginHandler::hkServerPluginHandler(hkStreambuf* stream,
							hkWorld* world, hkDisplayHandler* displayHandler)
	: m_connectionStreambuf(stream), m_world(world), m_displayHandler(displayHandler)
{
//	m_world->addReference();	// this causes a circular reference!
	m_displayHandler->addReference();

	m_connectionStreambuf->addReference();
	m_commandStream = new hkOArchive(&m_commandStreambuf);

	// reserve enough memory for the viewers references and initialize to HK_NULL;
	{
		const hkObjectArray<hkViewerFactory::ViewerIdPair>& viewerList =
									hkViewerFactory::getInstance().getViewerList();

		const int size = viewerList.getSize();
		m_viewers.setSize(size);
		for(int i = 0; i < size; i++)
		{
			m_viewers[i] = HK_NULL;
		}
	}
}

hkServerPluginHandler::~hkServerPluginHandler()
{
	delete m_commandStream;
	m_connectionStreambuf->removeReference();

	m_displayHandler->removeReference();
//	m_world->removeReference();
}

hkPluginHandler::Status hkServerPluginHandler::sendCommand()
{
	hkArray<char>& rawData = m_commandStreambuf.getWriteBuffer();
	const int rawDataSize = rawData.getSize();

	hkOArchive m_connectionStream(m_connectionStreambuf);
	m_connectionStream.write32(rawDataSize);
	m_connectionStream.write(&rawData[0], rawData.getSize());
	rawData.clear();

	return hkPluginHandler::HK_OK;
}


void hkServerPluginHandler::dereferenceAllViewers()
{
	for(int i = 0; i < m_viewers.getSize(); i++)
	{
		if(m_viewers[i])
		{
			m_viewers[i]->removeReference();
			m_viewers[i] = HK_NULL;
		}
	}
}

hkPluginHandler::Status hkServerPluginHandler::registerViewer(const char* name, int length, int id)
{
	// send command headder
	m_commandStream->write8u(hkPluginHandler::HK_REGISTER_VIEWER);

	// send id
	m_commandStream->write32(id);

	// send length (and truncate if necessary)
	if(length > 65535)
	{
		length = 65535;
	}
	m_commandStream->write16u((unsigned short)length);

	// send the actual string data
	m_commandStream->write(const_cast<char*>(name), length);

	return sendCommand();
}

hkPluginHandler::Status hkServerPluginHandler::selectViewer(int id)
{
	// send command headder
	m_commandStream->write8u(hkPluginHandler::HK_SELECT_VIEWER);

	// send id
	m_commandStream->write32(id);

	return sendCommand();
}


hkPluginHandler::Status hkServerPluginHandler::createViewer(int id)
{
	const hkObjectArray<hkViewerFactory::ViewerIdPair>& viewerList =
								hkViewerFactory::getInstance().getViewerList();

	// create the viewer of the given id...
	if(!m_viewers[id])
	{
		hkReferencedObject* obj = viewerList[id].m_viewerCreationFunction(m_world, m_displayHandler);
		m_viewers[id] = obj;
	}

	return hkPluginHandler::HK_OK;
}

hkPluginHandler::Status hkServerPluginHandler::deleteViewer(int id)
{
	if(m_viewers[id])
	{
		m_viewers[id]->removeReference();
		m_viewers[id] = HK_NULL;
	}

	return hkPluginHandler::HK_OK;
}


hkPluginHandler::Status hkServerPluginHandler::registerAllViewers()
{
	// send a list of all the viewers over to the client
	const hkObjectArray<hkViewerFactory::ViewerIdPair>& viewerList =
								hkViewerFactory::getInstance().getViewerList();

	for(int i = 0; i < viewerList.getSize(); i++)
	{
		HK_VISUAL_DEBUGGER_INFO(1, "Registering viewer " << viewerList[i].m_name.cString() << "[" << i << "]" << " with new client");
		registerViewer(viewerList[i].m_name.cString(), viewerList[i].m_name.getLength(), i);
	}

	return hkPluginHandler::HK_OK;
}


hkPluginHandler::Status hkServerPluginHandler::selectViewerByName(const char* name)
{
	const hkObjectArray<hkViewerFactory::ViewerIdPair>& viewerList =
								hkViewerFactory::getInstance().getViewerList();

	// create the viewer of the given id...
	int i;
	for(i = 0; i < viewerList.getSize(); i++)
	{
		if(!hkString::strCmp(viewerList[i].m_name.cString(), name))
		{
			selectViewer(i);
			HK_VISUAL_DEBUGGER_INFO(1, "Auto selecting the [" << name << "] viewer for the new client connection");
			break;
		}
	}
	if(i == viewerList.getSize())
	{
		HK_VISUAL_DEBUGGER_INFO(0, "Cannot auto select viewer [" << name << "] for the new client connection as it is not in the viewer list");
	}

	return hkPluginHandler::HK_OK;
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
