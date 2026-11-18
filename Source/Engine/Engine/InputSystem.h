
// #pragma once
#ifndef _INPUTSYSTEM_
#define _INPUTSYSTEM_


#include "Core/List.h"
#ifndef TRANSGAMING
# include "Core/Window.h"
#endif
#include "Core/LeString.h"
#include "Core/Math/LeMath.h"

#define VK_GRAVE	0xC0


#ifdef TRANSGAMING

// eric->
typedef enum{
    BUTTON_NONE =           0,
    BUTTON_UP =             1,
    BUTTON_DOWN =           2,
    BUTTON_LEFT =           3,
    BUTTON_RIGHT =          4,
    BUTTON_TRIANGLE =       5,
    BUTTON_CIRCLE =         6,
    BUTTON_SQUARE =         7,
    BUTTON_CROSS =          8,
    BUTTON_START =          9,
    BUTTON_SELECT =         10,
    BUTTON_HOME =           11,
    BUTTON_HOLD =           12,
    BUTTON_R1 =             13,
    BUTTON_L1 =             14,
    BUTTON_R2 =             15,
    BUTTON_L2 =             16,
    BUTTON_R3 =             17,
    BUTTON_L3 =             18,

    //mapped mouse buttons
    BUTTON_FAKE_LMOUSE =    19,
    BUTTON_FAKE_RMOUSE =    20,
    BUTTON_FAKE_MMOUSE =    21,

    //unmapped left joysticks
    BUTTON_POS_LX =         22,
    BUTTON_NEG_LX =         23,
    BUTTON_POS_LY =         24,
    BUTTON_NEG_LY =         25,

    //unmapped right joystick
    BUTTON_POS_RX =         26,
    BUTTON_NEG_RX =         27,
    BUTTON_POS_RY =         28,
    BUTTON_NEG_RY =         29,

    BUTTON_COUNT,
} PSButtonNames;
#endif


enum EInputAction
{
    EInputNone              =	0,      //
    EInputMoveForward       =	1,      //
    EInputMoveBackward      =	2,      //
    EInputMoveLeft          =	3,      //
    EInputMoveRight         =	4,      //
    EInputJump              =	5,      //
    EInputFire              =	6,      //
    EInputAltFire           =	7,      //
	EInputNextWeapon		=	8,      //
	EInputPrevWeapon		=	9,      //
#if !defined( TRANSGAMING ) || defined( TRANSGAMING_WINDOWS )
	EInputWeapon1			=	10,     //--------------------------------------------------------------------------------------
	EInputWeapon2			=	11,     // these bindings control switching to a specific weapon.  Not used on PSP/PS2
	EInputWeapon3			=	12,     //
	EInputWeapon4			=	13,     //
	EInputWeapon5			=	14,     //
	EInputWeapon6			=	15,     //
	EInputWeapon7			=	16,     //  NOT USED ON PSP/PS2
	EInputWeapon8			=	17,     //
	EInputWeapon9			=	18,     //
	EInputWeapon10			=	19,     //
	EInputWeapon11			=	20,     //
	EInputWeapon12			=	21,     //
	EInputWeapon13			=	22,     //
	EInputWeapon14			=	23,     //--------------------------------------------------------------------------------------
	EInputFireBestWeapon1	=	24,     //
	EInputFireBestWeapon2	=	25,     //  NOT USED ON PSP/PS2
	EInputRocketJump		=	26,     //
	EInputForwardRocketJump	=	27,     //--------------------------------------------------------------------------------------
#endif
	EInputUseCards			=	28,     //
	EInputComboFire			=	29,     //
#ifndef TRANSGAMING
	EInputSelectBestWeapon1	=	30,     //
	EInputSelectBestWeapon2	=	31,     //

	EInputStrafe			=   10,     //--------------------------------------------------------------------------------------
	EInputAim				=   11,     // these bindings just map to EInputWeapon1 through EInputWeapon10 which aren't used
	EInputRun				=   12,     //
	EInputPunch  			=   13,     //
	EInputKick  			=   14,     //
	EInputTorch  			=   15,     //  NOT USED ON PSP/PS2
	EInputAutoAim  			=   16,     //
	EInputSpecial1 			=   17,     //
	EInputSpecial2 			=   18,     //
	EInputSkyBlend			=	19,     //--------------------------------------------------------------------------------------
#endif
};

