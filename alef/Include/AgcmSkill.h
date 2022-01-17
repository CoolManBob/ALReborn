/******************************************************************************
Module:  AgcmSkill.h
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 25
******************************************************************************/

#if !defined(__AGCMSKILL_H__)
#define __AGCMSKILL_H__

#include "AgpmCharacter.h"
#include "AgpmSkill.h"
#include "AgpmGrid.h"
#include "AgpmPvP.h"
#include "ApmEventManager.h"

#include "AgcModule.h"

#include "AgcmCharacter.h"

#include "AgcdSkill.h"
#include "AgpmFactors.h"

#include "AgcaEffectData.h"

#include "AgcmUIControl.h"

//#include "AgcdEffect.h"
#include "AgcmResourceLoader.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmSkillD" )
#else
#pragma comment ( lib , "AgcmSkill" )
#endif
#endif

enum eAgcmSkillDataType {
	AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL			= 0,
	AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL_SOUND,
};

enum AgcmSkillCallbackPoint
{
	AGCMSKILL_CB_ID_NO_ANIM_SKILL = 0,
	AGCMSKILL_CB_ID_MISS_CAST_SKILL,
//	AGCMSKILL_CB_ID_TARGET_SKILL,
	AGCMSKILL_CB_ID_CHECK_CAST_SKILL_RESULT,
//	AGCMSKILL_CB_ID_MASTERY_UPDATE,
//	AGCMSKILL_CB_ID_MASTERY_ROLLBACK_SUCCESS,
//	AGCMSKILL_CB_ID_MASTERY_ROLLBACK_FAILED,
	AGCMSKILL_CB_ID_ADD_BUFFED_SKILL_LIST,
	AGCMSKILL_CB_ID_REMOVE_BUFFED_SKILL_LIST,
	AGCMSKILL_CB_ID_ADDITIONAL_HITEFFECT,
	AGCMSKILL_CB_ID_ADD_HPBAR,
	AGCMSKILL_CB_ID_CAST_FAIL_FOR_TRANSFORM,
	AGCMSKILL_CB_ID_PRE_PROCESS_SKILL,
	AGCMSKILL_CB_ID_GET_REGISTERED_MP_POTION_TID,
	AGCMSKILL_CB_ID_START_CAST_SELF_CHARACTER,
	AGCMSKILL_CB_ID_BLOCK_CAST_SKILL,
	AGCMSKILL_CB_ID_CAST_FAIL_FOR_RIDE,
	AGCMSKILL_CB_ID_START_GROUND_TARGET,
	AGCMSKILL_CB_ID_NUM
};

#define AGCMSKILL_INI_NAME_SKILL_INFO				"SKILL_INFO"
#define AGCMSKILL_INI_NAME_ANIMATION_NAME			"SKILL_ANIMATION_NAME"
#define AGCMSKILL_INI_NAME_ANIMATION_FLAGS			"SKILL_ANIMATION_FLAGS"
#define AGCMSKILL_INI_NAME_ANIMATION_ACTIVE_RATE	"SKILL_ANIMATION_ACTIVE_RATE"
#define AGCMSKILL_INI_NAME_ANIMATION_CUST_TYPE		"SKILL_ANIMATION_CUST_TYPE"
#define AGCMSKILL_INI_NAME_ANIMATION_POINT			"SKILL_ANIMATION_POINT"
#define	AGCMSKILL_INI_NAME_EFFECT_DATA				"SKILL_EFFECT_DATA"
#define	AGCMSKILL_INI_NAME_EFFECT_SOUND				"SKILL_EFFECT_SOUND"
#define AGCMSKILL_INI_NAME_EFFECT_CUST_DATA			"SKILL_EFFECT_CUST_DATA"
#define AGCMSKILL_INI_NAME_EFFECT_CONDITION			"SKILL_EFFECT_CONDITION"
#define AGCMSKILL_INI_NAME_EFFECT_SS_CONDITION		"SKILL_EFFECT_SS_CONDITION"
#define AGCMSKILL_INI_NAME_EFFECT_STATUS			"SKILL_EFFECT_STATUS"
#define	AGCMSKILL_INI_NAME_SKILL_TEXTURE			"SKILL_TEXTURE"
#define	AGCMSKILL_INI_NAME_SMALL_SKILL_TEXTURE		"SMALL_SKILL_TEXTURE"
#define AGCMSKILL_INI_NAME_UNABLE_SKILL_TEXTURE		"UNABLE_SKILL_TEXTURE"

