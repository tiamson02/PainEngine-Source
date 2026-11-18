#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Render/Video.h"

#include "Engine/PainMenu.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Net/DemoRecording2.h"


static String gWhiteStr = "";
static String gRedStr = "";

static String gPlayerStr = "";
static String gFragsStr = "";
static String gScoreStr = "";
static String gKillsStr = "";
static String gDeathsStr = "";
static String gPingStr = "";
static String gTimeStr = "";
static String gPacketLossStr = "";
static String gReadyStr = "";
static String gBreakStr = "";
static String gSpecStr = "";
static String gFragLimitStr = "";
static String gTimeLimitStr = "";
static String gTimeLeftStr = "";
static String gCaptureLimitStr = "";
static String gLMSLivesStr = "";


MultiStats::MultiStats()
{
	mActive = false;

	SetFont( "arial", 30 );

	mNumColumns = 7;

	mX = 0;
	mY = 0;
	mWidth = 0;
	mHeight = 0;

	mPositions[0] = 0;
	mPositions[1] = 240;
	mPositions[2] = 330;
	mPositions[3] = 420;
	mPositions[4] = 510;
	mPositions[5] = 600;
	mPositions[6] = 690;

	mFragLimit = "0";
	mTimeLimit = "0";
	mTimeLeft = "0";
	mCaptureLimit = "0";
	mLMSLives = "0";

	mBorder = NULL;
	mBorderLow = NULL;
	mLimitBorder = NULL;

	mTeam1Score = 0;
	mTeam2Score = 0;

	mTeamMode = false;
	teamWFragsFromDisconnectedPlayers = 0;
	teamRFragsFromDisconnectedPlayers = 0;
	mShowReady = false;
	mIsCTF = false;
	mIsLMS = false;

	ScriptObject txt = GScript.Globals()["TXT"];
	gWhiteStr = String::Sprintf( "%s Blue", txt["Menu"]["TeamName"].GetString() );
	gRedStr = String::Sprintf( "%s Red", txt["Menu"]["TeamName"].GetString() );

    ScriptObject stats = GScript.Globals()["TXT"]["MPStats"];
    gPlayerStr = stats["Player"].GetString();
    gFragsStr = stats["Frags"].GetString();
	gScoreStr = stats["Score"].GetString();
    gKillsStr = stats["Kills"].GetString();
    gDeathsStr = stats["Deaths"].GetString();
	gPingStr = stats["Ping"].GetString();  
	gTimeStr = stats["Time"].GetString();
    gPacketLossStr = stats["PacketLoss"].GetString();
	gReadyStr = stats["Ready"].GetString();
	gBreakStr = stats["Break"].GetString();
	gSpecStr = stats["SpecShort"].GetString();
	gFragLimitStr = stats["FragLimit"].GetString();
	gTimeLimitStr = stats["TimeLimit"].GetString();
	gTimeLeftStr = stats["TimeLeft"].GetString();
	gCaptureLimitStr = stats["CaptureLimit"].GetString();
	gLMSLivesStr = stats["LMSLives"].GetString();
}


MultiStats::~MultiStats()
{
	Clear();
}


void MultiStats::Clear()
{
	for( int i=0; i<mStats.Size(); i++ )
	{
		delete mStats[i];
	}
	teamWFragsFromDisconnectedPlayers = 0;
	teamRFragsFromDisconnectedPlayers = 0;
	mStats.ClearArray();

	if( mBorder )
	{
		delete mBorder;
		mBorder = NULL;
	}

	if( mBorderLow )
	{
		delete mBorderLow;
		mBorderLow = NULL;
	}

	if( mLimitBorder )
	{
		delete mLimitBorder;
		mLimitBorder = NULL;
	}

	mTeam1Score = 0;
	mTeam2Score = 0;
}


