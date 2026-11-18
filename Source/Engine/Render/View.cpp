// ----------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.						
// Not for public release.
//
// $Header: /Game/Source/Engine/Render/View.cpp 278   05-06-02 13:24 Treppie $
// ----------------------------------------------------------------------------

#include "Engine.h"
#include <malloc.h>

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Render/Material.h"
#include "Render/HUD.h"
#ifndef TRANSGAMING
# include "Render/RenderWindow.h"
#endif
#include "Render/PostProcess.h"
#include "Render/Billboard.h"
#include "Render/View.h"
#include "Render/ParticleEffect.h"
#include "Render/Decal.h"
#include "Engine/Portal.h"
#include "Engine/Mesh.h"
#include "Engine/Model.h"
#include "Render/FogVolume.h"
#include "Engine/InputSystem.h"
#include "Engine/BaseGame.h"
#include "Render/Video.h"
#include "Render/Movie.h"
#include "Engine/World.h"
#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"
#include "Net/NetworkDevice2.h"
#include "Physics/PhysicsObject.h"

#ifndef TRANSGAMING_NO_MILES
#include "Audio/MilesEngine.h"
#endif

#ifdef PKTV_ENABLED
# include "Net/PKTVManager.h"
#endif

/* min and max macros */
#ifndef NOMINMAX
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */

enum 
{
	EDraw_NoWarps		= (1<<0),
	EDraw_NoShadows		= (1<<1),
	EDraw_NoGun			= (1<<2),
	EDraw_NoSky			= (1<<3),
	EDraw_ReflectPass	= (1<<20),
};

static void RenderQuad(int BlendMode, Texture* ColorTex, int x, int y, int w, int h);
//static void RenderQuadDebug(int BlendMode, Texture* ColorTex, int x, int y, int w, int h);

// ===========================================================================================================

//#define REFLECTION_PLANE_SHIFT	-0.2F

static FLOAT REFLECTION_PLANE_SHIFT = 0.2F;
static Plane ReflectionPlane;
static BoundingBox ReflectionBox;

static BOOL ReflectUseFog = false;
static FogState ReflectFogParams;
static Entity* ReflectEntity = NULL;

# if defined(TRANSGAMING_PS2) && defined(TRANSGAMING_SHOW_FPS)
void VIEW_RenderStats(void)
{
    float	timeStep	= GEngine->SysDriver.GetLastTimeStep();
    float	currentTime	= GEngine->SysDriver.GetCurrentTime();
    float	luaMem		= GScript.GetMemUsage() / 1024.0f;
    float	moduleSize	= m_getModuleSize();
    float	memUsed		= (mallinfo().uordblks + moduleSize)/ 1024.0f;
    extern float g_gameStartTime;
	extern int g_AnimatedMeshes;
	extern int g_AnimatedVerts[4];

    GEngine->Video->Hud->SetFont("timesbd", 30);
	GEngine->Video->Hud->Printf(40, 40, 0xffffffff, "IFPS: %0.3f\nAFPS: %0.3f\nTFPS: %0.3f\nObjs: %d\nTris: %d\nVtcs: %d\nMem:  %.3fKB\nLuaM: %.3fKB\nAnimMesh:%d\nAnimVtx: 0(%d) 1(%d) 2(%d) 3(%d)",
                                    1.0f / timeStep, 
                                    GEngine->avgFPS, 
                                    totalFrameCount / (currentTime - g_gameStartTime),
                                    g_objectCount,
                                    g_triCount,
                                    g_vertexCount,
                                    memUsed,
                                    luaMem,
                                    g_AnimatedMeshes,
                                    g_AnimatedVerts[0],g_AnimatedVerts[1],g_AnimatedVerts[2],g_AnimatedVerts[3]);

    g_triCount = 0;
    g_objectCount = 0;
    g_vertexCount = 0;
	g_AnimatedMeshes = 0;
	g_AnimatedVerts[0] = 0;
	g_AnimatedVerts[1] = 0;
	g_AnimatedVerts[2] = 0;
	g_AnimatedVerts[3] = 0;
}
#endif

static inline Matrix GetReflectionMatrix(const Plane& P)
{
	return Matrix
	(
		Plane(-2 * P.X * P.X + 1, -2 * P.Y * P.X    , -2 * P.Z * P.X    ,   0),
		Plane(-2 * P.X * P.Y    , -2 * P.Y * P.Y + 1, -2 * P.Z * P.Y    ,   0),
		Plane(-2 * P.X * P.Z    , -2 * P.Y * P.Z    , -2 * P.Z * P.Z + 1,   0),
		Plane(-2 * P.X * -P.W    , -2 * P.Y * -P.W    , -2 * P.Z * -P.W    ,   1)
	);
}

static inline Matrix GetClipProjMatrix(const Plane& InClipPlane, const Matrix& WorldToCamera, const Matrix& CameraToScreen)
{
	Plane ClipPlane = InClipPlane;
	ClipPlane.W = -ClipPlane.W;		// UGLY HACK ALERT!

	ClipPlane.Normalize();
	Matrix WorldToScreen = WorldToCamera * CameraToScreen;
	Matrix ScreenToWorld = WorldToScreen.Inverse().GetTranspose();

	ClipPlane = ScreenToWorld.TransformVec4(ClipPlane);

	if (ClipPlane.W == 0)
		return *GIdentityMatrix;

	if (ClipPlane.W > 0)
		ClipPlane = ScreenToWorld.TransformVec4(-ClipPlane);

	ClipPlane.Normalize();

	Matrix ClipProj;    // = GIdentityMatrix;


    ClipProj.SetIdentity();

	ClipProj.A02 = ClipPlane.X;
	ClipProj.A12 = ClipPlane.Y;
	ClipProj.A22 = ClipPlane.Z;
	ClipProj.A32 = ClipPlane.W;

	return ClipProj;
}

// ===========================================================================================================

// This is used for windowed instance of the Painkiller game
#ifndef TRANSGAMING
class RenderSystemWindow : public Window
{
public:
	LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		if( msg == WM_CLOSE )
		{
			GEngine->Close();
			Status = EClosed;
			return 1;
		}

		return 0;
	}
};
#endif

// =======================================================================

static inline void SetActiveMaterial(const char* Name)
{
	Material* M = GEngine->Video->MaterialSys->FindMaterial(Name);
	Assert(M);

	RenderPass& pass = M->Passes[0];	
#ifdef TRANSGAMING
    MaterialInput tmpMat;

	pass.Activate();
	pass.SetInput(&tmpMat);
#else
	pass.Activate();
	pass.SetInput(&MaterialInput());
#endif
}

// =======================================================================

View::View() :
	mWorld(0),
	Mode( EModeShowTextures|EModeShowMeshes|EModeVisibility|EModeShowVolFog ), HUDMode( EModeHUD_Show ),
	ShowFrustum(false), mScene(LocalViewport), DetailDistance(10.0f), SkinShaderLODDistance(10.0f),
	RenderTargetTex(NULL), RenderTargetDepth(NULL), 
#ifndef TRANSGAMING
    WindowedWindow(0), HudText(""), 
#endif
    mMovieName(""),
	PostProc(NULL),
	SizeX(0), SizeY(0)
{
}

View::~View()
{
#ifndef TRANSGAMING
	if( WindowedWindow ) 
	{
		WindowedWindow->Destroy();
		delete WindowedWindow;
		WindowedWindow = 0;
	}
#endif

	for( int i = 0; i < GEngine->Video->mViews.Size(); ++i )
	{
		if( GEngine->Video->mViews[i] == this )
		{
			GEngine->Video->mViews.FastRemove(i);
			break;
		}
	}

	if (PostProc)
		delete PostProc; 
}

void View::Init( void )
{
	mCamera.SetFOV(90);

#ifndef TRANSGAMING
	GEngine->InputSys->SetWindow( GetActiveWindow() );
#endif

	Mode = EModeVisibility|EModeShowTextures|EModeShowLightmaps|EModeShowModels|EModeShowVolFog;//|EModeShowSky;
	HUDMode = 0;

    m_setTag("Engine");
	PostProc = new FPostProcess();

	ReflectUseFog = 0;
	ReflectFogParams.Color = 0;
	ReflectFogParams.Start = 0;
	ReflectFogParams.End = 90;	
	ReflectFogParams.Mode = FogState::EFogLinear;
}

void View::PrecacheMeshes( void )
{
	HWND hwnd = GetActiveWindow();
#ifndef TRANSGAMING
//	HWND hwnd = GEngine->Video->IsFullScreen ? GEngine->Video->FullScreenHWND() : WindowedWindow->GetHandle();

    //eric-> this is to handle ignoring the input, processing and loading when the window loses focus,
    //  but since we always have focus and are always in fullscreen, this is useless.
	if( GEngine->Video->IsFullScreen && !GEngine->Video->RenderWnd->HasFocus )
		return;
#endif


	mCamera.SetFOV(90);
	const Vector storeCam = mCamera.GetPosition();
	mCamera.SetPosition(Vector(0,0,0));
	mCamera.InverseTransform();

	LocalViewport.ClearColor = 0;
	LocalViewport.Resize( hwnd );
	LocalViewport.Update( mCamera );

	RenderDevice* dev = GEngine->Video->RenderDev;
	if( dev->Lock(LocalViewport) )
	{
		for( int i = 0; i < mWorld->Meshes.Size(); ++i )
		{
			WorldMesh* mesh = mWorld->Meshes[i];

			mCamera.SetPosition(mesh->Box.GetCenter() + mesh->Box.GetExtent());
			mCamera.LookAt( mesh->Box.GetCenter() );

			Matrix cam = mCamera.GetViewMatrix() * ScaleMatrix( Vector( 1, 1, -1 ) );
			dev->SetCameraTransform( &cam );
			mesh->Draw( dev, 0 );
		}

		dev->Unlock();
	}

	mCamera.SetPosition(storeCam);
}

HWND View::GetActiveWindow()
{
#ifdef TRANSGAMING
	return 0;
#else
    if (!GEngine->Video) return NULL;

    return GEngine->Video->IsFullScreen ? GEngine->Video->FullScreenHWND() : WindowedWindow->GetHandle();
#endif
}

HWND View::GetTopmostWindow()
{
#ifdef TRANSGAMING
    return 0;
#else
    if (!GEngine->Video) return NULL;

	return GEngine->Video->IsFullScreen ? GEngine->Video->FullScreenHWND() : WindowedWindow->GetHandle();
#endif
}

