// PatchCodeViewer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CPatchCodeViewerApp:
// See PatchCodeViewer.cpp for the implementation of this class
//

class CPatchCodeViewerApp : public CWinApp
{
public:
	CPatchCodeViewerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPatchCodeViewerApp theApp;