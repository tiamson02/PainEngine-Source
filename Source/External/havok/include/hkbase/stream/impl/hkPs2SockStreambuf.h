/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_PS2_SOCK_STREAM_BUFF
#define HK_PS2_SOCK_STREAM_BUFF

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkSockStreambuf.h>

class hkPs2SockServer;

class hkPs2SockStreambuf : public hkSockStreambuf
{
		friend class hkPs2SockServer;

	public:

		hkPs2SockStreambuf(const char* servername, int portNumber);

		virtual ~hkPs2SockStreambuf();

		virtual int sysWrite( const char* buf, int nbytes);

		virtual int sysRead( char* buf, int nbytes);

		virtual void sysFlush();

	private:

		hkPs2SockStreambuf(int existingSocket);

		hkPs2SockStreambuf(int existingSocket, hkPs2SockServer* server);
			// the server used here so that when the client dies the server can be informed
			// and do a BLOCKING wait for a new client...
			// NOT necessary when the sceInetPoll is used...


	private:
		int m_socket;

		// needed for blocking server		
		hkPs2SockServer* m_server;

};

#endif // HK_PS2_SOCK_STREAM_BUFF
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
