#include "Console.h"
#include "Engine/PCFSystem.h"
extern ENGINE_API	PCFSystem*	GEngine;
#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#undef MOUSE_MOVED
#undef getstr
#undef getch
#define PDC_STATIC_BUILD
#include <curses.h>


CursConsole::CursConsole()
{
	mText = "";
	mVisText = "";
	mVisStart = 0;
	mPrefix = ">";
	mCursorPos = 0;
	mHistorySize = 200;
	mHistoryPos = -1;
	mBufferSize = 100;
	mBufferPos = 1;
	mMessages.Clear();
	mHistory.Clear();
	mBuffer.Clear();
	mBufferColors.Clear();
	mLastCommand = "";
	mChanged = false;
}


CursConsole::~CursConsole()
{
}


bool CursConsole::Init()
{
	initscr();
	nodelay( stdscr, TRUE );
	keypad( stdscr, TRUE );
	noecho();
	if( has_colors() )
		start_color();

	return true;
}


void CursConsole::Release()
{
	endwin();

	for( int i=0; i<mMessages.Size(); i++ )
	{
		delete mMessages[i];
	}
	mMessages.Clear();

	mHistory.Clear();
	mBuffer.Clear();
	mBufferColors.Clear();

	mText = "";
	mVisText = "";
	mVisStart = 0;
	mPrefix = ">";
	mCursorPos = 0;
	mHistorySize = 200;
	mHistoryPos = -1;
	mBufferSize = 100;
	mBufferPos = 1;
	mLastCommand = "";
	mChanged = false;
}


void CursConsole::Draw()
{
	static float lastTime = 0.0f;

	float currTime = GEngine->SysDriver.GetCurrentTime();
	if( currTime - lastTime > 0.2f )
	{
		clear();
		mvprintw( LINES - 1, 0, ( char *)mPrefix.AsChar() );
		mvprintw( LINES - 1, 1, ( char *)mVisText.AsChar() );

		// Draw buffer
		if( mBuffer.Size() > 0 )
		{
			for( int i = mBuffer.Size() - mBufferPos, j = 2; i >= 0; i--, j++ )
			{
				int ypos = LINES - j;
				if( ypos < 0 )
					break;
				mvprintw( ypos, 0, ( char *)mBuffer[i].AsChar() );
			}
		}

		// Update cursor position
		move( LINES - 1, mCursorPos + 1 );
		refresh();

		lastTime = currTime;
	}
}


bool CursConsole::Update()
{
	int c = getch();

	if( c == '\n' )
	{
		OnEnter();
	}
	else if( c == KEY_DOWN )
	{
		OnCursorDown();
	}
	else if( c == KEY_UP )
	{
		OnCursorUp();
	}
	else if( c == KEY_LEFT )
	{
		OnCursorLeft();
	}
	else if( c == KEY_RIGHT )
	{
		OnCursorRight();
	}
	else if( c == KEY_NPAGE )
	{
		OnPageDown();
	}
	else if( c == KEY_PPAGE )
	{
		OnPageUp();
	}
	else if( c == KEY_BACKSPACE || c == 8 )
	{
		OnBackspace();
	}
	else if( c == KEY_DC )
	{
		OnDelete();
	}
	else if( c == 9 )
	{
		GScript.Call("Hud_OnConsoleTab",0,"%s",mText.AsChar());
	}
	else if( c < KEY_MIN && c != ERR )
	{
		OnPrintable( ( char )c );
	}

	if( mChanged )
	{
		int maxW = COLS - 1;
		if( mText.Len() <= maxW )
			mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );

		if( mVisText.Len() > maxW )
		{
			int len = mVisText.Len();
			while( len-- )
			{
				mVisText = mVisText.SubStr( len );
				if( mVisText.Len() <= maxW )
					break;
			}
		}
	}

	Draw();

	return true;
}


void CursConsole::AddMessage( String msg, DWORD color )
{
	CleanOldMessages();

	if( mMessages.Size() == CON_MAX_MESSAGES )
	{
		delete mMessages[0];
		mMessages.Remove( 0 );
	}

	if( strchr( msg.AsChar(), '\n' ) )
	{
		char buf[1024];
		strcpy( buf, msg.AsChar() );
		char *next = strchr( buf, '\n' );
		*next = 0; next++;
		AddMessage( buf, color );
		AddMessage( next, color );
		return;
	}

	if( msg.Len() > COLS - 1 )
	{
		int len = COLS - 1;
		AddMessage( msg.SubStr( len ), color );
		AddMessage( msg.SubStr( msg.Len() - len, len ), color );
		return;
	}

	ConMessage *cm = new ConMessage();
	cm->message = msg;
	cm->time = GEngine->SysDriver.GetCurrentTime();
	cm->color = color;
	mMessages.Add( cm );

	mChanged = true;
}


