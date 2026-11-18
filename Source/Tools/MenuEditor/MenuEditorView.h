// MenuEditorView.h : CMenuEditorView 類別的介面
//


#pragma once


class CMenuEditorView : public CListView
{
protected: // 僅從序列化建立
	CMenuEditorView();
	DECLARE_DYNCREATE(CMenuEditorView)

// 屬性
public:
	CMenuEditorDoc* GetDocument() const;

// 作業
public:

// 覆寫
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 建構後第一次呼叫

// 程式碼實作
public:
	virtual ~CMenuEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // MenuEditorView.cpp 中的偵錯版本
inline CMenuEditorDoc* CMenuEditorView::GetDocument() const
   { return reinterpret_cast<CMenuEditorDoc*>(m_pDocument); }
#endif

