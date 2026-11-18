
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include "IpAddress.h"
#include "HashMap.h"
#include "ProxySystem.h"
#include "SystemDriver.h"
#include "LogBuffer.h"

#define EXPIRE_TIME 95000	// 95 seconds

class PKTVDatabase
{
	struct ClientData
	{
		IpAddress		Address;
		DWORD			LastReportTime;
		unsigned char	TimesReportedBad;
		bool			BannedFromConnectingTo;
		ulong32	Latency;
		ulong32	FrameLatency;
	
		// Queue data
		ClientData*		NextClient;              // for the queue
		ClientData*		PrevClient;

		void ResortIntoQueue( ClientData*& queueStart )
		{
			// First make sure we're added to queue
			if( !( NextClient || PrevClient || ( queueStart == this ) ) )
			{
				if( !queueStart )
				{
					queueStart = this;
					return;
				}
				else	// add at beginning
				{
					queueStart->PrevClient = this;
					NextClient = queueStart;
					queueStart = this;
				}
			}

			// then resort - one or the other may fit; not both

			DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();

			while( PrevClient )
			{
				ClientData* c = PrevClient;
				if( curTime - c->LastReportTime > EXPIRE_TIME )
				{
					c->RemoveFromQueue( queueStart );
					continue;
				}

				if( ( c->FrameLatency < FrameLatency ) ||
					( ( c->FrameLatency == FrameLatency ) && ( c->Latency <= Latency ) ) )
					break;

				// xchange with prev client
				if( c->PrevClient )
					c->PrevClient->NextClient = this;
				if( NextClient )
					NextClient->PrevClient = c;
				c->NextClient = NextClient;
				PrevClient = c->PrevClient;
				NextClient = c;
				c->PrevClient = this;
				if( queueStart == c )
					queueStart = this;
			}

			while( NextClient )
			{
				ClientData* c = NextClient;
				if( curTime - c->LastReportTime > EXPIRE_TIME )
				{
					c->RemoveFromQueue( queueStart );
					continue;
				}

				if( ( c->FrameLatency > FrameLatency ) ||
					( ( c->FrameLatency == FrameLatency ) && ( c->Latency >= Latency ) ) )
					break;

				// xchange with next client
				if( c->NextClient )
					c->NextClient->PrevClient = this;
				if( PrevClient )
					PrevClient->NextClient = c;
				c->PrevClient = PrevClient;
				NextClient = c->NextClient;
				PrevClient = c;
				c->NextClient = this;
				if( queueStart == this )
					queueStart = c;
			}
		}

		void RemoveFromQueue( const ClientData*& queueStart )
		{
			if( PrevClient )
				PrevClient->NextClient = NextClient;
			if( NextClient )
				NextClient->PrevClient = PrevClient;
			if( queueStart == this )
				queueStart = NextClient;
			PrevClient = NextClient = 0;
		}

		void Report( void )
		{
			GLog->Print( "%s\n", Address.AsString().AsChar() );
		}
	};

	HashMap<IpAddress,ClientData> mClients;
	ClientData* QueueStart;

	// Proxy load part
	DynamicArray<IpAddress> mProxyAddresses;
	DynamicArray<float> mProxyLoads;

	void ClearQueue( void );

public:
	PKTVDatabase( const IpAddress& localAddr ) : QueueStart(0)
	{
		mProxyAddresses.Add( localAddr );
		mProxyLoads.Add( 0.0f );
	};

	~PKTVDatabase( void );

	void ProcessUpdate( const IpAddress& host, unsigned char freePlaces, DWORD timeLatency, ulong32 frameLatency );
	void ProcessBan( const IpAddress& host );
	void ProcessLeaving( const IpAddress& host );

	// Getting out the information where to connect them
	void GetListOfRandomSlots( unsigned short numberOfSlots, DynamicArray<IpAddress>& slots, unsigned short IPsToPass, const IpAddress& ipToExclude );

	// Input of proxy load information from proxies
	void UpdateProxyLoad( const IpAddress& proxyAddress, float proxyLoad );
	void UpdateLocalLoad( void );

	// Remove proxy from the list
	void RemoveProxy( const IpAddress& proxyAddress );

	// Redirecting decisions - where to redirect new clients
	bool RedirectNewClient( IpAddress& redirectionProxyAddress );

	// Calculating proxy load in my subtree to give to the proxy I'm connected to
	float SummarizeProxyLoad( void );
	float CalculateLocalLoad( void );

	void ListDB( void );	// for debugging
};
