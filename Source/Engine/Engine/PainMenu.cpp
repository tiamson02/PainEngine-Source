#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"

#if defined( TRANSGAMING_PS2 )
#include "Render/Movie.h"
#endif

#ifndef TRANSGAMING_NO_MILES
// PSP support will need to be added later.
#include "Audio/MilesEngine.h"
#endif

#include "Engine/InputSystem.h"
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;
#include "Engine/BaseGame.h"
#include "Render/Video.h"
#include "Render/Material.h"

#ifndef	TRANSGAMING_PS2
#include "Render/Movie.h"
#endif

#include "Engine/PainMenu.h"


//--------------------------------------------------------------------
//	PainMenu class
//--------------------------------------------------------------------
PainMenu::PainMenu()
{
	mActive = false;

	mShowMouse = false;
	mMouseOldSens = 0.11f;
	mMouseClickOffsetX = 0;
	mMouseClickOffsetY = 0;
	mMouseTexture = NULL;

	mSnd = -1;
	mPausedSounds = -1;
	mRoomType = 0;
	mRoomIntensity2D = 0.0f;
	mRoomIntensity3D = 0.0f;

	FrameChanged = false;
	CreatingFont = false;
	InGame = false;

	mShowSplash = true;
}


PainMenu::~PainMenu()
{
	Clear();
	ResumeSounds();
}


void PainMenu::Clear()
{
	Screen.Clear( true );

	if( mMouseTexture && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );
	mMouseTexture = NULL;
}


void PainMenu::Activate( bool active )
{
	mActive = active;

	static BOOL mouseLocked = GEngine->InputSys->IsMouseLocked();
	static BOOL mouseVisible = GEngine->InputSys->IsMouseVisible();
	static bool firstTime = true;

	GEngine->InputSys->Reset();

	if( active )
	{
		if( !mMouseTexture )
			SetMouseTexture( 0, 0, "HUD/kursor" );
		// BLAIR - We're disabling this mouse...permanently!
#ifndef TRANSGAMING
		ShowMouse( true );
#endif
		mouseLocked = GEngine->InputSys->IsMouseLocked();
		if( mouseLocked )
			GEngine->InputSys->LockMouse( FALSE );
		mouseVisible = GEngine->InputSys->IsMouseVisible();
		if( mouseVisible )
			GEngine->InputSys->ShowMouse( FALSE );
//		if( !firstTime )
		GScript.DoString( "Game:ApplySettings(); PainMenu:ActivateScreen(MainMenu)" );
		GScript.DoString( "PainMenu:OpenMenu()" );
		FrameChanged = true;
		Screen.Activate();
		firstTime = false;

//		mMouseOldSens = GEngine->InputSys->GetMouseSensitivity();
//		GEngine->InputSys->SetMouseSensitivity( 0.6f );
//		if( InGame )
//			EnteringFromGame = true;
	}
	else
	{
		ShowMouse( false );
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );
		mMouseTexture = NULL;
		if( mouseLocked )
			GEngine->InputSys->CenterMouse();
		if( mouseVisible != GEngine->InputSys->IsMouseVisible() )
		{
			GEngine->InputSys->ShowMouse( mouseVisible );
			mouseVisible = GEngine->InputSys->IsMouseVisible();
		}
		if( mouseLocked != GEngine->InputSys->IsMouseLocked() )
		{
			GEngine->InputSys->LockMouse( mouseLocked );
			mouseLocked = GEngine->InputSys->IsMouseLocked();
		}
		GEngine->Video->GetActiveView()->Mode &= ~View::EModeDoNotRenderWorld;
		GEngine->Game->mPaused = false;
		GScript.DoString( "Game.Paused = false" );
		Screen.StopMovie();
		GScript.DoString( "PainMenu:CloseMenu()" );
		InGame = true;
//		GEngine->InputSys->SetMouseSensitivity( mMouseOldSens );

		Clear();
	}
}

extern ENGINE_API bool IsBooHInstalled();

