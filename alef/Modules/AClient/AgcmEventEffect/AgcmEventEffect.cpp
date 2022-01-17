#include "AgcmEventEffect.h"
#include "ApMemoryTracker.h"
#include "AgcmMap.h"
#include "ApModuleStream.h"
#include "AcuObjecWire.h"
#include "AgcdCharacter.h"
#include "AuMath.h"
#include "AgpmSiegeWar.h"
#include "AgcmUIOption.h"
#include "AgcmUILogin.h"
#include "cslog.h"

namespace NS_HardCoding_AgcmEventEffect
{
	INT32	g_WeaponType_Of_Attacker	= -1;
};

AgpdItem* GetEquipItem(AgpmItem *pModule, AgpdCharacter *pcsAgpdCharacter, AgpmItemPart ePart)
{
	AgpdGridItem* pcsAgpdGridItem = pModule->GetEquipItem( pcsAgpdCharacter, ePart );
	return pcsAgpdGridItem ? pModule->GetItem(pcsAgpdGridItem->m_lItemID) : NULL;
}

AgcmEventEffect	* AgcmEventEffect::m_pcsThisAgcmEventEffect	= NULL;
AgcmEventEffect::AgcmEventEffect()
{
	SetModuleName("AgcmEventEffect");

	EnableIdle(TRUE);

	m_lSelfCID				= 0;
	m_ulNowClockCount		= 0;
	m_bForceStereo			= FALSE;
	m_ulStatus				= E_AEE_STATUS_NONE;
	
	memset(m_pacsCheckTimeDataList, 0, sizeof(AgcdEventEffectCheckTimeDataList *) * E_AGCD_EVENT_EFFECT_MAX_CHECK_TIME_ON_OFF * AGCMEVENTEFFECT_NUM_CHECK_TIME);

	m_bCastingEffectDisabled = FALSE;

	AgcmEventEffect::m_pcsThisAgcmEventEffect	= this;
}

AgcmEventEffect::~AgcmEventEffect()
{
}

BOOL AgcmEventEffect::OnInit()
{
	if (m_pcsAgcmCharacter)
	{
		if ( !m_pcsAgcmCharacter->GetAgcaAnimation2()->SetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, sizeof (AEE_CharAnimAttachedData), CBRemoveAnimation, this ) )
		{
			ASSERT(!"!m_pcsAgcmCharacter->GetAgcaAnimation2()->SetAttachedData()");
			return FALSE;
		}
	}

	m_pcsEventNature = ( AgcmEventNature * ) GetModule( "AgcmEventNature" );

	if( m_pcsEventNature )
	{
		if ( !m_pcsEventNature->SetCallbackSkyChange( CBSkyChange, this ) )
			return FALSE;
	}

	ASSERT( m_pcsAgcmSound );
	if ( m_pcsAgcmSound )
		if ( m_pcsAgcmSound->GetCurrentDeviceIndex() == -1 )
			SetForceStereoSound();

	return TRUE;
}

BOOL AgcmEventEffect::OnAddModule()
{
	// 모듈을 가져온다.
	m_pcsAgcmSound = (AgcmSound *)(GetModule("AgcmSound"));
	m_pcsApmMap = (ApmMap *)(GetModule("ApmMap"));
	m_pcsAgpmFactors = (AgpmFactors *)(GetModule("AgpmFactors"));
	m_pcsApmEventManager = (ApmEventManager *)(GetModule("ApmEventManager"));
	m_pcsAgpmTimer = (AgpmTimer *)(GetModule("AgpmTimer"));
	m_pcsApmObject = (ApmObject *)(GetModule("ApmObject"));
	m_pcsAgcmObject = (AgcmObject *)(GetModule("AgcmObject"));
	m_pcsAgpmCharacter = (AgpmCharacter *)(GetModule("AgpmCharacter"));
	m_pcsAgcmCharacter = (AgcmCharacter *)(GetModule("AgcmCharacter"));
	m_pcsAgpmItem = (AgpmItem *)(GetModule("AgpmItem"));
	m_pcsAgpmItemConvert = (AgpmItemConvert *)(GetModule("AgpmItemConvert"));
	m_pcsAgcmItem = (AgcmItem *)(GetModule("AgcmItem"));
	m_pcsAgpmSkill = (AgpmSkill *)(GetModule("AgpmSkill"));
	m_pcsAgcmSkill = (AgcmSkill *)(GetModule("AgcmSkill"));
	m_pcsAgcmEff2 = (AgcmEff2 *)(GetModule("AgcmEff2"));
	m_pcsAgcmRender = (AgcmRender *)(GetModule("AgcmRender"));
	m_pcsAgcmResourceLoader = (AgcmResourceLoader*)GetModule("AgcmResourceLoader");
	m_pcsAgpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");

	if (	(!m_pcsAgcmSound) ||
			(!m_pcsAgpmFactors) ||
			(!m_pcsApmEventManager) ||
			(!m_pcsAgpmTimer) ||
			(!m_pcsApmObject) ||
			(!m_pcsAgcmObject) ||
			(!m_pcsAgpmCharacter) ||
			(!m_pcsAgcmCharacter) ||
			(!m_pcsAgpmItem) ||
			(!m_pcsAgpmItemConvert) ||
			(!m_pcsAgpmSkill) ||
			(!m_pcsAgcmSkill) ||
			//(!m_pcsAgcmEff2) ||
			(!m_pcsAgcmRender) ||
			(!m_pcsAgcmResourceLoader) ||
			(!m_pcsAgpmArchlord)
			)
	{
		ASSERT(!"AgcmEventEffect::OnAddModule() Failed!");
	}

	if (m_pcsAgpmCharacter)
	{
		m_nAttachedIndexCharEventEffect = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgcdEventEffectCharacter), AttachedDataEventEffectDataConstructCB, AttachedDataEventEffectDataDestructCB);
		if (m_nAttachedIndexCharEventEffect < 0) return FALSE;

		m_nAttachedIndexCharEffectList = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgcmEventEffectList), AttachedDataEffectListConstructCB, AttachedDataEffectListDestructCB);
		if (m_nAttachedIndexCharEffectList < 0) return FALSE;

		m_nSkyFlagOffset = m_pcsAgpmCharacter->AttachCharacterData ( this, sizeof (ACharacterSkyFlags), NULL, NULL);
		if (m_nSkyFlagOffset < 0) return FALSE;

		m_nAttachedIndexCharUseEffectSet = m_pcsAgpmCharacter->AttachCharacterTemplateData(this, sizeof(AgcdUseEffectSet), AttachedDataCharUseEffectSetConstructCB, AttachedDataUseEffectSetDestructCB);
		if (m_nAttachedIndexCharUseEffectSet < 0) return FALSE;

		if (!AddStreamCallback( AGCMEVENT_EFFECT_DATA_TYPE_CHAR_TEMPLATE, CharTemplateStreamReadCB, CharTemplateStreamWriteCB, this ) ) return FALSE;

		m_pcsAgpmCharacter->SetCallbackUpdateLevel(EventCharLevelUPCB, this);
		m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(EventCharUpdateActionStatus, this); // 캐릭터 부활등...
		m_pcsAgpmCharacter->SetCallbackTransformAppear(EventCharTransform, this);
		m_pcsAgpmCharacter->SetCallbackUpdateSpecialStatus(EventCharSpecialStatus, this);

		if (!m_pcsAgpmCharacter->SetCallbackUseEffect(CBCharUseEffect, this)) return FALSE;
	}

	if (m_pcsAgpmItem)
	{
		m_nAttachedIndexItemEventEffect = m_pcsAgpmItem->AttachItemData(this, sizeof(AgcdEventEffectItem), AttachedDataEventEffectDataConstructCB, AttachedDataEventEffectDataDestructCB);
		if (m_nAttachedIndexItemEventEffect < 0) return FALSE;

		m_nAttachedIndexItemEffectList = m_pcsAgpmItem->AttachItemData(this, sizeof(AgcmEventEffectList), AttachedDataEffectListConstructCB, AttachedDataEffectListDestructCB);
		if (m_nAttachedIndexItemEffectList < 0) return FALSE;

		m_nAttachedIndexItemUseEffectSet = m_pcsAgpmItem->AttachItemTemplateData(this, sizeof(AgcdUseEffectSet), AttachedDataItemUseEffectSetConstructCB, AttachedDataUseEffectSetDestructCB);
		if (m_nAttachedIndexItemUseEffectSet < 0) return FALSE;

		if (!m_pcsAgpmItem->AddStreamCallback(AGPMITEM_DATA_TYPE_TEMPLATE, ItemTemplateStreamReadCB, ItemTemplateStreamWriteCB, this)) return FALSE;

		m_pcsAgpmItem->SetCallbackUseItemSuccess(EventItemUseCB, this);
	}

	if (m_pcsApmObject)
	{
		m_nAttachedIndexObjEffectList = m_pcsApmObject->AttachObjectData(this, sizeof(AgcmEventEffectList), AttachedDataEffectListConstructCB, AttachedDataEffectListDestructCB);
		if (m_nAttachedIndexObjEffectList < 0) return FALSE;

		m_nAttachedIndexObjUseEffectSet = m_pcsApmObject->AttachObjectTemplateData(this, sizeof(AgcdUseEffectSet), AttachedDataObjUseEffectSetConstructCB, AttachedDataUseEffectSetDestructCB);
		if (m_nAttachedIndexObjUseEffectSet < 0) return FALSE;
		if (!m_pcsApmObject->AddStreamCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, ObjTemplateStreamReadCB, ObjTemplateStreamWriteCB, this)) return FALSE;
	}

	if (m_pcsAgcmEff2)
	{
		m_pcsAgcmEff2->SetCallbackRemoveEffect(RemoveEffectCB, this);
	}

	if (m_pcsAgcmObject)
	{
		m_pcsAgcmObject->SetCallbackPreRemoveData(RemoveCommonObjectCB, this);
		m_pcsAgcmObject->SetCallbackInitObject(EventInitCB, this);
		m_pcsAgcmObject->SetCallbackWorkObject(EventObjectWorkCB, this);
	}

	if (m_pcsAgcmCharacter)
	{
		m_pcsAgcmCharacter->SetCallbackPreRemoveCharacter(RemoveCommonObjectCB, this);
		m_pcsAgcmCharacter->SetCallbackPreRemoveCharacter(PreRemoveCharacterCB, this);
		m_pcsAgcmCharacter->SetCallbackInitCharacter(EventInitCB, this);
		m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CharacterSetSelfCB, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnim(EventCharAnimCB, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnimWait(EventCharAnimWaitCB, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnimWalk(EventCharAnimWalkCB, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnimSocial(EventCharAnimSocialCB, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnimRun(EventCharAnimRunCB, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnimAttack(EventCharAnimAttackCB, this);
		m_pcsAgcmCharacter->SetCallbackAttackSuccess(EventCharAttackSuccessCB, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnimDead(EventCharAnimDeadCB, this);
		m_pcsAgcmCharacter->SetCallbackDisplayActionResult(EventCharActionResult, this);
		m_pcsAgcmCharacter->SetCallbackChangeAnimSkill(EventSkillCB, this); // 스킬이 애니메이션이 있는 경우
		m_pcsAgcmCharacter->SetCallbackUpdateMovement(EventCharUpdateMovemenntCB, this);
		m_pcsAgcmCharacter->SetCallbackEndSocialAnim(EventCharEndSocialAnimCB, this);
		m_pcsAgcmCharacter->SetCallbackWaveFxTrigger(EventWaveFxTrigger, this); 
	}

	if (m_pcsAgcmItem)
	{
		m_pcsAgcmItem->SetCallbackPreRemoveData(RemoveCommonObjectCB, this);
		m_pcsAgcmItem->SetCallbackFieldItem(EventInitCB, this);
		m_pcsAgcmItem->SetCallbackEquipItem(EventItemEquipCB, this);
		m_pcsAgcmItem->SetCallbackUnequipItem(EventItemUnequipCB, this);
		m_pcsAgcmItem->SetCallbackDetachedItem(EventItemDetachedCB, this);
	}

	if (m_pcsAgpmTimer)
	{
		m_pcsAgpmTimer->SetCallbackCheckHour(EventObjectCheckHourCB, this);
	}	
	
	if (m_pcsAgpmItemConvert)
	{
		m_pcsAgpmItemConvert->SetCallbackAdd(EventItemUpdateConvertHistoryCB, this);
		m_pcsAgpmItemConvert->SetCallbackUpdate(EventItemUpdateConvertHistoryCB, this);
	}

	if (m_pcsAgpmSkill)
	{
		m_pcsAgpmSkill->SetCallbackReceiveAdditionalEffect(EventCharAdditionalEffect, this);
	}

	if (m_pcsAgcmSkill)
	{
		m_pcsAgcmSkill->SetCallbackNoAnimSkill(EventSkillCB, this);		// 스킬이 애니메이션이 없는 경우
		m_pcsAgcmSkill->SetCallbackAddBuffedSkillList(EventSkillAddBuffedListCB, this);
		m_pcsAgcmSkill->SetCallbackRemoveBuffedSkillList(EventSkillRemoveBuffedListCB, this);
		m_pcsAgcmSkill->SetCallbackAddtionalHitEffect(EventSkillAdditionalEffectCB, this);
	}

	return TRUE;
}

BOOL AgcmEventEffect::OnIdle(UINT32 ulClockCount)
{
	m_ulNowClockCount = ulClockCount;
	return TRUE;
}

BOOL AgcmEventEffect::OnDestroy()
{
	m_ExclusiveEffectsMgr.Relase();

	return TRUE;
}

static INT32	CBCompareNodeY(const void *pvData1, const void *pvData2)
{
	RwFrame **	ppstData1 = (RwFrame **) pvData1;
	RwFrame **	ppstData2 = (RwFrame **) pvData2;

	RwMatrix *	pstMat1	= *ppstData1 ? RwFrameGetLTM(*ppstData1) : NULL ;
	RwMatrix *	pstMat2 = *ppstData2 ? RwFrameGetLTM(*ppstData2) : NULL ;

	return INT32( ( pstMat1 ? pstMat1->pos.y : 0.0f ) - ( pstMat2 ? pstMat2->pos.y : 0.0f ) );
}

float GetHeight( const RwSphere& cSphere, float fScale = 1.f )
{
	return ( cSphere.center.y + (cSphere.radius + 50.0f) ) * fScale;
}

BOOL AgcmEventEffect::SetStandardEffectUseInfo(stEffUseInfo *pstInfo, AgcmStartEventEffectParams *pcsParams, INT32 lNodeID, INT32 lTargetOption)
{
	AgcdCharacter* pcsAgcdOriginChar = NULL;
	RpHAnimHierarchy	*pcsNewHierarchy	= NULL;
	
	switch( lTargetOption )
	{
	case E_AGCD_EVENT_EFFECT_CDI_TARGET_OPTION_ALL:
	case E_AGCD_EVENT_EFFECT_CDI_TARGET_OPTION_FIRST:
		{
			pstInfo->m_pFrmParent	= pcsParams->m_csTarget.m_pcsFrame;
			pstInfo->m_pClumpParent	= pcsParams->m_csTarget.m_pcsClump;
			pstInfo->m_pFrmTarget	= pcsParams->m_csOrigin.m_pcsFrame;

			if ( pcsParams->m_csOrigin.m_pcsBase )
			{
				pcsAgcdOriginChar = m_pcsAgcmCharacter->GetCharacterData((AgpdCharacter*) pcsParams->m_csOrigin.m_pcsBase);

				if ( pcsAgcdOriginChar && pcsAgcdOriginChar->m_pPickAtomic )
					if ( pcsAgcdOriginChar->m_pPickAtomic != pcsAgcdOriginChar->m_pPickAtomic->next )
						pstInfo->m_pFrmTarget = RpAtomicGetFrame(pcsAgcdOriginChar->m_pPickAtomic);
			}			

			pstInfo->m_pBase			= pcsParams->m_csTarget.m_pcsBase;

			pcsNewHierarchy				= pcsParams->m_csTarget.m_pcsHierarchy;
		}
		break;

	case E_AGCD_EVENT_EFFECT_CDI_TARGET_OPTOIN_CHAIN:
		{
			if (pcsParams->m_csPreTarget.m_pcsBase)
			{
				pstInfo->m_pFrmParent		= pcsParams->m_csPreTarget.m_pcsFrame;
				pstInfo->m_pClumpParent		= pcsParams->m_csPreTarget.m_pcsClump;
				pstInfo->m_pFrmTarget		= pcsParams->m_csTarget.m_pcsFrame;
				pstInfo->m_pBase			= pcsParams->m_csOrigin.m_pcsBase;

				pcsNewHierarchy				= pcsParams->m_csPreTarget.m_pcsHierarchy;
			}
			else
			{
				pstInfo->m_pFrmParent		= pcsParams->m_csOrigin.m_pcsFrame;
				pstInfo->m_pClumpParent		= pcsParams->m_csOrigin.m_pcsClump;
				pstInfo->m_pFrmTarget		= pcsParams->m_csTarget.m_pcsFrame;
				pstInfo->m_pBase			= pcsParams->m_csOrigin.m_pcsBase;

				pcsNewHierarchy				= pcsParams->m_csOrigin.m_pcsHierarchy;
			}
		}
		break;

	default :
		{
			pstInfo->m_pFrmParent		= pcsParams->m_csOrigin.m_pcsFrame;
			pstInfo->m_pClumpParent		= pcsParams->m_csOrigin.m_pcsClump;
			pstInfo->m_pFrmTarget		= pcsParams->m_csTarget.m_pcsFrame;
			pstInfo->m_pBase			= pcsParams->m_csOrigin.m_pcsBase;

			pcsNewHierarchy				= pcsParams->m_csOrigin.m_pcsHierarchy;
		}
		break;
	}

	if ( lNodeID > -1 )
	{
		pstInfo->m_ulFlags |= stEffUseInfo::E_FLAG_LINKTOPARENT;

		if ( lNodeID > 0 )
		{
			RwInt32 lNodeIndex;

			if ( pcsNewHierarchy )
			{
				lNodeIndex = RpHAnimIDGetIndex(pcsNewHierarchy, lNodeID);
				if ( lNodeIndex < 0 )
				{
					ASSERT(pstInfo->m_pClumpParent);
					if ( pstInfo->m_pClumpParent )
					{
						RpHAnimHierarchy* pcsParentHierarchy = m_pcsAgcmCharacter->GetHierarchy(pstInfo->m_pClumpParent);
						if ( pcsParentHierarchy )
							lNodeIndex = RpHAnimIDGetIndex(pcsNewHierarchy, lNodeID);
					}
				}

				if ( (lNodeIndex > -1) && (pcsNewHierarchy->pNodeInfo[lNodeIndex].pFrame) )
					pstInfo->m_pFrmParent = pcsNewHierarchy->pNodeInfo[lNodeIndex].pFrame;
			}
			else
			{
				if ( pstInfo->m_pClumpParent )
				{
					RpHAnimHierarchy* pcsParentHierarchy = m_pcsAgcmCharacter->GetHierarchy(pstInfo->m_pClumpParent);
					if ( pcsParentHierarchy )
					{
						lNodeIndex = RpHAnimIDGetIndex(pcsParentHierarchy, lNodeID);

						if ( lNodeIndex > -1 && (pcsParentHierarchy->pNodeInfo[lNodeIndex].pFrame) )
							pstInfo->m_pFrmParent = pcsParentHierarchy->pNodeInfo[lNodeIndex].pFrame;
					}
				}
			}
		}
	}	
	else if ( lNodeID == -2 && pstInfo->m_pClumpParent )
	{
		pstInfo->m_ulFlags |= stEffUseInfo::E_FLAG_LINKTOPARENT;

		if( pstInfo->m_lTargetCID )
		{
			AgpdCharacter * pcsTarget = m_pcsAgpmCharacter->GetCharacter( pstInfo->m_lTargetCID );
			if( pcsTarget )
			{
				AgcdCharacter* pcdTarget = m_pcsAgcmCharacter->GetCharacterData( pcsTarget );
				if( pcdTarget )
				{	
					if( pstInfo->m_lOwnerCID == 0 && pcdTarget->m_pClump)
						pstInfo->m_v3dCenter.y = GetHeight( pcdTarget->m_pClump->stType.boundingSphere, pcdTarget->m_pstAgcdCharacterTemplate->m_fScale );
					else
						pstInfo->m_v3dCenter.y = GetHeight( pstInfo->m_pClumpParent->stType.boundingSphere, pcdTarget->m_pstAgcdCharacterTemplate->m_fScale );
				}
			}
		}
		else
		{
			pstInfo->m_v3dCenter.y = GetHeight( pstInfo->m_pClumpParent->stType.boundingSphere, pstInfo->m_fScale );
		}		
	}
	
	else if ( lNodeID == -3 )
	{
		RpHAnimHierarchy* pHierarchy = pcsNewHierarchy ? pcsNewHierarchy : pcsParams->m_csOrigin.m_pcsHierarchy;
		if( pHierarchy && !IsBadReadPtr( pcsParams->m_csOrigin.m_pcsHierarchy , sizeof( RpHAnimHierarchy ) ) )
		{
			const INT32	lNumSampling = 5;

			RwFrame*	apstNodeFrame[lNumSampling];
			INT32		lNumNode = pHierarchy->numNodes;
			for ( INT32 lIndex = 0; lIndex < lNumSampling; ++lIndex )
				apstNodeFrame[lIndex] = pHierarchy->pNodeInfo[rand() % lNumNode].pFrame;

			qsort( apstNodeFrame, lNumSampling, sizeof(RwFrame *), CBCompareNodeY );

			pstInfo->m_ulFlags |= stEffUseInfo::E_FLAG_LINKTOPARENT;

			pstInfo->m_pFrmParent = apstNodeFrame[(lNumSampling + 1) / 2 - 1];
		}
	}
	//@{ kday 20051010
	// ;)
	// "뱀파이어 기사"와 같은 공중에 떠있는 몹의 경우 단순한 옵셋만으로
	// 미사일 타격이펙트의 위치를 결정하기 어렵다.
	// 즉 이펙트의 위치는 모델툴에서 셋팅된 옵셋.(x,y,z)
	// 깊이 값에 hitRange를 더함.(y)
	// 바운딩스피어 중심의 높이 값을 더함.(z)
	
	else if ( lNodeID == -4 && pstInfo->m_pClumpParent )
	{
		bool bMultiAtomics = false;

		if ( pcsAgcdOriginChar )
		{
			if ( pcsAgcdOriginChar->m_pPickAtomic != pcsAgcdOriginChar->m_pPickAtomic->next )
			{
				pstInfo->m_v3dCenter.x += pcsAgcdOriginChar->m_pPickAtomic->boundingSphere.center.x * pcsAgcdOriginChar->m_pstAgcdCharacterTemplate->m_fScale;
				pstInfo->m_v3dCenter.y += pcsAgcdOriginChar->m_pPickAtomic->boundingSphere.center.y * pcsAgcdOriginChar->m_pstAgcdCharacterTemplate->m_fScale;
				pstInfo->m_v3dCenter.z += pcsAgcdOriginChar->m_pPickAtomic->boundingSphere.center.z * pcsAgcdOriginChar->m_pstAgcdCharacterTemplate->m_fScale;

				bMultiAtomics = true;
			}
		}
		
		if ( !bMultiAtomics && pcsAgcdOriginChar )
		{
			if( pstInfo->m_lTargetCID )
			{
				AgpdCharacter * pcsTarget = m_pcsAgpmCharacter->GetCharacter( pstInfo->m_lTargetCID );
				if( pcsTarget )
				{
					AgcdCharacter * pcdTarget = m_pcsAgcmCharacter->GetCharacterData( pcsTarget );
					if( pcdTarget )
					{	
						pstInfo->m_v3dCenter.y += pstInfo->m_pClumpParent->stType.boundingSphere.center.y * pcdTarget->m_pstAgcdCharacterTemplate->m_fScale;
					}
				}
			}
			else
			{
				pstInfo->m_v3dCenter.y += pstInfo->m_pClumpParent->stType.boundingSphere.center.y * pcsAgcdOriginChar->m_pstAgcdCharacterTemplate->m_fScale;
			}
		}
	}
	else if (lNodeID == -5)
	{
		AuMath::V3DSubtract((AuPOS *) &pstInfo->m_v3dCenter, (AuPOS *) &pcsParams->m_csTarget.m_stPosition, (AuPOS *) &pstInfo->m_v3dCenter);
	}
	
	return NULL;
}

BOOL AgcmEventEffect::SetBaseEffectUseInfo(stEffUseInfo* pstInfo, AgcdUseEffectSetData* pstData, AgcmStartEventEffectParams* pcsParams)
{
	pstInfo->m_ulEffID				= pstData->m_ulEID;

	pstInfo->m_v3dCenter.x			= (pstData->m_pv3dOffset) ? (pstData->m_pv3dOffset->x) : (0.0f);
	pstInfo->m_v3dCenter.y			= (pstData->m_pv3dOffset) ? (pstData->m_pv3dOffset->y) : (0.0f);
	pstInfo->m_v3dCenter.z			= (pstData->m_pv3dOffset) ? (pstData->m_pv3dOffset->z) : (0.0f);

	pstInfo->m_fScale				= (pstData->m_fScale * pcsParams->m_fScale);

	pstInfo->m_ulDelay				= pcsParams->m_fTimeScale ? (RwUInt32)(((FLOAT)(pstData->m_ulStartGap)) / pcsParams->m_fTimeScale) : pstData->m_ulStartGap;
	pstInfo->m_ulLife				= pcsParams->m_lLife;
	pstInfo->m_fTimeScale			= pcsParams->m_fTimeScale;
	pstInfo->m_fParticleNumScale	= pstData->m_fParticleNumScale;
	pstInfo->m_rgbScale				= pstData->m_rgbScale;
	
	//화살은 노드를 참조하므로.. 힛트래인지를 사용하지 않는다.
	if ( pstData->m_lParentNodeID != -3 )
		pstInfo->m_v3dCenter.z		+= pcsParams->m_fRange;

	if ( pcsParams->m_bSelfCharacter )
		DEF_FLAG_ON(pstInfo->m_ulFlags, stEffUseInfo::E_FLAG_MAINCHARAC);

	pstInfo->m_lOwnerCID			= pcsParams->m_lCID;
	pstInfo->m_lTargetCID			= pcsParams->m_lTCID;
	pstInfo->m_pNoticeCBClass		= pcsParams->m_pvClass;
	pstInfo->m_fptrNoticeCB			= pcsParams->m_pfCallback;
	pstInfo->m_lCustID				= pcsParams->m_lCustID;
	pstInfo->m_pHierarchy			= pcsParams->m_csOrigin.m_pcsHierarchy;

	if ( pstData->m_bAtomicEmitter )
	{
		pstInfo->m_ulFlags			|= stEffUseInfo::E_FLAG_EMITER_WITH_ATOMIC;
		pstInfo->m_pAtomicEmiter	= pcsParams->m_csOrigin.m_pcsAtomic;
	}
	else if ( pstData->m_bClumpEmitter )
	{
		pstInfo->m_ulFlags			|= stEffUseInfo::E_FLAG_EMITER_WITH_CLUMP;
		pstInfo->m_pClumpEmiter		= pcsParams->m_csOrigin.m_pcsClump;	
	}

	RtQuatInit(&pstInfo->m_quatRotation, 0, 0, 0, 1);

	if ( pstData->m_pcsRotation )
	{
		RwMatrix matRot;
		RwMatrixSetIdentity(&matRot);
		matRot.right = pstData->m_pcsRotation->m_stRight;
		matRot.up = pstData->m_pcsRotation->m_stUp;
		matRot.at = pstData->m_pcsRotation->m_stAt;
		RtQuatConvertFromMatrix(&(pstInfo->m_quatRotation), &matRot);
	}

	return TRUE;
}

eAcReturnType AgcmEventEffect::CheckCustomData(INT32 *alCustDataIndex, stEffUseInfo *pstInfo, AgcmStartEventEffectParams *pcsParams, BOOL bRepetition)
{
	if (bRepetition)
	{
		if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION] == -1)
			return E_AC_RETURN_TYPE_SKIP;

		if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION] == E_AGCD_EVENT_EFFECT_CDI_TARGET_OPTION_FIRST)
			return E_AC_RETURN_TYPE_SKIP;
	}

	if (pcsParams->m_lExceptionType > 0)
	{
		if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE] != pcsParams->m_lExceptionType)
			return E_AC_RETURN_TYPE_SKIP;
	}
	else
	{
		if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE] > 0)
			return E_AC_RETURN_TYPE_SKIP;
	}

	if (pcsParams->m_lExceptionTypeCustData > 0)
	{
		if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE_CUST_DATA] != pcsParams->m_lExceptionTypeCustData)
			return E_AC_RETURN_TYPE_SKIP;
	}
	else
	{
		if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE_CUST_DATA] > 0)
			return E_AC_RETURN_TYPE_SKIP;
	}

	if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_USER] > -1)
	{
		switch (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_USER])
		{
		case E_AGCD_EVENT_EFFECT_CDI_USER_TYPE_CHAR_PC:
			{
				if (pcsParams->m_csOrigin.m_pcsBase->m_eType != APBASE_TYPE_CHARACTER)
					return E_AC_RETURN_TYPE_SKIP;
				
				if (!m_pcsAgpmCharacter->IsPC((AgpdCharacter *)(pcsParams->m_csOrigin.m_pcsBase)))
					return E_AC_RETURN_TYPE_SKIP;
			}
			break;

		case E_AGCD_EVENT_EFFECT_CDI_USER_TYPE_CHAR_NPC:
			{
				if (pcsParams->m_csOrigin.m_pcsBase->m_eType != APBASE_TYPE_CHARACTER)
					return E_AC_RETURN_TYPE_SKIP;
				
				if (!m_pcsAgpmCharacter->IsNPC((AgpdCharacter *)(pcsParams->m_csOrigin.m_pcsBase)))
					return E_AC_RETURN_TYPE_SKIP;
			}
			break;

		case E_AGCD_EVENT_EFFECT_CDI_USER_TYPE_CHAR_MONSTER:
			{
				if (pcsParams->m_csOrigin.m_pcsBase->m_eType != APBASE_TYPE_CHARACTER)
					return E_AC_RETURN_TYPE_SKIP;

				if (!m_pcsAgpmCharacter->IsMonster((AgpdCharacter *)(pcsParams->m_csOrigin.m_pcsBase)))
						return E_AC_RETURN_TYPE_SKIP;
			}
			break;

		case E_AGCD_EVENT_EFFECT_CDI_USER_TYPE_CHAR_GM:
			{
				if (pcsParams->m_csOrigin.m_pcsBase->m_eType != APBASE_TYPE_CHARACTER)
					return E_AC_RETURN_TYPE_SKIP;

				if (!m_pcsAgpmCharacter->IsGM((AgpdCharacter *)(pcsParams->m_csOrigin.m_pcsBase)))
					return E_AC_RETURN_TYPE_SKIP;
			}
			break;
		}
	}

	if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_LINK_SKILL] > -1)
	{
		AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pcsParams->m_csOrigin.m_pcsBase);
		AgcdCharacter	*pcsAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
		ApBase			*pcsTemplateBase	= pcsAgpdCharacter->m_pcsCharacterTemplate;
		AgcdSkillADChar* pcsAgcdSkillADChar	= m_pcsAgcmSkill->GetCharacterAttachData(pcsParams->m_csOrigin.m_pcsBase);
		if ((pcsAgpdCharacter) && (pcsTemplateBase) && (pcsAgcdCharacter) && (pcsAgcdSkillADChar))
		{
			AgcdSkillAttachTemplateData* pstAttachedData = m_pcsAgcmSkill->GetAttachTemplateData(pcsTemplateBase);
			INT32 lUsableIndex = m_pcsAgcmSkill->GetUsableSkillTNameIndex( pcsTemplateBase, alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_LINK_SKILL] );
			if ( pstAttachedData && lUsableIndex >= 0 )
			{
				pcsAgcdSkillADChar->m_lSkillUsableIndex			= lUsableIndex;

				if( pstAttachedData->m_pacsSkillVisualInfo &&
					pstAttachedData->m_pacsSkillVisualInfo[lUsableIndex] &&
					pstAttachedData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT] )
				{
					m_pcsAgcmCharacter->StartAnimation( pcsAgpdCharacter, AGCMCHAR_ANIM_TYPE_SKILL, 
														pstAttachedData->m_pacsSkillVisualInfo[lUsableIndex]->m_pacsAnimation[AGCMCHAR_AT2_COMMON_DEFAULT] );
				}
				else
				{
					SetBaseSkillEventEffect(pcsAgpdCharacter);
				}
			}
		}

		return E_AC_RETURN_TYPE_SKIP;
	}

	if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH] > -1)
	{
		pstInfo->m_lCustData		= alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH];
		pstInfo->m_fptrNoticeCB		= pcsParams->m_pfDestroyMatchCB ? pcsParams->m_pfDestroyMatchCB : NoticeEffectSkillDestroyMatchCB;
		pstInfo->m_pNoticeCBClass	= this;
	}

	if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CALC_DIST_OPTION] > -1)
	{
		if ((pcsParams->m_csOrigin.m_pcsFrame) && (pcsParams->m_csTarget.m_pcsFrame))
		{
			RwV3d	*pv3dPos1, *pv3dPos2, v3dDist;

			LockFrame();
			pv3dPos1	= RwMatrixGetPos(RwFrameGetLTM(pcsParams->m_csOrigin.m_pcsFrame));
			pv3dPos2	= RwMatrixGetPos(RwFrameGetLTM(pcsParams->m_csTarget.m_pcsFrame));
			UnlockFrame();

			RwV3dSub(&v3dDist, pv3dPos1, pv3dPos2);
			RwReal fDist = RwV3dLength(&v3dDist);

			pstInfo->m_ulDelay += (INT32)(fDist * (FLOAT)(alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CALC_DIST_OPTION] / AGCMEVENTEFFECT_DIST_TO_GAP_OFFSET));
		}
	}

	if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_STRUCK_ANIM] > -1)
	{
		pstInfo->m_lCustData		= alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_STRUCK_ANIM];
		pstInfo->m_fptrNoticeCB		= NoticeEffectSetTargetStruckAnimCB;
		pstInfo->m_pNoticeCBClass	= this;
	}

	if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION] > -1)
	{
		switch (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION])
		{
		case E_AGCD_EVENT_EFFECT_CDI_DIRECTION_OPTION_ORG_TO_TAR:
			{
				pstInfo->m_ulFlags	|= stEffUseInfo::E_FLAG_DIR_PAR_TO_TAR;
				pstInfo->m_ulFlags	|= stEffUseInfo::E_FLAG_DIR_IGN_HEIGHT;
			}
			break;

		case E_AGCD_EVENT_EFFECT_CDI_DIRECTION_OPTION_TAR_TO_ORG:
			{
				pstInfo->m_ulFlags	|= stEffUseInfo::E_FLAG_DIR_TAR_TO_PAR;
				pstInfo->m_ulFlags	|= stEffUseInfo::E_FLAG_DIR_IGN_HEIGHT;
			}
			break;

		case E_AGCD_EVENT_EFFECT_CDI_DIRECTION_OPTION_ORG_TO_TAR_HEIGHT:
			{
				pstInfo->m_ulFlags	|= stEffUseInfo::E_FLAG_DIR_PAR_TO_TAR;
			}
			break;

		case E_AGCD_EVENT_EFFECT_CDI_DIRECTION_OPTION_TAR_TO_ORG_HEIGHT:
			{
				pstInfo->m_ulFlags	|= stEffUseInfo::E_FLAG_DIR_TAR_TO_PAR;
			}
			break;
		}
	}

	if (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT] > -1)
	{
		switch (alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT])
		{
		case E_AGCD_EVENT_EFFECT_CDI_ACTION_OBJ_HIDE_SHIELD_WEAPONS:
			{
				HideShieldAndWeaponsWhileCurrentAnimationDuration((AgpdCharacter *)(pcsParams->m_csOrigin.m_pcsBase));
			}
			break;
		}
	}

	return E_AC_RETURN_TYPE_SUCCESS;
}

