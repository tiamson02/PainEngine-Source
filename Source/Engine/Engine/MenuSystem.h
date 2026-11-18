// ---------------------------------------------------------------------------
// Copyright (c) 2002 People Can Fly. All Rights Reserved.
// 
// Not for public release.
// 
// Created: 2002/11 Krystian Galaj
//----------------------------------------------------------------------------
#error This file is obselete
class Sound2D;

enum MenuEvent
{
	// Events from input system - mouse keys are here too
	EMenuKeyDown			= 1,
	EMenuKeyUp				= 2,
	EMenuMouseUp			= 3,
	EMenuMouseDown			= 4,
	EMenuKeyDownRepeated	= 5,
	// Other events
	EMenuClicked			= 6,
	EMenuMouseLeft			= 7,
	EMenuMouseEntered		= 8,
};
	
class ENGINE_API MenuWindow
{
	friend class MenuSystem;
	friend class MenuScrollBar;
	friend static bool MenuEventHandlerGiveClickToParent( MenuWindow* window, int event, void* data );
public:

	enum {
		// Focus
		CanHaveFocus		= 1,
		HasFocus			= 1<<1,
		Modal				= 1<<2,
		// Visibility
		Shown				= 1<<3,
		// What to draw using HUD
		HasTexture			= 1<<4,
		HasRect				= 1<<5,
		HasText				= 1<<6,
	};

	enum {
		SimpleWindow		= 0,
		Button				= 1,
		ProgressBar			= 2,
		ScrollBar			= 3,
		EditField			= 4,
		ListControl			= 5,
		CheckBox			= 6,
		GameSpyControl		= 7,
		RedefineKeysControl	= 8,
	};

	enum {
		ETextNotJustified		= 0,
		ETextFromLeft			= 1,
		ETextFromRight			= 2,
		ETextCentered			= 3,
		ETextFromLeftOutside	= 4,
	};

	MenuWindow();
	virtual ~MenuWindow();

	// Get/set window identifying string
	const char *GetName( void ) { return mWindowName.AsChar(); };
	void SetName( const char * name ) { mWindowName = name; };

	int GetIdent( void ) { return mWindowIdent; };	// idents are set by MenuSystem, and can change during window lifetime

	// Main method used to position windows in windows ( and in 
	bool SetParent( int ident );
	bool SetParent( MenuWindow* window );
	bool SetParent( const char * windowName );

	void ToFront( const char *windowName );
	void ToFront( MenuWindow* window );
	void ToFront( int index );
	bool IsAtFront( void );

	void Show( void );
	void Hide( void );
	bool Visible( void ) { return ((Flags & Shown) != 0); };

	void MakeModal( bool modal );
	bool IsModal( void ) { return ( (Flags & Modal ) != 0 ); };

	// Position handling methods ( position inside parent window! Coords: 0.0 - 1.0 )
	void SetPos( float x, float y );
	void SetPos( Vector& pos ) { SetPos( pos.X, pos.Y ); };
	Vector GetPos( void ) { return Vector( X, Y , 0.0f ); };
	float GetX( void ) { return X; };
	float GetY( void ) { return Y; };

	void SetSize( float w, float h );
	void SetSize( Vector& size ) { SetSize( size.X, size.Y ); };
	Vector GetSize( void ) { return Vector( W, H, 0.0f ); };
	float GetW( void ) { return W; };
	float GetH( void ) { return H; };
	void SetPosAndSize( float x, float y, float w, float h ) { SetPos( x, y ); SetSize( w, h ); };

	void SetUV( float u1, float v1, float u2, float v2 ) { U1 = u1; V1 = v1; U2 = u2; V2 = v2; };
	void SetTextPos( float x, float y );
	void SetTextPos( Vector& pos ) { SetTextPos( pos.X, pos.Y ); };
	Vector GetTextPos( void ) { return Vector( mTextX, mTextY, 0.0f ); };
	void SetFont( const char * name ) { mFontName = name; };

	void SetTexture( const char * texName, DWORD texFlags = 0 );

	void PlayOnMouseSound( void );
	void StopOnMouseSound( void );

	void SetEventHandler( bool (*eventHandler)( MenuWindow* window, int event, void* data ) )
	{
		mEventHandler = eventHandler;
	};

	MenuWindow* GetChildWindow( float& x, float& y );	// it will change x, y to relatives in new window
	MenuWindow* GetWindowUnderMouse( float mouseX, float mouseY );

	virtual void Render( HUD& hud, float absX, float absY, float absW, float absH );

