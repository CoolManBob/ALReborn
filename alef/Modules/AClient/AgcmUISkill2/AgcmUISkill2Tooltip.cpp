#include "AgcmUISkill2.h"
#include "AuStrTable.h"
#include "AgcmUILogin.h"

#define AGCM_UISKILL_NEXT_LEVEL_START_X	220

BOOL AgcmUISkill2::CBOpenSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;

	return pThis->OpenSkillToolTip(pcsSourceControl, AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL);
}

BOOL AgcmUISkill2::CBOpenButtonSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISkill2			*pThis				= (AgcmUISkill2 *)	pClass;

	AgcdUIControl			*pcsControl			= (AgcdUIControl *)	pData1;

	AcUIGrid				*pGrid				= (AcUIGrid *) (pcsControl->m_pcsBase);

	pGrid->m_pToolTipAgpdGridItem	= pThis->m_pcsAgpmGrid->GetItem(pGrid->m_pAgpdGrid + pcsSourceControl->m_lUserDataIndex, 0, 0, 0);

	return pThis->OpenSkillToolTip(pcsControl, AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL);
}

BOOL AgcmUISkill2::CBOpenBuySkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;
	AcUIGrid				*pGrid			= (AcUIGrid*)( pcsSourceControl->m_pcsBase );

	if (!pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgcmUISkill2_TooltipType eTooltipType	= AGCMUI_SKILL2_TOOLTIP_TYPE_BUY;

	if (pGrid->m_pToolTipAgpdGridItem->m_lItemID != AP_INVALID_SKILLID &&
		pThis->m_pcsAgpmSkill->GetSkill(pGrid->m_pToolTipAgpdGridItem->m_lItemID))
		eTooltipType	= AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE;

	return pThis->OpenSkillToolTip(pcsSourceControl, eTooltipType);
}

BOOL AgcmUISkill2::CBOpenBuyButtonSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISkill2			*pThis				= (AgcmUISkill2 *)	pClass;
	AgcdUIControl			*pcsControl			= (AgcdUIControl *)	pData1;
	AcUIGrid				*pGrid				= (AcUIGrid *) (pcsControl->m_pcsBase);

	pGrid->m_pToolTipAgpdGridItem	= pThis->m_pcsAgpmGrid->GetItem(pGrid->m_pAgpdGrid + pcsSourceControl->m_lUserDataIndex, 0, 0, 0);

	if (!pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	AgcmUISkill2_TooltipType eTooltipType	= AGCMUI_SKILL2_TOOLTIP_TYPE_BUY;

	if (pGrid->m_pToolTipAgpdGridItem->m_lItemID != AP_INVALID_SKILLID &&
		pThis->m_pcsAgpmSkill->GetSkill(pGrid->m_pToolTipAgpdGridItem->m_lItemID))
		eTooltipType	= AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE;

	return pThis->OpenSkillToolTip(pcsControl, eTooltipType);
}

BOOL AgcmUISkill2::CBOpenUpgradeSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;

	return pThis->OpenSkillToolTip(pcsSourceControl, AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE);
}

BOOL AgcmUISkill2::CBOpenUpgradeButtonSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISkill2			*pThis				= (AgcmUISkill2 *)	pClass;
	AgcdUIControl			*pcsControl			= (AgcdUIControl *)	pData1;
	AcUIGrid				*pGrid				= (AcUIGrid *) (pcsControl->m_pcsBase);

	pGrid->m_pToolTipAgpdGridItem	= pThis->m_pcsAgpmGrid->GetItem(pGrid->m_pAgpdGrid + pcsSourceControl->m_lUserDataIndex, 0, 0, 0);

	return pThis->OpenSkillToolTip(pcsControl, AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE);
}

BOOL AgcmUISkill2::OpenSkillToolTip(AgcdUIControl *pcsSourceControl, AgcmUISkill2_TooltipType eTooltipType)
{
	if (!pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;
	
	AcUIGrid*	pGrid = (AcUIGrid*)( pcsSourceControl->m_pcsBase );

	if (!pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL)
		return FALSE;

	m_csSkillToolTip.DeleteAllStringInfo();
	m_csSkillToolTip.MoveWindow((INT32) m_pcsAgcmUIManager2->m_v2dCurMousePos.x + 30, (INT32) m_pcsAgcmUIManager2->m_v2dCurMousePos.y + 30,
								m_csSkillToolTip.w, m_csSkillToolTip.h);

	if (pGrid->m_pToolTipAgpdGridItem->m_lItemID != AP_INVALID_SKILLID &&
		m_pcsAgpmSkill->GetSkill(pGrid->m_pToolTipAgpdGridItem->m_lItemID))
		SetSkillToolTipInfo(pGrid->m_pToolTipAgpdGridItem->m_lItemID, eTooltipType);
	else
		SetSkillTemplateToolTipInfo(pGrid->m_pToolTipAgpdGridItem->m_lItemTID, eTooltipType);

	m_csSkillToolTip.ShowWindow( TRUE );

	return TRUE;
}

BOOL AgcmUISkill2::CloseSkillToolTip( VOID )
{
	m_csSkillToolTip.ShowWindow( FALSE );
	m_csSkillToolTip.DeleteAllStringInfo();

	return TRUE;
}

BOOL AgcmUISkill2::CBCloseSkillToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;

	pThis->m_csSkillToolTip.ShowWindow(FALSE);
	pThis->m_csSkillToolTip.DeleteAllStringInfo();

	return TRUE;
}

BOOL AgcmUISkill2::CBOpenQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;

	return pThis->CBOpenSkillToolTip(pClass, NULL, NULL, NULL, NULL, NULL, NULL, (AgcdUIControl *) pData);
}

BOOL AgcmUISkill2::CBCloseQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;

	return pThis->CBCloseSkillToolTip(pClass, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

BOOL AgcmUISkill2::CBCloseAllUIToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;

	pThis->m_csSkillToolTip.ShowWindow(FALSE);
	pThis->m_csSkillToolTip.DeleteAllStringInfo();

	pThis->m_csMasteryToolTip.ShowWindow(FALSE);
	pThis->m_csMasteryToolTip.DeleteAllStringInfo();

	return TRUE;
}

