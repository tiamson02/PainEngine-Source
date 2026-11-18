//---------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// File   : uxtheme.h
// Version: 1.0
//---------------------------------------------------------------------------
#ifndef _UXTHEME_H_
#define _UXTHEME_H_
//---------------------------------------------------------------------------
#include <commctrl.h>
//---------------------------------------------------------------------------
//#if (_WIN32_WINNT >= 0x0500)     // only available on XP
//---------------------------------------------------------------------------
// Define API decoration for direct importing of DLL references.
#ifndef THEMEAPI
#if !defined(_UXTHEME_)
#define THEMEAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define THEMEAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#else
#define THEMEAPI          STDAPI
#define THEMEAPI_(type)   STDAPI_(type)
#endif
#endif // THEMEAPI
//---------------------------------------------------------------------------
typedef HANDLE HTHEME;          // handle to a section of theme data for class

//---------------------------------------------------------------------------
//----- DrawThemeText() flags ----

#define DTT_GRAYED      0x1         // draw a grayed-out string  

//-------------------------------------------------------------------------
typedef enum THEMESIZE
{
    TS_MIN,             // minimum size
    TS_TRUE,            // size without stretching
    TS_DRAW,            // size that theme mgr will use to draw part
};

//----- HitTestThemeBackground, HitTestThemeBackgroundRegion flags ----

//  Theme background segment hit test flag (default). possible return values are:
//  HTCLIENT: hit test succeeded in the middle background segment
//  HTTOP, HTLEFT, HTTOPLEFT, etc:  // hit test succeeded in the the respective theme background segment.
#define HTTB_BACKGROUNDSEG          0x0000  

//  Fixed border hit test option.  possible return values are:
//  HTCLIENT: hit test succeeded in the middle background segment
//  HTBORDER: hit test succeeded in any other background segment
#define HTTB_FIXEDBORDER            0x0002  // Return code may be either HTCLIENT or HTBORDER. 

//  Caption hit test option.  Possible return values are:
//  HTCAPTION: hit test succeeded in the top, top left, or top right background segments
//  HTNOWHERE or another return code, depending on absence or presence of accompanying flags, resp.
#define HTTB_CAPTION                0x0004  

//  Resizing border hit test flags.  Possible return values are:
//  HTCLIENT: hit test succeeded in middle background segment
//  HTTOP, HTTOPLEFT, HTLEFT, HTRIGHT, etc:    hit test succeeded in the respective system resizing zone
//  HTBORDER: hit test failed in middle segment and resizing zones, but succeeded in a background border segment
#define HTTB_RESIZINGBORDER_LEFT    0x0010  // Hit test left resizing border, 
#define HTTB_RESIZINGBORDER_TOP     0x0020  // Hit test top resizing border
#define HTTB_RESIZINGBORDER_RIGHT   0x0040  // Hit test right resizing border
#define HTTB_RESIZINGBORDER_BOTTOM  0x0080  // Hit test bottom resizing border

#define HTTB_RESIZINGBORDER         (HTTB_RESIZINGBORDER_LEFT|HTTB_RESIZINGBORDER_TOP|\
                                     HTTB_RESIZINGBORDER_RIGHT|HTTB_RESIZINGBORDER_BOTTOM)

// Resizing border is specified as a template, not just window edges.
// This option is mutually exclusive with HTTB_SYSTEMSIZINGWIDTH; HTTB_SIZINGTEMPLATE takes precedence  
#define HTTB_SIZINGTEMPLATE      0x0100

// Use system resizing border width rather than theme content margins.   
// This option is mutually exclusive with HTTB_SIZINGTEMPLATE, which takes precedence.
#define HTTB_SYSTEMSIZINGMARGINS 0x0200

//-----------------------------------------------------------------------
typedef struct _MARGINS
{
    int cxLeftWidth;      // width of left border that retains its size
    int cxRightWidth;     // width of right border that retains its size
    int cyTopHeight;      // height of top border that retains its size
    int cyBottomHeight;   // height of bottom border that retains its size
} MARGINS, *PMARGINS;

//-----------------------------------------------------------------------
#define MAX_INTLIST_COUNT 10

typedef struct _INTLIST
{
    int iValueCount;      // number of values in iValues
    int iValues[MAX_INTLIST_COUNT];
} INTLIST, *PINTLIST;

//-----------------------------------------------------------------------
typedef enum PROPERTYORIGIN
{
    PO_STATE,           // property was found in the state section
    PO_PART,            // property was found in the part section
    PO_CLASS,           // property was found in the class section
    PO_GLOBAL,          // property was found in [globals] section
    PO_NOTFOUND         // property was not found
};

#define ETDT_DISABLE        0x00000001
#define ETDT_ENABLE         0x00000002
#define ETDT_USETABTEXTURE  0x00000004
#define ETDT_ENABLETAB      (ETDT_ENABLE  | ETDT_USETABTEXTURE)

//---- flags to control theming within an app ----

#define STAP_ALLOW_NONCLIENT    (1 << 0)
#define STAP_ALLOW_CONTROLS     (1 << 1)
#define STAP_ALLOW_WEBCONTENT   (1 << 2)

#define SZ_THDOCPROP_DISPLAYNAME                L"DisplayName"
#define SZ_THDOCPROP_CANONICALNAME              L"ThemeName"
#define SZ_THDOCPROP_TOOLTIP                    L"ToolTip"
#define SZ_THDOCPROP_AUTHOR                     L"author"

//---- bits used in dwFlags of DTBGOPTS ----
#define DTBG_CLIPRECT        0x00000001   // rcClip has been specified
#define DTBG_DRAWSOLID       0x00000002   // draw transparent/alpha images as solid
#define DTBG_OMITBORDER      0x00000004   // don't draw border of part
#define DTBG_OMITCONTENT     0x00000008   // don't draw content area of part

#define DTBG_COMPUTINGREGION 0x00000010   // TRUE if calling to compute region

#define DTBG_MIRRORDC        0x00000020   // assume the hDC is mirrorred and
                                          // flip images as appropriate (currently 
                                          // only supported for bgtype=imagefile)
//------------------------------------------------------------------------
typedef struct _DTBGOPTS
{
    DWORD dwSize;           // size of the struct
    DWORD dwFlags;          // which options have been specified
    RECT rcClip;            // clipping rectangle
}
DTBGOPTS, *PDTBGOPTS;

//---------------------------------------------------------------------------
//#endif  /* (_WIN32_WINNT >= 0x0500) *// 
//---------------------------------------------------------------------------
#endif // _UXTHEME_H_
//---------------------------------------------------------------------------
