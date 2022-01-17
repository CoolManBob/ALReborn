// AgcmAIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAIDlg.h"
#include "AgcmAIDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAIDialog dialog

const CHAR *g_szAIFactorName[AGPDAI_FACTOR_MAX_TYPE] =
{
	"Default",
	"Character Factor",
	"Character Status",
	"Character Item",
	"Action History",
	"Environment Position",
	"Emotion"
};

const CHAR *g_szBaseType[APBASE_MAX_TYPE] = 
{
	NULL,
	"Object",
	"Object Template",
	"Character",
	"Character Template",
	"Item",
	"Item Template",
	NULL,
	NULL,
	NULL,
	NULL,
	"Party",
	NULL,
	NULL
};

const CHAR *g_szActionName[AGPDAI_ACTION_MAX_TYPE] = 
{
	"Hold",
	"Move",
	"Follow",
	"Move Away",
	"Wander",
	"Guard",
	"Attack",
	"Assist",
	"Yell",
	"Pickup",
	"Use Item"
	"Use Skill"
};

AgcmAIDialog::AgcmAIDialog(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAIDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAIDialog)
	m_lAITID = 0;
	m_szName = _T("");
	m_fVisibility = 0.0f;
	m_lInterval = 0;
	m_lAIFactorID = 0;
	m_szAIFactorCondition = _T("");
	m_lTargetID = 0;
	m_fTargetPosX = 0.0f;
	m_fTargetPosY = 0.0f;
	m_fTargetPosZ = 0.0f;
	m_fTargetRadius = 0.0f;
	m_bTargetAggressive = FALSE;
	m_bTargetFriendly = FALSE;
	m_bTargetAgro = FALSE;
	m_bTargetDamage = FALSE;
	m_bTargetBuffer = FALSE;
	m_bTargetStrong = FALSE;
	m_bTargetWeak = FALSE;
	m_bTargetBoss = FALSE;
	m_bTargetFollower = FALSE;
	m_bTargetSelf = FALSE;
	m_bTargetOther = FALSE;
	m_lTargetAggressive = 0;
	m_lTargetFriendly = 0;
	m_lTargetAgro = 0;
	m_lTargetDamage = 0;
	m_lTargetBuffer = 0;
	m_lTargetNear = 0;
	m_lTargetFar = 0;
	m_lTargetStrong = 0;
	m_lTargetWeak = 0;
	m_lTargetBoss = 0;
	m_lTargetFollower = 0;
	m_lTargetSelf = 0;
	m_lTargetOther = 0;
	m_lActionRate = 0;
	//}}AFX_DATA_INIT
}


void AgcmAIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAIDialog)
	DDX_Control(pDX, IDC_AI_ACTION_TYPE, m_csActionType);
	DDX_Control(pDX, IDC_AI_TARGET_BASE, m_csTargetBase);
	DDX_Control(pDX, IDC_AI_ACTIONS, m_csAIActions);
	DDX_Control(pDX, IDC_AI_FACTOR_TYPE, m_csAIFactorType);
	DDX_Control(pDX, IDC_AI_FACTORS, m_csAIFactors);
	DDX_Text(pDX, IDC_AI_ID, m_lAITID);
	DDX_Text(pDX, IDC_AI_NAME, m_szName);
	DDX_Text(pDX, IDC_AI_VISIBILITY, m_fVisibility);
	DDX_Text(pDX, IDC_AI_INTERVAL, m_lInterval);
	DDX_Text(pDX, IDC_AI_FACTOR_ID, m_lAIFactorID);
	DDX_Text(pDX, IDC_AI_FACTOR_CONDITION, m_szAIFactorCondition);
	DDX_Text(pDX, IDC_AI_TARGET_ID, m_lTargetID);
	DDX_Text(pDX, IDC_AI_TARGET_POS_X, m_fTargetPosX);
	DDX_Text(pDX, IDC_AI_TARGET_POS_Y, m_fTargetPosY);
	DDX_Text(pDX, IDC_AI_TARGET_POS_Z, m_fTargetPosZ);
	DDX_Text(pDX, IDC_AI_TARGET_RADIUS, m_fTargetRadius);
	DDX_Check(pDX, IDC_AI_TARGET_RELATION_AGGRESSIVE, m_bTargetAggressive);
	DDX_Check(pDX, IDC_AI_TARGET_RELATION_FRIENDLY, m_bTargetFriendly);
	DDX_Check(pDX, IDC_AI_TARGET_AGRO, m_bTargetAgro);
	DDX_Check(pDX, IDC_AI_TARGET_HISTORY_DAMAGE, m_bTargetDamage);
	DDX_Check(pDX, IDC_AI_TARGET_HISTORY_BUFFER, m_bTargetBuffer);
	DDX_Check(pDX, IDC_AI_TARGET_STRONG, m_bTargetStrong);
	DDX_Check(pDX, IDC_AI_TARGET_WEAK, m_bTargetWeak);
	DDX_Check(pDX, IDC_AI_TARGET_BOSS, m_bTargetBoss);
	DDX_Check(pDX, IDC_AI_TARGET_FOLLOWER, m_bTargetFollower);
	DDX_Check(pDX, IDC_AI_TARGET_SELF, m_bTargetSelf);
	DDX_Check(pDX, IDC_AI_TARGET_OTHER, m_bTargetOther);
	DDX_Text(pDX, IDC_AI_WEIGHT_RELATION_AGGRESSIVE, m_lTargetAggressive);
	DDX_Text(pDX, IDC_AI_WEIGHT_RELATION_FRIENLDY, m_lTargetFriendly);
	DDX_Text(pDX, IDC_AI_WEIGHT_AGRO, m_lTargetAgro);
	DDX_Text(pDX, IDC_AI_WEIGHT_HISTORY_DAMAGE, m_lTargetDamage);
	DDX_Text(pDX, IDC_AI_WEIGHT_HISTORY_BUFFER, m_lTargetBuffer);
	DDX_Text(pDX, IDC_AI_WEIGHT_NEAR, m_lTargetNear);
	DDX_Text(pDX, IDC_AI_WEIGHT_FAR, m_lTargetFar);
	DDX_Text(pDX, IDC_AI_WEIGHT_STRONG, m_lTargetStrong);
	DDX_Text(pDX, IDC_AI_WEIGHT_WEAK, m_lTargetWeak);
	DDX_Text(pDX, IDC_AI_WEIGHT_BOSS, m_lTargetBoss);
	DDX_Text(pDX, IDC_AI_WEIGHT_FOLLOWER, m_lTargetFollower);
	DDX_Text(pDX, IDC_AI_WEIGHT_SELF, m_lTargetSelf);
	DDX_Text(pDX, IDC_AI_WEIGHT_OTHER, m_lTargetOther);
	DDX_Text(pDX, IDC_AI_ACTION_RATE, m_lActionRate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAIDialog, CDialog)
	//{{AFX_MSG_MAP(AgcmAIDialog)
	ON_BN_CLICKED(IDC_AI_EDIT_CONDITION, OnAIEditCondition)
	ON_BN_CLICKED(IDC_AI_FACTOR_ADD, OnAIFactorAdd)
	ON_BN_CLICKED(IDC_AI_FACTOR_DELETE, OnAIFactorDelete)
	ON_BN_CLICKED(IDC_AI_ACTION_UPDATE, OnAIActionAddUpdate)
	ON_BN_CLICKED(IDC_AI_ACTION_DELETE, OnAIActionDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AI_FACTORS, OnItemchangedAIFactors)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AI_ACTIONS, OnItemchangedAIActions)
	ON_EN_CHANGE(IDC_AI_VISIBILITY, OnChangeAIVisibility)
	ON_EN_CHANGE(IDC_AI_INTERVAL, OnChangeAIInterval)
	ON_BN_CLICKED(IDC_AI_USEITEM, OnAiUseitem)
	ON_BN_CLICKED(IDC_AI_SCREAM, OnAiScream)
	ON_BN_CLICKED(IDC_AI_USESKILL, OnAiUseskill)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmAIDialog message handlers

BOOL AgcmAIDialog::Create() 
{
	return CDialog::Create(IDD, NULL);
}

