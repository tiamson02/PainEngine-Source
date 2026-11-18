#include "Engine.h"
#include "Net/GameSpyData.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Render/HUD.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Render/View.h"
#include "Engine/InputSystem.h"

#include "Net/NetworkDevice2.h"

#include "Render/Video.h"

#include "Engine/PainMenu.h"

#ifndef	TRANSGAMING_NO_GAMESPY
#include "Net/GameSpy2/qr2/qr2.h"
#include "Net/GameSpy2/serverbrowsing/sb_serverbrowsing.h"
#endif

extern void RegisterQR2Keys( void );

//--------------------------------------------------------------------
//	MenuItemServerData class
//--------------------------------------------------------------------
MenuItemServerData::MenuItemServerData() : MenuItem()
{
	mType = EMenuItemServerData;
	mGSServer = NULL;
	mIndex = 0;
	mSelected = false;
	mNumVisible = 0;
	mIsLocal = false;
	mSortColumn = 0;
}


MenuItemServerData::~MenuItemServerData()
{
}


void MenuItemServerData::Render()
{
	if( !mVisible )
		return;

	if( mY == -1 )
		return;

	int y = mY;

	// Shadow
	if( mDrawShadow )
		DrawAt( mX + 1, y + 1, CalcTransColor( 0x50000000 ) );

	// Normal
	DWORD color = 0;
	if( !mDisabled && !mSelected && mIndex != 0 )
		color = CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor );
	else if( mIndex == 0 )
		color = CalcTransColor( mTextColor );
	else
		color = CalcTransColor( mDisabledColor );

	DrawAt( mX, y, color );
}


#define COL1	280
#define COL2	460
#define COL3	540
#define COL4	660
#define	COL5	720
#define COL6	810


void MenuItemServerData::DrawAt( int x, int y, DWORD color )
{
	int columns[6];
	columns[0] = (int)(COL1 * mScaleX - 5);
	columns[1] = (int)(COL2 * mScaleX - 5);
	columns[2] = (int)(COL3 * mScaleX - 5);
	columns[3] = (int)(COL4 * mScaleX - 5);
	columns[4] = (int)(COL5 * mScaleX - 5);
	columns[5] = (int)(COL6 * mScaleX - 5);

	HUD *hud = GEngine->Video->Hud;

	hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	int sortColor = ( mIndex != 0 ) ? color : CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor );
	hud->Print( x, y, mText.AsChar(), ( mSortColumn == 0 ) ? sortColor : color );
	hud->Print( x + columns[0] + ( ( columns[1] - columns[0] ) - hud->GetTextWidth( mMap.AsChar() ) ) / 2, y, mMap.AsChar(), ( mSortColumn == 1 ) ? sortColor : color );
	hud->Print( x + columns[1] + ( ( columns[2] - columns[1] ) - hud->GetTextWidth( mNumPlayers.AsChar() ) ) / 2, y, mNumPlayers.AsChar(), ( mSortColumn == 2 ) ? sortColor : color );
	hud->Print( x + columns[2] + ( ( columns[3] - columns[2] ) - hud->GetTextWidth( mGame.AsChar() ) ) / 2, y, mGame.AsChar(), ( mSortColumn == 3 ) ? sortColor : color );
	hud->Print( x + columns[3] + ( ( columns[4] - columns[3] ) - hud->GetTextWidth( mPing.AsChar() ) ) / 2, y, mPing.AsChar(), ( mSortColumn == 4 ) ? sortColor : color );
	hud->Print( (int)(x + columns[4] + ( ( 870 * mScaleX - columns[4] ) - hud->GetTextWidth( mIP.AsChar() ) ) / 2), y, mIP.AsChar(), ( mSortColumn == 5 ) ? sortColor : color );
}


void MenuItemServerData::CalcSize()
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	mWidth = (int)(( GEngine->Video->PMenu->Screen.GetMenuWidth() - 40 ) * mScaleX);
	mHeight = GEngine->Video->Hud->GetTextHeight();
}


