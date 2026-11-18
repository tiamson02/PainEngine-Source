// XTPReportPaintManager.h: interface for the CXTPReportPaintManager class.
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

//:Ignore
#if !defined(__XTPREPORTPAINTMANAGER_H__)
#define __XTPREPORTPAINTMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPReportDefines.h"

class CXTPReportControl;
class CXTPReportRow;
class CXTPReportHeader;
class CXTPReportColumn;
class CXTPReportGroupRow;

//===========================================================================
// Remarks:	Represents predefined grid line styles:
//			xtpGridNoLines		- empty line
//			xtpGridSmallDots	- line is drawn with small dots
//			xtpGridLargeDots	- line is drawn with large dots
//			xtpGridDashes		- line is drawn with dashes
//			xtpGridSolid		- draws solid line
//
//===========================================================================
enum XTPReportGridStyle
{
	xtpGridNoLines,		// empty line
	xtpGridSmallDots,	// line is drawn with small dots	
	xtpGridLargeDots,	// line is drawn with large dots	
	xtpGridDashes,		// line is drawn with dashes
	xtpGridSolid		// draws solid line
};

//===========================================================================
//Remarks: Represents predefined column styles:
//			xtpColumnShaded - columns are grey shaded
//			xtpColumnFlat	- flat style for drawing columns
//===========================================================================
enum XTPReportColumnStyle
{
	xtpColumnShaded,	// Columns are grey shaded.
	xtpColumnFlat		// Flat style for drawing columns.
};

//===========================================================================
// Remarks:	Utility class, handling most of the drawing activities. It stores 
//          all settings, needed by a control to perform drawing operations: 
//			fonts, colors, styles for all other classes of control. 
//			It also implements all functions for drawing typical graphical 
//			primitives, functions that directly work with device context.
//
//          Could be overridden to provide another look and feel for the control.    
//          Thus you have an easy way to change "skin" of your control; just provide
//          your own implementation of CXTPReportPaintManager and there's no need to touch
//          functionality all others classes of control      
//             
//          You create ReportPaintManager by calling its constructor, 
//			further you can call getter and setter functions to change 
//			every setting as you wish.
// See also: CXTPReportControl overview, CXTPReportRow, CXTPReportGroupRow,
//			 CXTPReportColumn, CXTPReportHeader
//===========================================================================
class _XTP_EXT_CLASS CXTPReportPaintManager : public CCmdTarget
{
	friend class CXTPReportControl;
public:
	class CReportColor
	{
	public:
		CReportColor()
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
		const CReportColor&  operator=(COLORREF clr) {
			m_clrCustomValue = clr;
			return *this;
		}		
		
	protected:
		COLORREF m_clrStandardValue;
		COLORREF m_clrCustomValue;
		
	};

public:
	//-----------------------------------------------------------------------
	// Summary: Default paint manager constructor, 
	//			handles properties initialization.
	// See also: RefreshMetrics
	//-----------------------------------------------------------------------
	CXTPReportPaintManager();
	
	//-----------------------------------------------------------------------
	// Summary: Default paint manager destructor, 
	//			handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportPaintManager();

	//-----------------------------------------------------------------------
	// Summary: Initializes all drawings defaults (fonts, colors, etc.). Most 
	//			of them are system defaults.
	// See also: CXTPReportPaintManager
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Input:	bGroupRow - TRUE for group rows, FALSE otherwise.
	// Summary: Returns default row height.
	// Returns: The height of the default rectangle where row's items will draw.
	// Example: 
	//          // Get default Group row height   
	//          int nRowHeight = GetRowHeight(TRUE) 
	//
	//          // Get default row height   
	//          int nRowHeight = GetRowHeight(FALSE) 
	//
	//-----------------------------------------------------------------------
	virtual int GetRowHeight(BOOL bGroupRow);

	//-----------------------------------------------------------------------
	// Summary: Returns the default column header height.
	// Returns: The current height of the rectangle where columns's headers draw.
	//-----------------------------------------------------------------------
	virtual int GetHeaderHeight();

	//-----------------------------------------------------------------------
	// Input:	pControl - Pointer to the parent report control object.
	// Summary: Returns control background color.
	// Returns: Color that is used for filling client rect.
	//-----------------------------------------------------------------------
	virtual COLORREF GetControlBackColor(CXTPReportControl* pControl);

