//////////////////////////////////////////////////////////////////////

#if !defined(__WATCHBAR_H__)
#define __WATCHBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CWatchBar dock window class

#include "TLFrame.h"

class CWatchBar : public CXTFlatTabCtrl
{
	//DECLARE_DYNAMIC(CWatchBar)

// Construction / destruction
public:
	//CWatchBar();
	//virtual ~CWatchBar();

// Attributes
protected:
	CTLFrame m_sheet1;
	CTLFrame m_sheet2;
	CTLFrame m_sheet3;
	CTLFrame m_sheet4;
	//CFont    m_sheetFont;

// Operations
public:
	BOOL CreateTabListCtrl(CTLFrame& listCtrl, UINT nID);

    HTREEITEM AddWatchVar(CTLFrame& listCtrl, LPCTSTR pszName, LPCTSTR pszVal,HTREEITEM hParent=TVI_ROOT);


protected:
	//{{AFX_MSG(CWatchBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTabSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__WATCHBAR_H__)

