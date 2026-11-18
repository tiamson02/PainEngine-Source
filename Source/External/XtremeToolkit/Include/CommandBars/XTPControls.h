// XTPControls.h : interface for the CXTPControls class.
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
#if !defined(__XTPCONTROLS_H__)
#define __XTPCONTROLS_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"

class CXTPCommandBar;
class CXTPControl;
class CXTPCommandBars;
class CXTPCommandBarList;

//===========================================================================
// Remarks: CXTPControls is a CCmdTarget derived class. It represents a collection
//			of the controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPControls : public CCmdTarget
{
	DECLARE_DYNCREATE(CXTPControls)
	//-----------------------------------------------------------------------
	// Summary: Protected constructor used by dynamic creation
	//-----------------------------------------------------------------------
	CXTPControls();           

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPControls object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPControls();

	struct XTPBUTTONINFO;		// Internal helper structure.

public:
	//-----------------------------------------------------------------------
	// Input:	nIndex - An integer index.
	// Returns: The CXTPControl pointer currently at this index.
	// Summary: Call this member to return the control at the specified index.
	//-----------------------------------------------------------------------
	CXTPControl* GetAt(int nIndex);

	
	//-----------------------------------------------------------------------
	// Input:	controlType - The type of the control to be added.
	//			nId - Identifier of the control to be added.
	//			strParameter - Parameter of the control to be added.
	//			nBefore - Index of the control to be inserted.
	//			bTemporary - TRUE if this control is temporary.
	// Returns: A pointer to the added control.
	// Summary: Call this member to add a new control.
	//-----------------------------------------------------------------------
	CXTPControl* Add(XTPControlType controlType, int nId, CString strParameter = _T(""), int nBefore = -1, BOOL bTemporary = FALSE);
	
	//-----------------------------------------------------------------------
	// Input:	pControl - A pointer to the control to be added.
	//			nId - Identifier of the control to be added.
	//			strParameter - Parameter of the control to be added.
	//			nBefore - Index of the control to be inserted.
	//			bTemporary - TRUE if this control is temporary.
	// Summary: Call this member to add a new control.
	// Returns: A pointer to the added control.
	//-----------------------------------------------------------------------
	CXTPControl* Add(CXTPControl* pControl, int nId, CString strParameter = _T(""), int nBefore = -1, BOOL bTemporary = FALSE);

	//-----------------------------------------------------------------------
	// Input:	pMenu - Menu that contains the control.
	//			nIndex - Index of the control to be added.
	// Summary: Call this member to add a new control.
	// Returns: Pointer to the added control.
	//-----------------------------------------------------------------------
	CXTPControl* AddMenuItem(CMenu* pMenu, int nIndex);

	//-----------------------------------------------------------------------
	// Input:	pControl - Control to be removed.
	// Summary: Call this member to remove a control.
	//-----------------------------------------------------------------------
	void Remove(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Input:	nIndex - Index of the control to be removed.
	// Summary: Call this member to remove a control.
	//-----------------------------------------------------------------------
	void Remove(int nIndex);

	//-----------------------------------------------------------------------
	// Summary: Call this member to remove all controls.
	//-----------------------------------------------------------------------
	void RemoveAll(void);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the count of the controls
	// Returns: The count of the controls.
	//-----------------------------------------------------------------------
	int GetCount();

	//-----------------------------------------------------------------------
	// Input:	nId - Identifier of the control to find.
	// Summary: Call this member to find a control by its identifier.
	// Returns: A pointer to the CXTPControl object if successful; otherwise returns NULL.
	//-----------------------------------------------------------------------
	CXTPControl* FindControl(long nId);

	//-----------------------------------------------------------------------
	// Input:	type - The type of the control to find.
	//			nId - The control's identifier.
	//			bVisible - TRUE if the control is visible.
	//			bRecursive - TRUE to find in the nested command bars.
	// Summary: Call this member to find the specified control.
	// Returns: Pointer to the CXTPControl object if successful; otherwise returns NULL.
	//-----------------------------------------------------------------------
	CXTPControl* FindControl(XTPControlType type, long nId, BOOL bVisible, BOOL bRecursive);

public:

	
	//-----------------------------------------------------------------------
	// Summary:	Call this member to retrieve the first control.
	// Returns: A pointer to a CXTPControl object
	//-----------------------------------------------------------------------
	CXTPControl* GetFirst();

	//-----------------------------------------------------------------------
	// Input:	pControl - Current control.
	// Summary: Call this member to retrieve the next control.
	//-----------------------------------------------------------------------
	void GetNext(CXTPControl*& pControl);
	
	//-----------------------------------------------------------------------
	// Input:	point - Specifies the point to be tested. 
	// Summary: This method determines where a point lies in a specified control.
	// Returns: A pointer to a CXTPControl control that occupies the specified 
	//			point or NULL if no control occupies the point.
	//-----------------------------------------------------------------------
	CXTPControl* HitTest(CPoint point);
	
	//-----------------------------------------------------------------------
	// Input:	pControl - Points to a CXTPControl object
	//			type - Type needed to be changed.
	// Summary: Call this member to change the type of the control.
	// Returns: A pointer to a CXTPControl object
	//-----------------------------------------------------------------------
	CXTPControl* SetControlType(CXTPControl* pControl, XTPControlType type);
	
	//-----------------------------------------------------------------------
	// Input:	nIndex - Index of the control
	//			type - Type needed to be changed.
	// Summary: Call this member to change the type of the control.
	// Returns: A pointer to a CXTPControl object
	//-----------------------------------------------------------------------
	CXTPControl* SetControlType(int nIndex, XTPControlType type);
	
	//-----------------------------------------------------------------------
	// Input:	pClone - Points to a CXTPControl object needed to copy.
	//			nBefore - Index of the control to be inserted.
	//			bRecursive - TRUE to copy recursively.
	// Summary:	Call this member to add the copy of the control.
	// Returns: A pointer to an added CXTPControl object
	//-----------------------------------------------------------------------
	CXTPControl* AddClone(CXTPControl* pClone, int nBefore = -1, BOOL bRecursive = FALSE);
	
	//-----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	//			nLength - Length of the parent bar.
	//			dwMode - Mode of the parent bar.
	//			rcBorder - Border to be set.
	//			nWidth - Width to be set.
	// Summary: This method is called internally to reposition controls and 
	//			determine their size.
	// Returns: Total size of the controls.
	//-----------------------------------------------------------------------
	CSize CalcDynamicSize(CDC* pDC, int nLength, DWORD dwMode, const CRect& rcBorder, int nWidth = 0);	
	
	//-----------------------------------------------------------------------
	// Input:	bIgnoreWrap - TRUE to ignore controls that are wrapped.
	// Summary: Call this member to get the number of visible controls.
	// Returns: Number of visible controls.
	//-----------------------------------------------------------------------
	int GetVisibleCount(BOOL bIgnoreWrap = FALSE);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to attach the specified command bars object.
	//-----------------------------------------------------------------------
	void SetCommandBars(CXTPCommandBars*);
	
	//-----------------------------------------------------------------------
	// Input:	nIndex - Current control index.
	//			nDirection - Direction to find.
	//			bKeyboard - TRUE to skip controls with xtpFlagSkipFocus flag.
	// Summary: Finds the next control in the given direction.
	// Returns: Index of the next control.
	//-----------------------------------------------------------------------
	long GetNext(long nIndex, int nDirection = +1, BOOL bKeyboard = TRUE, BOOL bSkipTemporary = FALSE, BOOL bSkipCollapsed = TRUE);
	
	//-----------------------------------------------------------------------
	// Input:	bRecursive - TRUE to copy recursively.
	// Summary: Makes a copy of all controls.
	// Returns: A pointer to a CXTPControls object.
	//-----------------------------------------------------------------------
	CXTPControls* Duplicate(BOOL bRecursive = FALSE);
	
	//-----------------------------------------------------------------------
	// Summary: Retrieves the parent command bars object.
	// Returns: A pointer to a CXTPCommandBars object
	//-----------------------------------------------------------------------
	CXTPCommandBars* GetCommandBars();
	
	//-----------------------------------------------------------------------
	// Input:	pControl - Points to a CXTPControl object
	//			nBefore - Index to be moved.
	// Summary: Move the control to a specified position.
	//-----------------------------------------------------------------------
	void MoveBefore(CXTPControl* pControl, int nBefore);
	
	//-----------------------------------------------------------------------
	// Input:	pParent - Points to a CXTPCommandBar object
	// Summary: Attaches the parent command bar.
	//-----------------------------------------------------------------------
	void SetParent(CXTPCommandBar* pParent);

	//-----------------------------------------------------------------------
	// Summary:	Creates original controls set.
	//-----------------------------------------------------------------------
	void CreateOriginalControls();
	
	//-----------------------------------------------------------------------
	// Summary: Removes the original controls set.
	//-----------------------------------------------------------------------
	void ClearOriginalControls();
	
	//-----------------------------------------------------------------------
	// Summary: Retrieves original controls set.
	// Returns: A pointer to a CXTPControls object
	//-----------------------------------------------------------------------
	CXTPControls* GetOriginalControls();

	//-----------------------------------------------------------------------
	// Input:	pControls - Points to a CXTPControls object
	// Summary: Call this member to compare the controls.
	// Returns: TRUE if the controls are identical.
	//-----------------------------------------------------------------------
	BOOL Compare(CXTPControls* pControls);

	//-----------------------------------------------------------------------
	// Summary: Returns the changed status flag of the control.
	// Returns: TRUE if the internal control state was changed since last drawing, 
	//			FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsChanged();

protected:
	
	//-----------------------------------------------------------------------
	// Input:	archive - A CArchive object to serialize to or from. 
	//          nSchema - Schema to be used.
	// Summary: Reads or writes this object from or to an archive.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar, int nSchema);

private:
	void GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList);
	void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList);
	void RefreshIndexes();
	void AddTail(CXTPControl* pControl);
	CSize _CalcSize(XTPBUTTONINFO* pData, BOOL bCommit = FALSE, BOOL bVert = FALSE);
	int _WrapToolBar(XTPBUTTONINFO* pData, int nWidth, DWORD& dwMode);
	void _SizeFloatableBar(XTPBUTTONINFO* pData, int nLength, DWORD dwMode);
	void _AdjustBorders(XTPBUTTONINFO* pData, CSize& sizeResult, DWORD dwMode, CRect rcBorder);
	void _CenterControlsInRow(XTPBUTTONINFO* pData, int nFirst, int nLast, int, BOOL, CSize sizeResult, CRect );
	void _MoveRightAlligned(XTPBUTTONINFO* pData, CSize sizeResult, CRect rcBorder, DWORD dwMode);

