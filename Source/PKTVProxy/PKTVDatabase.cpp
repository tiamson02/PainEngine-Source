
#include "PKTVDatabase.h"

#include "LogBuffer.h"

#define MAX_TIMES_REPORTED_BAD 30
//#define CLIENT_TO_CLIENT_TEST

PKTVDatabase::~PKTVDatabase( void )
{
}

void PKTVDatabase::ProcessUpdate( const IpAddress& host, unsigned char freePlaces, DWORD timeLatency, ulong32 frameLatency )
{
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "Database: Process Update: %s, places: %u, timeLatency: %u, frameLatency: %u\n", host.AsString().AsChar(), freePlaces, timeLatency, frameLatency );
#endif
	ClientData* c = mClients.Find( host );
	if( !c )
	{
		ClientData n;
		n.Address = host;
		n.BannedFromConnectingTo = false;
		n.TimesReportedBad = 0;
		n.NextClient = n.PrevClient = 0;
		mClients.Insert( host, n );
		c = mClients.Find( host );
	}


	c->LastReportTime = GSystem->SysDriver->GetCurrentTimeMS();
	c->Latency = timeLatency;
	c->FrameLatency = frameLatency;
	if( freePlaces )
		c->ResortIntoQueue( QueueStart );
	else
		c->RemoveFromQueue( QueueStart );
}

void PKTVDatabase::ProcessBan( const IpAddress& host )
{
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "Database: Process Ban: %s\n", host.AsString().AsChar() );
#endif
	ClientData* c = mClients.Find( host );
	if( c )
	{
		++c->TimesReportedBad;
		if( c->TimesReportedBad > MAX_TIMES_REPORTED_BAD )
			c->BannedFromConnectingTo = true;
	}
}

void PKTVDatabase::ProcessLeaving( const IpAddress& host )
{
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "Database: Process Leaving: %s\n", host.AsString().AsChar() );
#endif
	ClientData* c = mClients.Find( host );
	if( c )
		c->RemoveFromQueue(QueueStart);
}

// Getting out the information where to connect them
void PKTVDatabase::GetListOfRandomSlots( unsigned short numberOfSlots, DynamicArray<IpAddress>& slots, unsigned short IPsToPass, const IpAddress& ipToExclude )
{
#ifdef PKTV_CONNECTION_DEBUG
	PKTVDLog->Print( "%u: ", GSystem->SysDriver->GetCurrentTimeMS() );
	PKTVDLog->Print( "Database: Get List Of Random Slots: %u slots, %u to pass, exclude %s\n", numberOfSlots, IPsToPass, ipToExclude.AsString().AsChar() );
#endif

	if( !numberOfSlots )
		return;

	ClearQueue();

	int hosts = 0;

	for( ClientData* c = QueueStart; c; c = c->NextClient )
	{
		if( hosts == numberOfSlots )
			return;

		if( IPsToPass )
		{
			--IPsToPass;
			continue;
		}

		if( c->BannedFromConnectingTo )
			continue;

#ifdef CLIENT_TO_CLIENT_TEST
		if( ( c->Address == GEngine->PKTV->mUDP->GetLocalAddress() ) && c->NextClient )
			continue;
#endif
		if( !(c->Address == ipToExclude ) )
		{
			++hosts;
			slots.Add( c->Address );
		}
	}
}

// Input of proxy load information from proxies
void PKTVDatabase::UpdateProxyLoad( const IpAddress& proxyAddress, float proxyLoad )
{
	int index = mProxyAddresses.Find( proxyAddress );

	if( index != -1 )
	{
		Assert( index != 0 );
		if( proxyLoad == -1.0f )	// this proxy is to be removed
		{
			mProxyAddresses.FastRemove( index );
			mProxyLoads.FastRemove( index );
		}
		else
		{
			Assert( ( proxyLoad >= 0.0f ) && ( proxyLoad <= 256.0f ) );
			mProxyLoads[index] = proxyLoad;
		}
	}
	else
	{
		Assert( ( proxyLoad >= 0.0f ) && ( proxyLoad <= 256.0f ) );
		mProxyAddresses.Add( proxyAddress );
		mProxyLoads.Add( proxyLoad );
	}
}

void PKTVDatabase::UpdateLocalLoad( void )
{
	mProxyLoads[0] = CalculateLocalLoad();
}

// Remove proxy from the list
void PKTVDatabase::RemoveProxy( const IpAddress& proxyAddress )
{
	int index = mProxyAddresses.Find( proxyAddress );

	if( index != -1 )
	{
		Assert( index );	// not 0
		mProxyAddresses.FastRemove( index );
		mProxyLoads.FastRemove( index );
	}
}

// Redirecting decisions - where to redirect new clients
bool PKTVDatabase::RedirectNewClient( IpAddress& redirectionProxyAddress )
{
	int		minIndex	= 0;
	float	minLoad		= mProxyLoads[0];

	for( int i = 1; i < mProxyAddresses.Size(); ++i )
		if( mProxyLoads[i] < minLoad )
		{
			minLoad = mProxyLoads[i];
			minIndex = i;
		}

	if( !minIndex )
		return false;	// no redirection; give the client our address
	else
	{
		redirectionProxyAddress = mProxyAddresses[minIndex];
		return true;
	}
}

// Calculating proxy load in my subtree to give to the proxy I'm connected to
float PKTVDatabase::SummarizeProxyLoad( void )
{
	mProxyLoads[0] = CalculateLocalLoad();

	double sum = 0.0;
	for( int i = 0; i < mProxyAddresses.Size(); ++i )
		sum += mProxyLoads[i];
	sum /= mProxyAddresses.Size();
	return (float)sum;
}

float PKTVDatabase::CalculateLocalLoad( void )
{
	ClearQueue();
	ulong32 hostTimeSum = 0;
	ulong32 hostNumber = 0;

	for( ClientData* c = QueueStart; c; c = c->NextClient, ++hostNumber )
		hostTimeSum += c->Latency;

	if( !hostNumber )
		return 1.0f;
	return (float)(((double)hostTimeSum)/hostNumber);
}

void PKTVDatabase::ListDB( void )
{
	ClearQueue();
	for( ClientData* c = QueueStart; c; c = c->NextClient )
		c->Report();
}

void PKTVDatabase::ClearQueue( void )
{
	DWORD curTime = GSystem->SysDriver->GetCurrentTimeMS();

	while( QueueStart && ( curTime - QueueStart->LastReportTime > EXPIRE_TIME ) )
		QueueStart->RemoveFromQueue(QueueStart);

	ClientData* c = QueueStart;

	while( c )
	{
		while( c->NextClient && (curTime - c->NextClient->LastReportTime > EXPIRE_TIME ) )
			c->NextClient->RemoveFromQueue(QueueStart);
		c = c->NextClient;
	}
}