VOID AgcmAIDialog::SetModule(AgpmAI *pcsAgpmAI, AgcmAIDlg *pcsAgcmAIDlg, AgpmCharacter *pcsAgpmCharacter, AgpmItem *pcsAgpmItem)
{
	m_pcsAgpmAI = pcsAgpmAI;
	m_pcsAgcmAIDlg = pcsAgcmAIDlg;
	m_csAgcmAIScreamEditDlg.m_pcsAgpmCharacter = pcsAgpmCharacter;
	m_csAgcmAIUseItemEditDlg.m_pcsAgpmItem = pcsAgpmItem;

	m_dlgCharItem.SetModule(m_pcsAgcmAIDlg->m_pcsAgpmItem);
}

BOOL AgcmAIDialog::Init()
{
	return TRUE;
}

BOOL AgcmAIDialog::InitData(AgpdAITemplate *pstTemplate)
{
	m_pstTemplate = pstTemplate;

	return TRUE;
}

BOOL AgcmAIDialog::InitDataFactor(AgpdAIFactor *pstAIFactor)
{
	INT32		lIndex;
	INT32		lListIndex;
	CHAR		szTemp[32];

	m_pstAIFactor = pstAIFactor;

	m_csAIActions.DeleteAllItems();

	if (pstAIFactor)
	{
		m_lAIFactorID = pstAIFactor->m_lFactorID;
		m_csAIFactorType.SetCurSel(pstAIFactor->m_eType);

		for (lIndex = 0; lIndex < AGPDAI_ACTION_MAX_TYPE; ++lIndex)
		{
			if (pstAIFactor->m_astActions[lIndex].m_lActionRate > 0)
			{
				lListIndex = m_csAIActions.InsertItem(lIndex, g_szActionName[lIndex]);
				if (lListIndex < 0)
					return FALSE;

				wsprintf(szTemp, "%d", pstAIFactor->m_astActions[lIndex].m_lActionRate);
				m_csAIActions.SetItemText(lListIndex, 1, szTemp);

				m_csAIActions.SetItemData(lListIndex, (INT32) (&pstAIFactor->m_astActions[lIndex]));
			}
		}
	}
	else
	{
		m_lAIFactorID = 0;
		m_csAIFactorType.SetCurSel(-1);
		m_szAIFactorCondition = "";
	}

	UpdateData(FALSE);

	InitDataAction(NULL);

	return TRUE;
}

BOOL AgcmAIDialog::InitDataAction(AgpdAIAction *pstAction)
{
	AgpdAIAction	stAction;

	m_pstAction = pstAction;

	if (pstAction)
	{
		m_csActionType.SetCurSel(pstAction->m_eActionType);
		m_csTargetBase.SetCurSel(m_csTargetBase.FindString(-1, g_szBaseType[pstAction->m_stTarget.m_csTargetBase.m_eType]));
	}
	else
	{
		memset(&stAction, 0, sizeof(AgpdAIAction));
		pstAction = &stAction;

		m_csActionType.SetCurSel(-1);
		m_csTargetBase.SetCurSel(-1);
	}

	m_bTargetAggressive	= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_RELATION_AGGRESSIVE)	? TRUE : FALSE;
	m_bTargetFriendly	= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_RELATION_FRIENDLY)		? TRUE : FALSE;
	m_bTargetAgro		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_AGRO)					? TRUE : FALSE;
	m_bTargetDamage		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_HISTORY_DAMAGE)		? TRUE : FALSE;
	m_bTargetBuffer		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_HISTORY_BUFFER)		? TRUE : FALSE;
	m_bTargetStrong		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_STRONG)				? TRUE : FALSE;
	m_bTargetWeak		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_WEAK)					? TRUE : FALSE;
	m_bTargetBoss		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_BOSS)					? TRUE : FALSE;
	m_bTargetFollower	= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_FOLLOWER)				? TRUE : FALSE;
	m_bTargetSelf		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_SELF)					? TRUE : FALSE;
	m_bTargetOther		= (pstAction->m_stTarget.m_lTargetFlags & AGPDAI_TARGET_FLAG_OTHER)					? TRUE : FALSE;

	m_lTargetAggressive	= pstAction->m_stTarget.m_stTargetWeight.m_lRelationAggressive	;
	m_lTargetFriendly	= pstAction->m_stTarget.m_stTargetWeight.m_lRelationFriendly	;
	m_lTargetAgro		= pstAction->m_stTarget.m_stTargetWeight.m_lAgro				;
	m_lTargetDamage		= pstAction->m_stTarget.m_stTargetWeight.m_lHistoryDamage		;
	m_lTargetBuffer		= pstAction->m_stTarget.m_stTargetWeight.m_lHistoryBuffer		;
	m_lTargetNear		= pstAction->m_stTarget.m_stTargetWeight.m_lNear				;
	m_lTargetFar		= pstAction->m_stTarget.m_stTargetWeight.m_lFar					;
	m_lTargetStrong		= pstAction->m_stTarget.m_stTargetWeight.m_lStrong				;
	m_lTargetWeak		= pstAction->m_stTarget.m_stTargetWeight.m_lWeak				;
	m_lTargetBoss		= pstAction->m_stTarget.m_stTargetWeight.m_lBoss				;
	m_lTargetFollower	= pstAction->m_stTarget.m_stTargetWeight.m_lFollower			;
	m_lTargetSelf		= pstAction->m_stTarget.m_stTargetWeight.m_lSelf				;
	m_lTargetOther		= pstAction->m_stTarget.m_stTargetWeight.m_lOther				;

	UpdateData(FALSE);

	return TRUE;
}

