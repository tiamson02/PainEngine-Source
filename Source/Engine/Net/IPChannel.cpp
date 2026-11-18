#error This file is obselete
//#include "Engine.h"

#ifdef OIUYTREWLKJHGFDS

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Net/UdpMessage.h"
#include "Net/IPChannel.h"
#include "Net/NetworkClient.h"
#include "Net/NetworkProfiler.h"
#include "Net/NetworkDevice.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

// 500 ms = 0.5 sec
#define LATENCY_SEND_DELAY 500

String IPChannelStats::Print( void )
{
	String str = "******************* CHANNEL PACKET STATS ***********************\n";
	str += String::Sprintf( "    Packets sent: %d, received: %d", mPacketsSent, mPacketsReceived );
	if( mReliablePacketsAcknowledged )
		str += String::Sprintf( ", reliable acknowledged: %d\n", mReliablePacketsAcknowledged );
	else
		str += String::Sprintf( "\n" );
	str += String::Sprintf( "    Bytes   sent: %d, received: %d\n", mBytesSent, mBytesReceived );
	if( mPacketsSent )
		str += String::Sprintf( "    Average length of sent packet: %f bytes\n", ((double)mBytesSent/mPacketsSent) );
	if( mPacketsReceived )
		str += String::Sprintf( "    Average length of received packet: %f bytes\n", ((double)mBytesReceived/mPacketsReceived) );
	DWORD lifetime = GetCurrentTime() - mTimeOfCreation;
	if( lifetime )
	{
		str += String::Sprintf( "    Average speed of sending: %f Bps = %f bps\n", ((double)mBytesSent)*1000.0/lifetime, ((double)mBytesSent)*1000.0/lifetime*8.0 );
		str += String::Sprintf( "    Average speed of receiving: %f Bps = %f bps\n", ((double)mBytesReceived)*1000.0/lifetime, ((double)mBytesReceived)*1000.0/lifetime*8.0 );
		str += String::Sprintf( "    Average packets per second sent: %f\n", ((double)mPacketsSent)*1000.0/lifetime );
		str += String::Sprintf( "    Average packets per second received: %f\n", ((double)mPacketsReceived)*1000.0/lifetime );
		if( mReliablePacketsAcknowledged )
			str += String::Sprintf( "    Average reliable packets per second acknowledged: %f\n", ((double)mReliablePacketsAcknowledged)*1000.0/lifetime );
	}
	str += String::Sprintf( "****************************************************************\n" );
	return str;
}

IPChannel::IPChannel( IPDriver* driver, IpAddress* address )
:	mDriver(driver), mAddress(*address), mNetworkClient(NULL), mMaxUdpPacketSize(MAX_UDP_PACKET_SIZE)
,	mLastPacketApproxLatency(0), mStartTime(0), mLastReliableMessageToAck(0), mPingReset(false), mLastTimeLatencySent(0)
{
	State = OK;
	mLastSendTime = mLastReceivedTime = GetCurrentTime();
	mIncoming = mAck = mLastReliable = 0;
	mSent = 1;
	mReliableAck = mReliableIncoming = false;
	mReliableSent = true;
	mStreamingTimeout = STREAMING_TIMEOUT_TIME;

	DWORD curTime = GetCurrentTime();

	mLastReceivedTime = mStatsData.mTimeOfCreation = mStatsDataTemp.mTimeOfCreation = curTime;
	mLastSendTime = mLastReceivedTime - MAX_TIME_BETWEEN_PACKETS/2;

	// Init stats structures
	for( int i = 0; i < IPCHANNEL_STATS_FROM; ++i )
		mStatsChunks[i].Init( curTime );

	// Init ping structures
	for( int i = 0; i < PING_PACKETS_AVERAGED; ++i )
		mLastPacketsSendTime[i] = curTime;
	mAveragePing = 0.0f;

	// Init packet loss structures
	for( int i = 0; i < PACKET_LOSS_FROM; ++i )
		mPacketLoss_Sent[i] = mPacketLoss_Lost[i] = 0;
	mPacketLoss_Cursor = mStatsCursor = 0;
	mStatsUpdateTime = mPacketLoss_UpdateTime = curTime;
	mPacketLoss = 0.0f;
	mPacketsLostOutOfOrder = mPacketsLost = 0;
}

