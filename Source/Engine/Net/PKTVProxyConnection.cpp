
#include "Engine.h"
#include "PKTVProxyConnection.h"
#include "PKTVManager.h"
#include "PKTVTransportUDP.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#define STATS_STEP_MS 1000
#define STATS_STEPS_TO_STORE 5

extern String GetSocketErrorString( void );	// translate WSA last error to string
extern bool Resolve( const char* hostName, IpAddress& ipAddr );
extern void PKTV_Log( const char* fmt, ... );
extern void PKTV_Error( const char* fmt, ... );

#ifdef PKTV_CONNECTION_DEBUG
extern LogBuffer PKTVDLog;
#endif

PKTVProxyConnection::PKTVProxyConnection( String interfaceToUse, unsigned short portToUse )
:	mBroken( false )
,	mSocket(INVALID_SOCKET)
,	mMessageToResend(0)
,	mLastSentTime(0)
,	mTimeoutTime(10000)
,	mResendTimes(3)
,	mResentTimes(0)
,	mTimeSinceLastStatsChange(0)
{
	mTimeOfLastStatsTest = GEngine->SysDriver.GetCurrentTimeMS();
	mLastStats.Add(mOverallStats);
	// Make socket

	in_addr	hostAddr;
	hostAddr.S_un.S_addr = INADDR_ANY;

	if( interfaceToUse.Len() )
	{
		IpAddress temp;
		if( !Resolve( interfaceToUse.AsChar(), temp ) )
		{
			PKTV_Error( "Proxy Socket Init Error: tried to resolve given interface and failed!\n" );
			mBroken = true;
			return;
		}
		else
			hostAddr.S_un.S_addr = temp.IP;
	}
	else
	{
		char hostnam[256];

		if( gethostname( hostnam, 255 ) )
		{
			PKTV_Error( "Proxy Socket Init Error: gethostname() call failed!! Can't find this computer's name! Returned: %s\n", GetSocketErrorString().AsChar() );
			mBroken = true;
			return;
		}

		hostent* host = gethostbyname( hostnam );
		if( !host )
		{
			PKTV_Error( "Proxy Socket Init Error: failed to resolve our IP with gethostbyname()!!\n" );
			mBroken = true;
			return;
		}

		hostAddr = *(in_addr*)(*host->h_addr_list);
	}

	// Set default server connection on listening
	SOCKET sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( INVALID_SOCKET == sock )
	{
		PKTV_Error( "Proxy Socket Init Error: Couldn't open the socket: %s\n", GetSocketErrorString().AsChar() );
		mBroken = true;
		return;
	}

	// non-blocking socket
	ulong32 one = 1;
	if( ioctlsocket( sock, FIONBIO, &one ) == -1 )
	{
		PKTV_Error( "Proxy Socket Init Error: Couldn't make socket non-blocking: %s\n", GetSocketErrorString().AsChar() );
		closesocket( sock );
		mBroken = true;
		return;
	}

	// Win2k creeping ping fix
	{
		#define IOC_VENDOR 0x18000000
		#define SIO_UDP_CONNRESET (IOC_IN|IOC_VENDOR|12)

		u_long noUdpConnReset = FALSE;
		if( ioctlsocket( sock, SIO_UDP_CONNRESET, &noUdpConnReset ) == SOCKET_ERROR )
			GLog.Print( "Proxy Socket Init Warning: Failed to fix creeping ping for Win2k!\n" );
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr = hostAddr;
	addr.sin_port = htons((u_short)portToUse);

	if( bind(sock, (sockaddr*)&addr, sizeof(sockaddr_in) ) )
	{
		PKTV_Error( "Proxy Socket Init Error: Failed to bind socket on port %d: %s\n", portToUse, GetSocketErrorString().AsChar() );
		closesocket( sock );
		mBroken = true;
		return;
	}

	// Find out what port we were assigned
	sockaddr_in toRead;
	int nameLen = sizeof( sockaddr_in );
	if( getsockname( sock, (sockaddr*)&toRead, &nameLen ) )
	{
		PKTV_Error( "Proxy Socket Init Error: getsockname on bound socket failed!! Returned: %s\n", GetSocketErrorString().AsChar() );
		closesocket( sock );
		mBroken = true;
		return;
	}

	mLocalAddr.IP = toRead.sin_addr.S_un.S_addr;
	mLocalAddr.Port = ntohs(toRead.sin_port);

	PKTV_Log( "Proxy socket bound: %d.%d.%d.%d:%d", mLocalAddr.ip1, mLocalAddr.ip2, mLocalAddr.ip3, mLocalAddr.ip4, mLocalAddr.Port );

	mSocket = sock;
}

PKTVProxyConnection::~PKTVProxyConnection( void )
{
	// Destroy socket
	if( mSocket )
		closesocket( mSocket );

	// Stop resending module
	Answered();
}

bool PKTVProxyConnection::Send( UdpMessage* message, IpAddress addressToSendTo, bool awaitAnswer, bool deleteAfterSending )
{
	Assert( !awaitAnswer || deleteAfterSending );

	if( mSocket == INVALID_SOCKET )
		return false;

	if( awaitAnswer )
	{
		// Remeber time
		mLastSentTime = GEngine->SysDriver.GetCurrentTimeMS();
		mLastSentAddress = addressToSendTo;
	}

	// Add foreign identifier to the message
	ulong32* foreignIdent = mReplyIdents.Find( addressToSendTo );
	ulong32 fIdent = (DWORD)-1;
	if( foreignIdent )
		fIdent = *foreignIdent;

	// Add my identifier to the message, depending on the IP
	ulong32* identPtr = mInternalAddressTranslationTable.Find( addressToSendTo );
	ulong32 myIdent;
	if( identPtr )
		myIdent = *identPtr;
	else
	{
		myIdent = mInternalAddressTranslationTable.Size();
		mInternalAddressTranslationTable.Insert( addressToSendTo, myIdent );
	}

	static UdpMessage tempMesg(65536);
	tempMesg.SeekToBegin();
	tempMesg.WriteULong( fIdent );
	tempMesg.WriteULong( myIdent );
	tempMesg.WriteData( message );
	tempMesg.EndWriting();

	// Send it
	sockaddr_in addr;
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(addressToSendTo.Port);
	addr.sin_addr.s_addr = addressToSendTo.IP;

#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
	PKTVDLog.Print( "Sent message of %u bytes to %s\n", tempMesg.GetSize(), addressToSendTo.AsString().AsChar() );
#endif

	if( tempMesg.GetSize() >= 1400 )
	{
		GLog.Print( "Message sent of size: %d\n", tempMesg.GetSize() );
		GLog.Flush();
	}

	int bytesSend = sendto( mSocket, tempMesg.GetBuffer(), tempMesg.GetSize(), 0, (sockaddr*)&addr, sizeof(addr) );

	// Stats
	{
		TryStepStats();
		mOverallStats.BytesUL += tempMesg.GetSize();
		++mOverallStats.PacketsUL;
		mLastStats[mLastStats.Size()-1].BytesUL += tempMesg.GetSize();
		++mLastStats[mLastStats.Size()-1].PacketsUL;
	}

	if( SOCKET_ERROR != bytesSend )
	{
		if( mLocalAddr.IP == INADDR_ANY )
		{
			// Find out what IP we were assigned
			sockaddr_in toRead;
			int nameLen = sizeof( sockaddr_in );
			if( !getsockname( mSocket, (sockaddr*)&toRead, &nameLen ) )
				mLocalAddr.IP = toRead.sin_addr.S_un.S_addr;
		}

		// Remember it
		if( awaitAnswer )
		{
			if( mMessageToResend )
				delete mMessageToResend;
			mMessageToResend = message;
		}
		else if( ( mMessageToResend != message ) && deleteAfterSending )
			delete message;

		return true;
	}

	if( WSAGetLastError() == WSAEWOULDBLOCK )
	{
		// Remember it
		if( awaitAnswer )
		{
			if( mMessageToResend )
				delete mMessageToResend;
			mMessageToResend = message;
		}
		else if( ( mMessageToResend != message ) && deleteAfterSending )
			delete message;

		return true;		// that's ok, it's normal, we said: don't block and this would have blocked
	}

	PKTV_Log( "Low Level Error: UDP socket write error in sendto(): %s", GetSocketErrorString().AsChar() );
	mBroken = true;
	closesocket( mSocket );
	mSocket = INVALID_SOCKET;
	if( deleteAfterSending )
		delete message;
	return false;
}

bool PKTVProxyConnection::Receive( UdpMessage*& message, IpAddress& addressToReceiveFrom )
{
	if( INVALID_SOCKET == mSocket )
	{
		message = 0;
		return false;
	}

	// Check answer
	static unsigned char tempMesg[MAX_UDP_PACKET_SIZE];

	sockaddr_in fromAddr;
	int fromAddrSize = sizeof(fromAddr);

	message = new UdpMessage;
	int bytesRecv = recvfrom( mSocket, (char*)tempMesg, MAX_UDP_PACKET_SIZE, 0, (sockaddr*)&fromAddr, &fromAddrSize );

	TryStepStats();

	if( SOCKET_ERROR != bytesRecv )
	{
		// Stats
		{
			mOverallStats.BytesDL += bytesRecv;
			++mOverallStats.PacketsDL;
			mLastStats[mLastStats.Size()-1].BytesDL += bytesRecv;
			++mLastStats[mLastStats.Size()-1].PacketsDL;
		}

		mResentTimes = 0;

		IpAddress realAddress;
		realAddress.IP		= fromAddr.sin_addr.s_addr;
		realAddress.Port	= ntohs( fromAddr.sin_port );

		ulong32 ident = *(ulong32*)tempMesg;
		ulong32 ident2 = *(ulong32*)(tempMesg+4);

		if( ident >= (ulong32)mInternalAddressTranslationTable.Size() )
		{			// unknown ident; don't translate to anything
			addressToReceiveFrom = realAddress;
		}
		else
		{
			HashMap<IpAddress,ulong32>::HashPair pair = mInternalAddressTranslationTable.GetPair( ident );
			Assert( pair.Value == ident );

			// Fill up the return addresses memory
			while( (ulong32)mReturnAddressFilter.Size() <= ident ) mReturnAddressFilter.Add(IpAddress(0,0,0,0,0));

			if( !mReturnAddressFilter[ident].IP )
			{		// first reply from certain address; remember address; translate to send address
				mReturnAddressFilter[ident] = realAddress;
				addressToReceiveFrom = pair.Key;
			}
			else if( ( mReturnAddressFilter[ident].IP != fromAddr.sin_addr.s_addr ) ||
					 ( mReturnAddressFilter[ident].Port != ntohs( fromAddr.sin_port ) ) )
			{		// good ident; different address than the last one; don't translate to send address; may be a spoof
				addressToReceiveFrom = realAddress;
			}
			else	// good ident, good address; translate to send address
			{
				addressToReceiveFrom = pair.Key;
			}
		}

		// Remember how to reply to given address
		ulong32* targetIdent = mReplyIdents.Find( addressToReceiveFrom );
		if( targetIdent )
			*targetIdent = ident2;
		else
			mReplyIdents.Insert( addressToReceiveFrom, ident2 );

#ifdef PKTV_CONNECTION_DEBUG
		PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
		PKTVDLog.Print( "Received message of %u bytes from %s\n", bytesRecv, realAddress.AsString().AsChar() );
#endif

		message->SeekToBegin();
		message->WriteData( (char*)(tempMesg+2*sizeof(ulong32)), bytesRecv-2*sizeof(ulong32) );
		message->EndWriting();
		message->SeekToBegin();

		if( message->PeekUChar() == PKTVManager::CMD_TRANSPORT_MESG )
		{
			PKTVTransportUDP** transport = mTransports.Find( addressToReceiveFrom );
//			bool lose = ((((float)rand())/RAND_MAX) < 0.1f );
//			if( transport && (*transport) && !lose )	// if no transport connected, ignore message
			if( transport && (*transport) )	// if no transport connected, ignore message
				(*transport)->HandleMessage( message );
			delete message;
			return Receive( message, addressToReceiveFrom );
		}
		else if( message->PeekUChar() == PKTVManager::CMD_TRANSPORT_REQUEST )
		{
			PKTVTransportUDP** transport = mTransports.Find( addressToReceiveFrom );
			if( !transport || !(*transport) )	// if transport connected, ignore message
				GEngine->PKTV->ProcessTransportRequest( addressToReceiveFrom );
			else
				(*transport)->SendKeepalive();
			delete message;
			return Receive( message, addressToReceiveFrom );
		}

#ifdef PKTV_CONNECTION_DEBUG
		PKTVDLog.Print( "%u: ", GEngine->SysDriver.GetCurrentTimeMS() );
		PKTVDLog.Print( "Received message of %u bytes from %s\n", bytesRecv, addressToReceiveFrom.AsString().AsChar() );
#endif
		return true;
	}

	int error = WSAGetLastError();
	if( error == WSAEWOULDBLOCK )
	{
		delete message;
		message = 0;

		// No answer => check timeout
		if( mMessageToResend )
		{
			if( GEngine->SysDriver.GetCurrentTimeMS()-mLastSentTime > mTimeoutTime )
			{
				// If timeout, resend a few times, then give up.
				if( ( mResentTimes < mResendTimes ) && ( Send( mMessageToResend, mLastSentAddress ) ) )
				{
					mLastSentTime = GEngine->SysDriver.GetCurrentTimeMS();
					++mResentTimes;
				}
				else
				{
					Answered();	// not really, but we forget what we wanted to say
					PKTV_Error( "Proxy Socket Error: answer timeout!!\n" );
					mBroken = true;
					closesocket( mSocket );
					mSocket = INVALID_SOCKET;
					return false;
				}
			}
		}

		return true; // that's ok, it's normal, there was nothing to read and we said: don't block
	}

	if( error != WSAECONNRESET )
		PKTV_Log( "Low Level Error: UDP socket read error in recvfrom(): %s\n", GetSocketErrorString().AsChar() );

	Answered();
	mBroken = true;
	closesocket( mSocket );
	mSocket = INVALID_SOCKET;
	return false;
}

IpAddress PKTVProxyConnection::GetTranslatedAddress( const IpAddress& addr )
{
	// Get my ident for this address
	ulong32* identPtr = mInternalAddressTranslationTable.Find( addr );
	if( !identPtr )
		return addr;	// not found in sent idents
	ulong32 myIdent = *identPtr;
	while( (ulong32)mReturnAddressFilter.Size() <= myIdent ) mReturnAddressFilter.Add(IpAddress(0,0,0,0,0));
	if( !mReturnAddressFilter[myIdent].IP )
		return addr;	// not found in return addresses
	return mReturnAddressFilter[myIdent];	// found
}

void PKTVProxyConnection::Answered( void )
{
	if( mMessageToResend )
		delete mMessageToResend;
	mMessageToResend = 0;
}

void PKTVProxyConnection::TryStepStats( void )
{
	DWORD curTime = GEngine->SysDriver.GetCurrentTimeMS();
	DWORD diff = curTime-mTimeOfLastStatsTest;
	mTimeOfLastStatsTest = curTime;

	mTimeSinceLastStatsChange += diff;
	while( mTimeSinceLastStatsChange > STATS_STEP_MS )
	{
		if( mLastStats.Size() >= STATS_STEPS_TO_STORE )
			mLastStats.Remove(0);
		DlUl t;
		mLastStats.Add(t);
		mTimeSinceLastStatsChange -= STATS_STEP_MS;
	}
}

PKTVProxyConnection::DlUl PKTVProxyConnection::GetAverage( void )
{
	DlUl sum;
	if( mLastStats.Size() < 2 )
		return sum;

	for( int i = 0; i < mLastStats.Size()-1; ++i )
		sum += mLastStats[i];

	sum.BytesDL /= (mLastStats.Size()-1);
	sum.PacketsDL /= (mLastStats.Size()-1);
	sum.BytesUL /= (mLastStats.Size()-1);
	sum.PacketsUL /= (mLastStats.Size()-1);

	return sum;
}
