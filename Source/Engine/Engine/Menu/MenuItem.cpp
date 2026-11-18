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
//	MenuItem class
//--------------------------------------------------------------------
MenuItem::MenuItem()
{
	mPosX = -1;
	mPosY = -1;
	mUnderMouse = false;
	mAlign = EAlignNone;
	mDescX = -1;
	mDescY = -1;

#ifdef TRANSGAMING
	mBigFont = "timebd";
	mSmallFont = "timebd";
#else
	mBigFont = "painfont";
	mSmallFont = "painfont";
#endif
	mBigSize = 40;
	mSmallSize = 20;
	mBigFontTex = "";
	mSmallFontTex = "";

	mScaleX = 1.0f;
	mScaleY = 1.0f;

	mExitStart = -1;
	mExitEnd = -1;

	mTextColor = 0;
	mDisabledColor = 0;
	mTextColorUnderMouse = 0;
	mDescColor = 0;

	mBG[0] = NULL; mBG[1] = NULL; mBG[2] = NULL;
	mUseBackground = false;

	mDisabled = false;
	mVisible = true;
	mDrawShadow = false;
	mTransparency = 0xFF;

	mType = EMenuItemTextButton;

	mConfigOption = "";

	mSndLightOn = "";
	mSndAccept = "";
	mSndPlayLightOn = false;
	mSndPlayAccept = false;

	mAction = "";
	mApplyRequired = false;
#ifdef TRANSGAMING
    for (int i = 0; i < NAV_TOTALACTIONS; i++)
        mNavActions[i] = "";
#endif

	mWarning = false;
	mWarningTrans = 0xff;
	mWarningTransDir = 1;
	mWarningStart = 0.0f;

	mWidth = 0;
	mHeight = 0;
}


MenuItem::~MenuItem()
{
	mName = "";
	mText = "";
	mDesc = "";
	mAction = "";
	mBigFont = "";
	mSmallFont = "";
	for( int i=0; i<3; i++ )
	{
		if( mBG[i] && GEngine->Video->MaterialSys )
			GEngine->Video->MaterialSys->ReleaseTexture( mBG[i] );
		mBG[i] = NULL;
	}
}


void MenuItem::Render()
{
	if( !mVisible )
		return;

	DrawBackground();

	// Shadow
	if( mDrawShadow )
		DrawAt( mX + 1, mY + 1, CalcTransColor( 0x50000000 ) );

	// Normal
	DWORD color = 0;
	if( !mDisabled )
		color = CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor );
	else
		color = CalcTransColor( mDisabledColor );

	DrawAt( mX, mY, color );
}


void MenuItem::DrawAt( int x, int y, DWORD color )
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
	GEngine->Video->Hud->Print( x, y, mText.AsChar(), color );
}


void MenuItem::DrawBackground()
{
	if( !mUseBackground || !mBG[0] || !mBG[1] || !mBG[2] )
		return;

	if( mCenterRepeat == 0 )
		SetBGWidth( 400 );

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
	int fontHeight = GEngine->Video->Hud->GetTextHeight();

	int x = (int)(mBGRect[0].left * mScaleX);
	int y = (int)(mBGRect[0].top * mScaleY);
	int w = (int)(mBGRect[0].right * mScaleX);
	int h = (int)(mBGRect[0].bottom * mScaleY + fontHeight);
	GEngine->Video->Hud->DrawQuad( mBG[0], x, y, w, h );

	for( int i=0; i<mCenterRepeat; i++ )
	{
		x = (int)(mBGRect[1].left * mScaleX);
		y = (int)(mBGRect[1].top * mScaleY);
		w = (int)(mBGRect[1].right * mScaleX);
		x += w * i;
		h = (int)(mBGRect[1].bottom * mScaleY + fontHeight);
		GEngine->Video->Hud->DrawQuad( mBG[1], x, y, w, h );
	}

	x = (int)(mBGRect[2].left * mScaleX);
	y = (int)(mBGRect[2].top * mScaleY);
	w = (int)(mBGRect[2].right * mScaleX);
	h = (int)(mBGRect[2].bottom * mScaleY + fontHeight);
	GEngine->Video->Hud->DrawQuad( mBG[2], x, y, w, h );
}


