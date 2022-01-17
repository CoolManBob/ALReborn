#include "AgpmDropItem2.h"
#include "ApMemoryTracker.h"
#include <algorithm>
#include <random>

#define AGPMDROPITEM_STREAM_NAME_OPTION_NUM		"OptionNum"
#define AGPMDROPITEM_STREAM_NAME_SOCKET			"SocketNum"
#define AGPMDROPITEM_STREAM_NAME_GROUP			"GroupID"
#define AGPMDROPITEM_STREAM_NAME_RATE			"DropRate"
#define AGPMDROPITEM_STREAM_NAME_RANK			"DropRank"
#define AGPMDROPITEM_STREAM_NAME_SCALE			"DropMeditation"
#define AGPMDROPITEM_STREAM_NAME_LEVEL_MIN		"SuitableLevelMin"
#define AGPMDROPITEM_STREAM_NAME_LEVEL_MAX		"SuitableLevelMax"
#define AGPMDROPITEM_STREAM_NAME_LEVEL_BONUS	"LevelBonus"
#define AGPMDROPITEM_STREAM_NAME_SPIRIT_TYPE	"Spirit Type"

#define AGPMDROPITEM_SPIRIT_TYPE_SCALE			4
#define AGPMDROPITEM_EQUIP_SCALE				4

#define AGPMDROPITEM_OPTION_MAX_TRY				5

const INT32	AGPMDROP2_MAX_SECOND_DROP_PROB		= 99999;

INT32 GetBaseDropRate( AgpdCharacter* pAgpdCharacter, INT32 dropRate )
{
	if ( NULL == pAgpdCharacter ) return dropRate;

	if ( AS_GREEN == pAgpdCharacter->m_lAddictStatus )
		return dropRate;
	else if ( AS_YELLOW == pAgpdCharacter->m_lAddictStatus )
		return dropRate / 2;
	else
		return 0;
}

AgpmDropItem2::AgpmDropItem2()
{
	SetModuleName("AgpmDropItem2");

	m_pcsDropSockets	= NULL;
	m_pcsDropOptionNums	= NULL;

	memset(m_alRankRate, 0, sizeof(FLOAT) * AGPMDROP_MAX_RANK * AGPMDROP_MAX_RANK);
	m_bReadRankRate		= FALSE;

	m_lCurrentEventID = 0;
}

AgpmDropItem2::~AgpmDropItem2()
{
}

BOOL AgpmDropItem2::OnAddModule()
{
	m_pcsAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmItemConvert	= (AgpmItemConvert *) GetModule("AgpmItemConvert");
	m_pcsAgpmBillInfo		= (AgpmBillInfo *) GetModule("AgpmBillInfo");
	m_pcsAgpmConfig			= (AgpmConfig *) GetModule("AgpmConfig");

	if (!m_pcsAgpmItem || !m_pcsAgpmCharacter || !m_pcsAgpmFactors || !m_pcsAgpmBillInfo)
		return FALSE;

	m_lADIndexCharacterTemplate	= m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgpdDropItemADCharTemplate), NULL, NULL);
	m_lADIndexItemTemplate		= m_pcsAgpmItem->AttachItemTemplateData(this, sizeof(AgpdDropItemADItemTemplate), NULL, NULL);

	if (m_lADIndexCharacterTemplate < 0 || m_lADIndexItemTemplate < 0)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackStreamReadImportData(CBStreamImportCharacterData, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackStreamReadImportData(CBStreamImportItemData, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackStreamReadOptionTable(CBStreamEndItemOption, this))
		return FALSE;

	if (!m_pcsAgpmItemConvert->SetCallbackConvertAsDrop(CBConvertAsDrop, this))
		return FALSE;

	if (!m_pcsAgpmConfig)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgpmDropItem2::OnDestroy()
{
	if (m_pcsDropSockets)
	{
		delete [] m_pcsDropSockets;
		m_pcsDropSockets	= NULL;
	}

	for (AgpdDropItem2MapIter Iter = m_csDropGroups2.begin(); Iter != m_csDropGroups2.end(); Iter++)
	{
		AgpdDropItemGroup2 *pAgpdDropItemGroup2 = Iter->second;
		if (pAgpdDropItemGroup2)
		{
			delete pAgpdDropItemGroup2;
		}
	}
	m_csDropGroups2.clear();
	
	for (INT32 lPart = AGPDITEM_OPTION_PART_BODY; lPart < AGPDITEM_OPTION_MAX_PART; ++lPart)
	{
		for (INT32 lLevel = 0; lLevel < AGPMCHAR_MAX_LEVEL; ++lLevel)
		{
			for(INT32 lRank = 0; lRank < AGPMDROP_MAX_ITEMRANK; ++lRank)
			{
				if (m_acsDropOptions[lPart][lLevel][lRank].m_ppRateTable)
					delete [] m_acsDropOptions[lPart][lLevel][lRank].m_ppRateTable;

				if (m_acsRefineryOptions[lPart][lLevel][lRank].m_ppRateTable)
					delete [] m_acsRefineryOptions[lPart][lLevel][lRank].m_ppRateTable;

				if (m_acsGachaOptions[lPart][lLevel][lRank].m_ppRateTable)
					delete [] m_acsGachaOptions[lPart][lLevel][lRank].m_ppRateTable;
			}
		}
	}
	
	if(m_pcsDropOptionNums)
	{
		delete[] m_pcsDropOptionNums;
	}
	
	INT32 lIndex = 0;
	for (AgpdDropItemGroup** ppcsDropGroup = (AgpdDropItemGroup **) m_csDropGroups.GetObjectSequence(&lIndex);
		ppcsDropGroup;
		ppcsDropGroup = (AgpdDropItemGroup **) m_csDropGroups.GetObjectSequence(&lIndex))
	{
		AgpdDropItemGroup* pcsDropGroup	= *ppcsDropGroup;
		if (!pcsDropGroup)
			continue;
			
		delete pcsDropGroup;
	}
	
	m_csDropGroups.RemoveObjectAll();

	return TRUE;
}

AgpdDropItemGroup * AgpmDropItem2::AddDropGroup(AgpdDropItemGroup *pcsDropGroup)
{
	if (!pcsDropGroup)
		return NULL;

	if (!m_csDropGroups.AddObject(&pcsDropGroup, pcsDropGroup->m_lID))
		return NULL;

	return pcsDropGroup;
}

AgpdDropItemGroup * AgpmDropItem2::GetDropGroup(INT32 lID)
{
	AgpdDropItemGroup **	ppcsDropGroup	= (AgpdDropItemGroup **) m_csDropGroups.GetObject(lID);

	if (!ppcsDropGroup)
		return NULL;

	return *ppcsDropGroup;
}

AgpdDropItemGroup2* AgpmDropItem2::GetDropGroup2(INT32 lID)
{
	AgpdDropItemGroup2 *pGroup2 = NULL;
	AgpdDropItem2MapIter Iter = m_csDropGroups2.find(lID);
	if (Iter != m_csDropGroups2.end())
		pGroup2 = Iter->second;	
	
	return pGroup2;
}

BOOL AgpmDropItem2::ReadDropGroup(CHAR *szFile, BOOL bDecryption, BOOL bReload)
{
	INT32					lID;
	INT32					lRow;
	INT32					lColumn;
	AuExcelTxtLib			csStream;
	CHAR *					pszData					= NULL;
	INT32					lLevel;
	AgpdDropItemGroup *		pcsDropGroup;

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmDropItem2::ReadDropGroup() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	m_lGroupCount	= csStream.GetRow() - 1;

	if (!bReload)
		m_csDropGroups.InitializeObject(sizeof(AgpdDropItemGroup *), m_lGroupCount);

	for(lRow = 1; lRow <= m_lGroupCount; ++lRow)
	{
		for (lColumn = 0; lColumn < csStream.GetColumn(); ++lColumn)
		{
			pszData = csStream.GetData(lColumn, 0);
			if(!pszData)
			{
				continue;
			}

			if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_GROUP))
			{
				lID				= csStream.GetDataToInt(lColumn, lRow);
				pcsDropGroup	= GetDropGroup(lID);
				if (!pcsDropGroup)
				{
					pcsDropGroup	= new AgpdDropItemGroup;
					pcsDropGroup->m_lID	= lID;

					VERIFY(AddDropGroup(pcsDropGroup));
				}
				else
				{
					// 여기서 해당 Group의 ItemTemplate List를 날려주면 좋으나... 그렇게까진 안하겠음.. (Reload일 경우에는 Group 바꾸지 말것)
					pcsDropGroup->Init();
				}
			}
			else if (!strncmp(pszData, AGPMDROPITEM_STREAM_NAME_RATE, strlen(AGPMDROPITEM_STREAM_NAME_RATE)))
			{
				ASSERT(pcsDropGroup);

				pcsDropGroup->m_lDropRate	= csStream.GetDataToInt(lColumn, lRow);
			}
			else if (!strncmp(pszData, AGPMDROPITEM_STREAM_NAME_LEVEL_BONUS, strlen(AGPMDROPITEM_STREAM_NAME_LEVEL_BONUS)))
			{
				ASSERT(pcsDropGroup);

				lLevel	= atoi(pszData + strlen(AGPMDROPITEM_STREAM_NAME_LEVEL_BONUS));

				if (lLevel > AGPMCHAR_MAX_LEVEL)
					return FALSE;

				pcsDropGroup->m_alLevelBonus[INT32(lLevel / 10) - 1]	= csStream.GetDataToInt(lColumn, lRow);
			}
		}
	}

	return TRUE;
}

