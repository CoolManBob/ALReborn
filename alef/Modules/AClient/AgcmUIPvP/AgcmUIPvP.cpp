#include "AgcmUIPvP.h"
#include "AuStrTable.h"
#include "AgcmTextBoardMng.h"

AgcmUIPvP::AgcmUIPvP()
{
	SetModuleName("AgcmUIPvP");

	m_pcsAgcmChatting				= NULL;
	m_pcsAgpmItem					= NULL;

	m_pcsPvPFreeGridItem			= NULL;
	m_pcsPvPDangerGridItem			= NULL;

	m_bIsAddPvPFreeItem				= FALSE;
	m_bIsAddPvPDangerItem			= FALSE;

	m_pcsIsDisableInPvPDanger		= NULL;
	m_bIsDisableInPvPDanger			= FALSE;

	m_bIsInPvPDanger				= FALSE;
	m_llCurrentEXP					= 0;

	m_ulRaceBattleRemainedTime		= 0;
	m_ulPreviousClock				= 0;

/*	
중국종족전 삭제
	if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
		EnableIdle(TRUE);
	else
    */
		EnableIdle(FALSE);
}

AgcmUIPvP::~AgcmUIPvP()
{
}

BOOL AgcmUIPvP::OnAddModule()
{
	m_pcsApmMap				= (ApmMap *)			GetModule("ApmMap");
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgcmWorld			= (AgcmWorld *)			GetModule("AgcmWorld");
	m_pcsAgcmUIControl		= (AgcmUIControl *)		GetModule("AgcmUIControl");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsAgcmUIMain			= (AgcmUIMain *)		GetModule("AgcmUIMain");
	m_pcsAgcmUICharacter	= (AgcmUICharacter *)	GetModule("AgcmUICharacter");
	m_pcsAgcmChatting		= (AgcmChatting2 *)		GetModule("AgcmChatting2");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmPvP			= (AgpmPvP *)			GetModule("AgpmPvP");
	m_pcsAgpmGrid			= (AgpmGrid *)			GetModule("AgpmGrid");

	if (!m_pcsApmMap ||
		!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgcmWorld ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmUIControl ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmUIMain ||
		!m_pcsAgcmUICharacter ||
		!m_pcsAgcmChatting ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmPvP)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackBindingRegionChange(CBChangeRegionIndex, this))
		return FALSE;
	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorUI(CBUpdateExp, this))
		return FALSE;

	if (!m_pcsAgpmPvP->SetCallbackRecvCannotUseTeleport(CBRecvCannotUseTeleport, this))
		return FALSE;
	if (!m_pcsAgpmPvP->SetCallbackEnterSafePvPArea(CBEnterSafeArea, this))
		return FALSE;
	if (!m_pcsAgpmPvP->SetCallbackEnterFreePvPArea(CBEnterFreeArea, this))
		return FALSE;
	if (!m_pcsAgpmPvP->SetCallbackEnterCombatPvPArea(CBEnterCombatArea, this))
		return FALSE;
	if (!m_pcsAgpmPvP->SetCallbackSystemMessage(CBSystemMessage, this))
		return FALSE;
	if (!m_pcsAgpmPvP->SetCallbackRaceBattleStatus(CBRaceBattleStatus, this))
		return FALSE;

	if (!m_pcsAgcmUICharacter->SetCallbackUpdateResurrectStatus(CBUpdateResurrectStatus, this))
		return FALSE;

	m_lEventRecvCannotUseTeleport = m_pcsAgcmUIManager2->AddEvent("PvP_RecvCannotUseTeleport");
	if (m_lEventRecvCannotUseTeleport < 0)
		return FALSE;

	m_lEventRegionSafe = m_pcsAgcmUIManager2->AddEvent("PvP_RegionSafe");
	if (m_lEventRegionSafe < 0)
		return FALSE;
	m_lEventRegionPvPFree = m_pcsAgcmUIManager2->AddEvent("PvP_RegionPvPFree");
	if (m_lEventRegionPvPFree < 0)
		return FALSE;
	m_lEventRegionPvPDanger = m_pcsAgcmUIManager2->AddEvent("PvP_RegionPvPDanger");
	if (m_lEventRegionPvPDanger < 0)
		return FALSE;

	m_lEventStartPvP = m_pcsAgcmUIManager2->AddEvent("PvP_PvPStart");
	if (m_lEventStartPvP < 0)
		return FALSE;
	m_lEventEndPvP = m_pcsAgcmUIManager2->AddEvent("PvP_PvPEnd");
	if (m_lEventEndPvP < 0)
		return FALSE;

	if (!(m_pcsIsDisableInPvPDanger = m_pcsAgcmUIManager2->AddUserData("IsDisableInPvPDanger", &m_bIsDisableInPvPDanger, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_BOOL)))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIPvP::OnInit()
{
	InitBattleInfo();

	return TRUE;
}

