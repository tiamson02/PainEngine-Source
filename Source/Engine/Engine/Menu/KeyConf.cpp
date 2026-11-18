#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"

#include "Engine/PainMenu.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;
#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;


static const char *gNoneStr = "None";


//--------------------------------------------------------------------
//	MenuItemKeyConf class
//--------------------------------------------------------------------
MenuItemKeyConf::MenuItemKeyConf() : MenuItemList()
{
	mType = EMenuItemKeyConf;
	mKeyToChange = MenuItemKeyConf::ENone;

	mKey = "None";
	mAlternate = "None";
	mKeyEng = "None";
	mAlternateEng = "None";

	ScriptObject txt = GScript.Globals()["TXT"];
	gNoneStr = txt["None"].GetString();

	mIndex = 0;
}


MenuItemKeyConf::~MenuItemKeyConf()
{
}


void MenuItemKeyConf::Render()
{
	if( !mVisible )
		return;

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	if( mY == -1 )
		return;

	int keyX = 0, altX = 0, nameX = 0;

	int partWidth = (int)(300 * mScaleX);

	nameX = (int)(mX + ( partWidth - GEngine->Video->Hud->GetTextWidth( mText.AsChar() ) ) / 2 - 20 * mScaleX);
	keyX = (int)(mX + partWidth + 20 * mScaleX + ( partWidth - GEngine->Video->Hud->GetTextWidth( mKey.AsChar() ) ) / 2);
	altX = (int)(mX + ( partWidth * 2 ) + ( partWidth - GEngine->Video->Hud->GetTextWidth( mAlternate.AsChar() ) ) / 2);

	const char *prim = mKey.AsChar();
	const char *alt = mAlternate.AsChar();

	if( !strcmp(prim, "None") )
		prim = gNoneStr;
	if( !strcmp(alt, "None") )
		alt = gNoneStr;

	if( mDrawShadow )
	{
		if( mIndex == 0 )
			GEngine->Video->Hud->Print( nameX + 1, mY + 1, mText.AsChar(), CalcTransColor( 0x50000000 ) );
		else
			GEngine->Video->Hud->Print( mX + 1, mY + 1, mText.AsChar(), CalcTransColor( 0x50000000 ) );
		GEngine->Video->Hud->Print( keyX + 1, mY + 1, prim, CalcTransColor( 0x50000000 ) );
		GEngine->Video->Hud->Print( altX + 1, mY + 1, alt, CalcTransColor( 0x50000000 ) );
	}

	if( !mDisabled )
	{
		if( mIndex == 0 )
			GEngine->Video->Hud->Print( nameX, mY, mText.AsChar(), CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor ) );
		else
			GEngine->Video->Hud->Print( mX, mY, mText.AsChar(), CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor ) );
		if( mKeyToChange == MenuItemKeyConf::EPrimary )
			GEngine->Video->Hud->Print( keyX, mY, prim, CalcTransColor( mDisabledColor ) );
		else
			GEngine->Video->Hud->Print( keyX, mY, prim, CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor ) );
		if( mKeyToChange == MenuItemKeyConf::EAlternative )
			GEngine->Video->Hud->Print( altX, mY, alt, CalcTransColor( mDisabledColor ) );
		else
			GEngine->Video->Hud->Print( altX, mY, alt, CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor ) );
	}
	else
	{
		if( mIndex == 0 )
			GEngine->Video->Hud->Print( nameX, mY, mText.AsChar(), CalcTransColor( mTextColor ) );
		else
			GEngine->Video->Hud->Print( mX, mY, mText.AsChar(), CalcTransColor( mTextColor ) );
		GEngine->Video->Hud->Print( keyX, mY, prim, CalcTransColor( mTextColor ) );
		GEngine->Video->Hud->Print( altX, mY, alt, CalcTransColor( mTextColor ) );
	}
}


void MenuItemKeyConf::CalcSize()
{
	MenuItem::CalcSize();

	mWidth = (int)(GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX);
	if( GEngine->Video->PMenu->Screen.GetScroller() )
	{
		mWidth -= GEngine->Video->PMenu->Screen.GetScroller()->GetWidth();
	}
}