enum EInputUIAction
{
	EInputUINone			=	0,
	EInputUIPause			=	1,
	EInputUIScreenshot		=	2,
	EInputUIMenu			=	3,
	EInputUIScoreboard		=	4,
	EInputUISayToAll		=	5,
	EInputUISayToTeam		=	6,
	EInputUIQuickLoad		=	7,
	EInputUIQuickSave		=	8,
	EInputUIFlashlight		=	9,
	EInputUIZoom			=	10,
};

// mouse keys are: 0,1,2 for LEFT,RIGHT,MIDDLE
// mouse status is mouse keys as bits

// eric-> mouse button status bits.  These bits are set in the MouseStatus variable
enum EMouseStatus
{
    EMouseLeftButton        = 1,
    EMouseRightButton       = 2,
    EMouseMiddleButton      = 3,
};


// eric-> these names are simply to identify the mouse keys apart from most other
//   keys.  They were originally mapped onto some windows virtual keys that could
//   never be pressed (on the vast majority of input devices at least), but now
//   i've mapped them to some unused key names.
//   These names are only used to set pressed/released values of keys and then
//   later seperate them between mouse keys and keyboard keys
enum EMouseKeyIndex
{
#if defined( TRANSGAMING ) && ! defined( TRANSGAMING_WINDOWS )
    EMouseKeyLeft		= BUTTON_FAKE_LMOUSE,
    EMouseKeyRight		= BUTTON_FAKE_RMOUSE,
    EMouseKeyMiddle		= BUTTON_FAKE_MMOUSE,
#else
	EVirKeyNumlockEnter = 252,
    EMouseKeyLeft		= VK_LBUTTON,
    EMouseKeyRight		= VK_RBUTTON,
    EMouseKeyMiddle		= VK_MBUTTON,
	EMouseWheelForward	= 253,
	EMouseWheelBack		= 254,
#endif
};


// eric-> these names identify key states and are only used internally in InputSystem.cpp to
//   process queued key events.
enum EKeyStatus
{
    EKeyNone        = 0,
    EKeyPressed     = 1,
    EKeyDown        = 2,
    EKeyUp          = 3,
};

#ifndef TRANSGAMING
enum EKBType
{
	EKBTypeQWERTY	= 1,
	EKBTypeAZERTY	= 2,
	EKBTypeQWERTZ	= 3,
};
#endif


// eric-> these names identify physical gamepad buttons (ie: unmapped).  These are enough
//   names to cover both PSP and PS2, and their values don't actually matter (just the names)
//   They are used to index a button table in the PSPInputSystem.cpp file, and are actually
//   likely not needed since the gamepad code never really worked.
#ifndef TRANSGAMING
enum EGamePadButtons
{
    EPadButton1			= 0,
	EPadButton2			= 1,
	EPadButton3			= 2,
	EPadButton4			= 3,
	EPadTriggerLeft1	= 4,
	EPadTriggerRight1	= 5,
	EPadTriggerLeft2	= 6,
	EPadTriggerRight2	= 7,
	EPadSelect			= 8,
	EPadStart			= 9,
	EPadButtonLeft3		= 10,
	EPadButtonRight3	= 11,
	EPadDPadUp			= 12,
	EPadDPadRight		= 13,
	EPadDPadDown		= 14,
	EPadDPadLeft		= 15,
};
#endif


struct KeyBind
{

#ifdef TRANSGAMING
	KeyBind(){
        action = 0;
        command = NULL;
    }

    ~KeyBind(){
        setCommand(NULL);
    }

    void setCommand(const char *cmd){
        if (command)
            delete []command;

        if (cmd == NULL){
            command = NULL;

            return;
        }


        size_t len = strlen(cmd);
        command = new char[len + 1];
        strncpy(command, cmd, len + 1);
        command[len] = 0;
    }

	DWORD	action;
	char *  command;
#else
	KeyBind() { action = 0; command = ""; }

    DWORD	action;
	String	command;
#endif
};


class ENGINE_API InputSystem
#ifndef TRANSGAMING
    : public MessageHandler
