#include "Engine.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Render/HUD.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Engine/PainMenu.h"

//--------------------------------------------------------------------
//	MenuItemTextButtonEx class
//--------------------------------------------------------------------
MenuItemTextButtonEx::MenuItemTextButtonEx() : MenuItem()
{
	mType = EMenuItemTextButtonEx;
}


MenuItemTextButtonEx::~MenuItemTextButtonEx()
{
}


bool MenuItemTextButtonEx::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	switch( eventType )
	{
	case EMenuKeyDown:
//	case EMenuKeyDownRepeated:
		if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyRight )
			break;

		if( mUnderMouse && !mDisabled )
		{
			mIsPressed = true;
		}
		else
		{
			mIsPressed = false;
		}
		break;

	case EMenuKeyUp:
		if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyRight )
			break;
		if( mUnderMouse && !mDisabled && mIsPressed )
		{
			GEngine->Video->PMenu->PlayMenuSound( "menu/menu/option-click" );
			GScript.DoString( "PainMenu:SwapTextButtonEx('%s',%d)", mName.AsChar(), eventData1 == EMouseKeyLeft );
			if( mApplyRequired )
				GScript.DoString( "PainMenu:EnableApplyButton()" );
			if( mName != "GraphicsQuality" )
				GScript.DoString( "PainMenu:AfterControlChange(\'%s\')", mName.AsChar() );
		}
		break;

	default:
		break;
	}

	return false;
}


//--------------------------------------------------------------------
//	MenuItemNumRange class
//--------------------------------------------------------------------
MenuItemNumRange::MenuItemNumRange() : MenuItemTextButtonEx()
{
	mType = EMenuItemNumRange;
	mMinVal = 0;
	mMaxVal = -1;
	mCurrVal = 0;
}


MenuItemNumRange::~MenuItemNumRange()
{
}


void MenuItemNumRange::SetCurrValue( int curr )
{
	mCurrVal = curr;

	char value[255];
#ifdef TRANSGAMING
    snprintf( value, sizeof(value), "%d", mCurrVal );
#else
    sprintf( value, "%d", mCurrVal );
#endif

	ChangeValue( value );
}


bool MenuItemNumRange::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	switch( eventType )
	{
	case EMenuKeyDown:
//	case EMenuKeyDownRepeated:
		if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyRight )
			break;

		if( mUnderMouse && !mDisabled )
		{
			mIsPressed = true;
		}
		else
		{
			mIsPressed = false;
		}
		break;

	case EMenuKeyUp:
		if( mUnderMouse && !mDisabled && mIsPressed )
		{
			if( eventData1 == EMouseKeyLeft )
			{
				if( mMaxVal == -1 || mCurrVal < mMaxVal )
				{
					GEngine->Video->PMenu->PlayMenuSound( "menu/menu/option-click" );
					SetCurrValue( mCurrVal + 1 );
					if( mApplyRequired )
						GScript.DoString( "PainMenu:EnableApplyButton()" );
					if( mName != "GraphicsQuality" )
						GScript.DoString( "PainMenu:AfterControlChange(\'%s\')", mName.AsChar() );
				}
			}
			else if( eventData1 == EMouseKeyRight )
			{
				if( mCurrVal > mMinVal )
				{
					GEngine->Video->PMenu->PlayMenuSound( "menu/menu/option-click" );
					SetCurrValue( mCurrVal - 1 );
					if( mApplyRequired )
						GScript.DoString( "PainMenu:EnableApplyButton()" );
					if( mName != "GraphicsQuality" )
						GScript.DoString( "PainMenu:AfterControlChange(\'%s\')", mName.AsChar() );
				}
			}
		}
		break;

	default:
		break;
	}

	return false;
}


MenuItemImageButtonEx::MenuItemImageButtonEx()
{
	mType = EMenuItemImageButtonEx;
	mValue = 0;
}


MenuItemImageButtonEx::~MenuItemImageButtonEx()
{
	if( GEngine->Video->MaterialSys )
	{
		for( int i=0; i<mImages.Size(); i++ )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mImages[i] );
			mImages[i] = NULL;
		}
	}

	mImages.ClearArray();
}


void MenuItemImageButtonEx::Render()
{
	CalcSize();

	BYTE alpha = 0xFF;
	MenuItemSlider *item = ( MenuItemSlider *)GEngine->Video->PMenu->Screen.FindItem( "Crosshair" );
	if( item )
		alpha = ( BYTE )( 255.0f * item->GetValue() / 100.0f );
	DWORD color = ( alpha << 24 ) | 0x00FFFFFF;

	if( mImages.Size() > mValue )
		GEngine->Video->Hud->DrawQuad( mImages[mValue], mX - mImages[mValue]->OriginalWidth * mScaleX / 2, mY - mImages[mValue]->OriginalHeight * mScaleY / 2, mImages[mValue]->OriginalWidth * mScaleX, mImages[mValue]->OriginalHeight * mScaleY, color );
}


void MenuItemImageButtonEx::AddImage( const char *name ){
	if( name == NULL || name[0] == 0 ) 
        return;

    Texture *tex = GEngine->Video->MaterialSys->CreateTexture( name, Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	if( tex )
		mImages.Add( tex );
}

void MenuItemImageButtonEx::CalcSize()
{
	mWidth = (int)(32 * mScaleX);
	mHeight = (int)(32 * mScaleY);
}


bool MenuItemImageButtonEx::CheckIfUnderMouse()
{
	if( !mVisible || mImages.Size() <= mValue )
		return false;

	POINT mousePos = GEngine->InputSys->GetMousePosition();

	int x = (int)(mX - 32 * mScaleX / 2);
	int y = (int)(mY - 32 * mScaleY / 2);

	if( mousePos.x < x || mousePos.y < y || mousePos.x > ( x + mWidth ) || mousePos.y > ( y + mHeight ) )
		mUnderMouse = false;
	else
		mUnderMouse = true;

	if( mUnderMouse && !mSndPlayLightOn && !mDisabled && !mSndLightOn.IsNull() )
	{
		GEngine->Video->PMenu->PlayMenuSound( mSndLightOn.AsChar() );
		mSndPlayLightOn = true;
	}
	
	if( !mUnderMouse )
	{
		mSndPlayLightOn = false;
	}

	return mUnderMouse;
}


bool MenuItemImageButtonEx::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	switch( eventType )
	{
	case EMenuKeyDown:
//	case EMenuKeyDownRepeated:
#ifdef TRANSGAMING
		if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyMiddle )
#else
		if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyRight )
#endif
			break;

		if( mUnderMouse && !mDisabled )
		{
			mIsPressed = true;
		}
		else
		{
			mIsPressed = false;
		}
		break;

	case EMenuKeyUp:
#ifdef TRANSGAMING
		if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyMiddle )
#else
		if( eventData1 != EMouseKeyLeft && eventData1 != EMouseKeyRight )
#endif
            break;

		if( mUnderMouse && !mDisabled && mIsPressed )
		{
			if( eventData1 == EMouseKeyLeft )
			{
				if( mValue < mImages.Size() - 1 )
					mValue++;
				else
					mValue = 0;
			}
			else
			{
				if( mValue > 0 )
					mValue--;
				else
					mValue = mImages.Size() - 1;
			}
		}
		break;

	default:
		break;
	}

	return false;
}
