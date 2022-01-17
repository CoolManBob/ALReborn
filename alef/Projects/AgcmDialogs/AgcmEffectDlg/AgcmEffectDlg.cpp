// AgcmEffectDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "AgcmEffectDlg.h"
#include "../resource.h"
#include "AgcmEffectList.h"
#include "AgcmUseEffectSetDlg.h"
#include "AgcmEffectConditionDlg.h"
#include "AgcmEffectStatusDlg.h"
#include "ScriptDlg.h"
#include "ConditionListDlg.h"
#include "StatusListDlg.h"
#include "EffectDlg.h"
#include "EffectListModalDlg.h"
#include "RotationDlg.h"
#include "EffectHelpDlg.h"
#include "ApModuleStream.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static CEffectDlg			*g_pcsEffectDlg				= NULL;
static CStatusListDlg		*g_pcsStatusListDlg			= NULL;
static CConditionListDlg	*g_pcsConditionListDlg		= NULL;
static CScriptDlg			*g_pcsScriptDlg				= NULL;
static AgcmEffectDlg		*g_pcsAgcmEffectDlgInst		= NULL;
static AgcmEffectList		*g_pcsAgcmEffectList		= NULL;

/******************************************************************************
* Purpose :
*
* 121002. Bob Jung
******************************************************************************/
AgcmEffectDlg *AgcmEffectDlg::GetInstance()
{
	return g_pcsAgcmEffectDlgInst;
}

/******************************************************************************
* Purpose : Construct.
*
* 120902. Bob Jung
******************************************************************************/
AgcmEffectDlg::AgcmEffectDlg()
{
	SetModuleName("AgcmEffectDlg");

//	m_bOpenUseEffectSet		= FALSE;
	g_pcsAgcmEffectDlgInst	= this;
	m_pcsAgcmEventEffect	= NULL;
}

