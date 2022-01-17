// AgpmPvPStream.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 20.


#include "AgpmPvP.h"

BOOL AgpmPvP::SetMaxAreaDropAdmin(INT32 lCount)
{
	if(lCount <= 0)
		return FALSE;

	m_csAreaDropAdmin.SetCount(lCount);
	return m_csAreaDropAdmin.InitializeObject(sizeof(AgpdPvPAreaDrop*), lCount);
}

BOOL AgpmPvP::SetMaxItemDropAdmin(INT32 lCount)
{
	if(lCount <= 0)
		return FALSE;

	m_csItemDropAdmin.SetCount(lCount);
	return m_csItemDropAdmin.InitializeObject(sizeof(AgpdPvPItemDrop*), lCount);
}

BOOL AgpmPvP::SetMaxSkullDropAdmin(INT32 lCount)
{
	if(lCount <= 0)
		return FALSE;

	m_csSkullDropAdmin.SetCount(lCount);
	return m_csSkullDropAdmin.InitializeObject(sizeof(AgpdPvPSkullDrop*), lCount);
}

BOOL AgpmPvP::ReadItemDropTxt(CHAR* szFile, BOOL bDecryption)
{
	if(!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16 nRow = csExcelTxtLib.GetRow();
	INT16 nCol = csExcelTxtLib.GetColumn();

	INT16 nCurrentRow = 1;
	INT16 nCurrentCol = 1;
	INT16 nIndex = 0;

	CHAR* szKindName = NULL;
	CHAR* szBuffer = NULL;
	while(true)
	{
		szKindName = csExcelTxtLib.GetData(0, nCurrentRow);
		if(!szKindName || !strlen(szKindName))
		{
			nCurrentRow++;
			if(nCurrentRow > nRow)
				break;

			continue;
		}

		szBuffer = csExcelTxtLib.GetData(1, nCurrentRow);	// Index
		if(!szBuffer)
		{
			nCurrentRow++;
			if(nCurrentRow > nRow)
				break;

			continue;
		}

		nIndex = atoi(szBuffer);
		if(nIndex == 0)	// Area Drop
		{
			AgpdPvPAreaDrop* pcsPvPAreaDrop = (AgpdPvPAreaDrop*)CreateModuleData(AGPMPVP_DATA_TYPE_AREA_DROP);
			
			// Kind
			strcpy(pcsPvPAreaDrop->m_szKind, szKindName);

			// PvP Mode
			if(strcmp(pcsPvPAreaDrop->m_szKind, AGPMPVP_EXCEL_ITEMDROP_KIND_FREEAREA) == 0)
				pcsPvPAreaDrop->m_ePvPMode = AGPDPVP_MODE_FREE;
			else if(strcmp(pcsPvPAreaDrop->m_szKind, AGPMPVP_EXCEL_ITEMDROP_KIND_COMBATAREA) == 0)
				pcsPvPAreaDrop->m_ePvPMode = AGPDPVP_MODE_COMBAT;

			// Area Probability
			szBuffer = csExcelTxtLib.GetData(2, nCurrentRow);
			if(szBuffer)
				pcsPvPAreaDrop->m_nAreaProbability = atoi(szBuffer);

			// Equip Probability
			szBuffer = csExcelTxtLib.GetData(3, nCurrentRow);
			if(szBuffer)
				pcsPvPAreaDrop->m_nEquipProbability = atoi(szBuffer);

			// Inven Probability
			szBuffer = csExcelTxtLib.GetData(4, nCurrentRow);
			if(szBuffer)
				pcsPvPAreaDrop->m_nInvenProbability = atoi(szBuffer);

			if (!m_csAreaDropAdmin.AddObject((PVOID)&pcsPvPAreaDrop, pcsPvPAreaDrop->m_ePvPMode))
			{
				DestroyModuleData(pcsPvPAreaDrop, AGPMPVP_DATA_TYPE_AREA_DROP);
			}
		}
		else			// Item Drop
		{
			AgpdPvPItemDrop* pcsPvPItemDrop = (AgpdPvPItemDrop*)CreateModuleData(AGPMPVP_DATA_TYPE_ITEM_DROP);

			// Kind
			strcpy(pcsPvPItemDrop->m_szKind, szKindName);
		
			// Index
			szBuffer = csExcelTxtLib.GetData(1, nCurrentRow);
			if(szBuffer)
				pcsPvPItemDrop->m_lIndex = atoi(szBuffer);

			// Equip Probability
			szBuffer = csExcelTxtLib.GetData(3, nCurrentRow);
			if(szBuffer)
				pcsPvPItemDrop->m_nEquipProbability = atoi(szBuffer);

			// Inven Probability
			szBuffer = csExcelTxtLib.GetData(4, nCurrentRow);
			if(szBuffer)
				pcsPvPItemDrop->m_nInvenProbability = atoi(szBuffer);

			if (!m_csItemDropAdmin.AddObject((PVOID)&pcsPvPItemDrop, pcsPvPItemDrop->m_lIndex))
			{
				DestroyModuleData(pcsPvPItemDrop, AGPMPVP_DATA_TYPE_ITEM_DROP);
			}
		}

		nCurrentRow++;
		if(nCurrentRow > nRow)
			break;
	}

    return TRUE;
}

BOOL AgpmPvP::ReadSkullDropTxt(CHAR* szFile, BOOL bDecryption)
{
	if(!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16 nRow = csExcelTxtLib.GetRow();
	INT16 nCol = csExcelTxtLib.GetColumn();

	INT16 nCurrentRow = 1;
	INT16 nCurrentCol = 1;
	INT16 nIndex = 0;

	INT32 lLevelGap = 0;
	INT32 lDropProbability = 0;

	CHAR* szBuffer = NULL;
	while(true)
	{
		// Level Gap
		szBuffer = csExcelTxtLib.GetData(0, nCurrentRow);
		if(!szBuffer)
			break;
		lLevelGap = atoi(szBuffer);

		// Drop Probability
        szBuffer = csExcelTxtLib.GetData(2, nCurrentRow);
		lDropProbability = atoi(szBuffer);

		if(lDropProbability >= 0)
		{
			AgpdPvPSkullDrop* pcsPvPSkullDrop = (AgpdPvPSkullDrop*)CreateModuleData(AGPMPVP_DATA_TYPE_SKULL_DROP);
		
			// Level Gap
			pcsPvPSkullDrop->m_lLevelGap = lLevelGap;
			if(m_lSkullDropMinLevelGap > pcsPvPSkullDrop->m_lLevelGap)
				m_lSkullDropMinLevelGap = pcsPvPSkullDrop->m_lLevelGap;
			if(m_lSkullDropMaxLevelGap < pcsPvPSkullDrop->m_lLevelGap)
				m_lSkullDropMaxLevelGap = pcsPvPSkullDrop->m_lLevelGap;

			// Skull Level
			szBuffer = csExcelTxtLib.GetData(1, nCurrentRow);
			if(szBuffer)
				pcsPvPSkullDrop->m_lSkullLevel = atoi(szBuffer);

			pcsPvPSkullDrop->m_lSkullProbability = lDropProbability;

			// Admin 은 Key 를 0 으로 주면 안된다.
			if(pcsPvPSkullDrop->m_lLevelGap == 0)
				lLevelGap = AGPMPVP_LEVEL_GAP_ZERO;
			m_csSkullDropAdmin.AddObject((PVOID)&pcsPvPSkullDrop, lLevelGap);
		}
		else
		{
			szBuffer = csExcelTxtLib.GetData(0, nCurrentRow);
			CHAR szTmp[16];
			memset(szTmp, 0, sizeof(CHAR) * 16);

			UINT32 i = 0;
			for(i; i < strlen(szBuffer); i++)
			{
				if(szBuffer[i] == ':')
				{
					memcpy(szTmp, szBuffer, i);
					break;
				}
			}
			

			CHAR* szStartLevel = szTmp;
			CHAR* szEndLevel = szBuffer + i + 1;

			INT32 lStartLevel = atoi(szStartLevel);
			INT32 lEndLevel = atoi(szEndLevel);

			m_alSkullDropLevelBranch[nIndex] = lEndLevel;		// 뒷레벨만 저장한다.
			szBuffer = csExcelTxtLib.GetData(1, nCurrentRow);
			if(szBuffer)
				m_alSkullDropLevel[nIndex] = atoi(szBuffer);	// 해골 레벨 저장

			nIndex++;
		}

		nCurrentRow++;
		if(nCurrentRow > nRow)
			break;
	}

	return TRUE;
}

