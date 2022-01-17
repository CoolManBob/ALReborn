// AgcmAICharStatusDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAIDlg.h"
#include "AgcmAICharStatusDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharStatusDialog dialog


AgcmAICharStatusDialog::AgcmAICharStatusDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAICharStatusDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAICharStatusDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmAICharStatusDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAICharStatusDialog)
	DDX_Control(pDX, IDC_AI_STATUS, m_csActionStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAICharStatusDialog, CDialog)
	//{{AFX_MSG_MAP(AgcmAICharStatusDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharStatusDialog message handlers
