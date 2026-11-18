#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "Core/FileSystem/FileManagerStandard.h"
extern ENGINE_API GFileManager  	GFileMan;

#include "Engine/PainMenu.h"
#include "Engine/MapSelect.h"
#include "Engine/InputSystem.h"
#include "Engine/BaseGame.h"


#ifndef TRANSGAMING
ProgressIcon::~ProgressIcon()
{
	if( tex && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( tex );
}


void ProgressIcon::Set( int x, int y, String name )
{
	this->x = x;
	this->y = y;
	this->name = name;
	if( tex )
		GEngine->Video->MaterialSys->ReleaseTexture( tex );

	if( !name.IsNull() )
		tex = GEngine->Video->MaterialSys->CreateTexture( name.AsChar(), Texture::ETexNoMipMaps | Texture::ETexNoLOD );
}


void ProgressIcon::Draw()
{
	if( !tex )
		return;

	HUD *hud = GEngine->Video->Hud;
	View *view = GEngine->Video->GetActiveView();

	float screenWidth = ( float )view->LocalViewport.Width;
	float screenHeight = ( float )view->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

	hud->DrawQuad( tex, x * scaleX, y * scaleY, tex->OriginalWidth * scaleX, tex->OriginalHeight * scaleY );
}
#endif


LoadingScreen::LoadingScreen() : mActive( false ), mProgress( 0 ), mOverall( 0 ), mPercent( 0.0f ),
					  mAdvance( 0.0f ), mBackground( NULL ), mProgressBar( NULL ), 
					  mLevel( NULL ), 
#ifndef TRANSGAMING
                      mNumber( NULL ), 
#endif
                      mRing( NULL ), mShadow( NULL )
{
	ScriptObject cfg = GScript.Globals()["Tweak"];
	// Blair...temporary : mSoundRepeat = cfg["HUD"]["ProgressSoundRepeat"].GetFloat();
	mLastLevelTex = "";
	mLevelName = "";
}


static const char *GetRealMapName( const char *name )
{
	DynamicArray<String> files;
	GFileMan.FindFiles("../Data/Levels/*",files,EFindDirs);
	for (int i=0;i<files.Size();i++)
	{
		if( stricmp( name, files[i].AsChar() ) == 0 )
		{
			return files[i].AsChar();
		}
	}

	return name;
}


void LoadingScreen::Activate( bool active, int level, const char *levelTex, const char *levelName )
{
#ifdef TRANSGAMING
	if( mActive == active && !mLastLevelTex.IsNull() && (levelTex == NULL || levelTex[0] == 0))
#else
	if( mActive == active && !mLastLevelTex.IsNull() && levelTex[0] == 0 )
#endif
		return;

	if( !GEngine->Video->MaterialSys )
		return;

	if( active )
	{
		if( !mBackground )
			mBackground = GEngine->Video->MaterialSys->CreateTexture( "HUD/loading/loading", Texture::ETexNoMipMaps | Texture::ETexNoLOD );

#ifndef TRANSGAMING
		GScript.Call( "LoadScreen_FillIcons", 0, "" );
#endif
		if( !mProgressBar )
//			mProgressBar = GEngine->Video->MaterialSys->CreateTexture( "HUD/na_tlo_nowe-alpha.tga", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
			mProgressBar = GEngine->Video->MaterialSys->CreateTexture( "HUD/loading_screen_pasek.tga", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
//			mProgressBar = GEngine->Video->MaterialSys->CreateTexture( "HUD/kreseczka", Texture::ETexNoMipMaps | Texture::ETexNoLOD );

//		for( int i=0; i<30; i++ )
//		{
//			if( !mProgressBar[i] )
//			{
//				mProgressBar[i] = GEngine->Video->MaterialSys->CreateTexture( "HUD/loading_screen_pasek.tga", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
//			}
//		}

#ifdef TRANSGAMING
		if( !mLevel && (levelTex && levelTex[0] != 0) )
#else
		if( !mLevel && levelTex[0] != 0 )
#endif
		{
			if( mLevel )
				GEngine->Video->MaterialSys->ReleaseTexture( mLevel );
			mLevel = GEngine->Video->MaterialSys->CreateTexture( levelTex, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
			if( !mLevel )
				GLog.Print( "ls: Cannot load tex: %s\n", levelTex );
			mLastLevelTex = levelTex;
		}

		if( !mShadow )
			mShadow = GEngine->Video->MaterialSys->CreateTexture( "HUD/loading/shadow", Texture::ETexNoMipMaps | Texture::ETexNoLOD );

		mStartLevel = level;
		if( mStartLevel > 5 )
			mStartLevel = 0;

#ifndef TRANSGAMING
		mNumber = GEngine->Video->MaterialSys->CreateTexture( "HUD/loading/levels/1", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
#endif
		mRing = GEngine->Video->MaterialSys->CreateTexture( "HUD/loading/pierscien", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	

        if (levelName){
            const char *dmLevel1 = strstr( levelName, "DM_" );
            const char *dmLevel2 = strstr( levelName, "DMPCF_" );
            const char *ctfLevel = strstr( levelName, "CTF_" );

		    if( dmLevel1 && !stricmp(dmLevel1, levelName) || dmLevel2 && !stricmp(dmLevel2, levelName) || ctfLevel && !stricmp(ctfLevel, levelName))
                mLevelName = GetRealMapName( levelName );
            
            else
                mLevelName = "";
        }

		GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;
//		GEngine->InputSys->SetUseDInput( true );
	}
	else
	{
		if( mBackground )
			GEngine->Video->MaterialSys->ReleaseTexture( mBackground );
//		for( int i=0; i<30; i++ )
//		{
//			if( mProgressBar[i] )
//				GEngine->Video->MaterialSys->ReleaseTexture( mProgressBar[i] );
//			mProgressBar[i] = NULL;
//		}
		if( mProgressBar )
			GEngine->Video->MaterialSys->ReleaseTexture( mProgressBar );
		if( mLevel )
			GEngine->Video->MaterialSys->ReleaseTexture( mLevel );
		if( mRing )
			GEngine->Video->MaterialSys->ReleaseTexture( mRing );
		if( mShadow )
			GEngine->Video->MaterialSys->ReleaseTexture( mShadow );
		mBackground = NULL;
		mProgressBar = NULL;
		mRing = NULL;
		mLevel = NULL;
		mShadow = NULL;
		mOverall = 0;
		mProgress = 0;
		mAdvance = 0.0f;
		mPercent = 0.0f;
		mLevelName = "";
	}

	mActive = active;

	if( active )
		RenderLoadScreen();
}


void LoadingScreen::RenderLoadScreen()
{
	if( !mActive || !mBackground )
		return;

/*	GEngine->InputSys->Tick();
	if( GEngine->InputSys->GetKeyStatus( VK_ESCAPE ) )
	{
		GScript.DoString( "NET.Disconnect()" );
		GEngine->InputSys->SetUseDInput( false );
		GEngine->Video->PMenu->LoadScreen.Activate( false );
		GEngine->Game->ShowMenu();
		return;
	}
*/
	HUD *hud = GEngine->Video->Hud;
	View *view = GEngine->Video->GetActiveView();
	RenderDevice* dev = GEngine->GetRenderDevice();

	float screenWidth = ( float )view->LocalViewport.Width;
	float screenHeight = ( float )view->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

//	float scale = min( scaleX, scaleY );

	DWORD old = view->LocalViewport.ClearColor;
	view->LocalViewport.ClearColor = 0x00000000;
    dev->SetVBlankState(true);

	if( dev->Lock( view->LocalViewport ) )
	{
		if( mLevel )
#ifdef TRANSGAMING
			hud->DrawQuad( mLevel, 370 * scaleX, 140 * scaleY, mLevel->OriginalWidth * scaleX, mLevel->OriginalHeight * scaleY, 0xff0000ff);
#else
			hud->DrawQuad( mLevel, 370 * scaleX, 140 * scaleY, mLevel->OriginalWidth * scaleX, mLevel->OriginalHeight * scaleY, 0xffff0000);  //0xffff0000 );
#endif
		hud->DrawQuad( mBackground, 0, 0, screenWidth, screenHeight );

//		hud->DrawQuad( mBackground, 0, 0, screenWidth, screenHeight );
//		if( mLevel )
//			hud->DrawQuad( mLevel, 370 * scaleX, 140 * scaleY, mLevel->OriginalWidth * scaleX, mLevel->OriginalHeight * scaleY, 0xffff0000 );
		DrawProgress();

#ifdef TRANSGAMING
		if( !mLevelName.IsNull() ){
            const int   BUFSIZE = 128;
			char        text[BUFSIZE];


			snprintf( text, BUFSIZE, "Loading map %s...", mLevelName.AsChar() );
			hud->SetFont( "timesbd", 28 );
			hud->Print( (int)(80 * scaleX), (int)(660 * scaleY), text, 0xff7abaff );
		}
#else
		if( !mLevelName.IsNull() )
		{
			char text[255];
			sprintf( text, "Loading map %s...", mLevelName.AsChar() );
			hud->SetFont( "timesbd", 28 );
			hud->Print( (int)(80 * scaleX), (int)(660 * scaleY), text, 0xffffba7a );
		}
#endif

		dev->Unlock();

# if defined(TRANSGAMING_PS2) && defined(TRANSGAMING_SHOW_FPS)
        extern void VIEW_RenderStats(void);
        VIEW_RenderStats();
#endif

        Assert(dev->GetVBlankState() == true);
		dev->Flip( view->GetActiveWindow() );

#if defined(TRANSGAMING_SHOW_FPS) && defined(TRANSGAMING_PSP)
        extern void PCF_RenderStats(float,float,float);
        PCF_RenderStats(GEngine->SysDriver.GetTimeStep(),0.0f,GEngine->SysDriver.GetCurrentTime());
#endif

        //make sure to update the frame counter for the vertex buffers are reset
        totalFrameCount++;
	}

//	GEngine->TakeScreenshot();

	view->LocalViewport.ClearColor = old;
}


void LoadingScreen::DrawProgress()
{
/*	int count = ( int )ceil( 30.0f * mPercent / 100.0f );
	if( count > 30 ) count = 30;
	for( int i=mStartLevel * 5; i<mStartLevel * 5 + count; i++ )
	{
		if( i < 30 )
			mProgressIcons[i].Draw();
		else
			mProgressIcons[i-30].Draw();
	}
*/
	if( !mProgressBar )
		return;

	HUD *hud = GEngine->Video->Hud;
	View *view = GEngine->Video->GetActiveView();
/*
	char progress[128];
	sprintf( progress, "Overall: %d, loaded: %d, percent: %f", mOverall, mProgress, mPercent );
	hud->SetFont( "tahomabd", 20 );
	hud->Print( 100, 100, progress, 0xffffffff );
*/

	int screenWidth = view->LocalViewport.Width;
	int screenHeight = view->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

//	float scale = min( scaleX, scaleY );

	BYTE alpha = 255 - (BYTE)(254 * mPercent / 100.0f);
	if( alpha < 1 ) alpha = 1;
	hud->DrawQuadAlphaTest( mProgressBar, 553 * scaleX, 0, mProgressBar->OriginalWidth * scaleX, mProgressBar->OriginalHeight * scaleY, alpha );
//	hud->DrawQuadAlphaTest( mProgressBar, 553 * scale + ( screenWidth - 1024.0f * scale ) / 2, ( screenHeight - 768.0f * scale ) / 2, mProgressBar->OriginalWidth * scale, mProgressBar->OriginalHeight * scale, 255 - 254 * mPercent / 100.0f );

	POINT center;
	center.x = (int)(245 * scaleX);// + ( screenWidth - 1024.0f * scale ) / 2;
	center.y = (int)(268 * scaleY);// + ( screenHeight - 768.0f * scale ) / 2;
	hud->DrawQuadRotated( mRing, 100 * scaleX, -304.0f * scaleY/* + ( screenHeight - 768.0f * scale ) / 2*/, mRing->OriginalWidth * scaleX, mRing->OriginalHeight * scaleY, -F_PI * ( 1.0f - ( mPercent / 100.0f ) ) / 1.97f - F_PI/3 + 0.016f, center );

	if( mShadow )
	{
		hud->DrawQuad( mShadow, 683 * scaleX/* + ( screenWidth - 1024.0f * scale ) / 2*/, 0, mShadow->OriginalWidth * scaleX, mShadow->OriginalHeight * scaleY );
//		hud->DrawQuad( NULL, 0, 300, 600, 600 );
#ifdef TRANSGAMING
		hud->DrawQuad( mShadow, 0, screenHeight - mShadow->OriginalHeight * scaleY, screenWidth, mShadow->OriginalHeight * scaleY, 0xff0000ff, 0.0f,1.0f,1.0f,0.0f );
#else
		hud->DrawQuad( mShadow, 0, screenHeight - mShadow->OriginalHeight * scaleY, screenWidth, mShadow->OriginalHeight * scaleY, 0xffff0000, 0.0f,1.0f,1.0f,0.0f );
#endif
	}

#ifndef TRANSGAMING
	GEngine->Video->Hud->mConsole.Draw();
#endif
/*
	for( int i=0; i<255; i++ )
	{
		BYTE b = 255 - i;//( i >> 2 );
		DWORD color = 0xFF000000 | ( b << 16 ) | ( b << 8 ) | b;
		hud->DrawQuadRotated( NULL, 100 * scale, -304.0f * scale + ( screenHeight - 768.0f * scale ) / 2, 4 * scale, mRing->OriginalHeight * scale, -F_PI * ( 1.0f - ( ( float )i / 255.0f ) ) / 2 - F_PI/3, center, color );
	}
*/
//	hud->DrawQuadAlphaTest( mProgressBar, 553 * scale + ( screenWidth - 1024.0f * scale ) / 2, ( screenHeight - 768.0f * scale ) / 2, mProgressBar->OriginalWidth * scale, mProgressBar->OriginalHeight * scale, 255 - 255 * mPercent / 100.0f );
//	hud->DrawQuad( mProgressBar, 553 * scaleX, screenHeight - mProgressBar->OriginalHeight * scaleY * mPercent / 100.0f, mProgressBar->OriginalWidth * scaleX, mProgressBar->OriginalHeight * scaleY * mPercent / 100.0f, 0xffffffff, 0.0f, 1.0f - mPercent / 100.0f, 1.0f, 1.0f );
/*
	for( int i=0; i<(int)mPercent/2; i++ )
	{
		hud->DrawQuadRotated( mProgressBar, 100 * scale, -308.0f * scale + ( screenHeight - 768.0f * scale ) / 2, mProgressBar->OriginalWidth * scale, mProgressBar->OriginalHeight * scale, -3.14159265358979323846f * i / 100.0f - 1.0f, center );
	}
*/
//	hud->DrawQuad( NULL, 0, view->LocalViewport.Height - 20, view->LocalViewport.Width * mPercent / 100.0f, 20, 0xffff0000 );
}


void LoadingScreen::Progress( int p )
{
    //TRACE("LoadingScreen::Progress(): incrementing progress level by %d {progress = %d, overall = %d, percent = %g, advance = %g}\n", p, mProgress, mOverall, mPercent, mAdvance);
	static float playStart = 0.0f;
	static float updTime = 0.0f;
	float time = GEngine->SysDriver.GetCurrentTime();

	if( !mActive ) return;

	if( mOverall )
	{
		if( time - updTime > 0.1f )
		{
			playStart = 0.0f;
		}

		if( time - playStart > mSoundRepeat )
		{
//			GEngine->Video->PMenu->PlayMenuSound( "menu/mapselect/loadprogress" );
			playStart = time;
		}
		mProgress += p;
		mPercent += p * mAdvance;
		if( mPercent > 100.0f )
			mPercent = 100.0f;
		if( time - updTime > 0.02f )
		{
			updTime = time;
		}
	}

	RenderLoadScreen();
}


#ifndef TRANSGAMING
void LoadingScreen::SetIcon( int i, int x, int y, String name )
{
	mProgressIcons[i].Set( x, y, name );
}
#endif