void IPChannel::SetMaxUdpPacketSize( unsigned short mups )
{
	if( mups > MAX_UDP_PACKET_SIZE ) mups = MAX_UDP_PACKET_SIZE;
	else if( mups < 20 ) mups = 20;
	mMaxUdpPacketSize = mups;
}

void IPChannel::Disconnect( void )
{
	if( GEngine->Net->Recording.IsPlaying() ) return;
	if( State == OK )
	{
		// To be sure, three times :)
		UdpMessage mesg;
		mesg.WriteUChar( GENERAL_DISCONNECT );
		mesg.EndWriting();

		mDriver->SendMaintenanceMessage( mAddress, &mesg );
		mDriver->SendMaintenanceMessage( mAddress, &mesg );
		mDriver->SendMaintenanceMessage( mAddress, &mesg );

		State = DISCONNECTED;
	}
}

IPChannel::~IPChannel()
{
	Disconnect();

	// Remove remaining reliable messages
	while( mReliableMessages.Size() )
	{
		delete mReliableMessages[0];
		mReliableMessages.FastRemove(0);
	}

	if( mDriver )
		mDriver->RemoveChannel( this );

	if( mNetworkClient )
	{
		mNetworkClient->mNetChannel = 0;
		delete mNetworkClient;
	}
	mNetworkClient = 0;

	GLog.Print( mStatsData.Print().AsChar() );
	GLog.Print( "    Packets lost during connection: %d\n", mPacketsLost );
	GLog.Print( "    Packets lost because they came out of order: %d\n", mPacketsLostOutOfOrder );
	GLog.Print( "****************************************************************\n" );
}

String IPChannel::Stats( bool temporary )
{
	return temporary?mStatsDataTemp.Print():mStatsData.Print();
}

