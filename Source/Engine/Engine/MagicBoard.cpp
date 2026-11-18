#include "Engine.h"
#include "Render/HUD.h"
#include "MagicBoard.h"

#ifndef TRANSGAMING_NO_MILES
// PSP support will need to be added later.
#include "Audio/MilesEngine.h"
#endif

#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Engine/BaseGame.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;


//--------------------------------------------------------------------
//	MagicCard class
//--------------------------------------------------------------------
MagicCard::MagicCard()
{
	mName = "";
	mTexture = NULL;
	mAvailable = false;
	mIndex = -1;
	mBigTex = "";
    mCost = 0;
    mType = ECardTypeNone;
}


MagicCard::~MagicCard()
{
	if( mTexture && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture );
	mTexture = NULL;
}


void MagicCard::SetTexture( const char *name, DWORD flags )
{
	if( mTexture )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexture );

	mTexture = GEngine->Video->MaterialSys->CreateTexture( name, flags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );
}


//--------------------------------------------------------------------
//	BoardSlotGroup class
//--------------------------------------------------------------------

// Localized strings
static String gCostStr = "";
static String gWithdrawalStr = "";
static String gAfterWithdrawalStr = "";
static String gSilverUsageStr = "";
static String gGoldenUsageStr = "";
static String gNotEnoughGoldStr = "";
static String gYourGoldStr = "";
static String gGoldRequiredStr = "";
static String gExitToTheMapStr = "";

BoardSlotGroup::BoardSlotGroup()
{
	mPosX = -1;
	mPosY = -1;
	mNumSlots = 0;
	mSlotWidth = 0;
	mSlotHeight = 0;
	mSpaceWidth = 0;
	mUnderMouse = -1;
	mTimer = 0.0f;
}


BoardSlotGroup::~BoardSlotGroup()
{
	Clear();
}


void BoardSlotGroup::Clear()
{
	for( int i=0; i<mCards.Size(); i++ )
	{
		if( mCards[i] )
			mCards[i] = NULL;
	}
	mCards.ClearArray();
}


void BoardSlotGroup::CheckMouse()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	float x_scale = 1024.0f / screenWidth;
	float y_scale = 768.0f / screenHeight;

	POINT mousePos = GEngine->InputSys->GetMousePosition();
	mousePos.x = (int)(mousePos.x * x_scale);
	mousePos.y = (int)(mousePos.y * y_scale);

	bool wasUnder = ( mUnderMouse > -1 ) ? true : false;

	mUnderMouse = -1;

	for( int i=0; i<mNumSlots; i++ )
	{
		int x1 = mPositions[i];
		int x2 = x1 + mSlotWidth;

		int up = 1;
//		if( mPosY > 512 )
			up = 0;

		int pos_y = mPosY;
		int pos_y2 = mPosY2;
		if( GEngine->Video->MBoard->AddOnEnabled() && ( i % 2 ) == up && i < 12 && mSlotHeight < 200 )
		{
			pos_y -= 19;
			pos_y2 -= 19;
		}
		else if( GEngine->Video->MBoard->AddOnEnabled() && up == 0 && i > 11 && mSlotHeight < 200 )
		{
			pos_y -= 19;
			pos_y2 -= 19;
		}

		if( mousePos.x > x1 && mousePos.x < x2 )
		{
			if( i < 18 )
			{
				if( mousePos.y > pos_y && mousePos.y < pos_y + mSlotHeight )
				{
					mUnderMouse = i;
					break;
				}
			}
			else
			{
				if( mousePos.y > pos_y2 && mousePos.y < pos_y2 + mSlotHeight )
				{
					mUnderMouse = i;
					break;
				}
			}
		}
	}

	if( mUnderMouse == -1 )
		mTimer = 0.0f;
	else if( !wasUnder )
		mTimer = GEngine->SysDriver.GetCurrentTime();
}


bool BoardSlotGroup::InsertCard( MagicCard *card, int slot )
{
	if( slot == -1 )
	{
		if( mCards.Size() == 0 )
		{
			slot = 0;
		}
		else
		{
			for( int i=0; i<mCards.Size(); i++ )
			{
				if( mCards[i] == NULL )
				{
					slot = i;
					break;
				}
			}

			if( slot == -1 )
			{
				slot = mCards.Size();
				mCards.Add( card );
				return true;
			}
		}
	}

	if( mCards.Size() > slot )
	{
		if( mCards[slot] != NULL )
			return false;
		else
		{
			mCards[slot] = card;
			return true;
		}
	}
	else
	{
		int first = mCards.Size();
		for( int i=first; i<slot; i++ )
		{
			mCards.Add( NULL );
		}

		mCards.Add( card );
		return true;
	}

	return false;
}


void BoardSlotGroup::DrawCards()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scale_x = screenWidth / 1024.0f;
	float scale_y = screenHeight / 768.0f;

	CheckMouse();

	for( int i=0; i<mCards.Size(); i++ )
	{
		if( !mCards[i] )
			continue;
		if( !mCards[i]->GetTexture() )
			continue;
		if( !mCards[i]->IsAvailable() )
			continue;

		int up = 1;
//		if( mPosY > 512 )
			up = 0;

		int pos_x = (int)(( mPositions[i] ) * scale_x);
		int pos_y = (int)(mPosY * scale_y);
		if( GEngine->Video->MBoard->AddOnEnabled() && ( i % 2 ) == up && i < 12 && mSlotHeight < 200 )
			pos_y -= (int)( 19 * scale_y );
		else if( GEngine->Video->MBoard->AddOnEnabled() && up == 0 && i > 11 && mSlotHeight < 200 )
			pos_y -= (int)( 19 * scale_y );
//		if( i > 11 )
//			pos_y = mPosY2 * scale_y;
		int width = (int)(mSlotWidth * scale_x);
		int height = (int)(mSlotHeight * scale_y);

		GEngine->Video->Hud->DrawQuad( mCards[i]->GetTexture(), pos_x, pos_y, width, height );
	}

	if( IsUnderMouse() && mUnderMouse < mCards.Size() && !GEngine->Video->MBoard->GetZoomCard() )
	{
		if( mCards[mUnderMouse] && mCards[mUnderMouse]->IsAvailable() )
		{
			DrawCardDesc( mCards[mUnderMouse] );
		}
	}
	else if( GEngine->Video->MBoard->GetZoomCard() )
	{
		DrawCardDesc( GEngine->Video->MBoard->GetZoomCard() );
	}
}


