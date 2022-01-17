// AgcmAdminDlgXT_Notice.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAdminDlgXT.h"
#include "AgcmAdminDlgXT_Notice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Notice dialog


AgcmAdminDlgXT_Notice::AgcmAdminDlgXT_Notice(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAdminDlgXT_Notice::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAdminDlgXT_Notice)
	m_szNotice = _T("");
	//}}AFX_DATA_INIT

	m_bInitialized = FALSE;

	m_pfCBNotice = NULL;
}


void AgcmAdminDlgXT_Notice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAdminDlgXT_Notice)
	DDX_Control(pDX, IDC_E_NOTICE, m_csNoticeEdit);
	DDX_Control(pDX, IDC_B_NOTICE, m_csNoticeBtn);
	DDX_Text(pDX, IDC_E_NOTICE, m_szNotice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAdminDlgXT_Notice, CDialog)
	//{{AFX_MSG_MAP(AgcmAdminDlgXT_Notice)
	ON_BN_CLICKED(IDC_B_NOTICE, OnBNotice)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Notice message handlers

BOOL AgcmAdminDlgXT_Notice::Create()
{
	return TRUE;
}

BOOL AgcmAdminDlgXT_Notice::OpenDlg(INT nShowCmd /* = SW_SHOW */)
{
	if(::IsWindow(m_hWnd))
		SetFocus();
	else
		CDialog::Create(IDD, m_pParentWnd);

	ShowWindow(nShowCmd);
	
	return TRUE;
}

BOOL AgcmAdminDlgXT_Notice::CloseDlg()
{
	if(::IsWindow(m_hWnd))
		ShowWindow(SW_HIDE);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Notice::IsInitialized()
{
	return m_bInitialized;
}

BOOL AgcmAdminDlgXT_Notice::SetCBNotice(ADMIN_CB pfCBNotice)
{
	m_pfCBNotice = pfCBNotice;
	return TRUE;
}

void AgcmAdminDlgXT_Notice::Lock()
{
	m_csLock.Lock();
}

void AgcmAdminDlgXT_Notice::Unlock()
{
	m_csLock.Unlock();
}

BOOL AgcmAdminDlgXT_Notice::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bInitialized = TRUE;

	// 공지사항의 최고 글자수를 정해준다.
	m_csNoticeEdit.SetLimitText(240);
	
	DWORD dwStyle = BS_XT_SEMIFLAT | BS_XT_SHOWFOCUS | BS_XT_HILITEPRESSED;
	m_csNoticeBtn.SetXButtonStyle(dwStyle);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAdminDlgXT_Notice::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
}

void AgcmAdminDlgXT_Notice::OnBNotice() 
{
	// TODO: Add your control notification handler code here
	if(!m_pfCBNotice)
		return;

	UpdateData();

	if(m_szNotice.IsEmpty())
		return;

	m_pfCBNotice((PVOID)(LPCTSTR)m_szNotice, AgcmAdminDlgXT_Manager::Instance()->GetCBClass(), NULL);

	SetDlgItemText(IDC_E_NOTICE, "");
	m_csNoticeEdit.SetFocus();
}
