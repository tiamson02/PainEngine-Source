/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>

#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_UNIX)
#	include <hkbase/stream/impl/hkBsdSockStreambuf.h>
#	include <hkbase/stream/impl/hkBsdSockServer.h>
#elif defined(HK_PLATFORM_PS2)
#	include <hkbase/stream/impl/hkPs2SockStreambuf.h>
#	include <hkbase/stream/impl/hkPs2SockServer.h>
#elif defined(HK_PLATFORM_GC)
#	include <hkbase/stream/hkSockStreambuf.h>
#endif

static const int HK_SOCK_STREAM_BUFFER_SIZE = 4096;

hkSockStreambuf* HK_CALL hkSockStreambuf::create(const char* servername, int portNumber)
{
#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_UNIX)
	return new hkBsdSockStreambuf(servername, portNumber);
#elif defined(HK_PLATFORM_PS2)
	return new hkPs2SockStreambuf(servername, portNumber);
#else
	return HK_NULL; // Sockets are not supprted yet
#endif
}

hkSockStreambuf::hkSockStreambuf()
{
	m_buffer.setSize(HK_SOCK_STREAM_BUFFER_SIZE);
	initBuffer(OUTPUT, m_buffer.begin(), m_buffer.getSize());
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
