
#include "Engine.h"
#include "Movie.h"
#include "Render/HUD.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;
#include "FileSystem/GFileManager.h"
extern ENGINE_API GFileManager  	GFileMan;
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Render/View.h"
#include "Render/Video.h"
#include "Render/Material.h"

#ifndef TRANSGAMING_NO_MILES
#include "Audio/MilesEngine.h"
#endif

#include "Engine/PainMenu.h"
#include "Engine/MagicBoard.h"
#include "Engine/MapSelect.h"
#include "Engine/InputSystem.h"

//#include <DShow.h>
//#include <vfw.h>
#if !( defined( TRANSGAMING_PS2 ) || defined( TRANSGAMING_PSP ) )
#include <streams.h>
#endif
/*
#include <atlbase.h>
#include <wmsdk.h>
*/

#ifdef TRANSGAMING_NO_BINK
#define S32 signed int
#endif

bool Movie::Open( const char *name, bool preload, int sndTrack )
{
#ifndef TRANSGAMING_NO_BINK
	static bool firstTime = true;
	if( firstTime )
	{
#if defined( TRANSGAMING_PS2 )
#ifndef TRANSGAMING_NO_MILES
		U32 which_sound_core = 0;
		if( GEngine->Audio )
		{
			if( !BinkSoundUseMiles( GEngine->Audio->GetDigitalDriver() ) )
			{
				GLog.Print( "Cannot use Miles for movies.\n" );
				BinkSoundUseRAD_IOP( which_sound_core );
			}
		}
		else
		{
			BinkSoundUseRAD_IOP( which_sound_core );
		}
#endif
#else
		if( GEngine->Audio )
		{
			if( !BinkSoundUseMiles( GEngine->Audio->GetDigitalDriver() ) )
			{
				GLog.Print( "Cannot use Miles for movies.\n" );
				BinkSoundUseDirectSound( 0 );
			}
		}
		else
		{
			BinkSoundUseDirectSound( 0 );
		}
#endif
		firstTime = false;
	}

	U32 flags = 0;
	if( preload )
		flags |= BINKPRELOADALL;
	if( sndTrack != -1 )
	{
//		flags |= BINKSNDTRACK;
//		U32 track = ( U32 )sndTrack;
//		BinkSetSoundTrack( 1, &track );
	}

	// Open Bink
	mBink = BinkOpen( name, flags );
	if( !mBink )
	{
		GLog.Print( "Bink error: %s\n", BinkGetError() );
		return false;
	}

	mWidth = mBink->Width;
	mHeight = mBink->Height;

	mLastFrame = mBink->Frames;

	if( mWidth > 512 || mHeight > 512 )
		mTexture = GEngine->Video->MaterialSys->CreateEmptyTexture( name, 1024, 1024, 32, Texture::ETexRaw32 | Texture::ETexDynamic | Texture::ETexNoMipMaps );
	else
		mTexture = GEngine->Video->MaterialSys->CreateEmptyTexture( name, 512, 512, 32, Texture::ETexRaw32 | Texture::ETexDynamic | Texture::ETexNoMipMaps );
	if( !mTexture )
		return false;

	mCurrFrame = 0;
	mLastRendered = -1;

	return true;
#else // TRANSGAMING_NO_BINK
	return false;
#endif // TRANSGAMING_NO_BINK
}


void Movie::Close()
{
#ifndef TRANSGAMING_NO_BINK
	if( mBink )
	{
		BinkClose( mBink );
		mBink = NULL;
	}
#endif

	if( mTexture )
	{
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture );
		mTexture = NULL;
	}
}


bool Movie::Play( bool loop )
{
	return PlayFrames( -1, -1, loop );
}


bool Movie::PlayFrames( int start, int end, bool loop )
{
	mLoop = loop;
	mIsPlaying = true;

	if( start > -1 )
		mStartFrame = start;
	else
		mStartFrame = 0;

	if( end > -1 )
		mEndFrame = end;
	else
		mEndFrame = mLastFrame;

	mCurrFrame = mStartFrame;

	mLastTime = -1.0f;

	Seek( mCurrFrame );

	return true;
}


