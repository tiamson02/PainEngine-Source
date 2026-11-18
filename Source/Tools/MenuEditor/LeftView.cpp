// LeftView.cpp : CLeftView 類別的實作
//

#include "stdafx.h"
#include "MenuEditor.h"

#include "MenuEditorDoc.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
END_MESSAGE_MAP()


// CLeftView 建構/解構

CLeftView::CLeftView()
{
	// TODO: 在此加入建構程式碼
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs 達到修改視窗類別或樣式的目的

	return CTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: 您可以透過對 GetTreeCtrl() 的呼叫，直接存取其樹狀控制項來使用項目填入 TreeView。
}


// CLeftView 診斷

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CMenuEditorDoc* CLeftView::GetDocument() // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMenuEditorDoc)));
	return (CMenuEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView 訊息處理常式
