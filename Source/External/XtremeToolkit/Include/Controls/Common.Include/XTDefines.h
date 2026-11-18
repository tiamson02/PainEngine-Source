// XTDefines.h interface for the XT_AUX_DATA struct.
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

#if !defined(__XTDEFINES_H__)
#define __XTDEFINES_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTNotify.h"

//-----------------------------------------------------------------------------
// CXTSplitterDock
//-----------------------------------------------------------------------------

// Vertical docking window splitter.
const UINT XT_SPLITTER_VERT        = 0x0001;
// Horizontal docking window splitter.
const UINT XT_SPLITTER_HORZ        = 0x0002;

//-----------------------------------------------------------------------------
// CXTSplitterWnd
//-----------------------------------------------------------------------------

// Has a dotted tracker.
const UINT XT_SPLIT_DOTTRACKER     = 0x0001;
// Show pane contents while dragging.
const UINT XT_SPLIT_NOFULLDRAG     = 0x0002;
// Do not draw a border around the pane.
const UINT XT_SPLIT_NOBORDER       = 0x0004;
// Do allow sizing.
const UINT XT_SPLIT_NOSIZE         = 0x0008;

//-----------------------------------------------------------------------------
// CXTButton
//-----------------------------------------------------------------------------

// Draws a flat button.
const UINT BS_XT_FLAT              = 0x0001;
// Draws a semi-flat button.
const UINT BS_XT_SEMIFLAT          = 0x0002;
// Draws images and text are centered.
const UINT BS_XT_TWOROWS           = 0x0004;
// Draws a focus rect when the button has input focus.
const UINT BS_XT_SHOWFOCUS         = 0x0010;
// Highlights the button when pressed.
const UINT BS_XT_HILITEPRESSED     = 0x0020;
// Draws a flat button ala Office XP.
const UINT BS_XT_XPFLAT            = 0x0040;
// Same as (BS_XT_FLAT | BS_XT_SEMIFLAT)
const UINT BS_XT_FLAT_ANY          = (BS_XT_FLAT | BS_XT_SEMIFLAT);
// Uses Windows XP themes if available.  This setting overrides BS_XT_FLAT and BS_XT_SEMIFLAT, but <i>does not</i> override BS_XT_XPFLAT.
const UINT BS_XT_WINXP_COMPAT      = 0x1000;

//-----------------------------------------------------------------------------
// CXTBrowseEdit
//-----------------------------------------------------------------------------

// Display the choose folder dialog.
const UINT BES_XT_CHOOSEDIR        = 0x0001;
// Display the choose file dialog.
const UINT BES_XT_CHOOSEFILE       = 0x0002;
// Display a user defined context menu.
const UINT BES_XT_POPUPMENU        = 0x0004;

//-----------------------------------------------------------------------------
// CXTColorPopup
//-----------------------------------------------------------------------------

// Use No Fill style in place of Automatic.
const UINT CPS_XT_NOFILL           = 0x0001;
// Display extended colors.
const UINT CPS_XT_EXTENDED         = 0x0002;
// Display more colors button.
const UINT CPS_XT_MORECOLORS       = 0x0004;
// Display a list of user selected colors.
const UINT CPS_XT_USERCOLORS       = 0x0020;
//:Ignore - Used internally by the color popup window.
const UINT CPS_XT_PICKBOX          = 0x0008; 
const UINT CPS_XT_DEFAULT          = 0x0010; 
//:End Ignore

//-----------------------------------------------------------------------------
// CXTColorDialog
//-----------------------------------------------------------------------------

// Displays color in hex format.
const UINT CPS_XT_SHOWHEXVALUE     = 0x0020;
// Shows new / current display box with a 3D border.
const UINT CPS_XT_SHOW3DSELECTION  = 0x0040;

//-----------------------------------------------------------------------------
// CXTEditListBox
//-----------------------------------------------------------------------------

// Standard edit field.
const UINT LBS_XT_DEFAULT          = 0x0000;
// Choose directory browse edit field.
const UINT LBS_XT_CHOOSEDIR        = 0x0001;
// Choose file browse edit field.
const UINT LBS_XT_CHOOSEFILE       = 0x0002;
// Do not display edit toolbar.
const UINT LBS_XT_NOTOOLBAR        = 0x0008;

//-----------------------------------------------------------------------------
// CXTTipWindow
//-----------------------------------------------------------------------------

// Show a 3D border.
const UINT TWS_XT_THICKBORDER      = 0x0001;
// Show a drop shadow when the window is active.
const UINT TWS_XT_DROPSHADOW       = 0x0002;
// Show alpha style drop shadow.
const UINT TWS_XT_ALPHASHADOW      = 0x0004;

//-----------------------------------------------------------------------------
// CXTCaption
//-----------------------------------------------------------------------------

