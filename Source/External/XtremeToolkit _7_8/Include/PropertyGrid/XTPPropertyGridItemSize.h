// XTPPropertyGridItemSize.h interface for the CXTPPropertyGridItemSize class.
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
#if !defined(__XTPPROPERTYGRIDITEMSIZE_H__)
#define __XTPPROPERTYGRIDITEMSIZE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//////////////////////////////////////////////////////////////////////
// Summary: CXTPPropertyGridItemSize is a CXTPPropertyGridItem derived class.  
//			It is used to create a size value item in a Property Grid control.
class _XTP_EXT_CLASS CXTPPropertyGridItemSize : public CXTPPropertyGridItem
{
public:

	// Input:	strCaption - Caption of the item.
	//			size - Initial value.
	// Summary: Constructs a CXTPPropertyGridItemSize object
	CXTPPropertyGridItemSize(CString strCaption, CSize size = 0, CSize* pBindSize = NULL);

	// Input:	nID - Identifier of the item.
	// Summary: Constructs a CXTPPropertyGridItemSize object
	CXTPPropertyGridItemSize(UINT nID, CSize size = 0, CSize* pBindSize = NULL);

	// Summary: Destroys a CXTPPropertyGridItemSize object
	virtual ~CXTPPropertyGridItemSize(void);

public:

	// Input:	size - The new size value of the item.
	// Summary: Call this method to change the item's value.
	void SetSize(CSize size);

	// Returns: A CSize object representing the size value of the item.
	// Summary: Call this method to get the size value of the item.
	CSize GetSize();

	// Input:	pBindSize - Points to a CSize object.
	// Summary: Call this method to bind an item to a CSize object.
	void BindToSize(CSize* pBindSize);

private:

	void SetWidth(CString strWidth);
	void SetHeight(CString strHeight);

	CString SizeToString(CSize size);
	CSize StringToSize(CString str);

	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	
	void UpdateChilds();

private:
	
	class CXTPPropertyGridItemSizeWidth;
	class CXTPPropertyGridItemSizeHeight;

    CSize                          m_szValue;
    CXTPPropertyGridItemSizeWidth*  m_itemWidth;
    CXTPPropertyGridItemSizeHeight* m_itemHeight;

	CSize* m_pBindSize;

private:

	DECLARE_DYNAMIC(CXTPPropertyGridItemSize)

	friend class CXTPPropertyGridItemSizeWidth;
	friend class CXTPPropertyGridItemSizeHeight;

};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CSize CXTPPropertyGridItemSize::GetSize() {
	return m_szValue;
}


#endif // #if !defined(__XTPPROPERTYGRIDITEMSIZE_H__)