BOOL AgpmDropItem2::ReadDropGroup2(CHAR *szFile, BOOL bDecryption, BOOL bReload)
{
	AuExcelTxtLib			csStream;
	if (!csStream.OpenExcelFile(szFile, TRUE, bDecryption))		// ignore when no data file found
		return TRUE;

	//if (bReload)
	//	m_csDropGroups2.

	CHAR *psz = NULL;
	INT32 lPrevCharTID = 0;
	INT32 lRow = 0;
	INT32 lRowCount = csStream.GetRow();
	while(lRow <= lRowCount)
	{
		INT32 lCharTID = 0;
		do
		{
			psz = csStream.GetData(1, ++lRow);	// character TID
			lCharTID = atoi(psz ? psz : "0");
		} while(lCharTID == 0 && lRow <= lRowCount);

		if(lCharTID == 0)
			break;

		// find group
		AgpdDropItemGroup2 *pAgpdDropItemGroup2 = GetDropGroup2(lCharTID);
		if (!pAgpdDropItemGroup2)
		{
			pAgpdDropItemGroup2 = new AgpdDropItemGroup2;
			pAgpdDropItemGroup2->m_lCharacterTID = lCharTID;
				
			m_csDropGroups2.insert(AgpdDropItem2MapPair(lCharTID, pAgpdDropItemGroup2));
		}

		if (pAgpdDropItemGroup2->m_lCharacterTID != lCharTID)
			continue;

		AgpdDropItemGroupEntry2 GroupEntry2;
		GroupEntry2.m_lCharacterTID = lCharTID;

		// drop count
		psz = csStream.GetData(2, lRow);
		GroupEntry2.m_lDropCount = atoi(psz ? psz : "0");
		if (0 >= GroupEntry2.m_lDropCount)
			return FALSE;

		INT32 lTempRow = lRow;
		INT32 lNextCharTID = 0;
		do
		{
			psz = csStream.GetData(1, ++lTempRow);
			lNextCharTID = atoi(psz ? psz : "0");
		} while(lNextCharTID == 0 && lTempRow < lRowCount);

		for(; lRow < lTempRow; ++lRow)
		{
			// entry
			AgpdDropItemEntry2 Entry2;

			psz = csStream.GetData(3, lRow);						// item tid
			Entry2.m_lItemTID = atoi(psz ? psz : "0");
			AgpdItemTemplate *pAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate(Entry2.m_lItemTID);
			psz = csStream.GetData(4, lRow);						// min
			Entry2.m_lMinCount = atoi(psz ? psz : "0");
			psz = csStream.GetData(5, lRow);						// max
			Entry2.m_lMaxCount = atoi(psz ? psz : "0");
			psz = csStream.GetData(6, lRow);						// prob.
			Entry2.m_lProb = atoi(psz ? psz : "0");
			
			if (0 == Entry2.m_lItemTID ||
				NULL == pAgpdItemTemplate ||
				0 >= Entry2.m_lMinCount ||
				Entry2.m_lMinCount > Entry2.m_lMaxCount ||
				0 >= Entry2.m_lProb ||
				100000 < Entry2.m_lProb
				)
			{
				continue;
			}

			GroupEntry2.m_vtDropItems.push_back(Entry2);
		}
		--lRow;
		
		pAgpdDropItemGroup2->m_vtGroupEntries.push_back(GroupEntry2);
	}	

  #ifdef _DEBUG
	
	for (AgpdDropItem2MapIter Iter = m_csDropGroups2.begin(); Iter != m_csDropGroups2.end(); Iter++)
	{
		AgpdDropItemGroup2 *pAgpdDropItemGroup2 = Iter->second;
		ASSERT(NULL != pAgpdDropItemGroup2);
		ASSERT(pAgpdDropItemGroup2->m_vtGroupEntries.size() > 0);
	}
	  
  #endif

	return TRUE;
}

BOOL AgpmDropItem2::ReadDropSocket(CHAR *szFile, BOOL bDecryption, BOOL bReload)
{
	INT32					lRow;
	INT32					lColumn;
	AuExcelTxtLib			csStream;
	CHAR *					pszData					= NULL;
	INT32					lMaxSocketNum;
	INT32					lSocketNum;
	INT32					lSocketCount;

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmDropItem2::ReadDropSocket() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	lSocketCount	= csStream.GetRow();
	ASSERT(!bReload || lSocketCount == m_lSocketCount);
	if (bReload && lSocketCount != m_lSocketCount)
	{
		return FALSE;
	}

	m_lSocketCount	= lSocketCount;

	ASSERT(!bReload || m_pcsDropSockets);
	if (!bReload)
	{
		if (m_pcsDropSockets)
			delete [] m_pcsDropSockets;

		m_pcsDropSockets	= new AgpdDropItemSocket [m_lSocketCount];
	}

	for(lRow = 1; lRow < m_lGroupCount; ++lRow)
	{
		for (lColumn = 0; lColumn < csStream.GetColumn(); ++lColumn)
		{
			pszData = csStream.GetData(lColumn, 0);
			if(!pszData)
			{
				continue;
			}

			if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_SOCKET))
			{
				lSocketNum	= csStream.GetDataToInt(lColumn, lRow);
				ASSERT(lSocketNum < m_lSocketCount);
				if (lSocketNum >= m_lSocketCount)
					return FALSE;
			}
			else if (!strncmp(pszData, AGPMDROPITEM_STREAM_NAME_RATE, strlen(AGPMDROPITEM_STREAM_NAME_RATE)))
			{
				lMaxSocketNum	= atoi(pszData + strlen(AGPMDROPITEM_STREAM_NAME_RATE));

				ASSERT(lMaxSocketNum < m_lSocketCount);
				if (lMaxSocketNum >= m_lSocketCount)
					return FALSE;

				if (!m_pcsDropSockets[lMaxSocketNum].m_plSocketRate)
				{
					m_pcsDropSockets[lMaxSocketNum].m_plSocketRate	= new INT32 [m_lSocketCount];
					memset(m_pcsDropSockets[lMaxSocketNum].m_plSocketRate, 0, sizeof(INT32) * m_lSocketCount);
				}

				m_pcsDropSockets[lMaxSocketNum].m_plSocketRate[lSocketNum]	= csStream.GetDataToInt(lColumn, lRow);
			}
		}
	}

	return TRUE;
}

