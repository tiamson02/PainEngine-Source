// XTPTabManager.h: interface for the CXTPTabManager class.
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
#if !defined(__XTPTABMANAGER_H__)
#define __XTPTABMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTPTabPaintManager.h"

class CXTPTabManager;
class CXTPTabPaintManager;


class _XTP_EXT_CLASS  CXTPTabManagerItem : public CCmdTarget
{
protected:
	CXTPTabManagerItem();

public:

	void SetCaption(LPCTSTR lpszCaption);
	CString GetCaption();

	void SetTooltip(LPCTSTR lpszTooltip);
	CString GetTooltip() const;

	COLORREF GetColor();
	void SetColor(COLORREF clr);
	
	CRect GetRect();
	int GetLength();

	int GetIndex();
	
	int GetImageIndex();
	void SetImageIndex(int nImage);

	DWORD GetData();
	void SetData(DWORD dwData);

	void SetHandle(HWND hWnd);
	HWND GetHandle();
	
	void SetRect(CRect rcItem);

	BOOL IsVisible();
	void SetVisible(BOOL bVisible);

	BOOL IsEnabled();
	void SetEnabled(BOOL bEnabled);
	
public:
	BOOL IsSelected();
	BOOL IsHighlighted();

	void Remove();
	void Select();

public:
	CXTPTabManager* GetTabManager();


private:
	HWND	m_hWnd;			// A CWnd pointer to the window associated with a tab.
	CString m_strCaption;		// User specified label for the tab.
	CString m_strToolTip;		// Tooltip text for the tab.	
	
	HICON m_hIcon;			// Icon associated with the tab.
	int m_nImage;

	CRect m_rcItem;			// Rectangle of this tab.
	int m_nLength;
	int m_nIndex;
	DWORD m_dwData;	
	COLORREF m_clrItem;
	BOOL m_bFound;

	BOOL m_bEnabled;
	BOOL m_bVisible;


	CXTPTabManager* m_pTabManager;

	friend class CXTPTabManager;
	friend class CXTPTabPaintManager;
	friend class CXTPTabWorkspace;
	friend class CXTPTabClientWnd;
	friend class CXTPGroupClientWnd;


#ifdef _XTP_ACTIVEX
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CTabControlItem)
	afx_msg BSTR OleGetCaption();
	afx_msg void OleSetCaption(LPCTSTR lpszNewValue);
	afx_msg long OleGetIndex();
	afx_msg long OleGetTag();
	afx_msg void OleSetTag(long nNewValue);
	afx_msg BOOL OleGetSelected();
	afx_msg void OleSetSelected(BOOL bNewValue);
	afx_msg long OleGetImage();
	afx_msg void OleSetImage(long nNewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPTabManagerItem)
#endif
};


class _XTP_EXT_CLASS CXTPTabManagerNavigateButton
{

public:
	CXTPTabManagerNavigateButton(CXTPTabManager* pManager, int nID, XTPTabNavigateButtonFlags dwFlags);
	
	void AdjustWidth(int& nWidth);
	void SetFlags(XTPTabNavigateButtonFlags dwFlags);
	XTPTabNavigateButtonFlags GetFlags();

	void Draw(CDC* pDC);	
	void PerformMouseMove(HWND hWnd, CPoint pt);
	void PerformClick(HWND hWnd, CPoint pt);

	BOOL IsEnabled();
	BOOL IsPressed();
	BOOL IsHighlighted();
	BOOL IsVisible();
	CRect GetRect();

	virtual void Reposition(CRect& rcNavigateButtons);

protected:
	virtual void DrawEntry(CDC* pDC, CRect rc) = 0;
	virtual void OnExecute(BOOL bTick);

protected:
	int m_nID;
	XTPTabNavigateButtonFlags m_dwFlags;
	CSize m_szButton;
	CXTPTabManager* m_pManager;
	CRect m_rcButton;

