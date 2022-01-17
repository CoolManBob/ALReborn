/******************************************************************************
Module:  AgpmUnion.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
******************************************************************************/

#include "AgpmUnion.h"


AgpdUnionRank	g_csUnionRankTable;

AgpmUnion::AgpmUnion()
{
	SetModuleName("AgpmUnion");
	
	m_pcsAgpmFactors	= NULL;
	m_pcsAgpmCharacter	= NULL;
}

AgpmUnion::~AgpmUnion()
{
}

BOOL AgpmUnion::OnAddModule()
{
	m_pcsAgpmFactors			= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)		GetModule("AgpmCharacter");

	if (!m_pcsAgpmCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgpmUnion::OnInit()
{
	return TRUE;
}

BOOL AgpmUnion::OnDestroy()
{
	return TRUE;
}

BOOL AgpmUnion::SetCallbackUpdateUnionRank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMUNION_CB_UPDATE_UNION_RANK, pfCallback, pClass);
}

BOOL AgpmUnion::SetCallbackUpdateUnionRankPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMUNION_CB_UPDATE_UNION_RANK_POINT, pfCallback, pClass);
}

INT32 AgpmUnion::GetUnionRankPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	INT32	lUnionRankPoint = 0;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lUnionRankPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK);

	return lUnionRankPoint;
}

BOOL AgpmUnion::SetUnionRankPoint(AgpdCharacter *pcsCharacter, INT32 lNewUnionRank)
{
	if (!pcsCharacter || lNewUnionRank == 0)
		return FALSE;

	return m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lNewUnionRank, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK);
}

INT32 AgpmUnion::GetUnionRank(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	INT32	lUnionRankPoint = GetUnionRankPoint(pcsCharacter);

	int i;
	for (i = 1; i < AGPMUNION_MAX_UNION_RANK; ++i)
	{
		if (lUnionRankPoint < g_csUnionRankTable.m_csUnionRankTable[i].m_lNeedRankPoint)
			break;
	}

	return i;
}

BOOL AgpmUnion::UpdateUnionRankPoint(AgpdCharacter *pcsCharacter, INT32 lUpdateRankPoint)
{
	if (!pcsCharacter || lUpdateRankPoint == 0)
		return FALSE;

	INT32	lPrevUnionRank = GetUnionRank(pcsCharacter);
	INT32	lPrevUnionRankPoint = GetUnionRankPoint(pcsCharacter);

	INT32	lNewUnionRankPoint = lPrevUnionRankPoint + lUpdateRankPoint;
	if (lNewUnionRankPoint < 0) lNewUnionRankPoint = 0;

	if (!SetUnionRankPoint(pcsCharacter, lNewUnionRankPoint))
		return FALSE;

	INT32	lNewUnionRank = GetUnionRank(pcsCharacter);

	EnumCallback(AGPMUNION_CB_UPDATE_UNION_RANK_POINT, pcsCharacter, NULL);

	if (lNewUnionRank != lPrevUnionRank)
		EnumCallback(AGPMUNION_CB_UPDATE_UNION_RANK, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgpmUnion::StreamReadUnionRankTable(CHAR *szFile)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE))
		return FALSE;
	
	INT16			nRow	= csExcelTxtLib.GetRow();
	if (nRow < AGPMUNION_MAX_UNION_RANK + 1)
		return FALSE;

	INT16			nColumn	= csExcelTxtLib.GetColumn();

	for (int i = 1; i <= AGPMUNION_MAX_UNION_RANK; ++i)
	{
		// union rank
		CHAR			*szData	= csExcelTxtLib.GetData(1, i);
		if (!szData || !strlen(szData)) return FALSE;
		g_csUnionRankTable.m_csUnionRankTable[i - 1].m_lRank	= atoi(szData);

		// union rank title
		szData	= csExcelTxtLib.GetData(2, i);
		if (!szData || !strlen(szData)) return FALSE;
		strncpy(g_csUnionRankTable.m_csUnionRankTable[i - 1].m_szTitle, szData, AGPMUNION_MAX_UNION_TITLE);

		// bonus charisma
		szData	= csExcelTxtLib.GetData(3, i);
		if (!szData || !strlen(szData)) return FALSE;
		g_csUnionRankTable.m_csUnionRankTable[i - 1].m_lBonusCharisma	= atoi(szData);

		// need union rank point
		szData	= csExcelTxtLib.GetData(4, i);
		if (!szData || !strlen(szData)) return FALSE;
		g_csUnionRankTable.m_csUnionRankTable[i - 1].m_lNeedRankPoint	= atoi(szData);

		// gain union rank point
		for (int j = 0; j < 6; ++j)
		{
			szData	= csExcelTxtLib.GetData(5 + j, i);
			if (!szData || !strlen(szData)) return FALSE;
			g_csUnionRankTable.m_csUnionRankTable[i - 1].m_lBonusRankPoint[j]	= atoi(szData);
		}

		// lose union rank point by union vs union
		szData	= csExcelTxtLib.GetData(11, i);
		if (!szData || !strlen(szData)) return FALSE;
		g_csUnionRankTable.m_csUnionRankTable[i - 1].m_lLoseRankPointByUvU	= atoi(szData);

		// lose union rank point by normal battle
		szData	= csExcelTxtLib.GetData(12, i);
		if (!szData || !strlen(szData)) return FALSE;
		g_csUnionRankTable.m_csUnionRankTable[i - 1].m_lLoseRankPointByNormal	= atoi(szData);

		// restore exp by skull reward
		szData	= csExcelTxtLib.GetData(13, i);
		if (!szData || !strlen(szData)) return FALSE;
		g_csUnionRankTable.m_csUnionRankTable[i - 1].m_lRestoreExpBySkull	= atoi(szData);
	}

	return TRUE;
}