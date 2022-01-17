// AgcmSkillDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "../resource.h"

#include "AgcmSkillDlg.h"
#include "AgcmCharacterSkillDlg.h"
#include "AgcmCharSkillDlg.h"
#include "AgcmCharSkillDlg.h"
#include "AgcmSkillTemplateDlg.h"
#include "AgcmSkillTemplateListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//static AgcmCharacterSkillDlg	*g_pcsAgcmCharacterSkillDlg	= NULL;
static AgcmSkillDlg				*g_pcsAgcmSkillDlg			= NULL;
static AgcmCharSkillDlg			*g_pcsAgcmCharSkillDlg		= NULL;

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
AgcmSkillDlg *AgcmSkillDlg::GetInstance()
{
	return g_pcsAgcmSkillDlg;
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
AgcmSkillDlg::AgcmSkillDlg()
{
	SetModuleName("AgcmSkillDlg");

	m_pcsAgpmSkill			= NULL;
	m_pcsAgcmSkill			= NULL;
	m_pcsAgcmAnimationDlg	= NULL;
	m_pcsAgcmItem			= NULL;
	m_pcsAgcmEventEffect	= NULL;
	m_pcsAgcmCharacter		= NULL;
	g_pcsAgcmSkillDlg		= this;
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
AgcmSkillDlg::~AgcmSkillDlg()
{
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::OnInit()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::OnAddModule()
{
	m_pcsAgcmCharacter		= (AgcmCharacter *)(GetModule("AgcmCharacter"));
	m_pcsAgcmItem			= (AgcmItem *)(GetModule("AgcmItem"));
	m_pcsAgpmSkill			= (AgpmSkill *)(GetModule("AgpmSkill"));
	m_pcsAgcmSkill			= (AgcmSkill *)(GetModule("AgcmSkill"));
	m_pcsAgcmEventEffect	= (AgcmEventEffect *)(GetModule("AgcmEventEffect"));
	m_pcsAgcmAnimationDlg	= (AgcmAnimationDlg *)(GetModule("AgcmAnimationDlg"));
	m_pcsAgcmEffectDlg		= (AgcmEffectDlg *)(GetModule("AgcmEffectDlg"));

	if (m_pcsAgcmEffectDlg)
	{
		m_pcsAgcmEffectDlg->SetCallbackSaveEffectScript(UpdateSaveEffectCB, this);
		m_pcsAgcmEffectDlg->SetCallbackOpenEffectDlg(OpenEffectDlgCB, this);
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::OnDestroy()
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::SetCallbackGetAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILLDLG_CB_ID_GET_ANIMATION, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 052703. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::SetCallbackStartSkillAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILLDLG_CB_ID_START_SKILL_ANIMATION, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 041203. BOB
******************************************************************************/
BOOL AgcmSkillDlg::SetCallbackStartSkillEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILLDLG_CB_ID_START_SKILL_EFFECT, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::SetCallbackAddAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILLDLG_CB_ID_ADD_ANIMATION, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::SetCallbackRemoveAllAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILLDLG_CB_ID_REMOVE_ALL_ANIMATION, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::SetCallbackStartAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILLDLG_CB_ID_START_ANIM, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::SetCallbackReadRtAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMSKILLDLG_CB_ID_READ_RT_ANIM, pfCallback, pClass);
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
/*BOOL AgcmSkillDlg::EnumCallbackGetAnimation(AgcmSkillDlgGetAnimationSet *pcsSet)
{
	return EnumCallback(AGCMSKILLDLG_CB_ID_GET_ANIMATION, (PVOID)(pcsSet), NULL);
}*/

/******************************************************************************
* Purpose :
*
* 052703. Bob Jung
******************************************************************************/
/*BOOL AgcmSkillDlg::EnumCallbackStartSkillAnimation(AgcmSkillDlgStartAnimationSet *pcsSet)
{
	return EnumCallback(AGCMSKILLDLG_CB_ID_START_SKILL_ANIMATION, (PVOID)(pcsSet), NULL);
}*/

/******************************************************************************
* Purpose :
*
* 041203. BOB
******************************************************************************/
BOOL AgcmSkillDlg::EnumCallbackStartSkillEffect(INT32 lSkillIndex, INT32 lWeaponType)
{
	return EnumCallback(AGCMSKILLDLG_CB_ID_START_SKILL_EFFECT, (PVOID)(&lSkillIndex), (PVOID)(&lWeaponType));
}

/******************************************************************************
* Purpose :
*
* 160604. BOB
******************************************************************************/
BOOL AgcmSkillDlg::InitializeCharacterSkillDlg(
	AgcdCharacterTemplate *pcsAgcdCharacterTemplate,
	AgpdSkillTemplateAttachData *pcsAgpdSkillTemplateAttachData,
	AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData		)
{
	if (!g_pcsAgcmCharSkillDlg)
		return FALSE;

	return g_pcsAgcmCharSkillDlg->InitializeDlg(
		pcsAgcdCharacterTemplate,
		pcsAgpdSkillTemplateAttachData,
		pcsAgcdSkillAttachTemplateData				);
}

/******************************************************************************
* Purpose :
*
* 160604. BOB
******************************************************************************/
BOOL AgcmSkillDlg::IsOpenCharacterSkillDlg()
{
	return (g_pcsAgcmCharSkillDlg) ? (TRUE) : (FALSE);
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::OpenCharacterSkillDlg(
	AgcdCharacterTemplate *pcsAgcdCharacterTemplate,
	AgpdSkillTemplateAttachData *pcsAgpdSkillData,
	AgcdSkillAttachTemplateData *pcsAgcdSkillData,
	CHAR *szFindAnimPathName1,
	CHAR *szFindAnimPathName2							)
{
/*	if(!g_pcsAgcmCharacterSkillDlg)
	{
		g_pcsAgcmCharacterSkillDlg = new AgcmCharacterSkillDlg(pcsAgpdSkillData, pcsAgcdSkillData, szFindAnimPathName1, szFindAnimPathName2);
		g_pcsAgcmCharacterSkillDlg->Create();
		g_pcsAgcmCharacterSkillDlg->ShowWindow(SW_SHOW);
	}*/

	if (!g_pcsAgcmCharSkillDlg)
	{
		if (m_pcsAgcmEffectDlg)
			m_pcsAgcmEffectDlg->CloseAllEffectDlg();

		g_pcsAgcmCharSkillDlg = new AgcmCharSkillDlg(
			pcsAgcdCharacterTemplate,
			pcsAgpdSkillData,
			pcsAgcdSkillData,
			szFindAnimPathName1,
			szFindAnimPathName2							);
		g_pcsAgcmCharSkillDlg->Create();
		g_pcsAgcmCharSkillDlg->ShowWindow(SW_SHOW);
	}

//	AgcmCharSkillDlg dlg(pcsAgpdSkillData, pcsAgcdSkillData);
//	dlg.DoModal();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 052703. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::CloseCharacterSkillDlg()
{
/*	if(g_pcsAgcmCharacterSkillDlg)
	{
		g_pcsAgcmCharacterSkillDlg->ShowWindow(SW_HIDE);
		g_pcsAgcmCharacterSkillDlg->DestroyWindow();

		delete g_pcsAgcmCharacterSkillDlg;
		g_pcsAgcmCharacterSkillDlg = NULL;
	}*/

	if (g_pcsAgcmCharSkillDlg)
	{
		if (m_pcsAgcmEffectDlg)
			m_pcsAgcmEffectDlg->CloseAllEffectDlg();

		g_pcsAgcmCharSkillDlg->ShowWindow(SW_HIDE);
		g_pcsAgcmCharSkillDlg->DestroyWindow();

		delete g_pcsAgcmCharSkillDlg;
		g_pcsAgcmCharSkillDlg = NULL;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 052603. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::OpenSkillTemplateList(CHAR **ppszDest)
{
//	CHAR szTemp[256];

	CHAR	*pszTemp = NULL;
	AgcmSkillTemplateListDlg dlg(&pszTemp);
	if(dlg.DoModal() == IDOK)
	{
//		strcpy(pszDest, szTemp);
		*(ppszDest) = pszTemp;
		return TRUE;
	}

	return FALSE;
}

/******************************************************************************
* Purpose :
*
* 092403. BOB
******************************************************************************/
INT32 AgcmSkillDlg::OpenSkillTemplateList()
{
	INT32	lTID;

	AgcmSkillTemplateListDlg dlg(&lTID);
	if(dlg.DoModal() == IDOK)
	{
		return lTID;
	}

	return -1;
}

/******************************************************************************
* Purpose :
*
* 062203. Bob Jung
******************************************************************************/
BOOL AgcmSkillDlg::OpenSkillTemplateDlg(AgpdSkillTemplate *pcsAgpdSkillTemplate, AgcdSkillTemplate *pcsAgcdSkillTemplate)
{
	AgcmSkillTemplateDlg dlg(pcsAgpdSkillTemplate, pcsAgcdSkillTemplate);
	if(dlg.DoModal() == IDOK)
		return TRUE;

	return FALSE;
}

/******************************************************************************
* Purpose :
*
* 070203. Bob Jung
******************************************************************************/
VOID AgcmSkillDlg::SetTexturePathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2, CHAR *szExt3)
{
	sprintf(m_szTexturePathName1, "%s*.%s", szPath, szExt1);
	sprintf(m_szTexturePathName2, "%s*.%s", szPath, szExt2);
	sprintf(m_szTexturePathName3, "%s*.%s", szPath, szExt3);
}

/******************************************************************************
* Purpose :
*
* 070203. Bob Jung
******************************************************************************/
VOID AgcmSkillDlg::SetTextureFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2, CHAR *szFindPathName3)
{
	strcpy(m_szTexturePathName1, szFindPathName1);
	strcpy(m_szTexturePathName2, szFindPathName2);
	strcpy(m_szTexturePathName3, szFindPathName3);
}

/******************************************************************************
* Purpose :
*
* 070203. Bob Jung
******************************************************************************/
CHAR *AgcmSkillDlg::GetTexturePathName1()
{
	return m_szTexturePathName1;
}

/******************************************************************************
* Purpose :
*
* 070203. Bob Jung
******************************************************************************/
CHAR *AgcmSkillDlg::GetTexturePathName2()
{
	return m_szTexturePathName2;
}

/******************************************************************************
* Purpose :
*
* 070203. Bob Jung
******************************************************************************/
CHAR *AgcmSkillDlg::GetTexturePathName3()
{
	return m_szTexturePathName3;
}

/******************************************************************************
* Purpose :
*
* 030304 BOB
******************************************************************************/
BOOL AgcmSkillDlg::UpdateSaveEffectCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (g_pcsAgcmCharSkillDlg)
		g_pcsAgcmCharSkillDlg->UpdateSkillControl(TRUE);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 060304 BOB
******************************************************************************/
//BOOL AgcmSkillDlg::UpdateEffectConditionCB(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (g_pcsAgcmCharSkillDlg)
//	{
//		g_pcsAgcmCharSkillDlg->UpdateScriptDlg();
//		g_pcsAgcmCharSkillDlg->UpdateSearchCondition();
//	}

//	return TRUE;
//}

/******************************************************************************
* Purpose :
*
* 060304 BOB
******************************************************************************/
//BOOL AgcmSkillDlg::UpdateEffectFlagsCB(PVOID pData, PVOID pClass, PVOID pCustData)
//{
//	if (g_pcsAgcmCharSkillDlg)
//		g_pcsAgcmCharSkillDlg->UpdateScriptDlg();

//	return TRUE;
//}

/******************************************************************************
* Purpose :
*
* 060304 BOB
******************************************************************************/
BOOL AgcmSkillDlg::OpenEffectDlgCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmSkillDlg	*pThis	= (AgcmSkillDlg *)(pClass);
	if (!pThis)
		return FALSE;

	pThis->CloseCharacterSkillDlg();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::AddAnimation(AAD_AddAnimationParams *pcsParams)
{
	return EnumCallback(AGCMSKILLDLG_CB_ID_ADD_ANIMATION, (PVOID)(pcsParams), NULL);
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::RemoveAllAnimation(AAD_RemoveAllAnimationParams *pcsParams)
{
	return EnumCallback(AGCMSKILLDLG_CB_ID_REMOVE_ALL_ANIMATION, (PVOID)(pcsParams), NULL);
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::StartAnim(AAD_StartAnimationParams *pcsParams)
{
	return EnumCallback(AGCMSKILLDLG_CB_ID_START_ANIM, (PVOID)(pcsParams), NULL);
}

/******************************************************************************
* Purpose :
*
* 250205. BOB
******************************************************************************/
BOOL AgcmSkillDlg::ReadRtAnim(AAD_ReadRtAnimParams *pcsParams)
{
	return EnumCallback(AGCMSKILLDLG_CB_ID_READ_RT_ANIM, (PVOID)(pcsParams), NULL);
}

/******************************************************************************
******************************************************************************/