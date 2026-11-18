// XTDropSource.h : header file
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
#ifndef __XTDROPSOURCE_H__
#define __XTDROPSOURCE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//:End Ignore

// NUMBERED LIST:

//===========================================================================
// Remarks: CXTDropSource class is derived from IDropSource. It is used by the
//			shell list control to provide drag-and-drop operations in your application. 
//
//			<p>It contains methods used in any application used as a data source in
//			a drag-and-drop operation. The data source application in a drag-and-drop
//			operation is responsible for: </p>
//			[ol]
//			[li]Determining the data being dragged based on the user's selection.[/li]
//			[li]Initiating the drag-and-drop operation based on the user's mouse actions. [/li]
//			[li]Generating some of the visual feedback during the drag-and-drop operation, such
//			as setting the cursor and highlighting the data selected for the
//			drag-and-drop operation.[/li]
//			[li]Canceling or completing the drag-and-drop operation based on the
//			user's mouse actions.[/li]
//			[li]Performing any action on the original data caused by the drop operation,
//			such as deleting the data on a drag move.[/li]
//			[/ol]
//			<p>CXTDropSource contains the methods for generating visual feedback to
//			the end user and for canceling or completing the drag-and-drop operation.
//			You also need to call the DoDragDrop, RegisterDragDrop, and RevokeDragDrop
//			functions in drag-and-drop operations.</p>
//===========================================================================
class _XT_EXT_CLASS CXTDropSource : public IDropSource
{
protected:
	
	UINT m_uiRefCount; // Indicates the current reference count.

public: 
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTDropSource object
	//-----------------------------------------------------------------------
	CXTDropSource();
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTDropSource object, handles clean up and de-allocation
	//-----------------------------------------------------------------------
	~CXTDropSource();

	//////////////////////////////////////////////////////////////////////
	// IUnknown Interface Members
	//////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	// Input:	riid     - Identifier of the interface being requested. 
	//			ppReturn - Address of the pointer variable that receives the 
	//					   interface pointer requested in 'riid'. Upon successful 
	//					   return, 'ppReturn' contains the requested interface 
	//					   pointer to the object. If the object does not support 
	//					   the interface specified in 'riid', 'ppReturn' is set 
	//					   to NULL. 
	// Summary:	The QueryInterface method gives a client access to other interfaces
	//			on an object. For any one object, a specific query for the IUnknown
	//			interface on any of the object's interfaces must always return the
	//			same pointer value. This allows a client to determine whether two pointers
	//			point to the same component by calling QueryInterface on both and comparing
	//			the results. It is specifically not the case that queries for interfaces
	//			(even the same interface through the same pointer) must return the
	//			same pointer value.
	// Returns: A pointer to an interface if successful, otherwise returns NULL.
	//-----------------------------------------------------------------------
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID* ppReturn);

	//-----------------------------------------------------------------------
	// Summary:	The AddRef method increments the reference count for an interface on
	//			an object. It should be called for every new copy of a pointer to
	//			an interface on a given object.
	// Returns: A ULONG value that represents the new reference count.  The return 
	//			value can be an integer from 1 to n, the value of the new reference 
	//			count. This information is meant to be used for diagnostic and 
	//			testing purposes only because, in certain situations, the value may 
	//			be unstable.
	//-----------------------------------------------------------------------
	STDMETHODIMP_(ULONG) AddRef(void);

	//-----------------------------------------------------------------------
	// Summary:	This method decrements the reference count for the calling interface
	//			on a object. If the reference count on the object falls to 0, the object
	//			is freed from memory. 
	// Returns: A ULONG value that represents the resulting value of the reference count,
	//			which is used for diagnostic and testing purposes only. 
	//-----------------------------------------------------------------------
	STDMETHODIMP_(ULONG) Release(void);

	//////////////////////////////////////////////////////////////////////
	// IDropSource Interface Members
	//////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------
	// BULLETED LIST:

	// Input:	bEsc	   - Specifies whether the ESC key has been pressed since 
	//						 the previous call to QueryContinueDrag, or to DoDragDrop 
	//						 if this is the first call to QueryContinueDrag. A TRUE 
	//						 value indicates that the end user has pressed the 
	//						 escape key. A FALSE value indicates it has not been pressed. 
	//			dwKeyState - Current state of the keyboard modifier keys on the keyboard. 
	//						 Valid values can be a combination of any of the flags 
	//						 MK_CONTROL, MK_SHIFT, MK_ALT, MK_BUTTON, MK_LBUTTON, 
	//						 MK_MBUTTON, and MK_RBUTTON. 
	// Summary:	This method determines whether a drag-and-drop operation should be
	//			continued, canceled, or completed. You do not call this method directly.
	//			The OLE DoDragDrop function calls this method during a drag-and-drop
	//			operation. This method supports the standard return values E_UNEXPECTED
	//			and E_OUTOFMEMORY, as well as the following:
	//			[ul]
	//			[li]<b>S_OK</b> Drag operation should continue. This result occurs if no
	//			errors are detected, the mouse button starting the drag-and-drop
	//			operation has not been released, and the ESC key has not been detected.[/li]
	//			[li]<b>DRAGDROP_S_DROP</b> Drop operation should occur completing the drag
	//			operation. This result occurs if DWORD indicates that the key that
	//			started the drag-and-drop operation has been released.[/li]
	//			[li]<b>DRAGDROP_S_CANCEL</b> Drag operation should be canceled with no drop
	//			operation occurring. This result occurs if BOOL is TRUE, indicating
	//			the ESC key has been pressed.[/li]
	//			[/ul]
	// Returns: An HRESULT value.
	//-----------------------------------------------------------------------
	STDMETHODIMP QueryContinueDrag(BOOL bEsc,DWORD dwKeyState);
	
	// BULLETED LIST:

	//-----------------------------------------------------------------------
	// Input:	DWORD - Effect of a drop operation.
	// Summary:	This method enables a source application to give visual feedback to
	//			the end user during a drag-and-drop operation by providing the DoDragDrop
	//			function with an enumeration value specifying the visual effect. This
	//			method supports the standard return values E_INVALIDARG, E_UNEXPECTED,
	//			and E_OUTOFMEMORY, as well as the following: 
	//			[ul]
	//			[li]<b>S_OK</b> Method completed its task successfully, using the cursor
	//			set by the source application.[/li]
	//			[li]<b>DRAGDROP_S_USEDEFAULTCURSORS</b> Successful completion of the method,
	//			and requests OLE to update the cursor using the OLE-provided default
	//			cursors.[/li]
	//			[/ul]
	// Returns: An HRESULT value.
	//-----------------------------------------------------------------------
	STDMETHODIMP GiveFeedback(DWORD);
};

//:Ignore
//////////////////////////////////////////////////////////////////////

#endif // __XTDROPSOURCE_H__
//:End Ignore