eAcReturnType AgcmEventEffect::StartEffect(	AgcdUseEffectSetData			*pcsData,
											AgcmStartEventEffectParams		*pcsParams,											
											EFFCTRL_SET						**ppstNode,
											BOOL							bAddList,
											AES_EFFCTRL_SET_LIST			**ppCurrentNodeInfo,
											AgcmEventEffectParamFunction	pfCheckSkipping,
											BOOL							bSkillDestroyMatch)
{
	INT32 alCustDataIndex[D_AGCD_EFFECT_MAX_CUST_DATA];	
	
	if ( pcsData->m_pszCustData )
		GetCustDataIndex(pcsData->m_pszCustData, alCustDataIndex);
	else
		memset(alCustDataIndex, -1, sizeof(INT32) * D_AGCD_EFFECT_MAX_CUST_DATA);

	if ( (pfCheckSkipping) && (pfCheckSkipping(this, (PVOID)(alCustDataIndex), (PVOID)(pcsParams)))	)
		return E_AC_RETURN_TYPE_SKIP;	

	if( bSkillDestroyMatch && alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION] == 1)
	{
		AgpdFactor* pFactor = &((AgpdCharacter*)(pcsParams->m_csTarget.m_pcsBase))->m_pcsCharacterTemplate->m_csFactor;
		INT	lRange	= 0;
		m_pcsAgpmFactors->GetValue( pFactor, &lRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE );
		pcsParams->m_fRange = (FLOAT)lRange;
	}

	stEffUseInfo* pstInfo = new stEffUseInfo;
	SetBaseEffectUseInfo(pstInfo, pcsData, pcsParams);

	// 좀더 확실히 계산하자. 다른 타격도 올라가버린다.
	if ( E_AC_RETURN_TYPE_SKIP == CheckCustomData( alCustDataIndex, pstInfo, pcsParams, pcsParams->m_csPreTarget.m_pcsBase ? TRUE : FALSE ) )
	{
		DEF_SAFEDELETE( pstInfo );
		return E_AC_RETURN_TYPE_SKIP;
	}

	if ( pcsParams->m_csTarget.m_pcsClump && 
		(pcsParams->m_csTarget.m_pcsClump->stType.eType & ACUOBJECT_TYPE_CHARACTER) && 
		pcsParams->m_csTarget.m_pcsBase )
	{
		if( pcsParams->m_csTarget.m_pcsBase->m_eType == APBASE_TYPE_CHARACTER )
		{
			AgcdCharacter* pstAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData((AgpdCharacter*)(pcsParams->m_csTarget.m_pcsBase));
			RwV3dScale( &pstInfo->m_stMissileTargetInfo.m_v3dCenter, &pcsParams->m_csTarget.m_pcsClump->stType.boundingSphere.center, pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale );
		}
	}

	SetStandardEffectUseInfo( pstInfo, pcsParams, pcsData->m_lParentNodeID, alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION] );

	if ( ppCurrentNodeInfo )
	{
		if ( *ppCurrentNodeInfo )
		{
			// 아래와 같이 하면, 두번째부터 계속 덮어쓰게 되어... 중간 EffectSet들이 List에 존재하지 않게 된다. (Parn, 04/10/17 20:40)
			// 그래서 while을 사용해서 Next가 NULL일때까지 계속 Next따라간다. 나중에 수정 바람.
			AES_EFFCTRL_SET_LIST* pcsNext		= *ppCurrentNodeInfo;
			(*ppCurrentNodeInfo)				= new AES_EFFCTRL_SET_LIST;
			(*ppCurrentNodeInfo)->m_pcsNext		= pcsNext;
		}
		else
		{
			(*ppCurrentNodeInfo)				= new AES_EFFCTRL_SET_LIST;
		}
	}

	//. 로딩이 완료되면 EffCtrlSet*를 설정할 List와 Node정보 설정.
	if(!ppCurrentNodeInfo)
	{
		if(ppstNode)
		{
			pstInfo->m_bIsAddUpdateList = FALSE;
			pstInfo->m_pcsData = (void*)(*ppstNode);
		}
	}

	SetEffect( pstInfo, GetEventEffectList(pstInfo->m_pBase), (ppCurrentNodeInfo) ? (&((*ppCurrentNodeInfo)->m_pstNode)) : ppstNode, FALSE );

	return E_AC_RETURN_TYPE_SUCCESS;
}

BOOL AgcmEventEffect::StartEventEffect(AgcmStartEventEffectParams *pcsParams, INT32	lForceIndex, AgcmEventEffectParamFunction pfCheckSkipping, BOOL bSkillDestroyMatch)
{
	INT32 alCustDataIndex[D_AGCD_EFFECT_MAX_CUST_DATA];
	memset( alCustDataIndex, -1, sizeof(INT32) * D_AGCD_EFFECT_MAX_CUST_DATA );

	static AgpmSiegeWar* pAgpmSiegeWar = (AgpmSiegeWar*) GetModule("AgpmSiegeWar");
	ASSERT( pAgpmSiegeWar );

	//KTH Model툴은 class를 Regist를 하지 않기때문에 약간 변경..
	static AgcmUIOption* pAgcmUIOption = ( AgcmUIOption * ) GetModule( "AgcmUIOption" );
	if( pAgcmUIOption )
		DisableCastingEffect( pAgcmUIOption->m_eEffectCombat == AGCD_OPTION_EFFECT_OFF ? TRUE : FALSE );

	AgpdCharacter* pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdSiegeWar* pcsSiegeWar		= pcsSelfCharacter ? pAgpmSiegeWar->GetSiegeWarInfo( pcsSelfCharacter ) : NULL;

	AgcdUseEffectSetList	*pcsCurrent				= pcsParams->m_pstAgcdUseEffectSet->m_pcsHead;
	AES_EFFCTRL_SET_LIST	**ppCurrentNodeInfo		= &pcsParams->m_pcsNodeInfo;

	//. 현재 AgpdCharacter에 설정되어 있는 skyset flag.
	ACharacterSkyFlags		*pstAgcdSkySetFlag		= AgcmEventEffect::m_pcsThisAgcmEventEffect->GetSkySetFlagData( (AgpdCharacter*)pcsParams->m_csOrigin.m_pcsBase);

	RwReal	castSkillEffTimeScale = pcsParams->m_fTimeScale;
	while (pcsCurrent)
	{
		if (!(pcsCurrent->m_csData.m_ulConditionFlags & pcsParams->m_ulConditionFlags))
		{
			pcsCurrent	= pcsCurrent->m_pcsNext;
			continue;
		}

		pcsParams->m_fTimeScale = pcsCurrent->m_csData.m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_CAST ? castSkillEffTimeScale : 1.f;
		GetCustDataIndex( pcsCurrent->m_csData.m_pszCustData, alCustDataIndex );
		
		//Sound Effect
		if (	(pcsCurrent->m_csData.m_pszSoundName) &&
				(pcsCurrent->m_csData.m_pszSoundName[0]) &&
				(!pcsParams->m_lExceptionType) &&
				(!(pcsParams->m_ulFlags & AgcmStartEventEffectParams::E_ASEEP_SKIP_PLAY_SOUND))	)
		{
			PROFILE("StartEventEffect - Sound");
			PlaySound( AGCMEVENTEFFECT_EFFECT_SOUND_PATH, AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH, pcsCurrent->m_csData.m_pszSoundName, pcsParams->m_bSelfCharacter, pcsParams->m_csOrigin.m_pcsClump );
		}

		//Graphic Effect
		BOOL bStart = TRUE;
		if( pstAgcdSkySetFlag && alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_NATURE_CONDITION] > -1 )
		{
			if( FALSE == pstAgcdSkySetFlag->m_Flags || 
				alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_NATURE_CONDITION] != pstAgcdSkySetFlag->m_Flags )
				bStart = FALSE;
		}

		if ( m_bCastingEffectDisabled && alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CASTING_EFFECT] == 1 )
			bStart = FALSE;

		if( pcsCurrent->m_csData.m_ulEID && bStart )
		{
			if( pcsCurrent->m_csData.m_lParentNodeID == -3 && NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker != -1 )
			{
				if( NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW && 
					NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW )
					return TRUE;
			}

			if( E_AC_RETURN_TYPE_FAILURE == StartEffect( &pcsCurrent->m_csData, pcsParams, NULL, TRUE, ppCurrentNodeInfo, pfCheckSkipping, bSkillDestroyMatch ) )
				return FALSE;
		}

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmEventEffect::EventInitCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect					*pThis	= (AgcmEventEffect *)(pClass);
	ApBase							*pBase	= (ApBase *)(pData);

	AgcmStartEventEffectParams		csParams;

	switch(pBase->m_eType)
	{
	case APBASE_TYPE_OBJECT:
		{
			ApdObject	*ppObject			= (ApdObject *)(pBase);
			csParams.m_pstAgcdUseEffectSet	= pThis->GetUseEffectSet(ppObject->m_pcsTemplate);
			if (!csParams.m_pstAgcdUseEffectSet) return FALSE;
			if (!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT)) return TRUE; // 해당 이펙트가 없다.

			AgcdObject	*pcObject	= pThis->m_pcsAgcmObject->GetObjectData(ppObject);
			if ((!pcObject) || (!pcObject->m_stGroup.m_pstList) || (!pcObject->m_stGroup.m_pstList->m_csData.m_pstClump)) return FALSE;

			csParams.m_csOrigin.m_pcsClump		= pcObject->m_stGroup.m_pstList->m_csData.m_pstClump;
			csParams.m_csOrigin.m_pcsFrame		= RpClumpGetFrame(pcObject->m_stGroup.m_pstList->m_csData.m_pstClump);
			if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

			csParams.m_csOrigin.m_pcsHierarchy	= pcObject->m_stGroup.m_pstList->m_csData.m_pstInHierarchy;
			csParams.m_eType					= APBASE_TYPE_OBJECT;
		}
		break;

	case APBASE_TYPE_CHARACTER:
		{
			AgpdCharacter	*ppObject		= (AgpdCharacter *)(pBase);
			csParams.m_pstAgcdUseEffectSet	= pThis->GetUseEffectSet(ppObject->m_pcsCharacterTemplate);
			if (!csParams.m_pstAgcdUseEffectSet) return FALSE;

			AgcdCharacter	*pcObject		= pThis->m_pcsAgcmCharacter->GetCharacterData(ppObject);
			if ((!pcObject) || (!pcObject->m_pClump)) return FALSE;

			if (pcObject->m_lStatus & AGCMCHAR_STATUS_FLAG_NEW_CREATE)
			{
				if (pThis->m_pcsAgpmCharacter->IsPC((AgpdCharacter *) ppObject))
				{
					if (!pThis->SetCommonCharEffect((ApBase *)(ppObject), E_COMMON_CHAR_TYPE_SPAWN)) return FALSE;
				}
			}

			UINT64 ulOldStatus = AGPDCHAR_SPECIAL_STATUS_TRANSPARENT;		// 투명은 이렇게 세팅해서 넣어준다. 안그러면 나왔다가 없어짐.

			PVOID	pvParam[ 2 ];
			pvParam[ 0 ] = &ulOldStatus;
			
			EventCharSpecialStatus((PVOID)ppObject, pThis, &pvParam);

			if (((AgpdCharacter *) ppObject)->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_EVENT_GIFTBOX)
			{
				eAgcmEventEffectDataCommonCharType eEffectType = E_COMMON_CHAR_TYPE_GIFT_BOX;

				// 일단 북미에서는 이벤트 때문에 GIFT_FIRE 를 보여줘야 한다. 2007.08.09. steeple
				if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
					eEffectType = E_COMMON_CHAR_TYPE_GIFT_FIRE;
				else
				{
					AgcdUseEffectSetData* pstEffectSetData = pThis->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID(pThis->m_pcsAgpmCharacter->GetEventEffectID());
					if(pstEffectSetData)
						eEffectType = (eAgcmEventEffectDataCommonCharType)(pstEffectSetData->m_lID-1);
				}

				if (!pThis->SetCommonCharEffect((ApBase *)(ppObject), eEffectType)) return FALSE;
			}

			if (!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT))
				return TRUE; // 해당 이펙트가 없다.

			//csParams.m_fScale					= pcObject->m_pstAgcdCharacterTemplate->m_fScale;
			csParams.m_csOrigin.m_pcsClump		= pcObject->m_pClump;
			csParams.m_csOrigin.m_pcsFrame		= RpClumpGetFrame(pcObject->m_pClump);
			if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

			csParams.m_csOrigin.m_pcsHierarchy	= pcObject->m_pInHierarchy;

			if (pThis->GetSelfCID() == ppObject->m_lID)
				csParams.m_bSelfCharacter	= TRUE;

			csParams.m_eType = csParams.m_bSelfCharacter ? APBASE_TYPE_NONE : APBASE_TYPE_CHARACTER;		//. AddLoadEntry을 위한 Type 저장
		}
		break;

	case APBASE_TYPE_ITEM:
		{
			AgpdItem	*ppObject			= (AgpdItem *)(pBase);
			csParams.m_pstAgcdUseEffectSet	= pThis->GetUseEffectSet((AgpdItemTemplate *)(ppObject->m_pcsItemTemplate));
			if (!csParams.m_pstAgcdUseEffectSet) return FALSE;
			if (!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT)) return TRUE; // 해당 이펙트가 없다.

			AgcdItem	*pcObject			= NULL;
			
			if (pThis->m_pcsAgcmItem)
				pcObject = pThis->m_pcsAgcmItem->GetItemData(ppObject);

			if ((!pcObject) || (!pcObject->m_pstClump)) return FALSE;

			csParams.m_csOrigin.m_pcsClump		= pcObject->m_pstClump;
			csParams.m_csOrigin.m_pcsFrame		= RpClumpGetFrame(pcObject->m_pstClump);
			if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

			csParams.m_csOrigin.m_pcsHierarchy	= pcObject->m_pstHierarchy;
			csParams.m_eType					= ppObject->m_pcsCharacter == pThis->m_pcsAgcmCharacter->GetSelfCharacter() ? APBASE_TYPE_NONE : APBASE_TYPE_ITEM;
		}
		break;

	default:
		return TRUE;
	}

	if (csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_CHECK_TIME)
	{
		pThis->SetCheckTimeDataList(csParams.m_pstAgcdUseEffectSet, pBase);
	}

	csParams.m_csOrigin.m_pcsBase			= pBase;
	csParams.m_ulConditionFlags				= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT;
	
	return pThis->StartEventEffect(&csParams);
}

BOOL AgcmEventEffect::SetBaseEffectParams(ApdObject *pstApdObject, AgcmStartEventEffectParams *pcsParams)
{
	AgcdObject* pstAgcdObject = m_pcsAgcmObject->GetObjectData(pstApdObject);
	if( !pstAgcdObject || !pstAgcdObject->m_stGroup.m_pstList )
		return FALSE;

	// Group의 첫째 놈을 기준으로 한다.
	pcsParams->m_csOrigin.m_pcsBase			= (ApBase *)(pstApdObject);
	pcsParams->m_csOrigin.m_pcsClump		= pstAgcdObject->m_stGroup.m_pstList->m_csData.m_pstClump;
	pcsParams->m_csOrigin.m_pcsFrame		= pstAgcdObject->m_stGroup.m_pstList->m_csData.m_pstClump ? RpClumpGetFrame( pstAgcdObject->m_stGroup.m_pstList->m_csData.m_pstClump ) : NULL;
	pcsParams->m_csOrigin.m_pcsHierarchy	= pstAgcdObject->m_stGroup.m_pstList->m_csData.m_pstInHierarchy;

	return TRUE;
}

BOOL AgcmEventEffect::SetBaseEffectParams(AgpdCharacter *pstAgpdCharacter, AgcmStartEventEffectParams *pcsParams)
{
	AgcdCharacter* pstAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	if ( !pstAgcdCharacter || !pstAgcdCharacter->m_pClump )		return FALSE;

	pcsParams->m_csOrigin.m_pcsBase			= (ApBase *)(pstAgpdCharacter);
	pcsParams->m_csOrigin.m_pcsClump		= pstAgcdCharacter->m_pClump;
	pcsParams->m_csOrigin.m_pcsFrame		= pstAgcdCharacter->m_pClump ? RpClumpGetFrame(pstAgcdCharacter->m_pClump) : NULL;
	pcsParams->m_csOrigin.m_pcsHierarchy	= pstAgcdCharacter->m_pInHierarchy;

	AgpdCharacter* pstAgpdTarget = m_pcsAgpmCharacter->GetCharacter(pstAgpdCharacter->m_alTargetCID[0]);
	if( !pstAgpdTarget )
		pstAgpdTarget = m_pcsAgpmCharacter->GetCharacter(pcsParams->m_lTCID);

	if( pstAgpdTarget )
	{
		AgcdCharacter* pstAgcdTarget = m_pcsAgcmCharacter->GetCharacterData(pstAgpdTarget);
		if ( !pstAgcdTarget || !pstAgcdTarget->m_pClump )		return FALSE;

		pcsParams->m_csTarget.m_pcsBase			=	(ApBase *)(pstAgpdTarget);
		pcsParams->m_csTarget.m_pcsClump		=	pstAgcdTarget->m_pClump;
		pcsParams->m_csTarget.m_pcsFrame		=	pstAgcdTarget->m_pClump ? RpClumpGetFrame( pstAgcdTarget->m_pClump ) : NULL;
		pcsParams->m_csTarget.m_pcsHierarchy	=	pstAgcdTarget->m_pInHierarchy;
	}

	pcsParams->m_bSelfCharacter				=	pstAgpdCharacter->m_lID == m_lSelfCID ? TRUE : FALSE;

	return TRUE;
}

BOOL AgcmEventEffect::SetBaseEffectParams(AgpdItem *pstAgpdItem, AgcmStartEventEffectParams *pcsParams)
{
	if( !m_pcsAgcmItem )		return FALSE;

	AgcdItem	*pstAgcdItem		= m_pcsAgcmItem->GetItemData(pstAgpdItem);
	if( !pstAgcdItem )			return FALSE;
		
	pcsParams->m_csOrigin.m_pcsBase		= (ApBase *)(pstAgpdItem);

	if( pstAgcdItem->m_csAttachedAtomics.pcsList )
	{
		RpAtomic* pstAtomic	= pstAgcdItem->m_csAttachedAtomics.pcsList->m_pstAtomic;
		if (!pstAtomic)			return FALSE;

		pcsParams->m_csOrigin.m_pcsAtomic		= pstAtomic;
		pcsParams->m_csOrigin.m_pcsClump		= RpAtomicGetClump(pstAtomic);
		pcsParams->m_csOrigin.m_pcsFrame		= RpAtomicGetFrame(pstAtomic);
		pcsParams->m_csOrigin.m_pcsHierarchy	= pstAgcdItem->m_csAttachedAtomics.m_pstHierarchy;
	}
	else
	{
		RpClump* pstClump = pstAgcdItem->m_pstClump;
		if( !pstClump )		return FALSE;

		pcsParams->m_csOrigin.m_pcsClump		= pstClump;
		pcsParams->m_csOrigin.m_pcsFrame		= RpClumpGetFrame(pstClump);
		pcsParams->m_csOrigin.m_pcsHierarchy	= pstAgcdItem->m_pstHierarchy;
	}

	if( pstAgpdItem->m_pcsCharacter )
		pcsParams->m_bSelfCharacter = pstAgpdItem->m_pcsCharacter->m_lID == m_lSelfCID ? TRUE : FALSE;

	return TRUE;
}

BOOL AgcmEventEffect::SetBaseEffect(ApBase *pcsBase, AgcdUseEffectSetData *pcsData, UINT32 ulLife, BOOL bAddList, EFFCTRL_SET **ppstNode)
{
	AgcmStartEventEffectParams	csParams;
	csParams.m_lLife	= ulLife;
	csParams.m_lTCID	= pcsData ? pcsData->m_lTargetCID : 0;

	switch (pcsBase->m_eType)
	{
	case APBASE_TYPE_OBJECT:
		{
			SetBaseEffectParams((ApdObject *)(pcsBase), &csParams);
			csParams.m_eType = APBASE_TYPE_OBJECT;			//. AddLoadEntry을 위한 Type 저장 (Map상의 Object..)
		}
		break;

	case APBASE_TYPE_CHARACTER:
		{
			AgpdCharacter *pdCharacter = (AgpdCharacter *)(pcsBase);
			SetBaseEffectParams(pdCharacter, &csParams);
			csParams.m_eType = pdCharacter == m_pcsAgcmCharacter->GetSelfCharacter() ? APBASE_TYPE_NONE : APBASE_TYPE_CHARACTER;
		}
		break;

	case APBASE_TYPE_ITEM:
		{
			AgpdItem *pdItem = (AgpdItem *)(pcsBase);
			SetBaseEffectParams(pdItem, &csParams);
			csParams.m_eType = pdItem->m_pcsCharacter == m_pcsAgcmCharacter->GetSelfCharacter() ? APBASE_TYPE_NONE : APBASE_TYPE_ITEM;
		}
		break;

	default:
		return FALSE;
	}

	return StartEffect(pcsData, &csParams, ppstNode, bAddList);
}

BOOL AgcmEventEffect::SetBaseObjectEventEffect(ApdObject *pcsApdObject, UINT32 ulConditionFlags)
{
	if (!pcsApdObject->m_pcsTemplate) return FALSE;

	AgcdUseEffectSet			*pstAgcdUseEffectSet		= GetUseEffectSet(pcsApdObject->m_pcsTemplate);
	if (!pstAgcdUseEffectSet) return FALSE;
	if (!(pstAgcdUseEffectSet->m_ulConditionFlags & ulConditionFlags)) return TRUE; // 해당 조건이 없다.

	AgcdObject					*pcsAgcdObject				= m_pcsAgcmObject->GetObjectData(pcsApdObject);	
	if ((!pcsAgcdObject) || (!pcsAgcdObject->m_stGroup.m_pstList) || (!pcsAgcdObject->m_stGroup.m_pstList->m_csData.m_pstClump)) return FALSE;

	AgcmStartEventEffectParams	csParams;
	csParams.m_csOrigin.m_pcsClump		= pcsAgcdObject->m_stGroup.m_pstList->m_csData.m_pstClump;
	csParams.m_csOrigin.m_pcsFrame		= RpClumpGetFrame(pcsAgcdObject->m_stGroup.m_pstList->m_csData.m_pstClump);
	if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

	csParams.m_ulConditionFlags			= ulConditionFlags;
	csParams.m_pstAgcdUseEffectSet		= pstAgcdUseEffectSet;
	csParams.m_csOrigin.m_pcsBase		= (ApBase *)(pcsApdObject);
	csParams.m_csOrigin.m_pcsHierarchy	= pcsAgcdObject->m_stGroup.m_pstList->m_csData.m_pstInHierarchy;

	return StartEventEffect(&csParams);
}

BOOL AgcmEventEffect::SetCommonCharacterEffect(ApBase *pcsBase, eAgcmEventEffectDataCommonCharType eCommonCharType, INT32 lTargetCID)
{
	return pcsBase ? SetCommonCharEffect(pcsBase, eCommonCharType, lTargetCID) : FALSE;
}

BOOL AgcmEventEffect::RemoveCommonCharacterEffect(ApBase *pcsBase, eAgcmEventEffectDataCommonCharType eCommonCharType)
{
	if( !m_pcsAgcmEff2 || !pcsBase )		return FALSE;

	UINT32		aulCommonCharTypeEID[AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA];
	memset( aulCommonCharTypeEID, 0, sizeof(UINT32) );

	INT32 lCommonCharTypeIndex = 0;
	INT32 lCount;

	// eCommonCharType에 해당하는 effect ID를 설정한다.
	for (lCount = 0, lCommonCharTypeIndex = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
	{
		AgcdUseEffectSetData* pcsUseEffectSetData = m_csAgcmEventEffectData.GetCommonCharEffectData( eCommonCharType, lCount );
		if ( !pcsUseEffectSetData || !pcsUseEffectSetData->m_ulEID )
			break;

		aulCommonCharTypeEID[lCommonCharTypeIndex++] = pcsUseEffectSetData->m_ulEID;
	}

	// BASE에 붙어 있는 EFFECT LIST를 가져온다.
	AgcmEventEffectList		*pcsAgcmEventEffectList	= GetEventEffectList(pcsBase);
	ListUseEffectIter		Iter					=	pcsAgcmEventEffectList->m_pListUseEffect->begin();
	EFFCTRL_SET*			pCtrlSet				=	NULL;

	for( ; Iter != pcsAgcmEventEffectList->m_pListUseEffect->end() ; ++Iter )
	{
		pCtrlSet		=	(*Iter);

		if (!pCtrlSet)
			return FALSE;

		if (!pCtrlSet->GetPtrEffSet())
			return FALSE;

		// 리스트에서 effect ID를 가져온다.
		INT32 lEID = pCtrlSet->GetPtrEffSet()->bGetID();
		for (lCount = 0; lCount < lCommonCharTypeIndex; ++lCount)
		{
			if (aulCommonCharTypeEID[lCount] == lEID)
			{ 
				// 리스트에서 가져온 effect ID가 찾는 effect ID라면 EFFECT MODULE을 이용해 삭제하고 for문을 빠져 나온다.
				pcsAgcmEventEffectList->RemoveUseEffectList( pCtrlSet );
				m_pcsAgcmEff2->RemoveEffSet( pCtrlSet, FALSE );
				
				if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
				{
					AgpdCharacter*  pdCharacter = (AgpdCharacter *) pcsBase;
					AgcdCharacter*	cdCharacter = (pdCharacter) ? m_pcsAgcmCharacter->GetCharacterData(pdCharacter) : NULL;
					if(cdCharacter && cdCharacter->m_pClump)
					{	
						//. ref. AgcmRender::RenderAtomic.
						switch(eCommonCharType)
						{
						case E_COMMON_CHAR_TYPE_STUN:
						case E_COMMON_CHAR_TYPE_SLOW:
						case E_COMMON_CHAR_TYPE_POISON:
							m_pcsAgcmRender->ResetLighting(cdCharacter->m_pClump);
							break;
						}
					}
				}


				if (pcsBase->m_eType == APBASE_TYPE_OBJECT)
				{
					AgcdObject *			pstAgcdObject = m_pcsAgcmObject->GetObjectData((ApdObject *) pcsBase);
					AgcdObjectGroupList *	pstOGroupList;
					AgcdObjectGroupData *	pstOGroupData;

					pstOGroupList = pstAgcdObject->m_stGroup.m_pstList;

					while(pstOGroupList)
					{
						pstOGroupData = &pstOGroupList->m_csData;

						if (pstOGroupData->m_pstClump)
							m_pcsAgcmRender->ResetLighting(pstOGroupData->m_pstClump);

						pstOGroupList = pstOGroupList->m_pstNext;
					}
				}

				return TRUE;
			}

		}

	}

	return TRUE;
}

BOOL AgcmEventEffect::SetBaseCharacterEventEffect(AgpdCharacter *pcsAgpdCharacter, UINT32 ulConditionFlags)
{
	AgcdCharacter* pcsAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	return pcsAgcdCharacter ? SetBaseCharacterEventEffect(pcsAgpdCharacter, pcsAgcdCharacter, ulConditionFlags) : FALSE;
}

BOOL AgcmEventEffect::SetBaseCharacterEventEffect(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, UINT32 ulConditionFlags)
{
	if (!pcsAgpdCharacter->m_pcsCharacterTemplate) return FALSE;

	AgcmStartEventEffectParams	csParams;

	csParams.m_pstAgcdUseEffectSet					= GetUseEffectSet(pcsAgpdCharacter->m_pcsCharacterTemplate);
	if (!csParams.m_pstAgcdUseEffectSet) return FALSE;
	if (!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & ulConditionFlags)) return TRUE; // 해당 조건이 없다.
	if (!pcsAgcdCharacter->m_pClump) return FALSE;

	csParams.m_csOrigin.m_pcsClump					= pcsAgcdCharacter->m_pClump;
	csParams.m_csOrigin.m_pcsFrame					= RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

	csParams.m_csOrigin.m_pcsHierarchy				= pcsAgcdCharacter->m_pInHierarchy;
	if (!csParams.m_csOrigin.m_pcsHierarchy) return FALSE;

	if (pcsAgpdCharacter->m_alTargetCID[0])
	{
		AgpdCharacter		*pcsAgpdTarget			= m_pcsAgpmCharacter->GetCharacter(pcsAgpdCharacter->m_alTargetCID[0]);
		if (pcsAgpdTarget)
		{ 
			AgcdCharacter	*pcsAgcdTarget			= m_pcsAgcmCharacter->GetCharacterData(pcsAgpdTarget);
			if ((!pcsAgcdTarget) || (!pcsAgcdTarget->m_pClump)) return FALSE;

			csParams.m_csTarget.m_pcsClump			= pcsAgcdTarget->m_pClump;
			csParams.m_csTarget.m_pcsFrame			= RpClumpGetFrame(pcsAgcdTarget->m_pClump);
			if (!csParams.m_csTarget.m_pcsFrame) return FALSE;

			csParams.m_csTarget.m_pcsHierarchy		= pcsAgcdTarget->m_pInHierarchy;
			if (!csParams.m_csTarget.m_pcsHierarchy) return FALSE;

			csParams.m_csTarget.m_pcsBase			= (ApBase *)(pcsAgpdTarget);
		}
	}

	if (pcsAgpdCharacter->m_lID == m_lSelfCID)
		csParams.m_bSelfCharacter					= TRUE;

	csParams.m_ulConditionFlags						= ulConditionFlags;
	csParams.m_csOrigin.m_pcsBase					= (ApBase *)(pcsAgpdCharacter);

	csParams.m_lCID									= pcsAgpdCharacter->m_lID;
	csParams.m_lTCID								= pcsAgpdCharacter->m_alTargetCID[0];

	csParams.m_pfCallback							= NoticeEffectDestroyMatchCB;
	csParams.m_pfDestroyMatchCB						= NoticeEffectDestroyMatchCB;

	return StartEventEffect(&csParams);
}

