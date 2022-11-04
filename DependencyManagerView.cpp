
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
	//ON_WM_CONTEXTMENU()
	//ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)

	ON_BN_CLICKED(IDC_SEARCH_DLL_FILES, &CDependencyManagerView::OnBnClickedSearchDllFiles)
	ON_BN_CLICKED(IDC_STOP, &CDependencyManagerView::OnBnClickedStop)
	ON_BN_CLICKED(IDC_RESET_DLL_FOLDERS_FROM_PATH, &CDependencyManagerView::OnBnClickedResetDllFoldersFromPath)
	ON_BN_CLICKED(IDC_COPY_DLL_FILES, &CDependencyManagerView::OnBnClickedCopyDllFiles)
	ON_BN_CLICKED(IDC_CLEAR_DLL_FILES, &CDependencyManagerView::OnBnClickedClearDllFiles)

END_MESSAGE_MAP()

// CDependencyManagerView construction/destruction

CDependencyManagerView::CDependencyManagerView() noexcept : CFormView(IDD_DEPENDENCYMANAGER_FORM) {

}

CDependencyManagerView::~CDependencyManagerView() {
	OnBnClickedStop();
}

void CDependencyManagerView::DoDataExchange(CDataExchange* pDX) {
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LST_DLL, m_lstDll);
	DDX_Control(pDX, IDC_SEARCH_DLL_FILES, m_btnStart);
	DDX_Control(pDX, IDC_STOP, m_btnStop);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}

BOOL CDependencyManagerView::PreCreateWindow(CREATESTRUCT& cs) {

	return CFormView::PreCreateWindow(cs);
}

void CDependencyManagerView::OnInitialUpdate() {
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

	auto strPathDumpbin = theApp.GetProfileString(
		_T("misc"), _T("Dumpbin"), 
		_T(R"(C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64\dumpbin.exe)"));
	SetDlgItemText(IDC_PATH_DUMPBIN, strPathDumpbin);

	auto strPathExe = theApp.GetProfileString(_T("misc"), _T("PathExe"), _T(""));
	SetDlgItemText(IDC_PATH_EXE, strPathExe);

	auto strFolderDll = theApp.GetProfileString(_T("misc"), _T("FolderDll"), _T(""));
	SetDlgItemText(IDC_FOLDER_DLL, strFolderDll);

	CheckDlgButton(IDC_CHK_COPY_OVERWRITE, theApp.GetProfileInt(_T("misc"), _T("Overwrite"), true) ? 1 : 0);
	CheckDlgButton(IDC_CHK_COPY_PDB, theApp.GetProfileInt(_T("misc"), _T("CopyPDB"), false) ? 1 : 0);

	SetTimer((UINT_PTR)eTIMER::eUpdateUI, 100, nullptr);
}

// CDependencyManagerView diagnostics

#ifdef _DEBUG
void CDependencyManagerView::AssertValid() const { CFormView::AssertValid(); }
void CDependencyManagerView::Dump(CDumpContext& dc) const { CFormView::Dump(dc); }
CDependencyManagerDoc* CDependencyManagerView::GetDocument() const { ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDependencyManagerDoc))); return (CDependencyManagerDoc*)m_pDocument; }
#endif //_DEBUG

// CDependencyManagerView message handlers

BOOL CDependencyManagerView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	if (nCode == CN_COMMAND) {
		if (pExtra) {
			switch (nID) {
			case ID_EDIT_COPY :
			case ID_EDIT_CUT :
			case ID_EDIT_PASTE :
			case ID_EDIT_UNDO :
				if (GetFocusedEdit())
					return true;
				break;
			}
		} else {
			switch (nID) {
			case ID_EDIT_COPY :		OnEditCopy();	return true;
			case ID_EDIT_CUT :		OnEditCut();	return true;
			case ID_EDIT_PASTE :	OnEditPaste();	return true;
			case ID_EDIT_UNDO :		OnEditUndo();	return true;
			}
		}
	}

	return CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

