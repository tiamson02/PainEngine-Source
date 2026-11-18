#ifndef _SIMPLEGAME_
#define _SIMPLEGAME_

typedef VertexBuffer<Vector,RenderDevice::EVertexPos>  LineBuffer;

class DemoGame : public BaseGame
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
	~DemoGame();
	DemoGame();

    virtual BOOL Initialize();
    virtual void Close();
    virtual void Tick(float delta);

private:

    BOOL	InitRenderer();
    void	LoadScreen();

    void	RenderZones();
    void	RenderNormals();
    void	RenderTriangles();
    void	RenderWorld();
    void	ReadInput();
    void	RenderTriangles(const Mesh& mesh);

    const	Mesh* PickObject();

private:
    Win32InputSystem*   mInputSystem;
    World               mWorld;
    Camera              mCamera;
    Viewport            mViewport;
    ParticleSystem      mParticleSystem;

    PhysicsEngine*      mPhysics;
    HUD                 mHUD;
    LineBuffer          VBuf;
    int                 RenderMode;
    DWORD               Mode;

    int                 CurrentZone;

    float               Contrast,Gamma,Brightness;

    int                 LogoTexRef;
};


#endif	//_SIMPLEGAME_
