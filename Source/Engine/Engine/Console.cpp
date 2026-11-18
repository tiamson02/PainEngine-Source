#include "Engine.h"
#include "Render/HUD.h"
#include "Render/View.h"
#include "Engine/InputSystem.h"
#include "Render/Video.h"

#include "Engine/PainMenu.h"

#include "Core/FileSystem/LogBuffer.h"
extern ENGINE_API LogBuffer GLog;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Net/DemoRecording2.h"

#if defined(DEBUG) || defined(TRANSGAMING_ENABLE_CONSOLE)
# ifdef TRANSGAMING_PSP
#  include <iofilemgr.h>
#  include <kerror.h>
# elif defined TRANSGAMING_PS2
#  include <sifdev.h>
# else
#  include <stdio.h>
# endif
#endif


#ifdef TRANSGAMING
Console::ConMessage::ConMessage(){
    reset();
}

void Console::ConMessage::reset(){
    message[0] = 0;
    time = 0;
    color = 0xffffffff;    
}

void Console::ConMessage::set(const char *msg, float t, DWORD c){
    strncpy(message, msg, MAX_MSG_LENGTH);

    message[MAX_MSG_LENGTH - 1] = 0;

    time = t;
    color = c;
}

#endif

Console::Console()
{
#ifdef TRANSGAMING
    mMessageCount = 0;
    mFreeMessageCount = CON_MAX_MESSAGES;
    
    for (int i = 0; i < CON_MAX_MESSAGES; i++){
        mFreeMessages[i] =  &mMessagePool[i];
        mMessages[i] =      NULL;
    }


    SetMPMsgFont("timesbd", NULL, 20);
    mMsgColor = 0xFFFFFFFF;

#else
	mText = "";
	mVisText = "";
	mVisStart = 0;
	mPrefix = "";
	mFont = "courbd";
	mFontSize = 20;
	mCursorPos = 0;
	mActive = false;
	mX = 0;
	mY = 0;
	mWidth = 964;
	mHeight = 360;
	mHistorySize = 200;
	mHistoryPos = -1;
	mBufferSize = 100;
	mBufferPos = 1;
	mBorder = NULL;
	mMsgX = 0;
	mMsgY = 0;
	mMsgColor = 0xFFFFFFFF;
	mMsgFont = "courbd";
	mMsgFontTex = "";
	mMsgFontSize = 20;
	mLastCommand = "";
	mMode = Console::EModeFull;
#endif
}

#ifndef TRANSGAMING
bool Console::Init()
{
	SetFont( mFont, mFontSize );
	SetPosition( 30, 30 );
	SetSize( 964, 360 );
	SetPrefix( ">" );
/*
	AddMessage( "adfbvwe423ryukfsdgfyusdk1234 5678123" );
	AddMessage( "adfbvwer23yukfsdgfyusdk" );
	AddMessage( "adfbvweryukfsdg63423fyusdk" );
	AddMessage( "adfbvwer523yukfsdgfyusdk" );
	AddMessage( "adfbvfreweryukfsdgfyusdk" );
	AddMessage( "22222adfbvweryukfsdgfyusdk" );
*/
	return true;
}


void Console::Release()
{
	for( int i=0; i<mMessages.Size(); i++ )
	{
		delete mMessages[i];
	}
	mMessages.ClearArray();

	if( mBorder )
	{
		delete mBorder;
		mBorder = NULL;
	}

	mHistory.ClearArray();
	mBuffer.ClearArray();
	mBufferColors.ClearArray();
}

void Console::Activate( bool active, int mode )
{
    if( !mBorder ){
        m_setTag("Console");
		mBorder = new MenuItemBorder();
    }

	mMode = mode;

	if( active )
		Show();
	else
		Hide();

	mActive = active;
}
#endif

#ifdef TRANSGAMING