void MenuItemServerData::CalcPosition()
{
	mX = (int)(( mPosX + 10 ) * mScaleX);

	if( GEngine->Video->PMenu->Screen.GetFirstShown() <= mIndex && ( GEngine->Video->PMenu->Screen.GetFirstShown() + mNumVisible > mIndex ) && mIndex > 0 )
	{
		mY = (int)(GEngine->Video->PMenu->Screen.GetTopPos() * mScaleY + ( mIndex - GEngine->Video->PMenu->Screen.GetFirstShown() ) * mHeight);
		if( GEngine->Video->PMenu->Screen.GetScroller() )// && GEngine->Video->PMenu->Screen.GetScroller()->GetMax() >= 28 )
			mY +=  (int)(16 * mScaleY);
	}
	else if( mIndex == 0 )
		mY = (int)(( GEngine->Video->PMenu->Screen.GetTopPos() - 14 ) * mScaleY);
	else
		mY = -1;

	if( mY != -1 )
		mY += (int)(12 * mScaleY);
}


#ifndef	TRANSGAMING_NO_GAMESPY
//--------------------------------------------------------------------
//	MenuItemServerList class
//--------------------------------------------------------------------
void SBCallback( ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance )
{
	MenuItemServerList *list = ( MenuItemServerList *)instance;

	switch( reason )
	{
	case sbc_serveradded:
		GLog.Print( "sbc_serveradded\n" );
		list->AddServer( server );
		if( !GEngine->MyPublicIPAddress ) GEngine->MyPublicIPAddress = ServerBrowserGetMyPublicIPAddr( sb );
		break;

	case sbc_serverupdated:
		GLog.Print( "sbc_serverupdated\n" );
		list->AddServer( server );
		if( !GEngine->MyPublicIPAddress ) GEngine->MyPublicIPAddress = ServerBrowserGetMyPublicIPAddr( sb );
		break;

	case sbc_serverupdatefailed:
		GLog.Print( "sbc_serverupdatefailed\n" );
		break;

	case sbc_updatecomplete:
		list->SetStatus( MenuItemServerList::EListStatusIdle );
		if( !GEngine->MyPublicIPAddress ) GEngine->MyPublicIPAddress = ServerBrowserGetMyPublicIPAddr( sb );
		GLog.Print( "sbc_updatecomplete\n" );
		break;

	case sbc_queryerror:
		GLog.Print( "sbc_queryerror\n" );
		break;
	}
}

MenuItemServerList::MenuItemServerList() : MenuItemList()
{
	mServerBrowser = NULL;
	mType = EMenuItemServerList;
	mMaxHeight = 0;
	mNumVisible = 0;
	mStatusText = "";
	mStatus = EListStatusNone;
	mTotalPlayers = 0;
	mIsLAN = false;
	mIsFavorites = false;
	mBorder = NULL;
	mScroller = NULL;
	mStatBorder = NULL;
	mSelected = -1;
	mManualIP = NULL;
	mSortColumn = MenuItemServerList::EListSortName;
}


MenuItemServerList::~MenuItemServerList()
{
	Clear();
}


void MenuItemServerList::Render()
{
	if( !mVisible )
		return;

	UpdateScale();
	CalcPosition();

	if( mStatBorder )
	{
		mStatBorder->UpdateScale();
		mStatBorder->CalcPosition();
		mStatBorder->Render();
	}

	if( mBorder )
	{
		mBorder->UpdateScale();
		mBorder->CalcPosition();
		mBorder->Render();
	}

	for( int i=0; i<mServers.Size(); i++ )
	{
		mServers[i]->UpdateScale();
		mServers[i]->CalcSize();
		mServers[i]->CalcPosition();
		mServers[i]->SetUnderMouse( false );
	}

	for( int i=0; i<mServers.Size(); i++ )
	{
		if( mServers[i]->CheckIfUnderMouse() )
			break;
	}

	for( int i=0; i<mServers.Size(); i++ )
	{
		mServers[i]->CalcPosition();
		mServers[i]->Render();
		mServers[i]->RenderDesc();
	}

	DrawInfoLine();
}


