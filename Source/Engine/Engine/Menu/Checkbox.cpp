#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"
#include "Render/Material.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Engine/PainMenu.h"

//--------------------------------------------------------------------
//	MenuItemCheckbox class
//--------------------------------------------------------------------
MenuItemCheckbox::MenuItemCheckbox() : MenuItem()
{
	Checked = false;
	mTexOn = NULL;
	mTexOff = NULL;

	SetTextureChecked( "HUD/ikonki/checkbox_zaznaczony" );
	SetTextureUnchecked( "HUD/ikonki/checkbox_pusty" );

	mType = EMenuItemCheckbox;
}


MenuItemCheckbox::~MenuItemCheckbox()
{
	if( GEngine->Video->MaterialSys )
	{
		if( mTexOn )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mTexOn );
			mTexOn = NULL;
		}
		if( mTexOff )
		{
			GEngine->Video->MaterialSys->ReleaseTexture( mTexOff );
			mTexOff = NULL;
		}
	}
}


void MenuItemCheckbox::Render()
{
	if( !mVisible )
		return;

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	// Checkbox
	Texture *tex = Checked ? mTexOn : mTexOff;

	int texWidth = (int)(tex->OriginalWidth * mScaleX);
	int texHeight = (int)(tex->OriginalHeight * mScaleY);

	if( mBigSize  < 11 )
	{
		texWidth /= 2;
		texHeight /= 2;
	}

	int xpos = mX;

	if( tex  && !(tex->Flags & Texture::ETexInvalid ) )
	{
		if( mAlign == EAlignRight )
			xpos = mX + mWidth - texWidth - 3;
		else
			xpos = mX;

		if( !mDisabled )
			GEngine->Video->Hud->DrawQuad( tex, xpos, mY, texWidth, texHeight );
		else
			GEngine->Video->Hud->DrawQuad( tex, xpos, mY, texWidth, texHeight, CalcTransColor( mDisabledColor ) );
	}

	int text_y = mY + ( texHeight - GEngine->Video->Hud->GetTextHeight() ) / 2;

	if( mAlign == EAlignRight )
		xpos = mX;
	else
		xpos = mX + texWidth + 3;

	// Shadow
	if( mDrawShadow )
		GEngine->Video->Hud->Print( xpos + 1, text_y + 1, mText.AsChar(), CalcTransColor( 0x50000000 ) );

	// Normal
	if( !mDisabled )
		GEngine->Video->Hud->Print( xpos, text_y, mText.AsChar(), CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor ) );
	else
		GEngine->Video->Hud->Print( xpos, text_y, mText.AsChar(), CalcTransColor( mDisabledColor ) );
}


void MenuItemCheckbox::CalcSize()
{
	MenuItem::CalcSize();

	Texture *tex = Checked ? mTexOn : mTexOff;
	int texWidth = (int)(tex->OriginalWidth * mScaleX);
	int texHeight = (int)(tex->OriginalHeight * mScaleY);

	if( mBigSize  < 11 )
	{
		texWidth /= 2;
		texHeight /= 2;
	}

	mWidth += texWidth + 4;
	mHeight = texHeight;
}


void MenuItemCheckbox::SetTextureChecked( const char *texName, DWORD color, DWORD texFlags )
{
	if( mTexOn )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexOn );
	mTexOn = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mColorOn = color;
}

void MenuItemCheckbox::SetTextureUnchecked( const char *texName, DWORD color, DWORD texFlags )
{
	if( mTexOff )
		GEngine->Video->MaterialSys->ReleaseTexture( mTexOff );
	mTexOff = GEngine->Video->MaterialSys->CreateTexture( texName, texFlags | Texture::ETexNoMipMaps | Texture::ETexNoLOD );
	mColorOff = color;
}


bool MenuItemCheckbox::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	switch( eventType )
	{
	case EMenuKeyDown:
//	case EMenuKeyDownRepeated:
		if( eventData1 != EMouseKeyLeft )
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
		if( eventData1 != EMouseKeyLeft )
			break;
//		if( mUnderMouse && mAction != "" && !mDisabled )
		if( mUnderMouse && !mDisabled && mIsPressed )
		{
			GEngine->Video->PMenu->PlayMenuSound( "menu/menu/checkbox-click" );
			GEngine->Video->PMenu->Screen.SetItemAction( mAction.AsChar() );
			Checked = !Checked;
			if( mApplyRequired )
				GScript.DoString( "PainMenu:EnableApplyButton()" );
			GScript.DoString( "PainMenu:AfterControlChange(\'%s\')", mName.AsChar() );
			return true;
		}
		break;

	default:
		break;
	}

	return false;
}


void MenuItemCheckbox::CalcPosition()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;

	if( mPosX == -1 )
	{
		switch( mAlign )
		{
		case EAlignNone:
			mX = ( screenWidth - mWidth ) / 2;
			break;

		case EAlignLeft:
			mX = (int)(( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ) - 10 * mScaleX);
			break;

		case EAlignRight:
			mX = screenWidth - (int)(( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ) - mWidth + 10 * mScaleX);
			break;
		}
	}
	else
		mX = ( int )( ( float )mPosX * mScaleX );

	mY = ( int )( ( float )mPosY * mScaleY );
}