	//-----------------------------------------------------------------------
	// Input:	pDC		- Pointer to the used Device Context.
	//          pRow	- Pointer to the Row.
	//			rcRow	- Row area rectangle coordinates.
	// Summary: Fills the provided row with its background color.
	// Example:
	//			// paint row background
	//			//pPaintManager->FillRow(pDC, this, rcRow);
	//
	//-----------------------------------------------------------------------
	virtual void FillRow(CDC* pDC, CXTPReportRow* pRow, CRect rcRow);

	//-----------------------------------------------------------------------
	// Input:	pDC		  -	Pointer to the used Device Context.
	//			bVertical - boolean value determines orientation of grid
	//			rc        -	Row area rectangle coordinates.
	// Summary: Draws control's grid.
	// Example:
	//          // draw vertical grid in Row
	//			CRect rcGridItem(rcRowRect);
	//          pPaintManager->DrawGrid(pDC, TRUE, rcGridItem);
	//           
	//          // draw horizontal grid in Row
	//          CPaintDC pDC(this);      
	//			CRect rcGridItem(rcRowRect);
	//          pPaintManager->DrawGrid(pDC, FALSE, rcGridItem);
	//           
	//-----------------------------------------------------------------------
	virtual void DrawGrid(CDC* pDC, BOOL bVertical, CRect rc);

	//-----------------------------------------------------------------------
	// Input:   pDC   -	Pointer to the used Device Context.
	//          pRow  -	Pointer to the Group Row.
	//			rcRow - Group Row area rectangle coordinates.
	// Summary: Draws group row.
	// Example: 
	//			CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	//			pDC->SetBkMode(TRANSPARENT);
	//			pPaintManager->DrawGroupRow(pDC, this, rcRow);
	//
	//-----------------------------------------------------------------------
	virtual void DrawGroupRow(CDC* pDC, CXTPReportGroupRow* pRow, CRect rcRow);

	//-----------------------------------------------------------------------
	// Input:	pDC	  - Pointer to the used Device Context. 
	//			rcRow - Row area rectangle coordinates.
	// Summary: Fills indent area by its background.
	// Example:
	//			CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	//			CRect rcRow(rcClient);
	//          // Get default row height   
	//          int nRowHeight = m_pControl->GetRowHeight(FALSE) 
	//          rcRow.bottom = rcRow.top + nRowHeight;
	//		    pPaintManager->FillIndent(pDC, rcRow);	
	//-----------------------------------------------------------------------
	virtual void FillIndent(CDC* pDC, CRect rcRow);

	//-----------------------------------------------------------------------
	// Input:	pDC    - Pointer to the used Device Context.
	//			rcItem - Item area rectangle coordinates.
	// Summary: Fills item shade by its background if necessary.
	// Example:
	//			CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	//          CRect rcItem(m_rcRow);
	//          pPaintManager->FillItemShade(pDC, rcItem); 
	//-----------------------------------------------------------------------
	virtual void FillItemShade(CDC* pDC, CRect rcItem);

	//-----------------------------------------------------------------------
	// Input:	pDC      -	Pointer to the used Device Context. 
	//			rcHeader -	Header area rectangle coordinates.
	// Summary: Fills column header control area by its background.
	// Example:
	//			CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	//          CRect rcHeader(rcClient);
	//          int nHeaderHeight = m_pControl->GetHeaderHeight();
	//          rcHeader.bottom = rcHeader.top + nHeaderHeight;
	//          pPaintManager->FillHeaderControl(pDC, rcHeader); 
	//-----------------------------------------------------------------------
	virtual void FillHeaderControl(CDC* pDC, CRect rcHeader);

	//-----------------------------------------------------------------------
	// Input:   pDC           - Pointer to the used Device Context.
	//			pColumn	      - Column header area rectangle coordinates.
	//			pHeader       - Pointer to report header.
	//			rcColumn      - Column area rectangle coordinates.
	//          bDrawExternal - whether draw Column on Header area, default is FALSE.
	// Summary: Draws column header with all related attributes (sort order, icon, etc).
	// Example:
	//			CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	//			CXTPReportColumn* pColumn = CXTPReportColumn* pColumn = m_pColumns->GetAt(1);
	//          CRect rcHeader(rcClient);
	//          int nHeaderHeight = m_pControl->GetHeaderHeight();
	//          rcHeader.bottom = rcHeader.top + nHeaderHeight;
	//			GetPaintManager()->DrawColumn(pDC, pColumn, this, rcHeader);
	//-----------------------------------------------------------------------
	virtual void DrawColumn(CDC* pDC, CXTPReportColumn* pColumn, CXTPReportHeader* pHeader, CRect rcColumn, BOOL bDrawExternal = FALSE);