void PainMenu::Render()
{
    //TRACE("PainMenu::Render(): rendering menu stuff\n");
//#ifdef FINAL_BUILD
	static Texture *tex = NULL;
    //static DWORD originalClearColor = 0;
	if( mShowSplash ){
		View *view = GEngine->Video->GetActiveView();


        if (!tex){
            //set the clear color to something that won't show the screen alignment problems on the PSP
            //originalClearColor = view->LocalViewport.ClearColor;
            //view->LocalViewport.ClearColor = 0x00000000;

			bool blackEd = GScript.Globals()["Cfg"]["BlackEdition"].GetBool();
			if( blackEd )
				tex = GEngine->Video->MaterialSys->CreateTexture( "HUD/startup_logo_black", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
			else if( IsBooHInstalled() )
			{
				tex = GEngine->Video->MaterialSys->CreateTexture( "HUD/startup_logo_booh", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
				PlayMenuSound( "menu/painkiller-mainlogo" );
			}
			else
			{
				tex = GEngine->Video->MaterialSys->CreateTexture( "HUD/startup_logo", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
				PlayMenuSound( "menu/mapselect/option-sign_pact" );
			}
		}
		if( !tex )
		{
			mShowSplash = false;
			return;
		}

		static float start = GEngine->SysDriver.GetCurrentTime();


		GEngine->Video->Hud->DrawQuad( tex, 0, 0, view->LocalViewport.Width, view->LocalViewport.Height );

		if( GEngine->SysDriver.GetCurrentTime() - start > 12.0f )
			mShowSplash = false;

#ifdef TRANSGAMING
		DWORD escState = GEngine->InputSys->GetKeyStatus( BUTTON_START );
#else
		DWORD escState = GEngine->InputSys->GetKeyStatus( VK_ESCAPE );
#endif

        if( escState && GEngine->SysDriver.GetCurrentTime() - start > 1.0f )
			mShowSplash = false;

        if (!mShowSplash){
            if (tex){
                //reset the original clear colour after closing the splash screen (not entirely necessary
                //since it's set at the start of rendering every frame)
                //view->LocalViewport.ClearColor = originalClearColor;

			    StopSound();
			    GEngine->Video->MaterialSys->ReleaseTexture( tex );
            }

#ifdef TRANSGAMING_HACK
            GScript.DoString("Game:TNG_loadCurrentLevel()");
#endif
		}
	}
	else
	{
//#endif

	    if( mActive && !( GEngine->Video->GetActiveView()->Mode & View::EModeDoNotRenderWorld ) )
		    GEngine->Video->GetActiveView()->Mode |= View::EModeDoNotRenderWorld;

	    Screen.Render();

	    if( mMouseTexture && !( mMouseTexture->Flags & Texture::ETexInvalid ) && mShowMouse && !Screen.PlayingMovie() )
	    {
		    POINT mousePos = GEngine->InputSys->GetMousePosition();
		    GEngine->Video->Hud->DrawQuad( mMouseTexture, (float)(mousePos.x-mMouseClickOffsetX),
			    (float)(mousePos.y-mMouseClickOffsetY), (float)mMouseTexture->Width, (float)mMouseTexture->Height );
	    }
//#ifdef FINAL_BUILD
	}
//#endif
}


bool PainMenu::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( !Active() )
		return false;

	return Screen.SendEvent( eventType, eventData1, eventData2 );
}


void PainMenu::SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags )
{
	if( mMouseTexture )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );

	mMouseTexture = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mMouseClickOffsetX = xClickOffset;
	mMouseClickOffsetY = yClickOffset;

	if( mMouseTexture == GEngine->Video->MaterialSys->Textures.NoTex )
		mMouseTexture = 0;
}


void PainMenu::PlayMenuSound( const char *name, bool loop )
{
#ifndef TRANSGAMING_NO_MILES
    if (!GEngine->Audio) return;
	static char buf[MAX_PATH];

	sprintf( buf, soundFileFormatStr, name );

	int snd = GEngine->Audio->Sound2D_Create( buf, 0 );
	if( snd != -1 )
	{
		GEngine->Audio->Sound2D_SetVolume( snd, 1.0f );
		GEngine->Audio->Sound2D_SetDontSave( snd );

        bool samespeed = true;
        if( samespeed )
        {
			GEngine->Audio->Sound2D_NoSpeedRandomize( snd );
            GEngine->Audio->Sound2D_SetAlwaysSameSpeed( snd, true );
            GEngine->Audio->Sound2D_SetLowPass( snd, 1.0f );
        }

		if( loop )
			GEngine->Audio->Sound2D_SetLoopCount( snd, 0 );

		if( loop && mSnd == -1 )
		{
			GEngine->Audio->Sound2D_Play( snd );
			mSnd = snd;
		}
		else
			GEngine->Audio->Sound2D_PlayAndForget( snd );
	}
	else
		GScript.DoString( "Game:Print('Sound not created')" );
#endif
}