void Movie::Pause( bool p )
{
#ifndef TRANSGAMING_NO_BINK
	BinkPause( mBink, p );
#endif
}


void Movie::Stop()
{
	mCurrFrame = 0;
	mIsPlaying = false;
	Seek( 0, true );
}


bool Movie::Seek( int frame, bool pause )
{
	if( frame > mLastFrame )
		mCurrFrame = mLastFrame;
	else
		mCurrFrame = frame;

#ifndef TRANSGAMING_NO_BINK
	BinkGoto( mBink, mCurrFrame, NULL );
#endif
	if( pause )
		Pause();

	return true;
}


static void Good_sleep_us( S32 microseconds )
{
#ifndef TRANSGAMING_NO_BINK
  static S32 total_sleep=0;
  static S32 slept_in_advance=0;
# ifndef TRANSGAMING_PSP
  static U64 frequency=1000;
  static S32 got_frequency=0;
# endif

  //
  // If this is the first time called, get the high-performance timer count.
  //

# ifndef TRANSGAMING_PSP
  if ( !got_frequency )
  {
    got_frequency = 1;
    QueryPerformanceFrequency( ( LARGE_INTEGER* )&frequency );
  }
# endif

  total_sleep += microseconds;

  //
  // Have we exceeded our reserve of slept microseconds?
  //

  if (( total_sleep - slept_in_advance ) > 1000)
  {
    U64 start, end;
    total_sleep -= slept_in_advance;

    //
    // Do the timed sleep.
    //

# ifdef TRANSGAMING_PSP
    start = sceKernelGetSystemTimeWide();
    Sleep(total_sleep / 1000);
    end = sceKernelGetSystemTimeWide();

    //
    // Calculate delta time in microseconds.
    //

    end = (end - start);
# else
    QueryPerformanceCounter( ( LARGE_INTEGER* )&start );
    Sleep( total_sleep / 1000 );
    QueryPerformanceCounter( ( LARGE_INTEGER* )&end );

    //
    // Calculate delta time in microseconds.
    //

    end = ( (end - start) * (U64)1000000 ) / frequency;
# endif

    //
    // Keep track of how much extra we slept.
    //

    slept_in_advance = ( U32 )end - total_sleep;
    total_sleep %= 1000;
  }
#endif // TRANSGAMING_NO_BINK
}


bool Movie::Update()
{
#ifndef TRANSGAMING_NO_BINK
	GEngine->Video->PMenu->FrameChanged = true;

	if( !mIsPlaying )
		return false;

	RenderDevice *renderDev = GEngine->GetRenderDevice();

	if( BinkWait( mBink ) )
	{
		Good_sleep_us( 500 );
		return false;
	}

	mCurrFrame = mBink->FrameNum;

	BinkDoFrame( mBink );

	BYTE *buf = NULL;
	int pitch = 0;

	if( mTexture )
		buf = renderDev->LockTexture( mTexture, pitch );

	if( buf )
	{
		BinkCopyToBuffer( mBink,
						  buf, pitch,
						  mBink->Height,
						  0, 0,
						  BINKSURFACE32 );
		renderDev->UnlockTexture( mTexture );

		if( mBink->FrameNum >= ( U32 )mEndFrame )
		{
			if( mLoop )
			{
				Seek( mStartFrame );
			}
			else
			{
				Stop();
			}
		}
		else
		{
			BinkNextFrame( mBink );
		}

		return true;
	}
#endif // TRANSGAMING_NO_BINK
	return false;
}