void View::RecalcWindowAspect()
{
	if (GEngine->GetRenderDevice()->HWClass < RenderDevice::EHWClassNV20 || !mWorld->TexFB[1])
		return;

	Texture* Target = mWorld->TexFB[1];
	Assert(Target);

/*
#ifdef FINAL_BUILD

	RECT R;
	::GetWindowRect(GetActiveWindow(), &R);
	UINT WX = R.right  - R.left;
	UINT WY = R.bottom - R.top;

	RTSize[0] = Target->Width;
	RTSize[1] = Target->Height;

	RTAspect[0] = 1;
	RTAspect[1] = 1;

	SizeX = WX;
	SizeY = WY;

#else // FINAL_BUILD
*/
	RECT R;
	::GetWindowRect(GetActiveWindow(), &R);
	UINT WX = R.right  - R.left;
	UINT WY = R.bottom - R.top;

	if (WX == SizeX && WY == SizeY)
		return;

	float w = WX;
	float h = WY;
	//if ((w/2) > Target->Width)
	//	w = w/2;
	//if ((h/2) > Target->Height)
	//	h = h/2;

	while (w > Target->Width || h > Target->Height) {
		w *= 0.95F;
		h *= 0.95F;
	}
	
	RTSize[0] = (UINT) w;
	RTSize[1] = (UINT) h;

	RTAspect[0] = w / (float) Target->Width;
	RTAspect[1] = h / (float) Target->Height;

	SizeX = WX;
	SizeY = WY;

	GLog.Print("window (%d %d) target (%d %d) aspect (%.4f %.4f)\n",
		SizeX,SizeY,RTSize[0],RTSize[1],RTAspect[0],RTAspect[1]);
//#endif // FINAL_BUILD
}

static int fake = 0;
void View::FakeStuff()
{
	fake = !fake;
}

// ====================================================================================================

static BoundingBox RefractionBox;
static Plane RefractionPlanes[6];
//static ConvexPolygon DaPolyz[6];

static inline void CalcRefractionBox(const BoundingBox& inBox)
{
	BoundingBox box = inBox;
	Vector p1 = box.Min - ReflectionPlane.Normal * 4;
	box += p1;
	
	RefractionPlanes[0] = Plane(+1,0,0,box.Min.X);
	RefractionPlanes[1] = Plane(-1,0,0,-box.Max.X);
	RefractionPlanes[2] = Plane(0,+1,0,box.Min.Y);
	RefractionPlanes[3] = Plane(0,-1,0,-box.Max.Y);
	RefractionPlanes[4] = Plane(0,0,+1,box.Min.Z);
	RefractionPlanes[5] = Plane(0,0,-1,-box.Max.Z);

	//for (int i=0; i<6; ++i)
	//	DaPolyz[i].CreateFromPlane(DaPlanez[i],64);

	RefractionBox = box;
}

// =============================================================================================================

void View::Render( float timeDelta )
{
	if( !(Mode & EModeDoNotRenderWorld ) )
		RenderCubemap();

	HWND hwnd = GetActiveWindow();
	RenderDevice* dev = GEngine->Video->RenderDev;

	LocalViewport.ClearFlags = RenderDevice::EClearAll;

#ifdef TRANSGAMING
    //pick a nicer sky-ish colour for the clear colour
    if (!GEngine->Video->PMenu->Active() && !GEngine->Video->MBoard->Active() && !GEngine->Video->MapSel->Active()){
# ifdef FINAL_BUILD
	    LocalViewport.ClearColor = (mWorld && mWorld->UseFog) ? mWorld->Env.FogParams.Color:0x0;
# else
	    LocalViewport.ClearColor = (mWorld && mWorld->UseFog) ? mWorld->Env.FogParams.Color:0x00fdec9d;
# endif
#if defined(TRANSGAMING_PSP) || defined(TRANSGAMING_PS2)
        dev->SetVBlankState(true);
        //TRACE("View::Render(): disabling vblank state\n");
#endif
    }
    else{
        LocalViewport.ClearColor = 0;
#if defined(TRANSGAMING_PSP) || defined(TRANSGAMING_PS2)
        dev->SetVBlankState(true);
        //TRACE("View::Render(): enabling vblank state\n");
#endif
    }
#else
	LocalViewport.ClearColor = (mWorld && mWorld->UseFog) ? mWorld->Env.FogParams.Color:0x008a1a1a;
#endif
	LocalViewport.MinZ = 0.1f;
	LocalViewport.MaxZ = mWorld?mWorld->FarClipDist:4096;
	LocalViewport.Resize( hwnd );

	RecalcWindowAspect();

	// FIXME: this is a total voodoo, update sets fov, 
	// then setcameratoscreen sets projection matrix, 
	// then again update sets clipping planes based on projection matrix

#ifdef TRANSGAMING
	LocalViewport.Fov = mCamera.GetFOV();
	LocalViewport.SetCameraToScreen();
	LocalViewport.Update( mCamera );
#else
	LocalViewport.Update( mCamera ); 
	LocalViewport.SetCameraToScreen();
	LocalViewport.Update( mCamera );
#endif

	mScene.mWorld = mWorld;	

	if( mWorld && !(Mode & EModeDoNotRenderWorld ) && !( Mode & EModeFrozenOcclude ))
	{
		GProfiler.SetState(	CLIENT_RENDER_OCCLUDE );
		mWorld->TestSwitchZones( LocalViewport.EyePosition );
		mScene.Occlude( (Mode & EModeVisibility) != 0, (Mode & EModeNoAntiportals) != 0 );

#ifdef TRANSGAMING_NON_TNL
		if (mScene.NumReflectSurfs > 0 && (GEngine->GetRenderDevice()->HWClass >= RenderDevice::EHWClassNV20))
		{
			//ReflectionPlane = mScene.ReflectSurfs[0]->GetReflectionPlane();
			//REFLECTION_PLANE_SHIFT = mScene.ReflectSurfs[0]->Env.Link->Water.PlaneShift;
			//ReflectionPlane.Normalize();

			WorldMesh* Closest = NULL;
			float Dist = F_MAX;
			for (int i=0; i<mScene.NumReflectSurfs; ++i) {
				if (mScene.ReflectSurfs[i]->Distance < Dist) {
					Dist = mScene.ReflectSurfs[i]->Distance;
					Closest = mScene.ReflectSurfs[i];
				}
			}

			Assert(Closest);
			ReflectEntity = Closest;
			ReflectionPlane = Closest->GetReflectionPlane();
			
			ReflectionBox.Reset();

			const float Radius = Closest->Env.Link ? Closest->Env.Link->Water.ReflectRadius : 1024;
			const float ViewDist = Closest->Env.Link ? Closest->Env.Link->Water.ReflectDist : 1024;

			ReflectionBox += (Closest->Box.GetCenter() + Vector(Radius));
			ReflectionBox += (Closest->Box.GetCenter() - Vector(Radius));

			REFLECTION_PLANE_SHIFT = Closest->Env.Link ? Closest->Env.Link->Water.PlaneShift : 0;

			//ReflectFogParams.Color	= Closest->Env.Link ? Closest->Env.Link->FogParams.Color : 0;
			ReflectFogParams.Start	= Closest->Env.Link ? Closest->Env.Link->FogParams.Start : 0;
			ReflectFogParams.End	= Closest->Env.Link ? Closest->Env.Link->FogParams.End : 90;

			CalcRefractionBox(Closest->Box);

			int ReflectFlags = 0;
			for (int i=0; i<mScene.NumReflectSurfs; ++i)
				ReflectFlags |= mScene.ReflectSurfs[i]->ReflectSurf;

			// render stuff
			if (ReflectFlags & Entity::ERefractScene)
				RenderRefraction();			

			if (ReflectFlags & Entity::EReflectScene) {
				if (::sqrtf(Dist) < ViewDist) {
					RenderReflection(Closest->Env.Link ? (Closest->Env.Link->Water.ReflectSky ? 0 : EDraw_NoSky) : EDraw_NoSky);
				} else {
					RenderFakeReflection();
				}
			}

			// voodoo: restore viewport
		#ifdef FINAL_BUILD
			LocalViewport.ClearColor = (mWorld && mWorld->UseFog) ? mWorld->Env.FogParams.Color:0x0;
		#else
			LocalViewport.ClearColor = (mWorld && mWorld->UseFog) ? mWorld->Env.FogParams.Color:0x001a1a8a;
		#endif

			LocalViewport.MinZ = 0.1F;
			LocalViewport.MaxZ = mWorld?mWorld->FarClipDist:4096;

			LocalViewport.Resize( hwnd );
			LocalViewport.Update( mCamera ); 
			LocalViewport.SetCameraToScreen();
			LocalViewport.Update( mCamera );
		}
#endif  //TRANSGAMING_NON_TNL
	}


    bool hud = true;
#ifndef TRANSGAMING
# ifndef FINAL_BUILD
    if (GetAsyncKeyState( 74))
        hud = false;
# endif
#endif


	if( !(Mode & EModeDoNotRenderWorld ) )
	{
		GProfiler.SetState(	CLIENT_RENDER_RENDER_WORLD );

		LocalViewport.ClearFlags = RenderDevice::EClearColor;

		if( mWorld && dev->Lock( LocalViewport ) )
		{

			if (GEngine->Video->ShadowsEnabled)
				RenderShadowmaps(mScene);

			dev->SetScreenTransform(&LocalViewport.CameraToScreen);
			dev->SetCameraTransform(&LocalViewport.WorldToCamera);
			dev->SetObjectTransform(GIdentityMatrix);

			dev->SetFBRenderTarget();
			dev->ClearZ();

			bool ApplyBloom = false;

			if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
			{
				if (mWorld->DemonFX.Enabled)
				{
					RenderDemonFXWorld(mScene, false);
				}
				else if (mWorld->GhostsFX) 
				{
					RenderGhostFXWorld(mScene);
				}
				else
				{
					RenderWorld(mScene);			
				}
			}
#ifdef TRANSGAMING_NON_TNL
			else
			{
				if (mWorld->DemonFX.Enabled)
				{
					RenderDemonFXWorld(mScene, mWorld->DemonFX.Super);
				}
				else if (mWorld->GhostsFX) 
				{
					RenderGhostFXWorld(mScene);
				}
				else if (RenderTargetTex && RenderTargetTex->DeviceId != EInvalidIndex && 
					mWorld->TexSceneDepth && mWorld->TexSceneDepth->DeviceId != EInvalidIndex)
				{
					RenderWorldToTexture(mScene);
					RenderPostProcess();
				}
				else
				{
					RenderWorld(mScene);

					if (dev->HWClass > RenderDevice::EHWClassGenericTnL)
						ApplyBloom = (GEngine->Video->BloomEnabled && (mWorld->BloomFX.Multiplier > 0.0F));
				}
			}
#endif  //TRANSGAMING_NON_TNL

			dev->SetFog( false );

			
			//GEngine->Video->LineBuf.DrawBox(ReflectionBox, 0xFF00);
			//for (int i=0; i<6; ++i)
			//	GEngine->Video->LineBuf.DrawFilledPoly(DaPolyz[i],0xFF00);
		
			
			
			ApplyBloom = ApplyBloom && mWorld->TexFB[0] && dev->GrabBackBuffer(mWorld->TexFB[0], GetActiveWindow());
			if (ApplyBloom)
			{				
#ifdef TRANSGAMING_NON_TNL
				if (dev->HWClass >= RenderDevice::EHWClassNV30)
					PostProc->RenderBloom();
				else
#endif  //TRANSGAMING_NON_TNL
					PostProc->RenderBloomPS11();
			}
			
#ifndef TRANSGAMING     //editor specific rendering functions
			RenderPathfinder();

			if( Mode & EModeShowBoxes )
				RenderBoxes( mScene );

            // eric-> this render mode is only applied while in the editor
			if( Mode & EModeShowZones )
				RenderZones( mScene );
#endif

#ifdef TRANSGAMING_USE_EDITOR
			if (mWorld->PickedEntity && mWorld->PickedEntity->AsMesh())
			{
				const WorldMesh* Mesh = (const WorldMesh*) mWorld->PickedEntity->AsMesh();
				GEngine->Video->LineBuf.DrawMesh(Mesh, 0xFF0000);
			}
#endif

			GProfiler.SetState(	CLIENT_RENDER_LUA );

            if( hud && GEngine->Game && GEngine->Game->mUpdateLogic )
				//GScript.Call( "Game_PostRender", 0, "%f", timeDelta );
                ScriptGamePostRender(timeDelta);

			/*			
			if (mScene.NumReflectSurfs > 0) {
				GEngine->Video->Hud->SetFont( "verdana", 12 );
				for (int i=0; i<mScene.NumReflectSurfs; ++i) {
					GEngine->Video->Hud->Print( 0,i*16, mScene.ReflectSurfs[i]->Name.AsChar() );
				}
			}
			*/

			//#ifdef EDITOR
#ifdef PKTV_ENABLED
			if( GEngine->PKTV && GEngine->PKTV->IsPlaying() )
			{
				static char lbuf[1000];
				sprintf( lbuf, "DisplayTime: %.02f", GEngine->PKTV->GetDisplayTime() / 1000.0f );
				GEngine->Video->Hud->SetFont( "timesbd", 21 );
				GEngine->Video->Hud->Print( 640, 10, lbuf );
			}
#endif

			if( HUDMode & EModeHUD_Show )
			{
				GProfiler.SetState(	CLIENT_RENDER_HUD );
				RenderHUD( mScene );
#ifdef TRANSGAMING
                static const int    BUFSIZE = 32;
				static char         lbuf[BUFSIZE];


                snprintf(lbuf, BUFSIZE, "LUA: %d/%d/%d", GScript.GetGCCount(), GScript.GetGCThreshold(), GScript.GetTop());
#else
				static char lbuf[1000];
                sprintf( lbuf, "LUA: %d/%d%/%d", GScript.GetGCCount(), GScript.GetGCThreshold(), GScript.GetTop());
#endif
				GEngine->Video->Hud->Print( 3, 32, lbuf );
			}

			if( ( HUDMode & EModeHUD_ShowNetstat ) && GEngine->Net && GEngine->Video->Hud )
			{
				GProfiler.SetState(	CLIENT_RENDER_HUD );
				String netstats = GEngine->Net->GetNetstat();
				GEngine->Video->Hud->SetFont( "timesbd", 21 );
				GEngine->Video->Hud->Print( 30,30, netstats.AsChar() );
			}

			GProfiler.SetState(	CLIENT_RENDER );

			if (ApplyBloom)
			{
				/*
				RECT R;
				::GetWindowRect(GetActiveWindow(), &R);
				UINT W = R.right  - R.left;
				UINT H = R.bottom - R.top;

				Texture* T = mWorld->TexFB[2];

				RenderQuadDebug(RenderState::EBlendNone, T, 10, H-(T->Height)-32, T->Width, T->Height);				
				//RenderQuad(RenderState::EBlendAdd, T, 0, 0, W, H);				
				*/
				Texture* T = mWorld->TexFB[1];
				RenderQuad(RenderState::EBlendAdd, T, 0, 0, SizeX, SizeY);				
			}

			#ifdef EDITOR
			if (Mode & EModeFrozenOcclude)
			{
				GProfiler.SetState(	CLIENT_RENDER_HUD );
				GEngine->Video->Hud->SetRenderState();
				GEngine->Video->Hud->SetFont( 0 );
				GEngine->Video->Hud->Print( 680, 8, "Frozen Occlude");
			}
			#endif
			if (hud)
			{
//				GEngine->Video->Hud->MPStats.Draw();
#ifdef TRANSGAMING
				if( GEngine->Video->Hud->MPStats.Active() )
					GScript.DoString( "Hud._mpStatsDrawFunc()" );
					//GScript.Call("Hud._mpStatsDrawFunc", 0, NULL);
#else
				if( GEngine->Video->Hud->MPStats.Active() )
					GScript.DoString( "Hud._mpStatsDrawFunc()" );
#endif
	//		    if( GEngine->Video->Menu.Active() )
	//		    	GEngine->Video->Menu.Render();

				GEngine->Video->Hud->mConsole.Draw();
			}
			GProfiler.SetState(	CLIENT_RENDER );
			dev->Unlock();
			dev->Flip(hwnd);
		}       
    }

	if( mMovieName[0] )
	{
		PlayBink( mMovieName.AsChar() );
#ifdef TRANSGAMING
        mMovieName[0] = 0;
#else
		mMovieName = "";
#endif
	}

	if( GEngine->Video->PMenu->Active() )
	{
		GEngine->Video->PMenu->Screen.UpdateMovie();
//		if( GEngine->Video->PMenu->FrameChanged )
//		{

		static bool moviesPlayed = false;
		if( !moviesPlayed )
		{
#ifdef TRANSGAMING
			//GScript.Call("PainMenu:Draw", 0, NULL);
			GScript.DoString( "PainMenu:Draw()" );
#else
			GScript.DoString( "PainMenu:Draw()" );
#endif
			moviesPlayed = true;
		}

		if( dev->Lock( LocalViewport ) )
		{
			GEngine->Video->PMenu->CreatingFont = true;
			GEngine->Video->PMenu->Render();
			if( HUDMode & EModeHUD_Show )
			{
				GProfiler.SetState(	CLIENT_RENDER_HUD );
				RenderHUD( mScene );
			}

			if( ( HUDMode & EModeHUD_ShowNetstat ) && GEngine->Net && GEngine->Video->Hud )
			{
				GProfiler.SetState(	CLIENT_RENDER_HUD );
				String netstats = GEngine->Net->GetNetstat();
				GEngine->Video->Hud->SetFont( "timesbd", 21 );
				GEngine->Video->Hud->Print( 30,30, netstats.AsChar() );
			}

			GEngine->Video->Hud->mConsole.Draw();

			GProfiler.SetState(	CLIENT_RENDER );
			GEngine->Video->PMenu->FrameChanged = false;
			GEngine->Video->PMenu->CreatingFont = false;
			dev->Unlock();
			dev->Flip(hwnd);
		}
//		}
	}
	else if( GEngine->Video->MBoard->Active() )
	{
		if( dev->Lock( LocalViewport ) )
		{
			GEngine->Video->MBoard->Render();
			if( HUDMode & EModeHUD_Show )
			{
				GProfiler.SetState( CLIENT_RENDER_HUD );
				RenderHUD( mScene );
			}

			GEngine->Video->Hud->mConsole.Draw();

			GProfiler.SetState(	CLIENT_RENDER );
			dev->Unlock();
			dev->Flip(hwnd);
		}
	}
	else if( GEngine->Video->MapSel->Active() )
	{
		if( dev->Lock( LocalViewport ) )
		{
			GEngine->Video->MapSel->Render();
			if( HUDMode & EModeHUD_Show )
			{
				GProfiler.SetState( CLIENT_RENDER_HUD );
				RenderHUD( mScene );
			}

			GEngine->Video->Hud->mConsole.Draw();

			GProfiler.SetState(	CLIENT_RENDER );
			dev->Unlock();
			dev->Flip(hwnd);
		}
	}
#ifdef PKTV_ENABLED
	else if( GEngine->Video->PMenu->LoadScreen.Active() )
	{
        //TRACE("View::Render(): rendering load screen\n");
		if( GEngine->PKTV && GEngine->PKTV->IsConnecting() )
			GEngine->Video->PMenu->LoadScreen.RenderLoadScreen();
	}
#endif
}

