// XTPPropertyGridItem.h interface for the CXTPPropertyGridItem class.
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
#if !defined(__XTPPROPERTYGRIDITEM_H__)
#define __XTPPROPERTYGRIDITEM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

// class forwards.

class CXTPPropertyGridView;
class CXTPPropertyGridItem;

const int PGI_EXPAND_BORDER = 14;

//////////////////////////////////////////////////////////////////////
// Summary: Flags that indicate what styles to set for the grid item.
enum XTP_PROPERTYGRID_ITEMFLAGS
{
	pgiHasEdit         = 1, // Item has an edit control.
	pgiHasExpandButton = 2, // Item has an expand button.
	pgiHasComboButton  = 4  // Item has a combo button.
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPPropertyGridItems is a CList derived class. It represents the items 
//			collection with some internal functionality.
class _XTP_EXT_CLASS CXTPPropertyGridItems : public CCmdTarget, 
	private CList<CXTPPropertyGridItem*, CXTPPropertyGridItem*>
{

public:

	// Input:	strCaption - Caption for the item to find.
	// Returns: The pointer to the Item object.
	// Summary: Call this member function to find an item.
	CXTPPropertyGridItem* FindItem(CString strCaption);

	// Input:	nID - Identifier for the item to find.
	// Returns: A pointer to a CXTPPropertyGridItem object.
	// Summary: Call this member function to find an item.
	CXTPPropertyGridItem* FindItem(UINT nID);

	// Summary: Call this member to clear all the items in a list.
	void Clear();

	// Returns: Count of the items in a list.
	// Summary: Call this member to get count of the items in a list.
	long GetCount();
	
	// Input:	nIndex - Item's index.
	// Returns: A pointer to a CXTPPropertyGridItem object.
	// Summary: Call this member to get an item by its index.
	CXTPPropertyGridItem* GetAt(int nIndex);

	// Input:	nIndex - Item's index.
	// Summary: Call this member to remove an item by its index.
	void RemoveAt (int nIndex);

//:Ignore

	// Summary: Call this member function to sort items alphabetically. 
	void Sort();

protected:

	
	
	CXTPPropertyGridItems()
	{
		m_pGrid = 0;
	#ifdef _XTP_ACTIVEX
		EnableAutomation();
		EnableTypeLib();
	#endif

	}

	typedef CList<CXTPPropertyGridItem*, CXTPPropertyGridItem*> CListItems;

	CXTPPropertyGridView* m_pGrid;

	void PASCAL operator delete(void* p)
	{
		CCmdTarget::operator delete(p);
	}
	void* PASCAL operator new(size_t nSize)
	{
		return CCmdTarget::operator new(nSize);
	}

#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPPropertyGridItems)

	afx_msg long OleGetCount();
	afx_msg LPDISPATCH OleItem(long nIndex);

	afx_msg void OleClear();	
	afx_msg void OleRemove(long nIndex);	
	
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
		dispidRemove = 2L,
		dispidClear = 3L,
	};
	
#endif


	friend class CXTPPropertyGridItem;
	friend class CXTPPropertyGridView;
//:End Ignore
};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPItemConstraints is a CList derived class. It represents the item 
//			constraints collection.
class _XTP_EXT_CLASS CXTPItemConstraints : public CCmdTarget, public CStringList
{
public:

	// Input:	str - New constraint.
	// Returns: The integer value that represents the index of the added constraint.
	// Summary: Call this member function to add a new item constraint.
	int AddConstraint(CString str);
		
	// Input:	str - A constraint to search for.
	// Returns: The zero-based index of the located constraint or -1 if the constraint 
	//			is not found.
	// Summary: Call this member function to find a constraint in the list.
	int FindConstaint(CString str);

	// Summary: Call this member to sort constraints alphabetically.
	void Sort();

//:Ignore
public:
	void PASCAL operator delete(void* p)		
	{
		CCmdTarget::operator delete(p);
	}
	void* PASCAL operator new(size_t nSize)
	{
		return CCmdTarget::operator new(nSize);
	}

private:		

#ifdef _XTP_ACTIVEX

public:

	// Summary: Constructs a CXTPItemConstraints object
	CXTPItemConstraints()
	{
		EnableAutomation();
		EnableTypeLib();
	}
	
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPItemConstraints)


	afx_msg long OleGetCount();
	afx_msg BSTR OleItem(long nIndex);
	afx_msg void OleClear();	
	afx_msg void OleRemove(long nIndex);	
	afx_msg void OleAdd(LPCTSTR strItem);	
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
		dispidRemove = 2L,
		dispidAdd = 3L,
		dispidClear = 4L,
	};
	
