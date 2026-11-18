// XTPImageEditor.h : interface for the CXTPImageEditorDlg class.
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
#if !defined(__XTP_IMAGEEDITORDLG_H__)
#define __XTP_IMAGEEDITORDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//:Ignore

class CXTPImageEditorDlg;

#define XTPCOLOR_ERASE (0xFFFEFF)

enum XTPImageEditorTools
{
	xtpToolPencil, xtpToolFill, xtpToolGetColor, xtpToolLine, xtpToolRectangle, xtpToolCircle
};



/////////////////////////////////////////////////////////////////////////////
// CXTPImageEditorPicker window
class _XTP_EXT_CLASS CXTPImageEditorPicker : public CStatic
{
	DECLARE_DYNAMIC(CXTPImageEditorPicker);
// Construction
public: 

	CXTPImageEditorPicker();

// Attributes
public:	

// Operations
public:

	void SetColor(COLORREF clr);
	COLORREF GetColor();
	void SetSelected(BOOL bSelected = TRUE);

// Overrides
	//{{AFX_VIRTUAL(CXTPImageEditorPicker)
	//}}AFX_VIRTUAL

// Implementation
public:

private:
	COLORREF m_clr;
	BOOL m_bSelected;

	// Generated message map functions
protected:
	
	//{{AFX_MSG(CXTPImageEditorPicker)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	

	DECLARE_MESSAGE_MAP()
};
	

/////////////////////////////////////////////////////////////////////////////
// CXTPImageEditorPicture window
class _XTP_EXT_CLASS CXTPImageEditorPicture : public CStatic
{
public:
	class CAlphaBitmap : public CBitmap
	{
	public:
		CAlphaBitmap(BOOL bAlpha = FALSE)
		{
			m_bAlpha = bAlpha;
		}
	public:
		BOOL m_bAlpha;
		
	};
	
	
// Construction
public:
	CXTPImageEditorPicture();

// Attributes
public:
	CSize m_szPicture;						
	CSize m_szItem;							
	CXTPImageEditorDlg* m_pParentWnd;		

	CDC m_dcPicture;						
	
	CAlphaBitmap* m_pCurrentBitmap;				
	CAlphaBitmap* m_pPictureBitmap;				

	CList<CAlphaBitmap*, CAlphaBitmap*> m_lstUndo;	
	CList<CAlphaBitmap*, CAlphaBitmap*> m_lstRedo;	
	CPoint m_ptTracked;						
	BOOL m_bTracked;						
	COLORREF m_clrDraw;						

	HCURSOR m_hCursorLine;					
	HCURSOR m_hCursorFillColor;				
	HCURSOR m_hCursorPencil;				
	HCURSOR m_hCursorPickColor;				

	void Apply();							

// Operations
public:

	void Init(UINT nID, CXTPImageEditorDlg* pParentWnd);
	XTPImageEditorTools GetSelectedTool();
	COLORREF GetSelectedColor();
	CPoint ClientToPicture(CPoint pt);
	static void CopyBitmap(CAlphaBitmap* pBitmapCopyTo, CAlphaBitmap* pBitmapCopyFrom, CSize sz, CPoint ptOffset = 0, COLORREF clrTransparentFrom = XTPCOLOR_ERASE, COLORREF clrTransparentTo = XTPCOLOR_ERASE, CDC* pDC = 0);
	COLORREF GetPicturePixel(CPoint point);
	void FixAlphaLayer(CAlphaBitmap* pBitmap, CAlphaBitmap* pPictureBitmap);
	

	void Undo();	
	void Redo();
	void Clear();	
	void Copy();	
	void Paste();
	void Load();
	void MovePicture(CPoint ptOffset);
	void OnPictureChanged();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPImageEditorPicture)
	//}}AFX_VIRTUAL

// Implementation
public:
	~CXTPImageEditorPicture();

	// Generated message map functions
protected:

	
	//{{AFX_MSG(CXTPImageEditorPicture)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CXTPImageEditorPreview window
class _XTP_EXT_CLASS CXTPImageEditorPreview : public CStatic
{
// Construction
public:

	CXTPImageEditorPreview();

// Attributes
public:

// Operations
public:

	void Init(UINT nID, CXTPImageEditorDlg* pParentWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPImageEditorPreview)
	//}}AFX_VIRTUAL

// Implementation
public:

protected:
	CXTPImageEditorDlg* m_pParentWnd;	
	CSize m_szPicture;					



	// Generated message map functions
protected:

	
	//{{AFX_MSG(CXTPImageEditorPreview)
	afx_msg void OnPaint();
	//}}AFX_MSG
	

	DECLARE_MESSAGE_MAP()
};


//:End Ignore

/////////////////////////////////////////////////////////////////////////////
// Summary: CXTPImageEditorDlg is a CDialog derived class. It represents the dialog
//          for icons edition.
class _XTP_EXT_CLASS CXTPImageEditorDlg : public CDialog
{
	class CDlgToolBar : public CToolBar
	{
	public:
		CDlgToolBar()
		{
			m_sizeImage = CSize(16, 16);
			m_sizeButton = CSize(23, 23);
		}

		LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM)
		{
			if (IsWindowVisible())
			{
				CFrameWnd* pParent = (CFrameWnd*)GetParent();
				if (pParent)
					OnUpdateCmdUI(pParent, (BOOL)wParam);
			}
			return 0L;
		} 	
		DECLARE_MESSAGE_MAP()
	};

// Construction
public:
	CXTPImageEditorDlg(CWnd* pParent = NULL);	// standard constructor

//:Ignore
	//{{AFX_DATA(CXTPImageEditorDlg)
	enum { IDD = XTP_IDD_IMAGEEDITOR_DIALOG };
	//}}AFX_DATA
//:End Ignore


public:

	// Input:	hIcon - Handle of the icon.
	// Summary: Sets the icon to edit
	void SetIcon(HICON hIcon, BOOL bAlpha = FALSE);
	
	// Input:	szIcon - Icon size.
	// Summary: Sets the icon size to edit.
	void SetIconSize(CSize szIcon);
	
	// Returns: Returns the handle of the icon 
	// Summary: Retrieves the edited icon 
	HICON GetIcon(BOOL* pbAlpha = NULL);

private:
	void OnPictureChanged();
	void OnPickerClick(NMHDR* pNMHDR, LRESULT* pResult);
	void OnSelectorClick(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	void OnEraseClick(NMHDR* pNMHDR, LRESULT* pResult);
	void SetSelected(CXTPImageEditorPicker* pPicker);
	void FixAlphaLayer(CBitmap* pSrc);

// Implementation
private:
	CButton m_gboxTools;
	CButton m_gboxMove;

	CDlgToolBar m_wndToolbarTools;				
	CImageList m_ilToolbarTools;

	CDlgToolBar m_wndToolbarMove;				
	CImageList m_ilToolbarMove;

	
	CXTPImageEditorPicker m_wndSelector[16];	
	CXTPImageEditorPicker m_wndPicker;			
	CXTPImageEditorPicker m_wndSelectorErase;	
	CXTPImageEditorPicture m_wndPicture;		
	CXTPImageEditorPreview m_wndPreview;		
	CXTPImageEditorPicker* m_pSelected;			

	XTPImageEditorTools m_toolSelected;			
	XTPImageEditorTools m_toolPrevious;			
	CSize m_szPicture;			
	


private:

//:Ignore
	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPImageEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();	
	//}}AFX_VIRTUAL
	
	// Generated message map functions
	//{{AFX_MSG(CXTPImageEditorDlg)
	afx_msg void OnUpdateButtonTool(CCmdUI* pCmdUI);
	afx_msg void OnButtonTool(UINT nID);
	afx_msg LRESULT OnKickIdle(WPARAM wp, LPARAM lCount);
	afx_msg void OnUpdateButtonUndo(CCmdUI* pCmdUI);
	afx_msg void OnButtonUndo();
	afx_msg void OnUpdateButtonRedo(CCmdUI* pCmdUI);
	afx_msg void OnButtonRedo();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonCopy();
	afx_msg void OnButtonPaste();
	afx_msg void OnButtonOpen();
	afx_msg void OnMoveButton(UINT nID);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
//:End Ignore

private:
	friend class CXTPImageEditorPicture;
	friend class CXTPImageEditorPreview;
};

#endif // !defined(__XTP_IMAGEEDITORDLG_H__)