void View::ScriptGamePostRender(float timeDelta){
    GScript.Call("Game_PostRender", 0, "%f", timeDelta);
}

void View::ScriptGameRender(){
    GScript.Call("Game_Render", 0, "%f", GEngine->LastDelta);    
}




void View::RenderCubemap()
{
	if (!mWorld->RenderToCubemap || !mWorld->TexSceneCubemap || !(Mode & View::EModeShowModels))
		return;

	mWorld->CubemapFrame = mWorld->Frame;
	float WaterLevel = mWorld->Water.WaterLevel;
	RenderDevice* dev = GEngine->Video->RenderDev;
	Camera camera = mCamera;
	mCamera.SetFOV(90);
	Vector camPos = camera.GetPosition();
	// HACK: water level != 0.0f means that we generate cubemap from mirrored camPos!
	if (WaterLevel!=0.0f)
		mCamera.SetPosition(Vector(camPos.X,2*WaterLevel-camPos.Y,camPos.Z));


#ifdef FINAL_BUILD
	LocalViewport.ClearColor = mWorld->UseFog?mWorld->Env.FogParams.Color:0x0;
#else
	LocalViewport.ClearColor = mWorld->UseFog?mWorld->Env.FogParams.Color:0x001a1a8a;
#endif
	LocalViewport.MinZ = 0.1f;
	LocalViewport.MaxZ = mWorld->FarClipDist;
	LocalViewport.Resize( mWorld->TexSceneCubemap->Width, mWorld->TexSceneCubemap->Width );
	LocalViewport.Width = mWorld->TexSceneCubemap->Width;
	LocalViewport.Height = mWorld->TexSceneCubemap->Width;

	mScene.mWorld = mWorld;

	//Mode &= ~View::EModeShowModels;
	Mode |= View::EModeRenderRTCubemap;
/*
	if( mWorld && !(Mode & EModeDoNotRenderWorld ) && !( Mode & EModeFrozenOcclude ))
	{
		GProfiler.SetState(	CLIENT_RENDER_OCCLUDE );
		GEngine->World->TestSwitchZones( mCamera.GetPosition() );
		mScene.Occlude( Mode & EModeVisibility, Mode & EModeNoAntiportals );
	}
*/
	Vector camForward = mCamera.GetForwardVector();
	Vector camPosition = mCamera.GetPosition();

	mCamera.UpdateCubemapViewMatrix();

	if( !(Mode & EModeDoNotRenderWorld ) )
	{
		float ang[6][2] = { {0,270}, {0,90}, {90,0}, {-90,0}, {0,0}, {0,180}};

		static int j = 0;
		if (++j>=2) j=0;

		for (int i=0; i<6; i++)
		{
			mCamera.SetViewMatrix(ScaleMatrix(Vector(1,1,-1))
				*TranslationMatrix(Vector(-camPosition.X,-camPosition.Y,camPosition.Z))
				*YRotationMatrix(FDeg2Rad(ang[i][1]))
				*XRotationMatrix(FDeg2Rad(ang[i][0])));

//			if (camForward.Dot(mCamera.GetForwardVector())<0)
//				continue;

			dev->SetRenderTarget(mWorld->TexSceneCubemap,mWorld->TexSceneDepth,i);
			dev->ClearFB(0xFF000000);
			dev->ClearZ();
			dev->SetViewport(0,0,mWorld->TexSceneCubemap->Width,mWorld->TexSceneCubemap->Width);

			LocalViewport.Update( mCamera );
			LocalViewport.SetCameraToScreen();

			dev->SetScreenTransform(&LocalViewport.CameraToScreen);
			dev->SetCameraTransform(&LocalViewport.WorldToCamera);
			dev->SetObjectTransform(GIdentityMatrix);


			if( mWorld && !(Mode & EModeDoNotRenderWorld ) && !( Mode & EModeFrozenOcclude ))
			{
				GProfiler.SetState(	CLIENT_RENDER_OCCLUDE );
#ifdef	TRANSGAMING
				GEngine->pcfWorld->TestSwitchZones( LocalViewport.EyePosition );
#else
				GEngine->World->TestSwitchZones( LocalViewport.EyePosition );
#endif
				mScene.Occlude( (Mode & EModeVisibility) != 0, (Mode & EModeNoAntiportals) != 0 );
			}

			if( mWorld && dev->Lock( LocalViewport ) )
			{
				GProfiler.SetState(	CLIENT_RENDER_RENDER_WORLD );
				RenderWorld( mScene );
				dev->Unlock();
			}
		}
		dev->SetFBRenderTarget();
	}

	mCamera = camera;
	Mode &= ~View::EModeRenderRTCubemap;
	mWorld->RenderToCubemap = false;
}

