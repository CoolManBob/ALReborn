// ItemListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmitemdlg.h"
#include "ItemListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemListDlg dialog


CItemListDlg::CItemListDlg(INT32 *plTID, CWnd* pParent /*=NULL*/)
	: CDialog(CItemListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_plTID = plTID;
}


void CItemListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemListDlg)
	DDX_Control(pDX, IDC_LIST_ITEM_LIST, m_csItemList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemListDlg, CDialog)
	//{{AFX_MSG_MAP(CItemListDlg)
	ON_LBN_DBLCLK(IDC_LIST_ITEM_LIST, OnDblclkListItemList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemListDlg message handlers

BOOL CItemListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AgpmItem *pcsAgpmItem = AgcmItemDlg::GetInstance()->GetAgpmItemModule();

	INT32 lIndex = 0;
	INT32 lCount = 0;

	for(AgpdItemTemplate *pcsAgpdItemTemplate = pcsAgpmItem->GetTemplateSequence(&lIndex);
		pcsAgpdItemTemplate;
		pcsAgpdItemTemplate = pcsAgpmItem->GetTemplateSequence(&lIndex), ++lCount			)
	{
		m_csItemList.SetItemData(m_csItemList.InsertString(lCount, pcsAgpdItemTemplate->m_szName), pcsAgpdItemTemplate->m_lID);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemListDlg::OnOK() 
{
	// TODO: Add extra validation here
	*(m_plTID) = m_csItemList.GetItemData(m_csItemList.GetCurSel());

	CDialog::OnOK();
}

void CItemListDlg::OnDblclkListItemList() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}
