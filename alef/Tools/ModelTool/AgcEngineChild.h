#pragma once

#include "AcuFrameMemory.h"
#include "AgcmUIControl.h"
#include "ApBase.h"
#include "ApModule.h"
#include "ApModuleManager.h"
#include "AgcmMap.h"
#include "AgpmFactors.h"
#include "AgpmGrid.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmCharacter.h"
#include "AgpmArchlord.h"
#include "AgcmCharacter.h"
#include "AgpmBillInfo.h"
#include "AgpmItem.h"
#include "AgcmItem.h" 
#include "AgpmUnion.h"
#include "AgpmSkill.h"
#include "AgpmParty.h"
#include "AgcmParty.h"
#include "AgcmSkill.h"
#include "AgcmEff2.h"
#include "ApmEventManager.h"
#include "AgpmTimer.h"
#include "AgcmEventEffect.h"
#include "AgcmRender.h"
#include "AgcmPostFX.h"
#include "AgcmSound.h"
#include "AgcmLODManager.h"
#include "AgcmFont.h"
#include "AgcmUVAnimation.h"
#include "AgcmPreLODManager.h"
#include "AgcmResourceLoader.h"
#include "AgcmLensFlare.h"
#include "AgcmTextBoardMng.h"

#include "AgcmEventManager.h"
#include "AgcmItemConvert.h"
#include "AgcmCustomizeRender.h"
#include "AgcmDynamicLightmap.h"
#include "AgpmRide.h"
#include "AgcmRide.h"

#include "AgcmGeometryDataManager.h"

#include "AgcmLODDlg.h"
#include "AgcmEffectDlg.h"
#include "AgcmBlockingDlg.h"
#include "AgcmAnimationDlg.h"
#include "AgcmFactorsDlg.h"
#include "AgcmFileListDlg.h"
#include "AgcmCharacterDlg.h"
#include "AgcmItemDlg.h"
#include "AgcmObjectDlg.h"
#include "AgcmSkillDlg.h"
#include "AgcmUDADlg.h"
#include "AgcmObjectTypeDlg.h"
#include "AgcmSetFaceDlg.h"
#include "AgpmGuild.h"
#include "AgpmSummons.h"
#include "AgpmBattleGround.h"
#include "AgpmPvP.h"

#include "AgcmExportResource.h"

///////////////////////////////////////////////////////////////////////////////
#ifdef __D_UI_MODULE__
#include "AgcmUIControl.h"
#include "AgcmEventNature.h"
#include "AgpmAuctionCategory.h"
#include "AgpmAuction.h"
#include "AgcmAuction.h"
#include "AgpmEventSkillMaster.h"
#include "AgcmEventSkillMaster.h"
#include "AgpmQuest.h"
#include "AgcmQuest.h"
#include "AgpmEventQuest.h"
#include "AgcmEventQuest.h"
#include "AgcmUIManager2.h"

#include "AgpmProduct.h"
#include "AgcmProduct.h"
#include "AgpmRefinery.h"
#include "AgcmRefinery.h"
#include "AgpmEventRefinery.h"
#include "AgcmEventRefinery.h"
#include "AgcmUIProduct.h"
#include "AgcmUIMain.h"
#include "AgcmUICharacter.h"
#include "AgcmUIItem.h"
#include "AgcmUISkill2.h"
#include "AgcmUILogin.h"
#include "AgcmUIChatting2.h"
#include "AgcmUIEventItemRepair.h"
#include "AgcmUIEventTeleport.h"
#include "AgcmUIParty.h"
#include "AgcmUIPartyOption.h"
#include "AgcmUIOption.h"
#include "AgcmUIEventBank.h"

#include "AgcmMinimap.h"
#include "AgcmPrivateTrade.h"
#include "AgcmConnectManager.h"
#include "AgcmLogin.h"
#include "AgpmChatting.h"
#include "AgcmChatting2.h"
#include "AgpmEventItemRepair.h"
#include "AgcmEventItemRepair.h"
#include "AgpmEventTeleport.h"
#include "AgcmEventTeleport.h"

