// AgcmLODDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "../resource.h"
#include "AgcmLODDlg.h"
#include "AgcmLODDistanceDlg.h"
#include "LODDlg.h"
#include "AgcmFileListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AgcmLODDlg		*g_pcsAgcmLODDlgInst	= NULL;
static CLODDlg			*g_pcsLODDlg			= NULL;

/******************************************************************************
* Purpose :
*
* 121702. Bob Jung
******************************************************************************/
AgcmLODDlg *AgcmLODDlg::GetInstance()
{
	return g_pcsAgcmLODDlgInst;
}

/******************************************************************************
* Purpose : Construct.
*
* 121702. Bob Jung
******************************************************************************/
AgcmLODDlg::AgcmLODDlg()
{
	SetModuleName("AgcmLODDlg");

	m_pcsAgcmPreLODManager		= NULL;
	m_pcsLOD					= NULL;
	m_pcsPreLOD					= NULL;

	g_pcsAgcmLODDlgInst			= this;
}

/******************************************************************************
* Purpose : Destruct.
*
* 121702. Bob Jung
******************************************************************************/
AgcmLODDlg::~AgcmLODDlg()
{
	g_pcsAgcmLODDlgInst = NULL;
}

BOOL AgcmLODDlg::FindLODDataName(CHAR *szDest)
{
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	CHAR			szFilePathName1[256], szFilePathName2[256];

	sprintf(szFilePathName1, "%s*.rws", m_szDataDirectory);
	sprintf(szFilePathName2, "%s*.dff", m_szDataDirectory);

	pszTemp = dlg.OpenFileList(szFilePathName1, szFilePathName2, NULL);
	if(pszTemp)
	{
		strcpy(szDest, pszTemp);

		return TRUE;
	}

	return FALSE;
}

BOOL AgcmLODDlg::ApplyLODDlg(AgcdPreLOD *pstPreLOD)
{
	if(!pstPreLOD)
		return FALSE;

	INT32 lCount, lCount2, lBillCount, lMaxLODNum;

	for(lCount = 0; lCount < m_pcsPreLOD->m_lNum; ++lCount)
	{
		AgcdPreLODData	*pcsDest	= m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(m_pcsPreLOD, lCount);
		AgcdLODData		*pcsLODData	= m_pcsAgcmLODManager->m_csLODList.GetLODData(m_pcsLOD, lCount);
		if((pcsDest) && (pcsLODData))
		{
			AgcdPreLODData	*pcsSrc = m_pcsAgcmPreLODManager->m_csPreLODAdmin.GetPreLODData(pstPreLOD, lCount);
			memcpy(pcsDest, pcsSrc, sizeof(AgcdPreLODData));

			lBillCount	= 0;
			lMaxLODNum	= 0;
			for(lCount2 = 1; lCount2 < AGPDLOD_MAX_NUM; ++lCount2)
			{
				if(!strcmp(pcsDest->m_aszData[lCount2], ""))
					break;
				else
					++lMaxLODNum;

				if(pcsDest->m_stBillboard.isbill[lCount2])
				{
					pcsLODData->m_alBillInfo[lBillCount] = lCount2;
					++lBillCount;
				}
			}

			pcsLODData->m_lHasBillNum	= lBillCount;

			pcsDest->m_stBillboard.num	= (lMaxLODNum) ? (lMaxLODNum + 1) : (0);
			pcsLODData->m_ulMaxLODLevel	= (lMaxLODNum) ? (lMaxLODNum + 1) : (0);

			memcpy(pcsLODData->m_aulLODDistance, m_stLODInfo.m_aaulDistance + lCount, sizeof(UINT32) * AGPDLOD_MAX_NUM);
			pcsLODData->m_ulMaxDistanceRatio = m_stLODInfo.m_aulMaxDistanceRatio[lCount];

			pcsLODData->m_ulBoundary	=
				(!strcmp(pcsDest->m_aszData[lMaxLODNum], "DUMMY"))							?
				(pcsLODData->m_aulLODDistance[	(lMaxLODNum) ? (lMaxLODNum - 1) : (0)	])	:
				(pcsLODData->m_aulLODDistance[	lMaxLODNum								])	;
		}
	}

	m_pcsLOD->m_lDistanceType = m_stLODInfo.m_lDistanceType;

	EnumCallback(AGCMLODDLG_CB_ID_APPLY_LOD, (PVOID)(m_pcsPreLOD), NULL);

	return TRUE;
}

BOOL AgcmLODDlg::CloseLODDlg(AgcdPreLOD *pstPreLOD)
{
	if(g_pcsLODDlg)
	{
		if(pstPreLOD)
		{
			ApplyLODDlg(pstPreLOD);
		}

		g_pcsLODDlg->ShowWindow(SW_HIDE);
		g_pcsLODDlg->DestroyWindow();

		delete g_pcsLODDlg;
		g_pcsLODDlg = NULL;
	}

	return TRUE;
}

