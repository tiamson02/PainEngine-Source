// XTPReportRecordItem.h: interface for the CXTPReportRecordItem class.
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
#if !defined(__XTPREPORTRECORDITEM_H__)
#define __XTPREPORTRECORDITEM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPReportHyperlink.h"


class CXTPReportControl;
class CXTPReportRow;
class CXTPReportRecordItem;
class CXTPReportHyperlink;
class CXTPReportColumn;
class CXTPReportPaintManager;
class CXTPReportRecord;
class CXTPReportInplaceButton;

//===========================================================================
// Remarks: Basic set of parameters transferred to item handlers.
//			This structure groups togeter essential parameters used in
//			items processing activities. It is parent for all other structures:
//			XTP_REPORTRECORDITEM_DRAWARGS, XTP_REPORTRECORDITEM_CLICKARGS
//===========================================================================
struct _XTP_EXT_CLASS XTP_REPORTRECORDITEM_ARGS
{
	XTP_REPORTRECORDITEM_ARGS();
	XTP_REPORTRECORDITEM_ARGS(CXTPReportControl* pControl, CXTPReportRow* pRow, CXTPReportColumn* pColumn);

	CXTPReportControl* pControl;	// Pointer to the main Report control.
	CXTPReportRow* pRow;			// Pointer to the associated row.
	CXTPReportColumn* pColumn;		// Report column at click position, if any, NULL otherwise.
	CXTPReportRecordItem* pItem;
	CRect rcItem;					// Item position in control client coordinates.
};

//===========================================================================
// Remarks: A set of parameters transferred to Draw item handler.
//			It inherits essential parameters from XTP_REPORTRECORDITEM_ARGS.
//			Additional parameters are:
//							CDC* pDC       -Pointer to control drawing context.
//							int nTextAlign - Text alignment mode, can take next values:
//											  DT_LEFT   - align text left;
//                                            DT_RIGHT  - align text right;
//											  DT_CENTER - align text center.
//							CXTPReportRecordItem* pItem - Record item to draw.
//			This structure is used in almost all functions concerning drawing
//          activities 
//===========================================================================
struct _XTP_EXT_CLASS XTP_REPORTRECORDITEM_DRAWARGS : XTP_REPORTRECORDITEM_ARGS
{
	CDC* pDC;						// Pointer to control drawing context.
	int nTextAlign;                 // Text alignment mode DT_LEFT||DT_RIGHT||DT_CENTER
};

//===========================================================================
// Remarks: A set of parameters transferred to mouse click item handlers.
//			It inherits essential parameters from XTP_REPORTRECORDITEM_ARGS.
//			XTP_REPORTRECORDITEM_CLICKARGS is used in most functions involved  
//			in mouse events processing			
//===========================================================================
struct _XTP_EXT_CLASS XTP_REPORTRECORDITEM_CLICKARGS : XTP_REPORTRECORDITEM_ARGS
{
	CPoint ptClient;				// Coordinates of the mouse click point.
};

//===========================================================================
// Remarks: This structure is sent to Main window in a WM_NOTIFY message from Item
//			and provides all parameters that are needed in processing control specific 
//			notifications by the main window 
//===========================================================================
struct XTP_NM_REPORTRECORDITEM
{
    NMHDR hdr;							// Standard structure, containing information about a notification message. 
    CXTPReportRow* pRow;				// Pointer to the row assosiated with the notification.
    CXTPReportRecordItem* pItem;		// Pointer to the record item assosiated with the notification.
	CXTPReportColumn* pColumn;			// Pointer to the column assosiated with the notification.
	int   nHyperlink;                   // Index of clicked Hyperlink, if any, or -1 otherwise.
	POINT pt;							// Point where the message has happened.
};

struct XTP_NM_REPORTINPLACEBUTTON : public XTP_NM_REPORTRECORDITEM
{
	CXTPReportInplaceButton* pButton;
};

//===========================================================================
// Remarks: Record item drawing metrics.
//			Its helper structures group together parameters to store general
//			drawing metrics 
//===========================================================================
struct XTP_REPORTRECORDITEM_METRICS : public CCmdTarget
{
	CFont* pFont;						// Drawing font.
	COLORREF clrForeground;				// Item foreground color.
	COLORREF clrBackground;				// Item background color.
	CString strText;					// Item text.

#ifdef _XTP_ACTIVEX