void MultiStats::Activate( bool active )
{
	if( mActive == active )
		return;

	ScriptObject game = GScript.Globals()["MPCfg"];
#ifdef TRANSGAMING
	if( !game["GameMode"].IsNil())
#else
	if( !game["GameMode"].IsNil() && ( !gDemoRec || !gDemoRec->IsPlaying() ) )
#endif
	{
		mTeamMode = false;
		mShowReady = false;
		mIsCTF = false;
		mIsLMS = false;

		if( strcmp( game["GameMode"].GetString(), "Team Deathmatch" ) == 0 )
		{
			mTeamMode = true;
			mShowReady = true;
		}
		else if( strcmp( game["GameMode"].GetString(), "Capture The Flag" ) == 0 )
		{
			mTeamMode = true;
			mShowReady = true;
			mIsCTF = true;
		}
		else if( strcmp( game["GameMode"].GetString(), "Last Man Standing" ) == 0 )
		{
			mIsLMS = true;
			mShowReady = true;
		}
		else if( strcmp( game["GameMode"].GetString(), "People Can Fly" ) == 0
				 || strcmp( game["GameMode"].GetString(), "Duel" ) == 0 )
		{
			mShowReady = true;
		}
	}

	if( !mBorder )
	{
        m_setTag("Menu");
		mBorder = new MenuItemBorder();
		mBorder->SetHeader( 44 );
		if( mTeamMode )
		{
			mBorder->SetSize( 924, 274 );
			mBorder->SetColumnsCount( 8 );
			mBorder->SetColumnWidth( 7, 120 );
		}
		else
		{
			if( mShowReady )
			{
				mBorder->SetSize( 924, 508 );
				mBorder->SetColumnsCount( 8 );
				mBorder->SetColumnWidth( 7, 120 );
			}
			else
			{
				mBorder->SetSize( 824, 508 );
				mBorder->SetColumnsCount( 7 );
			}
		}

		mBorder->SetColumnWidth( 0, mPositions[1] - mPositions[0] );
		mBorder->SetColumnWidth( 1, mPositions[2] - mPositions[1] );
		mBorder->SetColumnWidth( 2, mPositions[3] - mPositions[2] );
		mBorder->SetColumnWidth( 3, mPositions[4] - mPositions[3] );
		mBorder->SetColumnWidth( 4, mPositions[5] - mPositions[4] );
		mBorder->SetColumnWidth( 5, mPositions[6] - mPositions[5] );
		mBorder->SetColumnWidth( 6, 824 - mPositions[6] );
	}

	if( !mBorderLow && mTeamMode )
	{
        m_setTag("Menu");
		mBorderLow = new MenuItemBorder();
		mBorderLow->SetSize( 924, 244 );
		mBorderLow->SetColumnsCount( 8 );
		mBorderLow->SetColumnWidth( 0, mPositions[1] - mPositions[0] );
		mBorderLow->SetColumnWidth( 1, mPositions[2] - mPositions[1] );
		mBorderLow->SetColumnWidth( 2, mPositions[3] - mPositions[2] );
		mBorderLow->SetColumnWidth( 3, mPositions[4] - mPositions[3] );
		mBorderLow->SetColumnWidth( 4, mPositions[5] - mPositions[4] );
		mBorderLow->SetColumnWidth( 5, mPositions[6] - mPositions[5] );
		mBorderLow->SetColumnWidth( 6, 824 - mPositions[6] );
		mBorderLow->SetColumnWidth( 7, 120 );
	}

	if( !mLimitBorder )
	{
        m_setTag("Menu");
		mLimitBorder = new MenuItemBorder();
		mLimitBorder->SetSize( 824, 60 );
		if( mTeamMode )
		{
			mLimitBorder->SetColumnsCount( 2 );
			mLimitBorder->SetColumnWidth( 0, 412 );
			mLimitBorder->SetColumnWidth( 1, 412 );
		}
	}

	mActive = active;

	if( active )
	{
		SetFont( "timesbd", 22 );
		if( mTeamMode || mShowReady )
		{
			SetSize( 924, 508 );
			SetPosition( 40, 100 );
		}
		else
		{
			SetSize( 824, 508 );
			SetPosition( 100, 100 );
		}
	}
}


