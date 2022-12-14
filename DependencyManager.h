
// DependencyManager.h : main header file for the DependencyManager application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDependencyManagerApp:
// See DependencyManager.cpp for the implementation of this class
//

class CDependencyManagerApp : public CWinAppEx {
public:
	CDependencyManagerApp() noexcept;

public:
	stdfs::path m_pathStart;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDependencyManagerApp theApp;