	DWORD Flags;
	DWORD Color;
	DWORD ColorUnderMouse;
	DWORD TextColor;
	DWORD TextColorUnderMouse;
	String Text;
	int TextJustification;
	bool MultiresolutionText;
	bool ScaledText;
	float TextScale;
	bool XAspected;
	bool Ticked;
	String MouseOnSoundName;
	String MouseClickedSoundName;

	char Type( void ) { return mType; };

protected:
	void AddChildWindow( MenuWindow* window );
	void RemoveChildWindow( MenuWindow* window );
	void RemoveAllChildren( void );
	void RefreshChildrenIndices( void );
	bool MouseEvent( float x, float y, int mouseButton, bool buttonDown );
	bool KeyEvent( int key, int event );
	bool OtherEvent( MenuEvent eventType, int eventData1, void* eventData2 );
#ifdef TRANSGAMING
    char *CutToWidth(const HUD& hud, const char *str, int width, char *buf, size_t len) const;
#else
	String CutToWidth( HUD& hud, String str, int width );
#endif
	bool CheckChildModal( void );
	void TextJustify( void );
	void GetTextExtents( float& x, float& y );
	void GetRelativeMouseCoords( float& x, float& y );
	void GenerateTick( void );

	// Variables

	char mType;	// Window type
	DynamicArray<MenuWindow*> mChildWindows;
	String mWindowName;
	int mWindowIdent;
	MenuWindow* mParentWindow;
	bool (*mEventHandler)( MenuWindow* window, int event, void* data );
	float X,Y,W,H;
	Texture *mTexture;
	String mTextureName;
	DWORD mTextureFlags;
	float U1, V1, U2, V2;
	float mTextX, mTextY;
	String mFontName;
	bool mUnderMouse;
#ifdef TRANSGAMING
	float  mLastTick;
#else
	double mLastTick;
#endif
	Sound2D* mMouseOnSound;
};

class ENGINE_API MenuButton : public MenuWindow
{
public:
	MenuButton();
	~MenuButton() {};
};

class ENGINE_API MenuProgressBar : public MenuWindow
{
public:
	MenuProgressBar();
	~MenuProgressBar();
	void Render( HUD& hud, float absX, float absY, float absW, float absH );

	// Public variable to set
	bool NonResizingTextures;
	bool CenteredPercentOver;
	float Percent;
	void SetTextureOver( const char * texName, DWORD color = 0xFFFFFFFF, DWORD texFlags = 0 );
	void SetTextureUnder( const char *texName, DWORD color = 0xFF000000, DWORD texFlags = 0 );

protected:
	// Texture 1 variables
	Texture *mTexture1, *mTexture2;
	DWORD mColor1, mColor2;
};

class ENGINE_API MenuCheckBox : public MenuWindow
{
public:
	MenuCheckBox();
	~MenuCheckBox();
	void Render( HUD& hud, float absX, float absY, float absW, float absH );

	// Public variable to set
	void SetTextureChecked( const char *texName, DWORD color = 0xFFFFFFFF, DWORD texFlags = 0 );
	void SetTextureUnchecked( const char *texName, DWORD color = 0xFFFFFFFF, DWORD texFlags = 0 );

	bool Checked;

protected:
	Texture *mTexture1, *mTexture2;
	DWORD mColor1, mColor2;
};

#define LISTCTRL_MAX_COLUMNS 20
#define LISTCTRL_PIXELS_BETWEEN_LINES 3

class ENGINE_API MenuListControl : public MenuWindow
{
	friend static bool MenuEventHandlerListControl( MenuWindow* window, int event, void* data );
	friend static bool MenuEventHandlerRedefineKeysControl( MenuWindow* window, int event, void* data );
public:
	MenuListControl();
	~MenuListControl();

	void Render( HUD& hud, float absX, float absY, float absW, float absH );

	void SetupControl( int numberOfColumns, const char * columnWidths, char delimiter = '|' );
	void ClearControl( void );
	void DeleteAllRecords( void );
	void AddRecord( const char * record );
	String GetRecord( int index );
	int GetRecordByValue( const char *value );
	void SetRecord( int index, const char *record );
	void DeleteRecord( int index );
	bool DeleteRecordByField( int columnIndex, String field );
	int NumberOfRecords( void ) { return mColumns[0].Size(); };
	int RecordsPerPage( void ) { return mLinesPerPage; };
	int Position( void ) { return mDisplayFromLine; };

	DWORD SelectionColor;
	bool NoSelect;
	bool KeepAtLast;