void MenuItemKeyConf::CalcPosition()
{
	UpdateScale();

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;

	mX = (int)(( ( screenWidth - GEngine->Video->PMenu->Screen.GetMenuWidth() * mScaleX ) / 2 ));

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	float scrHeight = (int)(410 * mScaleY);
	float txtHeight = GEngine->Video->Hud->GetTextHeight();

	if( GEngine->Video->PMenu->Screen.GetScroller() )
		scrHeight = ( GEngine->Video->PMenu->Screen.GetScroller()->GetScrollerHeight() - 130 ) * mScaleY;

//	GLog.Print( "scrHeight: %f, txtHeight: %f, scaleY: %f, scroller: %d\n", scrHeight, txtHeight, mScaleY, GEngine->Video->PMenu->Screen.GetScroller()->GetScrollerHeight() );

	int count = ( int )ceil( scrHeight / txtHeight );
	if( count > 0 )
	{
		if( GEngine->Video->PMenu->Screen.GetFirstShown() <= mIndex && ( GEngine->Video->PMenu->Screen.GetFirstShown() + count > mIndex ) && mIndex > 0 )
		{
			mY = (int)(( GEngine->Video->PMenu->Screen.GetTopPos() + 16 ) * mScaleY + ( mIndex - GEngine->Video->PMenu->Screen.GetFirstShown() + 1 ) * mHeight);
		}
		else if( mIndex == 0 )
			mY = (int)(( GEngine->Video->PMenu->Screen.GetTopPos() - 10 ) * mScaleY);
		else
			mY = -1;
	}
	else
		mY = -1;
}


bool MenuItemKeyConf::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	InputSystem* inputSys = GEngine->InputSys;

	int where = CheckMouse();

#ifndef TRANSGAMING
	if( eventType != EMenuKeyUp && eventData1 != EMouseWheelForward && eventData1 != EMouseWheelBack )
		return false;
#endif

	if( mKeyToChange == MenuItemKeyConf::ENone && eventData1 != EMouseKeyLeft )
		return false;

	if( eventData1 == EMouseKeyLeft && where != mKeyToChange )
	{
		GEngine->Video->PMenu->PlayMenuSound( "menu/menu/key-select" );
		mKeyToChange = where;
		return false;
	}

	if( eventData1 == VK_ESCAPE )
	{
		mKeyToChange = MenuItemKeyConf::ENone;
		return true;
	}

	unsigned char c = ( unsigned char )eventData1;
	const char *keyText = inputSys->GetKeyName( c );
	const char *keyTextEng = inputSys->GetKeyNameEng( c );

	if( mKeyToChange == MenuItemKeyConf::EPrimary )
	{
		GScript.Call( "Cfg_ClearKeyBinding", 0, "%s", keyTextEng );
		GEngine->Video->PMenu->PlayMenuSound( "menu/menu/key-set" );
		GEngine->Video->PMenu->Screen.ClearKeyConfig( keyTextEng );
		SetKey( keyText, keyTextEng );
		mKeyToChange = MenuItemKeyConf::ENone;
		GEngine->InputSys->ClearUIBinding( c );
#ifndef TRANSGAMING
		GEngine->InputSys->ClearMessageBinding( c );
#endif
	}
	else if( mKeyToChange == MenuItemKeyConf::EAlternative )
	{
		GScript.Call( "Cfg_ClearKeyBinding", 0, "%s", keyTextEng );
		GEngine->Video->PMenu->PlayMenuSound( "menu/menu/key-set" );
		GEngine->Video->PMenu->Screen.ClearKeyConfig( keyTextEng );
		SetAlternate( keyText, keyTextEng );
		mKeyToChange = MenuItemKeyConf::ENone;
		GEngine->InputSys->ClearUIBinding( c );
#ifndef TRANSGAMING
		GEngine->InputSys->ClearMessageBinding( c );
#endif
	}

	return false;
}