# if defined(DEBUG) || defined(TRANSGAMING_ENABLE_CONSOLE)
void Console::psuedoConsoleRead(float deltaTime){
    static const float  UPDATE_PERIOD = 5.0f;    
    static float        delay = UPDATE_PERIOD;
    const int           BUFSIZE = 4096;
    char                buf[BUFSIZE];
    int                 count;


    delay -= deltaTime;

    if (delay > 0)
        return;

    delay = UPDATE_PERIOD;

#ifdef TRANSGAMING_PSP
    SceUID fp = sceIoOpen("host0:console.in", SCE_O_RDONLY, 0);

    if (fp < 0)
        return;


    buf[0] = 0;
    count = sceIoRead(fp, buf, BUFSIZE);


    if (count > 0 && buf[0]){
        sceIoClose(fp);


        //make sure the string is terminated
        buf[count] = 0;

        //remove all trailing whitespace
        while (count >= 0 && strchr(" \n\r\t\0", buf[count]))
            count --;


        //nothing left in the string => fail
        if (count == 0)
            return;


        //remove any commented code from the stream
        for (int i = 0; buf[i]; i++){
            if (buf[i] == '-' && buf[i + 1] == '-'){
                char *tok = &buf[i + 2];


                while (tok[0] && tok[0] != '\n')
                    tok++;

                if (tok[0]){
                    memcpy(&buf[i], &tok[1], BUFSIZE - (&tok[1] - buf));

                    i--;
                }

                else{
                    buf[i] = 0;

                    break;
                }
            }
        }

        //attempt to execute the script
        TRACE("*** Console::psuedoConsoleRead(): executing the script command ['%s']\n", buf);
        GScript.DoString("%s", buf);
        TRACE("*** Console::psuedoConsoleRead(): script command complete\n");

        //truncate the file so another command can be entered
        fp = sceIoOpen("host0:console.in", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0644);
    }

    sceIoClose(fp);
#elif defined(TRANSGAMING_PS2)
    int fp = sceOpen("host0:console.in", SCE_RDONLY);

    if (fp < 0)
        return;


    buf[0] = 0;
    count = sceRead(fp, buf, BUFSIZE);


    if (count > 0 && buf[0]){
        sceClose(fp);

        //make sure the string is terminated
        buf[count] = 0;

        //truncate all whitespace after the string
        while (count >= 0 && strchr(" \n\r\t\0", buf[count]))
            count --;


        //nothing left in the string => fail
        if (count == 0)
            return;


        //attempt to execute the script
        TRACE("*** Console::psuedoConsoleRead(): executing the script command ['%s']\n", buf);
        GScript.DoString("%s", buf);
        TRACE("*** Console::psuedoConsoleRead(): script command complete\n");

        //truncate the file so another command can be entered
        fp = sceOpen("host0:console.in", SCE_WRONLY|SCE_CREAT|SCE_TRUNC);
    }

    sceClose(fp);
#else
    FILE *fp = fopen("host0:console.in", "rb");

    if (fp == NULL)
        return;

    buf[0] = 0;
    count = fread(buf, sizeof(char), BUFSIZE, fp);


    if (count > 0 && buf[0]){

        //make sure the string is terminated
        buf[count] = 0;

        //truncate all whitespace after the string
        while (count >= 0 && strchr(" \n\r\t\0", buf[count]))
            count --;


        //nothing left in the string => fail
        if (count == 0){
            fclose(fp);

            return;
        }


        //attempt to execute the script
        TRACE("*** Console::psuedoConsoleRead(): executing the script command ['%s']\n", buf);
        GScript.DoString("%s", buf);
        TRACE("*** Console::psuedoConsoleRead(): script command complete\n");

        //truncate the file so another command can be entered
        fclose(fp);
        fp = fopen("host0:console.in", "wb");
    }

    fclose(fp);
#endif


}
# endif

void Console::SetMPMsgFont( const char *name, const char *tex, int size){
    strncpy(mMsgFont,       name, 32);

    if (tex)
        strncpy(mMsgFontTex, tex, 32);
    else
        mMsgFontTex[0] = 0;

    
    mMsgFontSize = size;
}

#endif