#define AGCMSKILL_INI_NAME_SPECIALIZE_TEXTURE		"SPECIALIZE_TEXTURE"

//@{ 2006/05/04 burumal
#define AGCMSKILL_INI_NAME_DNF_1					"DID_NOT_FINISH_KOREA"
#define AGCMSKILL_INI_NAME_DNF_2					"DID_NOT_FINISH_CHINA"
#define AGCMSKILL_INI_NAME_DNF_3					"DID_NOT_FINISH_WESTERN"
#define AGCMSKILL_INI_NAME_DNF_4					"DID_NOT_FINISH_JAPAN"
//@}

#define AGCMSKILL_PATH_LENGTH						128

const int	AGCMSKILL_MAX_TEMP_CREATED_SKILL	= 64;

const int	AGCMSKILL_INTERVAL_TIME_FOR_ADD_SP	= 600;

class AgcmSkill : public AgcModule {
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmSkill			*m_pcsAgpmSkill;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmPvP				*m_pcsAgpmPvP;

	ApmEventManager		*m_pcsApmEventManager;

	AgcmCharacter		*m_pcsAgcmCharacter;
	AgpmFactors			*m_pcsAgpmFactors;

	AgcmUIControl		*m_pcsAgcmUIControl;

	AgcmResourceLoader	*m_pcsAgcmResourceLoader;

	AgpmOptimizedPacket2	*m_pcsAgpmOptimizedPacket2;

//	AgcaEffectData		m_csEffectDataAdmin;
	AgcaEffectData2		m_csEffectDataAdmin2;

	INT16				m_nIndexADCharacter;
	INT16				m_nIndexADCharacterTemplate;
	INT16				m_nIndexADSkillTemplate;
	INT16				m_nIndexADSpecialize;
	INT16				m_nIndexADSkill;

	ApBase				m_csSelectedBase;
	AuPOS				m_posSelectedPos;

	INT32				m_lTempSkillID;
	INT32				m_lTempCreatedSkillID[AGCMSKILL_MAX_TEMP_CREATED_SKILL];

	BOOL				m_bIsProcessUpdateMastery;
	UINT32				m_ulStartProcessUpdateMasteryTime;

	UINT32				m_ulNextSendAddSPTime;

	CHAR				m_szIconTexturePath[AGCMSKILL_PATH_LENGTH];

	BOOL				m_bFollowCastLock;
	INT32				m_lFollowCastTargetID;
	INT32				m_lFollowCastSkillID;

	INT32				m_lGroundTargetSkillID;

	stAgpmSkillActionData	m_stQueuedSkillActionData;

	//@{ kday 20050822
	// ;)
	BOOL				m_bDbgSkill;

public:
	void				DebugSkill_On()		{ m_bDbgSkill = TRUE; };
	void				DebugSkill_Off()	{ m_bDbgSkill = FALSE; };
	void				DebugSkill_Render();
	//@} kday


public:
	// 2003_05_12 Seong Yon-jun
//	AcUISkill			m_clSkill;

	eAcReturnType		CheckVisualInfo(AgcdCharacterTemplate *pstAgcdCharacterTemplate, AgcdSkillAttachTemplateData *pcsAttachedData, INT32 lVisualIndex);

private:
	/*BOOL				SetSkillAnimation(INT32 lTID, pstAgcmSkillVisualInfo pstInfo, CHAR *szData);
	BOOL				SetSkillEffectData(pstAgcmSkillVisualInfo pstInfo, CHAR *szData);
	BOOL				SetSkillEffectSound(pstAgcmSkillVisualInfo pstInfo, CHAR *szData);*/
	//BOOL				SetSkillAnimation(INT32 lTID, AgcmSkillVisualInfo *pstInfo, CHAR *szData);
	eAcReturnType		SetSkillEffectData(AgcdCharacterTemplate *pstAgcdCharacterTemplate, AgcdSkillAttachTemplateData *pcsAttachedData, CHAR *szData);
//	BOOL				SetSkillEffectSound(AgcmSkillVisualInfo *pstInfo, CHAR *szData);
//	BOOL				SetLevelInfo(pstAgcmSkillVisualInfo pstInfo, CHAR *szData);

