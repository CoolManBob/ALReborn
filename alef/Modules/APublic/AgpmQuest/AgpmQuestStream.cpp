#include "AgpmQuest.h"
#include "AgpmQuestStringTable.h"

const CHAR OLD_CHAR = ';';
const CHAR NEW_CHAR = ',';

void AgpmQuest::CharReplace(CHAR* szBuffer, CHAR cOld, CHAR cNew)
{
	if ( g_eServiceArea != AP_SERVICE_AREA_JAPAN )
	{
		CHAR* pFind;
		while(pFind = strchr(szBuffer, cOld))
		{
			*pFind = cNew;
		}
	}
}

BOOL AgpmQuest::StreamReadTemplate(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	CHAR*	pData = NULL;
	INT32	lTemp;

	int	AGPDQUEST_FIELD_TID								= 0;
	int	AGPDQUEST_FIELD_SCENARIO_NAME					= 0;
	int	AGPDQUEST_FIELD_QUEST_NAME						= 0;
	int	AGPDQUEST_FIELD_START_AREA						= 0;
	int	AGPDQUEST_FIELD_CATEGORY						= 0;
	int	AGPDQUEST_FIELD_LOCATION_TID					= 0;
	int	AGPDQUEST_FIELD_START_LEVEL						= 0;
	int	AGPDQUEST_FIELD_START_MAX_LEVEL					= 0;	//문엘프퀘스트 관련 수정추가 2005.6.2	AGSDQUEST_EXPAND_BLOCK
	int	AGPDQUEST_FIELD_START_CLASS						= 0;
	int	AGPDQUEST_FIELD_START_GENDER					= 0;
	int	AGPDQUEST_FIELD_START_RACE						= 0;
	int	AGPDQUEST_FIELD_START_PREV_QUEST_TID			= 0;
	int	AGPDQUEST_FIELD_START_PREV_QUEST_NAME			= 0;
	int	AGPDQUEST_FIELD_GIVE_NPC						= 0;
	int	AGPDQUEST_FIELD_CONFIRM_NPC						= 0;
	int	AGPDQUEST_FIELD_QUEST_ITEM_TID					= 0;
	int	AGPDQUEST_FIELD_QUEST_ITEM_NAME					= 0;
	int	AGPDQUEST_FIELD_QUEST_ITEM_COUNT				= 0;
	int	AGPDQUEST_FIELD_ACCEPT_TEXT						= 0;
	int	AGPDQUEST_FIELD_INCOMPLETE_TEXT					= 0;
	int	AGPDQUEST_FIELD_COMPLETE_TEXT					= 0;
	int	AGPDQUEST_FIELD_GOAL_LEVEL						= 0;
	int	AGPDQUEST_FIELD_GOAL_MONEY						= 0;
	int	AGPDQUEST_FIELD_GOAL_ITEM_TID					= 0;
	int	AGPDQUEST_FIELD_GOAL_ITEM_NAME					= 0;
	int	AGPDQUEST_FIELD_GOAL_ITEM_COUNT					= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_TID				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_NAME				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_COUNT				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_NAME			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_DROP_RATE	= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_COUNT		= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_TID				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_NAME				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_COUNT				= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_NAME			= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_DROP_RATE	= 0;
	int	AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_COUNT		= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT1_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT1_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT1_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT1_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT2_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT2_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT2_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT2_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT3_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT3_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT3_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT3_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT4_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT4_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT4_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT4_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT5_OBJECT_ID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT5_ITEM_TID			= 0;
	int	AGPDQUEST_FIELD_CHECKPOINT5_ITEM_NAME			= 0;
	int	AGPDQUETS_FIELD_CHECKPOINT5_ITEM_COUNT			= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT1					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR1					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT2					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR2					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT3					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR3					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT4					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR4					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_TEXT5					= 0;
	int	AGPDQUEST_FIELD_SUMMARY_FACTOR5					= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM1_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM2_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM3_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_UPGRADE			= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_SOCKET				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_TID				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_NAME				= 0;
	int	AGPDQUEST_FIELD_RESULT_ITEM4_COUNT				= 0;
	int	AGPDQUEST_FIELD_RESULT_MONEY					= 0;
	int	AGPDQUEST_FIELD_RESULT_EXP						= 0;
	int	AGPDQUEST_FIELD_RESULT_NEXT_QUEST_TID			= 0;
	int	AGPDQUEST_FIELD_RESULT_NEXT_QUEST_NAME			= 0;
	int	AGPDQUEST_FIELD_QUEST_TYPE						= 0;
	{
		// 각 필드의 옵셀을 2번째 Row 에서 찾아낸다.

		//FILE	* pFile = fopen( "quest.txt" , "wt" );

		const int	nCategoryRow = 2;
		for (INT32 lCol = 0 ; lCol < csExcelTxtLib.GetColumn(); lCol++ )
		{
			char * pDataString = csExcelTxtLib.GetData( lCol , nCategoryRow );

			if( NULL == pDataString ) continue;

			//fprintf( pFile , "%s\n" , pDataString );

			if		( !strncmp( pDataString , "Tid"								, 256 ) ) AGPDQUEST_FIELD_TID								= lCol;
			else if	( !strncmp( pDataString , "ScenarioName"					, 256 ) ) AGPDQUEST_FIELD_SCENARIO_NAME					= lCol;
			else if	( !strncmp( pDataString , "QuestName"						, 256 ) ) AGPDQUEST_FIELD_QUEST_NAME						= lCol;
			else if	( !strncmp( pDataString , "Area"							, 256 ) ) AGPDQUEST_FIELD_START_AREA						= lCol;
			else if	( !strncmp( pDataString , "Category"						, 256 ) ) AGPDQUEST_FIELD_CATEGORY						= lCol;
			else if	( !strncmp( pDataString , "Location"						, 256 ) ) AGPDQUEST_FIELD_LOCATION_TID					= lCol;
			else if	( !strncmp( pDataString , "Need_Level_Min"					, 256 ) ) AGPDQUEST_FIELD_START_LEVEL						= lCol;
			else if	( !strncmp( pDataString , "Need_Level_Max"					, 256 ) ) AGPDQUEST_FIELD_START_MAX_LEVEL					= lCol;
			else if	( !strncmp( pDataString , "Need_Class"						, 256 ) ) AGPDQUEST_FIELD_START_CLASS						= lCol;
			else if	( !strncmp( pDataString , "Need_Gender"						, 256 ) ) AGPDQUEST_FIELD_START_GENDER					= lCol;
			else if	( !strncmp( pDataString , "Need_Race"						, 256 ) ) AGPDQUEST_FIELD_START_RACE						= lCol;
			else if	( !strncmp( pDataString , "Need_PreviousQuestTid"			, 256 ) ) AGPDQUEST_FIELD_START_PREV_QUEST_TID			= lCol;
			else if	( !strncmp( pDataString , "Need_PreviousQuestName"			, 256 ) ) AGPDQUEST_FIELD_START_PREV_QUEST_NAME			= lCol;
			else if	( !strncmp( pDataString , "GiveNpcName"						, 256 ) ) AGPDQUEST_FIELD_GIVE_NPC						= lCol;
			else if	( !strncmp( pDataString , "ConfirmNpcName"					, 256 ) ) AGPDQUEST_FIELD_CONFIRM_NPC						= lCol;
			else if	( !strncmp( pDataString , "Quest_TransItemTid"				, 256 ) ) AGPDQUEST_FIELD_QUEST_ITEM_TID					= lCol;
			else if	( !strncmp( pDataString , "Quest_TransItemName"				, 256 ) ) AGPDQUEST_FIELD_QUEST_ITEM_NAME					= lCol;
			else if	( !strncmp( pDataString , "Quest_TransItemCount"			, 256 ) ) AGPDQUEST_FIELD_QUEST_ITEM_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Dialogue_Start"					, 256 ) ) AGPDQUEST_FIELD_ACCEPT_TEXT						= lCol;
			else if	( !strncmp( pDataString , "Dialogue_Progress"				, 256 ) ) AGPDQUEST_FIELD_INCOMPLETE_TEXT					= lCol;
			else if	( !strncmp( pDataString , "Dialogue_Finish"					, 256 ) ) AGPDQUEST_FIELD_COMPLETE_TEXT					= lCol;
			else if	( !strncmp( pDataString , "Complete_Level"					, 256 ) ) AGPDQUEST_FIELD_GOAL_LEVEL						= lCol;
			else if	( !strncmp( pDataString , "Complete_Money"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONEY						= lCol;
			else if	( !strncmp( pDataString , "Complete_ItemTid"				, 256 ) ) AGPDQUEST_FIELD_GOAL_ITEM_TID					= lCol;
			else if	( !strncmp( pDataString , "Complete_ItemName"				, 256 ) ) AGPDQUEST_FIELD_GOAL_ITEM_NAME					= lCol;
			else if	( !strncmp( pDataString , "Complete_ItemCount"				, 256 ) ) AGPDQUEST_FIELD_GOAL_ITEM_COUNT					= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterA_Tid"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_TID				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterA_Name"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_NAME				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterA_Count"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_COUNT				= lCol;
			else if	( !strncmp( pDataString , "DropItemA_TID"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "DropItemA_Name"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "DropItemA_Property"				, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_DROP_RATE	= lCol;
			else if	( !strncmp( pDataString , "DropItemA_Count"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_COUNT		= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterB_Tid"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_TID				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterB_Name"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_NAME				= lCol;
			else if	( !strncmp( pDataString , "Complete_MonsterB_Count"			, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_COUNT				= lCol;
			else if	( !strncmp( pDataString , "DropItemB_TID"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "DropItemB_Name"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "DropItemB_Property"				, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_DROP_RATE	= lCol;
			else if	( !strncmp( pDataString , "DropItemB_Count"					, 256 ) ) AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_COUNT		= lCol;
			else if	( !strncmp( pDataString , "ObjectA_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT1_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemA_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT1_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemA_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT1_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemA_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT1_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectB_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT2_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemB_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT2_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemB_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT2_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemB_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT2_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectC_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT3_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemC_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT3_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemC_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT3_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemC_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT3_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectD_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT4_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemD_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT4_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemD_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT4_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemD_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT4_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "ObjectE_ID"						, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT5_OBJECT_ID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemE_TID"					, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT5_ITEM_TID			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemE_Name"				, 256 ) ) AGPDQUEST_FIELD_CHECKPOINT5_ITEM_NAME			= lCol;
			else if	( !strncmp( pDataString , "ObjectItemE_Count"				, 256 ) ) AGPDQUETS_FIELD_CHECKPOINT5_ITEM_COUNT			= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary1"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT1					= lCol;
			else if	( !strncmp( pDataString , "CountFactor1"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR1					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary2"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT2					= lCol;
			else if	( !strncmp( pDataString , "CountFactor2"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR2					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary3"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT3					= lCol;
			else if	( !strncmp( pDataString , "CountFactor3"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR3					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary4"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT4					= lCol;
			else if	( !strncmp( pDataString , "CountFactor4"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR4					= lCol;
			else if	( !strncmp( pDataString , "Complete_Summary5"				, 256 ) ) AGPDQUEST_FIELD_SUMMARY_TEXT5					= lCol;
			else if	( !strncmp( pDataString , "CountFactor5"					, 256 ) ) AGPDQUEST_FIELD_SUMMARY_FACTOR5					= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemA_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM1_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemB_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM2_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemC_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM3_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Upgrade_Intensify"	, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_UPGRADE			= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Upgrade_Slot"			, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_SOCKET				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Tid"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_TID				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Name"					, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_NAME				= lCol;
			else if	( !strncmp( pDataString , "Gift_ItemD_Count"				, 256 ) ) AGPDQUEST_FIELD_RESULT_ITEM4_COUNT				= lCol;
			else if	( !strncmp( pDataString , "Gift_Money"						, 256 ) ) AGPDQUEST_FIELD_RESULT_MONEY					= lCol;
			else if	( !strncmp( pDataString , "Gift_Exp"						, 256 ) ) AGPDQUEST_FIELD_RESULT_EXP						= lCol;
			else if	( !strncmp( pDataString , "Gift_NextQuest_TID"				, 256 ) ) AGPDQUEST_FIELD_RESULT_NEXT_QUEST_TID			= lCol;
			else if	( !strncmp( pDataString , "Gift_NextQuest_Name"				, 256 ) ) AGPDQUEST_FIELD_RESULT_NEXT_QUEST_NAME			= lCol;
			else if	( !strncmp( pDataString , "Self_Quest"						, 256 ) ) AGPDQUEST_FIELD_QUEST_TYPE						= lCol;
		}
		// fclose( pFile );
	}

	for (INT32 lRow = 3; lRow < csExcelTxtLib.GetRow(); ++lRow)
	{
		AgpdQuestTemplate *pcsAgpdQuest = CreateTemplate();

		// AgpdQuestInfo
		pcsAgpdQuest->m_lID = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_TID, lRow);
		pcsAgpdQuest->m_QuestInfo.eQuestCategory = (Enum_AGPMQUEST_CATEGORY)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CATEGORY, lRow);
		pcsAgpdQuest->m_QuestInfo.lLocation = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_LOCATION_TID, lRow);
		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_QUEST_NAME, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_QUEST_NAME, lRow)) < AGPDQUEST_MAX_NAME);
			strncpy(pcsAgpdQuest->m_QuestInfo.szName, csExcelTxtLib.GetData(AGPDQUEST_FIELD_QUEST_NAME, lRow), AGPDQUEST_MAX_NAME);
			CharReplace(pcsAgpdQuest->m_QuestInfo.szName, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_SCENARIO_NAME, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_SCENARIO_NAME, lRow)) < AGPDQUEST_MAX_SCENARIO);
			strncpy(pcsAgpdQuest->m_QuestInfo.szScenarioName, csExcelTxtLib.GetData(AGPDQUEST_FIELD_SCENARIO_NAME, lRow), AGPDQUEST_MAX_SCENARIO);
			CharReplace(pcsAgpdQuest->m_QuestInfo.szScenarioName, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_START_AREA, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_START_AREA, lRow)) < AGPDQUEST_MAX_AREA);
			strncpy(pcsAgpdQuest->m_QuestInfo.szAreaInfo, csExcelTxtLib.GetData(AGPDQUEST_FIELD_START_AREA, lRow), AGPDQUEST_MAX_AREA);
			CharReplace(pcsAgpdQuest->m_QuestInfo.szAreaInfo, OLD_CHAR, NEW_CHAR);
		}

		// AgpdStartCondition
		pcsAgpdQuest->m_StartCondition.lClass	= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_START_CLASS, lRow);
		pcsAgpdQuest->m_StartCondition.lGender	= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_START_GENDER, lRow);
		pcsAgpdQuest->m_StartCondition.lLevel	= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_START_LEVEL, lRow);
		pcsAgpdQuest->m_StartCondition.lMaxLevel= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_START_MAX_LEVEL, lRow);	//문엘프퀘스트 관련 수정추가 2005.6.2 AGSDQUEST_EXPAND_BLOCK
		pcsAgpdQuest->m_StartCondition.lRace	= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_START_RACE, lRow);
		pcsAgpdQuest->m_StartCondition.Quest.lBackupTID = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_START_PREV_QUEST_TID, lRow);

		// AgpdQuestPreStart
		pcsAgpdQuest->m_PreStart.Item.pItemTemplate = m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_QUEST_ITEM_TID, lRow));
		pcsAgpdQuest->m_PreStart.Item.lCount = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_QUEST_ITEM_COUNT, lRow);

		// AgpdQuestDialogText
		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_ACCEPT_TEXT, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_ACCEPT_TEXT, lRow)) < AGPDQUEST_MAX_DIALOG_TEXT);
			strncpy_s(pcsAgpdQuest->m_DialogText.szAcceptText, AGPDQUEST_MAX_DIALOG_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_ACCEPT_TEXT, lRow), _TRUNCATE);
			CharReplace(pcsAgpdQuest->m_DialogText.szAcceptText, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_INCOMPLETE_TEXT, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_INCOMPLETE_TEXT, lRow)) < AGPDQUEST_MAX_DIALOG_TEXT);
			strncpy_s(pcsAgpdQuest->m_DialogText.szIncompleteText, AGPDQUEST_MAX_DIALOG_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_INCOMPLETE_TEXT, lRow), _TRUNCATE);
			CharReplace(pcsAgpdQuest->m_DialogText.szIncompleteText, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_COMPLETE_TEXT, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_COMPLETE_TEXT, lRow)) < AGPDQUEST_MAX_DIALOG_TEXT);
			strncpy_s(pcsAgpdQuest->m_DialogText.szCompleteText, AGPDQUEST_MAX_DIALOG_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_COMPLETE_TEXT, lRow), _TRUNCATE);
			CharReplace(pcsAgpdQuest->m_DialogText.szCompleteText, OLD_CHAR, NEW_CHAR);
		}

		// AgpdQuestSummaryText
		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT1, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT1, lRow)) < AGPDQUEST_MAX_SUMMARY_TEXT);
			strncpy_s(pcsAgpdQuest->m_SummaryText.SummaryText[0].szSummaryText, AGPDQUEST_MAX_SUMMARY_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT1, lRow), _TRUNCATE);
			pcsAgpdQuest->m_SummaryText.SummaryText[0].eFactor = (Enum_AGPDQUEST_SUMMARY_FACTOR)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_SUMMARY_FACTOR1, lRow);
			CharReplace(pcsAgpdQuest->m_SummaryText.SummaryText[0].szSummaryText, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT2, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT2, lRow)) < AGPDQUEST_MAX_SUMMARY_TEXT);
			strncpy_s(pcsAgpdQuest->m_SummaryText.SummaryText[1].szSummaryText, AGPDQUEST_MAX_SUMMARY_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT2, lRow), _TRUNCATE);
			pcsAgpdQuest->m_SummaryText.SummaryText[1].eFactor = (Enum_AGPDQUEST_SUMMARY_FACTOR)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_SUMMARY_FACTOR2, lRow);
			CharReplace(pcsAgpdQuest->m_SummaryText.SummaryText[1].szSummaryText, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT3, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT3, lRow)) < AGPDQUEST_MAX_SUMMARY_TEXT);
			strncpy_s(pcsAgpdQuest->m_SummaryText.SummaryText[2].szSummaryText, AGPDQUEST_MAX_SUMMARY_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT3, lRow), _TRUNCATE);
			pcsAgpdQuest->m_SummaryText.SummaryText[2].eFactor = (Enum_AGPDQUEST_SUMMARY_FACTOR)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_SUMMARY_FACTOR3, lRow);
			CharReplace(pcsAgpdQuest->m_SummaryText.SummaryText[2].szSummaryText, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT4, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT4, lRow)) < AGPDQUEST_MAX_SUMMARY_TEXT);
			strncpy_s(pcsAgpdQuest->m_SummaryText.SummaryText[3].szSummaryText, AGPDQUEST_MAX_SUMMARY_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT4, lRow), _TRUNCATE);
			pcsAgpdQuest->m_SummaryText.SummaryText[3].eFactor = (Enum_AGPDQUEST_SUMMARY_FACTOR)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_SUMMARY_FACTOR4, lRow);
			CharReplace(pcsAgpdQuest->m_SummaryText.SummaryText[3].szSummaryText, OLD_CHAR, NEW_CHAR);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT5, lRow))
		{
			ASSERT(strlen(csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT5, lRow)) < AGPDQUEST_MAX_SUMMARY_TEXT);
			strncpy_s(pcsAgpdQuest->m_SummaryText.SummaryText[4].szSummaryText, AGPDQUEST_MAX_SUMMARY_TEXT, csExcelTxtLib.GetData(AGPDQUEST_FIELD_SUMMARY_TEXT5, lRow), _TRUNCATE);
			pcsAgpdQuest->m_SummaryText.SummaryText[4].eFactor = (Enum_AGPDQUEST_SUMMARY_FACTOR)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_SUMMARY_FACTOR5, lRow);
			CharReplace(pcsAgpdQuest->m_SummaryText.SummaryText[4].szSummaryText, OLD_CHAR, NEW_CHAR);
		}

		// AgpdCompleteCondition
		pcsAgpdQuest->m_CompleteCondition.Item.pItemTemplate = m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_ITEM_TID, lRow));
		pcsAgpdQuest->m_CompleteCondition.Item.lCount = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_ITEM_COUNT, lRow);
		pcsAgpdQuest->m_CompleteCondition.lMoney = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONEY, lRow);
		pcsAgpdQuest->m_CompleteCondition.lLevel = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_LEVEL, lRow);

		pcsAgpdQuest->m_CompleteCondition.Monster1.pMonsterTemplate		= m_pcsAgpmCharacter->GetCharacterTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER1_TID, lRow));
		pcsAgpdQuest->m_CompleteCondition.Monster1.lCount				= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER1_COUNT, lRow);
		pcsAgpdQuest->m_CompleteCondition.Monster1.Item.pItemTemplate	= m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_TID, lRow));
		pcsAgpdQuest->m_CompleteCondition.Monster1.Item.lCount			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_COUNT, lRow);
		pcsAgpdQuest->m_CompleteCondition.Monster1.lDropRate			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER1_ITEM_DROP_RATE, lRow);

		pcsAgpdQuest->m_CompleteCondition.Monster2.pMonsterTemplate		= m_pcsAgpmCharacter->GetCharacterTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER2_TID, lRow));
		pcsAgpdQuest->m_CompleteCondition.Monster2.lCount				= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER2_COUNT, lRow);
		pcsAgpdQuest->m_CompleteCondition.Monster2.Item.pItemTemplate	= m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_TID, lRow));
		pcsAgpdQuest->m_CompleteCondition.Monster2.Item.lCount			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_COUNT, lRow);
		pcsAgpdQuest->m_CompleteCondition.Monster2.lDropRate			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_GOAL_MONSTER2_ITEM_DROP_RATE, lRow);

		// AgpdCheckPoint
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[0].lObjectID			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT1_OBJECT_ID, lRow);
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[0].pItemTemplate		= m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT1_ITEM_TID, lRow));
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[0].lItemCount			= csExcelTxtLib.GetDataToInt(AGPDQUETS_FIELD_CHECKPOINT1_ITEM_COUNT, lRow);

		pcsAgpdQuest->m_CheckPoint.CheckPointItem[1].lObjectID			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT2_OBJECT_ID, lRow);
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[1].pItemTemplate		= m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT2_ITEM_TID, lRow));
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[1].lItemCount			= csExcelTxtLib.GetDataToInt(AGPDQUETS_FIELD_CHECKPOINT2_ITEM_COUNT, lRow);

		pcsAgpdQuest->m_CheckPoint.CheckPointItem[2].lObjectID			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT3_OBJECT_ID, lRow);
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[2].pItemTemplate		= m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT3_ITEM_TID, lRow));
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[2].lItemCount			= csExcelTxtLib.GetDataToInt(AGPDQUETS_FIELD_CHECKPOINT3_ITEM_COUNT, lRow);

		pcsAgpdQuest->m_CheckPoint.CheckPointItem[3].lObjectID			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT4_OBJECT_ID, lRow);
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[3].pItemTemplate		= m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT4_ITEM_TID, lRow));
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[3].lItemCount			= csExcelTxtLib.GetDataToInt(AGPDQUETS_FIELD_CHECKPOINT4_ITEM_COUNT, lRow);

		pcsAgpdQuest->m_CheckPoint.CheckPointItem[4].lObjectID			= csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT5_OBJECT_ID, lRow);
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[4].pItemTemplate		= m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_CHECKPOINT5_ITEM_TID, lRow));
		pcsAgpdQuest->m_CheckPoint.CheckPointItem[4].lItemCount			= csExcelTxtLib.GetDataToInt(AGPDQUETS_FIELD_CHECKPOINT5_ITEM_COUNT, lRow);

		// AgpdResult
		pcsAgpdQuest->m_Result.lMoney = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_MONEY, lRow);
		pcsAgpdQuest->m_Result.lExp = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_EXP, lRow);
		pcsAgpdQuest->m_Result.Quest.lBackupTID = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_NEXT_QUEST_TID, lRow);
		/*
		pcsAgpdQuest->m_Result.ProductSkill.pSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_SKILL_TID, lRow));
		pcsAgpdQuest->m_Result.ProductSkill.lExp = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_SKILL_EXP, lRow);
		*/

		pcsAgpdQuest->m_Result.Item1.lUpgrade = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM1_UPGRADE, lRow);
		pcsAgpdQuest->m_Result.Item1.lSocket = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM1_SOCKET, lRow);
		pcsAgpdQuest->m_Result.Item1.pItemTemplate = m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM1_TID, lRow));
		pcsAgpdQuest->m_Result.Item1.lCount = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM1_COUNT, lRow);
		if (pcsAgpdQuest->m_Result.Item1.pItemTemplate)
			sscanf(csExcelTxtLib.GetData(AGPDQUEST_FIELD_RESULT_ITEM1_TID, lRow), "%d;%d", &lTemp, (INT32 *) &pcsAgpdQuest->m_Result.Item1.m_eBoundType);

		pcsAgpdQuest->m_Result.Item2.lUpgrade = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM2_UPGRADE, lRow);
		pcsAgpdQuest->m_Result.Item2.lSocket = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM2_SOCKET, lRow);
		pcsAgpdQuest->m_Result.Item2.pItemTemplate = m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM2_TID, lRow));
		pcsAgpdQuest->m_Result.Item2.lCount = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM2_COUNT, lRow);
		if (pcsAgpdQuest->m_Result.Item2.pItemTemplate)
			sscanf(csExcelTxtLib.GetData(AGPDQUEST_FIELD_RESULT_ITEM2_TID, lRow), "%d;%d", &lTemp, (INT32 *) &pcsAgpdQuest->m_Result.Item1.m_eBoundType);

		pcsAgpdQuest->m_Result.Item3.lUpgrade = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM3_UPGRADE, lRow);
		pcsAgpdQuest->m_Result.Item3.lSocket = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM3_SOCKET, lRow);
		pcsAgpdQuest->m_Result.Item3.pItemTemplate = m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM3_TID, lRow));
		pcsAgpdQuest->m_Result.Item3.lCount = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM3_COUNT, lRow);
		if (pcsAgpdQuest->m_Result.Item3.pItemTemplate)
			sscanf(csExcelTxtLib.GetData(AGPDQUEST_FIELD_RESULT_ITEM3_TID, lRow), "%d;%d", &lTemp, (INT32 *) &pcsAgpdQuest->m_Result.Item1.m_eBoundType);

		pcsAgpdQuest->m_Result.Item4.lUpgrade = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM4_UPGRADE, lRow);
		pcsAgpdQuest->m_Result.Item4.lSocket = (BOOL)csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM4_SOCKET, lRow);
		pcsAgpdQuest->m_Result.Item4.pItemTemplate = m_pcsAgpmItem->GetItemTemplate(csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM4_TID, lRow));
		pcsAgpdQuest->m_Result.Item4.lCount = csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_RESULT_ITEM4_COUNT, lRow);
		if (pcsAgpdQuest->m_Result.Item4.pItemTemplate)
			sscanf(csExcelTxtLib.GetData(AGPDQUEST_FIELD_RESULT_ITEM4_TID, lRow), "%d;%d", &lTemp, (INT32 *) &pcsAgpdQuest->m_Result.Item1.m_eBoundType);

		pcsAgpdQuest->m_eQuestType = (Enum_AGPDQUEST_TYPE) csExcelTxtLib.GetDataToInt(AGPDQUEST_FIELD_QUEST_TYPE, lRow);

		m_csQuestTemplate.Add(pcsAgpdQuest);
	}

	csExcelTxtLib.CloseFile();

	PrevNextQuestDataSetting();

	return TRUE;
}