	XTP_REPORTRECORDITEM_METRICS()
	{
		pFont = NULL;
		EnableAutomation();
		
		m_lpFont = NULL;
	}
	~XTP_REPORTRECORDITEM_METRICS()
	{
		if (m_lpFont)
		{
			m_lpFont->Release();
		}
	}

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	void OleOnFontChanged();
	LPFONTDISP m_lpFont;
	CFont m_fntMetrics;
#endif
};

class _XTP_EXT_CLASS CXTPReportRecordItemConstraint : public CCmdTarget
{
public:
	CXTPReportRecordItemConstraint();

public:
	int GetIndex();

public:
	CString m_strConstraint;
	DWORD m_dwData;

protected:
	int m_nIndex;

//:Ignore
#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportRecordItemConstraint);
	
#endif

	friend class CXTPReportRecordItemConstraints;
	friend class CXTPReportRecordItemEditOptions;
//:End Ignore
};

class _XTP_EXT_CLASS CXTPReportRecordItemConstraints : CCmdTarget
{
public:
	CXTPReportRecordItemConstraints();		
	~CXTPReportRecordItemConstraints();

public:
	int GetCount();
	void RemoveAll();
	CXTPReportRecordItemConstraint* GetAt(int nIndex);

protected:
	CArray<CXTPReportRecordItemConstraint*, CXTPReportRecordItemConstraint*> m_arrConstraints;

//:Ignore

#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportRecordItemConstraints);

	afx_msg LPDISPATCH OleItem(long nIndex);
	afx_msg void OleAdd(LPCTSTR lpszCaption, long dwData);
	afx_msg LPUNKNOWN OleNewEnum();	

	BEGIN_INTERFACE_PART(EnumVARIANT, IEnumVARIANT)
		HRESULT STDMETHODCALLTYPE Next( 
		/* [in] */ ULONG celt,
		/* [length_is][size_is][out] */ VARIANT *rgVar,
		/* [out] */ ULONG *pCeltFetched);

		HRESULT STDMETHODCALLTYPE Skip( 
		/* [in] */ ULONG celt);

		HRESULT STDMETHODCALLTYPE Reset( void);

		HRESULT STDMETHODCALLTYPE Clone( 
			/* [out] */ IEnumVARIANT **ppEnum);
			int m_nCurrent;
	END_INTERFACE_PART(EnumVARIANT)

	enum {
		dispidCount = 1L,
		dispidAdd = 2L,
	};
	
#endif
//:End Ignore

	friend class CXTPReportRecordItemEditOptions;

};

class CXTPReportInplaceButtons : public CArray<CXTPReportInplaceButton*, CXTPReportInplaceButton*>
{

};

//////////////////////////////////////////////////////////////////////////
class _XTP_EXT_CLASS CXTPReportRecordItemEditOptions : public CCmdTarget
{
public:
	CXTPReportRecordItemEditOptions();
	~CXTPReportRecordItemEditOptions();

public:
	CXTPReportRecordItemConstraint* AddConstraint(LPCTSTR lpszConstraint, DWORD dwData = 0);
	CXTPReportRecordItemConstraint* FindConstraint(DWORD dwData);
	CXTPReportRecordItemConstraint* FindConstraint(LPCTSTR lpszConstraint);
	
	CXTPReportRecordItemConstraints* GetConstraints() {
		return m_pConstaints;
	}

	void AddComboButton();
	void AddExpandButton();
	
public:
	BOOL m_bAllowEdit;
	BOOL m_bConstraintEdit;
	
	CXTPReportRecordItemConstraints* m_pConstaints;
	CXTPReportInplaceButtons arrInplaceButtons;	

//:Ignore
#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportRecordItemEditOptions);

	afx_msg LPDISPATCH OleGetConstraints();
	
#endif
//:End Ignore
};

