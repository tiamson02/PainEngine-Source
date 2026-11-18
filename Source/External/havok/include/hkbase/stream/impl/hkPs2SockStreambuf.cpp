/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stream/impl/hkPs2SockStreambuf.h>
#include <hkbase/stream/impl/hkPs2SockServer.h>

#include <libinsck.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

// It is not yet possible to setup a client socket on PS2...
// There has not yet been a need to implement this...
hkPs2SockStreambuf::hkPs2SockStreambuf(const char* servername, int portNumber)
	: m_socket(0), m_server(HK_NULL)
{
	HK_ASSERT2(0, "Client socket creation on PS2 has not yet been implemented");
}

// This constructor will be used when we have no blocking client
// polling on PS2.  We will then be able to have multiple clients on PS2
hkPs2SockStreambuf::hkPs2SockStreambuf(int existingSocket)
	: m_socket(existingSocket), m_server(HK_NULL)
{
}

// Temporary solution to stop PS2 trying to connect multiple client
// and thus blocking... (no non blocking comms on PS2 yet...)
hkPs2SockStreambuf::hkPs2SockStreambuf(int existingSocket, hkPs2SockServer* server)
	: m_socket(existingSocket), m_server(server)
{
}


hkPs2SockStreambuf::~hkPs2SockStreambuf()
{
	// close the socket
	if(m_socket != INVALID_SOCKET)
	{
		hkprintf("[server closing connection]\n");
		sceInsockShutdown(m_socket, SHUT_RDWR);
		// sceInsockTerminate(  Use this to clean up memory (for entire thread!)
		m_socket = INVALID_SOCKET;
		m_flags |= NETWORK_DISCONNECTED;
	}

	m_socket = INVALID_SOCKET;
}


int hkPs2SockStreambuf::sysWrite(const char* buf, int nbytes)
{
	int nWritten = 0;

	if(m_socket == INVALID_SOCKET)
	{
		return nWritten;
	}

	while(nWritten < nbytes)
	{
		int nWrittenSub = (int)send(m_socket, buf + nWritten, (unsigned)(nbytes - nWritten), 0);

		// Cleanup if there's been a socket error
		if(nWrittenSub == SOCKET_ERROR)
		{
			hkprintf("[socket error]\n");

			// close the socket
			{
				sceInsockShutdown(m_socket, SHUT_RDWR);
				// sceInsockTerminate(  Use this to clean up memory (for entire thread!)
				m_socket = INVALID_SOCKET;
				m_flags |= NETWORK_ERROR;
			}
			return 0;
		}
		else if(nWrittenSub == 0)	// the stream has been closed (EOF)
		{
			hkprintf("[socket error]\n");

			// close the socket
			{
				sceInsockShutdown(m_socket, SHUT_RDWR);
				// sceInsockTerminate(  Use this to clean up memory (for entire thread!)
				m_socket = INVALID_SOCKET;
				m_flags |= NETWORK_DISCONNECTED;
			}
			return 0;
		}

		nWritten += nWrittenSub;
	}

	return nWritten;
}

int hkPs2SockStreambuf::sysRead(char* buf, int nbytes)
{
	int nRead = 0;

	if(m_socket == INVALID_SOCKET)
	{
		return nRead;
	}

	// these reads may block...
	while(nRead < nbytes)
	{
		int nReadSub = (int)recv(m_socket, buf + nRead, (unsigned)(nbytes - nRead), 0);
		if(nReadSub == SOCKET_ERROR)
		{
			hkprintf("[socket error]\n");

			// close the socket
			{
				sceInsockShutdown(m_socket, SHUT_RDWR);
				// sceInsockTerminate(  Use this to clean up memory (for entire thread!)
				m_socket = INVALID_SOCKET;
				m_flags |= NETWORK_ERROR;
			}
			return 0;
		}
		else if(nReadSub == 0)	// the stream has been closed
		{
			hkprintf("[socket error]\n");

			// close the socket
			{
				sceInsockShutdown(m_socket, SHUT_RDWR);
				// sceInsockTerminate(  Use this to clean up memory (for entire thread!)
				m_socket = INVALID_SOCKET;
				m_flags |= NETWORK_DISCONNECTED;
			}
			return 0;
		}

		nRead += nReadSub;
	}

	return nRead;
}


void hkPs2SockStreambuf::sysFlush()
{
	// don't know how to 'flush' a socket yet!
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