void MultiStats::Draw()
{
	if( !mActive )
		return;

	if( mTeamMode || mShowReady )
	{
		SetSize( 924, 508 );
		SetPosition( 40, 100 );
	}
	else
	{
		SetSize( 824, 508 );
		SetPosition( 100, 100 );
	}

	if( mBorder )
	{
		mBorder->SetPosition( mX, mY );
		mBorder->CalcPosition();
		mBorder->Render();
	}

	if( mBorderLow )
	{
		mBorderLow->SetPosition( mX, mY + 266 );
		mBorderLow->CalcPosition();
		mBorderLow->Render();
	}

	if( mLimitBorder )
	{
		int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
		float scaleY = screenHeight / 768.0f;
		if( mLimitBorder )
			mLimitBorder->SetPosition( mX, mY + 508 - (int)ceil( 5.0f / scaleY ) );
		mLimitBorder->CalcPosition();
		mLimitBorder->Render();
	}

	// Header
#ifdef TRANSGAMING
	if( mIsCTF )
		DrawRow( 0, gPlayerStr.AsChar(), gScoreStr.AsChar(), gKillsStr.AsChar(), gDeathsStr.AsChar(), gPingStr.AsChar(), gTimeStr.AsChar(), gPacketLossStr.AsChar(), 0, gReadyStr.AsChar(), 0xFF0000ff );
	else
		DrawRow( 0, gPlayerStr.AsChar(), gFragsStr.AsChar(), gKillsStr.AsChar(), gDeathsStr.AsChar(), gPingStr.AsChar(), gTimeStr.AsChar(), gPacketLossStr.AsChar(), 0, gReadyStr.AsChar(), 0xFF0000ff );
#else
	if( mIsCTF )
		DrawRow( 0, gPlayerStr.AsChar(), gScoreStr.AsChar(), gKillsStr.AsChar(), gDeathsStr.AsChar(), gPingStr.AsChar(), gTimeStr.AsChar(), gPacketLossStr.AsChar(), 0, gReadyStr.AsChar(), 0xFFFF0000 );
	else
		DrawRow( 0, gPlayerStr.AsChar(), gFragsStr.AsChar(), gKillsStr.AsChar(), gDeathsStr.AsChar(), gPingStr.AsChar(), gTimeStr.AsChar(), gPacketLossStr.AsChar(), 0, gReadyStr.AsChar(), 0xFFFF0000 );
#endif

    static const int    STATSIZE = 16;
	static char         frags[STATSIZE];
	static char         kills[STATSIZE];
	static char         deaths[STATSIZE];
	static char         ping[STATSIZE];
	static char         time[STATSIZE];
	static char         plost[STATSIZE];
	static char         ready[STATSIZE];


	int team1Rows = 0;
	int team2Rows = 1;

	int numPlayers = 0;

	for( int i=0; i<mStats.Size(); i++ )
	{
		if( mStats[i]->spec )
			continue;


		snprintf( frags,    STATSIZE, "%d",     mStats[i]->frags );
		snprintf( kills,    STATSIZE, "%d",     mStats[i]->kills );
		snprintf( deaths,   STATSIZE, "%d",     mStats[i]->deaths );
		snprintf( ping,     STATSIZE, "%d",     mStats[i]->ping );
		snprintf( plost,    STATSIZE, "%d%%",   mStats[i]->plost );

        
        if( mStats[i]->ready == 1 )
            strncpy(ready, "OK", STATSIZE);

		else if( mStats[i]->ready == 2 )
            strncpy(ready, gBreakStr.AsChar(), STATSIZE);

		else
            strncpy(ready, "-", STATSIZE);


#ifdef	TRANSGAMING_PS2
		int elapsed = (int) ( GEngine->SysDriver.GetCurrentTime() - mStats[i]->gtime );
#else
		int elapsed = ( (int)GEngine->SysDriver.GetCurrentTime() - mStats[i]->time );
#endif
		snprintf( time, STATSIZE, "%02d:%02d", ( int )floor( ( float )elapsed / 60.0f ), ( int )( elapsed % 60 ) );


#ifdef TRANSGAMING
		if (!mTeamMode)
			DrawRow( numPlayers + 1, mStats[i]->name.AsChar(), frags, kills, deaths, ping, time, plost, mStats[i]->team, ready, 0xFF7aBAff );

		else{
			if( mStats[i]->team == 0 ){
				DrawRow( team1Rows + 1, mStats[i]->name.AsChar(), frags, kills, deaths, ping, time, plost, mStats[i]->team, ready, 0xFF7aBAff );
				team1Rows++;
			}
			else{
				DrawRow( team2Rows, mStats[i]->name.AsChar(), frags, kills, deaths, ping, time, plost, mStats[i]->team, ready, 0xFF7aBAff );
				team2Rows++;
			}
		}
#else
		if (!mTeamMode)
			DrawRow( numPlayers + 1, mStats[i]->name.AsChar(), frags, kills, deaths, ping, time, plost, mStats[i]->team, ready, 0xFFFFBA7A );

		else{
			if( mStats[i]->team == 0 ){
				DrawRow( team1Rows + 1, mStats[i]->name.AsChar(), frags, kills, deaths, ping, time, plost, mStats[i]->team, ready, 0xFFFFBA7A );
				team1Rows++;
			}
			else{
				DrawRow( team2Rows, mStats[i]->name.AsChar(), frags, kills, deaths, ping, time, plost, mStats[i]->team, ready, 0xFFFFBA7A );
				team2Rows++;
			}
		}
#endif

		numPlayers++;
	}

#ifdef TRANSGAMING
	if( numPlayers < mStats.Size() && !mTeamMode && numPlayers > 0 )
		DrawRow( numPlayers + 1, "-------------------------", "------", "------", "------", "------", "------", "------", 0, "------", 0xFF7aBAff );

	for( int i=0; i<mStats.Size(); i++ ){
		if( !mTeamMode && mStats[i]->spec ){
			DrawRow( numPlayers + 2, mStats[i]->name.AsChar(), gSpecStr.AsChar(), "-", "-", ping, "-", "-", mStats[i]->team, "-", 0xFF7aBAff );
			numPlayers++;
		}
	}
#else
	if( numPlayers < mStats.Size() && !mTeamMode && numPlayers > 0 )
		DrawRow( numPlayers + 1, "-------------------------", "------", "------", "------", "------", "------", "------", 0, "------", 0xFFFFBA7A );

	for( int i=0; i<mStats.Size(); i++ )
	{
		if( !mTeamMode && mStats[i]->spec )
		{
			DrawRow( numPlayers + 2, mStats[i]->name.AsChar(), gSpecStr.AsChar(), "-", "-", ping, "-", "-", mStats[i]->team, "-", 0xFFFFBA7A );
			numPlayers++;
		}
	}
#endif
	DrawLimits();
}


