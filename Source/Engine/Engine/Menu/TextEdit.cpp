#include "Engine.h"
#include "Render/HUD.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Engine/PainMenu.h"

//--------------------------------------------------------------------
//	MenuItemTextEdit class
//--------------------------------------------------------------------
MenuItemTextEdit::MenuItemTextEdit() : MenuItem()
{
	mEditMode = false;
	mEditText = "";
	mMaxLength = 0;
	mCursorPos = 0;
	mCurrColor = 0;
	mPrevColor = 0;
	mUnderCursorColor = 0;
	mType = EMenuItemTextEdit;
}


MenuItemTextEdit::~MenuItemTextEdit()
{
}


void MenuItemTextEdit::Render()
{
	if( !mVisible )
		return;

	MenuItem::Render();

	FindUnderCursorColor();

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	// Shadow
	if( mDrawShadow )
		GEngine->Video->Hud->Print( mX + 1, mY + 1, mText.AsChar(), CalcTransColor( 0x50000000 ) );

	DWORD color = 0;
	DWORD colorEdit = 0;
	DWORD colorCursor = CalcTransColor( mTextColor );
	if( !mDisabled && !mEditMode )
	{
		color = CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor );
		colorEdit = color;
	}
	else if( mDisabled )
	{
		color = CalcTransColor( mDisabledColor );
		colorEdit = color;
	}
	else
	{
		color = CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor );
		colorEdit = CalcTransColor( mDisabledColor );
	}

	// Normal
	GEngine->Video->Hud->Print( mX, mY, mText.AsChar(), color );

	int textEditPos = mX + GEngine->Video->Hud->GetTextWidth( mText.AsChar() ) + GEngine->Video->Hud->GetTextWidth( " " );

	if( mEditText.Len() > 0 && mEditText[mEditText.Len() - 1] == '#' )
	{
		mEditText = mEditText.SubStr( mEditText.Len() - 1, 0 );
	}

	// Shadow
	if( mDrawShadow )
	{
		GEngine->Video->Hud->Print( textEditPos + 1, mY + 1, mEditText.AsChar(), CalcTransColor( 0x50000000 ) );
	}

	// Normal
//	if( mUnderMouse )
//		GEngine->Video->Hud->UseMultiColor( false );
	if( mName == "PlayerName" )
		GEngine->Video->Hud->Print( textEditPos, mY, mEditText.AsChar(), CalcTransColor( mTextColor ) );
	else
	{
		GEngine->Video->Hud->UseMultiColor( false );
		GEngine->Video->Hud->Print( textEditPos, mY, mEditText.AsChar(), colorEdit );
		GEngine->Video->Hud->UseMultiColor( true );
	}
//	if( mUnderMouse )
//		GEngine->Video->Hud->UseMultiColor( true );

	// Draw cursor
	if( mEditMode )
	{
/*		char ccc[255];
		sprintf( ccc, "cursorPos: %d (%c), currColor %d, prevColor %d, underCursor %d\n", mCursorPos, mEditText[mCursorPos], mCurrColor, mPrevColor, mUnderCursorColor );
		GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), 22 );
		GEngine->Video->Hud->UseMultiColor( false );
		GEngine->Video->Hud->Print( 20, 400, ccc );
		GEngine->Video->Hud->UseMultiColor( true );
*/
		String sub = mEditText.SubStr( mCursorPos );
		char *toCur = ( char *)sub.AsChar();

//		GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

		GEngine->Video->Hud->Print( textEditPos + GEngine->Video->Hud->GetTextWidth( toCur ) + 1, mY + 1, "_", CalcTransColor( 0x50000000 ) );
		GEngine->Video->Hud->Print( textEditPos + GEngine->Video->Hud->GetTextWidth( toCur ), mY, "_", colorCursor );
	}
}


void MenuItemTextEdit::CalcSize()
{
	MenuItem::CalcSize();

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	if( mType != EMenuItemTextEdit )
		mWidth += GEngine->Video->Hud->GetTextWidth( mEditText.AsChar() );
	else
		mWidth = GEngine->Video->Hud->GetTextWidth( "W" ) * mMaxLength;
	mWidth += GEngine->Video->Hud->GetTextWidth( " _" );
}


