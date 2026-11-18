// CTabCtrlBar : header file
/////////////////////////////////////////////////////////////////////////////

#ifndef __TABCTRLBAR_H
#define __TABCTRLBAR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlBar class

class CTabCtrlBar : public CXTTabCtrlBar
{
	DECLARE_DYNAMIC(CTabCtrlBar)

// Construction / destruction
public:


// Member variables
protected:

// Member functions
public:

	//{{AFX_VIRTUAL(CTabCtrlBar)
	//}}AFX_VIRTUAL
	virtual void OnTabSelChange(int nIDCtrl, CXTTabCtrl* pTabCtrl);

	//{{AFX_MSG(CTabCtrlBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __TABCTRLBAR_H