BOOL AgcmUIPvP::OnDestroy()
{
	if (m_pcsPvPFreeGridItem)
	{
		delete m_pcsPvPFreeGridItem;
		m_pcsPvPFreeGridItem	= NULL;
	}

	if (m_pcsPvPDangerGridItem)
	{
		delete m_pcsPvPDangerGridItem;
		m_pcsPvPDangerGridItem	= NULL;
	}

	return TRUE;
}

BOOL AgcmUIPvP::OnIdle(UINT32 ulClockCount)
{
	if (ulClockCount > m_ulPreviousClock + AGCMUIPVP_IDLE_CLOCK_INTERVAL)
	{
		m_ulRaceBattleRemainedTime -= AGCMUIPVP_IDLE_CLOCK_INTERVAL;
		if (0 > m_ulRaceBattleRemainedTime)
			m_ulRaceBattleRemainedTime = 0;
		m_ulPreviousClock = ulClockCount;

		OnRaceBattleStatus(-1);
	}	

	return TRUE;
}

BOOL AgcmUIPvP::CBChangeRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIPvP				*pThis				= (AgcmUIPvP *)				pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	BOOL					bIsPvPFreeArea		= FALSE;
	BOOL					bIsPvPDangerArea	= FALSE;

	ApmMap::RegionTemplate	*pcsRegionTemplate	= pThis->m_pcsApmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
	if (pcsRegionTemplate)
	{
		if (pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_FREE)
		{
			bIsPvPFreeArea		= TRUE;
		}
		else if (pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_DANGER)
		{
			bIsPvPDangerArea	= TRUE;
		}
	}

	if (bIsPvPFreeArea)
	{
		if (!pThis->m_pcsPvPFreeGridItem && pThis->m_pcsAgpmGrid)
		{
			pThis->m_pcsPvPFreeGridItem	= pThis->m_pcsAgpmGrid->CreateGridItem();

			if (pThis->m_pcsPvPFreeGridItem)
			{
				pThis->m_pcsPvPFreeGridItem->m_eType	= AGPDGRID_ITEM_TYPE_ITEM;

				RwTexture **ppGridTexture	= pThis->m_pcsAgcmUIControl->GetAttachGridItemTextureData(pThis->m_pcsPvPFreeGridItem);
				if (ppGridTexture)
					*ppGridTexture	= pThis->m_pcsAgcmUIControl->m_pPvPFree;

				pThis->m_pcsPvPFreeGridItem->SetTooltip( "Free Area" );
			}
		}

		pThis->m_bIsAddPvPFreeItem		= pThis->m_pcsAgcmUIMain->AddSystemMessageGridItem(pThis->m_pcsPvPFreeGridItem, NULL, NULL);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRegionPvPFree);
	}
	else
	{
		if (pThis->m_pcsPvPFreeGridItem)
			pThis->m_pcsAgcmUIMain->RemoveSystemMessageGridItem(pThis->m_pcsPvPFreeGridItem);

		pThis->m_bIsAddPvPFreeItem		= FALSE;
	}

	if (bIsPvPDangerArea)
	{
		if (!pThis->m_pcsPvPDangerGridItem && pThis->m_pcsAgpmGrid)
		{
			pThis->m_pcsPvPDangerGridItem	= pThis->m_pcsAgpmGrid->CreateGridItem();

			if (pThis->m_pcsPvPDangerGridItem)
			{
				pThis->m_pcsPvPDangerGridItem->m_eType	= AGPDGRID_ITEM_TYPE_ITEM;

				RwTexture **ppGridTexture	= pThis->m_pcsAgcmUIControl->GetAttachGridItemTextureData(pThis->m_pcsPvPDangerGridItem);
				if (ppGridTexture)
					*ppGridTexture	= pThis->m_pcsAgcmUIControl->m_pPvPDanger;

				pThis->m_pcsPvPDangerGridItem->SetTooltip( "Danger Area" );
			}
		}

		pThis->m_bIsAddPvPDangerItem	= pThis->m_pcsAgcmUIMain->AddSystemMessageGridItem(pThis->m_pcsPvPDangerGridItem, NULL, NULL);

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRegionPvPDanger);

		pThis->m_bIsInPvPDanger	= TRUE;

		pThis->m_bIsDisableInPvPDanger	= FALSE;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsIsDisableInPvPDanger);
	}
	else
	{
		if (pThis->m_pcsPvPDangerGridItem)
			pThis->m_pcsAgcmUIMain->RemoveSystemMessageGridItem(pThis->m_pcsPvPDangerGridItem);

		pThis->m_bIsAddPvPDangerItem	= FALSE;

		pThis->m_bIsInPvPDanger			= FALSE;

		/*
		if (pThis->m_lCurrentEXP > 0)
		pThis->m_bIsDisableInPvPDanger	= TRUE;
		else
		pThis->m_bIsDisableInPvPDanger	= FALSE;
		*/

		pThis->m_bIsDisableInPvPDanger	= FALSE;

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsIsDisableInPvPDanger);
	}

	if (!bIsPvPFreeArea && !bIsPvPDangerArea)
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRegionSafe);

	return TRUE;
}