//	Tooltip Setting
/////////////////////////////////////////////////////////////////////////////////////////
BOOL AgcmUISkill2::SetSkillToolTipInfo(INT32 lSkillID, AgcmUISkill2_TooltipType eTooltipType)
{
	AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkill(lSkillID);
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return FALSE;

	m_csSkillToolTip.DeleteAllStringInfo();

	SetSkillToolTipName((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, TRUE);
//	SetSkillToolTipAttribute((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, TRUE);
	if (eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_BUY)
		SetSkillToolTipBasicAttribute(pcsSkill, (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, TRUE);
	SetSkillToolTipDescription((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, TRUE);

	if (m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkill))
	{
		SetSkillToolTipCurrentLevel(pcsSkill, eTooltipType);

	//	if (eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE)
			SetSkillToolTipNextLevel(pcsSkill, (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate, eTooltipType);

		if (eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE)
			SetSkillToolTipUpgradeInfo(pcsSkill);
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillTemplateToolTipInfo(INT32 lSkillTID, AgcmUISkill2_TooltipType eTooltipType)
{
	AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(lSkillTID);
	if (!pcsSkillTemplate)
		return FALSE;

	m_csSkillToolTip.DeleteAllStringInfo();

	SetSkillToolTipName(pcsSkillTemplate, TRUE);
//	SetSkillToolTipAttribute(pcsSkillTemplate, TRUE);
	if (eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_BUY)
		SetSkillToolTipBasicAttribute(NULL, pcsSkillTemplate, TRUE);
	SetSkillToolTipDescription(pcsSkillTemplate, TRUE);

	if (m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate))
	{
	//	SetSkillToolTipCurrentLevel(pcsSkill, eTooltipType);
	//
	//	if (eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE)
			SetSkillToolTipNextLevel(NULL, pcsSkillTemplate, eTooltipType);

	//	if (eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE)
	//		SetSkillToolTipUpgradeInfo(pcsSkill);
	}

	return TRUE;
}

//	세부 툴팁 세팅
/////////////////////////////////////////////////////////////////////////////////////////

BOOL AgcmUISkill2::SetSkillToolTipName(AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive)
{
	if (!pcsSkillTemplate)
		return FALSE;

	DWORD	dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NORMAL);

	if (!bIsActive)
		dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_DISABLE);

	// 스킬 이름
	m_csSkillToolTip.AddString( pcsSkillTemplate->m_szName, 14, dwColor, 4 );

	m_csSkillToolTip.AddNewLine( 14 );
	m_csSkillToolTip.AddNewLine( 14 );

	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 32);

	// 고레벨 스킬은 따로 논다. 2007.07.31. steeple
	if(m_pcsAgpmSkill->IsHighLevelSkill(pcsSkillTemplate))
	{
		sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_MAX_HIGHLEVEL_SKILL), ClientStr().GetStr(STI_SKILL));
		m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
		m_csSkillToolTip.AddNewLine( 14 );
	}
	else if( m_pcsAgpmSkill->IsHeroicSkill(pcsSkillTemplate))
	{
		sprintf(szBuffer, "%s %s" , ClientStr().GetStr(STI_HEROIC), ClientStr().GetStr(STI_SKILL));
		m_csSkillToolTip.AddString( szBuffer , 12 , dwColor );
		m_csSkillToolTip.AddNewLine( 14 );
	}
	else
	{
		INT32	lMasteryIndex	= m_pcsAgpmEventSkillMaster->GetMasteryIndex(pcsSkillTemplate);

		switch (lMasteryIndex)
		{
			case 0:
				sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_COMBAT), ClientStr().GetStr(STI_SKILL) );
				m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
				m_csSkillToolTip.AddNewLine( 14 );
				break;
			case 1:
				sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_REINFORCE), ClientStr().GetStr(STI_SKILL));
				m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
				m_csSkillToolTip.AddNewLine( 14 );
				break;
			case 2:
				sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_SPECIFIC), ClientStr().GetStr(STI_SKILL));
				m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
				m_csSkillToolTip.AddNewLine( 14 );
				break;
			case 3:
				sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_PASSIVE), ClientStr().GetStr(STI_SKILL));
				m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
				m_csSkillToolTip.AddNewLine( 14 );
				break;
			case 4:
				sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_PRODUCE), ClientStr().GetStr(STI_SKILL));
				m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
				m_csSkillToolTip.AddNewLine( 14 );
				break;
			case AGPMEVENT_SKILL_ARCHLORD_MASTERY_INDEX:
			{
				sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_ARCHLORD), ClientStr().GetStr(STI_SKILL));
				m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
				m_csSkillToolTip.AddNewLine( 14 );

				if(m_pcsAgpmSkill->IsPassiveSkill(pcsSkillTemplate))
				{
					sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_PASSIVE), ClientStr().GetStr(STI_SKILL));
					m_csSkillToolTip.AddString( szBuffer, 12, dwColor );
					m_csSkillToolTip.AddNewLine( 14 );
				}

				break;
			}
		}
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipAttribute(AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive)
{
	if (!pcsSkillTemplate)
		return FALSE;

	m_csSkillToolTip.AddNewLine( 14 );

	DWORD	dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NORMAL);

	if (!bIsActive)
		dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_DISABLE);

	if (pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PASSIVE)
		m_csSkillToolTip.AddString( ClientStr().GetStr(STI_PASSIVE_SKILL), 12, dwColor );
	else
		m_csSkillToolTip.AddString( ClientStr().GetStr(STI_ACTIVE_SKILL), 12, dwColor );

	m_csSkillToolTip.AddNewLine( 14 );

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipDescription(AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive)
{
	if (!pcsSkillTemplate)
		return FALSE;

	m_csSkillToolTip.AddNewLine( 14 );

	DWORD	dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NORMAL);

	if (!bIsActive)
		dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_DISABLE);

	char szBuffer[256];
	ZeroMemory(szBuffer, sizeof(CHAR) * 256);

	AgpdSkillTooltipTemplate	*pcsSkillTooltipTemplate	= m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
	if (!pcsSkillTooltipTemplate)
		return FALSE;

	// modified by laki
	// for line feed
	
	// 기술 설명
	//m_csSkillToolTip.AddString( pcsSkillTooltipTemplate->m_szSkillDescription, 12, dwColor);
	//m_csSkillToolTip.AddNewLine( 14 );

	CHAR *pszDesc = pcsSkillTooltipTemplate->m_szSkillDescription;
	CHAR *psz = szBuffer;

	while (TRUE)
	{
	if (_T('\0') == *pszDesc)
		{
			*psz = _T('\0');
			if (*psz != szBuffer[0])
			{
				m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
				m_csSkillToolTip.AddNewLine(14);
			}
			break;
		}
		// meet new line
		else if (_T('\\') == *pszDesc && _T('n') == *(pszDesc+1))
		{
			*psz = _T('\0');
			pszDesc += 2;
			psz = szBuffer;
			
			m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
			m_csSkillToolTip.AddNewLine(14);
		}
		else
			*psz++ = *pszDesc++;
	}

	// 무기 장착 필수
	if (pcsSkillTemplate->m_lRequirementType & AGPMSKILL_REQUIREMENT_EQUIP_WEAPON)
	{
		m_csSkillToolTip.AddString(ClientStr().GetStr(STI_NEED_WEAPON), 12, dwColor);
		m_csSkillToolTip.AddNewLine( 14 );
	}

	// 방패 장착 필수
	if (pcsSkillTemplate->m_lRequirementType & AGPMSKILL_REQUIREMENT_EQUIP_SHIELD)
	{
		m_csSkillToolTip.AddString(ClientStr().GetStr(STI_NEED_SHIELD), 12, dwColor);
		m_csSkillToolTip.AddNewLine( 14 );
	}

	if (pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT)
	{
		CHAR sz[128];
		INT32 lItemTID = pcsSkillTemplate->m_stConditionArg[0].lArg1;
		AgpdItemTemplate *pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(lItemTID);
		if (pcsItemTemplate)
			{
			m_csSkillToolTip.AddNewLine( 14 );
			sprintf(sz, "%s : %s", ClientStr().GetStr(STI_NEED_STUFF), pcsItemTemplate->m_szName);
			m_csSkillToolTip.AddString(sz, 12, dwColor);
			m_csSkillToolTip.AddNewLine( 14 );
			}
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipCurrentLevel(AgpdSkill *pcsSkill, AgcmUISkill2_TooltipType eTooltipType)
{
	if (!pcsSkill)
		return FALSE;

	DWORD	dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NORMAL);

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	lSkillLevel	= 0;
	m_pcsAgpmFactors->GetValue(&pcsSkill->m_csFactor, &lSkillLevel, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL);

	if (lSkillLevel <= 0)
		return FALSE;

	// Modified Skill Level 을 얻어온다.
	// Heroic Skill은 얻어오지 않는다.
	INT32 lModifiedSkillLevel = 0;
	if( !m_pcsAgpmEventSkillMaster->IsHeroicSkill( pcsSkillTemplate->m_lID ) )
	{
		if(m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate) &&
			!m_pcsAgpmSkill->IsProductSkill(pcsSkillTemplate) &&
			!m_pcsAgpmSkill->IsArchlordSkill(pcsSkillTemplate))
		{
			lModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel(m_pcsAgcmCharacter->GetSelfCharacter());
			lModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkillTemplate->m_lID);

			lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
		}
	}

	AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate	= m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
	if (!pcsSkillTooltipTemplate)
		return FALSE;

	if (eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL ||
		eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE)
	{
		m_lTooltipLineIndex	= m_csSkillToolTip.GetLinePos();
		
		m_csSkillToolTip.AddNewLine( 14 );

		CHAR	szBuffer[64];
		ZeroMemory(szBuffer, sizeof(CHAR) * 64);

		if(lModifiedSkillLevel == 0)
		{
			sprintf(szBuffer, "%s : %d", ClientStr().GetStr(STI_CURRENT_GRADE), lSkillLevel);
		}
		else
		{
			sprintf(szBuffer, "%s : %d (+%d)", ClientStr().GetStr(STI_CURRENT_GRADE), lSkillLevel + lModifiedSkillLevel, lModifiedSkillLevel);
		}

		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
	}

	return SetSkillToolTipInfo(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel + lModifiedSkillLevel, dwColor, eTooltipType);
}

