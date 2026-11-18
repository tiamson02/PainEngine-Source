
// #pragma once
#ifndef _MPSTATS_
#define _MPSTATS_

#include "Core/DynArray.h"
#include "Core/LeString.h"


struct PlayerStats
{
	int		pid;
	String	name;
	int		frags;
	int		kills;
	int		deaths;
	int		ping;
#ifdef	TRANSGAMING_PS2
	int		gtime;
#else
	int		time;
#endif
	int		plost;
	int		team;
	int 	ready;
	int		spec;
};


class MenuItemBorder;


class MultiStats
{
public:

	MultiStats();
	~MultiStats();

	void Draw();

	void Activate( bool active );
	bool Active() const { return mActive; }

	void Clear();

	void SetFont( const char *name, int size = 0 );

	void SetPosition( int x, int y );
	void SetSize( int w, int h );

	void AddPlayer( PlayerStats &stats );
	void RemovePlayer( int player );
	void Update( PlayerStats &stats );
    void SetPlayerGameTime( int player, int time );
	int GetPlayerGameTime( int player );

	void SetFragLimit( const char *limit ) { mFragLimit = limit; }
	void SetTimeLimit( const char * limit ) { mTimeLimit = limit; }
	void SetTimeLeft( const char * left ) { mTimeLeft = left; }
	void SetCaptureLimit( const char * limit ) { mCaptureLimit = limit; }
	void SetLMSLives( const char * lives ) { mLMSLives = lives; }

	const char *GetFragLimit() const { return mFragLimit.AsChar(); }
	const char *GetTimeLimit() const { return mTimeLimit.AsChar(); }
	const char *GetTimeLeft() const { return mTimeLeft.AsChar(); }
	const char *GetCaptureLimit() const { return mCaptureLimit.AsChar(); }
	const char *GetLMSLives() const { return mLMSLives.AsChar(); }

	int GetTeam1Score() const { return mTeam1Score; }
	int GetTeam2Score() const { return mTeam2Score; }
	void SetTeam1Score( int s ) { mTeam1Score = s; }
	void SetTeam2Score( int s ) { mTeam2Score = s; }

	// Demo recording
	void SaveForDemo( FileObject *f );
	void LoadForDemo( FileObject *f );

private:

	void DrawRow( int row, const char *player, const char *frags, const char *kills, const char *deaths, const char *ping, const char *time, const char *plost, int team, const char *ready, DWORD color = 0xFFFFFFFF );
	int FindPlayer( const char *name );
	void DrawLimits();

	int FindPlayer( int pid );

	bool	mActive;

	String	mFont;
	int		mFontSize;

	int		mNumColumns;

	int		mX;
	int		mY;
	int		mWidth;
	int		mHeight;

	int		mPositions[7];

	bool	mTeamMode;
	int		teamWFragsFromDisconnectedPlayers;
	int		teamRFragsFromDisconnectedPlayers;
	bool	mShowReady;
	bool	mIsCTF;
	bool	mIsLMS;

	String	mFragLimit;
	String	mTimeLimit;
	String	mTimeLeft;
	String	mCaptureLimit;
	String	mLMSLives;

	DynamicArray<PlayerStats *> mStats;

	int		mTeam1Score;
	int		mTeam2Score;

	MenuItemBorder	*mBorder;
	MenuItemBorder	*mBorderLow;
	MenuItemBorder	*mLimitBorder;
};


void MPStats_RegisterLUALib();


#endif //_MPSTATS_