void CursConsole::CleanOldMessages()
{
	float now = GEngine->SysDriver.GetCurrentTime();
	int toRem = -1;
	for( int i=0; i<mMessages.Size(); i++ )
	{
		if( now - mMessages[i]->time >= 15.0f )
			toRem = i;
	}

	if( toRem > -1 )
	{
		delete mMessages[toRem];
		mMessages.Remove( toRem );
	}

	mChanged = true;
}


void CursConsole::AddToHistory( String cmd )
{
	if( strchr( cmd.AsChar(), '\n' ) )
	{
		char buf[1024];
		strcpy( buf, cmd.AsChar() );
		char *next = strchr( buf, '\n' );
		*next = 0; next++;
		AddToHistory( buf );
		AddToHistory( next );
		return;
	}

	if( mHistory.Size() > 0 )
	{
		if( mHistory[mHistory.Size() - 1].IsNull() )
			mHistory[mHistory.Size() - 1] = cmd;
		else
			mHistory.Add( cmd );
	}
	else
		mHistory.Add( cmd );

	if( mHistory.Size() > mHistorySize )
		mHistory.Remove( 0 );

	mChanged = true;
}


void CursConsole::AddToBuffer( String str, DWORD color )
{
	if (str.Len() < 1)
		return;

	int maxW = COLS - 1;

	if( str.Len() > maxW )
	{
		int len = maxW;
		AddToBuffer( str.SubStr( len ), color );
		AddToBuffer( str.SubStr( str.Len() - len, len ), color );
		return;
	}

	if( strchr( str.AsChar(), '\n' ) )
	{
		char buf[1024];
		strcpy( buf, str.AsChar() );
		char *next = strchr( buf, '\n' );
		*next = 0; next++;
		AddToBuffer( buf, color );
		AddToBuffer( next, color );
		return;
	}

	mBuffer.Add( str );
	mBufferColors.Add( color );
	if( mBuffer.Size() > mBufferSize )
	{
		mBuffer.Remove( 0 );
		mBufferColors.Remove( 0 );
	}

	mChanged = true;
}


void CursConsole::SetText( String text )
{
	mText = text;
	mCursorPos = mText.Len();

	int maxW = COLS - 1;

	if( mText.Len() <= maxW )
	{
		mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		return;
	}

	if( mVisText.Len() > maxW )
	{
		int len = mVisText.Len();
		while( len-- )
		{
			mVisText = mVisText.SubStr( len );
			if( mVisText.Len() <= maxW )
				break;
		}
	}

	mChanged = true;
}


void CursConsole::ExecLastCommand()
{
	if( !mLastCommand.IsNull() )
	{
		GScript.Call("Hud_OnConsoleCommand",0,"%s",mLastCommand.AsChar());
		mLastCommand = "";
	}
}


void CursConsole::OnPageUp()
{
	int pageHeight = LINES;

	if( mBuffer.Size() - pageHeight > mBufferPos + pageHeight )
		mBufferPos += pageHeight;
	else if( mBuffer.Size() > pageHeight )
		mBufferPos = mBuffer.Size() - pageHeight;

	mChanged = true;
}


void CursConsole::OnPageDown()
{
	int pageHeight = LINES;

	if( mBufferPos > pageHeight + 1 )
		mBufferPos -= pageHeight;
	else
		mBufferPos = 1;

	mChanged = true;
}


void CursConsole::OnCursorLeft()
{
	if( mCursorPos > 0 )
	{
		mCursorPos--;

		if( mCursorPos < mVisStart )
		{
			mVisStart--;
			mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		}
	}

	mChanged = true;
}


void CursConsole::OnCursorRight()
{
	int maxW = COLS - 1;

	if( mCursorPos < mText.Len() )
	{
		mCursorPos++;

		if( mCursorPos - mVisStart > mVisText.Len() && mText.Len() > maxW )
		{
			mVisStart++;
			mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		}
	}

	mChanged = true;
}