bool Movie::Draw()
{
	View *view = GEngine->Video->GetActiveView();

	float screenWidth = ( float )view->LocalViewport.Width;
	float screenHeight = ( float )view->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

//	if( !Update() )
//		return false;

	Update();

	if( mPosX != 0 || mPosY != 0 )
		GEngine->Video->Hud->DrawQuad( mTexture, mPosX * scaleX, mPosY * scaleY, mWidth * scaleX, mHeight * scaleY, 0xffffffff, 0.0f, 0.0f, ( float )( mWidth )/( float )mTexture->Width, ( float )( mHeight )/( float )mTexture->Height );
	else
	{
		float sc = 1024.0f / mWidth;
		float w = mWidth * scaleX * sc;
		float h = mHeight * scaleY * sc;
		int y = (int)(( screenHeight - h ) / 2);
		GEngine->Video->Hud->DrawQuad( mTexture, 0, y, w, h, 0xffffffff, 0.0f, 0.0f, ( float )( mWidth )/( float )mTexture->Width, ( float )( mHeight )/( float )mTexture->Height );
	}

	return true;
}
/*

bool Movie::Open( const char *name )
{
	// Open AVi file
	AVIFileInit();

	if( AVIStreamOpenFromFile( &mAviStream, name, streamtypeVIDEO, 0, OF_READ, NULL ) != 0 )
	{
		GLog.Print( "Movie: Cannot open movie %s\n", name );
		AVIFileExit();
		return false;
	}

	if( ( mGetFrame = AVIStreamGetFrameOpen( mAviStream, NULL ) ) == NULL )
	{
		GLog.Print( "Movie: Cannot load video stream from %s\n", name );
		AVIFileExit();
		return false;
	}

	AVIStreamInfo( mAviStream, &mStreamInfo, sizeof( AVISTREAMINFO ) );
	mWidth = mStreamInfo.rcFrame.right - mStreamInfo.rcFrame.left;
	mHeight = mStreamInfo.rcFrame.bottom - mStreamInfo.rcFrame.top;

	mLastFrame = AVIStreamLength( mAviStream );

	mTimePerFrame = ( AVIStreamSampleToTime( mAviStream, mLastFrame ) / mLastFrame ) / 1000.0f;

	mTexture = GEngine->Video->MaterialSys->CreateEmptyTexture( name, 1024, 1024, 32, Texture::ETexRaw32 );
	if( !mTexture )
		return false;

	mCurrFrame = 0;
	mLastRendered = -1;

	return true;
}


void Movie::Close()
{
	if( mTexture )
	{
		AVIStreamGetFrameClose( mGetFrame );
		AVIStreamRelease( mAviStream );
		AVIFileExit();
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture );
		mTexture = NULL;
	}
}


void Movie::Stop()
{
	mCurrFrame = 0;
	mIsPlaying = false;
}


bool Movie::Play( bool loop )
{
	return PlayFrames( -1, -1, loop );
}


bool Movie::PlayFrames( int start, int end, bool loop )
{
	mLoop = loop;
	mIsPlaying = true;

	if( start > -1 )
		mStartFrame = start;
	else
		mStartFrame = 0;

	if( end > -1 )
		mEndFrame = end;
	else
		mEndFrame = mLastFrame;

	mCurrFrame = mStartFrame;

	mLastTime = -1.0f;

	return true;
}


void Movie::Pause()
{
}


bool Movie::Seek( int frame, bool pause )
{
	if( frame > mLastFrame )
		mCurrFrame = mLastFrame;
	else
		mCurrFrame = frame;

	if( pause )
		Pause();

	return true;
}


bool Movie::Update()
{
	GEngine->Video->PMenu->FrameChanged = true;

	if( !mIsPlaying )
		return false;

	RenderDevice *renderDev = GEngine->GetRenderDevice();

	if( mLastTime == -1.0f )
		mLastTime = GEngine->SysDriver.GetCurrentTime();

	float currTime = GEngine->SysDriver.GetCurrentTime();

	float diff = currTime - mLastTime;
	mCurrFrame = diff / mTimePerFrame;
	mCurrFrame += mStartFrame;

	if( mCurrFrame >= mEndFrame )
	{
		if( mLoop )
		{
			mCurrFrame = mStartFrame;
			mLastTime = currTime;
		}
		else
		{
			mIsPlaying = false;
		}
//		mLastRendered = -1;
//		return true;
	}

	if( mLastRendered == mCurrFrame )
		return true;

	mLastRendered = mCurrFrame;

	BITMAPINFO* lpbi;
	lpbi = ( BITMAPINFO *)AVIStreamGetFrame( mGetFrame, mCurrFrame );

	BYTE *src = NULL;
	if( lpbi )
		src = ( BYTE *)lpbi + sizeof( BITMAPINFO );

	BYTE *buf = NULL;
	int pitch = 0;

	if( mTexture )
		buf = renderDev->LockTexture( mTexture, pitch );

	if( buf && src )
	{
		for( int y=0; y<mHeight; y++ )
		{
			for( int x=0; x<mWidth; x++ )
			{
				BYTE *dst = buf + 4 * x;
				dst[1] = *src++; // g
				dst[2] = *src++; // r
				dst[0] = *src++; // b
			}
			buf += pitch;
		}
		renderDev->UnlockTexture( mTexture );

		return true;
	}

	return false;
}


bool Movie::Draw()
{
	View *view = GEngine->Video->GetActiveView();

	Update();
	GEngine->Video->Hud->DrawQuad( mTexture, 0, 0, view->LocalViewport.Width, view->LocalViewport.Height, 0xffffffff, 0.0f, ( float )( mHeight - 2 )/( float )mTexture->Height, ( float )( mWidth - 2 )/( float )mTexture->Width, 0.0f );

	return true;
}

*/
//--------------------------------------------------------------------

