// CXTPReportColumns.h: interface for the CXTPReportColumns class.
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
#if !defined(__XTPREPORTCOLUMNS_H__)
#define __XTPREPORTCOLUMNS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportColumn;
class CXTPReportHeader;
class CXTPReportControl;
class CXTPReportColumnOrder;

//===========================================================================
// Remarks: This class is a collection of report columns objects, 
//			each of which can be referred to by its zero-based index.
//			It provides main operations to manipulate with list of columns:
//		    - Adding/removing columns
//		    - Showing columns header
//		    - Working with field chooser
//		    - Working with "group by" field
//		    - (Re)Storing column settings 
//			To create an oblect of that class simply call it constructor with 
//			one parameter - pointer to the Report Control object
//===========================================================================
class _XTP_EXT_CLASS CXTPReportColumns : public CCmdTarget
{
	friend class CXTPReportColumn;
public:
	//-----------------------------------------------------------------------
	// Input:	pControl - Pointer to the parent Report Control object.
	// Summary: Constructs a CXTPReportColumns object
	//-----------------------------------------------------------------------
	CXTPReportColumns(CXTPReportControl* pControl);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPReportColumns object,
	// Remarks: handles cleanup and de-allocation, including all of objects 
	// presented in list by calling Clear() member function 
	//			
	//-----------------------------------------------------------------------
	virtual ~CXTPReportColumns(void);

	//-----------------------------------------------------------------------
	// Summary: Returns the total number of column items in the collection.
	// Returns: Total count of column items in the collection.
	//-----------------------------------------------------------------------
	int GetCount();

	//-----------------------------------------------------------------------
	// Summary: Clears columns collection items.
	//-----------------------------------------------------------------------
	void Clear();

	//-----------------------------------------------------------------------
	// Summary: Returns the number of visible columns in the collection.
	// Remarks: In opposite to GetCount() member function this one return
	//			count only those columns thah have visibility ptoperty set to 
	//			true
	// Returns: The number of visible columns in the collection.
	//-----------------------------------------------------------------------
	int GetVisibleColumnsCount();

	//-----------------------------------------------------------------------
	// Input:	nIndex - An integer index that is greater than or equal to 0 and less than the value returned by GetCount.
	// Summary: Returns specific column item by its numeric index.
	// Remarks: Returns the columns list element at the specified index.
	//          Passing a negative value or a value greater than the value returned by GetCount()
	//			will return NULL.
	// Returns: The pointer to column array element currently at this index.
	//-----------------------------------------------------------------------
	CXTPReportColumn* GetAt(int nIndex);

	//-----------------------------------------------------------------------
	// Input:	nIndex - Index of column that is visible on position number nIndex.
	// Summary: Returns a column at specified visible position.
	// Remarks: Returns first visible column before column with given index
	//			or column with given index. 
	//          Passing a negative value or a value greater than the value returned by 
	//          GetCount() or if visible element not found will return NULL.
	// Returns: Pointer to column object.
	//-----------------------------------------------------------------------
	CXTPReportColumn* GetVisibleAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary: Gets specific indentation column.
	// Remarks: Indentation column used for representing itends in
	//			grouping mode. Indentation column usually it is the first column
	//			in list.
	// Returns: An indentation column.
	//-----------------------------------------------------------------------
	CXTPReportColumn* GetIndentColumn();

	//-----------------------------------------------------------------------
	// Input:	nItemIndex - Column item index.
	// Summary: Finds the column by its item index.
	// Remarks: Searches the list sequentially to find the first element matching 
	//          the specified item index column's value. If no matching found
	//			NULL is returned
	// Returns: Found column.
	//-----------------------------------------------------------------------
	CXTPReportColumn* Find(int nItemIndex);

