
// #pragma once
#ifndef _VIDEO_
#define _VIDEO_


#ifndef TRANSGAMING
class RenderWindow;
#endif
class MaterialSystem;
class HUD;
class PainMenu;
class MagicBoard;
class MapSelect;

#ifdef TRANSGAMING_USE_EDITOR
# include "Render/LineRender.h"
#endif
#include "Render/ParticleSystem.h"
#include "Render/DecalSystem.h"
#include "Render/TrailSystem.h"

class ENGINE_API GraphicsDevice
{
	friend class View;
public:

    enum	 // Init flags
	{
        EInitUseConfig  = 0,
        EInitFullScreen = 1,
		EInitWindowed	= 2,
    };

	// Methods
	GraphicsDevice();
	~GraphicsDevice();

	BOOL Init( int renderDeviceType = RenderDeviceManager::ED3DRenderDevice, 
		int xRes = 0, int yRes = 0, DWORD initOpts = EInitUseConfig );

	BOOL SetRes( int renderDeviceType, int xRes = 0, int yRes = 0, BOOL fullscreen = FALSE, int multisample = 0);

	// View methods
	View* CreateView( void );
	void AddView( View* view );
	void RemoveView( View* view );
	void DeleteView( View* view );

	void SetViewAsActive( View* view );
	View* GetActiveView( void ) { return mActiveView; };

	// Shortcuts
#ifndef TRANSGAMING
	HWND            FullScreenHWND() const;
#endif

	// Variables
    RenderDevice*   RenderDev;
#ifndef TRANSGAMING
    RenderWindow*   RenderWnd;		// Fullscreen rendering window
#endif
    MaterialSystem* MaterialSys;

	ParticleSystem	ParticleSys;
	DecalSystem		DecalSys;
	TrailSystem		TrailSys;
#ifdef TRANSGAMING_USE_EDITOR
	LineRender		LineBuf;
#endif
    HUD             *Hud;
//	MenuSystem		Menu;
	PainMenu		*PMenu;
	MagicBoard		*MBoard;
	MapSelect		*MapSel;

    int             RenderDevType;

    DWORD           IsFullScreen:1;
    DWORD           ShadowsEnabled:1;
	DWORD			WarpEffectsEnabled:1;
	DWORD			BloomEnabled:1;
	bool			CoronasEnabled;

protected:
	DynamicArray<View*>	mViews;
	View* mActiveView;
};


#endif //_VIDEO_

