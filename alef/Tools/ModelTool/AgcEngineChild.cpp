#include "stdafx.h"
#include "AgcEngineChild.h"

AgcEngineChild::AgcEngineChild() : 
 m_pfIdleCountRatio(NULL),
 m_bExport(FALSE)
{
}

BOOL AgcEngineChild::Destroy()
{
	if( AuPackingManager::GetSingletonPtr() )
		delete AuPackingManager::GetSingletonPtr();

	return AgcEngine::Destroy();
}

RpWorld *	AgcEngineChild::OnCreateWorld() // 월드를 생성해서 리턴해준다.
{
	return AgcEngine::OnCreateWorld();
}

RwCamera *	AgcEngineChild::OnCreateCamera( RpWorld * pWorld )	// 메인 카메라를 Create해준다.
{
	return AgcEngine::OnCreateCamera(pWorld);
}

BOOL AgcEngineChild::OnIdle()
{
	return AgcEngine::OnIdle();
}

BOOL AgcEngineChild::OnRegisterModule()
{
	REGISTER_MODULE(m_pcsAgcmResourceLoader, AgcmResourceLoader);
	REGISTER_MODULE(m_pcsApmMap, ApmMap);
	REGISTER_MODULE(m_pcsApmOcTree, ApmOcTree);
	REGISTER_MODULE(m_pcsAgcmOcTree, AgcmOcTree);
	REGISTER_MODULE(m_pcsAgcmFont, AgcmFont);
	REGISTER_MODULE(m_pcsAgcmPostFX, AgcmPostFX);
	REGISTER_MODULE_SINGLETON(AgcmMap);
	REGISTER_MODULE(m_pcsAgcmRender, AgcmRender);
	REGISTER_MODULE(m_pcsAgpmFactors, AgpmFactors);
	REGISTER_MODULE(m_pcsAgcmLODManager, AgcmLODManager);
	REGISTER_MODULE(m_pcsApmObject, ApmObject);
	REGISTER_MODULE(m_pcsAgcmObject, AgcmObject);
	REGISTER_MODULE(m_pcsAgpmCharacter, AgpmCharacter);
	REGISTER_MODULE(m_pcsAgpmSummons, AgpmSummons);
	REGISTER_MODULE(m_pcsAgpmBillInfo, AgpmBillInfo);
	REGISTER_MODULE(m_pcsAgpmGrid, AgpmGrid);
	REGISTER_MODULE(m_pcsAgpmItem, AgpmItem);
	REGISTER_MODULE(m_pcsAgpmGuild, AgpmGuild);
	REGISTER_MODULE(m_pcsAgcmSound, AgcmSound);
	REGISTER_MODULE(m_pcsAgcmUVAnimation, AgcmUVAnimation);
	REGISTER_MODULE(m_pcsAgcmEff2, AgcmEff2);
	REGISTER_MODULE(m_pcsApmEventManager, ApmEventManager);
	REGISTER_MODULE(m_pcsAgpmTimer, AgpmTimer);
	REGISTER_MODULE(m_pcsAgpmUnion, AgpmUnion);
	REGISTER_MODULE(m_pcsAgpmParty, AgpmParty);
	REGISTER_MODULE(m_pcsAgcmUIControl, AgcmUIControl);
	REGISTER_MODULE(m_pcsAgpmSkill, AgpmSkill);
	REGISTER_MODULE(m_pcsAgpmBattleGround, AgpmBattleGround);
	REGISTER_MODULE(m_pcsAgpmPvP, AgpmPvP);
	REGISTER_MODULE(m_pcsAgpmArchlord, AgpmArchlord);
	REGISTER_MODULE(m_pcsAgcmCharacter, AgcmCharacter);
	REGISTER_MODULE(m_pcsAgcmItem, AgcmItem);
	REGISTER_MODULE(m_pcsAgcmParty, AgcmParty);
	REGISTER_MODULE(m_pcsAgcmSkill, AgcmSkill);
	REGISTER_MODULE(m_pcsAgpmItemConvert, AgpmItemConvert);
	REGISTER_MODULE(m_pcsAgcmItemConvert, AgcmItemConvert);
	REGISTER_MODULE(m_pcsAgcmEventEffect, AgcmEventEffect);
	REGISTER_MODULE(m_pcsAgcmEventManager, AgcmEventManager);
	REGISTER_MODULE(m_pcsAgcmCustomizeRender, AgcmCustomizeRender);
	REGISTER_MODULE(m_pcsAgpmRide, AgpmRide);
	REGISTER_MODULE(m_pcsAgcmRide, AgcmRide);
	REGISTER_MODULE(m_pcsAgpmSiegeWar, AgpmSiegeWar);
	REGISTER_MODULE(m_pcsAgcmSiegeWar, AgcmSiegeWar);

	if( !m_bExport )
		REGISTER_MODULE(m_pcsAgcmPreLODManager, AgcmPreLODManager);

	REGISTER_MODULE(m_pcsAgcmGeometryDataManager, AgcmGeometryDataManager);

	if( !m_bExport )
		REGISTER_MODULE(m_pcsAgcmLODDlg, AgcmLODDlg);

	REGISTER_MODULE(m_pcsAgcmBlockingDlg, AgcmBlockingDlg);
	REGISTER_MODULE(m_pcsAgcmEffectDlg, AgcmEffectDlg);
	REGISTER_MODULE(m_pcsAgcmFactorsDlg, AgcmFactorsDlg);
	REGISTER_MODULE(m_pcsAgcmAnimationDlg, AgcmAnimationDlg);
	REGISTER_MODULE(m_pcsAgcmCharacterDlg, AgcmCharacterDlg);
	REGISTER_MODULE(m_pcsAgcmItemDlg, AgcmItemDlg);
	REGISTER_MODULE(m_pcsAgcmObjectDlg, AgcmObjectDlg);
	REGISTER_MODULE(m_pcsAgcmSkillDlg, AgcmSkillDlg);
	REGISTER_MODULE(m_pcsAgcmUDADlg, AgcmUDADlg);
	REGISTER_MODULE(m_pcsAgcmSetFaceDlg, AgcmSetFaceDlg);

	REGISTER_MODULE(m_pcsAgcmTextBoard, AgcmTextBoardMng );	

#ifdef __D_UI_MODULE__
	REGISTER_MODULE(m_pcsAgpmGuild, AgpmGuild);
	REGISTER_MODULE(m_pcsAgcmGuild, AgcmGuild);
	REGISTER_MODULE(m_pcsAgpmPvP, AgpmPvP);
	REGISTER_MODULE(m_pcsAgpmEventBank, AgpmEventBank);
	REGISTER_MODULE(m_pcsAgcmEventBank, AgcmEventBank);
	REGISTER_MODULE(m_pcsAgpmEventItemConvert, AgpmEventItemConvert)
	REGISTER_MODULE(m_pcsAgcmEventItemConvert, AgcmEventItemConvert);
	REGISTER_MODULE(m_pcsAgpmEventNPCDialog, AgpmEventNPCDialog);
	REGISTER_MODULE(m_pcsAgcmEventNPCDialog, AgcmEventNPCDialog);
	REGISTER_MODULE(m_pcsAgpmEventGuild, AgpmEventGuild);
	REGISTER_MODULE(m_pcsAgcmEventGuild, AgcmEventGuild);

	REGISTER_MODULE(m_pcsAgcmCamera2, AgcmCamera2);
	REGISTER_MODULE(m_pcsAgcmUIManager2, AgcmUIManager2);
	REGISTER_MODULE(m_pcsAgpmEventNature, AgpmEventNature);
	REGISTER_MODULE(m_pcsAgcmEventNature, AgcmEventNature);
	REGISTER_MODULE(m_pcsAgcmTextBoard, AgcmTextBoardMng);
	REGISTER_MODULE(m_pcsAgcmLensFlare, AgcmLensFlare);
	REGISTER_MODULE(m_pcsAgcmPrivateTrade, AgcmPrivateTrade);
	REGISTER_MODULE(m_pcsAgcmEventNPCTrade, AgcmEventNPCTrade);
	REGISTER_MODULE(m_pcsAgcmConnectManager, AgcmConnectManager);
	REGISTER_MODULE(m_pcsAgcmLogin, AgcmLogin);
	REGISTER_MODULE(m_pcsAgpmChatting, AgpmChatting);
	REGISTER_MODULE(m_pcsAgcmChatting2, AgcmChatting2);
	REGISTER_MODULE(m_pcsAgpmEventItemRepair, AgpmEventItemRepair);
	REGISTER_MODULE(m_pcsAgcmEventItemRepair, AgcmEventItemRepair);
	REGISTER_MODULE(m_pcsAgpmEventTeleport, AgpmEventTeleport);
	REGISTER_MODULE(m_pcsAgcmEventTeleport, AgcmEventTeleport);
	REGISTER_MODULE(m_pcsAgpmWorld, AgpmWorld);
	REGISTER_MODULE(m_pcsAgcmWorld, AgcmWorld);
	REGISTER_MODULE(m_pcsAgpmProduct, AgpmProduct);
	REGISTER_MODULE(m_pcsAgcmProduct, AgcmProduct);

	REGISTER_MODULE(m_pcsAgpmAuction, AgpmAuction);
	REGISTER_MODULE(m_pcsAgcmAuction, AgcmAuction);
	REGISTER_MODULE(m_pcsAgpmAuctionCategory, AgpmAuctionCategory);	
	REGISTER_MODULE(m_pcsAgpmEventSkillMaster, AgpmEventSkillMaster);
	REGISTER_MODULE(m_pcsAgcmEventSkillMaster, AgcmEventSkillMaster);
	REGISTER_MODULE(m_pcsAgpmQuest, AgpmQuest);
	REGISTER_MODULE(m_pcsAgcmQuest, AgcmQuest);
	REGISTER_MODULE(m_pcsAgpmEventQuest, AgpmEventQuest);
	REGISTER_MODULE(m_pcsAgcmEventQuest, AgcmEventQuest);

	REGISTER_MODULE(m_pcsAgcmUIMain, AgcmUIMain);
	REGISTER_MODULE(m_pcsAgcmUICharacter, AgcmUICharacter);
	REGISTER_MODULE(m_pcsAgcmUISplitItem, AgcmUISplitItem);
	REGISTER_MODULE(m_pcsAgcmUIAuction, AgcmUIAuction);
	REGISTER_MODULE(m_pcsAgcmUIItem, AgcmUIItem);
	REGISTER_MODULE(m_pcsAgcmUISkill2, AgcmUISkill2);
	REGISTER_MODULE(m_pcsAgcmUILogin, AgcmUILogin);
	REGISTER_MODULE(m_pcsAgcmUIEventItemRepair, AgcmUIEventItemRepair);
	REGISTER_MODULE(m_pcsAgcmUIEventTeleport, AgcmUIEventTeleport);
	REGISTER_MODULE(m_pcsAgcmUIParty, AgcmUIParty);
	REGISTER_MODULE(m_pcsAgcmUIPartyOption, AgcmUIPartyOption);
	REGISTER_MODULE(m_pcsAgcmUIOption, AgcmUIOption);
	REGISTER_MODULE(m_pcsAgcmUIEventBank, AgcmUIEventBank);	
	REGISTER_MODULE(m_pcsAgcmUIEventNPCDialog, AgcmUIEventNPCDialog);
	REGISTER_MODULE(m_pcsAgcmUIEventItemConvert, AgcmUIEventItemConvert);
	REGISTER_MODULE(m_pcsAgcmUIGuild, AgcmUIGuild);
	REGISTER_MODULE(m_pcsAgcmUIPvP, AgcmUIPvP);
	REGISTER_MODULE(m_pcsAgcmUIQuest, AgcmUIQuest);
	REGISTER_MODULE(m_pcsAgcmUIQuest2, AgcmUIQuest2);
	REGISTER_MODULE(m_pcsAgpmRefinery, AgpmRefinery);
	REGISTER_MODULE(m_pcsAgcmRefinery, AgcmRefinery);
	REGISTER_MODULE(m_pcsAgpmEventRefinery, AgpmEventRefinery);
	REGISTER_MODULE(m_pcsAgcmEventRefinery, AgcmEventRefinery);

	REGISTER_MODULE(m_pcsAgpmEventProduct, AgpmEventProduct);
	REGISTER_MODULE(m_pcsAgcmEventProduct, AgcmEventProduct);
	REGISTER_MODULE(m_pcsAgcmUIProduct, AgcmUIProduct);
	REGISTER_MODULE(m_pcsAgcmUIProductSkill, AgcmUIProductSkill);
	REGISTER_MODULE(m_pcsAgcmProductMed, AgcmProductMed);
	REGISTER_MODULE(m_pcsAgcmUIRefinery, AgcmUIRefinery);
	REGISTER_MODULE(m_pcsAgcmUIChatting2, AgcmUIChatting2);
	REGISTER_MODULE(m_pcsAgpmUIStatus, AgpmUIStatus);
	REGISTER_MODULE(m_pcsAgcmUIStatus, AgcmUIStatus);
	REGISTER_MODULE(m_pcsAgcmUIDebugInfo, AgcmUIDebugInfo);
	REGISTER_MODULE(m_pcsAgcmUICooldown, AgcmUICooldown);

	REGISTER_MODULE(m_pcsAgcmMinimap, AgcmMinimap);
#endif // __D_UI_MODULE__
	
	AttachSaveDataInfo();

	SequenceNumberManager::GetInstance().AttachTemplateSequenceData();
	return TRUE;
}