BOOL AgpmDropItem2::ReadDropOption(CHAR *szFile, BOOL bDecryption, BOOL bReload)
{
	INT32					lRow;
	INT32					lColumn;
	AuExcelTxtLib			csStream;
	CHAR *					pszData					= NULL;
	INT32					lMaxOptionNum;
	INT32					lOptionNum;
	INT32					lOptionCount;

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmDropItem2::ReadDropOption() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	lOptionCount	= csStream.GetRow();
	ASSERT(!bReload || lOptionCount == m_lOptionCount);
	if (bReload && lOptionCount != m_lOptionCount)
	{
		csStream.CloseFile();
		return FALSE;
	}

	m_lOptionCount	= lOptionCount;

	ASSERT(!bReload || m_pcsDropOptionNums);
	if (!bReload)
	{
		if (m_pcsDropOptionNums)
			delete [] m_pcsDropOptionNums;

		m_pcsDropOptionNums	= new AgpdDropItemOptionNum [m_lOptionCount];
	}

	for(lRow = 1; lRow < m_lGroupCount; ++lRow)
	{
		for (lColumn = 0; lColumn < csStream.GetColumn(); ++lColumn)
		{
			pszData = csStream.GetData(lColumn, 0);
			if(!pszData)
			{
				continue;
			}

			if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_OPTION_NUM))
			{
				lOptionNum	= csStream.GetDataToInt(lColumn, lRow);

				ASSERT(lOptionNum < m_lOptionCount);
				if (lOptionNum >= m_lOptionCount)
					return FALSE;
			}
			else if (!strncmp(pszData, AGPMDROPITEM_STREAM_NAME_RATE, strlen(AGPMDROPITEM_STREAM_NAME_RATE)))
			{
				lMaxOptionNum	= atoi(pszData + strlen(AGPMDROPITEM_STREAM_NAME_RATE));

				ASSERT(lMaxOptionNum < m_lOptionCount);
				if (lMaxOptionNum >= m_lOptionCount)
					return FALSE;

				if (!m_pcsDropOptionNums[lMaxOptionNum].m_plOptionRate)
				{
					m_pcsDropOptionNums[lMaxOptionNum].m_plOptionRate	= new INT32 [m_lOptionCount];
					memset(m_pcsDropOptionNums[lMaxOptionNum].m_plOptionRate, 0, sizeof(INT32) * m_lOptionCount);
				}

				m_pcsDropOptionNums[lMaxOptionNum].m_plOptionRate[lOptionNum]	= csStream.GetDataToInt(lColumn, lRow);
			}
		}
	}

	return TRUE;
}

BOOL AgpmDropItem2::ReadDropRankRate(CHAR *szFile, BOOL bDecryption, BOOL bReload)
{
	INT32					lRow;
	INT32					lColumn;
	AuExcelTxtLib			csStream;
	CHAR *					pszData					= NULL;
	//INT32					lMaxOptionNum;
	INT32					lRankNum;
	//INT32					lOptionCount;

	m_bReadRankRate = FALSE;

	if(!csStream.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmDropItem2::ReadDropOption() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	// 이 Table은 정방형을 기본으로 한다.
	lRankNum	= csStream.GetRow();
	if (csStream.GetColumn() > lRankNum)
		lRankNum = csStream.GetColumn();

	ASSERT(lRankNum < AGPMDROP_MAX_RANK);
	if (lRankNum >= AGPMDROP_MAX_RANK)
	{
		OutputDebugString("AgpmDropItem2::ReadDropRankRate() Error (1) !!!\n");
		csStream.CloseFile();
		return FALSE;
	}

	for(lRow = 1; lRow <= AGPMDROP_MAX_RANK; ++lRow)
	{
		for (lColumn = 1; lColumn <= AGPMDROP_MAX_RANK; ++lColumn)
		{
			pszData = csStream.GetData(lColumn, lRow);
			if(!pszData)
			{
				m_alRankRate[lRow][lColumn] = 0;
			}
			else
			{
				m_alRankRate[lRow][lColumn] = atol(pszData);
			}
		}
	}

	m_bReadRankRate = TRUE;

	return TRUE;
}

BOOL AgpmDropItem2::CBStreamImportItemData(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpmDropItem2 *			pThis = (AgpmDropItem2 *) pvClass;
	AgpdItemTemplate *		pcsItemTemplate				= (AgpdItemTemplate *)		pvData;
	PVOID *					ppvBuffer					= (PVOID *)					pvCustData;

	AuExcelLib *			pcsExcelTxtLib				= (AuExcelLib *)			ppvBuffer[0];
	INT32					nRow						= PtrToInt(ppvBuffer[1]);
	INT32					nCol						= PtrToInt(ppvBuffer[2]);

	AgpdDropItemGroup *		pcsDropGroup;
	AgpdDropItemEntry		csEntry;
	AgpdDropItemsIter		iter;

	AgpdDropItemADItemTemplate *	pcsADItemTemplate	= pThis->GetItemTemplateData(pcsItemTemplate);

	if (!pcsExcelTxtLib)
		return FALSE;

	// key row is zero
	//////////////////////////////////////////////////////////

	CHAR					*pszData				= pcsExcelTxtLib->GetData(nCol, 0);
	if(!pszData)
		return FALSE;

	if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_RANK))
	{
		pcsADItemTemplate->m_lDropRank		= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
		if (pcsADItemTemplate->m_lGroupID)
		{
			pcsDropGroup = pThis->GetDropGroup(pcsADItemTemplate->m_lGroupID);
			if (pcsDropGroup)
				++(pcsDropGroup->m_alRankItemCount[pcsADItemTemplate->m_lDropRank]);
		}
	}
	else if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_GROUP))
	{
		pcsADItemTemplate->m_lGroupID	= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
		pcsDropGroup	= pThis->GetDropGroup(pcsADItemTemplate->m_lGroupID);
		if (pcsDropGroup)
		{
			for (iter = pcsDropGroup->m_vtTemplates.begin(); iter != pcsDropGroup->m_vtTemplates.end(); ++iter)
			{
				if ((*iter).m_pcsItemTemplate	== pcsItemTemplate)
					break;
			}

			// Group에 없다는 이야기이므로, add하자.
			if (iter == pcsDropGroup->m_vtTemplates.end())
			{
				csEntry.m_pcsItemTemplate	= pcsItemTemplate;
				pcsDropGroup->m_vtTemplates.push_back(csEntry);

				ASSERT(pcsADItemTemplate->m_lDropRank < AGPMDROP_MAX_RANK);
				if (pcsADItemTemplate->m_lDropRank)
					++(pcsDropGroup->m_alRankItemCount[pcsADItemTemplate->m_lDropRank]);
			}
		}
	}
	else if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_RATE))
	{
		pcsADItemTemplate->m_lDropRate	= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
	}
	else if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_LEVEL_MIN))
	{
		pcsADItemTemplate->m_lSuitableLevelMin	= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
	}
	else if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_LEVEL_MAX))
	{
		pcsADItemTemplate->m_lSuitableLevelMax	= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
	}
	else if (!strcmp(pszData, "GachaMinLv"))
	{
		pcsADItemTemplate->m_lGachaLevelMin	= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
	}
	else if (!strcmp(pszData, "GachaMaxLv"))
	{
		pcsADItemTemplate->m_lGachaLevelMax	= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
	}

	return TRUE;
}

BOOL AgpmDropItem2::CBStreamImportCharacterData(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpmDropItem2 *			pThis = (AgpmDropItem2 *) pvClass;
	AgpdCharacterTemplate *	pcsCharacterTemplate		= (AgpdCharacterTemplate *)	pvData;
	PVOID *					ppvBuffer					= (PVOID *)					pvCustData;

	AuExcelLib *			pcsExcelTxtLib				= (AuExcelLib *)			ppvBuffer[0];
	INT32					nRow						= PtrToInt(ppvBuffer[1]);
	INT32					nCol						= PtrToInt(ppvBuffer[2]);

	AgpdDropItemADCharTemplate *	pcsADCharacterTemplate	= pThis->GetCharacterTemplateData(pcsCharacterTemplate);

	if (!pcsExcelTxtLib)
		return FALSE;

	// key row is zero
	//////////////////////////////////////////////////////////

	CHAR					*pszData				= pcsExcelTxtLib->GetData(nCol, 0);
	if(!pszData)
		return FALSE;

	if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_RANK))
	{
		pcsADCharacterTemplate->m_lRank	= pcsExcelTxtLib->GetDataToInt(nCol, nRow);
	}
	else if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_SCALE))
	{
		if (pcsExcelTxtLib->GetData(nCol, nRow))
			pcsADCharacterTemplate->m_fRateScale	= (FLOAT) atof(pcsExcelTxtLib->GetData(nCol, nRow));
	}
	else if (!strcmp(pszData, AGPMDROPITEM_STREAM_NAME_SPIRIT_TYPE))
	{
		pcsADCharacterTemplate->m_eSpiritStoneType	= (AgpmItemUsableSpiritStoneType) pcsExcelTxtLib->GetDataToInt(nCol, nRow);
	}

	return TRUE;
}