//===========================================================================
// Remarks: Class for working with single list cell.
//          Base class for extended list cells. It determines the behavior 
//			of all specific record items.
//			You don't use its class directly in the control.
//			If you wish to add your own Record Item to the control, you must inherit
//			it from CXTPReportRecordItem. Most member functions in this class
//			are virtual.
// See also: CXTPReportRecordItemDateTime, CXTPReportRecordItemNumber,
//			 CXTPReportRecordItemPreview, CXTPReportRecordItemText,
//			 CXTPReportRecordItemVariant
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItem : public CCmdTarget
{
	DECLARE_DYNAMIC(CXTPReportRecordItem)
public:
	//-----------------------------------------------------------------------
	// Summary: CXTPReportRecordItem default constructor
	//-----------------------------------------------------------------------
	CXTPReportRecordItem();
	
	//-----------------------------------------------------------------------
	// Summary: CXTPReportRecordItem default destructor
	//-----------------------------------------------------------------------
	virtual ~CXTPReportRecordItem(void);

	//-----------------------------------------------------------------------
	// Input:    pDrawArgs - structure which contains drawind arguments:
	//							CDC* pDC       - Pointer to control drawing context.
	//							int nTextAlign - Text alignment mode, can take next values:
	//											  DT_LEFT   - align text left;
	//                                            DT_RIGHT  - align text right;
	//											  DT_CENTER - align text center.
	//							CXTPReportRecordItem* pItem - Record item to draw.

	// Summary: Should be overriden by descendants for drawing itself.
	// Remarks: Call this member functin to draw an item. Actually this function 
	//			only prepares and calls needed drawing functions from PaintManager.
	//			Thus if you wish to change the look of your report item, you must just provide
	//          your own implementation of PaintManager 
	// Example:
	//				// fill structure
	//				XTP_REPORTRECORDITEM_DRAWARGS drawArgs;
	//				drawArgs.pDC = pDC;
	//				drawArgs.pControl = m_pControl;
	//				drawArgs.pRow = this;
	//				// call function
	//				Draw(&drawArgs);
	//
	// See Also: XTP_REPORTRECORDITEM_DRAWARGS
	//-----------------------------------------------------------------------
	virtual int Draw(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs);

	//-----------------------------------------------------------------------
	// Input:   pDrawArgs - structure which contain drawind arguments.
	// Summary: Should be overriden by descendants for drawing itself.
	//-----------------------------------------------------------------------
	virtual void OnDrawCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics);

    //-----------------------------------------------------------------------
	// Input:	 pClickArgs	- structure	which contains mouse click arguments
    // Summary:	 Processes single mouse clicks.
    // Remarks:  Usually this function is called by ReportRow's OnClick function
    // See also: XTP_REPORTRECORDITEM_CLICKARGS
    //-----------------------------------------------------------------------
	virtual void OnClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs);

	//-----------------------------------------------------------------------
	// Input:    pClickArgs - structure which contains mouse click arguments:
	//				CXTPReportColumn* pColumn - Report column at click position, if any, NULL otherwise.
	//				CPoint ptClient  - Coordinates of the mouse click point.
	// Summary:  Processes single mouse clicks.
	// Remarks:  Usually this function is called by ReportRow's OnDblClick function
	// See also: XTP_REPORTRECORDITEM_CLICKARGS
	//-----------------------------------------------------------------------
	virtual void OnDblClick(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs);

	//-----------------------------------------------------------------------
	// Input:   point  - mouse point
	//			nFlags - additional flags.
	// Summary: Processes the move mouse event.
	// Remarks: Usually this function is called by ReportRow's OnDblClick function
	//-----------------------------------------------------------------------
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary: Returns item text caption.
	// Remarks: Not valid for general item. 
	//			Should be overriden by descendants.
	// Returns: Item text caption (empty string for base record item class).
	//-----------------------------------------------------------------------
	virtual CString GetCaption(CXTPReportColumn* /*pColumn*/) {
		return m_strCaption;
	};

	virtual void GetCaptionRect(XTP_REPORTRECORDITEM_ARGS* pDrawArgs, CRect& rcItem);

	//-----------------------------------------------------------------------
	// Input:   pColumn - point to the column
	// Summary: Returns group captipn.
	// Returns: Text of Group Caption.
	//-----------------------------------------------------------------------
	virtual CString GetGroupCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - Point to the column
	// Summary: Returns resource id of string containing text 
	// Returns: Integer value of resource id
	//-----------------------------------------------------------------------
	virtual int GetGroupCaptionID(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - Point to the column
	//			pItem   - Point to the item
	// Summary: Compares group captions.
	// Returns: Zero if the items' values are identical, 
	//			< 0 if this item value is less than provided, 
	//			or > 0 if this item value is greater than provided.
	//-----------------------------------------------------------------------
	virtual int CompareGroupCaption(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem);

	//-----------------------------------------------------------------------
	// Input:	pItem - point to the compared Item
	// Summary: Compares this item with the provided one.
	//			Provides default behaviour for descendants.
	// Returns: Zero if the items' values are identical, 
	//			< 0 if this item value is less than provided, 
	//			or > 0 if this item value is greater than provided.
	//-----------------------------------------------------------------------
	virtual int Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem);

	//-----------------------------------------------------------------------
	// Input:	pFont - New font for caption.
	// Summary: Sets item text font.
	//-----------------------------------------------------------------------
	virtual void SetFont(CFont* pFont);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to get item text font.
	// Returns:	Pointer to the current text font.
	//-----------------------------------------------------------------------
	virtual CFont* GetFont();

	//-----------------------------------------------------------------------
	// Input:	clrText - New color. 
	// Summary: Sets item text color.
	//-----------------------------------------------------------------------
	virtual void SetTextColor(COLORREF clrText);

	//-----------------------------------------------------------------------
	// Input:	bBold - If TRUE set font to bold.
	// Summary: Sets/clears bold flag for the item font.
	// Remarks: By using this function you can easily change font to bold or to normal. 
	//-----------------------------------------------------------------------
	virtual void SetBold(BOOL bBold = TRUE);

	//-----------------------------------------------------------------------
	// Input:	clrBackground - New background color.
	// Summary: Sets item text background color.
	//-----------------------------------------------------------------------
	virtual void SetBackgroundColor(COLORREF clrBackground);

	//-----------------------------------------------------------------------
	// Summary: Gets item caption format string.
	// Returns:	String object, containing current format string
	//-----------------------------------------------------------------------
	virtual CString GetFormatString();

	//-----------------------------------------------------------------------
	// Input:   strFormat - New format string.
	// Summary: Sets item caption format string.
	// Remarks: If the format string is set, a caption text will be formatted
	//			accordingly to this format string before drawing it; convenient 
	//          in many cases (drawing date for example). Format string is C - like
	//			style, see sprintf() C function or CString.Format() member function
	// Returns: Old caption format string.
	//-----------------------------------------------------------------------
	virtual CString SetFormatString(CString strFormat);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member function to programatically check if item is
	//			editable.
	// Returns: BOOLEAN value represents current editable flag 
	//-----------------------------------------------------------------------
	virtual BOOL IsEditable();

	//-----------------------------------------------------------------------
	// Input:	bEditable - new editable flag (default is TRUE).
	// Summary: Sets editable flag.
	// Returns: Old item editable state.
	//-----------------------------------------------------------------------
	virtual BOOL SetEditable(BOOL bEditable = TRUE);

	//-----------------------------------------------------------------------
	// Input:	pHyperlink - pointer to hyperlink.	
	// Summary: Add new Hyperlink.
	// Remarks: Each record item can have a number of hyperlinks.
	//			To add new hyperlink to record item you firs create create 
	//			the new instance of hyperlink class.
	// Returns: zero-based id of new Hyperlink.
	// Example: 
	//			AddHyperlink(new CXTPReportHyperlink(27, 7);
	//
	// See Also: CXTPReportHyperlink.
	//-----------------------------------------------------------------------
	int AddHyperlink(CXTPReportHyperlink* pHyperlink);

	//-----------------------------------------------------------------------
	// Summary: Returns a count of hyperlinks in record item
	// Remarks: Each record item can have a number of hyperlinks. To process
	//			all of them, you get the count of hyperlinks that are possesed by an item
	//			using GetHyperlinksCount() member function
	// Returns; count of hyperlinksin iten text
	//-----------------------------------------------------------------------
	int GetHyperlinksCount();

	//-----------------------------------------------------------------------
	// Input:	nHyperlink - zero-based index of hyperlink.
	// Summary: Return Hyperlink by id.
	// Remarks:	To process a hyperlink, you get a pointer to it by
	//			calling GetHyperlinkAt();
	// Returns:	Pointer to Hyperlink.
	// Example: 
	//			int nHyperlinks = GetHyperlinksCount();
	//			CXTPReportHyperlink* pHyperlink;
	//			for(int nHyperlink = 0; nHyperlink < nHyperlinks; nHyperlink++)
	//			{
	//				pHyperlink = GetHyperlinkAt(nHyperlink);
	//				// YOUR PROCESSING HERE
	//				// ...
	//				//
	//			}	
	// See Also: CXTPReportHyperlink.
	//-----------------------------------------------------------------------
	CXTPReportHyperlink* GetHyperlinkAt(int nHyperlink); 

	//-----------------------------------------------------------------------
	// Input:	pDrawArgs    - Draw arguments for calculating item metrics.
	//			pItemMetrics - Pointer to the metrics item to fill with values.
	// Summary: Fills XTP_REPORTRECORDITEM_METRICS structure
	// Remarks: Calculates preview item metrics based on provided draw arguments.
	//			Could be overridden by descendants.
	// See Also: XTP_REPORTRECORDITEM_DRAWARGS, XTP_REPORTRECORDITEM_METRICS	
	//-----------------------------------------------------------------------
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	//-----------------------------------------------------------------------
	// Summary: Determines if this item is a preview item.
	// Returns: TRUE for the preview item, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsPreviewItem();

	//-----------------------------------------------------------------------
	// Summary: Returns item index.
	// Returns: Item numeric index.
	//-----------------------------------------------------------------------
	int GetIndex();

	//-----------------------------------------------------------------------
	// Summary: Returns an ID of the associated resource bitmap.
	// Returns: An ID of the associated resource bitmap.
	//-----------------------------------------------------------------------
	int GetIconIndex();
	
	//-----------------------------------------------------------------------
	// Input:	nIconIndex - ID of the bitmap from the application resources.
	// Summary: Sets new associated bitmap ID.
	// Returns: Returns an ID of old resource bitmap.
	//-----------------------------------------------------------------------
	int SetIconIndex(int nIconIndex);

	//-----------------------------------------------------------------------
	// Summary: Retrieves sort priority of the item.
	// See Also: GetSortPriority, SetSortPriority, SetGroupPriority, GetGroupPriority
	//-----------------------------------------------------------------------
	int GetSortPriority();

	//-----------------------------------------------------------------------
	// Summary: Sets the sort priority of the item.
	// See Also: GetSortPriority, SetSortPriority, SetGroupPriority, GetGroupPriority
	//-----------------------------------------------------------------------
	void SetSortPriority(int nSortPriority);

	//-----------------------------------------------------------------------
	// Summary: Retrieves sort priority of the item.
	// See Also: GetSortPriority, SetSortPriority, SetGroupPriority, GetGroupPriority
	//-----------------------------------------------------------------------
	int GetGroupPriority();

	//-----------------------------------------------------------------------
	// Summary: Sets the group priority of the item.
	// See Also: GetSortPriority, SetSortPriority, SetGroupPriority, GetGroupPriority
	//-----------------------------------------------------------------------
	void SetGroupPriority(int nGroupPriority);
	
	//-----------------------------------------------------------------------
	// Summary: Sets the group caption of the item.
	//-----------------------------------------------------------------------
	void SetGroupCaption(CString strCaption);

	//-----------------------------------------------------------------------
	// Summary: Sets caption of the item.
	//-----------------------------------------------------------------------
	void SetCaption(CString strCaption);

	//-----------------------------------------------------------------------
	// Summary: Call this method to check the item.
	// See Also: IsChecked, HasCheckbox
	//-----------------------------------------------------------------------
	void SetChecked(BOOL bChecked);

	//-----------------------------------------------------------------------
	// Summary: Determinies if the item is checked
	// See Also: SetChecked, HasCheckbox
	//-----------------------------------------------------------------------
	BOOL IsChecked();

	//-----------------------------------------------------------------------
	// Summary: Determines if the item has a checkboxes area.
	// See Also: SetChecked, IsChecked
	//-----------------------------------------------------------------------
	void HasCheckbox(BOOL bHasCheckbox);

	CXTPReportRecord* GetRecord () const;

	DWORD GetItemData();
	void SetItemData(DWORD dwData);