void MenuItemServerList::DrawInfoLine()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	int y = ( GEngine->Video->PMenu->Screen.GetTopPos() + mMaxHeight + 40 ) * screenHeight / 768.0f;
	int xInfo = ( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * ( screenWidth / 1024.0f ) ) / 2 ) + 20 * ( screenWidth / 1024.0f );

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
	int statWidth = GEngine->Video->Hud->GetTextWidth( mStatusText.AsChar() );
	if( statWidth <= 0 && !mStatusText.IsNull() )
	{
		statWidth = mStatusText.Len() * 8 * ( screenWidth / 1024.0f );
	}

	int xStat = screenWidth - xInfo - statWidth - 320 * mScaleX;

	DWORD color = mServers[0]->GetTextColor();

    int t = GScript.GetTop();
	ScriptObject txt = GScript.Globals()["TXT"];
	char info[255];
	String servers = txt["Menu"]["Servers"].GetString();
	String players = txt["Menu"]["Players"].GetString();
	sprintf( info, "%s: %d, %s: %d\n", servers.AsChar(), mServers.Size() - 1, players.AsChar(), mTotalPlayers );
	GEngine->Video->Hud->Print( xInfo, y, info, color );
    GScript.SetTop(t);

	if( !mStatusText.IsNull() && statWidth > 0 )
		GEngine->Video->Hud->Print( xStat, y, mStatusText.AsChar(), color );

	DrawEnterIP();

#ifndef	TRANSGAMING_NO_GAMESPY
	if( mServerBrowser )
		ServerBrowserThink( (ServerBrowser)mServerBrowser );
#endif		
}


void MenuItemServerList::DrawEnterIP()
{
	if( !mManualIP )
	{

		ScriptObject cfg = GScript.Globals()["Cfg"];
		ScriptObject txt = GScript.Globals()["TXT"];

		const char *ip = "0.0.0.0";
		if( !cfg["ManualIP"].IsNil() )
			ip = cfg["ManualIP"].GetString();

        m_setTag("Menu");
		mManualIP = new MenuItemTextEdit();
		mManualIP->SetName( "ManualIP" );
		mManualIP->SetText( String( txt["Menu"]["EnterIP"].GetString() ) + ": " );
		mManualIP->SetDesc( txt["MenuDesc"]["EnterIP"].GetString() );
		mManualIP->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
		mManualIP->SetPosition( 650, 560 );
		mManualIP->SetBigFont( mBigFont, mBigSize );
		mManualIP->SetSmallFont( mSmallFont.AsChar(), mSmallSize );
		mManualIP->SetAlign( EAlignNone );
		mManualIP->SetMaxLength( 20 );
		mManualIP->SetConfigOption( "ManualIP" );
		mManualIP->SetEditText( ip );
	}

	if( mSelected == 0xff )
		mManualIP->SetColors( mDisabledColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	else
		mManualIP->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );

	mManualIP->UpdateScale();
	mManualIP->CalcSize();
	mManualIP->CalcPosition();
	mManualIP->CheckIfUnderMouse();
	mManualIP->Render();
	mManualIP->RenderDesc();
}


void MenuItemServerList::CalcSize()
{
	if( mServers.Size() == 0 )
	{
		mWidth = 840 * mScaleX;
		mHeight = mMaxHeight * mScaleY;
		mNumVisible = 28;
	}
	else
	{
		mWidth = 840 * mScaleX;
		mNumVisible = ( mMaxHeight * mScaleY ) / mServers[0]->GetHeight() - 1;
		mHeight = mMaxHeight * mScaleY;

		for( int i=0; i<mServers.Size(); i++ )
		{
			mServers[i]->SetNumVisible( mNumVisible );
		}
	}

	mColumns[0] = mX + COL1;
	mColumns[1] = mX + COL2;
	mColumns[2] = mX + COL3;
	mColumns[3] = mX + COL4;
	mColumns[4] = mX + COL5;
	mColumns[5] = mX + COL6;

	if( mBorder )
	{
		mBorder->SetSize( ( int )( ( float )mWidth / mScaleX ) + 40, mMaxHeight + 40 );
		mBorder->SetColumnsCount( 6 );
		mBorder->SetColumnWidth( 0, mColumns[0] - mX );
		mBorder->SetColumnWidth( 1, mColumns[1] - mColumns[0] );
		mBorder->SetColumnWidth( 2, mColumns[2] - mColumns[1] );
		mBorder->SetColumnWidth( 3, mColumns[3] - mColumns[2] );
		mBorder->SetColumnWidth( 4, mColumns[4] - mColumns[3] );
		mBorder->SetColumnWidth( 5, mColumns[5] - mColumns[4] );
		mBorder->SetHeader( 40 );
	}

	if( mStatBorder )
	{
		mStatBorder->SetSize( ( int )( ( float )mWidth / mScaleX ) + 40, 60 );
	}
}