BOOL AgcmUISkill2::SetSkillToolTipNextLevel(AgpdSkill *pcsSkill, AgpdSkillTemplate *pcsSkillTemplate, AgcmUISkill2_TooltipType eTooltipType)
{
	if (!pcsSkillTemplate)
		return FALSE;

	AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate	= m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
	if (!pcsSkillTooltipTemplate)
		return FALSE;

	if (pcsSkill)
	{
		if (m_pcsAgpmEventSkillMaster->IsFullUpgrade(pcsSkill))
		{
			m_csSkillToolTip.AddNewLine( 14 );
			return TRUE;
		}

		INT32	lSkillLevel		= m_pcsAgpmSkill->GetSkillLevel(pcsSkill);

		// Modified Skill Level 을 얻어온다.
		// 히로익 스킬이 아닐경우만 증가된 스킬레벨을 구해온다
		INT32 lModifiedSkillLevel = 0;
		if( !m_pcsAgpmEventSkillMaster->IsHeroicSkill(pcsSkillTemplate->m_lID) )
		{
			if(m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate) &&
				!m_pcsAgpmSkill->IsProductSkill(pcsSkillTemplate) &&
				!m_pcsAgpmSkill->IsArchlordSkill(pcsSkillTemplate))
			{
				lModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel(m_pcsAgcmCharacter->GetSelfCharacter());
				lModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkillTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}

		AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= m_pcsAgpmEventSkillMaster->GetSkillAttachTemplateData((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate);

		INT32	lSkillMaxLevel	= pcsAttachTemplateData->lMaxUpgradeLevel;
		if (lSkillLevel + lModifiedSkillLevel >= lSkillMaxLevel)
			return TRUE;

		m_csSkillToolTip.SetLinePos(m_lTooltipLineIndex);

		m_csSkillToolTip.AddNewLine( 14 );

		CHAR	szBuffer[64];
		ZeroMemory(szBuffer, sizeof(CHAR) * 64);

		if(lModifiedSkillLevel == 0)
		{
			sprintf(szBuffer, "%s : %d", ClientStr().GetStr(STI_NEXT_GRADE), lSkillLevel + 1);
		}
		else
		{
			sprintf(szBuffer, "%s : %d (+%d)", ClientStr().GetStr(STI_NEXT_GRADE), lSkillLevel + lModifiedSkillLevel + 1, lModifiedSkillLevel);
		}

		// 불어가 너무나 길어서 추가 공백이 필요하다
		int margin = 0;
		if ( AP_SERVICE_AREA_WESTERN == g_eServiceArea || AP_SERVICE_AREA_KOREA == g_eServiceArea) margin = 60;

		m_csSkillToolTip.AddString(szBuffer, AGCM_UISKILL_NEXT_LEVEL_START_X + margin, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NEXT_LEVEL));

		SetSkillToolTipInfo(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel + lModifiedSkillLevel + 1, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NEXT_LEVEL), eTooltipType, AGCM_UISKILL_NEXT_LEVEL_START_X + margin);

		m_csSkillToolTip.AddNewLine( 14 );
	}

	else
	{
		INT32	lSkillLevel	= 1;
		return SetSkillToolTipInfo(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NORMAL), eTooltipType);
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipUpgradeInfo(AgpdSkill *pcsSkill)
{
	if (!pcsSkill || !pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillTemplate*	pcsSkillTemplate	=	static_cast< AgpdSkillTemplate* >(pcsSkill->m_pcsTemplate);

	if (m_pcsAgpmEventSkillMaster->IsFullUpgrade(pcsSkill))
		return TRUE;

	INT32	lSkillLevel	= 1;
	m_pcsAgpmFactors->GetValue(&pcsSkill->m_csFactor, &lSkillLevel, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL);

	if (lSkillLevel <= 0)
		return FALSE;

	ASSERT(lSkillLevel+1 < AGPMSKILL_MAX_SKILL_CAP);			// gemani

	DWORD	dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NORMAL);

	CHAR	szBuffer[64];

	m_csSkillToolTip.AddNewLine( 14 );

	m_csSkillToolTip.AddString(ClientStr().GetStr(STI_UPGRADE), 12, dwColor);

	m_csSkillToolTip.AddNewLine( 14 );

	ZeroMemory(szBuffer, sizeof(szBuffer));
	sprintf(szBuffer, "%s : %d",
			ClientStr().GetStr(STI_UPGRADE_REQUIRE_LEVEL),
			(INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1]);

	INT32	lCharLevel	= m_pcsAgpmCharacter->GetLevel((AgpdCharacter *) pcsSkill->m_pcsBase);

	if( (INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1] )
	{
		if (lCharLevel >= (INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1])
			m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
		else
			m_csSkillToolTip.AddString(szBuffer, 12, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		m_csSkillToolTip.AddNewLine( 14 );
	}

	ZeroMemory(szBuffer, sizeof(szBuffer));
	sprintf(szBuffer, "%s : %d",
			ClientStr().GetStr(STI_UPGRADE_REQUIRE_POINT),
			(INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][lSkillLevel + 1]);

	if( (INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][lSkillLevel + 1] )
	{
		INT32	lSkillPoint	= m_pcsAgpmCharacter->GetSkillPoint((AgpdCharacter *) pcsSkill->m_pcsBase);
		if (lSkillPoint >= (INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][lSkillLevel + 1])
			m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
		else
			m_csSkillToolTip.AddString(szBuffer, 12, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		m_csSkillToolTip.AddNewLine( 14 );
	}

	// Require Heroic Point
	INT32	nRequireHeroicPoint	=	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_HEROIC_POINT ][lSkillLevel + 1];
	INT32	lHeroicPoint		=	m_pcsAgpmCharacter->GetHeroicPoint( (AgpdCharacter*)pcsSkill->m_pcsBase );

	if( nRequireHeroicPoint )
	{
		ZeroMemory( szBuffer , sizeof(szBuffer) );
		sprintf( szBuffer , "%s : %d" ,	ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_HEROIC) , nRequireHeroicPoint );

		if( lHeroicPoint >= nRequireHeroicPoint  )
			m_csSkillToolTip.AddString(szBuffer , 12 , dwColor );
		else
			m_csSkillToolTip.AddString(szBuffer , 12 , GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		m_csSkillToolTip.AddNewLine( 14 );
	}

	// Require Charisma Point
	INT32	nRequireCharismaPoint	=	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CHARISMA_POINT][lSkillLevel + 1];
	INT32	lCharismaPoint			=	m_pcsAgpmCharacter->GetCharismaPoint( (AgpdCharacter*)pcsSkill->m_pcsBase );

	if( nRequireCharismaPoint )
	{
		ZeroMemory( szBuffer , sizeof(szBuffer) );
		sprintf( szBuffer , "%s : %d" , ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CHARISMA) , nRequireCharismaPoint );

		if( lCharismaPoint >= nRequireCharismaPoint )
			m_csSkillToolTip.AddString( szBuffer , 12 , dwColor );
		else
			m_csSkillToolTip.AddString( szBuffer , 12 , GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		m_csSkillToolTip.AddNewLine( 14 );
	}
	

	// Require Money
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	INT32 upgradeCost
		= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_UPGRADE_COST][lSkillLevel + 1];

	// 세금포함된 업그레이드 가격을 구하자
	upgradeCost = GetCostWithTax(pcsCharacter, upgradeCost);

	if( upgradeCost )
	{
		ZeroMemory(szBuffer, sizeof(szBuffer));
		sprintf(szBuffer, "%s : %d%s",
				ClientStr().GetStr(STI_UPGRADE_COST),
				upgradeCost,
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));

		INT64	llMoney	= 0;
		m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llMoney);

		if (llMoney >= upgradeCost)
			m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
		else
			m_csSkillToolTip.AddString(szBuffer, 12, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		m_csSkillToolTip.AddNewLine( 14 );
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipInfo(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos)
{
	if (!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

//	SetSkillToolTipBasicAttribute(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor);
	SetSkillToolTipBasicEffect(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);
	SetSkillToolTipCostAttribute(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);
	SetSkillToolTipDamageAttribute(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);
	SetSkillToolTipFactorPoint(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);
	SetSkillToolTipFactorPercent(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);
	SetSkillToolTipSpecialEffect(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);

	// 2007.11.15. steeple
	// 특정 스킬의 레벨을 올려주는 스킬 출력
	SetSkillToolTipSpecificLevelUp(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);

	// 스킬이 소환수 관련 된 스킬일 때는 이 함수 속에서 처리된다. 2005.09.28. steeple
	if(pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_SUMMONS)
		SetSummonsSkillToolTip(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);

	// Additional Skill 이 있는 경우는 그 놈의 데이터도 뿌려준다.
	else if(pcsSkillTemplate->m_allAdditionalSkillTID[lSkillLevel].size() == 1)
	{
		AgpdSkillTemplate* pcsAddSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate((INT32)pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel));
		if(pcsAddSkillTemplate)
		{
			AgpdSkillTooltipTemplate* pcsAddToolTipTemplate = m_pcsAgpmSkill->GetTooltipTemplate(pcsAddSkillTemplate->m_szName);
			if(pcsAddToolTipTemplate)
			{
				// 얘들은 아래 각 타입들 사이의 간격을 띄우지 않는다. 끝에 LineFeed 를 FALSE 로 준다.
				SetSkillToolTipBasicEffect(pcsAddSkillTemplate, pcsAddToolTipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
				SetSkillToolTipCostAttribute(pcsAddSkillTemplate, pcsAddToolTipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
				SetSkillToolTipDamageAttribute(pcsAddSkillTemplate, pcsAddToolTipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
				SetSkillToolTipFactorPoint(pcsAddSkillTemplate, pcsAddToolTipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
				SetSkillToolTipFactorPercent(pcsAddSkillTemplate, pcsAddToolTipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
				SetSkillToolTipSpecialEffect(pcsAddSkillTemplate, pcsAddToolTipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);

				m_csSkillToolTip.AddNewLine(14);
			}
		}
	}

	// 가격은 맨 밑에 뿌려준다. 2005.09.28. steeple
	SetSkillToolTipBuyCost(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipBasicAttribute(AgpdSkill *pcsSkill, AgpdSkillTemplate *pcsSkillTemplate, BOOL bIsActive, INT32 lLineXPos, BOOL bLineFeed)
{
	if (!pcsSkillTemplate)
		return FALSE;

	DWORD	dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_NORMAL);

	if (!bIsActive)
		dwColor	= GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_DISABLE);

	CHAR	szBuffer[256];

	BOOL	bAddLine	= FALSE;

	INT32	lSkillLevel	= 1;
	INT32	lModifiedSkillLevel = 0;

	if (pcsSkill)
	{
		m_pcsAgpmFactors->GetValue(&pcsSkill->m_csFactor, &lSkillLevel, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL);

		if (lSkillLevel <= 0)
			return FALSE;

		if( !m_pcsAgpmEventSkillMaster->IsHeroicSkill( pcsSkillTemplate->m_lID ) )
		{
			if(m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate) &&
				!m_pcsAgpmSkill->IsProductSkill(pcsSkillTemplate) &&
				!m_pcsAgpmSkill->IsArchlordSkill(pcsSkillTemplate))
			{
				lModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel(m_pcsAgcmCharacter->GetSelfCharacter());
				lModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkillTemplate->m_lID);

				lModifiedSkillLevel += pcsSkill->m_lModifiedLevel;		// 2007.11.14. steeple
			}
		}
	}

	m_csSkillToolTip.AddNewLine( 14 );

	ZeroMemory(szBuffer, sizeof(CHAR) * 256);
	if(lModifiedSkillLevel == 0)
	{
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_BASIC_ATTR_LEVEL),
				lSkillLevel,
				ClientStr().GetStr(STI_LEVEL));
	}
	else
	{
		sprintf(szBuffer, "%s : %d (+%d) %s",
				ClientStr().GetStr(STI_BASIC_ATTR_LEVEL),
				lSkillLevel + lModifiedSkillLevel, lModifiedSkillLevel,
				ClientStr().GetStr(STI_LEVEL));
	}

	lSkillLevel += lModifiedSkillLevel;

	m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

	bAddLine	= TRUE;

	ZeroMemory(szBuffer, sizeof(CHAR) * 256);

	m_csSkillToolTip.AddNewLine( 14 );

	AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate	= m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
	if (!pcsSkillTooltipTemplate)
		return FALSE;
	
	// 가능 클래스라는 헤더삽입
	sprintf(szBuffer + strlen(szBuffer), "%s : ", ClientStr().GetStr(STI_HEAD_CLASS));

	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_KNIGHT)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_KNIGHT));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_ARCHER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_ARCHER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_WIZARD)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_WIZARD));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_BERSERKER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_BERSERKER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_HUNTER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_HUNTER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SORCERER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_SORCERER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_ASSASSIN)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_ASSASSIN));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_RANGER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_RANGER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_ELEMENTALLIST)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_ELEMENTALLIST));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SLAYER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CLASS_SLAYER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SCOUTER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_OBITER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SUMMONER)
	{
//		m_csSkillToolTip.AddNewLine( 14 );

//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
				ClientStr().GetStr(STI_SUMMERNER));

//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
//
//		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SCION)
	{
		//		m_csSkillToolTip.AddNewLine( 14 );

		//		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer + strlen(szBuffer), "%s ",
			ClientStr().GetStr(STI_SCION));

		//		m_csSkillToolTip.AddString(szBuffer, 12, dwColor);
		//
		//		bAddLine	= TRUE;
	}

	// 서구버전의 경우에 전 클래스 해당이면 All Classes라고 찍는다
	if ( AP_SERVICE_AREA_WESTERN == g_eServiceArea &&
		(pcsSkillTooltipTemplate->m_ullBasicAttributeType & 0x0000000000000DF8) == 0x0DF8 )
		strcpy( szBuffer, "All Classes" );

