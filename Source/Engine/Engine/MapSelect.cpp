#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Engine/BaseGame.h"
#include "Render/Video.h"
#include "Render/Material.h"

// Currently, in the miles headers, there is definitions for the case
// where it is being used by the PS2, but not the PSP.  We will have 
// to revisit this later, and add in PSP support.
#ifndef TRANSGAMING_NO_MILES
#include "Audio/MilesEngine.h"
#endif

#include "Engine/PainMenu.h"
#include "Engine/MapSelect.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;


static String chapterStr = "";
static String levelStr = "";
static String gSelectChapterStr = "";
static String gSelectLevelStr = "";
static String gLevelNotAvailableStr = "";


#define START_LEVEL 0
#define START_CHAPTER 0
#define DEMO_ONLY 0


LevelDesc::~LevelDesc(){
    /*Assert(GEngine->Video->MaterialSys);

    if (sketch && GEngine->Video->MaterialSys)
        GEngine->Video->MaterialSys->ReleaseTexture(sketch);//*/
}

MapSelect::MapSelect()
{
	mActive = false;
    mDisabled = false;
	mAddOn = false;

	mBackground = NULL;
    mCurrSketch = NULL;

#ifndef TRANSGAMING
	mShowMouse = false;
#endif
	mMouseClickOffsetX = 0;
	mMouseClickOffsetY = 0;
	mMouseTexture = NULL;

	mCard = NULL;
	mPent = NULL;
	mCrystal = NULL;

	mCurrChapter = START_CHAPTER;
	mTargetChapter = START_CHAPTER;
	mCurrLevel = START_LEVEL;
	mLastLevel = START_LEVEL;
	mLastCondition = "";

	mWindow = NULL;
	mWindowRot = 2.0f * 3.14159265358979323846f / 6.0f * START_LEVEL;
	mLoadingScreen = NULL;
	mFadeStart = 0.0f;
	mStartRot = 2.0f * 3.14159265358979323846f / 6.0f * START_LEVEL;
	mTargetRot = 2.0f * 3.14159265358979323846f / 6.0f * START_LEVEL;
	mTargetLevel = START_LEVEL;

	mPausedSounds = -1;
#ifdef TRANSGAMING
    mShowStats = false;
#endif

	for( int c=0; c<NUM_CHAPTERS; c++ )
	{
		for( int i=0; i<6; i++ )
		{
			mMaps[c][i] = NULL;
		}
		mNumMaps[c] = 0;
	}
}


MapSelect::~MapSelect()
{
	Clear();
}

const char *MapSelect::GetActionString(char *buf, size_t len) const{
    snprintf(buf, len, "PMENU.SwitchToMap(false); Game.CurrLevelette = 1; Lev:SetMap(%d, %d, Game.CurrLevelette); EDITOR.PostMessage(1,0)", mCurrChapter + 1, mCurrLevel + 1);
    
    return buf;
}

