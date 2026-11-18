/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <hkbase/hkBase.h>
#include <hkbase/stream/impl/hkPs2SockServer.h>
#include <hkbase/stream/impl/hkPs2SockStreambuf.h>

// bsd interface
#include <libinsck.h>

//
// Configuration
//
char* hkPs2SockServer::m_combination = "Combination1";
char* hkPs2SockServer::m_netDbAbsolutePath = "mc0:/BWNETCNF/BWNETCNF";
sceSifMClientData hkPs2SockServer::m_cdIF;	// Interface handle

//
// Bring up network interface
//
#include <eekernel.h>
#include <stdio.h>
#include <sifrpc.h>

#include <libnet.h>
#include <libcdvd.h>

#include <libpc.h>
#include <sifdev.h>

#include <hkbase/stream/impl/hkPs2SockServer.h>

// Power off handling
#define NETBUFSIZE 4096
u_int g_net_buf[NETBUFSIZE] __attribute__((aligned(64))); // Inet buffer

#define STACKSZ	(512 * 16)
static u_char stack[STACKSZ] __attribute__ ((aligned(16)));

void PowerOffThread(void *arg) {
	int sid = (int)arg;
	int stat;
	while(1) {
		WaitSema(sid);
		printf("power off request has come.\n");
		// dev9 power off, need to power off PS2
		sceDevctl("dev9", DDIOC_OFF, NULL, 0, NULL, 0);
		// PS2 power off
		sceCdPowerOff(&stat);
	}
}

void PowerOffHandler(void *arg) {
	int sid = (int)arg;
	iSignalSema(sid);
}

void PreparePowerOff(void) {
	struct ThreadParam tparam;
	struct SemaParam   sparam;
	int tid;
	int sid;

	sparam.initCount = 0;
	sparam.maxCount  = 1;
	sparam.option    = 0;
	sid = CreateSema(&sparam);

	ChangeThreadPriority(GetThreadId(), 2);
	tparam.stackSize = STACKSZ;
	tparam.gpReg = &_gp;
	tparam.entry = PowerOffThread;
	tparam.stack = (void *)stack;
	tparam.initPriority = 1;
	tid = CreateThread(&tparam);
	StartThread(tid, (void *)sid);

	sceCdPOffCallback(PowerOffHandler, (void *)sid);
}

static bool HK_CALL ps2setupNetwork()
{
	bool isOk = true;

	int ret;
	int if_id[sceLIBNET_MAX_INTERFACE];
	struct sceInetAddress myaddr;
	char myname[1024];
	char myptrname[1024];
	int tid;

	// *** PERFORMANCE COUNTER ISSUE ***
	// NOTE: The performance timers will and must be restarted when/if the 
	// visual debugger is initialized because setting up the network 
	// connection requires more than 7 seconds if a DHCP lookup is involved,
	// this will cause the performance timers to emit an exception which
	// cannot be caught by the user and therefore results in a crash!
	scePcStop();
	
	// Initialize the SIF RPC handler
//	sceSifInitRpc(0);
	
	// Prepare the poweroff handler
//	PreparePowerOff();
	
	// Initialize libnet
//	sceSifMInitRpc(0);

	ret = sceLibnetInitialize( &hkPs2SockServer::m_cdIF, sizeof(g_net_buf), sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY );
	if(ret < 0) 
	{
		HK_WARN("(PS2) Cannot initialize libNet!");
		isOk = false;
		return isOk;
	}

	ret = sceLibnetRegisterHandler( &hkPs2SockServer::m_cdIF, g_net_buf );
	if(ret < 0) 
	{
		HK_WARN("(PS2) Cannot register SIF handler!");
		isOk = false;
		return isOk;
	}

	// Load network configuration
	ret = load_set_conf_extra( &hkPs2SockServer::m_cdIF, g_net_buf, hkPs2SockServer::m_netDbAbsolutePath, hkPs2SockServer::m_combination, sceLIBNETF_AUTO_UPIF | sceLIBNETF_DECODE);
	if(ret < 0)
	{
		HK_WARN("(PS2) load_set_conf() failed!");
		isOk = false;
		return isOk;
	}
	ret = sceLibnetWaitGetAddress( &hkPs2SockServer::m_cdIF, g_net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, sceLIBNETF_AUTO_UPIF );
	if(ret < 0)
	{
		HK_WARN("(PS2) wait_get_addr() failed!");
		isOk = false;
		return isOk;
	}

	// Display IP
	ret = sceInetAddress2String( &hkPs2SockServer::m_cdIF, g_net_buf, myptrname, 1024, &myaddr );
	if(!ret)
	{
		hkprintf("[my ip address is: %s]\n", myptrname);
	}
	else
	{
		HK_WARN("(PS2) sceInetAddress2String() failed!");
		isOk = false;
		return isOk;
	}
	
	if(isOk)
	{
		hkprintf("[network interface is now up]\n");
	}
	else
	{
		hkprintf("[could not initialize the network interface]\n");
	}
	
	return isOk;
}


