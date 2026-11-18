
// #pragma once
#ifndef _PAINMENU_
#define _PAINMENU_


class Movie;
class Sound2D;
class SoundList;

#include "Core/LeString.h"

#include <winsock2.h>
#ifndef	TRANSGAMING_NO_GAMESPY
#include "Net/GameSpy2/serverbrowsing/sb_serverbrowsing.h"
#include "Net/GameSpy2/serverbrowsing/sb_internal.h"
#endif

/*
typedef struct
{
	unsigned int publicip;
	unsigned short publicport;
	unsigned int privateip;
	unsigned short privateport;
	unsigned int icmpip;	
	unsigned char state;
	unsigned char flags;
	unsigned int keyvals;
	unsigned int updatetime;
	void *next;
} MySBServerType;
*/

typedef struct _SBServer MySBServerType;


enum MenuEvent
{
	// Events from input system - mouse keys are here too
	EMenuKeyDown			= 1,
	EMenuKeyUp				= 2,
#ifndef TRANSGAMING
	EMenuMouseUp			= 3,
	EMenuMouseDown			= 4,
#endif
	EMenuKeyDownRepeated	= 5,
	// Other events
#ifndef TRANSGAMING
	EMenuClicked			= 6,
	EMenuMouseLeft			= 7,
	EMenuMouseEntered		= 8,
#endif
};


class SaveGame
{
public:

#ifdef TRANSGAMING
	SaveGame( const char *n, const char *e, const char *dt, const char *di ) { 
        name =          n;
        elapsed =       e;
        date =          dt;
        difficulty =    di;
    }
	SaveGame() { 
        name =          "";
        elapsed =       "";
        date =          "";
        difficulty =    "";
    }
#else
	SaveGame( String n, String e, String dt, String di ) { name = n; elapsed = e; date = dt; difficulty = di; }
	SaveGame() { SaveGame( "", "", "", "" ); }
#endif

	String name;
	String elapsed;
	String date;
	String difficulty;
};


enum MenuBGType
{
	EMenuBGImage	= 1,
	EMenuBGMovie	= 2
};


enum MenuItemType
{
	EMenuItemTextButton	= 1,
	EMenuItemCheckbox	= 2,
	EMenuItemTextButtonEx = 3,
	EMenuItemSlider = 4,
	EMenuItemKeyConf = 5,
	EMenuItemScroller = 6,
	EMenuItemTextEdit = 7,
	EMenuItemSliderImage = 8,
	EMenuItemServerList = 9,
	EMenuItemServerData = 10,
	EMenuItemPassword = 11,
	EMenuItemNumEdit = 12,
	EMenuItemList = 13,
	EMenuItemMapTable = 14,
	EMenuItemNumRange = 15,
	EMenuItemLoadSave = 16,
	EMenuItemStaticText = 17,
	EMenuItemImageButton = 18,
	EMenuItemKeyList = 19,
	EMenuItemBorder = 20,
	EMenuItemTabGroup = 21,
	EMenuItemPlayerModel = 22,
	EMenuItemColorPicker = 23,
	EMenuItemCharPicker = 24,
	EMenuItemImageButtonEx = 25,
	EMenuItemWeaponList = 26,
	EMenuItemMessagesKeys = 27,
	EMenuItemSimpleKeyConf = 28,
};


enum MenuAlign
{
	EAlignNone		= 1,
	EAlignLeft		= 2,
	EAlignRight		= 3,
	EAlignCenter	= 4,
};


class ENGINE_API MenuItem
{
public:
#ifdef TRANSGAMING
    typedef enum _NavAction{
        NAV_NONE =      -1,
        NAV_UP =        0,
        NAV_DOWN =      1,
        NAV_LEFT =      2,
        NAV_RIGHT =     3,
        NAV_MIDDLE =    4,

        NAV_TOTALACTIONS,
    } NavAction;
#endif


	MenuItem();
	virtual ~MenuItem();

	virtual void Render();
	virtual void RenderDesc();

#ifdef TRANSGAMING
	void SetName( const char *name );
	void SetText( const char *text );
	void SetDesc( const char *desc );
#else
	void SetName( String name );
	void SetText( String text );
	void SetDesc( String desc );
#endif

	virtual void SetPosition( int x, int y );

	void SetUnderMouse( bool val ) { mUnderMouse = val; }
#ifdef TRANSGAMING
	void SetAction( const char *action ) { mAction = action; }
	const char *GetAction() const { 
        if (mAction.IsNull())
            return NULL;
        else
            return mAction.AsChar(); 
    }
#else
	void SetAction( String action ) { mAction = action; }
	String GetAction() const { return mAction; }
#endif
	bool UnderMouse() const { return mUnderMouse; }
	bool ApplyRequired() const { return mApplyRequired; }
	void ApplyRequired( bool val ) { mApplyRequired = val; }