	void Home( void ) { mDisplayFromLine = 0; };
	void End( void ) { mDisplayFromLine = NumberOfRecords() - mLinesPerPage; if( mDisplayFromLine < 0 ) mDisplayFromLine = 0; };
	void NextLine( void ) { mDisplayFromLine++; if( mDisplayFromLine > NumberOfRecords()-mLinesPerPage ) mDisplayFromLine = NumberOfRecords()-mLinesPerPage; if( mDisplayFromLine < 0 ) mDisplayFromLine = 0; };
	void PreviousLine( void ) { mDisplayFromLine--; if( mDisplayFromLine < 0 ) mDisplayFromLine = 0; };
	void NextPage( void ) { mDisplayFromLine += mLinesPerPage; if( mDisplayFromLine > NumberOfRecords()-mLinesPerPage ) mDisplayFromLine = NumberOfRecords()-mLinesPerPage;  if( mDisplayFromLine < 0 ) mDisplayFromLine = 0; };
	void PreviousPage( void ) { mDisplayFromLine -= mLinesPerPage; if( mDisplayFromLine < 0 ) mDisplayFromLine = 0; };
	void SetAtPercent( float percent )
	{
		mDisplayFromLine = (int)floor(percent*(NumberOfRecords()-mLinesPerPage));
		if( mDisplayFromLine < 0 )
			mDisplayFromLine = 0;
	};

	int GetSelected( void ) { return mSelectedRecord; };
	void SetSelected( int record ) { mSelectedRecord = record; if( ( mSelectedRecord < 0 ) || ( mSelectedRecord > NumberOfRecords()-1 ) ) mSelectedRecord = -1; };
	void ValidateSelected( void ) { if( mSelectedRecord < 0 ) mSelectedRecord = 0; if( mSelectedRecord >= NumberOfRecords() ) mSelectedRecord = NumberOfRecords()-1; };
protected:
	bool mListCtrlInited;
	int mListCtrlColumns;
	char mDelimiter;
	DynamicArray<String> mColumns[LISTCTRL_MAX_COLUMNS];
	float mColumnWidths[LISTCTRL_MAX_COLUMNS];

	int mDisplayFromLine;
	int mLinesPerPage;
	int mSelectedRecord;
};
/*
class ENGINE_API MenuGameSpyControl : public MenuListControl
{
	friend static void GameSpyControlCallback( void* serverlist, int msg, void *instance, void *param1, void *param2 );
public:

	enum {
		EServerName = 0,
		EPing = 1,
		EPlayers = 2,
		EMapName = 3,
		EGameType = 4,
		EHostAddress = 5,
	};

	enum {
		EStatusDone = 0,
		EStatusGettingList = 1,
		EStatusSearchingLAN = 2,
		EStatusQuerying = 3,
	};

	MenuGameSpyControl();
	~MenuGameSpyControl();

	void StartGameSpy( bool noIP = false );
	void SortList( int sortType );
	void RefreshList( bool lan );
	void HaltRefreshList( void );
	bool FillListCtrlWithPlayers( MenuListControl* lc );

	int GameSpyStatus;
	int GameSpyPlayersNumber;

protected:
	void UpdateControlFromServerList( void );
	void AddRecordFromServerList( void* serv );

	void* mServerList;		// that will really be GameSpy GServerList
	bool mAscending;		// it will control sort direction in control
	bool mThink;			// should we think?
	bool mNoIP;				// GameSpy control subtype
};
*/
class ENGINE_API MenuRedefineKeysControl : public MenuWindow
{
	friend static bool MenuEventHandlerRedefineKeysControl( MenuWindow* window, int event, void* data );
public:

	MenuRedefineKeysControl();
	~MenuRedefineKeysControl();

	void LoadKeysFromLUA( void );
	void SaveKeysToLUA( void );
	void SetDefaultKeys( void );

	int NumberOfActions( void ) { return mActionList->NumberOfRecords(); };
	int ActionsPerPage( void ) { return mActionList->RecordsPerPage(); };
	int Position( void ) { return mActionList->Position(); };
	void SetAtPercent( float percent ) { mActionList->SetAtPercent(percent); mPrimaryList->SetAtPercent(percent); mAlternativeList->SetAtPercent(percent); };
protected:
	MenuListControl* mActionList;
	MenuListControl* mPrimaryList;
	MenuListControl* mAlternativeList;
};

class ENGINE_API MenuEditField : public MenuWindow
{
public:
	MenuEditField();
	~MenuEditField();
	void Render( HUD& hud, float absX, float absY, float absW, float absH );

	// Public variable to set
	bool ReadOnly;
	bool NumberField;
	bool IPField;
	int MinimalNumberOfCharacters;
	int MaximalNumberOfCharacters;
};

