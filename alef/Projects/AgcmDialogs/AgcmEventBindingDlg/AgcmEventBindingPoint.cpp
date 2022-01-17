// AgcmEventBindingPoint.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmEventBindingDlg.h"
#include "AgcmEventBindingPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEventBindingPoint dialog


AgcmEventBindingPoint::AgcmEventBindingPoint(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEventBindingPoint::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmEventBindingPoint)
	m_strBindingName = _T("");
	m_ulRadius = 0;
	//}}AFX_DATA_INIT

	m_pcsApmEventManager	= NULL;
	m_pcsAgpmEventBinding	= NULL;
	m_pcsApmMap				= NULL;
	m_pcsAgpmFactors		= NULL;

	m_pcsEvent				= NULL;

	ZeroMemory(m_astrBindingTypeName, sizeof(CHAR) * AGPDBINDING_TYPE_MAX * (AGPDBINDING_MAX_NAME + 1));
	ZeroMemory(m_astrBindingCharacterTypeName, sizeof(CHAR) * AURACE_TYPE_MAX * AUCHARCLASS_TYPE_MAX * 64);
}


void AgcmEventBindingPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEventBindingPoint)
	DDX_Control(pDX, IDC_COMBO_RACE_CLASS_TYPE, m_csCharacterType);
	DDX_Control(pDX, IDC_COMBO_BINDING_TYPE, m_csBindingType);
	DDX_Control(pDX, IDC_COMBO_REGION_NAME, m_csRegionName);
	DDX_Text(pDX, IDC_EDIT_BINDING_NAME, m_strBindingName);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_ulRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEventBindingPoint, CDialog)
	//{{AFX_MSG_MAP(AgcmEventBindingPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEventBindingPoint message handlers

AgcmEventBindingPoint::AgcmEventBindingPoint(ApmEventManager *pcsApmEventManager,
											 AgpmEventBinding *pcsAgpmEventBinding,
											 AgpmFactors *pcsAgpmFactors,
											 ApmMap *pcsApmMap,
											 ApdEvent *pcsEvent,
											 CWnd* pParent)
											 : CDialog(AgcmEventBindingPoint::IDD, pParent)
{
	m_pcsApmEventManager	= pcsApmEventManager;
	m_pcsAgpmEventBinding	= pcsAgpmEventBinding;
	m_pcsAgpmFactors		= pcsAgpmFactors;
	m_pcsApmMap				= pcsApmMap;
	
	m_pcsEvent				= pcsEvent;

	ZeroMemory(m_astrBindingTypeName, sizeof(CHAR) * AGPDBINDING_TYPE_MAX * (AGPDBINDING_MAX_NAME + 1));

	strcpy(m_astrBindingTypeName[AGPDBINDING_TYPE_RESURRECTION]					,	"부활"				);
	strcpy(m_astrBindingTypeName[AGPDBINDING_TYPE_NEW_CHARACTER]				,	"새캐릭터"			);
	strcpy(m_astrBindingTypeName[AGPDBINDING_TYPE_SIEGEWAR_OFFENSE]				,	"공성공격"			);
	strcpy(m_astrBindingTypeName[AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_INNER]		,	"공성수비(내성)"	);		
	strcpy(m_astrBindingTypeName[AGPDBINDING_TYPE_SIEGEWAR_DEFENSE_OUTTER]		,	"공성수비(외성)"	);		
	strcpy(m_astrBindingTypeName[AGPDBINDING_TYPE_SIEGEWAR_ARCHLORD]			,	"아크로드"			);		
	strcpy(m_astrBindingTypeName[AGPDBINDING_TYPE_SIEGEWAR_ARCHLORD_ATTACKER],		"아크로드공성길드"	);		

	ZeroMemory(m_astrBindingCharacterTypeName, sizeof(CHAR) * AURACE_TYPE_MAX * AUCHARCLASS_TYPE_MAX * 64);

//	for (int i = AURACE_TYPE_NONE + 1; i < AURACE_TYPE_MAX; ++i)
//	{
//		for (int j = AUCHARCLASS_TYPE_NONE + 1; j < AUCHARCLASS_TYPE_MAX; ++j)
//		{
//			CHAR	*szBuffer	= m_astrBindingCharacterTypeName[(i - (AURACE_TYPE_NONE + 1)) * (AUCHARCLASS_TYPE_MAX - 1) + (j - (AUCHARCLASS_TYPE_NONE + 1))];
//
//			sprintf(szBuffer, "%s/%s", m_pcsAgpmFactors->GetCharacterRaceName(i), m_pcsAgpmFactors->GetCharacterClassName(i, j));
//		}
//	}

	for (int i = AURACE_TYPE_NONE + 1; i < AURACE_TYPE_MAX; ++i)
	{
		CHAR	*szBuffer	= m_astrBindingCharacterTypeName[i];

		sprintf(szBuffer, "%s", m_pcsAgpmFactors->GetCharacterRaceName(i));
	}
}