BOOL AgcmUIPvP::CBUpdateExp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdFactor*		pcsFactor =	(AgpdFactor*)pData		;
	AgcmUIPvP*	pThis	=	(AgcmUIPvP*)pClass	;

	// Self Character인지 체크 
	INT32	lCID = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	if ( NULL == pThis->m_pcsAgpmFactors ) return TRUE;

	pThis->m_pcsAgpmFactors->GetValue( pcsFactor, &lCID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID );
	if ( pThis->m_pcsAgcmCharacter->m_lSelfCID != lCID ) return TRUE;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return TRUE;

	pThis->m_llCurrentEXP	= pThis->m_pcsAgpmFactors->GetExp((AgpdFactor *) pThis->m_pcsAgpmFactors->GetFactor(&pcsSelfCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));

	/*
	if (!pThis->m_bIsInPvPDanger && pThis->m_lCurrentEXP > 0)
	{
	pThis->m_bIsDisableInPvPDanger	= TRUE;
	}
	else
	{
	pThis->m_bIsDisableInPvPDanger	= FALSE;
	}
	*/

	pThis->m_bIsDisableInPvPDanger	= FALSE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsIsDisableInPvPDanger);

	return TRUE;
}

BOOL AgcmUIPvP::CBRecvCannotUseTeleport(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIPvP		*pThis			= (AgcmUIPvP *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventRecvCannotUseTeleport);

	return TRUE;
}

BOOL AgcmUIPvP::CBEnterSafeArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIPvP* pThis = (AgcmUIPvP*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if(pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	return pThis->m_pcsAgpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_ENTER_SAFE_AREA);
}

BOOL AgcmUIPvP::CBEnterFreeArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIPvP* pThis = (AgcmUIPvP*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if(pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	return pThis->m_pcsAgpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_ENTER_FREE_AREA);
}

BOOL AgcmUIPvP::CBEnterCombatArea(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIPvP* pThis = (AgcmUIPvP*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if(pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	return pThis->m_pcsAgpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_ENTER_COMBAT_AREA);
}

BOOL AgcmUIPvP::CBUpdateResurrectStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIPvP		*pThis			= (AgcmUIPvP *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	/*
	if (pThis->m_bIsDisableInPvPDanger)
	{
	if (pThis->m_pcsAgpmCharacter->GetExp(pcsCharacter) <= 0)
	pThis->m_bIsDisableInPvPDanger	= FALSE;
	}
	*/

	pThis->m_bIsDisableInPvPDanger	= FALSE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsIsDisableInPvPDanger);

	return TRUE;
}


