
// #pragma once
#ifndef _CONSOLE_
#define _CONSOLE_


#define CON_MAX_MESSAGES	4

#include "Core/LeString.h"


#ifndef TRANSGAMING
typedef struct
{
	String	message;
	float	time;
	DWORD	color;
} ConMessage;


class MenuItemBorder;
#endif

class Console
{
public:

#ifndef TRANSGAMING
	enum
	{
		EModeFull	 = 0,
		EModeSayAll  = 1,
		EModeSayTeam = 2,
	};
#endif

	Console();
	~Console() {
#ifndef TRANSGAMING
        Release();
#endif
    }

#ifndef TRANSGAMING
	bool Init();
    void Release();

	void Activate( bool active, int mode = EModeFull );
	bool Active() const { return mActive; }
#endif

	void Draw();

#ifndef TRANSGAMING
	void Show();
	void Hide();

	void SetFont( String name, int size = 0 );
	void SetPosition( int x, int y );
	void SetSize( int w, int h );
	void SetPrefix( String prefix ) { mPrefix = prefix; }

	bool SendEvent( int eventType, int eventData1, void *eventData2 = 0 );
	void AddMessage( String msg, DWORD color = 0xffffffff );
#else
	void AddMessage( const char *msg, DWORD color = 0xffffffff );
#endif

#ifndef TRANSGAMING
	void CleanOldMessages();

    void AddToHistory( String cmd );
	void AddToBuffer( String str, DWORD color = 0xFFFFBA7A );

	void SetText( String text );
	const char *GetText() const { return mText.AsChar(); }
	int GetCursorPos() const { return mCursorPos; }
#endif

	void SetMPMsgColor( DWORD color ) { mMsgColor = color; }
	void SetMPMsgPos( int x, int y )  { mMsgX = x; mMsgY = y; }
#ifdef TRANSGAMING
	void SetMPMsgFont( const char *name, const char *tex, int size );
# if defined(DEBUG) || defined(TRANSGAMING_ENABLE_CONSOLE)
    void psuedoConsoleRead(float timeDelta);
# endif
#else
	void SetMPMsgFont( String name, String tex, int size ) { mMsgFont = name; mMsgFontTex = tex; mMsgFontSize = size; }
#endif

#ifndef TRANSGAMING
	void ExecLastCommand();
#endif
private:

#ifndef TRANSGAMING
	void OnPageUp();
	void OnPageDown();
	void OnCursorLeft();
	void OnCursorRight();
	void OnCursorUp();
	void OnCursorDown();
	void OnEnter();
	void OnBackspace();
	void OnDelete();
	void OnPrintable( unsigned char c );

	bool	mActive;
	int		mMode;
	String	mFont;
	int		mFontSize;
	String	mText;
	String	mVisText;
	int		mVisStart;
	String	mPrefix;

	int		mMsgX;
	int		mMsgY;
	DWORD	mMsgColor;
	String	mMsgFont;
	String	mMsgFontTex;
	int		mMsgFontSize;

	int		mX;
	int		mY;
	int		mWidth;
	int		mHeight;
	int		mCursorPos;

	int		mHistorySize;
	int		mHistoryPos;

	int		mBufferSize;
	int		mBufferPos;

	DynamicArray<ConMessage *>	mMessages;
	DynamicArray<String>		mHistory;
	DynamicArray<String>		mBuffer;
	DynamicArray<DWORD>			mBufferColors;

	String	mLastCommand;

	MenuItemBorder	*mBorder;
#else
	void CleanOldMessages();
    void removeFirstMessage();

	/* ################# class ConMessage ################*/
    class ConMessage
	{
    public:
        static const int MAX_MSG_LENGTH = 128;

	    char    message[MAX_MSG_LENGTH];
	    float   time;
	    DWORD   color;

        ConMessage();

        void    reset();
        void    set(const char *msg, float t, DWORD c);
    };

    ConMessage  mMessagePool[CON_MAX_MESSAGES];
    ConMessage *mFreeMessages[CON_MAX_MESSAGES];
    ConMessage *mMessages[CON_MAX_MESSAGES];
    int         mMessageCount;
    int         mFreeMessageCount;

    int         mMsgX;
    int         mMsgY;

    char        mMsgFont[32];
	char        mMsgFontTex[32];
	DWORD       mMsgColor;
	int         mMsgFontSize;


#endif
};


void Console_RegisterLUALib();


#endif //_CONSOLE_

