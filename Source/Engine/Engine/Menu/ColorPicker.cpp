#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"

#include "Engine/PainMenu.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;


#ifdef TRANSGAMING
DWORD gTextColors[] = { 0xff7abaff, 0xff61a1e6, 0xff4888cd, 0xff16569b, 0xffd1d1d1, 0xffb8b8b8, 0xff9f9f9f, 0xff6d6d6d,
						0xff1700d6, 0xff0000bd, 0xff0000a4, 0xff000072, 0xff3a486c, 0xff212f53, 0xff08163a, 0xff000008 };
#else
DWORD gTextColors[] = { 0xffffba7a, 0xffe6a161, 0xffcd8848, 0xff9b5616, 0xffd1d1d1, 0xffb8b8b8, 0xff9f9f9f, 0xff6d6d6d,
						0xffd60017, 0xffbd0000, 0xffa40000, 0xff720000, 0xff6c483a, 0xff532f21, 0xff3a1608, 0xff080000 };
#endif


MenuItemColorPicker::MenuItemColorPicker() : MenuItem()
{
	mType = EMenuItemColorPicker;
	mSelected = 0;
}


MenuItemColorPicker::~MenuItemColorPicker()
{
}


void MenuItemColorPicker::Render()
{
	if( !mVisible )
		return;

	HUD *hud = GEngine->Video->Hud;

#ifdef TRANSGAMING
	hud->DrawQuad( NULL, mX + 38 * mScaleX * ( mSelected % 16 ) - 3 * mScaleX, mY - 3 * mScaleY, 38 * mScaleX, 38 * mScaleY, 0xff8bcbff );
#else
	hud->DrawQuad( NULL, mX + 38 * mScaleX * ( mSelected % 16 ) - 3 * mScaleX, mY - 3 * mScaleY, 38 * mScaleX, 38 * mScaleY, 0xffffcb8b );
#endif

//	for( int j=0; j<2; j++ )
		for( int i=0; i<16; i++ )
			hud->DrawQuad( NULL, mX + 38 * mScaleX * i, mY, 32 * mScaleX, 32 * mScaleY, gTextColors[i] );
}


void MenuItemColorPicker::CalcSize()
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	mWidth = (int)(38 * 16 * mScaleX);
	mHeight = (int)(38 * mScaleY);
}


bool MenuItemColorPicker::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( eventData1 != EMouseKeyLeft || eventType != EMenuKeyDown || !mUnderMouse )
		return false;

	POINT mpos = GEngine->InputSys->GetMousePosition();

	mpos.x = ( int )( ( float )mpos.x / mScaleX );
//	mpos.y = ( int )( ( float )mpos.y / mScaleY );

	mpos.x -= mPosX;
//	mpos.y -= mPosY;

	int x = mpos.x / 38;
//	int y = mpos.y / 38;

	mSelected = x;

	MenuItemTextEdit *edit = ( MenuItemTextEdit *)GEngine->Video->PMenu->Screen.FindItem( "PlayerName" );
	if( edit )
	{
		edit->SetCurrColor( mSelected );
	}

	MenuItemCharPicker *pick = ( MenuItemCharPicker *)GEngine->Video->PMenu->Screen.FindItem( "CharPicker" );
	if( pick )
	{
		pick->SetColor( gTextColors[mSelected] );
	}

	return false;
}


//--------------------------------------------------------------------


MenuItemCharPicker::MenuItemCharPicker()
{
	mType = EMenuItemCharPicker;
	mSelected = 0;
	mColor = gTextColors[0];
}


MenuItemCharPicker::~MenuItemCharPicker()
{
}


void MenuItemCharPicker::Render()
{
	if( !mVisible )
		return;

	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );
	int cw = hud->GetTextWidth( "W" );

	for( int j=0; j<3; j++ )
	{
		for( int i=0; i<32; i++ )
		{
			char str[2];
			str[0] = ( j + 1 ) * 32 + i;
			str[1] = 0;
			if( str[0] != '#' )
				hud->Print( mX + cw * i + ( cw - hud->GetTextWidth( str ) ) / 2, mY + j * hud->GetTextHeight(), str, mColor );
		}
	}

//	char pos[64];
//	sprintf( pos, "%d", mSelected );
//	hud->Print( 100, 100, pos );
}


void MenuItemCharPicker::CalcSize()
{
	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	mWidth = 40 * GEngine->Video->Hud->GetTextWidth( "W" );
	mHeight = 3 * GEngine->Video->Hud->GetTextHeight();
}


bool MenuItemCharPicker::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( eventData1 != EMouseKeyLeft || eventType != EMenuKeyDown || !mUnderMouse )
		return false;

	POINT mpos = GEngine->InputSys->GetMousePosition();

	mpos.x -= mX;
	mpos.y -= mY;

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	int x = mpos.x / GEngine->Video->Hud->GetTextWidth( "W" );
	int y = mpos.y / GEngine->Video->Hud->GetTextHeight();

	MenuItemTextEdit *edit = ( MenuItemTextEdit *)GEngine->Video->PMenu->Screen.FindItem( "PlayerName" );

	if( x < 32 )
	{
		mSelected = ( y + 1 ) * 32 + x;

		if( edit && edit->IsInEditMode() && mSelected < 127 )
		{
			edit->OnPrintable( ( unsigned char )mSelected, false );
		}
	}
	else
	{
		int half = mHeight / 2;
		if( mpos.y > half )
		{
			// Enter
			if( edit && edit->IsInEditMode() )
			{
				edit->OnEnter();
			}
		}
		else
		{
			// Delete
			if( edit && edit->IsInEditMode() )
			{
				edit->OnBackspace();
			}
		}
	}

	return false;
}
