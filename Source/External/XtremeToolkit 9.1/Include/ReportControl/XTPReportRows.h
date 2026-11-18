// XTPReportRows.h: interface for the CXTPReportRows class.
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
#if !defined(__XTPREPORTROWS_H__)
#define __XTPREPORTROWS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportRow;
class CXTPReportControl;
class CXTPReportColumns;

//===========================================================================
// Remarks: This class represents a rows collection class. 
//			It supports an array of CXTPReportRow pointers.
//
//			Array indexes start at position 0.
//
// Example: See CXTPReportRows::Add for an example of how to work with this class.
//
// See also: CXTPReportRow, CXTPReportSelectedRows
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRows : public CCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs an empty CXTPReportRow pointer array.
	// Example:	// Declare a local CXTPReportRows object.
	//			CXTPReportRows myList;
	//
	//			// Declare a dynamic CXTPReportRows object.
	//			CXTPReportRows* pTree = new CXTPReportRows();
	// See also: ~CXTPReportRows
	//-----------------------------------------------------------------------
	CXTPReportRows();

	//-----------------------------------------------------------------------
	// Summary: Destroys CXTPReportRows object. Performs cleanup operations.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportRows();

	//-----------------------------------------------------------------------
	// Summary: Removes all the elements from this array.
	// Remarks: Removes all the pointers from this array and releases instances 
	//			of all stored CXTPReportRow objects.
	// Example: See example for CXTPReportRows::Add method.
	// See also: CXTPReportRows overview
	//-----------------------------------------------------------------------
	void Clear();

	//-----------------------------------------------------------------------
	// Summary: Gets the number of CXTPReportRow elements in this collection.
	// Remarks: Call this method to retrieve the number of elements in the array. 
	//			Because indexes are zero-based, the size is 1 greater than 
	//			the largest index.
	// Example: See example for CXTPReportRows::Add method.
	// Returns: The number of items in the collection.
	// See also: CXTPReportRows overview
	//-----------------------------------------------------------------------
	int GetCount();

	//-----------------------------------------------------------------------
	// Input:	nIndex - An integer index that is greater than or equal to 0 
	//					 and less than the value returned by GetCount. 
	// Summary: Returns a row at the specified numeric index.
	// Remarks: Returns the array element at the specified index.
	// Example: See example for CXTPReportRows::Add method.
	// Returns: The row element currently at this index.
	//-----------------------------------------------------------------------
	CXTPReportRow* GetAt(int nIndex);
	
	//-----------------------------------------------------------------------
	// Input:	pRow - The row element to be added to this array.
	// Summary: Adds a new Row element to the end of an array.
	// Remarks: Use this method to add the specified CXTPReportRow pointer 
	//			to the end of the CXTPReportRows collection.
	// Example: CXTPReportRows* pTree = new CXTPReportRows();
	//			pTree->Add(new CXTPReportRow(pControl, pRecord1));
	//			pTree->Add(new CXTPReportRow(pControl, pRecord2));
	//			CXTPReportRow* pRow0 = pTree->GetAt(0);
	//			CXTPReportRow* pRow1 = pTree->GetAt(1);
	//			ASSERT( pRow0 == pTree->GetPrev(pRow1) );
	//			ASSERT( pRow1 == pTree->GetNext(pRow0) );
	//			pTree->RemoveAt(0);
	//			ASSERT( 1 == pTree->GetCount() );
	//			pTree->InsertAt(0, pRow0);
	//			ASSERT( 2 == pTree->GetCount() );
	//			pTree->Clear();
	//			ASSERT( 0 == pTree->GetCount() );
	// See also: CXTPReportRows overview, GetAt, InsertAt, Clear, 
	//			 GetNext, GetPrev, GetCount
	//-----------------------------------------------------------------------
	void Add(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Input:	nIndex - An integer index that is greater than or equal to 0 
	//					 and less than the value returned by GetCount. 
	// Summary:	Removes an item from the collection on specified position.
	// Remarks: In the process, it shifts down all the elements above the 
	//			removed element. It decrements the upper bound of the array 
	//			but does not free memory. 
	//			It also releases an instance of the removed element.
	// Example: See example for CXTPReportRows::Add method.
	//-----------------------------------------------------------------------
	void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Input:	nIndex - An integer index that is greater than or equal to 0 
	//					 and less than the value returned by GetCount. 
	//			pRow   - The row element to be placed in this array. 
	// Summary: Adds one more Row object to the collection at the specified position.
	// Remarks: InsertAt inserts one element at a specified index in an array.
	//			In the process, it shifts up (by incrementing the index) the 
	//			existing element at this index, and it shifts up all the elements 
	//			above it. 
	// Example: See example for CXTPReportRows::Add method.
	//-----------------------------------------------------------------------
	void InsertAt(int nIndex, CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Input:	pRow - A reference to the current element of the list.
	// Summary: Returns next row in the list.
	// Remarks: This function finds a pointer to the provided row element
	//			in the list, and returns a pointer to the elements following
	//			the found one.
	// Returns: A pointer to a next element of the list.
	// Example: See example for CXTPReportRows::Add method.
	// See also: GetPrev
	//-----------------------------------------------------------------------
	CXTPReportRow* GetNext(CXTPReportRow* pRow, BOOL bSkipGroupFocus);

	//-----------------------------------------------------------------------
	// Input:	pRow - A reference to the current element of the list.
	// Summary: Returns previous row in the list.
	// Remarks: This function finds a pointer to the provided row element
	//			in the list, and returns a pointer to the elements before
	//			the found one.
	// Returns: A pointer to a previous element of the list.
	// Example: See example for CXTPReportRows::Add method.
	// See also: GetNext
	//-----------------------------------------------------------------------
	CXTPReportRow* GetPrev(CXTPReportRow* pRow, BOOL bSkipGroupFocus);

	//-----------------------------------------------------------------------
	// Summary: Sorts collection elements.
	// Remarks: This method uses Visual C++ run-time library (MSVCRT) 
	//			implementation of the quick-sort function, qsort, for sorting
	//			stored CXTPReportRow objects.
	//
	//			It internally uses CompareRows method for comparing 2 rows. 
	// See also: CompareRows
	//-----------------------------------------------------------------------
	void Sort();


protected:

	//-----------------------------------------------------------------------
	// Input:	pRow1 - First row for comparison.
	//			pRow2 - Second row for comparison.
	// Summary: Compares 2 rows using groups and sort orders of the report control.
	// Remarks: This function is called directly by Sort method.
	//			
	//			The default implementation returns the result of the comparison 
	//			of *pRow1 and *pRow2. Override this function so that it compares 
	//			the elements in a way that is appropriate for your application.
	//
	//			This implementation uses CXTPReportRecordItem::Compare for comparing
	//			two corresponding items from the row records. First it compares
	//			record items in the group order, then by sort order. It returns
	//			the first comparison result differ from equality. If no differences was
	//			found, it returns 0.
	//
	// Returns: Zero if pRow1 is equal to pRow2; 
	//			Less than zero if pRow1 is less than pRow2; 
	//			Greater than zero if pRow1 is greater than pRow2.
	// See also: Sort, CXTPReportRecordItem::Compare
	//-----------------------------------------------------------------------
	static int _cdecl CompareRows(const CXTPReportRow** pRow1, const CXTPReportRow** pRow2);


protected:
	CArray<CXTPReportRow*, CXTPReportRow*> m_arrRows;	// Internal storage for CXTPReportRow objects.


#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportRows);

	afx_msg LPDISPATCH OleItem(long nIndex);
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
	};
	
#endif

};