class ENGINE_API MenuScrollBar : public MenuWindow
{
	friend static bool MenuEventHandlerScrollBar( MenuWindow* window, int event, void* data );
	friend static bool MenuEventHandlerGiveClickToParent( MenuWindow* window, int event, void* data );
public:
	MenuScrollBar();
	~MenuScrollBar();

	float Position;				// from 0 to 1
	float PercentCovered;		// from 0 to 1; it controls the extent of the bar inside
	bool DrawPercent;

	// Those fields control what is displayed in the percent line
	float PercentAddTo;
	float PercentMultiplyBy;
	String PercentFollowBy;

	void SetTextureButton0( const char * texName, DWORD color = 0xFFFFFFFF, DWORD texFlags = 0 );
	void SetTextureButton1( const char * texName, DWORD color = 0xFF000000, DWORD texFlags = 0 );
	void SetTextureBack( const char * texName, DWORD color = 0xFF000000, DWORD texFlags = 0 );
	void SetTextureBar( const char * texName, DWORD color = 0xFF000000, DWORD texFlags = 0 );

	void Render( HUD& hud, float absX, float absY, float absW, float absH );
protected:
	MenuButton* mButton0;
	MenuButton* mButton1;
	MenuButton* mBar;
	bool mMouseCaught;
};

// *******************************************************************************

class ENGINE_API MenuSystem
{
	friend class MenuWindow;
	friend class MenuProgressBar;
	friend class MenuCheckBox;
	friend class MenuEditField;
	friend class MenuScrollBar;
	friend class MenuGameSpyControl;	// for ServerListThink calls
	friend static void GameSpyControlCallback( void* serverlist, int msg, void *instance, void *param1, void *param2 );
public:
	MenuSystem();
	~MenuSystem();
	void Clear( void );

	void Activate( bool active );
	bool Active( void ) { return mActive; };

	MenuWindow* MakeNewWindow( char type );
	MenuWindow* GetWindowByName( String name );
	MenuWindow* GetWindowByIdent( int ident );
	MenuWindow* GetWindowUnderMouse( void );

	bool SendEvent( MenuEvent eventType, int eventData1, void* eventData2 = 0 );

	void SetRootWindow( MenuWindow* window );

	void SetFocus( MenuWindow* window );
	void SetFocus( int windowIdent );
	void SetFocus( String windowName );

	void ShowOnly( String windowName );	// makes this window the only window visible in parent window

	void Render( void );

	// Function result for LUA handlers
	void SetHandledEventMarker( bool marker ) { mHandledEventMarker = marker; };	// To be used only by ExportToLua.cpp
	bool GetLUAReply( void ) { return mHandledEventMarker; };

	// Mouse cursor methods
	void SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags = 0 );
	void ShowMouse( bool show );

	// Special map window access
	void SetMapWindow( String name ) { mMapWindow = GetWindowByName( name ); };
	void SetMapParams( float x, float y, float w, float h ) { mMapX = x; mMapY = y; mMapW = w; mMapH = h; };
	void SetMapExtent( float extent ) { mMapExtent = extent; };
	void SetMapTexture( String texture ) { mMapTexture = texture; };
	void SetMapPlayerPosOrientation( float x, float y, float angle ) { mMapPlayerX = x; mMapPlayerY = y; mMapPlayerAngle = angle; };

	// Special method, used mainly for rendering loading screen
	void RenderOnce( void );
	String GetResolutionString( void ) { return mResolutionString; };
private:
	void RememberWindow( MenuWindow* window );
	void ForgetWindow( int ident );
	void GetMenuMousePosition( float& x, float& y );

	bool mActive;
	bool mHandledEventMarker;
	MenuWindow* mRootWindow;
	MenuWindow* mFocusWindow;
	MenuWindow* mCurrentWindowUnderMouse;

	// Reander target width/height
	int mRenderTargetX;
	int mRenderTargetY;

	// Map params
	MenuWindow* mMapWindow;
	float mMapX, mMapY, mMapW, mMapH;
	String mMapTexture;
	float mMapPlayerX, mMapPlayerY, mMapPlayerAngle;
	float mMapExtent;

	DynamicArray<MenuWindow*> mAllWindows;
	DynamicArray<int> mFreeWindowIndices;

	// This is only for getting all existing windows in the system by name
	DynamicArray<MenuWindow*> mCompleteWindowSet;
//	DynamicArray<void*> mGameSpyServerLists;

	bool MouseShown;
	int mMouseClickOffsetX;
	int mMouseClickOffsetY;
	Texture* mMouseTexture;

	String mResolutionString;
};