bool MenuItemTextEdit::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	if( mDisabled )
		return false;

	MenuItemColorPicker *pick = ( MenuItemColorPicker *)GEngine->Video->PMenu->Screen.FindItem( "ColorPicker" );
	MenuItemColorPicker *pick2 = ( MenuItemColorPicker *)GEngine->Video->PMenu->Screen.FindItem( "CharPicker" );
	MenuItem *passOK = GEngine->Video->PMenu->Screen.FindItem( "OKButton" );
	MenuItem *back = GEngine->Video->PMenu->Screen.FindItem( "BackButton" );

	if( eventType == EMenuKeyUp && eventData1 == EMouseKeyLeft )
	{
		if( mUnderMouse && !mEditMode )
		{
			GEngine->Video->PMenu->Screen.SetActiveTextEdit( true );
			GEngine->Video->PMenu->Screen.AcceptActiveTextEdit();
			mEditMode = true;
			mCursorPos = mEditText.Len();
		}
		else
		{
			if( !pick && !pick2 && !passOK && !back )
			{
				if( mEditMode )
					GEngine->Video->PMenu->Screen.SetActiveTextEdit( false );
				mEditMode = false;
			}
			else if( ( pick && !pick->CheckIfUnderMouse() ) && ( pick2 && !pick2->CheckIfUnderMouse() ) && ( passOK && !passOK->CheckIfUnderMouse() ) && ( back && !back->CheckIfUnderMouse() ) )
			{
				if( mEditMode )
					GEngine->Video->PMenu->Screen.SetActiveTextEdit( false );
				mEditMode = false;
			}
		}
		return false;
	}

	if( !mEditMode )
		return false;

	if( eventData1 == VK_F10 )
		return false;

	if( eventType != EMenuKeyDown && eventType != EMenuKeyDownRepeated )
		return mUnderMouse;

	if( ( ( pick && pick->CheckIfUnderMouse() ) || ( pick2 && pick2->CheckIfUnderMouse() ) || ( passOK && passOK->CheckIfUnderMouse() ) || ( back && back->CheckIfUnderMouse() ) ) && eventData1 == EMouseKeyLeft )
		return false;

	unsigned char c = ( unsigned char )eventData1;

	if( c == VK_HOME )
	{
		OnHome();
	}
	else if( c == VK_END )
	{
		OnEnd();
	}
	else if( c == VK_LEFT )
	{
		OnCursorLeft();
	}
	else if( c == VK_RIGHT )
	{
		OnCursorRight();
	}
	else if( c == VK_RETURN )
	{
		OnEnter();
	}
	else if( c == VK_BACK )
	{
		OnBackspace();
	}
	else if( c == VK_DELETE )
	{
		OnDelete();
	}
	else if( GEngine->InputSys->IsKeyPrintable( c ) )
	{
		OnPrintable( c );
	}
	else
	{
		return false;
	}

	return true;
}


void MenuItemTextEdit::OnHome()
{
	mCursorPos = 0;
}


void MenuItemTextEdit::OnEnd()
{
	mCursorPos = mEditText.Len();
}


void MenuItemTextEdit::OnCursorLeft()
{
	if( mCursorPos > 0 )
	{
		if( mCursorPos == 2 && mEditText[0] == '#' )
		{
		}
		else if( mCursorPos > 2 && mEditText[mCursorPos - 2] == '#' )
		{
			mCursorPos -= 3;
		}
		else
		{
			mCursorPos--;
		}

		if( mCursorPos < 0 )
			mCursorPos = 0;
	}
}


void MenuItemTextEdit::OnCursorRight()
{
	if( mCursorPos < mEditText.Len() )
	{
		if( mEditText[mCursorPos + 1] == '#' )
		{
			mCursorPos += 2;
		}
		mCursorPos++;
	}
}


void MenuItemTextEdit::OnEnter()
{
	GEngine->Video->PMenu->PlayMenuSound( "menu/menu/key-set" );
	if( mEditMode )
		GEngine->Video->PMenu->Screen.SetActiveTextEdit( false );
	mEditMode = false;
	if( !mConfigOption.IsNull() )
		GScript.DoString( "Cfg['%s']='%s'; Cfg:Save()", mConfigOption.AsChar(), GetEditText() );

	if( mName == "CDKey" )
	{
		if( mEditText.Len() == 19 )
		{
			bool ok = true;
			for( int i=0; i<19; i++ )
			{
				char c = mEditText[i];
				if( c != '-' && c > '9' && c < '0' )
				{
					ok = false;
					break;
				}
			}

			if( ok )
				GEngine->Registry.CDKey = mEditText;
		}
		mEditText = "****";
		OnEnd();
	}
}


