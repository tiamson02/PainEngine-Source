// XTNotify.h : notification handlers
//
// This file is a part of the XTREME TOOLKIT MFC class library.
// ©1998-2004 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE 
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN 
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED 
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT.  CODEJOCK SOFTWARE GRANTS TO 
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A 
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__XTNOTIFY_H__)
#define __XTNOTIFY_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			color selection has changed.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_CPN_XT_SELCHANGE(IDC_CTRL_ID, OnSelChange)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnSelChange()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnSelChange( );</b></pre>
//===========================================================================
const UINT CPN_XT_SELCHANGE = (WM_USER + 2500); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			color selection window is displayed.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_CPN_XT_DROPDOWN(IDC_CTRL_ID, OnDropDown)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//
//			<pre>void CAppDialog::OnDropDown()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnDropDown( );</b></pre>
//===========================================================================
const UINT CPN_XT_DROPDOWN = (WM_USER + 2501); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			color selection window has closed.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_CPN_XT_CLOSEUP(IDC_CTRL_ID, OnCloseUp)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnCloseUp()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnCloseUp( );</b></pre>
//===========================================================================
const UINT CPN_XT_CLOSEUP = (WM_USER + 2502); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever a
//			color selection has been made.
//
// Example:	Here is an example of how an application would handle this message. 
// 
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_CPN_XT_SELENDOK(IDC_CTRL_ID, OnSelEndOk)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnSelEndOk()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnSelEndOk( );</b></pre>
//===========================================================================
const UINT CPN_XT_SELENDOK = (WM_USER + 2503); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever a
//			color selection has been canceled.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_CPN_XT_SELENDCANCEL(IDC_CTRL_ID, OnSelEndCancel)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnSelEndCancel()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnSelEndCancel( );</b></pre>
//===========================================================================
const UINT CPN_XT_SELENDCANCEL = (WM_USER + 2504); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			no fill / automatic color selection has been made.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_CPN_XT_SELNOFILL(IDC_CTRL_ID, OnSelNoFill)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnSelNoFill()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnSelNoFill( );</b></pre>
//===========================================================================
const UINT CPN_XT_SELNOFILL = (WM_USER + 2505); 

#define ON_CPN_XT_SELCHANGE(id, memberFxn) \
    ON_CONTROL(CPN_XT_SELCHANGE, id, memberFxn)
#define ON_CPN_XT_DROPDOWN(id, memberFxn) \
    ON_CONTROL(CPN_XT_DROPDOWN, id, memberFxn)
#define ON_CPN_XT_CLOSEUP(id, memberFxn) \
    ON_CONTROL(CPN_XT_CLOSEUP, id, memberFxn)
#define ON_CPN_XT_SELENDOK(id, memberFxn) \
    ON_CONTROL(CPN_XT_SELENDOK, id, memberFxn)
#define ON_CPN_XT_SELENDCANCEL(id, memberFxn) \
    ON_CONTROL(CPN_XT_SELENDCANCEL, id, memberFxn)
#define ON_CPN_XT_SELNOFILL(id, memberFxn) \
    ON_CONTROL(CPN_XT_SELNOFILL, id, memberFxn)

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			label edit operation has ended.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_BEN_XT_LABELEDITEND(IDC_CTRL_ID, OnLabelEditEnd)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnLabelEditEnd()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnLabelEditEnd( );</b></pre>
//===========================================================================
const UINT BEN_XT_LABELEDITEND = (WM_USER + 2506); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			label edit operation has been canceled.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_BEN_XT_LABELEDITCANCEL(IDC_CTRL_ID, OnLabelEditCancel)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnLabelEditCancel()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnLabelEditCancel( );</b></pre>
//===========================================================================
const UINT BEN_XT_LABELEDITCANCEL = (WM_USER + 2507); 

#define ON_BEN_XT_LABELEDITEND(id, memberFxn) \
    ON_CONTROL(BEN_XT_LABELEDITEND, id, memberFxn)