void MenuItemServerList::CalcPosition()
{
    if( !mBorder ){
        m_setTag("Menu");
		mBorder = new MenuItemBorder();
    }

	if( !mStatBorder )
	{
        m_setTag("Menu");
		mStatBorder = new MenuItemBorder();
		mStatBorder->SetDark( true );
		mStatBorder->SetColumnsCount( 2 );
		mStatBorder->SetColumnWidth( 0, 560 );
		mStatBorder->SetColumnWidth( 1, 300 );
	}

	UpdateScale();

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	mX = ( ( screenWidth - ( GEngine->Video->PMenu->Screen.GetMenuWidth() - 40 ) * mScaleX ) / 2 );
	mY = GEngine->Video->PMenu->Screen.GetTopPos() * mScaleY;

	mBorder->SetPosition( ( float )mX / mScaleX - 20, ( float )mY / mScaleY - 20 );
	mStatBorder->SetPosition( ( float )mX / mScaleX - 20, ( float )mY / mScaleY + mMaxHeight + 16 );

	CalcSize();

	if( mServers.Size() < 1 || mServers[0] == NULL )
		return;

	POINT mousePos = GEngine->InputSys->GetMousePosition();
	mousePos.x /= mScaleX;

	if( mousePos.x > mX && mousePos.x < mColumns[0] )
		mServers[0]->SetSortColumn( MenuItemServerList::EListSortName );
	else if( mousePos.x > mColumns[0] && mousePos.x < mColumns[1] )
		mServers[0]->SetSortColumn( MenuItemServerList::EListSortMap );
	else if( mousePos.x > mColumns[1] && mousePos.x < mColumns[2] )
		mServers[0]->SetSortColumn( MenuItemServerList::EListSortNumPlayers );
	else if( mousePos.x > mColumns[2] && mousePos.x < mColumns[3] )
		mServers[0]->SetSortColumn( MenuItemServerList::EListSortGame );
	else if( mousePos.x > mColumns[3] && mousePos.x < mColumns[4] )
		mServers[0]->SetSortColumn( MenuItemServerList::EListSortPing );
	else if( mousePos.x > mColumns[4] && mousePos.x < mColumns[5] )
		mServers[0]->SetSortColumn( MenuItemServerList::EListSortIP );
	else
		mServers[0]->SetSortColumn( -1 );
}


void MenuItemServerList::Clear()
{
	Stop( false );

	for( int i=0; i<mServers.Size(); i++ )
	{
		delete mServers[i];
	}
	mServers.ClearArray();


#ifndef	TRANSGAMING_NO_GAMESPY
	if( mServerBrowser )
	{
		ServerBrowserFree( (ServerBrowser)mServerBrowser );
		mServerBrowser = NULL;
	}
#endif

	mTotalPlayers = 0;

	if( mScroller )
	{
		GEngine->Video->PMenu->Screen.SetScroller( NULL );
		delete mScroller;
		mScroller = NULL;
	}

	if( mBorder )
	{
		delete mBorder;
		mBorder = NULL;
	}

	if( mStatBorder )
	{
		delete mStatBorder;
		mStatBorder = NULL;
	}

	if( mManualIP )
	{
		delete mManualIP;
		mManualIP = NULL;
	}
}


void MenuItemServerList::SortList( int column, bool reset )
{
	static bool lastDir = false;

	if( reset ) lastDir = !lastDir;

	if( mSortColumn == column )
		lastDir = !lastDir;
	else
		lastDir = true;

	bool asc = lastDir;

	switch( column )
	{
	case EListSortName:
		ServerBrowserSort( (ServerBrowser)mServerBrowser, ( SBBool )asc, "hostname", sbcm_stricase );
		break;

	case EListSortMap:
		ServerBrowserSort( (ServerBrowser)mServerBrowser, ( SBBool )asc, "mapname", sbcm_stricase );
		break;

	case EListSortNumPlayers:
		ServerBrowserSort( (ServerBrowser)mServerBrowser, ( SBBool )asc, "numplayers", sbcm_int );
		break;

	case EListSortGame:
		ServerBrowserSort( (ServerBrowser)mServerBrowser, ( SBBool )asc, "gametype", sbcm_stricase );
		break;

	case EListSortPing:
		ServerBrowserSort( (ServerBrowser)mServerBrowser, ( SBBool )asc, "ping", sbcm_int );
		break;

	case EListSortIP:
		ServerBrowserSort( (ServerBrowser)mServerBrowser, ( SBBool )asc, "hostaddr", sbcm_stricase );
		break;
	}

	MenuItemServerData *first = mServers[0];

	for( int i=1; i<mServers.Size(); i++ )
	{
		delete mServers[i];
	}
	mServers.ClearArray();

	mServers.Add( first );

	mTotalPlayers = 0;

	for( int i=0; i<ServerBrowserCount( (ServerBrowser)mServerBrowser ); i++ )
	{
		SBServer server = ServerBrowserGetServer( (ServerBrowser)mServerBrowser, i );
		if( SBServerHasBasicKeys( server ) )
			AddServer( server );
	}

	mSortColumn = column;
	mServers[0]->SetSortColumn( column );
}


