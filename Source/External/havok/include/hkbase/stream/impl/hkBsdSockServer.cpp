/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stream/impl/hkBsdSockServer.h>
#include <hkbase/stream/impl/hkBsdSockStreambuf.h>

#if defined(HK_PLATFORM_WIN32)
#	include <winsock.h>
#	pragma comment(lib,"wsock32.lib")
	typedef int socklen_t;
#elif defined(HK_PLATFORM_XBOX)
#	include <Xtl.h>
	typedef int socklen_t;
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <unistd.h>
#	include <errno.h>
#	include <string.h>
#	define closesocket close
#	define INVALID_SOCKET (-1)
#	define SOCKET_ERROR (-1)
#endif

#define WSAVERSION 0x202	// winsock version

hkBsdSockServer::hkBsdSockServer(int portNumber)
{
	bool sockError = false;

#if defined(HK_PLATFORM_XBOX)
	// Initialize the network stack.
	{
		// XNetStartup( NULL );  // This is the default startup
		XNetStartupParams xnsp;
		ZeroMemory(&xnsp, sizeof(xnsp));
		xnsp.cfgSizeOfStruct = sizeof(xnsp);
		xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;

		INT iResult = XNetStartup( &xnsp );
	}
#endif

#if defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_WIN32)
	// initialise winsock
	{
		WSADATA wsaData;
		sockError = (WSAStartup(WSAVERSION,&wsaData) == SOCKET_ERROR);
		if(sockError)
		{
			HK_WARN("(Windows) WSAStartup failed with error!");
		}
	}
#endif

	// create the server socket that listens for connections
	if(!sockError)
	{
		m_listenerSocket = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
		sockError = (m_listenerSocket == INVALID_SOCKET);
		if(sockError)
		{
			HK_WARN("Error creating socket!");
		}
	}

	// bind the socket to a local port
	if(!sockError)
	{
		struct sockaddr_in local;
		local.sin_family = AF_INET;
		local.sin_addr.s_addr = 0;
		local.sin_port = htons( (unsigned short)portNumber);

		sockError = (bind(m_listenerSocket,(struct sockaddr*)&local,sizeof(local) ) == SOCKET_ERROR);
		if(sockError)
		{
			HK_WARN("Error binding to socket!");
		}
	}

	// put the server socket into a listening state
	if(!sockError)
	{
		sockError = (listen(m_listenerSocket,2) == SOCKET_ERROR);
		if(sockError)
		{
			HK_WARN("Error listening to socket!");
		}
	}


	// cleanup if there was a error setting up the server
	if(sockError)
	{
#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX)
		WSACleanup();
#endif
		m_listenerSocket = INVALID_SOCKET;
	}

	m_isOk = !sockError;
}

hkBsdSockServer::~hkBsdSockServer()
{
	if(m_listenerSocket != INVALID_SOCKET)
	{
		closesocket(m_listenerSocket);
	}
}


hkSockStreambuf* hkBsdSockServer::pollForNewClient(void)
{
	// poll the listener socket for new client sockets
	if(m_isOk && (m_listenerSocket != INVALID_SOCKET))
	{
		fd_set readFds;
		FD_ZERO(&readFds);
		FD_SET(m_listenerSocket, &readFds);

		fd_set exceptFds;
		FD_ZERO(&exceptFds);
		FD_SET(m_listenerSocket, &exceptFds);

		timeval t = {0, 0};	// no wait time -- i.e. non blocking select

		// see if there is and client trying to connect to the well knowen port listener socket

#if !defined(HK_PLATFORM_XBOX) && !defined(HK_PLATFORM_WIN32)
		int maxFd = m_listenerSocket + 1;
#else
		int maxFd = 0;
#endif
		int numHits = select(maxFd, &readFds, HK_NULL, &exceptFds, &t);

		if(numHits == SOCKET_ERROR)
		{
#if defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_WIN32)
			int error = WSAGetLastError();
			switch(error)
			{
				case WSANOTINITIALISED:
					HK_WARN("WSANOTINITIALISED error has occured!");
					break;
				case WSAEFAULT:
					HK_WARN("WSAEFAULT error has occured!");
					break;
				case WSAENETDOWN:
					HK_WARN("WSAENETDOWN error has occured!");
					break;
				case WSAEINVAL:
					HK_WARN("WSAEINVAL error has occured!");
					break;
				case WSAEINTR:
					HK_WARN("WSAEINTR error has occured!");
					break;
				case WSAEINPROGRESS:
					HK_WARN("WSAEINPROGRESS error has occured!");
					break;
				case WSAENOTSOCK:
					HK_WARN("WSAENOTSOCK error has occured!");
					break;
				default:
					HK_WARN("An undefined (windows/BSD socket) select error has occured!");
					break;
			}
#else
			HK_WARN(strerror(errno));
#endif
		}

		// see if we want to accept a new client
		if((numHits > 0) && FD_ISSET(m_listenerSocket, &readFds))
		{
			struct sockaddr_in from;
			socklen_t fromlen = sizeof(from);

			SOCKET newClientSocket = accept(m_listenerSocket, (struct sockaddr*)&from, &fromlen);

		
			//hkprintf("[connected from %s:%d]\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));

			if(newClientSocket == INVALID_SOCKET)
			{
				// close current connection
				HK_WARN("Error accepting a connection!");
				closesocket(newClientSocket);
			}
			else
			{
				// Add the current connection to the servers list
				unsigned int optval = 1;
				setsockopt(newClientSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof (unsigned int));

				return new hkBsdSockStreambuf(newClientSocket);
			}
		}
	}

	return HK_NULL;
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
