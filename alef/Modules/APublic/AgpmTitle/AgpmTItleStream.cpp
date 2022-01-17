#include "AgpmTitle.h"
#include "AuExcelTxtLib.h"
#include "ApUtil.h"
#include "AuExcelBinaryLib.h"

BOOL AgpmTitle::StreamReadTitleDataTemplate(CHAR* pszFile, BOOL bDecryption)
{
	using namespace profile;
	using namespace AuExcel;

	Timer t;
	#define CHECK()	t.Log( __FILE__ , __LINE__ );

	if (!pszFile || !strlen(pszFile))
        return FALSE;

	AuExcelLib * pExcel = LoadExcelFile( pszFile , bDecryption );
	AuAutoPtr< AuExcelLib >	ptrExcel = pExcel;

	if( pExcel == NULL)
	{
		TRACE("AgpmTitle::StreamReadTitleDataTemplate() Error (1) !!!\n");
		return FALSE;
	}

	AgpdTitleTemplate*	pAgpdTitleTemplate	= NULL;
#ifdef _DEBUG
	CHECK();
#endif
	for(INT32 lRow = 1; lRow < pExcel->GetRow();lRow++)
	{
		pAgpdTitleTemplate = new AgpdTitleTemplate;

		pAgpdTitleTemplate->m_nTitleTid = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLETID, lRow);

		CHAR* pszName = pExcel->GetData(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLENAME, lRow);
		strncpy(pAgpdTitleTemplate->m_szTitleName, pszName, AGPDTITLE_MAX_TITLE_NAME_LENTH);

		CHAR* pszTitleClass = pExcel->GetData(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_CLASS, lRow);
		strncpy(pAgpdTitleTemplate->m_szTitleClass, pszTitleClass, AGPDTITLE_MAX_TITLE_CLASS_LENTH);

		pAgpdTitleTemplate->m_nStringTid = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_STRING_TID, lRow);
		pAgpdTitleTemplate->m_bDuration = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_DURATION, lRow);
		pAgpdTitleTemplate->m_bTradePossibility = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_TRADE_POSSIBILITY, lRow);

		AgpdTitleRequireCondition stAgpdTitleRequireCondition;

		stAgpdTitleRequireCondition.m_nRequireLevel = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_LEVEL, lRow);
		stAgpdTitleRequireCondition.m_nRequireClass = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_CLASS, lRow);
		stAgpdTitleRequireCondition.m_nRequireGuildMaster = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_GUILDMASTER, lRow);
		stAgpdTitleRequireCondition.m_nRequireClassArchlord = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_CLASS_ARCHLORD, lRow);
		stAgpdTitleRequireCondition.m_nRequireCon = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_CON, lRow);
		stAgpdTitleRequireCondition.m_nRequireStr = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_STR, lRow);
		stAgpdTitleRequireCondition.m_nRequireInt = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_INT, lRow);
		stAgpdTitleRequireCondition.m_nRequireDex = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_DEX, lRow);
		stAgpdTitleRequireCondition.m_nRequireWis = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_WIS, lRow);
		stAgpdTitleRequireCondition.m_nRequireCha = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REQUIRE_CHA, lRow);
		stAgpdTitleRequireCondition.m_nRequireGheld = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_COST_GHELD, lRow);
		stAgpdTitleRequireCondition.m_nRequireCash = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_COST_CASH, lRow);

		for(int i = 0; i < AGPDTITLE_MAX_TITLE_REQUIRE_ITEM; i++)
		{
			stAgpdTitleRequireCondition.m_nRequireItemTid[i][0] = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_COST_ITEM1+3*i, lRow);
			stAgpdTitleRequireCondition.m_nRequireItemTid[i][1] = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_COST_ITEM1_COUNT+3*i, lRow);
			stAgpdTitleRequireCondition.m_nRequireItemTid[i][2] = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_COST_ITEM1_TYPE+3*i, lRow);
		}

		for(int i = 0; i < AGPDTITLE_MAX_TITLE_REQUIRE_TITLE; i++)
		{
			stAgpdTitleRequireCondition.m_nRequireTitleTid[i] = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_FIRST_TITLE1+i, lRow);
		}

		pAgpdTitleTemplate->m_stRequireCondition = stAgpdTitleRequireCondition;

		AgpdTitleReward stAgpdTitleReward;

		stAgpdTitleReward.m_nRewardMoney = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REWARD_MONEY, lRow);

		for(int i = 0; i < AGPDTITLE_MAX_TITLE_REWARD_ITEM; i++)
		{
			stAgpdTitleReward.m_nRewardItemTid[i] = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_REWARD_ITEM1+i, lRow);
		}

		pAgpdTitleTemplate->m_stReward = stAgpdTitleReward;

		for(int i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
		{
			AgpdTitleCheck stAgpdTitleCheck;

			stAgpdTitleCheck.m_nTitleCheckType = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_CHECK_TYPE1 + 3*i, lRow);
			stAgpdTitleCheck.m_nTitleCheckSet = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_CHECK_SET1 + 3*i, lRow);
			stAgpdTitleCheck.m_nTitleCheckValue = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_CHECK_VALUE1 + 3*i, lRow);

			pAgpdTitleTemplate->m_vtTitleCheck.push_back(stAgpdTitleCheck);
		}

		for(int i = 0; i < AGPDTITLE_MAX_TITLE_SKILLEFFECT; i++)
		{
			AgpdTitleSkillEffect stAgpdTitleSkillEffect;

			stAgpdTitleSkillEffect.m_nSkillTid = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_SKILL_TID1 + i, lRow);

			pAgpdTitleTemplate->m_vtTitleSkillEffect.push_back(stAgpdTitleSkillEffect);
		}

		for(int i = 0; i < AGPDTITLE_MAX_TITLE_EFFECT; i++)
		{
			AgpdTitleEffect stAgpdTitleEffect;

			stAgpdTitleEffect.m_nEffectType = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_EFFECT_1_TYPE + 4*i, lRow); 
			stAgpdTitleEffect.m_nEffectSet = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_EFFECT_1_SET + 4*i, lRow);
			stAgpdTitleEffect.m_nEffectValue1 = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_EFFECT_1_VALUE1 + 4*i, lRow);
			stAgpdTitleEffect.m_nEffectValue2 = pExcel->GetDataToInt(AGPMTITLE_TITLE_EXCEL_COLUMN_TITLE_EFFECT_1_VALUE2 + 4*i, lRow);
			
			pAgpdTitleTemplate->m_vtTitleEffect.push_back(stAgpdTitleEffect);
		}

		m_stAgpaTitleTemplate.Add(pAgpdTitleTemplate);
	}

	return TRUE;
}