void MenuItem::RenderDesc()
{
	if( mUnderMouse && !mDesc.IsNull() && mTransparency == 0xFF )
	{
		GEngine->Video->Hud->SetFont( mSmallFont.AsChar(), mSmallFontTex.AsChar(), mSmallSize );
		int x = 0;
		if( mDescX == -1 )
			x = ( GEngine->Video->GetActiveView()->LocalViewport.Width - GEngine->Video->Hud->GetTextWidth( mDesc.AsChar() ) ) / 2 ;
		else
			x = (int)(mDescX * mScaleX);
		int y = 0;
		if( mDescY == -1 )
			y = ( int )( 700.0f * mScaleY );
		else
			y = (int)(mDescY * mScaleY);

		if( !mWarning )
		{
			GEngine->Video->Hud->Print( x + 1, y + 1, mDesc.AsChar(), 0xFF000000 );
			GEngine->Video->Hud->Print( x, y, mDesc.AsChar(), mDescColor );
		}
		else
		{
			float now = GEngine->SysDriver.GetCurrentTime();
			int trans = ( int )( ( now - mWarningStart ) * 255.0f );
			if( trans < 128 )
			{
				trans = 255 - trans;
			}
			if( trans > 255 )
			{
				mWarningStart = now;
				trans = 255;
			}
			DWORD color = ( trans << 24 );
			GEngine->Video->Hud->Print( x + 1, y + 1, mDesc.AsChar(), color );
			color = ( trans << 24 ) | ( mDescColor & 0x00FFFFFF );
			GEngine->Video->Hud->Print( x, y, mDesc.AsChar(), color );
		}
	}
}


void MenuItem::SetBackground( const char *bg )
{
    const int   BUFSIZE = 64;
    char        names[3][BUFSIZE];


	/*String names[3];
	names[0] = String( "HUD/blachy_menu/" ) + bg + String( "_lewa" );
	names[1] = String( "HUD/blachy_menu/" ) + bg + String( "_centrum" );
	names[2] = String( "HUD/blachy_menu/" ) + bg + String( "_prawa" );//*/

    snprintf(names[0], BUFSIZE, "HUD/blachy_menu/%s_lewa", bg);
    snprintf(names[1], BUFSIZE, "HUD/blachy_menu/%s_centrum", bg);
    snprintf(names[2], BUFSIZE, "HUD/blachy_menu/%s_prawa", bg);
    //TRACE("MenuItem::SetBackground(): loading background images '%s', '%s', and '%s' for the menu item '%s' (text = '%s')\n", names[0], names[1], names[2], mName.AsChar(), mText.AsChar());

	for( int i=0; i<3; i++ )
	{
		if( mBG[i] )
			GEngine->Video->MaterialSys->ReleaseTexture( mBG[i] );
	
		mBG[i] = GEngine->Video->MaterialSys->CreateTexture( names[i], Texture::ETexNoMipMaps | Texture::ETexNoLOD );
		if( !mBG[i] )
		{
			mUseBackground = false;
			return;
		}
	}

	mUseBackground = true;

	SetBGWidth( 400 );
}


void MenuItem::SetBGWidth( int w )
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
	int fontHeight = GEngine->Video->Hud->GetTextHeight();

	float scaleY = 1.0f / mScaleY;
	float fontReal = fontHeight * scaleY;

	// Center
	mBGRect[1].left = ( 1024 - w ) / 2;
	mBGRect[1].right = w;
	mBGRect[1].top = mPosY - 23;
	mBGRect[1].bottom = 42;

	float bgScale = ( fontReal + mBGRect[1].bottom ) / mBG[0]->OriginalHeight;
	int bgWidth = (int)(mBG[0]->OriginalWidth * bgScale);

	int centerWidth = (int)(mBG[1]->OriginalWidth * bgScale * 0.8f);
	mCenterRepeat = (int)(ceil( ( float )w / ( float )centerWidth ));

	mBGRect[1].right = centerWidth;

	// Left
	mBGRect[0].left = mBGRect[1].left - bgWidth + 1;
	mBGRect[0].right = bgWidth;
	mBGRect[0].top = mBGRect[1].top;
	mBGRect[0].bottom = mBGRect[1].bottom;

	// Right
	mBGRect[2].left = mBGRect[1].left + w - 1;
	mBGRect[2].right = bgWidth;
	mBGRect[2].top = mBGRect[1].top;
	mBGRect[2].bottom = mBGRect[1].bottom;
}


#ifdef TRANSGAMING
void MenuItem::SetName( const char *name ){
	mName = name;
}


void MenuItem::SetText( const char *text ){
	mText = text;
}


void MenuItem::SetDesc( const char *desc ){
	mDesc = HUD_PrepareString( desc );
}
#else
void MenuItem::SetName( String name )
{
	mName = name;
}


void MenuItem::SetText( String text )
{
	mText = text;
}


void MenuItem::SetDesc( String desc )
{
	mDesc = HUD_PrepareString( desc );
}
#endif


void MenuItem::SetPosition( int x, int y )
{
	mPosX = x;
	mPosY = y;
	UpdateScale();
	CalcPosition();
}


void MenuItem::SetBigFont( const char *name, int size )
{
	mBigFont = name;
	mBigSize = size;
}


void MenuItem::SetSmallFont( const char *name, int size )
{
	mSmallFont = name;
	mSmallSize = size;
}

