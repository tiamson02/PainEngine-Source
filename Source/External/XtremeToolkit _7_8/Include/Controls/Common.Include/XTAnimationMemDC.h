// XTAnimationMemDC.h : header file
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
#if !defined(__XTANIMATIONMEMDC_H__)
#define __XTANIMATIONMEMDC_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

//---------------------------------------------------------------------------
// Summary: Enumerated type used to determine the animation effect
//---------------------------------------------------------------------------
enum ANIMATIONTYPE
{
	animateWindowsDefault,	// As defined in the "Display" settings.
	animateRandom,			// Any of the first three in random selection.
	animateUnfold,			// Unfold top to bottom.
	animateSlide,			// Slide in from left.
	animateFade,			// Fade-in.
	animateNone				// No animation.
};

//---------------------------------------------------------------------------
// Summary:  Function pointer used to define a custom animation effect.
// Input:    rc             - Area to display.
//           pDestDC        - Destination device context.
//           pSrcDC         - Source device context.
//           nType          - Enumerated ANIMATIONTYPE to determine animation
//                            effect.
//           nSteps         - Number of steps to complete animation.
//           nAnimationTime - Amount of time in milliseconds between each
//                            step.
// See Also: CXTAnimationMemDC::SetCustomAnimation, ANIMATIONTYPE
//---------------------------------------------------------------------------
typedef void (*xtAnimationProc)(CRect rc, CDC* pDestDC, CDC* pSrcDC, int nType, int nSteps, int nAnimationTime);

//===========================================================================
// Summary: CXTAnimationMemDC is a CXTMemDC derived class. This class is a
//          replacement for the commonly used XTMemDC. By calling Animate(),
//          the contained bitmap is drawn on the screen with a given effect. 
//          This effect is equal to the Menu effect in Windows&reg; and
//          Office&trade;.
//===========================================================================
class _XT_EXT_CLASS CXTAnimationMemDC : public CXTMemDC
{
public: 
	
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTAnimationMemDC object 
	// Input:	pDC      - A pointer to a CDC object.
	//			rect     - An address of a CRect object.
	//			clrColor - An RGB value that represents the current system 
	//                     face color of three dimensional display elements.
	//-----------------------------------------------------------------------
    CXTAnimationMemDC(CDC* pDC, const CRect& rect, COLORREF clrColor=GetSysColor(COLOR_3DFACE));

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTAnimationMemDC object, handles cleanup and de-
	//          allocation 
	//-----------------------------------------------------------------------
    virtual ~CXTAnimationMemDC();

	//-----------------------------------------------------------------------
	// Summary: This member function performs the animation 
	// Input:	nType          - Type of animation to perform.
	//			nSteps         - Number of steps to take during animation.
	//			nAnimationTime - Amount of time to rest, in milliseconds, 
	//                           between each step.
	//-----------------------------------------------------------------------
	void Animate(int nType = animateWindowsDefault, int nSteps = 10, int nAnimationTime = 1000);

	//-----------------------------------------------------------------------
	// Summary: This member implements default animation effects - Fade,
	//          Slide and Unfold.  You can add new animation effects to call
	//          SetCustomAnimation member.
	// Input:	rc             - Bounding rectangle.
	//			pDestDC        - Pointer to device context you must draw to.
	//			pSrcDC         - Device context that contains the bitmap you
	//                           must take.
	//			nType          - Type of animation to perform. For custom
	//                           animation you must use numbers greater than
	//                           6.
	//			nSteps         - Number of steps to take during animation.
	//			nAnimationTime - Amount of time to rest, in milliseconds, 
	//                           between each step.
	//-----------------------------------------------------------------------
	static void DefaultAnimation(CRect rc, CDC* pDestDC, CDC* pSrcDC, int nType, int nSteps, int nAnimationTime);

	//-----------------------------------------------------------------------
    // Summary:  Call this member function to setup new Animation effects. 
    //           You must call DefaultAnimation in your function.
    // Input:    pCustom - pointer to custom animation function.
    // Example:  <pre>
    //           int CMainFrame::OnCreate()
    //           {
	//               // our animation type
    //               CXTCoolMenu::m_nAnimationType = 10;
	//               // custom animation procedure.
    //               CXTAnimationMemDC::SetCustomAnimation(CustomAnimation);
    //               ...
    //           }
    //           ...
    //           void CMainFrame::CustomAnimation(CRect rc, CDC* pDestDC, 
	//               CDC* pSrcDC, int nType, int nSteps, int nAnimationTime)
    //           {
    //               if (nType == 10)
    //               {
    //                   // do custom animation
    //               } 
    //               else
    //               {
    //                   CXTAnimationMemDC::DefaultAnimation(rc, pDestDC,
	//                       pSrcDC, nType, nSteps, nAnimationTime);
    //               }   
    //           }
    //           </pre>
    // See Also: xtAnimationProc, ANIMATIONTYPE
	//-----------------------------------------------------------------------
	static void SetCustomAnimation(xtAnimationProc pCustom);

private:

	static xtAnimationProc m_pCustomAnimation; // Holds function pointer.

	// Summary: Private method used for alpha-blending.
	static void AlphaBlendU(PBYTE, PBYTE, int, int, PBYTE, BYTE);
};

//:Ignore
#endif // !defined(__XTANIMATIONMEMDC_H__)
//:End Ignore