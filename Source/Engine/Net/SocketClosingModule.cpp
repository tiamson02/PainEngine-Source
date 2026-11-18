#error boo
#include "Engine.h"
#include "SocketClosingModule.h"

#include "Core/FileSystem/LogBuffer.h"
extern  LogBuffer GLog;

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

SocketClosingModule::SocketClosingModule( void )
{
}

SocketClosingModule::~SocketClosingModule( void )
{
	while( mSocketsToClose.Size() )
	{
		GEngine->SysDriver.ExactSleep(1);
		Tick();
	}

//	for( int i = 0; i < mSocketsToClose.Size(); ++i )
//		closesocket( mSocketsToClose[i] );
}

void SocketClosingModule::AddSocketToClose( SOCKET socket )
{
	shutdown( socket, SD_SEND );
	mSocketsToClose.Add( socket );
}

void SocketClosingModule::Tick( void )
{
#ifdef TRANSGAMING
	// We don't support Gamespy right now ( TIMEVAL )...we'll need something else here.
#else
	int i=0;
	TIMEVAL timeout = {0,0};
	fd_set okSet;
	FD_ZERO(&okSet);
	for( int i = 0; i < mSocketsToClose.Size(); ++i )
	{
		FD_SET(mSocketsToClose[i],&okSet);
	}
	fd_set badSet;
	FD_ZERO(&badSet);
	for( i = 0; i < mSocketsToClose.Size(); ++i )
	{
		FD_SET(mSocketsToClose[i],&badSet);
	}

	if( ::select( 0, &okSet, 0, &badSet, &timeout ) )
	{
		for( i = 0; i < mSocketsToClose.Size(); ++i )
		{
			bool remove = false;
			if( FD_ISSET( mSocketsToClose[i], &okSet ) )
			{
				char buffer[4096];
				int result = ::recv( mSocketsToClose[i], buffer, 4096, 0 );
				if( ( result == 0 ) || ( result == SOCKET_ERROR ) )
					remove = true;
			}
			else if( FD_ISSET( mSocketsToClose[i], &badSet ) )
				remove = true;

			if( remove )
			{
				closesocket( mSocketsToClose[i] );
				mSocketsToClose.FastRemove(i);
				--i;
			}
		}
	}
#endif
}
