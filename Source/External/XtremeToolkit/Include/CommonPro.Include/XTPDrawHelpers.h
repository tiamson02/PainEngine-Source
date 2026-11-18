// XTPDrawHelpers.h: interface for the CXTPDrawHelpers class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// ©2004 Codejock Software, All Rights Reserved.
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

#if !defined(__XTPDRAWHELPERS_H__)
#define __XTPDRAWHELPERS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPColorManager.h"

//===========================================================================
// Summary: CXTPTransparentBitmap is a helper class used to retrieve the cursor
//          position in client coordinates.
//===========================================================================
class _XTP_EXT_CLASS CXTPTransparentBitmap
{
public:

	CXTPTransparentBitmap(HBITMAP hBitmap);

	COLORREF GetTransparentColor() const;

	HICON ConvertToIcon() const;

protected:

	HBITMAP m_hBitmap;
};

//===========================================================================
// Summary: CXTPClientCursorPos is a helper class used to retrieve the cursor
//          position in client coordinates.
//===========================================================================
class _XTP_EXT_CLASS CXTPClientCursorPos : public CPoint
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPClientCursorPos object used to retrieve the
	//          cursor position in client coordinates.
	// Input:   pWnd - Pointer to the window that contains the client area to
	//                 get the cursor position for.
	//-----------------------------------------------------------------------
	CXTPClientCursorPos(CWnd* pWnd);
};

//===========================================================================
// Summary: CXTPEmptyRect is a helper class used to instantiate an empty
//          CRect object.
//===========================================================================
class _XTP_EXT_CLASS CXTPEmptyRect : public CRect
{
public:
	
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPEmptyRect object used to instantiate an
	//          empty CRect object.
	//-----------------------------------------------------------------------
	CXTPEmptyRect();
};

//===========================================================================
// Summary: CXTPWindowRect is a helper class used to retrieve the screen
//          size for a specified window.
//===========================================================================
class _XTP_EXT_CLASS CXTPWindowRect : public CRect
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPWindowRect object used to retrieve the
	//          screen size for the specified window.
	// Input:   hWnd - Handle to the window to retrieve the screen size for.
	//-----------------------------------------------------------------------
	CXTPWindowRect(HWND hWnd);

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPWindowRect object used to retrieve the
	//          screen size for the specified window.
	// Input:   pWnd - Points to the window to retrieve the screen size for.
	//-----------------------------------------------------------------------
	CXTPWindowRect(const CWnd* pWnd);
};

//===========================================================================
// Summary: CXTPClientRect is a helper class used to retrieve the client
//          area for a specified window.
//===========================================================================
class _XTP_EXT_CLASS CXTPClientRect : public CRect
{
public:
	
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPClientRect object used to retrieve the
	//          client area for the specified window.
	// Input:   hWnd - Handle to the window to retrieve the client area for.
	//-----------------------------------------------------------------------
	CXTPClientRect(HWND hWnd);

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPClientRect object used to retrieve the
	//          client area for the specified window.
	// Input:   pWnd - Points to the window to retrieve the client area for.
	//-----------------------------------------------------------------------
	CXTPClientRect(const CWnd* pWnd);
};

//===========================================================================
// Summary: CXTPBufferDC is a helper class used to create a memory device
//          context used to draw to an offscreen bitmap.  When destroyed, the
//          class selects the previous bitmap back into the device context to 
//          handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBufferDC : public CDC
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPBufferDC object used to create a memory
	//          device context used to draw to an offscreen bitmap.
	// Input:   hDestDC - Handle to the destination device context the memory
	//                    device is BitBlt to.
	//          rcPaint - Size of the area to paint.
	//-----------------------------------------------------------------------
	CXTPBufferDC(HDC hDestDC, const CRect rcPaint);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPBufferDC object, handles cleanup and de-
	//          allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBufferDC();

	//-----------------------------------------------------------------------
	// Summary: Call this method to retrieve a CDC pointer to the destination
	//          device context.
	//-----------------------------------------------------------------------
	CDC* GetDestDC();

protected:

	HDC     m_hDestDC;    // Handle to the destination device context.
	CBitmap m_bitmap;     // Bitmap in memory device context
	CRect   m_rect;       // Size of the area to paint.
	HGDIOBJ m_hOldBitmap; // Handle to the previously selected bitmap.
};

