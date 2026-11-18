// PainEditor.h : main header file for the PainEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "DedicatedServerDlg.h" 

///#include "LuaView.h"
//#include "LuaDoc.h"


// CPainEditorApp:
// See PainEditor.cpp for the implementation of this class
//

class CPainEditorApp : public CWinApp
{
public:

    CDedicatedServerDlg m_dlgDedicatedServer;
    bool Dedicated;
	CString CfgFileName;
	CPainEditorApp();


// Overrides
public:
	virtual BOOL InitInstance();
	void ExecuteProgram(LPCSTR  strPath, LPCSTR dir);
	CMultiDocTemplate* m_pScriptEditDocTemplate;
	CMultiDocTemplate* m_pModelEditDocTemplate;
	CDocument *FindDocument(CString title);

	//CLuaView* OpenScriptView(LPCSTR fname, int nLine=-1);
	void Tick();

// Implementation
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
};

extern CPainEditorApp theApp;