/******************************************************************************
* Purpose : Destruct.
*
* 120902. Bob Jung
******************************************************************************/
AgcmEffectDlg::~AgcmEffectDlg()
{
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::OnInit()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::OnAddModule()
{
	m_pcsAgcmEventEffect = (AgcmEventEffect *)(GetModule("AgcmEventEffect"));

	// Stream callback을 등록한다.
	if(!AddStreamCallback(AGCM_EFFECTDLG_STREAM_TYPE_CONDITION, EventEffectConditionStreamReadCB, EventEffectConditionStreamWriteCB, this))
	{
		OutputDebugString("AgcmEventEffect::OnAddModule() Error (80) !!!\n");
		return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::OnDestroy()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 080304. Bob Jung
******************************************************************************/
UINT32 AgcmEffectDlg::OpenEffectList()
{
	UINT32	ulIndex;

	CEffectListModalDlg dlg(&ulIndex);
	if (dlg.DoModal() == IDOK)
		return ulIndex;

	return 0;
}

/******************************************************************************
* Purpose :
*
* 121002. Bob Jung
******************************************************************************/
VOID AgcmEffectDlg::OpenEffectList(UINT32 *pulEID)
{
	if (!g_pcsAgcmEffectList)
	{
		g_pcsAgcmEffectList = new AgcmEffectList(pulEID);

		g_pcsAgcmEffectList->Create(NULL);
		g_pcsAgcmEffectList->ShowWindow(SW_SHOW);
	}
}

/******************************************************************************
* Purpose :
*
* 040304 BOB
******************************************************************************/
VOID AgcmEffectDlg::CloseEffectList()
{
	if (g_pcsAgcmEffectList)
	{
		g_pcsAgcmEffectList->ShowWindow(SW_HIDE);
		g_pcsAgcmEffectList->DestroyWindow();

		delete g_pcsAgcmEffectList;
		g_pcsAgcmEffectList = NULL;
	}
}

/******************************************************************************
* Purpose :
*
* 121102. Bob Jung
******************************************************************************/
VOID AgcmEffectDlg::OpenUseEffectSet(AgcdUseEffectSet *pstAgcdUseEffectSet/*, INT16 nMaxEffectNum*/)
{
	if (g_pcsEffectDlg)		return;
		
	_EnumCallback(AGCM_EFFECTDLG_CB_ID_OPEN_EFFECT_DLG, NULL, NULL);

	g_pcsEffectDlg		= new CEffectDlg(pstAgcdUseEffectSet);
	g_pcsEffectDlg->Create();
	g_pcsEffectDlg->ShowWindow(SW_SHOW);
}

/******************************************************************************
* Purpose :
*
* 160604. BOB
******************************************************************************/
BOOL AgcmEffectDlg::InitializeUseEffectSet(AgcdUseEffectSet *pstAgcdUseEffectSet)
{
	if (!g_pcsEffectDlg)
		return FALSE;

	return g_pcsEffectDlg->InitializeDlg(pstAgcdUseEffectSet);
}

/******************************************************************************
* Purpose :
*
* 160604. BOB
******************************************************************************/
BOOL AgcmEffectDlg::IsOpenUseEffectSet()
{
	return (g_pcsEffectDlg) ? (TRUE) : (FALSE);
}

/******************************************************************************
* Purpose :
*
* 121102. Bob Jung
******************************************************************************/
/*VOID AgcmEffectDlg::SetEventEffect(AgcdEventEffect *pstEventEffect)
{
	memcpy(&m_stEventEffect, pstEventEffect, sizeof(AgcdEventEffect));
}*/

/******************************************************************************
* Purpose :
*
* 010303. Bob Jung
******************************************************************************/
VOID AgcmEffectDlg::CloseUseEffectSet()
{
/*	if(g_pcsAgcmUseEffectSetDlg)
	{
		g_pcsAgcmUseEffectSetDlg->ShowWindow(SW_HIDE);
		g_pcsAgcmUseEffectSetDlg->DestroyWindow();

		delete g_pcsAgcmUseEffectSetDlg;
		g_pcsAgcmUseEffectSetDlg = NULL;

		m_bOpenUseEffectSet = FALSE;
	}*/

	if (!g_pcsEffectDlg)
		return;

	g_pcsEffectDlg->ShowWindow(SW_HIDE);
	g_pcsEffectDlg->DestroyWindow();

	delete g_pcsEffectDlg;
	g_pcsEffectDlg = NULL;
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::EventEffectConditionStreamRead(CHAR *szFile)
{
	ApModuleStream	csStream;
	UINT16			nNumKeys;
	CHAR			szSectionName[32];

	csStream.Open(szFile);
	nNumKeys = csStream.GetNumSections();

	for(INT16 nCount = 0; nCount < nNumKeys; ++nCount)
	{
		strcpy(szSectionName, csStream.ReadSectionName(nCount));

		if(!csStream.EnumReadCallback(AGCM_EFFECTDLG_STREAM_TYPE_CONDITION, &m_stCondition, this))
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::EventEffectConditionStreamWrite(CHAR *szFile)
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::EventEffectConditionStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmEffectDlgConditionSet	*pSet	= (AgcmEffectDlgConditionSet *)(pData);
	AgcmEffectDlg				*pThis	= (AgcmEffectDlg *)(pClass);

	// 제로 메모리한당~
	memset(pSet, 0, sizeof(AgcmEffectDlgConditionSet));

	const CHAR	*szValueName;
	CHAR		szValue[256];
	CHAR		szCondition[256];
	INT16		nIndex;

	while(pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		// 같은게 있으면 마구마구 넣는다~
		if(!strcmp(szValueName, AGCMEFFECTDLG_CONDITION_ITEM_NUM))
		{
			pStream->GetValue(szValue, 256);
			pSet->m_nItemNum = atoi(szValue);
		}
		else if(!strcmp(szValueName, AGCMEFFECTDLG_CONDITION_ITEM_INDEX))
		{
			pStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%s", &nIndex, szCondition);

			strcpy(pSet->m_aszCondition[nIndex], szCondition);
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::EventEffectConditionStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 021203. Bob Jung
******************************************************************************/
BOOL AgcmEffectDlg::OpenEffectCondition(AgcdUseEffectSet *pcsSet, INT16 nIndex)
{
//	if(!pstEventEffect->m_astData[nIndex])
//		return FALSE;

	AgcdUseEffectSetData	*pcsData	=
		m_pcsAgcmEventEffect->GetAgcaEffectData2()->GetData(	pcsSet,
																(UINT32)(nIndex)	);
	if (!pcsData)
		return FALSE;

//	AgcmEffectConditionDlg dlg(pstEventEffect, pstEventEffect->m_astData[nIndex]);
	AgcmEffectConditionDlg dlg(pcsSet, pcsData);
	if(dlg.DoModal() == IDOK)
		return TRUE;

	return FALSE;
}

BOOL AgcmEffectDlg::OpenEffectStatus(AgcdUseEffectSet *pcsSet, INT16 nIndex)
{
//	if(!pstEventEffect->m_astData[nIndex])
//		return FALSE;

	AgcdUseEffectSetData	*pcsData	=
		m_pcsAgcmEventEffect->GetAgcaEffectData2()->GetData(	pcsSet,
																(UINT32)(nIndex)	);
	if (!pcsData)
		return FALSE;

	AgcmEffectStatusDlg dlg(pcsData);
	if(dlg.DoModal() == IDOK)
		return TRUE;

	return FALSE;
}

/******************************************************************************
* Purpose :
*
* 270204. BOB
******************************************************************************/
BOOL AgcmEffectDlg::OpenEffectScript()
{
	if (g_pcsScriptDlg)
		return TRUE;

	g_pcsScriptDlg = new CScriptDlg();

	g_pcsScriptDlg->Create(NULL);
	g_pcsScriptDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 270204. BOB
******************************************************************************/
VOID AgcmEffectDlg::CloseEffectScript()
{
	if (g_pcsScriptDlg)
	{
		g_pcsScriptDlg->ShowWindow(SW_HIDE);
		g_pcsScriptDlg->DestroyWindow();

		delete g_pcsScriptDlg;
		g_pcsScriptDlg = NULL;
	}
}

/******************************************************************************
* Purpose :
*
* 270204. BOB
******************************************************************************/
BOOL AgcmEffectDlg::IsOpenScriptDlg()
{
	return (g_pcsScriptDlg) ? (TRUE) : (FALSE);
}

/******************************************************************************
* Purpose :
*
* 270204. BOB
******************************************************************************/
BOOL AgcmEffectDlg::SetCallbackSaveEffectScript(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCM_EFFECTDLG_CB_ID_SAVE_SCRIPT, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
//BOOL AgcmEffectDlg::SetCallbackSaveCondition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(AGCM_EFFECTDLG_CB_ID_APPLY_CONDITION, pfCallback, pClass);
//}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
//BOOL AgcmEffectDlg::SetCallbackSaveSSCondition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(AGCM_EFFECTDLG_CB_ID_APPLY_SS_CONDITION, pfCallback, pClass);
//}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
//BOOL AgcmEffectDlg::SetCallbackSaveStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(AGCM_EFFECTDLG_CB_ID_APPLY_STATUS, pfCallback, pClass);
//}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
BOOL AgcmEffectDlg::SetCallbackOpenEffectDlg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCM_EFFECTDLG_CB_ID_OPEN_EFFECT_DLG, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 270204. BOB
******************************************************************************/
BOOL AgcmEffectDlg::_EnumCallback(UINT32 ulPoint, PVOID pData, PVOID pCustData)
{
	return EnumCallback(ulPoint, pData, pCustData);
}

/******************************************************************************
* Purpose :
*
* 270204. BOB
******************************************************************************/
VOID AgcmEffectDlg::UpdateEffectScript(AgcdUseEffectSet *pstSet, INT32 lIndex)
{
	if (g_pcsScriptDlg)
		g_pcsScriptDlg->UpdateScriptDlg(pstSet, lIndex);
}

/*****************************************************************************
* Purpose :
*
* 040304. BOB
******************************************************************************/
BOOL AgcmEffectDlg::OpenEffectConditionList(UINT32 *plCondition, UINT32 *plSSCondition)
{
	if (g_pcsConditionListDlg)
		return TRUE; // skip

	g_pcsConditionListDlg = new CConditionListDlg(plCondition, plSSCondition);

	g_pcsConditionListDlg->Create(NULL);
	g_pcsConditionListDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 040304. BOB
******************************************************************************/
VOID AgcmEffectDlg::CloseEffectConditionList()
{
	if (g_pcsConditionListDlg)
	{
		g_pcsConditionListDlg->CloseSSConditionDlg();
		g_pcsConditionListDlg->ShowWindow(SW_HIDE);
		g_pcsConditionListDlg->DestroyWindow();

		delete g_pcsConditionListDlg;
		g_pcsConditionListDlg = NULL;
	}
}

/******************************************************************************
* Purpose :
*
* 040304. BOB
******************************************************************************/
BOOL AgcmEffectDlg::OpenEffectStatusList(UINT32 *plStatus)
{
/*	if (g_pcsStatusListDlg)
		return TRUE; // skip

	g_pcsStatusListDlg = new CStatusListDlg(plStatus);

	g_pcsStatusListDlg->Create(NULL);
	g_pcsStatusListDlg->ShowWindow(SW_SHOW);
*/
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 250804. BOB
******************************************************************************/
BOOL AgcmEffectDlg::OpenEffectRotationDlg(AgcdUseEffectSetDataRotation **ppstRotation)
{
	CRotationDlg	dlg(ppstRotation);
	dlg.DoModal();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 040304. BOB
******************************************************************************/
VOID AgcmEffectDlg::CloseEffectStatusList()
{
	if (g_pcsStatusListDlg)
	{
		g_pcsStatusListDlg->ShowWindow(SW_HIDE);
		g_pcsStatusListDlg->DestroyWindow();

		delete g_pcsStatusListDlg;
		g_pcsStatusListDlg = NULL;
	}
}


/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
VOID AgcmEffectDlg::CloseAllEffectDlg(BOOL bCloseMainEffectDlg)
{
	if (bCloseMainEffectDlg)
		CloseUseEffectSet();

	CloseEffectScript();
	CloseEffectConditionList();
	CloseEffectStatusList();
	CloseEffectList();

	if (g_pcsConditionListDlg)
		g_pcsConditionListDlg->CloseSSConditionDlg();
}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
VOID AgcmEffectDlg::UpdateEffectConditionList(UINT32 *plCondition, UINT32 *plSSCondition)
{
	if (g_pcsConditionListDlg)
		g_pcsConditionListDlg->UpdateConditionDlg(plCondition, plSSCondition);
}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
VOID AgcmEffectDlg::UpdateEffectStatusList(UINT32 *plStatus)
{
	if (g_pcsStatusListDlg)
		g_pcsStatusListDlg->UpdateStatusDlg(plStatus);
}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
VOID AgcmEffectDlg::UpdateEffectFlagsDlg(AgcdUseEffectSetData *pstData)
{
//	UpdateEffectConditionList(&pstData->m_ulConditionFlags, &pstData->m_ulSSConditionFlags);
	UpdateEffectConditionList(&pstData->m_ulConditionFlags, &pstData->m_ulConditionFlags);
//	UpdateEffectStatusList(&pstData->m_ulStatusFlags);

	if (g_pcsAgcmEffectList)
		g_pcsAgcmEffectList->UpdateEffectIndex(&pstData->m_ulEID);

	EnableEffectFlagsDlg(TRUE);
}

/******************************************************************************
* Purpose :
*
* 060304. BOB
******************************************************************************/
VOID AgcmEffectDlg::EnableEffectFlagsDlg(BOOL bEnable)
{
	if (g_pcsConditionListDlg)
		g_pcsConditionListDlg->EnableApplyButton(bEnable);
	if (g_pcsStatusListDlg)
		g_pcsStatusListDlg->EnableApplyButton(bEnable);
	if (g_pcsScriptDlg)
		g_pcsScriptDlg->EnableSaveButton(bEnable);
	if (g_pcsAgcmEffectList)
		g_pcsAgcmEffectList->EnableApplyButton(bEnable);
}

VOID AgcmEffectDlg::OpenEffectHelp()
{
	CEffectHelpDlg dlg;
	dlg.DoModal();
}

/******************************************************************************
******************************************************************************/