ApBase*					AgcEngineChild::GetTemplate( eTemplateKind eKind, INT32 nID )
{
	switch( eKind )
	{
	case eTemplateCharacter:	return (ApBase*)GetPCharTemplate( nID );
	case eTemplateItem:			return (ApBase*)GetPItemTemplate( nID );
	case eTemplateObject:		return (ApBase*)GetPObjectTemplate( nID );
	case eTemplateSkill:
	default:					
		return NULL;
	}
}

AgpdCharacterTemplate*	AgcEngineChild::GetPCharTemplate( INT32 nID )
{
	return GetAgpmCharacterModule()->GetCharacterTemplate( nID );
}

AgcdCharacterTemplate*	AgcEngineChild::GetCCharTemplate( INT32 nID )
{
	return GetCCharTemplate( GetPCharTemplate( nID ) );
}

AgcdCharacterTemplate*	AgcEngineChild::GetCCharTemplate( AgpdCharacterTemplate* pPublic )
{
	return pPublic ? GetAgcmCharacterModule()->GetTemplateData( pPublic ) : NULL;
}

BOOL					AgcEngineChild::GetCharTemplate( INT32 nID, AgpdCharacterTemplate*& pPublic, AgcdCharacterTemplate*& pClient )
{
	pPublic = GetPCharTemplate( nID );
	if( !pPublic )		return FALSE;

	pClient = GetCCharTemplate( pPublic );

	return pClient ? TRUE : FALSE;
}