#include "AgcmCamera2.h"
#include "AgpmGuild.h"
#include "AgcmGuild.h"
#include "AgpmPvP.h"
#include "AgpmEventProduct.h"
#include "AgcmEventProduct.h"
#include "AgpmEventBank.h"
#include "AgcmEventBank.h"
#include "AgpmItemConvert.h"
#include "AgpmEventItemConvert.h"
#include "AgcmEventItemConvert.h"
#include "AgpmEventNPCDialog.h"
#include "AgcmUIEventNPCDialog.h"
#include "AgcmUIEventItemConvert.h"
#include "AgcmUIGuild.h"
#include "AgcmUISplitItem.h"
#include "AgcmUIAuction.h"
#include "AgcmUIQuest2.h"
#include "AgpmUIStatus.h"
#include "AgcmUIStatus.h"
#include "AgcmUIDebugInfo.h"
#include "AgcmUIPvP.h"
#include "AgcmProductMed.h"
#include "AgcmUIRefinery.h"
#include "AgpmEventGuild.h"
#include "AgcmEventGuild.h"
#include "AgcmUICooldown.h"
#include "AgpmGuild.h"

#include "AgcmEventNPCDialog.h"
#include "AgpmWorld.h"
#include "AgcmWorld.h"

#include "AgcUIManager.h"
#include "AgpmSiegeWar.h"
#include "AgcmSiegeWar.h"

#endif // __D_UI_MODULE__
///////////////////////////////////////////////////////////////////////////////

enum eTemplateKind
{
	eTemplateCharacter,
	eTemplateItem,
	eTemplateObject,
	eTemplateSkill,
	eTemplateMax
};

class AgcEngineChild : public AgcEngine
{
protected:
	FLOAT*	m_pfIdleCountRatio;
	BOOL	m_bExport;

public:
	AgcEngineChild();
	virtual ~AgcEngineChild()	 {		}

	BOOL		Destroy();

	BOOL		OnIdle();
	RwCamera*	OnCreateCamera( RpWorld * pWorld );
	RpWorld*	OnCreateWorld();
	BOOL		OnRegisterModule();

protected:
	AgcmResourceLoader*				m_pcsAgcmResourceLoader;
	ApmMap*							m_pcsApmMap;
	ApmOcTree*						m_pcsApmOcTree;
	AgcmOcTree*						m_pcsAgcmOcTree;
	AgcmFont*						m_pcsAgcmFont;
	AgpmFactors*					m_pcsAgpmFactors;
	AgcmRender*						m_pcsAgcmRender;
	AgcmLODManager*					m_pcsAgcmLODManager;
	ApmObject*						m_pcsApmObject;
	AgcmObject*						m_pcsAgcmObject;
	AgpmCharacter*					m_pcsAgpmCharacter;
	AgcmCharacter*					m_pcsAgcmCharacter;
	AgpmArchlord*					m_pcsAgpmArchlord;
	AgpmBillInfo*					m_pcsAgpmBillInfo;
	AgpmGrid*						m_pcsAgpmGrid;
	AgpmItem*						m_pcsAgpmItem;
	AgcmItem*						m_pcsAgcmItem;
	AgcmSound*						m_pcsAgcmSound;
	AgcmPostFX*						m_pcsAgcmPostFX;
	AgcmEff2*						m_pcsAgcmEff2;
	ApmEventManager*				m_pcsApmEventManager;
	AgpmTimer*						m_pcsAgpmTimer;
	AgpmUnion*						m_pcsAgpmUnion;
	AgpmParty*						m_pcsAgpmParty;
	AgcmParty*						m_pcsAgcmParty;
	AgcmUIControl*					m_pcsAgcmUIControl;
	AgpmSkill*						m_pcsAgpmSkill;
	AgcmSkill*						m_pcsAgcmSkill;
	AgpmItemConvert*				m_pcsAgpmItemConvert;
	AgcmItemConvert*				m_pcsAgcmItemConvert;
	AgcmEventEffect*				m_pcsAgcmEventEffect;
	AgcmEventManager*				m_pcsAgcmEventManager;
	AgcmUVAnimation*				m_pcsAgcmUVAnimation;
	AgcmPreLODManager*				m_pcsAgcmPreLODManager;
	AgcmCustomizeRender*			m_pcsAgcmCustomizeRender;
	AgpmRide*						m_pcsAgpmRide;
	AgcmRide*						m_pcsAgcmRide;
	AgcmGeometryDataManager*		m_pcsAgcmGeometryDataManager;	
	AgcmLODDlg*						m_pcsAgcmLODDlg;
	AgcmEffectDlg*					m_pcsAgcmEffectDlg;
	AgcmBlockingDlg*				m_pcsAgcmBlockingDlg;
	AgcmAnimationDlg*				m_pcsAgcmAnimationDlg;
	AgcmFactorsDlg*					m_pcsAgcmFactorsDlg;
	AgcmCharacterDlg*				m_pcsAgcmCharacterDlg;
	AgcmItemDlg*					m_pcsAgcmItemDlg;
	AgcmObjectDlg*					m_pcsAgcmObjectDlg;
	AgcmSkillDlg*					m_pcsAgcmSkillDlg;
	AgcmUDADlg*						m_pcsAgcmUDADlg;
	AgcmSetFaceDlg*					m_pcsAgcmSetFaceDlg;
	AgcmObjectTypeDlg				m_csAgcmObjectTypeDlg;
	AgpmGuild*						m_pcsAgpmGuild;
	AgpmSummons*					m_pcsAgpmSummons;
	AgpmBattleGround*				m_pcsAgpmBattleGround;
	AgpmPvP*						m_pcsAgpmPvP;
	AgcmTextBoardMng*				m_pcsAgcmTextBoard;	
	AgpmSiegeWar*					m_pcsAgpmSiegeWar;	
	AgcmSiegeWar*					m_pcsAgcmSiegeWar;		

///////////////////////////////////////////////////////////////////////////////
#ifdef __D_UI_MODULE__
	AgcmLensFlare					*m_pcsAgcmLensFlare;
	//AgcmTextBoard					*m_pcsAgcmTextBoard;
	AgcmMinimap						*m_pcsAgcmMinimap;