void Console::Draw()
{
#ifdef TRANSGAMING
    const int   screenWidth =   GEngine->Video->GetActiveView()->LocalViewport.Width;
    const int   screenHeight =  GEngine->Video->GetActiveView()->LocalViewport.Height;
    const float scaleX =        screenWidth / 1024.0f;
    const float scaleY =        screenHeight / 768.0f;
    int x, y;

    HUD *hud = GEngine->Video->Hud;


    CleanOldMessages();


    if (mMessageCount > 0){
        x = (int)(mMsgX * scaleX);
        y = (int)(mMsgY * scaleY);

        hud->SetFont(mMsgFont, mMsgFontTex, mMsgFontSize);

        const int textHeight = hud->GetTextHeight();
        const int spacing = 0;
        const int indentX = (int)(x + 10 * scaleX);
        const int indentY = (int)(y + 10 * scaleY);


        for (int i = 0, j = 0; j < mMessageCount; i++, j++){
            DWORD color =  mMsgColor;


            if (mMessages[i]->color != 0xffffffff)
                color = mMessages[i]->color;

            //render the text drop shadow and the text itself
            hud->Print(indentX + 1, indentY + 1 + textHeight * (j + spacing),   mMessages[i]->message, 0xFF000000);
            hud->Print(indentX,     indentY + textHeight * (j + spacing),       mMessages[i]->message, color);
        }
    }
#else
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleX = screenWidth / 1024.0f;
	float scaleY = screenHeight / 768.0f;

	HUD *hud = GEngine->Video->Hud;

	CleanOldMessages();

	int x, y, w, h;

	if( mMessages.Size() > 0 && !mActive )
	{
		x = mMsgX * scaleX;
		y = mMsgY * scaleY;
		w = 300 * scaleX;
		h = mMessages.Size() * hud->GetTextHeight() + 20 * scaleY;

		hud->SetFont( mMsgFont.AsChar(), mMsgFontTex.AsChar(), mMsgFontSize );

		int offset = 0;
//		for( int i=0, j=mMessages.Size()-1; j>=0; i++, j-- )
		for( int i=0, j=0; j<mMessages.Size(); i++, j++ )
		{
			DWORD color = mMsgColor;
			if( mMessages[i]->color != 0xffffffff )
				color = mMessages[i]->color;
			hud->Print( x + 10 * scaleX + 1, y + 10 *scaleY + 1 + hud->GetTextHeight() * ( j + offset ), mMessages[i]->message.AsChar(), 0xFF000000 );
			hud->Print( x + 10 * scaleX, y + 10 *scaleY + hud->GetTextHeight() * ( j + offset ), mMessages[i]->message.AsChar(), color );
		}
	}


	if( !mActive )
		return;

	hud->SetFont( mFont.AsChar(), mFontSize );

	x = mX * scaleX;
	y = mY * scaleY;
	w = mWidth * scaleX;
	h = mHeight * scaleY;
//	hud->DrawQuad( NULL, x, y, w, h, 0x66000000 );

	if( mMode == Console::EModeFull )
		 h -= 20;
	else
		h -= 12;

	if( mBorder )
		mBorder->Render();

	x += 10 * scaleX;

	// Draw buffer
	if( mBuffer.Size() > 0 && mMode == Console::EModeFull )
	{
		for( int i = mBuffer.Size() - mBufferPos, j = 2; i >= 0; i--, j++ )
		{
			int ypos = y + h - j * hud->GetTextHeight() + 10;
			if( ypos < y )
				break;
			hud->Print( x, ypos, mBuffer[i].AsChar(), mBufferColors[i] );
		}
	}

	y += h - hud->GetTextHeight() + 10;

	int prefixLen = hud->GetTextWidth( mPrefix.AsChar() );

	hud->Print( x, y, mPrefix.AsChar(), 0xFFFFBA7A );
	hud->Print( x + prefixLen, y, mVisText.AsChar(), 0xFFFFBA7A );

	// Draw cursor
	String sub = mText.SubStr( mCursorPos );
	char *toCur = ( char *)sub.AsChar();
	toCur += mVisStart;
	hud->Print( x + prefixLen + hud->GetTextWidth( toCur ), y, "_", 0xFFFFBA7A );

//	static char hist_stat[1024];
//	sprintf( hist_stat, "mBuffer.Size(): %d, mBufferSize: %d, mBufferPos: %d", mBuffer.Size(), mBufferSize, mBufferPos );
//	hud->Print( 100, 100, hist_stat );
#endif

# if defined(TRANSGAMING_PS2) && defined(TRANSGAMING_SHOW_FPS)
	extern void VIEW_RenderStats(void);
	VIEW_RenderStats();
#endif
}

#ifndef TRANSGAMING
void Console::Show()
{
	mText = "";
	mVisText = "";
	mCursorPos = 0;
	mVisStart = 0;

	SetFont( mFont, mFontSize );
	SetPosition( 30, 30 );
	if( mMode == Console::EModeFull )
		SetSize( 964, 360 );
	else
		SetSize( 964, 42 );
	SetPrefix( ">" );

	GEngine->InputSys->SetActionStatus( 0 );
	GEngine->InputSys->SetUIActionStatus( 0 );
}


void Console::Hide()
{
}


void Console::SetFont( String name, int size )
{
	mFont = name;
	mFontSize = size;
}