// Message may be NULL
bool IPChannel::Send( UdpMessage* unreliable )
{
	if( unreliable && ( unreliable->GetSize() > MAX_UDP_PACKET_SIZE ) )
	{
		GLog.Print( "Tried to send unreliable of size %d\n", unreliable->GetSize() );
		unreliable = 0;	// forget it
	}

	bool unreliableSent = ( unreliable == NULL );
	bool sendingOk = true;
	DWORD curTime = GetCurrentTime();
/*
	GLog.Print( "-------\n" );
	GLog.Print( "Sending\n" );
	GLog.Print( "-------\n" );
*/
	do
	{
		// Determine if we need to append reliable message
		bool sendReliableMessage = CanSendReliableMessage();
//		GLog.Print( "send reliable message = %s\n", sendReliableMessage?"true":"false" );
//		GLog.Print( "mAck = %u, mLastReliable = %u, mReliableAck = %s, mReliableSent = %s\n", mAck, mLastReliable, mReliableAck?"true":"false", mReliableSent?"true":"false");

		UdpMessage mesg;
		// Write the packet header
		unsigned long h1 = mSent & 0x7FFFFFFF;
		if( sendReliableMessage ) h1 |= 0x80000000;
		unsigned long h2 = mIncoming & 0x7FFFFFFF;
		if( mReliableIncoming ) h2 |= 0x80000000;

//		GLog.Print( "mSent = %u, mIncoming = %u\n", mSent, mIncoming );
//		GLog.Print( "mReliableIncoming = %s\n", mReliableIncoming?"true":"false" );

		// Make sure highest is written first - this way collision with GameSpy messages won't occur too fast
		mesg.WriteUChar( (unsigned char)((h1&0xff000000)>>24) );
		mesg.WriteUChar( (unsigned char)((h1&0xff0000)>>16) );
		mesg.WriteUChar( (unsigned char)((h1&0xff00)>>8) );
		mesg.WriteUChar( (unsigned char)(h1&0xff) );

		mesg.WriteUChar( (unsigned char)((h2&0xff000000)>>24) );
		mesg.WriteUChar( (unsigned char)((h2&0xff0000)>>16) );
		mesg.WriteUChar( (unsigned char)((h2&0xff00)>>8) );
		mesg.WriteUChar( (unsigned char)(h2&0xff) );

		int tempSize = MAX_PACKET_HEADER_BYTES;
		if( sendReliableMessage )
		{
//			Assert( !mReliableMessages[0]->mWasSent );	// for loadmap testing only
//			if( mReliableMessages[0]->mWasSent )
//				GLog.Print( "HERE !!!!!!!!!!!!!!!!!!!!!!!!!!\n" );

			int i;
			for( i = 0; i < mReliableMessages.Size(); ++i )
				mReliableMessages[i]->mWasSent = false;

			i = 0;
			do
			{
				// Find out the size of new reliable
				int newSize = mReliableMessages[i]->GetSize();

				if( i )
				{
					// Disallow adding new reliable to message if mMaxUdpPacketSize would be passed, and we already have one in
					if( newSize + tempSize > mMaxUdpPacketSize )
						break;
				}
				else
				{
					// Disallow adding new reliable to message if MAX_UDP_PACKET_SIZE would be passed
					if( newSize + tempSize > MAX_UDP_PACKET_SIZE )
						break;
				}

				tempSize += newSize;
				mesg += *(mReliableMessages[i]);

				// Demo Recording
				if( mNetworkClient->mRecording && mNetworkClient->mRecording->IsRecording() && GEngine->Net->IsServer() )
					mNetworkClient->mRecording->RecordUdpMessage( mReliableMessages[i] );

				mReliableMessages[i]->mWasSent = true;
//				GLog.Print( "Reliable %d was sent = true\n", i );
				i++;
			}
			while( i < mReliableMessages.Size() );

			mLastReliable = mSent;

//			GLog.Print( "mLastReliable = mSent = %u\n", mLastReliable );
//			GLog.Print( "mReliableSent = %s\n", mReliableSent?"true":"false" );
		}
		mSent++;

		// Eventually append unreliable message
		if( unreliable )
		{
			if( tempSize == MAX_PACKET_HEADER_BYTES )
			{
				Assert( tempSize + unreliable->GetSize() <= MAX_UDP_PACKET_SIZE )
				mesg += *unreliable;
				unreliableSent = true;

				// Demo Recording
				if( mNetworkClient->mRecording && mNetworkClient->mRecording->IsRecording() && GEngine->Net->IsServer() )
					mNetworkClient->mRecording->RecordUdpMessage( unreliable );
			}
			else if( tempSize + unreliable->GetSize() <= mMaxUdpPacketSize )
			{
				mesg += *unreliable;
				unreliableSent = true;

				// Demo Recording
				if( mNetworkClient->mRecording && mNetworkClient->mRecording->IsRecording() && GEngine->Net->IsServer() )
					mNetworkClient->mRecording->RecordUdpMessage( unreliable );
			}
		}

		mesg.EndWriting();

#ifdef CRC_IN_PACKETS
		mesg.MakeCRCStamp();
#endif

		mLastPacketsSendTime[(mSent-1)%PING_PACKETS_AVERAGED] = curTime;
		mStatsData.mPacketsSent++;
		mStatsChunks[mStatsCursor].mPacketsSent++;
		mStatsData.mBytesSent += mesg.GetSize();
		mStatsChunks[mStatsCursor].mBytesSent += mesg.GetSize();
		if( !mDriver->Write( mAddress, &mesg ) )
			sendingOk = false;
	}
	while( !unreliableSent && sendingOk );

	mLastSendTime = curTime;
	return sendingOk;
}