CEdit* CDependencyManagerView::GetFocusedEdit() {
	if (!m_hWnd)
		return nullptr;
	CWnd* pWndFocused = GetFocus();
	if (!pWndFocused)
		return nullptr;
	if (pWndFocused->IsKindOf(RUNTIME_CLASS(CEdit)))
		return (CEdit*)pWndFocused;
	TCHAR szClassName[64];
	::GetClassName(pWndFocused->m_hWnd, szClassName, std::size(szClassName));
	if (_tcscmp(szClassName, _T("Edit")) == 0)
		return (CEdit*)pWndFocused;

	return nullptr;
}
void CDependencyManagerView::OnEditCopy() {
	if (auto* pEdit = GetFocusedEdit()) {
		pEdit->Copy();
	}
}
void CDependencyManagerView::OnEditCut() {
	if (auto* pEdit = GetFocusedEdit()) {
		pEdit->Cut();
	}
}
void CDependencyManagerView::OnEditPaste() {
	if (auto* pEdit = GetFocusedEdit()) {
		pEdit->Paste();
	}
}
void CDependencyManagerView::OnEditUndo() {
	if (auto* pEdit = GetFocusedEdit()) {
		pEdit->Undo();
	}
}

void CDependencyManagerView::OnTimer(UINT_PTR nIDEvent) {
	switch ((eTIMER)nIDEvent) {
		using enum eTIMER;
	case eUpdateUI :
		UpdateUI();
		return;
	}

	base_t::OnTimer(nIDEvent);
}

void CDependencyManagerView::UpdateUI() {
	// Check if threads are done.
	if (m_threadSearchDependencies and m_bDone) {
		m_threadSearchDependencies.reset();
	}

	// Status
	{
		std::shared_lock lock(m_mtxStatus);

		auto& s0 = m_status;
		auto& s1 = m_statusBackup;

		if (s1.nTotal != s0.nTotal) {
			s1.nTotal = s0.nTotal;
			m_progress.SetRange32(0, s1.nTotal);
		}
		if (s1.iPos != s0.iPos) {
			s1.iPos = s0.iPos;
			m_progress.SetPos(s1.iPos);
		}
		if (s1.text != s0.text) {
			s1.text = s0.text;
			SetDlgItemText(IDC_STATUS, s1.text.c_str());
		}

		if (m_lstDll.GetCount() != (int)m_pathsDll.size()) {
			CWaitCursor wc;
			m_lstDll.ResetContent();
			for (auto const& path : m_pathsDll) {
				m_lstDll.AddString(path.c_str());
			}
		}
	}

	//
	static std::vector const idcs0 = {
		IDC_SEARCH_DLL_FILES, IDC_PATH_DUMPBIN, IDC_PATH_EXE, IDC_FOLDER_DLL, IDC_RESET_DLL_FOLDERS_FROM_PATH,
		IDC_COPY_DLL_FILES, IDC_CLEAR_DLL_FILES, 
	};
	static std::vector const idcs1 = {
		IDC_STOP,
	};
	bool bSwitch = !!m_threadSearchDependencies;
	for (auto idc : bSwitch ? idcs0 : idcs1) {
		if (auto* pWnd = GetDlgItem(idc); pWnd and pWnd->IsWindowEnabled())
			pWnd->EnableWindow(false);
	}
	for (auto idc : bSwitch ? idcs1 : idcs0) {
		if (auto* pWnd = GetDlgItem(idc); pWnd and !pWnd->IsWindowEnabled())
			pWnd->EnableWindow(true);
	}

}