void BoardSlotGroup::DrawCardDesc( MagicCard *card )
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scale_x = screenWidth / 1024.0f;
	float scale_y = screenHeight / 768.0f;

	int x = (int)(80 * scale_x);
	int y = (int)(460 * scale_y);

#ifdef TRANSGAMING
    const int   BUFSIZE = 256;
	char        desc[BUFSIZE];


	strncpy( desc, card->GetDesc(), BUFSIZE);
#else
	char desc[255];
	strcpy( desc, card->GetDesc().AsChar() );
#endif

	char *tmp = desc;
	while( *tmp )
	{
		if( *tmp == '\\' && *( tmp + 1 ) == 'n' )
		{
			*tmp = ' '; *( tmp + 1 ) = '\n';
		}
		tmp++;
	}

	GEngine->Video->Hud->SetFont( "timesbd", 24 );

	int fontHeight = GEngine->Video->Hud->GetTextHeight();

#ifdef TRANSGAMING
	GEngine->Video->Hud->Print( x, y, card->GetName(), 0xff1700d6 );
	GEngine->Video->Hud->Print( x, y + fontHeight, desc, 0xff7abaff );
#else
	GEngine->Video->Hud->Print( x, y, card->GetName(), 0xffd60017 );
	GEngine->Video->Hud->Print( x, y + fontHeight, desc, 0xffffba7a );
#endif
	if( card->GetCost() )
	{
#ifdef TRANSGAMING
        const int   COSTSIZE = 32;
		char        cost[COSTSIZE];
#else
		char cost[30];
#endif

		if( mNumSlots > 4 || card == GEngine->Video->MBoard->GetZoomCard() )
		{
#ifdef TRANSGAMING
			snprintf( desc, BUFSIZE, "%s: ", gCostStr.AsChar() );
			GEngine->Video->Hud->Print( x, y + fontHeight * 3, desc, 0xff7abaff );
			snprintf( cost, COSTSIZE, "%d", card->GetCost() );
			GEngine->Video->Hud->Print( x + GEngine->Video->Hud->GetTextWidth( desc ), y + fontHeight * 3, cost, 0xff1700d6 );
#else
			sprintf( desc, "%s: ", gCostStr.AsChar() );
			GEngine->Video->Hud->Print( x, y + fontHeight * 3, desc, 0xffffba7a );
			sprintf( cost, "%d", card->GetCost() );
			GEngine->Video->Hud->Print( x + GEngine->Video->Hud->GetTextWidth( desc ), y + fontHeight * 3, cost, 0xffd60017 );
#endif
		}
		else
		{
#ifdef TRANSGAMING
			snprintf( desc, BUFSIZE, "%s: ", gWithdrawalStr.AsChar() );
			snprintf( cost, COSTSIZE, "%d", card->GetCost() / 2 );
			GEngine->Video->Hud->Print( x, y + fontHeight * 3, desc, 0xff7abaff );
			GEngine->Video->Hud->Print( x + GEngine->Video->Hud->GetTextWidth( desc ), y + fontHeight * 3, cost, 0xff1700d6 );
			snprintf( desc, BUFSIZE, "%s: %d", gAfterWithdrawalStr.AsChar(), GEngine->Video->MBoard->GetCash() + card->GetCost() / 2 );
			GEngine->Video->Hud->Print( x, y + fontHeight * 4, desc, 0xff7abaff );
#else
			sprintf( desc, "%s: ", gWithdrawalStr.AsChar() );
			sprintf( cost, "%d", card->GetCost() / 2 );
			GEngine->Video->Hud->Print( x, y + fontHeight * 3, desc, 0xffffba7a );
			GEngine->Video->Hud->Print( x + GEngine->Video->Hud->GetTextWidth( desc ), y + fontHeight * 3, cost, 0xffd60017 );
			char after[255];
			sprintf( after, "%s: %d", gAfterWithdrawalStr.AsChar(), GEngine->Video->MBoard->GetCash() + card->GetCost() / 2 );
			GEngine->Video->Hud->Print( x, y + fontHeight * 4, after, 0xffffba7a );
#endif
		}
	}
}