//===========================================================================
// Remarks: Encapsulates a collection of CXTPReportRow pointers that represent 
//			the selected rows in a Report Control. 
//
//			Use this class to programmatically manage a collection of 
//			CXTPReportRow pointers that represent the selected rows in a 
//			Report Control. This class is commonly used to add or remove rows
//			from the collection.
//
//			Typical workflow is using Add and Remove methods for changing
//			contents of the selection and using Contains method for checking
//			a specific row for its presence in the selection.
//
// Example: The following example demonstrates how to programmatically use 
//			the CXTPReportSelectedRows class to select rows in the Report control.
//
//			CXTPReportSelectedRows* pSelRows = pReportControl->GetSelectedRows();
//			pSelRows->Add(pRow1);
//			pSelRows->Add(pRow2);
//			ASSERT( TRUE == pSelRows->Contains(pRow1) );
//			ASSERT( TRUE == pSelRows->Contains(pRow2) );
//			pSelRows->Remove(pRow1);
//			ASSERT( FALSE == pSelRows->Contains(pRow1) );
//			pSelRows->Select(pRow1);
//			ASSERT( TRUE == pSelRows->Contains(pRow1) );
//			ASSERT( FALSE == pSelRows->Contains(pRow2) );
//
//
// See also: CXTPReportRow, CXTPReportSelectedRows, CXTPReportControl::GetSelectedRows
//===========================================================================
class _XTP_EXT_CLASS CXTPReportSelectedRows : public CCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Input:	pControl - Pointer to the parent report control.
	// Summary: Constructs an empty CXTPReportSelectedRows collection.
	// Remarks: This collection could be created only in association with 
	//			the CXTPReportControl object.
	// Example: // from CXTPReportControl member function
	//			CXTPReportSelectedRows* pSelectedRows = new CXTPReportSelectedRows(this);
	// See also: CXTPReportSelectedRows overview
	//-----------------------------------------------------------------------
	CXTPReportSelectedRows(CXTPReportControl* pControl);

	//-----------------------------------------------------------------------
	// Input:	pRowBegin - First row from the block.
	//			pRowEnd   - Final row from the block.
	// Summary: Selects a block of rows.
	// Remarks: This function uses pRowBegin and pRowEnd as the bound for the
	//			required selection. It enumerates parent report control rows
	//			collection and adds all rows from pRowBegin to pRowEnd inclusively
	//			to the selection.
	//-----------------------------------------------------------------------
	void SelectBlock(CXTPReportRow* pRowBegin, CXTPReportRow* pRowEnd);
	
	//-----------------------------------------------------------------------
	// Summary: Clears itself, removing selection.
	// Remarks: Removes all the elements from the selection.
	//-----------------------------------------------------------------------
	void Clear();

	//-----------------------------------------------------------------------
	// Input:	pRow - Pointer to the row to be added to the selection.
	// Summary: Adds a row to the selection.
	// Remarks: This method adds a pointer to the provided row to the selection.
	//			After adding, Contains method will return TRUE for all
	//			checking attempts of this row pointer.
	// Example: See example at CXTPReportSelectedRows overview
	// See also: CXTPReportSelectedRows overview, Remove, Select, Clear, Contains
	//-----------------------------------------------------------------------
	void Add(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Input:	pRow - Pointer to the row to be removed from the selection.
	// Summary: Removes a row from the selection.
	// Remarks: This method removes a provided row pointer from the selection.
	//			After removing, Contains method will return FALSE for all
	//			checking attempts of this row pointer.
	// Example: See example at CXTPReportSelectedRows overview
	// See also: CXTPReportSelectedRows overview, Add, Select, Clear, Contains
	//-----------------------------------------------------------------------
	void Remove(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Input:	pRow - The CXTPReportRow pointer to search for in the CXTPReportSelectedRows. 
	// Summary: Returns a value indicating whether the CXTPReportSelectedRows
	//			contains the specified CXTPReportRow pointer.
	// Remarks: Use this method to determine whether the CXTPReportSelectedRows
	//			contains the specified CXTPReportRow pointer.
	// Returns: TRUE if row is contained in the selection, FALSE otherwise.
	// Example: See example at CXTPReportSelectedRows overview
	// See also: CXTPReportSelectedRows overview, Add, Remove, Select, Clear
	//-----------------------------------------------------------------------
	BOOL Contains(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Input:	pRow - Pointer to the specified row.
	// Summary: Inverts selection for the specified row.
	// Remarks: This methods checks the specified method for its presence in
	//			the collection and adds or removes it in the reverse order
	//			depending on the result.
	// Example:
	//			CXTPReportSelectedRows* pSelRows = pReportControl->GetSelectedRows();
	//			ASSERT( TRUE == pSelRows->Contains(pRow1) );
	//			pSelRows->Invert(pRow1);
	//			ASSERT( FALSE == pSelRows->Contains(pRow1) );
	//			pSelRows->Invert(pRow1);
	//			ASSERT( TRUE == pSelRows->Contains(pRow1) );
	// See also: Add, Remove, Contains
	//-----------------------------------------------------------------------
	void Invert(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Input:	pRow - Pointer to the specified row.
	// Summary: Selects only the specified row.
	// Remarks: This method clears the initial selection and
	//			selects only the specified row. 
	// Example: See example at CXTPReportSelectedRows overview
	// See also: CXTPReportSelectedRows overview, Add, Remove, Contains, Clear
	//-----------------------------------------------------------------------
	void Select(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary: Gets the number of selected rows in the collection.
	// Remarks: Call this method to retrieve the number of selected rows 
	//			in the array. 
	// Returns: The number of items in the collection.
	// See also: CXTPReportRows overview
	//-----------------------------------------------------------------------
	int GetCount();


protected:
	CMap<CXTPReportRow*, CXTPReportRow*, BOOL, BOOL> m_mapRows;	// Internal selection mapping containing row items.
	CXTPReportControl* m_pControl;		// Pointer to the parent report control.
	CXTPReportRow* m_pRowBlockBegin;	// Pointer to the row where rows block begin from.



#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportSelectedRows);

	afx_msg LPUNKNOWN OleNewEnum();	
	afx_msg void OleAdd(LPDISPATCH lpColumn);

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
			POSITION m_posCurrent;
	END_INTERFACE_PART(EnumVARIANT)

	enum {
		dispidCount = 1L,
	};
	
#endif

};

AFX_INLINE CXTPReportRow* CXTPReportRows::GetAt(int nIndex) {
	return nIndex < GetCount()? m_arrRows.GetAt(nIndex): NULL;
}
#endif