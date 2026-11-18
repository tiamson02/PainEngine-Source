
#ifndef __PKTVPROXYCONNECTION_H__
#define __PKTVPROXYCONNECTION_H__

#include "LeString.h"
#include "IpAddress.h"
#include <winsock2.h>

class UdpMessage;

class PKTVProxyConnection
{
	friend class PKTVTransportUDP;

	bool			mBroken;
	String			mErrorString;
	SOCKET			mSocket;
	IpAddress		mLocalAddr;

	//Resending facility
	UdpMessage*		mMessageToResend;
	DWORD			mLastSentTime;
	IpAddress		mLastSentAddress;
	DWORD			mTimeoutTime;
	unsigned char	mResentTimes;
	unsigned char	mResendTimes;

	// Stats
	DlUl mOverallStats;
	DynamicArray<DlUl> mLastStats;
	DWORD mTimeOfLastStatsTest;
	DWORD mTimeSinceLastStatsChange;
	void TryStepStats( void );

	HashMap<IpAddress,ulong32>		mInternalAddressTranslationTable;
	DynamicArray<IpAddress>					mReturnAddressFilter;
	HashMap<IpAddress,ulong32>		mReplyIdents;

	// Transport binds
	HashMap<IpAddress,PKTVTransportUDP*>	mTransports;

public:

	PKTVProxyConnection( String interfaceToUse, unsigned short portToUse );
	~PKTVProxyConnection( void );

	// 0. Set timeout ( of leave at 10 seconds )
	void SetTimeoutAndResendTimes( DWORD timeout, unsigned char times ) { mTimeoutTime = timeout; mResendTimes = times; }

	// 1. Send one request ( if returns false, there was a problem )
	bool Send( UdpMessage* message, IpAddress addressToSendTo, bool awaitAnswer = false, bool deleteAfterSending = true );

	// 2. Receive one answer ( returns false if there was a problem, message can point to 0 if there was none )
	bool Receive( UdpMessage*& message, IpAddress& addressToReceiveFrom );

	IpAddress GetLocalAddress( void ) { return mLocalAddr; };
	unsigned short GetLocalPort( void ) { return mLocalAddr.Port; }
	bool Broken( String* error ) { if( error ) *error = mErrorString; return mBroken; }
	void Answered( void );

	DlUl GetOverallStats( void ) { return mOverallStats; };
	DlUl GetAverage( void );
};

#endif	// __PKTVPROXYCONNECTION_H__
