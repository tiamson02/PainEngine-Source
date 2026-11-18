/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2006 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_BASE_R3000_SOCKET_H
#define HK_BASE_R3000_SOCKET_H

#include <hkbase/hkBase.h>
#include <hkbase/stream/hkSocket.h>

class hkR3000Socket : public hkSocket
{
	public:

		typedef int socket_t;

		hkR3000Socket(){}//socket_t s=socket_t(-1));

		virtual ~hkR3000Socket(){}

		virtual hkBool isOk() const{ return true; }

		virtual void close(){}

		virtual int read( void* buf, int nbytes){ return 0; }

		virtual int write( const void* buf, int nbytes){ return 0; }

		// client

		virtual hkResult connect(const char* servername, int portNumber){ return HK_SUCCESS; }

		// server

		hkResult listen(int port){ return HK_SUCCESS; }
		hkSocket* pollForNewClient(){ return HK_NULL; }

	protected:

		hkResult createSocket(){ return HK_SUCCESS; }

		socket_t m_socket;
};

#endif // HK_BASE_R3000_SOCKET_H



/*
* Havok SDK - PUBLIC RELEASE, BUILD(#20060114)
*
* Confidential Information of Havok.  (C) Copyright 1999-2006 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available from salesteam@havok.com.
*
*/
