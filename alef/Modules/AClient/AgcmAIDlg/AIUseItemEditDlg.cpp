// AIUseItemEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmaidlg.h"
#include "AIUseItemEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAIUseItemEditDlg dialog


CAIUseItemEditDlg::CAIUseItemEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAIUseItemEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAIUseItemEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAIUseItemEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAIUseItemEditDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAIUseItemEditDlg, CDialog)
	//{{AFX_MSG_MAP(CAIUseItemEditDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAIUseItemEditDlg message handlers