	//-----------------------------------------------------------------------
	// Input:   pDC     - Pointer to the used Device Context.
	//			pHeader - Pointer to report header.
	//			rcRow   - Area rectangle coordinates.
	// Summary: Draws rows that were focused.
	// Example:
	//			CRect rcClient(GetParent()->GetClientRect());
	//			CXTPReportHeader* pHeader = m_pColumns->GetReportHeader();
	//			CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	//			GetPaintManager()->DrawResizingRect(pDC, pHeader, rcClient);
	//-----------------------------------------------------------------------
	virtual void DrawFocusedRow(CDC* pDC, CRect rcRow);

	//-----------------------------------------------------------------------
	// Input:   pDC       - Pointer to the used Device Context.
	//			rcGroupBy - Reference to Group By box area rectangle coordinates.
	// Summary: Draws Group By box.
	// Example:
	//			CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	//			pPaintManager->FillGroupByControl(pDC, rcGroupBy);
	//-----------------------------------------------------------------------
	virtual void FillGroupByControl(CDC* pDC, CRect& rcGroupBy);


	//-----------------------------------------------------------------------
	// Summary: Returns point to additional image list with Glyphs
	// Remarks: Additional image list usually used for bitmaps of collapsed icons, etc
	//			You call this member function to get a pointer to list and manipulate
	//			its content as you wish
	// Returns: Pointer to the instantiated object of CImageList class
	//-----------------------------------------------------------------------
	CImageList* GetGlyphsImageList();


	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to the used Device Context.
	//			x   - Horizontal coordinate of the beginning of line.
	//			y   - Vertical coordinate of the beginning of line.
	//			cx  - Length of line.
	//			clr - Color of line.
	// Summary: Draws Horizontal line.
	// Remarks: This member function is called in the control everywhere we need to 
	//			draw a simple horizontal line 
	//-----------------------------------------------------------------------
	void DrawHorizontalLine(CDC* pDC, int x, int y, int cx, COLORREF clr);

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to the used Device Context.
	//			x   - Horizontal coordinate of the beginning of line.
	//			y   - Vertical coordinate of the beginning of line.
	//			cy  - Length of line.
	//			clr - Color of line.
	// Summary: Draws vertical line.
	// Remarks: This member function is called in the control everywhere we need to 
	//			draw a simple vertical line 
	//-----------------------------------------------------------------------
	void DrawVerticalLine(CDC* pDC, int x, int y, int cy, COLORREF clr);

	//-----------------------------------------------------------------------
	// Input:	pDC        - Pointer to the used Device Context.
	//			rcTriangle - Triangle area rectangle coordinates.
	//			bToDown    - If TRUE the top of triangle turn downward, otherwise upward.
	// Summary: Draws triangle that shows the sort order of column. 
	// Remarks: You use this function to draw a triangle image on the header of the column
	//			that is sorted. Triangle represents the direction of sorting: ascending or descending.
	//			Sizes of the triangle depend on the rectangle area that is provided to draw
	// Example:
	//			// draw ascendant triangle
	//			CRect rcTriangle;
	//			rcTriangle.CopyRect(rcHeader);
	//			rcTriangle.DeflateRect(40, 5,5,5);
	//			DrawTriangle(pDC, rcTriangle, TRUE);
	//
	//			// draw descendant triangle
	//			CRect rcTriangle;
	//			rcTriangle.CopyRect(rcHeader);
	//			rcTriangle.DeflateRect(40, 5,5,5);
	//			DrawTriangle(pDC, rcTriangle, FALSE);
	//
	//-----------------------------------------------------------------------
	void DrawTriangle(CDC* pDC, CRect rcTriangle, BOOL bToDown);

	//-----------------------------------------------------------------------
	// Input:	pDC    - Pointer to the used Device Context.
	//			ptFrom - Source point of the connector.
	//			ptTo   - Target point of the connector.
	// Summary: Draws a column connector for Group By box. 
	// Remarks: You use this function to draw a line that represents a
	//			chain of columns in GroupBy box. It connects columns
	//			in GroupBy box altogether
	// Example:
	//			DrawConnector(pDC, CPoint(rcItem.right - 5, rcItem.bottom), CPoint(x, rcItem.bottom + 4));
	//-----------------------------------------------------------------------
	void DrawConnector(CDC* pDC, CPoint ptFrom, CPoint ptTo);