	BOOL				AttachedTemplateAnimationWrite(	ApModuleStream *pStream, AgcmSkillVisualInfo *pcsInfo, INT32 lVisualInfoIndex, INT32 lAnimType2	);
	BOOL				AttachedTemplateEffectWrite( ApModuleStream *pStream, AgcmSkillVisualInfo *pcsInfo,	INT32 lVisualInfoIndex,	INT32 lAnimType2 );
	BOOL				AttachedTemplateEffectSoundWrite( ApModuleStream *pStream, AgcmSkillVisualInfo *pcsInfo, INT32 lVisualInfoIndex, INT32 lAnimType2 );

public:
	AgcmSkill();
	virtual ~AgcmSkill();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	// 초기화시 불러준다 (092403, BOB)
//	BOOL				SetMaxEffectNum(INT32 lMax);

	BOOL				SetSelectedBase(INT32 lType, INT32 lID, AuPOS *pcsPos);
	BOOL				SetSelectedPos(AuPOS *pcsPos);

	BOOL				ParseTemplateString(CHAR *szString, CHAR *szFrontString, INT16 nFrontLength, CHAR *szBackString, INT16 nBackLength);
	INT16				GetUsableSkillTNameIndex(ApBase *pcsBase, CHAR *szTemplateName);
	INT16				GetUsableSkillTNameIndex(ApBase *pcsBase, INT32 lTID);