	virtual void SetSmallFont( const char * name, int size );
	virtual void SetBigFont( const char *name, int size );
	virtual void SetSmallFontTex( const char *tex ) { mSmallFontTex = tex; }
	virtual void SetBigFontTex( const char *tex )   { mBigFontTex = tex; }
	const char *GetBigFont() const { return mBigFont.AsChar(); }
	int GetBigFontSize() const { return mBigSize; }

	void SetAlign( MenuAlign align ) { mAlign = align; }

	int GetPosX() const { return mPosX; }
	int GetPosY() const { return mPosY; }
    int GetX() const { return mX; }
    int GetY() const { return mY; }
#ifdef TRANSGAMING
	const char *GetName() const { return mName.AsChar(); }
	const char *GetText() const { return mText.AsChar(); }
#else
	String GetName() const { return mName; }
	String GetText() const { return mText; }
#endif
	MenuItemType GetType() const { return mType; }
	void SetDescPos( int x, int y ) { mDescX = x; mDescY = y; }

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
#ifdef TRANSGAMING
    void GetCenter(int *x, int *y) const;
    void SetNavAction(NavAction action, const char *cmd);
    const char *GetNavAction(NavAction action) const;
    bool HasNavAction(NavAction action) const;
#endif
	void SetWidth( int w ) { mWidth = w; }
	void SetHeight( int h ) { mHeight = h; }

	virtual bool CheckIfUnderMouse();
	bool IsPressed() const { return mIsPressed; }

	void PlayExitMovie( int start, int end );
	bool PlayExitMovie() const { return mExitStart != -1 && mExitEnd != -1; }

	int ExitMovieStart() const { return mExitStart; }
	int ExitMovieEnd() const { return mExitEnd; }

	virtual void SetColors( DWORD text, DWORD disabled, DWORD underMouse, DWORD desc );
	void SetTransparency( BYTE trans ) { mTransparency = trans; }
	DWORD CalcTransColor( DWORD color );
	DWORD GetTextColor() const { return mTextColor; }

	void DrawShadow( bool draw ) { mDrawShadow = draw; }
	void SetBackground( const char *bg );
	void SetBGWidth( int w );

	void Enable() { mDisabled = false; }
	void Disable() { mDisabled = true; }
	bool IsDisabled() const { return mDisabled; }

	void SetVisibility( bool vis ) { mVisible = vis; }
	bool IsVisible() const { return mVisible; }

	virtual void CalcSize();
	virtual void CalcPosition();
	virtual void UpdateScale();

	void SetConfigOption( const char *opt ) { mConfigOption = opt; }

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	virtual void SetLightOnSound( const char *snd ) { mSndLightOn = snd; }
	virtual void SetAcceptSound( const char *snd ) { mSndAccept = snd; }
	const char *GetAcceptSound() const { return mSndAccept.AsChar(); }

	void SetWarning( bool warn ) { mWarning = warn; }
	bool GetWarning() const { return mWarning; }

protected:

	virtual void DrawAt( int x, int y, DWORD color );
	virtual void DrawBackground();

	String	mName;
	String	mText;
	String	mDesc;

	Texture	*mBG[3];
	RECT	mBGRect[3];
	int		mCenterRepeat;
	bool	mUseBackground;

	int		mPosX;
	int		mPosY;
	int		mX;
	int		mY;
	MenuAlign mAlign;

	int		mDescX;
	int		mDescY;

	bool	mDisabled;
	bool	mVisible;

	bool	mUnderMouse;
	bool	mIsPressed;

	bool	mDrawShadow;

	BYTE	mTransparency;

	String	mAction;
	bool	mApplyRequired;
#ifdef TRANSGAMING
    String  mNavActions[NAV_TOTALACTIONS];
#endif

	String	mBigFont;
	String	mSmallFont;
	int		mBigSize;
	int		mSmallSize;
	String	mBigFontTex;
	String	mSmallFontTex;

	int		mWidth;
	int		mHeight;
	float	mScaleX;
	float	mScaleY;

	int		mExitStart;
	int		mExitEnd;

	DWORD	mTextColor;
	DWORD	mDisabledColor;
	DWORD	mTextColorUnderMouse;
	DWORD	mDescColor;

	MenuItemType	mType;

	String	mConfigOption;

	bool	mWarning;
	BYTE	mWarningTrans;
	BYTE	mWarningTransDir;
	float	mWarningStart;

	// Sounds
	String	mSndLightOn;
	String	mSndAccept;
	bool	mSndPlayLightOn;
	bool	mSndPlayAccept;
};


class ENGINE_API MenuItemCheckbox : public MenuItem
{
public:

	MenuItemCheckbox();
	virtual ~MenuItemCheckbox();

	virtual void Render();

	virtual void CalcSize();

	void SetTextureChecked( const char *texName, DWORD color = 0xFFFFFFFF, DWORD texFlags = 0 );
	void SetTextureUnchecked( const char *texName, DWORD color = 0xFFFFFFFF, DWORD texFlags = 0 );