//	sprintf(szBuffer + strlen(szBuffer), "클래스 전용");

	m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_LEVEL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_LEVEL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel], 
				ClientStr().GetStr(STI_LEVEL) );

		if (m_pcsAgpmCharacter->GetLevel(m_pcsAgcmCharacter->GetSelfCharacter()) >= (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel])
			m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		else
			m_csSkillToolTip.AddString(szBuffer, lLineXPos, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_POINT)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_POINT),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][lSkillLevel],
				ClientStr().GetStr(STI_UI_STRING_POINT) );

		if (m_pcsAgpmCharacter->GetSkillPoint(m_pcsAgcmCharacter->GetSelfCharacter()) >= (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][lSkillLevel])
			m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		else
			m_csSkillToolTip.AddString(szBuffer, lLineXPos, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_HEROIC )
	{	
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR) * 256 );
		sprintf( szBuffer , "%s : %d %s" 
			,	ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_HEROIC) 
			, 	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_HEROIC_POINT][lSkillLevel]
			,	ClientStr().GetStr(STI_UI_STRING_POINT) );

		if( m_pcsAgpmCharacter->GetHeroicPoint(m_pcsAgcmCharacter->GetSelfCharacter()) >= (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_HEROIC_POINT][lSkillLevel])
			m_csSkillToolTip.AddString( szBuffer , lLineXPos , dwColor );
		else
			m_csSkillToolTip.AddString( szBuffer , lLineXPos , GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		bAddLine	=	TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullBasicAttributeType & AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CHARISMA )
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR) * 256 );
		sprintf( szBuffer , "%s : %d %s"
			,	ClientStr().GetStr(STI_BASIC_ATTR_REQUIRE_CHARISMA)
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CHARISMA_POINT][lSkillLevel]
			,	ClientStr().GetStr(STI_UI_STRING_POINT)	);

		if( m_pcsAgpmCharacter->GetCharismaPoint( m_pcsAgcmCharacter->GetSelfCharacter()) >= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CHARISMA_POINT][lSkillLevel])
			m_csSkillToolTip.AddString( szBuffer , lLineXPos , dwColor );
		else
			m_csSkillToolTip.AddString( szBuffer , lLineXPos , GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

		bAddLine	=	TRUE;
	}

	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipBasicEffect(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if (!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	CHAR	szBuffer[256];

	BOOL	bAddLine	= FALSE;

	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTIP_CAST_TIME)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %.1f %s",
				ClientStr().GetStr(STI_CAST_TIME),
				pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME][lSkillLevel] / 1000,
				ClientStr().GetStr(STI_SECOND));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTIP_RECAST_TIME)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_RECAST_TIME),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_RECAST_TIME][lSkillLevel] / 1000,
				ClientStr().GetStr(STI_SECOND));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTIP_DURATION)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DURATION),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DURATION][lSkillLevel] / 1000,
				ClientStr().GetStr(STI_SECOND));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTIP_RANGE_DISTANCE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_RANGE_DISTANCE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_RANGE][lSkillLevel] / 100,
				ClientStr().GetStr(STI_METER));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTIP_RANGE_RADIUS)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_RANGE_RADIUS),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_TARGET_AREA_R][lSkillLevel] / 100,
				ClientStr().GetStr(STI_METER));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTIP_RANGE_BOX_X)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_RANGE_BOX_X),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_TARGET_AREA_F1][lSkillLevel] / 100,
				ClientStr().GetStr(STI_METER));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTIP_RANGE_BOX_Y)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_RANGE_BOX_Y),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_TARGET_AREA_F2][lSkillLevel] / 100,
				ClientStr().GetStr(STI_METER));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullBasicEffectType & AGPMSKILL_TOOLTOP_INTERVAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_INTERVAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_INTERVAL][lSkillLevel] / 1000,
				ClientStr().GetStr(STI_SECOND));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipCostAttribute(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if (!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	CHAR	szBuffer[256];

	BOOL	bAddLine	= FALSE;

	if (pcsSkillTooltipTemplate->m_ullCostAttributeType & AGPMSKILL_TOOLTIP_COST_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_COST_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_COST_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullCostAttributeType & AGPMSKILL_TOOLTIP_COST_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_COST_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_COST_MP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullCostAttributeType & AGPMSKILL_TOOLTIP_COST_ARROW)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_COST_ARROW),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_COST_ARROW][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		// 문엘프 궁수 이고, 활과 카타리아 둘 다 사용가능한 스킬인지 확인. 2005.10.05. steeple
		if((pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_CLASS_RANGER][lSkillLevel] != 0.0f) &&
			(pcsSkillTemplate->m_lRequirementType & AGPMSKILL_REQUIREMENT_EQUIP_WEAPON) &&
			!(pcsSkillTemplate->m_lRequirementType & AGPMSKILL_REQUIREMENT_EQUIP_BOW))
		{
			if(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ARROW_NOT_COST_KATARIYA) != NULL)
			{
				m_csSkillToolTip.AddNewLine( 14 );

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);
				sprintf(szBuffer, "+%s", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ARROW_NOT_COST_KATARIYA));

				m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
			}
		}	

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullCostAttributeType & AGPMSKILL_TOOLTIP_ENDSKILL_COST_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_ENDSKILL_COST_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_ENDSKILL_COST_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipDamageAttribute(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if (!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	CHAR	szBuffer[256];

	BOOL	bAddLine	= FALSE;

	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_MAGIC][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_WATER][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_AIR][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_EARTH][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_FIRE][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_POISON][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_ICE][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DAMAGE_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_THUNDER][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	// percent
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_A_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_MAGIC_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_WATER_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_AIR_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_EARTH_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_FIRE_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_POISON_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_ICE_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%% %s",
				ClientStr().GetStr(STI_DAMAGE_PERCENT_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_THUNDER_PERCENT][lSkillLevel], ClientStr().GetStr(STI_UI_STRING_UP) );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}


	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_DOT_DAMAGE_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_THUNDER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_HEROIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR) * 256 );
		sprintf( szBuffer , "%s : %d %s" 
			,	ClientStr().GetStr(STI_HEROIC_ATTACK_POINT)
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_HEROIC][lSkillLevel]
			,	ClientStr().GetStr(STI_UI_STRING_UP));

		m_csSkillToolTip.AddString( szBuffer , lLineXPos , dwColor );

		bAddLine	=	TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_HEROIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR) * 256 );
		sprintf( szBuffer , "%s : %d %s" 
			,	ClientStr().GetStr(STI_HEROIC_ATTACK_PERCENT)
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_HEROIC_PERCENT][lSkillLevel]
		,	ClientStr().GetStr(STI_UI_STRING_UP));

		m_csSkillToolTip.AddString( szBuffer , lLineXPos , dwColor );

		bAddLine	=	TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DOT_DAMAGE_HEROIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR) * 256 );
		sprintf( szBuffer , "%s : %d %s" 
			,	ClientStr().GetStr(STI_HEROIC_DURATION_ATTACK_POINT)
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel]
		,	ClientStr().GetStr(STI_UI_STRING_UP));

		m_csSkillToolTip.AddString( szBuffer , lLineXPos , dwColor );

		bAddLine	=	TRUE;
	}


	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipSpecialEffect(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if (!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	CHAR	szBuffer[256];

	BOOL	bAddLine	= FALSE;

	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_MOVE_DISTANCE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_MOVE_DISTANCE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel] / 100,
				ClientStr().GetStr(STI_METER));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_REFLECT_DAMAGE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_REFLECT_DAMAGE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_BLOCK_RATE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_BLOCK_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_RATE_PHYSICAL_BLOCK][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_STUN_TIME)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_STUN_TIME),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_STUN_TIME][lSkillLevel] / 1000,
				ClientStr().GetStr(STI_SECOND));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	//if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_HIT_RATE)
	//{
	//	m_csSkillToolTip.AddNewLine( 14 );

	//	ZeroMemory(szBuffer, sizeof(CHAR) * 256);
	//	sprintf(szBuffer, "%s : %d",
	//			APS_HIT_RATE,
	//			(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_HIT_RATE][lSkillLevel]);

	//	m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

	//	bAddLine	= TRUE;
	//}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_CRITICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_CRITICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CRITICAL][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_ADJUST_SKILL_RANGE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_ADJUST_SKILL_RANGE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel],
				ClientStr().GetStr(STI_METER));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	//if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EVADE_MELEE)
	//{
	//	m_csSkillToolTip.AddNewLine( 14 );

	//	ZeroMemory(szBuffer, sizeof(CHAR) * 256);
	//	sprintf(szBuffer, "%s : %d",
	//			APS_EVADE_MELEE,
	//			(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel]);

	//	m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

	//	bAddLine	= TRUE;
	//}
	//if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EVADE_RANGE)
	//{
	//	m_csSkillToolTip.AddNewLine( 14 );

	//	ZeroMemory(szBuffer, sizeof(CHAR) * 256);
	//	sprintf(szBuffer, "%s : %d",
	//			APS_EVADE_RANGE,
	//			(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel]);

	//	m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

	//	bAddLine	= TRUE;
	//}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_SKILL_RATE)		
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_SKILL_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_DAMAGE_CONVERT_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_DAMAGE_CONVERT_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_DAMAGE_CONVERT_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_DAMAGE_CONVERT_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_MP_CONVERT_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_MP_CONVERT_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MP_CONVERT_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_REGEN_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_REGEN_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_REGEN_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_REGEN_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_MP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_CHARGE_DAMAGE_1)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_CHARGE_DAMAGE_1),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CHARGE_1][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_CHARGE_DAMAGE_2)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_CHARGE_DAMAGE_2),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CHARGE_2][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_CHARGE_DAMAGE_3)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_CHARGE_DAMAGE_3),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CHARGE_3][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_AGRODOLL_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_AGRODOLL_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_AGGRODOLL_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_DOT_INTERVAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_DOT_INTERVAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DAMAGE_TIME][lSkillLevel] / 1000,
				ClientStr().GetStr(STI_SECOND));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_DECREASE_COST_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",		// % 추가 2006.11.21. steeple
				ClientStr().GetStr(STI_DECREASE_COST_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_DECREASE_CAST_TIME)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_DECREASE_CAST_TIME),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_ADJUST_SKILL_LEVEL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_ADJUST_SKILL_LEVEL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel],
				ClientStr().GetStr(STI_LEVEL));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_DEATH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_DEATH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DEATH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_ADDITIONAL_DURATION)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : +%d%%",
				ClientStr().GetStr(STI_ADDITIONAL_DURATION),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_MAX_TARGET_NUM)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_MAX_TARGET_NUM),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MAX_TARGET_NUM][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_LIMIT_QUANTITY)
	{
		unsigned int nIndex = STI_LIMIT_QUANTITY;	
		
		INT32 lValue = (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];
		
		if (lValue != 0 )
		{
			m_csSkillToolTip.AddNewLine( 14 );

			if(pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_SUMMONS_COUNT)
				nIndex = STI_MAX_SUMMONS_COUNT;

			if((pcsSkillTemplate->m_lEffectType2[0] & AGPMSKILL_EFFECT2_SUMMONS) &&
				(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4))
				lValue++;		// 소환수 증가 숫자를 나타내는 Const Data 이므로, 기본값인 1을 더해준다.

			ZeroMemory(szBuffer, sizeof(CHAR) * 256);
			sprintf(szBuffer, "%s : %d",
					ClientStr().GetStr(nIndex),
					lValue);

			m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

			bAddLine	= TRUE;	
		}
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_SUMMONS_COUNT)
	{
		INT32 lValue = (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];

		if( lValue != 0)
		{
			m_csSkillToolTip.AddNewLine( 14 );


			ZeroMemory(szBuffer, sizeof(CHAR) * 256);
			sprintf(szBuffer, "%s : %d",
					ClientStr().GetStr(STI_MAX_SUMMONS_COUNT),
					lValue);

			m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

			bAddLine	= TRUE;
		}
		
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_BONUS_EXP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_BONUS_EXP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_EXP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_BONUS_MONEY)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_BONUS_MONEY),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_MONEY][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_BONUS_DROP_RATE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_BONUS_DROP_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_DROP_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_BONUS_DIVIDE_RATE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_DIVIDE_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DIVIDE_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_RATE_CRITICAL_ACTIVITY)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_RATE_CRITICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_RATE_STUN_ACTIVITY)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_RATE_STUN),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_ADJUST_SUMMON_LEVEL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_INCREASE_SUMMON_LEVEL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel],
				ClientStr().GetStr(STI_LEVEL));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EFFECT_SUMMONS_QUANTITY)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		INT32 lValue = (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];
		if((pcsSkillTemplate->m_lEffectType2[0] & AGPMSKILL_EFFECT2_SUMMONS) &&
			(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4))
			lValue++;		// 소환수 증가 숫자를 나타내는 Const Data 이므로, 기본값인 1을 더해준다.

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EFFECT_NUMBER),
				lValue);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if(pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_DURABILITY)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		INT32 lValue = (INT32) pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_ARMOR_DURABILITY_PERCENT][lSkillLevel];
		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
			ClientStr().GetStr(STI_DECREASE_DURABILITY),
			lValue);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if(pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_QUANTITY_ITEM)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		INT32 lValue = (INT32) pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_ARMOR_DURABILITY_NUM][lSkillLevel];
		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
			ClientStr().GetStr(STI_NUMBER_ITEM),
			lValue);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if( pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_STUN_GENERATE_TIME)
	{
		m_csSkillToolTip.AddNewLine( 14 );
		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d %s" 
			,	ClientStr().GetStr(STI_HEROIC_STUN_GENERATE_DURATION) 
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_STUN_GENERATE_TIME][lSkillLevel]
		,	ClientStr().GetStr(STI_SECOND) );

		m_csSkillToolTip.AddString( szBuffer , lLineXPos , dwColor );


		bAddLine	= TRUE;
	}

	if( pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_TIME_CONTROL)
	{
		m_csSkillToolTip.AddNewLine( 14 );
		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d %s" 
			,	ClientStr().GetStr(STI_HEROIC_TIME_CHANGE) 
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[ AGPMSKILL_CONST_TIME_CONTROL][lSkillLevel]
		,	ClientStr().GetStr(STI_SECOND) );

		m_csSkillToolTip.AddString( szBuffer , lLineXPos , dwColor );


		bAddLine	= TRUE;
	}
	
	// 2005.09.29. steeple. 따로 분리.
	SetSkillToolTipEADamageAttribute(pcsSkillTemplate, pcsSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, bLineFeed);

	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	// 비용은 다른 함수로 옮겨감. 2005.09.28. steeple

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipFactorPoint(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if (!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	CHAR	szBuffer[256];

	BOOL	bAddLine	= FALSE;

	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_CON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_CON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_CON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_STR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_STR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_STR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_INT)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_INT),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_INT][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEX)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEX),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEX][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_CHA)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_CHA),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_CHA][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_WIS)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_WIS),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_WIS][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_MOVEMENT)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_MOVEMENT),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_MOVEMENT][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_MP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_AGRO)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_AGRO),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_AGRO][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_MAX_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_MAX_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_HP_MAX][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_MAX_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_MAX_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_MP_MAX][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_ATK_SPEED)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_ATK_SPEED),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_ATKSPEED][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_ATK_RANGE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d %s",
				ClientStr().GetStr(STI_FACTOR_POINT_ATK_RANGE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_ATKRANGE][lSkillLevel],
				ClientStr().GetStr(STI_METER));

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	// 2005.12.22. steeple
	// Skill_Cast, Skill_Delay, Hit_Rate, Evade_Rate, Dodge_Rate 추가
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_SKILL_CAST)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",			// 얘들은 Point Factor 를 Percent 처럼 쓴다.
				ClientStr().GetStr(STI_FACTOR_POINT_SKILL_CAST),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_SKILL_CAST][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_SKILL_DELAY)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",			// 얘들은 Point Factor 를 Percent 처럼 쓴다.
				ClientStr().GetStr(STI_FACTOR_POINT_SKILL_DELAY),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_SKILL_DELAY][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_HIT_RATE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",			// 얘들은 Point Factor 를 Percent 처럼 쓴다.
				ClientStr().GetStr(STI_FACTOR_POINT_HIT_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_HIT_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_EVADE_RATE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",			// 얘들은 Point Factor 를 Percent 처럼 쓴다.
				ClientStr().GetStr(STI_FACTOR_POINT_EVADE_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_EVADE_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DODGE_RATE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",			// 얘들은 Point Factor 를 Percent 처럼 쓴다.
				ClientStr().GetStr(STI_FACTOR_POINT_DODGE_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DODGE_RATE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		switch(m_pcsAgpmSkill->GetTimeBuffType(pcsSkillTemplate))
		{
			case 1:		// 낮에는 방어력 증가, 밤에는 공격력 증가. 여기는 공격력이니 '밤' 으로 해줌.
				sprintf(szBuffer, "%s(%s) : %d",
						ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_PHYSICAL),
						m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TIME_UNIT_NIGHT) ? m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TIME_UNIT_NIGHT) : _T("Night"),
						(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_PHYSICAL][lSkillLevel]);
				break;

			default:
				sprintf(szBuffer, "%s : %d",
						ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_PHYSICAL),
						(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_PHYSICAL][lSkillLevel]);
				break;
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MIN_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_LIGHTENING][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		switch(m_pcsAgpmSkill->GetTimeBuffType(pcsSkillTemplate))
		{
			case 1:		// 낮에는 방어력 증가, 밤에는 공격력 증가. 여기는 공격력이니 '밤' 으로 해줌.
				sprintf(szBuffer, "%s(%s) : %d",
						ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_PHYSICAL),
						m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TIME_UNIT_NIGHT) ? m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TIME_UNIT_NIGHT) : _T("Night"),
						(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_PHYSICAL][lSkillLevel]);
				break;

			default:
				sprintf(szBuffer, "%s : %d",
						ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_PHYSICAL),
						(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_PHYSICAL][lSkillLevel]);
				break;
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DMG_MAX_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_LIGHTENING][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		switch(m_pcsAgpmSkill->GetTimeBuffType(pcsSkillTemplate))
		{
			case 1:		// 낮에는 방어력 증가, 밤에는 공격력 증가. 여기는 방어력이니 '낮' 으로 해줌.
				sprintf(szBuffer, "%s(%s) : %d",
						ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_PHYSICAL),
						m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TIME_UNIT_DAY) ? m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TIME_UNIT_DAY) : _T("Day"),
						(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_PHYSICAL][lSkillLevel]);
				break;

			default:
				sprintf(szBuffer, "%s : %d",
						ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_PHYSICAL),
						(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_PHYSICAL][lSkillLevel]);
				break;
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_POINT_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_LIGHTENING][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_RATE_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_RATE_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_RATE_PHYSICAL][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_RATE_PHYSICAL_BLOCK)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_PHYSICAL_BLOCK_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_RATE_PHYSICAL_BLOCK][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_RATE_SKILL_BLOCK)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_POINT_DEF_RATE_SKILL_BLOCK),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_RATE_SKILL_BLOCK][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_HEROIC )
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d"
				,	ClientStr().GetStr(STI_HEROIC_MIN_HEROIC_DAMAGE)
				,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MIN_HEROIC][lSkillLevel] );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_HEROIC )
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d"
				,	ClientStr().GetStr(STI_HEROIC_MAX_HEROIC_DAMAGE)
				,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_HEROIC][lSkillLevel] );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_HEROIC )
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d"
				,	ClientStr().GetStr(STI_HEROIC_HEROIC_DEFENCE)
				,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_POINT_HEROIC][lSkillLevel] );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_MEELEHEROIC_RESISTANCE )
	{	
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d%%"
			,	ClientStr().GetStr(STI_HEROIC_NEAR_RESISTANCE)
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_RATE_HEROIC_MELEE][lSkillLevel] );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_RANGEHEROIC_RESISTANCE )
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d%%"
			,	ClientStr().GetStr(STI_HEROIC_FAR_RESISTANCE)
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_RATE_HEROIC_RANGED][lSkillLevel] );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (pcsSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_MAGICHEROIC_RESISTANCE )
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory( szBuffer , sizeof(CHAR)*256 );
		sprintf( szBuffer , "%s : %d%%"
			,	ClientStr().GetStr(STI_HEROIC_MAGIC_RESISTANCE)
			,	(INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DEF_RATE_HEROIC_MAGIC][lSkillLevel] );

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipFactorPercent(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if (!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	CHAR	szBuffer[256];

	BOOL	bAddLine	= FALSE;

	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_CON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_CON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_CON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_STR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_STR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_STR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_INT)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_INT),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_INT][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEX)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEX),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEX][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_CHA)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_CHA),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_CHA][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_WIS)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_WIS),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_WIS][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MOVEMENT)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_MOVEMENT),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_MOVEMENT][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_HP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_MP][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_AGRO)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_AGRO),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_AGRO][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MAX_HP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_MAX_HP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_HP_MAX][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MAX_MP)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_MAX_MP),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_MP_MAX][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_ATK_SPEED)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_ATK_SPEED),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_ATKSPEED][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_ATK_RANGE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_ATK_RANGE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_ATKRANGE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_PHYSICAL][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MIN_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MIN_LIGHTENING][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_PHYSICAL][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DMG_MAX_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DMG_MAX_LIGHTENING][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_PHYSICAL][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_POINT_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_POINT_LIGHTENING][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_RATE_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_RATE_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_RATE_PHYSICAL][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_RATE_PHYSICAL_BLOCK)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_PHYSICAL_BLOCK_RATE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_RATE_PHYSICAL_BLOCK][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_RATE_SKILL_BLOCK)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
				ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_RATE_SKILL_BLOCK),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_DEF_RATE_SKILL_BLOCK][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_IGNORE_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
			ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_IGNORE_PHYSICAL),
			(INT32) pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_PHYSICAL_DEFENCE_PERCENT][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_IGNORE_ATTRIBUTE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
			ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_IGNORE_ATTRIBUTE),
			(INT32) pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_ATTRIBUTE_DEFENCE_PERCENT][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullFactorPercentType & AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_CRITICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d%%",
			ClientStr().GetStr(STI_FACTOR_PERCENT_DEF_CRITICAL),
			(INT32) pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL_DEFENCE_PERCENT][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipEADamageAttribute(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if(!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	CHAR szBuffer[256];
	BOOL bAddLine = FALSE;

	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_PHYSICAL)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_PHYSICAL),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_PHYSICAL][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_MAGIC)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_MAGIC),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_MAGIC][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_WATER)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_WATER),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_WATER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_AIR)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_AIR),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_AIR][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_EARTH)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_EARTH),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_EARTH][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_FIRE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_FIRE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_FIRE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_POISON)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_POISON),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_POISON][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_ICE)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_ICE),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_ICE][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}
	if (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_EA_DMG_POINT_LIGHTENING)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		ZeroMemory(szBuffer, sizeof(CHAR) * 256);
		sprintf(szBuffer, "%s : %d",
				ClientStr().GetStr(STI_EA_DMG_POINT_LIGHTENING),
				(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_EA_DMG_THUNDER][lSkillLevel]);

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		bAddLine	= TRUE;
	}

	if(bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine( 14 );

	return TRUE;
}