#endif

//:End Ignore

};

//////////////////////////////////////////////////////////////////////
// Summary: CXTPPropertyGridItem is the base class representing an item of the 
//			Property Grid Control. 
class _XTP_EXT_CLASS CXTPPropertyGridItem : public CCmdTarget
{
public:

	// Input:	strCaption - Caption of the item.
	//			strValue - Initial value.
	// Summary: Constructs a CXTPPropertyGridItem object
	//			Class CXTPPropertyGridItem has no default constructor. 
	//			Identifier of second constructor can be linked with a 
	//			STRINGTABLE resource with the same id in such form "Caption\\nDescription". 
	CXTPPropertyGridItem(CString strCaption, LPCTSTR strValue = NULL, CString* m_pBindString = NULL);
	
	// Input:	nID - Identifier of the item.
	// Summary: Constructs a CXTPPropertyGridItem object
	CXTPPropertyGridItem(UINT nID, LPCTSTR strValue = NULL, CString* m_pBindString = NULL);


	// Summary: Destroys a CXTPPropertyGridItem object, handles cleanup and de-allocation
	virtual ~CXTPPropertyGridItem(void);

public:

	// Input:	strCaption - The new caption of the item.
	// Summary: Call this method to set the caption of the item. 
	void SetCaption(CString strCaption);
	
	// Input:	strDescription - The new description of the item.
	// Summary: Call this method to set the description of the item. 
	void SetDescription(CString strDescription);

	// Input:	bReadOnly - Specifies whether to set or remove the read-only state of the 
	//			item. A value of TRUE sets the state to read-only. A value of FALSE sets
	//			the state to read/write. 
	// Summary: Call this method to set the read-only state of the item.
	void SetReadOnly(BOOL bReadOnly);

	// Input:	nID - The new identifier of the item.
	// Summary: Call this method to set the identifier of the item.
	void SetID(UINT nID);

	// BULLETED LIST:

	// Input:	nFlags - Flag to set. It can be one or more of the following:
	//			[ul]
    //			[li]<b>pgiHasEdit</b> - Item has edit control.[/li]
    //			[li]<b>pgiHasExpandButton</b> - Item has expand button.[/li]
    //			[li]<b>pgiHasComboButton</b> - Item has combo button.[/li]
	//			[/ul]
	// Summary:	This member function is called to set a specific flag for the item.
	void SetFlags(UINT nFlags);

	// Returns: A CString object containing the caption of the item.
	// Summary: Call this method to get the caption of the item.
	CString GetCaption();

	// Returns: A CString object containing the description of the item.
	// Summary: Call this method to get the description of the item.
	CString GetDescription();

	// Returns: TRUE if the item is read only, otherwise returns FALSE.
	// Summary: Call this method to get the read-only state of the item.
	BOOL GetReadOnly();

	// Returns: An integer value that represents the identifier of the item.
	// Summary: Call this method to get the identifier of the item.
	UINT GetID();

	// Returns: A CString object containing the value of the item..
	// Summary: Call this method to get the current value of the item.
	CString GetValue();

	// Returns: A integer value that represents item's flags.
	// Summary: Call this method to get the flags of the item.
	UINT GetFlags();

	// Returns: TRUE if the item is a category, otherwise returns FALSE.
	// Summary: The IsCategory function determines whether or not the item is a category. 
	BOOL IsCategory();

	// Input:	pItem - The item to be added as a child.
	// Returns: A pointer to a CXTPPropertyGridItem object.
	// Summary: Call this method to add a child item.
	CXTPPropertyGridItem* AddChildItem(CXTPPropertyGridItem* pItem);

	// Returns: A POSITION object.
	// Summary: Gets the position of the head element in the child list.
	POSITION GetFirstChildItem() const;

	// Input:	rPosition - A reference to a POSITION value returned by a previous
	//			GetNextChild, GetFirstChild, or another member function call.
	// Returns:	A pointer to a CXTPPropertyGridItem object.
	// Summary: Gets the child item identified by rPosition and then sets rPosition to
	//			the POSITION value of the next entry in the list.
	CXTPPropertyGridItem* GetNextChildItem(POSITION& rPosition) const;