	bool Checked;

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	virtual void CalcPosition();

private:

	Texture	*mTexOn, *mTexOff;
	DWORD	mColorOn, mColorOff;
};


class ENGINE_API MenuItemTextButtonEx : public MenuItem
{
public:

	MenuItemTextButtonEx();
	virtual ~MenuItemTextButtonEx();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void SetOriginalText( const char *name ) { mOrigText = name; }
	void ChangeValue( const char *value ) { mText = mOrigText + ": " + value; }

private:

	String	mOrigText;
};


class ENGINE_API MenuItemSlider : public MenuItem
{
public:

	enum
	{
		ENone		= 0,
		ELeft		= 1,
		ERight		= 2,
		EDrag		= 3,
	};

	MenuItemSlider();
	virtual ~MenuItemSlider();

	virtual void Render();

	virtual void CalcSize();
	virtual void CalcPosition();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void SetMinMax( int min, int max, bool isFloat ) { mMinValue = min; mMaxValue = max; mIsFloat = isFloat; }
	void SetValue( int value ) { mCurrValue = value; }
	int GetValue() const { return mCurrValue; }
	void SetSliderWidth( int width );
	void SetControlWidth( int width );
	void SetFullWidth( bool full ) { mFullWidth = full; }
	bool GetFullWidth() const { return mFullWidth; }

	bool IsFloat() const { return mIsFloat; }

protected:

	int CheckMouse();

	int		mMinValue;
	int		mMaxValue;
	int		mCurrValue;

	int		mSliderWidth;
	int		mCtrlWidth;
	bool	mFullWidth;

	int		mLeftPos;
	int		mRightPos;
	int		mDragPos;

	int		mDragX;
	int		mDragY;

	int		mActiveCtrl;

	bool	mIsFloat;

	bool	mDrawValue;

	Texture	*mLeft, *mRight, *mDrag, *mBG;
};


class ENGINE_API MenuItemScroller : public MenuItem
{
public:

	enum
	{
		ENone		= 0,
		EUp			= 1,
		EDown		= 2,
		EDrag		= 3,
	};

	MenuItemScroller();
	virtual ~MenuItemScroller();

	virtual void Render();

	virtual void CalcSize();
	virtual void CalcPosition();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void SetMinMax( int min, int max );
	int GetMax() const { return mMaxValue; }
	void SetValue( int value ) { mCurrValue = value; }
	int GetValue() const { return mCurrValue; }
	void SetScrollerHeight( int height ) { mScrollerHeight = height; }
	int GetScrollerHeight() const { return mScrollerHeight; }

	void ScrollDown();
	void ScrollUp();

protected:

	int CheckMouse();

	int		mMinValue;
	int		mMaxValue;
	int		mCurrValue;

	int		mScrollerHeight;

	int		mUpPos;
	int		mDownPos;
	int		mDragPos;

	int		mDragX;
	int		mDragY;

	int		mActiveCtrl;

	Texture	*mUp, *mDown, *mDrag, *mBG;
};


class ENGINE_API MenuItemBorder : public MenuItem
{
public:

	enum Tiles
	{
		kCornerLB	= 0,
		kCornerLT	= 1,
		kCornerRB	= 2,
		kCornerRT	= 3,
        kBorderB	= 4,
		kBorderT	= 5,
		kBorderL	= 6,
		kBorderR	= 7,
		kBG			= 8,
		kBGDark		= 9,
	};

	MenuItemBorder();
	virtual ~MenuItemBorder();

	virtual void Clear();

	void Render();
	void SetSize( int w, int h ) { mW = w; mH = h; }

	void AddScroller( MenuItemScroller *s );
	void RemoveScroller() { mScroller = NULL; }
	MenuItemScroller *GetScroller() const { return mScroller; }

	void SetColumnsCount( int c );
	void SetColumnWidth( int c, int w );
	void SetHeader( int h ) { mHeader = h; }

	void SetDark( bool set ) { mIsDark = set; }

protected:

	MenuItemScroller	*mScroller;

	Texture				*mTextures[10];

	int					mW;
	int					mH;

	DynamicArray<int>	mColumns;
	int					mHeader;

	bool				mIsDark;
};


class ENGINE_API MenuItemTabGroup : public MenuItemBorder
{
public:

	MenuItemTabGroup();
	virtual ~MenuItemTabGroup();

	virtual void Clear();

	void Render();

	void AddItem( MenuItem *item );

	bool IsActive() const { return mActive; }
	void Activate( bool active );

private:

	bool	mActive;

	DynamicArray<MenuItem *>	mItems;
};


class ENGINE_API MenuItemList : public MenuItem
{
public:

	friend class MenuItemMapTable;

	struct MenuListItem
	{
		String	name;
		void	*obj;
	};

	MenuItemList();
	virtual ~MenuItemList();