void CursConsole::OnCursorUp()
{
	int maxW = COLS - 1;

	if( mHistory.Size() > 0 )
	{
		if( mHistoryPos == -1 )
		{
			AddToHistory( mText );
			mHistoryPos = mHistory.Size() - 2;
		}
		else if( mHistoryPos == -2 )
		{
			mHistory.Remove( mHistory.Size() - 1 );
			AddToHistory( mText );
			mHistoryPos = mHistory.Size() - 2;
		}
		else if( mHistoryPos > 0 )
		{
			mHistoryPos--;
		}
		mText = mHistory[mHistoryPos];
		mCursorPos = mText.Len();
		mVisText = mText;

		if( mVisText.Len() > maxW )
		{
			int len = 1;
			while( 1 )
			{
				mVisText = mText.SubStr( len, mText.Len() - len );
				if( mVisText.Len() > maxW - 12 )
					break;
				len++;
			}
			mVisStart = mText.Len() - len;
		}
	}

	mChanged = true;
}


void CursConsole::OnCursorDown()
{
	int maxW = COLS - 1;

	if( mHistoryPos > -1 && mHistoryPos < mHistory.Size() - 1 )
	{
		mHistoryPos++;
	}

	if( mHistoryPos > -1 )
		mText = mHistory[mHistoryPos];
	else if( mHistoryPos > -2 )
		mText = "";
	if( mHistoryPos == mHistory.Size() - 1 )
		mHistoryPos = -2;

	mCursorPos = mText.Len();

	mVisText = mText;
	mVisStart = 0;

	if( mVisText.Len() > maxW )
	{
		int len = 1;
		while( 1 )
		{
			mVisText = mText.SubStr( len, mText.Len() - len );
			if( mVisText.Len() > maxW - 12 )
				break;
			len++;
		}
		mVisStart = mText.Len() - len;
	}

	mChanged = true;
}


void CursConsole::OnEnter()
{
	AddToBuffer( mText );

	if( !mText.IsNull() )
	{
		AddToHistory( mText );
		mHistoryPos = -1;
		mLastCommand = mText;
	}
	mText = "";
	mVisText = "";
	mCursorPos = 0;
	mVisStart = 0;
	mBufferPos = 1;

	mChanged = true;
}


void CursConsole::OnBackspace()
{
	int maxW = COLS - 1;

	if( mText.Len() > 0 && mCursorPos > 0 )
	{
		String left = mText.SubStr( mCursorPos - 1 );
		String right = mText.SubStr( mText.Len() - mCursorPos, mCursorPos );
		mText = left + right;
		mCursorPos--;

		if( mText.Len() > maxW )
		{
			if( mCursorPos >= mText.Len() ) // cursor at the end
			{
				mVisStart--;
				mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
			}
			else if( mVisStart < mCursorPos )
			{
				mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
			}
		}
		else
		{
			mVisStart = 0;
			mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		}
	}
	else
		mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );

	mChanged = true;
}


void CursConsole::OnDelete()
{
	int maxW = COLS - 1;

	if( mText.Len() > 0 && mCursorPos < mText.Len() )
	{
		String left = mText.SubStr( mCursorPos );
		String right = mText.SubStr( mText.Len() - mCursorPos - 1, mCursorPos + 1 );
		mText = left + right;

		if( mText.Len() > maxW )
		{
			if( mCursorPos >= mText.Len() ) // cursor at the end
			{
				mVisStart--;
				mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
			}
			else if( mVisStart < mCursorPos )
			{
				mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
			}
		}
		else
		{
			mVisStart = 0;
			mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		}
	}
	else
		mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );

	mChanged = true;
}


void CursConsole::OnPrintable( char c )
{
	int maxW = COLS - 1;

	if( c == '#' || c < 32 ) return;

	String left = mText.SubStr( mCursorPos );
	String right = mText.SubStr( mText.Len() - mCursorPos, mCursorPos );
	mText = left + c + right;
	mCursorPos++;

	if( mText.Len() > maxW )
	{
		if( mCursorPos >= mVisStart + mVisText.Len() ) // cursor at the end
		{
			mVisStart++;
			mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		}
		else
		{
			mVisText = mText.SubStr( mText.Len() - mVisStart - 1, mVisStart );
		}
	}

	mChanged = true;
}
