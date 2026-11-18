// MenuEditorView.cpp : CMenuEditorView 類別的實作
//

#include "stdafx.h"
#include "MenuEditor.h"

#include "MenuEditorDoc.h"
#include "MenuEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMenuEditorView

IMPLEMENT_DYNCREATE(CMenuEditorView, CListView)

BEGIN_MESSAGE_MAP(CMenuEditorView, CListView)
	ON_WM_STYLECHANGED()
END_MESSAGE_MAP()

// CMenuEditorView 建構/解構

CMenuEditorView::CMenuEditorView()
{
	// TODO: 在此加入建構程式碼

}

CMenuEditorView::~CMenuEditorView()
{
}

BOOL CMenuEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 經由修改 CREATESTRUCT cs 達到修改視窗類別或樣式的目的

	return CListView::PreCreateWindow(cs);
}

void CMenuEditorView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: 您可以透過對 GetListCtrl() 的呼叫，直接存取其清單控制項，使用項目填入您的 ListView。
}


// CMenuEditorView 診斷

#ifdef _DEBUG
void CMenuEditorView::AssertValid() const
{
	CListView::AssertValid();
}

void CMenuEditorView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMenuEditorDoc* CMenuEditorView::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMenuEditorDoc)));
	return (CMenuEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMenuEditorView 訊息處理常式
void CMenuEditorView::OnStyleChanged(int /*nStyleType*/, LPSTYLESTRUCT /*lpStyleStruct*/)
{
	//TODO: 加入程式碼來反應至使用者變更您視窗的檢視樣式
	
	Default();
}
