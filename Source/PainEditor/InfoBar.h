// CInfoBar : header file
/////////////////////////////////////////////////////////////////////////////

#pragma once
/////////////////////////////////////////////////////////////////////////////
// CInfoBar class

#include "EditLog.h"
#include "editlog_stream.h"

// Set the Edit-control as logger destination
// m_EditLogger.SetEditCtrl( m_ctrlEdit.m_hWnd );

class CInfoBar : public CXTTabCtrlBar
{
	DECLARE_DYNAMIC(CInfoBar)

// Construction / destruction
public:

	// Constructs a CInfoBar object.
	CInfoBar();

	// Destroys a CInfoBar object, handles cleanup and de-allocation.
	virtual ~CInfoBar();

// Member variables
protected:
	CXTListCtrl m_listCtrl;
	CXTEdit  m_editOutput;
	CXTEdit  m_editConsole;
    CString  m_Buffer;

	CEditLog			m_EditLogger;
	std::editstreambuf	m_EditStrBuf; 


	CImageList  m_ilTabImages;

// Member functions
public:
	void InitListControl();
	void OutputText(LPCSTR txt);

	//{{AFX_VIRTUAL(CInfoBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CInfoBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