void AgcmAIDialog::OnAIEditCondition() 
{
	if (!m_pstAIFactor)
	{
		AfxMessageBox("Select AI Factor first !!!");
		return;
	}

	switch (m_pstAIFactor->m_eType)
	{
	case AGPDAI_FACTOR_TYPE_CHAR_FACTOR:
		if (m_dlgCharFactor.DoModal() == IDOK)
		{
		}
		break;
	case AGPDAI_FACTOR_TYPE_CHAR_STATUS:
		if (m_dlgCharStatus.DoModal() == IDOK)
		{
		}
		break;
	case AGPDAI_FACTOR_TYPE_CHAR_ITEM:
		if (m_dlgCharItem.DoModal() == IDOK)
		{
			m_szAIFactorCondition.Format("If has item <%s>", m_dlgCharItem.m_pcsItemTemplate ? m_dlgCharItem.m_pcsItemTemplate->m_szName : "");
			UpdateData(FALSE);
		}
		break;
	case AGPDAI_FACTOR_TYPE_ACTION_HISTORY:
		break;
	case AGPDAI_FACTOR_TYPE_ENV_POSITION:
		break;
	case AGPDAI_FACTOR_TYPE_EMOTION:
		break;
	}
}

void AgcmAIDialog::OnAIFactorAdd() 
{
	INT32			lIndex;
	INT32			lListIndex;
	AgpdAIFactor *	pstAIFactor;

	UpdateData();

	if (!m_pstTemplate)
		return;

	lIndex = m_csAIFactorType.GetCurSel();
	if (lIndex < 0)
		return;

	lListIndex = m_csAIFactorType.GetItemData(lIndex);
	pstAIFactor = m_pcsAgpmAI->AddAIFactor(&m_pstTemplate->m_stAI, (AgpdAIFactorType) lListIndex);
	if (!pstAIFactor)
	{
		AfxMessageBox("Failed to add AI Factor !!!");
		return;
	}

	InitData(m_pstTemplate);
}

void AgcmAIDialog::OnAIFactorDelete() 
{
	if (!m_pstAIFactor)
	{
		AfxMessageBox("Select AI Factor first !!!");
		return;
	}

	m_pcsAgpmAI->RemoveAIFactor(&m_pstTemplate->m_stAI, m_pstAIFactor->m_lFactorID);

	InitData(m_pstTemplate);
}