void MenuItemServerList::Stop( bool updateStatus )
{
	if( !mServerBrowser )
		return;

	if( ServerBrowserState( (ServerBrowser)mServerBrowser ) != sb_disconnected )
		ServerBrowserHalt( (ServerBrowser)mServerBrowser );

	if( updateStatus )
		SetStatus( MenuItemServerList::EListStatusIdle );
}


void MenuItemServerList::Refresh()
{
	Clear();

	ScriptObject txt = GScript.Globals()["TXT"];

	// Table header
    m_setTag("Menu");
	MenuItemServerData *item = new MenuItemServerData;
	item->SetText( txt["Menu"]["ServerName"].GetString() );
	item->SetDesc( "" );
	item->SetAction( "" );
	item->SetMap( txt["Menu"]["Map"].GetString() );
	item->SetNumberOfPlayers( txt["Menu"]["Players"].GetString() );
	item->SetGame( txt["Menu"]["Game"].GetString() );
	item->SetPing( txt["Menu"]["Ping"].GetString() );
	if( mIsLAN )
		item->SetLocal( true );
	item->SetIP( "IP" );
	item->SetBigFont( mBigFont, mBigSize );
	item->SetSmallFont( mSmallFont.AsChar(), mSmallSize );
	item->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );

	item->CalcSize();

	item->SetPosition( ( 1024.0f - GEngine->Video->PMenu->Screen.GetMenuWidth() ) / 2, 0 );
	item->SetIndex( 0 );
	item->Disable();

	mServers.Add( item );

	UpdateScale();
	CalcPosition();

	char secret_key[9];

	secret_key[4] = 'd';
	secret_key[0] = 'k';
	secret_key[6] = '\0';
	secret_key[2] = 'F';
	secret_key[3] = '4';
	secret_key[1] = '7';
	secret_key[5] = '2';

	if( !GEngine->GameSpyOK )
	{
		// Print error message
		mStatusText = txt["Menu"]["CouldntFindAnyServer"].GetString();
		return;
	}

#ifndef	TRANSGAMING_NO_GAMESPY
//	ServerBrowser serverBrowser = ServerBrowserNew( "unreal", GAMESPY_GAMENAME, secret_key, 0, 10, /*QVERSION_QR2*/QVERSION_GOA, SBCallback, this );
	mServerBrowser = ( void *)ServerBrowserNew( GAMESPY_GAMENAME, GAMESPY_GAMENAME, secret_key, 0, 10, QVERSION_QR2, SBCallback, this );

	if( !mServerBrowser )
	{
		// Print error message
		mStatusText = txt["Menu"]["CouldntFindAnyServer"].GetString();
		return;
	}


	// TO DO - filter fields, etc.
	unsigned char basicFields[] = {HOSTNAME_KEY, GAMEVER_KEY, GAMENAME_KEY, GAMETYPE_KEY, MAPNAME_KEY, NUMPLAYERS_KEY, MAXPLAYERS_KEY, PING__KEY, GAMESPY_PKTV_PROXY_KEY};
	int numFields = sizeof(basicFields) / sizeof(basicFields[0]);

	if( !mIsLAN && !mIsFavorites )
	{
		RegisterQR2Keys();
		GEngine->MyPublicIPAddress = 0; // reset it, we'll get another soon
		ServerBrowserUpdate( (ServerBrowser)mServerBrowser, SBTrue, SBTrue, basicFields, numFields, NULL );
		GEngine->LastServerUpdateMethod = PCFSystem::INTERNET;
	}
	else if( !mIsFavorites )
	{
		ServerBrowserLANUpdate( (ServerBrowser)mServerBrowser, SBTrue, DEFAULT_SERVER_PORT, DEFAULT_SERVER_PORT+10 );
		GEngine->LastServerUpdateMethod = PCFSystem::LAN;
	}
	else
		GScript.DoString( "PainMenu:AddFavorites()" );

	SetStatus( MenuItemServerList::EListStatusIdle );

