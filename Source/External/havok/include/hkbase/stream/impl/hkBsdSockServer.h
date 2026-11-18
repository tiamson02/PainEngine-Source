/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_BSD_SOCK_SERVER
#define HK_BSD_SOCK_SERVER

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkSockServer.h>
class hkSockStreambuf;

#if defined(HK_PLATFORM_WIN32)
#	include <winsock.h>
#elif defined(HK_PLATFORM_XBOX)
#	include <Xtl.h>
#else
	typedef int SOCKET;
#endif


class hkBsdSockServer : public hkSockServer
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STREAM)

		hkBsdSockServer(int portNumber);
		~hkBsdSockServer();

			/// Returns a pointer to a newly created hkSockStreambuf if there is a new client
			/// else returns HK_NULL
		hkSockStreambuf* pollForNewClient(void);


	private:

		SOCKET m_listenerSocket;
};

#endif // HK_BSD_SOCK_SERVER

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