BOOL AgpmTitle::StreamReadTitleStringTemplate(CHAR* pszFile, BOOL bDecryption)
{
	using namespace profile;
	using namespace AuExcel;

	Timer t;
	#define CHECK()	t.Log( __FILE__ , __LINE__ );

	if (!pszFile || !strlen(pszFile))
        return FALSE;

	AuExcelLib * pExcel = LoadExcelFile( pszFile , bDecryption );
	AuAutoPtr< AuExcelLib >	ptrExcel = pExcel;

	if( pExcel == NULL)
	{
		TRACE("AgpmTitle::StreamReadTitleStringTemplate() Error (1) !!!\n");
		return FALSE;
	}
#ifdef _DEBUG
	CHECK();
#endif
	for(INT32 lRow = 1; lRow < pExcel->GetRow();lRow++)
	{
		AgpdTitleStringTemplate TitleStringTemplate;

		TitleStringTemplate.m_nTitleTid = pExcel->GetDataToInt(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_TID, lRow);
		TitleStringTemplate.m_nFirstCategory = pExcel->GetDataToInt(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_FIRST_CATEGORY, lRow);
		TitleStringTemplate.m_nSecondCategory = pExcel->GetDataToInt(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_SECOND_CATEGORY, lRow);

		CHAR* pszTitleName = pExcel->GetData(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_NAME_KOREA, lRow);
		strncpy(TitleStringTemplate.m_szTitleName, pszTitleName, AGPDTITLE_MAX_TITLE_NAME_LENTH);

		CHAR* pszTitleNecessaryConditionDesc = pExcel->GetData(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_NECESSARY_CONDITION_DESC_KOREA, lRow);
		strncpy(TitleStringTemplate.m_szTitleNecessaryConditionDesc, pszTitleNecessaryConditionDesc, AGPDTITLE_MAX_TITLE_NECESSARY_CONDITION_DESC);

		CHAR* pszTitleEffectDesc = pExcel->GetData(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_EFFECT_DESC_KOREA, lRow);
		strncpy(TitleStringTemplate.m_szTitleEffectDesc, pszTitleEffectDesc, AGPDTITLE_MAX_TITLE_EFFECT_DESC);

		m_stAgpaTitleStringTemplate.AddTitleStringTemplate(TitleStringTemplate);

		
		AgpdTitleCategory TitleCategory;
		TitleCategory.m_nFirstCategory = pExcel->GetDataToInt(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_FIRST_CATEGORY, lRow);

		CHAR* pszTitleClass = pExcel->GetData(AGPMTITLE_TITLE_STRING_EXCEL_COLUMN_TITLE_CLASS_KOREA, lRow);
		strncpy(TitleCategory.m_szTitleClass, pszTitleClass, AGPDTITLE_MAX_TITLE_CLASS_LENTH);

		if(!m_stAgpaTitleCategory.CheckForTitleCategoryID(TitleCategory.m_nFirstCategory))
			m_stAgpaTitleCategory.AddTitleCategory(TitleCategory);

	}

	return TRUE;
}