bool Console::SendEvent( int eventType, int eventData1, void *eventData2 )
{
	if( !mActive )
		return false;

	if( eventType != EKeyDown && eventType != EKeyPressed )
		return false;

	HUD *hud = GEngine->Video->Hud;

	hud->SetFont( mFont.AsChar(), mFontSize );

	unsigned char c = ( unsigned char )eventData1;

	if( c == 0xDF )
	{
		Activate( false );
		return true;
	}
	else if( c == VK_PRIOR )
	{
		OnPageUp();
	}
	else if( c == VK_NEXT )
	{
		OnPageDown();
	}
	else if( c == VK_LEFT )
	{
		OnCursorLeft();
	}
	else if( c == VK_RIGHT )
	{
		OnCursorRight();
	}
	else if( c == VK_UP )
	{
		OnCursorUp();
	}
	else if( c == VK_DOWN )
	{
		OnCursorDown();
	}
	else if( c == VK_RETURN || c == 0xfc )
	{
		OnEnter();
		return true;
	}
	else if( c == VK_BACK )
	{
		OnBackspace();
	}
	else if( c == VK_DELETE )
	{
		OnDelete();
	}
	else if( c == VK_TAB )
	{
//		GScript.DoString( "Hud:OnConsoleTab([[%s]])", mText.AsChar() );
		GScript.Call("Hud_OnConsoleTab",0,"%s",mText.AsChar());
	}
	else if( c == 'V' && ( GEngine->InputSys->GetKeyStatus( VK_LCONTROL ) == EKeyPressed || GEngine->InputSys->GetKeyStatus( VK_LCONTROL ) == EKeyDown
			|| GEngine->InputSys->GetKeyStatus( VK_RCONTROL ) == EKeyPressed || GEngine->InputSys->GetKeyStatus( VK_RCONTROL ) == EKeyDown ) )
	{
		if( OpenClipboard( NULL ) )
		{
			HANDLE data = GetClipboardData( CF_TEXT );
			char *text = ( char *)GlobalLock( data );
			if( text )
			{
				char *endOfLine = strchr( text, '\n' );
				if( endOfLine ) *endOfLine = 0;
				endOfLine = strchr( text, '\r' );
				if( endOfLine ) *endOfLine = 0;
				mText += String( text );
				mVisText = mText;
				mCursorPos = mText.Len();
				GlobalUnlock( data );
				CloseClipboard();

				int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
				int maxW = mWidth * screenWidth / 1024.0f - 20;
				if( hud->GetTextWidth( mVisText.AsChar() ) > maxW )
				{
					int len = 1;
					while( 1 )
					{
						mVisText = mText.SubStr( len, mText.Len() - len );
						if( hud->GetTextWidth( mVisText.AsChar() ) > maxW - 12 )
							break;
						len++;
					}
					mVisStart = mText.Len() - len;
				}
			}
		}
	}
	else if( GEngine->InputSys->IsKeyPrintable( c ) )
	{
		OnPrintable( c );
	}
	else
	{
//		GScript.DoString( "Game:Print('Unknown character: 0x%x')", c );
		return true;
	}

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
//	int maxW = screenWidth - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;
	int maxW = mWidth * screenWidth / 1024.0f - 20;

	if( hud->GetTextWidth( mText.AsChar() ) <= maxW )
	{
		mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		return true;
	}

	if( hud->GetTextWidth( mVisText.AsChar() ) > maxW )
	{
		int len = mVisText.Len();
		while( len-- )
		{
			mVisText = mVisText.SubStr( len );
			if( hud->GetTextWidth( mVisText.AsChar() ) <= maxW )
				break;
		}
	}

	return true;
}


void Console::OnPageUp()
{
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleY = screenHeight / 768.0f;

	int pageHeight = ( ( mHeight * scaleY ) / GEngine->Video->Hud->GetTextHeight() ) / 2;

	if( mBuffer.Size() - pageHeight > mBufferPos + pageHeight )
		mBufferPos += pageHeight;
	else if( mBuffer.Size() > pageHeight )
		mBufferPos = mBuffer.Size() - pageHeight;
}


void Console::OnPageDown()
{
	int screenHeight = GEngine->Video->GetActiveView()->LocalViewport.Height;
	float scaleY = screenHeight / 768.0f;

	int pageHeight = ( ( mHeight * scaleY ) / GEngine->Video->Hud->GetTextHeight() ) / 2;

	if( mBufferPos > pageHeight + 1 )
		mBufferPos -= pageHeight;
	else
		mBufferPos = 1;
}