	//-----------------------------------------------------------------------
	// Input:	pDC      - Pointer to the used Device Context.
	//			pRow     - Pointer to the Row.
	//			rcBitmap - Bitmap area rectangle coordinates.
	// Summary: Draws collapsed bitmap. 
	// Returns:	Intended value (right bound of drawn bitmap).
	// Example:
	//-----------------------------------------------------------------------
	int DrawCollapsedBitmap(CDC* pDC, CXTPReportRow* pRow, CRect rcBitmap);

	//-----------------------------------------------------------------------
	// Input:	pDC      - Pointer to the used Device Context.
	//			rcColumn - Bitmap area rectangle coordinates.
	//			iImage   - Image index.
	// Summary: Draws collapsed bitmap. 
	// Returns:	Right bound of drawn bitmap.
	// Remarks  You use this function to draw a bitmap in the column rectangle.
	//			If the provided rectangle is too small, the bitmap will not be drawn.
	//			Generally, the column rectangle must be big enougth to contain
	//			text (if presented) and bitmap 
	// Example:
	//			// draws bitmap with id ID_READ 
	//			DrawBitmap(pPaintManager->GetGlyphsImageList(), pDC, rcColumn, ID_READ);
	//-----------------------------------------------------------------------
	int DrawBitmap(CImageList* pImageList, CDC* pDC, CXTPReportControl* pControl, CRect rcColumn, int iImage);
	
	//-----------------------------------------------------------------------
	// Input:	clrGridLine - New grid color.
	// Summary: Changes report control grid lines color.
	// Returns:	Previous grid color
	// Example:
	//			// Set new color, save old one
	//			COLORREF clrNew;
	//			clrNew = RGB(0,255,0);
	//			COLORREF clrOld;
	//			clrOld = SetGridColor(clrNew);
	//-----------------------------------------------------------------------
	COLORREF SetGridColor(COLORREF clrGridLine);
	
	//-----------------------------------------------------------------------
	// Input:	bIsPreviewMode - New preview mode (TRUE or FALSE).
	// Summary: Changes the preview mode state for the control.
	// Remarks:	Preview mode has two states: enable and disabled. When preview is
	//			enabled, the control tries to show additional bands with preview text
	// Example:
	//			// enable preview mode
	//			EnablePreviewMode(TRUE);
	//
	//			// disable preview mode
	//			nablePreviewMode(FALSE);
	//
	//-----------------------------------------------------------------------
	void EnablePreviewMode(BOOL bIsPreviewMode);

	//-----------------------------------------------------------------------
	// Summary: Returns preview mode state for the control.
	// Returns:	Current preview mode state.
	// Remarks: You use this member function to programmatically identify 
	//			in which mode the control is
	//-----------------------------------------------------------------------
	BOOL IsPreviewMode();
	
	//-----------------------------------------------------------------------
	// Summary: Returns the default text font for the control items.
	// Returns:	Pointer to the current font used for drawing test.
	//-----------------------------------------------------------------------
	CFont* GetTextFont();

	//-----------------------------------------------------------------------
	// Input:	nMaxLines - New value of max lines of text for a preview item.
	// Summary: Set count of max lines of the preview text. 
	// Returns: Previous value of max lines of text for a preview item.
	// Remarks: You use this member function to bound the height of a textPreview item
	//			by setting the maximum rows of text for it
	// Example:
	//			int nOldMaxLines = SetMaxPreviewLines(4);
	//-----------------------------------------------------------------------
	int SetMaxPreviewLines(int nMaxLines);

	//-----------------------------------------------------------------------
	// Summary: Get the count of max lines of  preview text.
	// Returns: Current value of max lines of text for a preview item.
	//-----------------------------------------------------------------------
	int GetMaxPreviewLines();

	//-----------------------------------------------------------------------
	// Input:	pDC     - Pointer to the used Device Context.
	//			rcText  - Reference to text area rectangle coordinates.
	//			strText - String of text.
	// Summary: Calculates the line count for preview text, takes into account max lines.
	// Returns: Returns the count of lines needed to draw preview text. 
	// Remarks: This function calculates the count of preview text lines based on the lenth
	//			of preview text string, preview text font and width of provided rectangle.
	//			If default max lines count less than count of calculate lines, 
	//			returns default max lines
	// Example:
	//-----------------------------------------------------------------------
	int GetPreviewLinesCount(CDC* pDC, CRect& rcText, CString strText);