// If returns true, you can read something from message. Otherwise destroy it.
bool IPChannel::PrepareForReading( UdpMessage* incoming )
{
/*
	GLog.Print( "-------------------\n" );
	GLog.Print( "Prepare for Reading\n" );
	GLog.Print( "-------------------\n" );
*/
	mStatsChunks[mStatsCursor].mPacketsReceived++;
	mStatsData.mPacketsReceived++;
	mStatsChunks[mStatsCursor].mBytesReceived += incoming->GetSize();
	mStatsData.mBytesReceived += incoming->GetSize();

#ifdef CRC_IN_PACKETS
	if( !incoming->CheckRemoveCRCStamp() ) return false;
#endif

	unsigned long sent = 0;
	sent |= (((unsigned long)incoming->ReadUChar())<<24);
	sent |= (((unsigned long)incoming->ReadUChar())<<16);
	sent |= (((unsigned long)incoming->ReadUChar())<<8);
	sent |= (unsigned long)incoming->ReadUChar();

	unsigned long ack = 0;
	ack |= (((unsigned long)incoming->ReadUChar())<<24);
	ack |= (((unsigned long)incoming->ReadUChar())<<16);
	ack |= (((unsigned long)incoming->ReadUChar())<<8);
	ack |= (unsigned long)incoming->ReadUChar();

	bool hasReliable = ((sent&0x80000000)!=0);		sent &= 0x7FFFFFFF;
	bool relAck =      ((ack &0x80000000)!=0);		ack  &= 0x7FFFFFFF;

//	GLog.Print( "sent = %u, ack = %u, hasReliable = %s, relAck = %s\n", sent, ack, hasReliable?"true":"false", relAck?"true":"false" );

	mLastReceivedTime = GetCurrentTime();

	// discard old packets
	if( sent <= mIncoming )
	{
//		GLog.Print( "sent < mIncoming = %u\n", mIncoming );
		mPacketsLostOutOfOrder--;
		return false;
	}

	// recalculate ping
	if( mAck < ack )
	{
		mLastPacketApproxLatency = mLastReceivedTime-mLastPacketsSendTime[ack%PING_PACKETS_AVERAGED];
		mAveragePing = mPingReset?mLastPacketApproxLatency:((PING_PACKETS_AVERAGED-1)*mAveragePing + ((double)mLastPacketApproxLatency))/PING_PACKETS_AVERAGED;
		mPingReset = false;
		mAck = ack;
//		GLog.Print( "changed mAck = %d\n", mAck );
	}
//	else
//		GLog.Print( "mAck = %d\n", mAck );

	// Packet loss stats update
	if( mLastReceivedTime - mStatsUpdateTime > IPCHANNEL_STATS_TICK )
	{
		mStatsUpdateTime = mLastReceivedTime;

		// Calculate new TempStats
		mStatsDataTemp = mStatsChunks[0];
		for( int i = 1; i < IPCHANNEL_STATS_FROM; ++i )
			mStatsDataTemp += mStatsChunks[i];

		// Move cursor
		mStatsCursor++;
		if( mStatsCursor == IPCHANNEL_STATS_FROM ) mStatsCursor = 0;
		mStatsChunks[mStatsCursor].Init(mStatsUpdateTime);
	}

	// Packet loss stats update
	if( mLastReceivedTime - mPacketLoss_UpdateTime > PACKET_LOSS_TICK )
	{
		mPacketLoss_UpdateTime = mLastReceivedTime;
		int plSent = 0;
		int plLost = 0;
		for( int i = 0; i < PACKET_LOSS_FROM; ++i )
		{
			plSent += mPacketLoss_Sent[i];
			plLost += mPacketLoss_Lost[i];
		}
		mPacketLoss = plSent?((float)plLost)/plSent:0.0f;
		mPacketLoss_Cursor++;
		if( mPacketLoss_Cursor == PACKET_LOSS_FROM ) mPacketLoss_Cursor = 0;
		mPacketLoss_Sent[mPacketLoss_Cursor] = 0;
		mPacketLoss_Lost[mPacketLoss_Cursor] = 0;
	}

	// Packet loss stats gathering
	mPacketLoss_Lost[mPacketLoss_Cursor] += sent - mIncoming - 1;
	mPacketLoss_Sent[mPacketLoss_Cursor] += sent - mIncoming;
	mPacketsLost += sent - mIncoming - 1;

	if( relAck == mReliableSent )	// Our last reliable message has been received
	{
		while( mReliableMessages.Size() && mReliableMessages[0]->mWasSent )
		{
			for( int i = 0; i < mReliableMessages[0]->DeliveryReceipts.Size(); ++i )
				GScript.Call( "AcknowledgeNetMsgDelivery", 0, "%d", mReliableMessages[0]->DeliveryReceipts[i] );

			delete mReliableMessages[0];
			mReliableMessages.Remove(0);
		}
		mReliableSent = !mReliableSent;
		mStatsData.mReliablePacketsAcknowledged++;
		mStatsChunks[mStatsCursor].mReliablePacketsAcknowledged++;
	}

	mIncoming = sent;
	mReliableAck = relAck;

//	GLog.Print( "mIncoming = %u, mReliableAck = %s\n", mIncoming, mReliableAck?"true":"false" );

	if( hasReliable )
	{
		mReliableIncoming = !mReliableIncoming;
//		GLog.Print( "Had reliable, mReliableIncoming = %s\n", mReliableIncoming?"true":"false" );
	}
//	else
//		GLog.Print( "mReliableIncoming = %s\n", mReliableIncoming?"true":"false" );

	if( incoming->ReadFinished() ) return false;	// empty packet

//	GLog.Print( "Not empty\n" );

	// Demo Recording
	if( mNetworkClient->mRecording && mNetworkClient->mRecording->IsRecording() && !GEngine->Net->IsServer() )
		mNetworkClient->mRecording->RecordUdpMessage( incoming, true );

	return true;
}