void PainMenu::StopSound()
{
	if( mSnd != -1 )
	{
#ifndef TRANSGAMING_NO_MILES
		GEngine->Audio->Sound2D_SetVolume( mSnd, 0.0f );
		GEngine->Audio->Sound2D_Stop( mSnd );
		GEngine->Audio->Sound2D_Delete( mSnd );
		mSnd = -1;
#endif
	}
}


void PainMenu::PauseSounds()
{
#ifndef TRANSGAMING_NO_MILES
	if( mPausedSounds != -1 )
		if( GEngine->Audio )
			GEngine->Audio->ResumeSounds( mPausedSounds );
	mPausedSounds = GEngine->Audio?GEngine->Audio->PauseCurrentlyPlayingSounds():-1;

	if( GEngine->Audio )
	{
		mRoomType = GEngine->Audio->GetCurrentRoomType();
		mRoomIntensity2D = GEngine->Audio->GetCurrent2DIntensity();
		mRoomIntensity3D = GEngine->Audio->GetCurrent3DIntensity();
	}
#endif
}


void PainMenu::ResumeSounds()
{
#ifndef TRANSGAMING_NO_MILES
	if( mPausedSounds != -1 )
	{
		if( GEngine->Audio )
			GEngine->Audio->ResumeSounds( mPausedSounds );
		mPausedSounds = -1;
	}

	if( GEngine->Audio )
	{
		GEngine->Audio->SetRoom( mRoomType );
		GEngine->Audio->SetRoomIntensity2D( mRoomIntensity2D, 0.0f );
		GEngine->Audio->SetRoomIntensity3D( mRoomIntensity3D, 0.0f );
	}
#endif
}


void PainMenu::ResetRoomType()
{
#ifndef TRANSGAMING_NO_MILES
	if( GEngine->Audio )
	{
		mRoomType = 0;
		mRoomIntensity2D = 0.0f;
		mRoomIntensity3D = 0.0f;
	}
#endif
}


extern ENGINE_API bool IsBlackEdition();

