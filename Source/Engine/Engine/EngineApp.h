#error This file is obselete

class ENGINE_API EngineApp
{
public:
    HINSTANCE       hInstance;
    SystemDriver    SysDriver;

    PhysicsEngine*  Physics;
	AudioDevice*	Audio;
	IPDriver*		Net;
    BaseGame*       Game;
    RenderDevice*   RenderDev;
    RenderWindow*   RenderWnd;
    MaterialSystem* MaterialSys;
	ParticleSystem	ParticleSys;
	DecalSystem		DecalSys;
#ifdef TRANSGAMING_USE_EDITOR
	LineRender		LineBuf;
#endif
    HUD             Hud;
	MenuSystem		Menu;

    int             RenderDevType;
    DWORD           IsFullScreen:1;
    DWORD           ExitRequested:1;
    DWORD           IsDedicated:1;
    DWORD           UsingShaders:1;
	double			TimeMultiplier;

	// Information extracted from command line in Initialize()
	// it's stored because it may start a multiplayer client later in the game
	bool			StartMultiplayerClient;
	String			ServerToConnectTo;
	String			PlayerNameToAssume;
	String			PasswordToGive;

public:
    enum 
	{ // init flags
        EInitUseConfig  = 0,
        EInitFullScreen = 1,
		EInitWindowed	= 2,
    };


public:
    EngineApp();
    ~EngineApp();

    BOOL Initialize(HINSTANCE hinstance, const char* cmdLine=0);
    void RunLoop();
    void TickEngine(bool minimized);
    void ResetTimer() { SysDriver.ResetTimer(); }
	void ResetAudio() { 
        delete Audio; 

        m_setTag("Engine");
        Audio = new AudioDevice; 
    }
	void SetTimeMultiplier(double t) { TimeMultiplier = t; }

    void Close() { ExitRequested=1; }

    RenderDevice*   GetDevice()         const { return RenderDev; }
    HINSTANCE       GetInstance()       const { return hInstance; }
    HWND            FullScreenHWND()    const { return RenderWnd->GetHandle(); }
	
	// default game,client and world
	EngineGame* GetGame();
	World*      GetWorld();
	Client*     GetClient();

    BOOL SetRes(int renderDeviceType, int xRes=0, int yRes=0, BOOL fullscreen=FALSE);
    BOOL Init(int renderDeviceType=RenderDeviceManager::ED3DRenderDevice, 
              int xRes=0, int yRes=0, DWORD initOpts=EInitUseConfig);

private:
    void CloseRenderDevice();
    void Shutdown();
};