protected:

	//-----------------------------------------------------------------------
	// Input:	pDrawArgs        - Pointer to structure with drawind arguments.
	//			strText          - String of text. 
	//			rcItem           - Item area rectangle coordinates.
	//			nFlag            - Flag can allow next values:
	//								DT_END_ELLIPSIS - truncate text by elipses 
	//								DT_WORDBREAK - - truncate text by word end
	//			nCharCounter     - Reference to printed char counted, assepts value before, returns value after drawing
	//			nHyperlikCounter - Reference to printed hyperlinks counted, assepts value before, returns value after drawing
	//			bSelected        - If Row is selected
	// Summary: Draw a line of text with respect to the drawing rect
	// Remarks: Its member function is internally used by DrawText() and draws one line of text.
	//			It processes plain text/ hyperlink text for one line. For multi-line text
	//			if process last line - the truncate by elipses, otherwise truncate by word
	// Returns: Position in CString where drawing of one line of text was stopped
	//-----------------------------------------------------------------------
	void DrawTextLine(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, CString strText, 
		             CRect rcItem, int nFlag, int& nCharCounter, int& nHyperlikCounter);

	//-----------------------------------------------------------------------
	// Input:	pnCurrDrawPos - Pointer to current drawing position (is updated after each call of function)
	//			pDC           - Point to used Device Context
	//			strText       - String of text 
	//			rcDraw        - Text area rectangle coordinates
	//			nFlag         - Flag can allow next values:
	//								DT_END_ELLIPSIS - truncate text by elipses 
	//								DT_WORDBREAK - - truncate text by word end
	//			bSelected     - If Row is selected
	// Summary: Draw simple string into rectangle
	// Remarks:	Its member function is internally used by DrawTextLine and draws a plain chunk of
	//			text string.
	// Returns: Position in CString where drawing of one string of text line was stopped
	//-----------------------------------------------------------------------
	int DrawString(int* pnCurrDrawPos, CDC* pDC, CString strDraw, CRect rcDraw, UINT nFormat);

	//-----------------------------------------------------------------------
	// Input:	pnCurrDrawPos - pointer to current drawing position (is updated after each call of function)
	//			pDrawArgs     - pointer to structure with drawind arguments
	//			pHyperlink    - pointer to hyperlink object
	//			strText       - string of text 
	//			rcLink        - link hot spot area rectangle coordinates
	//			nFlag         - Flag can allow next values:
	//							DT_END_ELLIPSIS - truncate text by elipses 
	//							DT_WORDBREAK - - truncate text by word end
	//			bSelected     - if Row is selected
	// Summary: Draw hyperlink string using hyperlinks text settings.
	//          
	// Remarks:	Its member function is internally used by DrawTextLine and draws hyperlink
	//			chunk of text string, updates hyperlink's hotspot.
	// Returns: Position in CString where drawing of one link text was stopped
	//-----------------------------------------------------------------------
	int DrawLink(int* pnCurrDrawPos, XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, CXTPReportHyperlink* pHyperlink, CString strText, 
		         CRect rcLink, int nFlag);

	virtual void DrawCheckBox(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, CRect& rcItem);

	// Input:	ptClick - point of the mouse click
	// Summary: Determine if the click occured on the hyperlink  
	// Returns:	zero-based index of clicked link, -1 otherwise
	int HitTestHyperlink(CPoint ptClick);

	BOOL IsFocusable();	
	void SetFocusable(BOOL bFocusable);