BOOL AgcmUIPvP::CBRaceBattleStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIPvP *pThis = (AgcmUIPvP *) pClass;
	INT8 cPrevStatus = *((INT8 *) pData);
	INT32 lRemained= *((INT32 *) pCustData);

	return pThis->OnRaceBattleStatus(lRemained);
}


BOOL AgcmUIPvP::OnRaceBattleStatus(INT32 lRemained)
{	
	if (-1 != lRemained)
		m_ulRaceBattleRemainedTime = lRemained * 1000;

	switch (m_pcsAgpmPvP->GetRaceBattleStatus())
	{
	case AGPMPVP_RACE_BATTLE_STATUS_READY:
		DrawBattleInfoTime();
		break;

	case AGPMPVP_RACE_BATTLE_STATUS_ING:
		DrawBattleInfoTime();
		DrawBattleInfoScore();
		break;

	case AGPMPVP_RACE_BATTLE_STATUS_ENDED:
		m_pcsAgpmPvP->SetRaceBattleStatus(AGPMPVP_RACE_BATTLE_STATUS_REWARD);
		DrawBattleInfoResult();
		break;

	case AGPMPVP_RACE_BATTLE_STATUS_REWARD:
		break;

	case AGPMPVP_RACE_BATTLE_STATUS_NONE:
	default:
		break;

	}

	return TRUE;
}


void AgcmUIPvP::InitBattleInfo()
{
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText(_T(""), 0, 0, FALSE, 0, 0.0f, 0, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectAboveText(_T(""), 0, 0, FALSE, 0, 0.0f);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectSubText(_T(""), 0, 0, FALSE, 0, 0.0f);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow2.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow3.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow4.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow5.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow6.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow7.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
}


BOOL AgcmUIPvP::DrawBattleInfoResult()
{
	InitBattleInfo();			// init. time and point

	INT32 lFont	= 3;
	INT32 lOffsetX = 5;
	INT32 lOffsetY = 230;
	INT32 lDuration	= 5000;

	FLOAT fScale = 1.05f;

	UINT32 uColor = 0xFFFF6600;

	CHAR szMessage[255];
	ZeroMemory(szMessage, sizeof(szMessage));

	CHAR *pszRace = NULL;
	INT32 lRace = m_pcsAgpmPvP->GetRaceBattleWinner();
	switch (lRace)
	{	
	case AURACE_TYPE_HUMAN:
		pszRace = ClientStr().GetStr(STI_RACE_BATTLE_HUMAN);
		break;

	case AURACE_TYPE_ORC:
		pszRace = ClientStr().GetStr(STI_RACE_BATTLE_ORC);
		break;

	case AURACE_TYPE_MOONELF:
		pszRace = ClientStr().GetStr(STI_RACE_BATTLE_MOONELF);
		break;

	case AURACE_TYPE_DRAGONSCION:
		pszRace = ClientStr().GetStr(STI_DRAGONSCION);
		break;

	default:
		break;
	}

	if (!pszRace)
		return FALSE;

	sprintf(szMessage, ClientStr().GetStr(STI_RACE_BATTLE_RESULT), pszRace);
	if(strlen(szMessage) > 0)
	{
		m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow2.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor,
			fScale, lDuration, TRUE, 0);	// 마지막 Fade Delay 를 0 으로
	}

	return TRUE;
}


BOOL AgcmUIPvP::DrawBattleInfoTime()
{
	INT32 lFont	= 3;
	INT32 lOffsetX = 5;
	INT32 lOffsetY = 230;
	INT32 lDuration	= 0x7FFFFFFF;		// 이거 시간은 무한대루 준다. No-Fade

	FLOAT fScale = 1.05f;

	UINT32 uColor = 0xFFFF6600;

	CHAR szMessage[255];
	ZeroMemory(szMessage, sizeof(szMessage));

	INT32 lTotalRemainSec = (INT32)(m_ulRaceBattleRemainedTime / 1000);
	INT32 lRemainTimeMin = (INT32)(lTotalRemainSec / 60);
	INT32 lRemainTimeSec = (INT32)(lTotalRemainSec % 60);
	sprintf(szMessage, ClientStr().GetStr(STI_RACE_BATTLE_TIME), lRemainTimeMin, lRemainTimeSec);
	if(strlen(szMessage) > 0)
	{
		m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow2.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor,
			fScale, lDuration, TRUE, 0);	// 마지막 Fade Delay 를 0 으로
	}

	return TRUE;
}