void CDependencyManagerView::OnBnClickedSearchDllFiles() {
	if (m_threadSearchDependencies)
		return;

	m_pathsDll.clear();
	m_lstDll.ResetContent();

	stdfs::path pathDumpbin = (LPCTSTR)GetDlgItemText(IDC_PATH_DUMPBIN);
	theApp.WriteProfileString(_T("misc"), _T("Dumpbin"), pathDumpbin.c_str());
	stdfs::path pathExe = (LPCTSTR)GetDlgItemText(IDC_PATH_EXE);
	theApp.WriteProfileString(_T("misc"), _T("PathExe"), pathExe.c_str());
	CString strFoldersDll = GetDlgItemText(IDC_FOLDER_DLL);
	theApp.WriteProfileString(_T("misc"), _T("FolderDll"), strFoldersDll);

	// Search Dependencies
	m_bDone = false;
	m_threadSearchDependencies = std::jthread([&, pathDumpbin = std::move(pathDumpbin), pathExe = std::move(pathExe), strFoldersDll = std::move(strFoldersDll)](std::stop_token stop_token)
		{
			auto result = SearchDependencies(stop_token, pathDumpbin, pathExe, strFoldersDll, m_status, m_mtxStatus);
			std::unique_lock lock(m_mtxStatus);
			if (result)
				m_pathsDll = std::move(*result);
			m_status.iPos = m_status.nTotal = 0;
			m_bDone = true;
		});
	
}

std::optional<std::set<stdfs::path>> CDependencyManagerView::SearchDependencies(std::stop_token stop, stdfs::path const& pathDumpbin, stdfs::path const& pathExe, CString const& strFoldersDll, S_STATUS& s0, std::shared_mutex& mtx) {
	std::set<stdfs::path> pathsDll;	// result

	std::vector<stdfs::path> foldersDll;
	int i{};
	while (i >= 0) {
		auto str = strFoldersDll.Tokenize(_T("\n"), i);
		str.Trim();
		if (!str.IsEmpty())
			foldersDll.emplace_back((LPCTSTR)str);
	};

	std::deque<stdfs::path> pathsModuleToCheck;
	pathsModuleToCheck.push_back(pathExe);

	while (!stop.stop_requested() and pathsModuleToCheck.size()) {
		auto pathModule = pathsModuleToCheck.front();
		pathsModuleToCheck.pop_front();

		// UI
		{
			std::unique_lock lock(mtx);
			s0.nTotal = (int)(pathsDll.size() + pathsModuleToCheck.size());
			s0.iPos = (int)pathsDll.size();
			s0.text = std::format(L"Module: {}", pathModule.c_str());
		}

		// Create Pipe
		HANDLE hReadPipe{}, hWritePipe{};
		SECURITY_ATTRIBUTES sa{.nLength = sizeof(sa), .bInheritHandle = true};
		if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 1024 * 1024)) {
			std::unique_lock lock(mtx);
			s0.text = L"CANNOT Create Pipe!";
			return {};
		}
		xFinalAction faClosePipe{[&]{ if (hReadPipe) CloseHandle(hReadPipe); if (hWritePipe) CloseHandle(hWritePipe);}};

		STARTUPINFO si{.cb=sizeof(si)};
		si.dwFlags |= STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		//si.hStdInput = hReadPipe;
		si.hStdOutput = hWritePipe;
		si.hStdError = hWritePipe;
	
		PROCESS_INFORMATION pi{};
		std::wstring cmd = std::format(LR"("{}" /dependents "{}")", pathDumpbin.c_str(), pathModule.c_str());
		if (!CreateProcess(nullptr, cmd.data(), nullptr, nullptr, true, 0, nullptr, nullptr, &si, &pi)) {
			auto e = GetLastError();
			std::unique_lock lock(mtx);
			s0.text = std::format(L"Error No : {}", e);
			return {};
		}

		// Wait for the dumpbin
		std::jthread thread([&]
			{
				WaitForSingleObject(pi.hProcess, INFINITE);
				std::this_thread::sleep_for(100ms);
				CloseHandle(std::exchange(hWritePipe, nullptr));	// Signals Reader to stop.
				CloseHandle(std::exchange(pi.hProcess, nullptr));
				CloseHandle(std::exchange(pi.hThread, nullptr));
			});

		// Read dependents from pipe (stdout of dumpbin.exe)
		auto text = [&]() {
			std::list<std::vector<uint8_t>> lst;
			size_t total{};
			while (true) {
				uint8_t tmp[1024];
				DWORD len{};
				if (!ReadFile(hReadPipe, tmp, std::size(tmp), &len, nullptr))
					break;
				if (len) {
					lst.emplace_back(tmp, tmp+len);
					total += len;
				}
				//if (len < std::size(tmp))
				//	break;
			};

			std::string text;
			if (!total)
				return text;
			text.assign(total+1, 0);
			size_t pos{};
			for (auto& v : lst) {
				memcpy(text.data() + pos, v.data(), v.size());
				pos += v.size();
			}
			return text;
		}();
		thread.join();

		// Parse Dependencies (dll name)
		static auto const strStart = "Image has the following dependencies:"s;
		auto pos = text.find(strStart);
		if (pos == text.npos)
			continue;
		for (auto line : std::ranges::split_view(std::string_view{ text.begin() + pos + strStart.size()+4, text.end()}, '\n')) {
			std::string fname(line.begin(), line.end());
			CString strFName(fname.c_str());
			strFName.Trim();
			if (strFName.IsEmpty())
				break;

			for (auto const& folderDll : foldersDll) {
				if (stdfs::path pathNew{folderDll / (LPCTSTR)strFName}; stdfs::exists(pathNew)) {
					if (pathsDll.contains(pathNew))
						break;
					pathsDll.insert(pathNew);
					pathsModuleToCheck.push_back(pathNew);
					break;
				}
			}
		}
	}

	return pathsDll;
}