	virtual void Render();
	virtual void DrawElem( MenuItemList::MenuListItem *elem, int x, int y, bool header = false );

	virtual void CalcSize();
	virtual void CalcPosition();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	virtual void AddItem( const char *name, void *obj );
	virtual void RemoveItem( int index );
	virtual void MoveItemUp( int index );
	virtual void MoveItemDown( int index );
	void RemoveAll();

	int GetNumItems() const { return mItems.Size(); }

	virtual void SetMaxHeight( int height );
	int GetMaxHeight() const { return mMaxHeight; }
	void SetBorderWidth( int width );

	void SetHeaderUse( bool use ) { mUseHeader = use; }
	bool GetHeaderUse() const { return mUseHeader; }

	int GetSelectedIndex() const { return mSelected; }
	MenuListItem *GetSelected() const { if( mSelected == -1 ) return NULL; else return mItems[mSelected]; }
	MenuListItem *GetFirst() const;
	MenuListItem *GetDoubleClicked() const { if( mDoubleClicked == -1 ) return NULL; else return mItems[mDoubleClicked]; }
	int GetDoubleClickedIndex() const { return mDoubleClicked; }
	MenuListItem *GetItem( int index ) const { return mItems[index]; }
	MenuListItem **GetItems() const { return mItems.Elements(); }

	virtual void SetPosition( int x, int y );

	virtual void SetColors( DWORD text, DWORD disabled, DWORD underMouse, DWORD desc );

	int mNumVisible;

	MenuItemBorder *GetBorder() const { return mBorder; }
	MenuItemScroller *GetScroller() const { return mScroller; }

protected:

	DynamicArray<MenuListItem *>	mItems;

	int	mSelected;
	int mDoubleClicked;
	int mMaxHeight;
	int mBorderWidth;

	bool mUseHeader;

	MenuItemScroller *mScroller;
	MenuItemBorder *mBorder;
};


class MenuItemWeaponList : public MenuItemList
{
public:

	MenuItemWeaponList();
	~MenuItemWeaponList();

	void SetSeparatorPos( int pos ) { mSeparatorPos = pos; }
	int GetSeparatorPos() const { return mSeparatorPos; }

	void Render();

	virtual void MoveItemUp( int index );
	virtual void MoveItemDown( int index );

private:

	int	mSeparatorPos;
};


class MenuItemKeyConf : public MenuItemList
{
public:

	enum
	{
		ENone			= 0,
		EPrimary		= 1,
		EAlternative	= 2,
	};

	MenuItemKeyConf();
	virtual ~MenuItemKeyConf();

	virtual void Render();

	virtual void CalcSize();
	virtual void CalcPosition();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void SetOptionPrimary( const char *option ) { mOptionPrimary = option; }
	void SetOptionAlternate( const char *option ) { mOptionAlternate = option; }
	void SetKey( const char *key, const char *keyEng ) { mKey = key; mKeyEng = keyEng; }
	void SetAlternate( const char *key, const char *keyEng ) { mAlternate = key; mAlternateEng = keyEng; }
	void SetIndex( int index ) { mIndex = index; }

	const char *GetPrimary() const { return mKeyEng.AsChar(); }
	const char *GetAlternate() const { return mAlternateEng.AsChar(); }

private:

	int CheckMouse();

	String	mOptionPrimary;
	String	mOptionAlternate;
	String	mKey;
	String	mAlternate;
	String  mKeyEng;
	String  mAlternateEng;

	int		mIndex;
	int		mKeyToChange;
};


class MenuItemKeyList : public MenuItemList
{
public:
	MenuItemKeyList();

	virtual void DrawElem( MenuItemList::MenuListItem *elem, int x, int y, bool header = false );
};


class MenuItemSimpleKeyConf : public MenuItem
{
public:

	MenuItemSimpleKeyConf();
	~MenuItemSimpleKeyConf();

	virtual void Render();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	const char *GetKey() const { return mKeyEng.AsChar(); }
	void SetKey( const char *key, const char *eng );

	void SetIndex( int index ) { mIndex = index; }

private:

	bool	mChangeMode;
	String	mKey;
	String	mKeyEng;
	int		mIndex;
};


class ENGINE_API MenuItemTextEdit : public MenuItem
{
public:

	MenuItemTextEdit();
	virtual ~MenuItemTextEdit();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	virtual void Render();

	virtual void CalcSize();

	void SetMaxLength( int len ) { mMaxLength = len; }

	const char *GetEditText() const { return mEditText.AsChar(); }
	void SetEditText( const char *text ) { mEditText = text; mCursorPos = (int)strlen(text); }

	bool IsInEditMode() const { return mEditMode; }

	const char *GetEnterAction() const { return mEnterAction.AsChar(); }
	void SetEnterAction( const char *action ) { mEnterAction = action; }

	void SetCurrColor( int c ) { mCurrColor = c; }
	int GetTextLength();