void BoardSlotGroup::DrawTooltip()
{
	if( !IsUnderMouse() || ( GEngine->SysDriver.GetCurrentTime() - mTimer < 1.0f ) || GEngine->Video->MBoard->GetZoomCard() )
		return;

	HUD *hud = GEngine->Video->Hud;

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scale_x = screenWidth / 1024.0f;
	float scale_y = screenHeight / 768.0f;

	int x = (int)(200 * scale_x);
	int y = (int)(200 * scale_y);

	if( mNumSlots == 2 )
	{
		hud->DrawBorder( 200, 350, 624, 108 );
		hud->SetFont( "timesbd", 26 );
#ifdef TRANSGAMING
		hud->Print( (int)(x + 20 * scale_x), (int)(370 * scale_y), gSilverUsageStr.AsChar(), 0xff7abaff );
#else
		hud->Print( (int)(x + 20 * scale_x), (int)(370 * scale_y), gSilverUsageStr.AsChar(), 0xffffba7a );
#endif
	}
	else if( mNumSlots == 3 )
	{
		hud->DrawBorder( 200, 200, 624, 108 );
		hud->SetFont( "timesbd", 26 );

#ifdef TRANSGAMING
        // eric-> i'm not sure why they don't just include a '%d' format character in the language text
        //   then just snprintf() to the string here.  It could be that they are using a text encoding
        //   that could potentially use the '%' character as a shift character.  I will investigate this
        //   further before completely replacing this
        static const int    BUFSIZE = 256;
		static char         fixed[BUFSIZE];


		strncpy(fixed, gGoldenUsageStr.AsChar(), BUFSIZE);
		char *xxx = strstr( fixed, "[x]" );


		if( xxx )
		{
			if( GEngine->Video->MBoard->FindCardInGroup( MagicBoard::EPermSelSlots, 8 ) )
				xxx[0] = '2';
			if( GEngine->Video->MBoard->FindCardInGroup( MagicBoard::EPermSelSlots, 9 ) )
				xxx[0] = '3';
			else
				xxx[0] = '1';

			strncpy( xxx + 1, xxx + 3, BUFSIZE - (xxx - fixed) - 1 );
		}

		hud->Print( (int)(x + 20 * scale_x), (int)(y + 20 * scale_y), fixed, 0xff7abaff );
#else
		String tip = gGoldenUsageStr;
		static char fixed[1024];
		strcpy( fixed, tip.AsChar() );
		char *xxx = strstr( fixed, "[x]" );
		if( xxx )
		{
			if( GEngine->Video->MBoard->FindCardInGroup( MagicBoard::EPermSelSlots, 8 ) )
				xxx[0] = '2';
			else
				xxx[0] = '1';
			strcpy( xxx + 1, xxx + 3 );
		}

		hud->Print( (int)(x + 20 * scale_x), (int)(y + 20 * scale_y), fixed, 0xffffba7a );
#endif
	}
}


MagicCard *BoardSlotGroup::GetCard( int slot )
{
	if( slot >= mCards.Size() )
		return NULL;

	if( !mCards[slot] )
		return NULL;

	if( mCards[slot]->IsAvailable() )
		return mCards[slot];
	else
		return NULL;
}


void BoardSlotGroup::GetCardSize( int &x, int &y )
{
	x = mSlotWidth;
	y = mSlotHeight;
}


void BoardSlotGroup::GetCardPosition( MagicCard *card, int &x, int &y )
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scale_x = screenWidth / 1024.0f;
	float scale_y = screenHeight / 768.0f;

	for( int i=0; i<mCards.Size(); i++ )
	{
		if( mCards[i] == card )
		{
			int up = 1;
//			if( mPosY > 512 )
				up = 0;

			x = (int)(( mPositions[i] ) * scale_x);
//			if( i > 11 )
//				y = mPosY2 * scale_y;
//			else
				y = (int)(mPosY * scale_y);
			if( GEngine->Video->MBoard->AddOnEnabled() && ( i % 2 ) == up && i < 12 && mSlotHeight < 200 )
				y -= (int)( 19 * scale_y );
			else if( GEngine->Video->MBoard->AddOnEnabled() && up == 0 && i > 11 && mSlotHeight < 200 )
				y -= (int)( 19 * scale_y );
		}
	}
}

#ifdef TRANSGAMING
bool BoardSlotGroup::isSlotAvailable(){
    for( int i=0; i<mNumSlots; i++ ){
        if ( !GetCard( i ) )
            return true;
    }
	
    return false;
}
#endif

//--------------------------------------------------------------------
//	MagicBoard class
//--------------------------------------------------------------------

MagicBoard::MagicBoard()
{
	mActive = false;
	mAddOn = false;

	mBackground = NULL;

	mCrystal = NULL;

	mCash = 0;
	mTargetCash = 0;
	mAllFree = false;
	mAllFreeBonus = false;

	mShowMouse = false;
	mMouseClickOffsetX = 0;
	mMouseClickOffsetY = 0;
	mMouseTexture = NULL;

	mDragX = 0;
	mDragY = 0;
	mDraggedCard = NULL;
	mZoomCard = NULL;
	mZoomTex = NULL;
	mTakenFrom = -1;

	for( int i=0; i<36; i++ )
	{
		mTimeCards[i] = NULL;
		mPermCards[i] = NULL;
	}

	for( int i=0; i<10; i++ )
	{
		mNumbers[i] = NULL;
	}

	mCounterShadow = NULL;
	mCounterOverlay = NULL;

	mPausedSounds = -1;

	mNotEnoughMoney = 0;
	mTimer = GEngine->SysDriver.GetCurrentTime();

	ScriptObject txt = GScript.Globals()["TXT"];
	if( !txt.IsNil() )
	{
		gCostStr = txt["Board"]["Cost"].GetString();
		gWithdrawalStr = txt["Board"]["Withdrawal"].GetString();
		gAfterWithdrawalStr = txt["Board"]["AfterWithdrawal"].GetString();
		gSilverUsageStr = HUD_PrepareString( txt["Board"]["SilverUsage"].GetString() );
		gGoldenUsageStr = HUD_PrepareString( txt["Board"]["GoldenUsage"].GetString() );
		gNotEnoughGoldStr = HUD_PrepareString( txt["Board"]["NotEnoughGold"].GetString() );
		gYourGoldStr = txt["Board"]["YourGold"].GetString();
		gGoldRequiredStr = txt["Board"]["GoldRequired"].GetString();
		gExitToTheMapStr = txt["Board"]["ExitToTheMap"].GetString();
	}
}


MagicBoard::~MagicBoard()
{
	Clear();
}