BOOL AgpmQuest::StreamReadGroup(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT32 lTID;
	AgpdQuestGroup	*pcsAgpdQuestGroup;
	CHAR szListBuffer[LIST_BUFFER_SIZE];

	for (INT32 lRow = 0; lRow < csExcelTxtLib.GetRow(); ++lRow)
	{
		lTID = csExcelTxtLib.GetDataToInt(AGPDQUEST_GROUP_FIELD_TID, lRow);
		pcsAgpdQuestGroup = AddGroup(lTID);

		if (!pcsAgpdQuestGroup)
			continue;

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_NPC_NAME, lRow))
			strncpy(pcsAgpdQuestGroup->m_szName, csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_NPC_NAME, lRow), AGPDCHARACTER_MAX_ID_LENGTH);

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_GIVE, lRow))
		{
			ZeroMemory(szListBuffer, LIST_BUFFER_SIZE);
			strncpy(szListBuffer, csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_GIVE, lRow), LIST_BUFFER_SIZE);
			StreamReadQuestList(&pcsAgpdQuestGroup->m_GrantQuest, szListBuffer);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_CONFIRM, lRow))
		{
			ZeroMemory(szListBuffer, LIST_BUFFER_SIZE);
			strncpy(szListBuffer, csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_CONFIRM, lRow), LIST_BUFFER_SIZE);
			StreamReadQuestList(&pcsAgpdQuestGroup->m_EstimateQuest, szListBuffer);
		}

		if (NULL != csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_CHECKPOINT, lRow))
		{
			ZeroMemory(szListBuffer, LIST_BUFFER_SIZE);
			strncpy(szListBuffer, csExcelTxtLib.GetData(AGPDQUEST_GROUP_FIELD_CHECKPOINT, lRow), LIST_BUFFER_SIZE);
			StreamReadCheckPoint(&pcsAgpdQuestGroup->m_CheckPoint, szListBuffer);
		}
	}

	csExcelTxtLib.CloseFile();
	return TRUE;
}

