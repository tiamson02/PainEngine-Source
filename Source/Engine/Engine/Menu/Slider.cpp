#include "Engine.h"
#include "Render/HUD.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"

#define DRAG_WIDTH	18
#define DRAG_SPACE	9

//--------------------------------------------------------------------
//	MenuItemSlider class
//--------------------------------------------------------------------
MenuItemSlider::MenuItemSlider() : MenuItem()
{
	mMinValue = 0;
	mMaxValue = 100;
	mSliderWidth = 0;
	mFullWidth = false;
	mLeftPos = 0;
	mRightPos = 0;
	mDragPos = 0;
	mActiveCtrl = MenuItemSlider::ENone;
	mDrawValue = true;
	mDragX = 0;
	mDragY = 0;

	mLeft = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/strzalka_duza", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mRight = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/strzalka_duza", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mDrag = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/dzwigienka_duza", Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mBG = GEngine->Video->MaterialSys->CreateTexture( "HUD/border/kreska_duza", Texture::ETexNoMipMaps | Texture::ETexNoLOD );

	mType = EMenuItemSlider;
}


MenuItemSlider::~MenuItemSlider()
{
	if( GEngine->Video->MaterialSys )
	{
		if( mLeft )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mLeft );
			mLeft = NULL;
		}
		if( mRight )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mRight );
			mRight = NULL;
		}
		if( mDrag )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mDrag );
			mDrag = NULL;
		}
	}
}


void MenuItemSlider::SetSliderWidth( int width )
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int valueWidth = (int)(GEngine->Video->Hud->GetTextWidth( "9.99" ) * 1024.0f / screenWidth);
	mSliderWidth = width - valueWidth;
}


void MenuItemSlider::SetControlWidth( int width )
{
	mCtrlWidth = width;
}


void MenuItemSlider::Render()
{
	if( !mVisible )
		return;

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	int texY = (int)(mY + ( ( mHeight - mLeft->OriginalHeight * mScaleY ) / 2 ));

	if( mBG && !( mBG->Flags & Texture::ETexInvalid ) )
	{
		GEngine->Video->Hud->DrawTiles( mBG, mLeftPos + mLeft->OriginalWidth * mScaleX, mY + mHeight / 2 - 16 * mScaleY, mSliderWidth * mScaleX, 0.0f );
	}

	if( mLeft  && !(mLeft->Flags & Texture::ETexInvalid ) )
	{
		if( !mDisabled )
			GEngine->Video->Hud->DrawQuad( mLeft, mLeftPos, texY, mLeft->OriginalWidth * mScaleX, mLeft->OriginalHeight * mScaleY, 0xFFFFFFFF );
		else
			GEngine->Video->Hud->DrawQuad( mLeft, mLeftPos, texY, mLeft->OriginalWidth * mScaleX, mLeft->OriginalHeight * mScaleY, CalcTransColor( mDisabledColor ) );
	}

	if( mDrag  && !(mDrag->Flags & Texture::ETexInvalid ) )
	{
		if( !mDisabled )
			GEngine->Video->Hud->DrawQuad( mDrag, mDragPos, texY - 6 * mScaleY, mDrag->OriginalWidth * mScaleX, mDrag->OriginalHeight * mScaleY );
		else
			GEngine->Video->Hud->DrawQuad( mDrag, mDragPos, texY - 6 * mScaleY, mDrag->OriginalWidth * mScaleX, mDrag->OriginalHeight * mScaleY, CalcTransColor( mDisabledColor ) );
	}

	if( mRight  && !(mRight->Flags & Texture::ETexInvalid ) )
	{
		if( !mDisabled )
			GEngine->Video->Hud->DrawQuad( mRight, mRightPos, texY, mRight->OriginalWidth * mScaleX, mRight->OriginalHeight * mScaleY, 0xffffffff, 1.0f, 0.0f, 0.0f, 1.0f );
		else
			GEngine->Video->Hud->DrawQuad( mRight, mRightPos, texY, mRight->OriginalWidth * mScaleX, mRight->OriginalHeight * mScaleY, CalcTransColor( mDisabledColor ), 1.0f, 0.0f, 0.0f, 1.0f );
	}

	// Shadow
	if( mDrawShadow )
		GEngine->Video->Hud->Print( mX + 1, mY + 1, mText.AsChar(), CalcTransColor( 0x50000000 ) );

	// Normal
	if( !mDisabled )
		GEngine->Video->Hud->Print( mX, mY, mText.AsChar(), CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor ) );
	else
		GEngine->Video->Hud->Print( mX, mY, mText.AsChar(), CalcTransColor( mDisabledColor ) );

	// Value
	if( mDrawValue )
	{
#ifdef TRANSGAMING
        const int   BUFSIZE = 16;
		char        number[BUFSIZE];


		if( mIsFloat )
			snprintf( number, BUFSIZE, "%0.2f", ( float )mCurrValue / 100.0f );
		else
			snprintf( number, BUFSIZE, "%d", mCurrValue );
#else
		char number[50];
		if( mIsFloat )
			sprintf( number, "%0.2f", ( float )mCurrValue / 100.0f );
		else
			sprintf( number, "%d", mCurrValue );
#endif

		int valueWidth = GEngine->Video->Hud->GetTextWidth( "9.99" );
		int rightPos = (int)(mRightPos + mRight->OriginalWidth * mScaleX + ( valueWidth - GEngine->Video->Hud->GetTextWidth( number ) ) / 2);

		if( mDrawShadow )
			GEngine->Video->Hud->Print( rightPos + 1, mY + 1, number, CalcTransColor( 0x50000000 ) );
		if( !mDisabled )
			GEngine->Video->Hud->Print( rightPos, mY, number, CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor ) );
		else
			GEngine->Video->Hud->Print( rightPos, mY, number, CalcTransColor( mDisabledColor ) );
	}
}


