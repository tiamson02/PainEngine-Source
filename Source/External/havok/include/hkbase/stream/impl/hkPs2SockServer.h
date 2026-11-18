/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_PS2_SOCK_SERVER
#define HK_PS2_SOCK_SERVER

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkSockServer.h>
class hkSockStreambuf;

// sce includes
#include <libmrpc.h>

class hkPs2SockServer : public hkSockServer
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE)

		hkPs2SockServer(int portNumber);
		~hkPs2SockServer();

			/// Returns a pointer to a newly created hkSockStreambuf if there is a new client
			/// else returns HK_NULL
		hkSockStreambuf* pollForNewClient(void);
		
		static char* m_combination;
		static char* m_netDbAbsolutePath;
		static sceSifMClientData m_cdIF;	// Interface handle

	private:
		int m_listenerSocket;		// BSD style socket;

};

#endif // HK_PS2_SOCK_SERVER

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