//===========================================================================
// Summary: CXTPFontDC is a helper class used to select a font into the
//          device context specified by hDC.  When destroyed, the class
//          selects the previous font back into the device context to 
//          handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPFontDC
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPFontDC object and selects the specified font
	//          into the device context specified by pDC.
	// Input:   pDC   - Pointer to a valid device context.
	//          pFont - Pointer to a CFont object to select into the device
	//                  context.
	//-----------------------------------------------------------------------
	CXTPFontDC(CDC* pDC, CFont* pFont);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPFontDC object, handles cleanup and de-
	//          allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPFontDC();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to change the font selected by the
	//          device context.
	// Input:   pFont - Pointer to a CFont object to select into the device
	//                  context.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

protected:

	HDC     m_hDC;      // Device context handle.
	HGDIOBJ m_hOldFont; // Handle to the previously selected font.
};

//===========================================================================
// Summary: CXTPPenDC is a helper class used to create a pen using
//          the color specified by crColor.  The pen is then selected
//          into the device context specified by hDC.  When destroyed, the
//          class selects the previous pen back into the device context to 
//          handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPPenDC
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPPenDC object and creates a pen using
	//          the color specified by crColor.  The pen is then selected
	//          into the device context specified by hDC.
	// Input:   hDC     - Handle to a valid device context.
	//          crColor - RGB value used to create pen.
	//-----------------------------------------------------------------------
	CXTPPenDC(HDC hDC, COLORREF crColor);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPenDC object, handles cleanup and de-
	//          allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPenDC();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to change the color used by the
	//          device context pen.
	// Input:   crColor - RGB value to change the pen color to.
	//-----------------------------------------------------------------------
	void Color(COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to retrieve the color used by the
	//          device context pen.
	// Returns: An RGB value that represents the selected pen color.
	//-----------------------------------------------------------------------
	COLORREF Color();

protected:

	CPen m_pen;     // Pen selected into device context.
	HDC  m_hDC;     // Device context handle.
	HPEN m_hOldPen; // Handle to the previously selected pen.
};

//===========================================================================
// Summary: CXTPBrushDC is a helper class used to create a  brush using
//          the color specified by crColor.  The brush is then selected
//          into the device context specified by hDC.  When destroyed, the
//          class selects the previous brush back into the device context to 
//          handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBrushDC
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPBrushDC object and creates a brush using
	//          the color specified by crColor.  The brush is then selected
	//          into the device context specified by hDC.
	// Input:   hDC     - Handle to a valid device context.
	//          crColor - RGB value used to create brush.
	//-----------------------------------------------------------------------
	CXTPBrushDC(HDC hDC, COLORREF crColor);
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPBrushDC object, handles cleanup and de-
	//          allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBrushDC();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to change the color used by the
	//          device context brush.
	// Input:   crColor - RGB value to change the brush color to.
	//-----------------------------------------------------------------------
	void Color(COLORREF crColor);

protected:

	CBrush m_brush;     // Brush selected into device context.
	HDC    m_hDC;       // Device context handle.
	HBRUSH m_hOldBrush; // Handle to the previously selected brush.
};

//===========================================================================
// Summary: CXTPCompatibleDC is a helper class used to create a memory device
//          context (DC) compatible with the device specified by pDC.  When
//          destroyed, the class selects the previous bitmap back into the
//          device context to handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPCompatibleDC : public CDC
{
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPCompatibleDC object and creates a memory
	//          device context (DC) compatible with the device specified by
	//          pDC.  The bitmap specified by pBitmap is then selected into
	//          the device context.
	// Input:   pDC     - Points to a valid device context.
	//          pBitmap - Points to the previously selected bitmap.
	//-----------------------------------------------------------------------
	CXTPCompatibleDC(CDC* pDC, CBitmap* pBitmap);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPCompatibleDC object, handles cleanup and de-
	//          allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCompatibleDC();

protected:

	CBitmap* m_pBmp; // Pointer to the previously selected bitmap.
};


// Summary: CXTPSplitterTracker is a stand alone class. It is used 
//			to track a splitter rectangle.
class _XTP_EXT_CLASS CXTPSplitterTracker
{
public:
	CXTPSplitterTracker(BOOL bSolid = FALSE);
public:
	// Input:	pTrackWnd - Pointer to a CWnd object.
	//			rcAvail - Available rectangle of tracking.
	//			rectTracker - Current rectangle of tracking.
	//			point - Starting point.
	//			bHoriz - TRUE to track horizontally.
	// Returns: TRUE if successful; otherwise returns FALSE
	// Summary: This method is called to track size of the splitter.
	BOOL Track(CWnd* pTrackWnd, CRect rcAvail, CRect& rectTracker, CPoint point, BOOL bHoriz);

