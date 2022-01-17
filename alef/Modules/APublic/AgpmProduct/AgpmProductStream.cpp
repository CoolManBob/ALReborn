/*=============================================================

	AgpmProductStream.cpp

=============================================================*/

#include "AgpmProduct.h"


//	Stream
//========================================
//
BOOL AgpmProduct::StreamReadCategory(CHAR* pszFile, BOOL bDecryption)
	{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;
	
	INT32 lIndex = 0;
	INT32 eCategory = AGPMPRODUCT_CATEGORY_NONE;
	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
		{
		CHAR *psz = csExcelTxtLib.GetData(AGPMPRODUCT_CATEGORY_EXCEL_COLUMN_CATEGORY, lRow);
		if (psz && '\0' != *psz)
			{
			for (INT i=0; i<AGPMPRODUCT_CATEGORY_MAX; ++i)
				{
				if (0 == stricmp(g_szProductCategory[i], psz))
					{
					eCategory = i;
					break;
					}
				}
			}

		if (AGPMPRODUCT_CATEGORY_NONE == eCategory)
			continue;
			
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_CATEGORY_EXCEL_COLUMN_SKILLNAME, lRow);
		if (!psz || _T('\0') == *psz)
			continue;
		
		AgpdSkillTemplate *pAgpdSkillTemplate = m_pAgpmSkill->GetSkillTemplate(psz);
		if (!pAgpdSkillTemplate)
			continue;
		
		if (lIndex > AGPMPRODUCT_CATEGORY_ARRAY_MAX)
			{
			ASSERT(!"생산카테고리의 최대 배열사이즈를 넘어갔습니다.");
			return FALSE;
			}
		m_Category[lIndex].m_eCategory = eCategory;
		m_Category[lIndex].m_lSkillTID = pAgpdSkillTemplate->m_lID;
		++lIndex;
		}
	
	return (lIndex > 0);
	}