void MagicBoard::Render()
{
	View *view = GEngine->Video->GetActiveView();

	if( mBackground )
		GEngine->Video->Hud->DrawQuad( mBackground, 0.0f, 0.0f, view->LocalViewport.Width, view->LocalViewport.Height );

	GEngine->Video->Hud->DrawBorder( 60, 430, 370, 160 );

	for( int i=0; i<4; i++ )
	{
		mSlots[i].DrawCards();
		if( mSlots[i].IsUnderMouse() && mSlots[i].GetCard(mSlots[i].SlotUnderMouse()) )
			mNotEnoughMoney = 0;
	}

	if( mCrystal && !mZoomCard )
	{
		mCrystal->CalcSize();
		mCrystal->SetUnderMouse( false );
		if( mCrystal->CheckIfUnderMouse() )
			mNotEnoughMoney = 0;
		mCrystal->SetTransparency( 0xFF );
		mCrystal->Render();
		mCrystal->RenderDesc();
	}

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scale_x = screenWidth / 1024.0f;
	float scale_y = screenHeight / 768.0f;

	if( mNotEnoughMoney > 0 )
	{
		int x = (int)(80 * scale_x);
		int y = (int)(460 * scale_y);
		if( GEngine->SysDriver.GetCurrentTime() - mTimer < 2.0f )
		{
			GEngine->Video->Hud->SetFont( "timesbd", 24 );
			int fontHeight = GEngine->Video->Hud->GetTextHeight();
			
#ifdef TRANSGAMING
			GEngine->Video->Hud->Print( x, y, gNotEnoughGoldStr.AsChar(), 0xff7abaff );
            
            const int   BUFSIZE = 256;
			char        tmp[BUFSIZE];


			snprintf( tmp, BUFSIZE, "%s: ", gYourGoldStr.AsChar() );
			GEngine->Video->Hud->Print( x, y + fontHeight * 2, tmp, 0xff7abaff );
			int newx = x + GEngine->Video->Hud->GetTextWidth( tmp );
			snprintf( tmp, BUFSIZE, "%d", mTargetCash );
			GEngine->Video->Hud->Print( newx, y + fontHeight * 2, tmp, 0xff1700d6 );
			
			snprintf( tmp, BUFSIZE, "%s: ", gGoldRequiredStr.AsChar() );
			GEngine->Video->Hud->Print( x, y + fontHeight * 3, tmp, 0xff7abaff );
			newx = x + GEngine->Video->Hud->GetTextWidth( tmp );
			snprintf( tmp, BUFSIZE, "%d", mNotEnoughMoney );
			GEngine->Video->Hud->Print( newx, y + fontHeight * 3, tmp, 0xff1700d6 );
#else
			GEngine->Video->Hud->Print( x, y, gNotEnoughGoldStr.AsChar(), 0xffffba7a );
			
            char tmp[255];
			sprintf( tmp, "%s: ", gYourGoldStr.AsChar() );
			GEngine->Video->Hud->Print( x, y + fontHeight * 2, tmp, 0xffffba7a );
			int newx = x + GEngine->Video->Hud->GetTextWidth( tmp );
			sprintf( tmp, "%d", mTargetCash );
			GEngine->Video->Hud->Print( newx, y + fontHeight * 2, tmp, 0xffd60017 );
			
			sprintf( tmp, "%s: ", gGoldRequiredStr.AsChar() );
			GEngine->Video->Hud->Print( x, y + fontHeight * 3, tmp, 0xffffba7a );
			newx = x + GEngine->Video->Hud->GetTextWidth( tmp );
			sprintf( tmp, "%d", mNotEnoughMoney );
			GEngine->Video->Hud->Print( newx, y + fontHeight * 3, tmp, 0xffd60017 );
#endif
		}
		else
			mNotEnoughMoney = 0;
	}

	DrawCounter();

	for( int i=0; i<4; i++ )
	{
		mSlots[i].DrawTooltip();
	}

	POINT mousePos = GEngine->InputSys->GetMousePosition();

	if( mDraggedCard != NULL && mZoomCard == NULL )
	{
		int cardWidth, cardHeight;
		mSlots[ETimeAllSlots].GetCardSize( cardWidth, cardHeight );
		GEngine->Video->Hud->DrawQuad( mDraggedCard->GetTexture(), mousePos.x - mDragX, mousePos.y - mDragY, cardWidth * scale_x, cardHeight * scale_y );
	}

	if( mZoomCard && mZoomTex )
	{
		GEngine->Video->Hud->DrawBorder( 220, 118, 584, 560 );
		int cardWidth = (int)(mZoomTex->OriginalWidth * scale_x);
		int cardHeight = (int)(mZoomTex->OriginalHeight * scale_y);
		GEngine->Video->Hud->DrawQuad( mZoomTex, 256.0f * scale_x, 146.0f * scale_y, cardWidth, cardHeight );
	}

	if( mMouseTexture && !( mMouseTexture->Flags & Texture::ETexInvalid ) && mShowMouse )
	{
		GEngine->Video->Hud->DrawQuad( mMouseTexture, (float)(mousePos.x-mMouseClickOffsetX),
			(float)(mousePos.y-mMouseClickOffsetY), (float)mMouseTexture->Width, (float)mMouseTexture->Height );
	}
}