BOOL AgpmDropItem2::CBStreamEndItemOption(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpmDropItem2 *				pThis = (AgpmDropItem2 *) pvClass;
	AgpdItemOptionTemplate *	pcsOptionTemplate;
	INT32						lIndex	= 0;
	INT32						lPart;
	INT32						lRate	= 0;
	INT32						lLevel;

	AuAutoLock	lock(pThis->m_csCSectionOptionProbability);
	if (!lock.Result()) return FALSE;

	memset(pThis->m_acsDropOptions, 0, sizeof(pThis->m_acsDropOptions));
	memset(pThis->m_acsRefineryOptions, 0, sizeof(pThis->m_acsRefineryOptions));
	memset(pThis->m_acsGachaOptions, 0, sizeof(pThis->m_acsGachaOptions));

	for (AgpaItemOptionTemplate::iterator it = pThis->m_pcsAgpmItem->csOptionTemplateAdmin.begin();
		it != pThis->m_pcsAgpmItem->csOptionTemplateAdmin.end(); ++it)
	{
		pcsOptionTemplate = it->second;

		for (lPart = AGPDITEM_OPTION_PART_BODY; lPart < AGPDITEM_OPTION_MAX_PART; ++lPart)
		{
			if (!(pcsOptionTemplate->m_ulSetPart & (1 << lPart)))
				continue;

			for (lLevel = pcsOptionTemplate->m_lLevelLimit; lLevel < AGPMCHAR_MAX_LEVEL; ++lLevel)
			{
				for(INT32 lRank = pcsOptionTemplate->m_lRankLimit; lRank < AGPMDROP_MAX_ITEMRANK; ++lRank)
				{
					if(pcsOptionTemplate->m_ulSetPart & AGPDITEM_OPTION_SET_TYPE_REFINERY)
					{
						pThis->m_acsRefineryOptions[lPart][lLevel][lRank].m_lTotalRate += pcsOptionTemplate->m_lProbability;
						pThis->m_acsRefineryOptions[lPart][lLevel][lRank].m_vtOptions.push_back(pcsOptionTemplate);
					}
					else if(pcsOptionTemplate->m_ulSetPart & AGPDITEM_OPTION_SET_TYPE_GACHA)
					{
						// 가챠 확율 계산 추가
						pThis->m_acsGachaOptions[lPart][lLevel][lRank].m_lTotalRate	+= pcsOptionTemplate->m_lProbability;
						pThis->m_acsGachaOptions[lPart][lLevel][lRank].m_vtOptions.push_back(pcsOptionTemplate);
					}
					else
					{
						pThis->m_acsDropOptions[lPart][lLevel][lRank].m_lTotalRate	+= pcsOptionTemplate->m_lProbability;
						pThis->m_acsDropOptions[lPart][lLevel][lRank].m_vtOptions.push_back(pcsOptionTemplate);
					}
				}
			}
		}
	}

	for (lPart = AGPDITEM_OPTION_PART_BODY; lPart < AGPDITEM_OPTION_MAX_PART; ++lPart)
	{
		for (lLevel = 0; lLevel < AGPMCHAR_MAX_LEVEL; ++lLevel)
		{
			for(INT32 lRank = 0; lRank < AGPMDROP_MAX_ITEMRANK; ++lRank)
			{
				AgpdDropItemOption* pcsDropOption = &pThis->m_acsDropOptions[lPart][lLevel][lRank];

				if (!pcsDropOption->m_lTotalRate)
					continue;

				if (pcsDropOption->m_ppRateTable)
					delete [] pcsDropOption->m_ppRateTable;

				pcsDropOption->m_lRateTableCount = (INT32)pcsDropOption->m_vtOptions.size();
				pcsDropOption->m_ppRateTable = new AgpdDropItemOptionTable[pcsDropOption->m_lRateTableCount];
				
				for (lIndex	= 0, lRate	= 0; lIndex < pcsDropOption->m_lRateTableCount; ++lIndex)
				{
					AgpdItemOptionTemplate* pcsAgpdOptionTemplate = pcsDropOption->m_vtOptions[lIndex];
					lRate	+= pcsAgpdOptionTemplate->m_lProbability;
					pcsDropOption->m_ppRateTable[lIndex].Rate = lRate;
					pcsDropOption->m_ppRateTable[lIndex].pcsAgpdOptionTemplate = pcsAgpdOptionTemplate;
				}
			}
		}

		for (lLevel = 0; lLevel < AGPMCHAR_MAX_LEVEL; ++lLevel)
		{
			for(INT32 lRank = 0; lRank < AGPMDROP_MAX_ITEMRANK; ++lRank)
			{
				AgpdDropItemOption* pcsDropOption = &pThis->m_acsRefineryOptions[lPart][lLevel][lRank];

				if (!pcsDropOption->m_lTotalRate)
					continue;

				if (pcsDropOption->m_ppRateTable)
					delete [] pcsDropOption->m_ppRateTable;

				pcsDropOption->m_lRateTableCount = (INT32)pcsDropOption->m_vtOptions.size();
				pcsDropOption->m_ppRateTable = new AgpdDropItemOptionTable[pcsDropOption->m_lRateTableCount];

				for (lIndex	= 0, lRate	= 0; lIndex < pcsDropOption->m_lRateTableCount; ++lIndex)
				{
					AgpdItemOptionTemplate* pcsAgpdOptionTemplate = pcsDropOption->m_vtOptions[lIndex];
					lRate	+= pcsAgpdOptionTemplate->m_lProbability;
					pcsDropOption->m_ppRateTable[lIndex].Rate = lRate;
					pcsDropOption->m_ppRateTable[lIndex].pcsAgpdOptionTemplate = pcsAgpdOptionTemplate;
				}
			}
		}

		for (lLevel = 0; lLevel < AGPMCHAR_MAX_LEVEL; ++lLevel)
		{
			for(INT32 lRank = 0; lRank < AGPMDROP_MAX_ITEMRANK; ++lRank)
			{
				AgpdDropItemOption* pcsDropOption = &pThis->m_acsGachaOptions[lPart][lLevel][lRank];

				if (!pcsDropOption->m_lTotalRate)
					continue;

				if (pcsDropOption->m_ppRateTable)
					delete [] pcsDropOption->m_ppRateTable;

				pcsDropOption->m_lRateTableCount = (INT32)pcsDropOption->m_vtOptions.size();
				pcsDropOption->m_ppRateTable = new AgpdDropItemOptionTable[pcsDropOption->m_lRateTableCount];

				for (lIndex	= 0, lRate	= 0; lIndex < pcsDropOption->m_lRateTableCount; ++lIndex)
				{
					AgpdItemOptionTemplate* pcsAgpdOptionTemplate = pcsDropOption->m_vtOptions[lIndex];
					lRate	+= pcsAgpdOptionTemplate->m_lProbability;
					pcsDropOption->m_ppRateTable[lIndex].Rate = lRate;
					pcsDropOption->m_ppRateTable[lIndex].pcsAgpdOptionTemplate = pcsAgpdOptionTemplate;
				}
			}
		}
	}

	return TRUE;
}

INT32 AgpmDropItem2::AdjustRateByLevelGap(INT32	lDropRate, INT32 lTargetLevel, INT32 lAttackerLevel)
{
	/*
	<Mob Level ≥ PC Level>
		= DropRate 
		* (1 - (((Mob Level - PC Level) * 2)^2) / 500)

	<Mob Level < PC Level>
		= DropRate 
		* (1 - (((Mob Level - PC Level) * 2)^2) / 500)
	*/

	INT32	lAdjustRate	= 0;

	if (lTargetLevel >= lAttackerLevel)
		lAdjustRate	= (INT32) (lDropRate * CalcLevelGap(lTargetLevel, lAttackerLevel));
	else
		lAdjustRate	= (INT32) (lDropRate * CalcLevelGap(lAttackerLevel, lTargetLevel));

	if (lAdjustRate < 0)
		lAdjustRate	= 0;

	return lAdjustRate;

	/*
	INT32	lGap = lAttackerLevel - lTargetLevel;

	if (-2 <= lGap && lGap <= 1)
		return lDropRate;
	else if (-4 <= lGap && lGap <= 2)
		return INT32(lDropRate * 0.8f);
	else if (-6 <= lGap && lGap <= 3)
		return INT32(lDropRate * 0.6f);
	else if (-8 <= lGap && lGap <= 4)
		return INT32(lDropRate * 0.2f);

	return 0;
	*/
}

