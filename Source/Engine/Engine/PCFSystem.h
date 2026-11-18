// #pragma once
#ifndef _PCFSYSTEM_
#define _PCFSYSTEM_

#include "Core/DynArray.h"
#include "Core/SystemDriver.h"
#include "Core/RegistryManager.h"

class NetworkDevice2;
class PhysicsEngine;
class MilesEngine;
class EngineGame;
class InputSystem;
class World;
class GraphicsDevice;
class RenderDevice;
class CPackageManager;
#ifdef PKTV_ENABLED
class PKTVManager;
#endif


class ENGINE_API PCFSystem
{
public:

	// States info
	enum PCFSystemStates
	{
		ShutDown = 0,
		DemoPlay = 1,
		SingleGame = 2,
#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
        DedicatedServer = 3,
#endif
		MultiplayerServer = 4,
		MultiplayerClient = 5,
		Default = 2,
	};

	enum ServerUpdateMethod
	{
		INTERNET = 0,
		LAN = 1,
		IP = 2,
		NONE = 3,
	} LastServerUpdateMethod;

	// Variables
    HINSTANCE		hInstance;
    SystemDriver	SysDriver;
	RegistryManager	Registry;

    PhysicsEngine*	Physics;
	MilesEngine*	Audio;
	NetworkDevice2*	Net;
	InputSystem*	InputSys;
    EngineGame*		Game;
#ifdef	TRANSGAMING
    World*		pcfWorld;
#else
    World*		World;
#endif
    CPackageManager *PackManager;
#ifdef PKTV_ENABLED
	PKTVManager*	PKTV;
#endif

	GraphicsDevice*	Video;

	int				GameState;

    DWORD           ExitRequested:1;
	double			TimeMultiplier;
    float           avgFPS;
    float		    LastDelta;

	bool			UsePhysicsSubsteps;

	// Maximal framerates - for systems which need to do something else; setting those will cause engine to sleep() sometimes
#ifdef TRANSGAMING
	float           MinSecondsPerFrame;
#else
	double			MinSecondsPerFrame;
#endif

	// Information extracted from command line in Initialize()
	// it's stored because it may start a multiplayer client later in the game
	bool			StartMultiplayerClient;
	bool			StartMultiplayerServer;
	bool			StartPlayingRecording;
	String			ServerToConnectTo;
	String			PlayerNameToAssume;
	String			PasswordToGive;
#ifdef TRANSGAMING
    String          StartupCommand;
#endif
#ifndef TRANSGAMING
	String			InterfaceToUse;
	String			RecordingToPlay;
	int				EnforcedHWClass;
#endif
	unsigned int	SpeedToConnectAt;

	String			MapToPlay;
#ifndef TRANSGAMING
	bool			ServerIsPrivate;
#endif
	unsigned short	PortToPlayAt;

	unsigned int	MyPublicIPAddress;	// set from ServerBrowser in Menu, needed for negotiation callbacks in Net

	bool			GameSpyOK;

	DynamicArray<String>	MyInterfaces;	// Filled at startup, used by dedicated server in combo box.
											// List of IP of interfaces connected to our computer.

public:
	// Construction/destruction/startup
	PCFSystem();
	~PCFSystem();
#ifdef TRANSGAMING
    BOOL    Initialize(int argc, char **argc);
#else
	BOOL	Initialize( HINSTANCE hinstance, const char* cmdLine = 0 );
#endif
	bool	ZoneAlarmNetworkAccessPopUp( void );
	bool	InitVideo();

	// State switches
	void	SwitchToState( int newState );
	int		GetGameState( void ) { return GameState; };
	bool	InServerMode( void ) { 
#ifndef TRANSGAMING_NO_DEDICATED_SERVER 
        return ( ( GameState == MultiplayerServer ) || ( GameState == DedicatedServer ) );
#else
        return ( GameState == MultiplayerServer );
#endif
    }

	// RunLoop() and its exit method
	void	RunLoop();
	void	Close() { ExitRequested=1; };

	// Tick
	void	TickEngine( bool minimized );
	void	SetTimeMultiplier(double t) { TimeMultiplier = t; };
	void	LoadingTick( int progress = 1 );

	// Load/Save
	bool LoadGame( FileObject* f );
	bool SaveGame( FileObject* f );

	// Shortcuts
    void			ResetTimer( void ) { SysDriver.ResetTimer(); };
	HINSTANCE		GetInstance( void ) const { return hInstance; };
	EngineGame*		GetGame( void );
	RenderDevice*	GetRenderDevice( void ) const;
	void			TakeScreenshot() const;

private:
#ifdef TRANSGAMING
	void ParseCommandLine( int argc, char **argv );
#else
	void ParseCommandLine( const char* cmdLine );
#endif

    void ScriptGameGC();

};

#ifdef TRANSGAMING_SHOW_FPS
extern int g_triCount;
extern int g_objectCount;
extern int g_vertexCount;
#endif

#ifdef TRANSGAMING
extern int totalFrameCount;
#endif

#endif //_PCFSYSTEM_


