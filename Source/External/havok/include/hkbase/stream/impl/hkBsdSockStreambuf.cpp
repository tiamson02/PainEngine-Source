/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stream/impl/hkBsdSockStreambuf.h>

#if defined(HK_PLATFORM_WIN32)
#	include <winsock.h>
#	pragma comment(lib,"wsock32.lib")
#elif defined(HK_PLATFORM_XBOX)
#	include <Xtl.h>
#else
#	include <unistd.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netdb.h>
#	define INVALID_SOCKET (-1)
#	define SOCKET_ERROR (-1)
#	define closesocket close
#	define WSACleanup()
#endif

#define WSAVERSION 0x202		// Winsock Version

#define SOCKET_TIMEOUT 0		// microseconds before select statement gives up

static bool HK_CALL hkIsDigit(int c)
{
	return c >= '0' && c <= '9';
}

hkBsdSockStreambuf::hkBsdSockStreambuf(const char* servername, int portNumber)
{
#if defined(HK_PLATFORM_WIN32)
	// initialize win socket
	WSADATA wsaData;
	if(WSAStartup(WSAVERSION,&wsaData) == SOCKET_ERROR)
	{
		HK_WARN("(Windows)WSAStartup failed with error!");
		WSACleanup();
		m_flags |= NETWORK_ERROR;
		return;
	}
#endif

#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_UNIX)
	// find the address of the server
	struct sockaddr_in server;
	{
		hkString::memSet(&server,0,sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons( (unsigned short)portNumber);

		// Is this really necessary? Cant everything just go to gethostbyname?
#if defined(HK_PLATFORM_WIN32)
		if(hkIsDigit(servername[0]))
		{
			server.sin_addr.S_un.S_addr = inet_addr(servername);
		}
#else
		if(0) {}
#endif
		else
		{
			struct hostent* hp;
			hp = gethostbyname(servername);

			if(hp)
			{
				hkString::memCpy(&(server.sin_addr),hp->h_addr,hp->h_length);
			}
			else
			{
				HK_WARN("Invalid server address!");
				m_flags |= NETWORK_ERROR;
				return;
			}
		}
	}

	// create the clients socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	{
		if(m_socket == INVALID_SOCKET)
		{
			HK_WARN("Error creating socket!");
			WSACleanup();
			m_flags |= NETWORK_ERROR;
			return;
		}
	}

	// connect to the clients socket
	{
		if(connect(m_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		{
			HK_WARN("Cannot connect to server!");
			//WSACleanup();
			m_flags |= NETWORK_ERROR;
			return;
		}
	}
#endif

#if defined(HK_PLATFORM_XBOX)
// They cannot be supported as the whole name lookup is not yet supported
// by the XBOX api
//
//	// Initialize the network stack.
//	{
//		// XNetStartup( NULL );  // This is the default startup
//		XNetStartupParams xnsp;
//		ZeroMemory(&xnsp, sizeof(xnsp));
//		xnsp.cfgSizeOfStruct = sizeof(xnsp);
//		xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
//
//		INT iResult = XNetStartup( &xnsp );
//	}

	HK_ASSERT2(0, "Client socket creation on XBOX has not yet been implemented");
#endif
}

hkBsdSockStreambuf::hkBsdSockStreambuf(SOCKET existingSocket)
	: m_socket(existingSocket)
{
}


hkBsdSockStreambuf::~hkBsdSockStreambuf()
{
	if(m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
	}

	m_socket = INVALID_SOCKET;
}


int hkBsdSockStreambuf::sysWrite( const char* buf, int nbytes)
{
	int nWritten = 0;

	if(m_socket == INVALID_SOCKET)
	{
		return nWritten;
	}

	while(nWritten < (int)nbytes)
	{
		int nWrittenSub = send(m_socket, buf + nWritten, (int)nbytes - nWritten, 0);

		// Cleanup if there's been a socket error
		if(nWrittenSub == SOCKET_ERROR)
		{
			HK_WARN("Pipe closed at remote end or a network error has occured!");
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;

			m_flags |= NETWORK_ERROR;

			break;
		}
		else if(nWrittenSub == 0)	// the stream has been closed
		{
			HK_WARN("Write fail! Was the recieving end of socket closed?");
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;

			m_flags |= NETWORK_DISCONNECTED;

			break;
		}

		nWritten += nWrittenSub;
	}

	return nWritten;
}

int hkBsdSockStreambuf::sysRead(char* buf, int nbytes)
{
	int nRead = 0;

	if(m_socket == INVALID_SOCKET)
	{
		return nRead;
	}

	// these reads may block...
	while(nRead < (int)nbytes)
	{
		int nReadSub = recv(m_socket, buf + nRead, (int)nbytes - nRead, 0);
		if(nReadSub == SOCKET_ERROR)
		{
			HK_WARN("Pipe closed at remote end or a network error has occured!");
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;

			m_flags |= NETWORK_ERROR;

			break;
		}
		else if(nReadSub == 0)	// the stream has been closed
		{
			HK_WARN("Read fail! Was the recieving end of socket closed?");
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;

			m_flags |= NETWORK_DISCONNECTED;

			break;
		}

		nRead += nReadSub;
	}

	return nRead;
}


void hkBsdSockStreambuf::sysFlush()
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
