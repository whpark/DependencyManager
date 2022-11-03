
// DependencyManagerView.cpp : implementation of the CDependencyManagerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DependencyManager.h"
#endif

#include "DependencyManagerDoc.h"
#include "DependencyManagerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDependencyManagerView

IMPLEMENT_DYNCREATE(CDependencyManagerView, CFormView)

BEGIN_MESSAGE_MAP(CDependencyManagerView, CFormView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CDependencyManagerView construction/destruction

CDependencyManagerView::CDependencyManagerView() noexcept
	: CFormView(IDD_DEPENDENCYMANAGER_FORM)
{
	// TODO: add construction code here

}

CDependencyManagerView::~CDependencyManagerView()
{
}

void CDependencyManagerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CDependencyManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CDependencyManagerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

}

void CDependencyManagerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDependencyManagerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDependencyManagerView diagnostics

#ifdef _DEBUG
void CDependencyManagerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDependencyManagerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDependencyManagerDoc* CDependencyManagerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDependencyManagerDoc)));
	return (CDependencyManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CDependencyManagerView message handlers
