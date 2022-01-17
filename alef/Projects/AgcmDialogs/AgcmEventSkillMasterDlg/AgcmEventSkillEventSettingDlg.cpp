// AgcmEventSkillEventSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"		// main symbols
#include "AgcmEventSkillMasterDlg.h"
#include "AgcmEventSkillEventSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSkillEventSettingDlg dialog

const CHAR *g_szRaceType[AURACE_TYPE_MAX] = 
{
	NULL,
	"Human",
	"Orc"
};

const CHAR *g_szClassType[AUCHARCLASS_TYPE_MAX] = 
{
	NULL,
	"Knight",
	"Ranger",
	"Monk",
	"Mage"
};


AgcmEventSkillEventSettingDlg::AgcmEventSkillEventSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEventSkillEventSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmEventSkillEventSettingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pcsAgpmEventSkillMaster	= NULL;
	m_pcsEvent					= NULL;
}

AgcmEventSkillEventSettingDlg::AgcmEventSkillEventSettingDlg(AgpmEventSkillMaster *pcsAgpmEventSkillMaster, ApdEvent *pcsEvent, CWnd* pParent)
	: CDialog(AgcmEventSkillEventSettingDlg::IDD, pParent)
{
	m_pcsAgpmEventSkillMaster	= pcsAgpmEventSkillMaster;
	m_pcsEvent					= pcsEvent;
}

void AgcmEventSkillEventSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEventSkillEventSettingDlg)
	DDX_Control(pDX, IDC_CHAR_RACE_TYPE, m_csRaceType);
	DDX_Control(pDX, IDC_CHAR_CLASS_TYPE, m_csClassType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEventSkillEventSettingDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmEventSkillEventSettingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSkillEventSettingDlg message handlers

void AgcmEventSkillEventSettingDlg::OnOK() 
{
	// TODO: Add extra validation here

	UpdateData(TRUE);

	if (m_pcsAgpmEventSkillMaster && m_pcsEvent)
	{
		INT32	lRaceIndex		= m_csRaceType.GetCurSel();
		INT32	lRaceListIndex	= m_csRaceType.GetItemData(lRaceIndex);

		INT32	lClassIndex		= m_csClassType.GetCurSel();
		INT32	lClassListIndex	= m_csClassType.GetItemData(lClassIndex);

		AgpdSkillEventAttachData	*pcsAttachData	= (AgpdSkillEventAttachData *) m_pcsEvent->m_pvData;

		pcsAttachData->eRaceType	= (AuRaceType) lRaceListIndex;
		pcsAttachData->eClassType	= (AuCharClassType) lClassListIndex;
	}

	CDialog::OnOK();
}

void AgcmEventSkillEventSettingDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL AgcmEventSkillEventSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	return InitData(m_pcsAgpmEventSkillMaster, m_pcsEvent);
}

BOOL AgcmEventSkillEventSettingDlg::InitData(AgpmEventSkillMaster *pcsAgpmEventSkillMaster, ApdEvent *pcsEvent)
{
	if (!pcsAgpmEventSkillMaster || !pcsEvent || !pcsEvent->m_pvData)
		return FALSE;

	m_pcsAgpmEventSkillMaster	= pcsAgpmEventSkillMaster;
	m_pcsEvent					= pcsEvent;

	AgpdSkillEventAttachData	*pcsAttachData	= (AgpdSkillEventAttachData *) pcsEvent->m_pvData;

	m_csRaceType.ResetContent();
	
	INT32 lIndex;

	for (lIndex = AURACE_TYPE_HUMAN; lIndex < AURACE_TYPE_MAX; ++lIndex)
	{
		if (g_szRaceType[lIndex])
		{
			INT32	lListIndex = m_csRaceType.AddString(g_szRaceType[lIndex]);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csRaceType.SetItemData(lListIndex, lIndex);
		}
	}

	m_csClassType.ResetContent();

	for (INT32 lIndex = AUCHARCLASS_TYPE_KNIGHT; lIndex < AUCHARCLASS_TYPE_MAX; ++lIndex)
	{
		if (g_szClassType[lIndex])
		{
			INT32	lListIndex = m_csClassType.AddString(g_szClassType[lIndex]);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csClassType.SetItemData(lListIndex, lIndex);
		}
	}

	if (pcsAttachData->eRaceType != AURACE_TYPE_NONE)
		m_csRaceType.SelectString(-1, g_szRaceType[pcsAttachData->eRaceType]);

	if (pcsAttachData->eClassType != AUCHARCLASS_TYPE_NONE)
		m_csClassType.SelectString(-1, g_szClassType[pcsAttachData->eClassType]);

	UpdateData(FALSE);

	return TRUE;
}