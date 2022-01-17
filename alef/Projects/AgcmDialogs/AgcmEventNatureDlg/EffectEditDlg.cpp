// EffectEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeventnaturedlg.h"
#include "EffectEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectEditDlg dialog


CEffectEditDlg::CEffectEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectEditDlg)
	//}}AFX_DATA_INIT
}


void CEffectEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectEditDlg)
	DDX_Control(pDX, IDC_EFFECTEDIT, m_ctlPortion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffectEditDlg, CDialog)
	//{{AFX_MSG_MAP(CEffectEditDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectEditDlg message handlers