AgpdItemTemplate*		AgcEngineChild::GetPItemTemplate( INT32 nID )
{
	return GetAgpmItemModule()->GetItemTemplate( nID );
}

AgcdItemTemplate*		AgcEngineChild::GetCItemTemplate( INT32 nID )
{
	return GetCItemTemplate( GetPItemTemplate( nID ) );
}

AgcdItemTemplate*		AgcEngineChild::GetCItemTemplate( AgpdItemTemplate* pPublic )
{
	return pPublic ? GetAgcmItemModule()->GetTemplateData( pPublic ) : NULL;
}

BOOL					AgcEngineChild::GetItemTemplate( INT32 nID, AgpdItemTemplate*& pPublic, AgcdItemTemplate*& pClient )
{
	pPublic = GetPItemTemplate( nID );
	if( !pPublic )		return FALSE;

	pClient = GetCItemTemplate( pPublic );

	return pClient ? TRUE : FALSE;
}

ApdObjectTemplate*		AgcEngineChild::GetPObjectTemplate( INT32 nID )
{
	return GetApmObjectModule()->GetObjectTemplate( nID );
}

AgcdObjectTemplate*		AgcEngineChild::GetCObjectTemplate( INT32 nID )
{
	return GetCObjectTemplate( GetPObjectTemplate( nID ) );
}