// 2005.09.28. steeple
BOOL AgcmUISkill2::SetSkillToolTipBuyCost(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if(!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	BOOL bAddLine = FALSE;

	if(eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_BUY || eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE || eTooltipType == AGCMUI_SKILL2_TOOLTIP_TYPE_NORMAL)
	{
		CHAR szBuffer[256] = {0, };

		if ( (pcsSkillTooltipTemplate->m_ullSpecialEffectType & AGPMSKILL_TOOLTIP_LEARN_COST) &&
			eTooltipType	== AGCMUI_SKILL2_TOOLTIP_TYPE_BUY )
		{
			m_csSkillToolTip.AddNewLine( 14 );

			INT32 lSkillCost = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_COST][lSkillLevel];
			AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
			
			// Get Tax
			lSkillCost = GetCostWithTax(pcsCharacter, lSkillCost);

			// Get Inventory Money
			INT64 llInvenMoney = 0;
			m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llInvenMoney);

			// Make Cost String
			sprintf(szBuffer, "%s : %d%s",
					ClientStr().GetStr(STI_LEARN_COST),
					lSkillCost,
					m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));
			
			// Select Color of Cost String
			if (llInvenMoney > (INT64) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_COST][lSkillLevel])
				m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
			else
				m_csSkillToolTip.AddString(szBuffer, lLineXPos, GetSkillTooltipColor(AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY));

			bAddLine	= TRUE;
		}

		// 2007.07.09. steeple
		// 고렙스킬이라면 선결 스킬이 있다.
		if(m_pcsAgpmSkill->IsHighLevelSkill(pcsSkillTemplate))
		{
			AgpdEventSkillHighLevel stHighLevel = m_pcsAgpmEventSkillMaster->GetHighLevelSkillInfo(pcsSkillTemplate->m_lID);
			
			if(stHighLevel.m_lSkillTID != 0 )
			{
				for(int i = 0; i < AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION; ++i)
				{
					if(stHighLevel.m_stCondition[i].m_lSkillTID != 0)
					{

						DWORD		dwFontColor		=	dwColor;

						AgpdSkillTemplate* pcsPreviousSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_stCondition[i].m_lSkillTID);
						if(!pcsPreviousSkillTemplate)
							break;

						m_csSkillToolTip.AddNewLine( 14 );

						ZeroMemory(szBuffer, sizeof(CHAR) * 256);
						sprintf(szBuffer, "%s %s : %s %d%s",
								ClientStr().GetStr(STI_PREVIOUS_STEP),
								ClientStr().GetStr(STI_SKILL),
								pcsPreviousSkillTemplate->m_szName,
								stHighLevel.m_stCondition[i].m_lSkillLevel,
								ClientStr().GetStr(STI_LEVEL));

						AgpdSkill*		pcsSkill		=	m_pcsAgpmSkill->GetSkillByTID( m_pcsAgcmCharacter->GetSelfCharacter() , stHighLevel.m_stCondition[i].m_lSkillTID );

						if( m_pcsAgpmSkill->GetSkillLevel( pcsSkill ) < stHighLevel.m_stCondition[i].m_lSkillLevel )
							dwFontColor			=	0x00ff0000;

						m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwFontColor);

						bAddLine = TRUE;
					}

					if(stHighLevel.m_stCondition[i].m_lSkillTotalPoint != 0)
					{
						DWORD	dwFontColor		=	dwColor;
						m_csSkillToolTip.AddNewLine( 14 );

						if( m_pcsAgpmSkill->GetTotalSkillPoint( m_pcsAgcmCharacter->GetSelfCharacter() ) < stHighLevel.m_stCondition[i].m_lSkillTotalPoint )
							dwFontColor		=	0x00ff0000;

						ZeroMemory(szBuffer, sizeof(CHAR) * 256);
						char* pFormat = m_pcsAgcmUIManager2->GetUIMessage( "RequireTotalSkillPoint" );
						if( pFormat && strlen( pFormat ) > 0 )
						{
							sprintf( szBuffer, pFormat, stHighLevel.m_stCondition[i].m_lSkillTotalPoint );
							m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwFontColor);
							bAddLine = TRUE;
						}
					}

					if(stHighLevel.m_stCondition[i].m_lSkillPoint != 0)
					{
						AgpdCharacterTemplate* pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(stHighLevel.m_stCondition[i].m_lSkillTab);
						if(!pcsCharacterTemplate) 
							break;
						INT32	lClass	=	m_pcsAgpmFactors->GetClass( &pcsCharacterTemplate->m_csFactor );
						INT32	lRace	=	m_pcsAgpmFactors->GetRace( &pcsCharacterTemplate->m_csFactor );
						CHAR  pRaceBuffer[20];
						ZeroMemory(pRaceBuffer, sizeof(CHAR)*20);

						switch(lClass)
						{
						case AUCHARCLASS_TYPE_KNIGHT:
							memcpy(pRaceBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SLAYER), 
								    sizeof(CHAR)*20);
							break;
						case AUCHARCLASS_TYPE_RANGER:
							memcpy(pRaceBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_OBITER), 
								    sizeof(CHAR)*20);
							break;
						case AUCHARCLASS_TYPE_SCION:
							memcpy(pRaceBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SCION), 
								    sizeof(CHAR)*20);
							break;
						case AUCHARCLASS_TYPE_MAGE:
							memcpy(pRaceBuffer, m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_LG_CLASS_SUMMERNER), 
								    sizeof(CHAR)*20);
							break;
						}
						
						m_csSkillToolTip.AddNewLine( 14 );

						ZeroMemory(szBuffer, sizeof(CHAR) * 256);
						char* pFormat = m_pcsAgcmUIManager2->GetUIMessage( "RequireSkillPoint" );
						if( pFormat && strlen( pFormat ) > 0 )
						{
							sprintf( szBuffer, pFormat, pRaceBuffer, stHighLevel.m_stCondition[i].m_lSkillPoint );

							DWORD dwFontColor = dwColor;
							if( m_pcsAgpmSkill->GetTotalSkillPoint( m_pcsAgcmCharacter->GetSelfCharacter() , lRace , lClass ) < stHighLevel.m_stCondition[i].m_lSkillPoint )
							{
								dwFontColor	= 0x00ff0000;
							}

							m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwFontColor);	
							bAddLine = TRUE;
						}
					}
				}
			}
		}

		else if( m_pcsAgpmSkill->IsHeroicSkill(pcsSkillTemplate) )
		{
			AgpdEventSkillHeroic	stHeroicSkill		=	m_pcsAgpmEventSkillMaster->GetHeroicSkillInfo(pcsSkillTemplate->m_lID);
			BOOL					bConditionACheck	=	TRUE;
			DWORD					dwFontColor			=	GetSkillTooltipColor( AGCMUISKILL2_TOOLTIP_COLOR_NORMAL );

			for(int i =0; i < AGPMEVENT_SKILL_MAX_HEROIC_CONDITION; ++i)
			{
				if(stHeroicSkill.m_stCondition[i].m_lUsedTotalHeroicPoint != 0)
				{
					if(m_pcsAgpmSkill->GetTotalUsedHeroicPoint(m_pcsAgcmCharacter->GetSelfCharacter()) < stHeroicSkill.m_stCondition[i].m_lUsedTotalHeroicPoint)
						dwFontColor	=	GetSkillTooltipColor( AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY );

					else
						dwFontColor	=	GetSkillTooltipColor( AGCMUISKILL2_TOOLTIP_COLOR_NORMAL );
				
					m_csSkillToolTip.AddNewLine( 14 );

					ZeroMemory(szBuffer, sizeof(CHAR) * 256);
					char* pFormat = m_pcsAgcmUIManager2->GetUIMessage( "RequireTotalHeroicSkillPoint" );
					if( pFormat )
					{
						sprintf( szBuffer, pFormat, stHeroicSkill.m_stCondition[i].m_lUsedTotalHeroicPoint );
						m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwFontColor);	
						bAddLine = TRUE;
					}
				}

				if(stHeroicSkill.m_stCondition[i].m_lSkillTID != 0)
				{
					AgpdSkillTemplate *pcsConditionTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHeroicSkill.m_stCondition[i].m_lSkillTID);
					if(NULL == pcsConditionTemplate)
						bConditionACheck	=	FALSE;

					AgpdSkill *pcsCondtionSkill = m_pcsAgpmSkill->GetSkill(m_pcsAgcmCharacter->GetSelfCharacter(), pcsConditionTemplate->m_szName);
					if(NULL == pcsCondtionSkill)
						bConditionACheck	=	FALSE;

					INT32 lConditionLevel = m_pcsAgpmSkill->GetSkillLevel(pcsCondtionSkill);
					if( lConditionLevel < stHeroicSkill.m_stCondition[i].m_lSkillLevel )
						bConditionACheck	=	FALSE;

					if( !bConditionACheck )
						dwFontColor	=	GetSkillTooltipColor( AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY );

					else
						dwFontColor	=	GetSkillTooltipColor( AGCMUISKILL2_TOOLTIP_COLOR_NORMAL );

					m_csSkillToolTip.AddNewLine( 14 );

					ZeroMemory(szBuffer, sizeof(CHAR) * 256);
					sprintf(szBuffer, "%s %s : %s %d%s",
							ClientStr().GetStr(STI_PREVIOUS_STEP),
							ClientStr().GetStr(STI_SKILL),
							pcsConditionTemplate->m_szName,
							stHeroicSkill.m_stCondition[i].m_lSkillLevel,
							ClientStr().GetStr(STI_LEVEL) );

					m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwFontColor);	
					bAddLine = TRUE;
				}
			}

		}
	}

	if (bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine(14);

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillToolTipSpecificLevelUp(AgpdSkillTemplate *pcsSkillTemplate, AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if(!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	if(pcsSkillTemplate->m_allLevelUpSkillTID[lSkillLevel][0] == 0)
		return FALSE;

	CHAR szBuffer[256];
	BOOL bAddLine = FALSE;

	ZeroMemory(szBuffer, sizeof(szBuffer));

	m_csSkillToolTip.AddNewLine( 14 );

	_snprintf_s(szBuffer, 255, ClientStr().GetStr(STI_SKILL_SPECIFIC_LEVELUP), 255);
	m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

	m_csSkillToolTip.AddNewLine( 14 );

	for(int i = 0; i < AGPMSKILL_MAX_SKILL_LEVELUP_TID; ++i)
	{
		if(pcsSkillTemplate->m_allLevelUpSkillTID[lSkillLevel][i] == 0)
			break;

		AgpdSkillTemplate* pcsLevelUpTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsSkillTemplate->m_allLevelUpSkillTID[lSkillLevel][i]);
		if(!pcsLevelUpTemplate)
			continue;

		ZeroMemory(szBuffer, sizeof(szBuffer));
		sprintf(szBuffer, "%s +1", pcsLevelUpTemplate->m_szName);
		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

		m_csSkillToolTip.AddNewLine(14);
	}

	return TRUE;
}

// 2005.09.28. steeple
BOOL AgcmUISkill2::SetSummonsSkillToolTip(AgpdSkillTemplate* pcsSkillTemplate, AgpdSkillTooltipTemplate* pcsSkillTooltipTemplate, INT32 lSkillLevel, DWORD dwColor, AgcmUISkill2_TooltipType eTooltipType, INT32 lLineXPos, BOOL bLineFeed)
{
	if(!pcsSkillTemplate || !pcsSkillTooltipTemplate || lSkillLevel <= 0 || lSkillLevel > AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	if(m_pcsAgpmSkill->IsSummonsSkill(pcsSkillTemplate) == FALSE)
		return FALSE;

    // 캐릭터 템플릿을 가져와야 한다.
	AgpdCharacterTemplate* pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate((INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel]);
	if(!pcsCharacterTemplate)
		return FALSE;

	// 소환수의 능력을 올려주는 스킬 템플릿을 가져온다.
	AgpdSkillTemplate* pcsAddSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate((INT32)pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel));
	if(!pcsAddSkillTemplate)
		return FALSE;

	AgpdSkillTooltipTemplate* pcsAddSkillTooltipTemplate = m_pcsAgpmSkill->GetTooltipTemplate(pcsAddSkillTemplate->m_szName);
	if(!pcsAddSkillTooltipTemplate)
		return FALSE;

	// 보여줘야 하는 정보는 공격력, 속성공력력, 생명력 순서
	CHAR szBuffer[256];
	memset(szBuffer, 0, sizeof(szBuffer));
	
    INT32 lValue = 0;
	BOOL bAddLine = FALSE;

	// 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	sprintf(szBuffer, "%s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE), lValue);

	INT32 lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_PHYSICAL][lSkillLevel];
	if(lAddValue > 0)
		sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);

	m_csSkillToolTip.AddNewLine( 14 );
	m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);

	// 마법 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_MAGIC),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_MAGIC ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_MAGIC)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_MAGIC][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_MAGIC][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 물 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_WATER),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_WATER ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_WATER)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_WATER][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_WATER][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 공기 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_AIR),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_AIR ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_AIR)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_AIR][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_AIR][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 땅 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_EARTH),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_EARTH ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_EARTH)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_EARTH][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_EARTH][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 불 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_FIRE),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_FIRE ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_FIRE)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_FIRE][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_FIRE][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 독 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_POISON),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_POISON ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_POISON)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_POISON][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_POISON][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 얼음 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_ICE),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_ICE ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_ICE)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_ICE][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_ICE][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 번개 공격력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s %s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_LIGHTENING),
										m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_DAMAGE_RATE),
										lValue);

		if(pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_DAMAGE_LIGHTENING ||
			pcsAddSkillTooltipTemplate->m_ullDamageAttributeType & AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_LIGHTENING)
		{
			lAddValue = (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_THUNDER][lSkillLevel] +
						(INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_DMG_MAX_LIGHTENING][lSkillLevel];

			if(lAddValue > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", lAddValue);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	// 생명력
	m_pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	if(lValue > 0)
	{
		m_csSkillToolTip.AddNewLine( 14 );

		memset(szBuffer, 0, sizeof(szBuffer));
		sprintf(szBuffer, "%s : %d", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_HP_RATE), lValue);

		if(pcsAddSkillTooltipTemplate->m_ullFactorPointType & AGPMSKILL_TOOLTIP_FACTOR_POINT_MAX_HP)
		{
			if(((INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_HP_MAX][lSkillLevel]) > 0)
				sprintf(szBuffer + strlen(szBuffer), " + %d", (INT32)pcsAddSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_HP_MAX][lSkillLevel]);
		}

		m_csSkillToolTip.AddString(szBuffer, lLineXPos, dwColor);
		bAddLine = TRUE;
	}

	if(bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine( 14 );

	bAddLine = FALSE;

	SetSkillToolTipEADamageAttribute(pcsAddSkillTemplate, pcsAddSkillTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, bLineFeed);

	AgpdSkillTemplate* pcsAddEffectSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate((INT32)pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel));
	if(pcsAddEffectSkillTemplate)
	{
		AgpdSkillTooltipTemplate* pcsAddEffectTooltipTemplate = m_pcsAgpmSkill->GetTooltipTemplate(pcsAddEffectSkillTemplate->m_szName);
		if(pcsAddEffectTooltipTemplate)
		{
			// 얘들은 아래 각 타입들 사이의 간격을 띄우지 않는다. 끝에 LineFeed 를 FALSE 로 준다.
			SetSkillToolTipBasicEffect(pcsAddEffectSkillTemplate, pcsAddEffectTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
			SetSkillToolTipCostAttribute(pcsAddEffectSkillTemplate, pcsAddEffectTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
			SetSkillToolTipDamageAttribute(pcsAddEffectSkillTemplate, pcsAddEffectTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
			SetSkillToolTipFactorPoint(pcsAddEffectSkillTemplate, pcsAddEffectTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
			SetSkillToolTipFactorPercent(pcsAddEffectSkillTemplate, pcsAddEffectTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
			SetSkillToolTipSpecialEffect(pcsAddEffectSkillTemplate, pcsAddEffectTooltipTemplate, lSkillLevel, dwColor, eTooltipType, lLineXPos, FALSE);
		}
	}

	if(bAddLine && bLineFeed)
		m_csSkillToolTip.AddNewLine( 14 );

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillBookTooltip(AgpdItem *pcsItem, AcUIToolTip *pcsItemTooltip)
{
	if (!pcsItem || !pcsItemTooltip)
		return FALSE;

	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	DWORD	dwColor	= m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL);

	AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(pcsItem->m_lSkillTID);
	if (!pcsSkillTemplate)
		return FALSE;

	sprintf(szBuffer, "%s %s", pcsSkillTemplate->m_szName, ClientStr().GetStr(STI_SKILLBOOK));

	pcsItemTooltip->AddString(szBuffer, 14, dwColor);
	pcsItemTooltip->AddNewLine(14);

	INT32	lMasteryIndex	= m_pcsAgpmEventSkillMaster->GetMasteryIndex(pcsSkillTemplate);

	switch (lMasteryIndex) {
	case 0:
		sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_COMBAT), ClientStr().GetStr(STI_SKILL));
		pcsItemTooltip->AddString(szBuffer, 14, dwColor);
		pcsItemTooltip->AddNewLine(14);
		break;
	case 1:
		sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_REINFORCE), ClientStr().GetStr(STI_SKILL));
		pcsItemTooltip->AddString(szBuffer, 14, dwColor);
		pcsItemTooltip->AddNewLine(14);
		break;
	case 2:
		sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_SPECIFIC), ClientStr().GetStr(STI_SKILL));
		pcsItemTooltip->AddString(szBuffer, 14, dwColor);
		pcsItemTooltip->AddNewLine(14);
		break;
	case 3:
		sprintf(szBuffer, "%s %s", ClientStr().GetStr(STI_PASSIVE), ClientStr().GetStr(STI_SKILL));
		pcsItemTooltip->AddString(szBuffer, 14, dwColor);
		pcsItemTooltip->AddNewLine(14);
		break;
	}

	pcsItemTooltip->AddNewLine(14);

	INT32	lTemplateRace	= AURACE_TYPE_NONE;
	INT32	lTemplateClass	= AUCHARCLASS_TYPE_NONE;

	m_pcsAgpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lTemplateRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lTemplateClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	CHAR	*szRaceName		= m_pcsAgpmFactors->GetCharacterRaceName(lTemplateRace);
	CHAR	*szClassName	= m_pcsAgpmFactors->GetCharacterClassName(lTemplateRace, lTemplateClass);

	if (szRaceName && szClassName)
	{
		sprintf(szBuffer, "%s %s %s", szRaceName, szClassName, ClientStr().GetStr(STI_USE_ENABLE));

		INT32	lRace			= AURACE_TYPE_NONE;
		INT32	lClass			= AUCHARCLASS_TYPE_NONE;

		m_pcsAgpmFactors->GetValue(&m_pcsAgcmCharacter->m_pcsSelfCharacter->m_csFactor, &lRace, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
		m_pcsAgpmFactors->GetValue(&m_pcsAgcmCharacter->m_pcsSelfCharacter->m_csFactor, &lClass, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

		if ((lTemplateRace != AURACE_TYPE_NONE && lTemplateRace != lRace) ||
			(lTemplateClass != AUCHARCLASS_TYPE_NONE && lTemplateClass != lClass))
			pcsItemTooltip->AddString(szBuffer, 14, m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY));
		else
			pcsItemTooltip->AddString(szBuffer, 14, m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_NORMAL));

		pcsItemTooltip->AddNewLine(14);
	}

	sprintf(szBuffer, "%s %d",
		ClientStr().GetStr(STI_UPGRADE_REQUIRE_LEVEL),
		(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][1]);

	INT32	lCharLevel	= m_pcsAgpmCharacter->GetLevel(m_pcsAgcmCharacter->m_pcsSelfCharacter);
	if (lCharLevel >= (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][1])
		pcsItemTooltip->AddString(szBuffer, 14, dwColor);
	else
		pcsItemTooltip->AddString(szBuffer, 14, m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY));

	pcsItemTooltip->AddNewLine(14);

	sprintf(szBuffer, "%s %d",
		ClientStr().GetStr(STI_UPGRADE_REQUIRE_POINT),
		(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][1]);

	INT32	lSkillPoint	= m_pcsAgpmCharacter->GetSkillPoint(m_pcsAgcmCharacter->m_pcsSelfCharacter);
	if (lSkillPoint >= (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_POINT][1])
		pcsItemTooltip->AddString(szBuffer, 14, dwColor);
	else
		pcsItemTooltip->AddString(szBuffer, 14, m_pcsAgcmUIItem->GetTooltipColor(AGCMUIITEM_TOOLTIP_COLOR_LACK_ABILITY));

	pcsItemTooltip->AddNewLine(14);

	pcsItemTooltip->AddNewLine(14);

	AgpdSkillTooltipTemplate	*pcsSkillTooltipTemplate	= m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
	if (pcsSkillTooltipTemplate)
	{
		// 기술 설명
		pcsItemTooltip->AddString(pcsSkillTooltipTemplate->m_szSkillDescription, 14, dwColor);
		pcsItemTooltip->AddNewLine(14);
	}

	return TRUE;
}

