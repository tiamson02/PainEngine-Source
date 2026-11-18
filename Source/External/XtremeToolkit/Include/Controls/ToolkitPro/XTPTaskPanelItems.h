// XTPTaskPanelItems.h interface for the CXTPTaskPanelItems class.
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
#if !defined(__XTPTASKPANELITEMS_H__)
#define __XTPTASKPANELITEMS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPTaskPanelGroup;
class CXTPTaskPanelItem;
class CXTPTaskPanel;

//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanelItems is a CCmdTarget derived class. It is used as collection of 
//			CXTPTaskPanelItem classes.
class _XTP_EXT_CLASS CXTPTaskPanelItems : public CCmdTarget
{
	DECLARE_DYNCREATE(CXTPTaskPanelItems)

	// protected constructor used by dynamic creation
	CXTPTaskPanelItems(CXTPTaskPanel* pPanel = NULL);           

// Attributes
public:

// Operations
public:

	// Input:	pItem - Points to a CXTPTaskPanelItem to be added.
	//			nID - Identifier of the item.
	// Returns: A pointer to a CXTPTaskPanelItem object added.
	// Summary: Call this member to add a new item to the collection.
	CXTPTaskPanelItem* Add(CXTPTaskPanelItem* pItem, UINT nID);

	CXTPTaskPanelItem* InsertAt(int nIndex, CXTPTaskPanelItem* pItem);
	
	// Input:	nIndex - Specifies the zero-based index of the item to retrieve. 
	// Returns: A pointer to a CXTPTaskPanelItem object.
	// Summary: Call this member to return the item at the specified index
	CXTPTaskPanelItem* GetAt(int nIndex);
	
	// Summary: Call this method to remove all items in the collection.
	void Clear(BOOL bReposition = TRUE);

	// Input:	nID - Identifier of item to be found.
	// Returns: A pointer to item if successful; otherwise returns NULL.
	// Summary: Call this member function to find an item by its ID 
	CXTPTaskPanelItem* Find(UINT nID);
	
	// Input:	pItem - Item to be removed.
	// Summary: Call this method to remove the specified item.
	void Remove(CXTPTaskPanelItem* pItem);
	
	// Input:	nIndex - Specifies the zero-based index of the item to remove. 
	// Summary: Call this method to remove the specified item by its index.
	void RemoveAt(int nIndex);

	// Input:	pItem - Item to be tested
	// Returns: Index of item in collection.
	// Summary: Call this member to get index of item in the collection of items.
	int GetIndex(CXTPTaskPanelItem* pItem);

	// Returns: Number of items.
	// Summary: Call this member to get count of items
	int GetCount();

	// Returns: A pointer to a CCmdTarget object owned by this collection
	// Summary: Call this member function to get a pointer to the owner
	CCmdTarget* GetOwner();

	void Move(CXTPTaskPanelItem* pItem, int nIndex);

	virtual void Serialize(CArchive& ar);

protected:
	CXTPTaskPanel* m_pPanel;									// Parent TaskPanel control.
	CArray<CXTPTaskPanelItem*, CXTPTaskPanelItem*> m_arrItems;	// Array of items.

	CCmdTarget* m_pOwner;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTPTaskPanelItems)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Summary: Destroys a CXTPTaskPanelItems object, handles cleanup and de-allocation.
	virtual ~CXTPTaskPanelItems();

	// Generated message map functions
	//{{AFX_MSG(CXTPTaskPanelItems)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

//:Ignore
#ifdef _XTP_ACTIVEX

	afx_msg long OleGetCount();
	afx_msg LPDISPATCH OleItem(long nIndex);

	afx_msg void OleClear();	
	afx_msg void OleRemove(long nIndex);	
	afx_msg LPDISPATCH OleFind(long nID);
	afx_msg void OleMove(LPDISPATCH lpItem, long nIndex);

	
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
	
	DECLARE_OLETYPELIB_EX(CXTPTaskPanelItems)
	

	enum {
		dispidCount = 1L,
		dispidRemove = 2L,
		dispidClear = 3L,
		dispidAdd = 4L,
		dispidFind = 5L,
	};

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	
#endif

	friend class CXTPTaskPanel;			
	friend class CXTPTaskPanelGroup;	

	DECLARE_MESSAGE_MAP()
//:End Ignore

};


//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanelGroupItems is a CXTPTaskPanelItems derived class. It is used as collection of items in the group.
class _XTP_EXT_CLASS CXTPTaskPanelGroupItems : public CXTPTaskPanelItems
{
public:

	// Input:	pPanel - Points to a CXTPTaskPanel object
	// Summary: Constructs CXTPTaskPanelGroupItems object
	CXTPTaskPanelGroupItems(CXTPTaskPanel* pPanel)
		: CXTPTaskPanelItems(pPanel)
	{
		EnableAutomation();
	}
//:Ignore
#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPTaskPanelGroupItems)


	afx_msg LPDISPATCH OleAdd(long nID, LPCTSTR lpCaption, long nType, const VARIANT& varImageIndex);

#endif
//:End Ignore

};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPTaskPanelGroupItems is a CXTPTaskPanelItems derived class. It is used as a collection of 
//			groups in the control.
class _XTP_EXT_CLASS CXTPTaskPanelGroups : public CXTPTaskPanelItems
{
public:

	// Input:	pPanel - Points to a CXTPTaskPanel object
	// Summary: Constructs a CXTPTaskPanelGroups object
	CXTPTaskPanelGroups(CXTPTaskPanel* pPanel)
		: CXTPTaskPanelItems(pPanel)
	{
		EnableAutomation();
	}
//:Ignore
#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPTaskPanelGroups)


	afx_msg LPDISPATCH OleAdd(long nID, LPCTSTR lpCaption);	
#endif
//:End Ignore
};


AFX_INLINE int CXTPTaskPanelItems::GetCount()
{
	return (int)m_arrItems.GetSize();
}
AFX_INLINE CXTPTaskPanelItem* CXTPTaskPanelItems::GetAt(int nIndex)
{
	return nIndex >= 0 && nIndex < m_arrItems.GetSize()? m_arrItems.GetAt(nIndex): NULL;
}
AFX_INLINE CCmdTarget* CXTPTaskPanelItems::GetOwner()
{
	return m_pOwner;
}

//:Ignore
/////////////////////////////////////////////////////////////////////////////


#endif // !defined(__XTPTASKPANELITEMS_H__)
//:End Ignore