	AgcmCamera2						*m_pcsAgcmCamera2;	

	AgpmEventNature					*m_pcsAgpmEventNature;
	AgcmEventNature					*m_pcsAgcmEventNature;
	AgcmPrivateTrade				*m_pcsAgcmPrivateTrade;
	AgcmEventNPCTrade				*m_pcsAgcmEventNPCTrade;
	AgcmConnectManager				*m_pcsAgcmConnectManager;
	AgcmLogin						*m_pcsAgcmLogin;
	AgpmChatting					*m_pcsAgpmChatting;
	AgcmChatting2					*m_pcsAgcmChatting2;
	AgpmEventItemRepair				*m_pcsAgpmEventItemRepair;
	AgcmEventItemRepair				*m_pcsAgcmEventItemRepair;
	AgpmEventTeleport				*m_pcsAgpmEventTeleport;
	AgcmEventTeleport				*m_pcsAgcmEventTeleport;
	AgpmEventItemConvert			*m_pcsAgpmEventItemConvert;
	AgcmEventItemConvert			*m_pcsAgcmEventItemConvert;
	AgpmGuild						*m_pcsAgpmGuild;
	AgcmGuild						*m_pcsAgcmGuild;
	AgpmWorld						*m_pcsAgpmWorld;
	AgcmWorld						*m_pcsAgcmWorld;
	AgpmEventBank					*m_pcsAgpmEventBank;
	AgcmEventBank					*m_pcsAgcmEventBank;
	AgpmEventNPCDialog				*m_pcsAgpmEventNPCDialog;
	AgcmEventNPCDialog				*m_pcsAgcmEventNPCDialog;
	AgpmEventGuild					*m_pcsAgpmEventGuild;
	AgcmEventGuild					*m_pcsAgcmEventGuild;
	AgpmProduct						*m_pcsAgpmProduct;
	AgcmProduct						*m_pcsAgcmProduct;
	AgpmAuctionCategory				*m_pcsAgpmAuctionCategory;
	AgpmAuction						*m_pcsAgpmAuction;
	AgcmAuction						*m_pcsAgcmAuction;
	AgpmEventSkillMaster			*m_pcsAgpmEventSkillMaster;
	AgcmEventSkillMaster			*m_pcsAgcmEventSkillMaster;
	AgpmQuest						*m_pcsAgpmQuest;
	AgcmQuest						*m_pcsAgcmQuest;
	AgpmEventQuest					*m_pcsAgpmEventQuest;
	AgcmEventQuest					*m_pcsAgcmEventQuest;
	//AgpmPvP							*m_pcsAgpmPvP;
	AgcmProductMed					*m_pcsAgcmProductMed;
	AgcmUIRefinery					*m_pcsAgcmUIRefinery;
	AgpmRefinery					*m_pcsAgpmRefinery;
	AgcmRefinery					*m_pcsAgcmRefinery;
	AgpmEventRefinery				*m_pcsAgpmEventRefinery;
	AgcmEventRefinery				*m_pcsAgcmEventRefinery;
	AgpmEventProduct				*m_pcsAgpmEventProduct;
	AgcmEventProduct				*m_pcsAgcmEventProduct;
	