#define ON_BEN_XT_LABELEDITCANCEL(id, memberFxn) \
    ON_CONTROL(BEN_XT_LABELEDITCANCEL, id, memberFxn)

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			label edit operation has ended.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_LBN_XT_LABELEDITEND(IDC_CTRL_ID, OnLabelEditEnd)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnLabelEditEnd()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnLabelEditEnd( );</b></pre>
//===========================================================================
const UINT LBN_XT_LABELEDITEND = BEN_XT_LABELEDITEND; 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			label edit operation has been canceled.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_LBN_XT_LABELEDITCANCEL(IDC_CTRL_ID, OnLabelEditCancel)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnLabelEditCancel()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnLabelEditCancel( );</b></pre>
//===========================================================================
const UINT LBN_XT_LABELEDITCANCEL = BEN_XT_LABELEDITCANCEL; 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			new item button has been pressed.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_LBN_XT_NEWITEM(IDC_CTRL_ID, OnNewItem)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnNewItem()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnNewItem( );</b></pre>
//===========================================================================
const UINT LBN_XT_NEWITEM = (WM_USER + 2508); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner just before an item
//			is deleted.  This is useful for retrieving information about the selected
//			item, such as item data, before it is removed from the edit list control.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_LBN_XT_PREDELETEITEM(IDC_CTRL_ID, OnPreDeleteItem)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnPreDeleteItem()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnPreDeleteItem( );</b></pre>
//===========================================================================
const UINT LBN_XT_PREDELETEITEM = (WM_USER + 2509); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			delete item button has been pressed.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_LBN_XT_DELETEITEM(IDC_CTRL_ID, OnDeleteItem)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnDeleteItem()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnDeleteItem( );</b></pre>
//===========================================================================
const UINT LBN_XT_DELETEITEM = (WM_USER + 2510); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			move item up button has been pressed.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_LBN_XT_MOVEITEMUP(IDC_CTRL_ID, OnMoveItemUp)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnMoveItemUp()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnMoveItemUp( );</b></pre>
//===========================================================================
const UINT LBN_XT_MOVEITEMUP = (WM_USER + 2511); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			move item down button has been pressed.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_LBN_XT_MOVEITEMDOWN(IDC_CTRL_ID, OnMoveItemDown)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnMoveItemDown()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnMoveItemDown( );</b></pre>
//===========================================================================
const UINT LBN_XT_MOVEITEMDOWN = (WM_USER + 2512); 

#define ON_LBN_XT_LABELEDITEND(id, memberFxn) \
    ON_CONTROL(LBN_XT_LABELEDITEND, id, memberFxn)
#define ON_LBN_XT_LABELEDITCANCEL(id, memberFxn) \
    ON_CONTROL(LBN_XT_LABELEDITCANCEL, id, memberFxn)
#define ON_LBN_XT_NEWITEM(id, memberFxn) \
    ON_CONTROL(LBN_XT_NEWITEM, id, memberFxn)
#define ON_LBN_XT_PREDELETEITEM(id, memberFxn) \
    ON_CONTROL(LBN_XT_PREDELETEITEM, id, memberFxn)
#define ON_LBN_XT_DELETEITEM(id, memberFxn) \
    ON_CONTROL(LBN_XT_DELETEITEM, id, memberFxn)
#define ON_LBN_XT_MOVEITEMUP(id, memberFxn) \
    ON_CONTROL(LBN_XT_MOVEITEMUP, id, memberFxn)
#define ON_LBN_XT_MOVEITEMDOWN(id, memberFxn) \
    ON_CONTROL(LBN_XT_MOVEITEMDOWN, id, memberFxn)