void MenuItemSlider::CalcSize()
{
	MenuItem::CalcSize();

	if( mFullWidth )
		mWidth = (int)(GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX);
	else
		mWidth += (int)(mCtrlWidth * mScaleX);

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int valueWidth = GEngine->Video->Hud->GetTextWidth( "9.99" );

	int currValue = mCurrValue;

	if( mFullWidth )
	{
		int margin = ( screenWidth - mWidth ) / 2;

		mRightPos = (int)(screenWidth - margin - mRight->OriginalWidth * mScaleX);
		mLeftPos = (int)(screenWidth - margin - mRight->OriginalWidth * mScaleX - mLeft->OriginalWidth * mScaleX - mSliderWidth * mScaleX);
	}
	else
	{
		mLeftPos = (int)(mPosX * mScaleX + mCtrlWidth * mScaleX - mSliderWidth * mScaleX - mRight->OriginalWidth * mScaleX * 2 - valueWidth);
		mRightPos = (int)(mLeftPos + mLeft->OriginalWidth * mScaleX + mSliderWidth * mScaleX);
	}

	if( mActiveCtrl == MenuItemSlider::EDrag )
	{
		POINT mousePos = GEngine->InputSys->GetMousePosition();
		mDragPos = mousePos.x - mDragX;
		int min = (int)(mLeftPos + mLeft->OriginalWidth * mScaleX - DRAG_SPACE * mScaleX);
		int max = (int)(min + mSliderWidth * mScaleX - DRAG_WIDTH * mScaleX);

		if( mDragPos < min )
			mDragPos = min;
		if( mDragPos > max )
			mDragPos = max;
	}
	else
	{
		mDragPos = (int)(mLeftPos + mLeft->OriginalWidth * mScaleX + ( mSliderWidth * mScaleX - DRAG_WIDTH * mScaleX ) * ( mCurrValue - mMinValue ) / ( mMaxValue - mMinValue ) - DRAG_SPACE * mScaleX);
	}

	if( mActiveCtrl == MenuItemSlider::EDrag )
	{
		POINT mousePos = GEngine->InputSys->GetMousePosition();
		if( mousePos.x < ( mLeftPos + mLeft->OriginalWidth * mScaleX ) )
			mCurrValue = mMinValue;
		else if( mousePos.x > ( mRightPos ) )
			mCurrValue = mMaxValue;
		else
		{
			mCurrValue = ( int )( ( float )( mDragPos + DRAG_SPACE * mScaleX - ( mLeftPos + mLeft->OriginalWidth * mScaleX ) ) * ( float )( mMaxValue - mMinValue ) / ( float )( ( mSliderWidth - DRAG_WIDTH ) * mScaleX ) ) + mMinValue;
			if( mDragPos + DRAG_SPACE * mScaleX > ( mLeftPos + mLeft->OriginalWidth * mScaleX ) )
			{
				mCurrValue += 1;
			}

			if( mCurrValue > mMaxValue )
				mCurrValue = mMaxValue;
			if( mCurrValue < mMinValue )
				mCurrValue = mMinValue;
		}
	}

	if( !mAction.IsNull() && currValue != mCurrValue )
		GScript.DoString( mAction.AsChar() );

	if( mApplyRequired && currValue != mCurrValue )
		GScript.DoString( "PainMenu:EnableApplyButton()" );
}