static bool HK_CALL ps2shutdownNetwork()
{
	// shutdown inet	
	int ret;

	ret = sceInetCtlDownInterface( &hkPs2SockServer::m_cdIF, g_net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlDownInterface() failed.(%d)\n", ret );
		return false;
	}
	ret = sceLibnetUnregisterHandler( &hkPs2SockServer::m_cdIF, g_net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetUnregisterHandler() failed.(%d)\n", ret );
		return false;
	}
	ret = sceLibnetTerminate( &hkPs2SockServer::m_cdIF );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetTerminate() failed.(%d)\n", ret );
		return false;
	}
	
	return true;
}


/// This class assumes that the network layer has been setup correctly.
hkPs2SockServer::hkPs2SockServer(int portNumber)
{
	if(!ps2setupNetwork())
	{
		HK_WARN("Error bringnig up network interface!");
		sceInsockShutdown(m_listenerSocket, SHUT_RDWR);
		m_isOk = false;
		return;
	}

	int ret;
	struct sockaddr_in sin;

	m_listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_listenerSocket < 0)
	{
		HK_WARN("Error creating socket!");
		m_isOk = false;
		return;
	}

	hkString::memSet(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons((unsigned short)portNumber);

	ret = bind(m_listenerSocket, (struct sockaddr *)&sin, sizeof(sin));
	if(ret < 0)
	{
		HK_WARN("Error binding to socket!");
		sceInsockShutdown(m_listenerSocket, SHUT_RDWR);
		m_isOk = false;
		return;
	}
	
	ret = listen(m_listenerSocket, 5);
	if(ret < 0)
	{
		HK_WARN("Error listening to socket!");
		sceInsockShutdown(m_listenerSocket, SHUT_RDWR);
		m_isOk = false;
		return;
	}
}


hkPs2SockServer::~hkPs2SockServer()
{
	// close the listener socket
	if(m_listenerSocket >= 0)
	{
		sceInsockShutdown(m_listenerSocket, SHUT_RDWR);
	}
}


hkSockStreambuf* hkPs2SockServer::pollForNewClient(void)
{
	sceInetPollFd_t	fds[10];
	fds[0].cid = m_listenerSocket;
	fds[0].events = sceINET_POLLIN;

	// poll for net client connections
	if(sceInsockPoll(fds, 1, 0) < 0)
	{
		m_isOk = false;
		return HK_NULL;
	}

	if(fds[0].revents & sceINET_POLLIN) 
	{
		struct sockaddr_in sin;
		int len = sizeof(sin);
		int client_socket = accept(m_listenerSocket, (struct sockaddr *)&sin,(unsigned*)&len);

		if(0 > client_socket)
		{
			HK_WARN("Error accepting a connection!");
			m_isOk = false;
			return HK_NULL;
		}
		
		hkprintf("[connected from %s:%d]\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
	
		unsigned int optval = 1;
		int ret = setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(unsigned int));
		if(ret < 0)
		{
			HK_WARN("Error accepting a connection!");
		}
		
		return new hkPs2SockStreambuf(client_socket, this);			
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