void Console::OnCursorLeft()
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
}


void Console::OnCursorRight()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
//	int maxW = screenWidth - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;
	int maxW = mWidth * screenWidth / 1024.0f - 20;

	if( mCursorPos < mText.Len() )
	{
		mCursorPos++;

		if( mCursorPos - mVisStart > mVisText.Len() && hud->GetTextWidth( mText.AsChar() ) > maxW )
		{
			mVisStart++;
			mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		}
	}
}


void Console::OnCursorUp()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
	int maxW = mWidth * screenWidth / 1024.0f - 20; // - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;

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

		if( hud->GetTextWidth( mVisText.AsChar() ) > maxW )
		{
			int len = 1;
			while( 1 )
			{
				mVisText = mText.SubStr( len, mText.Len() - len );
				if( hud->GetTextWidth( mVisText.AsChar() ) > maxW - 12 )
					break;
				len++;
			}
			mVisStart = mText.Len() - len;
		}
	}
}


void Console::OnCursorDown()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
//	int maxW = screenWidth - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;
	int maxW = mWidth * screenWidth / 1024.0f - 20;

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

	if( hud->GetTextWidth( mVisText.AsChar() ) > maxW )
	{
		int len = 1;
		while( 1 )
		{
			mVisText = mText.SubStr( len, mText.Len() - len );
			if( hud->GetTextWidth( mVisText.AsChar() ) > maxW - 12 )
				break;
			len++;
		}
		mVisStart = mText.Len() - len;
	}
}


void Console::OnEnter()
{
	AddToBuffer( mText );

	if( !mText.IsNull() )
	{
		AddToHistory( mText );
		mHistoryPos = -1;
		mLastCommand = mText;
//		if( mLastCommand[0] == '\\' )
//			mLastCommand = String( "\\" ) + mLastCommand;
	}
	mText = "";
	mVisText = "";
	mCursorPos = 0;
	mVisStart = 0;
	mBufferPos = 1;
}


void Console::OnBackspace()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
//	int maxW = screenWidth - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;
	int maxW = mWidth * screenWidth / 1024.0f - 20;

	if( mText.Len() > 0 && mCursorPos > 0 )
	{
		String left = mText.SubStr( mCursorPos - 1 );
		String right = mText.SubStr( mText.Len() - mCursorPos, mCursorPos );
		mText = left + right;
		mCursorPos--;

		if( hud->GetTextWidth( mText.AsChar() ) > maxW )
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
}


void Console::OnDelete()
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
//	int maxW = screenWidth - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;
	int maxW = mWidth * screenWidth / 1024.0f - 20;

	if( mText.Len() > 0 && mCursorPos < mText.Len() )
	{
		String left = mText.SubStr( mCursorPos );
		String right = mText.SubStr( mText.Len() - mCursorPos - 1, mCursorPos + 1 );
		mText = left + right;

		if( hud->GetTextWidth( mText.AsChar() ) > maxW )
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
}


void Console::OnPrintable( unsigned char c )
{
	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
//	int maxW = screenWidth - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;
	int maxW = mWidth * screenWidth / 1024.0f - 20;

	if( hud->GetTextWidth( mText.AsChar() ) + 10 > maxW && mMode != Console::EModeFull )
		return;

	c = GEngine->InputSys->KeyToPrintable( c );

	if( c == '#' || c < 32 ) return;

	String left = mText.SubStr( mCursorPos );
	String right = mText.SubStr( mText.Len() - mCursorPos, mCursorPos );
	mText = left + c + right;
	mCursorPos++;

	if( hud->GetTextWidth( mText.AsChar() ) > maxW )
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
}


void Console::SetText( String text )
{
	mText = text;
	mCursorPos = mText.Len();

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
//	int maxW = screenWidth - 2 * mX * screenWidth / 1024.0f - hud->GetTextWidth( mPrefix.AsChar() ) - 16 * screenWidth / 1024.0f;
	int maxW = mWidth * screenWidth / 1024.0f - 20;

	if( hud->GetTextWidth( mText.AsChar() ) <= maxW )
	{
		mVisText = mText.SubStr( mText.Len() - mVisStart, mVisStart );
		return;
	}

	if( hud->GetTextWidth( mVisText.AsChar() ) > maxW )
	{
		int len = mVisText.Len();
		while( len-- )
		{
			mVisText = mVisText.SubStr( len );
			if( hud->GetTextWidth( mVisText.AsChar() ) <= maxW )
				break;
		}
	}
}
#endif

