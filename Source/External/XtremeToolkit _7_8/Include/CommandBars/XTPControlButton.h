// XTPControlButton.h : interface for the CXTPControlButton class.
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
#if !defined(__XTPCONTROLBUTTON_H__)
#define __XTPCONTROLBUTTON_H__			

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore


#include "XTPControl.h"

// Summary: CXTPControlButton is a CXTPControl derived class. 
//			It represents a button control.
class _XTP_EXT_CLASS CXTPControlButton : public CXTPControl
{
public:

	// Summary: Constructs a CXTPControlButton object
	CXTPControlButton();
	
	// Summary: Destroys a CXTPControlButton object, handles cleanup and de-allocation
	virtual ~CXTPControlButton();


protected:

	// Input:	bKeyboard - TRUE if the control is selected using the keyboard.
	//          pt - Mouse cursor position.
	// Summary: This method is called when the user clicks the control.
	virtual void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the user releases the left mouse button.
	virtual void OnLButtonUp(CPoint point);
	
	// Input:	pControl - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	virtual void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);
	
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Call this method to either read or write this object from or to an archive.
	virtual void Serialize(CArchive& archive);




private:
	

//:Ignore

#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPControlButton)


	enum {
		dispidStyle = 50L,
	};
	
#endif

	DECLARE_XTP_CONTROL(CXTPControlButton)

//:End Ignore
};

//////////////////////////////////////////////////////////////////////////	


#endif // #if !defined(__XTPCONTROLBUTTON_H__)