	void OnHome();
	void OnEnd();
	void OnCursorLeft();
	void OnCursorRight();
	void OnEnter();
	void OnBackspace();
	void OnDelete();
	void OnPrintable( unsigned char c, bool key = true );

	void FindUnderCursorColor();

protected:

	bool	mEditMode;
	String	mEditText;
	int		mMaxLength;
	String	mEnterAction;
	int		mCursorPos;

	int		mCurrColor;
	int		mPrevColor;
	int		mUnderCursorColor;
};


class MenuItemSliderImage : public MenuItemSlider
{
public:

	MenuItemSliderImage();
	virtual ~MenuItemSliderImage();

	virtual void Render();

	virtual void CalcSize();
	virtual void CalcPosition();

	void AddImage( const char *name );

private:

	PtrArray<Texture>		mImages;
};


class MenuItemServerData : public MenuItem
{
public:

	MenuItemServerData();
	virtual ~MenuItemServerData();

	virtual void Render();
	virtual void DrawAt( int x, int y, DWORD color );

	virtual void CalcSize();
	virtual void CalcPosition();

	void SetMap( const char *map ) { mMap = map; }
	void SetNumberOfPlayers( const char *numPlayers ) { mNumPlayers = numPlayers; }
	void SetGame( const char *game ) { mGame = game; }
	void SetPing( const char *ping ) { mPing = ping; }
	void SetIP( const char *ip ) { mIP = ip; }
	void SetPort( unsigned short port ) { mPort = port; }
	void SetIndex( int index ) { mIndex = index; }

	void SetSortColumn( int column ) { mSortColumn = column; }
	int GetSortColumn() const { return mSortColumn; }

	const char *GetIP() const { return mIP.AsChar(); }
	unsigned short GetPort() const { return mPort; }

	void SetSelected( bool val ) { mSelected = val; }
	void SetNumVisible( int num ) { mNumVisible = num; }

	bool IsSelected() const { return mSelected; }

	void SetServer( void *server ) { mGSServer = server; }
	void *GetServer() const { return mGSServer; }

	bool IsLocal() const { return mIsLocal; }
	void SetLocal( bool local ) { mIsLocal = local; }

private:

	String		mMap;
	String		mNumPlayers;
	String		mGame;
	String		mPing;
	String		mIP;
	unsigned short	mPort;
	bool		mIsLocal;

	int			mIndex;
	int			mNumVisible;
	int			mSortColumn;

	bool		mSelected;

	void		*mGSServer; // GameSpy server
};


#ifndef	TRANSGAMING_NO_GAMESPY
class MenuItemServerList : public MenuItemList
{
public:

	enum
	{
		EListStatusNone		= 0,
		EListStatusIdle		= 1,
		EListStatusGetting	= 2,
		EListStatusSearchingLAN = 3,
		EListStatusQuerying	= 4,
	};

	enum
	{
		EListSortName		= 0,
		EListSortMap		= 1,
		EListSortNumPlayers	= 2,
		EListSortGame		= 3,
		EListSortPing		= 4,
		EListSortIP			= 5,
	};

	MenuItemServerList();
	virtual ~MenuItemServerList();

	virtual void Render();
	void DrawInfoLine();
	void DrawEnterIP();

	virtual void CalcSize();
	virtual void CalcPosition();

	void Refresh();
	void Stop( bool updateStatus = true );
	void Clear();

	void SortList( int column, bool reset = false );
	int GetSortColumn() const { return mSortColumn; }

	void AddServer( void *server );
	void AddServer( void *server, const char *name, const char *map, int numPlayers, int maxPlayers, const char *game, int ping, const char *ip, unsigned short port );
	void AddServer( const char *ip, unsigned short port );

	const char *GetServerIP();
	unsigned short GetServerPort();
	void *GetServerData();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	int GetServerCount() const { return mServers.Size(); }

	virtual bool CheckIfUnderMouse();

	void SetStatus( int status );
	int FindServer( void *server );

	int mTotalPlayers;

	bool mIsLAN;
	bool mIsFavorites;
	bool mIsPKTV;

private:

	void	*mServerBrowser;
	PtrArray<MenuItemServerData>	mServers;
	int		mSelected;
	int		mSortColumn;

	int		mStatus;
	String	mStatusText;

	int mColumns[6];

	MenuItemTextEdit	*mManualIP;

	MenuItemBorder *mStatBorder;
};
#endif	//GAME_SPY


class ENGINE_API MenuItemPassword : public MenuItemTextEdit
{
public:

	MenuItemPassword();
	virtual ~MenuItemPassword();

	virtual void Render();
};


class ENGINE_API MenuItemNumEdit : public MenuItemTextEdit
{
public:

	MenuItemNumEdit();
	virtual ~MenuItemNumEdit();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );
};


class ENGINE_API MenuItemMapTable : public MenuItemList
{
public:

	MenuItemMapTable();
	~MenuItemMapTable();

