/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_BSD_SOCK_STREAM_BUFF
#define HK_BSD_SOCK_STREAM_BUFF

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkSockStreambuf.h>

#if defined(HK_PLATFORM_WIN32)
#	include <winsock.h>
#elif defined(HK_PLATFORM_XBOX)
#	include <Xtl.h>
#elif defined(HK_PLATFORM_UNIX)
	typedef int SOCKET;
#else
#	error not implemented
#endif

class hkBsdSockStreambuf : public hkSockStreambuf
{
		friend class hkBsdSockServer;

	public:

		hkBsdSockStreambuf(const char* servername, int portNumber);

		virtual ~hkBsdSockStreambuf();

		virtual int sysWrite( const char* buf, int nbytes);

		virtual int sysRead( char* buf, int nbytes);

		virtual void sysFlush();

	private:

		hkBsdSockStreambuf(SOCKET existingSocket);

	private:

		SOCKET m_socket;
};

#endif // HK_BSD_SOCK_STREAM_BUFF

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
