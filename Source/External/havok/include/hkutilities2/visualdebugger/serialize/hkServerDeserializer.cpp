/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkmath2/hkMath.h>

#include <hkutilities2/visualdebugger/serialize/hkServerDeserializer.h>

#include <hkutilities2/visualdebugger/hkPluginHandler.h>
#include <hkutilities2/visualdebugger/hkMousePickingCommand.h>
#include <hkutilities2/visualdebugger/hkMousePickingHandler.h>
#include <hkutilities2/visualdebugger/hkKeyboardCommand.h>
#include <hkutilities2/visualdebugger/hkKeyboard.h>

#include <hkutilities2/visualdebugger/serialize/hkCommandHandler.h>
#include <hkutilities2/visualdebugger/serialize/hkDisplaySerializeIStream.h>

#include <hkutilities2/visualdebugger/hkDisplayHandler.h>


hkServerDeserializer::hkServerDeserializer(hkStreambuf* stream, hkPluginHandler* pluginHandler, hkMousePickingHandler* mph, hkKeyboard* vkb)
	: m_pluginHandler(pluginHandler), m_mousePickingHandler(mph), m_keyboard(vkb)
{
	m_pluginHandler->addReference();

	m_commandStream = new hkDisplaySerializeIStream(stream);
}

hkServerDeserializer::~hkServerDeserializer()
{
	m_pluginHandler->removeReference();

	delete m_commandStream;
}

bool hkServerDeserializer::consumeCommands()
{
	unsigned char command;

	do
	{
		// read 8bytes from the tcp/ip buffer and make endian safe:
		command = m_commandStream->read8u();

		switch(command)
		{
			case hkPluginHandler::HK_CREATE_VIEWER:
				processCreateViewer();
				break;

			case hkPluginHandler::HK_DELETE_VIEWER:
				processDeleteViewer();
				break;

			case hkCommandHandler::HK_STEP:
				if( !processStep() )
					return false;
				break;

			case HK_PICK_OBJECT:
				processPickObject();
				break;

			case HK_DRAG_OBJECT:
				processDragObject();
				break;

			case HK_RELEASE_OBJECT:
				processReleaseObject();
				break;

			case HK_VIRTUAL_KEYBOARD_DUMP:
				processVirtualKeyboardDump();
				break;

			default:
				hkprintf("SERVER GOT GIGO...0x%x \n", (char)command);
				return true;
		}
	}
	while(m_commandStream->isOk() && (command != hkCommandHandler::HK_STEP));

	return true;
}


//
// Private process commands
//

void hkServerDeserializer::processCreateViewer()
{
	int id = m_commandStream->read32();

	if(m_commandStream->isOk())
	{
		m_pluginHandler->createViewer(id);
	}
}

void hkServerDeserializer::processDeleteViewer()
{
	int id = m_commandStream->read32();

	if(m_commandStream->isOk())
	{
		m_pluginHandler->deleteViewer(id);
	}
}


void hkServerDeserializer::processPickObject()
{
	hkVector4 worldPosition;
	m_commandStream->readQuadVector4(worldPosition);

	int id = m_commandStream->read32();

	if(m_commandStream->isOk())
	{
		m_mousePickingHandler->pickObject(id, worldPosition);
	}
}

void hkServerDeserializer::processDragObject()
{
	hkVector4 newWorldPosition;
	m_commandStream->readQuadVector4(newWorldPosition);

	if(m_commandStream->isOk())
	{
		m_mousePickingHandler->dragObject(newWorldPosition);
	}
}

void hkServerDeserializer::processReleaseObject()
{
	if(m_commandStream->isOk())
	{
		m_mousePickingHandler->releaseObject();
	}
}

void hkServerDeserializer::processVirtualKeyboardDump()
{
	if(m_keyboard)
	{
		m_commandStream->read((char*)(m_keyboard), sizeof(hkKeyboard));
	}
	
	if(m_commandStream->isOk())
	{
		// hurray!
	}
}


bool hkServerDeserializer::processStep()
{
	return true;
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
