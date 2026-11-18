// MenuEditorDoc.h : CMenuEditorDoc 類別的介面
//


#pragma once

class CMenuEditorDoc : public CDocument
{
protected: // 僅從序列化建立
	CMenuEditorDoc();
	DECLARE_DYNCREATE(CMenuEditorDoc)

// 屬性
public:

// 作業
public:

// 覆寫
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 程式碼實作
public:
	virtual ~CMenuEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 產生的訊息對應函式
protected:
	DECLARE_MESSAGE_MAP()
};