BOOL AgcmEventEffect::SetBaseEquipItemEventEffect(AgpdItem *pcsAgpdItem, UINT32 ulConditionFlags, BOOL bSelfCharacter, INT32 lLife, ApBase *pcsTarget, RpClump *pstTargetClump, RwFrame *pstTargetFrame, RpHAnimHierarchy *pstTargetHierarchy, FLOAT fTimeScale)
{
	AgcdUseEffectSet* pstAgcdUseEffectSet = GetUseEffectSet((ApBase *)(pcsAgpdItem->m_pcsItemTemplate));
	if( !pstAgcdUseEffectSet )											return FALSE;
	if( !(pstAgcdUseEffectSet->m_ulConditionFlags & ulConditionFlags))	return TRUE;

	return SetBaseEquipItemEventEffect(pcsAgpdItem, pstAgcdUseEffectSet, ulConditionFlags, bSelfCharacter, lLife, pcsTarget, pstTargetClump, pstTargetFrame, pstTargetHierarchy, fTimeScale);
}

BOOL AgcmEventEffect::SetBaseEquipItemEventEffect(AgpdItem *pcsAgpdItem, AgcdUseEffectSet *pstAgcdUseEffectSet, UINT32 ulConditionFlags, BOOL bSelfCharacter, INT32 lLife, ApBase *pcsTarget, RpClump *pstTargetClump, RwFrame *pstTargetFrame, RpHAnimHierarchy *pstTargetHierarchy, FLOAT fTimeScale)
{
	if( pcsAgpdItem->m_pcsCharacter->m_bIsTrasform )		return TRUE;

	AgcdItem* pstAgcdItem = m_pcsAgcmItem ? m_pcsAgcmItem->GetItemData(pcsAgpdItem) : NULL;
	if( !pstAgcdItem )		return FALSE;

	AgcmStartEventEffectParams	csParams;
	csParams.m_ulFlags						= bSelfCharacter ? AgcmStartEventEffectParams::E_ASEEP_SKIP_PLAY_SOUND : AgcmStartEventEffectParams::E_ASEEP_NONE;
	csParams.m_ulConditionFlags				= ulConditionFlags;
	csParams.m_pstAgcdUseEffectSet			= pstAgcdUseEffectSet;
	csParams.m_bSelfCharacter				= bSelfCharacter;
	csParams.m_lLife						= lLife;
	csParams.m_fTimeScale					= fTimeScale;
	csParams.m_csOrigin.m_pcsBase			= (ApBase *)(pcsAgpdItem);
	csParams.m_csOrigin.m_pcsHierarchy		= pstAgcdItem->m_csAttachedAtomics.m_pstHierarchy;
	csParams.m_csTarget.m_pcsBase			= pcsTarget;
	csParams.m_csTarget.m_pcsClump			= pstTargetClump;
	csParams.m_csTarget.m_pcsFrame			= pstTargetFrame;
	csParams.m_csTarget.m_pcsHierarchy		= pstTargetHierarchy;

	AttachedAtomicList* pcsCurrent = pstAgcdItem->m_csAttachedAtomics.pcsList;
	while (pcsCurrent)
	{
		if (!pcsCurrent->m_pstAtomic) return FALSE;

		csParams.m_csOrigin.m_pcsClump					= RpAtomicGetClump(pcsCurrent->m_pstAtomic);
		csParams.m_csOrigin.m_pcsFrame					= RpAtomicGetFrame(pcsCurrent->m_pstAtomic);
		if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;
		if (!StartEventEffect(&csParams)) return FALSE;

		pcsCurrent = pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmEventEffect::SetBaseUsableItemEventEffect(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, UINT32 ulConditionFlags)
{
	AgcdUseEffectSet			*pstAgcdUseEffectSet	= GetUseEffectSet((ApBase *)(pcsAgpdItem->m_pcsItemTemplate));
	if (!pstAgcdUseEffectSet) return FALSE;
	if (!(pstAgcdUseEffectSet->m_ulConditionFlags & ulConditionFlags)) return TRUE; // 해당 조건이 없당...

	AgcdCharacter				*pcsAgcdCharacter		= m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if ((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump)) return FALSE;
	
	AgcmStartEventEffectParams	csParams;
	csParams.m_ulConditionFlags			= ulConditionFlags;
	csParams.m_pstAgcdUseEffectSet		= pstAgcdUseEffectSet;
	csParams.m_csOrigin.m_pcsBase		= (ApBase *)(pcsAgpdCharacter);
	csParams.m_bSelfCharacter			= ((pcsAgpdCharacter->m_lID == m_lSelfCID) ? (TRUE) : (FALSE));
	csParams.m_csOrigin.m_pcsClump		= pcsAgcdCharacter->m_pClump;
	csParams.m_csOrigin.m_pcsFrame		= RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	csParams.m_csOrigin.m_pcsHierarchy	= pcsAgcdCharacter->m_pInHierarchy;

	if ((!csParams.m_csOrigin.m_pcsFrame) || (!csParams.m_csOrigin.m_pcsHierarchy)) return FALSE;
	if (!StartEventEffect(&csParams)) return FALSE;

	return TRUE;
}

BOOL AgcmEventEffect::CBCheckSkipSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2)
{
	if ((!pClass) || (!pData1) || (!pData2))
		return FALSE;

	AgcmEventEffect				*pcsThis		= (AgcmEventEffect *)(pClass);
	INT32						*alCustData		= (INT32 *)(pData1);
	AgcmStartEventEffectParams	*pcsParams		= (AgcmStartEventEffectParams *)(pData2);

	return alCustData[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION] > 0 ? TRUE : FALSE;
}

BOOL AgcmEventEffect::SetBaseCustomEventEffect(AgpdCharacter *pcsAgpdCharacter, UINT UIEffectID, float fWaveHeightOffset)
{
	AgcdCharacter* pcsAgcdCharacter = m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if( !pcsAgcdCharacter )		return FALSE;

	RpClump* pClump = pcsAgcdCharacter->m_pRide ? pcsAgcdCharacter->m_pRide->m_pClump : pcsAgcdCharacter->m_pClump;
	if( !pClump )				return FALSE;

	stEffUseInfo stUseInfo;
	stUseInfo.m_ulEffID		= UIEffectID;			//. 수정요함. 하드코딩된 effect index.
	stUseInfo.m_ulLife		= -1;					//. 툴에서 지정한 값을 그대로 사용.
	stUseInfo.m_v3dCenter.y	= fWaveHeightOffset;	//. 높이 오프셋값 설정.
	stUseInfo.m_pFrmParent	= RpClumpGetFrame(pClump/*pcsAgcdCharacter->m_pRide->m_pClump*/);
	stUseInfo.m_fScale		= pcsAgcdCharacter->m_pRide ? 0.6f : 0.3f;	//. 스케일링

	m_pcsAgcmEff2->UseEffSet( &stUseInfo );		

	return TRUE;
}

BOOL AgcmEventEffect::SetBaseSkillEventEffect(AgpdCharacter *pcsAgpdCharacter)
{	
	if (!pcsAgpdCharacter->m_pcsCharacterTemplate) return FALSE;

	AgcdSkillAttachTemplateData	*pstSkillData		= m_pcsAgcmSkill->GetAttachTemplateData((ApBase *)(pcsAgpdCharacter->m_pcsCharacterTemplate));
	if (!pstSkillData) return FALSE;

	AgcdCharacter				*pcsAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if ((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump)) return FALSE;

	AgcdSkillADChar* pcsAgcdSkillADChar = m_pcsAgcmSkill->GetCharacterAttachData((ApBase *)(pcsAgpdCharacter));
	if (!pcsAgcdSkillADChar) return FALSE;

	if (	(!pstSkillData->m_pacsSkillVisualInfo) ||
			(!pstSkillData->m_pacsSkillVisualInfo[pcsAgcdSkillADChar->m_lSkillUsableIndex])	) return TRUE;	// skip.

#ifndef USE_MFC
	AgcmUIOption* pcmUIOption = ( AgcmUIOption* )GetModule( "AgcmUIOption" );
	if( !pcmUIOption ) return FALSE;
#endif

	AgpdCharacter* ppdSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	AgcmStartEventEffectParams	csParams;

	if(m_pcsAgpmSkill)
	{
		csParams.m_fTimeScale -= m_pcsAgpmSkill->GetAdjustCastDelay(pcsAgpdCharacter) * 0.01f;
	}

	csParams.m_pstAgcdUseEffectSet			= pstSkillData->m_pacsSkillVisualInfo[pcsAgcdSkillADChar->m_lSkillUsableIndex]->m_pastEffect[pcsAgcdCharacter->m_lCurAnimType2];
    if (	!csParams.m_pstAgcdUseEffectSet || 
			(csParams.m_pstAgcdUseEffectSet) &&
			(!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET))	) // AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET가 없으면...
		csParams.m_pstAgcdUseEffectSet		= pstSkillData->m_pacsSkillVisualInfo[pcsAgcdSkillADChar->m_lSkillUsableIndex]->m_pastEffect[AGCMCHAR_AT2_COMMON_DEFAULT];

	if (	(!csParams.m_pstAgcdUseEffectSet) ||
			(!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET))	)
		return TRUE; // skip.

	csParams.m_ulConditionFlags				= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;

	csParams.m_csOrigin.m_pcsClump			= pcsAgcdCharacter->m_pClump;
	csParams.m_csOrigin.m_pcsFrame			= RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

	csParams.m_csOrigin.m_pcsBase			= (ApBase *)(pcsAgpdCharacter);
	csParams.m_lCID							= pcsAgpdCharacter->m_lID;
	csParams.m_csOrigin.m_pcsHierarchy		= pcsAgcdCharacter->m_pInHierarchy;
	csParams.m_bSelfCharacter				= (pcsAgpdCharacter->m_lID == m_lSelfCID ? TRUE : FALSE);
	csParams.m_pvClass						= m_pcsAgcmSkill;
	csParams.m_pfCallback					= AgcmSkill::ProcessNoticeEffect;
	csParams.m_lCustID						= pcsAgcdSkillADChar->m_lSkillID;
	if (pcsAgcdCharacter->m_pstSkillActionData)
	{
		AuMATRIX	stInv;
		AuV3D		v3dTemp;

		AuMath::MatrixInverse(&stInv, &pcsAgpdCharacter->m_stMatrix);
		AuMath::V3DSubtract(&v3dTemp, &pcsAgcdCharacter->m_pstSkillActionData->csTargetPos, &pcsAgpdCharacter->m_stPos);
		AuMath::V3DTransform(&csParams.m_csTarget.m_stPosition, &v3dTemp, &stInv);
	}

	for (INT16 nSkillTarget = 0; nSkillTarget < AGPDCHARACTER_MAX_TARGET; ++nSkillTarget)
	{
		if (pcsAgpdCharacter->m_alTargetCID[nSkillTarget])
		{
			AgpdCharacter *pstAgpdTarget		= m_pcsAgpmCharacter->GetCharacter(pcsAgpdCharacter->m_alTargetCID[nSkillTarget]);
			if (!pstAgpdTarget)
				continue;

			csParams.m_csTarget.m_pcsBase		= (ApBase *)(pstAgpdTarget);

			AgcdCharacter *pstAgcdTarget		= m_pcsAgcmCharacter->GetCharacterData(pstAgpdTarget);
			if ((!pstAgcdTarget) || (!pstAgcdTarget->m_pClump)) return FALSE;

			csParams.m_csTarget.m_pcsClump		= pstAgcdTarget->m_pClump;
			csParams.m_csTarget.m_pcsFrame		= RpClumpGetFrame(pstAgcdTarget->m_pClump);
			if (!csParams.m_csTarget.m_pcsFrame) return FALSE;

			csParams.m_csTarget.m_pcsHierarchy	= pstAgcdTarget->m_pInHierarchy;
			csParams.m_lTCID					= pcsAgpdCharacter->m_alTargetCID[nSkillTarget];

#ifdef USE_MFC
			if (!StartEventEffect(&csParams)) return FALSE;
#else
			if( !m_pcsAgpmCharacter->IsArchlord( pcsAgpdCharacter ) )
			{
				// 내 캐릭터의 이펙트를 끄도록 설정되었으면..
				if( ppdSelfCharacter->m_lID == pcsAgpdCharacter->m_lID && pcmUIOption->IsDisableMySkillEffect() )
				{
					// 꺼준다.
					SetOnlySoundSkill( &csParams );
				}
				// 내 캐릭터가 아니면..
				else if( ppdSelfCharacter->m_lID != pcsAgpdCharacter->m_lID && pcmUIOption->IsDisableOtherSkillEffect() )
				{
					// 꺼준다.
					SetOnlySoundSkill( &csParams );
				}
				else if( !StartEventEffect( &csParams ) ) 
				{
					return FALSE;
				}
			}
			else if (!StartEventEffect(&csParams))
			{
				return FALSE;
			}
#endif

			csParams.m_csPreTarget				= csParams.m_csTarget;
		}
		else if (nSkillTarget == 0)
		{
#ifdef USE_MFC
			if (!StartEventEffect(&csParams))
			{
				return FALSE;
			}
#else
			if( !m_pcsAgpmCharacter->IsArchlord( pcsAgpdCharacter ) )
			{
				// 내 캐릭터의 이펙트를 끄도록 설정되었으면..
				if( ppdSelfCharacter->m_lID == pcsAgpdCharacter->m_lID && pcmUIOption->IsDisableMySkillEffect() )
				{
					// 꺼준다.
					SetOnlySoundSkill( &csParams );
				}
				// 내 캐릭터가 아니면..
				else if( ppdSelfCharacter->m_lID != pcsAgpdCharacter->m_lID && pcmUIOption->IsDisableOtherSkillEffect() )
				{
					// 꺼준다.
					SetOnlySoundSkill( &csParams );
				}
				else if( !StartEventEffect( &csParams ) )
				{
					return FALSE;
				}
			}
			else if( !StartEventEffect( &csParams, -1, CBCheckSkipSkillEffect ) )
			{
				return FALSE;
			}
#endif

			break;
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::SetPassiveSkill(AgpdCharacter *pcsAgpdCharacter, AgcdUseEffectSet *pstAgcdUseEffectSet, AgcmStartEventEffectParams *pcsParams, INT32 lExceptionType, INT32 lLife , BOOL bForce )
{	
	AgcdCharacter *pcsAgcdCharacter = m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if ((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump)) return FALSE;

	AgcmUIOption* pcmUIOption = ( AgcmUIOption* )GetModule( "AgcmUIOption" );
	if( !pcmUIOption ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	pcsParams->m_pstAgcdUseEffectSet					= pstAgcdUseEffectSet;
	pcsParams->m_ulConditionFlags						= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;

	pcsParams->m_csOrigin.m_pcsBase						= (ApBase *)(pcsAgpdCharacter);
	pcsParams->m_csOrigin.m_pcsClump					= pcsAgcdCharacter->m_pClump;
	pcsParams->m_csOrigin.m_pcsFrame					= RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	if (!pcsParams->m_csOrigin.m_pcsFrame) return FALSE;

	pcsParams->m_lTCID									= pcsParams->m_csOrigin.m_pcsBase->m_lID;
	pcsParams->m_csOrigin.m_pcsHierarchy				= pcsAgcdCharacter->m_pInHierarchy;
	pcsParams->m_bSelfCharacter							= (pcsAgpdCharacter->m_lID == m_lSelfCID ? TRUE : FALSE);
	pcsParams->m_pvClass								= m_pcsAgcmSkill;
	pcsParams->m_pfCallback								= AgcmSkill::ProcessNoticeEffect;
	pcsParams->m_csTarget								= pcsParams->m_csOrigin;
	pcsParams->m_lExceptionType							= lExceptionType;
	pcsParams->m_lLife									= lLife;

#ifndef USE_MFC
	if( !bForce && !m_pcsAgpmCharacter->IsArchlord( pcsAgpdCharacter ) )
	{
		// 내 캐릭터의 이펙트를 끄도록 설정되었으면..
		if( ppdSelfCharacter->m_lID == pcsAgpdCharacter->m_lID && pcmUIOption->IsDisableMySkillEffect() )
		{
			// 꺼준다.
			return SetOnlySoundSkill( pcsParams );
		}
		// 내 캐릭터가 아니면..
		else if( ppdSelfCharacter->m_lID != pcsAgpdCharacter->m_lID && pcmUIOption->IsDisableOtherSkillEffect() )
		{
			// 꺼준다.
			return SetOnlySoundSkill( pcsParams );
		}
	}
#endif

	return StartEventEffect( pcsParams );
}

BOOL AgcmEventEffect::RemoveAllConvertedItemGradeEventEffect(AgcdEventEffectItem *pcsData)
{
	if( !m_pcsAgcmEff2 )		return FALSE;

	AES_EFFCTRL_SET_LIST	*pcsList	= NULL;
	if( !pcsData->m_csConvertedItemGradEffect.GetNode( AGCMEVENTEFFECT_CONVERTED_ITEM_G_OFFSET, &pcsList ) )
		return FALSE;

	while (pcsList)
	{
		if (pcsList->m_pstNode)
		{
			m_pcsAgcmEff2->RemoveEffSet(pcsList->m_pstNode, FALSE);
			pcsList->m_pstNode	= NULL;
		}

		pcsList	= pcsList->m_pcsNext;
	}

	return pcsData->m_csConvertedItemGradEffect.RemoveAllNode();
}

BOOL AgcmEventEffect::RemoveAllContinuativeSkillEventEffect(AgcdEventEffectCharacter *pcsData)
{
	while (1)
	{
		INT32 lSTID = pcsData->m_csContinuativeSkillList.GetHeadKey();
		if (lSTID == -1)
			break;

		RemoveContinuativetSkillEventEffect(pcsData, lSTID);
	}

	return TRUE;
}

BOOL AgcmEventEffect::RemoveContinuativetSkillEventEffect(AgcdEventEffectCharacter *pcsData, INT32 lSTID)
{
	if( !m_pcsAgcmEff2 )		return FALSE;

	AES_EFFCTRL_SET_LIST	*pcsList	= NULL;
	if( !pcsData->m_csContinuativeSkillList.GetNode( lSTID, &pcsList ) )
		return FALSE;

	while (pcsList)
	{
		if (pcsList->m_pstNode)
		{			
			BOOL bLockFrame = pcsList->m_pstNode->GetFrame() ? TRUE : FALSE;
			if( bLockFrame ) LockFrame();
			m_pcsAgcmEff2->RemoveEffSet(pcsList->m_pstNode, TRUE);
			if( bLockFrame ) UnlockFrame();
			pcsList->m_pstNode	= NULL;
		}

		pcsList	= pcsList->m_pcsNext;
	}

	return pcsData->m_csContinuativeSkillList.RemoveNode(lSTID);
}

BOOL AgcmEventEffect::EventObjectWorkCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis			= (AgcmEventEffect *)(pClass);
	ApdObject		*pcsApdObject	= (ApdObject *)(pData);

	return pThis->SetBaseObjectEventEffect(pcsApdObject, AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK);
}

BOOL AgcmEventEffect::EventObjectCheckHourCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect			*pcsThis		= (AgcmEventEffect *)(pClass);
	UINT8					cCurHour		= (UINT8)(pData);

	if (!pcsThis->AddEffectSet(pcsThis->m_pacsCheckTimeDataList[E_AGCD_EVENT_EFFECT_CHECK_TIME_ON][cCurHour])) return FALSE;
	if (!pcsThis->RemoveEffectSet(pcsThis->m_pacsCheckTimeDataList[E_AGCD_EVENT_EFFECT_CHECK_TIME_OFF][cCurHour])) return FALSE;

	return TRUE;
}

BOOL AgcmEventEffect::EventCharAnimCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pData) || (!pClass) || (!pCustData))
		return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pstAgpdCharacter	= (AgpdCharacter *)(pData);
	AgcdCharacter	*pstAgcdCharacter	= (AgcdCharacter *)(pCustData);

	if( pstAgcdCharacter->m_eCurAnimType >= AGCMCHAR_MAX_ANIM_TYPE ) 	return TRUE;
	if( pstAgcdCharacter->m_eCurAnimType <= AGCMCHAR_ANIM_TYPE_NONE )	return TRUE;
	if( !pstAgcdCharacter->m_pClump )									return FALSE;

	AgcaAnimation2*	pcsAgcaAnimation2 = pThis->m_pcsAgcmCharacter->GetAgcaAnimation2();
	AEE_CharAnimAttachedData *pcsAttachedData = (AEE_CharAnimAttachedData*)(pcsAgcaAnimation2->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pstAgcdCharacter->m_pcsCurAnimData ) );
	if( !pcsAttachedData )		return TRUE;

	for( AEE_CharAnimAttachedData::SoundListItr	iter = pcsAttachedData->GetList().begin(); iter != pcsAttachedData->GetList().end(); ++iter )
	{
		if ( !(*iter).m_strSoundName.empty() && E_AEE_CAASD_CONDITION_NONE == (*iter).m_unConditions )
		{
			pThis->PlaySound( AGCMEVENTEFFECT_EFFECT_SOUND_PATH, AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH, (*iter).m_strSoundName.c_str(),
							pstAgpdCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE, pstAgcdCharacter->m_pClump, FALSE, pcsAttachedData );
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::EventCharAnimWaitCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	BOOL br	= TRUE;
	PROFILE("AgcmEventEffect::AnimWaitCB");

	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);
	AgcdCharacter	*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);

	// 깃발이펙트 ㅡㅡ
	AgpdItem* pcsAgpdLancerItem	= GetEquipItem( pThis->m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_LANCER );
	if( pcsAgpdLancerItem && (static_cast<AgpdItemTemplateEquipWeapon*>(pcsAgpdLancerItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD ) )
	{
		if((!pcsAgpdLancerItem->m_pcsCharacter) || (!pcsAgpdLancerItem->m_pcsItemTemplate))
			return FALSE;

		if(	!pThis->SetBaseEquipItemEventEffect(
			pcsAgpdLancerItem,
			AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT,
			(pcsAgpdLancerItem->m_pcsCharacter->m_lID == pThis->m_lSelfCID) ? (TRUE) : (FALSE),
			-1,
			NULL,
			NULL,
			NULL,
			NULL,
			pcsAgcdCharacter->m_afAnimSpeedRate[pcsAgcdCharacter->m_eCurAnimType]	)	)
		{
			br	= FALSE;
		}
	}
	
	br	&= pThis->SetBaseCharacterEventEffect(pcsAgpdCharacter, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT);
	return br;
}

BOOL AgcmEventEffect::EventCharAnimWalkCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);
	AgcdCharacter	*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);

	if (!pThis->SetBaseCharacterEventEffect(pcsAgpdCharacter, pcsAgcdCharacter, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK)) return FALSE;
	if (!pThis->PlayCharacterMovingSound(pcsAgpdCharacter, pcsAgcdCharacter)) return FALSE;

	return TRUE;
}

BOOL AgcmEventEffect::EventCharAnimRunCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	BOOL br = TRUE;
	if ((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);
	AgcdCharacter	*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);

	
	AgpdItem* pcsAgpdLancerItem	= GetEquipItem(pThis->m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_LANCER);
	if( pcsAgpdLancerItem && (static_cast<AgpdItemTemplateEquipWeapon*>(pcsAgpdLancerItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD ) )
	{
		if((!pcsAgpdLancerItem->m_pcsCharacter) || (!pcsAgpdLancerItem->m_pcsItemTemplate))
			return FALSE;

		if(	!pThis->SetBaseEquipItemEventEffect(
			pcsAgpdLancerItem,
			AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT,
			(pcsAgpdLancerItem->m_pcsCharacter->m_lID == pThis->m_lSelfCID) ? (TRUE) : (FALSE),
			-1,
			NULL,
			NULL,
			NULL,
			NULL,
			pcsAgcdCharacter->m_afAnimSpeedRate[pcsAgcdCharacter->m_eCurAnimType]	)	)
		{
			br	= FALSE;
		}
	}

	if (!pThis->SetBaseCharacterEventEffect(pcsAgpdCharacter, pcsAgcdCharacter, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN))
	{
		br	= FALSE;
	}

	if (!pThis->PlayCharacterMovingSound(pcsAgpdCharacter, pcsAgcdCharacter))
	{
		br	= FALSE;
	}

	return br;
}

AgcdUseEffectSet *AgcmEventEffect::GetSkillUseEffectSet(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, INT32 lSkillTID, INT32 lConditionFlags)
{
	AgcdSkillAttachTemplateData* pstSkillData = m_pcsAgcmSkill->GetAttachTemplateData((ApBase*)(pcsAgpdCharacter->m_pcsCharacterTemplate));
	if( !pstSkillData )				return FALSE;

	INT32 lSkillUsableIndex	= m_pcsAgcmSkill->GetUsableSkillTNameIndex( (ApBase *)(pcsAgpdCharacter->m_pcsCharacterTemplate), lSkillTID );
	if( lSkillUsableIndex < 0 )		return FALSE;
	if( !pstSkillData->m_pacsSkillVisualInfo[lSkillUsableIndex] )		return FALSE;

	AgcdUseEffectSet* pcsAgcdUseEffectSet = pstSkillData->m_pacsSkillVisualInfo[lSkillUsableIndex]->m_pastEffect[pcsAgcdCharacter->m_lCurAnimType2];
	if( pcsAgcdUseEffectSet && (pcsAgcdUseEffectSet->m_ulConditionFlags & lConditionFlags)	)
		return pcsAgcdUseEffectSet;

	pcsAgcdUseEffectSet = pstSkillData->m_pacsSkillVisualInfo[lSkillUsableIndex]->m_pastEffect[AGCMCHAR_AT2_COMMON_DEFAULT];
	if ( pcsAgcdUseEffectSet && (pcsAgcdUseEffectSet->m_ulConditionFlags & lConditionFlags)	)
		return pcsAgcdUseEffectSet;

	return NULL;
}

BOOL AgcmEventEffect::SetBuffedAttackEffect( AgpdCharacter *pcsAgpdAttacker, AgpdCharacter *pcsAgpdTarget, AgcdCharacter *pcsAgcdAttacker, AgcdCharacter *pcsAgcdTarget )
{	
	if( !pcsAgpdAttacker || !pcsAgpdTarget)		return FALSE;
	
	stAgpmSkillBuffList* aSkillBuffedList = m_pcsAgpmSkill->GetBuffedList( (ApBase *)(pcsAgpdAttacker) );
	if( !aSkillBuffedList[0].lSkillTID )		return TRUE; // skip

	if( !pcsAgcdAttacker )
		pcsAgcdAttacker		= m_pcsAgcmCharacter->GetCharacterData( pcsAgpdAttacker );
	if( !pcsAgcdTarget )
		pcsAgcdTarget		= m_pcsAgcmCharacter->GetCharacterData( pcsAgpdTarget );

	if( !pcsAgcdAttacker || !pcsAgcdAttacker->m_pClump || !pcsAgcdTarget || !pcsAgcdTarget->m_pClump )
		return FALSE;

	BOOL bSelfCharacter	= pcsAgpdAttacker->m_lID == m_lSelfCID ? TRUE : FALSE;
	for (INT32 lCount = 0; lCount < AGPMSKILL_MAX_SKILL_BUFF; ++lCount)
	{
		if( !aSkillBuffedList[lCount].lSkillTID )	break;

		AgcmStartEventEffectParams	csParams;	
		csParams.m_pstAgcdUseEffectSet = GetSkillUseEffectSet( pcsAgpdAttacker, pcsAgcdAttacker, aSkillBuffedList[lCount].lSkillTID, AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET	);
		if( !csParams.m_pstAgcdUseEffectSet )		continue;

		csParams.m_ulConditionFlags			= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;
		csParams.m_csOrigin.m_pcsBase		= (ApBase *)(pcsAgpdAttacker);
		csParams.m_csOrigin.m_pcsClump		= pcsAgcdAttacker->m_pClump;
		csParams.m_csOrigin.m_pcsFrame		= RpClumpGetFrame(pcsAgcdAttacker->m_pClump);
		csParams.m_csOrigin.m_pcsHierarchy	= pcsAgcdAttacker->m_pInHierarchy;

		csParams.m_csTarget.m_pcsBase		= (ApBase *)(pcsAgpdTarget);
		csParams.m_csTarget.m_pcsClump		= pcsAgcdTarget->m_pClump;
		csParams.m_csTarget.m_pcsFrame		= RpClumpGetFrame(pcsAgcdTarget->m_pClump);
		csParams.m_csTarget.m_pcsHierarchy	= pcsAgcdTarget->m_pInHierarchy;

		csParams.m_bSelfCharacter			= bSelfCharacter;
		csParams.m_lExceptionType			= E_AGCD_EVNET_EFFECT_CDI_EXT_TYPE_SKILL_BUFFED_ATTACK;
		csParams.m_lExceptionTypeCustData	= aSkillBuffedList[lCount].lChargeLevel;

		StartEventEffect( &csParams );
	}

	return TRUE;
}