// =======================================================================================================================
void View::RenderFakeReflection()
{
	RenderDevice* dev = GEngine->Video->RenderDev;

	Texture* Target = mWorld->TexFB[1];

	const UINT W = Target->Width;
	const UINT H = Target->Height;

	LocalViewport.ClearFlags = RenderDevice::EClearAll;
	LocalViewport.ClearColor = 0;
	LocalViewport.MinZ = 0.1F;
	LocalViewport.MaxZ = 1.0F;	

	LocalViewport.Resize(W,H);
	LocalViewport.Width		= W;
	LocalViewport.Height	= H;

	// just clear the render target
	dev->SetRenderTarget(Target, mWorld->TexZBuf);
	if (dev->Lock(LocalViewport))
		dev->Unlock();

	dev->SetFBRenderTarget();
}

// =======================================================================================================================

void View::RenderReflection(int DrawFlags)
{
	RenderDevice* dev = GEngine->Video->RenderDev;

	Texture* Target = mWorld->TexFB[1];

	const UINT W = RTSize[0];
	const UINT H = RTSize[1];

	// save camera
	Camera SavedCam = mCamera;

/*
#ifdef FINAL_BUILD
	LocalViewport.ClearColor = mWorld->UseFog?mWorld->Env.FogParams.Color:0x0;
#else
	LocalViewport.ClearColor = mWorld->UseFog?mWorld->Env.FogParams.Color:0x001a1a8a;
#endif
*/
	LocalViewport.ClearFlags = RenderDevice::EClearZ|RenderDevice::EClearStencil;
	LocalViewport.ClearColor = 0;
	LocalViewport.MinZ = 0.1f;
	LocalViewport.MaxZ = ReflectFogParams.End;	

	//float Dist = ReflectionPlane.Distance(mCamera.GetPosition());
	//LocalViewport.MinZ = 1;
	//LocalViewport.MaxZ = (ReflectFogParams.End + Dist)/Dist;

	/*	
	LocalViewport.Resize(Target->Width, Target->Height);
	LocalViewport.Width		= SizeX;
	LocalViewport.Height	= SizeY;	
	*/

	LocalViewport.Resize(W,H);
	LocalViewport.Width		= W;
	LocalViewport.Height	= H;

	SceneRender Scene(LocalViewport);
	Scene.mWorld = mWorld;

	mCamera.SetViewMatrix(GetReflectionMatrix(ReflectionPlane) * SavedCam.GetViewMatrix() * ScaleMatrix(Vector(1,-1,1)));

	dev->SetRenderTarget(Target, mWorld->TexZBuf);
	dev->SetViewport(0,0,Target->Width,Target->Height);
	dev->ClearFB(0x00);
	//dev->ClearZ();
	//dev->SetViewport(0,0,W,H);

	//dev->SetViewport(0,0,Target->Width,Target->Height);
	//dev->SetCullFlip(1);

	LocalViewport.Update(mCamera);
	LocalViewport.SetCameraToScreen();
	
	//LocalViewport.ProjTweak = GetClipProjMatrix(ReflectionPlane,LocalViewport.WorldToCamera,LocalViewport.CameraToScreen);

	Plane P = ReflectionPlane;
	if (P.W < 0)
		P.W -= REFLECTION_PLANE_SHIFT;
	else
		P.W += REFLECTION_PLANE_SHIFT;

	LocalViewport.ProjTweak = GetClipProjMatrix(P,LocalViewport.WorldToCamera,LocalViewport.CameraToScreen);

	LocalViewport.SetCameraToScreen();
	LocalViewport.Update(mCamera);

	dev->SetScreenTransform(&LocalViewport.CameraToScreen);
	dev->SetCameraTransform(&LocalViewport.WorldToCamera);
	dev->SetObjectTransform(GIdentityMatrix);

	mWorld->TestSwitchZones(LocalViewport.EyePosition);
	//Scene.Occlude( (Mode & EModeVisibility) != 0, (Mode & EModeNoAntiportals) != 0 );
	//Scene.Occlude(false,false);

	Assert(ReflectEntity);
	if (ReflectEntity->Env.Link && ReflectEntity->Env.Link->Water.ReflectList.Size()) {
		Scene.OccludeReflection(ReflectEntity->Env.Link->Water.ReflectList, ReflectionPlane, ReflectionBox);
	} else {
		Scene.Occlude(false,false);
		for (int i=0; i<Scene.EntityList.Count; ++i)
			Scene.EntityList.Elems[i].EntPtr->VisCounter = mScene.VisCounter;
	}

	//LocalViewport.Width  = Target->Width;
	//LocalViewport.Height = Target->Height;

	if (dev->Lock(LocalViewport)) {

		//dev->SetViewport(0,0,W,H,LocalViewport.MinZ,LocalViewport.MaxZ);
		RenderWorld(Scene, EDraw_NoWarps|EDraw_NoShadows|EDraw_NoGun|EDraw_ReflectPass|DrawFlags);
		dev->Unlock();
	}

	dev->SetFBRenderTarget();

	// restore cam
	mCamera = SavedCam;
	//dev->SetCullFlip(0);

	LocalViewport.ProjTweak.SetIdentity();  // = GIdentityMatrix;
}

// =======================================================================================================================

void View::RenderRefraction()
{
	RenderDevice* dev = GEngine->Video->RenderDev;

	Texture* Target = mWorld->TexFB[2];

	const UINT W = RTSize[0];
	const UINT H = RTSize[1];

/*
#ifdef FINAL_BUILD
	LocalViewport.ClearColor = mWorld->UseFog?mWorld->Env.FogParams.Color:0x0;
#else
	LocalViewport.ClearColor = mWorld->UseFog?mWorld->Env.FogParams.Color:0x001a1a8a;
#endif
*/

	LocalViewport.ClearFlags = RenderDevice::EClearAll;
	LocalViewport.ClearColor = 0;
	LocalViewport.MinZ = 0.1f;
	LocalViewport.MaxZ = 90.0F;
	//LocalViewport.MaxZ = mWorld->FarClipDist;

	//LocalViewport.Resize(Target->Width, Target->Height);
	
	LocalViewport.Resize(W,H);
	LocalViewport.Width		= W;
	LocalViewport.Height	= H;
	//LocalViewport.Width		= SizeX;
	//LocalViewport.Height	= SizeY;	

	SceneRender Scene(LocalViewport);
	Scene.mWorld = mWorld;

	dev->SetRenderTarget(Target, mWorld->TexZBuf);
	dev->SetViewport(0,0,Target->Width,Target->Height);
	dev->ClearFB(0x00);

	//dev->SetRenderTarget(Target, mWorld->TexZBuf);

	/*
	dev->ClearFB(0xFF000000);
	dev->ClearZ();
	dev->SetViewport(0,0,Target->Width,Target->Height);
	*/

	Plane P = ReflectionPlane;
	if (P.W < 0)
		P.W += REFLECTION_PLANE_SHIFT;
	else
		P.W -= REFLECTION_PLANE_SHIFT;

	LocalViewport.Update(mCamera);
	LocalViewport.ProjTweak = GetClipProjMatrix(-P, LocalViewport.WorldToCamera, LocalViewport.CameraToScreen);
	LocalViewport.SetCameraToScreen();
	LocalViewport.Update(mCamera);

	dev->SetScreenTransform(&LocalViewport.CameraToScreen);
	dev->SetCameraTransform(&LocalViewport.WorldToCamera);
	dev->SetObjectTransform(GIdentityMatrix);

	//mWorld->TestSwitchZones(LocalViewport.EyePosition);
	//Scene.Occlude( (Mode & EModeVisibility) != 0, (Mode & EModeNoAntiportals) != 0 );	

	
	for (int i=0; i<6; ++i) {
		LocalViewport.ViewFrustum.BoundingPlanes[i] = -RefractionPlanes[i];
	//	LocalViewport.ViewFrustum.BoundingPlanes[i].W = -LocalViewport.ViewFrustum.BoundingPlanes[i].W;
		//LocalViewport.ViewFrustum.BoundingPlanes[i].Normalize();
	}
	//LocalViewport.ViewFrustum.BoundingPlanes[6] = P;
	//LocalViewport.ViewFrustum.PlaneCount = 7;
	

	Scene.Occlude(false,false);
	for (int i=0; i<Scene.EntityList.Count; ++i)
		Scene.EntityList.Elems[i].EntPtr->VisCounter = mScene.VisCounter;

	//LocalViewport.Width  = Target->Width;
	//LocalViewport.Height = Target->Height;

	if (dev->Lock(LocalViewport))
	{
		//dev->SetViewport(0,0,W,H,LocalViewport.MinZ,LocalViewport.MaxZ);
		RenderWorld(Scene, EDraw_NoWarps|EDraw_NoShadows|EDraw_NoGun|EDraw_ReflectPass|EDraw_NoSky);
		dev->Unlock();
	}

	dev->SetFBRenderTarget();

	LocalViewport.ProjTweak.SetIdentity();  // = GIdentityMatrix;
	//LocalViewport.ViewFrustum.PlaneCount = 6;
}


// =======================================================================================================================

#if 1

#define MAX_RECEIVERS	(16)

struct FShadowCaster
{
	Model*			ShadowCaster;
	ConvexVolume	CasterFrustum;
	Entity*			Receivers[MAX_RECEIVERS];
	int				NumReceivers;

	void AddEntity(Entity* E)
	{
		if (NumReceivers < MAX_RECEIVERS && CasterFrustum.BoxTest(E->Box.GetCenter(), E->Box.GetExtent()))
			Receivers[NumReceivers++] = E;
	}
};

#define MAX_CASTERS		(24)

static FShadowCaster	CasterList[MAX_CASTERS];
static int				NumCasters = 0;

void View::RenderShadowmaps(const SceneRender& scene)
{
    //this entire function is skipped because we're rendering with a TnL device
#ifndef TRANSGAMING
	Assert( mWorld );

	RenderDevice* dev = GEngine->Video->RenderDev;
	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
		return;

	NumCasters = 0;

	for( int a = 0; a < scene.EntityList.Count; ++a )
	{
		Entity* entity = scene.EntityList.Elems[a].EntPtr;

		if (NumCasters >= MAX_CASTERS)
			return;

		if (entity->AsModel() && !(entity->Flags&Entity::EEntNoDraw))
		{
			entity->Draw(dev, Mode|EModeShadowMapPass);
			if (!entity->AsModel()->ShadowMap)
				continue;

			FShadowCaster* C = &CasterList[NumCasters++];

			C->ShadowCaster = entity->AsModel();
			C->CasterFrustum = ConvexVolume(C->ShadowCaster->ShadowMatrix.Inverse());
			C->NumReceivers = 0;
		}
	}

	dev->SetFBRenderTarget();
#endif
}

