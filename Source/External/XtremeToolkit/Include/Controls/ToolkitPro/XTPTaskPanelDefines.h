// XTPTaskPanelDefines.h
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
#if !defined(__XTPTASKPANELDEFINES_H__)
#define __XTPTASKPANELDEFINES_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

// Summary: Task Panel item types.
enum XTPTaskPanelItemType
{
	xtpTaskItemTypeGroup,		// Item is group.
	xtpTaskItemTypeLink,		// Item is link.
	xtpTaskItemTypeText,		// Item is text.
	xtpTaskItemTypeControl		// Item is control.
};

enum XTPTaskPanelItemLayout
{
	xtpTaskItemLayoutDefault,
	xtpTaskItemLayoutImages,
	xtpTaskItemLayoutImagesWithText,
	xtpTaskItemLayoutImagesWithTextBelow

};

enum XTPTaskPanelHotTrackStyle
{
	xtpTaskPanelHighlightDefault,
	xtpTaskPanelHighlightText,
	xtpTaskPanelHighlightImage,
	xtpTaskPanelHighlightItem,
	xtpTaskPanelHighlightNone
};

enum XTPTaskPanelBehaviour
{
	xtpTaskPanelBehaviourExplorer,
	xtpTaskPanelBehaviourList,
	xtpTaskPanelBehaviourToolbox
};

// Summary: XTPTaskPanelPaintTheme is an enumeration used by CXTPTaskPanel to set the theme of the control.
enum XTPTaskPanelPaintTheme 
{
	xtpTaskPanelThemeOffice2000,			// Office 2000 theme
	xtpTaskPanelThemeOffice2003,			// Office 2003 theme
	xtpTaskPanelThemeNativeWinXP,			// Win XP theme

	xtpTaskPanelThemeOffice2000Plain,		// Office 2000 plain theme
	xtpTaskPanelThemeOfficeXPPlain,			// Office XP plain theme
	xtpTaskPanelThemeOffice2003Plain,		// Office 2003 plain theme
	xtpTaskPanelThemeNativeWinXPPlain,		// Win XP plain theme

	xtpTaskPanelThemeToolbox, 
	xtpTaskPanelThemeToolboxWhidbey, 
	xtpTaskPanelThemeListView, 
	xtpTaskPanelThemeListViewOfficeXP, 
	xtpTaskPanelThemeListViewOffice2003, 

	xtpTaskPanelThemeCustom
};


// Summary: XTPTaskPanelAnimation is an enumeration used by CXTPTaskPanel to set animation of the control.
enum XTPTaskPanelAnimation
{
	xtpTaskPanelAnimationYes,				// Enable animation
	xtpTaskPanelAnimationNo,				// Disable animation 
	xtpTaskPanelAnimationSystem				// Use system settings to determine animation
};

const int XTP_ACTION_CANCEL = -1;

const UINT XTP_TPN_CLICK = 1;
const UINT XTP_TPN_RCLICK = 2;
const UINT XTP_TPN_STARTDRAG = 3;
const UINT XTP_TPN_ENDLABELEDIT = 3;

// Input:   nAction -  Value of wParam specifies a TaskPanel value that indicates the user's 
//			request. This parameter can be one of the following values:
//			[ul]
//			[li]<b>XTP_TPN_CLICK</b> User click TaskPanel item.[/li]
//			[li]<b>XTP_TPN_RCLICK</b> User presses the right mouse button on the TaskPanel item.[/li]
//			[/ul]
//			pItem - The value of lParam points to an CXTPTaskPanelItem object that contains information for the 
//			specified item. This pointer should <b>never</b> be NULL.
// Returns: If the application is to process this message, the return value should be TRUE, otherwise the 
//			return value is FALSE. 
//
// Summary: The XTPWM_TASKPANEL_NOTIFY message is sent to the CXTPTaskPanel owner window 
//			whenever an action occurs within the CXTPTaskPanel
//   
//			<pre>XTPWM_SHORTCUTBAR_NOTIFY 
//			nAction = (int) wParam;             // TaskPanel action 
//			pItem = (CXTPTaskPanelItem*) lParam;  // pointer to an CXTPTaskPanelItem object</pre>

const UINT XTPWM_TASKPANEL_NOTIFY = (WM_APP + 2584);

#endif // !defined(__XTPTASKPANELDEFINES_H__)
