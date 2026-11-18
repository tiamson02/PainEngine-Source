// XTPControlListBox.h : interface for the CXTPControlListBox class.
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
#if !defined(__XTPCONTROLLISTBOX_H__)
#define __XTPCONTROLLISTBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#define XTP_LBN_SELCHANGE    0x1003		
#define XTP_LBN_POPUP    0x1004

//===========================================================================
// Remarks: CXTPControlComboBox is a CXTPControl derived class. 
//			It represents a list box control.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlListBox : public CXTPControl
{
	
	//-----------------------------------------------------------------------
	// Summary: ListBox control.
	//-----------------------------------------------------------------------
	class CListBoxExt : public CListBox
	{
		DECLARE_MESSAGE_MAP()
		friend class CXTPControlListBox;
	public:
		CXTPControlListBox* m_pControl;
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		void OnSelChanged();	
	};

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPControlListBox object
	//-----------------------------------------------------------------------
	CXTPControlListBox();
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPControlListBox object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPControlListBox();

	//-----------------------------------------------------------------------
	// Input:	nWidth - The width of the control
	// Summary: Call this member to set the width of the control
	//-----------------------------------------------------------------------
	void SetWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Input:	nMin - Minimum lines to be used.
	//          nMax - Maximum lines to be used
	// Summary: Call this member to set the total minimum and maximum lines.
	//-----------------------------------------------------------------------
	void SetLinesMinMax(int nMin, int nMax);

	//-----------------------------------------------------------------------
	// Summary: Retrieves list control.
	//-----------------------------------------------------------------------
	CListBox* GetListCtrl ();

	//-----------------------------------------------------------------------
	// Input:   bMultiplSel - TRUE to set multiple selection style.
	// Summary: Call this member to set the multiple selection style of the 
	//			control.
	//-----------------------------------------------------------------------
	void SetMultiplSel(BOOL bMultiplSel);


protected:
	//-----------------------------------------------------------------------
	// Input:	rcControl - Bounding rectangle of the control.
	// Summary: Call this member to set the bounding rectangle of the control.
	//-----------------------------------------------------------------------
	virtual void SetRect(CRect rcControl);

	//-----------------------------------------------------------------------
	// Input:	 bEnabled - TRUE if the control is enabled.  
	// Summary:  Call this member to enable or disable the control.
	//			 If the control does not have flags xtpFlagManualUpdate, you must call
	//			 the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// See Also: GetEnabled, SetChecked
	//-----------------------------------------------------------------------
	virtual void SetEnabled(BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	// Summary: This method is called to determine the size of the control.
	// Returns: Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize GetSize(CDC* pDC);

	//-----------------------------------------------------------------------
	// Input:	pParent - Points to a CXTPCommandBar object
	// Summary: This method is called to assign a parent command bar object.
	//-----------------------------------------------------------------------
	virtual void SetParent(CXTPCommandBar* pParent);

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context.
	// Summary: This method is called to draw the control.
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Input:	nChar - Specifies the virtual key code of the given key.
	// Summary:	The framework calls this member function when a non-system key 
	//			is pressed.
	// Returns:	TRUE if key handled, otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL OnKeyDown(UINT nChar);

	//-----------------------------------------------------------------------
	// Input:	bSelected - TRUE if the control becomes selected.
	// Summary: This method is called when the control becomes selected.
	// Returns: TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL OnSetSelected(int bSelected);

	//-----------------------------------------------------------------------
	// Input:	bKeyboard - TRUE if the control is selected using the keyboard.
	//          pt - Mouse cursor position.
	// Summary: This method is called when the user clicks the control.
	//-----------------------------------------------------------------------
	virtual void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));

	//-----------------------------------------------------------------------
	// Summary: This method is called before recalculating the parent command 
	//			bar size.
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD /*dwMode*/);

	//-----------------------------------------------------------------------
	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);

	//-----------------------------------------------------------------------
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to read or write this object from or to an archive.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);


protected:
	CListBoxExt* m_pListBox;					// List box control.
	int m_nListBoxLinesMin;						// Minimum lines.
	int m_nListBoxLinesMax;						// Maximum lines.
	BOOL m_bMultiplSel;							// Multiple selection style.

	int m_nWidth;								// Width of the control.
	BOOL m_bMouseLocked;						// TRUE to lock mouse events.


private:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


	DECLARE_XTP_CONTROL(CXTPControlListBox)

	friend class CListBoxExt;
};

//===========================================================================
// Remarks: CXTPControlStatic is a CXTPControl derived class. 
//			It represents a static text control.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlStatic : public CXTPControl
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPControlStatic object
	//-----------------------------------------------------------------------
	CXTPControlStatic();

	//-----------------------------------------------------------------------
	// Input:	nWidth - The width of the control
	// Summary: Call this member to set the width of the control
	//-----------------------------------------------------------------------
	void SetWidth(int nWidth);

protected:
	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	// Summary: This method is called to determine the size of the control.	
	// Returns: Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize GetSize(CDC* pDC);

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context.
	// Summary: This method is called to draw the control.
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);

	//-----------------------------------------------------------------------
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to read or write this object from or to an archive.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);

private:
	int m_nWidth;

private:
	DECLARE_XTP_CONTROL(CXTPControlStatic)
	
};


#endif // !defined(__XTPCONTROLLISTBOX_H__)