// Show the caption with a sunken border.
const UINT CPWS_EX_GROOVE_EDGE     = 0x0001;
// Show the caption with a raised border.
const UINT CPWS_EX_RAISED_EDGE     = 0x0002;
// Caption has a close button.
const UINT CPWS_EX_CLOSEBUTTON     = 0x0004;

//-----------------------------------------------------------------------------
// CXTFlatTabCtrl
//-----------------------------------------------------------------------------

// Show tabs on bottom.
const UINT FTS_XT_BOTTOM           = 0x0001;
// Show back and next arrows.
const UINT FTS_XT_HASARROWS        = 0x0002;
// Show home and end arrows.  Used with FTS_XT_HASARROWS.
const UINT FTS_XT_HASHOMEEND       = 0x0004;
// Show tooltips.
const UINT FTS_XT_TOOLTIPS         = 0x0008;
// Same as FTS_XT_BOTTOM | FTS_XT_HASARROWS | FTS_XT_HASHOMEEND | FTS_XT_TOOLTIPS.
const UINT FTS_XT_DEFAULT          = 0x000F;
// Show a horizontal scroll bar.
const UINT FTS_XT_HSCROLL          = 0x0010;
// All FTS_ mask items.
const UINT FTS_XT_MASK             = 0x001F;

//-----------------------------------------------------------------------------
// CXTOutBarCtrl
//-----------------------------------------------------------------------------

// Sets small icon mode.
const UINT OBS_XT_SMALLICON        = 0x0001;
// Sets large icon mode.
const UINT OBS_XT_LARGEICON        = 0x0002;
// Enables folder local editing (renaming).
const UINT OBS_XT_EDITGROUPS       = 0x0004;
// Enables item local editing (renaming).
const UINT OBS_XT_EDITITEMS        = 0x0008;
// Enables the "Remove" command for folders in context menu.
const UINT OBS_XT_REMOVEGROUPS     = 0x0010;
// Enables the "Remove" command for items in context menu.
const UINT OBS_XT_REMOVEITEMS      = 0x0020;
// Enables folder insertion.
const UINT OBS_XT_ADDGROUPS        = 0x0040;
// Enables item dragging to rearrange position.
const UINT OBS_XT_DRAGITEMS        = 0x0080;
// Enables animation while changing folder selection.
const UINT OBS_XT_ANIMATION        = 0x0100;
// Enables dimmed highlight of last pressed item.
const UINT OBS_XT_SELHIGHLIGHT     = 0x0200;
// Same as OBS_XT_DRAGITEMS | OBS_XT_EDITGROUPS | OBS_XT_EDITITEMS | OBS_XT_REMOVEGROUPS | OBS_XT_REMOVEITEMS | OBS_XT_ADDGROUPS.
const UINT OBS_XT_DEFAULT          = 0x00FC;

//-----------------------------------------------------------------------------
// CXTTreeCtrl, CXTTreeView
//-----------------------------------------------------------------------------

// For determining tree item focus state.
const UINT TVIS_FOCUSED            = 0x0001;
 
//:Ignore

//-----------------------------------------------------------------------------
// UNICODE support definitions:
//-----------------------------------------------------------------------------
#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif

#ifndef IDANI_CAPTION               
#define IDANI_CAPTION			    3
#endif//IDANI_CAPTION

#ifndef SPI_GETGRADIENTCAPTIONS
#define SPI_GETGRADIENTCAPTIONS     0x1008
#endif//SPI_GETGRADIENTCAPTIONS

#ifndef SPI_GETMENUUNDERLINES
#define SPI_GETMENUUNDERLINES       0x100A
#endif//SPI_GETMENUUNDERLINES

#ifndef BTNS_WHOLEDROPDOWN
#define BTNS_WHOLEDROPDOWN			0x80  // draw dropdown arrow, but without split arrow section
#endif//BTNS_WHOLEDROPDOWN

//-----------------------------------------------------------------------------
// Summary: The I_IMAGENONE constant is used when defining text-only toolbar 
//          buttons with no space allocated to the button icon.  The constant 
//          is normally defined in version 5.81 and greater SDK headers
//-----------------------------------------------------------------------------
#ifndef I_IMAGENONE
#define I_IMAGENONE                 (-2)
#endif//I_IMAGENONE

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)"): warning : "

//-----------------------------------------------------------------------------
//      Depreciated Values:      New Value:
//-----------------------------------------------------------------------------

#define _delete			         SAFE_DELETE			 
#define _deleteArray		     SAFE_DELETE_AR
#define _xtAfxShowPrintPreview   CXTPreviewView::ShowPrintPreview
#define CXTManageState           CXTPManageState
#define CXTDialogState           CXTPDialogState

//:End Ignore

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTDEFINES_H__)