FLOAT AgpmDropItem2::CalcLevelGap(INT32 lTargetLevel, INT32 lAttackerLevel)
{
	return (FLOAT) (1.0f - (FLOAT)(pow((double)((lTargetLevel - lAttackerLevel) * 2), 2)) / 500.0f);
}

BOOL AgpmDropItem2::ProcessDropItem(AgpdCharacter *pcsDropCharacter, INT32 lAttackerLevel, ApBase *pcsFirstLooter)
{
	if (!pcsDropCharacter || lAttackerLevel <= 0 || lAttackerLevel > AGPMCHAR_MAX_LEVEL)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessDropItem"));

	INT32	lRandomValue;
	INT32	lDropRate;
	INT32	lDropRateTotal;
	INT32	lRank;

	INT32	lMobLevel = m_pcsAgpmCharacter->GetLevel(pcsDropCharacter);

	AgpdDropItemGroup **	ppcsDropGroup;
	AgpdDropItemGroup *		pcsDropGroup;

	AgpdDropItems			vtDropList;
	AgpdDropItems			vtResultList;
	AgpdDropItemsIter		iter;

	AgpdDropItemInfo2		csDropInfo;

	AgpdDropItemADCharTemplate *	pcsADCharacterTemplate	= GetCharacterTemplateData(pcsDropCharacter->m_pcsCharacterTemplate);

	csDropInfo.m_pcsDropCharacter	= pcsDropCharacter;
	csDropInfo.m_lAttackerLevel		= lAttackerLevel;
	csDropInfo.m_pcsFirstLooter		= pcsFirstLooter;

	// 이벤트 몬스터라면 이벤트 아이템을 떨군다.
	if (pcsDropCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_EVENT_GIFTBOX &&
		CalcLevelGap(lMobLevel, lAttackerLevel) > 0)
	{
		EventMapIter iter = m_mapEventItem.find(m_lCurrentEventID);
		if(iter != m_mapEventItem.end())
		{
			EventItemEffect& stEventItemEffect = iter->second;

			INT32 lSize = (INT32)stEventItemEffect.m_vcItemTIDs.size();
			if(lSize > 0)
			{
				INT32 lRand = m_csRand.randInt(lSize);
				if(lRand < 0) lRand = 0;
				else if(lRand > lSize - 1) lRand = lSize - 1;

				EnumCallback(AGPM_DROPITEM_CB_DROP_ITEM, m_pcsAgpmItem->GetItemTemplate(stEventItemEffect.m_vcItemTIDs[lRand]), &csDropInfo);
			}
		}
	}

	// 하드코딩... ㅡㅡ;;;
	// TID가 197인 몬스터가 오면 아이템을 무조건 드랍
	if (197 == pcsDropCharacter->m_pcsCharacterTemplate->m_lID)
	{
		EnumCallback(AGPM_DROPITEM_CB_DROP_ITEM, m_pcsAgpmItem->GetItemTemplate(4608), &csDropInfo);
		return TRUE;
	}

	INT32 lIndex = 0;
	INT32 baseDropRate = 0;

	for (ppcsDropGroup = (AgpdDropItemGroup **) m_csDropGroups.GetObjectSequence(&lIndex);
		ppcsDropGroup;
		ppcsDropGroup = (AgpdDropItemGroup **) m_csDropGroups.GetObjectSequence(&lIndex))
	{
		pcsDropGroup	= *ppcsDropGroup;
		if (!pcsDropGroup)
			continue;

		baseDropRate = GetBaseDropRate( m_pcsAgpmCharacter->GetCharacter(pcsFirstLooter->m_lID), pcsDropGroup->m_lDropRate );

		lDropRate	= AdjustRateByLevelGap(baseDropRate, lMobLevel, lAttackerLevel);
		lDropRate	= INT32(lDropRate * pcsADCharacterTemplate->m_fRateScale);
		
		if ( 0 != lDropRate)
			lDropRate	+= pcsDropGroup->m_alLevelBonus[(lAttackerLevel - 1) / 10];

		// 2006.06.30. steeple. 이제 제대로 적용.
		// 어우샹. 일단 주석. 2006.06.02.
		// GameBonus 적용 2006.06.01. steeple
		if(pcsFirstLooter)
		{
			AgpdCharacter* pcsFirstLooterCharacter = m_pcsAgpmCharacter->GetCharacter(pcsFirstLooter->m_lID);

			if(pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER)
			{	
				INT32 lRate = m_pcsAgpmCharacter->GetGameBonusDropRate(pcsFirstLooterCharacter);
				/*lRate += m_pcsAgpmItemConvert->GetBonusDrop(pcsFirstLooterCharacter);*/

				lDropRate	+= (INT32)((double)lDropRate * (double)lRate / (double)100);
			}
			else if(pcsFirstLooter->m_eType == APBASE_TYPE_PARTY)
			{
				INT32 lRate = 0;
				EnumCallback(AGPM_DROPITEM_CB_GET_BONUS_OF_PARTY_MEMBER, &lRate, &csDropInfo);

				lDropRate	+= (INT32)((double)lDropRate * (double)lRate / (double)100);
			}

			BOOL  bTPackUser = FALSE;
			INT32 ulPCRoomType = 0;
			m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsFirstLooterCharacter, &ulPCRoomType);

			if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)  
				bTPackUser = TRUE;

			lDropRate = (INT32) ( (float) lDropRate * m_pcsAgpmConfig->GetDropAdjustmentRatio(bTPackUser) );
		}

		lRandomValue	= m_csRand.randInt(AGPMDROP_RATE_LIMIT - 1);

		if (lRandomValue >= lDropRate)
			continue;
		
		lRank = GetDropRank(pcsDropGroup, pcsADCharacterTemplate->m_lRank);

		if(lRank > 0)
		{
			pcsDropGroup->m_csCSection.Lock();

			// 해당 Group의 ItemTemplate 내에서 뒤져서, 가능한 List를 만든다.
			lDropRateTotal	= 0;
			for (iter = pcsDropGroup->m_vtTemplates.begin(); iter != pcsDropGroup->m_vtTemplates.end(); ++iter)
			{
				if (((*iter).m_pcsItemTemplate)->m_bIsVillainOnly &&
					pcsFirstLooter)
					if (pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER && !m_pcsAgpmCharacter->IsMurderer((AgpdCharacter *) pcsFirstLooter) ||
						pcsFirstLooter->m_eType != APBASE_TYPE_CHARACTER)
						continue;

				(*iter).m_lDropRate	= GetDropRate((*iter).m_pcsItemTemplate,
										  lMobLevel,
										  lRank,
										  pcsADCharacterTemplate->m_eSpiritStoneType,
										  (pcsFirstLooter && pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER) ? (AgpdCharacter *) pcsFirstLooter : NULL);
				if ((*iter).m_lDropRate)
				{
					lDropRateTotal += (*iter).m_lDropRate;

					vtDropList.push_back(*iter);
				}
			}

			// 최종적으로 확률계산까지 마친 ItemTemplate List를 가져온다.
			if (!GetDropItemList(&vtDropList, &vtResultList, lDropRateTotal))
			{
				pcsDropGroup->m_csCSection.Unlock();
				return FALSE;
			}

			// Item drop rate 100%
			if (pcsDropCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DROP_RATE_100)
			{
				vtResultList.clear();
				for (iter = pcsDropGroup->m_vtTemplates.begin(); iter != pcsDropGroup->m_vtTemplates.end(); ++iter)
				{
					vtResultList.push_back(*iter);
				}
			}

			pcsDropGroup->m_csCSection.Unlock();

			for (iter = vtResultList.begin(); iter != vtResultList.end(); ++iter)
			{
                EnumCallback(AGPM_DROPITEM_CB_DROP_ITEM, (*iter).m_pcsItemTemplate, &csDropInfo);
			}

			vtResultList.clear();
			vtDropList.clear();
		}
	}

	return TRUE;
}

