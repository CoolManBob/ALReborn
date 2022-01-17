#pragma once

#include "ApModule.h"
#include "AgcmEventEffect.h"

#define AGCMEFFECTDLG_CONDITION_SET_MAX			20
#define AGCMEFFECTDLG_CONDITION_ITEM_NUM		"NUM"
#define AGCMEFFECTDLG_CONDITION_ITEM_INDEX		"ITM"

enum AgcmEffectDlgStreamType
{
	AGCM_EFFECTDLG_STREAM_TYPE_CONDITION = 0,
	AGCM_EFFECTDLG_STREAM_TYPE_NUM
};

enum AgcmEffectDlgCallbackPoint
{
	AGCM_EFFECTDLG_CB_ID_SAVE_SCRIPT = 0,
	AGCM_EFFECTDLG_CB_ID_OPEN_EFFECT_DLG,
	AGCM_EFFECTDLG_CB_ID_NUM
};

struct AgcmEffectDlgConditionSet
{
	INT16	m_nItemNum;
	CHAR	m_aszCondition[AGCMEFFECTDLG_CONDITION_SET_MAX][256];
};

class AFX_EXT_CLASS AgcmEffectDlg : public ApModule
{
public:
	AgcmEffectDlg();
	virtual ~AgcmEffectDlg();

	static AgcmEffectDlg *GetInstance();

protected:
	AgcmEventEffect		*m_pcsAgcmEventEffect;

public:
	AgcmEffectDlgConditionSet m_stCondition;

public:
	BOOL	OpenEffectCondition(AgcdUseEffectSet *pcsSet, INT16 nIndex);
	BOOL	OpenEffectStatus(AgcdUseEffectSet *pcsSet, INT16 nIndex);

	VOID	OpenUseEffectSet(AgcdUseEffectSet *pstAgcdUseEffectSet/*, INT16 nMaxEffectNum = D_AGCD_USE_EFFECT_SET_DATA_MAX_NUM*/);
	VOID	OpenEffectList(UINT32 *pulEID);
	VOID	OpenEffectHelp();
	UINT32	OpenEffectList();
	BOOL	OpenEffectScript();
	BOOL	OpenEffectConditionList(UINT32 *plCondition, UINT32 *plSSCondition);
	BOOL	OpenEffectStatusList(UINT32 *plStatus);
	BOOL	OpenEffectRotationDlg(AgcdUseEffectSetDataRotation **ppstRotation);

	BOOL	InitializeUseEffectSet(AgcdUseEffectSet *pstAgcdUseEffectSet);

//	BOOL	IsOpenUseEffectSet() {return m_bOpenUseEffectSet;}
	BOOL	IsOpenUseEffectSet();
	BOOL	IsOpenScriptDlg();

	/* 내부적으로 쓰고 있는 것들인께 건들지 마쇼~ */
//	VOID	SetEventEffect(AgcdEventEffect *pstEventEffect);
	AgcmEventEffect *GetAgcmEventEffect() {return m_pcsAgcmEventEffect;}

	VOID	CloseAllEffectDlg(BOOL bCloseMainEffectDlg = TRUE);
	VOID	CloseUseEffectSet();
	VOID	CloseEffectScript();
	VOID	CloseEffectConditionList();
	VOID	CloseEffectStatusList();
	VOID	CloseEffectList();

	VOID	EnableEffectFlagsDlg(BOOL bEnable = TRUE);
	VOID	UpdateEffectFlagsDlg(AgcdUseEffectSetData *pstData);
	VOID	UpdateEffectConditionList(UINT32 *plCondition, UINT32 *plSSCondition);
	VOID	UpdateEffectStatusList(UINT32 *plStatus);

	VOID	UpdateEffectScript(AgcdUseEffectSet *pstSet = NULL, INT32 lIndex = -1);

	BOOL	SetCallbackSaveEffectScript(ApModuleDefaultCallBack pfCallback, PVOID pClass);
/*	BOOL	SetCallbackSaveCondition(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSaveStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSaveSSCondition(ApModuleDefaultCallBack pfCallback, PVOID pClass);*/
	BOOL	SetCallbackOpenEffectDlg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	_EnumCallback(UINT32 ulPoint, PVOID pData, PVOID pCustData);

public:
	BOOL	OnInit();
	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

	// Callback
	BOOL EventEffectConditionStreamRead(CHAR *szFile);
	BOOL EventEffectConditionStreamWrite(CHAR *szFile);

	static BOOL EventEffectConditionStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL EventEffectConditionStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
};