void MagicBoard::DrawCounter()
{
	static float lastChange = 0.0f;

	if( mCash != mTargetCash )
	{
		float time = GEngine->SysDriver.GetCurrentTime();
		if( lastChange == 0.0f )
		{
			lastChange = time;
			GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/money-rollnumbers-loop", true );
		}

		int dir = ( mCash > mTargetCash ) ? -1 : 1;

		mCash += ( int )( ( float )dir * ( time - lastChange ) * 10.0f );
		if( ( dir == 1 && mCash > mTargetCash ) || ( dir == -1 && mCash < mTargetCash ) )
			mCash = mTargetCash;
		if( mCash < 0 ) mCash = 0;
	}
	else
	{
		if( lastChange != 0.0f )
		{
			GEngine->Video->PMenu->StopSound();
			GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/money-end_transaction" );
		}

		lastChange = 0.0f;
	}


#ifdef TRANSGAMING
	char str[10];
	snprintf( str, sizeof(str), "%06d", mCash );
#else
	char str[10];
	sprintf( str, "%06d", mCash );
#endif

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scale_x = screenWidth / 1024.0f;
	float scale_y = screenHeight / 768.0f;

	if( mNumbers[0] )
	{
		GEngine->Video->Hud->DrawQuad( mNumbers[str[0]-'0'], 724 * scale_x, 232 * scale_y, mNumbers[0]->OriginalWidth * scale_x, mNumbers[0]->OriginalHeight * scale_y );
		GEngine->Video->Hud->DrawQuad( mNumbers[str[1]-'0'], 752 * scale_x, 232 * scale_y, mNumbers[0]->OriginalWidth * scale_x, mNumbers[0]->OriginalHeight * scale_y );
		GEngine->Video->Hud->DrawQuad( mNumbers[str[2]-'0'], 782 * scale_x, 232 * scale_y, mNumbers[0]->OriginalWidth * scale_x, mNumbers[0]->OriginalHeight * scale_y );
		GEngine->Video->Hud->DrawQuad( mNumbers[str[3]-'0'], 810 * scale_x, 232 * scale_y, mNumbers[0]->OriginalWidth * scale_x, mNumbers[0]->OriginalHeight * scale_y );
		GEngine->Video->Hud->DrawQuad( mNumbers[str[4]-'0'], 840 * scale_x, 232 * scale_y, mNumbers[0]->OriginalWidth * scale_x, mNumbers[0]->OriginalHeight * scale_y );
		GEngine->Video->Hud->DrawQuad( mNumbers[str[5]-'0'], 868 * scale_x, 232 * scale_y, mNumbers[0]->OriginalWidth * scale_x, mNumbers[0]->OriginalHeight * scale_y );
	}

	if( mCounterOverlay )
		GEngine->Video->Hud->DrawQuadOverlay( mCounterOverlay, 726 * scale_x, 236 * scale_y, mCounterOverlay->OriginalWidth * scale_x, mCounterOverlay->OriginalHeight * scale_y );

	if( mCounterShadow )
		GEngine->Video->Hud->DrawQuad( mCounterShadow, 726 * scale_x, 236 * scale_y, mCounterShadow->OriginalWidth * scale_x, mCounterShadow->OriginalHeight * scale_y );
}


void MagicBoard::Clear()
{
	if( mBackground && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mBackground );
	mBackground = NULL;

	if( mMouseTexture && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );
	mMouseTexture = NULL;

	for( int i=0; i<36; i++ )
	{
		if( mTimeCards[i] )
			delete mTimeCards[i];
		if( mPermCards[i] )
			delete mPermCards[i];
		mTimeCards[i] = NULL;
		mPermCards[i] = NULL;
	}

	for( int i=0; i<10; i++ )
	{
		if( mNumbers[i] && GEngine->Video->MaterialSys )
			GEngine->Video->MaterialSys->ReleaseTexture( mNumbers[i] );
		mNumbers[i] = NULL;
	}

	if( mCounterShadow && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mCounterShadow );
	if( mCounterOverlay && GEngine->Video->MaterialSys )
		GEngine->Video->MaterialSys->ReleaseTexture( mCounterOverlay );

	mCounterShadow = NULL;
	mCounterOverlay = NULL;

	for( int i=0; i<4; i++ )
	{
		mSlots[i].Clear();
	}

	if( mCrystal )
	{
		delete mCrystal;
		mCrystal = NULL;
	}

    if (mZoomTex)
        GEngine->Video->MaterialSys->ReleaseTexture(mZoomTex);

	mDragX = 0;
	mDragY = 0;
	mDraggedCard = NULL;
    mZoomTex = NULL;

	if( GEngine->Video->PMenu )
		GEngine->Video->PMenu->StopSound();
}


void MagicBoard::Setup()
{
    m_setTag("Game");

	MenuItemImageButton *butt = new MenuItemImageButton;
	butt->SetName( "Crystal" );
	butt->SetText( "" );
#ifdef TRANSGAMING
	butt->SetDesc( gExitToTheMapStr.AsChar() );
#else
	butt->SetDesc( gExitToTheMapStr );
#endif
	butt->SetImage( "HUD/Board/krysztal_board_czysty", "HUD/Board/krysztal_board_swiec", "HUD/Board/krysztal_board_wcisniety" );
	butt->SetGlowAnim( "HUD/Board/krysztal_board_swiec" );
	butt->SetPosition( 511, 196 );
	butt->SetAlign( EAlignNone );
	butt->SetAction( "MagicBoard_UpdateCardsStatus(); PMENU.SwitchToMap()" );
	butt->SetBigFont( "timesbd", 14 );
	butt->SetSmallFont( "timesbd", 26 );
#ifdef TRANSGAMING
    butt->SetColors( 0xff7abaff, 0xff7abaff, 0xff7abaff, 0xff7abaff );
#else
    butt->SetColors( 0xffffba7a, 0xffffba7a, 0xffffba7a, 0xffffba7a );
#endif
	butt->SetLightOnSound( "menu/magicboard/option-light-on" );
	butt->SetAcceptSound( "menu/magicboard/option-accept" );
	butt->SetDescPos( 80, 460 );

	mCrystal = butt;
}