AgcdObjectTemplate*		AgcEngineChild::GetCObjectTemplate( ApdObjectTemplate* pPublic )
{
	return GetAgcmObjectModule()->GetTemplateData( pPublic );
}

BOOL					AgcEngineChild::GetObjectTemplate( INT32 nID, ApdObjectTemplate*& pPublic, AgcdObjectTemplate*& pClient )
{
	pPublic = GetPObjectTemplate( nID );
	if( !pPublic )		return FALSE;

	pClient = GetCObjectTemplate( pPublic );

	return pClient ? TRUE : FALSE;
}

AgpdSkillTemplate*		AgcEngineChild::GetPSkillTemplate( INT32 nID )
{
	return GetAgpmSkillModule()->GetSkillTemplate( nID );
}

AgcdSkillTemplate*		AgcEngineChild::GetCSkillTemplate( INT32 nID )
{
	return GetCSkillTemplate( GetPSkillTemplate( nID ) );
}

AgcdSkillTemplate*		AgcEngineChild::GetCSkillTemplate( AgpdSkillTemplate* pPublic )
{
	return pPublic ? GetAgcmSkillModule()->GetADSkillTemplate( pPublic ) : NULL;
}

BOOL					AgcEngineChild::GetSkillTemplate( INT32 nID, AgpdSkillTemplate*& pPublic, AgcdSkillTemplate*& pClient )
{
	pPublic = GetPSkillTemplate( nID );
	if( !pPublic )		return FALSE;

	pClient = GetCSkillTemplate( pPublic );

	return pClient ? TRUE : FALSE;
}