static WorldMesh*	ShadowReceivers[1024];
static int			NumShadowReceivers = 0;

#endif

#if 0
void View::RenderShadowmaps(const SceneRender& scene)
{
	Assert( mWorld );

	RenderDevice* dev = GEngine->Video->RenderDev;
	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
		return;

	for( int a = 0; a < scene.EntityList.Count; ++a )
	{
		Entity* entity = scene.EntityList.Elems[a].EntPtr;

		if (entity->AsModel() && !(entity->Flags&Entity::EEntNoDraw))
			entity->Draw(dev, Mode|EModeShadowMapPass);
	}
}
#endif

void View::RenderGhostFXWorld(const SceneRender& scene)
{
	RenderDevice* dev = GEngine->Video->RenderDev;

	Texture *tex=mWorld->TexSceneMotionBlur[3];
	Texture *tex0=mWorld->TexSceneMotionBlur[mWorld->Frame%2];
	Texture *tex1=mWorld->TexSceneMotionBlur[(mWorld->Frame+1)%2];
	Texture *tex2=mWorld->TexSceneMotionBlur[2];

	
	struct Sortable 
	{
        Entity* Ent[4096];
		int Count;

		Sortable() : Count(0) {	}
		void Add(Entity* e) { Ent[Count++] = e; }

	} sortable[Entity::ESortNumEntries];


	for( int a = 0; a < scene.EntityList.Count; ++a )
	{
		Entity* entity = scene.EntityList.Elems[a].EntPtr;

		if( entity->Flags & Entity::EEntNoDraw )
			continue;

		sortable[entity->Sort].Add(entity);
	}

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
	{
		RenderWorld(scene);
		// render demons 

		for (int i=0; i<sortable[Entity::ESortOpaque].Count; ++i)
		{
			Entity *e = sortable[Entity::ESortOpaque].Ent[i];
			if (e->Demonic)
				e->Draw(dev, Mode|EModeGhostFX);
		}

		for( int i = sortable[Entity::ESortTrans].Count-1; i >= 0; --i )
		{
			Entity *e = sortable[Entity::ESortTrans].Ent[i];
			if (e->Demonic)
				e->Draw(dev, Mode|EModeMultiPass|EModeGhostFX);
		}
		return;
	}

	dev->SetRenderTarget(tex2,mWorld->TexSceneDepth);
	dev->SetViewport(0,0,tex2->Width,tex2->Height);
	dev->ClearFB(0x00);
	dev->ClearZ();

	for( int i = sortable[Entity::ESortOpaque].Count-1; i >= 0; --i )
	{
		Entity *e = sortable[Entity::ESortOpaque].Ent[i];
		if (e->Demonic)
			e->Draw(dev, Mode|EModeGhostFX);
	}

	dev->SetFog( false );

	Texture* texes0[] = {tex2,tex0};
	float args0[] = {0.2f, 0.8f};
	mWorld->ComposeTextures(tex1, texes0, args0, ARRAY_COUNT(texes0));

	Texture* texes1[] = {tex1,tex2};
	float args1[] = {1.0f, 0.0f};
	mWorld->ComposeTextures(tex, texes1, args1, ARRAY_COUNT(texes1));

	dev->SetFBRenderTarget();
	dev->ClearZ();

	RenderWorld(scene);

	HUD*		hud = GEngine->Video->Hud;
	View*		view = GEngine->Video->GetActiveView();
	Material*	mat = GEngine->Video->MaterialSys->CreateMaterial("hud_add");
	Texture*	notex = GEngine->Video->MaterialSys->Textures.NoTex;
#ifdef TRANSGAMING
    MaterialInput tmpMat(tex, notex, notex);

	hud->DrawQuadShader(mat, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height, &tmpMat);
#else
	hud->DrawQuadShader(mat, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height,
		&MaterialInput(tex,notex,notex));
#endif
}

void View::RenderWorldToTexture(const SceneRender& scene)
{
	RenderDevice* dev = GEngine->Video->RenderDev;
	dev->SetRenderTarget(RenderTargetTex,mWorld->TexSceneDepth);
	LocalViewport.Resize( RenderTargetTex->Width, RenderTargetTex->Height );
	LocalViewport.Update( mCamera );
	dev->ClearFB(LocalViewport.ClearColor);
	dev->ClearZ();

	RenderWorld(scene, EDraw_NoWarps);

	HWND hwnd = GetActiveWindow();
	LocalViewport.Resize( hwnd );
	LocalViewport.Update( mCamera );
}

void View::RenderPostProcess()
{
	RenderDevice* dev = GEngine->Video->RenderDev;
	dev->SetFog( false );
	Texture *tex=mWorld->TexSceneMotionBlur[3];

	mWorld->ComposeTextures(tex, PostProcess.Tex, PostProcess.Alpha, PostProcess.PassNum);

	dev->SetFBRenderTarget();
	dev->ClearZ();

	HUD*		hud = GEngine->Video->Hud;
	View*		view = GEngine->Video->GetActiveView();
	Material*	mat = GEngine->Video->MaterialSys->CreateMaterial("hud");
	Texture*	notex = GEngine->Video->MaterialSys->Textures.NoTex;
#ifdef TRANSGAMING
    MaterialInput tmpMat(tex, notex, notex);

	hud->DrawQuadShader(mat, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height, &tmpMat);
#else
	hud->DrawQuadShader(mat, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height,
		&MaterialInput(tex,notex,notex));
#endif
}

void View::RenderDemonFXWorld(const SceneRender& scene, bool SuperDemon)
{
	RenderDevice* dev = GEngine->Video->RenderDev;

	Texture *tex0=mWorld->TexSceneMotionBlur[2];					// current scene
	Texture *tex1=mWorld->TexSceneMotionBlur[mWorld->Frame%2];		// new framebuffer
	Texture *tex2=mWorld->TexSceneMotionBlur[(mWorld->Frame+1)%2];	// old framebuffer
	Texture *tex3=mWorld->TexSceneMotionBlur[mWorld->Frame%2+3];	// new superdemon
	Texture *tex4=mWorld->TexSceneMotionBlur[(mWorld->Frame+1)%2+3];// old superdemon
		
	struct Sortable 
	{
        Entity* Ent[4096];
		int Count;

		Sortable() : Count(0) {	}
		void Add(Entity* e) { Ent[Count++] = e; }

	} sortable[Entity::ESortNumEntries];


	for( int a = 0; a < scene.EntityList.Count; ++a )
	{
		Entity* entity = scene.EntityList.Elems[a].EntPtr;

		if( entity->Flags & Entity::EEntNoDraw )
			continue;

		sortable[entity->Sort].Add(entity);
	}

	if (dev->HWClass == RenderDevice::EHWClassGenericTnL)
	{
		RenderWorld(scene);
		// render demons 

		for (int i=0; i<sortable[Entity::ESortOpaque].Count; ++i)
		{
			Entity *e = sortable[Entity::ESortOpaque].Ent[i];
			if (e->Demonic)
				e->Draw(dev, Mode|EModeDemonFX);
		}

		for( int i = sortable[Entity::ESortTrans].Count-1; i >= 0; --i )
		{
			Entity *e = sortable[Entity::ESortTrans].Ent[i];
			if (e->Demonic)
				e->Draw(dev, Mode|EModeMultiPass|EModeDemonFX);
		}
		return;
	}


	LocalViewport.Resize(tex1->Width, tex1->Height);
	LocalViewport.Update(mCamera);

	// render world to tex1

	dev->SetRenderTarget(tex1,mWorld->TexSceneDepth);	
	dev->ClearFB(LocalViewport.ClearColor);
	dev->ClearZ();

	RenderWorld(scene, EDraw_NoWarps);

	// do grayscale post-processing on world from tex0 to tex1

	dev->SetRenderTarget(tex0,mWorld->TexSceneDepth);

	dev->SetFog( false );

	HUD*		hud = GEngine->Video->Hud;
	Material*	mat0 = GEngine->Video->MaterialSys->CreateMaterial("hud_grayscaleinv");
	Material*	mat1 = GEngine->Video->MaterialSys->CreateMaterial("hud_embm");
	Material*	mat2 = GEngine->Video->MaterialSys->CreateMaterial("hud");
	Material*	mat3 = GEngine->Video->MaterialSys->CreateMaterial("hud_trans");
#ifdef TRANSGAMING
    Texture*	nmap = GEngine->Video->MaterialSys->Textures.NoTex;
#else
    Texture*	nmap = GEngine->Video->MaterialSys->CreateTexture("special/warp_dudv",Texture::ETexNoMipMaps|Texture::ETexDuDv|Texture::ETexNoLOD );
#endif
	Texture*	notex = GEngine->Video->MaterialSys->Textures.NoTex;

	float s = mWorld->DemonFX.Scale*.25f;
	float b = mWorld->DemonFX.Bias*.25f;
#ifdef TRANSGAMING
    Plane tmpPlane;
    MaterialInput tmpMat(tex1, notex, notex);


	b = 0.25f-b-s;
    tmpPlane.set(0.3f,0.59f,0.11f,0);
	dev->SetPixelShaderMemory(0, &tmpPlane, 1);
	tmpPlane.set(s,s,s,s);
    dev->SetPixelShaderMemory(1, &tmpPlane, 1);
	tmpPlane.set(b,b,b,b);
	dev->SetPixelShaderMemory(2, &tmpPlane, 1);

	hud->DrawQuadShader(mat0, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height, &tmpMat);
#else
	b = 0.25f-b-s;
	dev->SetPixelShaderMemory(0,&Plane(0.3f,0.59f,0.11f,0),1);
	dev->SetPixelShaderMemory(1,&Plane(s,s,s,s),1);
	dev->SetPixelShaderMemory(2,&Plane(b,b,b,b),1);

	hud->DrawQuadShader(mat0, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height,
		&MaterialInput(tex1,notex,notex));
#endif

	if (SuperDemon)
	{
		// render demons into tex1
		dev->SetRenderTarget(tex1,mWorld->TexSceneDepth);	
		dev->ClearFB(0xFF0000);
	}

    dev->SetScreenTransform(&LocalViewport.CameraToScreen);
    dev->SetCameraTransform(&LocalViewport.WorldToCamera);
    dev->SetObjectTransform(GIdentityMatrix);


	for (int i=0; i<sortable[Entity::ESortOpaque].Count; ++i)
	{
		Entity *e = sortable[Entity::ESortOpaque].Ent[i];
		if (e->Demonic)
			e->Draw(dev, Mode|EModeDemonFX);
	}

	for( int i = sortable[Entity::ESortTrans].Count-1; i >= 0; --i )
	{
		Entity *e = sortable[Entity::ESortTrans].Ent[i];
		if (e->Demonic)
			e->Draw(dev, Mode|EModeMultiPass|EModeDemonFX);
	}

	dev->SetFog( false );

	if (SuperDemon)
	{
		Texture* texes0[] = {tex1,tex4};
		float args0[] = {0.3f, 0.7f};
		mWorld->ComposeTextures(tex3, texes0, args0, ARRAY_COUNT(texes0));

		dev->SetRenderTarget(tex0,mWorld->TexSceneDepth);

#ifdef TRANSGAMING
        MaterialInput tmpMat4(tex3, notex, notex);

		hud->DrawQuadShader(mat3, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height, &tmpMat4);
#else
		hud->DrawQuadShader(mat3, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height,
	    	&MaterialInput(tex3,notex,notex));
#endif
	}


	// warp & motion blur pass

	dev->SetRenderTarget(tex1,mWorld->TexSceneDepth);

	float a0 = mWorld->DemonFX.MBlur0;
	float a1 = mWorld->DemonFX.MBlur1;
#ifdef TRANSGAMING
    tmpPlane.set(a0,a0,a0,a0);
	dev->SetPixelShaderMemory(0,&tmpPlane,1);
    tmpPlane.set(a1,a1,a1,a1);
	dev->SetPixelShaderMemory(1,&tmpPlane,1);
    tmpPlane.set(0,0,0,0);
	dev->SetShaderMemory(10,&tmpPlane,1);
	dev->SetShaderMemory(11,&tmpPlane,1);
	dev->SetShaderMemory(12,&tmpPlane,1);
	dev->SetShaderMemory(13,&tmpPlane,1);

	TextureState st[1];
	float w = mWorld->DemonFX.Warp;
    ScaleMatrix tmpMatrix(Vector(w, w, 0));

    st[0].BumpPtr = &tmpMatrix;
	st[0].Stage = 1;
	dev->SetTextureState(st,ARRAY_COUNT(st));


    MaterialInput tmpMat2(tex0, tex2, nmap);

	hud->DrawQuadShader(mat1, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height, &tmpMat2);
#else
	dev->SetPixelShaderMemory(0,&Plane(a0,a0,a0,a0),1);
	dev->SetPixelShaderMemory(1,&Plane(a1,a1,a1,a1),1);
	dev->SetShaderMemory(10,&Plane(0,0,0,0),1);
	dev->SetShaderMemory(11,&Plane(0,0,0,0),1);
	dev->SetShaderMemory(12,&Plane(0,0,0,0),1);
	dev->SetShaderMemory(13,&Plane(0,0,0,0),1);

	TextureState st[1];
	float w = mWorld->DemonFX.Warp;
    st[0].BumpPtr = &ScaleMatrix(Vector(w,w,0));
	st[0].Stage = 1;
	dev->SetTextureState(st,ARRAY_COUNT(st));


	hud->DrawQuadShader(mat1, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height, &MaterialInput(tex0, tex2, nmap));
#endif


	dev->SetFBRenderTarget();
	dev->ClearZ();
	HWND hwnd = GetActiveWindow();
	LocalViewport.Resize(hwnd);
	LocalViewport.Update(mCamera);

#ifdef TRANSGAMING
	hud->DrawQuadShader(mat2, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height, &tmpMat);
#else
	hud->DrawQuadShader(mat2, 0.0f, 0.0f, LocalViewport.Width, LocalViewport.Height,
		&MaterialInput(tex1,notex,notex));
#endif
}

