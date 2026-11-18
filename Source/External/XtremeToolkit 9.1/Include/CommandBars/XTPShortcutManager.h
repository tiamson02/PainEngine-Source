// XTPShortcutManager.h : interface for the CXTPShortcutManager class.
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
#if !defined(__XTPSHORTCUTMANAGER_H__)
#define __XTPSHORTCUTMANAGER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore

class CXTPCommandBars;

// Internally used class
//:Ignore

class _XTP_EXT_CLASS CXTPShortcutManager
{
public:

	class _XTP_EXT_CLASS CKeyHelper
	{ 
	public:

		CKeyHelper(LPACCEL lpAccel);

		virtual ~CKeyHelper();
		
	public:

		void Format (CString& str) const;
	
		int Priority();
		static BOOL EqualAccels(const ACCEL* pFirst, const ACCEL* pSecond)
		{
			return ((pFirst->fVirt | FNOINVERT) == (pSecond->fVirt | FNOINVERT)) && (pFirst->key == pSecond->key);
		}
		
	protected:

		void AddVirtKeyStr (CString& str, UINT uiVirtKey, BOOL bLast = FALSE) const;
		
	protected:
		LPACCEL m_lpAccel;		
	};

	class _XTP_EXT_CLASS CKeyAssign : public CEdit
	{
	public:

		CKeyAssign();
		
		virtual ~CKeyAssign();

	public:

		BOOL IsKeyDefined() const;
		ACCEL const* GetAccel() const;
		void SetAccel(ACCEL& hAccel);
		void ResetKey();

	// Attributes
	protected:
		BOOL		m_bKeyDefined;	
		ACCEL	    m_hAccel;		
		CKeyHelper	m_keyHelper;	

	// Operations
	protected:

		void SetAccelFlag (BYTE bFlag, BOOL bOn);
		virtual BOOL PreTranslateMessage(MSG* pMsg);
	};

	CXTPShortcutManager();

public:

	BOOL FindDefaultAccelerator (UINT nCmd, CString& strShortcut, CXTPCommandBars* pCommandBars = 0);	
	TCHAR ToUpper(TCHAR vkTCHAR);
	void SaveShortcuts(CFrameWnd* pFrame, LPCTSTR lpszProfileName);
	void LoadShortcuts(CFrameWnd* pFrame, LPCTSTR lpszProfileName);
	void UpdateAcellTable(CFrameWnd* pFrame, LPACCEL lpAccel, int nSize);
	void SerializeShortcuts(CFrameWnd* pFrame, CArchive& ar);
	HACCEL CopyAccelTable(HACCEL hAccelTable);
	void AddShortcut(CFrameWnd* pFrame, long fVirt, long key, long cmd);
	ACCEL * GetShortcut(CFrameWnd* pFrame, int ID, ACCEL * pAccel);

private:
	static CXTPShortcutManager s_managerInstance;

public:

	friend class CXTPShortcutManager* XTPShortcutManager();
};

//:End Ignore

AFX_INLINE CXTPShortcutManager* XTPShortcutManager() { 
	return &CXTPShortcutManager::s_managerInstance; 
}

#endif //#if !defined(__XTPSHORTCUTMANAGER_H__)