#endif
{
public: 
    // public types
    struct KeyInfo
    {
        int Key;
        int Status;
    };

#ifdef TRANSGAMING
    typedef enum{
        PAD_PROFILE_NONE =  0,
        PAD_PROFILE_1 =     1,
        PAD_PROFILE_2 =     2,
        PAD_PROFILE_3 =     3,
        PAD_PROFILE_4 =     4,
    } ControlProfile;
#endif

public:
    InputSystem(HWND hwnd);
    virtual ~InputSystem() {}

	virtual void Release() {}

#ifndef TRANSGAMING
    // HWND related interface (MessageHandler)
    LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) { return 0; }
    virtual void SetWindow(HWND hwnd);
#endif

    void BindKey(int action, int key, const char *cmd = NULL);
	int GetKeyAction(int key) { return KeyBindings[key].action; };
#ifdef TRANSGAMING
	const char *GetKeyName(int key) { return KeyNames[key].AsChar(); };
	const char *GetKeyNameEng(int key) { return KeyNamesEng[key].AsChar(); };
    int GetKeyByName(const char *name);
	int GetKeyByShortName(const char *name);
	const char *GetKeyNameByEngName(const char *eng) { return GetKeyName( GetKeyByName( eng ) ); }
	const char *GetShortNameByEngName(const char *eng) { return KeyShortNames[GetKeyByName( eng )].AsChar(); }
#else
	String GetKeyName(int key) { return KeyNames[key]; };
	String GetKeyNameEng(int key) { return KeyNamesEng[key]; };
    int GetKeyByName( String name );
	int GetKeyByShortName( String name );
	String GetKeyNameByEngName( String eng ) { return GetKeyName( GetKeyByName( eng ) ); }
	String GetShortNameByEngName( String eng ) { return KeyShortNames[GetKeyByName( eng )]; }
#endif
    BOOL LoadBindings();
    BOOL SaveBindings();
	void ZeroBindings();
    virtual void Reset();
#ifndef TRANSGAMING
	void BindMessageKey( int msg, int key );
#endif

    // interface
    virtual void Tick()=0;
	virtual void LockMouse(BOOL lock) = 0;
	virtual void ShowMouse(BOOL show) = 0;
	virtual BOOL IsMouseLocked() const = 0;
	virtual BOOL IsMouseVisible() const = 0;

	virtual bool UsingGamePad() const = 0;
#ifdef TRANSGAMING
    virtual void setCurrentControlProfile(ControlProfile cp) = 0;
    virtual ControlProfile getCurrentControlProfile() const = 0;

    virtual float getMovementScale() const = 0;
    virtual float getStrafeScale() const = 0;
#endif

    virtual void   SetMouseSensitivity(float msens) = 0;
	virtual float  GetMouseSensitivity() const = 0;
	virtual void   SetMouseSmooth(bool s) = 0;
	virtual bool   GetMouseSmooth() const = 0;
#ifndef TRANSGAMING
	virtual void   SetWheelSensitivity(float msens) = 0;
	virtual float  GetWheelSensitivity() const = 0;
#endif
    virtual Vector GetMouseDelta() const = 0;
    virtual POINT  GetMousePosition() const = 0;
    virtual void   SetMousePosition(int x,int y) = 0;
	virtual void   CenterMouse( void ) = 0;
    virtual void   UpdateMousePosition() = 0;

#ifndef TRANSGAMING
	virtual Vector GetLookDelta() const = 0;
	virtual Vector GetMoveDelta() const = 0;
#endif

    DWORD   GetActionStatus() const { return ActionStatus; }
    void   SetActionStatus(DWORD action) { ActionStatus = action; }
	DWORD   GetUIActionStatus() const { return UIActionStatus; }
    void   SetUIActionStatus(DWORD action) { UIActionStatus = action; }
    DWORD   GetKeyStatus(int key) const { return KeyTable[key].Status; }
    DWORD   GetMouseStatus() const { return MouseStatus; }
	bool	IsFireSwitched() const { return mIsFireSwitched; }
	virtual void	SetUseDInput( bool use ) = 0;
	virtual bool	GetUseDInput() const = 0;
	virtual void	SetDIDeltaScale( float s ) = 0;
	virtual float	GetDIDeltaScale() const = 0;
	void ClearUIBinding( unsigned int key );
#ifndef TRANSGAMING
	void ClearMessageBinding( int key );
#endif

	unsigned int GetPauseKey() const { return mPauseKey; }
	unsigned int GetAltPauseKey() const { return mAltPauseKey; }

	KeyInfo*				GetKeyTable() { return KeyTable; }
	DynamicArray<int>&		GetActiveKeyList() { return ActiveKeyList; }

	virtual bool			IsKeyPrintable( unsigned char key ) = 0;
	virtual unsigned char	KeyToPrintable( unsigned char key ) = 0;

