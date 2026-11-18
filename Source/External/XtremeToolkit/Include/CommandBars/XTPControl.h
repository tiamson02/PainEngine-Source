// XTPControl.h : interface for the CXTPControl class.
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
#if !defined(__XTPCONTROL_H__)
#define __XTPCONTROL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

#include "XTPCommandBarsDefines.h"
#include "CommonPro.include/XTPManageState.h"

class CXTPControls;
class CXTPCommandBar;
class CXTPImageManager;
class CXTPImageManagerIcon;
class CXTPPaintManager;

//:Ignore
class CXTPDocTemplateMap : public CMap<UINT, UINT, BOOL, BOOL>
{
public:
	void Copy(CXTPDocTemplateMap& map);

};
//:End Ignore

//===========================================================================
// Remarks: CXTPControl is a CCmdTarget derived class. It represents the parent
//			class for the command bar's controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPControl : public CCmdTarget
{

protected:
	//-----------------------------------------------------------------------
	// Summary: Constructs a CXTPControl object
	//-----------------------------------------------------------------------
	CXTPControl();   
	
	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPControl object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPControl();

public:
	//-----------------------------------------------------------------------
	// Input:	buttonStyle - The style to be set.
	// Summary: Call this member to set the style of the control.
	//-----------------------------------------------------------------------
	void SetStyle(XTPButtonStyle buttonStyle);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's style.	
	// Returns: The style of the control.
	//-----------------------------------------------------------------------
	XTPButtonStyle GetStyle();	

public:
	
	//-----------------------------------------------------------------------
	// Input:	strCaption - Caption to be set.
	// Summary: Call this member to set the caption of the control.
	//-----------------------------------------------------------------------
	void SetCaption(CString strCaption);

	//-----------------------------------------------------------------------
	// Input:	nIDCaption - Caption resource identifier.
	// Summary: Call this member to set the caption of the control.
	//-----------------------------------------------------------------------
	void SetCaption(UINT nIDCaption);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's caption.
	// Returns: The caption of the control.
	//-----------------------------------------------------------------------
	CString GetCaption();

	//-----------------------------------------------------------------------
	// Input:	strShortcutText - Shortcut to be set.
	// Summary: Call this member to set the shortcut string to the control.
	//-----------------------------------------------------------------------
	void SetShortcutText(CString strShortcutText);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's shortcut.
	// Returns: The shortcut of the control.
	//-----------------------------------------------------------------------
	CString GetShortcutText();

	//-----------------------------------------------------------------------
	// Input:	strDescription - Description to be set.
	// Summary: Call this member to set the description of the control.
	//-----------------------------------------------------------------------
	void SetDescription(CString strDescription);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's description.
	// Returns: The description of the control.
	//-----------------------------------------------------------------------
	CString GetDescription();

	//-----------------------------------------------------------------------
	// Input:	strTooltip - Tooltip to be set.
	// Summary: Call this member to set the tooltip of the control.
	//-----------------------------------------------------------------------
	void SetTooltip(CString strTooltip);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's tooltip.
	// Returns: Tooltip of the control.
	//-----------------------------------------------------------------------
	virtual CString  GetTooltip(LPPOINT pPoint = 0, LPRECT lpRectTip = 0, int* nHit = 0);
	
	//-----------------------------------------------------------------------
	// Input:	strParameter - Parameter to be set.
	// Summary: Call this member to set the parameter of the control.
	//-----------------------------------------------------------------------
	void SetParameter(CString strParameter);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's parameter.
	// Returns: The parameter of the control.
	//-----------------------------------------------------------------------
	CString GetParameter();

	//-----------------------------------------------------------------------
	// Input:	nId - Identifier to be set.
	// Summary: Call this member to set the identifier of the control.
	//-----------------------------------------------------------------------
	void SetID(int nId);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's identifier.
	// Returns: The identifier of the control.
	//-----------------------------------------------------------------------
	int GetID();

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the index of the control.
	// Returns: The index of the control.
	//-----------------------------------------------------------------------
	long GetIndex();	

	//-----------------------------------------------------------------------
	// Input:	nId - Icon's identifier to be set.
	// Summary: Call this member to set the control icon's identifier.
	//-----------------------------------------------------------------------
	void SetIconId(int nId);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the icon's identifier.
	// Returns: An icon's identifier of the control.
	//-----------------------------------------------------------------------
	int GetIconId();

	//-----------------------------------------------------------------------
	// Input:	bBeginGroup - TRUE if the control starts a new group.
	// Summary: Call this member to begin a new group using a separator.
	//-----------------------------------------------------------------------
	virtual void SetBeginGroup(BOOL bBeginGroup);

	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if the control has a separator.
	// Returns: TRUE if the control starts a new group; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetBeginGroup();

	//-----------------------------------------------------------------------
	// Input:	 bEnabled - TRUE if the control is enabled.  
	// Summary:  Call this member to enable or disable the control.
	//			 If the control does not have flags xtpFlagManualUpdate, you must call
	//			 the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// See Also: GetEnabled, SetChecked
	//-----------------------------------------------------------------------
	virtual void SetEnabled(BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the state of the control.
	// Returns: TRUE if the control is enabled; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetEnabled();

	//-----------------------------------------------------------------------
	// Input:	 bChecked - TRUE if the control is checked.  
	// Summary:  Call this member to check the control.
	//			 If the control does not have flags xtpFlagManualUpdate, you must call
	//			 the SetCheck member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// See Also: GetChecked, SetEnabled
	//-----------------------------------------------------------------------
	void SetChecked(BOOL bChecked);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the state of the control.
	// Returns: TRUE if the control is checked; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetChecked();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to get the state of the control.
	// Returns: TRUE if the control is selected; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual int GetSelected();

	//-----------------------------------------------------------------------
	// Input:	bPressed - TRUE if the control is pressed.
	// Summary: Call this member to press the control.
	//-----------------------------------------------------------------------
	void SetPressed(BOOL bPressed);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the state of the control.
	// Returns: TRUE if the control is pressed; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetPressed();

	//-----------------------------------------------------------------------
	// Input:	rcControl - Bounding rectangle of the control.
	// Summary: Call this member to set the bounding rectangle of the control.
	//-----------------------------------------------------------------------
	virtual void SetRect(CRect rcControl);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the bounding rectangle of the control.
	// Returns: The bounding rectangle of the control.
	//-----------------------------------------------------------------------
	CRect GetRect();

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the parent command bar.
	// Returns: The parent command bar object.
	//-----------------------------------------------------------------------
	CXTPCommandBar* GetParent();
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if the control is visible.
	// Returns: TRUE if the control is visible.
	//-----------------------------------------------------------------------
	BOOL IsVisible();

	//-----------------------------------------------------------------------
	// Input:	bVisible - TRUE if the control is visible.
	// Summary: Call this member to change the state of the control.
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the parent controls collection.
	// Returns: A CXTPControls pointer to the parent controls.
	//-----------------------------------------------------------------------
	CXTPControls* GetControls();

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the control's type
	// Returns: The type of the control
	//-----------------------------------------------------------------------
	XTPControlType GetType();

	//-----------------------------------------------------------------------
	// Input:	 dwFlags - control flags.
	// Summary:  Call this member to set the flags of the control.
	// See Also: XTPControlFlags
	//-----------------------------------------------------------------------
	void SetFlags(DWORD dwFlags);	

	//-----------------------------------------------------------------------
	// Input:	dwTag - Contains the new value to associate with the command bar. 
	// Summary: This method sets the 32-bit value associated with the command bar.
	//-----------------------------------------------------------------------
	void SetTag(DWORD dwTag);
	
	//-----------------------------------------------------------------------
	// Summary: This method retrieves the application-supplied 32-bit value 
	//			associated with the command bar.
	// Returns: The 32-bit value associated with the command bar.
	//-----------------------------------------------------------------------
	DWORD GetTag();
	
	//-----------------------------------------------------------------------
	// Inputs:  pOther - The control need compare with.
	// Summary: Call this member to compare controls.
	// Returns: TRUE if the controls are identical.
	//-----------------------------------------------------------------------
	virtual BOOL Compare(CXTPControl* pOther);

	//-----------------------------------------------------------------------
	// Summary: Call this member to get the child command bar.
	// Returns: A pointer to the child command bar.
	//-----------------------------------------------------------------------
	virtual CXTPCommandBar* GetCommandBar();

	
	void SetCloseSubMenuOnClick(BOOL bCloseOnClick);

	BOOL GetCloseSubMenuOnClick();

	virtual BOOL IsLabel();

public:
	
	//----------------------------------------------------------------------
	// Input:	bHide - TRUE to set hide state.
	// Summary:	This method is called to hide the control.
	//----------------------------------------------------------------------
	void SetHideWrap(BOOL bHide);
	
	//----------------------------------------------------------------------
	// Input:	dwFlags - Reasons to hide.
	// Summary:	This method is called to hide the control.
	//----------------------------------------------------------------------
	virtual void SetHideFlags(DWORD dwFlags);
	
	//----------------------------------------------------------------------
	// Summary: Call this method to retrieve hidden flags.
	// Returns: Flags of the control visibility.
	//----------------------------------------------------------------------
	DWORD GetHideFlags();	
	
	//----------------------------------------------------------------------
	// Summary:	Call this method to retrieve the position of the control's row.
	// Returns: Bounding rectangle of the control's row.
	//----------------------------------------------------------------------
	CRect GetRawRect();
	
	//----------------------------------------------------------------------
	// Summary: Call this method to retrieve if the control is wrapped.
	// Returns:	TRUE if the control is wrapped; otherwise returns FALSE
	//----------------------------------------------------------------------
	BOOL GetWrap();

	//----------------------------------------------------------------------
	// Input:	rcRaw - Rectangle of the control's row.
	// Summary:	Call this method to set the row of the control.
	//----------------------------------------------------------------------
	void SetRawRect(CRect rcRaw);
	
	//----------------------------------------------------------------------
	// Input:	bWrap - TRUE to set wrap.
	// Summary: Call this member to set the wrap.
	//----------------------------------------------------------------------
	void SetWrap(BOOL bWrap);
	
	//----------------------------------------------------------------------
	// Summary: Call this member to retrieve if the control is expanded.
	// Returns: TRUE if the control is expanded; otherwise returns FALSE
	//----------------------------------------------------------------------
	BOOL GetExpanded();
	
	//----------------------------------------------------------------------
	// Summary: Call this member to determine if the mouse pointer is over 
	//			the control.
	// Returns: TRUE if the mouse cursor is over the control; otherwise returns 
	//			FALSE
	//----------------------------------------------------------------------
	BOOL IsCursorOver();

	//----------------------------------------------------------------------
	// Summary: Call this member to retrieve control special flags.
	// Returns: Controls special flags.
	//----------------------------------------------------------------------
	DWORD GetFlags();

	//----------------------------------------------------------------------
	// Summary: This method is called when the control is executed.
	//----------------------------------------------------------------------
	virtual void OnExecute();
	
	//----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context
	// Summary: This method is called to determine the size of the control.
	// Returns: Size of the control.
	//----------------------------------------------------------------------
	virtual CSize GetSize(CDC* pDC);
	
	//----------------------------------------------------------------------
	// Input:	pDC - Pointer to a valid device context.
	// Summary: This method is called to draw the control.
	//----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);
	
	//----------------------------------------------------------------------
	// Summary: This method is called before recalculating the parent command 
	//			bar size.
	//----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD /*dwMode*/) {};

	//----------------------------------------------------------------------
	// Summary: Call this method to redraw the parent command bar.
	//----------------------------------------------------------------------
	void DelayRedrawParent(); 
	
	//----------------------------------------------------------------------
	// Summary: Call this method to recalculate the parent command bar size.
	//----------------------------------------------------------------------
	void DelayLayoutParent();

	//----------------------------------------------------------------------
	// Input:	strCategory - Category of the control.
	// Summary: Call this method to set the category of the control.
	//----------------------------------------------------------------------
	void SetCategory(CString strCategory);
	
	//----------------------------------------------------------------------
	// Summary: Call this method to retrieve the category of the control.
	// Returns: Category of the control.
	//----------------------------------------------------------------------
	CString GetCategory();

	//----------------------------------------------------------------------
	// Summary: Call this method to retrieve the image manager.
	// Returns: A pointer to a CXTPImageManager object.
	//----------------------------------------------------------------------
	CXTPImageManager* GetImageManager();
	
	//----------------------------------------------------------------------
	// Summary: Call this method to retrieves the paint manager.
	// Returns: A pointer to a CXTPImageManager object.
	//----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager();
	
	//----------------------------------------------------------------------
	// Input:   nWidth - Width of the image to retrieve.
	// Summary: Call this method to retrieve the image of the control.
	// Returns: A pointer to a CXTPImageManagerIcon object.
	//----------------------------------------------------------------------
	virtual CXTPImageManagerIcon* GetImage(int nWidth = 16);

	//----------------------------------------------------------------------
	// Input:	nIDResource - document template identifier.
	// Summary: Call this method to assign a document template to the control. 
	//			Assigned control will be shown only when the child MDI window is activated
	//          with the assigned document template.
	//----------------------------------------------------------------------
	void AssignDocTemplate(UINT nIDResource);
	
	//----------------------------------------------------------------------
	// Input:	nIDResource - document template identifier. 
	// Summary: Call this method to exclude the document template from control. 
	//			Control will be shown only when the child MDI window is activated
	//          with the document template different from excluded.
	//----------------------------------------------------------------------
	void ExcludeDocTemplate(UINT nIDResource);

	 //----------------------------------------------------------------------
	// Summary: Call this method to retrieve if control is default menu item.
	// Returns: TRUE if control is default menu item.
	//----------------------------------------------------------------------
	BOOL IsItemDefault();
	
	//----------------------------------------------------------------------
	// Input:   bDefault - TRUE to set control defuault menu item.
	// Summary: Call this method to set the default menu item.
	//----------------------------------------------------------------------
	void SetItemDefault(BOOL bDefault);

	//----------------------------------------------------------------------
	// Summary: Call this method to determine if control is temporary. 
	// Returns: TRUE if control is temporary.
	//----------------------------------------------------------------------
	BOOL IsTemporary();

	void Reset();
	
	void SetPrompt(const CString& strPrompt);

	//----------------------------------------------------------------------
	// Summary: Call this member to retrieve CXTPControl from CCmdUI structure.
	//----------------------------------------------------------------------
	static CXTPControl* AFX_CDECL FromUI(CCmdUI* pCmdUI);

protected:
	
	//----------------------------------------------------------------------
	// Input:	nIndex - Index of the control.
	// Summary: This method is called to assign the order of the control in 
	//			the controls list.
	//----------------------------------------------------------------------
	void SetIndex(int nIndex) { m_nIndex = nIndex; }
	
	//----------------------------------------------------------------------
	// Input:	bExpanded - TRUE if the control is expanded.
	// Summary: This method is called to assign expanded state to the control.
	//----------------------------------------------------------------------
	void SetExpanded(BOOL bExpanded);

	//----------------------------------------------------------------------
	// Input:	nChar - Specifies the virtual key code of the given key.
	// Returns:	TRUE if key handled, otherwise returns FALSE
	// Summary:	This method is called when a non-system key is pressed.
	//----------------------------------------------------------------------
	virtual BOOL OnKeyDown(UINT /*nChar*/) { return TRUE; }
	
	//----------------------------------------------------------------------
	// Input:	archive - A CArchive object to serialize to or from. 
	// Summary: Reads or writes this object from or to an archive.
	//----------------------------------------------------------------------
	void Serialize( CArchive& archive );
	
	//----------------------------------------------------------------------
	// Input:	pParent - Points to a CXTPCommandBar object
	// Summary: This method is called to assign a parent command bar object.
	//----------------------------------------------------------------------
	virtual void SetParent(CXTPCommandBar* pParent) { m_pParent = pParent;}

	//----------------------------------------------------------------------
	// Summary:	This method is called to redraw the parent command bar.
	//----------------------------------------------------------------------
	void RedrawParent();

	//----------------------------------------------------------------------
	// Input:	point - Pointer to a CPoint object that specifies the screen 
	//			coordinates to be converted. 
	// Summary: The ScreenToClient function converts the screen coordinates 
	//			of a specified point on the screen 
	// to client-area coordinates. 
	//----------------------------------------------------------------------
	void ScreenToClient(CPoint* point);
	
	//----------------------------------------------------------------------
	// Input:	rcActiveRect - Rectangle of the control.
	// Summary: This helper method is called when the user clicks to the toolbar's 
	//			button.
	//----------------------------------------------------------------------
	void ClickToolBarButton(CRect rcActiveRect = CRect(0, 0, 0, 0));		
	
	//----------------------------------------------------------------------
	// Input:	pControl   - Points to a source CXTPControl object
	//			bRecursive - TRUE to copy recursively.
	// Summary: This method is called to copy the control.
	//----------------------------------------------------------------------
	virtual void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);
	
	//----------------------------------------------------------------------
	// Input:	bPopup - TRUE to set popup.
	// Summary: This method is called to popup the control.
	// Returns: TRUE if successful; otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnSetPopup(BOOL /*bPopup*/) { return FALSE; }
	
	//----------------------------------------------------------------------
	// Input:	bKeyboard - TRUE if the control is selected using the keyboard.
	//          pt        - Mouse cursor position.
	// Summary: This method is called when the user clicks the control.
	//----------------------------------------------------------------------
	virtual void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	
	//----------------------------------------------------------------------
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the user releases the left mouse button.
	//----------------------------------------------------------------------
	virtual void OnLButtonUp(CPoint /*point*/) {};
	
	//----------------------------------------------------------------------
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the user releases the right mouse button.
	//----------------------------------------------------------------------
	virtual void OnRButtonUp(CPoint /*point*/) {};

	//----------------------------------------------------------------------
	// Input:	point - Specifies the x- and y-coordinates of the cursor.
	// Summary: This member is called when the user double-clicks the left mouse button.
	// Returns: TRUE if successful; otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnLButtonDblClk(CPoint /*point*/);
	
	//----------------------------------------------------------------------
	// Input:	point - Specifies the x- and y-coordinate of the cursor.
	// Summary: This member is called when the mouse cursor moves.
	//----------------------------------------------------------------------
	virtual void OnMouseMove(CPoint /*point*/) {};
	
	//----------------------------------------------------------------------
	// Summary: Called after the mouse hovers over the control.
	//----------------------------------------------------------------------
	virtual void OnMouseHover();
	
	//----------------------------------------------------------------------
	// Input:	bSelected - TRUE if the control becomes selected.
	// Summary: This method is called when the control becomes selected.
	// Returns: TRUE if successful; otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnSetSelected(int bSelected);

	//----------------------------------------------------------------------
	// Summary: Call this method to determine if command bars is in customize 
	//			mode.
	// Returns: TRUE if command bars is in Customize mode; otherwise returns 
	//			FALSE
	//----------------------------------------------------------------------
	BOOL IsCustomizeMode();
	
	//----------------------------------------------------------------------
	// Summary: This member is called when the user starts to drag the control.
	//----------------------------------------------------------------------
	virtual void CustomizeStartDrag();	
	
	//----------------------------------------------------------------------
	// Input:	pDataObject - Points to a CXTPControl object
	//			point       - Mouse position.
	//			dropEffect  - DROPEFFECT enumerator.
	// Summary: This member is called when the user moves the mouse over the 
	//			control.
	//----------------------------------------------------------------------
	virtual void OnCustomizeDragOver(CXTPControl* pDataObject, CPoint point, DROPEFFECT& dropEffect);
	
	//----------------------------------------------------------------------
	// Input:	pCommandBar - Points to a CXTPCommandBar object
	//			point       - Mouse position.
	//			dropEffect  - DROPEFFECT enumerator.
	// Summary: This member checks if the user can drop the control.
	// Returns: TRUE if successful, otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);

	//----------------------------------------------------------------------
	// Summary: Call this member to set hide flags.
	//----------------------------------------------------------------------
	BOOL SetHideFlag(DWORD dwFlag, BOOL bSet);

private:
	void Copy(CXTPControl* pControl);

protected:

	DWORD m_nTag;					// Control tag.
	int m_nIndex;					// Index of the control.
	int m_nId;						// Identifier of the control.
	int m_nIconId;					// Identifier of the control's image.
	int m_nCustomIconId;			// Identifier of the control's custom image.
	BOOL m_bWrap;					// TRUE if the control is wrapped.
	BOOL m_bBeginGroup;				// TRUE if the control starts new group.
	BOOL m_bEnabled;				// TRUE if the control is enabled.
	BOOL m_bChecked;				// TRUE if the control is checked.
	BOOL m_bSelected;				// TRUE if the control is selected.
	BOOL m_bPressed;				// TRUE if the control is pushed.
	BOOL m_bTemporary;				// TRUE if the control is temporary.
	CRect m_rcRaw;					// Bounding rectangle of the control's row.
	CRect m_rcControl;				// Bounding rectangle of the control.
	DWORD m_dwHideFlags;			// Hidden flags.
	DWORD m_dwFlags;				// Flags of the control.
	CString m_strCaption;			// Caption of the control.
	CString m_strCustomCaption;		// User defined caption.
	CString m_strShortcutText;		// Shortcut text.
	CString m_strShortcutTextAuto;		// Shortcut text.
	CString m_strTooltipText;		// Tooltip text.
	CString m_strDescriptionText;	// Description text.
	CString m_strParameter;			// Parameter text.
	CXTPControls* m_pControls;		// Parent control collection
	XTPControlType m_controlType;	// Type of the control.
	CXTPCommandBar* m_pParent;		// The Parent command bar.
	BOOL m_bExpanded;				// TRUE if the control is expanded.

	CString m_strCategory;			// Category of the control.
	BOOL m_bDefaultItem;			// TRUE if the item is default popup item;

	CXTPDocTemplateMap m_mapDocTemplatesAssigned;  // Assigned templates.
	CXTPDocTemplateMap m_mapDocTemplatesExcluded;  // Excluded templates.
	
	int m_nSchema;					// Serialized schema.
	XTPButtonStyle m_buttonStyle;   // Button Style.

	BOOL m_bCloseSubMenuOnClick;

public:
	BOOL m_bDoubleGripper;			// TRUE if control has double gripper.
	
public:

//:Ignore	

#ifdef _XTP_ACTIVEX

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()	

	DECLARE_OLETYPELIB_EX(CXTPControl);

	afx_msg LPDISPATCH OleControls();
	afx_msg LPDISPATCH OleParent();
	afx_msg long OleGetIndex();
	afx_msg long OleGetType();
	afx_msg void OleDelete();
	afx_msg LPDISPATCH OleCopy(LPDISPATCH lpDispBar, const VARIANT& varBefore);

	afx_msg void OleSetFocus();
	afx_msg void OleExecute();

	enum {
		dispidControls = 1L,
		dispidParent = 2L,
		dispidBeginGroup = 3L,
		dispidCaption = 4L,
		dispidDescriptionText = 5L,
		dispidId = 6L,
		dispidIndex = 7L,
		dispidParameter = 8L,
		dispidShortcutText = 9L,
		dispidTooltipText = 10L,
		dispidType = 11,
		dispidVisible = 12L,
		dispidEnabled = 13L,
		dispidChecked = 14L,
		dispidIconId = 15L,
		dispidDelete = 16L,
		dispidDefaultItem = 17L,
		dispidFalgs = 18L,
		dispidCategory = 19L,
		dispidCopy = 20L,
		dispidSetFocus = 21L,
		dispidExecute = 22L,
	};
	
#endif

private:

	DECLARE_XTP_CONTROL(CXTPControl)

	friend class CXTPControls;				
	friend class CXTPCommandBar;			
	friend class CXTPToolBar;				
	friend class CXTPPopupBar;				
	friend class CXTPCustomizeSheet;		
	friend class CXTPCustomizeDropSource;	
//:End Ignore
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE long CXTPControl::GetIndex() { 
	return m_nIndex; 
}
AFX_INLINE void CXTPControl::SetShortcutText(CString strShortcutText) { 
	m_strShortcutText = strShortcutText; 
}
AFX_INLINE CString CXTPControl::GetShortcutText() { 
	return m_strShortcutText.IsEmpty()? m_strShortcutTextAuto: m_strShortcutText; 
}
AFX_INLINE void CXTPControl::SetDescription(CString strDescription) { 
	m_strDescriptionText = strDescription; 
}
AFX_INLINE CString CXTPControl::GetDescription() { 
	return m_strDescriptionText; 
}
AFX_INLINE CString CXTPControl::GetTooltip(LPPOINT /*pPoint = 0*/, LPRECT /*lpRectTip = 0*/, int* /*nHit = 0*/) { 
	return m_strTooltipText; 
}
AFX_INLINE void CXTPControl::SetTooltip(CString strTooltip) { 
	m_strTooltipText = strTooltip; 
}
AFX_INLINE CString CXTPControl::GetCaption() { 
	return m_strCustomCaption.IsEmpty()? m_strCaption: m_strCustomCaption; 
}
AFX_INLINE void CXTPControl::SetParameter(CString strParameter) { 
	m_strParameter = strParameter; 
}
AFX_INLINE CString CXTPControl::GetParameter() { 
	return m_strParameter; 
}
AFX_INLINE int CXTPControl::GetID () { 
	return m_nId; 
}
AFX_INLINE void CXTPControl::SetIconId(int nId) {
	if (m_nIconId != nId) {m_nIconId = nId; DelayRedrawParent();}
}
AFX_INLINE int CXTPControl::GetIconId() {
	return m_nCustomIconId != 0? m_nCustomIconId: m_nIconId <=0? m_nId: m_nIconId;
}
AFX_INLINE void CXTPControl::SetBeginGroup(BOOL bBeginGroup) { 
	m_bBeginGroup = bBeginGroup; DelayLayoutParent();
}
AFX_INLINE BOOL CXTPControl::GetBeginGroup() { 
	return m_bBeginGroup; 
}
AFX_INLINE BOOL CXTPControl::GetEnabled() { 
	return m_bEnabled;
}
AFX_INLINE void CXTPControl::SetEnabled(BOOL bEnabled) { 
	if (m_bEnabled != bEnabled) {m_bEnabled = bEnabled; DelayRedrawParent();}
}
AFX_INLINE BOOL CXTPControl::GetChecked() { 
	return m_bChecked; 
}
AFX_INLINE void CXTPControl::SetChecked(BOOL bChecked) { 
	if (m_bChecked != bChecked) {m_bChecked = bChecked; DelayRedrawParent();}
}
AFX_INLINE int CXTPControl::GetSelected() { 
	return m_bSelected; 
}
AFX_INLINE BOOL CXTPControl::GetPressed() { 
	return m_bPressed; 
}
AFX_INLINE void CXTPControl::SetPressed(BOOL bPressed) { 
	m_bPressed = bPressed; 
}
AFX_INLINE void CXTPControl::SetRect(CRect rcControl) { 
	m_rcControl = rcControl; 
}
AFX_INLINE CRect CXTPControl::GetRect() { 
	return m_rcControl; 
}
AFX_INLINE CXTPCommandBar* CXTPControl::GetParent() { 
	return m_pParent; 
}
AFX_INLINE BOOL CXTPControl::IsVisible() { 
	return m_dwHideFlags == xtpNoHide; 
}
AFX_INLINE void CXTPControl::SetVisible(BOOL bVisible) { 
	DWORD dwHideFlags = m_dwHideFlags;
	if (!bVisible) SetHideFlags(m_dwHideFlags | xtpHideGeneric); else SetHideFlags(m_dwHideFlags & ~xtpHideGeneric); 
	if (dwHideFlags != m_dwHideFlags) DelayLayoutParent();
}
AFX_INLINE BOOL CXTPControl::SetHideFlag(DWORD dwFlag, BOOL bHide)
{
	DWORD dwHideFlags = m_dwHideFlags;
	if (bHide) SetHideFlags (m_dwHideFlags | dwFlag); else SetHideFlags(m_dwHideFlags & ~dwFlag); 
	return dwHideFlags != m_dwHideFlags;	
}

AFX_INLINE void CXTPControl::SetHideWrap(BOOL bHide) { 
	SetHideFlag(xtpHideWrap, bHide); 
}
AFX_INLINE void CXTPControl::SetHideFlags(DWORD dwFlags) { 
	m_dwHideFlags = dwFlags;
}
AFX_INLINE DWORD CXTPControl::GetHideFlags() { 
	return m_dwHideFlags; 
}	
AFX_INLINE CRect CXTPControl::GetRawRect() { 
	return m_rcRaw;
}
AFX_INLINE void CXTPControl::SetRawRect(CRect rcRaw) { 
	m_rcRaw = rcRaw;
}
AFX_INLINE BOOL CXTPControl::GetWrap() { 
	return m_bWrap;
}
AFX_INLINE void CXTPControl::SetWrap(BOOL bWrap) { 
	m_bWrap = bWrap;
}
AFX_INLINE CXTPControls* CXTPControl::GetControls() { 
	return m_pControls;	
}

AFX_INLINE BOOL CXTPControl::IsTemporary() { 
	return m_bTemporary; 
}
AFX_INLINE XTPControlType CXTPControl::GetType() { 
	return m_controlType;
} 
AFX_INLINE BOOL CXTPControl::GetExpanded() {
	return m_bExpanded;
}
AFX_INLINE void CXTPControl::SetCategory(CString strCategory) { 
	m_strCategory = strCategory; 
}
AFX_INLINE CString CXTPControl::GetCategory() { 
	return m_strCategory; 
}
AFX_INLINE void CXTPControl::SetTag(DWORD dwTag) {
	m_nTag = dwTag;
}
AFX_INLINE DWORD CXTPControl::GetTag() {
	return m_nTag;	
}
AFX_INLINE void CXTPControl::AssignDocTemplate(UINT nIDResource) {
	m_mapDocTemplatesAssigned.SetAt(nIDResource, TRUE);
}
AFX_INLINE void CXTPControl::ExcludeDocTemplate(UINT nIDResource) {
	m_mapDocTemplatesExcluded.SetAt(nIDResource, TRUE);
}
AFX_INLINE void CXTPControl::OnClick(BOOL /*bKeyboard = FALSE*/, CPoint /*pt = CPoint(0, 0)*/) {
}
AFX_INLINE void CXTPControl::SetStyle(XTPButtonStyle buttonStyle) {
	m_buttonStyle = buttonStyle;
	DelayLayoutParent();
}
AFX_INLINE XTPButtonStyle CXTPControl::GetStyle() { 
	return m_buttonStyle; 
}
AFX_INLINE CXTPCommandBar* CXTPControl::GetCommandBar() {
	return NULL;
}
AFX_INLINE void CXTPControl::SetCloseSubMenuOnClick(BOOL bCloseOnClick) {
	m_bCloseSubMenuOnClick = bCloseOnClick;
}

AFX_INLINE BOOL CXTPControl::GetCloseSubMenuOnClick() {
	return m_bCloseSubMenuOnClick;
}
AFX_INLINE BOOL CXTPControl::IsLabel() {
	return FALSE;
}

#endif // #if !defined(__XTPCONTROL_H__)
