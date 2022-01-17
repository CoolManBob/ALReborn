// DetailInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "DetailInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetailInfoDlg dialog


CDetailInfoDlg::CDetailInfoDlg(DetailInfoDlgCallback pfCallback, PVOID pvCustClass /*=NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(CDetailInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDetailInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pvCustClass	= pvCustClass;
	m_pfCallback	= pfCallback;
	m_nIndex		= 0;
}


void CDetailInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDetailInfoDlg)
	DDX_Control(pDX, IDC_LIST_DETAIL_INFO, m_csDetailInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDetailInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CDetailInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailInfoDlg message handlers

BOOL CDetailInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_csDetailInfo.InsertColumn(1, "Member", LVCFMT_LEFT, 150, 0);
	m_csDetailInfo.InsertColumn(2, "Status", LVCFMT_LEFT, 340, 0);

	if(m_pfCallback)
		m_pfCallback(this, m_pvCustClass);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID CDetailInfoDlg::_Reset()
{
	m_csDetailInfo.DeleteAllItems();
	m_nIndex = 0;
}

VOID CDetailInfoDlg::_InsertItem(CHAR *szMember, CHAR *szStatus)
{
	m_csDetailInfo.InsertItem(m_nIndex, szMember);
	m_csDetailInfo.SetItemText(m_nIndex, 1, szStatus);

	++m_nIndex;
}