AgcdPreLOD*				AgcEngineChild::GetPreLod( eTemplateKind eKind, INT32 nID )
{
	ApBase* pcsBase	= NULL;
	switch( eKind )
	{
	case eTemplateCharacter:
		pcsBase = (ApBase*)GetPCharTemplate( nID );
		break;
	case eTemplateItem:
		pcsBase = (ApBase*)GetPItemTemplate( nID );
		break;
	case eTemplateObject:
		pcsBase = (ApBase*)GetPObjectTemplate( nID );
		break;
	case eTemplateSkill:
	default:
		return NULL;
	}

	return pcsBase ? GetAgcmPreLODManagerModule()->GetPreLOD( pcsBase ) : NULL;
}

AgcdLOD*				AgcEngineChild::GetLod( eTemplateKind eKind, INT32 nID )
{
	switch( eKind )
	{
	case eTemplateCharacter:
		{
			AgcdCharacterTemplate* pClient = GetCCharTemplate( nID );
			return pClient ? &pClient->m_stLOD : NULL;
		}
	case eTemplateItem:
		{
			AgcdItemTemplate* pClient = GetCItemTemplate( nID );
			return pClient ? &pClient->m_stLOD : NULL;
		}
	case eTemplateObject:
		{
			AgcdObjectTemplate* pClient = GetCObjectTemplate( nID );
			return pClient ? &pClient->m_stLOD : NULL;
		}
	case eTemplateSkill:
	default:
		return NULL;
	}
}
						