	// Returns: TRUE if the item has children, otherwise returns FALSE.
	// Summary: The HasChilds function determines whether or not the item has children. 
	BOOL HasChilds();

	// Returns: A pointer to a CXTPPropertyGridItems object.
	// Summary: Returns a list of the children
	CXTPPropertyGridItems* GetChilds();

	// Input:	pParent - Item to be tested.
	// Returns: TRUE if pParent is a parent of the item.
	// Summary: The HasParent function determines whether or not the item has a pParent item
	//			as a parent. 
	BOOL HasParent(CXTPPropertyGridItem* pParent);

	// Example: <pre>CXTPItemConstraints* pList = pItem->GetConstraints();</pre>
	// Returns:	A pointer to a CXTPItemConstraints object.
	// Summary: Call this method to get the constraints list.
	CXTPItemConstraints* GetConstraints();

	// Input:	bConstraintEdit - TRUE to constraint edit.
	// Summary: Call this method to edit only values from constraint list.
	void SetConstraintEdit(BOOL bConstraintEdit);
	
	// Returns: TRUE if item is constraint edit.
	// Summary: Retrieves constraint edit state.
	BOOL GetConstraintEdit();

	// Summary: Call this method to expand the item.
	void Expand();

	// Summary: Call this method to collapse the item.
	void Collapse();

	// Summary: Call this method to select the item.
	void Select();

	// Returns: A CRect object that represents the bounding rectangle of the item.
	// Summary: Retrieves the bounding rectangle of the item
	CRect GetItemRect();

	// Returns: A CRect object that represents the bounding rectangle of the item value's text.
	// Summary: Retrieves the bounding rectangle of the item value's text.
	//			Override this member function to change it.
	virtual CRect GetValueRect();


	// Input:	strValue - New value of the item.
	// Summary: Call this method to change an item's value.
	//			Override this method to add new functionality.
	//			You should call the base class version of this function from your
	//			override.
	virtual void SetValue(CString strValue);

	// Input:	strValue - New value of the item.
	// Summary: This method is called when an item's value is changed. Override this
	//			function if needed.
	virtual void OnValueChanged(CString strValue);
	
	// Returns: The parent window. 
	// Summary: Call this member to return the parent window
	CWnd* GetGrid();

	// Returns: TRUE if the item is visible, otherwise returns FALSE.
	// Summary: Call this member function to determine if the item is visible.
	BOOL IsVisible() const;

	// Returns: TRUE if the item is expanded, otherwise returns FALSE.
	// Summary: Call this member function to determine if the item is expanded.  
	BOOL IsExpanded() const;

	// Returns: A CXTPPropertyGridItem pointer that represents the parent item, can be
	//			NULL.
	// Summary: Call this member function to retrieve the parent property grid item.
	CXTPPropertyGridItem* GetParentItem();

	// Returns: Item's indent.
	// Summary: Call this member function to retrieve the item's indent.
	int GetIndent();


	// Input:	dc - Reference to the device context to be used for rendering an image
	//			of the item.
	//			rcValue - Bounding rectangle of the item.
	// Returns: TRUE if item is self-drawn.
	// Summary: This method is called when an item is drawn. Override this function if
	//			needed.
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);

	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	// Summary: Retrieves if the item is selected.
	BOOL IsSelected();

	// Input:	pBindString - Binded string.
	// Summary: Call this method to bind an item to CString object.
	void BindToString(CString* pBindString);


	// Summary: Remove an item.
	void Remove();

	void SetMask(LPCTSTR strMask, LPCTSTR strLiteral, TCHAR chPrompt = _T('_'));
	void SetPasswordMask(TCHAR chMask = _T('*'));

	CString GetMaskedText();

	virtual CString GetViewValue();


