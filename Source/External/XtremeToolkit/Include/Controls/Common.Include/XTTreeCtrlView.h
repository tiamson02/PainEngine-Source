// XTTreeView.h interface for the CXTTreeView class.
//
// This file is a part of the XTREME TOOLKIT MFC class library.
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
#if !defined(__XTTREEVIEW_H__)
#define __XTTREEVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

#include "XTTreeBase.h"

DECLATE_TREE_BASE(CXTTreeViewBase, CTreeView, CXTTreeBase);
DECLATE_TREE_BASE(CXTTreeCtrlBase, CTreeCtrl, CXTTreeBase);

//===========================================================================
// Remarks: CXTTreeView is a CTreeView derived class. It extends the CTreeView
//			class to add additional functionality, including tree traversal, searching,
//			color, and settings.
//===========================================================================
class _XT_EXT_CLASS CXTTreeView : public CXTTreeViewBase
{
	DECLARE_DYNCREATE(CXTTreeView)

protected:

    //-----------------------------------------------------------------------
	// Summary:	Protected constructor used by dynamic creation. Constructs a CXTTreeView
	//          object
	//-----------------------------------------------------------------------
	CXTTreeView();

	//-----------------------------------------------------------------------
    // Summary:	Destroys a CXTTreeView object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTTreeView();

	DECLARE_MESSAGE_MAP()
};

//===========================================================================
// Remarks: CXTTreeCtrl is a CTreeCtrl derived class. It extends the CTreeCtrl
//			class to add additional functionality, including tree traversal, searching,
//			color, and settings.
//===========================================================================
class _XT_EXT_CLASS CXTTreeCtrl : public CXTTreeCtrlBase
{
    DECLARE_DYNAMIC(CXTTreeCtrl)

public:

    //-----------------------------------------------------------------------
	// Summary:	Constructs a CXTTreeCtrl object
	//-----------------------------------------------------------------------
    CXTTreeCtrl();

	//-----------------------------------------------------------------------
    // Summary:	Destroys a CXTTreeCtrl object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
    virtual ~CXTTreeCtrl();

protected:

	DECLARE_MESSAGE_MAP()
};


//:Ignore
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTTREEVIEW_H__)
//:End Ignore