BOOL	AgcmEventEffect::SetOnlySoundSkill( AgcmStartEventEffectParams* pcsParams )
{
	if( !pcsParams )	return FALSE;

	AgcdUseEffectSetList	*pcsCurrent				= pcsParams->m_pstAgcdUseEffectSet->m_pcsHead;
	AES_EFFCTRL_SET_LIST	**ppCurrentNodeInfo		= &pcsParams->m_pcsNodeInfo;

	//. 현재 AgpdCharacter에 설정되어 있는 skyset flag.
	ACharacterSkyFlags		*pstAgcdSkySetFlag		= AgcmEventEffect::m_pcsThisAgcmEventEffect->GetSkySetFlagData( (AgpdCharacter*)pcsParams->m_csOrigin.m_pcsBase);

	RwReal	castSkillEffTimeScale = pcsParams->m_fTimeScale;
	while (pcsCurrent)
	{
		if (!(pcsCurrent->m_csData.m_ulConditionFlags & pcsParams->m_ulConditionFlags))
		{
			pcsCurrent	= pcsCurrent->m_pcsNext;
			continue;
		}

		if ( (pcsCurrent->m_csData.m_pszSoundName) &&
			 (pcsCurrent->m_csData.m_pszSoundName[0]) &&
			 (!pcsParams->m_lExceptionType) &&
			 (!(pcsParams->m_ulFlags & AgcmStartEventEffectParams::E_ASEEP_SKIP_PLAY_SOUND)) )
		{
			PROFILE("StartEventEffect - Sound");
			PlaySound( AGCMEVENTEFFECT_EFFECT_SOUND_PATH, AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH, pcsCurrent->m_csData.m_pszSoundName, pcsParams->m_bSelfCharacter, pcsParams->m_csOrigin.m_pcsClump );
		}

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmEventEffect::EventCharAnimAttackCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass || !pData ) return FALSE;

	AgcmEventEffect* pThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacter* pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	AgcdCharacter* pcsAgcdCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter ) return FALSE;

	FLOAT fLife	= AgcuEventEffect_GetCharacterCurrentAnimDuration( pcsAgcdCharacter );
	if( fLife <= 0.0f ) return FALSE;

	ApBase				*pcsTarget					= NULL;
	RpHAnimHierarchy	*pstTargetHierarchy			= NULL;
	RpClump				*pstTargetClump				= NULL;
	RwFrame				*pstTargetFrame				= NULL;
	AgpdCharacter		*pstAgpdTarget				= pThis->m_pcsAgpmCharacter->GetCharacter(pcsAgpdCharacter->m_alTargetCID[0]);
	AgcdCharacter		*pstAgcdTarget				= NULL;

	if (pstAgpdTarget)
	{
		pstAgcdTarget = pThis->m_pcsAgcmCharacter->GetCharacterData(pstAgpdTarget);
		if( pstAgcdTarget && pstAgcdTarget->m_pClump )
		{
			pcsTarget			= (ApBase *)(pstAgpdTarget);
			pstTargetClump		= pstAgcdTarget->m_pClump;
			pstTargetFrame		= RpClumpGetFrame(pstAgcdTarget->m_pClump);
			pstTargetHierarchy	= pstAgcdTarget->m_pInHierarchy;
		}
	}

	BOOL bSelfCharacter = FALSE;
	AgpdItem* pcsAgpdRItem = GetEquipItem( pThis->m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_HAND_RIGHT );
	if( pcsAgpdRItem )
	{
		if( !pcsAgpdRItem->m_pcsCharacter || !pcsAgpdRItem->m_pcsItemTemplate )
			return FALSE;

		bSelfCharacter = pcsAgpdRItem->m_pcsCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE;

		if(	!pThis->SetBaseEquipItemEventEffect(
			pcsAgpdRItem,
			AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK,
			bSelfCharacter,
			(INT32)(fLife * 1000),
			pcsTarget,
			pstTargetClump,
			pstTargetFrame,
			pstTargetHierarchy,
			pcsAgcdCharacter->m_afAnimSpeedRate[pcsAgcdCharacter->m_eCurAnimType]	)	)
		{
			return FALSE;
		}
	}

	AgpdItem* pcsAgpdLItem = GetEquipItem( pThis->m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_HAND_LEFT );
	if( pcsAgpdLItem )
	{
		if( !pcsAgpdLItem->m_pcsCharacter || !pcsAgpdLItem->m_pcsItemTemplate )	return FALSE;

		bSelfCharacter = pcsAgpdLItem->m_pcsCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE;

		if(!pThis->SetBaseEquipItemEventEffect(
			pcsAgpdLItem,
			AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK,
			bSelfCharacter,
			(INT32)(fLife * 1000),
			pcsTarget,
			pstTargetClump,
			pstTargetFrame,
			pstTargetHierarchy,
			pcsAgcdCharacter->m_afAnimSpeedRate[pcsAgcdCharacter->m_eCurAnimType]	)	)
		{
			return FALSE;
		}
	}

	AgpdItem* pcsAgpdArmsItem = GetEquipItem( pThis->m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_ARMS );
	if( pcsAgpdArmsItem )
	{
		if( !pcsAgpdArmsItem->m_pcsCharacter || !pcsAgpdArmsItem->m_pcsItemTemplate )
			return FALSE;

		bSelfCharacter = pcsAgpdArmsItem->m_pcsCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE;

		if(!pThis->SetBaseEquipItemEventEffect(
			pcsAgpdArmsItem,
			AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK,
			bSelfCharacter,
			(INT32)(fLife * 1000),
			pcsTarget,
			pstTargetClump,
			pstTargetFrame,
			pstTargetHierarchy,
			pcsAgcdCharacter->m_afAnimSpeedRate[pcsAgcdCharacter->m_eCurAnimType]	)	)
		{
			return FALSE;
		}
	}

	AgpdItem* pcsAgpdArms2Item = GetEquipItem( pThis->m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_ARMS2 );
	if( pcsAgpdArms2Item )
	{
		if( !pcsAgpdArms2Item->m_pcsCharacter || !pcsAgpdArms2Item->m_pcsItemTemplate )
			return FALSE;

		bSelfCharacter = pcsAgpdArms2Item->m_pcsCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE;

		if(!pThis->SetBaseEquipItemEventEffect(
			pcsAgpdArms2Item,
			AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK,
			bSelfCharacter,
			(INT32)(fLife * 1000),
			pcsTarget,
			pstTargetClump,
			pstTargetFrame,
			pstTargetHierarchy,
			pcsAgcdCharacter->m_afAnimSpeedRate[pcsAgcdCharacter->m_eCurAnimType]	)	)
		{
			return FALSE;
		}
	}

	// 기본 휘두르기음!
	INT32	lWeaponType = pThis->GetWeaponType( pcsAgpdRItem );
	if( lWeaponType == -1 )
	{
		lWeaponType = pThis->GetWeaponType( pcsAgpdLItem );
		if( lWeaponType == -1 )
		{
			lWeaponType = pThis->GetWeaponType( pcsAgpdArmsItem );
			if( lWeaponType == -1 )
				lWeaponType = pThis->GetWeaponType( pcsAgpdArms2Item );
			
		}
	}

	if( !pThis->PlayAttackSound( pcsAgpdCharacter, lWeaponType, bSelfCharacter, pcsAgcdCharacter->m_pClump ) ) return FALSE;
	if( !pThis->SetBuffedAttackEffect( pcsAgpdCharacter, pstAgpdTarget, pcsAgcdCharacter, pstAgcdTarget ) ) return FALSE;

	// 법사는 무기가 없을때 기본공격이펙트발동 하지 말라는군.
	if( lWeaponType == -1 )
	{
		INT32 lRace		= (INT32) AURACE_TYPE_NONE;
		INT32 lClass	= (INT32) AUCHARCLASS_TYPE_NONE;
		
		pThis->m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE );
		pThis->m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS );
		if( lRace != AURACE_TYPE_DRAGONSCION && lClass == AUCHARCLASS_TYPE_MAGE )
			return TRUE;
	}

	return pThis->SetBaseCharacterEventEffect( pcsAgpdCharacter, pcsAgcdCharacter, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK );
}

BOOL AgcmEventEffect::SetCharTargetStruck(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, AgcdUseEffectSet *pstAgcdUseEffectSet, BOOL bSetRange)
{
	if (!pcsAgcdCharacter->m_pClump) return FALSE;

	AgpdCharacter				*pstAgpdAttacker			= m_pcsAgpmCharacter->GetCharacter(pcsAgcdCharacter->m_lAttackerID);
	if (!pstAgpdAttacker)
		return TRUE; // skip

	AgcdCharacter				*pcsAgcdAttacker			= m_pcsAgcmCharacter->GetCharacterData(pstAgpdAttacker);
	if ((!pcsAgcdAttacker) || (!pcsAgcdAttacker->m_pClump)) return FALSE;

	AgcmStartEventEffectParams	csParams;
	csParams.m_csOrigin.m_pcsClump							= pcsAgcdAttacker->m_pClump;
	csParams.m_csOrigin.m_pcsFrame							= RpClumpGetFrame(pcsAgcdAttacker->m_pClump);
	if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

	csParams.m_csTarget.m_pcsClump							= pcsAgcdCharacter->m_pClump;
	csParams.m_csTarget.m_pcsFrame							= RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	if (!csParams.m_csTarget.m_pcsFrame) return FALSE;

	INT32 lRange = 0;
	if (bSetRange)		// 맞는 넘의 HitRange로 계산해야 된다. By Parn
		m_pcsAgpmFactors->GetValue(&pcsAgpdCharacter->m_pcsCharacterTemplate->m_csFactor, &lRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE);

	csParams.m_ulConditionFlags								= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK;
	csParams.m_pstAgcdUseEffectSet							= pstAgcdUseEffectSet;
	csParams.m_csOrigin.m_pcsBase							= (ApBase *)(pcsAgpdCharacter);
	csParams.m_csOrigin.m_pcsHierarchy						= pcsAgcdCharacter->m_pInHierarchy;
	csParams.m_bSelfCharacter								= (pcsAgpdCharacter->m_lID == m_lSelfCID ? TRUE : FALSE);
	csParams.m_fRange										= (FLOAT)(lRange);

	return StartEventEffect(&csParams);
}

BOOL AgcmEventEffect::EventCharAttackSuccessCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdAttacker	= (AgpdCharacter *)(pData);
	AgpdCharacter	*pcsAgpdTarget		= (AgpdCharacter *)(pCustData);

	pThis->SetBaseCharacterEventEffect(pcsAgpdTarget, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK);
	return pThis->SetTargetStruckEffect(pcsAgpdAttacker, pcsAgpdTarget);
}

BOOL AgcmEventEffect::EventCharAnimSocialCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData)) return FALSE;

	AgcmEventEffect	*pcsThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pstAgpdCharacter	= (AgpdCharacter *)(pData);
	AgcdCharacter	*pstAgcdCharacter	= (AgcdCharacter *)(pCustData);

	if (pstAgcdCharacter->m_ulAnimationFlags & AGCMCHAR_AF_NO_UNEQUIP)
	{
		pstAgcdCharacter->m_ulAnimationFlags	&= ~AGCMCHAR_AF_NO_UNEQUIP;
		return TRUE;
	}

	return pcsThis->HideShieldAndWeaponsWhileCurrentAnimationDuration(pstAgpdCharacter, pstAgcdCharacter);
}

VOID AgcmEventEffect::GetSpiritStoneInfo( AgpdItem *pcsAgpdItem, INT32 *plNumSpiritStone, INT32 *plRank, INT32 *plLevel,
						INT32 *plNumConvertedWaterAttr, INT32 *plNumConvertedFireAttr, INT32 *plNumConvertedEarthAttr, INT32 *plNumConvertedAirAttr, INT32 *plNumConvertedMagicAttr	)
{
	INT32 lNumSpiritStone = 0;
	if (plNumSpiritStone)
	{
		*plNumSpiritStone			= m_pcsAgpmItemConvert->GetNumSpiritStone( pcsAgpdItem );
		lNumSpiritStone = *plNumSpiritStone;
	}

	if (plRank)
	{
		INT32 lRank = 0;
		m_pcsAgpmFactors->GetValue(&pcsAgpdItem->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
		*plRank						= lRank;
	}

	if (plLevel)
		*plLevel					= (lNumSpiritStone % 2 == 0) ? (INT32)(lNumSpiritStone / 2) : (INT32)(lNumSpiritStone / 2) + 1;;

	if (plNumConvertedWaterAttr)
		*plNumConvertedWaterAttr	= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsAgpdItem, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);

	if (plNumConvertedFireAttr)
		*plNumConvertedFireAttr		= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsAgpdItem, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);

	if (plNumConvertedEarthAttr)
		*plNumConvertedEarthAttr	= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsAgpdItem, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);

	if (plNumConvertedAirAttr)
		*plNumConvertedAirAttr		= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsAgpdItem, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

	if (plNumConvertedMagicAttr)
		*plNumConvertedMagicAttr	= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsAgpdItem, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
}

/******************************************************************************
* Purpose :
*
* 090704. BOB
******************************************************************************/
AgcdUseEffectSetData *AgcmEventEffect::GetSpiritStoneHitEffect(AgpdItem *pcsAgpdItem, INT32 lIndex)
{
	INT32 lNumSpiritStone			= 0;
	INT32 lRank						= 0;
	INT32 lLevel					= 0;
	INT32 lNumConvertedWaterAttr	= 0;
	INT32 lNumConvertedFireAttr		= 0;
	INT32 lNumConvertedEarthAttr	= 0;
	INT32 lNumConvertedAirAttr		= 0;
	INT32 lNumConvertedMagicAttr	= 0;

	GetSpiritStoneInfo(
		pcsAgpdItem,
		&lNumSpiritStone,
		&lRank,
		&lLevel,
		&lNumConvertedWaterAttr,
		&lNumConvertedFireAttr,
		&lNumConvertedEarthAttr,
		&lNumConvertedAirAttr,
		&lNumConvertedMagicAttr);

	return m_csAgcmEventEffectData.GetSpiritStoneHitEffectData(lNumSpiritStone, lRank, lLevel, lNumConvertedWaterAttr, lNumConvertedFireAttr, lNumConvertedEarthAttr, lNumConvertedAirAttr, lNumConvertedMagicAttr, lIndex);
}


/******************************************************************************
* Purpose :
*
* 011203. BOB
******************************************************************************/
BOOL AgcmEventEffect::SetTargetStruckEffect(AgpdCharacter *pstAgpdAttacker, AgpdCharacter *pstAgpdTarget)
{
	AgcdCharacter	*pstAgcdTarget		= m_pcsAgcmCharacter->GetCharacterData(pstAgpdTarget);
	if (!pstAgcdTarget) return FALSE;

	AgcdCharacter	*pstAgcdAttacker	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdAttacker);
	if( !pstAgcdAttacker )			return FALSE;
	if( !pstAgcdTarget->m_pClump )	return TRUE; // skip
	if( !pstAgpdTarget->m_pcsCharacterTemplate ) return FALSE;

	static DebugValueINT32	_sTIDAttacker	( 0 , "Attacker TID = %d" );
	static DebugValueINT32	_sTIDTarget		( 0 , "Target TID = %d" );
	_sTIDAttacker	= pstAgpdAttacker->m_pcsCharacterTemplate->m_lID;
	_sTIDTarget		= pstAgpdTarget->m_pcsCharacterTemplate->m_lID;

	AgcdUseEffectSet		*pstAgcdUseEffectSet = NULL;
	AgpdItem				*pcsAgpdRItem = NULL, *pcsAgpdLItem = NULL;

	if (!pstAgpdAttacker->m_bIsTrasform)
	{
		pcsAgpdRItem = GetEquipItem(m_pcsAgpmItem, pstAgpdAttacker, AGPMITEM_PART_HAND_RIGHT);
		pcsAgpdLItem = GetEquipItem(m_pcsAgpmItem, pstAgpdAttacker, AGPMITEM_PART_HAND_LEFT);
	}

	INT32	lWeaponType = GetWeaponType(pcsAgpdRItem);
	if (lWeaponType == -1)
	{
		if (pcsAgpdLItem)
			lWeaponType = GetWeaponType(pcsAgpdLItem);
	}

	INT32	lRt = SetSpiritStoneHitEffect(pstAgpdAttacker);
	if (lRt == E_AC_RETURN_TYPE_FAILURE) return FALSE;

	if (lRt == E_AC_RETURN_TYPE_SKIP)
	{
		lRt = SetAttrDamageEffect(pstAgpdAttacker, pstAgpdTarget);
		if (lRt == E_AC_RETURN_TYPE_FAILURE)
		{
			return FALSE;
		}
		else if (lRt == E_AC_RETURN_TYPE_SUCCESS)
		{
			return TRUE;
		}
	}
	

	// 때린놈의 오른손에 아이템이 있으면...
	if (pcsAgpdRItem)
	{
		// Template이 없으면 안된다!
		if (!pcsAgpdRItem->m_pcsItemTemplate) return FALSE;

		// Item template에 있는 Data를 가져온다.
		pstAgcdUseEffectSet	= GetUseEffectSet( (AgpdItemTemplate*)pcsAgpdRItem->m_pcsItemTemplate );
		if (!pstAgcdUseEffectSet) return FALSE;
	}

	// 오른손에 아이템이 없거나 아이템에 붙어있는 데이터 플래그에 AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK 없으면...
	if( !pcsAgpdRItem || !DEF_FLAG_CHK( pstAgcdUseEffectSet->m_ulConditionFlags, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK ) )
	{
		// 때린놈의 왼손에 아이템이 있으면...
		if (pcsAgpdLItem)
		{
			// Template이 없으면 안된다!
			if (!pcsAgpdLItem->m_pcsItemTemplate) return FALSE;

			// Item template에 있는 Data를 가져온다.
			pstAgcdUseEffectSet	= GetUseEffectSet((AgpdItemTemplate *)(pcsAgpdLItem->m_pcsItemTemplate));
			if (!pstAgcdUseEffectSet) return FALSE;
		}
	}

	// 무기쪽에 설정된 struct effect와 캐릭터에 설정된 struct effect 를 둘다 설정가능하도록 수정함
	// 두개의 effect를 표현해주는 것이 기존 리소스에서 문제가 발생할시 이 부분을 삭제할것
	if( pstAgcdUseEffectSet && DEF_FLAG_CHK( pstAgcdUseEffectSet->m_ulConditionFlags, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK ) )
	{
		// TODO : 모델툴에서 스트럭이펙트에 적용가능 웹폰타입을 저장할수 있을때 제거후 인터페이스 수정
		NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker	= lWeaponType;

		if ( !SetCharTargetStruck(pstAgpdTarget, pstAgcdTarget, pstAgcdUseEffectSet) )
		{
			NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker = -1;
			return FALSE;
		}

		pstAgcdUseEffectSet = NULL;
	}

	// 아이템이 없거나 아이템에 붙어있는 데이터 플래그에 AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK 없으면...	
	if( !pstAgcdUseEffectSet || !DEF_FLAG_CHK( pstAgcdUseEffectSet->m_ulConditionFlags, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK ) )
	{
		// 때린 놈의 캐릭터 템플릿이 없으면 안된다...
		if (!pstAgpdAttacker->m_pcsCharacterTemplate) return FALSE;

		// 때린 놈의 캐릭터 템플릿에서 데이터를 가져온다.
		pstAgcdUseEffectSet	= GetUseEffectSet(pstAgpdAttacker->m_pcsCharacterTemplate);
		if (!pstAgcdUseEffectSet) return FALSE;
	}

	if( pstAgcdUseEffectSet && DEF_FLAG_CHK( pstAgcdUseEffectSet->m_ulConditionFlags, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK )	)
	{
		NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker	= lWeaponType;
		if (!SetCharTargetStruck(pstAgpdTarget, pstAgcdTarget, pstAgcdUseEffectSet))
		{
			NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker = -1;
			return FALSE;
		}
		NS_HardCoding_AgcmEventEffect::g_WeaponType_Of_Attacker = -1;

		if (!PlayHitSound(pstAgpdAttacker, pstAgcdAttacker, pstAgpdTarget)) return FALSE;
	}

	return TRUE;
}

BOOL AgcmEventEffect::EventCharLevelUPCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	// 현재 캐릭터가 게임에 들어가 있는 상태일때만 이펙트를 날려준다.
	if( pcsAgpdCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD )
		return pThis->SetCommonCharEffect((ApBase *)(pcsAgpdCharacter), E_COMMON_CHAR_TYPE_LV_UP);

	return TRUE;
}

BOOL AgcmEventEffect::EventCharCreate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	return ((AgcmEventEffect *)(pClass))->SetCommonCharEffect((ApBase *)(pData), E_COMMON_CHAR_TYPE_SPAWN);
}

BOOL AgcmEventEffect::SetActionResultSkill(AgpdCharacter *pcsAgpdCharacter, AgpdCharacterActionResultType eActionResult)
{
	if( !pcsAgpdCharacter->m_pcsCharacterTemplate )			return FALSE;

	AgcdCharacter* pcsAgcdCharacter = m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )									return FALSE;

	AgpdSkill* pcsAgpdSkill = m_pcsAgpmSkill->GetSkillByAction(pcsAgpdCharacter, eActionResult);
	if( !pcsAgpdSkill || !pcsAgpdSkill->m_pcsTemplate )		return FALSE;

	AgcdSkillADChar* pcsAgcdSkillADChar = m_pcsAgcmSkill->GetCharacterAttachData( (ApBase*)(pcsAgpdCharacter) );
	if( !pcsAgcdSkillADChar )								return FALSE;

	INT32 lSkillUsableIndex = m_pcsAgcmSkill->GetUsableSkillTNameIndex( (ApBase*)pcsAgpdCharacter->m_pcsCharacterTemplate, pcsAgpdSkill->m_pcsTemplate->m_lID );
	if( lSkillUsableIndex < 0 )								return FALSE;

	pcsAgcdSkillADChar->m_lSkillID			= pcsAgpdSkill->m_lID;
	pcsAgcdSkillADChar->m_lSkillUsableIndex	= lSkillUsableIndex;

	return SetBaseSkillEventEffect( pcsAgpdCharacter );
}

BOOL AgcmEventEffect::EventCharActionResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData))
		return FALSE;

	AgcmEventEffect					*pcsThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacter					*pcsAgpdTarget		= (AgpdCharacter *)(pData);
	AgcmCharacterActionQueueData	*pcsActionData		= (AgcmCharacterActionQueueData *)(pCustData);

	AgpdCharacter					*pcsAgpdActor		= pcsThis->m_pcsAgpmCharacter->GetCharacter(pcsActionData->lActorID);
	if (!pcsAgpdActor)
		return TRUE;

	AgcdCharacter					*pcsAgcdActor		= pcsThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdActor);
	if (!pcsAgcdActor)
		return FALSE;

	AgpdCharacter					*pcsSelfCharacter	= pcsThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	switch (pcsActionData->eActionType)
	{
	case AGPDCHAR_ACTION_TYPE_ATTACK:
		{
			switch (pcsActionData->eActionResultType)
			{
			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_CRITICAL:
				return pcsThis->SetActionResultSkill( pcsAgpdActor, pcsActionData->eActionResultType );
			case AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE:
				return pcsThis->SetActionResultSkill( pcsAgpdActor, pcsActionData->eActionResultType );
			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE:
			case AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_DODGE:
				return pcsThis->SetActionResultSkill( pcsAgpdActor, pcsActionData->eActionResultType );
			case AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK:
				return pcsThis->SetActionResultSkill( pcsAgpdActor, pcsActionData->eActionResultType );
			case AGPDCHAR_ACTION_RESULT_TYPE_REFLECT_DAMAGE:
			case AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE:
			case AGPDCHAR_ACTION_RESULT_TYPE_CONVERT_DAMAGE:		// 데미지 흡수(HP, MP)
				break;
			}
			
			if( DEF_FLAG_CHK( pcsActionData->ulAdditionalEffect, AGPDCHAR_ADDITIONAL_EFFECT_LENS_STONE ) && pcsSelfCharacter->m_lID == pcsAgpdActor->m_lID )		// Additional Effect 가 있으면 뿌려준다.
				pcsThis->SetCommonCharacterEffect( pcsAgpdTarget, E_COMMON_CHAR_TYPE_LENS_STONE );
		}
		break;

	case AGPDCHAR_ACTION_TYPE_SKILL:
		{
			if( DEF_FLAG_CHK( pcsActionData->ulAdditionalEffect, AGPDCHAR_ADDITIONAL_EFFECT_LENS_STONE ) && pcsSelfCharacter->m_lID == pcsAgpdActor->m_lID )		// Additional Effect 가 있으면 뿌려준다.
				pcsThis->SetCommonCharacterEffect(pcsAgpdTarget, E_COMMON_CHAR_TYPE_LENS_STONE);
		}
		break;
	}

	return TRUE;
}

BOOL AgcmEventEffect::EventCharUpdateMovemenntCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmEventEffect::EventCharSpecialStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect*	pThis = (AgcmEventEffect *) pClass;
	AgpdCharacter*		pcsCharacter = (AgpdCharacter *) pData;

	UINT64			ulOldStatus		=	*( UINT64*	)((PVOID*)pCustData)[0];

	if( DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_STUN ) && !DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_STUN ) )
		pThis->SetCommonCharacterEffect( pcsCharacter, E_COMMON_CHAR_TYPE_STUN );
	else if( !DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_STUN ) && DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_STUN ) )
		pThis->RemoveCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_STUN);

	//if( DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_FREEZE ) && !DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_FREEZE ) )
	//	pThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_FREEZE);
	//else if( !DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_FREEZE ) && DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_FREEZE ) )
	//	pThis->RemoveCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_FREEZE);

	if( DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_SLOW) && !DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_SLOW ) )
		pThis->SetCommonCharacterEffect( pcsCharacter, E_COMMON_CHAR_TYPE_SLOW );
	else if( !DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_SLOW ) && DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_SLOW ) )
		pThis->RemoveCommonCharacterEffect( pcsCharacter, E_COMMON_CHAR_TYPE_SLOW );

	if( DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT ) && !DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT ) )
		pThis->m_pcsAgcmCharacter->UpdateTransparent( pcsCharacter, TRUE );
	else if( !DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT ) && DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT ) )
		pThis->m_pcsAgcmCharacter->UpdateTransparent( pcsCharacter, FALSE );

	if( DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT ) && !DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT ) )
		pThis->m_pcsAgcmCharacter->UpdateTransparent( pcsCharacter, TRUE );
	else if( !DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT ) && DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT ) )
		pThis->m_pcsAgcmCharacter->UpdateTransparent( pcsCharacter, FALSE );

	if( DEF_FLAG_CHK( pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_COUNSEL ) && !DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_COUNSEL ) )
		pThis->SetCommonCharacterEffect( pcsCharacter, E_COMMON_CHAR_TYPE_CONSULT );
	else if( !DEF_FLAG_CHK(pcsCharacter->m_ulSpecialStatus, AGPDCHAR_SPECIAL_STATUS_COUNSEL ) && DEF_FLAG_CHK( ulOldStatus, AGPDCHAR_SPECIAL_STATUS_COUNSEL ) )
		pThis->RemoveCommonCharacterEffect( pcsCharacter, E_COMMON_CHAR_TYPE_CONSULT );

	return TRUE;
}

// 2005.08.19. steeple
// From AgpmSkill. Additional Effect
BOOL AgcmEventEffect::EventCharAdditionalEffect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventEffect* pThis = static_cast<AgcmEventEffect*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	INT32 lEffectID = ppvBuffer[0] ? *(INT32*)ppvBuffer[0] : 0;
	INT32 lTargetCID = ppvBuffer[1] ? *(INT32*)ppvBuffer[1] : 0;

	eAgcmEventEffectDataCommonCharType eEffectType;
	switch(lEffectID)
	{
		case AGPMSKILL_ADDITIONAL_EFFECT_END_BUFF_EXPLOSION:	eEffectType = E_COMMON_CHAR_TYPE_TIMEATTACK;	break;
		case AGPMSKILL_ADDITIONAL_EFFECT_TELEPORT:				eEffectType = E_COMMON_CHAR_TYPE_TELEPORT;		break;
		case AGPMSKILL_ADDITIONAL_EFFECT_ABSORB_MP:				eEffectType = E_COMMON_CHAR_TYPE_ABSORBMANA;	break;
		case AGPMSKILL_ADDITIONAL_EFFECT_ABSORB_HP:				eEffectType = E_COMMON_CHAR_TYPE_ABSORBLIFE;	break;
		case AGPMSKILL_ADDITIONAL_EFFECT_CONVERT_HP:			eEffectType = E_COMMON_CHAR_TYPE_MENTALCIRCUIT;	break;
		case AGPMSKILL_ADDITIONAL_EFFECT_LENS_STONE:			eEffectType = E_COMMON_CHAR_TYPE_LENS_STONE;	break;
		case AGPMSKILL_ADDITIONAL_EFFECT_DETECT_SCROLL:			eEffectType = E_COMMON_CHAR_TYPE_DETECT_SCROLL;	break;
		case AGPMSKILL_ADDITIONAL_EFFECT_CURE:					eEffectType = E_COMMON_CHAR_TYPE_CURE;			break;
		default:																								return FALSE;
	}

	// 걍 깔끔하게 뿌린다.
	pThis->SetCommonCharacterEffect(pcsCharacter, eEffectType, lTargetCID);

	return TRUE;
}

void	AgcmEventEffect::CharaterEquipEffectRemove( AgpdCharacter * pcsCharacter )
{
	if( !pcsCharacter )		return;

	for( int i = AGPMITEM_PART_BODY; i < AGPMITEM_PART_NUM; ++i )
	{
		AgpdGridItem* pcsItemGrid = m_pcsAgpmItem->GetEquipItem( pcsCharacter , i );
		AgpdItem* pcsAgpdItem = pcsItemGrid ? m_pcsAgpmItem->GetItem( pcsItemGrid ) : NULL;
		if( pcsAgpdItem )
			RemoveAllEffect( pcsAgpdItem );
	}
}

void	AgcmEventEffect::CharaterEquipEffectUpdate( AgpdCharacter * pcsCharacter )
{
	if( !pcsCharacter )		return;

	for( int i = AGPMITEM_PART_BODY; i < AGPMITEM_PART_NUM; i ++ )
	{
		AgpdGridItem* pcsItemGrid = m_pcsAgpmItem->GetEquipItem( pcsCharacter , i );
		AgpdItem* pcsAgpdItem = pcsItemGrid ? m_pcsAgpmItem->GetItem( pcsItemGrid ) : NULL;
		if( pcsAgpdItem )
			AgcmItem::CBSetItem( pcsAgpdItem, (PVOID) m_pcsAgcmItem , NULL );
	}
}


BOOL AgcmEventEffect::EventCharEndSocialAnimCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pData) || (!pClass) || (!pCustData))
		return FALSE;

	return ((AgcmEventEffect *)(pClass))->AttachShieldAndWeapons((AgpdCharacter *)(pData), (AgcdCharacter *)(pCustData));
}

BOOL AgcmEventEffect::EventCharTransform(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmEventEffect		*pcsThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	return pcsThis->SetCommonCharEffect((ApBase *)(pcsAgpdCharacter), E_COMMON_CHAR_TYPE_TRANSFORM);
}

BOOL AgcmEventEffect::EventCharUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData))
		return FALSE;

	AgcmEventEffect		*pcsThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)(pData);
	INT16				nOldStatus			= *((INT16 *)(pCustData));

	if (nOldStatus == AGPDCHAR_STATUS_DEAD && pcsAgpdCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD )
		pcsThis->SetCommonCharEffect( (ApBase *)(pcsAgpdCharacter), E_COMMON_CHAR_TYPE_SPAWN );		// 부활에 관한 부분은 여기서 처리한다.

	return TRUE;
}

BOOL AgcmEventEffect::EventCharAnimDeadCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	return pThis->SetBaseCharacterEventEffect(pcsAgpdCharacter, AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD);
}

BOOL AgcmEventEffect::EventCharDamageCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData)) return FALSE;

	AgpdFactor* pcsAgpdFactor = (AgpdFactor *)(pData);
	AgcmEventEffect* pcsThis = (AgcmEventEffect *)(pClass);

	AgpdFactorOwner* pcsAgpdFactorOwner = (AgpdFactorOwner*)pcsThis->m_pcsAgpmFactors->GetFactor( pcsAgpdFactor, AGPD_FACTORS_TYPE_OWNER );
	if( !pcsAgpdFactorOwner )	return FALSE;

	AgpdCharacter* pcsAgpdCharacter	= (AgpdCharacter*)pcsAgpdFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER];
	if( !pcsAgpdCharacter )		return FALSE;

	return pcsThis->SetBaseCharacterEventEffect( pcsAgpdCharacter, AGCDEVENTEFFECT_CONDITION_FLAG_NONE );
}

BOOL AgcmEventEffect::EventItemEquipCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdItem		*pcsAgpdItem		= (AgpdItem *)(pData);
	AgcdItem		*pcsAgcdItem		= (AgcdItem *)(pCustData);

	if (!pcsAgpdItem->m_pcsCharacter) return FALSE;

	// 현재 장착하는 아이템이 BODY가 아니라면 갑옷개조효과를 추가설정한다.
	if (	(((AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) &&
			(((AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate))->m_nPart != AGPMITEM_PART_BODY) &&
			(((AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate))->m_nPart != AGPMITEM_PART_HEAD)			)
	{	
		// BODY 아이템이 개조 되어 있는지 조사
		AgpdItem* pcsAgpdBodyItem = pThis->m_pcsAgpmItem->GetEquipSlotItem( pcsAgpdItem->m_pcsCharacter, AGPMITEM_PART_BODY );
		if(	pcsAgpdBodyItem && pThis->m_pcsAgpmItemConvert->GetTotalConvertPoint( pcsAgpdBodyItem ) > 0 )
			pThis->SetConvertedArmourAttrEventEffect(pcsAgpdBodyItem);
	}

	// 아이템 개조 효과를 설정한다.
	pThis->SetConvertedItemEffect( pcsAgpdItem, pcsAgcdItem );

	BOOL bMy = pcsAgpdItem->m_pcsCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE;
	return pThis->SetBaseEquipItemEventEffect( pcsAgpdItem, AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP, bMy );
}

BOOL AgcmEventEffect::EventItemDetachedCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdItem		*pcsAgpdItem		= (AgpdItem *)(pData);				// 이미 detach된 아이템, 정보만 참조한다.
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pCustData);		// detach된 아이템을 장착했던 캐릭터.

	// 장착했던 아이템=갑옷(o),BODY(x),HEAD(x)
	if (	(((AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) &&
			(((AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate))->m_nPart != AGPMITEM_PART_BODY) &&
			(((AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate))->m_nPart != AGPMITEM_PART_HEAD)			)
	{	
		// BODY 아이템이 개조 되어 있는지 조사
		AgpdItem* pcsAgpdBodyItem = pThis->m_pcsAgpmItem->GetEquipSlotItem( pcsAgpdCharacter, AGPMITEM_PART_BODY );
		if( pcsAgpdBodyItem && pThis->m_pcsAgpmItemConvert->GetNumSpiritStone(pcsAgpdBodyItem) > 0 )
			pThis->SetConvertedArmourAttrEventEffect( pcsAgpdBodyItem );
	}

	return TRUE;
}

BOOL AgcmEventEffect::EventItemUnequipCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdItem		*pcsAgpdItem		= (AgpdItem *)(pData);
	AgcdItem		*pcsAgcdItem		= (AgcdItem *)(pCustData);

	pThis->RemoveAllEffect((ApBase *)(pcsAgpdItem));

	if (pThis->m_pcsAgpmItemConvert->GetNumSpiritStone(pcsAgpdItem) > 0)
		pThis->ReleaseConvertedItemEffect(pcsAgpdItem, pcsAgcdItem);

	return TRUE;
}

BOOL AgcmEventEffect::EventItemUseCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData)) return FALSE;
	return ((AgcmEventEffect *)(pClass))->SetBaseUsableItemEventEffect((AgpdCharacter *)(pData), (AgpdItem *)(pCustData), AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_USE);
}