BOOL AgcmUISkill2::SetSkillScrollTooltip(AgpdItem *pcsItem, AcUIToolTip *pcsItemTooltip)
{
	if (!pcsItem || !pcsItemTooltip)
		return FALSE;

	return TRUE;
}

BOOL AgcmUISkill2::CBOpenSkillDescToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUISkill2	*pThis		= (AgcmUISkill2 *)	pClass;
	
	AcUIGrid*	pGrid = (AcUIGrid*)( pcsSourceControl->m_pcsBase );

	if (!pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL)
		return TRUE;

	AgpdSkillTemplate	*pcsSkillTemplate	= pThis->m_pcsAgpmSkill->GetSkillTemplate(pGrid->m_pToolTipAgpdGridItem->m_lItemTID);
	if (!pcsSkillTemplate)
		return TRUE;
	
	AgpdSkillTooltipTemplate *pcsSkillTooltipTemplate	= pThis->m_pcsAgpmSkill->GetTooltipTemplate(pcsSkillTemplate->m_szName);
	if (!pcsSkillTooltipTemplate ||
		!pcsSkillTooltipTemplate->m_szSkillMiniDescription ||
		!pcsSkillTooltipTemplate->m_szSkillMiniDescription[0])
		return TRUE;

	pThis->m_csSkillToolTip.MoveWindow((INT32) pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.x + 30, (INT32) pThis->m_pcsAgcmUIManager2->m_v2dCurMousePos.y + 30,
									   pThis->m_csSkillToolTip.w, pThis->m_csSkillToolTip.h);

	CHAR	szBuffer[64];
	ZeroMemory(szBuffer, sizeof(CHAR) * 64);

	INT32	lDescLength		= strlen(pcsSkillTooltipTemplate->m_szSkillMiniDescription);
	INT32	lStartStringPos	= 0;

	int i;
	for (i = 0; i < lDescLength; ++i)
	{
		if (pcsSkillTooltipTemplate->m_szSkillMiniDescription[i] == '\\' &&
			pcsSkillTooltipTemplate->m_szSkillMiniDescription[i + 1] == 'n')
		{
			ZeroMemory(szBuffer, sizeof(CHAR) * 64);
			strncpy(szBuffer, pcsSkillTooltipTemplate->m_szSkillMiniDescription + lStartStringPos, i - lStartStringPos);

			pThis->m_csSkillToolTip.AddString(szBuffer, 12);
			pThis->m_csSkillToolTip.AddNewLine(14);

			lStartStringPos	= i + 2;

			++i;
		}
	}

	ZeroMemory(szBuffer, sizeof(CHAR) * 64);
	strncpy(szBuffer, pcsSkillTooltipTemplate->m_szSkillMiniDescription + lStartStringPos, lDescLength - lStartStringPos);

	pThis->m_csSkillToolTip.AddString(szBuffer, 12);
	pThis->m_csSkillToolTip.AddNewLine(14);

	if (pThis->m_pcsAgpmSkill->IsCashSkill(pcsSkillTemplate))
	{
		// 렌즈스톤
		if (pThis->m_pcsAgpmSkill->GetActionOnActionType(pcsSkillTemplate) == 4)
		{
			pThis->m_csSkillToolTip.AddNewLine(14);

			INT32 lCount = 0;

			AgpdItemADChar* pcsItemADChar = pThis->m_pcsAgpmItem->GetADCharacter(pThis->m_pcsAgcmCharacter->GetSelfCharacter());
			if (pcsItemADChar)
			{
				for (i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
				{
					AgpdGridItem* pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
					if(!pcsGridItem)
						continue;

					AgpdItem* pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
					if(!pcsItem || !pcsItem->m_pcsItemTemplate)
						continue;

					if((pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&	//	사용중
						pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
						pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_ONE_ATTACK)
					{
						lCount	= pcsItem->m_nCount;
						break;
					}
				}
			}

			ZeroMemory(szBuffer, sizeof(szBuffer));
			sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_REMAIN_COUNT), lCount);

			pThis->m_csSkillToolTip.AddString(szBuffer, 12);
			pThis->m_csSkillToolTip.AddNewLine(14);
		}
		else if (pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_BUFF)
		{
			AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pThis->m_pcsAgpmItem->GetUsingCashItemBySkillTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsSkillTemplate->m_lID));

			if (pcsItem)
			{
				pThis->m_csSkillToolTip.AddNewLine(14);

				ZeroMemory(szBuffer, sizeof(szBuffer));
				pThis->m_pcsAgcmUICashInven->SetItemInfoTime(pcsItem, szBuffer, AGCMUICASHINVEN_CALLTYPE_BUFF_ICON);

				pThis->m_csSkillToolTip.AddString(szBuffer, 12);
				pThis->m_csSkillToolTip.AddNewLine(14);
			}
		}
	}
	
	ApBase*			pCharacter		=	NULL;

	if( strcmp( pcsSourceControl->m_szName , "CONTROL_2436" ) == 0 )
		pCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	else
		pCharacter	=	pThis->m_pcsAgcmUICharacter->GetTargetCharacter();


	//	BuffedList에 등록되어 있으면 시간을 표시해준다
	AgpdSkillAttachData *pcsAttachData = pThis->m_pcsAgpmSkill->GetAttachSkillData(pCharacter);
	if( pcsAttachData )
	{
		stAgpmSkillBuffList*		pSkillBuff	=	NULL;
		UINT32						nMinute		=	0;
		
		pSkillBuff		=	pThis->_GetBuffedSkill( &pcsAttachData->m_astBuffSkillList[0] , pGrid->m_pToolTipAgpdGridItem->m_lItemTID );
		if( pSkillBuff )
		{
			nMinute			=	pThis->_GetDurationTime( pSkillBuff->ulExpireTime );

			CTime			LastTime( pSkillBuff->ulExpireTime );

			if( pSkillBuff->ulExpireTime != 0 )
			{
				pThis->m_csSkillToolTip.AddNewLine( 14 );
				pThis->m_csSkillToolTip.AddNewLine( 14 );
				sprintf_s( szBuffer , 64 , pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TOOLTIP_TIME) );
				pThis->m_csSkillToolTip.AddString( szBuffer );

				pThis->m_csSkillToolTip.AddNewLine( 14 );
				sprintf_s( szBuffer , 64 , pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TOOLTIP_COMPLETETIME) , LastTime.GetYear() , LastTime.GetMonth() , LastTime.GetDay() , LastTime.GetHour() , LastTime.GetMinute() );
				pThis->m_csSkillToolTip.AddString( szBuffer );

				//pThis->m_csSkillToolTip.AddNewLine( 14 );
				//pThis->m_csSkillToolTip.AddNewLine( 14 );
				//sprintf_s( szBuffer , 64 , pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TOOLTIP_DURATIONTIME) ,  nMinute );
				//pThis->m_csSkillToolTip.AddString( szBuffer );

				pThis->m_csSkillToolTip.AddNewLine( 12 );
			}
		}
			
	}

	pThis->m_csSkillToolTip.ShowWindow( TRUE );

	return TRUE;
}