void AgcmAIDialog::OnAIActionAddUpdate() 
{
	INT32		lIndex;
	INT32		lListIndex;

	UpdateData();

	if (!m_pstAIFactor)
	{
		AfxMessageBox("Select AI Factor first !!!");
		return;
	}

	lIndex = m_csActionType.GetCurSel();
	if (lIndex == CB_ERR)
	{
		AfxMessageBox("Select Action Type first !!!");
		return;
	}

	if (!m_lActionRate)
	{
		AfxMessageBox("Action Rate must not be zero !!!");
		return;
	}

	lListIndex = m_csActionType.GetItemData(lIndex);

	m_pstAction = &m_pstAIFactor->m_astActions[lListIndex];

	lIndex = m_csTargetBase.GetCurSel();
	lListIndex = m_csTargetBase.GetItemData(lIndex);

	m_pstAction->m_lActionRate = m_lActionRate;
	m_pstAction->m_stTarget.m_csTargetBase.m_eType = (ApBaseType) lListIndex;
	m_pstAction->m_stTarget.m_csTargetBase.m_lID = m_lTargetID;
	m_pstAction->m_stTarget.m_stTargetPos.x = m_fTargetPosX;
	m_pstAction->m_stTarget.m_stTargetPos.y = m_fTargetPosY;
	m_pstAction->m_stTarget.m_stTargetPos.z = m_fTargetPosZ;
	m_pstAction->m_stTarget.m_fRadius = m_fTargetRadius;

	m_pstAction->m_stTarget.m_lTargetFlags = 0;

	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetAggressive	? AGPDAI_TARGET_FLAG_RELATION_AGGRESSIVE	: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetFriendly		? AGPDAI_TARGET_FLAG_RELATION_FRIENDLY		: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetAgro			? AGPDAI_TARGET_FLAG_AGRO					: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetDamage		? AGPDAI_TARGET_FLAG_HISTORY_DAMAGE			: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetBuffer		? AGPDAI_TARGET_FLAG_HISTORY_BUFFER			: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetStrong		? AGPDAI_TARGET_FLAG_STRONG					: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetWeak			? AGPDAI_TARGET_FLAG_WEAK					: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetBoss			? AGPDAI_TARGET_FLAG_BOSS					: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetFollower		? AGPDAI_TARGET_FLAG_FOLLOWER				: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetSelf			? AGPDAI_TARGET_FLAG_SELF					: 0;
	m_pstAction->m_stTarget.m_lTargetFlags |= m_bTargetOther		? AGPDAI_TARGET_FLAG_OTHER					: 0;

	m_pstAction->m_stTarget.m_stTargetWeight.m_lRelationAggressive	= m_lTargetAggressive	;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lRelationFriendly	= m_lTargetFriendly		;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lAgro				= m_lTargetAgro			;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lHistoryDamage		= m_lTargetDamage		;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lHistoryBuffer		= m_lTargetBuffer		;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lNear				= m_lTargetNear			;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lFar					= m_lTargetFar			;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lStrong				= m_lTargetStrong		;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lWeak				= m_lTargetWeak			;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lBoss				= m_lTargetBoss			;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lFollower			= m_lTargetFollower		;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lSelf				= m_lTargetSelf			;
	m_pstAction->m_stTarget.m_stTargetWeight.m_lOther				= m_lTargetOther		;

	InitDataFactor(m_pstAIFactor);
}

void AgcmAIDialog::OnAIActionDelete() 
{
}

void AgcmAIDialog::OnItemchangedAIFactors(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	INT32					lListIndex;
	POSITION				pos = m_csAIFactors.GetFirstSelectedItemPosition();

	UpdateData();

	if (pos)
	{
		lListIndex = m_csAIFactors.GetNextSelectedItem(pos);
		m_pstAIFactor = (AgpdAIFactor *) m_csAIFactors.GetItemData(lListIndex);
		if (!m_pstAIFactor)
			return;

		InitDataFactor(m_pstAIFactor);
	}
}

void AgcmAIDialog::OnItemchangedAIActions(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	INT32					lListIndex;
	POSITION				pos = m_csAIActions.GetFirstSelectedItemPosition();

	UpdateData();

	if (pos)
	{
		lListIndex = m_csAIActions.GetNextSelectedItem(pos);
		m_pstAction = (AgpdAIAction *) m_csAIActions.GetItemData(lListIndex);
		if (!m_pstAction)
			return;

		InitDataAction(m_pstAction);
	}
}

void AgcmAIDialog::OnChangeAIVisibility() 
{
	UpdateData();

	if (m_pstTemplate)
		m_pstTemplate->m_stAI.m_fVisibility = m_fVisibility;
}