void MenuItemTextEdit::OnBackspace()
{
	if( mEditText.Len() <= 0 || mCursorPos <= 0 )
		return;

	String left = "";
	String right = mEditText.SubStr( mEditText.Len() - mCursorPos, mCursorPos );

	if( mCursorPos > 2 && mEditText[mCursorPos - 3] == '#' )	// cursor after first colored letter
	{
		if( mCursorPos < mEditText.Len() && mEditText[mCursorPos] != ' ' && mEditText[mCursorPos] != 0 && mEditText[mCursorPos] != '#' ) // color still needed
		{
			left = mEditText.SubStr( mCursorPos - 1 );
			mCursorPos--;
		}
		else
		{
			left = mEditText.SubStr( mCursorPos - 3 );
			mCursorPos -= 3;
		}
	}
	else if( mCursorPos > 1 && mEditText[mCursorPos - 2] == '#' )	// cursor on first colored letter
	{
		right = mEditText.SubStr( mEditText.Len() - mCursorPos + 2, mCursorPos - 2 );
		if( mCursorPos > 4 && mEditText[mCursorPos - 5] == '#' )
		{
			left = mEditText.SubStr( mCursorPos - 5 );
			mCursorPos -= 3;
		}
		else if( mCursorPos > 2 )
		{
			left = mEditText.SubStr( mCursorPos - 3 );
			mCursorPos--;
		}
	}
	else
	{
		left = mEditText.SubStr( mCursorPos - 1 );
		mCursorPos--;
	}

	if( mCursorPos < 0 )
		mCursorPos = 0;

	mEditText = left + right;

	GLog.Print( "%s\n", mEditText.AsChar() );
}


void MenuItemTextEdit::OnDelete()
{
	if( mEditText.Len() <= 0 || mCursorPos >= mEditText.Len() )
		return;

	String left = mEditText.SubStr( mCursorPos );
	String right = mEditText.SubStr( mEditText.Len() - mCursorPos - 1, mCursorPos + 1 );

	if( right[0] == '#' )
	{
		if( left.Len() > 1 && left[left.Len() - 2] == '#' )
			left = left.SubStr( left.Len() - 2 );
	}

	if( left.Len() == 2 && left[0] == '#' && right.Len() == 0 )
	{
		left = "";
		mCursorPos = 0;
	}

	mEditText = left + right;

	GLog.Print( "%s\n", mEditText.AsChar() );
}


int MenuItemTextEdit::GetTextLength()
{
	char *str = ( char *)mEditText.AsChar();
	int len = 0;

	while( *str )
	{
		char c = *str++;
		if( c == '#' )
		{
			str++;
			continue;
		}

		len++;
	}

	return len;
}