BOOL AgcmUIPvP::DrawBattleInfoScore()
{
	INT32 lFont	= 3;
	INT32 lOffsetX = 10;
	INT32 lOffsetY = 300;
	INT32 lDuration	= 0x7FFFFFFF;		// 이거 시간은 무한대루 준다. No-Fade

	FLOAT fScale = 1.05f;

	UINT32 uColor = 0xFFFFFF00;

	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(szMessage));

	AgpdCharacter *pAgpdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pAgpdCharacter)
		return FALSE;

	INT32 lRace = m_pcsAgpmFactors->GetRace(&pAgpdCharacter->m_csFactor);
	if (AURACE_TYPE_NONE >= lRace || lRace >= AURACE_TYPE_MAX)
		return FALSE;

	CHAR *pszRace[AURACE_TYPE_MAX];
	pszRace[AURACE_TYPE_HUMAN] = ClientStr().GetStr(STI_RACE_BATTLE_HUMAN);
	pszRace[AURACE_TYPE_ORC] = ClientStr().GetStr(STI_RACE_BATTLE_ORC);
	pszRace[AURACE_TYPE_MOONELF] = ClientStr().GetStr(STI_RACE_BATTLE_MOONELF);
	pszRace[AURACE_TYPE_DRAGONSCION] = ClientStr().GetStr(STI_DRAGONSCION);	

	uColor = 0xFF00FF00;	// 녹색
	INT32 lTargetRace = lRace;
	sprintf(szMessage, "%s:%d", pszRace[lTargetRace], m_pcsAgpmPvP->m_lRaceBattlePoint[lTargetRace]);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow3.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor,
		fScale, lDuration, TRUE, 0);	// 마지막 Fade Delay 를 0으로

	uColor = 0xFFFF0000;	// 빨강
	lOffsetY += 45;
	lTargetRace = (lRace + 1) % AURACE_TYPE_MAX;
	if (AURACE_TYPE_NONE == lTargetRace)
		lTargetRace++;
	sprintf(szMessage, "%s:%d", pszRace[lTargetRace], m_pcsAgpmPvP->m_lRaceBattlePoint[lTargetRace]);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow4.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor,
		fScale, lDuration, TRUE, 0);	// 마지막 Fade Delay 를 0으로

	uColor = 0xFFFF0000;	// 빨강
	lOffsetY += 45;
	lTargetRace = (lTargetRace + 1) % AURACE_TYPE_MAX;
	if (AURACE_TYPE_NONE == lTargetRace)
		lTargetRace++;	
	sprintf(szMessage, "%s:%d", pszRace[lTargetRace], m_pcsAgpmPvP->m_lRaceBattlePoint[lTargetRace]);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow5.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor,
		fScale, lDuration, TRUE, 0);	// 마지막 Fade Delay 를 0으로

	return TRUE;
}