	//-----------------------------------------------------------------------
	// Input:	bVertical - determines which grid lines will be tested, 
	//						vertical or horizontal.
	// Summary: Return visibility of grid line
	// Returns: boolean value which identifies the visibility of the line
	//-----------------------------------------------------------------------
	BOOL IsGridVisible(BOOL bVertical);

	//-----------------------------------------------------------------------
	// Input:	bVertical           - determines which grid lines settings, 
	//						          vertical or horizontal
	//			gridStyle           - greed style:
	//			xtpGridNoLines		- empty line
	//			xtpGridSmallDots	- line drawn by small dots	
	//			xtpGridLargeDots	- line drawn by large dots	
	//			xtpGridDashes		- line drawn by dashes	
	//			xtpGridSolid		- draws solid line
	// Summary: Sets one of the predefined grid line styles
	// See Also: XTPReportGridStyle
	//-----------------------------------------------------------------------
	void SetGridStyle(BOOL bVertical, XTPReportGridStyle gridStyle);

	//--------------------------------------------------------------------------
	// Input:	 lf - Reference to LOGFONT structure
	// Summary:  Creates and sets default text font
	// See also: LOGFONT
	//-----------------------------------------------------------------------
	void SetTextFont(LOGFONT& lf);

	//-----------------------------------------------------------------------
	// Input:	 lf - Reference to LOGFONT structure
	// Summary:  Creates and sets the default caption font
	// See also: LOGFONT
	//-----------------------------------------------------------------------
	void SetCaptionFont(LOGFONT& lf);

	//-----------------------------------------------------------------------
	// Summary: Call this method to set the preview item indent.
	//-----------------------------------------------------------------------
	void SetPreviewIndent(int nLeft, int nTop, int nRight, int nBottom);
	
	
protected:
	
	//-----------------------------------------------------------------------
	// Input:	pDC		 -	Pointer to device context.
	//			pBrush	 -	Pointer to brush object.
	//			rc		 -	Rectangle area.
	//			pPattern -	Points to a short-integer array that contains the 	
	//						initial bitmap bit values. If it is NULL, the new
	//						bitmap is left uninitialized. 
 	// Summary: Generates a custom grid pattern
	//-----------------------------------------------------------------------
	void DrawGridPat(CDC* pDC, CBrush* pBrush, CRect rc, CONST VOID*pPattern);

public:
	CFont m_fontText;			// Report items default text.
	CFont m_fontBoldText;		// Report items default bold text.
	CFont m_fontCaption;		// Column header caption font.
	CFont m_fontPreview;		// Column header caption font.

public:
	CReportColor m_clrHighlight;		// Background color of the highlighted row.
	CReportColor m_clrGridLine;			// Grid lines color.
	CReportColor m_clrWindowText;		// Report items default text color.
	CReportColor m_clrHighlightText;	// Text color of the highlighted text.
	CReportColor m_clrHeaderControl;	// Background color of the report header.
	CReportColor m_clrCaptionText;		// Column header text color.
	CReportColor m_clrControlBack;		// Background color of the control report area.
	CReportColor m_clrGroupRowText;		// Row text color.
	CReportColor m_clrGroupShadeBack;	// Group row background color when indentation shade is enabled.
	CReportColor m_clrGroupShadeBorder;	// Color for group border's shade.

	CReportColor m_clrControlDark;		// Dark background color of the report control (used on Group By area).
	CReportColor m_clrControlLightLight;// Light background color used for drawing shades (used on column header area).
	CReportColor m_clrHotDivider;		// Color of the column hot divider window (2 arrows).
	CReportColor m_clrHyper;			// Hyperlink color.
	CReportColor m_clrIndentControl;	// Color of the tree indentation area.
	CReportColor m_clrItemShade;		// Color of the shade on sorted by column items.

	CReportColor m_clrPreviewText;		// Preview text color.
	BOOL m_bShadeGroupHeadings;		// Show or not show the indentation shade.
	BOOL m_bShadeSortColumn;		// Show or not show the items shade in the sorted by column.

	CReportColor m_clrBtnFace;			// Standard button face color.
	CReportColor m_clrBtnText;			// Standard button text color.


	CString m_strNoItems;			//  A string which contains customized "No Items" text for 
									//+ displaying in view when there are no visible rows.
	CString m_strNoGroupBy;			//  A string which contains customized "No items" text message for
									//+ displaying in Group By area when there are no items inside it.