	virtual void Render();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void SetSmallFont( const char *name, int size );
	void SetBigFont( const char *name, int size );

	void SetPosition( int x, int y );

	void SetColors( DWORD text, DWORD disabled, DWORD underMouse, DWORD desc );

	void SetMaxHeight( int height );

	void FillMapList();

	void AddMapToServer( MenuItemList::MenuListItem *elem );
	void RemoveMapFromServer( int index );
	void AddAllMapsToServer();
	void RemoveAllMapsFromServer();

	void RemoveMap( const char *name );
	void Update( const char *mode );

	const char *GetSelectedMap();
	MenuItemList *GetServerMapList() const { return mOnServer; }

protected:

	MenuItemList	*mAvail;
	MenuItemList	*mOnServer;

	MenuItem		*mAddButton;
	MenuItem		*mRemoveButton;
	MenuItem		*mUpButton;
	MenuItem		*mDownButton;
	MenuItem		*mAddAllButton;
	MenuItem		*mRemoveAllButton;

	MenuItemBorder	*mBorder;

	String			mMode;
};


class MenuItemNumRange : public MenuItemTextButtonEx
{
public:

	MenuItemNumRange();
	~MenuItemNumRange();

	void SetMinMax( int min, int max ) { mMinVal = min; mMaxVal = max; }
	void SetCurrValue( int curr );

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	int GetCurrValue() const { return mCurrVal; }

protected:

	int		mMinVal;
	int		mMaxVal;
	int		mCurrVal;
};


class MenuItemLoadSave : public MenuItemList
{
public:

	MenuItemLoadSave();
	~MenuItemLoadSave();

	virtual void DrawElem( MenuItemList::MenuListItem *elem, int x, int y, bool header = false );

	virtual void CalcSize();

	void AllowSave( bool allow ) { mAllowSave = allow; }

private:

	bool	mAllowSave;
};


class MenuItemStaticText : public MenuItem
{
public:

	MenuItemStaticText() : MenuItem() { mType = EMenuItemStaticText; mFitInRect = false; }
	~MenuItemStaticText() {}

	virtual void DrawAt( int x, int y, DWORD color );

	virtual bool CheckIfUnderMouse() { return false; }

	void SetRect( RECT *rect );

protected:

	bool	mFitInRect;
	RECT	mRect;
};


class MenuItemImageButton : public MenuItem
{
public:

	MenuItemImageButton();
	~MenuItemImageButton();

	virtual void Render();

	virtual void CalcSize();
	virtual bool CheckIfUnderMouse();

	void SetImage( const char *normal, const char *under, const char *pressed );
	void SetGlowAnim( const char *glow );

private:

	Texture *mImage;
	Texture *mImageUnder;
	Texture *mImagePressed;
	Texture	*mImageGlow;

	bool	mFade;
	float	mFadeStart;
	float	mCurrTime;
	float	mFadeLen;

	float	mGlowTime;
	int		mGlowDir;
	int		mGlowTrans;
	float	mGlowStart;
};


class MenuItemPlayerModel : public MenuItem
{
public:

	MenuItemPlayerModel();
	~MenuItemPlayerModel();

	virtual void Render();
};


extern DWORD gTextColors[];


class MenuItemColorPicker : public MenuItem
{
public:

	MenuItemColorPicker();
	~MenuItemColorPicker();

	virtual void Render();

	void CalcSize();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	int GetColor() const { return mSelected; }

private:

	int mSelected;
};


class MenuItemCharPicker : public MenuItem
{
public:

	MenuItemCharPicker();
	~MenuItemCharPicker();

	virtual void Render();

	void CalcSize();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	char GetChar() const { return mSelected; }
	void SetColor( DWORD color ) { mColor = color; }

private:

	char mSelected;
	DWORD mColor;
};


class MenuItemImageButtonEx : public MenuItem
{
public:

	MenuItemImageButtonEx();
	~MenuItemImageButtonEx();

	virtual void Render();

	virtual bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	virtual bool CheckIfUnderMouse();

	void CalcSize();

	void SetValue( int v ) { mValue = v; }
	int GetValue() const { return mValue; }
	void AddImage( const char *name );

private:

	int		mValue;
	DynamicArray<Texture *> mImages;
};


//--------------------------------------------------------------------


class ENGINE_API MenuScreen
{
public:

	MenuScreen();
	~MenuScreen();

	void Activate();
	void Clear( bool delMovie = false );

	void Render();

	bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void SetBackground( const char *name, MenuBGType type );
	Texture *GetBackground() const { return mBackground; }

	void SetMovieLoop( int start, int end );
	void SetItemsFadeLength( int len ) { mItemsFadeLength = len; }
	void SetShowItemsFrame( int frame ) { mShowItemsFrame = frame; }
	void SetItemsDrawShadow( bool draw );

	void SetMenuWidth( int width ) { mMenuWidth = width; }
	int GetMenuWidth() const { return mMenuWidth; }

