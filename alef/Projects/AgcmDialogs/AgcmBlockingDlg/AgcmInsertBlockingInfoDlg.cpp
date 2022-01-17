// AgcmInsertBlockingInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmBlockingDlg.h"
#include "AgcmInsertBlockingInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmInsertBlockingInfoDlg dialog

AgcmInsertBlockingInfoDlg::AgcmInsertBlockingInfoDlg(UINT16 unNewIndex, CWnd* pParent)
	: CDialog(AgcmInsertBlockingInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmInsertBlockingInfoDlg)
	m_unNewIndex = unNewIndex;
	//}}AFX_DATA_INIT
}


void AgcmInsertBlockingInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmInsertBlockingInfoDlg)
	DDX_Control(pDX, IDC_COMBO_BLOCKING_TYPE, m_csBlockingType);
	DDX_Text(pDX, IDC_EDIT_NEW_INDEX, m_unNewIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmInsertBlockingInfoDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmInsertBlockingInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmInsertBlockingInfoDlg message handlers

BOOL AgcmInsertBlockingInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);

	m_csBlockingType.InsertString(0, "Box");
	m_csBlockingType.InsertString(1, "Sphere");
	m_csBlockingType.InsertString(2, "Cylinder");
//	m_csBlockingType.InsertString(3, "MinBox");

	m_csBlockingType.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmInsertBlockingInfoDlg::OnOK() 
{
	// TODO: Add extra validation here
	AgcmBlockingDlg::GetInstance()->SetBlockingType((UINT16)(m_csBlockingType.GetCurSel()));

	CDialog::OnOK();
}