void MagicBoard::Activate( bool active )
{
	mActive = active;

	static BOOL mouseLocked = GEngine->InputSys->IsMouseLocked();
	static BOOL mouseVisible = GEngine->InputSys->IsMouseVisible();

	ScriptObject game = GScript.Globals()["Game"];
	mAddOn = game["AddOn"].GetBool();

	bool blackEd = GScript.Globals()["Cfg"]["BlackEdition"].GetBool();

	if( active )
	{
		if( !mMouseTexture )
			SetMouseTexture( 0, 0, "HUD/kursor" );
		// BLAIR - We're disabling this mouse...permanently!
#ifndef TRANSGAMING
		ShowMouse( true );
#endif
		if( !mAddOn )
		{
			if( !blackEd )
				SetBackground( "HUD/Board/board" );
			else
				SetBackground( "HUD/Board/board_black" );
		}
		else
		{
			if( !blackEd )
				SetBackground( "HUD/Board/board_addon" );
			else
				SetBackground( "HUD/Board/board_addon_black" );
		}

		mouseLocked = GEngine->InputSys->IsMouseLocked();
		if( mouseLocked )
			GEngine->InputSys->LockMouse( FALSE );
		mouseVisible = GEngine->InputSys->IsMouseVisible();
		if( mouseVisible )
			GEngine->InputSys->ShowMouse( FALSE );

		GScript.DoString( "MagicBoard:Setup()" );

		mAllFreeBonus = game["GoldenCardsUseUnlimited"].GetBool();

//		mPausedSounds = GEngine->Audio->PauseCurrentlyPlayingSounds();

		if( !game.IsNil() )
		{
			mCash = game["PlayerMoney"].GetInt();
			mTargetCash = mCash;
		}

		Setup();
	}
	else
	{
		ShowMouse( false );
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
		GEngine->Video->GetActiveView()->Mode &= ~View::EModeDoNotRenderWorld;
		GEngine->Game->mPaused = false;
		GScript.DoString( "Game.Paused = false" );

		if( mPausedSounds != -1 )
		{
//			GEngine->Audio->ResumeSounds( mPausedSounds );
			mPausedSounds = -1;
		}

		if( !game.IsNil() )
			GScript.DoString( "Game.PlayerMoney=%d; MagicBoard_SaveStatus()", mCash );
	}

}


void MagicBoard::SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags )
{
	if( mMouseTexture )
		GEngine->Video->MaterialSys->ReleaseTexture( mMouseTexture );

	mMouseTexture = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mMouseClickOffsetX = xClickOffset;
	mMouseClickOffsetY = yClickOffset;

	if( mMouseTexture == GEngine->Video->MaterialSys->Textures.NoTex )
		mMouseTexture = 0;
}


