// XTPResourceManager.h: interface for the CXTPResourceManager class.
//
// This file is a part of the XTREME TOOLKIT MFC class library.
// ©1998-2004 Codejock Software, All Rights Reserved.
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

#if !defined(__XTPRESOURCEMANAGER_H__)
#define __XTPRESOURCEMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER < 1200 
#define _XTP_EXCLUDE_XML
#endif


class _XTP_EXT_CLASS CXTPResourceManager : public CNoTrackObject
{
public:
	CXTPResourceManager();

public:
	virtual BOOL LoadString(CString* pString, UINT nIDResource);
	virtual BOOL LoadMenu(CMenu* lpMenu, UINT nIDResource);
	virtual BOOL LoadToolBar(CToolBar* pToolBar, UINT nIDResource);
	virtual BOOL LoadBitmap(CBitmap* pBitmap, UINT nIDResource);	
	virtual LPCDLGTEMPLATE LoadDialogTemplate(LPCTSTR pszResource);

public:
	int ShowMessageBox(UINT nIDPrompt, UINT nType = MB_OK);
 
public:
	BOOL SetResourceManager(CXTPResourceManager* pResourceManager);	

protected:
	BOOL m_bValid;
	static CProcessLocal<CXTPResourceManager> m_managerInstance;

protected:	
	friend CXTPResourceManager* XTPResourceManager();
	friend class CProcessLocal<CXTPResourceManager>;
};	   


class _XTP_EXT_CLASS CXTPResourceManagerText : public CXTPResourceManager
{
public:
	CXTPResourceManagerText(CString strFileName);

public:
	virtual BOOL LoadString(CString* pString, UINT nIDResource);
	virtual BOOL LoadMenu(CMenu* lpMenu, UINT nIDResource);
	virtual LPCDLGTEMPLATE LoadDialogTemplate(LPCTSTR pszResource); 

protected:
	virtual BOOL LoadResource(CString* pString, UINT nIDResource, CString strResourceType);

protected:
	LPCDLGTEMPLATE CreateDialogTemplate(CString strTemplate);

protected:	
	CString m_strFileName;
};

#ifndef  _XTP_EXCLUDE_XML

class _XTP_EXT_CLASS CXTPResourceManagerXML : public CXTPResourceManagerText
{
public:
	CXTPResourceManagerXML(CString strFileName);
	~CXTPResourceManagerXML();

protected:
	virtual BOOL LoadResource(CString* pString, UINT nIDResource, CString strResourceType);	

};

#endif

AFX_INLINE CXTPResourceManager* XTPResourceManager() {
	return CXTPResourceManager::m_managerInstance.GetData();
}


#endif // !defined(__XTPRESOURCEMANAGER_H__)