BOOL AgpmQuest::PrevNextQuestDataSetting()
{
	INT32 lIndex = 0;
	AgpdQuestTemplate** pQuestTemplate = NULL;
	AgpdQuestTemplate* pFindQuestTemplate = NULL;

	for (pQuestTemplate = (AgpdQuestTemplate**)m_csQuestTemplate.GetObjectSequence(&lIndex); pQuestTemplate; 
			pQuestTemplate = (AgpdQuestTemplate**)m_csQuestTemplate.GetObjectSequence(&lIndex))
	{
		// 이전 퀘스트가 있을경우
		pFindQuestTemplate = m_csQuestTemplate.Get((*pQuestTemplate)->m_StartCondition.Quest.lBackupTID);
		if (pFindQuestTemplate)
			(*pQuestTemplate)->m_StartCondition.Quest.pQuestTemplate = pFindQuestTemplate;
		else
			(*pQuestTemplate)->m_StartCondition.Quest.pQuestTemplate = NULL;

		// 다음 퀘스트가 있을경우
		pFindQuestTemplate = m_csQuestTemplate.Get((*pQuestTemplate)->m_Result.Quest.lBackupTID);
		if (pFindQuestTemplate)
			(*pQuestTemplate)->m_Result.Quest.pQuestTemplate = pFindQuestTemplate;
		else
			(*pQuestTemplate)->m_Result.Quest.pQuestTemplate = NULL;
	}

	return TRUE;
}

BOOL AgpmQuest::StreamReadQuestList(list<INT32>* pList, CHAR* szBuffer)
{
	CHAR* token;

	token = strtok(szBuffer, ":");
	while (token != NULL)
	{
		pList->push_back(atoi(token));
		token = strtok(NULL, ":");
	}

	return TRUE;
}

BOOL AgpmQuest::StreamReadCheckPoint(list< AgpdQuestCheckPoint >* pList, CHAR* szBuffer)
{
	CHAR *token;
	token = strtok(szBuffer, ":-");
	AgpdQuestCheckPoint CheckPoint;
	while (token != NULL)
	{
		CheckPoint.lQuestTID = atoi(token);
		token = strtok(NULL, ":-");

		CheckPoint.lIndex = atoi(token);
		pList->push_back(CheckPoint);

		token = strtok(NULL, ":-");
	}
	return TRUE;
}