	//-----------------------------------------------------------------------
	// Input:	pColumn - A column to add to the collection.
	// Summary: Adds one more column item to the collection.
	// Remarks: Adds a new element to the end of a collection, growing the 
	//          collection by 1
	//-----------------------------------------------------------------------
	void Add(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary: Clear sort order for each column.
	//-----------------------------------------------------------------------
	void ResetSortOrder();

	//-----------------------------------------------------------------------
	// Summary: Returns pointer to the associated report header object.
	// Returns: A pointer to the associated report header.
	//-----------------------------------------------------------------------
	CXTPReportHeader* GetReportHeader();

	//-----------------------------------------------------------------------
	// Input:	nNewOrder - A new column order index.
	//			nItemIndex - Item index of the changing column.
	// Summary: Changes column order in the collection.
	// Remarks: Moves column with index nItemIndex to the new position with
	//          nNewOrder. If any peramater's value is negative, no changdes 
	//			will be done and returns -1
	// Returns: New column order index.
	//-----------------------------------------------------------------------
	int ChangeColumnOrder(int nNewOrder, int nItemIndex);

	//-----------------------------------------------------------------------
	// Input:	pColumn - The column.
	// Summary: Call this method to get index of the column.
	// Remarks: Searches for the first occurence of the given Column, beginning
	//          the search at index, and first position for equality using pointers
	//          equality. If no matching found return -1 
	// Returns: The index of the column;
	//-----------------------------------------------------------------------
	int IndexOf(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - A column to insert.
	// Summary: Inserts provided column to the sort columns array acoording to 
	//          the columns order in the group columns array.
	// Returns: The index of the column inserted into the sort columns array.
	//-----------------------------------------------------------------------
	void InsertSortColumn(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - Pointer to the specified column object.
	//			bIncreasing - TRUE to sort increasing.
	// Summary: Sets a sort order for the column.
	// Remarks: Sets given column as the sorted column, reset previous sort 
	//          order 
	//-----------------------------------------------------------------------
	void SetSortColumn(CXTPReportColumn* pColumn, BOOL bIncreasing);

	//-----------------------------------------------------------------------
	// Summary: Provides a user with a columns groups order array.
	// Returns: A pointer to the columns groups order array.
	//-----------------------------------------------------------------------
	CXTPReportColumnOrder* GetGroupsOrder();

	//-----------------------------------------------------------------------
	// Summary: Provides a user with a columns sort order array.
	// Returns: A pointer to the columns sort order array.
	//-----------------------------------------------------------------------
	CXTPReportColumnOrder* GetSortOrder();

	//-----------------------------------------------------------------------
	// Input:	ar - Archive stream to write columns data to.
	// Summary: Serializes columns data and state to/from the provided archive stream.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);
protected:
	CXTPReportHeader* m_pReportHeader;	// List header member.

	CXTPReportColumnOrder* m_pGroupsOrder;	// Container for the columns order in auto grouping mode.	
	CXTPReportColumnOrder* m_pSortOrder;	// Container for the columns sort order.	

	CArray<CXTPReportColumn*, CXTPReportColumn*> m_arrColumns;	// Storage for column items.
	CXTPReportControl* m_pControl;	// Pointer to the associated report control object.

	CXTPReportColumn* m_pTreeColumn;	// Column containing tree view images.


#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportColumns);

	afx_msg LPDISPATCH OleItem(long nIndex);
	afx_msg LPDISPATCH OleAdd(long nId, LPCTSTR strCaption, int nWidth, BOOL bAutoSize);
	afx_msg LPUNKNOWN OleNewEnum();	
	afx_msg LPDISPATCH OleFindColumn(long nId);

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
		dispidFindColumn = 4L,
	};
	
#endif
};

AFX_INLINE int CXTPReportColumns::GetCount() {
	return (int)m_arrColumns.GetSize();
}

AFX_INLINE CXTPReportColumn* CXTPReportColumns::GetAt(int nIndex) {
	return (nIndex >= 0 && nIndex < GetCount()) ? m_arrColumns.GetAt(nIndex) : NULL;		
}

AFX_INLINE CXTPReportColumnOrder* CXTPReportColumns::GetGroupsOrder() {
	return m_pGroupsOrder;
}

AFX_INLINE CXTPReportColumnOrder* CXTPReportColumns::GetSortOrder() {
	return m_pSortOrder;
}

AFX_INLINE CXTPReportColumn* CXTPReportColumns::GetIndentColumn() {
	return (GetCount() > 0) ? m_arrColumns.GetAt(0) : NULL;
}

AFX_INLINE CXTPReportHeader* CXTPReportColumns::GetReportHeader() { 
	return m_pReportHeader; 
}

/////////////////////////////////////////////////////////////////////////////
// CXTPReportColumnOrder

//===========================================================================
// Remarks:	Represents column order, used for sorting and grouping.
//          This class is a collection of sorted columns objects, 
//			each of which can be referred to by its zero-based index.
//			It provides main operations to manipulate with list of columns:
//		    Adding/removing columns, get at given position, clear collection,
//			get index of object by it pointer
//          You create object simply by calling constructor with one parameter 
//          pointer to the columns collection
//===========================================================================
class _XTP_EXT_CLASS CXTPReportColumnOrder : public CCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Input:	pColumns - Parent columns collection.
	// Summary: Default column order constructor.
	//-----------------------------------------------------------------------
	CXTPReportColumnOrder(CXTPReportColumns* pColumns);

	//-----------------------------------------------------------------------
	// Input:	nIndex - An integer index that is greater than or equal to 0 and 
	//          less than the value returned by GetCount().
	// Summary: Returns specific column item by its numeric index.
	// Remarks: Returns the columns list element at the specified index.
	//          Passing a negative value or a value greater than the value 
	//          returned by GetCount() will return NULL.
	// Returns: The pointer to column array element currently at this index.
	//-----------------------------------------------------------------------
	CXTPReportColumn* GetAt(int nIndex);

	//-----------------------------------------------------------------------
	// Input:	nIndex - Order index to insert column to.
	//			pColumn - A pointer to the inserted column.
	// Summary:	Inserts specified column at a specified position.
	// Returns:	The index of the inserted column.
	//			-1 in case of an error.
	//-----------------------------------------------------------------------
	int InsertAt(int nIndex, CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	nIndex - Order index of the column to remove.
	// Summary:	Removes specified column at a specified position from the order array.
	//-----------------------------------------------------------------------
	void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Input:	pColumn - A column to remove from the order array.
	// Summary: Removes specified column from order array.
	//-----------------------------------------------------------------------
	void Remove(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - An added column pointer.
	// Summary:	Inserts specified column at the end of the order list.
	// Remarks: Adds a new element to the end of a collection, growing the 
	//          collection by 1
	// Returns:	The index of the added element.
	//-----------------------------------------------------------------------
	int Add(CXTPReportColumn* pColumn, BOOL bSortIncreasing);

	int Add(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Input:	pColumn - The column.
	// Summary: Call this method to get index of the column.
	// Remarks: Searches for the first occurence of the given Column, beginning
	//          the search at index, and first position for equality using pointers
	//          equality. If no matching found return -1 
	// Returns: The index of the column;
	//-----------------------------------------------------------------------
	int IndexOf(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:	Clears all stored order information.
	//-----------------------------------------------------------------------
	void Clear();

	//-----------------------------------------------------------------------
	// Summary:	Returns the number of stored order items.
	// Returns: The count of items stored in the order array.
	//-----------------------------------------------------------------------
	int GetCount();

	//-----------------------------------------------------------------------
	// Input:	ar - Provided archive stream for serialization.
	// Summary: Serializes column order settings from/to the provided archive stream.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);

protected:
	CXTPReportColumns* m_pColumns;	// Parent columns array.

	CArray<CXTPReportColumn*, CXTPReportColumn*> m_arrColumns;	// Storage for column items with the corresponding order.


#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportColumnOrder);

	afx_msg LPUNKNOWN OleNewEnum();	
	
	afx_msg LPDISPATCH OleItem(long nIndex);
	afx_msg void OleAdd(LPDISPATCH lpColumn);
	afx_msg int OleIndexOf(LPDISPATCH lpColumn);


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
			int m_nCurrent;;
	END_INTERFACE_PART(EnumVARIANT)

	enum {
		dispidCount = 1L,
	};
	
#endif

};

#endif