BOOL AgpmProduct::StreamReadCompose(CHAR* pszFile, BOOL bDecryption)
	{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;

	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
		{
		AgpdComposeTemplate *pAgpdComposeTemplate = (AgpdComposeTemplate *) CreateModuleData(AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE);
		ZeroMemory(pAgpdComposeTemplate, sizeof(AgpdComposeTemplate));

		CHAR *psz = NULL;

		// category
		INT32 eCategory = AGPMPRODUCT_CATEGORY_NONE;
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_CATEGORY, lRow);
		if (!psz || _T('\0') == *psz)
			{
			DestroyModuleData(pAgpdComposeTemplate, AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE);
			continue;
			}

		for (INT i=0; i<AGPMPRODUCT_CATEGORY_MAX; ++i)
			{
			if (0 == stricmp(g_szProductCategory[i], psz))
				{
				eCategory = i;
				break;
				}
			}

		if (AGPMPRODUCT_CATEGORY_NONE == eCategory)
			{
			DestroyModuleData(pAgpdComposeTemplate, AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE);
			continue;
			}
			
		pAgpdComposeTemplate->m_eCategory = eCategory;

		// result item
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RESULT_TID, lRow);
		if (!psz)
			{
			DestroyModuleData(pAgpdComposeTemplate, AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE);
			continue;
			}
		pAgpdComposeTemplate->m_lComposeID = atoi(psz);			// result item tid to compose id
		pAgpdComposeTemplate->m_ResultItem.m_pItemTemplate =  m_pAgpmItem->GetItemTemplate(pAgpdComposeTemplate->m_lComposeID);
		pAgpdComposeTemplate->m_ResultItem.m_lCount = 1;

		// result name
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RESULT_NAME, lRow);
		if (!psz || _T('\0') == *psz)
			{
			DestroyModuleData(pAgpdComposeTemplate, AGPMPRODUCT_DATATYPE_COMPOSE_TEMPLATE);
			continue;
			}
		strcpy(pAgpdComposeTemplate->m_szCompose, psz);

		// making price
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_PRICE, lRow);
		if (psz)
			pAgpdComposeTemplate->m_lPrice = atoi(psz);
		else
			pAgpdComposeTemplate->m_lPrice = 0;

		// success prob.
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_SUCCESS_PROB, lRow);
		pAgpdComposeTemplate->m_lSuccessProb = atoi(psz);

		// receipe
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RECEIPE_TID, lRow);
		if (psz)
			pAgpdComposeTemplate->m_Receipe.m_pItemTemplate	= m_pAgpmItem->GetItemTemplate(atoi(psz));
		else
			pAgpdComposeTemplate->m_Receipe.m_pItemTemplate	= NULL;

		// additional success prob.
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_RECEIPE_ADD_PROB, lRow);
		if (psz)
			pAgpdComposeTemplate->m_lAdditionalProb = atoi(psz);
		else
			pAgpdComposeTemplate->m_lAdditionalProb = 0;

		// level
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_LEVEL, lRow);
		if (psz)
			pAgpdComposeTemplate->m_lLevel = atoi(psz);
		else
			pAgpdComposeTemplate->m_lLevel = 1;

		// need title requirement
		psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_USING_MONSTER, lRow);
		if (psz)
			pAgpdComposeTemplate->m_lNeedTitleTid = atoi(psz);
		else
			pAgpdComposeTemplate->m_lNeedTitleTid = 0;

		
		// required items
		pAgpdComposeTemplate->m_lRequiredItems = 0;
		pAgpdComposeTemplate->m_RequiredItems.MemSetAll();
		for (INT32 j=0; j < AGPMPRODUCT_MAX_COMPOSE_SOURCE; ++j)
			{
			psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_A_TID + (j*3), lRow);
			if (!psz || _T('\0') == *psz)
				continue;
				
			INT32 lTID = atoi(psz);
			if (0 == lTID)
				continue;

			pAgpdComposeTemplate->m_RequiredItems[pAgpdComposeTemplate->m_lRequiredItems].m_pItemTemplate = m_pAgpmItem->GetItemTemplate(lTID);

			psz = csExcelTxtLib.GetData(AGPMPRODUCT_COMPOSE_EXCEL_COLUMN_ITEM_A_COUNT + (j*3), lRow);
			if (!psz || _T('\0') == *psz)
				{
				pAgpdComposeTemplate->m_RequiredItems[pAgpdComposeTemplate->m_lRequiredItems].m_pItemTemplate = NULL;
				continue;
				}
				
			pAgpdComposeTemplate->m_RequiredItems[pAgpdComposeTemplate->m_lRequiredItems].m_lCount = atoi(psz);
		
			if (pAgpdComposeTemplate->m_RequiredItems[pAgpdComposeTemplate->m_lRequiredItems].m_pItemTemplate)
				++pAgpdComposeTemplate->m_lRequiredItems;

			}
		
		m_csAdminComposeTemplate.Add(pAgpdComposeTemplate);
		}

	return TRUE;
	};


BOOL AgpmProduct::StreamReadFactor(CHAR* pszFile, BOOL bDecryption)
	{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;

	INT32 lRead = 0;
	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
		{
		// check skill template
		CHAR *psz = csExcelTxtLib.GetData(AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_SKILL_NAME, lRow);
		if (!psz || !strlen(psz))
			continue;

		AgpdSkillTemplate *pAgpdSkillTemplate = m_pAgpmSkill->GetSkillTemplate(psz);
		if (!pAgpdSkillTemplate)
			continue;
		
		AgpdProductSkillFactor *pAgpdProductSkillFactor = m_csAdminSkillFactor.Get(psz);
		if (!pAgpdProductSkillFactor)
			{
			pAgpdProductSkillFactor = (AgpdProductSkillFactor *) CreateModuleData(AGPMPRODUCT_DATATYPE_SKILL_FACTOR);
			if (!pAgpdProductSkillFactor)
				return FALSE;

			strcpy(pAgpdProductSkillFactor->m_szName, psz);
			pAgpdProductSkillFactor->m_lMaxLevel = 1;
			ZeroMemory(pAgpdProductSkillFactor->m_lFactors, sizeof(INT32) * (AGPMPRODUCT_MAX_LEVEL+1) * AGPMPRODUCT_SKILL_FACTOR_NUM);
			if (!m_csAdminSkillFactor.Add(pAgpdProductSkillFactor))
				return FALSE;
			}
		
		// level
		INT32 lLevel = atoi(csExcelTxtLib.GetData(AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_LEVEL, lRow));
		if (lLevel <= 0 || lLevel > AGPMPRODUCT_MAX_LEVEL)
			continue;

		pAgpdProductSkillFactor->m_lMaxLevel = max(pAgpdProductSkillFactor->m_lMaxLevel, lLevel);

		// min exp.
		pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_MIN_EXP] =
							atoi(csExcelTxtLib.GetData(AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_MIN_EXP, lRow));

		// max exp.
		pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_MAX_EXP] =
							atoi(csExcelTxtLib.GetData(AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_MAX_EXP, lRow));

		// exp. to next level
		pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_LEVELUP_EXP] =
							atoi(csExcelTxtLib.GetData(AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_LEVELUP_EXP, lRow));

		// total exp. to next level
		pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_ACC_LEVELUP_EXP] =
							atoi(csExcelTxtLib.GetData(AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_ACC_LEVELUP_EXP, lRow));

		// additional success probability
		pAgpdProductSkillFactor->m_lFactors[lLevel][AGPMPRODUCT_SKILL_FACTOR_ADD_PROB] =
							atoi(csExcelTxtLib.GetData(AGPMPRODUCT_SKILL_FACTOR_EXCEL_COLUMN_ADD_PROB, lRow));

		++lRead;
		}

	TRACE("Info : [%d] rows readed in AgpmProduct::StreamReadFactor()\n", lRead);
	return (lRead > 0);
	}