BOOL AgcmUIPvP::CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdPvPSystemMessage* pstSystemMessage = (AgpdPvPSystemMessage*)pData;
	AgcmUIPvP* pThis = (AgcmUIPvP*)pClass;

	CHAR* szUIMessage = NULL;
	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(CHAR) * 255);
	DWORD dwColor = 0XFFFFFF00;		// ARGB (UI쪽에 뿌리는 건 ARGB)

	switch(pstSystemMessage->m_lCode)
	{
	case AGPMPVP_SYSTEM_CODE_CHARISMA_UP:
		{
			INT32 lPoint = pstSystemMessage->m_alData[0];
			INT32 lLevelDiff = pstSystemMessage->m_alData[1];
			
			AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			if( pcsSelfCharacter )
			{
				static AgcmTextBoardMng* s_pAgcmTextBoardMng = (AgcmTextBoardMng *)pThis->GetModule("AgcmTextBoardMng");
				if( s_pAgcmTextBoardMng )
				{
					int nID;
					if( lLevelDiff < 0 )		nID = 0;
					else if( lLevelDiff == 0 )	nID = 1;
					else						nID = 2;

					sBoardAttr  Attr;
					Attr.eType   = (eBoardType)(IMOTICON_CHARISMA1+nID);   Attr.ViewCount = 800;
					s_pAgcmTextBoardMng->CreateBoard( pcsSelfCharacter->m_lID , &Attr );
				}
			}
			
			CHAR szBuffer[256];
			CHAR *pszFormat = pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_CHARISMA_GET_POINT );
			if (pszFormat)
			{
				sprintf(szBuffer, pszFormat, lPoint);
				SystemMessage.ProcessSystemMessage(szBuffer, 0xffcc00);
			}
		}
		break;

	case AGPMPVP_SYSTEM_CODE_ENTER_SAFE_AREA:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_SAFE_AREA);
		dwColor = 0xFFFF9900;
		break;

	case AGPMPVP_SYSTEM_CODE_ENTER_FREE_AREA:
		dwColor = 0xFFFF9900;
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA), dwColor);
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA_2))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA_2), dwColor);

		if (NULL == pThis->m_pcsAgcmWorld->m_pAgpdWorldSelected
			|| FALSE == pThis->m_pcsAgcmWorld->m_pAgpdWorldSelected->IsNC17())
		{
			if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA_3))
				SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA_3), dwColor);
			if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA_4))
				SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_FREE_AREA_4), dwColor);
		}

		break;

	case AGPMPVP_SYSTEM_CODE_ENTER_COMBAT_AREA:
		dwColor = 0xFFFF9900;
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA), dwColor);

		if (NULL == pThis->m_pcsAgcmWorld->m_pAgpdWorldSelected
			|| FALSE == pThis->m_pcsAgcmWorld->m_pAgpdWorldSelected->IsNC17())
		{
			if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_2))
				SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_2), dwColor);
			if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_3))
				SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_3), dwColor);
		}
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_4))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_4), dwColor);
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_5))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_5), dwColor);
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_6))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ENTER_COMBAT_AREA_6), dwColor);
		break;

	case AGPMPVP_SYSTEM_CODE_NOW_PVP_STATUS:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_NOW_PVP_STATUS);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_NONE_PVP_STATUS:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_NONE_PVP_STATUS);
		dwColor = 0xFF00CC99;
		break;

	case AGPMPVP_SYSTEM_CODE_DEAD:
		if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DEAD))
			break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DEAD), pstSystemMessage->m_aszData[0]);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_DEAD_GUILD_MEMBER:
		if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DEAD_GUILD_MEMBER))
			break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DEAD_GUILD_MEMBER),
			pstSystemMessage->m_aszData[0], pstSystemMessage->m_aszData[1]);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_DEAD_PARTY_MEMBER:
		if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DEAD_PARTY_MEMBER))
			break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DEAD_PARTY_MEMBER),
			pstSystemMessage->m_aszData[0], pstSystemMessage->m_aszData[1]);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_ITEM_DROP:
		{
			if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_DROP))
				break;

			AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pstSystemMessage->m_alData[0]);
			if(!pcsItemTemplate)
				break;

			sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_DROP), pcsItemTemplate->m_szName);
			dwColor = 0xFFCC0000;
			break;
		}

	case AGPMPVP_SYSTEM_CODE_ITEM_ANTI_DROP:
		{
			if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ANTI_DROP))
				break;

			AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pstSystemMessage->m_alData[0]);
			if(!pcsItemTemplate)
				break;

			sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_ANTI_DROP), pcsItemTemplate->m_szName);
			dwColor = 0xFFCC0000;
			break;
		}

	case AGPMPVP_SYSTEM_CODE_START_INVINCIBLE:
		if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_START_INVINCIBLE))
			break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_START_INVINCIBLE), AGPMPVP_INVINCIBLE_TIME  / 1000);
		dwColor = 0xFFFF9900;
		break;

	case AGPMPVP_SYSTEM_CODE_END_INVINCIBLE:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_END_INVINCIBLE);
		dwColor = 0xFF00CC99;
		break;

	case AGPMPVP_SYSTEM_CODE_CANNOT_USE_TELEPORT:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_USE_TELEPORT);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_KILL_PLAYER:
		if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_KILL_PLAYER))
			break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_KILL_PLAYER), pstSystemMessage->m_aszData[0]);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_MOVE_BATTLE_SQAURE:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_MOVE_BATTLE_SQAURE);
		dwColor = 0xFF00CC99;
		break;

	case AGPMPVP_SYSTEM_CODE_LEAVE_GUILD_OR_PARTY:
		dwColor = 0xFFCC0000;
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LEAVE_GUILD_OR_PARTY))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LEAVE_GUILD_OR_PARTY), dwColor);
		if(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LEAVE_GUILD_OR_PARTY_2))
			SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LEAVE_GUILD_OR_PARTY_2), dwColor);
		break;

	case AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_GUILD:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_INVITE_GUILD);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_PARTY:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_INVITE_PARTY);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_TARGET_INVINCIBLE:
		{
			AgpmBattleGround* ppmBattleGround = ( AgpmBattleGround* )g_pEngine->GetModule( "AgpmBattleGround" );
			AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
			AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
			if( !ppmBattleGround || !pcmCharacter || !ppmCharacter ) return FALSE;

			AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
			if( !ppdSelfCharacter ) return FALSE;

			int nTargetCharacterID = pcmCharacter->GetSelectTarget( ppdSelfCharacter );
			AgpdCharacter* ppdTarget = ppmCharacter->GetCharacter( nTargetCharacterID );
			if( !ppdTarget ) return FALSE;

			char* pFormat = pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_TARGET_INVINCIBLE );
			if( !pFormat || strlen( pFormat ) <= 0 ) return FALSE;

			memset( szMessage, 0, sizeof( char ) * 255 );
			sprintf_s( szMessage, sizeof( char ) * 255, pFormat, ppmBattleGround->IsInBattleGround( ppdTarget ) ? "****" : ppdTarget->m_szID );
			return SystemMessage.ProcessSystemMessage(szMessage, dwColor);
		}
		break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TARGET_INVINCIBLE), pstSystemMessage->m_aszData[0]);
		dwColor = 0xFFFF9900;
		break;

	case AGPMPVP_SYSTEM_CODE_NOT_ENOUGH_LEVEL:
		if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_NOT_ENOUGH_LEVEL))
			break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_NOT_ENOUGH_LEVEL), AGPMPVP_BATTLE_SQAURE_REQUIRE_LEVEL);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_MEMBER:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_INVITE_MEMBER);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_FRIEND:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_ATTACK_FRIEND);
		dwColor = 0xFF00CC99;
		break;

	case AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_OTHER:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_ATTACK_OTHER);
		dwColor = 0xFF00CC99;
		break;

	case AGPMPVP_SYSTEM_CODE_SKILL_CANNOT_APPLY_EFFECT:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_SKILL_CANNOT_APPLY_EFFECT);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL:
		if(!pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_PVP_BY_LEVEL))
			break;

		sprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CANNOT_PVP_BY_LEVEL), pstSystemMessage->m_alData[0]);
		dwColor = 0xFFCC0000;
		break;

	case AGPMPVP_SYSTEM_CODE_DISABLE_NORMAL_ATTACK:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DISABLE_NORMAL_ATTACK);
		dwColor = 0xFF00CC99;
		break;

	case AGPMPVP_SYSTEM_CODE_DISABLE_SKILL_CAST:
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_DISABLE_SKILL_CAST);
		dwColor = 0xFF00CC99;
		break;
	}

	if(szUIMessage)
		SystemMessage.ProcessSystemMessage(szUIMessage, dwColor);
	else if(strlen(szMessage) > 0)
		SystemMessage.ProcessSystemMessage(szMessage, dwColor);

	return TRUE;
}

// System Message 를 밀어넣는다.
BOOL AgcmUIPvP::AddSystemMessage(CHAR* szMsg, DWORD dwColor)
{
	if(!szMsg)
		return FALSE;

	m_pcsAgcmChatting->AddSystemMessage(szMsg, dwColor);
	m_pcsAgcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}