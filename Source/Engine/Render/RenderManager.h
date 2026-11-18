#error This file is obselete

class ENGINE_API RenderManager
{
    enum
    {
        EModeLockedMouse    = (1<<1),
        EModeFlyMove        = (1<<2),
        EModeOnlyZones      = (1<<3),
        EModePortals        = (1<<4),
        EModeZonePortals    = (1<<5),
        EModeShowTris       = (1<<6),
        EModeShowHUD        = (1<<8),
        EModeShowDebugHUD   = (1<<9),
    };

public:

    enum 
	{ // init flags
        EInitUseConfig  = 0,
        EInitFullScreen = 1,
		EInitWindowed	= 2,
    };

    RenderManager();
    ~RenderManager();

    BOOL SetRes(int renderDeviceType, int xRes=0, int yRes=0, BOOL fullscreen=FALSE);
    BOOL Init(int renderDeviceType=RenderDeviceManager::ED3DRenderDevice, 
              int xRes=0, int yRes=0, DWORD initOpts=EInitUseConfig);
    void Close();
    BOOL ToggleFullScreen(BOOL toggle, int width=0, int height=0, Client* client=0);

	Client* CreateClient(HWND hwnd);
	void	RemoveClient(Client* client);
	Client* GetMainClient() const { return MainClient; }

	RenderWindow*	GetRenderWnd() const { return RenderWnd; }
	RenderDevice*	GetDevice()    const { return RenderDev; }

	MaterialSystem& GetMaterialSystem() { return MaterialSys; }
	HUD&			GetHUD() { return Hud; }
#ifdef TRANSGAMING_USE_EDITOR
    LineRender&		GetLineBuf() { return LineBuf; }
#endif

private:
    void CloseRenderDevice();

private:
	typedef DoubleLinkList<Client>	TheClient;

	TheClient*		ClientList;
	Client*			MainClient;

    int             RenderDevType;
    RenderWindow*   RenderWnd;
	RenderDevice*	RenderDev;
#ifdef TRANSGAMING_USE_EDITOR
	LineRender		LineBuf;
#endif
    ParticleSystem	ParticleSys;
	MaterialSystem	MaterialSys;
    HUD             Hud;
};
