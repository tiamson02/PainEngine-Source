
// #pragma once
#ifndef _MOVIE_
#define _MOVIE_


class Texture;

//#include <DShow.h>
//#include <vfw.h>

#ifndef TRANSGAMING_NO_BINK
// It doesn't appear that our bink recognizes the PSP platform.  We'll
// have to update this later.
#include <bink.h>
#endif


class ENGINE_API Movie
{
public:

	Movie() : mLoop( false ), mIsPlaying( false ), mTexture( NULL ), mCurrFrame( -1 ), mStartFrame( 0 ),
//			  mEndFrame( 0 ), mAviStream( NULL ), mGetFrame( NULL ) {}
#ifndef TRANSGAMING_NO_BINK
			  mEndFrame( 0 ), mPosX( 0 ), mPosY( 0 ), mBink( NULL ) {}
#else
			  mEndFrame( 0 ), mPosX( 0 ), mPosY( 0 ) {}
#endif
	~Movie() { Close(); }

	bool	Open( const char *name, bool preload = false, int sndTrack = -1 );
	void	Close();
	bool	Play( bool loop = false );
	bool	PlayFrames( int start = -1, int end = -1, bool loop = false );
	void	Loop( bool loop ) { mLoop = loop; }
	void	Pause( bool p = true );
	bool	Seek( int frame, bool pause = false );
	bool	Update();
	void	Stop();
	int		GetCurrentFrame() const { return mCurrFrame; }

	bool	IsPlaying() const { return mIsPlaying; }
	bool	LoopEnabled() const { return mLoop; }

	bool	Draw();

	Texture	*GetTexture() const { return mTexture; }

	void SetPosition( int x, int y ) { mPosX = x; mPosY = y; }

private:

	int		mWidth;
	int		mHeight;

	bool	mLoop;
	bool	mIsPlaying;
	Texture	*mTexture;

	int		mCurrFrame;
	int		mStartFrame;
	int		mEndFrame;

	float	mTimePerFrame;
	float	mLastTime;

	int		mLastRendered;

	int		mPosX;
	int		mPosY;
/*
	// Windows AVI stuff
	AVISTREAMINFO	mStreamInfo;
	PAVISTREAM		mAviStream;
	PGETFRAME		mGetFrame;
*/
	long32			mLastFrame;

#ifndef TRANSGAMING_NO_BINK
	HBINK			mBink;
#endif
};


bool PlayWMV( const char *name );
bool PlayBink( const char *name, int sndTrack = -1 );


#endif //_MOVIE_