bool MenuItem::CheckIfUnderMouse()
{
	if( !mVisible )
		return false;

	POINT mousePos = GEngine->InputSys->GetMousePosition();

	if( mousePos.x < mX || mousePos.y < mY || mousePos.x > ( mX + mWidth ) || mousePos.y > ( mY + mHeight ) )
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

	if( !mUnderMouse )
	{
		mWarningStart = 0.0f;
	}
	else if( mWarningStart == 0.0f )
	{
		mWarningStart = GEngine->SysDriver.GetCurrentTime();
		mWarningTrans = 0xff;
		mWarningTransDir = 1;
	}

	return mUnderMouse;
}


void MenuItem::PlayExitMovie( int start, int end )
{
	mExitStart = start;
	mExitEnd = end;
}


void MenuItem::SetColors( DWORD text, DWORD disabled, DWORD underMouse, DWORD desc )
{
	mTextColor = text;
	mDisabledColor = disabled;
	mTextColorUnderMouse = underMouse;
	mDescColor = desc;
}


void MenuItem::CalcSize()
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	mWidth = GEngine->Video->Hud->GetTextWidth( mText.AsChar() );
	mHeight = GEngine->Video->Hud->GetTextHeight( mText.AsChar() );
}


void MenuItem::CalcPosition()
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
			mX = (int)(( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ));
			break;

		case EAlignRight:
			mX = (int)(screenWidth - ( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ) - mWidth);
			break;
		}
	}
	else
	{
		if( mAlign == EAlignRight )
		{
			GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
			mX = ( int )( ( float )mPosX * mScaleX ) - GEngine->Video->Hud->GetTextWidth( mText.AsChar() );
		}
		else if( mAlign == EAlignCenter )
		{
			GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
			mX = ( int )( ( float )mPosX * mScaleX ) - GEngine->Video->Hud->GetTextWidth( mText.AsChar() ) / 2;
		}
		else
			mX = ( int )( ( float )mPosX * mScaleX );
	}

	mY = ( int )( ( float )mPosY * mScaleY );
}


DWORD MenuItem::CalcTransColor( DWORD color )
{
	DWORD trans = color;

	if( mTransparency != 0xFF )
	{
		BYTE *c = ( BYTE *)&trans;
		float scale = mTransparency / 255.0f;
#ifdef TRANSGAMING
		c[3] = (BYTE)(c[3] * scale);
#else
		c[3] *= scale;
#endif
	}

	return trans;
}


void MenuItem::UpdateScale()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;

	mScaleX = screenWidth / 1024.0f;
	mScaleY = screenHeight / 768.0f;
}

#ifdef TRANSGAMING
void MenuItem::GetCenter(int *x, int *y) const{
    *x = (mX + (mWidth >> 1));
    *y = (mY + (mHeight >> 1));
}

void MenuItem::SetNavAction(NavAction action, const char *cmd){
    if (cmd == NULL)
        return;


    if (action > NAV_NONE && action < NAV_TOTALACTIONS)
        mNavActions[action] = cmd;
}

const char *MenuItem::GetNavAction(NavAction action) const{
    if (action == NAV_NONE || action == NAV_TOTALACTIONS)
        return NULL;


    return mNavActions[action].AsChar();
}

bool MenuItem::HasNavAction(NavAction action) const{
    if (action == NAV_NONE || action == NAV_TOTALACTIONS)
        return false;


    return !mNavActions[action].IsNull();
}

#endif


bool MenuItem::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
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

		if( mUnderMouse && !mAction.IsNull() && !mDisabled && mIsPressed )
		{
			if( PlayExitMovie() )
			{
				GEngine->Video->PMenu->Screen.PlayExitMovie( mExitStart, mExitEnd, mAction.AsChar() );
			}
			else
			{
				GEngine->Video->PMenu->PlayMenuSound( mSndAccept.AsChar() );
				GEngine->Video->PMenu->Screen.SetItemAction( mAction.AsChar() );
				if( mApplyRequired )
					GScript.DoString( "PainMenu:EnableApplyButton()" );
				mIsPressed = false;
				return true;
			}
		}
		else if( mUnderMouse && !mDisabled && mIsPressed )
		{
			if( !mSndAccept.IsNull() )
				GEngine->Video->PMenu->PlayMenuSound( mSndAccept.AsChar() );
		}
		mIsPressed = false;
		break;

	default:
		break;
	}

	return false;
}


void MenuItemStaticText::SetRect( RECT *rect )
{
	if( rect == NULL )
	{
		mFitInRect = false;
		return;
	}

	mRect.left = rect->left;
	mRect.right = rect->right;
	mRect.top = rect->top;
	mRect.bottom = rect->bottom;

	mFitInRect = true;
}


void MenuItemStaticText::DrawAt( int x, int y, DWORD color )
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	if( !mFitInRect )
		GEngine->Video->Hud->Print( x, y, mText.AsChar(), color );
	else
		GEngine->Video->Hud->Print( mRect, mText.AsChar(), color );
}
