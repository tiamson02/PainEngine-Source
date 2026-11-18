// ProfilerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PainEditor.h"
#include "ProfilerDlg.h"
#include ".\profilerdlg.h"

#include "Core/SimpleProfiler.h"
extern ENGINE_API SimpleProfiler GProfiler;

#include "Engine.h"

#include "Engine/PCFSystem.h"
extern ENGINE_API PCFSystem* GEngine;

#include "Scripting/Scripting.h"
extern ENGINE_API Script	GScript;

#include "Net/NetworkDevice2.h"

// CProfilerDlg dialog

IMPLEMENT_DYNAMIC(CProfilerDlg, CDialog)
CProfilerDlg::CProfilerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProfilerDlg::IDD, pParent)
    , m_txtStats(_T(""))
{
}

CProfilerDlg::~CProfilerDlg()
{
}

void CProfilerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TEXT, m_txtStats);
}


BEGIN_MESSAGE_MAP(CProfilerDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CProfilerDlg message handlers

void CProfilerDlg::OnTimer(UINT nIDEvent)
{
    m_txtStats = GProfiler.GetTextInfo().AsChar();

    if( GEngine && GEngine->Net )
    {
        ulong32 sent, received;
        GEngine->Net->GetLastUpdateStats( sent, received );

        String sentBytes( 'O', sent/1000 );
        String recvBytes( 'O', received/1000 );
        m_txtStats += String::Sprintf( "Bytes sent: %d %s\n", sent, sentBytes ).AsChar();
        m_txtStats += String::Sprintf( "Bytes received: %d %s\n", received, recvBytes ).AsChar();
    }

    UpdateData(false);
    CDialog::OnTimer(nIDEvent);
}