protected:
	virtual void OnBeginEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs);
	virtual void OnCancelEdit(CXTPReportControl* pControl, BOOL bApply);

	virtual BOOL OnChar(XTP_REPORTRECORDITEM_ARGS* pItemArgs, UINT nChar);

public:
	virtual void OnValidateEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs);
	virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/, LPCTSTR /*szText*/) {}

	virtual void OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint);
	virtual DWORD GetSelectedConstraintData(XTP_REPORTRECORDITEM_ARGS* /*pItemArgs*/) { return DWORD(-1); }



	virtual void OnInplaceButtonDown(CXTPReportInplaceButton* pButton);

	CXTPReportRecordItemEditOptions* GetEditOptions(CXTPReportColumn* pColumn);
	 
private:
	BOOL IsAllowEdit(XTP_REPORTRECORDITEM_ARGS* pItemArgs);


protected:

	CFont* m_pFontCaption;		// Storage for item caption font.

	COLORREF m_clrText;			// Storage for item text color.
	COLORREF m_clrBackground;	// Storage for item background color.
	BOOL m_bBoldText;			// Is bold text.

	CString m_strFormatString;	// Stores string for extra formatting of item caption.

	BOOL m_bEditable;			// Stores editable flag.
	CArray< CXTPReportHyperlink*, CXTPReportHyperlink* > m_arrHyperlinks; // Array of the Item's hyperlinks

	CXTPReportRecord* m_pRecord;// Pointer to the associated record item, if any, or NULL otherwise.

	int m_nIconIndex;	// ID of the bitmap from the application recources.

	int m_nSortPriority;
	int m_nGroupPriority;

	CString m_strGroupCaption;
	CString m_strCaption;
	BOOL m_bFocusable;

	BOOL m_bChecked;
	BOOL m_bHasCheckbox;
	DWORD m_dwData;

	CXTPReportRecordItemEditOptions* m_pEditOptions;
	