void MultiStats::DrawRow( int row, const char *player, const char *frags, const char *kills, const char *deaths, const char *ping, const char *time, const char *plost, int team, const char *ready, DWORD color)
{
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

	int y = 0;
	int x1 = 0, x2 = 0, x3 = 0, x4 = 0, x5 = 0, x6 = 0, x7 = 0, x8 = 0;

	if( row == 0 && ( team == 0 || !mTeamMode ) )
	{
		y = (int)(( mY + 16 ) * scaleY);
		x1 = (int)(mX * scaleX + ( ( mPositions[1] - mPositions[0] ) * scaleX - hud->GetTextWidth( player ) ) / 2);
	}
	else
	{
		if( team == 0 || !mTeamMode )
			y = (int)(( mY + 38 ) * scaleY + row * hud->GetTextHeight());
		else
			y = (int)(( mY + 264 ) * scaleY + row * hud->GetTextHeight());
		x1 = (int)(( mX + 14 ) * scaleX);
	}

	x2 = (int)(mX * scaleX + mPositions[1] * scaleX + ( ( mPositions[2] - mPositions[1] ) * scaleX - hud->GetTextWidth( frags ) ) / 2);
	x3 = (int)(mX * scaleX + mPositions[2] * scaleX + ( ( mPositions[3] - mPositions[2] ) * scaleX - hud->GetTextWidth( kills ) ) / 2);
	x4 = (int)(mX * scaleX + mPositions[3] * scaleX + ( ( mPositions[4] - mPositions[3] ) * scaleX - hud->GetTextWidth( deaths ) ) / 2);
	x5 = (int)(mX * scaleX + mPositions[4] * scaleX + ( ( mPositions[5] - mPositions[4] ) * scaleX - hud->GetTextWidth( ping ) ) / 2);
	x6 = (int)(mX * scaleX + mPositions[5] * scaleX + ( ( mPositions[6] - mPositions[5] ) * scaleX - hud->GetTextWidth( time ) ) / 2);
	x7 = (int)(mX * scaleX + mPositions[6] * scaleX + ( ( 814 - mPositions[6] ) * scaleX - hud->GetTextWidth( plost ) ) / 2);
	x8 = (int)(mX * scaleX + 814 * scaleX + ( 120 * scaleX - hud->GetTextWidth( ready ) ) / 2);

	hud->Print( x1, y, player, color );
	hud->Print( x2, y, frags, color );
	hud->Print( x3, y, kills, color );
	hud->Print( x4, y, deaths, color );
	hud->Print( x5, y, ping, color );
	hud->Print( x6, y, time, color );
	hud->Print( x7, y, plost, color );


	if( mTeamMode || mShowReady )
	{
		//if( ready == "OK" )
		//{
#ifdef TRANSGAMING
			if (team == 1 || row == 0)
				hud->Print( x8, y, ready, 0xff0000ff );
			else if (team == 0) 
				hud->Print( x8, y, ready, 0xffff0000 );
#else
			if (team == 1 || row == 0)
				hud->Print( x8, y, ready, 0xffff0000 );
			else if (team == 0) 
				hud->Print( x8, y, ready, 0xff0000ff );
#endif
		//}
		//else
		//	hud->Print( x8, y, ready.AsChar(), 0xffff0000 );
	}
}


