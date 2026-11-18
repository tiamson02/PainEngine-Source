// PKTVProxy.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <tchar.h>

#include "Engine.h"
#include "ProxySystem.h"

// GLOBALS
LogBuffer*		GLog = 0;
ProxySystem*	GSystem = 0;
#ifdef PKTV_CONNECTION_DEBUG
LogBuffer*		PKTVDLog = 0;
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	GSystem = new ProxySystem;
	GSystem->LoadIniFile();

	while( GSystem->Tick() );

	delete GSystem;
	GSystem = 0;
	return 0;
}

