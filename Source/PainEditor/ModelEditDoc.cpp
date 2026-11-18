//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ModelEditDoc.h"
#include "MainFrm.h"
#include "Engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelEditDoc

IMPLEMENT_DYNCREATE(CModelEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CModelEditDoc, CDocument)
	//{{AFX_MSG_MAP(CModelEditDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelEditDoc construction/destruction

CModelEditDoc::CModelEditDoc()
{
	// TODO: add one-time construction code here

}

CModelEditDoc::~CModelEditDoc()
{
}

BOOL CModelEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CModelEditDoc serialization

void CModelEditDoc::Serialize(CArchive& ar)
{
	// CEditView contains an edit control which handles all serialization
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CModelEditDoc diagnostics

#ifdef _DEBUG
void CModelEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CModelEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CModelEditDoc commands

BOOL CModelEditDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	//if (!CDocument::OnOpenDocument(lpszPathName))
	//	return FALSE;
	
//	m_Model.Load(lpszPathName);
	m_FileName = lpszPathName;

	return TRUE;
}


IMPLEMENT_DYNCREATE(CModelEditView, CView)
BEGIN_MESSAGE_MAP(CModelEditView, CView)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelEditView construction/destruction

CModelEditView::CModelEditView()
{
	// TODO: add construction code here

}

CModelEditView::~CModelEditView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CModelEditView drawing

void CModelEditView::OnDraw(CDC* pDC)
{
	CModelEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CModelEditView diagnostics

#ifdef _DEBUG
void CModelEditView::AssertValid() const
{
	CView::AssertValid();
}

void CModelEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CModelEditDoc* CModelEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CModelEditDoc)));
	return (CModelEditDoc*)m_pDocument;
}
#endif //_DEBUG


void CModelEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CString model = GetDocument()->m_FileName;
	int i = model.Find(AnimatedMesh::DefaultExt);
	model = model.Left(i);

	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();

	FileManagerStd fm;
	DynamicArray<String> files;
	fm.FindFiles(model+".*.ani",files,EFindFiles);

	pWnd->m_wndModel.m_InnerDlg.m_lstAnimations.ResetContent();
	for (int i=0;i<files.Size();i++)
		pWnd->m_wndModel.m_InnerDlg.m_lstAnimations.AddString(files[i].AsChar());

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
