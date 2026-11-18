// XTPReportRecords.h: interface for the CXTPReportRecords class.
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
#if !defined(__XTPREPORTRECORDS_H__)
#define __XTPREPORTRECORDS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

class CXTPReportRecord;

//===========================================================================
// Remarks: This class represents a records collection class. 
//			It supports an array of CXTPReportRecord pointers.
//
//			Array indexes start at position 0.
//
// Example: See example for CXTPReportRecords::Add method.
//
// See also: CXTPReportRecord
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecords : public CCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary: Constructs an empty CXTPReportRecord pointer array.
	// Example:	// Declare a local CXTPReportRecords object.
	//			CXTPReportRecords myList;
	//
	//			// Declare a dynamic CXTPReportRecords object.
	//			CXTPReportRecords* pTree = new CXTPReportRecords();
	// See also: ~CXTPReportRecords
	//-----------------------------------------------------------------------
	CXTPReportRecords();

	//-----------------------------------------------------------------------
	// Summary: Destroys CXTPReportRecords object. Performs cleanup operations.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportRecords(void);

	//-----------------------------------------------------------------------
	// Summary: Gets the number of CXTPReportRecord elements in this collection.
	// Remarks: Call this method to retrieve the number of elements in the array. 
	//			Because indexes are zero-based, the size is 1 greater than 
	//			the largest index.
	// Example: See example for CXTPReportRecords::Add method.
	// Returns: The number of items in the collection.
	// See also: CXTPReportRecords overview
	//-----------------------------------------------------------------------
	int GetCount();

	//-----------------------------------------------------------------------
	// Input:	nIndex - An integer index that is greater than or equal to 0 
	//					 and less than the value returned by GetCount. 
	// Summary: Returns a record at the specified numeric index.
	// Remarks: Returns the array element at the specified index.
	// Example: See example for CXTPReportRecords::Add method.
	// Returns: The record element currently at this index.
	//-----------------------------------------------------------------------
	CXTPReportRecord* GetAt(int nIndex);

	//-----------------------------------------------------------------------
	// Input:	pRecord - The record element to be added to this array.
	// Summary: Adds a new Record element to the end of an array.
	// Remarks: Use this method to add the specified CXTPReportRecord pointer 
	//			to the end of the CXTPReportRecords collection.
	// Example: CXTPReportRecords* pList = new CXTPReportRecords();
	//			pList->Add(new CXTPReportRecord());
	//			pList->Add(new CXTPReportRecord());
	//			CXTPReportRecord* pRecord0 = pList->GetAt(0);
	//			CXTPReportRecord* pRecord1 = pList->GetAt(1);
	//			ASSERT( 2 == pList->GetCount() );
	//			pList->RemoveAll();
	//			ASSERT( 0 == pList->GetCount() );
	// See also: CXTPReportRecords overview, GetAt, RemoveAll, GetCount
	//-----------------------------------------------------------------------
	void Add(CXTPReportRecord* pRecord);

	//-----------------------------------------------------------------------
	// Summary: Removes all the elements from this array.
	// Remarks: Removes all the pointers from this array and releases instances 
	//			of all stored CXTPReportRecord objects.
	// Example: See example for CXTPReportRecords::Add method.
	// See also: CXTPReportRecords overview
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Input:   nIndex - index of record to remove.
	// Summary: Removes record by its index.
	//-----------------------------------------------------------------------
	void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary: Call this method to add record in the specified position.
	//-----------------------------------------------------------------------
	void InsertAt(int nIndex, CXTPReportRecord* pRecord);

private:
	void UpdateIndexes(int nStart = 0);

protected:
	CArray<CXTPReportRecord*, CXTPReportRecord*> m_arrRecords;	// Internal storage array for Record items.


#ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPReportRecords);

	afx_msg LPDISPATCH OleItem(long nIndex);
	afx_msg LPDISPATCH OleAdd();
	afx_msg LPUNKNOWN OleNewEnum();	
	afx_msg LPDISPATCH OleInsert(int nIndex);

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

AFX_INLINE int CXTPReportRecords::GetCount() {
	return (int)m_arrRecords.GetSize();
}
AFX_INLINE CXTPReportRecord* CXTPReportRecords::GetAt(int nIndex) {
	return m_arrRecords.GetAt(nIndex);
}
#endif