#ifdef TRANSGAMING
void Console::removeFirstMessage(){
    if (mMessageCount <= 0)
        return;


    Assert(mFreeMessageCount < CON_MAX_MESSAGES);

    //put the message back onto the free list
    mFreeMessages[mFreeMessageCount] = mMessages[0];
    mFreeMessageCount++;
    mMessageCount--;

    if (mMessageCount)
        memcpy(mMessages, &mMessages[1], sizeof(ConMessage *) * mMessageCount);
}
#endif

void Console::CleanOldMessages()
{
	float now = GEngine->SysDriver.GetCurrentTime();

#ifdef TRANSGAMING

    //the first message has expired => remove it from the list
    if (mMessageCount > 0 && now - mMessages[0]->time >= 15.0f)
        removeFirstMessage();
    
#else
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
#endif
}


#ifdef TRANSGAMING
void Console::AddMessage( const char *msg, DWORD color )
#else
void Console::AddMessage( String msg, DWORD color )
#endif
{
	CleanOldMessages();

#ifdef TRANSGAMING
    if (mMessageCount == CON_MAX_MESSAGES)
        removeFirstMessage();

    //check if the message is multi-line => split it up into multiple messages if it is
    if (strchr(msg, '\n')){
        char buf[ConMessage::MAX_MSG_LENGTH];
        char *next;


        strncpy(buf, msg, ConMessage::MAX_MSG_LENGTH);
        next = strchr(buf, '\n');
        next[0] = 0; 
        next++;

        //recursively add each line of the message
        AddMessage( buf, color );
        AddMessage( next, color );

        return;
    }


    ConMessage *cm = mFreeMessages[mFreeMessageCount - 1];
    
    
    mFreeMessages[mFreeMessageCount - 1] = NULL;
    mMessages[mMessageCount] = cm;
    mMessageCount++;
    mFreeMessageCount--;

    cm->set(msg, GEngine->SysDriver.GetCurrentTime(), color);

#else
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


	int screenWidth = mWidth * GEngine->Video->GetActiveView()->LocalViewport.Width / 1024.0f - 20;
	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );
	if( hud->GetTextWidth( msg.AsChar() ) > screenWidth )
	{
		int len = 0;
		while( 1 )
		{
			char c = msg[len];
			msg[len] = 0;
			int w = hud->GetTextWidth( msg.AsChar() );
			msg[len] = c;
			if( w >= screenWidth )
				break;
			len++;
		}
		len--;
		AddMessage( msg.SubStr( len ), color );
		AddMessage( msg.SubStr( msg.Len() - len, len ), color );
		return;
	}

    m_setTag("Console");
	ConMessage *cm = new ConMessage();
	cm->message = msg;
	cm->time = GEngine->SysDriver.GetCurrentTime();
	cm->color = color;
	mMessages.Add( cm );
    /// XXX WYWALIC
    /*FILE* f=fopen("console.log","at");
    if(f) {
        fprintf(f,"%s\n",msg.AsChar());
        fclose(f);
    }*/
#endif
}


#ifndef TRANSGAMING
void Console::AddToHistory( String cmd )
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
}


