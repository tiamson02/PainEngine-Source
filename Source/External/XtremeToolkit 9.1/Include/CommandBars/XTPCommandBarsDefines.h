// XTPCommandBarDefines.h
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO 
// BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED 
// WRITTEN CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS 
// OUTLINED IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE 
// GRANTS TO YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE 
// THIS SOFTWARE ON A SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//:Ignore
#if !defined(__XTPCOMMANDBARDEFINES_H__)
#define __XTPCOMMANDBARDEFINES_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

// Summary: Position of the command bar
enum XTPBarPosition 
{
	xtpBarTop = 0,		// Docked at top.
	xtpBarBottom = 1,	// Docked at bottom.
	xtpBarLeft = 2,		// Docked at left.
	xtpBarRight = 3,	// Docked at right.
	xtpBarFloating = 4,	// Floated.
	xtpBarPopup = 5,	// Popup.
	xtpBarListBox = 6,	// List box.
	xtpBarNone = 7		// None.
};

// Summary: Type of the command bar.
enum XTPBarType 
{
	xtpBarTypeMenuBar, // Command bar is a menu bar.
	xtpBarTypeNormal,  // Command bar is a toolbar.
	xtpBarTypePopup    // Command bar is a popup.
};

// Summary: Type of the control.
enum XTPControlType 
{
	xtpControlError,			// Type is not defined.
	xtpControlButton,			// Button type.
	xtpControlPopup,			// Popup type.
	xtpControlButtonPopup,		// Button popup.
	xtpControlSplitButtonPopup, // Split button popup.
	xtpControlComboBox,			// Combo box button.
	xtpControlEdit,				// Edit control.
	xtpControlCustom,			// Custom control.
	xtpControlLabel,
};

// Summary: Flags of the control.
enum XTPControlFlags 
{
	xtpFlagRightAlign = 1,	 // Control is right aligned.
	xtpFlagSkipFocus = 2,    // Control does not have focus.
	xtpFlagLeftPopup = 4, 	 // To Pop up child bar left.
	xtpFlagManualUpdate = 8, // Control is manually updated.
	xtpFlagNoMovable = 16    // To disable customization.
};

// Summary: Control's hide flags 
enum XTPControlHideFlags 
{
	xtpNoHide = 0,				// Control is visible.
	xtpHideGeneric = 1,			// Control is hidden by generic reason.
	xtpHideWrap = 2,			// Control is hidden by wrap.
	xtpHideDockingPosition = 4, // Control is hidden by docking position.
	xtpHideScroll = 8,			// Control is hidden by scrolling.
	xtpHideCustomize = 16,		// Control is hidden by customize settings.
	xtpHideExpand = 32,
	xtpHideDocTemplate = 64
};

// Summary: Button's styles
enum XTPButtonStyle 
{
	xtpButtonAutomatic = 0,		// Automatic style.
	xtpButtonCaption = 1,		// Button draw only caption.
	xtpButtonIcon = 2,			// Button draw only icon.
	xtpButtonIconAndCaption = 3	// Button draw icon and caption.
};

// Summary: Combobox's styles
enum XTPComboStyle 
{
	xtpComboNormal,		// Normal combo box.
	xtpComboLabel		// Combo box with label.
};

// Summary: Paint themes.
enum XTPPaintTheme 
{
	xtpThemeOfficeXP,		// Office XP theme.
	xtpThemeOffice2000,		// Office 2000 theme.
	xtpThemeOffice2003,		// Office 2003 theme.
	xtpThemeNativeWinXP,	// Windows XP themes support.
	xtpThemeCustom			// Custom theme.
};

// Summary: Menu flags.
enum XTPMenuBarFlags 
{
	xtpFlagHideMinimizeBox      = 0x0100L,			// To hide minimize box.
	xtpFlagHideMaximizeBox      = 0x0200L,			// To hide maximize box.
	xtpFlagIgnoreSetMenuMessage = 0x0400L,			// To ignore MDI menus. 
	xtpFlagUseMDIMenus          = 0x0800L,			// To use MDI menus. Not longer used.
	xtpFlagHideClose	        = 0x1000L,			// To hide close button.
	xtpFlagHideMDIButtons = xtpFlagHideMinimizeBox|xtpFlagHideMaximizeBox|xtpFlagHideClose,
	xtpFlagAddMDISysPopup       = 0x2000L			// To add system MDI popup bar.
};