void PainMenu::ShowCredits( bool addOn )
{
	View *view = GEngine->Video->GetActiveView();
	RenderDevice* dev = GEngine->GetRenderDevice();
	HUD *hud = GEngine->Video->Hud;

	WINDOWPLACEMENT place;
	memset( &place, 0, sizeof( WINDOWPLACEMENT ) );
	place.length = sizeof( WINDOWPLACEMENT );
	if( GetWindowPlacement( view->GetTopmostWindow(), &place ) )
	{
		if( place.showCmd == SW_SHOWMINIMIZED )
			return;
	}

	if( GetForegroundWindow() != view->GetTopmostWindow() )
		return;

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	int part = 0;

	const int fontSizes[4] = { 40, 32, 24, 20 };
#ifdef TRANSGAMING
	const DWORD fontColors[4] = { 0x00ffffff, 0x007abaff, 0x00d1d1d1, 0x001700d6 };
#else
	const DWORD fontColors[4] = { 0x00ffffff, 0x00ffba7a, 0x00d1d1d1, 0x00d60017 };
#endif


	hud->SetFont( "timesbd", fontSizes[3] );
	int fontHeight = hud->GetTextHeight() + 2;

	int visible = (int)(430 * screenHeight / 768.0f / fontHeight);

	float scrollSpeed = 0.02f;
	ScriptObject tweak = GScript.Globals()["Tweak"];
	if( !tweak["HUD"]["CreditsScrollSpeed"].IsNil() )
	{
		scrollSpeed = tweak["HUD"]["CreditsScrollSpeed"].GetFloat() / 1000;
	}

    m_setTag("Menu");
	Movie *mov = new Movie();
	if( !mov->Open( "../Data/Movies/credits.bik" ) )
	{
		delete mov;
		return;
	}

    m_setTag("Menu");
	Movie *left = new Movie();
	if( !left->Open( "../Data/Movies/left.bik", true ) )
	{
		delete left;
		delete mov;
		return;
	}

    m_setTag("Menu");
	Movie *right = new Movie();
	if( !right->Open( "../Data/Movies/right.bik", true ) )
	{
		delete right;
		delete left;
		delete mov;
		return;
	}

	GFile *f = NULL;
	if( !addOn )
		f = GFileMan.CreateFileReader("../Data/LScripts/Main/Credits.txt");
	else
		f = GFileMan.CreateFileReader("../Data/LScripts/Main/Credits_BooH.txt");
	if( !f )
	{
		delete right;
		delete left;
		delete mov;
		return;
	}

	DynamicArray<String> credits;

	for( int i=0; i<visible; i++ )
	{
		credits.Add( String( "" ) );
	}

	char buf[1024];
	while( f->GetS( buf, sizeof( buf ) ) )
	{
		int len = strlen( buf );
		if( buf[len-1] == '\n' )
			buf[len-1] = 0;

		char *tmp = buf;
		while( *tmp )
		{
			if( *tmp == '#' ) *tmp = ' ';
			tmp++;
		}

		if( buf[0] == '1' || buf[0] == '2' )
			credits.Add( String( "" ) );
		credits.Add( String( buf ) );
		if( buf[0] == '1' || buf[0] == '2' )
			credits.Add( String( "" ) );
	}

	GFileMan.Close( f );

	int currLine = 0;
	int shift = 0;
	int top = (int)(220 * ( float )screenHeight / 768.0f);
	float lastTime = 0.0f;
	float timeCheck = 0.0f;
	float deltaCheck = 0.0f;

	left->SetPosition( 0, 300 );
	right->SetPosition( 900, 240 );

	int pausedSounds = -1;
	int musicHandle = -1;
	bool scrollOnPlayed = false;

#ifndef TRANSGAMING_NO_MILES
	if( GEngine->Audio )
	{
		pausedSounds = GEngine->Audio->PauseCurrentlyPlayingSounds();

		musicHandle = GEngine->Audio->StreamingSound_Create( "../Data/Music/painkiller-credits.mp3" );
		if( musicHandle != -1 )
		{
//			GEngine->Audio->StreamingSound_SetLoopCount( musicHandle, 0 );
			GEngine->Audio->StreamingSound_SetVolume( musicHandle, 1.0f );
		}
	}
#endif
	mov->Play( true );

	while( mov->IsPlaying() )
	{
		MSG msg;
		msg.message=NULL;

#ifndef TRANSGAMING_NO_MILES
		if( GEngine->Audio )
		{
			if( deltaCheck > 0.0f )
			{
				float timeDelta = GEngine->SysDriver.GetCurrentTime() - deltaCheck;
				GEngine->Audio->Tick(timeDelta);
				deltaCheck = GEngine->SysDriver.GetCurrentTime();
			}
		}
#endif
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE | PM_QS_INPUT ) )
		{
			if( msg.message == WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP
				|| msg.message == WM_KEYDOWN || msg.message == WM_KEYUP )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );

				if( msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP )
				{
//					DWORD escState = GEngine->InputSys->GetKeyStatus( VK_ESCAPE );
//					if( escState )
//					{
						GEngine->InputSys->Reset();
						break;
//					}
				}
			}
		}
		else
		{
			GEngine->InputSys->Tick();

			if( dev->Lock( view->LocalViewport ) )
			{
				bool flip = false;
				if( mov->Draw() )
				{
					if( left->IsPlaying() )
						left->Draw();
					if( right->IsPlaying() )
						right->Draw();
					if( part > 0 && part < 4 )
					{
						for( int i=currLine; i<currLine + visible; i++ )
						{
							if( i < credits.Size() && credits[i].Size() > 3
								&& credits[i][0] > '0' && credits[i][0] < '5'
								&& credits[i][1] > '0' && credits[i][1] < '5' )
							{
								DWORD color = fontColors[credits[i][1]-'1'];
								BYTE alpha = 0xff;
								if( i < currLine + 1 )
								{
									alpha = 128 - ( BYTE )( 128.0f * ( float )shift / ( float )fontHeight );
								}
								else if( i < currLine + 2 )
								{
									alpha = 255 - ( BYTE )( 128.0f * ( float )shift / ( float )fontHeight );
								}
								else if( i > currLine + visible - 2 )
								{
									alpha = ( BYTE )( 128.0f * ( float )shift / ( float )fontHeight );
								}
								else if( i > currLine + visible - 3 )
								{
									alpha = 128 + ( BYTE )( 128.0f * ( float )shift / ( float )fontHeight );
								}
								color |= ( alpha << 24 );

                                hud->SetFont( "timesbd", fontSizes[credits[i][0]-'1'] );
								int fh = hud->GetTextHeight();
								int width = hud->GetTextWidth( credits[i].AsChar() + 2 );
								hud->Print( (screenWidth-width)/2+1, top + ( i - currLine ) * fontHeight - shift + ( fontHeight - fh ) / 2 + 1, credits[i].AsChar() + 2, ( DWORD )( alpha << 24 ) );
								hud->Print( (screenWidth-width)/2, top + ( i - currLine ) * fontHeight - shift + ( fontHeight - fh ) / 2, credits[i].AsChar() + 2, color );
							}
						}
					}
/*
					// Calculate FPS
					static int FPS = 0;
					static DWORD lastGetCurrentTime = GetCurrentTime();
					static int frameCount = 0;
					++frameCount;
					DWORD curTime = GetCurrentTime();
					if( curTime-lastGetCurrentTime >= 1000 ) // a second passed
					{
						FPS = frameCount;	// no multiplies/divides anymore here
						frameCount = 0;
						lastGetCurrentTime = curTime;
					}

					char text[255];
					sprintf( text, "FPS: %d, Shift: %d, fontHeight: %d", FPS, shift, fontHeight );
					hud->Print( 0, 0, text );
*/
					flip = true;
				}

				dev->Unlock();
				if( flip )
					dev->Flip( view->GetActiveWindow() );
			}

			int frame = mov->GetCurrentFrame();
			bool chooseNext = false;

			if( part == 0 && frame > 50 && !scrollOnPlayed )
			{
				PlayMenuSound( "menu/menu/scroll-roll_on" );
				scrollOnPlayed = true;
			}
			else if( part == 0 && frame > 100 )
			{
				part++;
				mov->Pause();
#ifndef TRANSGAMING_NO_MILES
				if( GEngine->Audio && musicHandle != -1 )
				GEngine->Audio->StreamingSound_Play( musicHandle );
#endif
			}
			else if( part == 4 && ( frame >= 240 || frame < 180 ) )
			{
				if( frame >= 240 )
					break;
				else
					mov->Seek( 180 );
			}

			float time = GEngine->SysDriver.GetCurrentTime();
			if( time - timeCheck > 1.5f )
			{
				chooseNext = true;
				timeCheck = time;
			}

			if( chooseNext && part != 0 && part != 4 )
			{
				part = ( int )( ( ( float )rand() / ( float )RAND_MAX ) * 3.0f ) + 1;
				if( part > 3 ) part = 3;
				GLog.Print( "Random: %d\n", part );

				if( part == 2 )
				{
					right->Stop();
					left->Play();
					left->Pause( false );
				}
				else if( part == 3 )
				{
					left->Stop();
					right->Play();
					right->Pause( false );
				}
				else if( part == 1 )
				{
					left->Stop();
					right->Stop();
				}
			}

			if( part > 0 && part < 4 )
			{
				float currTime = GEngine->SysDriver.GetCurrentTime();
				if( currTime - lastTime > scrollSpeed )
				{
					shift += ( int )( ( currTime - lastTime ) / scrollSpeed );
					lastTime = currTime;
				}

				if( shift >= fontHeight )
				{
					shift = 0;
					currLine++;
				}

				if( currLine > credits.Size() )
				{
					part = 4;
					left->Stop();
					right->Stop();
					mov->Seek( 180 );
					mov->Pause( false );
				}
			}
		}
	}

	credits.ClearArray();

	right->Stop();
	left->Stop();
	mov->Stop();
	delete right;
	delete left;
	delete mov;

#ifndef TRANSGAMING_NO_MILES
	if( GEngine->Audio && musicHandle != -1 )
		GEngine->Audio->StreamingSound_Delete( musicHandle );
	if( pausedSounds != -1 && GEngine->Audio )
		GEngine->Audio->ResumeSounds( pausedSounds );
#endif
	if( IsBlackEdition() )
		GScript.DoString( "PainMenu:ActivateScreen(OptionsMenu)" );
	else
		GScript.DoString( "PainMenu:ActivateScreen(MainMenu)" );
}