void Console::AddToBuffer( String str, DWORD color )
{
	if (str.Len() < 1)
		return;

	int screenWidth = GEngine->Video->GetActiveView()->LocalViewport.Width;
	float scaleX = screenWidth / 1024.0f;

	HUD *hud = GEngine->Video->Hud;
	hud->SetFont( mFont.AsChar(), mFontSize );

	int maxW = /*screenWidth - ( 2 * mX + 16 )*/mWidth * scaleX - 20;

	if( hud->GetTextWidth( str.AsChar() ) > maxW )
	{
		int len = 0;
		while( 1 )
		{
			char c = str[len];
			str[len] = 0;
			int w = hud->GetTextWidth( str.AsChar() );
			str[len] = c;
			if( w >= maxW )
				break;
			len++;
		}
		len--;
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
}


void Console::SetPosition( int x, int y )
{
	mX = x;
	mY = y;

	if( mBorder )
		mBorder->SetPosition( x, y );
}


void Console::SetSize( int w, int h )
{
	mWidth = w;
	mHeight = h;

	if( mBorder )
	{
		mBorder->SetSize( w, h );
	}
}


void Console::ExecLastCommand()
{
	if( !mLastCommand.IsNull() )
	{
		switch( mMode )
		{
		case Console::EModeSayAll:
			GScript.Call("Hud_OnSayToAll",0,"%s",mLastCommand.AsChar());
			break;

		case Console::EModeSayTeam:
			GScript.Call("Hud_OnSayToTeam",0,"%s,%d",mLastCommand.AsChar(),0xFF00FF00);
			break;

		default:
			GScript.Call("Hud_OnConsoleCommand",0,"%s",mLastCommand.AsChar());
//			GScript.DoString("Hud_OnConsoleCommand(\"%s\")", mLastCommand.AsChar());
			break;
		}
		mLastCommand = "";
	}
}
#endif

//--------------------------------------------------------------------
//	LUA Interface
//--------------------------------------------------------------------
static int luaConsole_AddMessage( lua_State *state )
{
    Script L( state );
#ifdef TRANSGAMING
    const char *text = L.GetString( 1 );
	DWORD       color = (DWORD)L.GetInt(2, 0xFF7ABAFF);


    if (!GEngine->Video)
        return 0;
 
# ifdef DEBUG
    TRACE("luaConsole_AddMessage(): CONSOLE MESSAGE = [%s]\n", text);
# endif
#else
    const char *text = L.GetString( 1 );
	DWORD color = ( DWORD )L.GetInt( 2, 0xFFFFBA7A );

    //GLog.Print( (text + "\n").AsChar() );

    if (!GEngine->Video) return 0;

	GEngine->Video->Hud->mConsole.AddToBuffer( text, color );
#endif

	GEngine->Video->Hud->mConsole.AddMessage( text, color );
	return 0;
}


static int luaConsole_Activate( lua_State *state )
{
#ifndef TRANSGAMING
	Script L( state );

	GEngine->Video->Hud->mConsole.Activate( L.GetBool( 1, true ), L.GetInt( 2, Console::EModeFull ) );
#endif
	return 0;
}


static int luaConsole_IsActive( lua_State *state )
{
	Script L( state );

#ifdef TRANSGAMING
    L.PushBool(false);
#else
	L.PushBool( GEngine->Video->Hud->mConsole.Active() );
#endif

	return 1;
}


static int luaConsole_Print( lua_State *state )
{
#ifndef TRANSGAMING
    if (!GEngine->Video) return 0;
	Script L( state );

	String text = L.GetString( 1 );

	GEngine->Video->Hud->mConsole.AddToBuffer( text );
	GEngine->Video->Hud->mConsole.AddMessage( text );
#endif
	return 0;
}


static int luaConsole_SetMPMsgColor( lua_State *state )
{
    if (!GEngine->Video) return 0;

	Script L( state );

	DWORD r = ( DWORD )L.GetInt( 1 );
	DWORD g = ( DWORD )L.GetInt( 2 );
	DWORD b = ( DWORD )L.GetInt( 3 );

#ifdef TRANSGAMING
	DWORD color = 0xFF000000 | ( b << 16 ) | ( g << 8 ) | r;
#else
	DWORD color = 0xFF000000 | ( r << 16 ) | ( g << 8 ) | b;
#endif

	GEngine->Video->Hud->mConsole.SetMPMsgColor( color );

	return 0;
}


static int luaConsole_SetMPMsgPosition( lua_State *state )
{
    if (!GEngine->Video) return 0;
	Script L( state );

	int x = L.GetInt( 1 );
	int y = L.GetInt( 2 );

	GEngine->Video->Hud->mConsole.SetMPMsgPos( x, y );

    return 0;
}


static int luaConsole_SetMPMsgFont( lua_State *state )
{
    if (!GEngine->Video) return 0;
	Script L( state );

#ifdef TRANSGAMING
	const char *name = L.GetString( 1 );
	const char *tex = L.GetString( 2 );
#else
	String name = L.GetString( 1 );
	String tex = L.GetString( 2 );
#endif
	int size = L.GetInt( 3 );

	GEngine->Video->Hud->mConsole.SetMPMsgFont( name, tex, size );
	return 0;
}


static int luaConsole_SetCurrentText( lua_State *state )
{
#ifndef TRANSGAMING
    if (!GEngine->Video) return 0;
	Script L( state );

	String text = L.GetString( 1 );

	GEngine->Video->Hud->mConsole.SetText( text );
#endif
	return 0;
}


static int luaConsole_GetCurrentText( lua_State *state )
{
#ifndef TRANSGAMING     //this function is never called from the scripts

    if (!GEngine->Video) return 0;
	Script L( state );

	L.PushString( GEngine->Video->Hud->mConsole.GetText() );

	return 1;
#else

    return 0;
#endif
}


static int luaConsole_GetCursorPos( lua_State *state )
{
#ifndef TRANSGAMING     //this function is never called from the scripts
    if (!GEngine->Video) return 0;
	Script L( state );

	L.PushInt( GEngine->Video->Hud->mConsole.GetCursorPos() );

    return 1;
#else
    return 0;
#endif
}


static int luaConsole_SetFont( lua_State *state )
{
#ifndef TRANSGAMING     //this function is never called from the scripts
    if (!GEngine->Video) return 0;
	Script L( state );

	String name = L.GetString( 1 );
	int size = L.GetInt( 2 );

	GEngine->Video->Hud->mConsole.SetFont( name, size );
#endif
	return 0;
}


#ifndef TRANSGAMING
static int luaDemo_Record( lua_State *state )
{
#ifndef TRANSGAMING
	Script L( state );
    if( !gDemoRec ){
        m_setTag("Console");

		gDemoRec = new DemoRecording2();
    }
	gDemoRec->StartRecording( L.GetString( 1 ) );
#endif
	return 0;
}


static int luaDemo_Stop( lua_State *state )
{
#ifndef TRANSGAMING
	Script L( state );
	if( gDemoRec )
		gDemoRec->Stop();
#endif
	return 0;
}


static int luaDemo_Play( lua_State *state )
{
#ifndef TRANSGAMING
	Script L( state );
    if( !gDemoRec ){
        m_setTag("Console");

        gDemoRec = new DemoRecording2();
    }

	if( gDemoRec )
	{
		String errorText;
		if( !gDemoRec->StartPlaying( L.GetString( 1 ), &errorText, L.GetBool( 2 ), L.GetBool( 3 ) ) )
		{
//			GScript.Call( "PainMenu_MultiplayerErrorCallback", 0, "%d,%s", MREC_Disconnected, errorText.AsChar() );
//			SwitchToState( PCFSystemStates::Default );
		}
# ifndef TRANSGAMING
		else if( GEngine->Video )
			GEngine->Video->Hud->mConsole.Activate( false );
# endif
	}
#endif
	return 0;
}


static int luaDemo_IsPlaying( lua_State *state )
{
    Script L( state );

#ifndef TRANSGAMING
	if( gDemoRec )
		L.PushBool( gDemoRec->IsPlaying() );
	else
#endif
		L.PushBool( false );
	return 1;
}


static int luaDemo_RecordPlayer( lua_State *state )
{
#ifndef TRANSGAMING
    Script L( state );

    if( gDemoRec )
		gDemoRec->RecordPlayer( L.GetInt( 1 ), L.GetInt( 2 ), L.GetInt( 3 ), L.GetInt( 4 ) );
#endif
	return 0;
}


static int luaDemo_GetLastFPS( lua_State *state )
{
	Script L( state );

#ifndef TRANSGAMING
	if( gDemoRec )
		L.PushFloat( gDemoRec->GetFPS() );
#else
    L.PushFloat(0);
#endif
    return 1;
}
#endif


static const luaL_reg p_console_lib[] = {
	{ "AddMessage", luaConsole_AddMessage },
	{ "Activate", luaConsole_Activate },
	{ "IsActive", luaConsole_IsActive },
	{ "Print", luaConsole_Print },
	{ "SetMPMsgColor", luaConsole_SetMPMsgColor },
	{ "SetMPMsgPosition", luaConsole_SetMPMsgPosition },
	{ "SetMPMsgFont", luaConsole_SetMPMsgFont },
	{ "SetCurrentText", luaConsole_SetCurrentText },
	{ "GetCurrentText", luaConsole_GetCurrentText },
	{ "GetCursorPos", luaConsole_GetCursorPos },
	{ "SetFont", luaConsole_SetFont },
#ifndef TRANSGAMING
    { "DemoRecord", luaDemo_Record },
	{ "DemoStop", luaDemo_Stop },
	{ "DemoPlay", luaDemo_Play },
	{ "DemoIsPlaying", luaDemo_IsPlaying },
	{ "DemoRecordPlayer", luaDemo_RecordPlayer },
	{ "DemoGetLastFPS", luaDemo_GetLastFPS },
#endif
	{ NULL, NULL }
};


void Console_RegisterLUALib()
{
	GScript.RegisterLibrary("CONSOLE",p_console_lib,false);
}
