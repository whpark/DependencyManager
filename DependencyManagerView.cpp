
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
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)

	ON_BN_CLICKED(IDC_SEARCH_DLL_FILES, &CDependencyManagerView::OnBnClickedSearchDllFiles)
	ON_BN_CLICKED(IDC_COPY_DLL_FILES, &CDependencyManagerView::OnBnClickedCopyDllFiles)
	ON_BN_CLICKED(IDC_CLEAR_DLL_FILES, &CDependencyManagerView::OnBnClickedClearDllFiles)
	ON_BN_CLICKED(IDC_CLEAR_DLL_FILES2, &CDependencyManagerView::OnBnClickedClearDllFiles2)

END_MESSAGE_MAP()

// CDependencyManagerView construction/destruction

CDependencyManagerView::CDependencyManagerView() noexcept : CFormView(IDD_DEPENDENCYMANAGER_FORM) {

}

CDependencyManagerView::~CDependencyManagerView() {
}

void CDependencyManagerView::DoDataExchange(CDataExchange* pDX) {
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_DLL, m_lstDll);
}

BOOL CDependencyManagerView::PreCreateWindow(CREATESTRUCT& cs) {

	return CFormView::PreCreateWindow(cs);
}

void CDependencyManagerView::OnInitialUpdate() {
	CFormView::OnInitialUpdate();
	ResizeParentToFit();

	auto strPathDumpbin = theApp.GetProfileString(_T("misc"), _T("Dumpbin"), _T(R"(C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.33.31629\bin\Hostx64\x64\dumpbin.exe)"));
	SetDlgItemText(IDC_PATH_DUMPBIN, strPathDumpbin);

	auto strPathExe = theApp.GetProfileString(_T("misc"), _T("PathExe"), _T(""));
	SetDlgItemText(IDC_PATH_EXE, strPathExe);

	auto strFolderDll = theApp.GetProfileString(_T("misc"), _T("FolderDll"), _T(""));
	SetDlgItemText(IDC_FOLDER_DLL, strFolderDll);

	CheckDlgButton(IDC_CHK_COPY_OVERWRITE, theApp.GetProfileInt(_T("misc"), _T("Overwrite"), true) ? 1 : 0);
	CheckDlgButton(IDC_CHK_COPY_PDB, theApp.GetProfileInt(_T("misc"), _T("CopyPDB"), false) ? 1 : 0);

}

void CDependencyManagerView::OnRButtonUp(UINT /* nFlags */, CPoint point) {
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDependencyManagerView::OnContextMenu(CWnd* /* pWnd */, CPoint point) {
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
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
			case ID_EDIT_COPY :		OnEditCopy(); return true;
			case ID_EDIT_CUT :		OnEditCut(); return true;
			case ID_EDIT_PASTE :	OnEditPaste(); return true;
			case ID_EDIT_UNDO :		OnEditUndo(); return true;
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


void CDependencyManagerView::OnBnClickedSearchDllFiles() {
	CWaitCursor wc;

	//auto const& home = theApp.m_pathStart;

	stdfs::path pathDumpbin = (LPCTSTR)GetDlgItemText(IDC_PATH_DUMPBIN);
	theApp.WriteProfileString(_T("misc"), _T("Dumpbin"), pathDumpbin.c_str());
	stdfs::path pathExe = (LPCTSTR)GetDlgItemText(IDC_PATH_EXE);
	theApp.WriteProfileString(_T("misc"), _T("PathExe"), pathExe.c_str());
	CString strFoldersDll = GetDlgItemText(IDC_FOLDER_DLL);
	theApp.WriteProfileString(_T("misc"), _T("FolderDll"), strFoldersDll);
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

	std::set<stdfs::path> pathsDll;
	while (pathsModuleToCheck.size()) {
		auto pathModule = pathsModuleToCheck.front();
		pathsModuleToCheck.pop_front();

		// Create Pipe
		HANDLE hReadPipe{}, hWritePipe{};
		SECURITY_ATTRIBUTES sa{.nLength = sizeof(sa), .bInheritHandle = true};
		if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 1024 * 1024)) {
			MessageBox(_T("CANNOT Create Pipe!"));
			return;
		}
		gsl::final_action faClosePipe{[&]{ if (hReadPipe) CloseHandle(hReadPipe); if (hWritePipe) CloseHandle(hWritePipe);}};

		STARTUPINFO si{.cb=sizeof(si)};
		//si.hStdInput = hReadPipe;
		si.dwFlags |= STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdOutput = hWritePipe;
		si.hStdError = hWritePipe;
	
		PROCESS_INFORMATION pi{};
		std::wstring cmd = fmt::format(LR"("{}" /dependents "{}")", pathDumpbin.c_str(), pathModule.c_str());
		if (!CreateProcess(nullptr, cmd.data(), nullptr, nullptr, true, 0, nullptr, nullptr, &si, &pi)) {
			auto e = GetLastError();
			MessageBox(fmt::format(_T("Error No : {}"), e).c_str());
			return;
		}
		std::jthread thread([&]
			{
				WaitForSingleObject(pi.hProcess, INFINITE);
				std::this_thread::sleep_for(100ms);
				CloseHandle(std::exchange(hWritePipe, nullptr));	// Signals Reader to stop.
				CloseHandle(std::exchange(pi.hProcess, nullptr));
				CloseHandle(std::exchange(pi.hThread, nullptr));
			});
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

		static auto const strStart = "Image has the following dependencies:"s;
		auto pos = text.find(strStart);
		if (pos == text.npos)
			continue;
		std::vector<std::string> dlls;
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

	for (auto const& path : pathsDll) {
		m_lstDll.AddString(path.c_str());
	}
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


void CDependencyManagerView::OnBnClickedClearDllFiles2() {
	//char* p = std::getenv("path");
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