#endif
	
}


void MenuItemServerList::AddServer( void *server, const char *name, const char *map, int numPlayers, int maxPlayers, const char *game, int ping, const char *ip, unsigned short port )
{
	char text[20];
	MenuItemServerData *item = NULL;

	int index = FindServer( server );
	if( index == -1 )
	{
		index = mServers.Size();
        m_setTag("Menu");
		item = new MenuItemServerData;
		mServers.Add( item );
	}
	else
		item = mServers[index];

	ScriptObject txt = GScript.Globals()["TXT"];

	item->SetText( name );
	item->SetDesc( txt["Menu"]["ClickToSelect"].GetString() );
	item->SetAction( "" );
	item->SetMap( map );
	sprintf( text, "%d/%d", numPlayers, maxPlayers );
	item->SetNumberOfPlayers( text );
	if( !strcmp(game, "Free For All") )
		game = "FFA";
	else if( !strcmp(game, "Team Deathmatch") )
		game = "Team DM";
	else if( !strcmp(game, "The Light Bearer") )
		game = "TLB";
	else if( !strcmp(game, "Capture The Flag") )
		game = "CTF";
	else if( !strcmp(game, "People Can Fly") )
		game = "PCF";
	else if( !strcmp(game, "Last Man Standing") )
		game = "LMS";
	item->SetGame( game );
	sprintf( text, "%d", ping );
	item->SetPing( text );
	item->SetIP( ip );
	item->SetPort( port );
	item->SetBigFont( mBigFont, mBigSize );
	item->SetSmallFont( mSmallFont.AsChar(), mSmallSize );
	item->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
	item->SetLocal( mIsLAN );

	item->CalcSize();

	item->SetPosition( ( 1024.0f - GEngine->Video->PMenu->Screen.GetMenuWidth() ) / 2, 0 );
	item->SetIndex( index );
	item->SetServer( server );

	int itemCount = GetServerCount();
	if( itemCount > mNumVisible )
	{
		CalcSize();
		if( index <= mNumVisible )
		{
			if( !mScroller )
			{
				int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;

                m_setTag("Menu");
				mScroller = new MenuItemScroller;

				mScroller->SetName( (mName+"Scroller").AsChar() );
				mScroller->SetText( "" );
				mScroller->SetDesc( "" );
				mScroller->SetValue( 0 );
				mScroller->SetScrollerHeight( mMaxHeight );
				mScroller->SetPosition( mPosX + ( mWidth + 10 ) * 1024.0f / screenWidth, mPosY );
				mScroller->SetColors( mTextColor, mDisabledColor, mTextColorUnderMouse, mDescColor );
				mScroller->CalcSize();
				mScroller->CalcPosition();

				if( mBorder )
					mBorder->AddScroller( mScroller );

				GEngine->Video->PMenu->Screen.SetScroller( mScroller );
			}
		}
		if( itemCount > mNumVisible )
			mScroller->SetMinMax( 0, itemCount - mNumVisible - 1 );
	}

	mTotalPlayers += numPlayers;
}