	BOOL m_bEnabled;
	BOOL m_bPressed;
	BOOL m_bHighlighted;

private:
	friend class CXTPTabManager;	
};

class CXTPTabManagerAtom
{
public:
	virtual void OnPropertyChanged()
	{
		Reposition();
	}
protected:
	virtual ~CXTPTabManagerAtom()
	{
		
	}	

	virtual void Reposition() = 0;
};

class _XTP_EXT_CLASS CXTPTabManager : public CXTPTabManagerAtom
{
protected:
	class CNavigateButtonArrow;
	class CNavigateButtonArrowLeft;	
	class CNavigateButtonArrowRight;	
	class CNavigateButtonClose;

public:
	CXTPTabManager();
	virtual ~CXTPTabManager();

public:
	// Inputs:  Zero-based index of the tab. 
	// Returns: Pointer to CXTPTabManagerItem if successful; NULL otherwise.
	// Summary: Retrieves information about a tab in a tab control
	CXTPTabManagerItem* GetItem(int nIndex) const;

	// Inputs:  nItem - Zero-based value of the item to delete. 
	// Returns: Nonzero if successful; otherwise 0.
	// Summary: Removes the specified item from a tab control.
	BOOL DeleteItem(int nItem);

	// Summary: Removes all items from a tab control.
	void DeleteAllItems();

	// Returns: Number of items in the tab control.
	// Summary: Retrieves the number of tabs in the tab control.
	int GetItemCount() const;


public:
	virtual CString GetItemCaption(CXTPTabManagerItem* pItem);
	virtual HICON GetItemIcon(CXTPTabManagerItem* pItem);
	virtual COLORREF GetItemColor(CXTPTabManagerItem* pItem);
	virtual CString GetItemTooltip(CXTPTabManagerItem* pItem) const;

	virtual void OnItemsChanged();

public:
	void SetCurSel(int nIndex);
	int GetCurSel();

	virtual void SetSelectedItem(CXTPTabManagerItem* pItem);

	CXTPTabManagerItem* GetSelectedItem();

	CXTPTabManagerItem* HitTest(CPoint point) const;

	virtual void GetItemMetrics(CSize* lpszNormal, CSize* lpszMin = NULL, CSize* lpszMax = NULL);
	void SetItemMetrics(CSize szNormal, CSize szMin = CSize(0, 0), CSize szMax = CSize(0, 0));


public:
	int GetItemsLength();
	int GetRectLength(CRect rc);
	CXTPTabManagerNavigateButton* FindNavigateButton(int nID);
	BOOL IsHorizontalPosition();
	int GetHeaderOffset();
	void SetHeaderOffset(int nOffset);

	void EnsureVisible(CXTPTabManagerItem* pItem);


public:
	virtual CXTPTabPaintManager* GetPaintManager() = 0;
	virtual void SetPaintManager(CXTPTabPaintManager* pPaintManager);

	virtual void RedrawControl() = 0;
	virtual BOOL IsAllowReorder();
	virtual void SetAllowReorder(BOOL bAllowReorder);	
	
	virtual BOOL IsDrawStaticFrame();

	virtual BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) = 0;


public:
	CXTPTabPaintManager::CColorSet* SetColor(XTPTabColorStyle tabColor);
	CXTPTabPaintManager::CColorSet* SetColorSet(CXTPTabPaintManager::CColorSet* pColorSet);

	CXTPTabPaintManager::CAppearanceSet* SetAppearance(XTPTabAppearanceStyle tabAppearance);
	CXTPTabPaintManager::CAppearanceSet* SetAppearanceSet(CXTPTabPaintManager::CAppearanceSet* pAppearanceSet);

	CXTPTabPaintManager::CAppearanceSet* GetAppearanceSet();
	CXTPTabPaintManager::CColorSet* GetColorSet();

	XTPTabAppearanceStyle GetAppearance();
	XTPTabColorStyle GetColor();

	void SetPosition(XTPTabPosition tabPosition);
	void SetLayoutStyle(XTPTabLayoutStyle tabLayout);

	virtual XTPTabPosition GetPosition();
	XTPTabLayoutStyle GetLayout();

	void ShowIcons(BOOL bShowIcons);	

	void SetActive(BOOL bActive);
	BOOL IsActive();

	CRect GetControlRect();
	CRect GetClientRect();
	CRect GetHeaderRect();