void MenuItemTextEdit::OnPrintable( unsigned char c, bool key )
{
	if( mMaxLength != 0 && GetTextLength() >= mMaxLength )
		return;

	if( key )
		c = GEngine->InputSys->KeyToPrintable( c );

	if( c == '#' || c < 32 || c > 127 )
		return;

	String left = mEditText.SubStr( mCursorPos );
	String right = mEditText.SubStr( mEditText.Len() - mCursorPos, mCursorPos );
#ifdef TRANSGAMING
    const int   BUFSIZE = 1024;
	char        buf[BUFSIZE];
#else
	char buf[1024];
#endif

	if( left.Len() > 1 )
	{
		if( left[left.Len()-2] == '#' ) // color of first right letter at the end of left
		{
			// move it to the right string
			right = String( "  " ) + right;
			right[0] = '#';
			right[1] = left[left.Len()-1];
			left = left.SubStr( left.Len() - 2 );
		}
	}

	if( c == ' ' )
	{
		if( right[0] == ' ' || right[0] == '#' || right[0] == 0 || mName != "PlayerName" ) // no need to write color after space
		{
			mEditText = left + c + right;
			mCursorPos++;
		}
		else
		{
#ifdef TRANSGAMING
			snprintf( buf, BUFSIZE, "%s %c%x%s", left.AsChar(), '#', mUnderCursorColor, right.AsChar() );
#else
			sprintf( buf, "%s %c%x%s", left.AsChar(), '#', mUnderCursorColor, right.AsChar() );
#endif
            mEditText = String( buf );
			mCursorPos += 3;
		}
	}
	else
	{
		int leftEndColor = 0;
		int len = left.Len();
		while( len-- )
		{
			if( left[len] == '#' )
			{
				buf[0] = left[len+1];
				buf[1] = 0;
				sscanf( buf, "%x", &leftEndColor );
				break;
			}
		}

		int rightStartColor = leftEndColor;
		if( right[0] == '#' )
		{
			buf[0] = right[1];
			buf[1] = 0;
			sscanf( buf, "%x", &rightStartColor );
		}

		if( leftEndColor == mCurrColor )
		{
#ifdef TRANSGAMING
			snprintf( buf, BUFSIZE, "%s%c%s", left.AsChar(), c, right.AsChar() );
#else
			sprintf( buf, "%s%c%s", left.AsChar(), c, right.AsChar() );
#endif
            mCursorPos++;
		}
		else if( rightStartColor == mCurrColor )
		{
			right = right.SubStr( right.Len() - 2, 2 );
#ifdef TRANSGAMING
			snprintf( buf, BUFSIZE, "%s%c%x%c%s", left.AsChar(), '#', rightStartColor, c, right.AsChar() );
#else
			sprintf( buf, "%s%c%x%c%s", left.AsChar(), '#', rightStartColor, c, right.AsChar() );
#endif
            mCursorPos++;
		}
		else if( rightStartColor != leftEndColor || right[0] == 0 )
		{
#ifdef TRANSGAMING
			snprintf( buf, BUFSIZE, "%s%c%x%c%s", left.AsChar(), '#', mCurrColor, c, right.AsChar() );
#else
			sprintf( buf, "%s%c%x%c%s", left.AsChar(), '#', mCurrColor, c, right.AsChar() );
#endif
            mCursorPos += 3;
		}
		else
		{
#ifdef TRANSGAMING
			snprintf( buf, BUFSIZE, "%s%c%x%c%c%x%s", left.AsChar(), '#', mCurrColor, c, '#', rightStartColor, right.AsChar() );
#else
			sprintf( buf, "%s%c%x%c%c%x%s", left.AsChar(), '#', mCurrColor, c, '#', rightStartColor, right.AsChar() );
#endif
            mCursorPos += 5;
		}

		mEditText = String( buf );
	}

	GLog.Print( "%s\n", mEditText.AsChar() );
}


void MenuItemTextEdit::FindUnderCursorColor()
{
	mUnderCursorColor = 0;

	if( mType != EMenuItemTextEdit )
		return;

	int pos = mCursorPos;
	do {
		if( mEditText[pos] == '#' )
		{
			char buf[2];
			buf[0] = mEditText[pos+1];
			buf[1] = 0;
			sscanf( buf, "%x", &mUnderCursorColor );
			break;
		}
	} while( pos-- );
}


//--------------------------------------------------------------------
//	MenuItemPassword class
//--------------------------------------------------------------------
MenuItemPassword::MenuItemPassword() : MenuItemTextEdit()
{
	mType = EMenuItemPassword;
	mEnterAction = "";
}


MenuItemPassword::~MenuItemPassword()
{
}