void View::RenderZ(Sortable* sortList)
{
	RenderDevice* dev = GEngine->Video->RenderDev;
	dev->ResetTextureStages();

	SetActiveMaterial("zfill_only");
	dev->SetColorWrite(0,0);

	for (int i=0; i<sortList->Count; ++i) {
		Entity* e = sortList->Ent[i];
		WorldMesh* mesh = e->AsMesh();
		if (mesh)
			mesh->DrawZ(LocalViewport.WorldToScreen);
	}

	dev->SetColorWrite(1,1);
}

/*
static void SetWarpConsts(RenderDevice* Device, Entity* E, const Matrix& WorldToScreen)
{
	Matrix ObjectToWorld = (E->Type == Entity::ETypeParticleFx && !((ParticleEffect*)E)->FixedTransform) 
							? GIdentityMatrix
							: E->ObjectToWorld;

	const Matrix ViewMat =	  ObjectToWorld 
							* WorldToScreen
							* ScaleMatrix(Vector(0.5F,-0.5F,1))
							* TranslationMatrix(Vector(0.5F, 0.5F, 0));

	Device->SetShaderMemory(40, &ViewMat.GetTranspose()[0], 4);
}
*/

#define MAX_MULTIPASS_WARPS	16

void View::RenderWorld(const SceneRender& scene, DWORD DrawFlags)
{
	Assert( mWorld );

	RenderDevice* dev = GEngine->Video->RenderDev;

	// for volumes
	Matrix C2WInvTran = LocalViewport.WorldToCamera;
	C2WInvTran.Transpose();

	// setup sorted table of entities
	Sortable sortable[Entity::ESortNumEntries];

	// warp stuff
	Entity* ClosestWarp = NULL;
	Entity* MultiPassWarps[MAX_MULTIPASS_WARPS];
	int NumMultiPassWarps = 0;
	FLOAT WarpDist = F_MAX;
	Mode &= ~EModeGrabFB;

	NumShadowReceivers = 0;

	for( int a = 0; a < scene.EntityList.Count; ++a )
	{	
		Entity* entity = scene.EntityList.Elems[a].EntPtr;

		if( entity->Flags & Entity::EEntNoDraw )
			continue;

		if ((DrawFlags & EDraw_ReflectPass) && entity->ReflectSurf)
			continue;

		entity->UpdateVolEntity();

#if 1 //#ifdef TRANSGAMING_ENABLE_LIGHTING
		// loop through inflictiong lights
		switch (entity->Lighting)
		{
			case Entity::ELightingDLights:
			{
				entity->ResetLights();
				for (int i = 0; i < mWorld->DLights.Size(); ++i)
				{
					Light* light = mWorld->DLights[i];
					if (light)
					{
						// filter lights
						if (light->GetType() == Light::ELightNone)
							continue;


						if (light->Intersect(entity->Box))
							entity->AddLight( light );
					}
				}
			}
			break;
			case Entity::ELightingAllLights:
			{
				entity->ResetLights();
				for (int i = 0; i < mWorld->Lights.Size(); ++i)
				{
					Light* light = mWorld->Lights[i];
					if (light)
					{
						// filter lights
						if (light->GetType() == Light::ELightNone)
							continue;

						// ugly hack (don't draw spotlights on gun)
						if (light->GetType() == Light::ELightSpot && 
							entity->AsModel() && 
							entity->Sort == Entity::ESortGun)
							continue;

						if( light->Intersect(entity->Box) )
							entity->AddLight( light );
					}
				}
			}
			break;
		}
#endif

		if (entity->AsVolume())
		{
			Volume* volume = entity->AsVolume();
			volume->FarPlane = C2WInvTran.TransformPlane(Plane(0,0,1,volume->VolEnd));

			if (volume->FarPlane.BoxTest(volume->Box.GetCenter(), volume->Box.GetExtent()))
				sortable[entity->Sort].Add(entity);
		}
		else
		{
			const FLOAT EntityDist = scene.EntityList.Elems[a].Distance;
			//if (entity->WarpScreen && EntityDist < 2.0F)
			//	continue;

			sortable[entity->Sort].Add(entity);	

			//if (GEngine->Video->WarpEffectsEnabled && entity->WarpScreen && entity->Sort == Entity::ESortTrans)

			// NOTE: even if warps are disabled, these multi pass warps should have a path
			// when warps are not supported by hardware (models, glass)
#ifdef TRANSGAMING_ENABLE_WARPS
            if (/*GEngine->Video->WarpEffectsEnabled &&*/ entity->WarpScreen && entity->Sort != Entity::ESortWarp)
			{
				if (EntityDist < WarpDist)
				{
					ClosestWarp = entity;
					WarpDist = EntityDist;
				}

				if (NumMultiPassWarps < MAX_MULTIPASS_WARPS)
					MultiPassWarps[NumMultiPassWarps++] = entity;
			}
#endif

#ifdef TRANSGAMING_ENABLE_SHADOWS
			if (!(DrawFlags & EDraw_NoShadows) && GEngine->Video->ShadowsEnabled)
			{
				if ((entity->Sort == Entity::ESortOpaque || entity->Sort == Entity::ESortDecal) && entity->AsMesh())
				{
					/*
					for (int Index=0; Index<NumCasters; ++Index)
					{
						FShadowCaster* C = &CasterList[Index];
						C->AddEntity(entity);
					}
					*/

					WorldMesh* M = entity->AsMesh();
					M->NumShadowCasters = 0;

					if (!(M->Flags & WorldMesh::EWater)) {

						for (int Index=0; Index<NumCasters; ++Index) {
							if (M->NumShadowCasters == MAX_MESH_SHADOWS)
								break;

							FShadowCaster* C = &CasterList[Index];
							if (C->CasterFrustum.BoxTest(M->Box.GetCenter(), M->Box.GetExtent())) {
								M->ShadowCasters[M->NumShadowCasters++] = C->ShadowCaster;
							}
						}

						if (M->NumShadowCasters > 0)
							ShadowReceivers[NumShadowReceivers++] = M;
					}
				}
			}
#endif
		}
	}

	/*
	// do we need depth map?
	//if (GEngine->Video->WarpEffectsEnabled && mWorld->TexFrame[3] 
	//	&& (sortable[Entity::ESortWarp].Count > 0 || ClosestWarp))
	{		
		SetActiveMaterial("depthmap_only");

		Texture* Dst = mWorld->TexFB[2];

		dev->SetRenderTarget(0, Dst);
		dev->SetDepthTarget(mWorld->TexZBuf);

		dev->SetViewport(0, 0, Dst->Width, Dst->Height);
		dev->SetFog(false);
		dev->ClearZ();
		dev->ClearFB();

		for (int i=0; i<sortable[Entity::ESortOpaque].Count; ++i)
		{
			Entity* E = sortable[Entity::ESortOpaque].Ent[i];
			if (E->Type == Entity::ETypeMesh)								// FIXME: models too!
				((WorldMesh*) E)->DrawZ(LocalViewport.WorldToScreen);
		}

		dev->SetFBRenderTarget();
		dev->SetFog(mWorld->UseFog && !(Mode&View::EModeShowTris));
	}	
	*/

	// --------------------- drawing begins ----------------------------
	//RenderZ(&sortable[Entity::ESortOpaque]);

	Viewport &pViewport = LocalViewport;

	// ==========================================================================================
	// setup fog
	// ==========================================================================================

#ifdef TRANSGAMING_ENABLE_FOG
	float volfogScale;
	const float volfogBias	= 0;

	float fogStart;
	float fogEnd;
	float fogBias;
	float fogScale;

	if (DrawFlags & EDraw_ReflectPass) {

		dev->SetFog(ReflectUseFog && !(Mode&View::EModeShowTris));
		if (ReflectUseFog)
			dev->SetFogState(ReflectFogParams.UseVSFog());

		// Setup constant memory

		volfogScale	= 1/ReflectFogParams.VolEnd;
		fogStart	= ReflectFogParams.Start;
		fogEnd		= ReflectFogParams.End;
		fogBias		= -fogEnd/(fogStart-fogEnd);
		fogScale	= 1/(fogStart-fogEnd);

		const Plane Fog( fogBias, fogScale, volfogBias, volfogScale);
		
		dev->SetShaderMemory(4,&LocalViewport.WorldToScreen.GetTranspose()[0],4);
		dev->SetShaderMemory(8,&Plane(27.5,255*3,1,0.5),1);
		dev->SetShaderMemory(9,&Fog,1);

	} else {

		dev->SetFog( mWorld->UseFog && !(Mode&View::EModeShowTris) );
		if( mWorld->UseFog )
			dev->SetFogState( mWorld->Env.FogParams.UseVSFog() );

		// Setup constant memory

		volfogScale	= 1/mWorld->Env.FogParams.VolEnd;
		fogStart	= mWorld->Env.FogParams.Start;
		fogEnd		= mWorld->Env.FogParams.End;
		fogBias		= -fogEnd/(fogStart-fogEnd);
		fogScale	= 1/(fogStart-fogEnd);

		const Plane Fog( fogBias, fogScale,	volfogBias, volfogScale);
		
		dev->SetShaderMemory(4,&LocalViewport.WorldToScreen.GetTranspose()[0],4);
		dev->SetShaderMemory(8,&Plane(27.5,255*3,1,0.5),1);
		dev->SetShaderMemory(9,&Fog,1);
	}
#endif

	/*
	if (DrawFlags & EDraw_ReflectPass) {
		
		dev->SetFog(0);

	} else {

		dev->SetFog( mWorld->UseFog && !(Mode&View::EModeShowTris) );

		if( mWorld->UseFog )
		{
			dev->SetFogState( mWorld->Env.FogParams.UseVSFog() );
		}

		// Setup constant memory

		float volfogScale		= 1/mWorld->Env.FogParams.VolEnd;
		const float volfogBias	= 0;
		const float fogStart	= mWorld->Env.FogParams.Start;
		const float fogEnd		= mWorld->Env.FogParams.End;
		const float fogBias		= -fogEnd/(fogStart-fogEnd);
		const float fogScale	= 1/(fogStart-fogEnd);
		const Plane Fog( fogBias, fogScale,	volfogBias, volfogScale);
		
		dev->SetShaderMemory(4,&LocalViewport.WorldToScreen.GetTranspose()[0],4);
		dev->SetShaderMemory(8,&Plane(27.5,255*3,1,0.5),1);
		dev->SetShaderMemory(9,&Fog,1);
	}
	*/


	// ==========================================================================================
	// draw stuff
	// ==========================================================================================

	// draw opaque zpass
	for (int i=0; i<sortable[Entity::ESortOpaque].Count; ++i)
		sortable[Entity::ESortOpaque].Ent[i]->Draw(dev, Mode|EModeZPass);

//#ifdef TRANSGAMING_ENABLE_SKY
	if ((Mode & EModeShowSky) && !(DrawFlags & EDraw_NoSky))
	//if (Mode & EModeShowSky)
		mWorld->mSky.Draw();
//#endif


	// [Alex] Commented this pass for now because it's broken when used with display lists
	// and it's not needed for the demo (responsible for dynamic lighting)
/*
	// draw opaque multipass
	for (int i=0; i<sortable[Entity::ESortOpaque].Count; ++i)
		sortable[Entity::ESortOpaque].Ent[i]->Draw(dev, Mode|EModeMultiPass);
*/

	// draw decals
#ifdef TRANSGAMING_ENABLE_DECALS
	pViewport.SetCameraToScreen(1.01f);
	dev->SetScreenTransform(&pViewport.GetCameraToScreen());
//	dev->SetShaderMemory(4,&LocalViewport.WorldToScreen.GetTranspose()[0],4);

	for (int decals=Entity::ESortDecal; decals<=Entity::ESortDecalDyn; decals++)
	{
		for( int i=0; i<sortable[decals].Count; ++i )
			sortable[decals].Ent[i]->Draw( dev, Mode|EModeZPass );

		for( int i = sortable[decals].Count-1; i >= 0; --i )
			sortable[decals].Ent[i]->Draw( dev, Mode|EModeMultiPass );
	}

	pViewport.SetCameraToScreen();
	dev->SetScreenTransform(&pViewport.GetCameraToScreen());
//	dev->SetShaderMemory(4,&LocalViewport.WorldToScreen.GetTranspose()[0],4);
#endif

#if 0
	// draw shadows
	if (GEngine->Video->ShadowsEnabled)
		for (int i=0; i<sortable[Entity::ESortOpaque].Count; ++i)
			sortable[Entity::ESortOpaque].Ent[i]->Draw(dev, Mode|EModeShadowProjPass);
#endif

#ifdef TRANSGAMING_ENABLE_SHADOWS
	// draw shadows
	if (!(DrawFlags & EDraw_NoShadows) && GEngine->Video->ShadowsEnabled) {
		for (int i=0; i<NumShadowReceivers; ++i) {
			WorldMesh* M = ShadowReceivers[i];
			M->DrawShadows();
		}

	}
#endif

#ifdef TRANSGAMING_ENABLE_WARPS
	if (!(DrawFlags & EDraw_NoWarps))
	{
		if (GEngine->Video->WarpEffectsEnabled) {
			if (sortable[Entity::ESortWarp].Count > 0 || ClosestWarp) {
				if (dev->GrabBackBuffer(mWorld->TexFB[0], GetActiveWindow())) {
					for( int i = sortable[Entity::ESortWarp].Count-1; i >= 0; --i )
					{
						Entity* E = sortable[Entity::ESortWarp].Ent[i];
						//SetWarpConsts(dev, E, LocalViewport.WorldToScreen);
						E->Draw(dev, Mode|EModeMultiPass);
					}		

					if (ClosestWarp) {
						for (int i=0; i<NumMultiPassWarps; ++i) {
							//SetWarpConsts(dev, MultiPassWarps[i], LocalViewport.WorldToScreen);
							MultiPassWarps[i]->Draw(dev, Mode|EModeWarpPass);
						}
					}
				}
			}
		} else {
			if (ClosestWarp) {
				for (int i=0; i<NumMultiPassWarps; ++i) {
					//SetWarpConsts(dev, MultiPassWarps[i], LocalViewport.WorldToScreen);
					MultiPassWarps[i]->Draw(dev, Mode|EModeWarpPass);
				}
			}
		}

		/*
		// grab FB for warp sorted
		if (GEngine->Video->WarpEffectsEnabled && mWorld->TexFB[0] && sortable[Entity::ESortWarp].Count > 0)
		{
			Mode |= EModeGrabFB;
			if (!dev->GrabBackBuffer(mWorld->TexFB[0], GetActiveWindow()))
			{
				Mode &= ~EModeGrabFB;
			}	
		}

		// draw warps
		if (Mode & EModeGrabFB && mWorld->TexFB[0])
		{
			for( int i = sortable[Entity::ESortWarp].Count-1; i >= 0; --i )
			{
				Entity* E = sortable[Entity::ESortWarp].Ent[i];
				SetWarpConsts(dev, E, LocalViewport.WorldToScreen);
				E->Draw(dev, Mode|EModeMultiPass);
			}		
		}

		if (ClosestWarp)
		{
			//Mode |= EModeGrabFB;
			//dev->GrabBackBuffer(mWorld->TexFB[0], GetActiveWindow());
			//ClosestWarp->Draw(dev, Mode|EModeWarpPass);

			if (!(Mode & EModeGrabFB))
				dev->GrabBackBuffer(mWorld->TexFB[0], GetActiveWindow());

			for (int i=0; i<NumMultiPassWarps; ++i)
				MultiPassWarps[i]->Draw(dev, Mode|EModeWarpPass);
		}

		Mode &= ~EModeGrabFB;
		*/
	}
#endif

	/*	
	// draw trans
	if (!(DrawFlags & EDraw_NoWarps) && (Mode & EModeGrabFB && mWorld->TexFB[0]))
	{
		for( int i = sortable[Entity::ESortTrans].Count-1; i >= 0; --i )
		{
			Entity* E = sortable[Entity::ESortTrans].Ent[i];

			if (E->WarpScreen)
			{
				if (!dev->GrabBackBuffer(mWorld->TexFB[0], GetActiveWindow()))
					Mode &= ~EModeGrabFB;

				Matrix ObjectToWorld = (E->Type == Entity::ETypeParticleFx && !((ParticleEffect*)E)->FixedTransform) 
								     ? GIdentityMatrix
									 : E->ObjectToWorld;

				const Matrix ViewMat =	  ObjectToWorld 
										* LocalViewport.WorldToScreen
										* ScaleMatrix(Vector(0.5F,-0.5F,1))
										* TranslationMatrix(Vector(0.5F, 0.5F, 0));

				dev->SetShaderMemory(40, &ViewMat.GetTranspose()[0], 4);
			}

			E->Draw( dev, Mode|EModeMultiPass );
		}
	}
	else
	*/

	// Particles draw
	{
		for( int i = sortable[Entity::ESortTrans].Count-1; i >= 0; --i )
			sortable[Entity::ESortTrans].Ent[i]->Draw( dev, Mode|EModeMultiPass );
	}

#ifdef TRANSGAMING_ENABLE_FOG
	// draw volumes
	if( Mode & EModeShowVolFog )
	{
		dev->SetFog( false );

		for( int f = sortable[Entity::ESortVolume].Count-1; f >= 0; --f )
		{
			Volume *fvol = (Volume *)sortable[Entity::ESortVolume].Ent[f];

#ifdef	TRANSGAMING
			const Plane Fog( fogBias, fogScale,	volfogBias, 1.0f/min(GEngine->pcfWorld->FarClipDist-1,fvol->VolEnd));
#else
			const Plane Fog( fogBias, fogScale,	volfogBias, 1.0f/min(GEngine->World->FarClipDist-1,fvol->VolEnd));
#endif
			dev->SetShaderMemory(9,&Fog, 1);
			//dev->SetShaderMemory( 10, &Plane(fvol->VolColor), 1 );
			
			Plane color = fvol->VolColor;
			if (mWorld->BloomFX.Multiplier > 0.0F)
				color.Normal *= mWorld->BloomFX.DimScale;

			dev->SetShaderMemory( 10, &color, 1 );
			

			fvol->DrawBackPass( dev );
			for( int i = 0; i < fvol->DEntities.Size(); ++i )
			{
				Entity* e = fvol->DEntities[i];
				if (scene.IsVisible(e))// && fvol->FarPlane.BoxTest(e->Box.GetCenter(), e->Box.GetExtent()))
					e->Draw( dev, Mode|EModeVolPass );
			}


			for( int i = 0; i < fvol->Meshes.Size(); ++i )
			{
				WorldMesh* m = fvol->Meshes[i];
				if (scene.IsVisible(m))// && fvol->FarPlane.BoxTest(m->Box.GetCenter(), m->Box.GetExtent()))
					m->Draw( dev, Mode|EModeVolPass );
			}

			fvol->DrawFrontPass( dev );
		}

		for( int i = 0; i < mWorld->Volumes.Size(); ++i )
			mWorld->Volumes[i]->DEntities.Clear();
	}

	// draw coronas
	for( int i = sortable[Entity::ESortCorona].Count-1; i >= 0; --i )
		sortable[Entity::ESortCorona].Ent[i]->Draw( dev, Mode|EModeMultiPass );
#endif


	if (!(DrawFlags & EDraw_NoGun))
	{
		{
			Viewport view = pViewport;
			view.zNear = 0.0f;
			view.zFar = 0.3f;
			dev->SetViewport(0,0,view.ViewportWidth,view.ViewportHeight,view.zNear,view.zFar);
		}

		// draw gun zpass
		for( int i = 0; i < sortable[Entity::ESortGun].Count; ++i )
			sortable[Entity::ESortGun].Ent[i]->Draw( dev, Mode|EModeZPass );

		// draw gun multipass
		for( int i = 0; i < sortable[Entity::ESortGun].Count; ++i )
			sortable[Entity::ESortGun].Ent[i]->Draw( dev, Mode|EModeMultiPass );

		{
			dev->SetViewport(0,0,pViewport.ViewportWidth,pViewport.ViewportHeight,pViewport.zNear,pViewport.zFar);  
		}    
	}      
    
    int state = GProfiler.GetState();
    GProfiler.SetState(	CLIENT_RENDER_LUA );

    if(GEngine->Game && GEngine->Game->mUpdateLogic )
        //GScript.Call( "Game_Render", 0, "%f", GEngine->LastDelta);    
        ScriptGameRender();

    GProfiler.SetState(	state);
}