//===========================================================================
// Input:	uID		  - Value of wParam specifies the resource ID of the icon associated with the
//						CXTTrayIcon object.
//			uMouseMsg - Value of lParam specifies the mouse or keyboard message associated with the event.
//
// Returns:	If the application is to process this message, the return value should be 0.
//
// Remarks:	When a mouse or keyboard event occurs on a tray icon, the TIN_XT_TRAYICON
//			message is sent to the tray icon's owner window. 
//
// Example:	Here is an example of how an application would process the TIN_XT_TRAYICON 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//			    //{{AFX_MSG_MAP(CMainFrame)
//			    ON_MESSAGE(TIN_XT_TRAYICON, OnTrayIconNotify)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CMainFrame::OnTrayIconNotify(WPARAM wParam, LPARAM lParam) 
//			{
//			    UINT uID = (UINT) wParam;        // resource ID of the tray icon.
//			    UINT uMouseMsg = (UINT) lParam;  // mouse message that was sent.
//				
//			    switch( uMouseMsg )
//			    {
//			        case WM_RBUTTONUP:
//			        {
//			            CMenu menu;
//			            VERIFY(menu.LoadMenu(IDR_MAINFRAME));
//						
//			            CMenu* pPopup = menu.GetSubMenu(0);
//			            ASSERT(pPopup != NULL);
//			            CWnd* pWndPopupOwner = this;
//						
//			            // Insert the restore menu command into the popup.
//			            if (m_bIsVisible == false)
//			            {
//			                pPopup->InsertMenu(0, MF_BYPOSITION,
//			                    IDR_RESTORE, _T("&Restore Window..."));
//			
//			                pPopup->InsertMenu(1, MF_BYPOSITION|MF_SEPARATOR,
//			                    IDR_RESTORE);
//			
//			                // Make restore command bold.
//			                ::SetMenuDefaultItem(pPopup->m_hMenu, IDR_RESTORE, FALSE);
//			            }
//			            else
//			            {
//			                // Make the exit command bold.
//			                ::SetMenuDefaultItem(pPopup->m_hMenu, ID_APP_EXIT, FALSE);
//			            }
//			
//			            // Display the menu at the current mouse location. There's a "bug"
//			            // (Microsoft calls it a feature) in Windows 95 that requires calling
//			            // SetForegroundWindow. To find out more, search for Q135788 in MSDN.
//			            //
//			            CPoint point;
//			            GetCursorPos( &point );
//			            ::SetForegroundWindow(m_hWnd);
//			
//			            while (pWndPopupOwner->GetStyle() & WS_CHILD)
//			                pWndPopupOwner = pWndPopupOwner->GetParent();
//			
//			            int iCmd = pPopup->TrackPopupMenu(
//			                TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
//			                point.x, point.y, pWndPopupOwner);
//			
//			            // at this point we want to make sure that the app is visible
//			            // after the user makes a menu selection - this is just one way
//			            // of doing so:
//			
//			            switch (iCmd)
//			            {
//			                case 0:
//			                    // user cancelled menu - do nothing
//			                    break;
//			
//			                // these are commands for which we don't need to make the
//			                // main app window visible
//			                case ID_APP_EXIT:
//			                    PostMessage(WM_COMMAND, iCmd, 0);  // just post the command
//			                    break;
//			
//			                // for all other menu commands - make sure the window is
//			                // visible before we continue
//			                default:
//			                    // make main window visible
//			                    OnRestore();
//			                    PostMessage(WM_COMMAND, iCmd, 0); // post the command
//			                    break;
//			            }
//			
//			            return 1; // let the tray icon know that we handled this message.
//			        }
//			
//			        case WM_LBUTTONDBLCLK:
//			            OnRestore();
//			            return 1; // let the tray icon know that we handled this message.
//			    }
//			
//			    return 0;
//			}</pre>
// Summary: The TIN_XT_TRAYICON message is sent to the CXTTrayIcon window's owner
//			whenever a mouse event occurs in the CXTTrayIcon. 
//   
//			<pre>TIN_XT_TRAYICON 
//			uID = (UINT) wParam;        // resource ID of the tray icon.
//			uMouseMsg = (UINT) lParam;  // mouse message that was sent.</pre>
//===========================================================================
const UINT TIN_XT_TRAYICON = (WM_USER + 2513); 

//===========================================================================
// Input:	pBar - Value of wParam specifies a CXTControlBar object that represents the toolbar
//			where the context menu event occurred.
//  
// Returns:	If the application is to process this message, the return value should be 0.
//
// Remarks:	When the user right clicks on a control bar, the CBRN_XT_CONTEXTMENU
//			message is sent to the control bar's owner window. 
//
// Example:	Here is an example of how an application would process the CBRN_XT_CONTEXTMENU 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//			    //{{AFX_MSG_MAP(CMainFrame)
//			    ON_MESSAGE(CBRN_XT_CONTEXTMENU, OnBarContextMenu)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CMainFrame::OnBarContextMenu(WPARAM wParam, LPARAM lParam)
//			{
//			    UNREFERENCED_PARAMETER(lParam);
//			
//			    CXTControlBar* pBar = (CXTControlBar*)wParam;
//			    ASSERT_KINDOF(CXTControlBar, pBar);
//			
//			    return 0;
//			}</pre>
//
// Summary: The CBRN_XT_CONTEXTMENU message is sent to the CXTControlBar window's owner
//			whenever a context menu event occurs in the CXTControlBar. 
//   
//			<pre>CBRN_XT_CONTEXTMENU 
//			pBar = (NMTOOLBAR*)wParam;  // pointer to the control bar context menu owner</pre>
//===========================================================================
const UINT CBRN_XT_CONTEXTMENU = (WM_USER + 2514); 

//===========================================================================
// Input:	pNMTB - Value of wParam specifies a NMTOOLBAR structure that represents the toolbar
//					Button where the dropdown event occurred.
//
//			pRect - Value of lParam points to a CRect object that represents the size and location
//					of the toolbar button where the dropdown event occurred.
//
// Returns:	Dropdown is displaying a modal window, the return value should be 0, otherwise
//			return 1 for menu style displays.
//
// Remarks:	When the user clicks on a dropdown arrow for a toolbar button, the CBRN_XT_DROPDOWN
//			message is sent to the toolbar's owner window. 
//
// Example:	Here is an example of how an application would process the CBRN_XT_DROPDOWN 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//			    //{{AFX_MSG_MAP(CMainFrame)
//			    ON_MESSAGE(CBRN_XT_DROPDOWN, OnDropDown)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CMainFrame::OnDropDown(WPARAM wParam, LPARAM lParam)
//			{
//			    NMTOOLBAR* pNMTB = (NMTOOLBAR*)wParam;
//			    ASSERT(pNMTB != NULL);
//			
//			    CRect* pRect = (CRect*)lParam;
//			    ASSERT(pRect != NULL);
//			
//			    // TODO: Handle toolbar dropdown notification (click on REDO drop arrow).
//			
//			    return 0;
//			}</pre>
// Summary: The CBRN_XT_DROPDOWN message is sent to the CXTToolBar window 
//			whenever a dropdown event occurs in the CXTToolBar. 
//   
//			<pre>CBRN_XT_DROPDOWN 
//			pNMTB = (NMTOOLBAR*)wParam;  // pointer to a NMTOOLBAR struct 
//			pRect = (CRect*)lParam;      // pointer to a CRect object</pre>
//===========================================================================
const UINT CBRN_XT_DROPDOWN = (WM_USER + 2515); 