	void SetTopPos( int pos ) { mTopPos = pos; }
	int GetTopPos() const { return mTopPos; }

	void PlayExitMovie( int start, int end, const char *action );
	bool PlayingExitMovie() const { return mPlayingExitMovie; }
	void StopMovie();

#ifdef TRANSGAMING
	MenuItem *FindItem( const char * name );
	MenuItem *AddTextButton( const char * name, const char * text, const char * desc );
	MenuItem *AddCheckbox( const char * name, const char * text, const char * desc );
	MenuItem *AddTextButtonEx( const char * name, const char * text, const char * desc, const char * value );
	MenuItem *AddSlider( const char * name, const char * text, const char * desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth );
	MenuItem *AddKeyControl( const char * name, const char * text, const char * opt1, const char * opt2, const char * val1, const char * val2, const char * eng1, const char * eng2 );
	MenuItem *AddScroller( const char * name, const char * text, const char * desc, int min, int max, int curr, int height );
	MenuItem *AddTextEdit( const char * name, const char * text, const char * desc, int len, const char * value );
	MenuItem *AddSliderImage( const char * name, const char * text, const char * desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth );
	MenuItem *AddServerList( const char * name, const char * text, const char * desc, bool lanOnly, bool favorites, bool pktv );
	MenuItem *AddPassword( const char * name, const char * text, const char * desc, int len, const char * value );
	MenuItem *AddNumEdit( const char * name, const char * text, const char * desc, int len, const char * value );
	MenuItem *AddList( const char * name, bool useHeader );
	MenuItem *AddMapTable( const char * name );
	MenuItem *AddNumRange( const char * name, const char * text, const char * desc, int minVal, int maxVal, int curr );
	MenuItem *AddLoadSave( const char * name, bool useHeader );
	MenuItem *AddStaticText( const char * name, const char * text );
	MenuItem *AddImageButton( const char * name, const char * text, const char * desc, const char * image, const char * imageUnder, const char * imagePressed );
	MenuItem *AddKeyList( const char * name, bool useHeader );
	MenuItem *AddBorder( const char * name );
	MenuItem *AddTabGroup( const char * name );
	MenuItem *AddPlayerModel( const char * name );
	MenuItem *AddColorPicker( const char * name, const char * text, const char * desc );
	MenuItem *AddCharPicker( const char * name, const char * text, const char * desc );
	MenuItem *AddImageButtonEx( const char * name, const char * text, const char * desc, int value );
	MenuItem *AddWeaponList( const char * name, bool useHeader );
	MenuItem *AddSimpleKeyConf( const char * name, const char * val, const char * eng, int index );
#else
	MenuItem *FindItem( String name );
	MenuItem *AddTextButton( String name, String text, String desc );
	MenuItem *AddCheckbox( String name, String text, String desc );
	MenuItem *AddTextButtonEx( String name, String text, String desc, String value );
	MenuItem *AddSlider( String name, String text, String desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth );
	MenuItem *AddKeyControl( String name, String text, String opt1, String opt2, String val1, String val2, String eng1, String eng2 );
	MenuItem *AddScroller( String name, String text, String desc, int min, int max, int curr, int height );
	MenuItem *AddTextEdit( String name, String text, String desc, int len, String value );
	MenuItem *AddSliderImage( String name, String text, String desc, int min, int max, bool isFloat, int curr, int width, int ctrlWidth );
	MenuItem *AddServerList( String name, String text, String desc, bool lanOnly, bool favorites, bool pktv );
	MenuItem *AddPassword( String name, String text, String desc, int len, String value );
	MenuItem *AddNumEdit( String name, String text, String desc, int len, String value );
	MenuItem *AddList( String name, bool useHeader );
	MenuItem *AddMapTable( String name );
	MenuItem *AddNumRange( String name, String text, String desc, int minVal, int maxVal, int curr );
	MenuItem *AddLoadSave( String name, bool useHeader );
	MenuItem *AddStaticText( String name, String text );
	MenuItem *AddImageButton( String name, String text, String desc, String image, String imageUnder, String imagePressed );
	MenuItem *AddKeyList( String name, bool useHeader );
	MenuItem *AddBorder( String name );
	MenuItem *AddTabGroup( String name );
	MenuItem *AddPlayerModel( String name );
	MenuItem *AddColorPicker( String name, String text, String desc );
	MenuItem *AddCharPicker( String name, String text, String desc );
	MenuItem *AddImageButtonEx( String name, String text, String desc, int value );
	MenuItem *AddWeaponList( String name, bool useHeader );
	MenuItem *AddSimpleKeyConf( String name, String val, String eng, int index );
#endif

	MenuItemScroller *GetScroller() const { return mScroller; }
	void SetScroller( MenuItemScroller *s ) { mScroller = s; }
	void UpdateKeyConfig();

	bool PlayingMovie();
	void UpdateMovie();