#ifndef WM_GRAPHNOTIFY
#define WM_GRAPHNOTIFY	WM_APP + 1
#endif

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

bool PlayWMV( const char *name )
{
/*	USES_CONVERSION;
	WCHAR wFile[MAX_PATH];
	HRESULT hr;

	// DirectShow interfaces
	IGraphBuilder *pGB = NULL;
	IMediaControl *pMC = NULL;
	IMediaEventEx *pME = NULL;
	IVideoWindow  *pVW = NULL;
	IBasicAudio   *pBA = NULL;
	IBasicVideo   *pBV = NULL;
	IMediaSeeking *pMS = NULL;

    // Check input string
	if( !name )
		return false;

	// Convert filename to wide character string
	wcsncpy( wFile, T2W( name ), NUMELMS( wFile )-1 );
	wFile[MAX_PATH-1] = 0;

	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
						   IID_IGraphBuilder, ( void **)&pGB );
	if( FAILED( hr ) )
		return false;

	hr = pGB->QueryInterface( IID_IMediaEventEx, ( void **)&pME );
	if( FAILED( hr ) )
		return false;

	hr = pGB->RenderFile( wFile, NULL );
	if( FAILED( hr ) )
		return false;

	if( FAILED( pGB->QueryInterface( IID_IMediaControl, ( void **)&pMC ) ) )
		return false;

	if( FAILED( pGB->QueryInterface( IID_IMediaSeeking, ( void **)&pMS ) ) )
		return false;

	if( FAILED( pGB->QueryInterface( IID_IVideoWindow, ( void **)&pVW ) ) )
		return false;

	if( FAILED( pGB->QueryInterface( IID_IBasicVideo, ( void **)&pBV ) ) )
		return false;

	if( FAILED( pGB->QueryInterface( IID_IBasicAudio, ( void **)&pBA ) ) )
		return false;

	HWND window = GEngine->Video->GetActiveView()->GetActiveWindow();
	if( !window )
		return false;

	if( FAILED( pVW->put_Owner( ( OAHWND )window ) ) )
		return false;

	if( !GEngine->Video->IsFullScreen )
		pVW->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN );

	pVW->put_MessageDrain( ( OAHWND )window );

	if( GEngine->Video->IsFullScreen )
	{
//		pVW->put_Width( GEngine->Video->GetActiveView()->LocalViewport.Width );
//		pVW->put_Height( GEngine->Video->GetActiveView()->LocalViewport.Height );
		pVW->put_FullScreenMode( OATRUE );
	}
	else
	{
		pVW->put_FullScreenMode( OAFALSE );
//		SetForegroundWindow( window );
	}

	RECT rect;
	GetClientRect( window, &rect );
	pVW->SetWindowPosition( rect.left, rect.top, rect.right, rect.bottom );

	if( FAILED( pMC->Run() ) )
		return false;

	bool playing = true;
	while( playing )
	{
		long32 lEventCode;
		long32 lParam1;
		long32 lParam2;

		HRESULT hr = pME->GetEvent( &lEventCode, ( LONG_PTR *)&lParam1, ( LONG_PTR *)&lParam2, 0 );
		if( SUCCEEDED( hr ) )
		{
			// If we have reached the end of the media file, reset to beginning
			if( EC_COMPLETE == lEventCode )
			{
				playing = false;
			}

			// Free any memory associated with this event
			hr = pME->FreeEventParams( lEventCode, lParam1, lParam2 );
		}

		if( playing )
		{
			MSG msg;
			msg.message=NULL;

			if (PeekMessage(&msg,NULL,0U,0U,PM_REMOVE)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			GEngine->InputSys->Tick();

			DWORD escState = GEngine->InputSys->GetKeyStatus( VK_ESCAPE );
			if( escState )
				playing = false;

			Sleep( 40 );
		}
	}

	pMC->Stop();

	// Relinquish ownership (IMPORTANT!) after hiding video window
    if( pVW )
    {
        hr = pVW->put_Visible( 0 );
        hr = pVW->put_Owner( NULL );
    }

    // Release and zero DirectShow interfaces
    SAFE_RELEASE( pME );
    SAFE_RELEASE( pMS );
    SAFE_RELEASE( pMC );
    SAFE_RELEASE( pBA );
    SAFE_RELEASE( pBV );
    SAFE_RELEASE( pVW );
    SAFE_RELEASE( pGB );
*/
	return true;
}