private:
	CXTPCommandBar* m_pParent;
	CArray<CXTPControl*, CXTPControl*> m_arrControls;
	CXTPCommandBars* m_pCommandBars;

	CXTPControls* m_pOriginalControls;

public:

//:Ignore	
#ifdef _XTP_ACTIVEX

	CArray<CXTPControl*, CXTPControl*>* CXTPControls::GetArray(){
		return &m_arrControls;
	}


	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPControls);

	afx_msg long OleGetCount();
	afx_msg LPDISPATCH OleItem(long nIndex);
	afx_msg LPDISPATCH OleAdd(long controlType, long nId, LPCTSTR strCaption, const VARIANT& pBefore, const VARIANT& pTemporary);

	afx_msg LPUNKNOWN OleNewEnum();	
	afx_msg LPDISPATCH OleParent();
	LPDISPATCH OleFindControl(const VARIANT& varType, long nId, const VARIANT& varVisible, const VARIANT& varRecursive);

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
		dispidParent = 3L,
		dispidFindControl = 4L,
	};
	
#endif

	friend class CXTPCommandBar;
	friend class CXTPCommandBars;
	friend class CXTPToolBar;
	friend class CXTPMenuBar;
//:End Ignore
	
};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPControl* CXTPControls::GetAt(int nIndex) { 
	ASSERT(nIndex < m_arrControls.GetSize());
	return (nIndex >= 0 && nIndex < m_arrControls.GetSize())? m_arrControls.GetAt(nIndex): NULL; 
}
AFX_INLINE int CXTPControls::GetCount() {
	return (int)m_arrControls.GetSize();
}
AFX_INLINE CXTPControls* CXTPControls::GetOriginalControls() {
	return m_pOriginalControls;
}

#endif // #if !defined(__XTPCONTROLS_H__)