void AgcmAIDialog::OnChangeAIInterval() 
{
	UpdateData();

	if (m_pstTemplate)
		m_pstTemplate->m_stAI.m_ulProcessInterval = m_lInterval;
}

void AgcmAIDialog::OnOK() 
{
	UpdateData();

	if (m_pstTemplate)
	{
		m_pstTemplate->m_stAI.m_fVisibility = m_fVisibility;
		m_pstTemplate->m_stAI.m_ulProcessInterval = m_lInterval;
	}
	
	CDialog::OnOK();
}

BOOL AgcmAIDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	INT32						lIndex;
	INT32						lListIndex;
	AgpdAIFactor *				pstAIFactor;
	AuNode< AgpdAIFactor > *	pNode;
	CHAR						szTemp[32];

	m_csAIFactors.InsertColumn(1, "ID", LVCFMT_LEFT, 120);
	m_csAIFactors.InsertColumn(2, "Type", LVCFMT_LEFT, 200);

	m_csAIActions.InsertColumn(1, "Type", LVCFMT_LEFT, 160);
	m_csAIActions.InsertColumn(2, "Rate", LVCFMT_LEFT, 80);

	for (lIndex = 0; lIndex < AGPDAI_FACTOR_MAX_TYPE; ++lIndex)
	{
		lListIndex = m_csAIFactorType.AddString(g_szAIFactorName[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csAIFactorType.SetItemData(lListIndex, lIndex);
	}

	for (lIndex = 0; lIndex < AGPDAI_ACTION_MAX_TYPE; ++lIndex)
	{
		if (g_szActionName[lIndex])
		{
			lListIndex = m_csActionType.AddString(g_szActionName[lIndex]);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csActionType.SetItemData(lListIndex, lIndex);
		}
	}

	for (lIndex = 0; lIndex < APBASE_MAX_TYPE; ++lIndex)
	{
		if (g_szBaseType[lIndex])
		{
			lListIndex = m_csTargetBase.AddString(g_szBaseType[lIndex]);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csTargetBase.SetItemData(lListIndex, lIndex);
		}
	}

	UpdateData(FALSE);

	m_csAIFactors.DeleteAllItems();

	if (m_pstTemplate)
	{
		m_lAITID = m_pstTemplate->m_lID;
		m_szName = m_pstTemplate->m_szName;
		m_fVisibility = m_pstTemplate->m_stAI.m_fVisibility;
		m_lInterval = m_pstTemplate->m_stAI.m_ulProcessInterval;

		UpdateData(FALSE);

		lIndex = 0;
		pNode = m_pstTemplate->m_stAI.m_listAIFactors.GetHeadNode();
		while (pNode)
		{
			pstAIFactor	= &pNode->GetData();

			sprintf(szTemp, "%d", pstAIFactor->m_lFactorID);
			lListIndex = m_csAIFactors.InsertItem(lIndex, szTemp);
			if (lListIndex < 0)
				return FALSE;

			m_csAIFactors.SetItemText(lListIndex, 1, g_szAIFactorName[pstAIFactor->m_eType]);
			m_csAIFactors.SetItemData(lListIndex, (INT32) pstAIFactor);

			m_pstTemplate->m_stAI.m_listAIFactors.GetNext(pNode);

			++lIndex;
		}
	}
	else
	{
		m_lAITID = 0;
		m_szName = "";
		m_fVisibility = 0;
		m_lInterval = 0;

		UpdateData(FALSE);
	}

	InitDataFactor(NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAIDialog::OnAiUseitem() 
{
	// TODO: Add your control notification handler code here
	m_csAgcmAIUseItemEditDlg.m_pstTemplate = m_pstTemplate;
	m_csAgcmAIUseItemEditDlg.DoModal();
}

void AgcmAIDialog::OnAiScream() 
{
	// TODO: Add your control notification handler code here
	m_csAgcmAIScreamEditDlg.m_pstTemplate = m_pstTemplate;
	m_csAgcmAIScreamEditDlg.DoModal();
}

void AgcmAIDialog::OnAiUseskill() 
{
	// TODO: Add your control notification handler code here
	m_csAgcmAIUseSkillEditDlg.m_pstTemplate = m_pstTemplate;
	m_csAgcmAIUseSkillEditDlg.DoModal();

}