protected:
	virtual void OnItemClick(CXTPTabManagerItem* pItem);
	virtual void ReOrder(HWND hWnd, CPoint pt, CXTPTabManagerItem* pItem);
	virtual void OnNavigateButtonClick(UINT nID);
	virtual BOOL IsMouseLocked();

	CXTPTabManagerItem* AddItem(int nItem, CXTPTabManagerItem* pItem = NULL);
	void PerformMouseMove(HWND hwnd, CPoint pt);
	BOOL PerformClick(HWND hWnd, CPoint pt, BOOL bNoItemClick = FALSE);
	int PerformToolHitTest(HWND hWnd, CXTPTabManagerItem* pItem, TOOLINFO* pTI) const;

	int GetNavigateButtonCount();
	CXTPTabManagerNavigateButton* GetNavigateButton(int nIndex);


protected:
	CXTPTabManagerItem* m_pSelected;
	CXTPTabManagerItem* m_pHighlighted;
	
	int m_nHeaderOffset;
	BOOL m_bAllowReorder;
	BOOL m_bActive;

	CSize m_szItemNormal;
	CSize m_szItemMin;
	CSize m_szItemMax;


	CRect m_rcHeaderRect;
	CRect m_rcControl;
	CRect m_rcClient;

	CArray<CXTPTabManagerItem*, CXTPTabManagerItem*> m_arrItems;
	CArray<CXTPTabManagerNavigateButton*, CXTPTabManagerNavigateButton*> m_arrNavigateButtons;

private:
	int m_nItemColor;

	friend class CXTPTabManagerItem;
	friend class CXTPTabPaintManager;
	friend class CXTPTabManagerNavigateButton;
	friend class CXTPTabClientWnd;


};


//////////////////////////////////////////////////////////////////////////
// CXTPTabManagerItem

AFX_INLINE CXTPTabManager* CXTPTabManagerItem::GetTabManager() {
	return m_pTabManager;
}
AFX_INLINE BOOL CXTPTabManagerItem::IsSelected() {
	return m_pTabManager->m_pSelected == this;
}
AFX_INLINE BOOL CXTPTabManagerItem::IsHighlighted() {
	return m_pTabManager->m_pHighlighted == this;
}
AFX_INLINE CRect CXTPTabManagerItem::GetRect() {
	return m_rcItem;
}
AFX_INLINE void CXTPTabManagerItem::SetRect(CRect rcItem) {
	m_rcItem = rcItem;
}
AFX_INLINE int CXTPTabManagerItem::GetIndex() {
	return m_nIndex;
}
AFX_INLINE DWORD CXTPTabManagerItem::GetData() {
	return m_dwData;
}
AFX_INLINE void CXTPTabManagerItem::SetData(DWORD dwData) {
	m_dwData = dwData;
}
AFX_INLINE int CXTPTabManagerItem::GetImageIndex() {
	return m_nImage;
}
AFX_INLINE void CXTPTabManagerItem::SetImageIndex(int nImage) {
	m_nImage = nImage;
	m_pTabManager->Reposition();
}
AFX_INLINE int CXTPTabManagerItem::GetLength() {
	return m_nLength;
}
AFX_INLINE void CXTPTabManagerItem::SetTooltip(LPCTSTR lpszTooltip) {
	m_strToolTip = lpszTooltip;
}

//////////////////////////////////////////////////////////////////////////
// CXTPTabManagerNavigateButton

