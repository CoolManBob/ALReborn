// XListCtrlTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XListCtrlTest.h"
#include "XListCtrlTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CXListCtrlTestApp

BEGIN_MESSAGE_MAP(CXListCtrlTestApp, CWinApp)
	//{{AFX_MSG_MAP(CXListCtrlTestApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// CXListCtrlTestApp construction

CXListCtrlTestApp::CXListCtrlTestApp()
{
}

///////////////////////////////////////////////////////////////////////////////
// The one and only CXListCtrlTestApp object

CXListCtrlTestApp theApp;

///////////////////////////////////////////////////////////////////////////////
// CXListCtrlTestApp initialization

BOOL CXListCtrlTestApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CXListCtrlTestDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