BOOL AgcmEventEffect::EventSkillCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis				= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	return pThis->SetBaseSkillEventEffect(pcsAgpdCharacter);
}

BOOL AgcmEventEffect::EventSkillAdditionalEffectCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( pClass && pData );
	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pThis = (AgcmEventEffect *)(pClass);
	RwInt32	 num = *static_cast<RwInt32*>(pData);
	RwInt32 *arr = static_cast<RwInt32*>(pCustData);

	std::copy( arr, arr+num, std::back_insert_iterator< std::vector<RwInt32> >(pThis->m_container_additionalHitEff));
	
	return TRUE;
};

BOOL AgcmEventEffect::CharacterSetSelfCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect *pThis				= (AgcmEventEffect *)(pClass);

	if (!pThis->m_pcsAgcmEff2)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);
	AgcdCharacter	*pcsAgcdCharacter	= (AgcdCharacter *)(pCustData);

	if(!pcsAgcdCharacter->m_pClump) return FALSE;

	RwFrame *pstSelfCharFrame = RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	if(!pstSelfCharFrame) return FALSE;

	pThis->m_pcsAgcmEff2->SetMainCharacterFrame( pcsAgcdCharacter->m_pClump, pstSelfCharFrame );	// effect 모듈에 주인공 frame을 알려주고...
	pThis->SetSelfCID( pcsAgpdCharacter->m_lID );													// 주인공 CID를 저장한다.

	return TRUE;
}

BOOL AgcmEventEffect::AttachedDataEffectListConstructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect		*pThis	= (AgcmEventEffect *)(pClass);
	ApBase				*pBase	= (ApBase *)(pData);

	AgcmEventEffectList *pList = pThis->GetEventEffectList(pBase);
	if(!pList) return FALSE;

	pList->CreateList();

	return TRUE;
}

BOOL AgcmEventEffect::AttachedDataEffectListDestructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect		*pThis	= (AgcmEventEffect *)(pClass);
	ApBase				*pBase	= (ApBase *)(pData);

	AgcmEventEffectList *pList = pThis->GetEventEffectList(pBase);
	if(!pList) return FALSE;

	pList->DestroyUseEffectList();

	return TRUE;
}

BOOL AgcmEventEffect::AttachedDataObjUseEffectSetConstructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect		*pThis			= (AgcmEventEffect *)(pClass);
	ApdObjectTemplate	*pcsTemplate	= (ApdObjectTemplate *)(pData);

	AgcdUseEffectSet	*pEffectData	= pThis->GetUseEffectSet(pcsTemplate);
	if(!pEffectData) return FALSE;

	memset(pEffectData, 0, sizeof(AgcdUseEffectSet));
	return TRUE;
}

BOOL AgcmEventEffect::AttachedDataCharUseEffectSetConstructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect			*pThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacterTemplate	*pcsTemplate	= (AgpdCharacterTemplate *)(pData);

	AgcdUseEffectSet		*pEffectData	= pThis->GetUseEffectSet(pcsTemplate);
	if(!pEffectData) return FALSE;

	memset(pEffectData, 0, sizeof(AgcdUseEffectSet));
	return TRUE;
}

BOOL AgcmEventEffect::AttachedDataItemUseEffectSetConstructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect		*pThis			= (AgcmEventEffect *)(pClass);
	AgpdItemTemplate	*pcsTemplate	= (AgpdItemTemplate *)(pData);

	AgcdUseEffectSet	*pEffectData	= pThis->GetUseEffectSet(pcsTemplate);
	if(!pEffectData) return FALSE;

	memset(pEffectData, 0, sizeof(AgcdUseEffectSet));
	return TRUE;
}

BOOL AgcmEventEffect::AttachedDataUseEffectSetDestructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pData) || (!pClass))
		return FALSE;

	AgcmEventEffect		*pcsThis	= (AgcmEventEffect *)(pClass);
	ApBase				*pcsBase	= (ApBase *)(pData);

	AgcdUseEffectSet	*pcsSet		= pcsThis->GetUseEffectSet(pcsBase);
	if (!pcsSet) return FALSE;

	pcsThis->m_csEffectDataAdmin2.DeleteAll(pcsSet);

	return TRUE;
}

AgcmEventEffectList	*AgcmEventEffect::GetEventEffectList(ApBase *pcsBase)
{
	if( !pcsBase )			return NULL;
	
	switch( pcsBase->m_eType )
	{
	case APBASE_TYPE_OBJECT:	return (AgcmEventEffectList*)m_pcsApmObject->GetAttachedModuleData( m_nAttachedIndexObjEffectList, (void*)(pcsBase) );
	case APBASE_TYPE_CHARACTER:	return (AgcmEventEffectList*)m_pcsAgpmCharacter->GetAttachedModuleData( m_nAttachedIndexCharEffectList, (void*)(pcsBase) );
	case APBASE_TYPE_ITEM:		return (AgcmEventEffectList*)m_pcsAgpmItem->GetAttachedModuleData( m_nAttachedIndexItemEffectList, (void*)(pcsBase) );
	}

	return NULL;
}

AgcdUseEffectSet *AgcmEventEffect::GetUseEffectSet(ApdObjectTemplate *pcsTemplate)
{
	return (AgcdUseEffectSet *)(m_pcsApmObject->GetAttachedModuleData(m_nAttachedIndexObjUseEffectSet, (void *)(pcsTemplate)));
}

AgcdUseEffectSet *AgcmEventEffect::GetUseEffectSet(AgpdCharacterTemplate *pcsTemplate)
{
	return (AgcdUseEffectSet *)(m_pcsAgpmCharacter->GetAttachedModuleData(m_nAttachedIndexCharUseEffectSet, (void *)(pcsTemplate)));
}

AgcdUseEffectSet *AgcmEventEffect::GetUseEffectSet(AgpdItemTemplate *pcsTemplate)
{
	return (AgcdUseEffectSet *)(m_pcsAgpmItem->GetAttachedModuleData(m_nAttachedIndexItemUseEffectSet, (void *)(pcsTemplate)));
}

AgcdUseEffectSet *AgcmEventEffect::GetUseEffectSet(ApBase *pcsBase)
{
	switch(pcsBase->m_eType)
	{
	case APBASE_TYPE_OBJECT_TEMPLATE:		return GetUseEffectSet((ApdObjectTemplate *)(pcsBase));
	case APBASE_TYPE_CHARACTER_TEMPLATE:	return GetUseEffectSet((AgpdCharacterTemplate *)(pcsBase));
	case APBASE_TYPE_ITEM_TEMPLATE:			return GetUseEffectSet((AgpdItemTemplate *)(pcsBase));
	}

	return NULL;
}

