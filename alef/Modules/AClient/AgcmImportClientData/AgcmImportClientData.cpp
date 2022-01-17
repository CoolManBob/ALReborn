#include "AgcmImportClientData.h"

/******************************************************************************
* Purpose :
*
* 061503. Bob Jung
******************************************************************************/
AgcmImportClientData::AgcmImportClientData()
{
	SetModuleName("AgcmImportClientData");

	m_pcsAgpmFactors		= NULL;
	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmItem			= NULL;
	m_bOpenProcessInfoDlg	= FALSE;
}

/******************************************************************************
* Purpose :
*
* 032403. Bob Jung
******************************************************************************/
BOOL AgcmImportClientData::OnInit()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 032403. Bob Jung
******************************************************************************/
BOOL AgcmImportClientData::OnAddModule()
{
	m_pcsAgpmFactors	= (AgpmFactors *)(GetModule("AgpmFactors"));
	if(!m_pcsAgpmFactors)
	{
		OutputDebugString("AgcmImportClientData::OnAddModule() Error (1) !!!\n");
		return FALSE;
	}

	m_pcsAgpmCharacter	= (AgpmCharacter *)(GetModule("AgpmCharacter"));
	if(!m_pcsAgpmCharacter)
	{
		OutputDebugString("AgcmImportClientData::OnAddModule() Error (2) !!!\n");
		return FALSE;
	}

	m_pcsAgpmItem		= (AgpmItem *)(GetModule("AgpmItem"));
	if(!m_pcsAgpmItem)
	{
		OutputDebugString("AgcmImportClientData::OnAddModule() Error (3) !!!\n");
		return FALSE;
	}

	if (!m_pcsAgpmCharacter->SetCallbackStreamReadImportDataErrorReport(CBImportCharacterDataErrorReport, this))
	{
		OutputDebugString("AgcmImportClientData::OnAddModule() Error (4) !!!\n");
		return FALSE;
	}

	if (!m_pcsAgpmItem->SetCallbackStreamReadImportDataErrorReport(CBImportItemDataErrorReport, this))
	{
		OutputDebugString("AgcmImportClientData::OnAddModule() Error (5) !!!\n");
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 032403. Bob Jung
******************************************************************************/
BOOL AgcmImportClientData::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 032403. Bob Jung
******************************************************************************/
BOOL AgcmImportClientData::OnDestroy()
{
	return TRUE;
}

/******************************************************************************
* Purpose : 아이템 데이터를 import한다.
*
* Params :	szPath	- 아이템 정보가 있는 파일의 path.
*
* 061503. Bob Jung
******************************************************************************/
BOOL AgcmImportClientData::ImportItemData(CHAR *szPath)
{
	if(m_bOpenProcessInfoDlg)
	{
		if(!m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
		{
			if(!m_csAgcmProcessInfoDlg.OpenProcessInfoDlg("[IMPORT ITEM]"))
			{
				OutputDebugString("AgcmImportClientData::ImportItemData() Error (1-2) !!!\n");
				return FALSE;
			}
		}
		else
		{
			m_csAgcmProcessInfoDlg.SetProcessInfoDlg("[IMPORT ITEM]");
		}
	}

	CHAR	szDebug[256];
	ZeroMemory(szDebug, sizeof(CHAR) * 256);

	if (!m_pcsAgpmItem->StreamReadImportData(szPath, szDebug, FALSE))
	{
		if (m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
			m_csAgcmProcessInfoDlg.SetProcessInfoDlg(szDebug);

		return FALSE;
	}

	if(m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
	{
		m_csAgcmProcessInfoDlg.SetProcessInfoDlg("Fin > Import item");
		m_csAgcmProcessInfoDlg.SetEmptyProcessInfoDlg(3);
	}

	return TRUE;
}

/******************************************************************************
* Purpose : 캐릭터 데이터를 import한다.
*
* Params :	szPath	- 캐릭터 정보가 있는 파일의 path.
*
* 032403. Bob Jung
******************************************************************************/
BOOL AgcmImportClientData::ImportCharacterData(CHAR *szPath)
{
	if(m_bOpenProcessInfoDlg)
	{
		if(!m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
		{
			if(!m_csAgcmProcessInfoDlg.OpenProcessInfoDlg("[IMPORT CHARACTER]"))
			{
				OutputDebugString("AgcmImportClientData::ImportCharacterData() Error (1-2) !!!\n");
				return FALSE;
			}
		}
		else
		{
			m_csAgcmProcessInfoDlg.SetProcessInfoDlg("[IMPORT CHARACTER]");
		}
	}

	CHAR	szDebug[256];
	ZeroMemory(szDebug, sizeof(CHAR) * 256);

	if (!m_pcsAgpmCharacter->StreamReadImportData(szPath, szDebug, FALSE))
	{
		if (m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
			m_csAgcmProcessInfoDlg.SetProcessInfoDlg(szDebug);

		return FALSE;
	}

	if(m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
	{
		m_csAgcmProcessInfoDlg.SetProcessInfoDlg("Fin > Import character");
		m_csAgcmProcessInfoDlg.SetEmptyProcessInfoDlg(3);
	}

	return TRUE;
}

/*
BOOL AgcmImportClientData::SetDefaultEquipments(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lKind, INT32 lPart)
{
	AgpdItemADCharTemplate *pstAgpdItemADCharTemplate = m_pcsAgpmItem->GetADCharacterTemplate(pstAgpdCharacterTemplate);
	if(!pstAgpdItemADCharTemplate)
		return FALSE;

	if(lITID)
	{
		AgpdItemTemplate *pstAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate(lITID);
		if((!pstAgpdItemTemplate) || (pstAgpdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP))
			return FALSE;

		AgpdItemTemplateEquip *pstAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(pstAgpdItemTemplate);
		if((pstAgpdItemTemplateEquip->m_nKind != lKind) || ((lPart) && (pstAgpdItemTemplateEquip->m_nPart != lPart)))
			return FALSE;

		pstAgpdItemADCharTemplate = m_pcsAgpmItem->GetADCharacterTemplate(pstAgpdCharacterTemplate);
		if(!pstAgpdItemADCharTemplate)
			return FALSE;

		pstAgpdItemADCharTemplate->m_lDefaultEquipITID[pstAgpdItemTemplateEquip->m_nPart] = lITID;
	}
	else
	{
		if(lKind == AGPMITEM_EQUIP_KIND_ARMOUR)
		{
			if(lPart)
			{
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[lPart] = 0;
			}
			else
			{
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_BODY] = 0;
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_HEAD] = 0;
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_ARMS] = 0;
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_HANDS] = 0;
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_LEGS] = 0;
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_FOOT] = 0;
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_CLOAK] = 0;
			}
		}
		else if((lKind == AGPMITEM_EQUIP_KIND_WEAPON) || (lKind == AGPMITEM_EQUIP_KIND_SHIELD))
		{
			if(lPart)
			{
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[lPart] = 0;
			}
			else
			{
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_HAND_LEFT] = 0;
				pstAgpdItemADCharTemplate->m_lDefaultEquipITID[AGPMITEM_PART_HAND_RIGHT] = 0;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmImportClientData::SetDefaultInventoryItems(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lNum, INT32 lIndex)
{
	AgpdItemTemplate *pstAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate(lITID);
	if(!pstAgpdItemTemplate)
		return FALSE;

	AgpdItemADCharTemplate *pstAgpdItemADCharTemplate = m_pcsAgpmItem->GetADCharacterTemplate(pstAgpdCharacterTemplate);
	if(!pstAgpdItemADCharTemplate)
		return FALSE;

	pstAgpdItemADCharTemplate->m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID]			= lITID;
	pstAgpdItemADCharTemplate->m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT]	= lNum;

	return TRUE;
}
*/

/******************************************************************************
* Purpose : 비어있는 캐릭터 TID를 검색한다.
*
* Params :	pcsAgpmCharacter	-	등록된 public 캐릭터 모듈.
*
* Desc : 0을 리턴하면 실패다.
*
* 032403. Bob Jung
******************************************************************************/
/*INT32 AgcmImportClientData::SearchEmptyCharacterTID()
{
	INT32 lTID = 0;
	for(AgpdCharacterTemplate *pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(++lTID); pcsTemplate; pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(++lTID));

	return lTID;
}*/

/******************************************************************************
* Purpose : TID가 비어있는지 비교한다.
*
* 032403. Bob Jung
******************************************************************************/
/*BOOL AgcmImportClientData::IsEmptyCharacterTID(INT32 lTID)
{
	if(!m_pcsAgpmCharacter->GetCharacterTemplate(lTID))
		return TRUE;

	return FALSE;
}*/

/******************************************************************************
* Purpose : ID(TID)를 파싱해서 character template을 맹근당....
*
* Params :	nColumn				-	ID가 속해있는 열.
*
* 032403. Bob Jung
******************************************************************************/
/*BOOL AgcmImportClientData::ParseCharacterID(INT16 nColumn)
{
	INT32	lTID;
	char	*pszData;

	for(INT16 nRow = 1; nRow < m_csAuExcelTxtLib.GetRow(); ++nRow)
	{
		pszData = m_csAuExcelTxtLib.GetData(nColumn, nRow);
		if(!pszData)
		{
			OutputDebugString("AgcmImportClientData::ParseCharacterStatusID() Error (1) !!!\n");
			return FALSE;
		}

		lTID = atoi(pszData);
		if(!m_pcsAgpmCharacter->AddCharacterTemplate(lTID))
		{
			OutputDebugString("AgcmImportClientData::ParseCharacterStatusID() Error (3) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}*/


/******************************************************************************
* Purpose : Factor를 설정한다.
*
* Params :	pcsAgpdFactor		-	설정할 Factor
			nRow				-	값을 파싱할 행.
*			nColumn				-	파싱할 열.
*			eType				-	Factor type.
*			lSubType...			-	Factor sub type...
*
* 032403. Bob Jung
******************************************************************************/
/*
BOOL AgcmImportClientData::SetFactor(AgpdFactor *pcsAgpdFactor, INT16 nRow, INT16 nColumn, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	INT32					lValue;
	char					*pszData;

	pszData = m_csAuExcelTxtLib.GetData(nColumn, nRow);
	if(!pszData)
	{
		return TRUE;
	}

	lValue = atoi(pszData);

	// Factor를 할당한다.
	if(!m_pcsAgpmFactors->SetFactor(pcsAgpdFactor, NULL, eType))
	{
		OutputDebugString("AgcmImportClientData::SetFactor() Error (4) !!!\n");
		return FALSE;
	}

	// Factor를 설정한다.
	if(!m_pcsAgpmFactors->SetValue(pcsAgpdFactor, lValue, eType, lSubType1, lSubType2, lSubType3))
	{
		OutputDebugString("AgcmImportClientData::SetFactor() Error (5) !!!\n");
		return FALSE;
	}

	return TRUE;
}
*/

/******************************************************************************
* Purpose : Race, Gender, Class 조합으로 캐릭터 템플릿 이름을 정한다.
*
* 032503. Bob Jung
******************************************************************************/
/*BOOL AgcmImportClientData::SetCharacterTemplateName()
{
	INT32	lIndex = 0;
	INT32	lRace, lGender, lClass;

	if(!m_pcsAgpmFactors->CharacterTypeStreamRead(AGCM_IMPORT_CHAR_TYPE_INI_PATH))
	{
		OutputDebugString("AgcmImportClientData::SetCharacterTemplateName() Error (1) !!!\n");
		return FALSE;
	}

	AgpdFactorCharTypeNameSet *pstCharTypeName = m_pcsAgpmFactors->GetCharTypeNameSet();

	for(AgpdCharacterTemplate *pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); pcsAgpdCharacterTemplate; pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		if(!m_pcsAgpmFactors->GetValue(&pcsAgpdCharacterTemplate->m_csFactor, &lRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE))
		{
			OutputDebugString("AgcmImportClientData::SetCharacterTemplateName() Error (2) !!!\n");
			return FALSE;
		}

		if(!m_pcsAgpmFactors->GetValue(&pcsAgpdCharacterTemplate->m_csFactor, &lGender, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_GENDER))
		{
			OutputDebugString("AgcmImportClientData::SetCharacterTemplateName() Error (3) !!!\n");
			return FALSE;
		}

		if(!m_pcsAgpmFactors->GetValue(&pcsAgpdCharacterTemplate->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS))
		{
			OutputDebugString("AgcmImportClientData::SetCharacterTemplateName() Error (4) !!!\n");
			return FALSE;
		}

		sprintf(pcsAgpdCharacterTemplate->m_szTName, "%s%s%s", pstCharTypeName->m_stRace[lRace].m_szName, pstCharTypeName->m_stGender[lGender].m_szName, pstCharTypeName->m_stClass[lRace][lClass].m_szName);
	}

	return TRUE;
}*/

/******************************************************************************
* Purpose : 캐릭터 템플릿 이름으로 캐릭터 템플릿을 찾아서 템플릿 포인터를 넘겨준다.
*
* Desc : 실패시 NULL을 리턴한다.
*
* 061503. Bob Jung
******************************************************************************/
AgpdCharacterTemplate *AgcmImportClientData::SearchCharacterTemplate(CHAR *szTName)
{
	INT32 lIndex	= 0;
	for(AgpdCharacterTemplate *pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); pcsAgpdCharacterTemplate; pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		if(!strcmp(pcsAgpdCharacterTemplate->m_szTName, szTName))
		{
			return pcsAgpdCharacterTemplate;
		}
	}

	return NULL;
}

/******************************************************************************
* Purpose : 아이템 템플릿 이름으로 아이템 템플릿을 찾아서 템플릿 포인터를 넘겨준다.
*
* Desc : 실패시 NULL을 리턴한다.
*
* 061503. Bob Jung
******************************************************************************/
AgpdItemTemplate *AgcmImportClientData::SearchItemTemplate(CHAR *szTName)
{
	INT32 lIndex	= 0;
	for(AgpdItemTemplate *pcsAgpdItemTemplate = m_pcsAgpmItem->GetTemplateSequence(&lIndex); pcsAgpdItemTemplate; pcsAgpdItemTemplate = m_pcsAgpmItem->GetTemplateSequence(&lIndex))
	{
		if(!strcmp(pcsAgpdItemTemplate->m_szName, szTName))
		{
			return pcsAgpdItemTemplate;
		}
	}

	return NULL;
}

VOID AgcmImportClientData::SetOpenProcessInfoDlg()
{
	m_bOpenProcessInfoDlg = TRUE;
}

BOOL AgcmImportClientData::CloseProcessInfoDlg()
{
//	m_bOpenProcessInfoDlg = FALSE;

	if(m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
	{
		m_csAgcmProcessInfoDlg.CloseProcessInfoDlg();
	}
	else return FALSE;

	return TRUE;
}

BOOL AgcmImportClientData::CBImportCharacterDataErrorReport(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmImportClientData		*pThis					= (AgcmImportClientData *)		pClass;
	CHAR						*szDebugString			= (CHAR *)						pData;

	if (pThis->m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
		pThis->m_csAgcmProcessInfoDlg.SetProcessInfoDlg(szDebugString);

	return TRUE;
}

BOOL AgcmImportClientData::CBImportItemDataErrorReport(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmImportClientData		*pThis					= (AgcmImportClientData *)		pClass;
	CHAR						*szDebugString			= (CHAR *)						pData;

	if (pThis->m_csAgcmProcessInfoDlg.IsOpenProcessInfoDlg())
		pThis->m_csAgcmProcessInfoDlg.SetProcessInfoDlg(szDebugString);

	return TRUE;
}

/******************************************************************************
******************************************************************************/