// UITModeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITModeDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITModeDialog dialog

extern CHAR *g_aszUIMode[];


UITModeDialog::UITModeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(UITModeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UITModeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void UITModeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITModeDialog)
	DDX_Control(pDX, IDC_UIT_UI_MODES, m_csModeList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITModeDialog, CDialog)
	//{{AFX_MSG_MAP(UITModeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITModeDialog message handlers

void UITModeDialog::OnOK() 
{
	m_eMode = (AgcdUIMode) m_csModeList.GetItemData(m_csModeList.GetCurSel());

	if ((INT32) m_eMode == -1)
		m_eMode = AGCDUI_MODE_1024_768;

	CDialog::OnOK();
}

BOOL UITModeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32	lIndex;
	INT32	lListIndex;

	for (lIndex = 0; lIndex < AGCDUI_MAX_MODE; ++lIndex)
	{
		lListIndex = m_csModeList.AddString(g_aszUIMode[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csModeList.SetItemData(lListIndex, lIndex);
	}

	m_eMode = g_pcsAgcmUIManager2->GetUIMode();

	m_csModeList.SelectString(-1, g_aszUIMode[m_eMode]);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
