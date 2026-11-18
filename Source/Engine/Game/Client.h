#error This file is obselete
class ENGINE_API Client
{
public:
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
	Client();
	~Client();

	void SetWindow(HWND hwnd=0) { RenderHWND=hwnd; }
    void LockMouse(BOOL lock);
	void Tick(float timeDelta, FWorld& world);

#ifndef TRANSGAMING
	void RenderWorld(int renderMode, FWorld& world);
	void RenderZones(FWorld& world);
#endif
	void DrawHUD();

    int  GetMode() const { return Mode; }

protected:
    // overridables
    virtual void FrameSetup(float timeDelta, FWorld& world);
	virtual void Render(FWorld& world);

private:
    int                 Mode;
	HWND				RenderHWND;
    Camera              mCamera;
    Viewport            mViewport;
};
