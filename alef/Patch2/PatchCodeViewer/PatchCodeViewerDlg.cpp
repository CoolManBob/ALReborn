// PatchCodeViewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PatchCodeViewer.h"
#include "PatchCodeViewerDlg.h"
#include ".\patchcodeviewerdlg.h"

#include "AuPatchCheckCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPatchCodeViewerDlg dialog



CPatchCodeViewerDlg::CPatchCodeViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchCodeViewerDlg::IDD, pParent)
	, m_strPatchCodeFile(_T(""))
	, m_ulPatchVersion(0)
	, m_dwPatchCode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPatchCodeViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PATCH_CODE_FILE, m_strPatchCodeFile);
	DDX_Text(pDX, IDC_PATCH_VERSION, m_ulPatchVersion);
	DDX_Text(pDX, IDC_PATCH_CODE, m_dwPatchCode);
}

BEGIN_MESSAGE_MAP(CPatchCodeViewerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SET_PATCH_CODE_FILE, OnBnClickedSetPatchCodeFile)
	ON_BN_CLICKED(IDC_GET_CODE, OnBnClickedGetCode)
END_MESSAGE_MAP()


// CPatchCodeViewerDlg message handlers

BOOL CPatchCodeViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPatchCodeViewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPatchCodeViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPatchCodeViewerDlg::OnBnClickedSetPatchCodeFile()
{
	CFileDialog	csDlg(TRUE, NULL, "alefpatch", 0, NULL, NULL, 0);

	if (csDlg.DoModal() == IDOK)
	{
		m_strPatchCodeFile = csDlg.GetPathName();
	}

	UpdateData(FALSE);
}

void CPatchCodeViewerDlg::OnBnClickedGetCode()
{
	UpdateData();

	AuPatchCheckCode	csCodeManager;

	if (!csCodeManager.LoadPatchCode((LPSTR) (LPCTSTR) m_strPatchCodeFile))
	{
		AfxMessageBox("Error Reading Patch Code File !!!", MB_OK);
	}

	m_dwPatchCode = csCodeManager.GetCode(m_ulPatchVersion);

	UpdateData(FALSE);
}