	void SetBoundRect(CRect rcBoundRect);

private:
	void OnInvertTracker(CRect rc);
private:
	CDC* m_pDC;
	BOOL m_bSolid;
	CRect m_rcBoundRect;
};


class _XTP_EXT_CLASS CXTPPaintManagerColor
{
public:
	CXTPPaintManagerColor()
	{
		m_clrStandardValue = m_clrCustomValue = (COLORREF)-1;
	}
	
	BOOL IsDefaultValue() {
		return (m_clrCustomValue == (COLORREF)-1) && (m_clrStandardValue == (COLORREF)-1);
	}
	
	void SetStandardValue(COLORREF clr) {
		m_clrStandardValue = clr;
	}
	void SetDefaultValue() {
		m_clrCustomValue = (COLORREF)-1;
	}
	operator COLORREF() {
		return  (m_clrCustomValue == (COLORREF)-1)? m_clrStandardValue: m_clrCustomValue;
	}
	const CXTPPaintManagerColor&  operator=(COLORREF clr) {
		m_clrCustomValue = clr;
		return *this;
	}		
	
protected:
	COLORREF m_clrStandardValue;
	COLORREF m_clrCustomValue;
	
};

//===========================================================================
// Summary: CXTPDrawHelpers is a helper class used to perform specialized 
//          drawing tasks.  You can use this class to perform such tasks as
//          gradient fills and blending colors.
//===========================================================================
class _XTP_EXT_CLASS CXTPDrawHelpers  
{
protected:
	typedef BOOL (__stdcall *PFNGRADIENTFILL)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
	
public:

	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPDrawHelpers object.
	//-----------------------------------------------------------------------
	CXTPDrawHelpers();
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXCSplitPath object, handles cleanup and de-
	//          allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDrawHelpers();

private:

	HMODULE m_hMsImgDll; // Handle to MsImg32.dll.

public:

	//-----------------------------------------------------------------------
	// Summary: Call this member function to perform a gradient fill using
	//          the specified COLORREF values crFrom and crTo.
	// Input:   pDC    - Points to the device context
	//          lpRect - Size of area to fill
	//          crFrom - RGB value to start from
	//          crTo   - RGB value to fill to
	//          bHorz  - TRUE if fill is horizontal.
	//-----------------------------------------------------------------------
	void GradientFill(CDC* pDC, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to perform a gradient fill using
	//          the specified CXTP_GRADIENT_COLOR structure.
	// Input:   pDC    - Points to the device context
	//          lpRect - Size of area to fill
	//          grc    - XTP_GRADIENT_COLOR struct containing start from and
	//                   fill to RGB color values.
	//          bHorz  - TRUE if fill is horizontal.
	//-----------------------------------------------------------------------
	void GradientFill(CDC* pDC, LPRECT lpRect, XTP_GRADIENT_COLOR& grc, BOOL bHorz);

	//-----------------------------------------------------------------------
	// Summary: Call this member to exclude the pixels for each corner of the
	//          area specified by rc.
	// Input:   pDC - Points to the device context
	//          rc  - Area to exclude corners from
	//-----------------------------------------------------------------------
	void ExcludeCorners(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to blur the color value for the 
	//          points specified by pts
	// Input:   pDC    - Points to the device context
	//          pts    - Array of points to blur
	//          nCount - Number of points in array.
	//-----------------------------------------------------------------------
	void BlurPoints(CDC* pDC, LPPOINT pts, int nCount);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to blend the colors specified by
	//          crA and crB.
	// Input:   crA      - RGB value to blend
	//          crB      - RGB value to blend
	//          fAmountA - The amount that crA saturates crB.
	// Returns: An RGB value that represents the blended color.
	//-----------------------------------------------------------------------
	COLORREF BlendColors(COLORREF crA, COLORREF crB, float fAmountA);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the specified font
	//          exists.
	// Input:   strFaceName - A NULL terminated string that represents the
	//                        font face name.
	// Returns: TRUE if the font was found, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL FontExists(CString& strFaceName);

	static CString AFX_CDECL GetDefaultFontName();

	CString GetVerticalFontName(BOOL bUseOfficeFont);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw a triangle onto the
	//          specified device context.
	// Input:   pDC - Points to the device context
	//          pt0 - First point of the triangle
	//          pt1 - Second point of the triangle
	//          pt2 - Third point of the triangle
	//          clr - An RGB value that represents the fill color.
	//-----------------------------------------------------------------------
	static void AFX_CDECL Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine if the system display
	//          is set to low resolution.
	// Input:   hDC - Handle to a device context, if NULL the desktop window
	//                device context is used.
	// Returns: TRUE if the system display is set to low resolution,
	//          otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsLowResolution(HDC hDC = 0);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to determine the current Windows XP
	//          theme in use.
	// Returns: A XTPCurrentSystemTheme enumeration that represents the 
	//          current Windows theme in use, can be one of the following 
	//          values:
	//          xtpSystemThemeUnknown - No known theme in use
	//          xtpSystemThemeBlue    - Blue theme in use
	//          xtpSystemThemeOlive   - Olive theme in use
	//          xtpSystemThemeSilver  - Silver theme in use
	//-----------------------------------------------------------------------
	XTPCurrentSystemTheme GetCurrentSystemTheme();

	BOOL IsFastFillAvailable();
	BOOL GradientFill(HDC hdc, PTRIVERTEX pVertex, ULONG dwNumVertex, PVOID pMesh, ULONG dwNumMesh, ULONG dwMode); 

	//-----------------------------------------------------------------------
	// Summary: This function retrieves work area for the display monitor that 
	//          the mouse cursor is currently positioned over.
	// Returns: If the mouse cursor is positioned over a display monitor, the 
	//          return value is a CRect object that specifies the work area 
	//          rectangle of the display monitor, expressed in virtual-screen 
	//          coordinates.  Otherwise, returns the size of the work area on 
	//          the primary display monitor. 
	//-----------------------------------------------------------------------
	static CRect AFX_CDECL GetWorkArea();
	
	//-----------------------------------------------------------------------
	// Input:   lpRect  - Pointer to a RECT structure that specifies the 
	//                    coordinates of the rectangle of interest in virtual 
	//                    screen coordinates. 
	// Summary: This function retrieves work area for the display monitor that 
	//          contains the specified RECT coordinates.
	// Returns: If the display monitor contains the specified coordinates, the 
	//          return value is a CRect object that specifies the work area 
	//          rectangle of the display monitor, expressed in virtual-screen 
	//          coordinates.  Otherwise, returns the size of the work area on 
	//          the primary display monitor. 
	//-----------------------------------------------------------------------
	static CRect AFX_CDECL GetWorkArea(const CRect& rect);
	
	//-----------------------------------------------------------------------
	// Input:   pWnd    - Pointer to the window of interest.
	// Summary: This function retrieves work area for the display monitor that 
	//          contains the specified CWnd object.
	// Returns: If the display monitor contains the specified CWnd object, the 
	//          return value is a CRect object that specifies the work area 
	//          rectangle of the display monitor, expressed in virtual-screen 
	//          coordinates.  Otherwise, returns the size of the work area on 
	//          the primary display monitor. 
	//-----------------------------------------------------------------------
	static CRect AFX_CDECL GetWorkArea(const CWnd* pWnd);

private:

	PFNGRADIENTFILL m_pfnFastGradientFill;

	// private members used for draw routines.
	void GradientFillFast(CDC* pDC, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);
	void GradientFillSlow(CDC* pDC, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);

	// singleton
	static CXTPDrawHelpers m_instance;
	friend CXTPDrawHelpers* XTPDrawHelpers();
};


//---------------------------------------------------------------------------
// Summary:	Call this member function to access CXTPDrawHelpers members. 
//          Since this class is designed as a single instance object you can 
//          only access its members thru this method. You <b>cannot</b> 
//          directly instantiate an object of type CXTPDrawHelpers.
// Example: <pre>BOOL bLowRes = XTPDrawHelpers()->IsLowResolution();</pre>
//---------------------------------------------------------------------------
AFX_INLINE CXTPDrawHelpers* XTPDrawHelpers() {
	return &CXTPDrawHelpers::m_instance;
}

AFX_INLINE BOOL CXTPDrawHelpers::IsFastFillAvailable() {
	return m_pfnFastGradientFill != NULL;
}
AFX_INLINE void CXTPSplitterTracker::SetBoundRect(CRect rcBoundRect) {
	m_rcBoundRect = rcBoundRect;
}
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPDRAWHELPERS_H__)