BOOL AgpmDropItem2::ProcessDropItem2(AgpdCharacter *pcsDropCharacter, INT32 lAttackerLevel, ApBase *pcsFirstLooter)
{
	if (!pcsDropCharacter || lAttackerLevel <= 0 || lAttackerLevel > AGPMCHAR_MAX_LEVEL)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessDropItem2"));

	AgpdDropItemInfo2		csDropInfo;
	csDropInfo.m_pcsDropCharacter	= pcsDropCharacter;
	csDropInfo.m_lAttackerLevel		= lAttackerLevel;
	csDropInfo.m_pcsFirstLooter		= pcsFirstLooter;
	INT32 lTargetLevel = m_pcsAgpmCharacter->GetLevel(pcsDropCharacter);

	// find group2
	AgpdDropItemGroup2 *pAgpdDropItemGroup2 = GetDropGroup2(pcsDropCharacter->m_pcsCharacterTemplate->m_lID);
	if (!pAgpdDropItemGroup2)
		return TRUE;

	// Check FirstLooter is TPack User 
	BOOL	bTPackUser		= FALSE;
	INT32	ulPCRoomType	= 0;

	m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, m_pcsAgpmCharacter->GetCharacter(pcsFirstLooter->m_lID), &ulPCRoomType);

	if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
		bTPackUser = TRUE;

	// Group 에 Depth 하나가 추가되었다. 2008.05.16. steeple
	AgpdDropItemGroupEntryIter GroupEntryIter = pAgpdDropItemGroup2->m_vtGroupEntries.begin();
	while(GroupEntryIter != pAgpdDropItemGroup2->m_vtGroupEntries.end())
	{
		if(GroupEntryIter->m_lDropCount < 1)
		{
			++GroupEntryIter;
			continue;
		}

		AgpdDropItems2 RandomSample = GroupEntryIter->m_vtDropItems;
		/*RandomSample.reserve(GroupEntryIter->m_lDropCount);
		std::random_sample_n(GroupEntryIter->m_vtDropItems.begin(),
							GroupEntryIter->m_vtDropItems.end(),
							back_inserter(RandomSample),
							GroupEntryIter->m_lDropCount);*/

		std::random_device rd;
		std::mt19937 g(rd());

		std::shuffle(RandomSample.begin(), RandomSample.end(), g);

		AgpdDropItemsIter2 iter = RandomSample.begin();
		while(iter != RandomSample.end())
		{
			AgpdDropItemEntry2& ItemEntry2 = *iter;

			// item template validation
			AgpdItemTemplate *pAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate(ItemEntry2.m_lItemTID);
			if (!pAgpdItemTemplate)
			{
				++iter;
				continue;
			}
			
			// evaluate
			INT32 lProb = ItemEntry2.m_lProb;
			lProb = AdjustRateByLevelGap(lProb, lTargetLevel, lAttackerLevel);

			// 2008.07.16. steeple
			// 확률 계산으로 변경.
			double dProb = (double)lProb;
			dProb += dProb * (double)m_pcsAgpmCharacter->GetGameBonusDropRate2(m_pcsAgpmCharacter->GetCharacter(pcsFirstLooter->m_lID)) / (double)100;
			lProb = (INT32)dProb;

			// bonus ratio
			lProb = (INT32)((double)lProb * (double)m_pcsAgpmConfig->GetDrop2AdjustmentRatio(bTPackUser));
			if (m_csRand.randInt(AGPMDROP2_MAX_SECOND_DROP_PROB) > lProb)
			{
				++iter;
				continue;
			}		

			// determine stack count
			INT32 lStackCount = ItemEntry2.m_lMinCount + m_csRand.randInt(ItemEntry2.m_lMaxCount - ItemEntry2.m_lMinCount);
			csDropInfo.m_lItemStackCount = min(lStackCount, pAgpdItemTemplate->m_lMaxStackableCount);	// can't exceed maximum stack count

			// drop
			EnumCallback(AGPM_DROPITEM_CB_DROP_ITEM, pAgpdItemTemplate, &csDropInfo);

			++iter;
		}

		++GroupEntryIter;
	}
	
	return TRUE;
}

INT32 AgpmDropItem2::GetDropRank(AgpdDropItemGroup *pcsDropGroup, INT32 lMobRank)
{
	INT32	lIndex;
	INT32	lTotalRate = 0;
	INT32	lRandomValue;

	for (lIndex = 1; lIndex < AGPMDROP_MAX_RANK; ++lIndex)
	{
		if (!pcsDropGroup->m_alRankItemCount[lIndex] || m_alRankRate[lMobRank][lIndex] == 0)
			continue;

		lTotalRate += m_alRankRate[lMobRank][lIndex];
	}

	if (lTotalRate == 0)
		return lMobRank;

	lRandomValue = m_csRand.randInt(lTotalRate - 1);
	lTotalRate = 0;
	for (lIndex = 1; lIndex < AGPMDROP_MAX_RANK; ++lIndex)
	{
		if (!pcsDropGroup->m_alRankItemCount[lIndex] || m_alRankRate[lMobRank][lIndex] == 0)
			continue;

		lTotalRate += m_alRankRate[lMobRank][lIndex];
		if (lTotalRate >= lRandomValue)
			return lIndex;
	}

	return lMobRank;
}