void MultiStats::DrawLimits()
{
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

	int y = (int)(mLimitBorder->GetPosY() * scaleY + ( 70 * scaleY - hud->GetTextHeight() ) / 2);

#ifdef TRANSGAMING
    const int   BUFSIZE = 128;
	char        limit[BUFSIZE];


	if( mIsCTF )
		snprintf( limit, BUFSIZE, "%s: %s", gCaptureLimitStr.AsChar(), mCaptureLimit.AsChar() );
	else if( mIsLMS )
		snprintf( limit, BUFSIZE, "%s: %s", gLMSLivesStr.AsChar(), mLMSLives.AsChar() );
	else
		snprintf( limit, BUFSIZE, "%s: %s", gFragLimitStr.AsChar(), mFragLimit.AsChar() );
	if( !mTeamMode )
		hud->Print( (int)(120 * scaleX), y, limit, 0xFF7aBAff );
	else
		hud->Print( (int)(60 * scaleX), y, limit, 0xFF7aBAff );

	snprintf( limit, BUFSIZE, "%s: %s", gTimeLeftStr.AsChar(), mTimeLeft.AsChar() );


    if( !mTeamMode )
		hud->Print( (int)(300 * scaleX), y, limit, 0xFF7aBAff );
	else
		hud->Print( (int)(240 * scaleX), y, limit, 0xFF7aBAff );
#else
	char limit[128];
	if( mIsCTF )
		sprintf( limit, "%s: %s", gCaptureLimitStr.AsChar(), mCaptureLimit.AsChar() );
	else if( mIsLMS )
		sprintf( limit, "%s: %s", gLMSLivesStr.AsChar(), mLMSLives.AsChar() );
	else
		sprintf( limit, "%s: %s", gFragLimitStr.AsChar(), mFragLimit.AsChar() );
	if( !mTeamMode )
		hud->Print( (int)(120 * scaleX), y, limit, 0xFFFFBA7A );
	else
		hud->Print( (int)(60 * scaleX), y, limit, 0xFFFFBA7A );

	sprintf( limit, "%s: %s", gTimeLeftStr.AsChar(), mTimeLeft.AsChar() );


	if( !mTeamMode )
		hud->Print( (int)(300 * scaleX), y, limit, 0xFFFFBA7A );
	else
		hud->Print( (int)(240 * scaleX), y, limit, 0xFFFFBA7A );
#endif


//	int t1Frags = 0;
//	int t2Frags = 0;

	if( mTeamMode )
	{
/*		for( int i=0; i<mStats.Size(); i++ )
		{
			if( mStats[i]->team == 0 )
			{
				t1Frags += mStats[i]->frags;
			}
			else
			{
				t2Frags += mStats[i]->frags;
			}
		}
		t1Frags += teamWFragsFromDisconnectedPlayers;
		t2Frags += teamRFragsFromDisconnectedPlayers;
*/
		int t1Frags = mTeam1Score;
		int t2Frags = mTeam2Score;

#ifdef TRANSGAMING
		snprintf( limit, BUFSIZE, "%s: %d", gWhiteStr.AsChar(), t1Frags );
		//sprintf( frags, "%s: %d %d", gWhiteStr.AsChar(), t1Frags, teamWFragsFromDisconnectedPlayers );
		if( !mTeamMode )
			hud->Print( (int)(540 * scaleX), y, limit, 0xFF7aBAff );
		else
			hud->Print( (int)(480 * scaleX), y, limit, 0xffff0000 );
		snprintf( limit, BUFSIZE, "%s: %d", gRedStr.AsChar(), t2Frags );
		//sprintf( frags, "%s: %d %d", gRedStr.AsChar(), t2Frags, teamRFragsFromDisconnectedPlayers);


		if( !mTeamMode )
			hud->Print( (int)(760 * scaleX), y, limit, 0xFF7aBAff );
		else
			hud->Print( (int)(700 * scaleX), y, limit, 0xff0000ff );
#else
		char frags[128];
		sprintf( frags, "%s: %d", gWhiteStr.AsChar(), t1Frags );
		//sprintf( frags, "%s: %d %d", gWhiteStr.AsChar(), t1Frags, teamWFragsFromDisconnectedPlayers );
		if( !mTeamMode )
			hud->Print( (int)(540 * scaleX), y, frags, 0xFFFFBA7A );
		else
			hud->Print( (int)(480 * scaleX), y, frags, 0xff0000ff );
		sprintf( frags, "%s: %d", gRedStr.AsChar(), t2Frags );
		//sprintf( frags, "%s: %d %d", gRedStr.AsChar(), t2Frags, teamRFragsFromDisconnectedPlayers);

        if( !mTeamMode )
			hud->Print( (int)(760 * scaleX), y, frags, 0xFFFFBA7A );
		else
			hud->Print( (int)(700 * scaleX), y, frags, 0xffff0000 );
#endif
	}
}


void MultiStats::SetFont( const char *name, int size )
{
	mFont = name;
	mFontSize = size;
}


