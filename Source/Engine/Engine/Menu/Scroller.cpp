#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"

#define DRAG_HEIGHT	14
#define DRAG_SPACE	2

//--------------------------------------------------------------------
//	MenuItemScroller class
//--------------------------------------------------------------------
MenuItemScroller::MenuItemScroller() : MenuItem()
{
	mMinValue = 0;
	mMaxValue = 100;
	mCurrValue = 0;
	mScrollerHeight = 0;
	mUpPos = 0;
	mDownPos = 0;
	mDragPos = 0;
	mActiveCtrl = MenuItemScroller::ENone;

	mUp = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/strzalka_mala", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mDown = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/strzalka_mala", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mDrag = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/dzwigienka_mala", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mBG = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/kreska_mala", Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	mType = EMenuItemScroller;
}


MenuItemScroller::~MenuItemScroller()
{
	if( GEngine->Video->MaterialSys )
	{
		if( mUp )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mUp );
			mUp = NULL;
		}
		if( mDown )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mDown );
			mDown = NULL;
		}
		if( mDrag )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mDrag );
			mDrag = NULL;
		}
		if( mBG )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mBG );
			mBG = NULL;
		}
	}
}


void MenuItemScroller::Render()
{
	if( !mVisible )
		return;

	HUD *hud = GEngine->Video->Hud;

	hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	CalcSize();

	// Arrows
	if( mBG  && !(mBG->Flags & Texture::ETexInvalid ) )
	{
		hud->DrawTiles( mBG, mX, mUpPos + mUp->OriginalHeight, 0.0f, mScrollerHeight * mScaleY - 2 * mUp->OriginalHeight, false );
	}

	if( mUp  && !(mUp->Flags & Texture::ETexInvalid ) )
	{
		if( !mDisabled )
			GEngine->Video->Hud->DrawQuad( mUp, mX, mUpPos, mUp->OriginalWidth, mUp->OriginalHeight, CalcTransColor( 0xFFFFFFFF ), 0.0f, 1.0f, 1.0f, 0.0f );
		else
			GEngine->Video->Hud->DrawQuad( mUp, mX, mUpPos, mUp->OriginalWidth, mUp->OriginalHeight, CalcTransColor( mDisabledColor ) );
	}

	if( mDrag  && !(mDrag->Flags & Texture::ETexInvalid ) )
	{
		if( !mDisabled )
			GEngine->Video->Hud->DrawQuad( mDrag, mX, mDragPos, mDrag->OriginalWidth, mDrag->OriginalHeight, CalcTransColor( 0xFFFFFFFF ) );
		else
			GEngine->Video->Hud->DrawQuad( mDrag, mX, mDragPos, mDrag->OriginalWidth, mDrag->OriginalHeight, CalcTransColor( mDisabledColor ) );
	}

	if( mDown  && !(mDown->Flags & Texture::ETexInvalid ) )
	{
		if( !mDisabled )
			GEngine->Video->Hud->DrawQuad( mDown, mX, mDownPos, mDown->OriginalWidth, mDown->OriginalHeight, CalcTransColor( 0xFFFFFFFF ) );
		else
			GEngine->Video->Hud->DrawQuad( mDown, mX, mDownPos, mDown->OriginalWidth, mDown->OriginalHeight, CalcTransColor( mDisabledColor ) );
	}

//	char number[255];
//	sprintf( number, "%d/%d", mCurrValue, mMaxValue );
//	GEngine->Video->Hud->Print( 100, 100, number, CalcTransColor( 0xffffffff ) );
}


