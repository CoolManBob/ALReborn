// TreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "TreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeDlg dialog


CTreeDlg::CTreeDlg(DWORD *pdwData, CTreeCtrlEx *pcsSrcTree, CWnd* pParent /*=NULL*/)
	: CDialog(CTreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pcsSrcTree	= pcsSrcTree;
	m_pdwData		= pdwData;
}


void CTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeDlg)
	DDX_Control(pDX, IDC_TREE_ITEM, m_csTreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreeDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, OnDblclkTreeItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeDlg message handlers
BOOL CTreeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_pcsSrcTree)
		m_csTreeCtrl._Copy(m_pcsSrcTree);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTreeDlg::OnDblclkTreeItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hSelected = m_csTreeCtrl.GetSelectedItem();
	if (hSelected)
	{
		CString strText = m_csTreeCtrl.GetItemText(hSelected);
		*(m_pdwData)	= m_csTreeCtrl.GetItemData(hSelected);
	}

	*pResult = 0;

	CDialog::OnOK();
}
