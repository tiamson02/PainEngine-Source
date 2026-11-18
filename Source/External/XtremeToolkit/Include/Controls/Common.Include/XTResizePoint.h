// XTResizePoint.h: interface for the CXTResizePoint class.
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
#if !defined(__XTRESIZEPOINT_H__)
#define __XTRESIZEPOINT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//===========================================================================
// Remarks: XT_RESIZEPOINT structure is a stand alone structure class. It defines
//			the x- and y-coordinates of a point.
//===========================================================================
struct XT_RESIZEPOINT
{
	XT_RESIZE x; // Specifies the x-coordinate of a point
	XT_RESIZE y; // Specifies the y-coordinate of a point
};

//===========================================================================
// Remarks: CXTResizePoint is an XT_RESIZEPOINT structure derived class. The 
//			CXTResizePoint class is similar to the XT_RESIZEPOINT structure.
//			It also includes member functions to manipulate CXTResizePoint and 
//			XT_RESIZEPOINT structures. A CXTResizePoint object can be used wherever
//			an XT_RESIZEPOINT structure is used.
//===========================================================================
class _XT_EXT_CLASS CXTResizePoint : public XT_RESIZEPOINT
{
public:

    //-----------------------------------------------------------------------
	// Summary: Constructs a CXTResizePoint object
	//-----------------------------------------------------------------------
	CXTResizePoint();

	//-----------------------------------------------------------------------
	// Input:	cx - Specifies the x-coordinate of a point.
	//			cy - Specifies the y-coordinate of a point.
    // Summary:	Constructs a CXTResizePoint object
	//-----------------------------------------------------------------------
	CXTResizePoint(XT_RESIZE cx,XT_RESIZE cy);
};
//:Associate with "CXTResize"

/////////////////////////////////////////////////////////////////////////////

#define SZ_TOP_LEFT      CXTResizePoint( 0, 0)  // Specifies a top left position.
#define SZ_TOP_CENTER    CXTResizePoint(.5, 0)  // Specifies a top center position.
#define SZ_TOP_RIGHT     CXTResizePoint( 1, 0)  // Specifies a top right position.
#define SZ_MIDDLE_LEFT   CXTResizePoint( 0,.5)  // Specifies a middle left position.
#define SZ_MIDDLE_CENTER CXTResizePoint(.5,.5)  // Specifies a middle center position.
#define SZ_MIDDLE_RIGHT  CXTResizePoint( 1,.5)  // Specifies a middle right position.
#define SZ_BOTTOM_LEFT   CXTResizePoint( 0, 1)  // Specifies a bottom left position.
#define SZ_BOTTOM_CENTER CXTResizePoint(.5, 1)  // Specifies a bottom center position.
#define SZ_BOTTOM_RIGHT  CXTResizePoint( 1, 1)  // Specifies a bottom right position.

//:Ignore
//////////////////////////////////////////////////////////////////////

#endif // !defined(__XTRESIZEPOINT_H__)
//:End Ignore