	static BOOL			CBReceiveAction(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBActionSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBActionSkillResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBCheckNowUpdateActionFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ProcessSkillEffectIdleEvent(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);

	UINT32				GetMaxSkillPreserveTime(INT32 lSkillTID, INT32 lSkillLevel);

	BOOL				ProcessStartEffect(INT32 lOwner, INT32 lTarget, BOOL bIsMissile);
	BOOL				ProcessEndEffect(INT32 lOwner, INT32 lTarget, BOOL bIsMissile, INT32 lSkillTID);

	BOOL				SkillTest(INT32 lSTID, INT32 lTCID, INT32 lLID);
	BOOL				EnumCallbackNoAnimSkill(PVOID pData, PVOID pCustData);

	BOOL				CastSkillTest(INT32 lSkillNum, INT32 lTargetCID, BOOL bForceAttack); 
	BOOL				CastSkillTest(INT32 lSkillNum, AuPOS *pposTarget, BOOL bForceAttack);

	BOOL				PreProcessSkill(INT32 lSkillID, INT32 lTargetCID, BOOL bForceAttack);

	BOOL				CastSkill(INT32 lTargetCID, BOOL bForceAttack); 
	BOOL				CastSkill(ApBase *pcsTargetBase, BOOL bForceAttack); 
	BOOL				CastSkill(AuPOS *pposTarget, BOOL bForceAttack);
	BOOL				CastSkill(ApBase *pcsTargetBase, AuPOS *pposTarget, BOOL bForceAttack);

	BOOL				CastSkill(INT32 lSkillID, INT32 lTargetCID, BOOL bForceAttack);
	BOOL				CastSkill(INT32 lSkillID, ApBase *pcsTargetBase, BOOL bForceAttack);
	BOOL				CastSkill(INT32 lSkillID, AuPOS *pposTarget, BOOL bForceAttack);
	BOOL				CastSkill(INT32 lSkillID, ApBase *pcsTargetBase, AuPOS *pposTarget, BOOL bForceAttack);
	BOOL				CastSkillCheck(INT32 lSkillID, ApBase *pcsTargetBase, AuPOS *pposTarget, BOOL bForceAttack);
			// 스킬이 사용 가능한지 확인.
			// return TRUE 면 바로 사용 가능.

	BOOL				UpdateSkillPoint(INT32 lSkillID, INT32 lPoint);
	BOOL				UpdateSkillPoint(AgpdSkill *pcsSkill, INT32 lPoint);

	BOOL				UpdateDIRTPoint(INT32 lSkillID, INT32 lDuration, INT32 lIntensity, INT32 lRange, INT32 lTarget);
	BOOL				UpdateDIRTPoint(AgpdSkill *pcsSkill, INT32 lDuration, INT32 lIntensity, INT32 lRange, INT32 lTarget);

	BOOL				ProcessReceiveCastSkill(pstAgpmSkillActionData pstActionData);

	BOOL				SendRequestRollback(INT32 lSkillID);

	static BOOL			ConAgcdSkillAttachData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgcdSkillAttachData(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConTemplateAttachData(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesTemplateAttachData(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConAgcdSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgcdSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	//stream

	BOOL				StreamReadTemplateSkill( CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE );
	BOOL				StreamReadTemplateSkillSound( CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE );

	BOOL				StreamWriteTemplateSkill(CHAR *szFile, BOOL bEncryption);
	BOOL				StreamWriteTemplateSkillSound(CHAR *szFile, BOOL bEncryption);

	static BOOL			AttachedTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			AttachedTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			AttachedSoundTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			AttachedSoundTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	
	static BOOL			ConAgcdSkillTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgcdSkillTemplate(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConAgcdSkillSpecialize(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgcdSkillSpecialize(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			AgcdSkillTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			AgcdSkillTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL			AgcdSkillSpecializeReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			AgcdSkillSpecializeWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL			CBSkillStatusUpdate( PVOID pData , PVOID pClass , PVOID pCustData );

	AgcdSkillADChar					*GetCharacterAttachData(ApBase *pcsBase);
	AgpdCharacterTemplate			*GetCharacterTemplate(AgcdSkillAttachTemplateData *pcsAgcdSkillAttachTemplateData);
	AgcdSkillAttachTemplateData		*GetAttachTemplateData(ApBase *pcsBase);
	AgcdSkillTemplate				*GetADSkillTemplate(PVOID pvData);
	AgcdSkillSpecializeTemplate		*GetADSpecializeTemplate(PVOID pvData);
	
	AgcdSkillTemplate				*GetAgcdSkillTemplate( INT32 lSkillID );

	AgcdSkill*						GetAgcdSkill(AgpdSkill* pcsSkill);

	INT32							GetNumVisualInfo(AgcdCharacterTemplate *pstAgcdCharacterTemplate);

	// Bob님 작업~(061503)
	BOOL							SetCallbackNoAnimSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL							SetCallbackTargetSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL							SetCallbackMissCastSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL							SetCallbackCheckCastSkillResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

//	BOOL							SetCallbackMasteryUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//
//	BOOL							SetCallbackMasteryRollbackSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL							SetCallbackMasteryRollbackFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL							SetCallbackAddBuffedSkillList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL							SetCallbackRemoveBuffedSkillList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL							SetCallbackAddHpBar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL							SetCallbackAddtionalHitEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL							SetCallbackCastFailForTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL							SetCallbackCastFailForRide(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL							SetCallbackPreProcessSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL							SetCallbackGetRegisteredMPPotionTID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL							SetCallbackBlockCastSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL							SetCallbackStartGroundTargetSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//AgcdAnimation					*GetAnimation(INT32 lTID, INT32 lDataIndex, INT32 lEquipType, BOOL bAdd);

	AgcmCharacter					*GetAgcmCharacter() { return m_pcsAgcmCharacter; }

	AgcaEffectData2&				GetEffectAdmin2()	{ return m_csEffectDataAdmin2; }
	
	// Seong Yon jun - UI 관련 작업 
//	void				SetSkillUIInfo()							;
//	void				SetSkillPoint()								;		// 남은 Skill Point를 써준다. 
//	void				SetDIRTWindow( INT32 lSkillID )				;		// DIRT Window를 Set하고 Open한다. 
	RwRaster*			GetSkillRaster( INT32 lSkillID )			;
	RwRaster*			GetSmallSkillRaster( INT32 lSkillID )		;
	RwTexture*			GetSkillTexture( INT32 lSkillID )			;
	RwTexture*			GetSmallSkillTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate )		;
	RwTexture*			GetBuffSkillSmallTexture( AgpdCharacter* pdCharacter, INT32 nNum, BOOL bBattleGround = FALSE );

	INT32				GetSkillTotalPoint( INT32 lSkillID	)		;
	INT32				GetSkillFactor( INT32 lSkillID, AgpdFactorDIRTType eDIRT );

	BOOL				SetGridSkillAttachedTexture( AgpdSkill* pcsAgpdSkill );
	BOOL				SetGridSkillAttachedUnableTexture( AgpdSkill* pcsAgpdSkill );
	BOOL				SetGridSkillAttachedUnableTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate );
	BOOL				SetGridSkillAttachedTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate );
	BOOL				SetGridSpecializeAttachedTexture( AgpdSkillSpecializeTemplate *pcsSpecialize );

	BOOL				SetGridSkillAttachedSmallTexture( AgpdSkillTemplate* pcsAgpdSkillTemplate );

//	static BOOL			CBUpdateSkillUIInfo(PVOID pData, PVOID pClass, PVOID pCustData)						;
//	static BOOL			CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData)						;
//	static BOOL			CSUpdateSkillDIRT(PVOID pData, PVOID pClass, PVOID pCustData)						;

	// skill mastery functions
//	BOOL				SetRollbackMastery();
//	BOOL				ResetRollbackMastery();
//
//	BOOL				IsUpdateMastery();
//	BOOL				SetUpdateMasteryFlag();
//	BOOL				ResetUpdateMasteryFlag();
//
//	BOOL				StartUpdateMastery(BOOL bRollback = FALSE);
//	BOOL				StopUpdateMastery(BOOL bOK = TRUE);

//	BOOL				RefreshNeedSkillPoint(ApBase *pcsBase);

//	BOOL				RefreshSkillPoint(ApBase *pcsBase);

//	BOOL				SetBaseSPForActive(AgpdSkillTemplate *pcsSkillTemplate, INT32 lSP);
//
//	//static BOOL			CBMasteryResult(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasteryUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
//#ifdef	__NEW_MASTERY__
//	static BOOL			CBMasteryNodeUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
//#endif	//__NEW_MASTERY__
//	static BOOL			CBMasteryChangeResult(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasterySpecialize(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			CBMasterySpecializeResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBInitSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBInitSkillTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBForgetSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBInitSpecialize(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBAddBuffedSkillList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveBuffedSkillList(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBUpdateSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBLoadCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBReleaseCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBMoveActionSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBMoveActionRelease(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBStopSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBActionProductSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBAdditionalEffect(PVOID pData, PVOID pClass, PVOID pCustData);

//	static BOOL			CBMasteryRollbackResult(PVOID pData, PVOID pClass, PVOID pCustData);
//
//#ifdef	__NEW_MASTERY__
//	static BOOL			CBAddSPToMasteryResult(PVOID pData, PVOID pClass, PVOID pCustData);
//#endif	//__NEW_MASTERY__

	/*
	BOOL				SendMasterySkillSelect(INT32 lMasteryIndex, CHAR *szSkillName);
	BOOL				SendAddSPToMastery(INT32 lMasteryIndex);
	BOOL				SendSubSPFromMastery(INT32 lMasteryIndex);
	*/

//	BOOL				SendChangeMastery();
//	BOOL				SendMasteryRollback();

//#ifdef	__NEW_MASTERY__
//	BOOL				SendAddSPToMastery(INT32 lMastery, INT32 *plSelectSkillTID, INT32 lArraySize);
//#endif	//__NEW_MASTERY__

	static BOOL			ProcessNoticeEffect(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pvClass);

	BOOL				IsExistCastQueue(ApBase *pcsBase, INT32 lCasterID);
	BOOL				AddCastQueue(ApBase *pcsBase, INT32 lCasterID);
	BOOL				RemoveCastQueue(ApBase *pcsBase, INT32 lCasterID);

	static BOOL			CBSkillStruckAnimation(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBProcessSkillStruck(PVOID pData, PVOID pClass, PVOID pCustData);

	VOID				SetIconTexturePath(CHAR *szPath);

	static BOOL			CBReservedActionSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				SetCallbackStartCastSelfCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				AddIdleEventTwicePacketSkill(ApBase* pcsOwner, AgpdSkill* pcsSkill, UINT32 ulClock);
	static BOOL			CBProcessTwicePacketSkill(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);
	FLOAT				GetTwicePacketInteval(AgpdSkill* pcsSkill, ApBase* pcsOwner, FLOAT fAnimationDuration);

	BOOL				SetCurrentSkill(AgpdCharacter *pcsCharacter, INT32 lSkillTID);
	BOOL				SetCurrentSkillUsableIndex(AgpdCharacter *pcsCharacter, INT32 lUsableIndex);

	void				SetGroundTargetSkillID(INT32 lSkillID) { m_lGroundTargetSkillID = lSkillID; }
	INT32				GetGroundTargetSkillID() { return m_lGroundTargetSkillID; }
	BOOL				CastGroundTargetSkill(AuPOS stPos, BOOL bForce = FALSE);

	// 2007.02.13. steeple
	BOOL				ProcessAffectedTarget(AgpdSkill* pcsSkill, stAgcmEffectNoticeEffectProcessData& stProcessData);

	// For Console
	// 2007.02.13. steeple
	BOOL				m_bShowAffectedTargetCount;
	void				ToggleAffectedTargetCount() { m_bShowAffectedTargetCount = !m_bShowAffectedTargetCount; }
	void				ShowAffectedTargetCount(AgcdSkill* pcsAgcdSkill);

private:
	BOOL				_CheckVisualInfo	( AgpdCharacterTemplate* pcsCharacterTemplate , INT32 nIndex );
};

#endif	//__AGCMSKILL_H__