//--------------------------------------------------------------------

typedef struct
{
	float start;
	float end;
	String text;
} Sub;

bool PlayBink( const char *name, int sndTrack )
{/*
	HBINK bink = BinkOpen( name, 0 );
	if( !bink )
		return false;

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	HBINKBUFFER binkBuffer;
	if( GEngine->Video->IsFullScreen )
	{
		BinkBufferSetResolution( screenWidth, screenHeight, 32 );
		binkBuffer = BinkBufferOpen( GEngine->Video->GetActiveView()->GetTopmostWindow(),
									bink->Width, bink->Height,
									//BINKBUFFERAUTO | BINKBUFFERRESOLUTION );
									BINKBUFFERAUTO | BINKBUFFERSTRETCHX | BINKBUFFERSHRINKX
									| BINKBUFFERSTRETCHY | BINKBUFFERSHRINKY );
	}
	else
	{
		binkBuffer = BinkBufferOpen( GEngine->Video->GetActiveView()->GetActiveWindow(),
									bink->Width, bink->Height,
									BINKBUFFERAUTO | BINKBUFFERSTRETCHX | BINKBUFFERSHRINKX
									| BINKBUFFERSTRETCHY | BINKBUFFERSHRINKY );
	}

	if( !binkBuffer )
	{
		BinkClose( bink );
		return false;
	}

//	if( !GEngine->Video->IsFullScreen )
		BinkBufferSetScale( binkBuffer, screenWidth, screenHeight );

	while( 1 )
	{
		MSG msg;
		msg.message=NULL;

		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			if( msg.message == WM_QUIT )
				break;
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			if( BinkWait( bink ) )
			{
				GEngine->InputSys->Tick();

				DWORD escState = GEngine->InputSys->GetKeyStatus( VK_ESCAPE );
				if( escState )
				{
					GEngine->InputSys->Reset();
					break;
				}
			}
			else
			{
				// Show frame
				BinkDoFrame( bink );

				if( BinkBufferLock( binkBuffer ) )
				{
					BinkCopyToBuffer( bink,
									  binkBuffer->Buffer,
									  binkBuffer->BufferPitch,
									  binkBuffer->Height,
									  0, 0,
									  binkBuffer->SurfaceType );

					BinkBufferUnlock( binkBuffer );
				}

				BinkBufferBlit( binkBuffer, bink->FrameRects, BinkGetRects( bink, binkBuffer->SurfaceType ) );

				if( bink->FrameNum == bink->Frames )
					break;
				else
					BinkNextFrame( bink );
			}
		}
	}

	BinkClose( bink );
	BinkBufferClose( binkBuffer );
*/

	View *view = GEngine->Video->GetActiveView();
	RenderDevice* dev = GEngine->GetRenderDevice();

	WINDOWPLACEMENT place;
	memset( &place, 0, sizeof( WINDOWPLACEMENT ) );
	place.length = sizeof( WINDOWPLACEMENT );
	if( GetWindowPlacement( view->GetTopmostWindow(), &place ) )
	{
		if( place.showCmd == SW_SHOWMINIMIZED )
			return false;
	}

	if( GetForegroundWindow() != view->GetTopmostWindow() )
		return false;

	int screenWidth = view->LocalViewport.Width;
	int screenHeight = view->LocalViewport.Height;

	bool esc_pressed = false;

    m_setTag("Menu");
	Movie *mov = new Movie();
	if( !mov->Open( name, false, sndTrack ) )
	{
		GLog.Print( "Cannot open movie: %s\n", name );
		delete mov;
		return false;
	}

	ScriptObject cfg = GScript.Globals()["Cfg"];
    const char *lang = "";


	if( !cfg["Language"].IsNil() )
		lang = cfg["Language"].GetString();

	DynamicArray<Sub *> mSubs;

	if ( !strcmp(lang, "polish") )
	{
#ifdef TRANSGAMING
        const int   BUFSIZE = 256;
		char        subName[BUFSIZE];
        int         len;


		strncpy(subName, name, BUFSIZE);
        len = strlen(name);

        strncpy(&subName[len - 3], "srt", BUFSIZE - len);
#else
		char subName[255];
		strcpy( subName, name );
		subName[strlen(name)-3] = 0;
		sprintf( subName, "%ssrt", subName );
#endif

		Sub curr;
		curr.start = 0.0f;
		curr.end = 0.0f;
		curr.text = "";

		int currText = 1;

		GFile *f = GFileMan.CreateFileReader(subName);
		if( f )
		{
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

				int hrsStart = 0;
				int minStart = 0;
				int secStart = 0;
				int milStart = 0;
				int hrsEnd = 0;
				int minEnd = 0;
				int secEnd = 0;
				int milEnd = 0;
				if( sscanf( buf, "%d:%d:%d,%d --> %d:%d:%d,%d", &hrsStart, &minStart, &secStart, &milStart, &hrsEnd, &minEnd, &secEnd, &milEnd ) )
				{
					curr.start = hrsStart * 3600.0f + minStart * 60.0f + ( float )secStart + ( float )milStart / 1000.0f;
					curr.end = hrsEnd * 3600.0f + minEnd * 60.0f + ( float )secEnd + ( float )milEnd / 1000.0f;
				}
				else if( strlen( buf ) > 1 )
				{
                    m_setTag("Menu");
					Sub *s = new Sub;
					s->start = curr.start;
					s->end = curr.end;
					s->text = buf;
					mSubs.Add( s );
				}
			}
			GFileMan.Close( f );
		}
	}

	float startTime = GEngine->SysDriver.GetCurrentTime();
	float currTime = 0;
	int lastSub = 0;

	mov->Play();

	DWORD old = view->LocalViewport.ClearColor;
	view->LocalViewport.ClearColor = 0x00000000;

	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( "timesbd", 30 );

	while( mov->IsPlaying() )
	{
		MSG msg;
		msg.message=NULL;

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
//				if( mov->Update() )
					mov->Draw();
				int linesLeft = 3;
				for( int i = lastSub; i<mSubs.Size(); i++ )
				{
					if( mSubs[i]->start <= currTime && mSubs[i]->end >= currTime )
					{
						hud->SetFont( "timesbd", 30 );
						int tw = hud->GetTextWidth( mSubs[i]->text.AsChar() );
						hud->Print( ( screenWidth - tw ) / 2, screenHeight - hud->GetTextHeight() * linesLeft, mSubs[i]->text.AsChar(), 0xffffffff );
						linesLeft--;
						if( linesLeft == 2 )
							lastSub = i;
						else if( linesLeft < 0 )
							break;
					}
				}

//				hud->SetFont( "tahomabd", 22 );
//				hud->Print( 0, 0, String::Sprintf("%.2f",currTime).AsChar(), 0xffffffff );

				dev->Unlock();
				dev->Flip( view->GetActiveWindow() );
			}

			currTime = GEngine->SysDriver.GetCurrentTime() - startTime;
		}
	}
	mov->Stop();
	delete mov;

	view->LocalViewport.ClearColor = old;

	for( int i=0; i<mSubs.Size(); i++ )
		delete mSubs[i];
	mSubs.ClearArray();
    
	return true;
}