BOOL AgcmLODDlg::OpenLODDlg(AgcdLOD *pstLOD, AgcdPreLOD *pstAgcdPreLOD, CHAR *szDataDirectory, AgcdDefaultLODInfo *pstLODInfo)
{
	if((pstLOD->m_lNum < 1) || (g_pcsLODDlg))
		return FALSE; // 이미 열려 있다.

	INT32 lCount;

	if(pstLOD->m_lNum > pstAgcdPreLOD->m_lNum)
	{
		for(lCount = 0; lCount < pstLOD->m_lNum - pstAgcdPreLOD->m_lNum; ++lCount)
		{
			AgcdPreLODData	*pcsData = m_pcsAgcmPreLODManager->m_csPreLODAdmin.AddPreLODData(pstAgcdPreLOD);
			if(!pcsData)
				return FALSE;
		}
	}
	else if(pstLOD->m_lNum < pstAgcdPreLOD->m_lNum)
	{
		for(lCount = 0; lCount < pstAgcdPreLOD->m_lNum - pstLOD->m_lNum; ++lCount)
		{
			m_pcsAgcmPreLODManager->m_csPreLODAdmin.RemovePreLODData(pstAgcdPreLOD, pstAgcdPreLOD->m_lNum - 1);
		}
	}

	memset(&m_stLODInfo, 0, sizeof(AgcdDefaultLODInfo));
	memcpy(m_stLODInfo.m_paszDFFInfo, pstLODInfo->m_paszDFFInfo, sizeof(CHAR *) * AGCM_LODDLG_MAX_INFO);

	for(lCount = 0; lCount < pstLOD->m_lNum; ++lCount)
	{
		AgcdLODData *pcsLODData = m_pcsAgcmLODManager->m_csLODList.GetLODData(pstLOD, lCount);
		if(!pcsLODData)
			return FALSE;

		if(pcsLODData->m_ulMaxLODLevel == 0)
		{
			memcpy(pcsLODData->m_aulLODDistance, m_aulDefaultDistance, sizeof(UINT32) * AGPDLOD_MAX_NUM);
		}

		memcpy(m_stLODInfo.m_aaulDistance[lCount], pcsLODData->m_aulLODDistance, sizeof(UINT32) * AGPDLOD_MAX_NUM);

		m_stLODInfo.m_aulMaxDistanceRatio[lCount] = pcsLODData->m_ulMaxDistanceRatio;
	}

	m_stLODInfo.m_lDistanceType = pstLOD->m_lDistanceType;

	strcpy(m_szDataDirectory, szDataDirectory);
	m_pcsPreLOD	= pstAgcdPreLOD;
	m_pcsLOD	= pstLOD;

	g_pcsLODDlg = new CLODDlg(m_pcsPreLOD, &m_stLODInfo);
	g_pcsLODDlg->Create();
	g_pcsLODDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 021803. Bob Jung
******************************************************************************/
BOOL AgcmLODDlg::OpenLODDistance(UINT32 *pulDistance)
{
	AgcmLODDistanceDlg dlg(pulDistance);
	if(dlg.DoModal() == IDOK)
		return TRUE;

	return FALSE;
}

/******************************************************************************
* Purpose :
*
* 011403. Bob Jung
******************************************************************************/
BOOL AgcmLODDlg::OnAddModule()
{
	m_pcsAgcmLODManager		= (AgcmLODManager *)(GetModule("AgcmLODManager"));
	if(!m_pcsAgcmLODManager)
		return FALSE;

	m_pcsAgcmPreLODManager	= (AgcmPreLODManager *)(GetModule("AgcmPreLODManager"));
	if(!m_pcsAgcmPreLODManager)
		return FALSE;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 051403. Bob Jung
******************************************************************************/
/*BOOL AgcmLODDlg::MergeObjectLODListStreamRead(CHAR *szFile, INT16 nIndex)
{
	ApModuleStream	csStream;
	if(!csStream.Open(szFile))
	{
		OutputDebugString("AgcmLODDlg::MergeObjectLODListStreamRead() Error (1) !!!\n");
		return FALSE;
	}

	INT16	nNumKeys = csStream.GetNumSections();
	for(INT16 i = 0; i < nNumKeys; ++i)
	{
		ApdObjectTemplate *pcsApdObjectTemplate = m_pcsApmObject->GetMergeTemplate(atoi(csStream.ReadSectionName(i)), nIndex);
		if(!pcsApdObjectTemplate)
		{
			OutputDebugString("AgcmLODDlg::MergeObjectLODListStreamRead() Error (2) !!!\n");
			return FALSE;
		}

		AgcdLODList *pcsAgcdLODList = GetObjectLODList(pcsApdObjectTemplate);
		if(!pcsAgcdLODList)
		{
			OutputDebugString("AgcmLODDlg::MergeObjectLODListStreamRead() Error (3) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumReadCallback(AGCM_LODDLG_STREAM_TYPE_LODLIST_OBJECT, pcsAgcdLODList, this))
		{
			OutputDebugString("AgcmLODDlg::MergeObjectLODListStreamRead() Error (4) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}*/

/******************************************************************************
* Purpose :
*
* 012803. Bob Jung
******************************************************************************/
BOOL AgcmLODDlg::SetCallbackApplyLOD(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMLODDLG_CB_ID_APPLY_LOD, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 021803. Bob Jung
******************************************************************************/
VOID AgcmLODDlg::SetDefaultLODRange(UINT32 *pulRange)
{
	memcpy(m_aulDefaultDistance, pulRange, sizeof(UINT32) * AGPDLOD_MAX_NUM);
}

/******************************************************************************
******************************************************************************/