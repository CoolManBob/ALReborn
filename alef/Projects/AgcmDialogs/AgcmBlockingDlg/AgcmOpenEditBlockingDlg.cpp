// AgcmOpenEditBlockingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AgcmBlockingDlg.h"
#include "AgcmOpenEditBlockingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmOpenEditBlockingDlg dialog


AgcmOpenEditBlockingDlg::AgcmOpenEditBlockingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmOpenEditBlockingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmOpenEditBlockingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmOpenEditBlockingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmOpenEditBlockingDlg)
	DDX_Control(pDX, IDC_COMBO_HAS_BLOCKNIG_INDEX, m_csHasBlockingIndex);
	DDX_Control(pDX, ID_NEW_BLOCKING, m_csBtNew);
	DDX_Control(pDX, IDOK, m_csBtOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmOpenEditBlockingDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmOpenEditBlockingDlg)
	ON_BN_CLICKED(ID_NEW_BLOCKING, OnNewBlocking)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmOpenEditBlockingDlg message handlers
VOID AgcmOpenEditBlockingDlg::SetBlockingIndex(INT16 nIndex)
{
	AgcmBlockingDlg::GetInstance()->SetCurrentBlockingIndex(nIndex);
}

void AgcmOpenEditBlockingDlg::OnOK() 
{
	// TODO: Add extra validation here
	SetBlockingIndex(m_csHasBlockingIndex.GetCurSel());
	
	CDialog::OnOK();
}

void AgcmOpenEditBlockingDlg::OnNewBlocking() 
{
	// TODO: Add your control notification handler code here
	SetBlockingIndex(m_csHasBlockingIndex.GetCount());

	EndDialog(ID_NEW_BLOCKING);
}

BOOL AgcmOpenEditBlockingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	
	InitializeControl();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID AgcmOpenEditBlockingDlg::InitializeControl()
{
	AuBLOCKING	*pstBlocking = AgcmBlockingDlg::GetInstance()->GetBlocking();
	INT16		nMaxBlocking = AgcmBlockingDlg::GetInstance()->GetMaxBlocking();
	CHAR		szIndex[256];

	INT16 nIndex;

	for(nIndex = 0; nIndex < nMaxBlocking; ++nIndex)
	{
		if(pstBlocking[nIndex].type == AUBLOCKING_TYPE_NONE)
			break;

		sprintf(szIndex, "%d", nIndex);
		m_csHasBlockingIndex.InsertString(nIndex, szIndex);
	}

	if(nIndex > 0)
	{
		m_csHasBlockingIndex.SetCurSel(0);
	}
	else
	{
		m_csHasBlockingIndex.EnableWindow(FALSE);
		m_csBtOK.EnableWindow(FALSE);
	}
}
