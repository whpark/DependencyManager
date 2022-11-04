
// DependencyManagerView.h : interface of the CDependencyManagerView class
//

#pragma once

#include "FinalAction.h"

//====================================================================================
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
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnInitialUpdate() override; // called first time after construct
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;

// Implementation
public:
	virtual ~CDependencyManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CListBox m_lstDll;

protected:

	// UI
	CProgressCtrl m_progress;

	std::shared_mutex m_mtxStatus;
	struct S_STATUS {
		int nTotal{}, iPos{};
		std::wstring text;
	} m_status, m_statusBackup;

	CButton m_btnStart, m_btnStop;
	void UpdateUI();
	// Search Dependencies
	std::set<stdfs::path> m_pathsDll;

	std::optional<std::jthread> m_threadSearchDependencies;
	bool m_bDone{};
	static std::optional<std::set<stdfs::path>> SearchDependencies(std::stop_token stop, stdfs::path const& pathDumpbin, stdfs::path const& pathExe, CString const& strFoldersDll, S_STATUS& status, std::shared_mutex& mtx);

protected:
	CString GetDlgItemText(int idc) {
		CString str;
		base_t::GetDlgItemText(idc, str);
		return str;
	}

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnFilePrintPreview();

	enum class eTIMER : UINT_PTR { _ = 1023, eUpdateUI, };
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CEdit* GetFocusedEdit();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();

	afx_msg void OnBnClickedSearchDllFiles();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedResetDllFoldersFromPath();
	afx_msg void OnBnClickedCopyDllFiles();
	afx_msg void OnBnClickedClearDllFiles();
};

#ifndef _DEBUG  // debug version in DependencyManagerView.cpp
inline CDependencyManagerDoc* CDependencyManagerView::GetDocument() const { return reinterpret_cast<CDependencyManagerDoc*>(m_pDocument); }
#endif