BOOL AgcmEventEffect::UseEffectSetStreamRead(ApModuleStream *pStream, AgcdUseEffectSet *pstAgcdUseEffectSet, AgcmEventEffectParamFunction pfParam, ApBase *pcsBase)
{
	CHAR					szTemp[512];
	CHAR					szTemp2[128];
	INT32					lIndex = 0;
	INT32					lParentNodeID = 0;
	UINT32					ulEID = 0;
	FLOAT					fOffsetX = 0.0f, fOffsetY = 0.0f, fOffsetZ = 0.0f, fScale = 0.0f;
	UINT32					ulStartGap = 0, ulConditionFlags = 0;
	AgcdUseEffectSetData	*pcsData = NULL;

	memset(szTemp, 0, 512);
	memset(szTemp, 0, 128);

	while (pStream->ReadNextValue())
	{
		if (!strncmp(pStream->GetValueName(), AGCMEVENTEFFECT_CONDITION_FLAGS_INI_NAME, strlen(AGCMEVENTEFFECT_CONDITION_FLAGS_INI_NAME)))
		{
			pStream->GetValue(szTemp, 512);
			sscanf(szTemp, "%d:%d",	&lIndex, &ulConditionFlags);
			pcsData	= m_csEffectDataAdmin2.GetData(pstAgcdUseEffectSet, lIndex, TRUE);
			if (!pcsData)
				return FALSE;

			pcsData->m_ulConditionFlags									=	ulConditionFlags;
			pstAgcdUseEffectSet->m_ulConditionFlags						|=	ulConditionFlags;
		}
		else if (!strncmp(pStream->GetValueName(), AGCMEVENTEFFECT_EFFECT_DATA_INI_NAME, strlen(AGCMEVENTEFFECT_EFFECT_DATA_INI_NAME)))
		{
			pStream->GetValue(szTemp, 512);
			sscanf(szTemp,
				"%d:%d:%f:%f:%f:%f:%d:%d",
				&lIndex, &ulEID, &fOffsetX, &fOffsetY, &fOffsetZ, &fScale, &lParentNodeID, &ulStartGap);

			pcsData	= m_csEffectDataAdmin2.GetData(pstAgcdUseEffectSet, lIndex, TRUE);
			if (!pcsData)
				return FALSE;

			pcsData->m_ulEID				= ulEID;
			pcsData->m_fScale				= fScale;
			pcsData->m_lParentNodeID		= lParentNodeID;
			pcsData->m_ulStartGap			= ulStartGap;

			if ((fOffsetX != 0.0f) || (fOffsetY != 0.0f) || (fOffsetZ != 0.0f))
			{
				if (pcsData->m_pv3dOffset)
					return FALSE;

				pcsData->m_pv3dOffset		= new RwV3d;
				pcsData->m_pv3dOffset->x	= fOffsetX;
				pcsData->m_pv3dOffset->y	= fOffsetY;
				pcsData->m_pv3dOffset->z	= fOffsetZ;
			}
		}
		else if (!strncmp(pStream->GetValueName(), AGCMEVENTEFFECT_EFFECT_CUST_DATA_INI_NAME, strlen(AGCMEVENTEFFECT_EFFECT_CUST_DATA_INI_NAME)))
		{
			memset(szTemp2, 0, 128);

			pStream->GetValue(szTemp, 512);
			sscanf(szTemp, "%d:%s",	&lIndex, szTemp2);

			if (strcmp(szTemp2, ""))
			{
				pcsData	= m_csEffectDataAdmin2.GetData(pstAgcdUseEffectSet, lIndex, TRUE);
				if (!pcsData)
					return FALSE;

				if (pcsData->m_pszCustData)
					return FALSE;

				pcsData->m_pszCustData	= new CHAR[strlen(szTemp2) + 1];
				if (!pcsData->m_pszCustData)
					return FALSE;

				strcpy(pcsData->m_pszCustData, szTemp2);
			}
		}
		else if (!strncmp(pStream->GetValueName(), AGCMEVENTEFFECT_SOUND_DATA_INI_NAME, strlen(AGCMEVENTEFFECT_SOUND_DATA_INI_NAME)))
		{
			pStream->GetValue(szTemp, 512);
			sscanf(szTemp, "%d:%s",	&lIndex, szTemp2);

			if (strcmp(szTemp2, ""))
			{
				pcsData	= m_csEffectDataAdmin2.GetData(pstAgcdUseEffectSet, lIndex, TRUE);
				if (!pcsData)
					return FALSE;

				if (pcsData->m_pszSoundName)
					return FALSE;

				pcsData->m_pszSoundName	= new CHAR[strlen(szTemp2) + 1];
				if (!pcsData->m_pszSoundName)
					return FALSE;

				strcpy(pcsData->m_pszSoundName, szTemp2);
			}
		}
		else if (!strncmp(pStream->GetValueName(), AGCMEVENTEFFECT_EFFECT_ROTATION_INI_NAME, strlen(AGCMEVENTEFFECT_EFFECT_ROTATION_INI_NAME)))
		{
			AgcdUseEffectSetDataRotation	stRotation;

			pStream->GetValue(szTemp, 512);
			sscanf(szTemp, "%d:%f:%f:%f:%f:%f:%f:%f:%f:%f",	&lIndex,
															&stRotation.m_stRight.x,
															&stRotation.m_stRight.y,
															&stRotation.m_stRight.z,
															&stRotation.m_stUp.x,
															&stRotation.m_stUp.y,
															&stRotation.m_stUp.z,
															&stRotation.m_stAt.x,
															&stRotation.m_stAt.y,
															&stRotation.m_stAt.z		);

			pcsData	= m_csEffectDataAdmin2.GetData(pstAgcdUseEffectSet, lIndex, TRUE);
			if (!pcsData)
				return FALSE;

			if (pcsData->m_pcsRotation)
				return FALSE;

			pcsData->m_pcsRotation	= new AgcdUseEffectSetDataRotation();
			if (!pcsData->m_pcsRotation)
				return FALSE;

			pcsData->m_pcsRotation->m_stRight.x	= stRotation.m_stRight.x;
			pcsData->m_pcsRotation->m_stRight.y	= stRotation.m_stRight.y;
			pcsData->m_pcsRotation->m_stRight.z	= stRotation.m_stRight.z;

			pcsData->m_pcsRotation->m_stUp.x	= stRotation.m_stUp.x;
			pcsData->m_pcsRotation->m_stUp.y	= stRotation.m_stUp.y;
			pcsData->m_pcsRotation->m_stUp.z	= stRotation.m_stUp.z;

			pcsData->m_pcsRotation->m_stAt.x	= stRotation.m_stAt.x;
			pcsData->m_pcsRotation->m_stAt.y	= stRotation.m_stAt.y;
			pcsData->m_pcsRotation->m_stAt.z	= stRotation.m_stAt.z;
		}
		else if (!strcmp(pStream->GetValueName(), AGCMEVENTEFFECT_CUSTOM_FLAGS_INI_NAME))
		{
			pStream->GetValue(szTemp, 512);
			sscanf(szTemp, "%d", &pstAgcdUseEffectSet->m_ulCustomFlags);
		}
		else if (pfParam)
		{
			if (	!(pfParam(this, (PVOID)(pStream), (PVOID)(pcsBase)))	) return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::UseEffectSetStreamWrite(ApModuleStream *pStream, AgcdUseEffectSet *pstAgcdUseEffectSet)
{
	CHAR					szValueName[256];
	CHAR					szValue[256];

	memset(szValueName, 0, 256);
	memset(szValue, 0, 256);

	if (pstAgcdUseEffectSet->m_ulCustomFlags)
	{
		sprintf(szValue, "%d", pstAgcdUseEffectSet->m_ulCustomFlags);
		pStream->WriteValue(AGCMEVENTEFFECT_CUSTOM_FLAGS_INI_NAME, szValue);
	}

	AgcdUseEffectSetList	*pcsCurrent	= pstAgcdUseEffectSet->m_pcsHead;
	while (pcsCurrent)
	{
		if ((!pcsCurrent->m_csData.m_ulEID) && (!pcsCurrent->m_csData.m_pszSoundName))
		{
			pcsCurrent	= pcsCurrent->m_pcsNext;
			continue;
		}

		// Condition flags
		if (pcsCurrent->m_csData.m_ulConditionFlags)
		{
			sprintf(szValueName,		"%s%d",						AGCMEVENTEFFECT_CONDITION_FLAGS_INI_NAME, pcsCurrent->m_csData.m_ulIndex);
			sprintf(szValue,			"%d:%d",					pcsCurrent->m_csData.m_ulIndex, pcsCurrent->m_csData.m_ulConditionFlags);

			pStream->WriteValue(szValueName, szValue);
		}

		// Spirit stone condition flags
		// Use effect set
//		if (pstData->m_ulEID) 사운드에서 쓰일 경우가 있으므로 체크하지 말자!
		{
			sprintf(szValueName,		"%s%d",						AGCMEVENTEFFECT_EFFECT_DATA_INI_NAME, pcsCurrent->m_csData.m_ulIndex);
			print_compact_format(szValue,"%d:%d:%f:%f:%f:%f:%d:%d",	pcsCurrent->m_csData.m_ulIndex,
																	pcsCurrent->m_csData.m_ulEID,
																	(pcsCurrent->m_csData.m_pv3dOffset) ? (pcsCurrent->m_csData.m_pv3dOffset->x) : (0.0f),
																	(pcsCurrent->m_csData.m_pv3dOffset) ? (pcsCurrent->m_csData.m_pv3dOffset->y) : (0.0f),
																	(pcsCurrent->m_csData.m_pv3dOffset) ? (pcsCurrent->m_csData.m_pv3dOffset->z) : (0.0f),
																	pcsCurrent->m_csData.m_fScale,
																	pcsCurrent->m_csData.m_lParentNodeID,
																	pcsCurrent->m_csData.m_ulStartGap		);
			pStream->WriteValue(szValueName, szValue);
		}

		// Rotation
		if (pcsCurrent->m_csData.m_pcsRotation)
		{
			sprintf(szValueName,		"%s%d",								AGCMEVENTEFFECT_EFFECT_ROTATION_INI_NAME, pcsCurrent->m_csData.m_ulIndex);
			print_compact_format(szValue,"%d:%f:%f:%f:%f:%f:%f:%f:%f:%f",	pcsCurrent->m_csData.m_ulIndex,
																			pcsCurrent->m_csData.m_pcsRotation->m_stRight.x,
																			pcsCurrent->m_csData.m_pcsRotation->m_stRight.y,
																			pcsCurrent->m_csData.m_pcsRotation->m_stRight.z,
																			pcsCurrent->m_csData.m_pcsRotation->m_stUp.x,
																			pcsCurrent->m_csData.m_pcsRotation->m_stUp.y,
																			pcsCurrent->m_csData.m_pcsRotation->m_stUp.z,
																			pcsCurrent->m_csData.m_pcsRotation->m_stAt.x,
																			pcsCurrent->m_csData.m_pcsRotation->m_stAt.y,
																			pcsCurrent->m_csData.m_pcsRotation->m_stAt.z		);
			pStream->WriteValue(szValueName, szValue);
		}

		// CUSTOM DATA
		if (pcsCurrent->m_csData.m_pszCustData)
		{
			sprintf(szValueName,		"%s%d",						AGCMEVENTEFFECT_EFFECT_CUST_DATA_INI_NAME, pcsCurrent->m_csData.m_ulIndex);
			sprintf(szValue,			"%d:%s",					pcsCurrent->m_csData.m_ulIndex, pcsCurrent->m_csData.m_pszCustData);

			pStream->WriteValue(szValueName, szValue);
		}

		// Sound
		if (pcsCurrent->m_csData.m_pszSoundName)
		{
			sprintf(szValueName,		"%s%d",						AGCMEVENTEFFECT_SOUND_DATA_INI_NAME, pcsCurrent->m_csData.m_ulIndex);
			sprintf(szValue,			"%d:%s",					pcsCurrent->m_csData.m_ulIndex, pcsCurrent->m_csData.m_pszSoundName);

			pStream->WriteValue(szValueName, szValue);
		}

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmEventEffect::RemoveEquipmentsEffect(AgpdCharacter *pcsAgpdCharacter)
{
	AgpdItem	*pcsAgpdItem	= GetEquipItem(m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_HAND_RIGHT);
	if (pcsAgpdItem)
		RemoveAllEffect((ApBase *)(pcsAgpdItem));

	pcsAgpdItem	= GetEquipItem(m_pcsAgpmItem, pcsAgpdCharacter, AGPMITEM_PART_HAND_LEFT);
	if (pcsAgpdItem)
		RemoveAllEffect((ApBase *)(pcsAgpdItem));

	return TRUE;
}

BOOL	AgcmEventEffect::PlaySocialSound( AgpdCharacter* pstAgpdCharacter , AgpdCharacterSocialType eType )
{
#ifdef _AREA_JAPAN_
	// 일본에서는 소셜사운드를 플레이하지 않습니다.
	return TRUE;
#endif

	if( !m_pcsAgpmCharacter->IsPC( pstAgpdCharacter ) )
		return FALSE;

	AgcdCharacter* pcsAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	if( m_pcsAgcmSound && pcsAgcdCharacter && pcsAgcdCharacter->m_pClump )
	{
		if ( PlaySound( AGCMEVENTEFFECT_EFFECT_SOUND_PATH, AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH,
						m_csAgcmEventEffectData.GetSocialSound(pstAgpdCharacter->ApdCharacter::m_pcsCharacterTemplate->ApBase::m_lID, eType),
						pstAgpdCharacter->m_lID == m_lSelfCID ? TRUE : FALSE, pcsAgcdCharacter->m_pClump ) )
			return TRUE;
	}

	return FALSE;
}

BOOL AgcmEventEffect::RemoveAllEffect(ApBase *pcsBase)
{
	if( !m_pcsAgcmEff2 )		return FALSE;

	static CSLogIndicator	_sIndicator( "AgcmEventEffect::RemoveAllEffect" );
	CSLogAutoIndicator	stAutoIndicator( &_sIndicator );

	AgcmEventEffectList		*pList			=	GetEventEffectList(pcsBase);
	ListUseEffectIter		Iter			=	pList->m_pListUseEffect->begin();
	EFFCTRL_SET*			pCtrlSet		=	NULL;

	for( ; Iter != pList->m_pListUseEffect->end() ; ++Iter )
	{
		pCtrlSet			=	(*Iter);
		m_pcsAgcmEff2->RemoveEffSet( pCtrlSet , FALSE ) ;
	}

	pList->DestroyUseEffectList();

	if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
	{
		if (!RemoveEquipmentsEffect((AgpdCharacter *)(pcsBase)))
			return FALSE;

		if (!RemoveAllContinuativeSkillEventEffect((AgcdEventEffectCharacter *)(GetEventEffectData(pcsBase))))
			return FALSE;
	}
	else if (pcsBase->m_eType == APBASE_TYPE_ITEM)
	{
		AgpdItem *	pstAgpdItem = (AgpdItem *) pcsBase;

		// 목걸이인 경우
		AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
		if( pcmItem )
		{
			if(((AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE)
			{
				AgcdItem* pcdItem = pcmItem->GetItemData( pstAgpdItem );
				pcmItem->OnUnEquipItemNecklace( pstAgpdItem->m_pcsCharacter, pstAgpdItem, pcdItem );
			}
		}

		if (!RemoveAllConvertedItemGradeEventEffect((AgcdEventEffectItem *)(GetEventEffectData(pcsBase))))
			return FALSE;
	}

	return TRUE;
}

BOOL AgcmEventEffect::PreRemoveCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmEventEffect		*pThis				= (AgcmEventEffect *)(pClass);

	if (!pThis->m_pcsAgcmEff2)
		return FALSE;

	AgpdCharacter		*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	if (pcsAgpdCharacter->m_lID == pThis->m_lSelfCID)
		pThis->m_pcsAgcmEff2->SetMainCharacterFrame(NULL , NULL);

	return TRUE;
}

BOOL AgcmEventEffect::PreStop3DSound(ApBase *pcsBase)
{
	RpClump *pstClump	= NULL;

	switch (pcsBase->m_eType)
	{
	case APBASE_TYPE_OBJECT:
		{
			AgcdObject *pcObject	= m_pcsAgcmObject->GetObjectData((ApdObject *)(pcsBase));
			if (!pcObject)
				return FALSE;

			if (!pcObject->m_stGroup.m_pstList)
				return FALSE;

			pstClump		= pcObject->m_stGroup.m_pstList->m_csData.m_pstClump;
		}
		break;

	case APBASE_TYPE_CHARACTER:
		{
			AgcdCharacter *pcObject	= m_pcsAgcmCharacter->GetCharacterData((AgpdCharacter *)(pcsBase));
			if (!pcObject)
				return FALSE;

			pstClump		= pcObject->m_pClump;
		}
		break;

	case APBASE_TYPE_ITEM:
		{
			AgpdItem *ppObject	= (AgpdItem *)(pcsBase);

			if (ppObject->m_eStatus == AGPDITEM_STATUS_EQUIP)
			{
				AgpdCharacter *pcsAgpdOwner	= ppObject->m_pcsCharacter;
				if (!pcsAgpdOwner)
					return FALSE;

				AgcdCharacter *pcsAgcdOwner = m_pcsAgcmCharacter->GetCharacterData(pcsAgpdOwner);
				if (!pcsAgcdOwner)
					return FALSE;

				pstClump		= pcsAgcdOwner->m_pClump;
			}
			else
			{
				AgcdItem *pcObject	= m_pcsAgcmItem->GetItemData(ppObject);
				if (!pcObject)
					return FALSE;

				pstClump		= pcObject->m_pstClump;
			}
		}
		break;
	}

	if (!pstClump)
		return FALSE;	

	return m_pcsAgcmSound->PreStop3DSound(pstClump);;
}

/******************************************************************************
* Purpose :
*
* 030603. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::RemoveCommonObjectCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect		*pThis			= (AgcmEventEffect *)(pClass);
	ApBase				*pBase			= (ApBase *)(pData);

	pThis->ReleaseCheckTimeDataList(pBase);

	pThis->PreStop3DSound(pBase);

	return pThis->RemoveAllEffect(pBase);
}

/******************************************************************************
* Purpose :
*
* 030603. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::RemoveEffectCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData))
		return FALSE;

	CCSLog	stLog( 11 );

	AgcmEventEffect		*pThis			= (AgcmEventEffect *)(pClass);
	EFFCTRL_SET			*pEffectNode	= (EFFCTRL_SET *)(pData);
	ApBase				*pBase			= (ApBase *)(pCustData);

	AgcmEventEffectList *pList = NULL;
	
	switch(pBase->m_eType)
	{
	case APBASE_TYPE_OBJECT:
		{
			ApdObject *pObject = pThis->m_pcsApmObject->GetObject(pBase->m_lID);
			pList = pThis->GetEventEffectList(pObject);
		}
		break;

	case APBASE_TYPE_CHARACTER:
		{
			AgpdCharacter *pCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(pBase->m_lID);
			pList = pThis->GetEventEffectList(pCharacter);
		}
		break;

	case APBASE_TYPE_ITEM:
		{
			AgpdItem *pItem = pThis->m_pcsAgpmItem->GetItem(pBase->m_lID);
			pList = pThis->GetEventEffectList(pItem);
		}
		break;
	}

	if(!pList) return FALSE;
	if(!pList->RemoveUseEffectList(pEffectNode)) return FALSE;

	return TRUE;
}

/******************************************************************************
* Purpose : stAgcmEffectUseInfo 구조를 만들고 effect node를 리스트(AgcmEventEffectList에)에 넣는다.
*
* 032703. Bob Jung
******************************************************************************/
VOID AgcmEventEffect:: SetEffect(stEffUseInfo	*pstInfo,
								AgcmEventEffectList *pstList,
								EFFCTRL_SET	**ppstNode,
								BOOL bForceImmediate)
{
	if (!m_pcsAgcmEff2)
		return; // 잠시 스킵~(BOB, 210904)

	if ((pstInfo->m_pBase) && (pstInfo->m_pBase->m_eType == APBASE_TYPE_OBJECT))
		pstInfo->m_ulFlags	|= stEffUseInfo::E_FLAG_STATICEFFECT;


	if( pstInfo->m_pBase && pstInfo->m_pBase->m_eType == APBASE_TYPE_CHARACTER )
	{
		AgpdCharacter*	pCharacter	=	(AgpdCharacter*)pstInfo->m_pBase;

		if( pCharacter && m_pcsAgcmCharacter->IsSelectTarget( pCharacter ) )
		{
			AgcdCharacter*	pcsAgcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( pCharacter );
			AgpdCharacter*	pTarget				=	m_pcsAgpmCharacter->GetCharacter( pcsAgcdCharacter->m_lSelectTargetID );
			if( pTarget )
			{
				pstInfo->m_vBaseDir.x	=	(pCharacter->m_stPos	-	pTarget->m_stPos).x;
				pstInfo->m_vBaseDir.y	=	(pCharacter->m_stPos	-	pTarget->m_stPos).y;
				pstInfo->m_vBaseDir.z	=	(pCharacter->m_stPos	-	pTarget->m_stPos).z;

				RwV3dNormalize( &pstInfo->m_vBaseDir , &pstInfo->m_vBaseDir );
			}

		}
	}


	EFFCTRL_SET *pEffectNode = m_pcsAgcmEff2->UseEffSet(pstInfo, bForceImmediate);
	
	if (pEffectNode)
	{
		if (ppstNode)
		{
			*(ppstNode)		= pEffectNode;
		}

		if (pstList && pEffectNode->GetPtrEffSet() ) //. 로딩이 완료된 Effset을 갖는 pEffectNode일때만.
		{
			pstList->AddUseEffectList(pEffectNode);
		}
	}

	DEF_SAFEDELETE( pstInfo );
}

VOID AgcmEventEffect::SetSoundDataNum(INT32 lNum)
{
	m_csAgcmEventEffectData.SetMaxSoundData(lNum);
}

BOOL AgcmEventEffect::ReadMovingSoundData(CHAR *szName, BOOL bDecryption)
{
	return m_csAgcmEventEffectData.ReadMovingSoundData(szName, bDecryption);
}

BOOL AgcmEventEffect::ReadConvertedItemGradeEffectData(CHAR *szName, BOOL bDecryption)
{
	return m_csAgcmEventEffectData.ReadConvertedItemGradeEffectData(szName, bDecryption);
}

BOOL AgcmEventEffect::ReadConvertedArmourAttrEffectData(CHAR * szName, BOOL bDecryption)
{
	return m_csAgcmEventEffectData.ReadConvertedArmourEffectData(szName, bDecryption);
}

BOOL AgcmEventEffect::ReadSpiritStoneHitEffectData(CHAR *szName, BOOL bDecryption)
{
	return m_csAgcmEventEffectData.ReadSpiritStoneHitEffectData(szName, bDecryption);
}

BOOL AgcmEventEffect::ReadSpiritStoneAttrEffectData(CHAR *szName, BOOL bDecryption)
{
	return m_csAgcmEventEffectData.ReadSpiritStoneAttrEffectData(szName, bDecryption);
}

BOOL AgcmEventEffect::ReadCommonCharEffectData(CHAR *szName, BOOL bDecryption)
{
	return m_csAgcmEventEffectData.ReadCommonCharEffectData(szName, bDecryption);
}

BOOL AgcmEventEffect::ReadSocialAnimationData(CHAR *szSocialPC, CHAR *szSocialGM, CHAR *szAnimationPath, BOOL bDecryption)
{
	return m_csAgcmEventEffectData.ReadSocialAnimationData(szSocialPC, szSocialGM, szAnimationPath, bDecryption);
}

eAcReturnType AgcmEventEffect::SetAttrDamageEffect(AgpdCharacter *pcsAgpdAttacker, AgpdCharacter *pcsAgpdTarget)
{
	AgpdFactor* pcsAgpdFactor = m_pcsAgcmCharacter->GetNextReceivedActionDamageFactor( pcsAgpdAttacker, pcsAgpdTarget, AGPDCHAR_ACTION_TYPE_ATTACK );
	if( !pcsAgpdFactor )		return E_AC_RETURN_TYPE_SKIP;

	INT32 lDamage = 0;
	if( !m_pcsAgpmFactors->GetValue( pcsAgpdFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL ) )
		return E_AC_RETURN_TYPE_FAILURE;

	if( lDamage >= 0 )
		return E_AC_RETURN_TYPE_FAILURE;

	INT32 lSpiritStoneType = -1;
	if( lSpiritStoneType == -1 && m_pcsAgpmFactors->GetValue( pcsAgpdFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC ) )
		if( lDamage < 0 )
			lSpiritStoneType = E_SPIRIT_STONE_TYPE_MAGIC;

	if(	lSpiritStoneType == -1 && m_pcsAgpmFactors->GetValue( pcsAgpdFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_WATER	) )
		if (lDamage < 0)
			lSpiritStoneType = E_SPIRIT_STONE_TYPE_WATER;

	if( lSpiritStoneType == -1 && m_pcsAgpmFactors->GetValue( pcsAgpdFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_FIRE ) )
		if (lDamage < 0)
			lSpiritStoneType = E_SPIRIT_STONE_TYPE_FIRE;

	if( lSpiritStoneType == -1 && m_pcsAgpmFactors->GetValue( pcsAgpdFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_EARTH ) )
		if (lDamage < 0)
			lSpiritStoneType = E_SPIRIT_STONE_TYPE_EARTH;

	if(	lSpiritStoneType == -1 && m_pcsAgpmFactors->GetValue( pcsAgpdFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_AIR ) )
		if (lDamage < 0)
			lSpiritStoneType = E_SPIRIT_STONE_TYPE_AIR;

	if( lSpiritStoneType == -1 )		return E_AC_RETURN_TYPE_SKIP;

	for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
	{
		AgcdUseEffectSetData* pcsEffectSetData = m_csAgcmEventEffectData.GetSpiritStoneHitEffectData( lSpiritStoneType, 0, lCount );
		if( !pcsEffectSetData || !pcsEffectSetData->m_ulEID )		break;

		if( !pcsEffectSetData->m_pszCustData )
		{
			CHAR	szTempCustData[256], szAddData[16];
			szTempCustData[0]	= NULL;

			sprintf(szAddData, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_TARGET_OPTION, E_AGCD_EVENT_EFFECT_CDI_TARGET_OPTION_ALL);
			AddCustData(szTempCustData, szAddData);

			sprintf(szAddData, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION, E_AGCD_EVENT_EFFECT_CDI_DIRECTION_OPTION_ORG_TO_TAR);
			AddCustData(szTempCustData, szAddData);

			pcsEffectSetData->m_pszCustData = new CHAR [strlen(szTempCustData) + 1];
			strcpy( pcsEffectSetData->m_pszCustData, szTempCustData );
		}

		if( !SetBaseEffect( (ApBase*)(pcsAgpdAttacker), pcsEffectSetData) )
			return E_AC_RETURN_TYPE_FAILURE;
	}

	return E_AC_RETURN_TYPE_SUCCESS;
}

eAcReturnType AgcmEventEffect::SetSpiritStoneHitEffect(AgpdCharacter *pcsAgpdAttacker)
{
	AgpdItem* pcsAgpdItem = GetEquipItem( m_pcsAgpmItem, pcsAgpdAttacker, AGPMITEM_PART_HAND_RIGHT );
	if ( !pcsAgpdItem )
	{
		pcsAgpdItem = GetEquipItem( m_pcsAgpmItem, pcsAgpdAttacker, AGPMITEM_PART_HAND_LEFT );
		if( !pcsAgpdItem )
			return E_AC_RETURN_TYPE_SKIP;
	}

	if( !m_pcsAgpmItemConvert->GetNumSpiritStone( pcsAgpdItem ) )		return E_AC_RETURN_TYPE_SKIP;

	INT32 lNumSpiritStone			= 0;
	INT32 lRank						= 0;
	INT32 lLevel					= 0;
	INT32 lNumConvertedWaterAttr	= 0;
	INT32 lNumConvertedFireAttr		= 0;
	INT32 lNumConvertedEarthAttr	= 0;
	INT32 lNumConvertedAirAttr		= 0;
	INT32 lNumConvertedMagicAttr	= 0;
	GetSpiritStoneInfo( pcsAgpdItem, &lNumSpiritStone, &lRank, &lLevel, &lNumConvertedWaterAttr, &lNumConvertedFireAttr, &lNumConvertedEarthAttr, &lNumConvertedAirAttr, &lNumConvertedMagicAttr );
		
	for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
	{
		AgcdUseEffectSetData* pcsEffectSetData = m_csAgcmEventEffectData.GetSpiritStoneHitEffectData( lNumSpiritStone, lRank, lLevel, 
										lNumConvertedWaterAttr, lNumConvertedFireAttr, lNumConvertedEarthAttr, lNumConvertedAirAttr, lNumConvertedMagicAttr, lCount );
		if( !pcsEffectSetData || !pcsEffectSetData->m_ulEID )		break;
			
		if( !pcsEffectSetData->m_pszCustData )
		{
			CHAR	szTempCustData[256], szAddData[16];
			szTempCustData[0]	= NULL;

			sprintf(szAddData, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_TARGET_OPTION, E_AGCD_EVENT_EFFECT_CDI_TARGET_OPTION_ALL);
			AddCustData(szTempCustData, szAddData);

			sprintf(szAddData, "%c%d", D_AGCD_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION, E_AGCD_EVENT_EFFECT_CDI_DIRECTION_OPTION_ORG_TO_TAR);
			AddCustData(szTempCustData, szAddData);

			pcsEffectSetData->m_pszCustData = new CHAR [strlen(szTempCustData) + 1];
			strcpy(pcsEffectSetData->m_pszCustData, szTempCustData);
		}

		if( !SetBaseEffect( (ApBase*)(pcsAgpdAttacker), pcsEffectSetData ) )
			return E_AC_RETURN_TYPE_FAILURE;
	}

	return E_AC_RETURN_TYPE_SUCCESS;
}

BOOL AgcmEventEffect::SetConvertedItemGradeEffect(AgpdItem *pcsAgpdConvertedItem, AgcdUseEffectSetData *pstEffectSetData)
{
	EFFCTRL_SET	*pstEffectSetNode	= NULL;
	if (!SetBaseEffect((ApBase *)(pcsAgpdConvertedItem), pstEffectSetData, -1, FALSE, &pstEffectSetNode)) return FALSE;
	if (!pstEffectSetNode) return FALSE;

	AgcdEventEffectItem	*pcsAttachedData	= (AgcdEventEffectItem *)(GetEventEffectData((ApBase*)(pcsAgpdConvertedItem)));
	if (!pcsAttachedData) return FALSE;

	if (!pcsAttachedData->m_csConvertedItemGradEffect.AddNode(AGCMEVENTEFFECT_CONVERTED_ITEM_G_OFFSET, pstEffectSetNode)) return FALSE;

	return TRUE;
}

BOOL AgcmEventEffect::StopConvertedWeaponGradeEventEffect(AgpdItem *pstAgpdConvertedItem)
{
	AgcdEventEffectItem* pcsData = (AgcdEventEffectItem *)(GetEventEffectData((ApBase *)(pstAgpdConvertedItem)));
	if( !pcsData ) return FALSE;

	return RemoveAllConvertedItemGradeEventEffect(pcsData);
}

BOOL AgcmEventEffect::StartConvertedWeaponGradeEventEffect(AgpdItem *pstAgpdConvertedItem)
{
	if( !pstAgpdConvertedItem->m_pcsItemTemplate ) return FALSE;

	AgpdItemTemplateEquip* pcsEquipTemplate	 = (AgpdItemTemplateEquip *)(pstAgpdConvertedItem->m_pcsItemTemplate);
	if( pcsEquipTemplate->m_nType != AGPMITEM_TYPE_EQUIP ) return FALSE;

	INT32 lNumConverted = m_pcsAgpmItemConvert->GetPhysicalConvertLevel( pstAgpdConvertedItem );
	AgcdUseEffectSetData* pcsData = m_csAgcmEventEffectData.GetConvertedWeaponGradeEffectData( lNumConverted );

	return pcsData ? SetConvertedItemGradeEffect( pstAgpdConvertedItem, pcsData ) : TRUE;
}

BOOL AgcmEventEffect::SetConvertedItemEffect(AgpdItem *pstAgpdItem, AgcdItem *pstAgcdItem)
{
	// 목걸이인 경우
	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	if( pcmItem )
	{
		if(((AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE)
		{
			pcmItem->OnEquipItemNecklace( pstAgpdItem->m_pcsCharacter, pstAgpdItem, pstAgcdItem );
		}
	}

	// 방어구의 경우
	if (((AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)
	{
		if (((AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate))->m_nPart == AGPMITEM_PART_BODY)
		{
			StartConvertedArmourAttrEventEffect(pstAgpdItem);
		}
	}
	else if (m_pcsAgpmItemConvert->GetTotalConvertPoint(pstAgpdItem) > 0)
	{
		StartConvertedWeaponAttrEventEffect(pstAgpdItem, pstAgcdItem);
	}

	// 무기의 경우
	if( ( ( AgpdItemTemplateEquip* )( pstAgpdItem->m_pcsItemTemplate ) )->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
	{
		StartConvertedWeaponAttrEventEffect( pstAgpdItem, pstAgcdItem );
		StartConvertedWeaponGradeEventEffect( pstAgpdItem );
	}

	return TRUE;
}

BOOL AgcmEventEffect::ReleaseConvertedItemEffect(AgpdItem *pstAgpdItem, AgcdItem *pstAgcdItem)
{
	if (!StopConvertedWeaponAttrEventEffect(pstAgpdItem, pstAgcdItem)) return FALSE;
	if (!StopConvertedWeaponGradeEventEffect(pstAgpdItem)) return FALSE;
	if (!StopConvertedArmourAttrEventEffect(pstAgpdItem)) return FALSE;
	return TRUE;
}

BOOL AgcmEventEffect::ResetConvertedItemEffect(AgpdItem *pstAgpdItem, AgcdItem *pstAgcdItem)
{
	if (!ReleaseConvertedItemEffect(pstAgpdItem, pstAgcdItem)) return FALSE;
	return SetConvertedItemEffect(pstAgpdItem, pstAgcdItem);
}

BOOL AgcmEventEffect::StartConvertedWeaponAttrEventEffect(AgpdItem *pstAgpdConvertedItem, AgcdItem *pstAgcdConvertedItem)
{
	if (((AgpdItemTemplateEquip *)(pstAgpdConvertedItem->m_pcsItemTemplate))->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON)
		return TRUE; // skip!

	AgcdEventEffectItem	*pcsData				= (AgcdEventEffectItem *)(GetEventEffectData((ApBase *)(pstAgpdConvertedItem)));
	if (!pcsData) return FALSE;

	pcsData->m_csConvertedItemAttrEffect.StartConvertedItemEffect();
	RemoveAllEffect((ApBase *)(pstAgpdConvertedItem));
	EventItemSpiritStoneAttrCB((PVOID)pstAgpdConvertedItem, (PVOID)this, (PVOID)pstAgcdConvertedItem);
	return TRUE;
}

BOOL AgcmEventEffect::StopConvertedWeaponAttrEventEffect(AgpdItem *pstAgpdConvertedItem, AgcdItem *pstAgcdConvertedItem)
{
	AgcdEventEffectItem	*pcsData	= (AgcdEventEffectItem *)(GetEventEffectData((ApBase *)(pstAgpdConvertedItem)));
	if (!pcsData) return FALSE;

	pcsData->m_csConvertedItemAttrEffect.InitConvertedItemEffect();

	return TRUE;
}

RpAtomic *AgcmEventEffect::ReleaseConvertedArmourAttrEffectCB(RpAtomic *pstAtomic, void *pvData)
{
	switch(  pstAtomic->iPartID )
	{
	case AGPMITEM_PART_BODY:
	case AGPMITEM_PART_ARMS:
	case AGPMITEM_PART_HANDS:
	case AGPMITEM_PART_LEGS:
	case AGPMITEM_PART_FOOT:
	case AGPMITEM_PART_ARMS2:
		{
			bool noShader = ((((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) < 0x0101);
			if( !noShader )
			{
				pstAtomic->stRenderInfo.pData1	= NULL;
				AgcmEventEffect::m_pcsThisAgcmEventEffect->m_pcsAgcmRender->ReturnUVAnimRenderCB( pstAtomic );
			}
		}
		break;
	}

	return pstAtomic;
}

VOID AgcmEventEffect::SetConvertedArmourEffectData(RpAtomic *pstAtomic, RpMaterial **ppstMaterial)
{
	bool noShader = ((((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) < 0x0101);

	pstAtomic->stRenderInfo.pData1 = (void*)(ppstMaterial);

	if( ppstMaterial && !noShader )
		m_pcsAgcmRender->SetUVAnimRenderCB(pstAtomic);
}

RpAtomic *AgcmEventEffect::SetConvertedArmourAttrEffectCB(RpAtomic *pstAtomic, void *pvData)
{
	switch( pstAtomic->iPartID )
	{
	case AGPMITEM_PART_BODY:
	case AGPMITEM_PART_ARMS:
	case AGPMITEM_PART_HANDS:
	case AGPMITEM_PART_LEGS:
	case AGPMITEM_PART_FOOT:
	case AGPMITEM_PART_ARMS2:
		AgcmEventEffect::m_pcsThisAgcmEventEffect->SetConvertedArmourEffectData( pstAtomic, (RpMaterial**)(pvData) );
		break;
	}

	return pstAtomic;
}

eAgcmEventEffectDataSpiritStoneType AgcmEventEffect::GetMaxSpiritStoneType(AgpdItem *pstAgpdConvertedItem, INT32 *plMaxPoint)
{
	*plMaxPoint = 0;

	// 가장 높은 점수의 속성을 보여준다.
	// 단, 우선순위는 Physical 이 제일 높다.
	// 나머지 중 같은 점수가 있다면, 같은 점수 중 발린 순서에 따라 정한다.

	INT32 alPoint[E_SPIRIT_STONE_TYPE_NUM];
	memset( alPoint, 0, sizeof(INT32) * E_SPIRIT_STONE_TYPE_NUM );
	alPoint[E_SPIRIT_STONE_TYPE_FIRE]	= m_pcsAgpmItemConvert->GetSpiritStoneAttrConvertPoint(pstAgpdConvertedItem, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
	alPoint[E_SPIRIT_STONE_TYPE_AIR]	= m_pcsAgpmItemConvert->GetSpiritStoneAttrConvertPoint(pstAgpdConvertedItem, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
	alPoint[E_SPIRIT_STONE_TYPE_WATER]	= m_pcsAgpmItemConvert->GetSpiritStoneAttrConvertPoint(pstAgpdConvertedItem, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
	alPoint[E_SPIRIT_STONE_TYPE_MAGIC]	= m_pcsAgpmItemConvert->GetSpiritStoneAttrConvertPoint(pstAgpdConvertedItem, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
	alPoint[E_SPIRIT_STONE_TYPE_EARTH]	= m_pcsAgpmItemConvert->GetSpiritStoneAttrConvertPoint(pstAgpdConvertedItem, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
	alPoint[E_SPIRIT_STONE_TYPE_PHYSICAL]= m_pcsAgpmItemConvert->GetTotalPhysicalConvertPoint(pstAgpdConvertedItem);
	alPoint[E_SPIRIT_STONE_TYPE_RUNE]	= m_pcsAgpmItemConvert->GetTotalRuneConvertPoint(pstAgpdConvertedItem);

	INT32	lMaxPointIndex = E_SPIRIT_STONE_TYPE_NONE;
	BOOL	bCheckOrder = FALSE;
	for( INT32 lIndex = 0; lIndex < E_SPIRIT_STONE_TYPE_NUM; ++lIndex )
	{
		if (alPoint[lIndex])
		{
			if (*plMaxPoint < alPoint[lIndex])
			{
				*plMaxPoint = alPoint[lIndex];
				lMaxPointIndex = lIndex;
			}
			else if (*plMaxPoint == alPoint[lIndex])
			{
				if (lIndex == E_SPIRIT_STONE_TYPE_PHYSICAL)
				{
					bCheckOrder = FALSE;
					lMaxPointIndex = E_SPIRIT_STONE_TYPE_PHYSICAL;
				}
				else if (lMaxPointIndex != E_SPIRIT_STONE_TYPE_PHYSICAL)
					bCheckOrder = TRUE;
			}
		}
	}

	if( bCheckOrder )
	{
		// 개조 히스토리를 뒤진다.
		AgpdItemConvertADItem* pcsAttachData	= m_pcsAgpmItemConvert->GetADItem(pstAgpdConvertedItem);
		for( INT32 lCount = 0, lSSIndex = -1; lCount < pcsAttachData->m_lNumConvert; ++lCount)
		{
			// 정령석을 발라서 개조된 형태이고
			if( pcsAttachData->m_stSocketAttr[lCount].bIsSpiritStone )
			{
				AgpdItemTemplateUsableSpiritStone* pcsSpiritStone =  (AgpdItemTemplateUsableSpiritStone*)m_pcsAgpmItem->GetItemTemplate( pcsAttachData->m_stSocketAttr[lCount].lTID );
				VERIFY( pcsSpiritStone );
				INT32 lSSIndex = m_csAgcmEventEffectData.SearchSpiritStoneIndex( pcsSpiritStone->m_eSpiritStoneType );
				ASSERT(lSSIndex >= 0);

				if( alPoint[lSSIndex] == *plMaxPoint )
				{
					lMaxPointIndex = lSSIndex;
					break;
				}
			}
			else
			{
				if (alPoint[E_SPIRIT_STONE_TYPE_RUNE] == *plMaxPoint)
				{
					lMaxPointIndex = E_SPIRIT_STONE_TYPE_RUNE;
					break;
				}
			}
		}
	}

	ASSERT(lMaxPointIndex < E_SPIRIT_STONE_TYPE_NUM);

	return (eAgcmEventEffectDataSpiritStoneType) lMaxPointIndex;
}

RpMaterial** AgcmEventEffect::GetConvertedArmourEffectMaterial(AgpdItem *pstAgpdConvertedItem)
{
	INT32 lTotalPoint = m_pcsAgpmItemConvert->GetTotalConvertPoint(pstAgpdConvertedItem);
	INT32 lMaxPoint = lTotalPoint;
	eAgcmEventEffectDataSpiritStoneType	eSSType	= GetMaxSpiritStoneType( pstAgpdConvertedItem, &lMaxPoint );
	if( E_SPIRIT_STONE_TYPE_NONE == eSSType )		return NULL;

	AgcdConvertedArmourFX* pcsFX = m_csAgcmEventEffectData.GetConvertedArmourFX( (INT32)eSSType, lTotalPoint );
	return pcsFX ? pcsFX->m_pacsMaterial : NULL;
}

BOOL AgcmEventEffect::SetConvertedArmourAttrEventEffect(AgpdItem *pstAgpdConvertedArmour)
{
	if( !pstAgpdConvertedArmour->m_pcsCharacter )			return FALSE;

	AgcdCharacter* pstAgcdCharacter = m_pcsAgcmCharacter->GetCharacterData( pstAgpdConvertedArmour->m_pcsCharacter );
	if( !pstAgcdCharacter || !pstAgcdCharacter->m_pClump )	return FALSE;

	RpMaterial** ppstMaterial = GetConvertedArmourEffectMaterial( pstAgpdConvertedArmour );
	RpClumpForAllAtomics( pstAgcdCharacter->m_pClump, SetConvertedArmourAttrEffectCB, (void *)(ppstMaterial) );

	return TRUE;
}

BOOL AgcmEventEffect::StartConvertedArmourAttrEventEffect(AgpdItem *pstAgpdConvertedArmour)
{
	return pstAgpdConvertedArmour->m_pcsItemTemplate ? SetConvertedArmourAttrEventEffect(pstAgpdConvertedArmour) : FALSE;
}

BOOL AgcmEventEffect::StopConvertedArmourAttrEventEffect(AgpdItem *pstAgpdConvertedArmour)
{
	if( !pstAgpdConvertedArmour->m_pcsItemTemplate )		return FALSE;
	if( ((AgpdItemTemplateEquip*)(pstAgpdConvertedArmour->m_pcsItemTemplate))->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR )	return TRUE; // skip!
	if( !pstAgpdConvertedArmour->m_pcsCharacter )			return FALSE;

	AgcdCharacter	*pstAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdConvertedArmour->m_pcsCharacter);
	if( !pstAgcdCharacter || !pstAgcdCharacter->m_pClump )	return FALSE;

	RpClumpForAllAtomics( pstAgcdCharacter->m_pClump, ReleaseConvertedArmourAttrEffectCB, NULL );

	return TRUE;
}

BOOL AgcmEventEffect::UpdateConvertedItemEventEffect(AgpdItem *pstAgpdConvertedItem)
{
	// 아이템 상태가 장착이 아닐경우는 이펙트를 보여줄 필요가 없다.
	if( !m_pcsAgcmItem )		return TRUE;
	if( pstAgpdConvertedItem->m_eStatus != AGPDITEM_STATUS_EQUIP )		return TRUE; // skip!
	
	AgcdItem* pstAgcdConvertedItem = m_pcsAgcmItem->GetItemData(pstAgpdConvertedItem);
	if( pstAgcdConvertedItem->m_lStatus == AGCDITEM_STATUS_INIT )		return TRUE; // skip!

	if( !ResetConvertedItemEffect( pstAgpdConvertedItem, pstAgcdConvertedItem ) ) return FALSE;

	return TRUE;
}

BOOL AgcmEventEffect::EventItemUpdateConvertHistoryCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pData || !pClass ) return FALSE;
	return ((AgcmEventEffect*)(pClass))->UpdateConvertedItemEventEffect( (AgpdItem*)(pData) );
}

BOOL AgcmEventEffect::AttachedDataEventEffectDataConstructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass || !pData ) return FALSE;

	AgcmEventEffect			*pcsThis			= (AgcmEventEffect *)(pClass);
	ApBase					*pcsBase			= (ApBase *)(pData);

	AgcdEventEffect			*pcsData			= (AgcdEventEffect *)(pcsThis->GetEventEffectData(pcsBase));
	if (!pcsData) return FALSE;

	pcsData->m_pcsBase							= pcsBase;

	if (pcsBase->m_eType == APBASE_TYPE_ITEM)
	{
		pcsData->m_eType						= E_AGCD_EE_TYPE_ITEM;
		AgcdEventEffectItem	*pItemData			= (AgcdEventEffectItem *)(pcsData);
		pItemData->m_csConvertedItemAttrEffect.InitConvertedItemEffect();
	}
	else if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
	{
		pcsData->m_eType						= E_AGCD_EE_TYPE_CHARACTER;
	}

	return TRUE;
}

BOOL AgcmEventEffect::AttachedDataEventEffectDataDestructCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect					*pcsThis			= (AgcmEventEffect *)(pClass);
	ApBase							*pcsBase			= (ApBase *)(pData);

	AgcdEventEffect					*pcsData			= (AgcdEventEffect *)(pcsThis->GetEventEffectData(pcsBase));
	if (!pcsData) return FALSE;

	if (pcsData->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgcdEventEffectCharacter	*pCharacterData		= (AgcdEventEffectCharacter *)(pcsData);
		pCharacterData->m_csContinuativeSkillList.RemoveAllNode();
	}

	return TRUE;
}

void *AgcmEventEffect::GetEventEffectData(ApBase *pcsBase)
{
	ASSERT( "kday" && pcsBase && "AgcmEventEffect::GetEventEffectData" );
	if( !pcsBase )		return NULL;

	switch(pcsBase->m_eType)
	{
	case APBASE_TYPE_ITEM:		return m_pcsAgpmItem->GetAttachedModuleData( m_nAttachedIndexItemEventEffect, (PVOID)(pcsBase) );
	case APBASE_TYPE_CHARACTER:	return m_pcsAgpmCharacter->GetAttachedModuleData( m_nAttachedIndexCharEventEffect, (PVOID)(pcsBase) );
	}

	return NULL;
}

BOOL AgcmEventEffect::EventItemSpiritStoneAttrCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect		*pcsThis				= (AgcmEventEffect *)(pClass);
	AgpdItem			*pstAgpdConvertedItem	= (AgpdItem *)(pData);	// 개조된 무기

	ASSERT( pcsThis == (AgcmEventEffect *)(pcsThis->GetModule("AgcmEventEffect")) && "AgcmEventEffect::EventItemSpiritStoneAttrCB");
	AgcdEventEffectItem	*pcsData	= (AgcdEventEffectItem *)(pcsThis->GetEventEffectData((ApBase *)(pstAgpdConvertedItem)));
	if (!pcsData) return FALSE;

	pcsData->m_csConvertedItemAttrEffect.StartConvertedItemEffect();
	//pcsData->m_csConvertedItemAttrEffect.m_ulCountTick	+= (pcsThis->m_ulNowClockCount - pcsData->m_csConvertedItemAttrEffect.m_ulPrevTick);		// Count를 증가하고.
	//pcsData->m_csConvertedItemAttrEffect.m_ulPrevTick	= pcsThis->m_ulNowClockCount;																// 이전 tick을 업데이트하고.

	// Count가 정해진 시간이 지나면 일단 Rotation안시킨다.
	if( pcsData->m_csConvertedItemAttrEffect.m_lCurIndex != -1 )		return TRUE;

	pcsData->m_csConvertedItemAttrEffect.m_lCurIndex++;		// 개조무기 이펙트 인덱스를 증가시키고
	//pcsData->m_csConvertedItemAttrEffect.m_ulCountTick = 0;	// Count는 초기화시킨다.

	INT32					lTotalPoint = pcsThis->m_pcsAgpmItemConvert->GetTotalConvertPoint(pstAgpdConvertedItem);
	INT32					lMaxPoint = lTotalPoint;
	INT32					lMaxPointIndex = pcsThis->GetMaxSpiritStoneType(pstAgpdConvertedItem, &lMaxPoint);
	INT32					lProperPoint = -1;

	if (lMaxPointIndex == E_SPIRIT_STONE_TYPE_NONE)
		return TRUE;

	for (INT32 lCount = AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA - 1; lCount >= 0 ; --lCount)
	{
		// 뒤에서부터 뒤져서 알맞은 넘들 찾아낸다.
		AgcdUseEffectSetData* pcsEffectSetData = pcsThis->m_csAgcmEventEffectData.GetSpiritStoneAttrEffectData( lMaxPointIndex, lCount );
		if( !pcsEffectSetData )		continue;

		INT32 lCurrentPoint = pcsThis->m_csAgcmEventEffectData.GetSpiritStoneAttrPointData(lMaxPointIndex, lCount);
		if( lProperPoint != -1 && lCurrentPoint != lProperPoint )
			break;

		if (lTotalPoint >= lCurrentPoint)
		{
			lProperPoint = lCurrentPoint;
			if( !pcsThis->SetBaseEffect( (ApBase*)(pstAgpdConvertedItem), pcsEffectSetData, 0 ) )
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::NoticeEffectSetTargetStruckAnimCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass)
{
	if (!pClass) return FALSE;

	if( stProcessData.lEffectProcessType != AGCMEFFECT_EFFECT_PROCESS_TYPE_EFFECT_START )		return TRUE;

	AgcmEventEffect *pThis = (AgcmEventEffect*)(pClass);

	AgpdCharacter* pcsAgpdCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(stProcessData.lOwnerCID);
	if( !pcsAgpdCharacter )		return FALSE;

	pcsAgpdCharacter->m_alTargetCID[0]	= stProcessData.lTargetCID;

	return pThis->m_pcsAgcmCharacter->SetStruckAnimation( pcsAgpdCharacter, NULL, (FLOAT)(stProcessData.lCustData) / 100.0f );
}

//Purpose : Player Character가 물위에서 걷거나 뛸때 Effect을 발동시킨다.
BOOL AgcmEventEffect::EventWaveFxTrigger(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData)) return FALSE;

	AgcmEventEffect	*pcsThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)(pData);

	return pcsThis->SetBaseCustomEventEffect(pcsAgpdCharacter, 3633, *((float*)pCustData));
}

ACharacterSkyFlags*	AgcmEventEffect::GetSkySetFlagData( AgpdCharacter* pdCharacter )
{
	return (ACharacterSkyFlags *) AgcmEventEffect::m_pcsThisAgcmEventEffect->m_pcsAgpmCharacter->GetAttachedModuleData( m_nSkyFlagOffset, pdCharacter );
}

BOOL AgcmEventEffect::CBSkyChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( pData );
	ASSERT( pClass );

	ASkySetting* pSkySet = (ASkySetting*)pData;
	AgcmEventEffect* pThis = (AgcmEventEffect*)(pClass);

	INT16 nSkyFlag = DEF_FLAG_CHK( pSkySet->nWeatherEffect1, ASkySetting::SHOW_BREATH ) ? ACharacterSkyFlags::AGPMCHAR_CHARACTER_SKYSET_COLD : ACharacterSkyFlags::AGPMCHAR_CHARACTER_SKYSET_NORMAL;
	return pThis->UpdateNatureStatus(nSkyFlag);
}

BOOL	AgcmEventEffect::UpdateNatureStatus(const INT16 nSkySetFlag)
{
	INT32 lIndex = 0;
	AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);

	while( pcsCharacter )
	{
		if( m_pcsAgpmCharacter->IsPC(pcsCharacter) )
		{
			ACharacterSkyFlags* pstAgcdSkySetFlag = GetSkySetFlagData(pcsCharacter);
			pstAgcdSkySetFlag->m_Flags	= nSkySetFlag;
		}

		pcsCharacter	= m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgcmEventEffect::NoticeEffectDestroyMatchCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass)
{
	if (stProcessData.lEffectProcessType != AGCMEFFECT_EFFECT_PROCESS_TYPE_EFFECT_END)
		return TRUE; // skip

	if (!pClass) return FALSE;

	AgcmEventEffect					*pThis					= (AgcmEventEffect *)(pClass);

	AgpdCharacter					*pcsAgpdCharacter		= pThis->m_pcsAgpmCharacter->GetCharacter(stProcessData.lOwnerCID);
	if (!pcsAgpdCharacter)
		return FALSE;

	if (!pcsAgpdCharacter->m_pcsCharacterTemplate) return FALSE;

	AgcdCharacter				*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if ((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump)) return FALSE;

	AgcmStartEventEffectParams	csParams;

	csParams.m_pstAgcdUseEffectSet		=
		pThis->GetUseEffectSet( pcsAgpdCharacter->m_pcsCharacterTemplate );

	if (!csParams.m_pstAgcdUseEffectSet)
		return FALSE;
	
	if (!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY))
		return TRUE; // skip

	csParams.m_ulConditionFlags				= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY;

	csParams.m_csOrigin.m_pcsClump			= pcsAgcdCharacter->m_pClump;
	csParams.m_csOrigin.m_pcsFrame			= RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

	csParams.m_bSelfCharacter				= (pcsAgpdCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE);

	AgpdCharacter *pstAgpdTarget = pThis->m_pcsAgpmCharacter->GetCharacter(stProcessData.lTargetCID);
	if (!pstAgpdTarget)
		return TRUE;	// skip

	csParams.m_csTarget.m_pcsBase			= (ApBase *)(pstAgpdTarget);
	csParams.m_csOrigin.m_pcsBase			= (ApBase *)(pcsAgpdCharacter);

	AgcdCharacter *pstAgcdTarget			= pThis->m_pcsAgcmCharacter->GetCharacterData(pstAgpdTarget);
	if ((!pstAgcdTarget) || (!pstAgcdTarget->m_pClump)) return FALSE;

	csParams.m_csTarget.m_pcsClump			= pstAgcdTarget->m_pClump;
	csParams.m_csTarget.m_pcsFrame			= RpClumpGetFrame(pstAgcdTarget->m_pClump);
	if (!csParams.m_csTarget.m_pcsFrame) return FALSE;

	csParams.m_csOrigin.m_pcsHierarchy		= pstAgcdTarget->m_pInHierarchy;
	csParams.m_csTarget.m_pcsHierarchy		= pstAgcdTarget->m_pInHierarchy;

	csParams.m_lCID							= stProcessData.lOwnerCID;
	csParams.m_lTCID						= stProcessData.lTargetCID;

	return pThis->StartEventEffect( &csParams, stProcessData.lCustData, NULL, TRUE );
}

BOOL AgcmEventEffect::NoticeEffectSkillDestroyMatchCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass)
{
	if( stProcessData.lEffectProcessType != AGCMEFFECT_EFFECT_PROCESS_TYPE_EFFECT_END )		return TRUE; // skip

	if (!pClass) return FALSE;

	AgcmEventEffect* pThis = (AgcmEventEffect *)(pClass);

	AgpdCharacter* pcsAgpdCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter( stProcessData.lOwnerCID );
	if( !pcsAgpdCharacter )		return FALSE;
	if (!pcsAgpdCharacter->m_pcsCharacterTemplate) return FALSE;

	AgcdSkillAttachTemplateData	*pstSkillData		= pThis->m_pcsAgcmSkill->GetAttachTemplateData((ApBase *)(pcsAgpdCharacter->m_pcsCharacterTemplate));
	if (!pstSkillData) return FALSE;

	AgcdCharacter				*pcsAgcdCharacter	= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdCharacter);
	if ((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump)) return FALSE;

	AgcdSkillADChar				*pcsAgcdSkillADChar	= pThis->m_pcsAgcmSkill->GetCharacterAttachData((ApBase *)(pcsAgpdCharacter));
	if (!pcsAgcdSkillADChar) return FALSE;
	if (pcsAgcdSkillADChar->m_lSkillUsableIndex < 0) return FALSE;
	if( !pstSkillData->m_pacsSkillVisualInfo ) return FALSE;
	if (!pstSkillData->m_pacsSkillVisualInfo[pcsAgcdSkillADChar->m_lSkillUsableIndex]) return FALSE;

	AgcmStartEventEffectParams	csParams;
	csParams.m_pstAgcdUseEffectSet = pstSkillData->m_pacsSkillVisualInfo[pcsAgcdSkillADChar->m_lSkillUsableIndex]->m_pastEffect[pcsAgcdCharacter->m_lCurAnimType2];
		
	if (	(!csParams.m_pstAgcdUseEffectSet) ||
			(!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET))	)
		csParams.m_pstAgcdUseEffectSet	= pstSkillData->m_pacsSkillVisualInfo[pcsAgcdSkillADChar->m_lSkillUsableIndex]->m_pastEffect[AGCMCHAR_AT2_COMMON_DEFAULT];

	if (!csParams.m_pstAgcdUseEffectSet)
		return FALSE;
	
	if (!(csParams.m_pstAgcdUseEffectSet->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY))
		return TRUE; // skip

	csParams.m_ulConditionFlags				= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_DESTROY;

	csParams.m_csOrigin.m_pcsClump			= pcsAgcdCharacter->m_pClump;
	csParams.m_csOrigin.m_pcsFrame			= RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
	if (!csParams.m_csOrigin.m_pcsFrame) return FALSE;

	csParams.m_bSelfCharacter				= (pcsAgpdCharacter->m_lID == pThis->m_lSelfCID ? TRUE : FALSE);

	AgpdCharacter *pstAgpdTarget = pThis->m_pcsAgpmCharacter->GetCharacter(stProcessData.lTargetCID);
	if (!pstAgpdTarget) return TRUE;	// skip

	csParams.m_csTarget.m_pcsBase			= (ApBase *)(pstAgpdTarget);
	csParams.m_csOrigin.m_pcsBase			= (ApBase *)(pcsAgpdCharacter);

	AgcdCharacter *pstAgcdTarget			= pThis->m_pcsAgcmCharacter->GetCharacterData(pstAgpdTarget);
	if ((!pstAgcdTarget) || (!pstAgcdTarget->m_pClump)) return FALSE;

	csParams.m_csTarget.m_pcsClump			= pstAgcdTarget->m_pClump;
	csParams.m_csTarget.m_pcsFrame			= RpClumpGetFrame(pstAgcdTarget->m_pClump);
	if (!csParams.m_csTarget.m_pcsFrame) return FALSE;

	csParams.m_csOrigin.m_pcsHierarchy		= pstAgcdTarget->m_pInHierarchy;
	csParams.m_csTarget.m_pcsHierarchy		= pstAgcdTarget->m_pInHierarchy;

	csParams.m_lCID							= stProcessData.lOwnerCID;
	csParams.m_lTCID						= stProcessData.lTargetCID;

	if (!pThis->StartEventEffect(&csParams, stProcessData.lCustData, NULL, TRUE)) return FALSE;

	if( !pThis->m_container_additionalHitEff.empty() )
	{
		typedef std::vector<RwInt32>::iterator	ITR;
		ITR	it_curr	= pThis->m_container_additionalHitEff.begin();
		ITR	it_last	= pThis->m_container_additionalHitEff.end();

		for( ; it_curr != it_last; ++it_curr )
		{
			pstAgpdTarget = pThis->m_pcsAgpmCharacter->GetCharacter(*it_curr);
			if( !pstAgpdTarget )								continue;
				
			pstAgcdTarget = pThis->m_pcsAgcmCharacter->GetCharacterData(pstAgpdTarget);
			if( !pstAgcdTarget || !pstAgcdTarget->m_pClump )	continue;
				

			
			csParams.m_csTarget.m_pcsBase			= (ApBase *)(pstAgpdTarget);
			csParams.m_csTarget.m_pcsClump			= pstAgcdTarget->m_pClump;
			csParams.m_csTarget.m_pcsFrame			= RpClumpGetFrame(pstAgcdTarget->m_pClump);
			if (!csParams.m_csTarget.m_pcsFrame) continue;

			csParams.m_csOrigin.m_pcsHierarchy		= pstAgcdTarget->m_pInHierarchy;
			csParams.m_csTarget.m_pcsHierarchy		= pstAgcdTarget->m_pInHierarchy;

			csParams.m_lTCID						= stProcessData.lTargetCID;

			if( !pThis->StartEventEffect( &csParams, stProcessData.lCustData, NULL, TRUE ) ) continue;
		}

		pThis->m_container_additionalHitEff.clear();
	}

	return TRUE;
}

BOOL AgcmEventEffect::NoticeEffectHitEffectCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass)
{
	if( stProcessData.lEffectProcessType != AGCMEFFECT_EFFECT_PROCESS_TYPE_EFFECT_MISSLEEND )		return TRUE;

	if( !pClass )
		return FALSE;

	AgcmEventEffect* pThis = (AgcmEventEffect *)(pClass);

	AgpdCharacter* pcsAgpdTargetCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(stProcessData.lTargetCID);
	if( !pcsAgpdTargetCharacter )		return FALSE;

	AgcdCharacter* pcsAgcdTargetCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdTargetCharacter);
	if( !pcsAgcdTargetCharacter )		return FALSE;

	AgpdItemTemplate* pcsAgpdItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(0);
	if( !pcsAgpdItemTemplate )			return FALSE;

	AgcdUseEffectSet* pstAgcdUseEffectSet = pThis->GetUseEffectSet(pcsAgpdItemTemplate);
	if( !pstAgcdUseEffectSet )			return FALSE;

	return pThis->SetCharTargetStruck( pcsAgpdTargetCharacter, pcsAgcdTargetCharacter, pstAgcdUseEffectSet );
}

INT32 AgcmEventEffect::GetWeaponType(AgpdItem *pcsAgpdItem)
{
	if( !pcsAgpdItem )		return -1;

	AgpdItemTemplateEquipWeapon* pcsWeapon = (AgpdItemTemplateEquipWeapon *)(pcsAgpdItem->m_pcsItemTemplate);
	if( pcsWeapon->m_nType != AGPMITEM_TYPE_EQUIP )			return -1;
	if( pcsWeapon->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON )	return -1;
	if( pcsWeapon->m_nWeaponType < AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD || pcsWeapon->m_nWeaponType >= AGPMITEM_EQUIP_WEAPON_TYPE_NUM )	return -1;

	return pcsWeapon->m_nWeaponType;
}

UINT32 AgcmEventEffect::PlaySound( const CHAR *szSoundPath, const CHAR *szMonoPath, const CHAR *szSoundName, BOOL bSelfCharacter, RpClump *pstParentClump, BOOL bLoop, AEE_CharAnimAttachedData *pcsAttachedData, FLOAT fPlaySpeed )
{
	UINT32	ulIndex	= 0;
	CHAR	szSoundPathName[256];	

	if( bSelfCharacter || m_bForceStereo )
	{
		PROFILE("PlaySound - sample");

		sprintf( szSoundPathName, "%s%s", szSoundPath, szSoundName );
		if( m_pcsAgcmSound )
			ulIndex	= m_pcsAgcmSound->PlaySampleSound( szSoundPathName, bLoop ? AGCMEVENTEFFECT_SOUND_LOOP_COUNT : 1, 1.1f, fPlaySpeed );
	}
	else
	{
		if( m_lSelfCID && pstParentClump )
		{
			float volume = 1.1f;

			sprintf(szSoundPathName, "%s%s", szMonoPath, szSoundName);
			if( m_pcsAgcmSound )
				ulIndex	= m_pcsAgcmSound->Play3DSound( szSoundPathName, bLoop ? AGCMEVENTEFFECT_SOUND_LOOP_COUNT : 1, volume, TRUE, pstParentClump,  fPlaySpeed, TRUE );
		}
	}

	return ulIndex;
}

BOOL AgcmEventEffect::PlayHitSound(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AgpdCharacter *pstAgpdTarget)
{
	if (!pstAgpdTarget)
	{
		pstAgpdTarget	= m_pcsAgpmCharacter->GetCharacter(pstAgpdCharacter->m_alTargetCID[0]);
		if (!pstAgpdTarget)
			return FALSE;
	}

	if (!pstAgcdCharacter->m_pClump)
		return FALSE;

	if (!pstAgcdCharacter)
	{
		pstAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
		if (!pstAgcdCharacter)
			return FALSE;
	}

	AEE_CharAnimAttachedData* pcsAttachedData = (AEE_CharAnimAttachedData*)m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pstAgcdCharacter->m_pcsCurAnimData );
	if( !pcsAttachedData )		return TRUE;

	if( pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_ATTACK )	return TRUE;

	BOOL bMy = pstAgpdCharacter->m_lID == m_lSelfCID ? TRUE : FALSE;
	for( AEE_CharAnimAttachedData::SoundListItr	iter = pcsAttachedData->GetList().begin(); iter != pcsAttachedData->GetList().end(); ++iter )
	{
		AEE_CharAnimAttachedSoundData* pcsCurrent = &(*iter);
		if( !pcsCurrent->m_strSoundName.empty() && pcsCurrent->m_unConditions == E_AEE_CAASD_CONDITION_ATTACK_SUCCESS )
			if( !PlaySound( AGCMEVENTEFFECT_EFFECT_SOUND_PATH, AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH, pcsCurrent->m_strSoundName.c_str(), bMy, pstAgcdCharacter->m_pClump ) )
				return FALSE;
	}

	return TRUE;
}

BOOL AgcmEventEffect::PlayAttackSound(AgpdCharacter *pcsAgpdCharacter, INT32 lWeaponType, BOOL bSelfCharacter, RpClump *pstParentClump)
{
	if( !m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter) )		return TRUE; // skip!

	AgcdCharacter* pAgcdCharacter = m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	AgcdEventEffectSound* pcsSound = m_csAgcmEventEffectData.GetAttackSound( pcsAgpdCharacter->m_bIsTrasform ? AGPMITEM_EQUIP_WEAPON_TYPE_NONE : lWeaponType );
	return pcsSound ? PlaySound( AGCMEVENTEFFECT_EFFECT_SOUND_PATH, AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH, pcsSound->m_szSoundName, bSelfCharacter, pstParentClump, FALSE, NULL, pAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_ATTACK] ) : TRUE;
}

BOOL AgcmEventEffect::PlayAnimLoopSound(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, CHAR *szSoundName)
{
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 090204 BOB
******************************************************************************/
BOOL AgcmEventEffect::PlayCharacterMovingSound(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter)
{
	if (pcsAgpdCharacter->m_lID != m_lSelfCID)
		return TRUE; // 태형씨요구로 주인공이 아닌 모든 캐릭터 skip(251104, BOB)
	if (!m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter))
		return TRUE; // skip!
	if (m_pcsAgpmCharacter->IsGM(pcsAgpdCharacter))
		return TRUE; // skip!

	FLOAT						fX = 0.0f, fZ = 0.0f;
	fX	= pcsAgpdCharacter->m_stPos.x;
	fZ	= pcsAgpdCharacter->m_stPos.z;

	UINT8	unMaterialType;
	static BOOL _sbUseClientType = TRUE;
	if( _sbUseClientType )	unMaterialType	= AGCMMAP_THIS->GetType(fX,pcsAgpdCharacter->m_stPos.y, fZ);
	else					unMaterialType	= m_pcsApmMap->GetType(fX, fZ);

	AgcdAnimationCallbackData	csData;
	csData.m_fTime			= 0.0f + 0.25f;
	csData.m_fPlaySpeed		= 1.f;
	csData.m_pfCallback		= AnimSoundCB;

	// 마고자 (2005-08-16 오후 7:00:09) : 
	// 탈것 체크해서 사운드를 뿌려줌.
	AgcmEventEffectData::stMovingSoundSet * pSoundSet;
	pSoundSet = m_csAgcmEventEffectData.GetMovingSoundSet( m_pcsAgpmCharacter->GetVehicleTID( pcsAgpdCharacter ) );
	// 안타고 있으면..

	csData.m_pavData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_PARENT_MODULE]	= (PVOID)(this);
	csData.m_pavData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_SOUND]		= m_csAgcmEventEffectData.GetMovingSound( pSoundSet , unMaterialType, 0);	

	if (m_lSelfCID != pcsAgpdCharacter->m_lID)
	{
		csData.m_pavData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_CLUMP]	= (PVOID)(pcsAgcdCharacter->m_pClump);
	}
	else
	{
		csData.m_pavData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_CLUMP]	= NULL;
	}

	pcsAgcdCharacter->m_csAnimation.AddAnimCB(&csData);

	csData.m_fTime			= (FLOAT)((pcsAgcdCharacter->m_csAnimation.GetDuration() / 2.0f) + 0.25f);
	csData.m_fPlaySpeed		= pcsAgcdCharacter->m_afAnimSpeedRate[AGCMCHAR_ANIM_TYPE_RUN];
	csData.m_pavData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_SOUND]		= m_csAgcmEventEffectData.GetMovingSound( pSoundSet , unMaterialType, 1);
	pcsAgcdCharacter->m_csAnimation.AddAnimCB(&csData);

	return TRUE;
}

