#include "AgpmEventSkillMaster.h"
#include "ApModuleStream.h"

BOOL AgpmEventSkillMaster::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent					*pcsEvent			= (ApdEvent *)					pData;
	AgpmEventSkillMaster		*pThis				= (AgpmEventSkillMaster *)		pClass;
	ApModuleStream				*pStream			= (ApModuleStream *)			pCustData;

	AgpdSkillEventAttachData	*pcsAttachData		= (AgpdSkillEventAttachData *)	pcsEvent->m_pvData;
	if (!pcsAttachData)
		return FALSE;

	if (!pStream->WriteValue(AGPMEVENT_SKILL_STREAM_NAME_RACE, (INT32) pcsAttachData->eRaceType))
	{
		OutputDebugString("AgpmEventSkillMaster::CBStreamWriteEvent() Error (1) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMEVENT_SKILL_STREAM_NAME_CLASS, (INT32) pcsAttachData->eClassType))
	{
		OutputDebugString("AgpmEventSkillMaster::CBStreamWriteEvent() Error (2) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMEVENT_SKILL_STREAM_NAME_EVENT_END, 0))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventSkillMaster::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ApdEvent					*pcsEvent			= (ApdEvent *)					pData;
	AgpmEventSkillMaster		*pThis				= (AgpmEventSkillMaster *)		pClass;
	ApModuleStream				*pStream			= (ApModuleStream *)			pCustData;

	AgpdSkillEventAttachData	*pcsAttachData		= (AgpdSkillEventAttachData *)	pcsEvent->m_pvData;
	if (!pcsAttachData)
		return FALSE;

	const CHAR					*szValueName		= NULL;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMEVENT_SKILL_STREAM_NAME_RACE))
		{
			INT32	lRaceType	= 0;

			if (!pStream->GetValue(&lRaceType))
			{
				TRACE("AgpmEventSkillMaster::CBStreamReadEvent() GetRaceType Failed\n"); 
				ASSERT(!"AgpmEventSkillMaster::CBStreamReadEvent() GetRaceType Failed");
			}

			pcsAttachData->eRaceType	= (AuRaceType) lRaceType;
		}
		else if (!strcmp(szValueName, AGPMEVENT_SKILL_STREAM_NAME_CLASS))
		{
			INT32	lClassType	= 0;

			if (!pStream->GetValue(&lClassType))
			{
				TRACE("AgpmEventSkillMaster::CBStreamReadEvent() GetClassType Failed\n"); 
				ASSERT(!"AgpmEventSkillMaster::CBStreamReadEvent() GetClassType Failed");
			}

			pcsAttachData->eClassType	= (AuCharClassType) lClassType;
		}
		else if (!strcmp(szValueName, AGPMEVENT_SKILL_STREAM_NAME_EVENT_END))
			break;
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::StreamReadMasteryTxt(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	INT16			nCurRow			= 0;
	SIZE_T			nIndex			= 0;

	for ( ; ; )
	{
		CHAR			*szClassName	= csExcelTxtLib.GetData(0, nCurRow++);
		if (!szClassName || !strlen(szClassName))
		{
			if (nCurRow > nRow)
				break;

			continue;
		}

		// 아크로드 스킬을 읽는다. 2006.10.12. steeple
		if (!_tcscmp(szClassName, AGPMEVENT_SKILL_STREAM_NAME_ARCHLORD))
		{
			nCurRow = StreamReadArchlordSkill(csExcelTxtLib, nRow, nColumn, nCurRow);
			if (nCurRow > nRow)
				break;

			continue;
		}

		// 종족전 스킬을 읽는다. 2006.12.18. steeple
		if (!_tcscmp(szClassName, AGPMEVENT_SKILL_STREAM_NAME_RACESKILL))
		{
			nCurRow = StreamReadRaceSkill(csExcelTxtLib, nRow, nColumn, nCurRow);
			if (nCurRow > nRow)
				break;

			continue;
		}

		for (nIndex	= 0; nIndex < strlen(szClassName); ++nIndex)
		{
			if (szClassName[nIndex] == ' ')
			{
				break;
			}
		}

		if (nIndex == strlen(szClassName))
			continue;

		CHAR	szRaceName[32];
		ZeroMemory(szRaceName, sizeof(CHAR) * 32);

		strncpy(szRaceName, szClassName, nIndex);

		// szRaceName을 보고 lRace 타입을 가져온다.
		INT32	lRaceType	= m_pcsAgpmFactors->FindCharacterRaceIndex(szRaceName);
		if (lRaceType >= AURACE_TYPE_MAX || lRaceType <= AURACE_TYPE_NONE)
		{
			if (nCurRow > nRow)
				break;

			continue;
		}

		// szClassName을 보고 lClass 타입을 가져온다.
		INT32	lClassType	= m_pcsAgpmFactors->FindCharacterClassIndex(szClassName + nIndex + 1);
		if (lClassType >= AUCHARCLASS_TYPE_MAX || lClassType <= AUCHARCLASS_TYPE_NONE)
		{
			if (nCurRow > nRow)
				break;

			continue;
		}

		INT32	lMasteryIndex		= 0;

		CHAR	*szMasteryName		= csExcelTxtLib.GetData(1, nCurRow);
		CHAR	*szSkillName		= csExcelTxtLib.GetData(2, nCurRow++);

		INT32	lNumMasterySkill	= 0;

		while (szSkillName && strlen(szSkillName))
		{
			if (szMasteryName && strlen(szMasteryName))
			{
				if (strcmp(AGPMEVENT_SKILL_STREAM_NAME_MASTERY_1, szMasteryName) == 0)
					lMasteryIndex	= 0;
				else if (strcmp(AGPMEVENT_SKILL_STREAM_NAME_MASTERY_2, szMasteryName) == 0)
					lMasteryIndex	= 1;
				else if (strcmp(AGPMEVENT_SKILL_STREAM_NAME_MASTERY_3, szMasteryName) == 0)
					lMasteryIndex	= 2;
				else if (strcmp(AGPMEVENT_SKILL_STREAM_NAME_MASTERY_4, szMasteryName) == 0)
					lMasteryIndex	= 3;
				else
					lMasteryIndex	= 4;

				lNumMasterySkill	= 0;
			}

			AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(szSkillName);
			if (pcsSkillTemplate && 
				(pcsSkillTemplate->m_nAttribute & AGPMSKILL_SKILL_ATTR_PRODUCT ||
				 pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][1] != 0.0f))
			{
				m_stMastery[lRaceType][lClassType][lMasteryIndex][lNumMasterySkill]	= pcsSkillTemplate->m_lID;

				AgpdEventSkillAttachTemplateData	*pcsAttachTemplateData	= GetSkillAttachTemplateData(pcsSkillTemplate);
				pcsAttachTemplateData->lMasteryIndex	= lMasteryIndex;

				++lNumMasterySkill;
			}
			else
			{
				lNumMasterySkill	= lNumMasterySkill;
			}

			szMasteryName		= csExcelTxtLib.GetData(1, nCurRow);
			szSkillName			= csExcelTxtLib.GetData(2, nCurRow++);
		}

		if (nCurRow > nRow)
			break;
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::StreamReadMasteryEvolution(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	INT16			nCurRow			= 0;
	SIZE_T			nIndex			= 0;

	m_vcSkillMasteryEvolution.Initialize();

	for ( INT16 nCurRow = 1; nCurRow < nRow; nCurRow++)
	{
		CHAR* szTID = csExcelTxtLib.GetData(0, nCurRow);
		CHAR* szName = csExcelTxtLib.GetData(1, nCurRow);
		CHAR* szPreviousTID = csExcelTxtLib.GetData(2, nCurRow);

		if(!szTID || !szName || !szPreviousTID)
			return FALSE;

		INT32 lTID		= atoi(szTID);
		INT32 lPrevTID	= atoi(szPreviousTID);

		m_vcSkillMasteryEvolution.Insert(lTID, lPrevTID);
	}

	return TRUE;
}

BOOL AgpmEventSkillMaster::StreamReadDefaultSkill(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	INT16			nCurRow			= 0;

	for ( ; ; )
	{
		CHAR			*szCharTID		= csExcelTxtLib.GetData(0, nCurRow++);
		if (!szCharTID)
		{
			if (nCurRow > nRow)
				break;

			continue;
		}

		AgpdCharacterTemplate	*pcsCharacterTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(atoi(szCharTID));
		if (!pcsCharacterTemplate)
		{
			if (nCurRow > nRow)
				break;

			continue;
		}

		for (int i = 3; i < nColumn; ++i)
		{
			CHAR	*szSkillName	= csExcelTxtLib.GetData(i, nCurRow - 1);

			if (!szSkillName || !szSkillName[0])
				break;

			AgpdSkillTemplate	*pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(szSkillName);
			if (!pcsSkillTemplate)
				break;

			AgpdEventSkillAttachCharTemplateData	*pcsCharTemplateAttachData	= GetSkillAttachCharTemplateData(pcsCharacterTemplate);

			if (!pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate)
			{
				pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate	= new AgpdSkillTemplate *[nColumn - 3 + 1];

				ZeroMemory(pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate, sizeof(AgpdSkillTemplate *) * (nColumn - 3 + 1));
			}

			if (!pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate)
				break;

			pcsCharTemplateAttachData->m_apcsDefaultSkillTemplate[pcsCharTemplateAttachData->m_lNumDefaultSkill++]	= pcsSkillTemplate;
		}

		if (nCurRow > nRow)
			break;
	}
	
	return TRUE;
}

// 2007.05.19. steeple
// 고레벨 스킬 트리
BOOL AgpmEventSkillMaster::StreamReadHighLevelSkill(CHAR* szFile, BOOL bDecryption)
{
	if(!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16 nRow = csExcelTxtLib.GetRow();
	INT16 nColumn = csExcelTxtLib.GetColumn();

	// 컬럼명 한줄 보낸다.
	INT16 nCurRow = 1;

	while(true)
	{
		CHAR* szCharTID = csExcelTxtLib.GetData(0, nCurRow++);
		if(!szCharTID)
		{
			if(nCurRow > nRow)
				break;

			continue;
		}

		CHAR* pSplit	= _tcsstr(szCharTID, ";");
		INT32 lCharRace = 0;
		CHAR pRace[16];
		memset(pRace, 0, sizeof(pRace));
		if(pSplit)
		{	
			size_t lTotalLength = _tcslen(szCharTID);	
			memcpy(pRace, pSplit + 1, (INT32)lTotalLength - (pSplit - szCharTID + 1));
		}

		INT32 lCharTID = atoi(szCharTID);
		if(!lCharTID)
			continue;

		HighLevelSkillVector vtHighLevel;

		while(true)
		{
			CHAR* szSkillTID = csExcelTxtLib.GetData(1, nCurRow);
			if(!szSkillTID)
			{
				break;
			}

			AgpdEventSkillHighLevel stHighLevelSkill;
			memset(&stHighLevelSkill, 0, sizeof(stHighLevelSkill));

			stHighLevelSkill.m_lCharTID = lCharTID;
			stHighLevelSkill.m_lSkillTID = atoi(szSkillTID);
			stHighLevelSkill.m_lCharRace = atoi(pRace);

			AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHighLevelSkill.m_lSkillTID);
			if(!pcsSkillTemplate)
				break;

			pcsSkillTemplate->m_bHighLevelSkill = TRUE;

			INT32 lCondition = 0;
			for(INT32 j = 3; j < 3 + AGPMEVENT_SKILL_MAX_HIGHLEVEL_CONDITION; ++j)
			{
				CHAR* szCondition = csExcelTxtLib.GetData(j, nCurRow);
				if(!szCondition)
					continue;

				CHAR* pDest = _tcsstr(szCondition, ";");
				if(!pDest)
					continue;

				size_t lConditionLength = _tcslen(szCondition);
				if((INT32)lConditionLength - (pDest - szCondition + 1) < 0)
					continue;

				CHAR pBuffer[32];
				memset(pBuffer, 0, sizeof(pBuffer));

				switch(j)
				{
				case AGPMEVENT_SKILL_OPTION_CONDITIONA:
				case AGPMEVENT_SKILL_OPTION_CONDITIONB:
					{
						memcpy(pBuffer, szCondition, pDest - szCondition);

						stHighLevelSkill.m_stCondition[lCondition].m_lSkillTID = atoi(pBuffer);

						memset(pBuffer, 0, sizeof(pBuffer));
						memcpy(pBuffer, pDest + 1, (INT32)lConditionLength - (pDest - szCondition + 1));

						stHighLevelSkill.m_stCondition[lCondition].m_lSkillLevel = atoi(pBuffer);
					}
				break;

				case AGPMEVENT_SKILL_OPTION_CONDITIONC:
					{
						memcpy(pBuffer, szCondition, pDest - szCondition);

						stHighLevelSkill.m_stCondition[lCondition].m_lSkillTab = atoi(pBuffer);

						memset(pBuffer, 0, sizeof(pBuffer));
						memcpy(pBuffer, pDest + 1, (INT32)lConditionLength - (pDest - szCondition + 1));

						stHighLevelSkill.m_stCondition[lCondition].m_lSkillPoint = atoi(pBuffer);
					}
				break;

				case AGPMEVENT_SKILL_OPTION_CONDITIOND:
					{
						memcpy(pBuffer, szCondition, pDest - szCondition);

						stHighLevelSkill.m_stCondition[lCondition].m_lSkillTotalPoint = atoi(pBuffer);
					}
				break;

				}

				++lCondition;
			}

			vtHighLevel.push_back(stHighLevelSkill);

			++nCurRow;
		}

		m_mapHighLevelSkill.insert(pair<INT32, HighLevelSkillVector>(lCharTID, vtHighLevel));

		if(nCurRow > nRow)
			break;
	}
	
	return TRUE;
}

BOOL AgpmEventSkillMaster::StreamReadHeroicSkill(CHAR *szFile, BOOL bDecryption)
{
	if(NULL == szFile)
		return FALSE;

	AuExcelTxtLib csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16 nRow = csExcelTxtLib.GetRow();
	INT16 nColumn = csExcelTxtLib.GetColumn();

	// Except First Row because It is a title Row
	INT16 nCurRow = 1;

	while(TRUE)
	{
		CHAR* szCharTID = csExcelTxtLib.GetData(0, nCurRow++);
		if(!szCharTID)
		{
			if(nCurRow > nRow)
				break;

			continue;
		}

		INT32 lCharTID = atoi(szCharTID);
		if(!lCharTID)
			continue;

		HeroicSkillVector	vtHeroicSkill;

		while(TRUE)
		{
			CHAR *szSkillTID = csExcelTxtLib.GetData(1, nCurRow);
			if(!szSkillTID)
			{
				break;
			}

			AgpdEventSkillHeroic	stHeroicSkill;
			ZeroMemory(&stHeroicSkill, sizeof(stHeroicSkill));

			stHeroicSkill.m_lCharTID = lCharTID;
			stHeroicSkill.m_lSkillTID = atoi(szSkillTID);

			AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(stHeroicSkill.m_lSkillTID);
			if(!pcsSkillTemplate)
				break;
			
			pcsSkillTemplate->m_bHeroicSkill = TRUE;

			INT32	lCondition = 0;
			for(INT32 j = 3; j < 3 + AGPMEVENT_SKILL_MAX_HEROIC_CONDITION; ++j)
			{
				CHAR* szCondition = csExcelTxtLib.GetData(j, nCurRow);
				if(!szCondition)
					continue;

				CHAR* pDest = _tcsstr(szCondition, ";");
				if(!pDest)
					continue;

				size_t lConditionLength = _tcslen(szCondition);
				if((INT32)lConditionLength - (pDest - szCondition + 1) < 0)
					continue;

				CHAR pBuffer[32];
				ZeroMemory(pBuffer, sizeof(pBuffer));

				switch(j)
				{
				case AGPMEVENT_SKILL_OPTION_CONDITIONA:
					{
						// This part make a conditionA to learn skill
						//////////////////////////////////////////////////////////////////////////////
						memcpy(pBuffer, szCondition, pDest - szCondition);

						stHeroicSkill.m_stCondition[lCondition].m_lSkillTID = atoi(pBuffer);

						ZeroMemory(pBuffer, sizeof(pBuffer));
						memcpy(pBuffer, pDest + 1, (INT32)lConditionLength - (pDest - szCondition + 1));

						stHeroicSkill.m_stCondition[lCondition].m_lSkillLevel = atoi(pBuffer);
						///////////////////////////////////////////////////////////////////////////////
					} break;

				case AGPMEVENT_SKILL_OPTION_CONDITIOND:
					{
						// This part make a conditionD to learn skill (Check Total Used HeroicPoint)
						///////////////////////////////////////////////////////////////////////////////
						memcpy(pBuffer, szCondition, pDest - szCondition);

						stHeroicSkill.m_stCondition[lCondition].m_lUsedTotalHeroicPoint = atoi(pBuffer);
					} break;
				}

				++lCondition;
			}

			vtHeroicSkill.push_back(stHeroicSkill);

			++nCurRow;
		}

		m_mapHeroicSkill.insert(pair<INT32, HeroicSkillVector>(lCharTID, vtHeroicSkill));

		if(nCurRow > nRow)
			break;
	}

	return TRUE;
}

INT16 AgpmEventSkillMaster::StreamReadArchlordSkill(AuExcelLib& csExcelTxtLib, INT16 nRow, INT16 nCol, INT16 nCurRow)
{
	CHAR* szSkillName = NULL;
	AgpdSkillTemplate* pcsSkillTemplate = NULL;

	while(TRUE)
	{
		szSkillName = csExcelTxtLib.GetData(2, nCurRow++);
		if(!szSkillName || _tcslen(szSkillName) < 1)
		{
			if(nCurRow > nRow)
				break;

			break;
		}

		pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(szSkillName);
		if(!pcsSkillTemplate)
		{
			if(nCurRow > nRow)
				break;

			break;
		}

		m_vcArchlordSkill.push_back(pcsSkillTemplate->m_lID);
		
		if(nCurRow > nRow)
			break;
	}
	
	return nCurRow;
}

INT16 AgpmEventSkillMaster::StreamReadRaceSkill(AuExcelLib& csExcelTxtLib, INT16 nRow, INT16 nCol, INT16 nCurRow)
{
	CHAR* szSkillName = NULL;
	AgpdSkillTemplate* pcsSkillTemplate = NULL;

	while(TRUE)
	{
		szSkillName = csExcelTxtLib.GetData(2, nCurRow++);
		if(!szSkillName || _tcslen(szSkillName) < 1)
		{
			if(nCurRow > nRow)
				break;

			break;
		}

		pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(szSkillName);
		if(!pcsSkillTemplate)
		{
			if(nCurRow > nRow)
				break;

			break;
		}

		m_vcRaceSkill.push_back(pcsSkillTemplate->m_lID);
		
		if(nCurRow > nRow)
			break;
	}
	
	return nCurRow;
}