void MenuItemServerList::AddServer( void *serv )
{
	ScriptObject txt = GScript.Globals()["TXT"];

	SBServer server = (SBServer)serv;
	String name = String( SBServerGetStringValue( server, "hostname", ( char *)( txt["Menu"]["NoName"].GetString() ) ) );
	String version = SBServerGetStringValue( server, "gamever", "" );
	String map = SBServerGetStringValue( server, "mapname", ( char *)( txt["Menu"]["NoMap"].GetString() ) );
	int numPlayers = SBServerGetIntValue( server, "numplayers", 0 );
	int maxPlayers = SBServerGetIntValue( server, "maxplayers", 0 );
	String type = SBServerGetStringValue( server, "gametype", "" );
	String pktv = SBServerGetStringValue( server, "PKTVProxy", "" );
	int ping = SBServerGetPing( server );
	if( ping == 9999 )
    {
		return;
    }

	if( name.Len() > 20 )
		name = name.SubStr( 20, 0 );

	if( !version.IsNull() )
	{
		name = String::Sprintf( "%s (v%s)", name.AsChar(), version.AsChar() );
	}

	String ip = "";
//	if( !mIsLAN )
		ip = SBServerGetPublicAddress( server );
//	else
//		ip = SBServerGetPrivateAddress( server );

	if( GEngine->Net )
	{
		if( ip == GEngine->Net->GetMyIP( NULL ) )
        {
			return;
        }
	}

	if( ip.IsNull() ) ip = txt["Menu"]["Unknown"].GetString();

	unsigned short port = SBServerGetPublicQueryPort( server );

	static const int maxStrLen = 30;

	if( name.Len() > maxStrLen )
		name = name.SubStr( maxStrLen, 0 );
	if( map.Len() > maxStrLen )
		map = map.SubStr( maxStrLen, 0 );
	if( type.Len() > maxStrLen )
		type = type.SubStr( maxStrLen, 0 );

	if( mIsPKTV )
	{
//		GLog.Print( "PKTVServer: %s, proxy: %s\n", name.AsChar(), pktv.AsChar() );
		if( pktv.Len() < 3 )
			return;
		else
			ip = pktv;
	}

	AddServer( serv, name, map, numPlayers, maxPlayers, type, ping, ip, port );
}


void MenuItemServerList::AddServer( const char *ip, unsigned short port )
{
	ServerBrowserAuxUpdateIP( (ServerBrowser)mServerBrowser, ( char *)ip, port, SBFalse, SBTrue, SBTrue );
}


void MenuItemServerList::SetStatus( int status )
{
	MenuItem *stop = NULL;

	ScriptObject txt = GScript.Globals()["TXT"];

	mStatus = status;
	switch( status )
	{
	case MenuItemServerList::EListStatusGetting:
	case MenuItemServerList::EListStatusSearchingLAN:
		mStatusText = txt["Menu"]["UpdatingServerList"].GetString();
		stop = GEngine->Video->PMenu->Screen.FindItem( "RefreshStop" );
		if( stop )
		{
			stop->SetText( txt["Menu"]["Stop"].GetString() );
			stop->SetDesc( txt["MenuDesc"]["Stop"].GetString() );
			stop->SetAction( "PainMenu:StopServerList()" );
		}
		break;

	case MenuItemServerList::EListStatusQuerying:
		mStatusText = txt["Menu"]["UpdatingServerList"].GetString();
		break;

	case MenuItemServerList::EListStatusIdle:
		mStatusText = txt["Menu"]["UpdateFinished"].GetString();
		stop = GEngine->Video->PMenu->Screen.FindItem( "RefreshStop" );
		if( stop )
		{
			stop->SetText( txt["Menu"]["Refresh"].GetString() );
			stop->SetDesc( txt["MenuDesc"]["Refresh"].GetString() );
			stop->SetAction( "PainMenu:RefreshServerList()" );
		}
		if( mServers.Size() <= 1 )
		{
			mStatusText = txt["Menu"]["CouldntFindAnyServer"].GetString();
		}
		else
		{
			SortList( mSortColumn, true );
		}
		break;

	default:
		mStatusText = txt["Menu"]["CouldntFindAnyServer"].GetString();
		break;
	}
}