PVOID AgcmEventEffect::AnimSoundCB(PVOID pvData)
{
	if (!pvData)
		return FALSE;

	PVOID					*ppvData		= (PVOID *)(pvData);

	FLOAT					fPlaySpeed		= ((AgcdAnimationCallbackData*)(pvData))->m_fPlaySpeed;
	AgcmEventEffect			*pcsThis		= (AgcmEventEffect *)(ppvData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_PARENT_MODULE]);
	AgcdEventEffectSound	*pcsSound		= (AgcdEventEffectSound *)(ppvData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_SOUND]);
	RpClump					*pstClump		= (RpClump *)(ppvData[AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_CLUMP]);

	if( pcsSound )
	{
		if( !pcsThis->PlaySound( AGCMEVENTEFFECT_EFFECT_SOUND_PATH, AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH, pcsSound->m_szSoundName,
								pstClump ? FALSE : TRUE, pstClump, FALSE, NULL, fPlaySpeed ) ) return FALSE;
	}

	return (PVOID)(FALSE);
}

BOOL AgcmEventEffect::AddCustData(CHAR *pszDestCustData, CHAR *pszAddCustData)
{
	if (!pszDestCustData[0])
		sprintf(pszDestCustData, "0%c%c", D_AGCD_EFFECT_CUST_DATA_START, D_AGCD_EFFECT_CUST_DATA_END);

	CHAR	szTemp[256], szTemp2[256];
	INT32	lProgress		= 0;
	INT32	lProgress2		= 0;

	INT32	lCustDataNum	= atoi(pszDestCustData);
	INT32	lCustDataLen	= strlen(pszDestCustData);
	INT32	lAddDataLen		= strlen(pszAddCustData);
	INT32	lOffset			= (lCustDataNum > 9) ? (2) : (1);

	memset(szTemp, 0, 256);
	memset(szTemp2, 0, 256);

	strcpy(szTemp, pszDestCustData + lOffset);

	for (lProgress = 0; lProgress < lCustDataLen; ++lProgress)
	{
		if( szTemp[lProgress] == pszAddCustData[0] )
		{
			for (lProgress2 = lProgress; lProgress2 < lCustDataLen; ++lProgress2)
			{
				if ((szTemp[lProgress2] == D_AGCD_EFFECT_CUST_DATA_SEP) || (szTemp[lProgress2] == D_AGCD_EFFECT_CUST_DATA_END))
				{
					strcpy(szTemp2, pszDestCustData);
					szTemp2[lOffset + lProgress] = '\0';
					sprintf(pszDestCustData, "%s%s%s", szTemp2, pszAddCustData, szTemp + lProgress2);
					return TRUE;
				}
			}

			return FALSE;
		}

		if (szTemp[lProgress] == D_AGCD_EFFECT_CUST_DATA_END)
		{
			if (lCustDataNum > 0)
			{
				szTemp[lProgress] = D_AGCD_EFFECT_CUST_DATA_SEP;
				++lProgress;
			}

			strcpy(szTemp + lProgress, pszAddCustData);
			szTemp[lProgress + (lAddDataLen + 0)] = D_AGCD_EFFECT_CUST_DATA_END;
			szTemp[lProgress + (lAddDataLen + 1)] = '\0';
			break;
		}
	}

	sprintf(pszDestCustData, "%d%s", ++lCustDataNum, szTemp);

	return TRUE;
}

INT32 AgcmEventEffect::GetCustDataIndex(CHAR szCustIndex)
{
	switch( szCustIndex )
	{
	case D_AGCD_EFFECT_CUST_DATA_INDEX_ANIM_POINT:			return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ANIM_POINT;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH:		return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DESTROY_MATCH;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_TARGET_STRUCK_ANIM:	return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_STRUCK_ANIM;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE:		return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_CHECK_TIME_ON:		return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_ON;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_CHECK_TIME_OFF:		return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_OFF;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_TARGET_OPTION:		return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_TARGET_OPTION;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_CALC_DIST_OPTION:	return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CALC_DIST_OPTION;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_LINK_SKILL:			return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_LINK_SKILL;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION:	return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_DIRECTION_OPTION;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE_CUST_DATA:	return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_EXCEPTION_TYPE_CUST_DATA;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_CHECK_USER:			return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_USER;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT:		return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_NATURE_CONDITION:	return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_NATURE_CONDITION;
	case D_AGCD_EFFECT_CUST_DATA_INDEX_CASTING_EFFECT:		return E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CASTING_EFFECT;
	default:												return -1;
	}
}

/******************************************************************************
* Purpose :
*
* 030304 BOB
******************************************************************************/
INT32 AgcmEventEffect::GetCustDataIndex(CHAR *szCustData, INT32 *palIndex)
{
	if (!szCustData)
	{
		memset(palIndex, -1, sizeof(INT32) * D_AGCD_EFFECT_MAX_CUST_DATA);
		return -1;
	}

	// 인덱스를 초기화한다.
	INT32 lIndexCount = 0;
	memset(palIndex, -1, sizeof(INT32) * D_AGCD_EFFECT_MAX_CUST_DATA);

	// 버퍼의 길이를 구한다.
	INT32 lStrLen		= strlen(szCustData);
	if (lStrLen < 1)
		return lIndexCount;

	// 데이터가 몇개나 있는지 알아낸다.
	INT32 lCustDataNum	= atoi(szCustData);
	if ((lCustDataNum < 1) || (lCustDataNum > 99))
		return lIndexCount;

	// 데이터 갯수가 한 자리의 숫자인지, 두 자리의 숫자인지 알아낸다.
	INT32 lProgress		= (lCustDataNum > 10) ? (2) : (1);

	// 데이터 구조를 확인한다.
	if (szCustData[lProgress] != D_AGCD_EFFECT_CUST_DATA_START)
		return lIndexCount;

	
	INT32 lCustArrayIndex	= 0;
	// 루프를 돌면서 해당 인덱스를 찾아낸다.
	for (++lProgress; lProgress < lStrLen; ++lProgress)
	{
		// 해당 인덱스를 찾았다!
		if ((szCustData[lProgress] >= 'A') && (szCustData[lProgress] <= 'Z'))
		{
			lCustArrayIndex = GetCustDataIndex(szCustData[lProgress]);
			if (lCustArrayIndex > 0)
			{
				palIndex[lCustArrayIndex] = atoi(szCustData + (lProgress + 1));
				++lIndexCount;
			}

			continue;
		}

		// 전체 버퍼길이보다 작을 때만 루프를 순회한다.
		while (++lProgress < lStrLen)
		{
			// 현재 버퍼가 구분자라면 다음 루프로 이동한다.
			if (szCustData[lProgress] == D_AGCD_EFFECT_CUST_DATA_SEP)
				break;

			// 현재 버퍼가 데이터의 마지막이라면 function을 끝낸다.
			if (szCustData[lProgress] == D_AGCD_EFFECT_CUST_DATA_END)
				return lIndexCount;
		}
	}

	// error!
	return lIndexCount;
}

INT32 AgcmEventEffect::GetCustDataIndex(CHAR *szCustData, CHAR szCustIndex)
{
	// 버퍼의 길이를 구한다.
	INT32 lStrLen		= strlen(szCustData);
	if (lStrLen < 1)
		return -1;

	// 데이터가 몇개나 있는지 알아낸다.
	INT32 lCustDataNum	= atoi(szCustData);
	if ((lCustDataNum < 1) || (lCustDataNum > 99))
		return -1;

	// 데이터 갯수가 한 자리의 숫자인지, 두 자리의 숫자인지 알아낸다.
	INT32 lProgress		= (lCustDataNum > 10) ? (2) : (1);

	// 데이터 구조를 확인한다.
	if (szCustData[lProgress] != D_AGCD_EFFECT_CUST_DATA_START)
		return -1;

	// 루프를 돌면서 해당 인덱스를 찾아낸다.
	for (++lProgress; lProgress < lStrLen; ++lProgress)
	{
		// 해당 인덱스를 찾았다!
		if (szCustData[lProgress] == szCustIndex)
		{
			return atoi(szCustData + (lProgress + 1));
		}

		// 전체 버퍼길이보다 작을 때만 루프를 순회한다.
		while (++lProgress < lStrLen)
		{
			// 현재 버퍼가 구분자라면 다음 루프로 이동한다.
			if (szCustData[lProgress] == D_AGCD_EFFECT_CUST_DATA_SEP)
				break;

			// 현재 버퍼가 데이터의 마지막이라면 function을 끝낸다.
			if (szCustData[lProgress] == D_AGCD_EFFECT_CUST_DATA_END)
				return -1;
		}
	}

	// error!
	return -1;
}

BOOL AgcmEventEffect::EventSkillChargeThirdAttackCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmEventEffect::EventSkillConsumeThirdAttackCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmEventEffect::EventSkillEvationCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmEventEffect::EventSkillDamageReflectionCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmEventEffect::EventSkillChangeVelocityCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmEventEffect::EventSkillAbsorbDamageCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgcmEventEffect::EventSkillAddBuffedListCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData))
		return FALSE;

	AgcmEventEffect				*pcsThis			= (AgcmEventEffect *)(pClass);
	AgpdCharacter				*pcsAgpdTarget		= (AgpdCharacter *)(pData);
	PVOID						*pplBuffer			= (PVOID *)(pCustData);

	INT32						lBuffedTID			= (INT32)(pplBuffer[0]);
	INT32						lCasterTID			= (INT32)(pplBuffer[1]);

	// 2005.12.20. steeple
	AgpdCharacter* pcsSelfCharacter = pcsThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = pcsThis->m_pcsAgpmSkill->GetSkillTemplate(lBuffedTID);
	if(!pcsSkillTemplate)
		return FALSE;

	// Self 에게만 Effect 가 보여야 하는 스킬인데, 타겟이왔다. 2005.12.20. steeple
	if(pcsThis->m_pcsAgpmSkill->IsVisibleEffectTypeSelfOnly(pcsSkillTemplate) &&
		pcsSelfCharacter->m_lID != pcsAgpdTarget->m_lID)
		return TRUE;

	ApBase						*pcsCasterTemplate	= (ApBase *)(pcsThis->m_pcsAgpmCharacter->GetCharacterTemplate(lCasterTID));
	if (!pcsCasterTemplate)
		return FALSE;

	AgcdSkillAttachTemplateData	*pstSkillData		= pcsThis->m_pcsAgcmSkill->GetAttachTemplateData(pcsCasterTemplate);
	if (!pstSkillData)
		return FALSE;

	INT32						lUsableIndex		= pcsThis->m_pcsAgcmSkill->GetUsableSkillTNameIndex(pcsCasterTemplate, lBuffedTID);
	if (lUsableIndex < 0)
		return FALSE;

	if (!pstSkillData->m_pacsSkillVisualInfo[lUsableIndex])
		return FALSE;

	AgcdUseEffectSet			*pcsAgcdUseEffectSet	=
		pstSkillData->m_pacsSkillVisualInfo[lUsableIndex]->m_pastEffect[AGCMCHAR_AT2_COMMON_DEFAULT];

	if (!pcsAgcdUseEffectSet)
		return FALSE;

	BOOL	bShowForce = FALSE;
	if( pstSkillData->m_pacsSkillVisualInfo[lUsableIndex]->m_eShowOption == AgcmSkillVisualInfo::ALWAYS_SHOW )
	{
		bShowForce = TRUE;
	}

	AgcmStartEventEffectParams	csParams;
	if (pcsThis->SetPassiveSkill(pcsAgpdTarget, pcsAgcdUseEffectSet, &csParams, E_AGCD_EVENT_EFFECT_CDI_EXT_TYPE_CONTINUATIVE_SKILL_EFFECT, 0 , bShowForce ))
	{
		AgcdEventEffectCharacter	*pcsAttachedData	= (AgcdEventEffectCharacter *)(pcsThis->GetEventEffectData((ApBase *)(pcsAgpdTarget)));
		if (!pcsAttachedData)
			return FALSE;

		AES_EFFCTRL_SET_LIST		*pcsCurrent			= csParams.m_pcsNodeInfo;
		while (pcsCurrent)
		{
			pcsAttachedData->m_csContinuativeSkillList.AddNode(lBuffedTID, pcsCurrent->m_pstNode);
			pcsCurrent									= pcsCurrent->m_pcsNext;
		}
	}	
	//@{ 2006/11/03 burumal
	else
	{
		AgcdCharacter* pcsAgcdCharacter = pcsThis->m_pcsAgcmCharacter->GetCharacterData(pcsAgpdTarget);
	
		if ( pcsAgcdCharacter && (!pcsAgcdCharacter->m_pClump) )
		{			
			BOOL bPreengagedEffect = TRUE;

			INT32 nIdx;
			for ( nIdx = 0; nIdx < pcsAgcdCharacter->m_nPreengagedEventEffectCount; nIdx++ )
			{
				ASSERT(pcsAgcdCharacter->m_pPreengagedEventEffect[nIdx]);
				if ( pcsAgcdCharacter->m_pPreengagedEventEffect[nIdx]->lBuffedTID == lBuffedTID &&
					pcsAgcdCharacter->m_pPreengagedEventEffect[nIdx]->lCasterTID == lCasterTID &&
					pcsAgcdCharacter->m_pPreengagedEventEffect[nIdx]->pcsAgpdTarget == pcsAgpdTarget )
				{
					bPreengagedEffect = FALSE;
					break;
				}
			}

			if ( bPreengagedEffect && pcsAgcdCharacter->m_nPreengagedEventEffectCount < AGCD_CHARACTER_MAX_PREENGAGED_EVENT_EFFECT )
			{
				AgcdPreengagedEventEffect* pPreengagedEffect = new AgcdPreengagedEventEffect;
				ASSERT(pPreengagedEffect);
				if ( pPreengagedEffect )
				{
					pPreengagedEffect->lBuffedTID			= lBuffedTID;
					pPreengagedEffect->lCasterTID			= lCasterTID;
					pPreengagedEffect->lCommonEffectID		= -1;
					pPreengagedEffect->pcsAgpdTarget		= pcsAgpdTarget;
				
					pcsAgcdCharacter->m_pPreengagedEventEffect[pcsAgcdCharacter->m_nPreengagedEventEffectCount++] = pPreengagedEffect;
				}
			}
		}
	}
	//@}

	return TRUE;
}

BOOL AgcmEventEffect::EventSkillAddPreengagedPassiveSkillEffect(INT32 lBuffedTID, INT32 lCasterTID, AgpdCharacter* pcsAgpdTarget)
{	
	if ( !m_pcsAgcmCharacter )
		return FALSE;

	if ( lBuffedTID == -1 || lCasterTID == -1 || pcsAgpdTarget == NULL )
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(lBuffedTID);
	if(!pcsSkillTemplate)
		return FALSE;

	// Self 에게만 Effect 가 보여야 하는 스킬인데, 타겟이왔다. 2005.12.20. steeple
	if(m_pcsAgpmSkill->IsVisibleEffectTypeSelfOnly(pcsSkillTemplate) && pcsSelfCharacter->m_lID != pcsAgpdTarget->m_lID)
		return TRUE;

	ApBase* pcsCasterTemplate = (ApBase*) (m_pcsAgpmCharacter->GetCharacterTemplate(lCasterTID));
	if (!pcsCasterTemplate)
		return FALSE;

	AgcdSkillAttachTemplateData	*pstSkillData = m_pcsAgcmSkill->GetAttachTemplateData(pcsCasterTemplate);
	if (!pstSkillData)
		return FALSE;

	INT32						lUsableIndex = m_pcsAgcmSkill->GetUsableSkillTNameIndex(pcsCasterTemplate, lBuffedTID);
	if (lUsableIndex < 0)
		return FALSE;

	if (!pstSkillData->m_pacsSkillVisualInfo[lUsableIndex])
		return FALSE;

	AgcdUseEffectSet			*pcsAgcdUseEffectSet	=
		pstSkillData->m_pacsSkillVisualInfo[lUsableIndex]->m_pastEffect[AGCMCHAR_AT2_COMMON_DEFAULT];

	if (!pcsAgcdUseEffectSet)
		return FALSE;

	AgcmStartEventEffectParams	csParams;
	if ( SetPassiveSkill(pcsAgpdTarget, pcsAgcdUseEffectSet, &csParams, E_AGCD_EVENT_EFFECT_CDI_EXT_TYPE_CONTINUATIVE_SKILL_EFFECT, 0) )
	{
		AgcdEventEffectCharacter	*pcsAttachedData	= (AgcdEventEffectCharacter *)(GetEventEffectData((ApBase *)(pcsAgpdTarget)));
		if (!pcsAttachedData)
			return FALSE;

		AES_EFFCTRL_SET_LIST		*pcsCurrent			= csParams.m_pcsNodeInfo;
		while (pcsCurrent)
		{
			pcsAttachedData->m_csContinuativeSkillList.AddNode(lBuffedTID, pcsCurrent->m_pstNode);
			pcsCurrent									= pcsCurrent->m_pcsNext;
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::EventSkillRemoveBuffedListCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData) || (!pCustData))
		return FALSE;

	AgcmEventEffect				*pcsThis			= (AgcmEventEffect *)(pClass);
	ApBase						*pcsAgpdTarget		= (ApBase *)(pData);
	INT32						lBuffedTID			= (INT32)(pCustData);

	ASSERT(pcsAgpdTarget->m_eType == APBASE_TYPE_CHARACTER);
	if (pcsAgpdTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	AgcdEventEffectCharacter	*pcsAttachedData	= (AgcdEventEffectCharacter *)(pcsThis->GetEventEffectData(pcsAgpdTarget));
	if (!pcsAttachedData) return FALSE;

	return pcsThis->RemoveContinuativetSkillEventEffect(pcsAttachedData, lBuffedTID);
}

PVOID AgcmEventEffect::CBRemoveAnimation(PVOID pvData1, PVOID pvData2)
{
	AEE_CharAnimAttachedData	*pcsAttachedData	= (AEE_CharAnimAttachedData *)(pvData1);
	AgcmEventEffect				*pcsThis			= (AgcmEventEffect *)(pvData2);

	pcsAttachedData->Release();
	return NULL;
}

BOOL AgcmEventEffect::StreamWriteCharacterAnimationAttachedData(AgpdCharacterTemplate *pstAgpdCharacterTemplate, ApModuleStream *pStream)
{
	AgcdCharacterTemplate* pstAgcdCharacterTemplate	= m_pcsAgcmCharacter->GetTemplateData(pstAgpdCharacterTemplate);
	if (!pstAgcdCharacterTemplate) return FALSE;

	INT32 lAnimDataIndex = 0, lSoundDataIndex = 0;
	INT32 lNumAnimType2 = m_pcsAgcmCharacter->GetAnimType2Num(pstAgcdCharacterTemplate);

	for( INT32 lAnimType = AGCMCHAR_ANIM_TYPE_WAIT; lAnimType < AGCMCHAR_MAX_ANIM_TYPE; ++lAnimType )
	{
		for( INT32 lAnimType2 = 0; lAnimType2 < lNumAnimType2; ++lAnimType2 )
		{
			if( !pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2] ||
				!pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation )
				continue;

			lAnimDataIndex		= 0;
			AgcdAnimData2* pcsCurrentAnimData = pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation->m_pcsHead;			
			while( pcsCurrentAnimData )
			{
				AEE_CharAnimAttachedData* pcsAgcdCharAnimAttachedData = (AEE_CharAnimAttachedData*)m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pcsCurrentAnimData );
				if( !pcsAgcdCharAnimAttachedData ) return FALSE;

				for( AEE_CharAnimAttachedData::SoundListItr iter = pcsAgcdCharAnimAttachedData->GetList().begin(); iter != pcsAgcdCharAnimAttachedData->GetList().end(); ++iter )
				{
					AEE_CharAnimAttachedSoundData* pcsCurrent = &*iter;
					if( !pcsCurrent->m_strSoundName.empty() )
					{
						CHAR	szName[256], szValue[256];
						sprintf( szName, "%s%d%d%d%d", AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_INI_NAME, lAnimType, lAnimType2, lAnimDataIndex, lSoundDataIndex );
						sprintf( szValue, "%d:%d:%d:%d:%s", lAnimType, lAnimType2, lAnimDataIndex, lSoundDataIndex, pcsCurrent->m_strSoundName.c_str() );
						pStream->WriteValue(szName, szValue);

						sprintf( szName, "%s%d%d%d%d", AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_CONDITION_INI_NAME, lAnimType, lAnimType2, lAnimDataIndex, lSoundDataIndex );
						sprintf( szValue, "%d:%d:%d:%d:%d", lAnimType, lAnimType2, lAnimDataIndex, lSoundDataIndex, pcsCurrent->m_unConditions );
						pStream->WriteValue(szName, szValue);
					}
				}

				++lAnimDataIndex;
				pcsCurrentAnimData	= pcsCurrentAnimData->m_pcsNext;
			}
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::StreamReadCharacterAnimationAttachedData(AgpdCharacterTemplate *pstAgpdCharacterTemplate, ApModuleStream *pStream)
{
	AgcdCharacterTemplate* pstAgcdCharacterTemplate = m_pcsAgcmCharacter->GetTemplateData( pstAgpdCharacterTemplate );
	if( !pstAgcdCharacterTemplate ) return FALSE;

	CHAR	szValue[256], szTemp[256];
	INT32	lAnimType = 0, lAnimType2 = 0, lAnimDataIndex = 0, lSoundDataIndex = 0;
	memset(szValue, 0, 256);
	memset(szTemp, 0, 256);

	AgcdAnimData2* pcsCurrentAnimData = NULL;
	AEE_CharAnimAttachedData* pcsAgcdCharAnimAttachedData = NULL;
	if( !strncmp( pStream->GetValueName(), AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_INI_NAME, strlen(AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_INI_NAME) ) )
	{
		pStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d:%d:%d:%s", &lAnimType, &lAnimType2, &lAnimDataIndex, &lSoundDataIndex, szTemp );
		if( !szTemp[0] )		return TRUE;

		lAnimType2	= m_pcsAgcmCharacter->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lAnimType2);
		if( lAnimType2 < 0 )	return TRUE; // 일단 SKIP
		if( !pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2] )	return FALSE;
			
		pcsCurrentAnimData = m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAnimData( pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation, lAnimDataIndex );
		if( !pcsCurrentAnimData )		return TRUE;

		pcsAgcdCharAnimAttachedData	= (AEE_CharAnimAttachedData*)m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pcsCurrentAnimData );
		if( !pcsAgcdCharAnimAttachedData ) return FALSE;

		pcsAgcdCharAnimAttachedData->GetList().push_back( AEE_CharAnimAttachedSoundData( szTemp ) );
	}
	else if( !strncmp( pStream->GetValueName(), AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_CONDITION_INI_NAME, strlen( AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_INI_NAME ) ) )
	{
		UINT32	ulSoundConditions = 0;
		pStream->GetValue(szValue, 256);
		sscanf( szValue, "%d:%d:%d:%d:%d", &lAnimType, &lAnimType2, &lAnimDataIndex, &lSoundDataIndex, &ulSoundConditions );

		lAnimType2	= m_pcsAgcmCharacter->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lAnimType2);
		if( lAnimType2 < 0 )		return TRUE; // 일단 SKIP
		if( !pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2] )		return FALSE;
			
		pcsCurrentAnimData	= m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAnimData( pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimType2]->m_pcsAnimation, lAnimDataIndex );
		if( !pcsCurrentAnimData )				return TRUE;

		pcsAgcdCharAnimAttachedData	= (AEE_CharAnimAttachedData*)m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pcsCurrentAnimData );
		if( !pcsAgcdCharAnimAttachedData )		return FALSE;
			
		AEE_CharAnimAttachedSoundData* pSoundData = pcsAgcdCharAnimAttachedData->GetTail();
		if( !pSoundData )						return FALSE;
			
		pSoundData->m_unConditions	= ulSoundConditions;
	}

	return TRUE;
}

