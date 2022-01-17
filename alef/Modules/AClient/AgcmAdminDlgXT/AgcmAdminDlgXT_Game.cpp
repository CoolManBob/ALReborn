// AgcmAdminDlgXT_Game.cpp : implementation file
//

#include "stdafx.h"
#include "agcmadmindlgxt.h"
#include "AgcmAdminDlgXT_Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Game dialog


AgcmAdminDlgXT_Game::AgcmAdminDlgXT_Game(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAdminDlgXT_Game::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAdminDlgXT_Game)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bInitialized = FALSE;
}


void AgcmAdminDlgXT_Game::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAdminDlgXT_Game)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAdminDlgXT_Game, CDialog)
	//{{AFX_MSG_MAP(AgcmAdminDlgXT_Game)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_Game message handlers

BOOL AgcmAdminDlgXT_Game::Create()
{
	return TRUE;
}

BOOL AgcmAdminDlgXT_Game::OpenDlg(INT nShowCmd /* = SW_SHOW */)
{
	if(::IsWindow(m_hWnd))
		SetFocus();
	else
	{
		CDialog::Create(IDD, NULL);
	}

	ShowWindow(nShowCmd);

	return TRUE;
}

BOOL AgcmAdminDlgXT_Game::IsInitialized()
{
	return m_bInitialized;
}

void AgcmAdminDlgXT_Game::Lock()
{
	m_csLock.Lock();
}

void AgcmAdminDlgXT_Game::Unlock()
{
	m_csLock.Unlock();
}

BOOL AgcmAdminDlgXT_Game::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_bInitialized = TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAdminDlgXT_Game::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
}