// Summary: Docking flags.
enum XTPToolBarFlags 
{
	xtpFlagAlignTop = 1,                // Allows docking at the top of the client area. 
	xtpFlagAlignBottom = 2,	            // Allows docking at the bottom of the client area. 
	xtpFlagAlignLeft = 4,               // Allows docking on the left side of the client area. 
	xtpFlagAlignRight = 8,              // Allows docking on the left side of the client area. 
	xtpFlagAlignAny = xtpFlagAlignTop |	xtpFlagAlignBottom | xtpFlagAlignLeft | xtpFlagAlignRight, // Allows docking on any side of the client area. 
	xtpFlagFloating = 16,               // Allows floating. 
	xtpFlagHideWrap = 32,               // Allow to hide wrapped controls.
	xtpFlagStretched = 64				// Bar is stretched.
};

// Summary: Enumerated type used to determine the animation effect of popup bars.
enum XTPAnimationType
{
	xtpAnimateWindowsDefault,	// As defined in the "Display" settings.
	xtpAnimateRandom,			// Any of the first three in random selection.
	xtpAnimateUnfold,			// Unfold top to bottom.
	xtpAnimateSlide,			// Slide in from left.
	xtpAnimateFade,				// Fade-in.
	xtpAnimateNone,				// No animation.
};

//:Ignore

//Internal enumerators

// Docking direction enumerator.
enum XTPPopupDirection {xtpPopupRight = 0, xtpPopupLeft = 1, xtpPopupDown = 2};

// Special keys enumerator.
enum XTPSpecialKey {xtpKeyNext, xtpKeyPrev, xtpKeyBack, xtpKeyPopup, xtpKeyEscape, xtpKeyReturn, xtpKeyHome, xtpKeyEnd};

// Flags for idle routine.
enum XTPIdleFlags {idleLayout = 1, idleRedraw = 2};

#define TRUE_POPUP 2
#define TRUE_KEYBOARD 2
#define FALSE_EXIT 2

#define LM_HIDEWRAP 0x80
#define LM_POPUP 0x100

#define WM_XTP_COMMAND (WM_USER + 301)
#define WM_XTP_UPDATE_COMMAND (WM_USER + 302)
#define WM_XTP_CREATECONTROL (WM_USER + 303)
#define WM_XTP_CREATECOMMANDBAR (WM_USER + 304)
#define WM_XTP_CONTROLSELECTED (WM_USER + 305)
#define WM_XTP_DRAGCONTROLCHANGED (WM_USER + 306)
#define WM_XTP_INITCOMMANDSPOPUP (WM_USER + 307)
#define WM_XTP_INITMENU (WM_USER + 308)

//:End Ignore

#define _EMBOSSED_STYLE

#define ON_XTP_INITCOMMANDSPOPUP()\
	{ WM_XTP_INITCOMMANDSPOPUP, 0, 0, 0, AfxSig_vs, \
	(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(CXTPPopupBar*))&OnInitCommandsPopup } ,


class CXTPControl;
class CXTPCommandBar;
class CXTPPopupBar;

// Summary: NMXTPCONTROL struct.
typedef struct {
    NMHDR   hdr;			// Header.
	CXTPControl* pControl;	// Control.
} NMXTPCONTROL, FAR* LPNMXTPCONTROL;	

#define CBN_XTP_EXECUTE 100

// This macro maps an execute message to a member function.
#define ON_XTP_EXECUTE(id, memberFxn) \
    ON_NOTIFY(CBN_XTP_EXECUTE, id, memberFxn)	

// Use this macro to map a contiguous range of command IDs to a single message handler function.
#define ON_XTP_EXECUTE_RANGE(id, idLast, memberFxn) ON_NOTIFY_EX_RANGE(CBN_XTP_EXECUTE, id, idLast, memberFxn)
	

//Create control structure.
typedef struct {
	UINT nID;						// Identifier of the control to be created.
	CXTPControl* pControl;			// Control to be created.
	BOOL bToolBar;					// TRUE if control is toolbar located.
	int nIndex;						// Index of the control to be created.
	CMenu* pMenu;					// A Pointer to CMenu object.
	CXTPCommandBar* pCommandBar;	// Pointer to the parent command bar class.
	CString strCaption;				// Caption of the control to be created.
	XTPControlType controlType;		// Type of the control.
	XTPButtonStyle buttonStyle;		// Button Style of the control.
} 
XTP_COMMANDBARS_CREATECONTROL, FAR* LPCREATECONTROLSTRUCT;

// This macro maps a create control event to a member function.
#define ON_XTP_CREATECONTROL()\
	{ WM_XTP_CREATECONTROL, 0, 0, 0, AfxSig_is, \
	(AFX_PMSG)(AFX_PMSGW)(int (AFX_MSG_CALL CWnd::*)(LPCREATECONTROLSTRUCT))&OnCreateControl } ,