int MenuItemKeyConf::CheckMouse()
{
	if( !mUnderMouse )
		return MenuItemKeyConf::ENone;

	int keyX = 0, altX = 0;

	int partWidth = (int)(300 * mScaleX);

	keyX = mX + partWidth;
	altX = mX + ( partWidth * 2 );

	POINT mousePos = GEngine->InputSys->GetMousePosition();

	if( mousePos.x > keyX && mousePos.x < altX )
		return MenuItemKeyConf::EPrimary;
	if( mousePos.x > altX && mousePos.x < ( altX + partWidth ) )
		return MenuItemKeyConf::EAlternative;

	return MenuItemSlider::ENone;
}


//--------------------------------------------------------------------
//	MenuItemKeyList class
//--------------------------------------------------------------------
MenuItemKeyList::MenuItemKeyList() : MenuItemList()
{
	mType = EMenuItemKeyList;
}


void MenuItemKeyList::DrawElem( MenuItemList::MenuListItem *elem, int x, int y, bool header )
{
}


//--------------------------------------------------------------------
//	MenuItemSimpleKeyConf
//--------------------------------------------------------------------
MenuItemSimpleKeyConf::MenuItemSimpleKeyConf() : MenuItem()
{
	mType = EMenuItemSimpleKeyConf;
	mChangeMode = false;
	mIndex = 0;
	mKey = "";
	mKeyEng = "";
}


MenuItemSimpleKeyConf::~MenuItemSimpleKeyConf()
{
}


void MenuItemSimpleKeyConf::Render()
{
	mText = mKey;

	static DWORD color = mTextColor;
	static DWORD colorUnder = mTextColorUnderMouse;

	if( mChangeMode )
	{
		mTextColor = mDisabledColor;
		mTextColorUnderMouse = mDisabledColor;
	}
	else
	{
		mTextColor = color;
		mTextColorUnderMouse = colorUnder;
	}

	MenuItem::Render();
}


bool MenuItemSimpleKeyConf::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( eventType != EMenuKeyUp )
		return false;

	static bool isSelected = false;

	if( mChangeMode )
	{
		if( eventData1 == EMouseKeyLeft || eventData1 == EMouseKeyRight
			|| eventData1 == EMouseKeyMiddle 
#ifndef TRANSGAMING
            || eventData1 == EMouseWheelForward
			|| eventData1 == EMouseWheelBack 
#endif
            )
		{
			return true;
		}

		if( eventData1 != VK_ESCAPE )
		{
			unsigned char c = ( unsigned char )eventData1;
			const char *keyText = GEngine->InputSys->GetKeyName( c );
			const char *keyTextEng = GEngine->InputSys->GetKeyNameEng( c );

			if( GEngine->Video->PMenu->Screen.IsKeyInUse( eventData1 ) )
				GEngine->Video->PMenu->Screen.ClearKey( eventData1 );

			SetKey( keyText, keyTextEng );
#ifndef TRANSGAMING
			if( mIndex > 0 )
				GEngine->InputSys->BindMessageKey( mIndex - 1, eventData1 );
#endif

			if( GEngine->InputSys->GetKeyAction( c ) != EInputNone )
			{
				GEngine->InputSys->BindKey( EInputNone, c );
			}

			GEngine->InputSys->ClearUIBinding( c );
			GScript.Call( "Cfg_ClearKeyBinding", 0, "%s", keyTextEng );
		}

		GEngine->Video->PMenu->PlayMenuSound( "menu/menu/key-set" );
		mChangeMode = false;
		isSelected = false;

		return true;
	}
	else if( !isSelected )
	{
		if( eventData1 == EMouseKeyLeft && mUnderMouse )
		{
			GEngine->Video->PMenu->PlayMenuSound( "menu/menu/key-select" );
			mChangeMode = true;
			isSelected = true;
			GEngine->Video->PMenu->Screen.AcceptActiveTextEdit();
			return false;
		}
	}

	return false;
}


void MenuItemSimpleKeyConf::SetKey( const char *key, const char *eng )
{
#ifdef TRANSGAMING
	if( key[0] != 0 && eng[0] != 0 )
#else
	if( key != "" && eng != "" )
#endif
	{
		mKey = key;
		mKeyEng = eng;
	}
	else
	{
		mKey = gNoneStr;
		mKeyEng = "None";
	}
}
