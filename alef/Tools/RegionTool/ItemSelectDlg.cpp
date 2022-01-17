// ItemSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"
#include "ItemSelectDlg.h"
#include ".\itemselectdlg.h"


// CItemSelectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemSelectDlg, CDialog)
CItemSelectDlg::CItemSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItemSelectDlg::IDD, pParent)
{
	m_nItemID	= -1;
}

CItemSelectDlg::~CItemSelectDlg()
{
}

void CItemSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
}


BEGIN_MESSAGE_MAP(CItemSelectDlg, CDialog)
	ON_LBN_DBLCLK(IDC_LIST, OnLbnDblclkList)
	ON_BN_CLICKED(IDC_NONEEDITEM, OnBnClickedNoneeditem)
END_MESSAGE_MAP()


// CItemSelectDlg 메시지 처리기입니다.

void CItemSelectDlg::OnLbnDblclkList()
{
	int	nIndex = m_ctlList.GetCurSel();
	CString	str;
	m_ctlList.GetText( nIndex , str );

	m_nItemID	= atoi( ( LPCTSTR ) str );

	OnOK();
}

BOOL CItemSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	AgpdItemTemplate* pcsItemTemplate;
	INT32					lIndex = 0;
	CString	str;

	for( pcsItemTemplate = g_pcsAgpmItem->GetTemplateSequence(&lIndex);
		 pcsItemTemplate;
		 pcsItemTemplate = g_pcsAgpmItem->GetTemplateSequence(&lIndex) )
	{
		str.Format( "%04d , %s" , pcsItemTemplate->m_lID , pcsItemTemplate->m_szName );
		m_ctlList.AddString( str );
	}	

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CItemSelectDlg::OnBnClickedNoneeditem()
{
	m_nItemID	= -1;
	UpdateData( FALSE );

	OnOK();
}
