/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//
// Havok 2.0 Command Handler - Defines an interface for viewing 
// serializing and deserializing graphical information
//

#ifndef HK_COMMAND_HANDLER
#define HK_COMMAND_HANDLER

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkStringStreambuf.h>

class hkOArchive;

	/// This class acts as a command buffer.  Instead of the hkPluginHandler writing
	/// directly to the client/server stream (a socket) it writes commands to the
	/// hkCommandHandler::getStream() stream.  This buffers up the commands until
	/// it is possible to send them over the network.  This mechanism is necessary
	/// when blocking commmunications are used so that sending and recieving can be
	/// coordinated between a blocking client and a blocking server.
class hkCommandHandler : public hkReferencedObject
{
	public:
		enum Status
		{
			HK_OK = 0,
			HK_FAIL
		};

	
		enum Command // for serialization 
		{
			HK_STEP = 0xF0
		};

	public:

		hkCommandHandler(hkStreambuf* stream);
		~hkCommandHandler();

		hkStreambuf* getStream();

		Status step();

	private:
		hkStringStreambuf* m_commandBuffer;
		hkOArchive* m_commandStream;

		hkStreambuf* m_connectionStream;
};

#endif // HK_COMMAND_HANDLER

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