BOOL AgcmUISkill2::CBCloseSkillDescToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUISkill2			*pThis			= (AgcmUISkill2 *)	pClass;

	pThis->m_csSkillToolTip.ShowWindow(FALSE);
	pThis->m_csSkillToolTip.DeleteAllStringInfo();

	return TRUE;
}

INT32 AgcmUISkill2::GetCostWithTax(AgpdCharacter* pcsCharacter, INT32 cost)
{
	INT32 costWithTax = cost;

	if (NULL != pcsCharacter)
	{
		INT32 lTaxRatio = m_pcsAgpmCharacter->GetTaxRatio(pcsCharacter);
		if (lTaxRatio > 0)
			costWithTax += ((costWithTax * lTaxRatio) / 100);
	}

	return costWithTax;
}

UINT32		AgcmUISkill2::_GetDurationTime( UINT ulExpiredTime )
{
	time_t		CurrentTime;
	time( &CurrentTime );

	CTimeSpan	atlCurrentTime( CurrentTime );
	CTimeSpan	atlLastTime( (time_t)ulExpiredTime );

	atlLastTime	=	atlLastTime	- atlCurrentTime;

	LONGLONG	Result	=	atlLastTime.GetTotalMinutes();
	if( Result == 0 )
		Result	=	1;
	
	return	Result;
}