#ifndef TRANSGAMING
	virtual int				GetKBType() = 0;
#endif

	bool	InvertMouse;


#ifdef TRANSGAMING
	void SetLuaToDo(const char *todo, bool copy = false);
    void QueueLuaToDo(const char *todo);
#else
	void SetLuaToDo( String todo ) { mLuaToDo = todo; }
#endif

protected:

    // binding table returns action
#ifdef TRANSGAMING_PSP
	KeyBind KeyBindings[BUTTON_COUNT];   // index=virtual key code, e.g. KeyBindings[VK_UP]=action
	String  KeyNames[BUTTON_COUNT];      // KeyBindings[VK_UP] = "Cursor Up"
	String  KeyNamesEng[BUTTON_COUNT];
	String  KeyShortNames[BUTTON_COUNT];

#else
	KeyBind KeyBindings[256]; // index=virtual key code, e.g. KeyBindings[VK_UP]=action
	String KeyNames[256];	// KeyBindings[VK_UP] = "Cursor Up"
	String KeyNamesEng[256];
	String KeyShortNames[256];

	int MessagesKeys[18];
#endif

    // action enum bits set
    DWORD ActionStatus;
	DWORD UIActionStatus;

    //
    static const int MaxEventQueueSize=64;

    struct InputEvent
    {
        InputEvent() 
        {}
        InputEvent(int eventType, int eventData)
        :   EventType(eventType), EventData(eventData)
        {}

        int EventType;
        int EventData;
    };

    enum
    {
        EEventKeyDown       = 1,
        EEventKeyUp         = 2,
        EEventMouseDown     = 3,
        EEventMouseUp       = 4,
    };

    InputEvent  EventQueue[MaxEventQueueSize];
    int         EventQueueSize;

#ifndef TRANSGAMING     //this buffer never seems to be used
    DWORD   KeyStatus[256];
#endif
    DWORD   MouseStatus;    

#ifdef TRANSGAMING
    KeyInfo					KeyTable[BUTTON_COUNT];
#else
    KeyInfo					KeyTable[256];
#endif
    DynamicArray<int>		ActiveKeyList;

	unsigned int	mFireSwitchKey;
	unsigned int	mFireSwitchToggleKey;
	unsigned int	mAltFireSwitchKey;
	unsigned int	mAltFireSwitchToggleKey;
	unsigned int	mPauseKey;
	unsigned int	mScreenshotKey;
	unsigned int	mMenuKey;
	unsigned int	mAltPauseKey;
	unsigned int	mAltScreenshotKey;
	unsigned int	mAltMenuKey;
	unsigned int	mScoreboardKey;
	unsigned int	mAltScoreboardKey;
	unsigned int	mSayToAllKey;
	unsigned int	mAltSayToAllKey;
	unsigned int	mSayToTeamKey;
	unsigned int	mAltSayToTeamKey;
	unsigned int	mQuickLoadKey;
	unsigned int	mAltQuickLoadKey;
	unsigned int	mQuickSaveKey;
	unsigned int	mAltQuickSaveKey;
	unsigned int	mFlashlightKey;
	unsigned int	mAltFlashlightKey;
	unsigned int	mZoomKey;
	unsigned int	mAltZoomKey;

	bool			mIsFireSwitched;

#ifdef TRANSGAMING
    static const int        LUA_TODO_MAX = 16;
	int                     mLuaToDoCount;
    bool                    mLuaDeleteFirst;
    const char *            mLuaToDo[LUA_TODO_MAX];
#else
	String                  mLuaToDo;
#endif

#ifdef TRANSGAMING
    void SendEvent(int eventType, int eventData);
#else
    void SendEvent(const InputEvent& event);
#endif
    void ProcessEvents();
	virtual void FillKeyNamesTable( void )
	{
#ifdef TRANSGAMING
		for( int i = 0; i < BUTTON_COUNT; ++i )
#else
		for( int i = 0; i < 256; ++i )
#endif
		{
			KeyNames[i] = "NoName Key";
			KeyNamesEng[i] = "NoName Key";
		}
	}
};


#endif //_INPUTSYSTEM_

