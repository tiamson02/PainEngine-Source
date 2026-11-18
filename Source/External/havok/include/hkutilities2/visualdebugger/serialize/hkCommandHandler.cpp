/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkutilities2/visualdebugger/serialize/hkCommandHandler.h>

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkOArchive.h>


hkCommandHandler::hkCommandHandler(hkStreambuf* stream)
	: m_connectionStream(stream)
{
	m_connectionStream->addReference();

	// setup the internal memorybuffer...
	m_commandBuffer = new hkStringStreambuf();
	m_commandStream = new hkOArchive(m_commandBuffer);
}

hkCommandHandler::~hkCommandHandler()
{
	delete m_commandStream;
	m_commandBuffer->removeReference();

	m_connectionStream->removeReference();
}

hkStreambuf* hkCommandHandler::getStream()
{
	return m_commandBuffer;
}

hkCommandHandler::Status hkCommandHandler::step()
{
//	// Write out all the commands in the buffer out to the stream
//	{
//
//	}

	m_commandStream->write8u(HK_STEP);	// this is a dummy command for the moment...

	// send the command buffer back to the server
	hkArray<char>& writeBuffer = m_commandBuffer->getWriteBuffer();
	m_connectionStream->write(&writeBuffer[0], writeBuffer.getSize());
	m_connectionStream->flush();

	writeBuffer.clear();

	return HK_OK;
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