bool MenuItemServerList::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	MenuItem *join = GEngine->Video->PMenu->Screen.FindItem( "Join" );
	MenuItem *spec = GEngine->Video->PMenu->Screen.FindItem( "Spectate" );
	MenuItem *addToFav = GEngine->Video->PMenu->Screen.FindItem( "AddToFavorites" );
	if( !join || !addToFav || !spec )
		return false;

	if( mScroller )
	{
		mScroller->SendEvent( eventType, eventData1, eventData2 );
	}

	if( spec->SendEvent( eventType, eventData1, eventData2 ) )
	{
		if( mSelected == 0xff && mManualIP )
		{
			GEngine->Video->PMenu->Screen.SetSelectedServer( NULL );
			GScript.DoString( "Cfg['ManualIP']='%s'; Cfg:Save()", mManualIP->GetEditText() );
		}
		return true;
	}

	if( join->SendEvent( eventType, eventData1, eventData2 ) )
	{
		if( mSelected == 0xff && mManualIP )
		{
			GEngine->Video->PMenu->Screen.SetSelectedServer( NULL );
			GScript.DoString( "Cfg['ManualIP']='%s'; Cfg:Save()", mManualIP->GetEditText() );
		}
		return true;
	}

	if( mManualIP )
	{
		if( mManualIP->SendEvent( eventType, eventData1, eventData2 ) )
		{
			GEngine->Video->PMenu->Screen.SetSelectedServer( NULL );
			mSelected = 0xff;
			join->Enable();
			spec->Enable();
			addToFav->SetVisibility( true );
			return true;
		}
	}

	static float selectTime = 0.0f;

	if( eventType == EMenuKeyUp && eventData1 == EMouseKeyLeft )
	{
		int sel = -1;
		if( mManualIP )
			if( mManualIP->IsInEditMode() )
				sel = 0xff;

		for( int i=0; i<mServers.Size(); i++ )
		{
			if( mServers[i]->UnderMouse() )
			{
				if( i > 0 )
				{
					if( !mServers[i]->IsSelected() )
					{
						mServers[i]->SetSelected( true );
						selectTime = GEngine->SysDriver.GetCurrentTime();
					}
					else if( !join->IsDisabled() )
					{
						if( GEngine->SysDriver.GetCurrentTime() - selectTime < 1.0f )
						{
							GEngine->Video->PMenu->Screen.SetItemAction( join->GetAction() );
							return true;
						}
						else
							selectTime = GEngine->SysDriver.GetCurrentTime();
					}

					sel = i;
				}
				else
				{
					if( mServers.Size() > 0 && mServers[0] != NULL )
						SortList( mServers[0]->GetSortColumn() );
				}
			}
			else if( !addToFav->CheckIfUnderMouse() )
				mServers[i]->SetSelected( false );
		}

		if( sel > -1 )
		{
			mSelected = sel;
			join->Enable();
			spec->Enable();
			addToFav->SetVisibility( true );
			GEngine->Video->PMenu->Screen.SetSelectedServer( GetServerData() );
		}
		else
		{
			GEngine->Video->PMenu->Screen.SetSelectedServer( NULL );
			if( !addToFav->CheckIfUnderMouse() && !join->CheckIfUnderMouse() )
			{
				if( mSelected == 0xff && mManualIP )
					GScript.DoString( "Cfg['ManualIP']='%s'; Cfg:Save()", mManualIP->GetEditText() );
				mSelected = -1;
				join->Disable();
				spec->Disable();
				GEngine->Video->PMenu->Screen.SetSelectedServer( NULL );
				addToFav->SetVisibility( false );
			}
		}
	}

	return false;
}


bool MenuItemServerList::CheckIfUnderMouse()
{
	return false;
}


int MenuItemServerList::FindServer( void *server )
{
	for( int i=0; i<mServers.Size(); i++ )
	{
		if( mServers[i]->GetServer() == server )
			return i;
	}

	return -1;
}


const char *MenuItemServerList::GetServerIP()
{
	if( mSelected == 0xff )
	{
		char ip[64];
		strcpy( ip, mManualIP->GetEditText() );
		char *port = strchr( ip, ':' );
		if( port ) *port = 0;
		return String( ip );
	}

	for( int i=0; i<mServers.Size(); i++ )
	{
		if( mServers[i]->IsSelected() )
			return mServers[i]->GetIP();
	}

	return "";
}


unsigned short MenuItemServerList::GetServerPort()
{
	if( mSelected == 0xff )
	{
		char ip[64];
		strcpy( ip, mManualIP->GetEditText() );
		char *port = strchr( ip, ':' );
		int p = DEFAULT_SERVER_PORT;
		if( port )
		{
			port++;
			sscanf( port, "%d", &p );
		}
		return p;
	}

	for( int i=0; i<mServers.Size(); i++ )
	{
		if( mServers[i]->IsSelected() )
			return mServers[i]->GetPort();
	}

	return 0;
}


void *MenuItemServerList::GetServerData()
{
	if( mSelected == 0xff )
		return NULL;

	for( int i=0; i<mServers.Size(); i++ )
	{
		if( mServers[i]->IsSelected() )
		{
			void *s = mServers[i]->GetServer();
			return s;
		}
	}

	return NULL;
}

#endif	//GAME_SPY