void MenuItemPassword::Render()
{
	if( !mVisible )
		return;

	MenuItem::Render();

	GEngine->Video->Hud->SetFont( mBigFont.AsChar(), mBigFontTex.AsChar(), mBigSize );

	// Shadow
	if( mDrawShadow )
		GEngine->Video->Hud->Print( mX + 1, mY + 1, mText.AsChar(), CalcTransColor( 0x50000000 ) );

	DWORD color = 0;
	if( !mDisabled && !mEditMode )
		color = CalcTransColor( mUnderMouse ? mTextColorUnderMouse : mTextColor );
	else
		color = CalcTransColor( mDisabledColor );

	// Normal
	GEngine->Video->Hud->Print( mX, mY, mText.AsChar(), color );

	int textEditPos = mX + GEngine->Video->Hud->GetTextWidth( mText.AsChar() ) + GEngine->Video->Hud->GetTextWidth( " " );

	int len = mEditText.Len();
	char editText[255];
	memset( editText, 0, 255 );
	for( int i=0; i<len; i++ )
	{
		editText[i] = '*';
	}

	// Shadow
	if( mDrawShadow )
	{
		GEngine->Video->Hud->Print( textEditPos + 1, mY + 1, editText, CalcTransColor( 0x50000000 ) );
		if( mEditMode )
			GEngine->Video->Hud->Print( mX + ( mWidth - GEngine->Video->Hud->GetTextWidth( "_" ) ) + 1, mY + 1, "_", CalcTransColor( 0x50000000 ) );
	}

	// Normal
	GEngine->Video->Hud->Print( textEditPos, mY, editText, color );
	if( mEditMode )
		GEngine->Video->Hud->Print( mX + ( mWidth - GEngine->Video->Hud->GetTextWidth( "_" ) ), mY, "_", color );
}


//--------------------------------------------------------------------
//	MenuItemNumEdit class
//--------------------------------------------------------------------
MenuItemNumEdit::MenuItemNumEdit() : MenuItemTextEdit()
{
	mType = EMenuItemNumEdit;
}


MenuItemNumEdit::~MenuItemNumEdit()
{
}


bool MenuItemNumEdit::SendEvent( MenuEvent eventType, int eventData1, void *eventData2 )
{
	MenuItemColorPicker *pick = ( MenuItemColorPicker *)GEngine->Video->PMenu->Screen.FindItem( "ColorPicker" );
	MenuItemColorPicker *pick2 = ( MenuItemColorPicker *)GEngine->Video->PMenu->Screen.FindItem( "CharPicker" );
	MenuItem *passOK = GEngine->Video->PMenu->Screen.FindItem( "OKButton" );
	MenuItem *back = GEngine->Video->PMenu->Screen.FindItem( "BackButton" );

	if( eventType == EMenuKeyUp && eventData1 == EMouseKeyLeft )
	{
		if( mUnderMouse && !mEditMode )
		{
			GEngine->Video->PMenu->Screen.SetActiveTextEdit( true );
			GEngine->Video->PMenu->Screen.AcceptActiveTextEdit();
			mEditMode = true;
			mCursorPos = mEditText.Len();
		}
		else
		{
			if( !pick && !pick2 && !passOK && !back )
			{
				if( mEditMode )
					GEngine->Video->PMenu->Screen.SetActiveTextEdit( false );
				mEditMode = false;
			}
			else if( ( pick && !pick->CheckIfUnderMouse() ) && ( pick2 && !pick2->CheckIfUnderMouse() ) && ( passOK && !passOK->CheckIfUnderMouse() ) && ( back && !back->CheckIfUnderMouse() ) )
			{
				if( mEditMode )
					GEngine->Video->PMenu->Screen.SetActiveTextEdit( false );
				mEditMode = false;
			}
		}
		return false;
	}

	if( !mEditMode )
		return false;

	if( eventData1 == VK_F10 )
		return false;

	if( eventType != EMenuKeyDown && eventType != EMenuKeyDownRepeated )
		return mUnderMouse;

	if( ( ( pick && pick->CheckIfUnderMouse() ) || ( pick2 && pick2->CheckIfUnderMouse() ) || ( passOK && passOK->CheckIfUnderMouse() ) || ( back && back->CheckIfUnderMouse() ) ) && eventData1 == EMouseKeyLeft )
		return false;

	unsigned char c = ( unsigned char )eventData1;

	if( c == VK_HOME )
	{
		OnHome();
	}
	else if( c == VK_END )
	{
		OnEnd();
	}
	else if( c == VK_LEFT )
	{
		OnCursorLeft();
	}
	else if( c == VK_RIGHT )
	{
		OnCursorRight();
	}
	else if( c == VK_RETURN )
	{
		if( mEditText.IsNull() )
			mEditText = "0";
		OnEnter();
	}
	else if( c == VK_BACK )
	{
		OnBackspace();
	}
	else if( c == VK_DELETE )
	{
		OnDelete();
	}
	else if( GEngine->InputSys->IsKeyPrintable( c ) )
	{
		if( c >= '0' && c <= '9' )
			OnPrintable( c );
	}
	else
	{
		return false;
	}

	return true;
}