stAgpmSkillBuffList*		AgcmUISkill2::_GetBuffedSkill( stAgpmSkillBuffList*	pBUffList , INT32 lSkillTID )
{
	for( INT i = 0 ; i < AGPMSKILL_MAX_SKILL_BUFF  ; ++i )
	{
		if( pBUffList[i].lSkillTID	== lSkillTID )
			return &pBUffList[i];
	}

	return NULL;
}

VOID		AgcmUISkill2::_InitSkillTooltipColor( VOID )
{
	AgcmResourceLoader*	pLoader		=	static_cast< AgcmResourceLoader* >(GetModule( "AgcmResourceLoader" ));

	m_SkillTooltipColor[ AGCMUISKILL2_TOOLTIP_COLOR_NORMAL		]	=	pLoader->GetColor("SKILL_TOOLTIP_COLOR_NORMAL");
	m_SkillTooltipColor[ AGCMUISKILL2_TOOLTIP_COLOR_NEXT_LEVEL	]	=	pLoader->GetColor("SKILL_TOOLTIP_COLOR_NEXT_LEVEL");
	m_SkillTooltipColor[ AGCMUISKILL2_TOOLTIP_COLOR_DISABLE		]	=	pLoader->GetColor("SKILL_TOOLTIP_COLOR_DISABLE");
	m_SkillTooltipColor[ AGCMUISKILL2_TOOLTIP_COLOR_LACK_ABILITY]	=	pLoader->GetColor("SKILL_TOOLTIP_COLOR_LACK_ABILITY");
}