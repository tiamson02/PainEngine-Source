// MenuEditorDoc.cpp : CMenuEditorDoc 類別的實作
//

#include "stdafx.h"
#include "MenuEditor.h"

#include "MenuEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMenuEditorDoc

IMPLEMENT_DYNCREATE(CMenuEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMenuEditorDoc, CDocument)
END_MESSAGE_MAP()


// CMenuEditorDoc 建構/解構

CMenuEditorDoc::CMenuEditorDoc()
{
	// TODO: 在此加入一次建構程式碼

}

CMenuEditorDoc::~CMenuEditorDoc()
{
}

BOOL CMenuEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此加入重新初始化程式碼
	// (SDI 文件會重用此文件)

	return TRUE;
}




// CMenuEditorDoc 序列化

void CMenuEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此加入儲存程式碼
	}
	else
	{
		// TODO: 在此加入載入程式碼
	}
}


// CMenuEditorDoc 診斷

#ifdef _DEBUG
void CMenuEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMenuEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMenuEditorDoc 命令
