#ifndef __SIMPLEGAME__H
#define __SIMPLEGAME__H

enum
{
    EModeLockedMouse       = (1<<1),
    EModeFlyMove           = (1<<2),
    EModeOnlyZones         = (1<<3),
    EModePortals           = (1<<4),
    EModeZonePortals       = (1<<5),
    EModeShowTris          = (1<<6),
    EModeShowHUD           = (1<<8),
    EModeShowDebugHUD      = (1<<9),
	EModeShowWaypoints     = (1<<10),
	EModeShowFloors        = (1<<11),
	EModeShowStaticMeshes  = (1<<12),
};

class DemoGame : public BaseGame
{
    typedef VertexBuffer<Vector,RenderDevice::EVertexPos>  LineBuffer;


public:
    DemoGame();
    ~DemoGame();

	//WaypointSet			mWaypoints;
	WaypointPath		mCurrentZonePath;
	WaypointGPath		mCurrentZoneGPath;
    int                 RenderMode;

	virtual BOOL Initialize();
    virtual void Close();
    virtual void Tick(float delta);
    BOOL    StartWorld(const char* mapFile, const float scale);
	void Resize();
    void ChangeMode(bool fullscreen);
    HWND GetWindow();
	int GetWindowHeight();
	int GetWindowWidth();
    Win32InputSystem*   GetInputSystem();
	bool mActive,mUpdateLogic;
	void SetMode(DWORD mode);
	DWORD GetMode();
	Camera * GetCamera() {return &mCamera;};
	World *GetWorld() {return &mWorld;};
	HUD *GetHUD() {return &mHUD;};
	//WaypointSet *GetWayPoints() {return &mWaypoints;};	
	Viewport *GetViewport() {return &mViewport;};
	void RenderLine(const Vector& p1, const Vector& p2, DWORD color );
	void RenderBox(const BoundingBox& box, DWORD color);

private:

    BOOL    InitRenderer();
    void    LoadScreen();


    void    RenderZones();
    void    RenderNormals();
    void    RenderTriangles();
    void    RenderWorld();
    void    ReadInput();
	HWND    hwndFullScreen;

    Mesh*   PickObject( Vector& hitPoint );

private:
    Win32InputSystem*   mInputSystem;
    World               mWorld;
    Camera              mCamera;
    Viewport            mViewport;
    ParticleSystem      mParticleSystem;

    PhysicsEngine*      mPhysics;
    HUD                 mHUD;
    LineBuffer          VBuf;
    DWORD               Mode;

    int                 CurrentZone;

    float               Contrast,Gamma,Brightness;

    int                 LogoTexRef;
};

#endif