#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportRecordItem);

	afx_msg LPDISPATCH OleGetRecord();
	afx_msg LPDISPATCH OleGetEditOptions();
	afx_msg void OleCreateEditOptions();

#endif
	friend class CXTPReportRecord;
	friend class CXTPReportControl;
};

AFX_INLINE CString CXTPReportRecordItem::GetFormatString() {
	return m_strFormatString;
}

AFX_INLINE CString CXTPReportRecordItem::SetFormatString(CString strFormat) {
	CString sOldString(m_strFormatString);
	m_strFormatString = strFormat;
	return sOldString;
}


AFX_INLINE BOOL CXTPReportRecordItem::SetEditable(BOOL bEditable) {
	BOOL bOldEditable = m_bEditable;
	m_bEditable = bEditable;
	return bOldEditable;
}

AFX_INLINE int CXTPReportRecordItem::AddHyperlink(CXTPReportHyperlink* pHyperlink){
	return (int)m_arrHyperlinks.Add(pHyperlink);
}

AFX_INLINE int CXTPReportRecordItem::GetHyperlinksCount(){
	return (int)m_arrHyperlinks.GetSize();
}

AFX_INLINE CXTPReportHyperlink* CXTPReportRecordItem::GetHyperlinkAt(int nHyperlink){
	return m_arrHyperlinks.GetAt(nHyperlink);
} 