BOOL AgcmEventBindingPoint::InitEventData()
{
	if (!m_pcsEvent || !m_pcsEvent->m_pvData)
		return FALSE;

	m_csBindingType.ResetContent();

	int i;

	for (i = AGPDBINDING_TYPE_NONE + 1; i < AGPDBINDING_TYPE_MAX; ++i)
	{
		INT32	lListIndex = m_csBindingType.AddString(m_astrBindingTypeName[i]);
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csBindingType.SetItemData(lListIndex, i);
	}

	m_csRegionName.ResetContent();

	AuNode< ApmMap::RegionTemplate > * pNode = m_pcsApmMap->m_listTemplate.GetHeadNode();
	ApmMap::RegionTemplate * pTemplateInList;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		if (pTemplateInList &&
			pTemplateInList->pStrName &&
			pTemplateInList->pStrName[0])
		{
			INT32	lListIndex = m_csRegionName.AddString(pTemplateInList->pStrName);
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csRegionName.SetItemData(lListIndex, (DWORD) pTemplateInList->pStrName);
		}

		pNode = pNode->GetNextNode();
	}

	m_csCharacterType.ResetContent();

	/*
	for (i = AURACE_TYPE_NONE + 1; i < AURACE_TYPE_MAX; ++i)
	{
		for (int j = AUCHARCLASS_TYPE_NONE + 1; j < AUCHARCLASS_TYPE_MAX; ++j)
		{
			CHAR	*szBuffer	= m_astrBindingCharacterTypeName[(i - (AURACE_TYPE_NONE + 1)) * (AUCHARCLASS_TYPE_MAX - 1) + (j - (AUCHARCLASS_TYPE_NONE + 1))];

			INT32	lListIndex	= m_csCharacterType.AddString(szBuffer);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csCharacterType.SetItemData(lListIndex, i * 100 + j);
		}
	}
	*/

	for (int i = AURACE_TYPE_NONE + 1; i < AURACE_TYPE_MAX; ++i)
	{
		CHAR	*szBuffer	= m_astrBindingCharacterTypeName[i];

		INT32	lListIndex	= m_csCharacterType.AddString(szBuffer);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csCharacterType.SetItemData(lListIndex, i);
	}

	AgpdBindingAttach		*pcsAttachData	= (AgpdBindingAttach *)	m_pcsEvent->m_pvData;

	pcsAttachData->m_pcsBinding				= m_pcsAgpmEventBinding->GetBinding(pcsAttachData->m_lBindingID);
	if (pcsAttachData->m_pcsBinding)
	{
		m_ulRadius			= pcsAttachData->m_pcsBinding->m_ulRadius		;
		m_strBindingName	= pcsAttachData->m_pcsBinding->m_szBindingName	;

		m_csRegionName.SelectString(-1, pcsAttachData->m_pcsBinding->m_szTownName);
		m_csBindingType.SelectString(-1, m_astrBindingTypeName[pcsAttachData->m_pcsBinding->m_eBindingType]);

		INT32				lRace	= pcsAttachData->m_pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE];
		//INT32				lClass	= pcsAttachData->m_pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS];

		//m_csBindingType.SelectString(-1, m_astrBindingCharacterTypeName[(lRace - (AURACE_TYPE_NONE + 1)) * (AUCHARCLASS_TYPE_MAX - 1) + (lClass - (AUCHARCLASS_TYPE_NONE + 1))]);
		m_csCharacterType.SelectString(-1, m_astrBindingCharacterTypeName[lRace]);
	}
	else
	{
		m_ulRadius			= 0						;
		m_strBindingName	= "여기에 바인딩 네임..";
	}

	UpdateData(FALSE);

	return TRUE;
}

void AgcmEventBindingPoint::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	AgpdBindingAttach		*pcsAttachData	= (AgpdBindingAttach *) m_pcsEvent->m_pvData;

	AuPOS	stBasePos;
	ZeroMemory(&stBasePos, sizeof(AuPOS));
	
	m_pcsApmEventManager->GetBasePos(m_pcsEvent, &stBasePos);

	AgpdBindingType	eBindingType			= (AgpdBindingType) m_csBindingType.GetItemData(m_csBindingType.GetCurSel());

	CHAR			*szRegionName			= (CHAR *) m_csRegionName.GetItemData(m_csRegionName.GetCurSel());

	INT32			lSelectedCharacterType	= m_csCharacterType.GetItemData(m_csCharacterType.GetCurSel());

//	INT32			lRace					= lSelectedCharacterType / 100;
//	INT32			lClass					= lSelectedCharacterType % 100;

	INT32			lRace					= lSelectedCharacterType;
	INT32			lClass					= lSelectedCharacterType;

	pcsAttachData->m_pcsBinding				= m_pcsAgpmEventBinding->GetBinding(pcsAttachData->m_lBindingID);
	if (!pcsAttachData->m_pcsBinding)
	{
		pcsAttachData->m_pcsBinding			= m_pcsAgpmEventBinding->AddBinding((LPSTR) (LPCTSTR) m_strBindingName, szRegionName, &stBasePos, m_ulRadius, eBindingType);
		if (pcsAttachData->m_pcsBinding)
		{
			pcsAttachData->m_lBindingID		= pcsAttachData->m_pcsBinding->m_lID;

			pcsAttachData->m_pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE]		= lRace;
			pcsAttachData->m_pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS]	= lClass;
		}
	}
	else
	{
		strcpy(pcsAttachData->m_pcsBinding->m_szBindingName, m_strBindingName);
		pcsAttachData->m_pcsBinding->m_ulRadius		= m_ulRadius;
		pcsAttachData->m_pcsBinding->m_eBindingType	= eBindingType;
		pcsAttachData->m_pcsBinding->m_stBasePos	= stBasePos;
		strcpy(pcsAttachData->m_pcsBinding->m_szTownName, szRegionName);

		pcsAttachData->m_pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE]		= lRace;
		pcsAttachData->m_pcsBinding->m_csFactorCharType.lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS]	= lClass;
	}

	CDialog::OnOK();
}

void AgcmEventBindingPoint::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL AgcmEventBindingPoint::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// 마고자 (2004-09-20 오후 4:45:14) : 여기로 데이타 초기화 이사함..
	if (!InitEventData())
		return FALSE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
