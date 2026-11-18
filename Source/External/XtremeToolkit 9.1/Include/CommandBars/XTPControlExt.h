// XTPControlExt.h
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
#if !defined(__XTPCONTROLEXT_H__)
#define __XTPCONTROLEXT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore


#include "XTPCommandBarsDefines.h"
#include "XTPControlButton.h"

//===========================================================================
// Remarks: CXTPControlWindowList is a CXTPControlButton derived class. 
//			It is used internally to create window list items.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlWindowList : public CXTPControlButton
{
protected:
	
	//-----------------------------------------------------------------------
	// Summary: This method is called before recalculating the parent command 
	//			bar size.
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);
	
	//-----------------------------------------------------------------------
	// Input:	pCommandBar - Points to a CXTPCommandBar object
	//			point		- Mouse position.
	//			dropEffect	- DROPEFFECT enumerator.
	// Summary: Call this member to check if the user can drop the control.
	// Returns: TRUE if successful, otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);

public:	
	
	static UINT m_nFistMDIChild;                       // First MDI child id.
	static UINT m_nItemCount;
	
private:
	DECLARE_XTP_CONTROL(CXTPControlWindowList)
};

class _XTP_EXT_CLASS CXTPControlWorkspaceActions : public CXTPControlButton
{
protected:

	//-----------------------------------------------------------------------
	// Remarks: This method is called before recalculating the parent command 
	//			bar size.
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);

private:
	DECLARE_XTP_CONTROL(CXTPControlWorkspaceActions)
};

//===========================================================================
// Remarks: CXTPControlToolbars is a CXTPControlButton derived class. 
//			It is used internally to create toolbars list items.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlToolbars : public CXTPControlButton
{
public:
	
	//-----------------------------------------------------------------------
	// Summary: Represents single toolbar item.
	//-----------------------------------------------------------------------
	class CXTPControlToolbar : public CXTPControlButton
	{
		virtual void OnExecute();
	};

protected:

	//-----------------------------------------------------------------------
	// Summary: This method is called before recalculating the parent command 
	//			bar size.
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);
	
	//-----------------------------------------------------------------------
	// Input:	pCommandBar - Points to a CXTPCommandBar object
	//			point       - Mouse position.
	//			dropEffect  - DROPEFFECT enumerator.
	// Summary: Call this member to check if the user can drop the control.
	// Returns: TRUE if successful, otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);

private:
	DECLARE_XTP_CONTROL(CXTPControlToolbars)
};

//===========================================================================
// Remarks: CXTPControlOleItems is a CXTPControlButton derived class. 
//			It is used internally to create ole items.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlOleItems : public CXTPControlButton
{
protected:

	//-----------------------------------------------------------------------
	// Summary: This method is called before recalculating the parent command 
	//			bar size.
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);
	
	//-----------------------------------------------------------------------
	// Input:	pCommandBar - Points to a CXTPCommandBar object
	//			point - Mouse position.
	//			dropEffect - DROPEFFECT enumerator.
	// Summary: Call this member to check if the user can drop the control.
	// Returns: TRUE if successful, otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);

private:
	DECLARE_XTP_CONTROL(CXTPControlOleItems)
};

//===========================================================================
// Remarks: CXTPControlRecentFileList is a CXTPControlButton derived class. 
//			It is used internally to create recent items list.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlRecentFileList : public CXTPControlButton
{
protected:
	
	//-----------------------------------------------------------------------
	// Summary: This method is called before recalculating the parent command 
	//			bar size.
	//-----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);
	
	//-----------------------------------------------------------------------
	// Input:	pCommandBar - Points to a CXTPCommandBar object
	//			point - Mouse position.
	//			dropEffect - DROPEFFECT enumerator.
	// Summary: Call this member to check if the user can drop the control.
	// Returns: TRUE if successful, otherwise returns FALSE
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);

private:
	DECLARE_XTP_CONTROL(CXTPControlRecentFileList)
};


#define XTP_SLN_SELCHANGE    0x1006

class _XTP_EXT_CLASS CXTPControlSelector : public CXTPControl
{
	DECLARE_XTP_CONTROL(CXTPControlSelector);

public:
	CXTPControlSelector();

protected:
	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	// Summary: This method is called to determine the size of the control.
	// Returns: Size of the control.
	//-----------------------------------------------------------------------
	CSize GetSize(CDC* pDC);

	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context.
	// Summary: This method is called to draw the control.
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the mouse cursor moves.
	//-----------------------------------------------------------------------
	void OnMouseMove(CPoint point);

	//-----------------------------------------------------------------------
	// Input:	bSelected - TRUE if the control becomes selected.
	// Summary: This method is called when the control becomes selected.
	// Returns: TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL OnSetSelected(int bSelected);

	//-----------------------------------------------------------------------
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the user releases the left mouse button.
	//-----------------------------------------------------------------------
	void OnLButtonUp(CPoint point);

	//-----------------------------------------------------------------------
	// Summary: This method is called before recalculating the parent command 
	//			bar size.
	//-----------------------------------------------------------------------
	void OnCalcDynamicSize(DWORD dwMode);

	//-----------------------------------------------------------------------
	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	//-----------------------------------------------------------------------
	void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);

	//-----------------------------------------------------------------------
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Reads or writes this object from or to an archive.
	//-----------------------------------------------------------------------
	void Serialize(CArchive& ar);

protected:
	void SetItemsActive(CSize szActive, BOOL bNotify = FALSE);
	virtual void OnDrawItem(CDC* pDC, CRect rcItem, COLORREF clrText);

public:
	CSize m_szItemsCount;
	CSize m_szItemsMax;
	CSize m_szItemsActive;
	CSize m_szItem;
	CSize m_szResult;
	CSize m_szItemsVisible;

protected:
	BOOL m_bLockUpdate;
};

class _XTP_EXT_CLASS CXTPControlLabel : public CXTPControl
{
	DECLARE_XTP_CONTROL(CXTPControlLabel);
public:
	CXTPControlLabel()
	{
		m_dwFlags = xtpFlagSkipFocus|xtpFlagNoMovable;
	}

	int GetSelected() {
		return FALSE;
	}

	BOOL IsLabel() {
		return TRUE;
	}

};


#endif // #if !defined(__XTPCONTROLEXT_H__)