INT32 AgpmDropItem2::GetDropRate(AgpdItemTemplate *pcsItemTemplate, INT32 lMobLevel, INT32 lMobRank, AgpmItemUsableSpiritStoneType eMobSpiritType, AgpdCharacter *pcsAttacker)
{
	if (!pcsItemTemplate)
		return 0;

	AgpdDropItemADItemTemplate *	pcsADItemTemplate	= GetItemTemplateData(pcsItemTemplate);
	INT32							lScaleFactor		= 1;

	ASSERT(lMobRank <= AGPMDROP_MAX_RANK && pcsADItemTemplate->m_lDropRank <= AGPMDROP_MAX_RANK);
	if (lMobRank > AGPMDROP_MAX_RANK || pcsADItemTemplate->m_lDropRank > AGPMDROP_MAX_RANK)
		return 0;

	if (pcsADItemTemplate->m_lSuitableLevelMin > lMobLevel ||
		lMobLevel > pcsADItemTemplate->m_lSuitableLevelMax)
	{
		return 0;
	}

	if (pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
		((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
	{
		AgpdItemTemplateUsableSpiritStone *	pcsSpiritStone	= (AgpdItemTemplateUsableSpiritStone *) pcsItemTemplate;
		if (pcsSpiritStone->m_eSpiritStoneType == eMobSpiritType)
			lScaleFactor *= AGPMDROPITEM_SPIRIT_TYPE_SCALE;
	}

	if (pcsADItemTemplate->m_lDropRank != lMobRank)
		return 0;

	if (pcsItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
	{
		// 장비형 중에 사용 가능한 것은 4배 확률
		if (pcsAttacker && m_pcsAgpmItem->CheckUseItem(pcsAttacker, pcsItemTemplate))
			lScaleFactor	*= AGPMDROPITEM_EQUIP_SCALE;
	}

	BOOL	bTPackUser		= FALSE;
	INT32	ulPCRoomType	= 0;

	m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsAttacker, &ulPCRoomType);

	if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
		bTPackUser = TRUE;

	return (INT32) (m_pcsAgpmConfig->GetDropAdjustmentRatio(bTPackUser) * pcsADItemTemplate->m_lDropRate * lScaleFactor);
}

BOOL AgpmDropItem2::GetDropItemList(AgpdDropItems *pvtDropList, AgpdDropItems *pvtResultList, INT32 lTotalRate, INT32 lItemNum)
{
	INT32					lIndex;
	AgpdDropItemsIter		itItemList;
	INT32					lDropRate			= 0;
	INT32					lRemainItem			= lItemNum;
	AgpdDropItemADItemTemplate *	pcsADItemTemplate;

	vector<INT32> plRandomValue;
	plRandomValue.reserve(lItemNum);

	for (lIndex = 0; lIndex < lItemNum; ++lIndex)
	{
		plRandomValue[lIndex]	= m_csRand.randInt(lTotalRate - 1);
	}

	itItemList	= pvtDropList->begin();
	while (itItemList != pvtDropList->end())
	{
		pcsADItemTemplate	= GetItemTemplateData((*itItemList).m_pcsItemTemplate);

		lDropRate	+= (*itItemList).m_lDropRate;

		for (lIndex = 0; lIndex < lItemNum; ++lIndex)
		{
			if (lDropRate >= plRandomValue[lIndex])
			{
				// OK, Drop 한다.
				pvtResultList->push_back(*itItemList);
				plRandomValue[lIndex]	= lTotalRate + 100;

				--lRemainItem;
			}
		}

		if (!lRemainItem)
			break;

		++itItemList;
	}

	return TRUE;
}

BOOL AgpmDropItem2::ProcessConvertItem(AgpdItem *pcsItem, AgpdCharacter *pcsDropCharacter)
{
	if (!m_pcsAgpmItemConvert)
		return TRUE;

	AgpdItemTemplate *		pcsItemTemplate			= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
	if (pcsItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP)
		return TRUE;
	VERIFY(ProcessConvertItemSocket(pcsItem, pcsItemTemplate->m_lMinSocketNum, pcsItemTemplate->m_lMaxSocketNum));
	VERIFY(ProcessConvertItemPhysical(pcsItem, pcsDropCharacter));
	VERIFY(ProcessConvertItemOption(pcsItem, pcsDropCharacter));

	return TRUE;
}

INT32 AgpmDropItem2::GetItemOptionNum(AgpdItem *pcsItem)
{
	if (!m_pcsAgpmItemConvert)
		return 0;

	INT32				lRandomValue	= 0;
	AgpdItemTemplate *	pcsItemTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
	INT32				lIndex;
	INT32				lRateTotal		= 0;

	if (m_pcsDropOptionNums[pcsItemTemplate->m_lMaxOptionNum].m_plOptionRate)
	{
		for (lIndex = pcsItemTemplate->m_lMinOptionNum; lIndex <= pcsItemTemplate->m_lMaxOptionNum; ++lIndex)
		{
			lRateTotal	+= m_pcsDropOptionNums[pcsItemTemplate->m_lMaxOptionNum].m_plOptionRate[lIndex];
		}

		lRandomValue	= m_csRand.randInt(lRateTotal - 1);
		lRateTotal		= 0;

		for (lIndex = pcsItemTemplate->m_lMinOptionNum; lIndex <= pcsItemTemplate->m_lMaxOptionNum; ++lIndex)
		{
			lRateTotal	+= m_pcsDropOptionNums[pcsItemTemplate->m_lMaxOptionNum].m_plOptionRate[lIndex];
			if (lRateTotal >= lRandomValue)
			{
				return lIndex;
			}
		}
	}

	return 0;
}

AgpdItemOptionPart	AgpmDropItem2::GetItemOptionPart(AgpdItemTemplate *pcsItemTemplate)
{
	switch (((AgpdItemTemplateEquip *) pcsItemTemplate)->m_nKind)
	{
		case AGPMITEM_EQUIP_KIND_WEAPON:
			{
				return AGPDITEM_OPTION_PART_WEAPON;
			}
			break;

		case AGPMITEM_EQUIP_KIND_SHIELD:
			{
				return AGPDITEM_OPTION_PART_SHIELD;
			}
			break;

		case AGPMITEM_EQUIP_KIND_RING:
			{
				return AGPDITEM_OPTION_PART_RING;
			}
			break;

		case AGPMITEM_EQUIP_KIND_NECKLACE:
			{
				return AGPDITEM_OPTION_PART_NECKLACE;
			}
			break;

		case AGPMITEM_EQUIP_KIND_ARMOUR:
			{
				switch (((AgpdItemTemplateEquip *) pcsItemTemplate)->m_nPart) {
					case AGPMITEM_PART_BODY:
						{
							return AGPDITEM_OPTION_PART_BODY;
						}
						break;

					case AGPMITEM_PART_HEAD:
						{
							return AGPDITEM_OPTION_PART_HEAD;
						}
						break;

					case AGPMITEM_PART_ARMS:
						{
						}
						break;

					case AGPMITEM_PART_HANDS:
						{
							return AGPDITEM_OPTION_PART_HANDS;
						}
						break;

					case AGPMITEM_PART_LEGS:
						{
							return AGPDITEM_OPTION_PART_LEGS;
						}
						break;

					case AGPMITEM_PART_FOOT:
						{
							return AGPDITEM_OPTION_PART_FOOTS;
						}
						break;
				}
			}
			break;
	}

	return AGPDITEM_OPTION_MAX_PART;
}

BOOL AgpmDropItem2::ProcessConvertItemSocket(AgpdItem *pcsItem, INT32 MinSocket, INT32 MaxSocket)
{
	INT32	lRandomValue = 0;
	INT32	lIndex       = 0;
	INT32	lRateTotal	 = 0;

	if (m_pcsDropSockets[MaxSocket].m_plSocketRate)
	{
		for (lIndex = MinSocket; lIndex <= MaxSocket; ++lIndex)
		{
			lRateTotal	+= m_pcsDropSockets[MaxSocket].m_plSocketRate[lIndex];
		}

		lRandomValue	= m_csRand.randInt(lRateTotal - 1);
		lRateTotal		= 0;

		for (lIndex = MinSocket; lIndex <= MaxSocket; ++lIndex)
		{
			lRateTotal	+= m_pcsDropSockets[MaxSocket].m_plSocketRate[lIndex];
			if (lRateTotal >= lRandomValue && lIndex > 0)
			{
				VERIFY(m_pcsAgpmItemConvert->SetSocketConvert(pcsItem, lIndex));

				break;
			}
		}
	}

	return TRUE;
}

BOOL AgpmDropItem2::ProcessConvertItemPhysical(AgpdItem *pcsItem, AgpdCharacter *pcsDropCharacter)
{
	// 장비형 아이템 중 Weapon이나 Armour(Body)인 경우에만 Physical Convert를 한다.
	AgpdItemTemplate *		pcsItemTemplate			= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
	AgpdItemTemplateEquip *	pcsEquipItemTemplate	= (AgpdItemTemplateEquip *) pcsItemTemplate;

	if (pcsEquipItemTemplate->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON &&
		(pcsEquipItemTemplate->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR || pcsEquipItemTemplate->m_nPart != AGPMITEM_PART_BODY))
		return TRUE;

	INT32					lRandomValue	= m_csRand.randInt(AGPMDROP_RATE_LIMIT - 1);
	INT32					lPhysical		= 0;

	// +2 : 5, +1 : 25, +0 : 70
	if (lRandomValue <= 5000)
		lPhysical	= 2;
	else if (lRandomValue <= 30000)
		lPhysical	= 1;

	if (lPhysical)
	{
		VERIFY(m_pcsAgpmItemConvert->SetPhysicalConvert(pcsItem, lPhysical, FALSE));
	}

	return TRUE;
}

BOOL AgpmDropItem2::ProcessConvertItemOption(AgpdItem *pcsItem, AgpdCharacter *pcsDropCharacter)
{
	if (!pcsDropCharacter)
		return FALSE;

	return ProcessConvertItemOption(pcsItem, pcsDropCharacter ? m_pcsAgpmCharacter->GetLevel(pcsDropCharacter) : 1);
}

BOOL AgpmDropItem2::ProcessConvertItemOption(AgpdItem *pcsItem, INT32 lTargetLevel)
{
	INT32	lIndex	= 0;
	INT32	lRandomNumber	= 0;

	AgpdItemOptionTemplate *	pcsOptionTemplate;
	BOOL						bValidOption;
	INT32						lTryNum	= 0;
	AgpdItemOptionPart			eOptionPart	= GetItemOptionPart((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

	if (lTargetLevel >= AGPMCHAR_MAX_LEVEL)
		lTargetLevel = AGPMCHAR_MAX_LEVEL - 1;

	ASSERT(lTargetLevel > 0 && lTargetLevel < AGPMCHAR_MAX_LEVEL);
	ASSERT(eOptionPart < AGPDITEM_OPTION_MAX_PART);
	if (lTargetLevel <= 0 || lTargetLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;
	if (eOptionPart >= AGPDITEM_OPTION_MAX_PART)
		return FALSE;

	INT32	lRank	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_pcsItemTemplate->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	if( lRank == 0 )
		lRank = 1;

	INT32	lOptionNum		= GetItemOptionNum(pcsItem);

	for (lIndex = 0; lIndex < lOptionNum; ++lIndex)
	{
		{
			AuAutoLock	lock(m_csCSectionOptionProbability);
			if (lock.Result())
			{
				AgpdDropItemOption* pDropOption = &m_acsDropOptions[eOptionPart][lTargetLevel][lRank];
				if (!pDropOption->m_lTotalRate)
					return TRUE;

				lRandomNumber	= m_csRand.randInt(pDropOption->m_lTotalRate - 1);

				for(INT32 i = 0; i < pDropOption->m_lRateTableCount; ++i)
				{
					pcsOptionTemplate = pDropOption->m_ppRateTable[i].pcsAgpdOptionTemplate;

					if(pDropOption->m_ppRateTable[i].Rate > lRandomNumber)
						break;
				}
			}
		}

		ASSERT(pcsOptionTemplate);
		if (!pcsOptionTemplate)
			return FALSE;

		bValidOption	= TRUE;

		if (!m_pcsAgpmItem->IsProperPart((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate, pcsOptionTemplate) ||
			m_pcsAgpmItem->IsAlreadySetType(pcsItem, pcsOptionTemplate))
			bValidOption	= FALSE;

		/*
		if (bValidOption &&
			!(pcsOptionTemplate->m_eOptionSetType == AGPDITEM_OPTION_SET_DROP ||
			  pcsOptionTemplate->m_eOptionSetType == AGPDITEM_OPTION_SET_ALL))
		{
			bValidOption	= FALSE;
		}
		*/

		if (bValidOption)
		{
			VERIFY(m_pcsAgpmItem->AddItemOption(pcsItem, pcsOptionTemplate));

			lTryNum	= 0;
		}
		else if (lTryNum >= AGPMDROPITEM_OPTION_MAX_TRY)
			break;
		else
		{
			--lIndex;
			++lTryNum;
		}
	}

	return TRUE;
}
// refinery시에만 사용.
BOOL AgpmDropItem2::ProcessConvertItemOptionRefinery(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter, INT32 OptionMin, INT32 OptionMax)
{
    INT32 Scope			= OptionMax - OptionMin + 1;
	INT32 OptionNum		= 0;
    INT32 lIndex		= 0;
    INT32 lRandomNum	= 0;
	INT32 lTryNum		= 0;
	INT32 IUserLevel	= m_pcsAgpmCharacter->GetLevel(pcsCharacter);
    AgpdItemOptionTemplate *	pcsOptionTemplate = NULL;

	AgpdItemOptionPart			eOptionPart	= GetItemOptionPart((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

	if (OptionMin == OptionMax) 
        OptionNum = OptionMax;
	else
		OptionNum = OptionMin + rand() % Scope;

	INT32	lRank	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_pcsItemTemplate->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
	
	if( lRank == 0 )
		lRank = 1;

	for (lIndex = 0; lIndex < OptionNum; ++lIndex)
    {
        AuAutoLock	lock(m_csCSectionOptionProbability);
        if (lock.Result())
        {
			AgpdDropItemOption* pDropOption = &m_acsRefineryOptions[eOptionPart][IUserLevel][lRank];
			if (!pDropOption->m_lTotalRate)
				return TRUE;

			lRandomNum	= m_csRand.randInt(pDropOption->m_lTotalRate - 1);

			for(INT32 i = 0; i < pDropOption->m_lRateTableCount; ++i)
			{
				pcsOptionTemplate = pDropOption->m_ppRateTable[i].pcsAgpdOptionTemplate;

				if(pDropOption->m_ppRateTable[i].Rate > lRandomNum)
					break;
			}
        }

		ASSERT(pcsOptionTemplate);
        if (!pcsOptionTemplate)
            return FALSE;
        if (m_pcsAgpmItem->AddRefineItemOption(pcsItem, pcsOptionTemplate))
            lTryNum	= 0;
        else if (lTryNum >= AGPMDROPITEM_OPTION_MAX_TRY)
            break;
        else
		{
			--lIndex;
			++lTryNum;
        }
    }
    return TRUE;
}

BOOL	AgpmDropItem2::ProcessConvertItemOptionGacha(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter , INT32 OptionMin, INT32 OptionMax)
{
    INT32 Scope			= OptionMax - OptionMin + 1;
	INT32 OptionNum		= 0;
    INT32 lIndex		= 0;
    INT32 lRandomNum	= 0;
	INT32 lTryNum		= 0;
	INT32 IUserLevel	= m_pcsAgpmCharacter->GetLevel(pcsCharacter);
    AgpdItemOptionTemplate *	pcsOptionTemplate = NULL;

	AgpdItemOptionPart			eOptionPart	= GetItemOptionPart((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate);

	if (OptionMin == OptionMax) 
        OptionNum = OptionMax;
	else
		OptionNum = OptionMin + rand() % Scope;

	INT32	lRank	= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_pcsItemTemplate->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	if( lRank == 0 )
		lRank = 1;

	for (lIndex = 0; lIndex < OptionNum; ++lIndex)
    {
        AuAutoLock	lock(m_csCSectionOptionProbability);
        if (lock.Result())
        {
			AgpdDropItemOption* pDropOption = &m_acsGachaOptions[eOptionPart][IUserLevel][lRank];
			if (!pDropOption->m_lTotalRate)
				return TRUE;

			lRandomNum	= m_csRand.randInt(pDropOption->m_lTotalRate - 1);

			for(INT32 i = 0; i < pDropOption->m_lRateTableCount; ++i)
			{
				pcsOptionTemplate = pDropOption->m_ppRateTable[i].pcsAgpdOptionTemplate;

				if(pDropOption->m_ppRateTable[i].Rate > lRandomNum)
					break;
			}
        }

		ASSERT(pcsOptionTemplate);
        if (!pcsOptionTemplate)
            return FALSE;
        if (m_pcsAgpmItem->AddRefineItemOption(pcsItem, pcsOptionTemplate))
            lTryNum	= 0;
        else if (lTryNum >= AGPMDROPITEM_OPTION_MAX_TRY)
            break;
        else
		{
			--lIndex;
			++lTryNum;
        }
    }
    return TRUE;
}

BOOL AgpmDropItem2::SetCallbackDropItem(ApModuleDefaultCallBack fnCallback, PVOID pClass)
{
	return SetCallback(AGPM_DROPITEM_CB_DROP_ITEM, fnCallback, pClass);
}

BOOL AgpmDropItem2::SetCallbackGetBonusOfPartyMember(ApModuleDefaultCallBack fnCallback, PVOID pClass)
{
	return SetCallback(AGPM_DROPITEM_CB_GET_BONUS_OF_PARTY_MEMBER, fnCallback, pClass);
}

BOOL AgpmDropItem2::CBConvertAsDrop(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpmDropItem2 *	pThis = (AgpmDropItem2 *) pvClass;
	AgpdItem *		pcsItem = (AgpdItem *) pvData;

	pThis->ProcessConvertItem(pcsItem, pcsItem->m_pcsCharacter);

	return TRUE;
}

BOOL AgpmDropItem2::IsEventItem(INT32 lTID)
{
	if(lTID < 1)
		return FALSE;

	EventMapIter iter = m_mapEventItem.find(m_lCurrentEventID);
	if(iter != m_mapEventItem.end())
	{
		EventItemEffect& stEventItemEffect = iter->second;

		std::vector<INT32>::iterator iterTID = std::find(stEventItemEffect.m_vcItemTIDs.begin(),
														stEventItemEffect.m_vcItemTIDs.end(),
														lTID);

		if(iterTID != stEventItemEffect.m_vcItemTIDs.end())
			return TRUE;
	}
	
	return FALSE;
}
