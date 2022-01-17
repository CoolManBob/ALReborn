// DlgTree.cpp : implementation file
//

#include "stdafx.h"
#include "..\EffTool.h"
#include "DlgTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTree dialog


CDlgTree::CDlgTree(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTree::IDD, pParent)
	, m_bInit(FALSE)
	, ptEdit(1,5)
	, ptTree(1,35)
{
	//{{AFX_DATA_INIT(CDlgTree)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTree)
	DDX_Control(pDX, IDC_TREE_DATA, m_ctrlTreeData);
	DDX_Control(pDX, IDC_EDIT_INFO, m_ctrlEditInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTree, CDialog)
	//{{AFX_MSG_MAP(CDlgTree)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTree message handlers
BOOL CDlgTree::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bInit	= TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTree::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( !m_bInit )
		return;

	CRect	rc(0,0,0,0);
	CRect	rcCtrl(0,0,0,0);

	if( m_ctrlEditInfo.GetSafeHwnd() )
	{
		this->GetWindowRect(&rc);
		rc.DeflateRect(ptEdit.x, ptEdit.y, ptEdit.x, 0);
		m_ctrlEditInfo.GetWindowRect(&rcCtrl);
		ScreenToClient(&rc);
		rc.bottom = rc.top+rcCtrl.Height();
		m_ctrlEditInfo.MoveWindow(&rc);
	}
	if( m_ctrlTreeData.GetSafeHwnd() )
	{
		this->GetWindowRect(&rc);
		rc.DeflateRect(ptTree.x, ptTree.y,ptTree.x, ptTree.x);
		ScreenToClient(&rc);
		m_ctrlTreeData.MoveWindow(&rc);	
	}
}

TV_ITEM CDlgTree::tGetTV_ITEM(HTREEITEM hItem)
{
	static TCHAR szBuff[MAX_PATH]	= "";
	memset(szBuff, 0, sizeof(szBuff));

	TV_ITEM item = {
		TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM
		, hItem
		, 0LU
		, 0LU
		, szBuff
		, MAX_PATH
		, 0
		, 0
		, 0
		, NULL
	};

	m_ctrlTreeData.GetItem( &item );
	return item;
}
