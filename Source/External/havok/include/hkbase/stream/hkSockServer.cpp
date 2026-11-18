/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkSockServer.h>

#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_UNIX)
#	include <hkbase/stream/impl/hkBsdSockServer.h>
#elif defined(HK_PLATFORM_PS2)
#	include <hkbase/stream/impl/hkPs2SockServer.h>
#endif


hkSockServer* HK_CALL hkSockServer::create(int portNumber)
{
#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX) || defined(HK_PLATFORM_UNIX)
	return new hkBsdSockServer(portNumber);
#elif defined(HK_PLATFORM_PS2)
	return new hkPs2SockServer(portNumber);
#else
	return HK_NULL; // Sockets not supported yet
#endif
}

bool hkSockServer::isOk()
{
	return m_isOk;
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
