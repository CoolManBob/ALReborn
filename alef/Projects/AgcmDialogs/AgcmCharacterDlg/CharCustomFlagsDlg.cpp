// CharCustomFlagsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmcharacterdlg.h"
#include "CharCustomFlagsDlg.h"
#include "AgcdEventEffect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCharCustomFlagsDlg dialog


CCharCustomFlagsDlg::CCharCustomFlagsDlg(UINT32 *pulCustomFlags, CWnd* pParent /*=NULL*/)
	: CDialog(CCharCustomFlagsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharCustomFlagsDlg)
	m_bPlayMovingSound = (*(pulCustomFlags) & E_AEE_CHAR_CUSTOM_FLAGS_PLAY_MOVING_SOUND) ? (TRUE) : (FALSE);
	//}}AFX_DATA_INIT

	m_pulCustomFlags	= pulCustomFlags;
}


void CCharCustomFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharCustomFlagsDlg)
	DDX_Check(pDX, IDC_CHECK_PLAY_MOVING_SOUND, m_bPlayMovingSound);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharCustomFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(CCharCustomFlagsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharCustomFlagsDlg message handlers

void CCharCustomFlagsDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	*(m_pulCustomFlags)	= E_AEE_CHAR_CUSTOM_FLAGS_NONE;

	if (m_bPlayMovingSound)
		*(m_pulCustomFlags)	|= E_AEE_CHAR_CUSTOM_FLAGS_PLAY_MOVING_SOUND;

	CDialog::OnOK();
}