void MenuItemSlider::CalcPosition()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;

	if( mPosX == -1 )
	{
		switch( mAlign )
		{
		case EAlignNone:
			mX = (int)(( screenWidth - mCtrlWidth * screenWidth / 1024.0f ) / 2);
			break;

		case EAlignLeft:
			mX = (int)(( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ));
			break;

		case EAlignRight:
			mX = (int)(screenWidth - ( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ) - mWidth - 40 * mScaleX);
			break;
		}
	}
	else
		mX = ( int )( ( float )mPosX * mScaleX );

	mY = ( int )( ( float )mPosY * mScaleY );
}


bool MenuItemSlider::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{

#ifdef TRANSGAMING
    if (!mUnderMouse)
        return false;


    switch (eventData1){
        case EMouseKeyMiddle:
        case BUTTON_LEFT:
            if (eventType == EMenuKeyDown || eventType == EMenuKeyDownRepeated){
                if (mCurrValue > mMinValue){
                    GEngine->Video->PMenu->PlayMenuSound( "menu/menu/scroller-move" );
                    mCurrValue--;
                    if (!mAction.IsNull())
                        GScript.DoString( mAction.AsChar() );
                }
                else
                    GEngine->Video->PMenu->StopSound();
            }

            break;

        case EMouseKeyLeft:
        case BUTTON_RIGHT:
            if (eventType == EMenuKeyDown || eventType == EMenuKeyDownRepeated){
                if (mCurrValue < mMaxValue){
                    GEngine->Video->PMenu->PlayMenuSound( "menu/menu/scroller-move" );
                    mCurrValue++;
                    if (!mAction.IsNull())
                        GScript.DoString( mAction.AsChar() );
                }
                else
                    GEngine->Video->PMenu->StopSound();
            }

            break;
    }
#else
	int item = CheckMouse();

    if( item == MenuItemSlider::ENone && eventType != EMenuKeyUp )
		return false;

	if( eventData1 != EMouseKeyLeft )
		return false;

	switch( eventType )
	{
	case EMenuKeyDown:
	case EMenuKeyDownRepeated:
		mActiveCtrl = item;
		break;

	case EMenuKeyUp:
		if( item == MenuItemSlider::ELeft )
		{
			if( mCurrValue > mMinValue )
			{
				GEngine->Video->PMenu->PlayMenuSound( "menu/menu/scroller-move" );
				mCurrValue--;
				if( !mAction.IsNull() )
					GScript.DoString( mAction.AsChar() );
			}
			else
				GEngine->Video->PMenu->StopSound();
		}
		else if( item == MenuItemSlider::ERight )
		{
			if( mCurrValue < mMaxValue )
			{
				GEngine->Video->PMenu->PlayMenuSound( "menu/menu/scroller-move" );
				mCurrValue++;
				if( !mAction.IsNull() )
					GScript.DoString( mAction.AsChar() );
			}
			else
				GEngine->Video->PMenu->StopSound();
		}
		mActiveCtrl = MenuItemSlider::ENone;
		break;

	default:
		break;
	}
#endif

	return false;
}


