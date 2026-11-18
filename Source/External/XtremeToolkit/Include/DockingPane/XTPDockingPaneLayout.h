// XTPDockingPaneLayout.h : interface for the CXTPDockingPaneLayout class.
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
#if !defined(__XTPDOCKINGPANELAYOUT_H__)
#define __XTPDOCKINGPANELAYOUT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore


#if _MSC_VER < 1200 
#define _XTP_EXCLUDE_XML
#endif

#include "XTPDockingPaneDefines.h"

#ifndef  _XTP_EXCLUDE_XML
#pragma warning( disable : 4786) //identifier was truncated to '255' characters in the debug information
#import "CommonPro.Include/MsXml.Tlb" named_guids raw_interfaces_only
#endif



//forwards

class CXTPDockingPaneBase;
class CXTPDockingPaneSplitterContainer;
class CXTPDockingPaneBase;
class CXTPDockingPaneAutoHidePanel;
class CXTPDockingPaneManager;

//===========================================================================
// Remarks: CXTPDockingPaneLayout is a stand alone class. It is used to manipulate
//			a pane's layouts.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneLayout
{
public:

	//-----------------------------------------------------------------------
	// Input:	pManager - Points to a CXTPDockingPaneManager object
	// Summary: Constructs a CXTPDockingPaneLayout object.
	// Returns: A CXTPDockingPaneLayout object.
	//-----------------------------------------------------------------------
	CXTPDockingPaneLayout(CXTPDockingPaneManager* pManager);

	//-----------------------------------------------------------------------
	// Summary: Destroys a CXTPDockingPaneLayout object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPaneLayout();
	
	//-----------------------------------------------------------------------
	// Input:	pLayout - Source Layout.
	// Summary: Call this member function to copy one layout to another.
	//-----------------------------------------------------------------------
	void Copy(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Input:	strSection - Registry section.
	// Summary: Call this member function to save the layout to the registry.
	//-----------------------------------------------------------------------
	void Save(CString strSection);

	//-----------------------------------------------------------------------
	// Input:	strSection - Registry section.
	// Summary: Call this member function to load the layout from the registry.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL Load(CString strSection);

	//-----------------------------------------------------------------------
	// Summary: Retrieves list of layout's panes.
	// Returns: A list of the layout's panes
	//-----------------------------------------------------------------------
	CXTPDockingPaneInfoList& GetPaneList();

#ifndef  _XTP_EXCLUDE_XML

	//-----------------------------------------------------------------------
	// Input:	strFileName - The full path and file name.
	//			strSection  - Registry section.
	// Summary: Call this member function to save the layout to the file.
	//-----------------------------------------------------------------------
	void SaveToFile(CString strFileName, CString strSection);

	//-----------------------------------------------------------------------
	// Input:	strFileName - The full path and file name.
	//			strSection  - Registry section.
	// Summary: Call this member function to load the layout from the file.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL LoadFromFile(CString strFileName, CString strSection);

	//-----------------------------------------------------------------------
	// Input:	MSXML::IXMLDOMDocumentPtr     - XMLDocument pointer.
	//			MSXML::IXMLDOMNodePtr xmlNode - XML node to save.
	//			strSection					  - Registry section.
	// Summary: Call this member function to save the layout to the XML node.
	//-----------------------------------------------------------------------
	void SaveToNode(MSXML::IXMLDOMDocumentPtr, MSXML::IXMLDOMNodePtr xmlNode,	CString strSection);

	//-----------------------------------------------------------------------
	// Input:	MSXML::IXMLDOMDocumentPtr	  - XMLDocument pointer.
	//			MSXML::IXMLDOMNodePtr xmlNode - XML node to load.
	//			strSection					  - Registry section.
	// Summary: Call this member function to load the layout from the XML node.
	// Returns: TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL LoadFromNode(MSXML::IXMLDOMDocumentPtr, MSXML::IXMLDOMNodePtr xmlNode, CString strSection);
#endif

	//-----------------------------------------------------------------------
	// Summary: Call this member to determine if it is a user created layout.
	// Returns: TRUE if it is a user created layout.
	//-----------------------------------------------------------------------
	BOOL IsUserLayout();

public:

	//-----------------------------------------------------------------------
	// Input:	lpszSection - Section to write.
	//			lpszEntry   - Entry to write.
	//			nValue      - Value to write.
	// Summary: Call this member to write an integer value to registry or XML node.
	// Returns: TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL _WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	
	//-----------------------------------------------------------------------
	// Input:	lpszSection - Section to write.
	//			lpszEntry   - Entry to write.
	//			lpszValue   - String value to write.
	// Summary: Call this member to write string to registry or XML node.
	// Returns: TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL _WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	
	//-----------------------------------------------------------------------
	// Input:	lpszSection - Section needed to get.
	// Summary: Call this member to get HKEY of the specified section in registry.
	// Returns: HKEY of the section.
	//-----------------------------------------------------------------------
	HKEY _GetSectionKey(LPCTSTR lpszSection);
	
	//-----------------------------------------------------------------------
	// Input:	lpszSection - Section to read.
	//			lpszEntry   - Entry to read
	//			nDefault    - Default value.
	// Summary: Call this member to read integer value from registry or XML node.
	// Returns: Read value if successful; otherwise returns nDefault.
	//-----------------------------------------------------------------------
	UINT _GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
	
	//-----------------------------------------------------------------------
	// Input:	lpszSection - Section to read.
	//			lpszEntry   - Entry to read.
	//			lpszDefault - Default value.
	// Summary: Call this member to read string from registry or XML node.
	// Returns: Read value if successful; otherwise returns lpszDefault.
	//-----------------------------------------------------------------------
	CString _GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
	
	//-----------------------------------------------------------------------
	// Summary: Call this member to delete all brunch.
	//-----------------------------------------------------------------------
	void _DeleteBrunch();

//:Ignore
#ifndef  _XTP_EXCLUDE_XML
	MSXML::IXMLDOMNodePtr GetProfileNode(MSXML::IXMLDOMElementPtr xmlRootPtr, CString strSection);
	MSXML::IXMLDOMNodePtr GetEntryNode(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bCreate = TRUE);
#endif
//:End Ignore

private:	
	void Push(CXTPDockingPaneBase* pPane) {	m_lstStack.AddTail(pPane); }
	void HidePane(CXTPDockingPaneBase* pPane);
	void OnSizeParent(CFrameWnd* pParent, CRect rect, LPVOID lParam);
	void DestroyPane(CXTPDockingPane* pPane);

private:
	void Free();
	void _FreeEmptyPanes();
	XTPDockingPaneDirection _GetPaneDirection(CXTPDockingPaneBase* pPane);

	void _Save();
	BOOL _Load();

private:
	CXTPDockingPaneSplitterContainer* m_pTopContainer;
	CXTPDockingPaneBase* m_pClient;

	CXTPDockingPaneInfoList m_lstPanes;
	CXTPDockingPaneBaseList m_lstStack;

	XTP_DOCKINGPANE_INFO* FindPane(CXTPDockingPane* pPane);

	CXTPDockingPaneAutoHidePanel* m_wndPanels[4];

	HKEY m_hLayoutKey;
	BOOL m_bUserLayout;
	CXTPDockingPaneManager* m_pManager;

#ifndef _XTP_EXCLUDE_XML
	MSXML::IXMLDOMDocumentPtr m_xmlDocPtr;	
	MSXML::IXMLDOMElementPtr m_xmlProfilePtr;
#endif
	BOOL m_bRegistry;

//:Ignore
#ifdef _XTP_DOTNET
	// Variables required for saving/loading layout from a string.
public:
	CString m_state;
	LPSTR ptr;
	LPSTR ptrStart;
	BOOL bSaveToString;
#endif //_XTP_DOTNET
//:End Ignore

private:

	friend class CXTPDockingPaneManager;
	friend class CXTPDockingPaneBase;
	friend class CXTPDockingPane;	
	friend class CXTPDockingPaneAutoHidePanel;
	friend class CDockingPaneSite;
};

//////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPDockingPaneLayout::IsUserLayout() { 
	return m_bUserLayout; 
}

AFX_INLINE CXTPDockingPaneInfoList& CXTPDockingPaneLayout::GetPaneList() {
	return m_lstPanes;
}


#endif // #if !defined(__XTPDOCKINGPANELAYOUT_H__)