protected:

	// Summary: This method is called when an item is selected. 
	virtual void OnSelect();

	// Summary: This method is called when an item is de-selected. 
	virtual void OnDeselect();

	// Input:	nChar - Contains the character code value of the key.
	// Returns: TRUE if a key is pressed, otherwise returns FALSE.
	// Summary: This method is called when a key is pressed.
	virtual BOOL OnChar(UINT nChar);

	// Summary: This method is called when the user double-clicks the left mouse button. 
	virtual void OnLButtonDblClk();

	virtual void OnRButtonDown();

	// Input:	nFlags - Indicates whether various virtual keys are down
	//			(see Visual C++ documentation).
	//			point - Specifies the x- and y-coordinate of the cursor.
	// Returns: TRUE if the left mouse button is clicked, otherwise returns FALSE.
	// Summary: This method is called when the user presses the left mouse button.
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);

	// Summary: This method is called when an item is added to the parent.
	virtual void OnAddChildItem();

	// Summary: This method is called when the user presses the inplace button.
	//			Override the method to show an item-specific dialog.
	virtual void OnInplaceButtonDown();

	// Returns: A reference to an inplace edit control.
	// Summary: Call this member function to return inplace edit control of the item
	virtual CXTPPropertyGridInplaceEdit& GetInplaceEdit();

	// Returns: A reference to an inplace button control.
	// Summary: Call this member function to return inplace button control of the item
	CXTPPropertyGridInplaceButton& GetInplaceButton();

	// Returns: A reference to an inplace list control.
	// Summary: Call this member function to return inplace list control of the item
	CXTPPropertyGridInplaceList& GetInplaceList();

	// Summary: This method is called to check value from inplace edit control.
	virtual void OnValidateEdit();
	
	// Input:	str - String value
	// Summary: This method is called to set text of the inplace edit control.
	virtual void SetEditText(CString str);

	
	
protected:
	BOOL					m_bConstraintEdit;	// TRUE to constraint edit.

	int						m_nIndex;			// Index of the item.
	int						m_nIndent;			// Indent of the item.
	UINT					m_nID;				// Identifier of the item.
	UINT					m_nFlags;			// Item's flags.
	BOOL					m_bReadOnly;		// TRUE to disable item's edition.
	BOOL					m_bVisible;			// Visibility of the item.
	BOOL					m_bCategory;		// TRUE if the item is category.
	BOOL					m_bExpanded;		// TRUE if item is expanded.
	CString					m_strValue;			// Value of the item.
	CString					m_strCaption;		// Caption of the item.
	CString					m_strDescription;	// Description of the item.
	CXTPPropertyGridItem*	m_pParent;			// Parent item.
	CXTPPropertyGridView*	m_pGrid;			// Parent grid class.
	CXTPPropertyGridItems*	m_pChilds;			// Child items.
	CXTPItemConstraints*	m_pContraints;		// Item's constraints.

	CString*			    m_pBindString;		// Binded string.
	CString m_strFormat;						// Format of the double value.

	CString m_strMask;
	CString m_strLiteral;
	TCHAR m_chPrompt;
	bool m_bUseMask;

	BOOL m_bPassword;

private:
	void Init();
	void Refresh(BOOL bInvalidate = TRUE);
	

	friend class CXTPPropertyGridItems;
	friend class CXTPPropertyGridView;
	friend class CXTPPropertyGridInplaceEdit;
	friend class CXTPPropertyGridInplaceButton;
	
	DECLARE_DYNAMIC(CXTPPropertyGridItem)

//:Ignore	

#ifdef _XTP_ACTIVEX

protected:

	DISPID m_dispidBinded;
	COleDispatchDriver m_dispDriverBinded;


	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPPropertyGridItem)

	afx_msg LPDISPATCH OleAddChildItem(long nType, LPCTSTR strTitle, const VARIANT* varValue);
	afx_msg LPDISPATCH OleAddChildItemBinded(long nType, LPDISPATCH lpHandler, LPCTSTR lpzsProperty);
	afx_msg BOOL OleGetExpanded();
	afx_msg void OleSetExpanded(BOOL bExpanded);
	afx_msg BOOL OleGetSelected();
	afx_msg void OleSetSelected(BOOL bSelected);
	afx_msg LPDISPATCH OleGetParent();
	afx_msg LPDISPATCH OleGetChilds();
	afx_msg LPDISPATCH OleGetConstraints();
	afx_msg long OleGetType();
	afx_msg BSTR OleGetMaskedText();
	
	afx_msg virtual const VARIANT OleGetValue();
	afx_msg virtual void OleSetValue(const VARIANT* varValue);
	afx_msg void OleSetMask(LPCTSTR strMask, LPCTSTR strLiteral, const VARIANT* pPrompt);

	virtual void BindDispatch();


	long m_nType;
	enum {
		dispidValue = 0L,
		dispidExpanded = 1L,
		dispidReadOnly = 2L,
		dispidSelected = 3L,
		dispidCaption = 4L,
		dispidDescription = 5L,
		dispidTag = 6L,
		dispidFlags = 7L,
		dispidConstraintEdit = 8L,
		dispidFormat = 9L,
		dispidConstraints = 97L,
		dispidChilds = 98L,
		dispidParent = 99L,
		dispidAddChildItem = 100L,	
		dispidType = 101L,	
		dispidAddChildItemBinded = 102,
	};
	