void View::RenderHUD(const SceneRender& scene)
{
#ifndef TRANSGAMING
    HUD*            hud = GEngine->Video?GEngine->Video->Hud:NULL;
    RenderDevice*   dev = GEngine->Video?GEngine->Video->RenderDev:NULL;
#ifdef TRANSGAMING
    String          HudText;
#endif


    if (hud) hud->SetFont( 0 );

    HudText = String::Sprintf("FPS: %0.3f\n", GEngine->avgFPS);
    HudText += String::Sprintf("ZPEDTC: %d %d %d %d %d %d\n",
			scene.StatZones, scene.StatPortals,
			scene.StatEntities, dev->LastFrameDIPs, dev->LastFrameTris, scene.CoronaCount);

	float sum = 0;
	for (int i=0; i<RenderDevice::EMemTexEntries; i++)
		sum += dev->TextureMemory[i];

	HudText += String::Sprintf("VMem (%.1f-%.1f)", sum,dev->TextureMemory[RenderDevice::EMemTexAvailable]);

	char *names[] = {
		"U:","D:", "H:", "I:", "L:", "M:", "P:", "S:", "S:", "W:", 0
	};

	for (int i=0; i<RenderDevice::EMemTexEntries; i++)
		HudText += String::Sprintf(" %s %.1f", names[i], dev->TextureMemory[i]);

	HudText += String::Sprintf(" IBs: %.1f VBs: %.1f", 
		dev->TextureMemory[RenderDevice::EMemIBs],
		dev->TextureMemory[RenderDevice::EMemVBs]);

	if( !(HUDMode&EModeHUD_FPSOnly) )
	{
		HudText += String::Sprintf("\n\n\nCAM: %.2f %.2f %.2f", mCamera.GetPosition().X, mCamera.GetPosition().Y, mCamera.GetPosition().Z);
#ifdef	TRANSGAMING
		if( GEngine->pcfWorld )
		{
			int zone = GEngine->pcfWorld->FindZone( LocalViewport.EyePosition );
			if( zone == EInvalidIndex )
				HudText += String::Sprintf(", eye zone: out of zones, ");
			else
				HudText += String::Sprintf(", eye zone: %d, ", zone);
			HudText += String::Sprintf( "switch zone: %s\n", GEngine->pcfWorld->LastSwitchZoneName.AsChar() );
		}
#else
		if( GEngine->World )
		{
			int zone = GEngine->World->FindZone( LocalViewport.EyePosition );
			if( zone == EInvalidIndex )
				HudText += String::Sprintf(", eye zone: out of zones, ");
			else
				HudText += String::Sprintf(", eye zone: %d, ", zone);
			HudText += String::Sprintf( "switch zone: %s\n", GEngine->World->LastSwitchZoneName.AsChar() );
		}
#endif

		HudText += String::Sprintf("\n");

		if( GEngine->Net )
		{
			ulong32 sent, received;
			GEngine->Net->GetLastUpdateStats( sent, received );
			String sentBytes( 'O', sent/1000 );
			String recvBytes( 'O', received/1000 );
			HudText += String::Sprintf( "Bytes sent: %d %s\n", sent, sentBytes );
			HudText += String::Sprintf( "Bytes received: %d %s\n", received, recvBytes );
		}

#ifndef TRANSGAMING_NO_MILES
		if( GEngine->Audio )
		{
			int sounds2D, sounds3D, streams, sounds2DHeard, sounds3DHeard, streamsHeard;
			GEngine->Audio->GetSoundCounts( sounds2D, sounds3D, streams, sounds2DHeard, sounds3DHeard, streamsHeard );
			HudText += String::Sprintf( "Sounds 2D: %d, 3D: %d, streams: %d, playing 2D: %d, 3D: %d, streams: %d\n",
				sounds2D, sounds3D, streams, sounds2DHeard, sounds3DHeard, streamsHeard );
		}
#endif

		if( GProfiler.GiveNumberOfFrames() )
            HudText += GProfiler.GetTextInfo();
	}

    if (hud)
        hud->Print( 2, 2, HudText.AsChar() );
#endif
}