//===========================================================================
// Remarks:	When the user makes a selection of the push pin button, the CPWN_XT_PUSHPINBUTTON message is 
//			sent to the caption popup window's owner window. 
//
// Example:	Here is an example of how an application would process the TCN_XT_SELCHANGE 
//			message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//			    //{{AFX_MSG_MAP(CMainFrame)
//			    ON_MESSAGE_VOID(CPWN_XT_PUSHPINBUTTON, OnPushPinButton)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CMainFrame::OnPushPinButton()
//			{
//			    // TODO: Add your message handler code here and/or call default
//			    m_wndSplitter2.ShowColumn();
//			    m_nColumn = 1;
//			}</pre>
// Summary: The CPWN_XT_PUSHPINBUTTON message is sent to the owner of a CXTCaptionPopupWnd whenever
//			the push pin button selection has been made.
//===========================================================================
const UINT CPWN_XT_PUSHPINBUTTON = (WM_USER + 2516); 

//===========================================================================
// Remarks:	When the user cancels a selection of the push pin button, the CPWN_XT_PUSHPINCANCEL message is 
//			sent to the caption popup window's owner window. 
//
// Example:	Here is an example of how an application would process the TCN_XT_SELCHANGE 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//			    //{{AFX_MSG_MAP(CMainFrame)
//			    ON_MESSAGE_VOID(CPWN_XT_PUSHPINCANCEL, OnPushPinCancel)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CMainFrame::OnPushPinCancel()
//			{
//			    // TODO: Add your message handler code here and/or call default
//			}</pre>
// Summary: The CPWN_XT_PUSHPINCANCEL message is sent to the owner of a CXTCaptionPopupWnd whenever
//			the push pin button selection has been canceled.
//===========================================================================
const UINT CPWN_XT_PUSHPINCANCEL = (WM_USER + 2517); 

// BULLETED LIST:

//===========================================================================
// Input:	nBarAction - Value of wParam specifies an Outlook bar value that indicates the users 
//						 request. This parameter can be one of the following values:
//			[ul]
//			[li]<b>OBN_XT_ITEMCLICK</b> The user has selected an item in the
//			Outlook bar.[/li]
//			[li]<b>OBN_XT_ONLABELENDEDIT</b> The user has completed editing an
//			item's label.[/li]
//			[li]<b>OBN_XT_ONGROUPENDEDIT</b> The user has completed editing a
//			folder's label.[/li]
//			[li]<b>OBN_XT_DRAGITEM</b> The user has dragged an item to a new
//			location in the Outlook bar.[/li]
//			[li]<b>OBN_XT_FOLDERCHANGE</b> The user has selected a new folder
//			item.[/li]
//			[li]<b>OBN_XT_ITEMHOVER</b> The user's mouse is hovering over an item
//			in the Outlook bar.[/li]
//			[li]<b>OBN_XT_DELETEITEM</b> The user has chosen to delete an item
//			from the Outlook bar.[/li]
//			[li]<b>OBN_XT_DELETEFOLDER</b> The user has chosen to delete a folder
//			from the Outlook bar.[/li]
//			[li]<b>OBN_XT_ITEMRCLICK</b> The user has right clicked on the Outlook
//			bar folder.[/li]
//			[/ul]
//			pOBInfo	   - Value of lParam points to an XT_OUTBAR_INFO structure that contains information for the 
//						 specified item. The item can either be a folder group or icon item, depending on the 
//						 value of the nBarAction parameter.  This pointer should <b>never</b> be NULL.
//  
// Returns:	If the application is to process this message, the return value should be TRUE. If the 
//			return value is FALSE, the user's action is ignored. 
//
// Remarks:	When the user performs an action in the Outlook bar, the XTWM_OUTBAR_NOTIFY message is 
//			sent to the Outlook bar's owner window. 
//
// Example:	Here is an example of how an application would process the XTWM_OUTBAR_NOTIFY 
//			message. 
//
//			<pre>int nBarAction = (int)wParam;</pre>
//			 
//			<pre>// Cast the lParam to an XT_OUTBAR_INFO* struct pointer.
//			XT_OUTBAR_INFO* pOBInfo = (XT_OUTBAR_INFO*)lParam;
//			ASSERT(pOBInfo);</pre>
//			
//			<pre>switch (nBarAction)
//			{
//			    case OBN_XT_ITEMCLICK:
//			    case OBN_XT_FOLDERCHANGE:
//			    case OBN_XT_ONLABELENDEDIT:
//			    case OBN_XT_ONGROUPENDEDIT:
//			    case OBN_XT_DRAGITEM:
//			    case OBN_XT_ITEMHOVER:
//			    case OBN_XT_ITEMRCLICK:
//			        TRACE2( "Index: %d, Name: %s.\n", pOBInfo->nIndex, pOBInfo->lpszText);
//			        break;
//			
//			    case OBN_XT_DELETEITEM:
//			        if (AfxMessageBox(_T("Remove this folder shortcut?"),
//			            MB_ICONWARNING|MB_YESNO) == IDNO)
//			        {
//			            return FALSE; // Return FALSE to abort the action.
//			        }
//			        break;
//			
//			    case OBN_XT_DELETEFOLDER:
//			        if (AfxMessageBox(_T("Remove the specified folder?"),
//			            MB_ICONWARNING|MB_YESNO) == IDNO)
//			        {
//			            return FALSE; // Return FALSE to abort the action.
//			        }
//			        break;
//			}
//			return TRUE;</pre>
// Summary: The XTWM_OUTBAR_NOTIFY message is sent to the CXTOutBarCtrl owner window 
//			whenever an action occurs within the CXTOutBarCtrl. 
//   
//			<pre>XTWM_OUTBAR_NOTIFY 
//			nBarAction = (int) wParam;           // Outlook bar action 
//			pOBInfo = (XT_OUTBAR_INFO*) lParam;  // pointer to an XT_OUTBAR_INFO struct</pre>
const UINT XTWM_OUTBAR_NOTIFY = (WM_USER + 2518); 
const UINT OBN_XT_ITEMCLICK        = 1;  // Item was selected.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_ONLABELENDEDIT   = 2;  // Item label edit completed.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_ONGROUPENDEDIT   = 3;  // Folder label edit completed.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_DRAGITEM         = 4;  // Item drag operation in process.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_FOLDERCHANGE     = 5;  // Folder selection changed.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_ITEMHOVER        = 6;  // Mouse is hovering over item.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_DELETEITEM       = 7;  // Item deleted.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_DELETEFOLDER     = 8;  // Folder deleted.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_BEGINDRAG        = 9;  // Item drag operation begun.  See XTWM_OUTBAR_NOTIFY.
const UINT OBN_XT_ITEMRCLICK       = 10; // Right click on Outlook bar occurred.  See XTWM_OUTBAR_NOTIFY.

// BULLETED LIST:

//===========================================================================
// Input:	nShellAction - Value of wParam specifies a shell tree value that indicates the users 
//						   request. This parameter can be one of the following values:
//			[ul]
//			[li]<b>SHN_XT_SHELLMENU</b> Shell context menu selection made.[/li]
//			[li]<b>SHN_XT_TREESELCHANGE</b> Tree selection made.[/li]
//			[li]<b>SHN_XT_SELECTCHILD</b> Child tree node selected.[/li]
//			[li]<b>SHN_XT_NOFOLDER</b> Item selected was not a folder.[/li]
//			[li]<b>SHN_XT_INETFOLDER</b> Item selected was the internet folder.[/li]
//			[li]<b>SHN_XT_CONTENTSCHANGED</b> Indicates the folder's contents have changed.[/li]
//			[li]<b>SHN_XT_REFRESHFOLDER</b> Indicates the folder needs to be updated.[/li]
//			[li]<b>SHN_XT_REFRESHTREE</b> Indicates the tree needs to be updated.[/li]
//			[/ul]
//			pItemData	 - Value of lParam points to an XT_TVITEMDATA structure that contains 
//						   information for the specified item. Depending on the action, this 
//						   pointer can be NULL.
//
// Returns:	If the application is to process this message, the return value should be 0.
//
// Remarks:	When the user performs an action in the shell tree, the XTWM_SHELL_NOTIFY message is 
//			sent to the shell tree's owner window. 
//
// Example:	Here is an example of how an application would process the XTWM_SHELL_NOTIFY 
//			message. 
//
//			<pre>int nShellAction = (int)wParam;</pre>
//			 
//			<pre>// Cast the lParam to an XT_TVITEMDATA* struct pointer.
//			XT_TVITEMDATA* pItemData = (XT_TVITEMDATA*)lParam;
//			ASSERT(pItemData);</pre>
//			
//			<pre>switch (nBarAction)
//			{
//			    case SHN_XT_SHELLMENU:
//			    case SHN_XT_TREESELCHANGE:
//			    case SHN_XT_SELECTCHILD:
//			    case SHN_XT_NOFOLDER:
//			    case SHN_XT_INETFOLDER:
//              case SHN_XT_CONTENTSCHANGED:
//              case SHN_XT_REFRESHFOLDER:
//              case SHN_XT_REFRESHTREE:
//			        // TODO: Handle shell notification message.
//			        TRACE0("Shell notification was sent.\n");
//			        break;
//			}
//			return 0;</pre>
// Summary: The XTWM_SHELL_NOTIFY message is sent to the CXTShellTree and list owner window 
//			whenever an action occurs within the CXTShellTree and list. 
//			<pre>XTWM_SHELL_NOTIFY 
//			nShellAction = (int) wParam;         // tree or list action 
//			pItemData = (XT_TVITEMDATA*) lParam;  // pointer to an XT_TVITEMDATA struct</pre>
//===========================================================================
const UINT XTWM_SHELL_NOTIFY = (WM_USER + 2519); 
const UINT SHN_XT_SHELLMENU       = 1; // Shell context menu selection made.  See XTWM_SHELL_NOTIFY
const UINT SHN_XT_TREESELCHANGE   = 2; // Tree selection made.  See XTWM_SHELL_NOTIFY.
const UINT SHN_XT_SELECTCHILD     = 3; // Child tree node selected.  See XTWM_SHELL_NOTIFY.
const UINT SHN_XT_NOFOLDER        = 4; // Item selected was not a folder.  See XTWM_SHELL_NOTIFY.
const UINT SHN_XT_INETFOLDER      = 5; // Item selected was the internet folder.  See XTWM_SHELL_NOTIFY.
const UINT SHN_XT_CONTENTSCHANGED = 6; // Indicates the folder's contents have changed.  See XTWM_SHELL_NOTIFY.
const UINT SHN_XT_REFRESHFOLDER   = 7; // Directory monitoring thread notification, list needs updating.  See XTWM_SHELL_NOTIFY.
const UINT SHN_XT_REFRESHTREE     = 8; // Directory monitoring thread notification, tree needs updating.  See XTWM_SHELL_NOTIFY.