void MultiStats::SetPosition( int x, int y )
{
	mX = x;
	mY = y;

	if( mBorder )
		mBorder->SetPosition( x, y );

	if( mBorderLow )
		mBorderLow->SetPosition( x, y + 266 );

	if( mLimitBorder )
	{
		mLimitBorder->SetPosition( x, y + 508 );
	}
}


void MultiStats::SetSize( int w, int h )
{
	mWidth = w;
	mHeight = h;

	if( mBorder )
	{
		if( mTeamMode )
			mBorder->SetSize( w, 274 );
		else
			mBorder->SetSize( w, h );
	}

	if( mBorderLow )
	{
		mBorderLow->SetSize( w, 244 );
	}

	if( mLimitBorder )
	{
		mLimitBorder->SetSize( w, 60 );
	}
}


int MultiStats::FindPlayer(const char *name){
	for (int i=0; i<mStats.Size(); i++){
		if( !strcmp(mStats[i]->name.AsChar(), name) )
			return i;
	}

	return -1;
}


int MultiStats::FindPlayer( int pid )
{
	for( int i=0; i<mStats.Size(); i++ )
	{
		if( mStats[i]->pid == pid )
			return i;
	}

	return -1;
}


void MultiStats::AddPlayer( PlayerStats &stats )
{
	if( mStats.Size() < 1 )
	{
		ScriptObject game = GScript.Globals()["MPCfg"];
		mTeamMode = false;
		if( !game["GameMode"].IsNil() )
		{
			if( strcmp( game["GameMode"].GetString(), "Team Deathmatch" ) == 0  || strcmp( game["GameMode"].GetString(), "Capture The Flag" ) == 0 )
			{
				mTeamMode = true;
			}
		}
	}
	
    m_setTag("Game");
	PlayerStats *snew = new PlayerStats();
	snew->name = stats.name;
	snew->pid = stats.pid;
	if( mTeamMode )
		snew->team = stats.team;
	else
		snew->team = 0;
#ifdef	TRANSGAMING_PS2
	snew->gtime = (int)GEngine->SysDriver.GetCurrentTime();
#else
	snew->time = (int)GEngine->SysDriver.GetCurrentTime();
#endif	
	snew->ready = stats.ready;
	snew->spec = stats.spec;
	snew->frags = 0;
	snew->kills = 0;
	snew->deaths = 0;
	mStats.Add( snew );
	Update( stats );

	if( mStats.Size() > 17 )
		SetFont( "timesbd", 18 );
	else
		SetFont( "timesbd", 22 );
}


static int SortStats( const void *elem1, const void *elem2 )
{
	PlayerStats *s1 = *( ( PlayerStats **)elem1 );
	PlayerStats *s2 = *( ( PlayerStats **)elem2 );

	if( s2->team > s1->team )
		return 1;
	else if( s2->team < s1->team )
		return -1;

	if( s2->frags > s1->frags )
		return 1;
	else if( s2->frags < s1->frags )
		return -1;

	if( s2->kills > s1->kills )
		return 1;
	else if( s2->kills < s1->kills )
		return -1;

	if( s2->deaths < s1->deaths )
		return 1;
	else if( s2->deaths > s1->deaths )
		return -1;

	return ( strcmp( s2->name.AsChar(), s1->name.AsChar() ) > 0 ) ? -1 : 1;
}


void MultiStats::RemovePlayer( int player )
{
	int index = FindPlayer( player );
	if( index > -1 )
	{
		if( mStats[index]->team == 0 )
		{
			teamWFragsFromDisconnectedPlayers += mStats[index]->frags;
		}
		else
		{
			teamRFragsFromDisconnectedPlayers += mStats[index]->frags;
		}

		delete mStats[index];
		mStats.Remove( index );
	}

	qsort( mStats.Elements(), mStats.Size(), sizeof( PlayerStats *), SortStats );
}


void MultiStats::SetPlayerGameTime( int player, int time )
{
	int index = FindPlayer( player );
	if( index > -1 )
	{
#ifdef	TRANSGAMING_PS2
		mStats[index]->gtime = (int)(GEngine->SysDriver.GetCurrentTime() - time);
#else
		mStats[index]->time = (int)GEngine->SysDriver.GetCurrentTime() - time;
#endif
	}

	qsort( mStats.Elements(), mStats.Size(), sizeof( PlayerStats *), SortStats );
}


int MultiStats::GetPlayerGameTime( int player )
{
	int index = FindPlayer( player );
	if( index > -1 )
#ifdef	TRANSGAMING_PS2
		return mStats[index]->gtime;
#else
		return mStats[index]->time;
#endif
	return 0;
}


