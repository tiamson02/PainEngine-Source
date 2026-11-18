
#include "Core/LeString.h"
#include "Render/Camera.h"
#include "Render/Viewport.h"
#include "Render/SceneRender.h"

#ifndef TRANSGAMING
class RenderSystemWindow;
#endif
class SceneRender;
class Material;
class Texture;
class FPostProcess;

class ENGINE_API View
{
	friend class GraphicsDevice;

	// setup sorted table of entities
	struct Sortable 
	{
		Entity* Ent[4096];
		int Count;

		Sortable() : Count(0) {	}
		void Add(Entity* e) { Ent[Count++] = e; }
	};

public:
    enum RenderModes
    {
		EModeVisibility			= (1<<0),
        EModeShowZones			= (1<<1),
        EModeShowPortals		= (1<<2),
		EModeShowBoxes			= (1<<3),
		EModeShowTextures		= (1<<4),
		EModeShowLightmaps		= (1<<5),
        EModeShowTris			= (1<<6),
        EModeShadowProjPass		= (1<<8),
        EModeShowJoints			= (1<<9),
		EModeShowWaypoints		= (1<<10),
		EModeShowFloors			= (1<<11),
		EModeShowModels			= (1<<12),
		EModeShowVolumes		= (1<<13),
        EModeShowHKPrimitives	= (1<<14),
		EModeShowSky			= (1<<15),
		EModeShowMeshes			= (1<<16),
        EModeShowVolFog			= (1<<17),
		EModeWarpPass			= (1<<18),
		EModeFrozenOcclude		= (1<<20),
		EModeNewWaypoints		= (1<<21),
		EModeNoAntiportals		= (1<<22),
		EModeRenderRTCubemap	= (1<<23),

		EModeDoNotRenderWorld	= (1<<24),	// means do not render the world at all. use it when in menu

		EModeGrabFB				= (1<<25),
		EModeDemonFX			= (1<<26),
		EModeGhostFX			= (1<<27),
		EModeShadowMapPass		= (1<<28),
		EModeVolPass			= (1<<29),
		EModeMultiPass			= (1<<30),
		EModeZPass				= (1<<31),
    };

	enum RenderHudModes
	{
		EModeHUD_Show			= (1<<0),
		EModeHUD_FPSOnly		= (1<<1),
		EModeHUD_ShowNetstat	= (1<<2),
	};

	// Methods
	View();
	virtual ~View();

	void Init();
	void SetWorld( World* world ) { mWorld = world; };
	void Render( float timeDelta );

	void PrecacheMeshes( void );

//	void LoadingScreen();
#ifdef TRANSGAMING
	// At runtime, our devkit doesn't like these virtual.
	HWND GetActiveWindow();
	HWND GetTopmostWindow();
#else
	virtual HWND GetActiveWindow();
	virtual HWND GetTopmostWindow();
#endif

	void PlayMovie( const char *name ) { mMovieName = name; }
	void FakeStuff();

	// Variables
#ifndef TRANSGAMING
	RenderSystemWindow*		WindowedWindow;
#endif

	bool				ShowFrustum;
	QWORD				Mode;
	unsigned char		HUDMode;

	Camera				mCamera;            //245 bytes (packed)
	Viewport			LocalViewport;      //584 bytes (packed)
	World*				mWorld;
	SceneRender			mScene;             //42696 bytes (packed)
	Texture*			RenderTargetTex;
	Texture*			RenderTargetDepth;

	const static	int	TEX_MBLUR=3;

	struct sPostProcess {
		int				PassNum;
		Texture*		Tex[TEX_MBLUR];
		float			Alpha[TEX_MBLUR];
	} PostProcess;

	FPostProcess*		PostProc;
	float				DetailDistance;
	float				SkinShaderLODDistance;
#ifndef TRANSGAMING
    String              HudText;
#endif

	UINT	SizeX,SizeY;
	UINT	RTSize[2];		// size of viewport that maintains aspect ration
	float	RTAspect[2];	// aspect


protected:
	// Helper methods
	void RenderZ(Sortable* sortList);
    void RenderWorld(const SceneRender& scene, DWORD DrawFlags = 0);
	void RenderCubemap();
	void RenderWorldToTexture(const SceneRender& scene);
	void RenderPostProcess();
	void RenderDemonFXWorld(const SceneRender& scene, bool SuperDemon);
	void RenderGhostFXWorld(const SceneRender& scene);
	void RenderShadowmaps(const SceneRender& scene);
#ifndef TRANSGAMING
    void RenderZones(const SceneRender& scene);
	void RenderBoxes(const SceneRender& scene);
	void RenderPathfinder();
	void RenderNormals(const SceneRender& scene);
	void RenderVolumes();
#endif
    void RenderHUD(const SceneRender& scene);
	void RecalcWindowAspect();

	void RenderReflection(int DrawFlags);
	void RenderFakeReflection();
	void RenderRefraction();

private:

    void ScriptGamePostRender(float timeDelta);
    void ScriptGameRender();

	String	mMovieName;
};