BOOL AgpmProduct::StreamReadGatherCharacterTAD(CHAR* pszFile, BOOL bDecryption)
{
	if (!pszFile || !strlen(pszFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;

	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
	{
		CHAR* pszTargetTID	= csExcelTxtLib.GetData(AGPMPRODUCT_GATHER_EXCEL_COLUMN_TARGET_TID, lRow);
		CHAR* pszClass		= csExcelTxtLib.GetData(AGPMPRODUCT_GATHER_EXCEL_COLUMN_CLASS, lRow);
		CHAR* pszSkillTID	= csExcelTxtLib.GetData(AGPMPRODUCT_GATHER_EXCEL_COLUMN_SKILL_TID_A, lRow);
		CHAR* pszProb		= csExcelTxtLib.GetData(AGPMPRODUCT_GATHER_EXCEL_COLUMN_PROB_A, lRow);
		CHAR* pszItem		= csExcelTxtLib.GetData(AGPMPRODUCT_GATHER_EXCEL_COLUMN_ITEM_SET_A, lRow);
		
		INT32 lTargetTID = atoi(pszTargetTID);

		AgpdCharacterTemplate* pAgpdCharacterTemplate = m_pAgpmCharacter->GetCharacterTemplate(lTargetTID);
		if (!pAgpdCharacterTemplate)
		{
			TRACE("!!! Warning : Can't get character template of TID[%d] in AgpmProduct::StreamReadGatherCharacterTAD()\n", lTargetTID);
			continue;
		}

		AgpdGatherCharacterTAD *pAgpdGatherCharacterTAD = GetGatherCharacterTAD(pAgpdCharacterTemplate);
		if (!pAgpdGatherCharacterTAD)
		{
			TRACE("!!! Warning : Can't get AD of TID[%d] in AgpmProduct::StreamReadGatherCharacterTAD()\n", lTargetTID);
			continue;
		}

		AgpdGatherElement pagpdGatherElement;
		pagpdGatherElement.m_lSkillTID		= (pszSkillTID) ? atoi(pszSkillTID) : 0;
		pagpdGatherElement.m_lProbability	= (pszProb)		? atoi(pszProb) : 0;
		pagpdGatherElement.m_lItemSetID		= (pszItem)		? atoi(pszItem) : 0;

		{
			CHAR* szBuffer = pszClass;
			CHAR* szToken = NULL;
			CHAR szSeps[] = ";";

			if(szBuffer && strlen(szBuffer))
			{
				szToken = strtok(szBuffer, szSeps);
				while(szToken)
				{
					INT32 lClass = (INT32)atoi(szToken);

					if(lClass > AUCHARCLASS_TYPE_NONE && lClass < AUCHARCLASS_TYPE_MAX)
					{
						pAgpdGatherCharacterTAD->m_ClassElement[lClass] = pagpdGatherElement;
					}
					
					szToken = strtok(NULL, szSeps);
				}
			}
		}
	}

	return TRUE;
}



