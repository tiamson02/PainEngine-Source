// XTPPropertyGridInplaceButton.h interface for the CXTPPropertyGridInplaceButton class.
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
#if !defined(__XTPPROPERTYGRIDINPLACEBUTTON_H__)
#define __XTPPROPERTYGRIDINPLACEBUTTON_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

// class forwards

class CXTPPropertyGridView;
class CXTPPropertyGridItem;

//===========================================================================
// Remarks: CXTPPropertyGridInplaceButton is a CStatic derived class. 
//			It is an internal class used by Property Grid control.
//===========================================================================
class CXTPPropertyGridInplaceButton : public CStatic
{
	DECLARE_DYNAMIC(CXTPPropertyGridInplaceButton)

public:
	
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPPropertyGridInplaceButton object
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButton();
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPPropertyGridInplaceButton object, handles cleanup 
	//			and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridInplaceButton();

	//-----------------------------------------------------------------------
	// Summary: Call this method to hide the inplace button control.
	//-----------------------------------------------------------------------
	virtual void HideWindow();
	
	//-----------------------------------------------------------------------
	// Input:	pItem - Points to a CXTPPropertyGridItem object
	//			rect  -  Specifies the size and position of the item. 
	// Summary: Call this method to create the inplace button control.
	//-----------------------------------------------------------------------
	virtual void Create(CXTPPropertyGridItem* pItem, CRect rect);
	
	//-----------------------------------------------------------------------
	// Summary: This method is called when the parent item is destroyed.
	//-----------------------------------------------------------------------
	virtual void DestroyItem();
	
	//-----------------------------------------------------------------------
	// Summary: This member function retrieves the child property grid item
	// Returns: A pointer to a CXTPPropertyGridItem object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetItem();


public:
    BOOL                 m_bPressed;	// TRUE if the button is pressed.
    BOOL                 m_bOver;		// TRUE if the mouse cursor is over the button.

private:
	
    CWnd*                m_pGrid;
    CXTPPropertyGridItem* m_pItem;

protected:

//:Ignore
    //{{AFX_VIRTUAL(CXTPPropertyGridInplaceButton)
    //}}AFX_VIRTUAL

    //{{AFX_MSG(CXTPPropertyGridInplaceButton)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CXTPPropertyGridItem;	
//:End Ignore
};


AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGridInplaceButton::GetItem() {
	return m_pItem;
}

#endif //#if !defined(__XTPPROPERTYGRIDINPLACEBUTTON_H__)