AgpdFactor*				AgcEngineChild::GetFactor( eTemplateKind eKind, INT32 nID )
{
	ApBase* pcsBase	= NULL;
	switch( eKind )
	{
	case eTemplateCharacter:
		{
			AgpdCharacterTemplate* pPublic= GetPCharTemplate( nID );
			return pPublic ? &pPublic->m_csFactor : NULL;
		}
	case eTemplateItem:
		{
			AgpdItemTemplate* pPublic= GetPItemTemplate( nID );
			return pPublic ? &pPublic->m_csFactor : NULL;
		}
	case eTemplateObject:
	case eTemplateSkill:
	default:
		return NULL;
	}
}

AgcdUseEffectSet*		AgcEngineChild::GetUseEffect( eTemplateKind eKind, INT32 nID )
{
	ApBase* pcsBase	= NULL;
	switch( eKind )
	{
	case eTemplateCharacter:
		pcsBase = (ApBase*)GetPCharTemplate( nID );
		break;
	case eTemplateItem:
		pcsBase = (ApBase*)GetPItemTemplate( nID );
		break;
	case eTemplateObject:
		pcsBase = (ApBase*)GetPObjectTemplate( nID );
		break;
	case eTemplateSkill:
	default:
		return NULL;
	}

	return pcsBase ? GetAgcmEventEffectModule()->GetUseEffectSet( pcsBase ) : NULL;
}

AgpdCharacter*			AgcEngineChild::GetPCharacter( INT32 nID )
{
	return GetAgpmCharacterModule()->GetCharacter( nID );
}

AgcdCharacter*			AgcEngineChild::GetCCharacter( INT32 nID )
{
	return GetCCharacter( GetPCharacter( nID ) );
}

AgcdCharacter*			AgcEngineChild::GetCCharacter( AgpdCharacter* pPublic )
{
	return pPublic ? GetAgcmCharacterModule()->GetCharacterData( pPublic ) : NULL;
}

template< typename T >
BOOL AgcEngineChild::OnSaveDataInfoCreate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcEngineChild	* pThis	= (AgcEngineChild *) pClass;

	T	*pcsTemplate		= (T *)(pData);

	AgcEngineChild::stSaveDataInfo	*pSaveData	= (AgcEngineChild::stSaveDataInfo *)pThis->GetAttachedData(pcsTemplate);
	pSaveData->Init();
	return TRUE;
}

void	AgcEngineChild::AttachSaveDataInfo()
{
	m_nAttachDataIndexCharacter	= m_pcsAgpmCharacter	->AttachCharacterTemplateData	(this, sizeof(stSaveDataInfo), OnSaveDataInfoCreate< AgpdCharacterTemplate > , NULL );
	m_nAttachDataIndexObject	= m_pcsApmObject		->AttachObjectTemplateData		(this, sizeof(stSaveDataInfo), OnSaveDataInfoCreate< ApdObjectTemplate > , NULL );
	m_nAttachDataIndexItem		= m_pcsAgpmItem			->AttachItemTemplateData		(this, sizeof(stSaveDataInfo), OnSaveDataInfoCreate< AgpdItemTemplate > , NULL );
}

AgcEngineChild::stSaveDataInfo	* AgcEngineChild::GetAttachedData(	AgpdCharacterTemplate	* pcsTemplate )
{
	return	(AgcEngineChild::stSaveDataInfo	*) m_pcsAgpmCharacter->GetAttachedModuleData(m_nAttachDataIndexCharacter, (void *) pcsTemplate);
}
AgcEngineChild::stSaveDataInfo	* AgcEngineChild::GetAttachedData(	ApdObjectTemplate		* pcsTemplate )
{
	return	(AgcEngineChild::stSaveDataInfo	*) m_pcsApmObject->GetAttachedModuleData(m_nAttachDataIndexObject, (void *) pcsTemplate);
}
AgcEngineChild::stSaveDataInfo	* AgcEngineChild::GetAttachedData(	AgpdItemTemplate		* pcsTemplate )
{
	return	(AgcEngineChild::stSaveDataInfo	*) m_pcsAgpmItem->GetAttachedModuleData(m_nAttachDataIndexItem, (void *) pcsTemplate);
}