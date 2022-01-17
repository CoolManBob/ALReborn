// AgcmEffectStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeffectdlg.h"
#include "AgcmEffectStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEffectStatusDlg dialog


AgcmEffectStatusDlg::AgcmEffectStatusDlg(AgcdUseEffectSetData *pcsData, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEffectStatusDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmEffectStatusDlg)
	m_bDirection	= FALSE;
	m_bHitEffect	= FALSE;
	m_bOnlyTarget	= FALSE;
	//}}AFX_DATA_INIT

	m_pcsData = pcsData;
}


void AgcmEffectStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEffectStatusDlg)
	DDX_Check(pDX, IDC_CHECK_ES_SET_DIRECTION, m_bDirection);
	DDX_Check(pDX, IDC_CHECK_ES_SET_HIT_EFFECT, m_bHitEffect);
	DDX_Check(pDX, IDC_CHECK_ES_SET_ONLY_TARGET, m_bOnlyTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEffectStatusDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmEffectStatusDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEffectStatusDlg message handlers

void AgcmEffectStatusDlg::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}
