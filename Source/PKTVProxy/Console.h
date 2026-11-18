#ifndef __CURSCONSOLE_H__
#define __CURSCONSOLE_H__

#include "Engine.h"
#include "LeString.h"

#define CON_MAX_MESSAGES	4

typedef struct
{
	String	message;
	float	time;
	DWORD	color;
} ConMessage;

class CursConsole
{
public:

	CursConsole();
	~CursConsole();

	bool Init();
	void Release();

	void Draw();
	bool Update();

	void SetPrefix( String prefix ) { mPrefix = prefix; }

	void AddMessage( String msg, DWORD color = 0xffffffff );
	void CleanOldMessages();

	void AddToHistory( String cmd );
	void AddToBuffer( String str, DWORD color = 0xFFFFBA7A );

	void SetText( String text );
	String GetText() const { return mText; }
	int GetCursorPos() const { return mCursorPos; }

	void ExecLastCommand();

private:

	void OnPageUp();
	void OnPageDown();
	void OnCursorLeft();
	void OnCursorRight();
	void OnCursorUp();
	void OnCursorDown();
	void OnEnter();
	void OnBackspace();
	void OnDelete();
	void OnPrintable( char c );

	String	mText;
	String	mVisText;
	int		mVisStart;
	String	mPrefix;

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

	bool	mChanged;
};


#endif //__CURSCONSOLE_H__