#ifndef TRANSGAMING
void View::RenderZones( const SceneRender& scene )
{
	Assert( mWorld );

	LineRender& lineBuf = GEngine->Video->LineBuf;
	RenderDevice* dev = GEngine->Video->RenderDev;

	dev->SetObjectTransform( GIdentityMatrix );

	for( int i = 0; i < mWorld->Zones.Size(); ++i )
		if( mWorld->Zones[i]->OccludeEnteredInLastFrame )
			lineBuf.DrawBox( mWorld->Zones[i]->BBox, 0xff );
		else
			lineBuf.DrawBox( mWorld->Zones[i]->BBox, 0x80 );

	for( int i = 0; i < mWorld->Portals.Size(); ++i )
	{
		DWORD color = 0x8000;
		const Portal* portal = mWorld->Portals[i];
		if( portal->OccludeEnteredInLastFrame )
			color = 0xff00;

		for( int j = 0; j < portal->Zones.Size(); ++j )
		{
			if (portal->Zones[j] == scene.EyeZone)
			{
				color = 0xff00aa;
				break;
			}
		}
		lineBuf.DrawPoly( portal->Poly, color );
	}

	lineBuf.DrawTranslucentZones(mWorld->Zones, 0x10);
	lineBuf.DrawAntiPortals(mWorld->AntiPortals, 0x101000);
}

void View::RenderBoxes( const SceneRender& scene )
{
	LineRender& lineBuf = GEngine->Video->LineBuf;
	RenderDevice* dev = GEngine->Video->RenderDev;

    dev->SetScreenTransform(&LocalViewport.CameraToScreen);
    dev->SetCameraTransform(&LocalViewport.WorldToCamera);
    dev->SetObjectTransform(GIdentityMatrix);

	RenderState state;
	state.SetDepthTest( false );
	dev->SetState( state );

	for( int i = 0; i < scene.EntityList.Count; ++i )
	{
		Entity* entity = scene.EntityList.Elems[i].EntPtr;
		DWORD color = 0xff0000;

		if( scene.EyeZone && entity->ZoneLinks[scene.EyeZone->Index].ZoneLink == scene.EyeZone )
			color = 0xffa1a1;

		lineBuf.DrawBox( entity->Box, color );
	}

	for( int i = 0; i < scene.CoronaCount; ++i )
	{
		Billboard* corona = scene.Coronas[i];
		lineBuf.DrawBox( corona->Box, 0xff0000 );
	}
}

void View::RenderPathfinder()
{
	Assert( mWorld );
	if( Mode & EModeNewWaypoints )
		mWorld->Paths2.Render( Mode & EModeShowWaypoints, Mode & EModeShowFloors );
	else
		mWorld->Paths.Render( Mode & EModeShowWaypoints, Mode & EModeShowFloors );
}
#endif


static void RenderQuad(int BlendMode, Texture* ColorTex, int x, int y, int w, int h)
{
	RenderDevice* dev=GEngine->Video->RenderDev;
	HUD* hud = GEngine->Video->Hud;

	hud->SetRenderState();

	RenderState state;
	state.SetDepthWrite(false);
	state.SetDepthTest(false);
	state.SetAlphaTest(false);
	state.SetCullMode(RenderState::ECullNone);	
	state.SetBlendMode(BlendMode);
	dev->SetState(state);

#ifdef	TRANSGAMING
	unsigned int C = GEngine->pcfWorld->BloomFX.OverlayColor;
#else
	const DWORD C = GEngine->World->BloomFX.OverlayColor;
#endif

	int unlock = hud->BuildQuad(	x+0-0.5f,y+h-0.5f, 0.0f,1.0f, C,
									x+0-0.5f,y+0-0.5f, 0.0f,0.0f, C,
									x+w-0.5f,y+h-0.5f, 1.0f,1.0f, C,
									x+w-0.5f,y+0-0.5f, 1.0f,0.0f, C);

	TextureState st[2];
	st[0].Set(0,TextureState::ETexOpModulate,TextureState::ETexArgDiffuse,TextureState::ETexFilterBilinearNoMips);
	st[1].Set(1,TextureState::ETexOpDisable);

	dev->SetTextureState(st,ARRAY_COUNT(st));

    dev->SetTexture(0,ColorTex->DeviceId);
    dev->SetTexture(1,EInvalidIndex);
	dev->Draw(RenderDevice::EPrimTriStrip,unlock,2);

	dev->SetTexture(0,EInvalidIndex);
}

/*
static void RenderQuadDebug(int BlendMode, Texture* ColorTex, int x, int y, int w, int h)
{
	RenderDevice* dev=GEngine->Video->RenderDev;
	HUD* hud = GEngine->Video->Hud;

	hud->SetRenderState();

	RenderState state;
	state.SetDepthWrite(false);
	state.SetDepthTest(false);
	state.SetAlphaTest(false);
	state.SetCullMode(RenderState::ECullNone);	
	state.SetBlendMode(BlendMode);
	dev->SetState(state);

	ScreenVertex* VPtr = hud->Buf.Lock(4);
	const DWORD C = 0xFFFFFF;

	VPtr[0] = ScreenVertex(x+0-0.5f,y+h-0.5f,0,1,C);
	VPtr[1] = ScreenVertex(x+0-0.5f,y+0-0.5f,0,0,C);
	VPtr[2] = ScreenVertex(x+w-0.5f,y+h-0.5f,1,1,C);
	VPtr[3] = ScreenVertex(x+w-0.5f,y+0-0.5f,1,0,C);

	TextureState st[2];
	st[0].Set(0,TextureState::ETexOpTexture,0,TextureState::ETexFilterBilinearNoMips);
	st[1].Set(1,TextureState::ETexOpDisable);

	dev->SetTextureState(st,ARRAY_COUNT(st));

    dev->SetTexture(0,ColorTex->DeviceId);
    dev->SetTexture(1,EInvalidIndex);
	dev->Draw(RenderDevice::EPrimTriStrip,hud->Buf.Unlock(),2);

	dev->SetTexture(0,EInvalidIndex);
}
*/
