// $Header: /Game/Source/Engine/Render/Video.cpp 30    9/10/04 5:54p Bart $

#include "Engine.h"
#include "Render/HUD.h"
#include "Render/RenderWindow.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;


GraphicsDevice::GraphicsDevice() :
	RenderDev(0)
,	RenderWnd(0)
,	MaterialSys(0)
,	IsFullScreen(0)
,   ShadowsEnabled(1)
,	WarpEffectsEnabled(0)
,	BloomEnabled(0)
,	CoronasEnabled(false)
,	RenderDevType( RenderDeviceManager::ENullRenderDevice )
,	mActiveView(0)
	{
        m_setTag("HUD");
		Hud = new HUD();
        m_setTag("Menu");
		MBoard = new MagicBoard();
        m_setTag("Menu");
		MapSel = new MapSelect();
        m_setTag("Menu");
		PMenu = new PainMenu();
	}

GraphicsDevice::~GraphicsDevice()
{
	while( mViews.Size() )
		delete mViews[0];

	PMenu->Clear();
	delete PMenu;
	PMenu = NULL;

	delete MapSel;
	MapSel = NULL;

	delete MBoard;
	MBoard = NULL;

	ParticleSys.RenderCleanup();
	Hud->RenderCleanup();
#ifdef TRANSGAMING_USE_EDITOR
	LineBuf.RenderCleanup();
#endif
	TrailSys.RenderCleanup();
	DecalSys.RenderCleanup();

	if( MaterialSys )
	{
		delete MaterialSys;
		MaterialSys = 0;
	}

	if( RenderDev )													// Close render device
	{
		RenderDev->Close();
		RenderDeviceManager::DestroyRenderDevice( RenderDev );
		RenderDevType = RenderDeviceManager::ENullRenderDevice;
		RenderDev = 0;
	}

	if( RenderWnd )
	{
		RenderWnd->Destroy();
		delete RenderWnd;
		RenderWnd = 0;
	}

	delete Hud;
	Hud = NULL;
}

// Shortcuts
HWND GraphicsDevice::FullScreenHWND() const
{
	return RenderWnd->GetHandle();
}

BOOL GraphicsDevice::Init( int renderDeviceType, int xRes, int yRes, DWORD initOpts )
{
	// Find out x and y resolution
	if( !xRes || !yRes )
	{
		xRes = 640;
		yRes = 480;
	}

	if( xRes > 3000 || yRes > 3000 ) return FALSE;

	// Find out if fullscreen or not
	BOOL fullscreen = FALSE;
	if( initOpts == EInitFullScreen )
		fullscreen = TRUE;
	else
        fullscreen = FALSE;

	if( !RenderWnd )
	{
		// Create rendering window
        m_setTag("EngineWindow");
		RenderWnd = new RenderWindow;
		Assert( RenderWnd );
		Window::Resources res;
		RenderWnd->Initialize( GEngine->GetInstance(), res, "PKRender3D" );
        TRACE("GraphicsDevice::Init(): creating 320x200 window\n");
		RenderWnd->Create( "PainKiller", 0, 0, 320, 200, Window::EStylePopup );

		if( FALSE == fullscreen )
			RenderWnd->AdjustSize( xRes, yRes );
	}



	// Set resolution
	if( !SetRes( renderDeviceType, xRes, yRes, fullscreen ) )
		return FALSE;

	// Init subsystems
    if( !MaterialSys ){
        m_setTag("TextureMaterialSystem");
        MaterialSys = new MaterialSystem;
    }
	if( !MaterialSys->Init() )
	{
		GLog.Print("ERROR: Could not initialize MaterialSystem\n");
		return FALSE;
	}

	if( !Hud->Init() )
	{
		GLog.Print("ERROR: Could not initialize HUD\n");
		return FALSE;
	}

#ifdef TRANSGAMING_USE_EDITOR
	if( !LineBuf.Init() )
	{
		GLog.Print("ERROR: Could not initialize LineBuf\n");
		return FALSE;
	}
#endif

	if( !ParticleSys.Init() )
	{
		GLog.Print("ERROR: Could not initialize ParticleSystem\n");
		return FALSE;
	}

	if( !DecalSys.Init() )
	{
		GLog.Print("ERROR: could not initialize DecalSystem\n");
		return FALSE;
	}

	if( !TrailSys.Init() )
	{
		GLog.Print("ERROR: could not initialize TrailSystem\n");
		return FALSE;
	}
    
	return TRUE;
}

BOOL GraphicsDevice::SetRes( int renderDeviceType, int xRes, int yRes, BOOL fullscreen, int multisample)
{
	BOOL success = FALSE;

	if( RenderDev )		// Device already opened
	{ 
		// If it's the same device that was requested
		if( RenderDevType == renderDeviceType )
		{
			if( RenderDev->SetMode( xRes, yRes, fullscreen, multisample ) )
				success = TRUE;
		}
		else	// They want different device, free this one
		{
			RenderDev->Close();
			RenderDeviceManager::DestroyRenderDevice( RenderDev );
			RenderDevType = RenderDeviceManager::ENullRenderDevice;
			RenderDev = 0;
		}
	}

	if( !RenderDev )
	{
		RenderDev = RenderDeviceManager::CreateRenderDevice( renderDeviceType );
		if( RenderDev )
		{
			if( RenderDev->Open( RenderWnd->GetHandle() ) ) 
			{
				GLog.Print("GraphicsEngine: Created rendering device: %d\n", RenderDevType );

				if( !RenderDev->SetMode( xRes, yRes, fullscreen, multisample ) )
				{
					GLog.Print("GraphicsEngine: Warning - Could not init RenderDevice with: %d,%d %s, trying safe mode\n",
						xRes, yRes, fullscreen?"FullScreen":"Windowed" );

					if( !RenderDev->SetMode( 640, 480, TRUE ) )
					{
						GLog.Print("GraphicsEngine: Fatal - Safe mode didn't quite work, bailing out\n" );
						return FALSE;
					}
				}

				// Contrast,Gamma,Brightness values
				float contrast  =0.5f;
				float gamma     =1.0f;
				float brightness=0.5f;

				RenderDev->SetGamma( contrast, gamma, brightness );
				success = TRUE;
			}
		}
	}

	if( success )
	{
		IsFullScreen = fullscreen;
		RenderDevType = renderDeviceType;
	}


	return success;
}

void GraphicsDevice::SetViewAsActive( View* view ) 
{
    mActiveView = view; 
    ScriptObject GfxScript = GScript.Globals()["Tweak"]["Gfx"];
    view->DetailDistance = GfxScript["DetailTextureDistance"].GetFloat();
    view->SkinShaderLODDistance = GfxScript["SkinShaderLODDistance"].GetFloat();
};

View* GraphicsDevice::CreateView( void )
{
    m_setTag("Engine");
	View* view = new View;
	mViews.Add( view );
	return view;
}

void GraphicsDevice::AddView( View* view )
{
	for( int i = 0; i < mViews.Size(); ++i )
		if( mViews[i] == view )
			return;	// already in device
	mViews.Add( view );
}

void GraphicsDevice::RemoveView( View* view )
{
	for( int i = 0; i < mViews.Size(); ++i )
		if( mViews[i] == view )
		{
			mViews.FastRemove(i);
			return;
		}
}

void GraphicsDevice::DeleteView( View* view )
{
	for( int i = 0; i < mViews.Size(); ++i )
		if( mViews[i] == view )
		{
			mViews.FastRemove(i);
			delete view;
			return;
		}
}