AFX_INLINE BOOL CXTPReportRecordItem::IsPreviewItem() {
	return FALSE;
}
AFX_INLINE int CXTPReportRecordItem::GetIconIndex() {
	return m_nIconIndex;
}
AFX_INLINE int CXTPReportRecordItem::SetIconIndex(int nIconIndex){
	int nOldID = GetIconIndex(); m_nIconIndex = nIconIndex; return nOldID;
}
AFX_INLINE int CXTPReportRecordItem::GetSortPriority() {
	return m_nSortPriority;
}
AFX_INLINE void CXTPReportRecordItem::SetSortPriority(int nSortPriority) {
	m_nSortPriority = nSortPriority;
}
AFX_INLINE int CXTPReportRecordItem::GetGroupPriority() {
	return m_nGroupPriority;
}
AFX_INLINE void CXTPReportRecordItem::SetGroupPriority(int nGroupPriority) {
	m_nGroupPriority = nGroupPriority;
}
AFX_INLINE void CXTPReportRecordItem::SetGroupCaption(CString strCaption) {
	m_strGroupCaption = strCaption;
}
AFX_INLINE void CXTPReportRecordItem::SetCaption(CString strCaption) {
	m_strCaption = strCaption;
}
AFX_INLINE void CXTPReportRecordItem::SetChecked(BOOL bChecked) {
	m_bChecked = bChecked;
}
 
AFX_INLINE BOOL CXTPReportRecordItem::IsChecked() {
	return m_bChecked;
}
AFX_INLINE void CXTPReportRecordItem::HasCheckbox(BOOL bHasCheckbox) {
	m_bHasCheckbox = bHasCheckbox;
}
AFX_INLINE CXTPReportRecord* CXTPReportRecordItem::GetRecord () const {
	return m_pRecord;
}
AFX_INLINE void CXTPReportRecordItem::SetFocusable(BOOL bFocusable) {
	m_bFocusable = bFocusable;
}
AFX_INLINE DWORD CXTPReportRecordItem::GetItemData() {
	return m_dwData;
}
AFX_INLINE void CXTPReportRecordItem::SetItemData(DWORD dwData) {
	m_dwData = dwData;
}

#endif