void CDependencyManagerView::OnBnClickedStop() {
	if (m_threadSearchDependencies) {
		m_threadSearchDependencies->get_stop_source().request_stop();
		m_threadSearchDependencies->join();
		m_threadSearchDependencies.reset();
	}
}

void CDependencyManagerView::OnBnClickedResetDllFoldersFromPath() {
	std::wstring str;
	size_t len = GetEnvironmentVariable(_T("Path"), nullptr, 0);
	if (!len or len == (size_t)-1)
		return;
	str.assign(len, {});
	GetEnvironmentVariable(_T("Path"), str.data(), str.size());

	CString strW(str.c_str());
	strW.Replace(_T(";"), _T("\r\n"));
	SetDlgItemText(IDC_FOLDER_DLL, strW);
}

void CDependencyManagerView::OnBnClickedCopyDllFiles() {
	stdfs::path pathExe = (LPCTSTR)GetDlgItemText(IDC_PATH_EXE);
	auto folderExe = pathExe.parent_path();

	bool bOverwrite = IsDlgButtonChecked(IDC_CHK_COPY_OVERWRITE);
	std::filesystem::copy_options option = bOverwrite ? stdfs::copy_options::overwrite_existing : stdfs::copy_options::none;
	bool bCopyPDB = IsDlgButtonChecked(IDC_CHK_COPY_PDB);

	theApp.WriteProfileInt(_T("misc"), _T("Overwrite"), bOverwrite);
	theApp.WriteProfileInt(_T("misc"), _T("CopyPDB"), bCopyPDB);

	if (m_lstDll.GetSelCount() == 0) {
		for (int i{}, n = m_lstDll.GetCount(); i < n; i++) {
			m_lstDll.SetSel(i, true);
		}
	}

	for (int i{}, n = m_lstDll.GetCount(); i < n; i++) {
		if (!m_lstDll.GetSel(i))
			continue;

		CString str;
		m_lstDll.GetText(i, str);

		stdfs::path s {(LPCTSTR)str};
		auto folderDll = s.parent_path();
		auto d = folderExe / s.filename();
		if (pathExe == folderDll or folderExe == folderDll)
			return;

		std::error_code ec{};
		bool bOK = stdfs::copy_file(s, d, option, ec);
		if (bCopyPDB) {
			ec = {};
			bOK &= stdfs::copy_file(s.replace_extension(L".pdb"), d.replace_extension(L".pdb"), option, ec);
		}
		if (bOK)
			m_lstDll.SetSel(i, false);
	}
}

void CDependencyManagerView::OnBnClickedClearDllFiles() {
	m_lstDll.ResetContent();
}