// Create bar structure
typedef struct {
	CXTPCommandBar* pCommandBar;	// Created command bar.
	BOOL bPopup;					// TRUE if it is a popup command bar,
	BOOL bExpandBar;				// TRUE if it is an expanded popup.
	BOOL bCustomBar;				// TRUE if it is a user defined toolbar.
	BOOL bTearOffBar;				// TRUE if it is a user tear-off popup.
	LPCTSTR lpcstrCaption;			// Caption of the command bar.
	UINT nID;						// Identifier of the command bar.
} 
XTP_COMMANDBARS_CREATEBAR, FAR* LPCREATEBARSTRUCT;

// This macro maps a create command bar event to a member function.
#define ON_XTP_CREATECOMMANDBAR()\
	{ WM_XTP_CREATECOMMANDBAR, 0, 0, 0, AfxSig_is, \
	(AFX_PMSG)(AFX_PMSGW)(int (AFX_MSG_CALL CWnd::*)(LPCREATEBARSTRUCT))&OnCreateCommandBar} ,

inline BOOL IsDockingPosition(XTPBarPosition barPosition)
{
	return barPosition == xtpBarLeft || barPosition == xtpBarTop || 
		barPosition == xtpBarRight || barPosition == xtpBarBottom;
}

inline BOOL IsCaptionStyle(XTPButtonStyle buttonStyle)
{
	return buttonStyle == xtpButtonCaption || buttonStyle == xtpButtonIconAndCaption;
}
inline BOOL IsPopupControlType(XTPControlType controlType)
{
	return controlType == xtpControlPopup || controlType == xtpControlButtonPopup || controlType == xtpControlSplitButtonPopup;
}
inline BOOL IsVerticalPosition(XTPBarPosition barPosition)
{
	return barPosition == xtpBarLeft || barPosition == xtpBarRight || barPosition == xtpBarPopup;
}

inline BOOL IsAnimateType(XTPAnimationType animationType)
{
	return animationType == xtpAnimateFade || animationType == xtpAnimateSlide || animationType == xtpAnimateUnfold;
}

// Summary: HookAble interface.
class IHookAble
{
public:
	// Input:	hWnd - Window handle that the message belongs to.
	//			nMessage - Specifies the message to be sent. 
	//			wParam - Specifies additional message-dependent information. 
	//			lParam - Specifies additional message-dependent information.
	//			lResult - The return value of WindowProc. Depends on the message; may be NULL. 
	// Summary: This member function is called by WindowProc, or is called during message reflection.
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult) = 0;
};

#define _XTP_SCHEMA_71  (0x1)
#define _XTP_SCHEMA_80  (0x2)
#define _XTP_SCHEMA_85  (0x3)
#define _XTP_SCHEMA_86  (0x4)
#define _XTP_SCHEMA_861 (0x5)
#define _XTP_SCHEMA_87  (0x6)
#define _XTP_SCHEMA_875 (0x7)

#define _XTP_SCHEMA_CURRENT _XTP_SCHEMA_875

#define DECLARE_XTP_CONTROL(class_name) \
	DECLARE_SERIAL(class_name)\
	virtual CXTPControl* Clone(BOOL bRecursive);

#define IMPLEMENT_XTP_CONTROL(class_name, base_class_name) \
	IMPLEMENT_SERIAL( class_name, base_class_name, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)\
	CXTPControl* class_name::Clone(BOOL bRecursive) {\
		class_name* pButton = (class_name*)class_name::CreateObject();\
		pButton->Copy(this, bRecursive);\
		return pButton;\
	}

#define DECLARE_XTP_COMMANDBAR(class_name) \
	DECLARE_SERIAL(class_name)\
	virtual CXTPCommandBar* Clone(BOOL bRecursive);


#define IMPLEMENT_XTP_COMMANDBAR(class_name, base_class_name) \
	IMPLEMENT_SERIAL(class_name, base_class_name, VERSIONABLE_SCHEMA | _XTP_SCHEMA_CURRENT)\
	CXTPCommandBar* class_name::Clone(BOOL bRecursive) {\
		class_name* pCommandBar = (class_name*)class_name::CreateObject();\
		pCommandBar->Copy(this, bRecursive);\
		return pCommandBar;\
	}

#define CMDTARGET_RELEASE(x) if (x) { x->InternalRelease(); x = 0;}
#define CMDTARGET_ADDREF(x) if (x) { x->InternalAddRef(); } 

#define SAFE_INVALIDATE(pWnd) if (pWnd && pWnd->GetSafeHwnd()) pWnd->Invalidate(FALSE);

#endif //#if !defined(__XTPCOMMANDBARDEFINES_H__)