void MenuItemScroller::CalcSize()
{
	MenuItem::CalcSize();

	mWidth = (int)(mDrag->OriginalWidth * mScaleX);
	mHeight = (int)(mScrollerHeight * mScaleY);

	mUpPos = (int)(mPosY * mScaleY);
	mDownPos = (int)(mUpPos + mScrollerHeight * mScaleY - mDown->OriginalHeight);

	if( mActiveCtrl == MenuItemSlider::EDrag )
	{
		POINT mousePos = GEngine->InputSys->GetMousePosition();
		mDragPos = mousePos.y - mDragY;
		int min = mUpPos + mUp->OriginalHeight - DRAG_SPACE;
		int max = (int)(mUpPos + ( mScrollerHeight * mScaleY - ( DRAG_SPACE + DRAG_HEIGHT ) - mDown->OriginalHeight ));

		if( mDragPos < min )
			mDragPos = min;
		if( mDragPos > max )
			mDragPos = max;
	}
	else
	{
		if( mMaxValue > mMinValue && mCurrValue > 0 )
			mDragPos = (int)(mUpPos + mUp->OriginalHeight + ( mScrollerHeight * mScaleY - DRAG_HEIGHT - mDown->OriginalHeight * 2 ) * mCurrValue / ( mMaxValue - mMinValue ) - DRAG_SPACE);
		else
			mDragPos = mUpPos + mUp->OriginalHeight - DRAG_SPACE;
	}

	if( mActiveCtrl == MenuItemScroller::EDrag )
	{
		POINT mousePos = GEngine->InputSys->GetMousePosition();
		if( mousePos.y < ( mUpPos + mUp->OriginalHeight ) )
			mCurrValue = mMinValue;
		else if( mousePos.y > ( mDownPos + mDown->OriginalHeight ) )
			mCurrValue = mMaxValue;
		else
		{
			mCurrValue = ( int )( ( float )( mDragPos + DRAG_SPACE - ( mUpPos + mUp->OriginalHeight ) ) * ( float )( mMaxValue - mMinValue ) / ( float )( mScrollerHeight * mScaleY - ( DRAG_HEIGHT + mDown->OriginalHeight * 2  ) ) ) + mMinValue;

			if( mDragPos + DRAG_SPACE > mUpPos + mUp->OriginalHeight )
			{
				mCurrValue += 1;
			}

			if( mCurrValue > mMaxValue )
				mCurrValue = mMaxValue;
			if( mCurrValue < mMinValue )
				mCurrValue = mMinValue;
		}
	}
}


void MenuItemScroller::CalcPosition()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;

	if( mPosX == -1 )
		mX = (int)(screenWidth - ( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ) - mWidth);
	else
		mX = ( int )( ( float )mPosX * mScaleX );

	if( mPosY == -1 )
		mPosY = GEngine->Video->PMenu->Screen.GetTopPos();
	mY = ( int )( ( float )mPosY * mScaleY );
}


bool MenuItemScroller::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( !mVisible )
		return false;

	int item = CheckMouse();

	if( item == MenuItemScroller::ENone && eventType != EMenuKeyUp )
		return false;

	if( eventData1 != EMouseKeyLeft )
		return false;

	switch( eventType )
	{
	case EMenuKeyDown:
	case EMenuKeyDownRepeated:
//		if( item == MenuItemScroller::EUp || item == MenuItemScroller::EDown )
		mActiveCtrl = item;
		break;

	case EMenuKeyUp:
		if( item == MenuItemScroller::EUp )
		{
			if( mCurrValue > mMinValue )
			{
				GEngine->Video->PMenu->PlayMenuSound( "menu/menu/scroller-move" );
				mCurrValue--;
			}
			else
				GEngine->Video->PMenu->StopSound();
		}
		else if( item == MenuItemScroller::EDown )
		{
			if( mCurrValue < mMaxValue )
			{
				GEngine->Video->PMenu->PlayMenuSound( "menu/menu/scroller-move" );
				mCurrValue++;
			}
			else
				GEngine->Video->PMenu->StopSound();
		}
		mActiveCtrl = MenuItemScroller::ENone;
		break;

	default:
		break;
	}

	return false;
}


int MenuItemScroller::CheckMouse()
{
	if( !mUnderMouse )
		return MenuItemScroller::ENone;

	POINT mousePos = GEngine->InputSys->GetMousePosition();
//	float texScale = ( float )mWidth * 0.75f / ( float )mUp->Width;
//	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
//	float texScale = screenHeight / 768.0f;

	if( mousePos.y > mUpPos && mousePos.y < mUpPos + mUp->OriginalHeight )
	{
		mDragX = 0;
		mDragY = 0;
		return MenuItemScroller::EUp;
	}
	if( mousePos.y > mDownPos && mousePos.y < mDownPos + mDown->OriginalHeight )
	{
		mDragX = 0;
		mDragY = 0;
		return MenuItemScroller::EDown;
	}
	if( mousePos.y > mDragPos + DRAG_SPACE && mousePos.y < mDragPos + DRAG_HEIGHT + DRAG_SPACE )
	{
		mDragX = mousePos.x - mPosX;
		mDragY = mousePos.y - mDragPos;
		return MenuItemScroller::EDrag;
	}

	return MenuItemScroller::ENone;
}


void MenuItemScroller::ScrollDown()
{
	if( mCurrValue < mMaxValue )
		mCurrValue++;
}


void MenuItemScroller::ScrollUp()
{
	if( mCurrValue > 0 )
		mCurrValue--;
}


void MenuItemScroller::SetMinMax( int min, int max )
{
	mMinValue = min;
	mMaxValue = max;

	if( mCurrValue < mMinValue )
		mCurrValue = mMinValue;
	if( mCurrValue > mMaxValue )
		mCurrValue = mMaxValue;

	if( min == max )
	{
		mVisible = false;
		Disable();
	}
	else
	{
		mVisible = true;
		Enable();
	}
}