	AgcmUIManager2					*m_pcsAgcmUIManager2;
	
	AgcmUIProductSkill				*m_pcsAgcmUIProductSkill;
	AgcmUIMain						*m_pcsAgcmUIMain;
	AgcmUICharacter					*m_pcsAgcmUICharacter;
	AgcmUIItem						*m_pcsAgcmUIItem;
	AgcmUILogin						*m_pcsAgcmUILogin;
	AgcmUIChatting2					*m_pcsAgcmUIChatting2;
	AgcmUIEventItemRepair			*m_pcsAgcmUIEventItemRepair;
	AgcmUIEventTeleport				*m_pcsAgcmUIEventTeleport;
	AgcmUIParty						*m_pcsAgcmUIParty;
	AgcmUIPartyOption				*m_pcsAgcmUIPartyOption;
	AgcmUIOption					*m_pcsAgcmUIOption;
	AgcmUIEventBank					*m_pcsAgcmUIEventBank;
	AgcmUIEventNPCDialog			*m_pcsAgcmUIEventNPCDialog;
	AgcmUIEventItemConvert			*m_pcsAgcmUIEventItemConvert;
	AgcmUIGuild						*m_pcsAgcmUIGuild;
	AgcmUIProduct					*m_pcsAgcmUIProduct;
	AgcmUISplitItem					*m_pcsAgcmUISplitItem;
	AgcmUIAuction					*m_pcsAgcmUIAuction;
	AgcmUISkill2					*m_pcsAgcmUISkill2;
	AgcmUIQuest2					*m_pcsAgcmUIQuest2;
	AgpmUIStatus					*m_pcsAgpmUIStatus;
	AgcmUIStatus					*m_pcsAgcmUIStatus;
	AgcmUIDebugInfo					*m_pcsAgcmUIDebugInfo;
	AgcmUIPvP						*m_pcsAgcmUIPvP;
	AgcmUICooldown					*m_pcsAgcmUICooldown;	
#endif // __D_UI_MODULE__
///////////////////////////////////////////////////////////////////////////////

public:
	VOID SetIdleCountRatio(FLOAT *pfRatio)	{m_pfIdleCountRatio = pfRatio;}
	VOID SetExport(BOOL bExport)			{m_bExport = bExport;}

