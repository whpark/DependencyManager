
// DependencyManagerView.h : interface of the CDependencyManagerView class
//

#pragma once


class CDependencyManagerView : public CFormView {
public:
	using this_t = CDependencyManagerView;
	using base_t = CFormView;
	
protected: // create from serialization only
	CDependencyManagerView() noexcept;
	DECLARE_DYNCREATE(CDependencyManagerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_DEPENDENCYMANAGER_FORM };
#endif

// Attributes
public:
	CDependencyManagerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CDependencyManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString GetDlgItemText(int idc) {
		CString str;
		base_t::GetDlgItemText(idc, str);
		return str;
	}

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSearchDllFiles();
	afx_msg void OnBnClickedCopyDllFiles();
	CListBox m_lstDll;
	afx_msg void OnBnClickedClearDllFiles();
};

#ifndef _DEBUG  // debug version in DependencyManagerView.cpp
inline CDependencyManagerDoc* CDependencyManagerView::GetDocument() const { return reinterpret_cast<CDependencyManagerDoc*>(m_pDocument); }
#endif

