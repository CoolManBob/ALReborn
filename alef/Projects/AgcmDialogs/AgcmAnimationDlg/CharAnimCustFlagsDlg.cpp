// CharAnimCustFlagsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmanimationdlg.h"
#include "CharAnimCustFlagsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCharAnimCustFlagsDlg dialog


CCharAnimCustFlagsDlg::CCharAnimCustFlagsDlg(UINT16 *pulCustFlags, 
											 INT32* pClumpShowOffsetTime, 
											 UINT32* pClumpShowFadeInTime,
											 INT32* pClumpHideOffsetTime,
											 UINT32* pClumpHideFadeOutTime,
											 CWnd* pParent /*=NULL*/)
	: CDialog(CCharAnimCustFlagsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharAnimCustFlagsDlg)
	m_bLoop = (*(pulCustFlags) & AGCMCHAR_ACF_LOCK_LINK_ANIM) ? (TRUE) : (FALSE);

	//@{ 2006/09/08 burumal
	m_bClumpShowStartTime = (*(pulCustFlags) & AGCMCHAR_ACF_CLUMP_SHOW_START_TIME) ? (TRUE) : (FALSE);
	m_plClumpShowOffsetTime = pClumpShowOffsetTime;
	m_plClumpShowFadeInTime = pClumpShowFadeInTime;

	if ( *m_plClumpShowFadeInTime == 1 )
		*m_plClumpShowFadeInTime = 0;

	m_bClumpHideEndTime = (*(pulCustFlags) & AGCMCHAR_ACF_CLUMP_HIDE_END_TIME) ? (TRUE) : (FALSE);
	m_plClumpHideOffsetTime = pClumpHideOffsetTime;
	m_plClumpHideFadeOutTime = pClumpHideFadeOutTime;
	//@}

	//}}AFX_DATA_INIT

	m_pulCustFlags	= pulCustFlags;
}


void CCharAnimCustFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharAnimCustFlagsDlg)
	DDX_Check(pDX, IDC_CHECK_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_CHECK_CLUMP_SHOW_START_TIME, m_bClumpShowStartTime);
	DDX_Check(pDX, IDC_CHECK_CLUMP_HIDE_END_TIME, m_bClumpHideEndTime);
	DDX_Text(pDX, IDC_EDIT_CLUMP_SHOW_FADE_IN_TIME, *m_plClumpShowFadeInTime);
	DDV_MinMaxInt(pDX, *m_plClumpShowFadeInTime, 0, 2000);
	DDX_Text(pDX, IDC_EDIT_CLUMP_HIDE_FADE_OUT_TIME, *m_plClumpHideFadeOutTime);
	DDV_MinMaxInt(pDX, *m_plClumpHideFadeOutTime, 0, 2000);
	DDX_Text(pDX, IDC_EDIT_CLUMP_SHOW_OFFSET_TIME, *m_plClumpShowOffsetTime);
	DDV_MinMaxInt(pDX, *m_plClumpShowOffsetTime, 0, 2000);
	DDX_Text(pDX, IDC_EDIT_CLUMP_HIDE_OFFSET_TIME, (INT32) *m_plClumpHideOffsetTime);
	DDV_MinMaxInt(pDX, *m_plClumpHideOffsetTime, -2000, 0);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCharAnimCustFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(CCharAnimCustFlagsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharAnimCustFlagsDlg message handlers

void CCharAnimCustFlagsDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	*(m_pulCustFlags)	= 0;

	if (m_bLoop)
		*(m_pulCustFlags)	|= AGCMCHAR_ACF_LOCK_LINK_ANIM;

	if ( m_bClumpHideEndTime )
		*(m_pulCustFlags)	|= AGCMCHAR_ACF_CLUMP_HIDE_END_TIME;

	if ( m_bClumpShowStartTime )
		*(m_pulCustFlags)	|= AGCMCHAR_ACF_CLUMP_SHOW_START_TIME;

	if ( *m_plClumpShowFadeInTime == 0 )
		*m_plClumpShowFadeInTime = 1;

	CDialog::OnOK();
}