	AgcmResourceLoader*		GetAgcmResourceLoaderModule()			{return m_pcsAgcmResourceLoader;}
	ApmOcTree*				GetApmOcTreeModule()					{return m_pcsApmOcTree;}
	AgpmFactors*			GetAgpmFactorsModule()					{return m_pcsAgpmFactors;}
	AgcmRender*				GetAgcmRenderModule()					{return m_pcsAgcmRender;}
	AgcmLODManager*			GetAgcmLODManagerModule()				{return m_pcsAgcmLODManager;}
	ApmObject*				GetApmObjectModule()					{return m_pcsApmObject;}
	AgcmObject*				GetAgcmObjectModule()					{return m_pcsAgcmObject;}
	AgpmCharacter*			GetAgpmCharacterModule()				{return m_pcsAgpmCharacter;}
	AgcmCharacter*			GetAgcmCharacterModule()				{return m_pcsAgcmCharacter;}
	AgpmItem*				GetAgpmItemModule()						{return m_pcsAgpmItem;}
	AgcmItem*				GetAgcmItemModule()						{return m_pcsAgcmItem;}
	AgcmEff2*				GetAgcmEff2Module()						{return m_pcsAgcmEff2;}
	ApmEventManager*		GetApmEventManagerModule()				{return m_pcsApmEventManager;}
	AgcmUIControl*			GetAgcmUIControlModule()				{return m_pcsAgcmUIControl;}
	AgpmSkill*				GetAgpmSkillModule()					{return m_pcsAgpmSkill;}
	AgcmSkill*				GetAgcmSkillModule()					{return m_pcsAgcmSkill;}
	AgcmEventEffect*		GetAgcmEventEffectModule()				{return m_pcsAgcmEventEffect;}
	AgcmPreLODManager*		GetAgcmPreLODManagerModule()			{return m_pcsAgcmPreLODManager;}
	AgcmSound*				GetAgcmSound()							{return m_pcsAgcmSound;}
	AgcmCustomizeRender*	GetAgcmCustomizeRenderModule()			{return m_pcsAgcmCustomizeRender;}
	AgcmRide*				GetAgcmRideModule()						{return m_pcsAgcmRide;}
	AgcmEff2*				GetAgcmEff2()							{return m_pcsAgcmEff2; }
	AgpmSiegeWar*			GetAgpmSiegeWarModule()					{return m_pcsAgpmSiegeWar;}
	AgcmSiegeWar*			GetAgcmSiegeWarModule()					{return m_pcsAgcmSiegeWar;}
	AgcmGeometryDataManager	*GetAgcmGeometryDataManagerModule()		{return m_pcsAgcmGeometryDataManager;}
	AgcmAnimationDlg*		GetAgcmAnimationDlgModule()				{return m_pcsAgcmAnimationDlg;}
	AgcmFactorsDlg*			GetAgcmFactorsDlgModule()				{return m_pcsAgcmFactorsDlg;}
	AgcmBlockingDlg*		GetAgcmBlockingDlgModule()				{return m_pcsAgcmBlockingDlg;}
	AgcmCharacterDlg*		GetAgcmCharacterDlgModule()				{return m_pcsAgcmCharacterDlg;}
	AgcmItemDlg*			GetAgcmItemDlgModule()					{return m_pcsAgcmItemDlg;}
	AgcmObjectDlg*			GetAgcmObjectDlgModule()				{return m_pcsAgcmObjectDlg;}
	AgcmSkillDlg*			GetAgcmSkillDlgModule()					{return m_pcsAgcmSkillDlg;}
	AgcmLODDlg*				GetAgcmLODDlgModule()					{return m_pcsAgcmLODDlg;}
	AgcmUDADlg*				GetAgcmUDADlgModule()					{return m_pcsAgcmUDADlg;}
	AgcmSetFaceDlg*			GetAgcmSetFaceDlgModule()				{return m_pcsAgcmSetFaceDlg;}
	AgcmEffectDlg*			GetAgcmEffectDlgModule()				{return m_pcsAgcmEffectDlg;}
	AgcmObjectTypeDlg*		GetAgcmObjectTypeDlg()					{return &m_csAgcmObjectTypeDlg;}
	AgcmTextBoardMng*		GetAgcmTextBoardModule()				{return m_pcsAgcmTextBoard;}
	AgcmFont*				GetAgcmFont()							{return m_pcsAgcmFont;}

///////////////////////////////////////////////////////////////////////////////
#ifdef __D_UI_MODULE__
	AgcmEventNature*		GetAgcmEventNatureModule()				{return m_pcsAgcmEventNature;}
	AgcmLensFlare*			GetAgcmLensFlareModule()				{return m_pcsAgcmLensFlare;}
	//AgcmTextBoard*			GetAgcmTextBoardModule()				{return m_pcsAgcmTextBoard;}
	AgcmUIManager2*			GetAgcmUIManager2Module()				{return m_pcsAgcmUIManager2;}
#endif // __D_UI_MODULE__
///////////////////////////////////////////////////////////////////////////////	