void IPChannel::ClearReliableMessages( void )
{
	if( mReliableMessages.Size() && mReliableMessages[0]->mWasSent )
		mLastReliableMessageToAck = mLastReliable;

	for( int i = 0; i < mReliableMessages.Size(); ++i )
		delete mReliableMessages[i];
	mReliableMessages.Clear();
}

void IPChannel::SendLatencyPacket( void )
{
	UdpMessage mesg;
	mesg.WriteUChar( IPChannel::MAINTENANCE|NETCLIENT_LATENCY );
	mesg.WriteULong( GEngine->SysDriver.GetCurrentTimeMS()-mStartTime );
	mesg.WriteLong( mNetworkClient->mPushLatency );
	mesg.WriteCheckToken( NCL_LATENCY );
	mesg.EndWriting();
	SendMaintenanceMessage( false, &mesg );
	mLastTimeLatencySent = GetCurrentTime();
}

void IPChannel::Tick( void )
{
	if( State == DISCONNECTED )
		return;

	// Keep the streaming alive so reliable packets have a chance to get to the other side
	DWORD currentTime = GetCurrentTime();
	if( CanSendReliableMessage() || currentTime - mLastSendTime > MAX_TIME_BETWEEN_PACKETS )
		Send( NULL );

	// Check streaming timeout
	if( currentTime - mLastReceivedTime > mStreamingTimeout )
	{
		Disconnect();

		if( mNetworkClient )
		{
			mNetworkClient->mNetChannel = NULL;
			delete mNetworkClient;
		}
		mNetworkClient = 0;

		// Remove remaining reliable messages
		while( mReliableMessages.Size() )
		{
			delete mReliableMessages[0];
			mReliableMessages.FastRemove(0);
		}

		if( !GEngine->Net->IsServer() )
		{
			String errorText = String::Sprintf( "Disconnected on timeout: %d ms\n", mStreamingTimeout );
			GLog.Print( errorText.AsChar() );
			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
		}
	}
}

void IPChannel::HandleChanneledPackage( UdpMessage* message )
{
	while( message->GetPos() < message->GetSize() )
		if( !HandleSingleMessage( message ) )
			return;
}

// This returns false if the message is unrecognized and cannot be subtracted from the buffer
bool IPChannel::HandleSingleMessage( UdpMessage* message )
{
	static unsigned char lastCommand = 0;

	unsigned char c = message->PeekUChar();
	unsigned char command = (c & 0xF0);
	bool result;

	switch( command )
	{
	case NETCLIENT:
		#ifdef PACKET_LOG
				GLog.Print( "*P: ReadBinaryMessage \n" );
		#endif
		result = mNetworkClient?mNetworkClient->ReadBinaryMessage( message ):false;
		break;
	case MAINTENANCE:
		#ifdef PACKET_LOG
				GLog.Print( "*P: ReadMaintenanceMessage \n" );
		#endif
		result = mNetworkClient?mNetworkClient->ReadMaintenanceMessage( (mDriver->State == IPDriver::SERVER), message ):false;
		break;
	default:
		GLog.Print( "Unknown channel command received: %d\n", command );
		return false;
	}
	lastCommand = c;
	return result;
}

bool IPChannel::SendMaintenanceMessage( bool reliable, UdpMessage* message )
{
	if( reliable )
	{
        m_setTag("Net");
		UdpMessage* mesg = new UdpMessage();
		mesg->WriteData( message ); NPROF2(NPIPChannelTextMessageString,message->GetSize());
		mesg->EndWriting();
		SendReliable( mesg );
		return true;
	}
	else
	{
		UdpMessage mesg;
		mesg.WriteData( message ); NPROF2(NPIPChannelTextMessageString,message->GetSize());
		mesg.EndWriting();
		return Send( &mesg );
	}
}

void IPChannel::ReportUnsentReliables( void )
{
//	if( mReliableMessages.Size() )
//		GLog.Print( "End of net tick in client %d, %d reliables left to send\n", mNetworkClient?mNetworkClient->mMyIndex:-1, mReliableMessages.Size() );

}

#endif