void MultiStats::Update( PlayerStats &stats )
{
	int index = FindPlayer( stats.pid );
	if( index == -1 )
	{
		AddPlayer( stats );
	}
	else
	{
		PlayerStats *st = mStats[index];

		int fragsDiff = stats.frags - st->frags;
		if( mTeamMode && fragsDiff != 0 && st->team == stats.team )
		{
			if( stats.team == 0 )
				mTeam1Score += fragsDiff;
			else
				mTeam2Score += fragsDiff;
		}

		st->pid = stats.pid;
		st->name = stats.name;
		st->frags = stats.frags;
		st->kills = stats.kills;
		st->deaths = stats.deaths;
		st->ping = stats.ping;
		st->plost = stats.plost;
		if( mTeamMode )
			st->team = stats.team;
		else
			st->team = 0;
		st->ready = stats.ready;
		st->spec = stats.spec;
	}

	qsort( mStats.Elements(), mStats.Size(), sizeof( PlayerStats *), SortStats );
}


//--------------------------------------------------------------------
//	Demo recording
//--------------------------------------------------------------------
void MultiStats::SaveForDemo( FileObject *f )
{
	*f << mTeamMode;
	*f << teamWFragsFromDisconnectedPlayers;
	*f << teamRFragsFromDisconnectedPlayers;
	*f << mShowReady;
	*f << mIsCTF;
	*f << mIsLMS;

	*f << mFragLimit;
	*f << mTimeLimit;
	*f << mTimeLeft;
	*f << mCaptureLimit;
	*f << mLMSLives;

	*f << mTeam1Score;
	*f << mTeam2Score;

	*f << mStats.Size();
	for( int i = 0; i < mStats.Size(); i++ )
	{
		*f << mStats[i]->deaths;
		*f << mStats[i]->frags;
		*f << mStats[i]->kills;
		*f << mStats[i]->name;
		*f << mStats[i]->pid;
		*f << mStats[i]->ping;
		*f << mStats[i]->plost;
		*f << mStats[i]->ready;
		*f << mStats[i]->spec;
		*f << mStats[i]->team;
#ifdef	TRANSGAMING_PS2
		int elapsed = (int)( GEngine->SysDriver.GetCurrentTime() - mStats[i]->gtime );
#else
		int elapsed = ( (int)GEngine->SysDriver.GetCurrentTime() - mStats[i]->time );
#endif
		*f << elapsed;
	}
}


void MultiStats::LoadForDemo( FileObject *f )
{
	*f >> mTeamMode;
	*f >> teamWFragsFromDisconnectedPlayers;
	*f >> teamRFragsFromDisconnectedPlayers;
	*f >> mShowReady;
	*f >> mIsCTF;
	*f >> mIsLMS;

	*f >> mFragLimit;
	*f >> mTimeLimit;
	*f >> mTimeLeft;
	*f >> mCaptureLimit;
	*f >> mLMSLives;

	*f >> mTeam1Score;
	*f >> mTeam2Score;

	int size = 0;
	*f >> size;

	for( int i = 0; i < mStats.Size(); i++ )
	{
		delete mStats[i];
	}
	mStats.ClearArray();

	for( int i = 0; i < size; i++ )
	{
        m_setTag("Game");
		PlayerStats *stat = new PlayerStats;
		*f >> stat->deaths;
		*f >> stat->frags;
		*f >> stat->kills;
		*f >> stat->name;
		*f >> stat->pid;
		*f >> stat->ping;
		*f >> stat->plost;
		*f >> stat->ready;
		*f >> stat->spec;
		*f >> stat->team;
#ifdef	TRANSGAMING_PS2
		*f >> stat->gtime;
		stat->gtime = (int)(GEngine->SysDriver.GetCurrentTime() - stat->gtime);
#else
		*f >> stat->time;
		stat->time = (int)GEngine->SysDriver.GetCurrentTime() - stat->time;
#endif
		mStats.Add( stat );
	}

	if( !Active() ) Activate( true );
}


//--------------------------------------------------------------------
//	LUA Interface
//--------------------------------------------------------------------
static int luaMPStats_Update( lua_State *state )
{
	Script L( state );

	PlayerStats st;
	st.pid = L.GetInt( 1 );
	st.name = L.GetString( 2 );
	st.frags = L.GetInt( 3 );
	st.kills = L.GetInt( 4 );
	st.deaths = L.GetInt( 5 );
	st.ping = L.GetInt( 6 );
	st.plost = L.GetInt( 7, 0 );
	st.team = L.GetInt( 8, 0 );
	st.ready = L.GetInt( 9, 0 );
	st.spec = L.GetInt( 10, 0 );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.Update( st );

	return 0;
}