	BOOL m_bHideSelection;			//  TRUE when hiding report control's selection by drawing selected items as others,
									//+ FALSE otherwise.

	XTPReportColumnStyle m_columnStyle; // Contains column drawing style.
	CRect m_rcPreviewIndent;


protected:
	int m_nRowHeight;			// Report row default height.
	int m_nHeaderHeight;		// Column header height.
	int m_nGroupGridLineHeight;	// The height of the group grid line.


	CImageList m_ilGlyphs;		// Contains an additional image list for report control

	BOOL m_bIsPreviewMode;		// Show or not show the item preview.

	int m_nResizingWidth;		// The width of the column resizing marker area.

	int m_nMaxPreviewLines;     // Restrict maximum lines for preview text

	CSize m_szGlyph;			// Glyph size

	CBrush m_brushVeriticalGrid;	// Stores brush to draw vertical grid lines
	CBrush m_brushHorizontalGrid;	// Stores brush to draw horizontal grid lines
	
	XTPReportGridStyle m_verticalGridStyle;		// Stores current style for vertical grid lines
	XTPReportGridStyle m_horizontalGridStyle;	// Stores current style for horizontal grid lines


#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportPaintManager);

	afx_msg int OleGetVerticalGridStyle();
	afx_msg void OleSetVerticalGridStyle(int nGridStyle);
	afx_msg int OleGetHorizontalGridStyle();
	afx_msg void OleSetHorizontalGridStyle(int nGridStyle);

	afx_msg LPFONTDISP OleGetTextFont();
	afx_msg void OleSetTextFont(LPFONTDISP pFontDisp);
	afx_msg LPFONTDISP OleGetCaptionFont();
	afx_msg void OleSetCaptionFont(LPFONTDISP pFontDisp);
	afx_msg LPFONTDISP OleGetPreviewTextFont();
	afx_msg void OleSetPreviewTextFont(LPFONTDISP pFontDisp);


#define DECLARE_PROPERTY_UN(theProperty, theType)\
	afx_msg theType OleGet##theProperty();\
	afx_msg void OleSet##theProperty(theType);

	DECLARE_PROPERTY_UN(BackColor, COLORREF);
	DECLARE_PROPERTY_UN(ForeColor, COLORREF);
	DECLARE_PROPERTY_UN(GridLineColor, COLORREF);
	DECLARE_PROPERTY_UN(ShadeSortColor, COLORREF);
	DECLARE_PROPERTY_UN(GroupIndentColor, COLORREF);
	DECLARE_PROPERTY_UN(GroupShadeBackColor, COLORREF);
	DECLARE_PROPERTY_UN(GroupShadeBorderColor, COLORREF);
	DECLARE_PROPERTY_UN(GroupForeColor, COLORREF);
	DECLARE_PROPERTY_UN(CaptionBackColor, COLORREF);
	DECLARE_PROPERTY_UN(CaptionForeColor, COLORREF);	

#endif
};

AFX_INLINE int CXTPReportPaintManager::GetHeaderHeight() {
	return m_nHeaderHeight;
}
AFX_INLINE BOOL CXTPReportPaintManager::IsGridVisible(BOOL bVertical) {
	return bVertical? (m_verticalGridStyle != xtpGridNoLines): (m_horizontalGridStyle != xtpGridNoLines);
}

AFX_INLINE void CXTPReportPaintManager::EnablePreviewMode(BOOL bIsPreviewMode) {
	m_bIsPreviewMode = bIsPreviewMode;
}
AFX_INLINE BOOL CXTPReportPaintManager::IsPreviewMode(){
	return m_bIsPreviewMode;
}
AFX_INLINE COLORREF CXTPReportPaintManager::SetGridColor(COLORREF clrGridLine){
	COLORREF clrOldColor = m_clrGridLine;
	m_clrGridLine = clrGridLine;
	return clrOldColor;
}

AFX_INLINE CFont* CXTPReportPaintManager::GetTextFont() {
	return &m_fontText;
}
AFX_INLINE int CXTPReportPaintManager::SetMaxPreviewLines(int nMaxLines) {
	int nOldMaxLines = m_nMaxPreviewLines;
	m_nMaxPreviewLines = nMaxLines;
	return nOldMaxLines;
}
AFX_INLINE int CXTPReportPaintManager::GetMaxPreviewLines() {
	return m_nMaxPreviewLines;
}
AFX_INLINE CImageList* CXTPReportPaintManager::GetGlyphsImageList() {
	return &m_ilGlyphs;
}


#endif