void MapSelect::Setup()
{
	GScript.DoString( "Levels_FillMap()" );

	ScriptObject txt = GScript.Globals()["TXT"];
	chapterStr = txt["Menu"]["Chapter"].GetString();
	levelStr = txt["Menu"]["Level"].GetString();
	gSelectLevelStr = txt["Menu"]["SelectLevel"].GetString();
	gSelectChapterStr = txt["Menu"]["SelectChapter"].GetString();
	gLevelNotAvailableStr = HUD_PrepareString( txt["Menu"]["LevelNotAvailable"].GetString() );

	ScriptObject game = GScript.Globals()["Game"];

	if( mAddOn )
		chapterStr = txt["Menu"]["BattleOutOfHell"].GetString();

	int numLevels = 6;
	if( mAddOn )
		numLevels = 10;

	// Levels
	for( int i=0; i<numLevels; i++ )
	{
#ifdef TRANSGAMING
        const int   BUFSIZE = 64;
		char        tex[3][BUFSIZE];


		snprintf(tex[0], BUFSIZE, "Level%d", i + 1);
		MenuItemImageButton *item = CreateButton(tex[0]);


		snprintf( tex[0], BUFSIZE, "HUD/Map/cyferka%d_normalna", i + 1 );
		snprintf( tex[1], BUFSIZE, "HUD/Map/cyferka%d_swiec", i + 1 );
		snprintf( tex[2], BUFSIZE, "HUD/Map/cyferka%d_swiec", i + 1 );
		item->SetImage( tex[0], tex[1], tex[2] );
		item->SetLightOnSound( "menu/mapselect/option-light-on" );
		item->SetDesc( gSelectLevelStr.AsChar() );
		item->SetColors( 0, 0, 0, 0xff1700d6 );
#else
		char name[50];
		sprintf( name, "Level%d", i + 1 );
		MenuItemImageButton *item = CreateButton( name );

		char tex[3][255];

		sprintf( tex[0], "HUD/Map/cyferka%d_normalna", i + 1 );
		sprintf( tex[1], "HUD/Map/cyferka%d_swiec", i + 1 );
		sprintf( tex[2], "HUD/Map/cyferka%d_swiec", i + 1 );
		item->SetImage( tex[0], tex[1], tex[2] );
		item->SetLightOnSound( "menu/mapselect/option-light-on" );
		item->SetDesc( gSelectLevelStr );
		item->SetColors( 0, 0, 0, 0xffd60017 );
#endif
		item->SetDescPos( 80, 680 );

		mLevels.Add( item );
	}

	if( !mAddOn )
	{
		mLevels[0]->SetPosition( 240, 80 );
		mLevels[1]->SetPosition( 383, 161 );
		mLevels[2]->SetPosition( 383, 324 );
		mLevels[3]->SetPosition( 239, 409 );
		mLevels[4]->SetPosition( 100, 324 );
		mLevels[5]->SetPosition( 100, 161 );
	}
	else
	{
		mLevels[0]->SetPosition( 240, 80 );
		mLevels[1]->SetPosition( 338, 110/*323, 101*/ );
		mLevels[2]->SetPosition( 399, 192/*383, 161*/ );
		mLevels[3]->SetPosition( 399, 294/*406, 242*/ );
		mLevels[4]->SetPosition( 340, 377/*383, 324*/ );
		mLevels[5]->SetPosition( 242, 409/*327, 385*/ );
		mLevels[6]->SetPosition( 145, 379/*242, 409*/ );
		mLevels[7]->SetPosition( 82, 294/*160, 387*/ );
		mLevels[8]->SetPosition( 82, 192/*99, 325*/ );
		mLevels[9]->SetPosition( 142, 110/*77, 242*/ );
//		mLevels[10]->SetPosition( 97, 160 );
//		mLevels[11]->SetPosition( 158, 98 );
	}

	// Levels selection
	for( int i=0; i<numLevels; i++ )
	{
#ifdef TRANSGAMING
        const int   BUFSIZE = 64;
		char        tex[BUFSIZE];


		snprintf(tex, BUFSIZE, "LevelSel%d", i + 1);
		MenuItemImageButton *item = CreateButton(tex);

		snprintf(tex, BUFSIZE, "HUD/Map/cyferka%d_swiec", i + 1);
#else
		char name[50];
		sprintf( name, "LevelSel%d", i + 1 );
		MenuItemImageButton *item = CreateButton( name );

		char tex[255];
		sprintf( tex, "HUD/Map/cyferka%d_swiec", i + 1 );
#endif

		item->SetImage( tex, tex, tex );

		mLevelsSel.Add( item );
	}

	if( !mAddOn )
	{
		mLevelsSel[0]->SetPosition( 240, 80 );
		mLevelsSel[1]->SetPosition( 383, 161 );
		mLevelsSel[2]->SetPosition( 383, 324 );
		mLevelsSel[3]->SetPosition( 239, 409 );
		mLevelsSel[4]->SetPosition( 100, 324 );
		mLevelsSel[5]->SetPosition( 100, 161 );
	}
	else
	{
		mLevelsSel[0]->SetPosition( 240, 80 );
		mLevelsSel[1]->SetPosition( 338, 110/*323, 101*/ );
		mLevelsSel[2]->SetPosition( 399, 192/*383, 161*/ );
		mLevelsSel[3]->SetPosition( 399, 294/*406, 242*/ );
		mLevelsSel[4]->SetPosition( 340, 377/*383, 324*/ );
		mLevelsSel[5]->SetPosition( 242, 409/*327, 385*/ );
		mLevelsSel[6]->SetPosition( 145, 379/*242, 409*/ );
		mLevelsSel[7]->SetPosition( 82, 294/*160, 387*/ );
		mLevelsSel[8]->SetPosition( 82, 192/*99, 325*/ );
		mLevelsSel[9]->SetPosition( 142, 110/*77, 242*/ );
//		mLevelsSel[10]->SetPosition( 97, 160 );
//		mLevelsSel[11]->SetPosition( 158, 98 );
	}

	// Question marks
	for( int i=0; i<numLevels; i++ )
	{
#ifdef TRANSGAMING
        const int   BUFSIZE = 64;
        char        name[BUFSIZE];


		snprintf(name, BUFSIZE, "Question%d", i + 1);
		MenuItemImageButton *item = CreateButton(name);
#else
        char name[50];
		sprintf( name, "Question%d", i + 1 );
		MenuItemImageButton *item = CreateButton( name );
#endif
		item->SetImage( "HUD/Map/question", "HUD/Map/question", "HUD/Map/question" );

		mQuestions.Add( item );
	}

	if( !mAddOn )
	{
		mQuestions[0]->SetPosition( 240, 80 );
		mQuestions[1]->SetPosition( 383, 161 );
		mQuestions[2]->SetPosition( 383, 324 );
		mQuestions[3]->SetPosition( 242, 409 );
		mQuestions[4]->SetPosition( 100, 324 );
		mQuestions[5]->SetPosition( 100, 161 );
	}
	else
	{
		mQuestions[0]->SetPosition( 240, 80 );
		mQuestions[1]->SetPosition( 338, 110/*323, 101*/ );
		mQuestions[2]->SetPosition( 399, 192/*383, 161*/ );
		mQuestions[3]->SetPosition( 399, 294/*406, 242*/ );
		mQuestions[4]->SetPosition( 340, 377/*383, 324*/ );
		mQuestions[5]->SetPosition( 242, 409/*327, 385*/ );
		mQuestions[6]->SetPosition( 145, 379/*242, 409*/ );
		mQuestions[7]->SetPosition( 82, 294/*160, 387*/ );
		mQuestions[8]->SetPosition( 82, 192/*99, 325*/ );
		mQuestions[9]->SetPosition( 142, 110/*77, 242*/ );
//		mQuestions[10]->SetPosition( 100, 161 );
//		mQuestions[11]->SetPosition( 160, 101 );
	}

	mCard = CreateButton( "Card" );
	mCard->SetDesc( txt["Menu"]["GoToBlackBoard"].GetString() );
	mCard->SetImage( "HUD/Map/karta_czysta", "HUD/Map/karta_swiec", "HUD/Map/karta_wcisnieta" );
	mCard->SetPosition( 753, 589 );
	mCard->SetAction( "PMENU.SwitchToBoard()" );
	mCard->SetLightOnSound( "menu/mapselect/card-light-on" );
	mCard->SetAcceptSound( "menu/mapselect/card-accept" );
#ifdef TRANSGAMING
    mCard->SetColors( 0, 0, 0, 0xff1700d6 );
#else
    mCard->SetColors( 0, 0, 0, 0xffd60017 );
#endif
/*	if( game["LevelStarted"].GetBool() )
	{
		mCard->Disable();
		mCard->SetDesc( txt["MenuDesc"]["BlackBoardInactive"].GetString() );
	}*/
	if( game["Difficulty"].GetInt() == 0 )
	{
		mCard->Disable();
		mCard->SetDesc( txt["MenuDesc"]["BlackBoardNotDaydream"].GetString() );
	}
	mCard->SetDescPos( 80, 680 );

#if DEMO_ONLY
	mCard->Disable();
	mCard->SetDesc( txt["MenuDesc"]["BlackBoardNotDemo"].GetString() );
#endif

	mPent = CreateButton( "Pent" );
	mPent->SetDesc( txt["Menu"]["GoToMainMenu"].GetString() );
	mPent->SetImage( "HUD/Map/pentagra_czysty", "HUD/Map/pentagra_swiec", "HUD/Map/pentagra_wcisniety" );
	mPent->SetPosition( 835, 583 );
	mPent->SetAction( "PMENU.SwitchToMenu()" );
	mPent->SetLightOnSound( "menu/mapselect/pent-light-on" );
	mPent->SetAcceptSound( "menu/mapselect/pent-accept" );
	mPent->SetDescPos( 80, 680 );
#ifdef TRANSGAMING
    mPent->SetColors( 0, 0, 0, 0xff1700d6 );
#else
    mPent->SetColors( 0, 0, 0, 0xffd60017 );
#endif

	if( mNumMaps[mCurrChapter] < 1 || mNumMaps[mCurrChapter] <= mCurrLevel )
	{
		ForceCurrLevel( START_LEVEL, START_CHAPTER );
	}

	mCrystal = CreateButton( "Crystal" );
	mCrystal->SetDesc( txt["Menu"]["PlayLevel"].GetString() );
	mCrystal->SetImage( "HUD/Map/krysztal_zgaszony", "HUD/Map/krysztal_swiecacy", "HUD/Map/krysztal_wcisniety" );
	mCrystal->SetGlowAnim( "HUD/Map/krysztal_swiecacy" );
	mCrystal->SetPosition( 198, 201 );
#ifdef TRANSGAMING
    char buf[MAX_PATH];

    mCrystal->SetAction( GetActionString(buf, MAX_PATH) );
#else
	mCrystal->SetAction( String( "PMENU.SwitchToMap(false); Lev._Name = '" ) + mMaps[mCurrChapter][mCurrLevel]->dir + String( "'; EDITOR.PostMessage(1,0)" ) );
#endif
	mCrystal->SetLightOnSound( "menu/mapselect/sign_pact-light-on" );
	mCrystal->SetAcceptSound( "menu/mapselect/option-sign_pact" );
	mCrystal->SetDescPos( 80, 680 );
#ifdef TRANSGAMING
    mCrystal->SetColors( 0, 0, 0, 0xff1700d6 );
#else
    mCrystal->SetColors( 0, 0, 0, 0xffd60017 );
#endif

	if( !mAddOn )
	{
		// Chapters
		for( int i=0; i<NUM_CHAPTERS; i++ )
		{
#ifdef TRANSGAMING
            const int   BUFSIZE = 64;
			char        tex[3][BUFSIZE];


			snprintf(tex[0], BUFSIZE, "Chapter%d", i + 1);
			MenuItemImageButton *item = CreateButton(tex[0]);

			snprintf(tex[0], BUFSIZE, "HUD/Map/klawisz%d_czysty", i + 1);
			snprintf(tex[1], BUFSIZE, "HUD/Map/klawisz%d_czysty_czerwony", i + 1);
			snprintf(tex[2], BUFSIZE, "HUD/Map/klawisz%d_czysty_czerwony", i + 1);
			item->SetImage( tex[0], tex[1], tex[2] );
			item->SetGlowAnim( tex[2] );
			item->SetLightOnSound( "menu/mapselect/option-light-on" );
			item->SetAcceptSound( "menu/mapselect/chapter-accept" );
			item->SetDesc( gSelectChapterStr.AsChar() );
			item->SetColors( 0, 0, 0, 0xff1700d6 );
#else
			char name[50];
			sprintf( name, "Chapter%d", i + 1 );
			MenuItemImageButton *item = CreateButton( name );

			char tex[3][255];
			sprintf( tex[0], "HUD/Map/klawisz%d_czysty", i + 1 );
			sprintf( tex[1], "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
			sprintf( tex[2], "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
			item->SetImage( tex[0], tex[1], tex[2] );
			item->SetGlowAnim( tex[2] );
			item->SetLightOnSound( "menu/mapselect/option-light-on" );
			item->SetAcceptSound( "menu/mapselect/chapter-accept" );
			item->SetDesc( gSelectChapterStr );
			item->SetColors( 0, 0, 0, 0xffd60017 );
#endif

			item->SetDescPos( 80, 680 );

			mChapters.Add( item );
		}

		mChapters[0]->SetPosition( 176, 118 );
		mChapters[1]->SetPosition( 320, 170 );
		mChapters[2]->SetPosition( 267, 310 );
		mChapters[3]->SetPosition( 129, 300 );
		mChapters[4]->SetPosition( 125, 165 );

		// Chapters selections
		for( int i=0; i<NUM_CHAPTERS; i++ )
		{
#ifdef TRANSGAMING
            const int   BUFSIZE = 64;
			char        tex[BUFSIZE];


			snprintf(tex, BUFSIZE, "Chapter%dSel", i + 1);
			MenuItemImageButton *item = CreateButton(tex);

			snprintf(tex, BUFSIZE, "HUD/Map/klawisz%d_wcisniety", i + 1);
#else
			char name[50];
			sprintf( name, "Chapter%dSel", i + 1 );
			MenuItemImageButton *item = CreateButton( name );

			char tex[255];
			sprintf( tex, "HUD/Map/klawisz%d_wcisniety", i + 1 );
#endif

			item->SetImage( tex, tex, tex );

			mChaptersSel.Add( item );
		}

		mChaptersSel[0]->SetPosition( 176, 118 );
		mChaptersSel[1]->SetPosition( 320, 170 );
		mChaptersSel[2]->SetPosition( 267, 310 );
		mChaptersSel[3]->SetPosition( 129, 300 );
		mChaptersSel[4]->SetPosition( 125, 165 );
	}

	if( !mAddOn )
		mWindow = GEngine->Video->MaterialSys->CreateTexture( "HUD/Map/okienko", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	else
		mWindow = GEngine->Video->MaterialSys->CreateTexture( "HUD/Map/okienko_addon", Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	UpdateButtons();
}

void MapSelect::SetSketch(const char *filename){
    if (mCurrSketchName != filename || mCurrSketch == NULL){
        if (mCurrSketch)
            GEngine->Video->MaterialSys->ReleaseTexture(mCurrSketch);

        mCurrSketch = GEngine->Video->MaterialSys->CreateTexture( filename, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
        mCurrSketchName = filename;
    }
}



MenuItemImageButton *MapSelect::CreateButton(const char *name)
{
    m_setTag("Game");
	MenuItemImageButton *item = new MenuItemImageButton;
	item->SetName( name );
	item->SetText( "" );
	item->SetDesc( "" );
	item->SetAlign( EAlignNone );
	item->SetAction( "" );
	item->SetBigFont( "timesbd", 14 );
	item->SetSmallFont( "timesbd", 26 );
#ifdef TRANSGAMING
    item->SetColors( 0xff7abaff, 0xff7abaff, 0xff7abaff, 0xff7abaff );
#else
    item->SetColors( 0xffffba7a, 0xffffba7a, 0xffffba7a, 0xffffba7a );
#endif

	mButtons.Add( item );

	return item;
}


void MapSelect::DrawButton( MenuItemImageButton *button )
{
	button->SetTransparency( 0xFF );
	button->Render();
	button->RenderDesc();
}


void MapSelect::DrawWindow()
{
	if( !mWindow )
		return;

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

	POINT pt;
	pt.x = (int)(267 * scaleX);
	pt.y = (int)(268 * scaleY);

	static float startTime = 0.0f;
	static float lastTime = 0.0f;
	static float rotDir = 1.0f;

	if( mWindowRot != mTargetRot )
	{
		if( mWindowRot == mStartRot )
		{
			if( mStartRot > mTargetRot )
				rotDir = -1.0f;
			else
				rotDir = 1.0f;
			startTime = GEngine->SysDriver.GetCurrentTime();
			lastTime = startTime;
			mStartRot += 0.0001f * rotDir;
		}
		else
		{
			lastTime = GEngine->SysDriver.GetCurrentTime();
			mWindowRot = mStartRot + ( lastTime - startTime ) * ( lastTime - startTime ) * 6.0f * rotDir;
			if( ( mWindowRot > mTargetRot && rotDir == 1.0f ) || ( mWindowRot < mTargetRot && rotDir == -1.0f ) )
			{
				mWindowRot = mTargetRot;
			}
		}
	}
	else if( mTargetLevel != mCurrLevel )
	{
		mCurrLevel = mTargetLevel;
        //TRACE("MapSelect::DrawWindow(): [!=] map status for level <%d, %d> ('%s', '%s') is %d\n", mCurrChapter, mCurrLevel, mMaps[mCurrChapter][mCurrLevel]->dir.AsChar(), mMaps[mCurrChapter][mCurrLevel]->name.AsChar(), mMaps[mCurrChapter][mCurrLevel]->status);
        if( mMaps[mCurrChapter][mCurrLevel]->status == LevelDesc::STATUS_CURRENT || mMaps[mCurrChapter][mCurrLevel]->status == LevelDesc::STATUS_FINISHED )
		{
#ifdef TRANSGAMING
            char buf[MAX_PATH];

            mCrystal->SetAction( GetActionString(buf, MAX_PATH) );
#else
			mCrystal->SetAction( String( "PMENU.SwitchToMap(false); Lev._Name = '" ) + mMaps[mCurrChapter][mCurrLevel]->dir + String( "'; EDITOR.PostMessage(1,0)" ) );
#endif
			mCrystal->SetAcceptSound( "menu/mapselect/option-sign_pact" );
		}
		else
		{
            //TRACE("MapSelect::DrawWindow(): [!=] clearing level button action\n");
			mCrystal->SetAction( "" );
			mCrystal->SetAcceptSound( "misc/card-cannot_use" );
		}


        SetSketch(mMaps[mTargetChapter][mCurrLevel]->sketchName.AsChar());
	}

	if( mCurrLevel == mTargetLevel )
	{
        if (mCurrChapter != mTargetChapter)
            SetSketch(mMaps[mTargetChapter][mCurrLevel]->sketchName.AsChar());

		mCurrChapter = mTargetChapter;
        //TRACE("MapSelect::DrawWindow(): [==] map status for level <%d, %d> ('%s', '%s') is %d\n", mCurrChapter, mCurrLevel, mMaps[mCurrChapter][mCurrLevel]->dir.AsChar(), mMaps[mCurrChapter][mCurrLevel]->name.AsChar(), mMaps[mCurrChapter][mCurrLevel]->status);


		if( mMaps[mCurrChapter][mCurrLevel]->status == LevelDesc::STATUS_CURRENT || mMaps[mCurrChapter][mCurrLevel]->status == LevelDesc::STATUS_FINISHED )
		{
#ifdef TRANSGAMING
            char buf[MAX_PATH];

            mCrystal->SetAction( GetActionString(buf, MAX_PATH) );
#else
			mCrystal->SetAction( String( "PMENU.SwitchToMap(false); Lev._Name = '" ) + mMaps[mCurrChapter][mCurrLevel]->dir + String( "'; EDITOR.PostMessage(1,0)" ) );
#endif
            mCrystal->SetAcceptSound( "menu/mapselect/option-sign_pact" );
		}
		else
		{
            //TRACE("MapSelect::DrawWindow(): [==] clearing level button action\n");
			mCrystal->SetAction( "" );
			mCrystal->SetAcceptSound( "misc/card-cannot_use" );
		}
	}

	if( !mAddOn )
		GEngine->Video->Hud->DrawQuadRotated( mWindow, ( float )262 * scaleX, ( float )119 * scaleY, ( float )mWindow->OriginalWidth * scaleX, ( float )mWindow->OriginalHeight * scaleY, -mWindowRot+0.00001f, pt );
	else
		GEngine->Video->Hud->DrawQuadRotated( mWindow, ( float )262 * scaleX, ( float )104 * scaleY, ( float )mWindow->OriginalWidth * scaleX, ( float )mWindow->OriginalHeight * scaleY, -mWindowRot+0.00001f, pt );
}


void MapSelect::DrawLevelDesc()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	int x = (int)(100 * screenWidth / 1024.0f);
	int y = (int)(540 * screenHeight / 768.0f);

	HUD *hud = GEngine->Video->Hud;

	hud->DrawBorder( 80, 520, 320, 120 );

	if( mMaps[mCurrChapter][mCurrLevel]->status != LevelDesc::STATUS_WRONGDIFF )
	{
		hud->SetFont( "timesbd", 30 );
		int fontHeight = GEngine->Video->Hud->GetTextHeight();

		String chapter;
		if( !mAddOn )
			chapter = String::Sprintf( "%s %d", chapterStr.AsChar(), mCurrChapter + 1 );
		else
			chapter = chapterStr;
		String level = String::Sprintf( "%s %d", levelStr.AsChar(), mCurrLevel + 1 );

#ifdef TRANSGAMING
        hud->Print( x, y, chapter.AsChar(), 0xff61a1e6 );
		hud->Print( x, y + fontHeight, level.AsChar(), 0xff61a1e6 );
		hud->Print( x, y + fontHeight * 2, mMaps[mCurrChapter][mCurrLevel]->name.AsChar(), 0xff0000bd );
#else
        hud->Print( x, y, chapter.AsChar(), 0xffe6a161 );
		hud->Print( x, y + fontHeight, level.AsChar(), 0xffe6a161 );
		hud->Print( x, y + fontHeight * 2, mMaps[mCurrChapter][mCurrLevel]->name.AsChar(), 0xffbd0000 );
#endif
	}
	else
	{
		hud->SetFont( "timesbd", 30 );
#ifdef TRANSGAMING
        hud->Print( x, y, gLevelNotAvailableStr.AsChar(), 0xff61a1e6 );
#else
        hud->Print( x, y, gLevelNotAvailableStr.AsChar(), 0xffe6a161 );
#endif
	}

//	GEngine->Video->Hud->SetFont( "timesbd", "HUD/font_texturka_alpha", 28 );

//	int fontHeight = GEngine->Video->Hud->GetTextHeight();

//	GEngine->Video->Hud->Print( x, y + fontHeight * 2, "Cards found: 0/3", 0xFFFFFFFF );
//	GEngine->Video->Hud->Print( x, y + fontHeight * 3, "Gold found: -", 0xFFFFFFFF );
}


void MapSelect::Render()
{
    if (mDisabled)
        return;

	View *view = GEngine->Video->GetActiveView();

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

#ifndef TRANSGAMING
	GEngine->Video->Hud->DrawQuad( NULL, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height, 0xFF000000 );
#endif

#ifdef TRANSGAMING
	if (mCurrSketch){
		if (!mLoadingScreen)
			GEngine->Video->Hud->DrawQuad( mCurrSketch, 370 * scaleX, 140 * scaleY, mCurrSketch->OriginalWidth * scaleX, mCurrSketch->OriginalHeight * scaleY );

		else{
			float diff = ( GEngine->SysDriver.GetCurrentTime() - mFadeStart ) / 1.0f;
			BYTE alpha = ( BYTE )( diff * 255.0f );
			if (diff > 1.0f)
				alpha = 255;

			DWORD color = ( ( 255 - alpha ) << 8 ) | (( 255 - alpha ) << 16) | 0xff0000ff;
			GEngine->Video->Hud->DrawQuad( mCurrSketch, 370 * scaleX, 140 * scaleY, mCurrSketch->OriginalWidth * scaleX, mCurrSketch->OriginalHeight * scaleY, color );
		}
	}
#else
	if( mMaps[mCurrChapter][mCurrLevel]->sketch )
	{
		if( !mLoadingScreen )
		{
			GEngine->Video->Hud->DrawQuad( mMaps[mCurrChapter][mCurrLevel]->sketch, 370 * scaleX, 140 * scaleY, mMaps[mCurrChapter][mCurrLevel]->sketch->OriginalWidth * scaleX, mMaps[mCurrChapter][mCurrLevel]->sketch->OriginalHeight * scaleY );
		}
		else
		{
			float diff = ( GEngine->SysDriver.GetCurrentTime() - mFadeStart ) / 1.0f;
			BYTE alpha = ( BYTE )( diff * 255.0f );
			if( diff > 1.0f )
				alpha = 255;
			DWORD color = ( ( 255 - alpha ) << 8 ) | ( 255 - alpha ) | 0xFFFF0000;
			GEngine->Video->Hud->DrawQuad( mMaps[mCurrChapter][mCurrLevel]->sketch, 370 * scaleX, 140 * scaleY, mMaps[mCurrChapter][mCurrLevel]->sketch->OriginalWidth * scaleX, mMaps[mCurrChapter][mCurrLevel]->sketch->OriginalHeight * scaleY, color );
		}
	}
#endif

	if( mBackground )
	{
		if( !mLoadingScreen )
		{
			GEngine->Video->Hud->DrawQuad( mBackground, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height );
		}
		else
		{
			float diff = ( GEngine->SysDriver.GetCurrentTime() - mFadeStart ) / 1.0f;
			BYTE alpha = ( BYTE )( diff * 255.0f );
			if( diff > 1.0f )
				alpha = 255;
//			DWORD color = ( ( 255 - alpha ) << 16 ) | ( ( 255 - alpha ) << 8 ) | ( 255 - alpha ) | 0xFF000000;
			DWORD color = ( ( 255 - alpha ) << 24 ) | 0x00FFFFFF;
			GEngine->Video->Hud->DrawQuad( mBackground, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height, color );
		}
	}

	DrawLevelDesc();

	for( int i=0; i<mButtons.Size(); i++ )
	{
		mButtons[i]->UpdateScale();
		mButtons[i]->CalcPosition();
		mButtons[i]->CalcSize();
		mButtons[i]->SetUnderMouse( false );
	}

	for( int i=0; i<mButtons.Size(); i++ )
	{
		mButtons[i]->CheckIfUnderMouse();
		if( mButtons[i]->CheckIfUnderMouse() )
			break;
	}

	DrawButton( mCard );
	DrawButton( mPent );
	DrawButton( mCrystal );

	for( int i=0; i<mChapters.Size(); i++ )
	{
		if( mCurrChapter != i )
			DrawButton( mChapters[i] );
		else
			DrawButton( mChaptersSel[i] );
	}

	for( int i=0; i<mNumMaps[mCurrChapter]; i++ )
	{
		if( mMaps[mCurrChapter][i]->status == LevelDesc::STATUS_CURRENT || mMaps[mCurrChapter][i]->status == LevelDesc::STATUS_FINISHED )
		{
#ifdef TRANSGAMING
			if( !mMaps[mCurrChapter][i]->name.IsNull() )
#else
			if( mMaps[mCurrChapter][i]->name != "" )
#endif
			{
				DrawButton( mLevels[i] );
			}
		}
		else
		{
			DrawButton( mQuestions[i] );
		}
	}

//	if( !mLoadingScreen )
		DrawWindow();

	if( mLoadingScreen )
	{
		float diff = ( GEngine->SysDriver.GetCurrentTime() - mFadeStart ) / 1.0f;
		BYTE alpha = ( BYTE )( diff * 255.0f );
		if( diff > 1.0f )
		{
			alpha = 255;
#ifdef TRANSGAMING
			if( mCrystal->GetAction() != NULL )
#else
			if( mCrystal->GetAction() != "" )
#endif
			{
#ifdef TRANSGAMING
				GLog.Print( "%s\n", mPent->GetAction() );
#else
				GLog.Print( "%s\n", mPent->GetAction().AsChar() );
#endif
				GEngine->InputSys->SetLuaToDo( mCrystal->GetAction() );
				mLastLevel = mCurrLevel;
				mLastCondition = mMaps[mCurrChapter][mCurrLevel]->cardCondition;
				GEngine->Video->PMenu->LoadScreen.Activate( true, mCurrLevel, mMaps[mCurrChapter][mCurrLevel]->sketchName.AsChar() );
#ifdef TRANSGAMING
                //clear out the fonts to regain us some memory at play time
                //GEngine->Video->Hud->removeAllFonts();
#endif
			}
		}

		DWORD color = ( (alpha | 0x00000000) << 24 ) | 0x00FFFFFF;
		GEngine->Video->Hud->DrawQuad( mLoadingScreen, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height, color );
	}

	POINT mousePos = GEngine->InputSys->GetMousePosition();

#ifdef TRANSGAMING
	if( mShowStats || (mousePos.x > 80 * scaleX && mousePos.x < 400 * scaleX &&
		               mousePos.y > 520 * scaleY && mousePos.y < 640 * scaleY) ){
		GScript.Call( "Hud_RenderLevelStats", 0, "%d, %d", mCurrChapter, mCurrLevel );
	}
#else
	if( (mousePos.x > 80 * scaleX && mousePos.x < 400 * scaleX &&
		 mousePos.y > 520 * scaleY && mousePos.y < 640 * scaleY) )
	{
		GScript.Call( "Hud_RenderLevelStats", 0, "" );
	}
#endif

#ifndef TRANSGAMING
	if( mMouseTexture && !( mMouseTexture->Flags & Texture::ETexInvalid ) && mShowMouse )
	{
		GEngine->Video->Hud->DrawQuad( mMouseTexture, (float)(mousePos.x-mMouseClickOffsetX),
			(float)(mousePos.y-mMouseClickOffsetY), (float)mMouseTexture->Width, (float)mMouseTexture->Height );
	}
#endif
}


void MapSelect::Clear()
{
    TRACE("MapSelect::Clear(): clearing the level table\n");
	for( int i=0; i<mButtons.Size(); i++ )
	{
		if( mButtons[i] )
			delete mButtons[i];
		mButtons[i] = NULL;
	}

	mButtons.ClearArray();
	mChapters.ClearArray();
	mChaptersSel.ClearArray();
	mLevels.ClearArray();
	mLevelsSel.ClearArray();
	mQuestions.ClearArray();

	mCard = NULL;
	mPent = NULL;
	mCrystal = NULL;

#ifdef TRANSGAMING
    mShowStats = false;
#endif

	if( mBackground && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mBackground );
	mBackground = NULL;

    if (mCurrSketch && GEngine->Video->MaterialSys)
        GEngine->Video->MaterialSys->ReleaseTexture(mCurrSketch);
    mCurrSketch = NULL;
    mCurrSketchName = "";

	if( mLoadingScreen && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mLoadingScreen );
	mLoadingScreen = NULL;

	if( mMouseTexture && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );
	mMouseTexture = NULL;

	for( int c=0; c<NUM_CHAPTERS; c++ )
	{
		/*for( int i=0; i<mNumMaps[c]; i++ )
		{
			if( mMaps[c][i]->sketch && GEngine->Video->MaterialSys )
				GEngine->Video->MaterialSys->ReleaseTexture( mMaps[c][i]->sketch );
			mMaps[c][i]->sketch = NULL;
		}//*/

        for( int i=0; i<mNumMaps[c]; i++ ){
			delete mMaps[c][i];

            mMaps[c][i] = NULL;
        }

		mNumMaps[c] = 0;
	}
}


void MapSelect::Activate( bool active )
{
    mActive = active;

    static BOOL mouseLocked = GEngine->InputSys->IsMouseLocked();
    static BOOL mouseVisible = GEngine->InputSys->IsMouseVisible();

    ScriptObject game = GScript.Globals()["Game"];
    mAddOn = game["AddOn"].GetBool();

    bool blackEd = GScript.Globals()["Cfg"]["BlackEdition"].GetBool();

    if( active )
    {
#ifndef TRANSGAMING
        if( !mMouseTexture )
            SetMouseTexture( 0, 0, "HUD/kursor" );
        // BLAIR - We're disabling this mouse...permanently!
        ShowMouse( true );
#endif

        if (!mDisabled){
            if( mAddOn )
            {
                if( !blackEd )
                    SetBackground( "HUD/Map/MapAddOn" );
                else
                    SetBackground( "HUD/Map/MapAddOn_black" );
            }
            else
            {
                if( !blackEd )
                    SetBackground( "HUD/Map/Map" );
                else
                    SetBackground( "HUD/Map/Map_black" );
            }

            mouseLocked = GEngine->InputSys->IsMouseLocked();
            if( mouseLocked )
                GEngine->InputSys->LockMouse( FALSE );
            mouseVisible = GEngine->InputSys->IsMouseVisible();
            if( mouseVisible )
                GEngine->InputSys->ShowMouse( FALSE );

    //		mPausedSounds = GEngine->Audio->PauseCurrentlyPlayingSounds();

#ifndef TRANSGAMING_NO_MILES
            if( GEngine->Audio )
            {
                GEngine->Audio->SetRoom(0);
                GEngine->Audio->SetRoomIntensity2D(0,0);
                GEngine->Audio->SetRoomIntensity3D(0,0);
            }
#endif
            Setup();
        }
    }
    else
    {
        mDisabled = false;
#ifndef TRANSGAMING
        ShowMouse( false );
#endif
        Clear();

        if( mouseLocked )
            GEngine->InputSys->CenterMouse();
        if( mouseLocked != GEngine->InputSys->IsMouseLocked() )
        {
            GEngine->InputSys->LockMouse( mouseLocked );
            mouseLocked = GEngine->InputSys->IsMouseLocked();
        }
        if( mouseVisible != GEngine->InputSys->IsMouseVisible() )
        {
            GEngine->InputSys->ShowMouse( mouseVisible );
            mouseVisible = GEngine->InputSys->IsMouseVisible();
        }
//		GEngine->Video->GetActiveView()->Mode &= ~View::EModeDoNotRenderWorld;
        GEngine->Game->mPaused = false;
        GScript.DoString( "Game.Paused = false" );

        if( mPausedSounds != -1 )
        {
//			GEngine->Audio->ResumeSounds( mPausedSounds );
            mPausedSounds = -1;
        }
    }
}


#ifndef TRANSGAMING
void MapSelect::SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags )
{
	if( mMouseTexture )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );

	mMouseTexture = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mMouseClickOffsetX = xClickOffset;
	mMouseClickOffsetY = yClickOffset;

	if( mMouseTexture == GEngine->Video->MaterialSys->Textures.NoTex )
		mMouseTexture = 0;
}
#endif


void MapSelect::SetBackground( const char *texName, DWORD texFlags )
{
	if( mBackground )
		GEngine->Video->MaterialSys->ReleaseTexture( mBackground );

	mBackground = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	if( mBackground == GEngine->Video->MaterialSys->Textures.NoTex )
		mBackground = NULL;
}


bool MapSelect::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( !Active() )
		return false;

    if (mDisabled)
        return true;

    if ( mLoadingScreen )
		return false;

	if ( !CheckButtons() )
		return false;

	static bool esc_pressed = false;

    //check if the user is trying to leave the screen
#ifdef TRANSGAMING
	if (eventType == EMenuKeyDown && eventData1 == EMouseKeyRight)
#else
	if( eventType == EMenuKeyDown && eventData1 == VK_ESCAPE )
#endif
	{
		esc_pressed = true;
		return true;
	}
#ifdef TRANSGAMING
	else if (eventType == EMenuKeyUp && eventData1 == EMouseKeyRight && esc_pressed){
		if (mPent->GetAction() != NULL)
			GEngine->InputSys->SetLuaToDo( mPent->GetAction() );

        esc_pressed = false;
		return true;
	}


    //check if the level stats should be shown.  If they are to be shown, make sure
    //no other buttons get a chance to be processed.
    if (eventType == EMenuKeyDown && eventData1 == BUTTON_SELECT){
        mShowStats = true;
        return true;
    }

    else if (mShowStats && eventType == EMenuKeyUp && eventData1 == BUTTON_SELECT){
        mShowStats = false;
        return true;
    }


    //make sure all other buttons (except 'back') are disabled while the stats are showing
    if (mShowStats)
        return true;
#else
	else if( eventType == EMenuKeyUp && eventData1 == VK_ESCAPE && esc_pressed )
	{
		if( !mPent->GetAction().IsNull() )
		{
			GEngine->InputSys->SetLuaToDo( mPent->GetAction() );
		}
		esc_pressed = false;
		return true;
	}
#endif


    //check if the user clicked on the card button
#ifdef TRANSGAMING
    if (eventType == EMenuKeyUp && eventData1 == EMouseKeyMiddle || mCard->SendEvent( eventType, eventData1, eventData2 )){
        if (mCard->GetAction() != NULL)
            GEngine->InputSys->SetLuaToDo( mCard->GetAction() );

        return true;
	}
#else
	if( mCard->SendEvent( eventType, eventData1, eventData2 ) )
	{
		if( !mCard->GetAction().IsNull() )
		{
			GEngine->InputSys->SetLuaToDo( mCard->GetAction() );
		}
		return true;
	}
#endif

	if( !CheckButtons() )
		return false;

    
    //check if the user clicked on the 'back' button (pentagram)
#ifdef TRANSGAMING
	/*if( mPent->SendEvent( eventType, eventData1, eventData2 ) ){
		if( mPent->GetAction() != NULL )
			GEngine->InputSys->SetLuaToDo( mPent->GetAction() );

		return true;
	}//*/
#else
	if( mPent->SendEvent( eventType, eventData1, eventData2 ) ){
		if( !mPent->GetAction().IsNull() )
			GEngine->InputSys->SetLuaToDo( mPent->GetAction() );

        return true;
	}
#endif

	if( !CheckButtons() )
		return false;


    //check if the user wants to start the level
#ifdef TRANSGAMING
    if ((eventType == EMenuKeyUp && (eventData1 == BUTTON_START || eventData1 == EMouseKeyLeft) && mCrystal->GetAction() != NULL) || mCrystal->SendEvent( eventType, eventData1, eventData2 )) // || eventData1 == EMouseKeyLeft))
#else
    if( mCrystal->SendEvent( eventType, eventData1, eventData2 ) )
#endif
	{
		if( !mLoadingScreen )
			mLoadingScreen = GEngine->Video->MaterialSys->CreateTexture( "HUD/loading/loading", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
		mFadeStart = GEngine->SysDriver.GetCurrentTime();
#ifndef TRANSGAMING
        mShowMouse = false;
#endif

		return true;
	}


    //check if the user clicked on any of the chapter buttons
#ifdef TRANSGAMING
    bool checkLevel = false;


    //check for chapter change requests
    if (eventType == EMenuKeyDown){
        
        //make sure the button list has finished being updated
		if (!CheckButtons())
			return false;

        //user wants to go to the previous chapter
        if (eventData1 == BUTTON_L1 || eventData1 == BUTTON_UP){
            checkLevel = true;

            /*if (mTargetChapter > 0)
                mTargetChapter--;//*/
            mTargetChapter = (NUM_CHAPTERS + mTargetChapter - 1) % NUM_CHAPTERS;
        }

        //user wants to go to the next chapter
        else if (eventData1 == BUTTON_R1 || eventData1 == BUTTON_DOWN){
            checkLevel = true;

            /*if (mTargetChapter < NUM_CHAPTERS - 1)
                mTargetChapter++;//*/
            mTargetChapter = (mTargetChapter + 1) % NUM_CHAPTERS;
        }


        if (checkLevel){
			if( mTargetLevel != 0 ){
                const int   BUFSIZE = 64;
				int         dist = mTargetLevel;
				char        sound[BUFSIZE];

                
                if ( dist < 0 ) dist = -dist;
				if ( dist > 5 ) dist = 5;

				snprintf(sound, BUFSIZE, "menu/mapselect/slide%d", dist);
				GEngine->Video->PMenu->PlayMenuSound( sound );
				mTargetLevel = 0;
				mTargetRot = 0.0f;
				mStartRot = mWindowRot;
			}
			UpdateButtons();

            return true;
        }
    }
#else
	for( int i=0; i<mChapters.Size(); i++ )
	{
		if( !CheckButtons() )
			return false;

		bool ret = mChapters[i]->SendEvent( eventType, eventData1, eventData2 );

        if( mChapters[i]->IsPressed() && mNumMaps[i] > 0 )
		{
# if !DEMO_ONLY
			mTargetChapter = i;
			if( mTargetLevel != 0 )
			{
                const int BUFSIZE = 64;
				int dist = mTargetLevel;
				char sound[BUFSIZE];

                
                if( dist < 0 ) dist = -dist;
				if( dist > 5 ) dist = 5;

				snprintf(sound, BUFSIZE, "menu/mapselect/slide%d", dist);
				GEngine->Video->PMenu->PlayMenuSound( sound );
				mTargetLevel = 0;
				mTargetRot = 0.0f;
				mStartRot = mWindowRot;
			}
			UpdateButtons();
# endif // DEMO_ONLY
		}
		if( ret )
			return true;
	}
#endif



#ifdef TRANSGAMING
    checkLevel = false;
    int currLevel = mTargetLevel;


    //check for level change requests
    if (eventType == EMenuKeyDown){
        if ( !CheckButtons() )
			return false;

        
        //user wants to go to the previous level
        if (eventData1 == BUTTON_LEFT){
            checkLevel = true;
            
            /*if (mTargetLevel > 0)
                mTargetLevel--;//*/
            mTargetLevel = (mNumMaps[mCurrChapter] + mTargetLevel - 1) % mNumMaps[mCurrChapter];
        }

        //user wants to go to the next level
        else if (eventData1 == BUTTON_RIGHT){
            checkLevel = true;

            /*if (mTargetLevel < mNumMaps[mCurrChapter] - 1)
                mTargetLevel++;//*/
            mTargetLevel = (mTargetLevel + 1) % mNumMaps[mCurrChapter];
        }


        //setup the level change
        if (checkLevel){
            const int   BUFSIZE = 64;
			int         dist = currLevel - mTargetLevel;
			char        sound[BUFSIZE];

            
            if( dist < 0 ) dist = -dist;
			if( dist > 5 ) dist = 5;



			snprintf(sound, BUFSIZE, "menu/mapselect/slide%d", dist);
			
            GEngine->Video->PMenu->PlayMenuSound( sound );
			if(!mAddOn)
				mTargetRot = 2.0f * 3.14159265358979323846f / 6.0f * ( float )mTargetLevel;
			else
				mTargetRot = 2.0f * 3.14159265358979323846f / 10.0f * ( float )mTargetLevel;
			mStartRot = mWindowRot;
			UpdateButtons();
            return true;
        }
    }
#else
	for( int i=0; i<mNumMaps[mCurrChapter]; i++ )
	{
		if( !CheckButtons() )
			return false;

		bool ret = mLevels[i]->SendEvent( eventType, eventData1, eventData2 );
		if( mLevels[i]->IsPressed() && mTargetLevel != i && !mMaps[mCurrChapter][i]->name.IsNull() )
		{
			int dist = mTargetLevel - i;
			if( dist < 0 ) dist = -dist;
			if( dist > 5 ) dist = 5;

			static char sound[1024];
			sprintf( sound, "menu/mapselect/slide%d", dist );

            GEngine->Video->PMenu->PlayMenuSound( sound );
			mTargetLevel = i;
			if( !mAddOn )
				mTargetRot = 2.0f * 3.14159265358979323846f / 6.0f * ( float )i;
			else
				mTargetRot = 2.0f * 3.14159265358979323846f / 10.0f * ( float )i;
			mStartRot = mWindowRot;
			UpdateButtons();
		}
		if( ret )
			return true;
	}
#endif

	return false;
}


bool MapSelect::CheckButtons()
{
	if( mButtons.Size() <= 1 )
		return false;

	for( int i=0; i<mButtons.Size(); i++ )
	{
		if( !mButtons[i] )
			return false;
	}

	return true;
}


void MapSelect::AddMap( int chapter, const LevelDesc &desc )
{
    if (mDisabled)
        return;

    m_setTag("Game");
	LevelDesc *n = new LevelDesc();
	//n->dir = desc.dir;
	n->name = desc.name;
	n->sketchName = desc.sketchName;
	n->cardCondition = desc.cardCondition;
	//n->sketch = GEngine->Video->MaterialSys->CreateTexture( n->sketchName.AsChar(), Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	n->cardIndex = desc.cardIndex;
	n->status = desc.status;

	mMaps[chapter-1][mNumMaps[chapter-1]++] = n;

    if (desc.status == LevelDesc::STATUS_CURRENT && mCurrSketch == NULL)
        SetSketch(n->sketchName.AsChar());
}

void MapSelect::UpdateMapStatus(int chapter, int level, const LevelDesc::LevelStatus status){
    if (mMaps[chapter - 1][level - 1])
        mMaps[chapter - 1][level - 1]->status = status;

    else
        TRACE("MapSelect::UpdateStatus(): ERROR-> chapter %d, level %d does not exist!\n", chapter, level);
}

void MapSelect::SetMapSketch(int chapter, int level, const char *sketch){
    if (mMaps[chapter - 1][level - 1])
        mMaps[chapter - 1][level - 1]->sketchName = sketch;

    else
        TRACE("MapSelect::SetMapSketch(): ERROR-> chapter %d, level %d does not exist!\n", chapter, level);
}



void MapSelect::SelectNextLevel()
{
    if (mDisabled)
        return;

#if !DEMO_ONLY
	if( mLastLevel + 1 >= mNumMaps[mCurrChapter] )
	{
		mTargetLevel = START_LEVEL;
		if( mTargetChapter == 4 )
			mTargetChapter = START_CHAPTER; // demo
		else
			mTargetChapter++;
	}
	else
	{
		if( mTargetLevel + 1 >= mNumMaps[mCurrChapter] )
			mTargetLevel = START_LEVEL; //demo
		else
			mTargetLevel++;
	}

	if( mNumMaps[mTargetChapter] == 0 )
	{
		mTargetChapter = START_CHAPTER; //demo
		mTargetLevel = START_LEVEL; // demo
	}

	if( mMaps[mTargetChapter][mTargetLevel]->status == LevelDesc::STATUS_WRONGDIFF )
	{
		mLastLevel = mTargetLevel;
		return SelectNextLevel();
	}

	if( mTargetLevel != mCurrLevel )
	{
		int dist = mTargetLevel - mCurrLevel;
		if( dist < 0 ) dist = -dist;
		if( dist > 5 ) dist = 5;

#ifdef TRANSGAMING
        const int BUFSIZE = 1024;
		char sound[BUFSIZE];


		snprintf(sound, BUFSIZE, "menu/mapselect/slide%d", dist);
#else
		static char sound[1024];
		sprintf( sound, "menu/mapselect/slide%d", dist );
#endif
		GEngine->Video->PMenu->PlayMenuSound( sound );
		if( !mAddOn )
			mTargetRot = 2.0f * 3.14159265358979323846f / 6.0f * ( float )mTargetLevel;
		else
			mTargetRot = 2.0f * 3.14159265358979323846f / 10.0f * ( float )mTargetLevel;
		mStartRot = mWindowRot;
	}
#endif
	UpdateButtons();
}


void MapSelect::Reset()
{
	mCurrChapter = START_CHAPTER;
	mTargetChapter = START_CHAPTER;
	mCurrLevel = START_LEVEL;
	mLastLevel = START_LEVEL;

	mWindowRot = 2.0f * 3.14159265358979323846f / 6.0f * START_LEVEL;
	mFadeStart = 0.0f;
	mStartRot = 2.0f * 3.14159265358979323846f / 6.0f * START_LEVEL;
	mTargetRot = 2.0f * 3.14159265358979323846f / 6.0f * START_LEVEL;
	mTargetLevel = START_LEVEL;

	UpdateButtons();
}


void MapSelect::UpdateButtons()
{
    if (mDisabled)
        return;

#ifdef TRANSGAMING
    const int   BUFSIZE = 64;
    char        tex[3][BUFSIZE];
#endif
        
    if( !mAddOn )
	{

		int chMode = 0;
		for( int i=0; i<NUM_CHAPTERS; i++ )
		{
#ifndef TRANSGAMING
			char tex[3][255];
#endif
			if( mNumMaps[i] < 1 )
			{
				if( mChapters.Size() > i )
				{
#ifdef TRANSGAMING
					snprintf( tex[0], BUFSIZE, "HUD/Map/klawisz%d_czysty", i + 1 );
					snprintf( tex[1], BUFSIZE, "HUD/Map/klawisz%d_czysty", i + 1 );
					snprintf( tex[2], BUFSIZE, "HUD/Map/klawisz%d_czysty", i + 1 );
#else
					sprintf( tex[0], "HUD/Map/klawisz%d_czysty", i + 1 );
					sprintf( tex[1], "HUD/Map/klawisz%d_czysty", i + 1 );
					sprintf( tex[2], "HUD/Map/klawisz%d_czysty", i + 1 );
#endif
                    mChapters[i]->SetImage( tex[0], tex[1], tex[2] );
					mChapters[i]->SetGlowAnim( NULL );
					mChapters[i]->SetDesc( "" );
				}
				continue;
			}
			bool finished = ( mMaps[i][mNumMaps[i]-1]->status == LevelDesc::STATUS_FINISHED ) ? true : false;

			if( !finished && chMode < 2 )
				chMode++;

	#if DEMO_ONLY
			if( i == 4 )
				chMode = 2;
			else
				chMode = 0;
	#endif

			switch( chMode )
			{
			case 0:
#ifdef TRANSGAMING
				snprintf( tex[0], BUFSIZE, "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
				snprintf( tex[1], BUFSIZE, "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
				snprintf( tex[2], BUFSIZE, "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
				mChapters[i]->SetImage( tex[0], tex[1], tex[2] );
				mChapters[i]->SetGlowAnim( NULL );
				snprintf( tex[0], BUFSIZE, "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
				snprintf( tex[1], BUFSIZE, "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
				snprintf( tex[2], BUFSIZE, "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
				mChaptersSel[i]->SetImage( tex[0], tex[1], tex[2] );
				mChaptersSel[i]->SetGlowAnim( NULL );
#else
				sprintf( tex[0], "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
				sprintf( tex[1], "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
				sprintf( tex[2], "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
				mChapters[i]->SetImage( tex[0], tex[1], tex[2] );
				mChapters[i]->SetGlowAnim( NULL );
				sprintf( tex[0], "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
				sprintf( tex[1], "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
				sprintf( tex[2], "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
				mChaptersSel[i]->SetImage( tex[0], tex[1], tex[2] );
				mChaptersSel[i]->SetGlowAnim( NULL );
#endif
				break;

			case 1:
				if( i != mCurrChapter )
				{
#ifdef TRANSGAMING
					snprintf( tex[0], BUFSIZE, "HUD/Map/klawisz%d_czysty", i + 1 );
					snprintf( tex[1], BUFSIZE, "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
					snprintf( tex[2], BUFSIZE, "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
#else
					sprintf( tex[0], "HUD/Map/klawisz%d_czysty", i + 1 );
					sprintf( tex[1], "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
					sprintf( tex[2], "HUD/Map/klawisz%d_czysty_czerwony", i + 1 );
#endif
					mChapters[i]->SetImage( tex[0], tex[1], tex[2] );
					mChapters[i]->SetGlowAnim( tex[2] );
				}
				else
				{
#ifdef TRANSGAMING
					snprintf( tex[0], BUFSIZE, "HUD/Map/klawisz%d_wcisniety", i + 1 );
					snprintf( tex[1], BUFSIZE, "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
					snprintf( tex[2], BUFSIZE, "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
#else
					sprintf( tex[0], "HUD/Map/klawisz%d_wcisniety", i + 1 );
					sprintf( tex[1], "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
					sprintf( tex[2], "HUD/Map/klawisz%d_wcisniety_czerwony", i + 1 );
#endif
					mChaptersSel[i]->SetImage( tex[0], tex[1], tex[2] );
					mChaptersSel[i]->SetGlowAnim( tex[2] );
				}
				break;

			case 2:
#ifdef TRANSGAMING
				snprintf( tex[0], BUFSIZE, "HUD/Map/klawisz%d_czysty", i + 1 );
				snprintf( tex[1], BUFSIZE, "HUD/Map/klawisz%d_swiec", i + 1 );
				snprintf( tex[2], BUFSIZE, "HUD/Map/klawisz%d_swiec", i + 1 );
				mChapters[i]->SetImage( tex[0], tex[1], tex[2] );
				mChapters[i]->SetGlowAnim( NULL );
				snprintf( tex[0], BUFSIZE, "HUD/Map/klawisz%d_wcisniety", i + 1 );
				snprintf( tex[1], BUFSIZE, "HUD/Map/klawisz%d_wcisniety", i + 1 );
				snprintf( tex[2], BUFSIZE, "HUD/Map/klawisz%d_wcisniety", i + 1 );
				mChaptersSel[i]->SetImage( tex[0], tex[1], tex[2] );
				mChaptersSel[i]->SetGlowAnim( NULL );
#else
				sprintf( tex[0], "HUD/Map/klawisz%d_czysty", i + 1 );
				sprintf( tex[1], "HUD/Map/klawisz%d_swiec", i + 1 );
				sprintf( tex[2], "HUD/Map/klawisz%d_swiec", i + 1 );
				mChapters[i]->SetImage( tex[0], tex[1], tex[2] );
				mChapters[i]->SetGlowAnim( NULL );
				sprintf( tex[0], "HUD/Map/klawisz%d_wcisniety", i + 1 );
				sprintf( tex[1], "HUD/Map/klawisz%d_wcisniety", i + 1 );
				sprintf( tex[2], "HUD/Map/klawisz%d_wcisniety", i + 1 );
				mChaptersSel[i]->SetImage( tex[0], tex[1], tex[2] );
				mChaptersSel[i]->SetGlowAnim( NULL );
#endif
				break;

			default:
				break;
			}
		}
	}

	for( int i=0; i<mNumMaps[mTargetChapter]; i++ )
	{
        LevelDesc::LevelStatus status = mMaps[mTargetChapter][i]->status;
#ifndef TRANSGAMING
		char tex[3][255];
#endif
		switch( status )
		{
		case LevelDesc::STATUS_UNAVAILABLE:
		case LevelDesc::STATUS_FINISHED:
		case LevelDesc::STATUS_WRONGDIFF:
#ifdef TRANSGAMING
			snprintf( tex[0], BUFSIZE, "HUD/Map/cyferka%d_normalna", i + 1 );
			snprintf( tex[1], BUFSIZE, "HUD/Map/cyferka%d_normalna", i + 1 );
			snprintf( tex[2], BUFSIZE, "HUD/Map/cyferka%d_normalna", i + 1 );
#else
			sprintf( tex[0], "HUD/Map/cyferka%d_normalna", i + 1 );
			sprintf( tex[1], "HUD/Map/cyferka%d_normalna", i + 1 );
			sprintf( tex[2], "HUD/Map/cyferka%d_normalna", i + 1 );
#endif

			mLevels[i]->SetImage( tex[0], tex[1], tex[2] );
			mLevels[i]->SetGlowAnim( NULL );
			break;

		case LevelDesc::STATUS_CURRENT:
#ifdef TRANSGAMING
			snprintf( tex[0], BUFSIZE, "HUD/Map/cyferka%d_czysta", i + 1 );
			snprintf( tex[1], BUFSIZE, "HUD/Map/cyferka%d_normalna", i + 1 );
			snprintf( tex[2], BUFSIZE, "HUD/Map/cyferka%d_normalna", i + 1 );
#else
			sprintf( tex[0], "HUD/Map/cyferka%d_czysta", i + 1 );
			sprintf( tex[1], "HUD/Map/cyferka%d_normalna", i + 1 );
			sprintf( tex[2], "HUD/Map/cyferka%d_normalna", i + 1 );
#endif

			mLevels[i]->SetImage( tex[0], tex[1], tex[2] );
			mLevels[i]->SetGlowAnim( tex[2] );
			break;
		}
	}
}


void MapSelect::ForceCurrLevel( int l, int c )
{
	if( l < 0 ) l = 0;
	if( c < 0 ) c = 0;
	mCurrLevel = mTargetLevel = mLastLevel = l;
	mCurrChapter = mTargetChapter = c;
	float cnt = 6.0f;
	if( mAddOn ) cnt = 10.0f;
	mWindowRot = 2.0f * 3.14159265358979323846f / cnt * l;
	mStartRot = 2.0f * 3.14159265358979323846f / cnt * l;
	mTargetRot = 2.0f * 3.14159265358979323846f / cnt * l;
}