BOOL AgcmEventEffect::CBStreamReadCharacterAnimationAttachedData(PVOID pvClass, PVOID pvData1, PVOID pvData2)
{
	return ((AgcmEventEffect *)(pvClass))->StreamReadCharacterAnimationAttachedData( (AgpdCharacterTemplate *)(pvData2), (ApModuleStream *)(pvData1) );
}

AgcdEventEffectCheckTimeDataList *AgcmEventEffect::GetCheckTimeDataList(AgcdEventEffectCheckTimeDataList *pcsHead, ApBase *pcsKey)
{
	if (pcsHead)
	{
		AgcdEventEffectCheckTimeDataList	*pcsList	= pcsHead;
		while (pcsList)
		{
			if (pcsList->m_pcsKey->m_eType == pcsKey->m_eType)
			{
				if (pcsList->m_pcsKey->m_lID == pcsKey->m_lID)
					return pcsList;
			}

			pcsList	= pcsList->m_pcsNext;
		}
	}

	return NULL;
}

BOOL AgcmEventEffect::RemoveAllCheckTimeData(AgcdEventEffectCheckTimeDataList *pcsParant)
{
	AgcdEventEffectCheckTimeData	*pcsNext	= NULL;
	AgcdEventEffectCheckTimeData	*pcsCur		= pcsParant->m_pcsData;
	while (pcsCur)
	{
		pcsNext	= pcsCur->m_pcsNext;
		delete pcsCur;
		pcsCur	= pcsNext;
	}

	pcsParant->m_pcsData	= NULL;

	return TRUE;
}

BOOL AgcmEventEffect::ReleaseCheckTimeDataList(ApBase *pcsBase)
{
	return TRUE;
}

BOOL AgcmEventEffect::AddEffectSet(AgcdEventEffectCheckTimeDataList *pcsList)
{
	ApBase								*pcsBase	= NULL;
	AgcdEventEffectCheckTimeData		*pcsData	= NULL;
	while (pcsList)
	{
		pcsBase		= pcsList->m_pcsKey;
		pcsData		= pcsList->m_pcsData;
		while (pcsData)
		{
			if (!pcsData->m_pcsEffectSetNode)
			{
				if (!SetBaseEffect(pcsBase, &pcsData->m_csData, -1, FALSE, &pcsData->m_pcsEffectSetNode)) return FALSE;
				if (!pcsData->m_pcsEffectSetNode) return FALSE;
			}

			pcsData	= pcsData->m_pcsNext;
		}

		pcsList		= pcsList->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmEventEffect::RemoveEffectSet(AgcdEventEffectCheckTimeDataList *pcsList)
{
	ApAutoWriterLock	csLock( m_pcsAgcmEff2->m_RWLock );

	if (!m_pcsAgcmEff2)
		return FALSE;

	AgcdEventEffectCheckTimeData		*pcsCurData	= NULL;
	AgcdEventEffectCheckTimeDataList	*pcsCurList	= pcsList;
	while (pcsCurList)
	{
		pcsCurData		= pcsCurList->m_pcsData;
		while (pcsCurData)
		{
			if (pcsCurData->m_pcsEffectSetNode)
			{
				m_pcsAgcmEff2->RemoveEffSet(pcsCurData->m_pcsEffectSetNode, FALSE);
				pcsCurData->m_pcsEffectSetNode	= NULL;
			}

			pcsCurData	= pcsCurData->m_pcsNext;
		}

		pcsCurList		= pcsCurList->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmEventEffect::RemoveCheckTimeDataList(ApBase *pcsKey)
{
	AgcdEventEffectCheckTimeDataList	*pcsHead	= NULL;

	for (INT32 lOnOffIndex = 0; lOnOffIndex < E_AGCD_EVENT_EFFECT_MAX_CHECK_TIME_ON_OFF; ++lOnOffIndex)
	{
		for (INT32 lTimeIndex = 0; lTimeIndex < AGCMEVENTEFFECT_NUM_CHECK_TIME; ++lTimeIndex)
		{
			pcsHead		= m_pacsCheckTimeDataList[lOnOffIndex][lTimeIndex];
			if (!pcsHead)
				continue;

			AgcdEventEffectCheckTimeDataList	*pcsPre	= NULL;
			AgcdEventEffectCheckTimeDataList	*pcsCur	= pcsHead;		
			while (pcsCur)
			{
				if (pcsCur->m_pcsKey->m_eType == pcsKey->m_eType)
				{
					if (pcsCur->m_pcsKey->m_lID == pcsKey->m_lID)
					{
						if (pcsPre)
							pcsPre->m_pcsNext									= pcsCur->m_pcsNext;
						else if (pcsCur == pcsHead)
							m_pacsCheckTimeDataList[lOnOffIndex][lTimeIndex]	= pcsCur->m_pcsNext;

						if (!RemoveAllCheckTimeData(pcsCur)) return FALSE;

						delete pcsCur;

						break;
					}
				}

				pcsCur	= pcsCur->m_pcsNext;
			}
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::AddCheckTimeData(AgcdEventEffectCheckTimeDataList *pcsList, AgcdUseEffectSetData *pcsData)
{
	return TRUE;
}

BOOL AgcmEventEffect::SetCheckTimeDataList(AgcdUseEffectSet *pcsEffectSet, ApBase *pcsBase)
{
	AgcdUseEffectSetList	*pcsCurrent	= pcsEffectSet->m_pcsHead;
	while (pcsCurrent)
	{
		if (!(pcsCurrent->m_csData.m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_CHECK_TIME))
		{
			pcsCurrent	= pcsCurrent->m_pcsNext;
			continue;
		}

		if (!pcsCurrent->m_csData.m_pszCustData) return FALSE;

		INT32 alCustDataIndex[D_AGCD_EFFECT_MAX_CUST_DATA];
		if (GetCustDataIndex(pcsCurrent->m_csData.m_pszCustData, alCustDataIndex) <= 0) return FALSE;

		if (	(alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_ON] <= -1) &&
				(alCustDataIndex[E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_OFF] <= -1)		) return FALSE;

		INT32	lOnOffIndex = 0, lCustDataIndex = 0;
		for (lOnOffIndex = 0; lOnOffIndex < E_AGCD_EVENT_EFFECT_MAX_CHECK_TIME_ON_OFF; ++lOnOffIndex)
		{
			if (lOnOffIndex == E_AGCD_EVENT_EFFECT_CHECK_TIME_ON)
				lCustDataIndex	= E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_ON;
			else if (lOnOffIndex == E_AGCD_EVENT_EFFECT_CHECK_TIME_OFF)
				lCustDataIndex	= E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_CHECK_TIME_OFF;
			else return FALSE;

			if (alCustDataIndex[lCustDataIndex] <= -1)
			{
				pcsCurrent	= pcsCurrent->m_pcsNext;
				continue;
			}

			AgcdEventEffectCheckTimeDataList	*pcsList	= GetCheckTimeDataList(m_pacsCheckTimeDataList[lOnOffIndex][alCustDataIndex[lCustDataIndex]], pcsBase);
			AgcdEventEffectCheckTimeDataList	*pcsNext	= m_pacsCheckTimeDataList[lOnOffIndex][alCustDataIndex[lCustDataIndex]];


			if (!m_pacsCheckTimeDataList[lOnOffIndex][alCustDataIndex[lCustDataIndex]])
			{
				m_pacsCheckTimeDataList[lOnOffIndex][alCustDataIndex[lCustDataIndex]]	= new AgcdEventEffectCheckTimeDataList;

				pcsList	= m_pacsCheckTimeDataList[lOnOffIndex][alCustDataIndex[lCustDataIndex]];
				pcsNext = NULL;
			}

			if (!pcsList)
			{
				pcsList	= new AgcdEventEffectCheckTimeDataList;
			}

			pcsList->m_pcsKey	= pcsBase;
			pcsList->m_pcsNext	= pcsNext;

			if (!AddCheckTimeData(pcsList, &pcsCurrent->m_csData)) return FALSE;
			m_pacsCheckTimeDataList[lOnOffIndex][alCustDataIndex[lCustDataIndex]]	= pcsList;
		}

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

VOID AgcmEventEffect::SetStatus(UINT32 ulStatus)
{
	m_ulStatus	|= ulStatus;
}

BOOL AgcmEventEffect::SetCommonCharEffect(ApBase *pcsBase, eAgcmEventEffectDataCommonCharType eType, INT32 lTargetCID)
{
	if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgpdCharacter* pdCharacter = (AgpdCharacter*)pcsBase;
		AgcdCharacter* cdCharacter = (pdCharacter) ? m_pcsAgcmCharacter->GetCharacterData(pdCharacter) : NULL;
		if( cdCharacter )
		{	
			if( cdCharacter->m_pClump )
			{
				switch(eType)
				{
				case E_COMMON_CHAR_TYPE_STUN:	m_pcsAgcmRender->CustomizeLighting(cdCharacter->m_pClump, LIGHT_STATUS_STUN);	break;
				case E_COMMON_CHAR_TYPE_SLOW:	m_pcsAgcmRender->CustomizeLighting(cdCharacter->m_pClump, LIGHT_QUEST_FREEZE);	break;
				case E_COMMON_CHAR_TYPE_POISON:	m_pcsAgcmRender->CustomizeLighting(cdCharacter->m_pClump, LIGHT_QUEST_POISON);	break;
				}
			}
			else
			{
				// clump가 load가 되지 않아 effect 설정에 실패한 경우 예약처리를 한다
				BOOL bPreengagedEffect = TRUE;
				for( INT32 nIdx = 0; nIdx < cdCharacter->m_nPreengagedEventEffectCount; nIdx++ )
				{
					ASSERT(cdCharacter->m_pPreengagedEventEffect[nIdx]);
					if( cdCharacter->m_pPreengagedEventEffect[nIdx]->lCommonEffectID == eType &&
						cdCharacter->m_pPreengagedEventEffect[nIdx]->pcsAgpdTarget == pdCharacter )
					{
						bPreengagedEffect = FALSE;
						break;
					}
				}

				if ( bPreengagedEffect && cdCharacter->m_nPreengagedEventEffectCount < AGCD_CHARACTER_MAX_PREENGAGED_EVENT_EFFECT )
				{
					AgcdPreengagedEventEffect* pPreengagedEffect = new AgcdPreengagedEventEffect;
					ASSERT(pPreengagedEffect);
					if( pPreengagedEffect )
					{
						pPreengagedEffect->lBuffedTID			= -1;
						pPreengagedEffect->lCasterTID			= -1;
						pPreengagedEffect->lCommonEffectID		= eType;
						pPreengagedEffect->pcsAgpdTarget		= pdCharacter;

						cdCharacter->m_pPreengagedEventEffect[cdCharacter->m_nPreengagedEventEffectCount++] = pPreengagedEffect;
					}
				}		
			}
		}
	}

	for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
	{
		AgcdUseEffectSetData* pcsEffectSetData = m_csAgcmEventEffectData.GetCommonCharEffectData(eType, lCount);
		if( !pcsEffectSetData || !pcsEffectSetData->m_ulEID )		return TRUE;
			
		pcsEffectSetData->m_lTargetCID = lTargetCID;
		if( !SetBaseEffect( pcsBase, pcsEffectSetData ) )			return FALSE;
		
		if( pcsBase->m_eType == APBASE_TYPE_OBJECT )
		{
			AgcdObject* pstAgcdObject = m_pcsAgcmObject->GetObjectData((ApdObject*) pcsBase);
			AgcdObjectGroupList* pstOGroupList = pstAgcdObject->m_stGroup.m_pstList;
			while( pstOGroupList )
			{
				AgcdObjectGroupData* pstOGroupData = &pstOGroupList->m_csData;

				if( pstOGroupData->m_pstClump )
					m_pcsAgcmRender->CustomizeLighting( pstOGroupData->m_pstClump, LIGHT_QUEST_OBJECT );

				pstOGroupList = pstOGroupList->m_pstNext;
			}
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::StartSelectionAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AcCallbackData3 pfCustCallback, BOOL bGoBack)
{
	if (!pstAgcdCharacter)
	{
		pstAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
		if (!pstAgcdCharacter) return FALSE;
	}

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )GetModule( "AgcmUILogin" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgpmItem" );
	if( !pcmUILogin || !ppmFactor|| !pcmItem ) return FALSE;

	INT32 nSelectAnimation = pcmUILogin->GetSelectAnimationType( pstAgpdCharacter, pstAgcdCharacter, ppmFactor, pcmItem, bGoBack );
	if( nSelectAnimation < 0 ) return FALSE;

	AgcdAnimData2* pcsAnimData = m_csAgcmEventEffectData.GetSocialAnim( pstAgpdCharacter->ApdCharacter::m_pcsCharacterTemplate->ApBase::m_lID, static_cast<AgpdCharacterSocialType>(nSelectAnimation) );
	if (!pcsAnimData) return FALSE;

	pstAgcdCharacter->m_pcsNextAnimData		= pcsAnimData;
	pstAgcdCharacter->m_ulAnimationFlags	|= AGCMCHAR_AF_NO_UNEQUIP;

	return m_pcsAgcmCharacter->SetNextAnimation( pstAgpdCharacter, pstAgcdCharacter, AGCMCHAR_ANIM_TYPE_SOCIAL, FALSE, pfCustCallback );
}

template <AgpdCharacterSocialType eSocialType> 
static PVOID PlaySocialSoundCB(PVOID pvData1, PVOID pvData2, PVOID pvData3)
{
	AgcmEventEffect::m_pcsThisAgcmEventEffect->PlaySocialSound((AgpdCharacter*)pvData2, eSocialType);
	return NULL;
}

template <AgcmCharacterAnimType animType, AgpdCharacterSocialType eSocialType> 
static PVOID SetNextAnimationAfterBlendingCB(PVOID pvData1, PVOID pvData2, PVOID pvData3)
{
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter*)pvData2;
	AgcdCharacter *pstAgcdCharacter = AgcmEventEffect::m_pcsThisAgcmEventEffect->m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	if( !pstAgcdCharacter )
		return NULL;
	
	AgcmEventEffect::m_pcsThisAgcmEventEffect->PlaySocialSound(pstAgpdCharacter, eSocialType);

	BOOL bLoop = FALSE;
	AcCallbackData3 pfCustCallback = NULL;
	if( animType == AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT)
	{
		bLoop = TRUE;
		pfCustCallback = PlaySocialSoundCB<eSocialType>;
	}

	AgcmEventEffect::m_pcsThisAgcmEventEffect->m_pcsAgcmCharacter->SetNextAnimation( pstAgpdCharacter, pstAgcdCharacter, animType, bLoop, pfCustCallback );
	return pfCustCallback;
}

BOOL AgcmEventEffect::StartSocialAnimation(AgpdCharacter *pstAgpdCharacter, AgpdCharacterSocialType eSocialType, AgcdCharacter *pstAgcdCharacter, AcCallbackData3 pfCustCallback)
{
	ASSERT( !pfCustCallback );

	if( !pstAgcdCharacter )
	{
		pstAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
		if (!pstAgcdCharacter) return FALSE;
	}

	if( !m_pcsAgpmArchlord->IsArchlord( pstAgpdCharacter->m_szID ) && pstAgpdCharacter->m_bIsTrasform )		return TRUE;

	if( pstAgcdCharacter->m_lLastAttackTime > -1 )						return TRUE;
	if( pstAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE )	return TRUE;
		
	AgcdAnimData2* pcsAnimData =  m_csAgcmEventEffectData.GetSocialAnim( pstAgpdCharacter->ApdCharacter::m_pcsCharacterTemplate->ApBase::m_lID ,eSocialType );
	if( !pcsAnimData )		return FALSE;

	pstAgcdCharacter->m_pcsNextAnimData	= pcsAnimData;

	FLOAT fBlendingDuration = 1.0f;
	AgcmCharacterAnimType animType = AGCMCHAR_ANIM_TYPE_SOCIAL;
	switch(eSocialType)
	{
	// Once
	case AGPDCHAR_SOCIAL_TYPE_GREETING:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GREETING>;		break;
	case AGPDCHAR_SOCIAL_TYPE_CELEBRATION:		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_CELEBRATION>;	break;
	case AGPDCHAR_SOCIAL_TYPE_GRATITUDE:		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GRATITUDE>;	break;
	case AGPDCHAR_SOCIAL_TYPE_ENCOURAGEMENT:	pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_ENCOURAGEMENT>;break;
	case AGPDCHAR_SOCIAL_TYPE_DISREGARD:		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_DISREGARD>;	break;
	case AGPDCHAR_SOCIAL_TYPE_STRETCH:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_STRETCH>;		break;
	case AGPDCHAR_SOCIAL_TYPE_SPECIAL1:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_SPECIAL1>;		break;
	case AGPDCHAR_SOCIAL_TYPE_LAUGH:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_LAUGH>;		break;
	case AGPDCHAR_SOCIAL_TYPE_WEEPING:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_WEEPING>;		break;
	case AGPDCHAR_SOCIAL_TYPE_RAGE:				pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_RAGE>;			break;
	case AGPDCHAR_SOCIAL_TYPE_POUT:				pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_POUT>;			break;
	case AGPDCHAR_SOCIAL_TYPE_APOLOGY:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_APOLOGY>;		break;
	case AGPDCHAR_SOCIAL_TYPE_TOAST:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_TOAST>;		break;
	case AGPDCHAR_SOCIAL_TYPE_CHEER:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_CHEER>;		break;
	case AGPDCHAR_SOCIAL_TYPE_RUSH:				pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_RUSH>;			break;

	case AGPDCHAR_SOCIAL_TYPE_GM_GREETING:		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_GREETING>;	break;
	case AGPDCHAR_SOCIAL_TYPE_GM_CELEBRATION:	pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_CELEBRATION>;break;
	case AGPDCHAR_SOCIAL_TYPE_GM_WEEPING:		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_WEEPING>;	break;
	case AGPDCHAR_SOCIAL_TYPE_GM_TOAST:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_TOAST>;		break;
	case AGPDCHAR_SOCIAL_TYPE_GM_CHEER:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_CHEER>;		break;
	case AGPDCHAR_SOCIAL_TYPE_GM_DEEPBOW:		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_DEEPBOW>;	break;
	case AGPDCHAR_SOCIAL_TYPE_GM_HI:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_HI>;		break;
	case AGPDCHAR_SOCIAL_TYPE_GM_WAIT:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_WAIT>;		break;
	case AGPDCHAR_SOCIAL_TYPE_GM_HAPPY:			pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_HAPPY>;		break;

	// Repeat
	case AGPDCHAR_SOCIAL_TYPE_DANCING:			// 춤추기
		animType = AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT;
		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT, AGPDCHAR_SOCIAL_TYPE_DANCING>;
		break;
	case AGPDCHAR_SOCIAL_TYPE_DOZINESS:			// 졸기
		animType = AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT;
		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT, AGPDCHAR_SOCIAL_TYPE_DOZINESS>;
		break;
	case AGPDCHAR_SOCIAL_TYPE_SIT:				// 앉기
		fBlendingDuration = 2.0f;
		animType = AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT;
		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT, AGPDCHAR_SOCIAL_TYPE_SIT>;
		break;
	case AGPDCHAR_SOCIAL_TYPE_GM_DANCING:	// 춤
		animType = AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT;
		pfCustCallback = SetNextAnimationAfterBlendingCB<AGCMCHAR_ANIM_TYPE_SOCIAL, AGPDCHAR_SOCIAL_TYPE_GM_DANCING>;
		break;
	}

	return m_pcsAgcmCharacter->SetBlendingAnimation(pstAgpdCharacter, pstAgcdCharacter, animType, fBlendingDuration, pfCustCallback);
}

BOOL AgcmEventEffect::HideShieldAndWeaponsWhileCurrentAnimationDuration(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter)
{
	if (!pstAgcdCharacter)
	{
		pstAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
		if (!pstAgcdCharacter)
			return FALSE;
	}

	AgpdItem		*pstAgpdItem		= NULL;
	AgcdItem		*pstAgcdItem		= NULL;

	pstAgpdItem	= GetEquipItem(m_pcsAgpmItem, pstAgpdCharacter, AGPMITEM_PART_HAND_RIGHT);
	if (pstAgpdItem && m_pcsAgcmItem)
	{
		pstAgcdItem = m_pcsAgcmItem->GetItemData(pstAgpdItem);
		if (!pstAgcdItem)
			return FALSE;

		ReleaseConvertedItemEffect(pstAgpdItem, pstAgcdItem);
		RemoveAllEffect((ApBase *)(pstAgpdItem));

		AuCharClassType eCurrentClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &pstAgpdCharacter->m_csFactor );
		m_pcsAgcmItem->DetachItem( pstAgcdCharacter, (AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate), pstAgcdItem, NULL, eCurrentClassType );
	}

	pstAgpdItem	= GetEquipItem(m_pcsAgpmItem, pstAgpdCharacter, AGPMITEM_PART_HAND_LEFT);
	if (pstAgpdItem && m_pcsAgcmItem)
	{
		pstAgcdItem	= m_pcsAgcmItem->GetItemData(pstAgpdItem);
		if (!pstAgcdItem)
			return FALSE;

		ReleaseConvertedItemEffect(pstAgpdItem, pstAgcdItem);
		RemoveAllEffect((ApBase *)(pstAgpdItem));

		AuCharClassType eCurrentClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &pstAgpdCharacter->m_csFactor );
		m_pcsAgcmItem->DetachItem( pstAgcdCharacter, (AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate), pstAgcdItem, NULL, eCurrentClassType );
	}

	AgcdAnimationCallbackData	csData;
	m_pcsAgcmCharacter->SetAnimCBData( &csData, CBAttachShieldAndWeapons, pstAgcdCharacter->m_csAnimation.GetDuration(), FALSE, pstAgpdCharacter->m_lID );
	m_pcsAgcmCharacter->AddAnimCallback( pstAgcdCharacter, &csData );

	return TRUE;
}

BOOL AgcmEventEffect::AttachShieldAndWeapons(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, BOOL bSetConvertedItemEffect)
{
	if( !m_pcsAgcmItem )		return TRUE;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	AgpdItem			*pstAgpdItem			= NULL;
	AgcdItem			*pstAgcdItem			= NULL;
	AgcdItemTemplate	*pstAgcdItemTemplate	= NULL;	

	pstAgpdItem							= GetEquipItem(m_pcsAgpmItem, pstAgpdCharacter, AGPMITEM_PART_HAND_RIGHT);
	if( pstAgpdItem )
	{
		pstAgcdItem						= m_pcsAgcmItem->GetItemData(pstAgpdItem);
		if (!pstAgcdItem)
			return FALSE;

		pstAgcdItemTemplate				= m_pcsAgcmItem->GetTemplateData((AgpdItemTemplate *)(pstAgpdItem->m_pcsItemTemplate));
		if (!pstAgcdItemTemplate)
			return FALSE;

		AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_pcsCharacterTemplate->m_lID );
		m_pcsAgcmItem->AttachItem(
			pstAgpdCharacter->m_pcsCharacterTemplate->m_lID,
			pstAgcdCharacter,
			(AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate),
			pstAgcdItem,
			pstAgcdItemTemplate, eClassType );

		if (bSetConvertedItemEffect)
			SetConvertedItemEffect(pstAgpdItem, pstAgcdItem);
	}

	pstAgpdItem							= GetEquipItem(m_pcsAgpmItem, pstAgpdCharacter, AGPMITEM_PART_HAND_LEFT);
	if( pstAgpdItem )
	{
		pstAgcdItem						= m_pcsAgcmItem->GetItemData(pstAgpdItem);
		if (!pstAgcdItem)
			return FALSE;

		pstAgcdItemTemplate				= m_pcsAgcmItem->GetTemplateData((AgpdItemTemplate *)(pstAgpdItem->m_pcsItemTemplate));
		if (!pstAgcdItemTemplate)
			return FALSE;

		AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_pcsCharacterTemplate->m_lID );
		m_pcsAgcmItem->AttachItem(
			pstAgpdCharacter->m_pcsCharacterTemplate->m_lID,
			pstAgcdCharacter,
			(AgpdItemTemplateEquip *)(pstAgpdItem->m_pcsItemTemplate),
			pstAgcdItem,
			pstAgcdItemTemplate, eClassType );

		if (bSetConvertedItemEffect)
			SetConvertedItemEffect(pstAgpdItem, pstAgcdItem);
	}

	return TRUE;
}

BOOL AgcmEventEffect::AttachShieldAndWeapons(AgpdCharacter *pstAgpdCharacter, BOOL bSetConvertedItemEffect)
{
	AgcdCharacter* pstAgcdCharacter = m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	return pstAgcdCharacter ? AttachShieldAndWeapons(pstAgpdCharacter, pstAgcdCharacter, bSetConvertedItemEffect) : FALSE;
}

BOOL AgcmEventEffect::AttachShieldAndWeapons(INT32 lCID, BOOL bSetConvertedItemEffect)
{
	AgpdCharacter* pstAgpdCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);
	return pstAgpdCharacter ? AttachShieldAndWeapons(pstAgpdCharacter, bSetConvertedItemEffect) : FALSE;
}

PVOID AgcmEventEffect::CBAttachShieldAndWeapons(PVOID pvData)
{
	if( !pvData || !AgcmEventEffect::m_pcsThisAgcmEventEffect )		return NULL;

	PVOID* ppvData = (PVOID *)(pvData);

	m_pcsThisAgcmEventEffect->AttachShieldAndWeapons( (INT32)(ppvData[AGCMCHAR_ACDA_CHARACTER_ID]) );

	return ppvData[AGCMCHAR_AGCD_INIT_ANIM];
}

BOOL AgcmEventEffect::CopyCharAnimAttachedData(AgcdAnimation2 *pcsSrc, AgcdAnimation2 *pcsDest)
{
	if(	!pcsSrc || !pcsDest )		return FALSE;

	AgcdAnimData2	*pcsSrcData		= pcsSrc->m_pcsHead;
	AgcdAnimData2	*pcsDestData	= pcsDest->m_pcsHead;

	AEE_CharAnimAttachedData	*pcsSrcAttachedData		= NULL;
	AEE_CharAnimAttachedData	*pcsDestAttachedData	= NULL;

	AEE_CharAnimAttachedSoundData	*pcsNextSoundData	= NULL;
	AEE_CharAnimAttachedSoundData	*pcsSrcSoundData	= NULL;
	AEE_CharAnimAttachedSoundData	*pcsDestSoundData	= NULL;
		

	while (pcsSrcData)
	{
		if( !pcsDestData )		return FALSE;
			
		pcsSrcAttachedData	= (AEE_CharAnimAttachedData *)(m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pcsSrcData ) );
		pcsDestAttachedData	= (AEE_CharAnimAttachedData *)(m_pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData( AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME, pcsDestData ) );
		pcsDestAttachedData->Release();

		for( AEE_CharAnimAttachedData::SoundList::iterator	iter = pcsSrcAttachedData->GetList().begin(); iter != pcsSrcAttachedData->GetList().end(); ++iter )
			pcsDestAttachedData->GetList().push_back( AEE_CharAnimAttachedSoundData( (*iter).m_strSoundName.c_str() , (*iter).m_unConditions ) );

		pcsSrcData	= pcsSrcData->m_pcsNext;
		pcsDestData	= pcsDestData->m_pcsNext;
	}

	return TRUE;
}

BOOL AgcmEventEffect::CBCharUseEffect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventEffect *		pcsThis			= (AgcmEventEffect *) pClass;
	AgpdCharacter *			pcsCharacter	= (AgpdCharacter *) pData;
	AgpdCharacterAction	*	stActionBackup	= (AgpdCharacterAction *) pCustData;

	switch(stActionBackup->m_ulAdditionalEffect)
	{
		case AGPDCHAR_ADDITIONAL_EFFECT_END_BUFF_EXPLOSION:	pcsThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_TIMEATTACK);		break;
		case AGPDCHAR_ADDITIONAL_EFFECT_TELEPORT:			pcsThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_TELEPORT);		break;
		case AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_MP:			pcsThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_ABSORBMANA);		break;
		case AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_HP:			pcsThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_ABSORBLIFE);		break;
		case AGPDCHAR_ADDITIONAL_EFFECT_CONVERT_MP:			pcsThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_PULLINGMANNA);	break;
		case AGPDCHAR_ADDITIONAL_EFFECT_CONVERT_HP:			pcsThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_MENTALCIRCUIT);	break;
		case AGPDCHAR_ADDITIONAL_EFFECT_LENS_STONE:			pcsThis->SetCommonCharacterEffect(pcsCharacter, E_COMMON_CHAR_TYPE_LENS_STONE);		break;
		case AGPDCHAR_ADDITIONAL_EFFECT_FIRECRACKER:
		{
			// 폭죽은 그 종류가 다양하다.
			AgcdUseEffectSetData* pstEffectSetData = pcsThis->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID(stActionBackup->m_lUserData[0]);
			if( !pstEffectSetData )
				pcsThis->SetCommonCharacterEffect( pcsCharacter, E_COMMON_CHAR_TYPE_FIRECRACKER );
			else
			{
				pcsThis->SetCommonCharacterEffect( pcsCharacter, (eAgcmEventEffectDataCommonCharType)pstEffectSetData->m_lType );
				for(INT32 i = 0; i < AGCM_EVENT_EFFECT_DATA_MAX_SUB_EFFECT; ++i)
				{
					AgcdUseEffectSetData* pstEffectSetDataSub = pcsThis->m_csAgcmEventEffectData.SearchCommonCharEffectDataByEID(pstEffectSetData->m_alSubEffect[i]);
					if(pstEffectSetDataSub)
						pcsThis->SetCommonCharacterEffect(pcsCharacter, (eAgcmEventEffectDataCommonCharType)pstEffectSetDataSub->m_lType);
				}
			}

			break;
		}
	}

	return TRUE;
}
