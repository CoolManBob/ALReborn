#include "stdafx.h"
#include "..\EffTool.h"
#include "dlg_effbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlg_EffBase::CDlg_EffBase(CWnd* pParent /*=NULL*/) : CDialog(CDlg_EffBase::IDD, pParent)
{
	m_bBillboard	= FALSE;
	m_bBillboardY	= FALSE;
	m_ulDelay		= 0;
	m_ulLife		= 0;
	m_szTitle		= _T("");
	m_nLoopOpt		= 1;
}

void CDlg_EffBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_EffBase)
	DDX_Control(pDX, IDC_COMBO_LOOPOPT, m_ctrlCombo_LoopOpt);
	DDX_Check(pDX, IDC_CHECK_BILLBOARD, m_bBillboard);
	DDX_Check(pDX, IDC_CHECK_BILLBOARDY, m_bBillboardY);
	DDX_Text(pDX, IDC_EDIT_DELAY, m_ulDelay);
	DDX_Text(pDX, IDC_EDIT_LIFE, m_ulLife);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_szTitle);
	DDV_MaxChars(pDX, m_szTitle, 31);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_EffBase, CDialog)
	//{{AFX_MSG_MAP(CDlg_EffBase)
	ON_BN_CLICKED(IDC_CHECK_BILLBOARD, OnCheckBillboard)
	ON_BN_CLICKED(IDC_CHECK_BILLBOARDY, OnCheckBillboardy)
	ON_CBN_SELCHANGE(IDC_COMBO_LOOPOPT, OnSelchangeComboLoopopt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_EffBase message handlers

BOOL CDlg_EffBase::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrlCombo_LoopOpt.SetCurSel(m_nLoopOpt);//INFINITY
	UpdateData(FALSE);
	
	return TRUE; 
}

void CDlg_EffBase::OnCheckBillboard() 
{
	// TODO: Add your control notification handler code here
}

void CDlg_EffBase::OnCheckBillboardy() 
{
	// TODO: Add your control notification handler code here
}

void CDlg_EffBase::OnSelchangeComboLoopopt() 
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	m_nLoopOpt = m_ctrlCombo_LoopOpt.GetCurSel();	
}