static int luaMPStats_RemovePlayer( lua_State *state )
{
	Script L( state );

	int pid = L.GetInt( 1 );

    if( GEngine->Video )
        GEngine->Video->Hud->MPStats.RemovePlayer( pid );

	return 0;
}


static int luaMPStats_SetPlayerGameTime( lua_State *state )
{
	Script L( state );

	int pid = L.GetInt( 1 );
	int time = L.GetInt( 2 );

    if( GEngine->Video )
        GEngine->Video->Hud->MPStats.SetPlayerGameTime( pid, time );

	return 0;
}


static int luaMPStats_ClearAll( lua_State *state )
{
	Script L( state );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.Clear();

	return 0;
}


static int luaMPStats_Show( lua_State *state )
{
	Script L( state );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.Activate( true );

	return 0;
}


static int luaMPStats_Hide( lua_State *state )
{
	Script L( state );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.Activate( false );

	return 0;
}


static int luaMPStats_IsActive( lua_State *state )
{
	Script L( state );

    if (GEngine->Video)
	{
		L.PushBool( GEngine->Video->Hud->MPStats.Active() );
		return 1;
	}

	return 0;
}


static int luaMPStats_SetFragLimit( lua_State *state )
{
	Script L( state );

	const char *limit = L.GetString( 1 );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.SetFragLimit( limit );

	return 0;
}


static int luaMPStats_SetCaptureLimit( lua_State *state )
{
	Script L( state );

	const char *limit = L.GetString( 1 );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.SetCaptureLimit( limit );

	return 0;
}


static int luaMPStats_SetLMSLives( lua_State *state )
{
	Script L( state );

	const char *limit = L.GetString( 1 );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.SetLMSLives( limit );

	return 0;
}


static int luaMPStats_SetTimeLimit( lua_State *state )
{
	Script L( state );

	const char *limit = L.GetString( 1 );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.SetTimeLimit( limit );

	return 0;
}


static int luaMPStats_SetTimeLeft( lua_State *state )
{
	Script L( state );

	const char *left = L.GetString( 1 );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.SetTimeLeft( left );

	return 0;
}


static int luaMPStats_GetTeamsScore( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
	{
		L.PushInt( GEngine->Video->Hud->MPStats.GetTeam1Score() );
		L.PushInt( GEngine->Video->Hud->MPStats.GetTeam2Score() );
		return 2;
	}

	return 0;
}


static int luaMPStats_SetTeamsScore( lua_State *state )
{
	Script L( state );

	if( GEngine->Video )
	{
		GEngine->Video->Hud->MPStats.SetTeam1Score( L.GetInt( 1 ) );
		GEngine->Video->Hud->MPStats.SetTeam2Score( L.GetInt( 2 ) );
	}

	return 0;
}


static int luaMPStats_Draw( lua_State *state )
{
	Script L( state );

    if (GEngine->Video)
        GEngine->Video->Hud->MPStats.Draw();

	return 0;
}


static int luaMPStats_GetPlayerTime( lua_State *state )
{
	Script L( state );

	int pid = L.GetInt( 1 );

    if (GEngine->Video)
	{
		L.PushInt( GEngine->Video->Hud->MPStats.GetPlayerGameTime( pid ) );
		return 1;
	}

	return 0;
}


static const luaL_reg p_mpstats_lib[] = {
	{ "Update", luaMPStats_Update },
	{ "RemovePlayer", luaMPStats_RemovePlayer },
    { "SetPlayerGameTime", luaMPStats_SetPlayerGameTime},
	{ "ClearAll", luaMPStats_ClearAll },
	{ "Show", luaMPStats_Show },
	{ "Hide", luaMPStats_Hide },
	{ "IsActive", luaMPStats_IsActive },
	{ "SetCaptureLimit", luaMPStats_SetCaptureLimit },
	{ "SetLMSLives", luaMPStats_SetLMSLives },
	{ "SetFragLimit", luaMPStats_SetFragLimit },
	{ "SetTimeLimit", luaMPStats_SetTimeLimit },
	{ "SetTimeLeft", luaMPStats_SetTimeLeft },
	{ "GetTeamsScore", luaMPStats_GetTeamsScore },
	{ "SetTeamsScore", luaMPStats_SetTeamsScore },
	{ "GetPlayerTime", luaMPStats_GetPlayerTime },
	{ "Draw", luaMPStats_Draw },
	{ NULL, NULL }
};


void MPStats_RegisterLUALib()
{
	GScript.RegisterLibrary("MPSTATS",p_mpstats_lib);
}
