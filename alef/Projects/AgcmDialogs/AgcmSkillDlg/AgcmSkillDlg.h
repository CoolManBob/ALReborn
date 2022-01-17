#pragma once

#include "ApModule.h"
#include "AgcmSkill.h"
#include "AgcmAnimationDlg.h"
#include "AgcmEffectDlg.h"

enum eAgcmSkillDlgCallbackPoint
{
	AGCMSKILLDLG_CB_ID_GET_ANIMATION = 0,
	AGCMSKILLDLG_CB_ID_START_SKILL_ANIMATION,
	AGCMSKILLDLG_CB_ID_START_SKILL_EFFECT,
	AGCMSKILLDLG_CB_ID_ADD_ANIMATION,
	AGCMSKILLDLG_CB_ID_REMOVE_ALL_ANIMATION,
	AGCMSKILLDLG_CB_ID_START_ANIM,
	AGCMSKILLDLG_CB_ID_READ_RT_ANIM,
	AGCMSKILLDLG_CB_ID_NUM
};

class AFX_EXT_CLASS AgcmSkillDlg : public ApModule
{
public:
	AgcmSkillDlg();
	virtual ~AgcmSkillDlg();

public:
	static AgcmSkillDlg *GetInstance();

	BOOL				OnInit();
	BOOL				OnAddModule();
	BOOL				OnIdle(UINT32 ulClockCount);
	BOOL				OnDestroy();

	BOOL				EnumCallbackStartSkillEffect(INT32 lSkillIndex, INT32 lWeaponType);

	AgpmSkill			*GetAgpmSkill()			{	return m_pcsAgpmSkill;			}
	AgcmSkill			*GetAgcmSkill()			{	return m_pcsAgcmSkill;			}
	AgcmAnimationDlg	*GetAgcmAnimationDlg()	{	return m_pcsAgcmAnimationDlg;	}
	AgcmEffectDlg		*GetAgcmEffectDlg()		{	return m_pcsAgcmEffectDlg;		}
	AgcmItem			*GetAgcmItem()			{	return m_pcsAgcmItem;			}
	AgcmEventEffect		*GetAgcmEventEffect()	{	return m_pcsAgcmEventEffect;	}
	AgcmCharacter		*GetAgcmCharacter()		{	return m_pcsAgcmCharacter;		}

	CHAR			*GetTexturePathName1();
	CHAR			*GetTexturePathName2();
	CHAR			*GetTexturePathName3();

protected:
	AgcmCharacter		*m_pcsAgcmCharacter;
	AgpmSkill			*m_pcsAgpmSkill;
	AgcmSkill			*m_pcsAgcmSkill;
	AgcmItem			*m_pcsAgcmItem;
	AgcmAnimationDlg	*m_pcsAgcmAnimationDlg;
	AgcmEffectDlg		*m_pcsAgcmEffectDlg;
	AgcmEventEffect		*m_pcsAgcmEventEffect;

	CHAR		m_szTexturePathName1[256];
	CHAR		m_szTexturePathName2[256];
	CHAR		m_szTexturePathName3[256];

public:
	BOOL	SetCallbackGetAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackStartSkillAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackStartSkillEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAddAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveAllAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackStartAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReadRtAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	AddAnimation(AAD_AddAnimationParams *pcsParams);
	BOOL	RemoveAllAnimation(AAD_RemoveAllAnimationParams *pcsParams);
	BOOL	StartAnim(AAD_StartAnimationParams *pcsParams);
	BOOL	ReadRtAnim(AAD_ReadRtAnimParams *pcsParams);

	VOID	SetTexturePathExt(CHAR *szPath, CHAR *szExt1, CHAR *szExt2, CHAR *szExt3);
	VOID	SetTextureFindPathName(CHAR *szFindPathName1, CHAR *szFindPathName2, CHAR *szFindPathName3);

	BOOL	OpenCharacterSkillDlg( AgcdCharacterTemplate *pcsAgcdCharacterTemplate, AgpdSkillTemplateAttachData *pcsAgpdSkillData, AgcdSkillAttachTemplateData *pcsAgcdSkillData, CHAR *szFindAnimPathName1, CHAR *szFindAnimPathName2 );
	BOOL	CloseCharacterSkillDlg();
	BOOL	IsOpenCharacterSkillDlg();
	BOOL	InitializeCharacterSkillDlg( AgcdCharacterTemplate *pcsAgcdCharacterTemplate, AgpdSkillTemplateAttachData *pcsAgpdSkillTemplateAttachData, AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData );

	BOOL	OpenSkillTemplateList(CHAR **ppszDest);
	INT32	OpenSkillTemplateList();

	BOOL	OpenSkillTemplateDlg(AgpdSkillTemplate *pcsAgpdSkillTemplate, AgcdSkillTemplate *pcsAgcdSkillTemplate);

	// Callback
	static BOOL UpdateSaveEffectCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL OpenEffectDlgCB(PVOID pData, PVOID pClass, PVOID pCustData);
};