	// For control configuration
	void ClearKeyConfig( const char *key );

	void SetFirstShown( int n ) { mFirstShown = n; }
	int GetFirstShown() const { return mFirstShown; }

	void SetItemAction( const char *action ) { mItemAction = action; }

#ifndef	TRANSGAMING_NO_GAMESPY
	void SetSelectedServer( void *s )
	{
		if( s )
			memcpy( &mSelectedServerData, s, sizeof(MySBServerType) );
		else
			memset( &mSelectedServerData, 0, sizeof(MySBServerType) );
	}

	void *GetSelectedServer() const { return (void*)&mSelectedServerData; }
#endif

	bool GetActiveTextEdit() const { return mActiveTextEdit; }
	void SetActiveTextEdit( bool active ) { mActiveTextEdit = active; }
	void AcceptActiveTextEdit();

	void SetWaitTime( float wait );

	bool IsKeyInUse( int key );
	void ClearKey( int key );

//	void ShowWarning();

private:

	PtrArray<MenuItem> mItems;

	MenuBGType	mBGType;
	Movie		*mMovie;
	Texture		*mBackground;

	int			mItemsFadeLength;
	int			mShowItemsFrame;
	int			mLoopStart;
	int			mLoopEnd;
	bool		mLoopChanged;

	int			mMenuWidth;
	int			mTopPos;

	bool		mPlayingExitMovie;
	String		mAfterExitMovie;
	String		mItemAction;

#ifndef	TRANSGAMING_NO_GAMESPY
	BYTE		mSelectedServerData[sizeof(MySBServerType)];
#endif

	bool		mActiveTextEdit;

	float		mWait;
	float		mWaitStart;

	// For warning
/*	bool		mWarningMode;
	MenuItem	*mWarningYes;
	MenuItem	*mWarningNo;
	MenuItemCheckBox *mWarningAgain;
	String		*mWarningText;
*/
	// For scrolling
	int			mFirstShown;
	MenuItemScroller *mScroller;
};


class GraphicsDevice;

#ifndef TRANSGAMING
class ProgressIcon
{
public:

	ProgressIcon() : x( 0 ), y( 0 ), tex( NULL ) {}
	~ProgressIcon();

	void Set( int x, int y, String name );
	void Draw();

	int		x;
	int		y;
	String	name;
	Texture *tex;
};
#endif


class ENGINE_API LoadingScreen
{
public:

	LoadingScreen();
	~LoadingScreen() { Activate( false ); }

	void Activate( bool active, int level = 0, const char *levelTex = NULL, const char *levelName = NULL );
	bool Active() const { return mActive; }

	void RenderLoadScreen();

#ifndef TRANSGAMING
	void SetIcon( int i, int x, int y, String name );
#endif

	void Progress( int p = 1 );
	int  GetProgress() const { return mProgress; }
	void SetOverall( int o, float adv = 100.0f ) { mAdvance = adv / ( o - mOverall ); mOverall = o; }
	int  GetOverall() const { return mOverall; }

private:

	void DrawProgress();

	bool mActive;
	int  mProgress;
	int  mOverall;
	float  mPercent;
	float  mAdvance;
	Texture *mBackground;
	Texture *mProgressBar;
	Texture *mLevel;
	int  mStartLevel;
#ifndef TRANSGAMING
	ProgressIcon mProgressIcons[30];
	Texture *mNumber;
#endif
	Texture *mRing;
	Texture *mShadow;
	float mSoundRepeat;
	String mLastLevelTex;
	String mLevelName;
};


class ENGINE_API PainMenu
{
public:

	PainMenu();
	~PainMenu();
	void Clear();

	void Activate( bool active );
	bool Active() const { return mActive; }

	void SetMouseTexture( int xClickOffset, int yClickOffset, const char *texName, DWORD texFlags = 0 );
	void ShowMouse( bool show ) { mShowMouse = show; }

	void Render();

	bool SendEvent( MenuEvent eventType, int eventData1, void *eventData2 = 0 );

	void PlayMenuSound( const char *name, bool loop = false );
	void StopSound();

	void PauseSounds();
	void ResumeSounds();

	void ShowCredits( bool addon );

	void EnableSplash( bool enable ) { mShowSplash = enable; }

	void ResetRoomType();

	MenuScreen Screen;
	LoadingScreen LoadScreen;

	bool	FrameChanged;
	bool	CreatingFont;
	bool	InGame;

	int		mPausedSounds;

private:

	bool		mActive;

	// Mouse
	bool		mShowMouse;
	int			mMouseClickOffsetX;
	int			mMouseClickOffsetY;
	Texture		*mMouseTexture;
	float		mMouseOldSens;

	int		mRoomType;
	float	mRoomIntensity2D;
	float	mRoomIntensity3D;

	int			mSnd;

	bool		mShowSplash;
};


//--------------------------------------------------------------------


void PainMenu_RegisterLUALib();


#endif //_PAINMENU_

