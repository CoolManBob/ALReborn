// AgcmAICharFactorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAIDlg.h"
#include "AgcmAICharFactorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharFactorDialog dialog


AgcmAICharFactorDialog::AgcmAICharFactorDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAICharFactorDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAICharFactorDialog)
	m_lCompare = -1;
	//}}AFX_DATA_INIT
}


void AgcmAICharFactorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAICharFactorDialog)
	DDX_CBIndex(pDX, IDC_AI_FACTOR_COMPARE, m_lCompare);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAICharFactorDialog, CDialog)
	//{{AFX_MSG_MAP(AgcmAICharFactorDialog)
	ON_BN_CLICKED(IDC_AI_FACTOR_EDIT, OnAIFactorEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharFactorDialog message handlers

void AgcmAICharFactorDialog::OnAIFactorEdit() 
{
	// TODO: Add your control notification handler code here
	
}