	ApBase*					GetTemplate( eTemplateKind eKind, INT32 nID );
	AgpdCharacterTemplate*	GetPCharTemplate( INT32 nID );
	AgcdCharacterTemplate*	GetCCharTemplate( INT32 nID );
	AgcdCharacterTemplate*	GetCCharTemplate( AgpdCharacterTemplate* pPublic );
	BOOL					GetCharTemplate( INT32 nID, AgpdCharacterTemplate*& pPublic, AgcdCharacterTemplate*& pClient );
	AgpdItemTemplate*		GetPItemTemplate( INT32 nID );
	AgcdItemTemplate*		GetCItemTemplate( INT32 nID );
	AgcdItemTemplate*		GetCItemTemplate( AgpdItemTemplate* pPublic );
	BOOL					GetItemTemplate( INT32 nID, AgpdItemTemplate*& pPublic, AgcdItemTemplate*& pClient );
	ApdObjectTemplate*		GetPObjectTemplate( INT32 nID );
	AgcdObjectTemplate*		GetCObjectTemplate( INT32 nID );
	AgcdObjectTemplate*		GetCObjectTemplate( ApdObjectTemplate* pPublic );
	BOOL					GetObjectTemplate( INT32 nID, ApdObjectTemplate*& pPublic, AgcdObjectTemplate*& pClient );
	AgpdSkillTemplate*		GetPSkillTemplate( INT32 nID );
	AgcdSkillTemplate*		GetCSkillTemplate( INT32 nID );
	AgcdSkillTemplate*		GetCSkillTemplate( AgpdSkillTemplate* pPublic );
	BOOL					GetSkillTemplate( INT32 nID, AgpdSkillTemplate*& pPublic, AgcdSkillTemplate*& pClient );
	
	AgcdPreLOD*				GetPreLod( eTemplateKind eKind, INT32 nID );
	AgcdLOD*				GetLod( eTemplateKind eKind, INT32 nID );

	AgpdFactor*				GetFactor( eTemplateKind eKind, INT32 nID );
	AgcdUseEffectSet*		GetUseEffect( eTemplateKind eKind, INT32 nID );

	AgpdCharacter*			GetPCharacter( INT32 nID );
	AgcdCharacter*			GetCCharacter( INT32 nID );
	AgcdCharacter*			GetCCharacter( AgpdCharacter* pPublic );

	AgpdItem*				GetCharCurEquipItem( AgpdCharacter *pcsAgpdCharacter, INT32 lPartID )
	{
		AgpdGridItem	*pcsAgpdGridItem	= GetAgpmItemModule()->GetEquipItem( pcsAgpdCharacter, lPartID );
		return pcsAgpdGridItem ? GetAgpmItemModule()->GetItem( pcsAgpdGridItem->m_lItemID ) : NULL;
	}

	enum	SaveFlag
	{
		SF_NOT_CHANGED,
		SF_CHANGED,
		SF_SAVED
	};
	// Attach Data..

	struct	stSaveDataInfo
	{
	protected:
		SaveFlag	eFlag;

	public:
		void	Init()			{ eFlag = SF_NOT_CHANGED;}
		void	SetChanged()	{ eFlag = SF_CHANGED;}
		void	Clear()			{ eFlag = SF_NOT_CHANGED; }
		void	SetSaved()		{ eFlag = SF_SAVED; }
		BOOL	GetChagned() const	{ return eFlag == SF_CHANGED ? TRUE : FALSE; }
		SaveFlag	GetState() const	{ return eFlag; }
	};

	template< typename T >
	static	BOOL OnSaveDataInfoCreate(PVOID pData, PVOID pClass, PVOID pCustData);

	void	AttachSaveDataInfo();
	
	INT16	m_nAttachDataIndexCharacter;
	INT16	m_nAttachDataIndexObject;
	INT16	m_nAttachDataIndexItem;

	stSaveDataInfo	* GetAttachedData(	AgpdCharacterTemplate	* pcsTemplate );
	stSaveDataInfo	* GetAttachedData(	ApdObjectTemplate		* pcsTemplate );
	stSaveDataInfo	* GetAttachedData(	AgpdItemTemplate		* pcsTemplate );

	template< typename T >
	void SetSaveData( T * pcsTemplate )
	{
		stSaveDataInfo	* pSaveData = GetAttachedData( pcsTemplate );
		pSaveData->SetChanged();
	}

	template< typename T >
	void SetSaved( T * pcsTemplate )
	{
		stSaveDataInfo	* pSaveData = GetAttachedData( pcsTemplate );
		pSaveData->SetSaved();
	}
};

class MessageBoxLogger
{
public:
	bool	operator()( const char * pMessage )
	{
		MessageBox( NULL , pMessage , "ModelTool" , MB_OK );
		return true;
	}
};