//===========================================================================
// Input:	nID		 - Dialog control ID for the tab control.
//			pTabCtrl - Points to the tab control whose selection has changed.
//
// Remarks:	When the user selects a new tab in the tab control, the TCN_XT_SELCHANGE message is 
//			sent to the tab control's owner window. 
//
// Example:	Here is an example of how an application would process the TCN_XT_SELCHANGE 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//			    //{{AFX_MSG_MAP(CMainFrame)
//			    ON_MESSAGE(TCN_XT_SELCHANGE, OnTabSelChange)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CMainFrame::OnTabSelChange(WPARAM wParam, LPARAM lParam)
//			{
//			    CXTTabCtrl* pTabCtrl = (CXTTabCtrl*)lParam;
//			    ASSERT_VALID(pTabCtrl);
//			
//			    int nID = (int)wParam;
//			    if (nID == ID_VIEW_WORKSPACEBAR)
//			    {
//				      // TODO: handle TCN_SELCHANGE from tab control bar.
//			    }
//			    return 0;
//			}</pre>
// Summary: The TCN_XT_SELCHANGE message is sent to the owner of a CXTTabCtrl whenever
//			the tab selection has changed.
//   
//			<pre>TCN_XT_SELCHANGE 
//			nID = (int) wParam;               // dialog control ID 
//			pTabCtrl = (CXTTabCtrl*) lParam;  // pointer to a CXTTabCtrl object</pre>
//===========================================================================
const UINT TCN_XT_SELCHANGE = (WM_USER + 2520); 

//===========================================================================
// Input:	nID		 - Dialog control ID for the tab control.
//			pTabCtrl - Points to the tab control whose selection is about to change.
//
// Remarks:	When the user selects a new tab in the tab control, the TCN_XT_SELCHANGING message is 
//			sent to the tab control's owner window. 
//
// Example:	Here is an example of how an application would process the TCN_XT_SELCHANGING 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//			    //{{AFX_MSG_MAP(CMainFrame)
//			    ON_MESSAGE(TCN_XT_SELCHANGING, OnTabSelChanging)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CMainFrame::OnTabSelChanging(WPARAM wParam, LPARAM lParam)
//			{
//			    CXTTabCtrl* pTabCtrl = (CXTTabCtrl*)lParam;
//			    ASSERT_VALID(pTabCtrl);
//			
//			    int nID = (int)wParam;
//			
//			    if (nID == ID_VIEW_WORKSPACEBAR)
//			    {
//				      // TODO: handle TCN_SELCHANGE from tab control bar.
//			    }
//			    return 0;
//			}</pre>
// Summary: The TCN_XT_SELCHANGING message is sent to the owner of a CXTTabCtrl whenever
//			the tab selection is about to change.
//   
//			<pre>TCN_XT_SELCHANGING 
//			nID = (int) wParam;               // dialog control ID 
//			pTabCtrl = (CXTTabCtrl*) lParam;  // pointer to a CXTTabCtrl object</pre>
//===========================================================================
const UINT TCN_XT_SELCHANGING = (WM_USER + 2521); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			search options control is expanding.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_SON_XT_ITEMEXPANDING(IDC_CTRL_ID, OnItemExpanding)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnItemExpanding()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnItemExpanding( );</b></pre>
//===========================================================================
const UINT SON_XT_ITEMEXPANDING = (WM_USER + 2522); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			search options control has expanded.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_SON_XT_ITEMEXPAND(IDC_CTRL_ID, OnItemExpand)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnItemExpand()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnItemExpand( );</b></pre>
//===========================================================================
const UINT SON_XT_ITEMEXPAND = (WM_USER + 2523); 

