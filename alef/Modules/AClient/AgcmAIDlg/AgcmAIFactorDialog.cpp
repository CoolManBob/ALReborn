// AgcmAIFactorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAIDlg.h"
#include "AgcmAIFactorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAIFactorDialog dialog


AgcmAIFactorDialog::AgcmAIFactorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAIFactorDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAIFactorDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmAIFactorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAIFactorDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAIFactorDialog, CDialog)
	//{{AFX_MSG_MAP(AgcmAIFactorDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAIFactorDialog message handlers