AFX_INLINE void CXTPTabManagerNavigateButton::SetFlags(XTPTabNavigateButtonFlags dwFlags) {
	m_dwFlags = dwFlags;
}
AFX_INLINE XTPTabNavigateButtonFlags CXTPTabManagerNavigateButton::GetFlags() {
	return m_dwFlags;
}
AFX_INLINE BOOL CXTPTabManagerNavigateButton::IsEnabled() {
	return m_bEnabled;
}
AFX_INLINE BOOL CXTPTabManagerNavigateButton::IsPressed() {
	return m_bPressed;
}
AFX_INLINE BOOL CXTPTabManagerNavigateButton::IsHighlighted() {
	return m_bHighlighted;
}
AFX_INLINE CRect CXTPTabManagerNavigateButton::GetRect() {
	return m_rcButton;
}
AFX_INLINE BOOL CXTPTabManagerNavigateButton::IsVisible() {
	return !m_rcButton.IsRectEmpty();
}

//////////////////////////////////////////////////////////////////////////
// CXTPTabManager

AFX_INLINE CXTPTabPaintManager::CAppearanceSet* CXTPTabManager::GetAppearanceSet() {
	return GetPaintManager()->GetAppearanceSet();
}
AFX_INLINE CXTPTabPaintManager::CColorSet* CXTPTabManager::GetColorSet() {
	return GetPaintManager()->GetColorSet();
}
AFX_INLINE XTPTabAppearanceStyle CXTPTabManager::GetAppearance() {
	return GetPaintManager()->GetAppearance();
}
AFX_INLINE XTPTabColorStyle CXTPTabManager::GetColor() {
	return GetPaintManager()->GetColor();
}
AFX_INLINE CXTPTabManagerItem* CXTPTabManager::GetItem(int nIndex) const{
	return nIndex >= 0 && nIndex < GetItemCount()? m_arrItems[nIndex]: NULL;
}
AFX_INLINE int CXTPTabManager::GetItemCount() const {
	return (int)m_arrItems.GetSize();
}
AFX_INLINE int CXTPTabManager::GetNavigateButtonCount() {
	return (int)m_arrNavigateButtons.GetSize();
}
AFX_INLINE CXTPTabManagerNavigateButton* CXTPTabManager::GetNavigateButton(int nIndex) {
	return nIndex >= 0 && nIndex < GetNavigateButtonCount()? m_arrNavigateButtons[nIndex]: NULL;
}
AFX_INLINE CXTPTabManagerItem* CXTPTabManager::GetSelectedItem() {
	return m_pSelected;
}
AFX_INLINE BOOL CXTPTabManager::IsHorizontalPosition()
{
	return (GetPosition() == xtpTabPositionBottom || GetPosition() == xtpTabPositionTop);
}
AFX_INLINE int CXTPTabManager::GetRectLength(CRect rc)
{
	if (IsHorizontalPosition()) return rc.Width();
	return rc.Height();
}
AFX_INLINE int CXTPTabManager::GetHeaderOffset()
{
	return m_nHeaderOffset;
}
AFX_INLINE void CXTPTabManager::OnNavigateButtonClick(UINT /*nID*/) {

}
AFX_INLINE BOOL CXTPTabManager::IsAllowReorder() {
	return m_bAllowReorder;
}
AFX_INLINE void CXTPTabManager::SetAllowReorder(BOOL bAllowReorder) {
	m_bAllowReorder = bAllowReorder;
}
AFX_INLINE void CXTPTabManager::SetPaintManager(CXTPTabPaintManager* /*pPaintManager*/) {

}
AFX_INLINE BOOL CXTPTabManager::IsActive() {
	return m_bActive;
}
AFX_INLINE CRect CXTPTabManager::GetControlRect() {
	return m_rcControl;
}
AFX_INLINE CRect CXTPTabManager::GetClientRect() {
	return m_rcClient;
}
AFX_INLINE CRect CXTPTabManager::GetHeaderRect() {
	return m_rcHeaderRect;
}
AFX_INLINE BOOL CXTPTabManager::IsMouseLocked() {
	return FALSE;
}

#endif // !defined(__XTPTABMANAGER_H__)