#define ON_SON_XT_ITEMEXPANDING(id, memberFxn) \
    ON_CONTROL(SON_XT_ITEMEXPANDING, id, memberFxn)
#define ON_SON_XT_ITEMEXPAND(id, memberFxn) \
    ON_CONTROL(SON_XT_ITEMEXPAND, id, memberFxn)

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			search options control is contracting.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_SON_XT_ITEMCONTRACTING(IDC_CTRL_ID, OnItemContracting)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnItemContracting()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary:  <pre>function prototype: <b>afx_msg void OnItemContracting( );</b></pre>
//===========================================================================
const UINT SON_XT_ITEMCONTRACTING = (WM_USER + 2524); 

//===========================================================================
// Remarks:	This command handler is called to inform the owner whenever the
//			search options control has contracted.
//
// Example:	Here is an example of how an application would handle this message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CAppDialog, CDialog
//			    //{{AFX_MSG_MAP(CAppDialog)
//			    ON_SON_XT_ITEMCONTRACT(IDC_CTRL_ID, OnItemContract)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>void CAppDialog::OnItemContract()
//			{
//			    // TODO: Handle command.
//			}</pre>
// Summary: <pre>function prototype: <b>afx_msg void OnItemContract( );</b></pre>
//===========================================================================
const UINT SON_XT_ITEMCONTRACT = (WM_USER + 2525); 

#define ON_SON_XT_ITEMCONTRACTING(id, memberFxn) \
    ON_CONTROL(SON_XT_ITEMCONTRACTING, id, memberFxn)
#define ON_SON_XT_ITEMCONTRACT(id, memberFxn) \
    ON_CONTROL(SON_XT_ITEMCONTRACT, id, memberFxn)

//===========================================================================
// Summary: This message handler is used to send initial update notifications to
//			controls when they are created. This message is primarily used by the
//			toolkit during initialization, however you can use this message to initialize
//			your own control as well.
//===========================================================================
const UINT XTWM_INITIAL_UPDATE = (WM_USER + 2526); 

//===========================================================================
// Notification message: toolbar expansion popup window has closed.
// NB: This is a private XT toolkit library message subject to changes.
//===========================================================================
const UINT XTWM_POPUPCLOSED = (WM_USER + 2527);

//===========================================================================
// Input:	bHorz - TRUE if horizontally oriented, FALSE otherwise.
//			pRBBI - A pointer to a BARBANDINFO structure, the window can modify its parameters as appropriate.
//
// Returns:	If the application is to process this message, the return value should be TRUE.
//
// Remarks:	When a window is added to a rebar, the XTWM_ADDREBAR message is sent to the window. 
//
// Example:	Here is an example of how an application would process the XTWM_ADDREBAR 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CWidget, CWnd)
//			    //{{AFX_MSG_MAP(CWidget)
//			    ON_MESSAGE(XTWM_ADDREBAR, OnAddReBar)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CWidget::OnAddReBar(WPARAM wParam, LPARAM lParam) 
//			{
//			    BOOL bHorz = (BOOL) wParam;
//			
//			    REBARBANDINFO* pRBBI = (REBARBANDINFO*) lParam;
//			        // TODO: Handle message.
//			
//			    return TRUE;
//			}</pre>
// Summary: The XTWM_ADDREBAR message is sent to a window whenever it is added
//			to a CXTReBar control. 
//   
//			<pre>XTWM_ADDREBAR 
//			bHorz = (BOOL) wParam;            // TRUE if horizontal.
//			pRBBI = (REBARBANDINFO*) lParam;  // Points to a REBARBANDINFO structure.</pre>
//===========================================================================
const UINT XTWM_ADDREBAR = (WM_USER + 2528); 

