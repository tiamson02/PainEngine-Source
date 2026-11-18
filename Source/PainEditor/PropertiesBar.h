#if !defined(AFX_PropertiesBar_H__1CA9DB3D_2253_11D3_A634_00105A98E9A1__INCLUDED_)
#define AFX_PropertiesBar_H__1CA9DB3D_2253_11D3_A634_00105A98E9A1__INCLUDED_

#ifdef EDITOR

#pragma once

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

class CPropertiesBar : public 	CWnd	
{
protected: // create from serialization only
	
// Attributes
public:
	CPropertiesBar();
	CString m_ObjName;
	CXTPToolBar m_wndToolBar;
	CXTPPropertyGrid m_wndGrid;


// Operations
public:

	BOOL Create(CWnd* pParentWnd);
	void UpdateView(CString obj);
	CXTPPropertyGridItem* FillTree(ScriptObject obj, CString key, CString fullkey, CXTPPropertyGridItem* pParent, bool group);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesBar)
	protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertiesBar();
	LRESULT OnValueChanged(CXTPPropertyGridItem*pItem);

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPropertiesBar)
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnAfterChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnToolbarCommand(UINT nID);
	afx_msg LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PropertiesBar_H__1CA9DB3D_2253_11D3_A634_00105A98E9A1__INCLUDED_)
