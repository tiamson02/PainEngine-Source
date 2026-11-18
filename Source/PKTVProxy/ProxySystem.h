
#pragma once

#include "LeString.h"

class SystemDriver;
class CursConsole;
class PKTVManager;

class ProxySystem
{
	double			mTimeSum;
	bool			mQuit;
public:
	SystemDriver*	SysDriver;
#ifndef PKTV_ENABLED
    PKTVManager*	PKTV;
#endif
	CursConsole*	Console;

	// Config file data
	// ===========================
	String			InterfaceToUse;
	bool			NoClientDispatch;
	bool			NoProxyDispatch;
	String			SourcePassword;
	String			PasswordForProxies;
	String			PasswordForClients;
	ulong32	MaxClients;
	DWORD			BroadcastDelayMS;
	DWORD			BufferTime;
	double			MinSecondsPerFrame;
	String			IdleMessage;
	bool			AutoRecordOnConnect;
	String			AutoRecordFilename;
	unsigned short	ListeningUDPPort;
	bool			DisplayViewersMesgs;

	ProxySystem( void );
	~ProxySystem( void );

	void LoadIniFile( void );
	void SaveIniFile( void );
	bool Tick( void );
	void ExecuteCommand( String command );
	void PrintProxyStats( void );
	void PrintProxyConfig( void );

};