//===========================================================================
// Input:	bHorz  - TRUE if horizontally oriented, FALSE otherwise.
//			pRBSCI - A pointer to an XT_REBARSIZECHILDINFO structure, the window can 
//					 modify its parameters as appropriate.
//
// Returns:	If the application is to process this message, the return value should be TRUE.
//
// Remarks:	When a window is added to a rebar, the XTWM_REBAR_SIZECHILD message is sent to the window. 
//
// Example:	Here is an example of how an application would process the XTWM_REBAR_SIZECHILD 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CWidget, CWnd)
//			    //{{AFX_MSG_MAP(CWidget)
//			    ON_MESSAGE(XTWM_REBAR_SIZECHILD, OnReBarSizeChild)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CWidget::OnReBarSizeChild(WPARAM wParam, LPARAM lParam) 
//			{
//			    BOOL bHorz = (BOOL) wParam;
//			
//			    XT_REBARSIZECHILDINFO* pRBSCI = (XT_REBARSIZECHILDINFO*) lParam;
//			        // TODO: Handle message.
//			
//			    return TRUE;
//			}</pre>
// Summary: The XTWM_REBAR_SIZECHILD message is sent when CXTReBar control has resized 
//			the band in which the recipient resides.
//   
//			<pre>XTWM_REBAR_SIZECHILD 
//			bHorz = (BOOL) wParam; // TRUE if horizontal.
//			pRBSCI = (XT_REBARSIZECHILDINFO*) lParam;  // Points to an XT_REBARSIZECHILDINFO structure.</pre>
//===========================================================================
const UINT XTWM_REBAR_SIZECHILD = (WM_USER + 2529); 

//===========================================================================
// Returns: TRUE if the control bar shall be fully visualized, FALSE if just a wire frame must be rendered.
//
// Remarks:	When a control bar is dragged the XTWM_QUERYVISUALIZE message is sent to the control bar. 
//
// Example:	Here is an example of how an application would process the XTWM_QUERYVISUALIZE 
//			message. 
//  
//			<pre>BEGIN_MESSAGE_MAP(CWidget, CXTControlBar)
//			    //{{AFX_MSG_MAP(CWidget)
//			    ON_MESSAGE(XTWM_QUERYVISUALIZE, OnQueryVisualize)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CWidget::OnQueryVisualize(WPARAM wParam, LPARAM lParam) 
//			{
//			    UNREFERENCED_PARAMETER( wParam );
//			    UNREFERENCED_PARAMETER( lParam );
//			    // Allow full visualizations.
//			    return TRUE;
//			}</pre>
// Summary: CXTDockContext sends the XTWM_QUERYVISUALIZE message to a control bar to 
//			check if the control bar shall be visualized while dragging or just a wire 
//			frame shall be rendered.
//   
//			<pre>XTWM_QUERYVISUALIZE 
//			wParam - not used, must be zero
//			lParam - not used, must be zero</pre>
//===========================================================================
const UINT XTWM_QUERYVISUALIZE = (WM_USER + 2530);

//===========================================================================
// Returns: TRUE if control bar has a hot spot, FALSE if it does not.
//
// Remarks: CXTDockContext sends the XTWM_QUERYHOTSPOT message the control bar to check 
//			for a hot spot. 
//
// Example: Here is an example of how an application would process the XTWM_QUERYHOTSPOT 
//			message. 
//
//			<pre>BEGIN_MESSAGE_MAP(CWidget, CXTControlBar)
//			    //{{AFX_MSG_MAP(CWidget)
//			    ON_MESSAGE(XTWM_QUERYHOTSPOT, OnQueryHotSpot)
//			    //}}AFX_MSG_MAP
//			END_MESSAGE_MAP()</pre>
//			
//			<pre>LRESULT CWidget::OnQueryHotSpot(WPARAM wParam, LPARAM lParam) 
//			{
//			    UNREFERENCED_PARAMETER( wParam );
//			    LPSIZE lpSize = (LPSIZE) lParam;  // Points to a LPSIZE structure.
//			    // TODO: Handle message.
//			    return TRUE;
//			}</pre>
// Summary: CXTDockContext sends the XTWM_QUERYHOTSPOT message to a control bar to 
//			check if the control bar has a hot spot, a point that must match the position 
//			of the cursor.
//   
//			<pre>XTWM_QUERYHOTSPOT 
//			wParam - not used, must be zero
//			lpSize = (LPSIZE) lParam;  // IN  - Extent of the rectangle in which 
//			                           //       hot spot is to be defined
//			                           // OUT - Offset of the hot spot location 
//			                           //       within the rectangle.</pre>
//===========================================================================
const UINT XTWM_QUERYHOTSPOT = (WM_USER + 2531);

//===========================================================================
// Summary: This notification handler is used internally to notify the rebar 
//          control that the ideal size of the embedded control has changed.
//   
//			<pre>XTWM_IDEALSIZECHANGED 
//			hWnd = (HWND) wParam; // Rebar control's window handle.
//			uSize = (UINT) lParam;  // New ideal size.</pre>
//
// Returns: Return value is ignored.
//===========================================================================
const UINT XTWM_IDEALSIZECHANGED = (WM_USER + 2532);

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTNOTIFY_H__)
