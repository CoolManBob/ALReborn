// SetCharLinkAnimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmanimationdlg.h"
#include "SetCharLinkAnimDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetCharLinkAnimDlg dialog


SetCharLinkAnimDlg::SetCharLinkAnimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetCharLinkAnimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetCharLinkAnimDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SetCharLinkAnimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetCharLinkAnimDlg)
	DDX_Control(pDX, IDC_LINK_ANIM_LIST, m_csLinkAnimList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SetCharLinkAnimDlg, CDialog)
	//{{AFX_MSG_MAP(SetCharLinkAnimDlg)
	ON_BN_CLICKED(IDC_BUTTON_LA_ADD, OnButtonLaAdd)
	ON_BN_CLICKED(IDC_BUTTON_LA_REMOVE, OnButtonLaRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetCharLinkAnimDlg message handlers

void SetCharLinkAnimDlg::OnButtonLaAdd() 
{
	// TODO: Add your control notification handler code here
	
}

void SetCharLinkAnimDlg::OnButtonLaRemove() 
{
	// TODO: Add your control notification handler code here
	
}