#endif
//:End Ignore
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE POSITION CXTPPropertyGridItem::GetFirstChildItem() const { 
	return m_pChilds->GetHeadPosition();
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGridItem::GetNextChildItem(POSITION& rPosition) const { 
	return m_pChilds->GetNext(rPosition); 
}
AFX_INLINE BOOL CXTPPropertyGridItem::HasChilds() { 
	return !m_pChilds->IsEmpty(); 
}
AFX_INLINE void CXTPPropertyGridItem::SetCaption(CString strCaption) { 
	m_strCaption = strCaption; 
}
AFX_INLINE CString CXTPPropertyGridItem::GetCaption() { 
	return m_strCaption; 
}
AFX_INLINE void CXTPPropertyGridItem::SetDescription(CString strDescription) {
	m_strDescription = strDescription; 
}
AFX_INLINE CString CXTPPropertyGridItem::GetDescription() {
	return m_strDescription;
}
AFX_INLINE BOOL CXTPPropertyGridItem::GetReadOnly() {
	return m_bReadOnly;
}
AFX_INLINE void CXTPPropertyGridItem::SetID(UINT nID) {
	m_nID = nID;
}
AFX_INLINE UINT CXTPPropertyGridItem::GetID() {
	return m_nID;
}
AFX_INLINE CString CXTPPropertyGridItem::GetValue() {
	return m_strValue;
}
AFX_INLINE BOOL CXTPPropertyGridItem::IsCategory() { 
	return m_bCategory; 
}
AFX_INLINE CWnd* CXTPPropertyGridItem::GetGrid() {
	return (CWnd*)m_pGrid;
}
AFX_INLINE BOOL CXTPPropertyGridItem::OnDrawItemValue(CDC&, CRect) { 
	return FALSE; 
}
AFX_INLINE void CXTPPropertyGridItem::OnAddChildItem() {

}
AFX_INLINE void CXTPPropertyGridItem::SetFlags(UINT nFlags) {
	ASSERT(!m_bCategory); ASSERT((nFlags & (pgiHasComboButton | pgiHasExpandButton)) != (pgiHasComboButton | pgiHasExpandButton)); m_nFlags = nFlags;
}
AFX_INLINE UINT CXTPPropertyGridItem::GetFlags() {
	return m_nFlags;
}
AFX_INLINE BOOL CXTPPropertyGridItem::IsVisible() const {
	return m_bVisible;
}
AFX_INLINE BOOL CXTPPropertyGridItem::IsExpanded() const {
	return m_bExpanded;
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGridItem::GetParentItem() {
	return m_pParent;
}
AFX_INLINE CXTPPropertyGridItems* CXTPPropertyGridItem::GetChilds() {
	return m_pChilds;
}
AFX_INLINE int CXTPPropertyGridItem::GetIndent() {
	return m_nIndent;
}
AFX_INLINE void CXTPPropertyGridItem::SetConstraintEdit(BOOL bConstraintEdit) {
	m_bConstraintEdit = bConstraintEdit;
}
AFX_INLINE BOOL CXTPPropertyGridItem::GetConstraintEdit() {
	return m_bConstraintEdit;
}
AFX_INLINE void CXTPPropertyGridItem::SetPasswordMask(TCHAR chMask ) {
	m_bPassword = TRUE;
	m_chPrompt = chMask;
}
AFX_INLINE void CXTPPropertyGridItem::OnRButtonDown() {

}
AFX_INLINE void CXTPPropertyGridItem::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/) {

}

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTPItemConstraints::AddConstraint(CString str){
	AddTail(str); return (int)GetCount() - 1;
}
AFX_INLINE CXTPItemConstraints* CXTPPropertyGridItem::GetConstraints(){
	return m_pContraints;
}

AFX_INLINE long CXTPPropertyGridItems::GetCount() {
	return (long)CList<CXTPPropertyGridItem*, CXTPPropertyGridItem*>::GetCount();
}

#endif // #if !defined(__XTPPROPERTYGRIDITEM_H__)