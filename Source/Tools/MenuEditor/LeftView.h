// LeftView.h : CLeftView 類別的介面
//


#pragma once

class CMenuEditorDoc;

class CLeftView : public CTreeView
{
protected: // 僅從序列化建立
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// 屬性
public:
	CMenuEditorDoc* GetDocument();

// 作業
public:

// 覆寫
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // 建構後第一次呼叫

// 程式碼實作
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // LeftView.cpp 中的偵錯版本
inline CMenuEditorDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CMenuEditorDoc*>(m_pDocument); }
#endif