void MagicBoard::SetBackground( const char *texName, DWORD texFlags )
{
	if( mBackground )
		GEngine->Video->MaterialSys->ReleaseTexture( mBackground );

	mBackground = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	if( mBackground == GEngine->Video->MaterialSys->Textures.NoTex )
		mBackground = NULL;

	if( !mCounterShadow )
		mCounterShadow = GEngine->Video->MaterialSys->CreateTexture( "HUD/Board/prostokat_cienie", texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	if( !mCounterOverlay )
		mCounterOverlay = GEngine->Video->MaterialSys->CreateTexture( "HUD/Board/pasek_szary_bez_alfy", texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	for( int i=0; i<10; i++ )
	{
		if( !mNumbers[i] )
		{
#ifdef TRANSGAMING
			char name[256];
			snprintf( name, sizeof(name), "HUD/Board/cyferki/%d", i );
#else
			char name[255];
			sprintf( name, "HUD/Board/cyferki/%d", i );
#endif
			mNumbers[i] = GEngine->Video->MaterialSys->CreateTexture( name, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );
		}
	}
}


bool MagicBoard::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( !Active() )
		return false;

	if( mCrystal && mDraggedCard == NULL && mZoomCard == NULL )
	{
#ifdef TRANSGAMING
		if ((eventType == EMenuKeyUp && eventData1 == EMouseKeyRight) || mCrystal->SendEvent(eventType, eventData1, eventData2)){
			if (mCrystal->GetAction() != NULL)
				GEngine->InputSys->SetLuaToDo( mCrystal->GetAction() );

			return true;
		}
#else
		if( (eventType == EMenuKeyUp && eventData1 == VK_ESCAPE) || mCrystal->SendEvent( eventType, eventData1, eventData2 )  )
		{
			if( !mCrystal->GetAction().IsNull() )
			{
				GEngine->InputSys->SetLuaToDo( mCrystal->GetAction() );
			}
			return true;
		}
#endif
	}

	if( eventType != EMenuKeyUp )
		return false;

    //!!// add card navigation controls here

#ifdef TRANSGAMING
    //show the large version of a card if the middle button is pushed
	if (eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyMiddle)
		return false;
#else
	if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyRight )
		return false;
#endif

	int slot = -1;
	int group = -1;


    //find the slot that a card is over
	if ( !mZoomCard )
	{

        //search all the slots to see if the mouse is over any of them
		for( int i=0; i<4; i++ )
		{
			mSlots[i].CheckMouse();
			if( mSlots[i].IsUnderMouse() )
			{
				slot = mSlots[i].SlotUnderMouse();
				group = i;
				break;
			}
		}

		if( slot >= mSlots[group].GetNumSlots() )
			slot = -1;

        //released the card
		if( slot == -1 && !mZoomCard )
		{
			if( mDraggedCard )
			{
				if( mDraggedCard->GetType() == MagicCard::ECardTypePerm )
				{
					if( mTakenFrom == EPermSelSlots && !mAllFree && !mAllFreeBonus )
					{
						mTargetCash = mCash + mDraggedCard->GetCost() / 2;
					}
					mSlots[EPermAllSlots].InsertCard( mDraggedCard, mDraggedCard->GetIndex() );
				}
				else
				{
					if( mTakenFrom == ETimeSelSlots && !mAllFree && !mAllFreeBonus )
					{
						mTargetCash = mCash + mDraggedCard->GetCost() / 2;
					}
					mSlots[ETimeAllSlots].InsertCard( mDraggedCard, mDraggedCard->GetIndex() );
				}
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/card-put_off" );
			}
			mDraggedCard = NULL;
			return false;
		}
	}	

#ifdef TRANSGAMING
    //select a card for zooming in on
	if( eventData1 == EMouseKeyMiddle || ( eventData1 == EMouseKeyLeft && mZoomCard ) )
#else
    if( eventData1 == EMouseKeyRight || ( eventData1 == EMouseKeyLeft && mZoomCard ) )
#endif
	{
		if( mZoomCard != NULL )
		{
			mZoomCard = NULL;
			if( mZoomTex )
			{
				GEngine->Video->MaterialSys->ReleaseTexture( mZoomTex );
				mZoomTex = NULL;
			}
		}
		else if( slot != -1 )
		{
			mZoomCard = mSlots[group].GetCard( slot );
			if( mZoomCard )
				mZoomTex = GEngine->Video->MaterialSys->CreateTexture( mZoomCard->GetBigTex(), Texture::ETexNoMipMaps | Texture::ETexNoLOD );
		}
		return true;
	}

	if( !mDraggedCard )
	{
		mDraggedCard = mSlots[group].GetCard( slot );
		if( mDraggedCard )
		{
			GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/card-take" );
			if( group == ETimeAllSlots || group == EPermAllSlots )
			{
				mSlots[group].GetCardPosition( mDraggedCard, mDragX, mDragY );
				POINT mousePos = GEngine->InputSys->GetMousePosition();
				mDragX = mousePos.x - mDragX;
				mDragY = mousePos.y - mDragY;
			}
			else
			{
				mSlots[ETimeAllSlots].GetCardSize( mDragX, mDragY );
				mDragX /= 2;
				mDragY /= 2;
			}
			mSlots[group].RemoveCard( slot );
			mTakenFrom = group;
		}
	}
	else
	{
		bool insert = false;

		if( mDraggedCard->GetIndex() == 10 && group == EPermAllSlots && group != mTakenFrom && !mAllFreeBonus && !CanPutDivineAway() )
		{
			GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/wrong_place" );
			return false;
		}

		if( mDraggedCard->GetType() == MagicCard::ECardTypePerm )
		{
			if( group == ETimeSelSlots || group == ETimeAllSlots )
			{
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/wrong_place" );
				return false;
			}
			if( group == EPermSelSlots )
			{
				if( mCash >= mDraggedCard->GetCost() || mTakenFrom == group || mAllFree || mAllFreeBonus )
				{
					insert = true;
				}
				else
				{
					mNotEnoughMoney = mDraggedCard->GetCost();
					mTimer = GEngine->SysDriver.GetCurrentTime();
					GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/wrong_place" );
					return false;
				}
			}
			else if( !mZoomCard )
			{
				if( mTakenFrom == EPermSelSlots && ( !mAllFree || mDraggedCard->GetIndex() == 10 ) && !mAllFreeBonus )
				{
					mTargetCash = mCash + mDraggedCard->GetCost() / 2;
				}
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/card-put_off" );
				mSlots[EPermAllSlots].InsertCard( mDraggedCard, mDraggedCard->GetIndex() );
				if( mDraggedCard->GetIndex() == 10 && mTakenFrom != group && !mAllFreeBonus )
					MakeAllFree( false );
				mDraggedCard = NULL;
			}
		}
		else if( mDraggedCard->GetType() == MagicCard::ECardTypeTime )
		{
			if( group == EPermSelSlots || group == EPermAllSlots )
			{
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/wrong_place" );
				return false;
			}
			if( group == ETimeSelSlots )
			{
				if( mCash >= mDraggedCard->GetCost() || mTakenFrom == group || mAllFree || mAllFreeBonus )
				{
					insert = true;
				}
				else
				{
					mNotEnoughMoney = mDraggedCard->GetCost();
					mTimer = GEngine->SysDriver.GetCurrentTime();
					GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/wrong_place" );
					return false;
				}
			}
			else if( !mZoomCard )
			{
				if( mTakenFrom == ETimeSelSlots && !mAllFree && !mAllFreeBonus )
				{
					mTargetCash = mCash + mDraggedCard->GetCost() / 2;
				}
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/card-put_off" );
				mSlots[ETimeAllSlots].InsertCard( mDraggedCard, mDraggedCard->GetIndex() );
				mDraggedCard = NULL;
			}
		}

		if( insert )
		{
			if( group == ETimeAllSlots || group == EPermAllSlots )
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/card-put_off" );
			else if( group == EPermSelSlots )
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/card-put_off-special_place" );
			else if( group == ETimeSelSlots )
				GEngine->Video->PMenu->PlayMenuSound( "menu/magicboard/card-put_off-special_place2" );
			if( mSlots[group].InsertCard( mDraggedCard, slot ) )
			{
				if( ( group == ETimeSelSlots || group == EPermSelSlots ) && group != mTakenFrom && !mAllFree && !mAllFreeBonus )
				{
					mTargetCash = mCash - mDraggedCard->GetCost();
				}

				if( mDraggedCard->GetIndex() == 10 && group != mTakenFrom && !mAllFreeBonus )
				{
					if( group == EPermSelSlots )
					{
						MakeAllFree( true );
					}
					else if( group == EPermAllSlots )
					{
						MakeAllFree( false );
					}
				}
				mDraggedCard = NULL;
			}
		}
	}

	return false;
}


void MagicBoard::SetupSlots( int index, int count, int y, int size_x, int size_y, int spaceWidth, int y2 )
{
	mSlots[index].SetNumSlots( count );
	mSlots[index].SetPosition( 0, y, y2 );
	mSlots[index].SetSlotSize( size_x, size_y );
	mSlots[index].SetSpaceWidth( spaceWidth );
}