int MenuItemSlider::CheckMouse()
{
	if( !mUnderMouse || mDisabled )
		return MenuItemSlider::ENone;

	POINT mousePos = GEngine->InputSys->GetMousePosition();

	if( mousePos.x > mLeftPos && mousePos.x < ( mLeftPos + mLeft->OriginalWidth * mScaleX ) )
	{
		mDragX = 0;
		mDragY = 0;
		return MenuItemSlider::ELeft;
	}
	if( mousePos.x > mRightPos && mousePos.x < ( mRightPos + mRight->OriginalWidth * mScaleX ) )
	{
		mDragX = 0;
		mDragY = 0;
		return MenuItemSlider::ERight;
	}
	if( mousePos.x > ( mDragPos + DRAG_SPACE * mScaleX ) && mousePos.x < ( mDragPos + ( DRAG_WIDTH + DRAG_SPACE ) * mScaleX ) )
	{
		mDragX = mousePos.x - mDragPos;
		mDragY = mousePos.y - mPosY;
		return MenuItemSlider::EDrag;
	}
	else if( mousePos.x > ( mLeftPos + mLeft->OriginalWidth * mScaleX ) && mousePos.x < mRightPos )
	{
		mDragPos = (int)(mousePos.x - ( ( mDrag->OriginalWidth - DRAG_SPACE ) / 2 ) * mScaleX);
		mDragX = mousePos.x - mDragPos;
		mDragY = mousePos.y - mPosY;
		return MenuItemSlider::EDrag;
	}

	return MenuItemSlider::ENone;
}


//--------------------------------------------------------------------
//	MenuItemSliderImage class
//--------------------------------------------------------------------
MenuItemSliderImage::MenuItemSliderImage() : MenuItemSlider()
{
	mDrawValue = false;
	mType = EMenuItemSliderImage;
}


MenuItemSliderImage::~MenuItemSliderImage()
{
	if( GEngine->Video->MaterialSys )
	{
		if( mLeft )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mLeft );
			mLeft = NULL;
		}
		if( mRight )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mRight );
			mRight = NULL;
		}
		if( mDrag )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mDrag );
			mDrag = NULL;
		}

		for( int i=0; i<mImages.Size(); i++ )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mImages[i] );
		}
		mImages.ClearArray();
	}
}


void MenuItemSliderImage::Render()
{
	if( !mVisible )
		return;

	MenuItemSlider::Render();

	int img = GetValue();
	if( img <= mImages.Size() )
	{
		Texture *tex = mImages[img-1];
		int valueWidth = GEngine->Video->Hud->GetTextWidth( "9.99" );
		int rightPos = (int)(mRightPos + mRight->OriginalWidth * mScaleX + ( valueWidth - tex->OriginalWidth * mScaleX ) / 2);

		BYTE alpha = 0xFF;
		BYTE r = 0xFF, g = 0xFF, b = 0xFF;
		MenuItemSlider *itemTrans = ( MenuItemSlider *)GEngine->Video->PMenu->Screen.FindItem( "CrosshairTrans" );
		MenuItemSlider *itemR = ( MenuItemSlider *)GEngine->Video->PMenu->Screen.FindItem( "CrosshairR" );
		MenuItemSlider *itemG = ( MenuItemSlider *)GEngine->Video->PMenu->Screen.FindItem( "CrosshairG" );
		MenuItemSlider *itemB = ( MenuItemSlider *)GEngine->Video->PMenu->Screen.FindItem( "CrosshairB" );
		if( itemTrans )
			alpha = ( BYTE )( 255.0f * itemTrans->GetValue() / 100.0f );
		if( itemR ) r = itemR->GetValue();
		if( itemG ) g = itemG->GetValue();
		if( itemB ) b = itemB->GetValue();
		DWORD color = ( alpha << 24 ) | ( r << 16 ) | ( g << 8 ) | b;

		GEngine->Video->Hud->DrawQuad( tex, rightPos, mPosY * mScaleY - ( tex->OriginalHeight * mScaleY - mHeight * mScaleY ) / 2, tex->OriginalWidth * mScaleX, tex->OriginalHeight * mScaleY, color );
	}
}


void MenuItemSliderImage::CalcSize()
{
	MenuItemSlider::CalcSize();

//	int img = GetValue();
//	if( img < mImages.Size() )
//	{
//		mWidth += mImages[img]->Width + 15;
//	}
}


void MenuItemSliderImage::CalcPosition()
{
	MenuItemSlider::CalcPosition();
}


void MenuItemSliderImage::AddImage( const char *name )
{
	Texture *tex = GEngine->Video->MaterialSys->CreateTexture( name, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mImages.Add( tex );
}