void MagicBoard::AddCard( int type, const char *name, const char *tex, const char *desc, int cost, bool avail, bool sel, const char *bigTex )
{
    m_setTag("Game");
	MagicCard *card = new MagicCard;
	if( !card )
		return;

	card->SetName( name );
	card->SetAvailable( avail );
	card->SetTexture( tex );
	card->SetType( type );
	card->SetDesc( desc );
	card->SetCost( cost );
	card->SetBigTex( bigTex );

	MagicCard **tab = NULL;
	BoardSlotGroup *slots = NULL;
	BoardSlotGroup *selSlots = NULL;

	if( type == MagicCard::ECardTypeTime )
	{
		tab = mTimeCards;
		slots = &mSlots[ETimeAllSlots];
		selSlots = &mSlots[ETimeSelSlots];
	}
	else if( type == MagicCard::ECardTypePerm )
	{
		tab = mPermCards;
		slots = &mSlots[EPermAllSlots];
		selSlots = &mSlots[EPermSelSlots];
	}

	if( !tab )
		return;

	for( int i=0; i<slots->GetNumSlots(); i++ )
	{
		if( !tab[i] )
		{
			card->SetIndex( i );
			tab[i] = card;
			if( !sel )
				slots->InsertCard( card, i );
			else
				selSlots->InsertCard( card );
			return;
		}
	}

	if( IsCardInSlot( EPermSelSlots, 10 ) )
		mAllFree = true;
}


void MagicBoard::SetSlotPosition( int group, int slot, int pos )
{
	mSlots[group].SetSlotPosition( slot, pos );
}


bool MagicBoard::FindCardInGroup( int group, int index )
{
	for( int i=0; i<mSlots[group].GetNumSlots(); i++ )
	{
		if( mSlots[group].GetCard( i ) )
		{
			if( mSlots[group].GetCard( i )->GetIndex() == index )
				return true;
		}
	}
	return false;
}


void MagicBoard::MakeAllFree( bool free )
{
	mAllFree = free;

	int refund = 0;
	int count = mSlots[EPermSelSlots].GetNumSlots();
	for( int i=0; i<count; i++ )
	{
		MagicCard *card = mSlots[EPermSelSlots].GetCard( i );
		if( card && card->GetIndex() != 10 )
			refund += card->GetCost();
	}

	count = mSlots[ETimeSelSlots].GetNumSlots();
	for( int i=0; i<count; i++ )
	{
		MagicCard *card = mSlots[ETimeSelSlots].GetCard( i );
		if( card )
			refund += card->GetCost();
	}

	if( !free )
		refund = -refund;

	mTargetCash += refund;
}


bool MagicBoard::CanPutDivineAway()
{
	unsigned int refund = 0;
	int count = mSlots[EPermSelSlots].GetNumSlots();
	for( int i=0; i<count; i++ )
	{
		MagicCard *card = mSlots[EPermSelSlots].GetCard( i );
		if( card && card->GetIndex() != 10 )
			refund += card->GetCost();
	}

	count = mSlots[ETimeSelSlots].GetNumSlots();
	for( int i=0; i<count; i++ )
	{
		MagicCard *card = mSlots[ETimeSelSlots].GetCard( i );
		if( card )
			refund += card->GetCost();
	}

	if( mCash >= refund )
		return true;
	else
		return false;
}



//--------------------------------------------------------------------
//	LUA Interface
//--------------------------------------------------------------------

static int luaBoard_SetupSlots( lua_State *state )
{
	Script L( state );

	int index = L.GetInt( 1 );
	int count = L.GetInt( 2 );
	int y = L.GetInt( 3 );
	int size_x = L.GetInt( 4 );
	int size_y = L.GetInt( 5 );
	int spaceWidth = L.GetInt( 6 );
	int y2 = L.GetInt( 7 );

	GEngine->Video->MBoard->SetupSlots( index, count, y, size_x, size_y, spaceWidth, y2 );

	return 0;
}


static int luaBoard_AddCard( lua_State *state )
{
	Script L( state );

	int type = L.GetInt( 1 );
	const char * name = L.GetString( 2 );
	const char * tex = L.GetString( 3 );
	const char * desc = L.GetString( 4 );
	int cost = L.GetInt( 5 );
	bool avail = L.GetBool( 6 );
	bool sel = L.GetBool( 7 );
	const char * bigTex = L.GetString( 8 );

	GEngine->Video->MBoard->AddCard( type, name, tex, desc, cost, avail, sel, bigTex );

	return 0;
}


static int luaBoard_SetSlotPosition( lua_State *state )
{
	Script L( state );

	int index = L.GetInt( 1 );
	int slot = L.GetInt( 2 );
	int pos = L.GetInt( 3 );

	GEngine->Video->MBoard->SetSlotPosition( index, slot, pos );

	return 0;
}


static int luaBoard_IsCardInSlot( lua_State *state )
{
	Script L( state );

	int group = L.GetInt( 1 );
	int slot = L.GetInt( 2 );

	L.PushBool( GEngine->Video->MBoard->IsCardInSlot( group, slot ) );

	return 1;
}


static int luaBoard_SetCashCheat( lua_State *state )
{
	Script L( state );

	int cash = L.GetInt( 1, 0 );

	GEngine->Video->MBoard->SetCashCheat( cash );

	return 0;
}


//--------------------------------------------------------------------
//	MBOARD library
//--------------------------------------------------------------------
static const luaL_reg p_mboard_lib[] = {
	{ "SetupSlots", luaBoard_SetupSlots },
	{ "AddCard", luaBoard_AddCard },
	{ "SetSlotPosition", luaBoard_SetSlotPosition },
	{ "IsCardInSlot", luaBoard_IsCardInSlot },
	{ "SetCashCheat", luaBoard_SetCashCheat },
	{ NULL, NULL }
};


void MagicBoard_RegisterLUALib()
{
	GScript.RegisterLibrary("MBOARD",p_mboard_lib);
}
