#include "AgcmItem.h"
#include "AgcCharacterUtil.h"
#include "RtPITexD.h"
#include "AgcmRide.h"
#include "AgcmSkill.h"
#include "AgcmEventEffect.h"
#include "AgcmUIItem.h"

#ifdef _DEBUG
extern DWORD	g_dwThreadID;
#endif

BOOL AgcmItem::ReleaseEquipItems( AgpdCharacter* pcsAgpdCharacter )
{
	AgpdItemADChar* pstItemADChar = m_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);
	AgpdGrid* pcsEquipGrid = &pstItemADChar->m_csEquipGrid;

	INT32 lIndex = 0;
	for( AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex) )
	{
		AgpdItem* pcsItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
		if ( pcsItem )
			ReleaseItemData( pcsItem );
	}

	return TRUE;
}

BOOL AgcmItem::RefreshEquipItems(AgpdCharacter *pcsAgpdCharacter)
{
	AgpdItemADChar* pstItemADChar = m_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);
	AgpdGrid* pcsEquipGrid = &pstItemADChar->m_csEquipGrid;

	INT32 lIndex = 0;
	for( AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex))
	{
		AgpdItem* pcsItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
		if( !pcsItem )		continue;

		ReleaseItemData(pcsItem);
		MakeItemClump(pcsItem);
		EquipItem(pcsAgpdCharacter, pcsItem);
	}

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgcdCharacter* pcdCharacter = pcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcdCharacter ) return FALSE;

	OnUpdateViewHelmet( pcdCharacter );
	return TRUE;
}

RpAtomic *AgcmItem::AttachNodeIndex( RpClump* pstBaseClump, RpHAnimHierarchy* pstBaseHierarchy, RpHAnimHierarchy* pstAttachHierarchy, RpAtomic* pstSrcAtomic, 
									RwInt32 lNodeID, RwInt32 lPartID, RwMatrix* pstTransform, RpHAnimHierarchy** ppstAttachedAtomicHierarchy, enumRenderType eForedRenderType )
{
	LockFrame();

	RpAtomic* pstCloneAtomic = RpAtomicClone(pstSrcAtomic);
	if( !pstCloneAtomic )
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() Cloning Error!!!");

		UnlockFrame();
		return NULL;
	}

#ifdef _DEBUG
	pstCloneAtomic->pvApBase = RpAtomicGetClump(pstSrcAtomic) ? RpAtomicGetClump(pstSrcAtomic)->pvApBase : pstSrcAtomic->pvApBase;
#endif

	RwInt32 lNodeIndex = RpHAnimIDGetIndex( pstBaseHierarchy, lNodeID );
	if( lNodeIndex < 0 )
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() NodeID Error!!!");

		UnlockFrame();
		return NULL;
	}

	RpClumpAddAtomic( pstBaseClump, pstCloneAtomic );

	if( !pstBaseHierarchy->pNodeInfo[lNodeIndex].pFrame )
		RpHAnimHierarchyAttachFrameIndex( pstBaseHierarchy, lNodeIndex );

	RwFrame* pstCloneAtomicFrame	= NULL;

	if( pstAttachHierarchy )
	{
		RpClump *pstSrcClump				= RpAtomicGetClump(pstSrcAtomic);
		RwFrame *pstSrcClumpFrame			= RpClumpGetFrame(pstSrcClump);
		pstCloneAtomicFrame					= RwFrameCloneHierarchy(pstSrcClumpFrame);

		RpHAnimHierarchy *pstNewHierarchy	= RpHAnimHierarchyCreateFromHierarchy(pstAttachHierarchy, (RpHAnimHierarchyFlag)((pstAttachHierarchy)->flags), 0);

		RpHAnimFrameSetHierarchy(pstCloneAtomicFrame, pstNewHierarchy);
		RpHAnimHierarchyAttach(pstNewHierarchy);

		*(ppstAttachedAtomicHierarchy)		= pstNewHierarchy;

		TRACE("AgcmItem::AttachNodeIndex() Clump(Src %x, Base %x), Atomic(Src %x, Clone %x), Node(%d), Part(%d)\n", pstSrcClump, pstBaseClump, pstSrcAtomic, pstCloneAtomic, lNodeID, lPartID);
	}
	else
	{
		pstCloneAtomicFrame	= RwFrameCreate();
	}

	if (!pstCloneAtomicFrame)
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() CloneAtomic Frame Error!!!");

		UnlockFrame();
		return NULL;
	}

	RpAtomicSetFrame(pstCloneAtomic, pstCloneAtomicFrame);

	RwFrameAddChild(pstBaseHierarchy->pNodeInfo[lNodeIndex].pFrame, pstCloneAtomicFrame);
	if( pstTransform )
		RwFrameTransform(pstCloneAtomicFrame, pstTransform, rwCOMBINEREPLACE);

	UnlockFrame();

	if( lPartID )
		pstCloneAtomic->iPartID = lPartID;
	
	if(m_pWorld && (pstBaseClump->ulFlag & RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		if(m_pcsAgcmRender)
		{
			AgcdType* pTypeClone = AcuObject::GetAtomicTypeStruct(pstCloneAtomic);
			AgcdType* pTypeSrc = AcuObject::GetAtomicTypeStruct(pstSrcAtomic);
			ASSERT( pTypeClone && pTypeSrc );

			if( AcuObject::GetProperty(pTypeClone->eType) & ACUOBJECT_TYPE_RENDER_UDA && AcuObject::GetProperty(pTypeSrc->eType) & ACUOBJECT_TYPE_USE_ALPHAFUNC )
				if ( eForedRenderType != R_DEFAULT )
					pstCloneAtomic->stRenderInfo.renderType = eForedRenderType;

			m_pcsAgcmRender->AddAtomicToWorld(pstCloneAtomic, ONLY_ALPHA,AGCMRENDER_ADD_NONE, true);
		}
		else
			RpWorldAddAtomic( m_pWorld, pstCloneAtomic );
	}

	return pstCloneAtomic;
}

RpAtomic *AgcmItem::AttachItem(RpAtomic *atomic, AgcdItemTemplateEquipSet *pstAgcdItemTemplateEquipSet, BOOL bCheckPartID)
{
	if( !pstAgcdItemTemplateEquipSet->m_pstBaseClump )		return NULL;
	AgpdItemTemplate* ppdItemTemplate					= m_pcsAgpmItem->GetItemTemplate( pstAgcdItemTemplateEquipSet->m_nItemTID );

	PROFILE("AgcmItem::AttachItem");

	if ( bCheckPartID && atomic->iPartID != (RwInt32)(pstAgcdItemTemplateEquipSet->m_nPart ))
		return atomic;

	INT16 nKind = pstAgcdItemTemplateEquipSet->m_nKind;
	INT16 nPart	= pstAgcdItemTemplateEquipSet->m_nPart;

	RpAtomic* pstAtomic = NULL;
	RpAtomic* pstAtomic2 = NULL;

	switch( nKind )
	{
	case AGPMITEM_EQUIP_KIND_ARMOUR: pstAtomic = _AttachItemArmor( atomic, pstAgcdItemTemplateEquipSet, nPart );		break;
	case AGPMITEM_EQUIP_KIND_WEAPON:
		{
			AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon  = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplate;
			if( ppdItemTemplateEquipWeapon && 
				ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON &&
				nPart == AGPMITEM_PART_HAND_RIGHT )
			{
				if( atomic->iPartID != ( RwInt32 )( pstAgcdItemTemplateEquipSet->m_nPart ) )
				{
					// 예외처리 추가 : 드래곤시온용 제논아이템은 Max 상에서 Export 될때 Arms 로 설정되서 Export 된다.
					// 따라서 atomic->iPartID 가 AGPMITEM_PART_ARMS 인 경우에는 리턴하지 않고 정상처리하도록 한다.
					if( atomic->iPartID != AGPMITEM_PART_ARMS )
					return atomic;
				}

				pstAtomic = _AttachItemArmor( atomic, pstAgcdItemTemplateEquipSet, nPart );
			}
			else
			{
				pstAtomic = _AttachItemWeapon( atomic, pstAgcdItemTemplateEquipSet, nPart );
			}
		}
		break;

	case AGPMITEM_EQUIP_KIND_SHIELD:	pstAtomic = _AttachItemShield( atomic, pstAgcdItemTemplateEquipSet, nPart );	break;
	case AGPMITEM_EQUIP_KIND_RING:
	case AGPMITEM_EQUIP_KIND_NECKLACE:																					break;
	default:																											break;
	}

	++pstAgcdItemTemplateEquipSet->m_nCBCount;
	return atomic;
}

RpAtomic *AgcmItem::AttachCharonToSummoner( RpAtomic *atomic, AgcdItemTemplateEquipSet *pcdEquipSet )
{
	if( !pcdEquipSet || !pcdEquipSet->m_pstBaseClump ) return NULL;
	AgpdItemTemplate* ppdItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcdEquipSet->m_nItemTID );

	// 카론을 가슴에 착용하기 때문에 무조건 AGPMITEM_PART_BODY 에다가 건다.
	RpAtomic* pAtomic = _AttachItemArmor( atomic, pcdEquipSet, AGPMITEM_PART_BODY );

	++pcdEquipSet->m_nCBCount;
	return atomic;
}

RpAtomic* AgcmItem::AttachItemCheckPartIDCB(RpAtomic* atomic, PVOID pData)
{
	AgcdItemTemplateEquipSet	*pstAgcdItemTemplateEquipSet	= (AgcdItemTemplateEquipSet*)(pData);
	AgcmItem					*pThis							= (AgcmItem *)(pstAgcdItemTemplateEquipSet->m_pvClass);

	return pThis->AttachItem(atomic, pstAgcdItemTemplateEquipSet, TRUE);
}

RpAtomic* AgcmItem::AttachItemCB(RpAtomic* atomic, PVOID pData)
{
	AgcdItemTemplateEquipSet	*pstAgcdItemTemplateEquipSet	= (AgcdItemTemplateEquipSet*)(pData);
	AgcmItem					*pThis							= (AgcmItem *)(pstAgcdItemTemplateEquipSet->m_pvClass);

	return pThis->AttachItem(atomic, pstAgcdItemTemplateEquipSet, FALSE);
}

RpAtomic* AgcmItem::AttachCharonToSummonerCB( RpAtomic* atomic, PVOID pData )
{
	AgcdItemTemplateEquipSet	*pcdEquipSet = ( AgcdItemTemplateEquipSet* )pData;
	AgcmItem					*pThis		 = ( AgcmItem* )pcdEquipSet->m_pvClass;

	return pThis->AttachCharonToSummoner( atomic, pcdEquipSet );
}

BOOL AgcmItem::DetachNodeIndex(RpClump *pstBaseClump, RpHAnimHierarchy *pstBaseHierarchy, RpAtomic *pstDetachAtomic, RwInt32 lNodeID, RwInt32 lNextNodeID)
{
	RwFrame *	pstFrame;

	if ( pstDetachAtomic->ulFlag & RWFLAG_RENDER_ADD )
	{
		if(m_pcsAgcmRender)
			m_pcsAgcmRender->RemoveAtomicFromWorld(pstDetachAtomic);
		else if (m_pWorld)
			RpWorldRemoveAtomic(m_pWorld, pstDetachAtomic);
	}

	LockFrame();

	RpClumpRemoveAtomic(pstBaseClump, pstDetachAtomic);
	pstFrame = RpAtomicGetFrame(pstDetachAtomic);
	RpAtomicSetFrame(pstDetachAtomic, NULL);

	if (pstFrame != RpClumpGetFrame(pstBaseClump))
		RwFrameDestroy(pstFrame);

	m_pcsAgcmResourceLoader->AddDestroyAtomic(pstDetachAtomic);

	UnlockFrame();

	return TRUE;
}

RpAtomic *AgcmItem::DetachItem(RpAtomic *atomic, AgcdItemTemplateEquipSet *pstAgcdItemTemplateEquipSet)
{
	PROFILE("AgcmItem::DetachItem");

	RwFrame						*pstFrame;
	RpClump						*pstCharacterClump				= pstAgcdItemTemplateEquipSet->m_pstBaseClump;
	RpHAnimHierarchy			*pstCharacterHierarchy			= pstAgcdItemTemplateEquipSet->m_pstBaseHierarchy;
	INT16						nKind							= pstAgcdItemTemplateEquipSet->m_nKind;
	INT16						nPart							= pstAgcdItemTemplateEquipSet->m_nPart;
	AgpdItemTemplate*			ppdItemTemplate					= m_pcsAgpmItem->GetItemTemplate( pstAgcdItemTemplateEquipSet->m_nItemTID );

	if(atomic->iPartID != (RwInt32)(pstAgcdItemTemplateEquipSet->m_nPart))
	{
		// 원래는 iPartID 가 m_nPart 와 맞지 않으면 빠져나가야 하지만..
		// 드래곤시온용 제논의 경우 어쩔수 없이 맞지 않게 된다.. iPartID 는 4가 되고 m_nPart 는 10이 될 것이다.
		// 그래소 그걸 위해서 아래의 예외처리를 추가..

		// 템플릿 없으면 빠져나간다.
		if( !ppdItemTemplate ) return atomic;

		// 무기가 아니면 빠져나간다.
		if( nKind != AGPMITEM_EQUIP_KIND_WEAPON ) return atomic;

		// 무기타입이 한손제논이 아니면 빠져나간다.	
		AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplate;
		if( ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON &&
			ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON &&
			ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON &&
			ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON ) return atomic;

		// 장착부위가 오른손이 아니면 빠져나간다.
		if( nPart != AGPMITEM_PART_HAND_RIGHT ) return atomic;

		// 아토믹에 설정된 장착부위가 ARMS 가 아니면 빠져나간다. 제논은 맥스상에서 ARMS 로 세팅되어 Export 된다.
		if( atomic->iPartID != AGPMITEM_PART_ARMS ) return atomic;
	}

	switch(nKind)
	{
	case AGPMITEM_EQUIP_KIND_ARMOUR:
		{
			if (atomic->ulFlag & RWFLAG_RENDER_ADD)
			{
#ifdef _DEBUG
				if (g_dwThreadID != GetCurrentThreadId())
					ASSERT(!"No proper thread process!!!");
#endif
				if(m_pcsAgcmRender)
					m_pcsAgcmRender->RemoveAtomicFromWorld(atomic);
				else if (m_pWorld)
					RpWorldRemoveAtomic(m_pWorld, atomic);
			}

			LockFrame();

			RpClumpRemoveAtomic(pstCharacterClump, atomic);

			pstFrame = RpAtomicGetFrame(atomic);
			RpAtomicSetFrame(atomic, NULL);

			if (pstFrame != RpClumpGetFrame(pstCharacterClump))
			{
				m_pcsAgcmResourceLoader->AddDestroyAtomic(atomic);
			}

			UnlockFrame();
		}
		break;

	case AGPMITEM_EQUIP_KIND_WEAPON:
		{
			AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(pstAgcdItemTemplateEquipSet->m_pvCustData);
			DetachNodeIndex(pstCharacterClump, pstCharacterHierarchy, atomic, AGCMITEM_NODE_ID_RIGHT_HAND, (pcsAgcdItemTemplate->m_bEquipTwoHands) ? AGCMITEM_NODE_ID_LEFT_HAND : 0);
		}
		break;

	case AGPMITEM_EQUIP_KIND_SHIELD:
		{
			DetachNodeIndex(pstCharacterClump, pstCharacterHierarchy, atomic, AGCMITEM_NODE_ID_LEFT_HAND);
		}
		break;

	case AGPMITEM_EQUIP_KIND_RING:
	case AGPMITEM_EQUIP_KIND_NECKLACE:
		{
		}
		break;

	default: // error
		break;
	}

	return atomic;
}

RpAtomic* AgcmItem::DetachItemCB(RpAtomic* atomic, PVOID pData)
{
	AgcdItemTemplateEquipSet	*pstAgcdItemTemplateEquipSet	= (AgcdItemTemplateEquipSet*)(pData);
	AgcmItem					*pThis							= (AgcmItem *)(pstAgcdItemTemplateEquipSet->m_pvClass);

	return pThis->DetachItem(atomic, pstAgcdItemTemplateEquipSet);
}

BOOL AgcmItem::AttachItem(INT32 lCharacterTemplateID, AgcdCharacter *pstAgcdCharacter, AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip, AgcdItem *pstAgcdItem, AgcdItemTemplate *pstAgcdItemTemplate, AuCharClassType eClassType)
{
	AgpdItemTemplateEquip* ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
	AgcdItemTemplate* pcdEquipItemTemplate = pstAgcdItemTemplate;
	AgcdItem* pcdEquipItem = pstAgcdItem;

	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetCharacter( pstAgcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eCharClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eCharClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	// 드래곤시온의 경우 현재 진화형태에 따라 착용해야 할 아이템이 달라진다.
	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// 해당하는 물건이 없거나 해당되는 물건이 원래 물건이랑 같은 거면 원래 템플릿을 사용
		ppdEquipItemTemplate = _GetDragonScionItemAgpdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
		if( !ppdEquipItemTemplate )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else if( ppdEquipItemTemplate->m_lID == pcsAgpdItemTemplateEquip->m_lID )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else
		{
			pcdEquipItemTemplate = _GetDragonScionItemAgcdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
			pcdEquipItem = _GetDragonScionItemAgcdItem( ppdEquipItemTemplate, ppdCharacter, eClassType );
		}

		// 무기착용시 예외처리
		if( pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon = ( AgpdItemTemplateEquipWeapon* )pcsAgpdItemTemplateEquip;

			//// 캐릭터가 서머너이고 착용할 물건이 카론인 경우 예외처리 루틴을 탄다.
			//INT32 nWeaponType = ppdItemTemplateEquipWeapon->m_nWeaponType;
			//if( eClassType == AUCHARCLASS_TYPE_MAGE &&
			//	( nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON || nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON ) )
			//{
			//	return OnAttachCharonToSummoner( ppdCharacter, pstAgcdCharacter, ppdEquipItemTemplate, pcdEquipItemTemplate, pcdEquipItem );
			//}
		}
	}

	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
	if( pcmUIItem )
	{
		AgpdItem* ppdItem = GetItem( pcdEquipItem );
		if( !pcmUIItem->IsUsableItem( ppdCharacter, ppdItem, eClassType ) )
		{
			// 착용불가능한 아이템이라면 이하의 모델링작업은 처리하지 않는다.
			return FALSE;
		}
	}

	// 머리카락과 얼굴처리..
	// 기존 아이템이 얼굴과 헤어가 붙어있는지 확인해야한다.
	OnUpdateHairAndFace( ppdEquipItemTemplate, pstAgcdCharacter, TRUE );

	// EquipItem()을 직접호출 할 경우 초기화되지 않는다.
	RemoveAllAttachedAtomics( &pcdEquipItem->m_csAttachedAtomics );

	AgcdItemTemplateEquipSet stAgcdItemTemplateEquipSet;
	memset(&stAgcdItemTemplateEquipSet, 0, sizeof(AgcdItemTemplateEquipSet));
	stAgcdItemTemplateEquipSet.m_pvClass				= this;
	stAgcdItemTemplateEquipSet.m_nCBCount				= 0;
	stAgcdItemTemplateEquipSet.m_nPart					= ppdEquipItemTemplate->GetPartIndex();
	stAgcdItemTemplateEquipSet.m_nKind					= ppdEquipItemTemplate->m_nKind;
	stAgcdItemTemplateEquipSet.m_pstBaseClump			= pstAgcdCharacter->m_pClump;
	stAgcdItemTemplateEquipSet.m_pstBaseHierarchy		= pstAgcdCharacter->m_pInHierarchy;
	stAgcdItemTemplateEquipSet.m_pstEquipmentHierarchy	= pcdEquipItem->m_pstHierarchy;
	stAgcdItemTemplateEquipSet.m_pcsAttachedAtomics		= &pcdEquipItem->m_csAttachedAtomics;
	stAgcdItemTemplateEquipSet.m_pvCustData				= (PVOID)(pcdEquipItemTemplate);
	stAgcdItemTemplateEquipSet.m_pstTransformInfo		= pcdEquipItemTemplate->m_pItemTransformInfo->GetInfo( lCharacterTemplateID );
	stAgcdItemTemplateEquipSet.m_nItemTID				= ppdEquipItemTemplate->m_lID;

	if( !pcdEquipItem->m_pstClump ) return FALSE;

	RpClumpForAllAtomics( pcdEquipItem->m_pstClump,
						 (ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) ? (AttachItemCheckPartIDCB) : (AttachItemCB),
						 (PVOID)(&stAgcdItemTemplateEquipSet) );

	if (	ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR && (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY)	)
	{
		AttachedAtomicList* pcsCurrent = pcdEquipItem->m_csAttachedAtomics.pcsList;
		while (pcsCurrent)
		{
			m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
			pcsCurrent	= pcsCurrent->m_pcsNext;
		}
	}

	if( ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
	{
		AgpdItemTemplateEquipWeapon* ppdITemTemplateWeapon = ( AgpdItemTemplateEquipWeapon* )ppdEquipItemTemplate;
		if( ppdITemTemplateWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON &&
			ppdITemTemplateWeapon->m_nPart == AGPMITEM_PART_HAND_RIGHT )
		{
			AttachedAtomicList* pcsCurrent = pcdEquipItem->m_csAttachedAtomics.pcsList;
			while (pcsCurrent)
			{
				m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
				pcsCurrent	= pcsCurrent->m_pcsNext;
			}
		}
	}

	// Light 변경..
	if( ppdEquipItemTemplate->m_lightInfo.GetType() )
	{		
		AuCharacterLightInfo*	pLightInfo = pstAgcdCharacter->GetLightInfo( ppdEquipItemTemplate->GetPartIndex() );
		if( pLightInfo )
			*pLightInfo = ppdEquipItemTemplate->m_lightInfo;
	}

	return TRUE;
}

BOOL AgcmItem::DetachItem(AgcdCharacter *pstAgcdCharacter, AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip, AgcdItem *pstAgcdItem, AgcdItemTemplate *pstAgcdItemTemplate, AuCharClassType eClassType )
{
	if (!m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter))
		return TRUE;

	AgpdItemTemplateEquip* ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
	AgcdItemTemplate* pcdEquipItemTemplate = pstAgcdItemTemplate;
	AgcdItem* pcdEquipItem = pstAgcdItem;

	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetCharacter( pstAgcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// 드래곤시온의 경우 현재 진화형태에 따라 착용해야 할 아이템이 달라진다.
	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// 해당하는 물건이 없거나 해당되는 물건이 원래 물건이랑 같은 거면 원래 템플릿을 사용
		ppdEquipItemTemplate = _GetDragonScionItemAgpdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
		if( !ppdEquipItemTemplate )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else if( ppdEquipItemTemplate->m_lID == pcsAgpdItemTemplateEquip->m_lID )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else
		{
			pcdEquipItemTemplate = _GetDragonScionItemAgcdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
			pcdEquipItem = _GetDragonScionItemAgcdItem( ppdEquipItemTemplate, ppdCharacter, eClassType );
		}
	}

	AgcdItemTemplateEquipSet stAgcdItemTemplateEquipSet;
	memset(&stAgcdItemTemplateEquipSet, 0, sizeof(AgcdItemTemplateEquipSet));

	stAgcdItemTemplateEquipSet.m_pvClass					= this;
	stAgcdItemTemplateEquipSet.m_nCBCount					= 0;
	stAgcdItemTemplateEquipSet.m_nKind						= ppdEquipItemTemplate->m_nKind;
	stAgcdItemTemplateEquipSet.m_nPart						= ppdEquipItemTemplate->GetPartIndex();
	stAgcdItemTemplateEquipSet.m_pstBaseClump				= pstAgcdCharacter->m_pClump;
	stAgcdItemTemplateEquipSet.m_pstBaseHierarchy			= pstAgcdCharacter->m_pInHierarchy;
	stAgcdItemTemplateEquipSet.m_nItemTID					= ppdEquipItemTemplate->m_lID;

	if (ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)
	{
		stAgcdItemTemplateEquipSet.m_pvCustData = NULL;
	}
	else
	{
		if (!pcdEquipItemTemplate)
			pcdEquipItemTemplate = GetTemplateData((AgpdItemTemplate *)(ppdEquipItemTemplate));

		stAgcdItemTemplateEquipSet.m_pvCustData = (PVOID)(pcdEquipItemTemplate);
	}

	if( !pstAgcdCharacter->m_pClump )
	{
		ASSERT(!"AgcmItem::DetachItem() Error- pstAgcdCharacter->m_pClump!!!");
		return TRUE;
	}

	RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, DetachItemCB, (PVOID)(&stAgcdItemTemplateEquipSet));

	RemoveAllAttachedAtomics(& pstAgcdItem->m_csAttachedAtomics );

	AuCharacterLightInfo* pLightInfo = pstAgcdCharacter->GetLightInfo( ppdEquipItemTemplate->GetPartIndex() );
	if( pLightInfo )
		pLightInfo->Clear();

	return TRUE;
}

BOOL AgcmItem::UnEquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem, BOOL bViewHelmet )
{
	PROFILE("AgcmItem::UnEquipItem");

	if(!pstAgpdCharacter)
		return FALSE;

	// 변신중이면 무시한다.
	if (pstAgpdCharacter->m_bIsTrasform)
		return TRUE;

	// 독핸드 스킬에 적중당해 있는 경우에는 아이템 착용해제 불가능 하다.
	if( pstAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) return TRUE;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	return _UnEquipItem( pstAgpdCharacter, pcsAgpdItem, bEquipDefaultItem, bViewHelmet );
}

BOOL AgcmItem::ItemUnequipCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::ItemUnequipCB");
	if((!pData) || (!pClass) || (!pCustData)) return FALSE;

	AgpdItem				*pcsAgpdItem				= (AgpdItem *)(pData);
	AgcmItem				*pThis						= (AgcmItem *)(pClass);

	AgpdCharacter			*pstAgpdCharacter			= pcsAgpdItem->m_pcsCharacter;
	if(!pstAgpdCharacter) return FALSE;

	AgcdCharacter			*pstAgcdCharacter			= pThis->m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);

	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);

	// 해당 케릭터가 Remove된 넘이면 암것두 안한다.
	if (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED)
		return TRUE;

	pThis->UnEquipItem(pstAgpdCharacter, pcsAgpdItem, TRUE);
	pThis->RefreshEquipItems( pstAgpdCharacter );
	return TRUE;
}

BOOL AgcmItem::EquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckDefaultArmour, BOOL bViewHelmet )
{
	PROFILE("AgcmItem::EquipItem");
	TRACE("AgcmItem::EquipItem() CHR : %s , Item : %s\n", pstAgpdCharacter->m_szID, pcsAgpdItem->m_pcsItemTemplate ? ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_szName : "NULL");

	if( pstAgpdCharacter->m_bIsTrasform )		return TRUE;

	// 독핸드 스킬에 적중당해 있는 경우에는 아이템 착용불가능 하다.
	if( pstAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) return TRUE;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( pstAgpdCharacter );

		// 애니메이션 타입세팅만 해주고 넘어간다
		SetEquipAnimType( pstAgpdCharacter, pcdCharacter );
		return TRUE;
	}

	return _EquipItem( pstAgpdCharacter, pcsAgpdItem, bCheckDefaultArmour, bViewHelmet );
}

BOOL AgcmItem::ItemEquipCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::ItemEquipCB");
	if((!pData) || (!pClass)) return FALSE;

	AgpdItem				*pcsAgpdItem				= (AgpdItem *)(pData);
	AgcmItem				*pThis						= (AgcmItem *)(pClass);
	AgcdItem				*pstAgcdItem				= pThis->GetItemData(pcsAgpdItem);
	AgpdCharacter			*pstAgpdCharacter			= pcsAgpdItem->m_pcsCharacter;
	if(!pstAgpdCharacter) return FALSE;

	pThis->ReleaseItemData( pcsAgpdItem );		// 일단 ItemClump 를 Release한다.

	AgcdCharacter* pstAgcdCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData( pstAgpdCharacter);

	// Remove된 케릭터가 여기를 타면 안된다.
	ASSERT(!(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED));
	if (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED)
		return FALSE;

	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);

	return pThis->m_pcsAgcmResourceLoader->AddLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);
}

BOOL AgcmItem::SetEquipAnimType( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter )
{
	// 원래 애니메이션 타입 임시 보관
	INT32 nPrevAnimType = pcdCharacter->m_lCurAnimType2;

	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );

	AgpdItem* ppdWeaponLeft = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_LEFT );
	AgpdItem* ppdWeaponRight = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_RIGHT );

	if( ppdCharacter->m_bRidable )
	{		
		// 승마중이라면
		AgpdItem* ppdWeaponRide = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_LANCER );
		if( ppdWeaponRide )
		{
			// 랜스를 장착하고 있으면
			return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE );
		}
	}

	AgpdItemTemplateEquipWeapon* ppdWeaponTemplateLeft = ppdWeaponLeft ? ( AgpdItemTemplateEquipWeapon* )ppdWeaponLeft->m_pcsItemTemplate : NULL;
	AgpdItemTemplateEquipWeapon* ppdWeaponTemplateRight = ppdWeaponRight ? ( AgpdItemTemplateEquipWeapon* )ppdWeaponRight->m_pcsItemTemplate : NULL;
	if( !ppdWeaponTemplateLeft && !ppdWeaponTemplateRight )
	{
		// 양손모두 무기를 착용하지 않은 경우 무기착용 안한 것으로 간주한다.
		return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
	}

	// 오른손에 장착한 무기를 기준으로 처리한다.
	if( ppdWeaponTemplateRight )
	{
		// 왼손에도 무기를 장착한 경우.. 즉 이도류의 경우.. 적용가능한건 한손단검뿐이다.
		if( ppdWeaponTemplateLeft && ppdWeaponTemplateLeft->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER )
		{
			// 레이피어 + 단검 이도류의 경우 스워시버클러에게만 허용된다.
			if( ppdWeaponTemplateRight->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER )
			{
				if( eRaceType == AURACE_TYPE_MOONELF && eClassType == AUCHARCLASS_TYPE_KNIGHT )
				{
					return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_RAPIERDAGGER );
				}
			}
		}

		// 드래곤시온 슬레이어의 경우
		if( eRaceType == AURACE_TYPE_DRAGONSCION && ( eClassType == AUCHARCLASS_TYPE_KNIGHT || eClassType == AUCHARCLASS_TYPE_MAGE ) )
		{
			// 왼손에 뭔가 끼고 있고.. 카론인 경우 카론으로 세팅
			if( ppdWeaponTemplateLeft && ppdWeaponTemplateLeft->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON )
			{
				return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON );
			}

			// 왼손에 암것도 안끼고 있으면 맨손이다.
			if( !ppdWeaponTemplateLeft )
			{
				// 슬레이어가 왼손에다가 카론말고 다른걸 낄수나 있는지 모르지만 일단 무기착용 안한걸로 처리
				return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
			}
		}

		// 왼손무기를 신경쓰지 않는 경우 오른손에 장착한 무기 타입에 따라 애니메이션을 세팅한다.
		return _SetEquipAnimType( pcdCharacter, ppdWeaponTemplateRight->m_nWeaponType );
	}
	else
	{
		// 오른손에 장착한게 없는데 왼손에 장착한 것을 기준으로 처리해야 하는 경우는..
		// 드래곤시온 슬레이어의 경우 카론이 왼손장착이므로 왼손에 카론을 끼고 있는지 검사해야 한다.
		if( ppdWeaponTemplateLeft )
		{
			if( eRaceType == AURACE_TYPE_DRAGONSCION && ( eClassType == AUCHARCLASS_TYPE_KNIGHT || eClassType == AUCHARCLASS_TYPE_MAGE ) )
			{
				// 드래곤시온에다가 슬레이어의 경우
				if( ppdWeaponTemplateLeft->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON )
				{
					// 카론이라면 정상이다.
					return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON );
				}
				else
				{
					// 슬레이어가 왼손에다가 카론말고 다른걸 낄수나 있는지 모르지만 일단 무기착용 안한걸로 처리
					return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
				}
			}
			else
			{
				//// 슬레이어 아닌 넘들은 왼손에 멀 끼든간에 맨손이다.
				//return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
				// 왼손에 낀 무기에 따라 애니메이션을 설정한다.
				return _SetEquipAnimType( pcdCharacter, ppdWeaponTemplateLeft->m_nWeaponType );
			}
		}
		else
		{
			// 왼손에도 낀게 없으면 물론.. 무기착용을 안한것이다.
			return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
		}
	}

	// 애니메이션 타입이 변경되었으면 현재 애니메이션을 타입을 바꿔서 다시 틀어준다.
	if( nPrevAnimType != pcdCharacter->m_lCurAnimType2 )
	{
		if( AGCMCHAR_ANIM_TYPE_WAIT <= pcdCharacter->m_eCurAnimType && pcdCharacter->m_eCurAnimType < AGCMCHAR_MAX_ANIM_TYPE )
		{
			return m_pcsAgcmCharacter->StartAnimation( ppdCharacter, pcdCharacter, pcdCharacter->m_eCurAnimType );
		}
	}

	return TRUE;
}

VOID AgcmItem::ViewHelmet( AgcdCharacter* pstAgcdCharacter, BOOL bViewHelmet )
{
	//AgpdCharacter*		pcsAgpdCharacter= m_pcsAgcmCharacter->GetCharacter( pstAgcdCharacter );
	//AgpdItemADChar*		pstItemADChar	= m_pcsAgpmItem->GetADCharacter( pcsAgpdCharacter );
	//AgpdGrid*			pcsEquipGrid	= &pstItemADChar->m_csEquipGrid;
	//AgpdGridItem*		pcsGridItem;
	//INT32				nIndex;
	//AgpdItem*			pcsAgpdItem;

	//if( pstAgcdCharacter->m_bEquipHelmet && !pstAgcdCharacter->m_bViewHelmet && bViewHelmet ) {
	//	pstAgcdCharacter->m_bViewHelmet = bViewHelmet;

	//	nIndex = 0;
	//	for( pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ) )
	//	{
	//		pcsAgpdItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
	//		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)( pcsAgpdItem->m_pcsItemTemplate );

	//		if( pcsAgpdItem && pcsAgpdItemTemplateEquip &&
	//			pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD &&
	//			!pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	//		{
	//			ReleaseItemData( pcsAgpdItem );
	//			MakeItemClump( pcsAgpdItem );
	//			EquipItem( pcsAgpdCharacter, pcsAgpdItem, TRUE, TRUE );
	//		}
	//	}
	//	return;
	//}

	//if( pstAgcdCharacter->m_bEquipHelmet && pstAgcdCharacter->m_bViewHelmet && !bViewHelmet ) {
	//	pstAgcdCharacter->m_bViewHelmet = bViewHelmet;

	//	nIndex = 0;
	//	for( pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ) )
	//	{
	//		pcsAgpdItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
	//		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)( pcsAgpdItem->m_pcsItemTemplate );

	//		if( pcsAgpdItem && pcsAgpdItemTemplateEquip &&
	//			pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD &&
	//			!pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	//		{
	//			UnEquipItem( pcsAgpdCharacter, pcsAgpdItem, FALSE, TRUE );
	//		}
	//	}
	//	return;
	//}

	pstAgcdCharacter->m_bViewHelmet = bViewHelmet;
	OnUpdateViewHelmet( pstAgcdCharacter );
}

int AgcmItem::GetAnimDefaultRide( int lAnimType2 ) const
{
	switch( lAnimType2 )
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_DEFAULT;
	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_DEFAULT_RIDE;
	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE;
	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_DEFAULT_RIDE;
	case AGCMCHAR_AT2_ARCHLORD:
		return AGCMCHAR_AT2_ARCHLORD_DEFAULT_RIDE;
	}

	return -1;
}

int AgcmItem::GetAnimWeaponRide( int lAnimType2 ) const
{
	switch( lAnimType2 )
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_DEFAULT;
	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_WEAPON_RIDE;
	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_WEAPON_RIDE;
	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_WEAPON_RIDE;
	case AGCMCHAR_AT2_ARCHLORD:
		return AGCMCHAR_AT2_ARCHLORD_DEFAULT_RIDE;
	}

	return -1;
}

int AgcmItem::GetAnimStandardRide( int lAnimType2 ) const
{
	switch( lAnimType2 )
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_DEFAULT;
	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_STANDARD_RIDE;
	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_STANDARD_RIDE;
	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_STANDARD_RIDE;
	case AGCMCHAR_AT2_ARCHLORD:
		return AGCMCHAR_AT2_ARCHLORD_STANDARD_RIDE;
	}

	return -1;
}

BOOL AgcmItem::CBUpdateFactorDurability(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmItem	*pThis		= (AgcmItem *)		pClass;
	AgpdFactor	*pcsFactor	= (AgpdFactor *)	pData;

	INT32	lValue	= 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lValue, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_OWNER);
	if( ((ApBase *)lValue)->m_eType != APBASE_TYPE_ITEM )
		return TRUE;

	AgpdItem* pcsItem = (AgpdItem *)	lValue;
	if( !pThis->m_pcsAgpmCharacter->IsPC(pcsItem->m_pcsCharacter) )		return TRUE;
	if( pcsItem->m_eStatus != AGPDITEM_STATUS_EQUIP )					return TRUE;

	INT32	lDurability	= 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

	if (lDurability <= 0)
		pThis->UnEquipItem(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsItem, TRUE);

	return TRUE;
}

BOOL AgcmItem::CBUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmItem* pThis	= (AgcmItem *)		pClass;
	AgpdItem* pcsItem = (AgpdItem *)		pData;

	if( !pThis->m_pcsAgpmCharacter->IsPC( pcsItem->m_pcsCharacter ) )		return TRUE;

	if( pcsItem->m_eStatus != AGPDITEM_STATUS_EQUIP &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_NOTSETTING)
		return TRUE;

	// 내구도가 없는 반지, 목걸이는 내구도 검사에서 제외된다.
	if( pThis->IsNoneDurabilityItem( pcsItem ) ) return TRUE;

	INT32	lDurability	= 0;
	pThis->m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	if ( lDurability <= 0 )
	{
		pThis->UnEquipItem(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsItem, TRUE);

		// Equip Item (Weapon & Armour & Shield) 인 경우 Durability가 0 이하로 떨어졌는지 검사한다.
		////////////////////////////////////////////////////////////////////////////////////////////
		if (!(pcsItem->m_pcsItemTemplate &&
			((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP &&
			(((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR ||
			((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON ||
			((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)))
			pcsItem->m_pcsGridItem->SetDurabilityZero( FALSE );
		else
			pcsItem->m_pcsGridItem->SetDurabilityZero( TRUE );
	}
	else
		pcsItem->m_pcsGridItem->SetDurabilityZero( FALSE );

	return TRUE;
}

BOOL AgcmItem::OnEquipItemNecklace( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AgcdItem* pcdItem, AuCharClassType eClassType )
{
	AgcmEventEffect* pcmEventEffect = ( AgcmEventEffect* )GetModule( "AgcmEventEffect" );
	if( !pcmEventEffect ) return FALSE;

	BOOL bIsCloak = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );
	BOOL bIsWing = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );
	BOOL bIsWing2 = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );

	AuRaceType eRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = AUCHARCLASS_TYPE_NONE;

	if( eClassType == AUCHARCLASS_TYPE_NONE )
	{
		eClass = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	}
	else
	{
		eClass = eClassType;
	}

	// 망토 아이템인가?
	if( bIsCloak )
	{
		// 발동해야할 이펙트 타입은 무엇인가?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );
		
		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		if( !pcdEffectData ) return FALSE;

		INT32 nEffectID = m_ItemEffectFinder.GetEffectIDCloak( nEffectIndex, eRace, eClass );
		if( nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
		pcmEventEffect->SetCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}
	
	// 날개인가
	if( bIsWing || bIsWing2 )
	{
		// 발동해야할 이펙트 타입은 무엇인가?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );
		
		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		if( !pcdEffectData )
		{
			nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );
			pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
			if( !pcdEffectData ) return FALSE;
		}

		INT32 nEffectID = m_ItemEffectFinder.GetEffectIDWing( pcdEffectData->m_lType, eRace, eClass );
		if( nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
		pcmEventEffect->SetCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}

	return TRUE;
}

BOOL AgcmItem::OnUnEquipItemNecklace( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AgcdItem* pcdItem, AuCharClassType eClassType )
{
	if( !ppdCharacter || !ppdItem || !pcdItem || !m_pcsAgpmItemConvert ) return FALSE;

	AgcmEventEffect* pcmEventEffect = ( AgcmEventEffect* )GetModule( "AgcmEventEffect" );
	if( !pcmEventEffect ) return FALSE;

	BOOL bIsCloak = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );
	BOOL bIsWing = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );
	BOOL bIsWing2 = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );

	AuRaceType eRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = AUCHARCLASS_TYPE_NONE;

	if( eClassType == AUCHARCLASS_TYPE_NONE )
	{
		eClass = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	}
	else
	{
		eClass = eClassType;
	}

	// 망토 아이템인가?
	if( bIsCloak )
	{
		// 발동해야할 이펙트 타입은 무엇인가?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );
		
		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		INT32 nEffectID = m_ItemEffectFinder.GetEffectIDCloak( nEffectIndex, eRace, eClass );
		if( !pcdEffectData || nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}
	
	// 날개인가
	if( bIsWing || bIsWing2 )
	{
		// 발동해야할 이펙트 타입은 무엇인가?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );
		
		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		if( !pcdEffectData )
		{
			nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );
			pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
			if( !pcdEffectData ) return FALSE;
		}

		INT32 nEffectID = m_ItemEffectFinder.GetEffectIDWing( pcdEffectData->m_lType, eRace, eClass );
		if( nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}

	return TRUE;
}

BOOL AgcmItem::OnAttachCharonToSummoner( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter, AgpdItemTemplateEquip* ppdItemTemplateEquip, AgcdItemTemplate* pcdItemTemplate, AgcdItem* pcdItem )
{
	// 무기가 아니면 패스..
	if( ppdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON ) return FALSE;

	// 카론이 아니면 패스..
	AgpdItemTemplateEquipWeapon* ppdITemTemplateWeapon = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplateEquip;
	if( ppdITemTemplateWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON &&
		ppdITemTemplateWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON ) return FALSE;

	// EquipItem()을 직접호출 할 경우 초기화되지 않는다.
	RemoveAllAttachedAtomics( &pcdItem->m_csAttachedAtomics );

	AgcdItemTemplateEquipSet ItemTemplateEquipSet;
	memset( &ItemTemplateEquipSet, 0, sizeof( AgcdItemTemplateEquipSet ) );

	ItemTemplateEquipSet.m_pvClass					= this;
	ItemTemplateEquipSet.m_nCBCount					= 0;
	ItemTemplateEquipSet.m_nPart					= AGPMITEM_PART_BODY;				// 카론을 가슴에 착용하므로 무조건 BODY 다
	ItemTemplateEquipSet.m_nKind					= ppdItemTemplateEquip->m_nKind;
	ItemTemplateEquipSet.m_pstBaseClump				= pcdCharacter->m_pClump;
	ItemTemplateEquipSet.m_pstBaseHierarchy			= pcdCharacter->m_pInHierarchy;
	ItemTemplateEquipSet.m_pstEquipmentHierarchy	= pcdItem->m_pstHierarchy;
	ItemTemplateEquipSet.m_pcsAttachedAtomics		= &pcdItem->m_csAttachedAtomics;
	ItemTemplateEquipSet.m_pvCustData				= (PVOID)(pcdItemTemplate);
	ItemTemplateEquipSet.m_pstTransformInfo			= pcdItemTemplate->m_pItemTransformInfo->GetInfo( ppdCharacter->m_lTID1 );
	ItemTemplateEquipSet.m_nItemTID					= ppdItemTemplateEquip->m_lID;

	if( !pcdItem->m_pstClump ) return FALSE;

	RpClumpForAllAtomics( pcdItem->m_pstClump, AttachCharonToSummonerCB, ( PVOID )( &ItemTemplateEquipSet ) );

	// Light 변경..
	if( ppdItemTemplateEquip->m_lightInfo.GetType() )
	{		
		AuCharacterLightInfo* pLightInfo = pcdCharacter->GetLightInfo( ppdItemTemplateEquip->GetPartIndex() );
		if( pLightInfo )
		{
			*pLightInfo = ppdItemTemplateEquip->m_lightInfo;
		}
	}

	return TRUE;
}

AgpdItem* AgcmItem::GetCurrentEquipWeapon( void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return NULL;

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	if( !ppmItem ) return NULL;

	// 오른손 먼저 검사
	AgpdItem* ppdWeapon = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_RIGHT );
	if( !ppdWeapon )
	{
		// 오른손에 장착중인 무기가 없으면 왼손 검사
		ppdWeapon = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_LEFT );

		// 왼손에는 낀게 있는게 이게 방패면 NULL 을 리턴
		if( ppdWeapon )
		{
			AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdWeapon->m_pcsItemTemplate;
			if( !ppdItemTemplateEquip ) return NULL;
			if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD ) return NULL;
		}
	}

	return ppdWeapon;
}

AgpdItem* AgcmItem::GetCurrentEquipWeapon( void* pCharacter, INT32 nHand )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return NULL;

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	if( !ppmItem ) return NULL;

	AgpdItem* ppdWeapon = NULL;
	return ppmItem->GetEquipSlotItem( ppdCharacter, ( AgpmItemPart )nHand );
}

BOOL AgcmItem::OnAttachWeapon( void* pCharacter)
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	// 장착중인 아이템들을 돌면서 장착중인 무기를 찾는다.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	if( !ppmItem || !ppmGrid ) return FALSE;

	AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );

	// 애니메이션 타입세팅
	SetEquipAnimType( ppdCharacter, pcdCharacter );

	// 드래곤시온종족 서머너 클래스는 여기까지만 하고 넘어간다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		return TRUE;
	}

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// 해당 그리드의 아이템을 찾는다.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) continue;

		if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			//_UnEquipItem( ppdCharacter, ppdItemCurrent, FALSE );
			_EquipItem( ppdCharacter, ppdItemCurrent );
		}

		// 다음거..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}

BOOL AgcmItem::OnDetachWeapon( void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	// 장착중인 아이템들을 돌면서 장착중인 무기를 찾는다.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	if( !ppmItem || !ppmGrid ) return FALSE;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// 해당 그리드의 아이템을 찾는다.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) continue;

		if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			_UnEquipItem( ppdCharacter, ppdItemCurrent, FALSE );
		}

		// 다음거..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}

BOOL AgcmItem::OnAttachPart( VOID* pCharacter , AgpmItemPart ePartType )
{
	AgpdCharacter*	ppdCharacter	= static_cast< AgpdCharacter* >(pCharacter);
	AgpmItem*		ppmItem			= static_cast< AgpmItem* >(GetModule( "AgpmItem" ));
	AgpmGrid*		ppmGrid			= static_cast< AgpmGrid* >(GetModule( "AgpmGrid" ));	
	if( !ppmItem || !ppmGrid || !ppdCharacter ) 
		return FALSE;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType		eRaceType	= static_cast< AuRaceType >(m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ));
	AuCharClassType eClassType	= m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );

		SetEquipAnimType( ppdCharacter, pcdCharacter );
		return TRUE;
	}

	AgpdItemADChar* ppdItemADChar	= ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid*		ppdEquipGrid	= &ppdItemADChar->m_csEquipGrid;

	INT32			nSequencialIndex	= 0;	
	AgpdGridItem*	ppdGridItem			= ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// 해당 그리드의 아이템을 찾는다.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) 
			continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) 
			continue;

		if( ppdItemTemplateEquip->m_nPart == ePartType )
			_EquipItem( ppdCharacter, ppdItemCurrent );

		// 다음거..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}

BOOL AgcmItem::OnDetachPart( VOID* pCharacter , AgpmItemPart ePartType )
{
	AgpdCharacter*	ppdCharacter	= static_cast< AgpdCharacter* >(pCharacter);
	AgpmItem*		ppmItem			= static_cast< AgpmItem* >(GetModule( "AgpmItem" ));
	AgpmGrid*		ppmGrid			= static_cast< AgpmGrid* >(GetModule( "AgpmGrid" ));	
	if( !ppmItem || !ppmGrid || !ppdCharacter ) 
		return FALSE;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType		eRaceType	= static_cast< AuRaceType >(m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ));
	AuCharClassType eClassType	= m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );

		SetEquipAnimType( ppdCharacter, pcdCharacter );
		return TRUE;
	}

	AgpdItemADChar* ppdItemADChar	= ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid*		ppdEquipGrid	= &ppdItemADChar->m_csEquipGrid;

	INT32			nSequencialIndex	= 0;	
	AgpdGridItem*	ppdGridItem			= ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// 해당 그리드의 아이템을 찾는다.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) 
			continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) 
			continue;

		if( ppdItemTemplateEquip->m_nPart == ePartType )
			_UnEquipItem( ppdCharacter, ppdItemCurrent , TRUE );

		// 다음거..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}


BOOL AgcmItem::OnUpdateHairAndFace( AgpdItemTemplate* ppdItemTemplate, AgcdCharacter* pcdCharacter, BOOL bIsEquip )
{
	if( !ppdItemTemplate || !pcdCharacter ) return FALSE;
	if( ppdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP ) return FALSE;
	
	AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemTemplate;
	if( ppdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR ) return FALSE;
	if( ppdItemTemplateEquip->GetPartIndex() != AGPMITEM_PART_HEAD ) return FALSE;

	AgcdItemTemplate* pcdItemTemplate = GetTemplateData( ppdItemTemplate );
	if( !pcdItemTemplate ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
	if( !pcmCharacter || !ppmFactor ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetCharacter( pcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// 서머너는 패스
	AuRaceType eRace = ( AuRaceType )ppmFactor->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_csFactor );
	if( eRace == AURACE_TYPE_DRAGONSCION && eClass == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	// Case 1-1. 캐릭터의 원래 머리와 함께 나와야 하는 경우
	if( pcdItemTemplate->m_bWithHair )
	{
		// 캐릭터의 원래 머리를 붙인다.
		pcmCharacter->SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
	}
	// Case 1-2. 캐릭터의 원래 머리를 제거하고 아이템만 나와야 하는 경우
	else 
	{
		// Case 1-2-1. 이 아이템을 착용하려 하는 경우
		if( bIsEquip )
		{
			// 캐릭터에 붙어 있는 머리 제거
			m_pcsAgcmCharacter->DetachHair( pcdCharacter );

			// 머리 붙일수 없다고 명시
			pcdCharacter->m_bAttachableHair = FALSE;
		}
		// Case 1-2-2. 이 아이템을 착용해제하는 경우
		else
		{
			// 제거했던 머리를 원래대로 복구해준다.
			pcdCharacter->m_bAttachableHair = TRUE;
			pcmCharacter->SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
		}
	}

	// Case 2-1. 캐릭터의 원래 얼굴과 함께 나와야 하는 경우
	if( pcdItemTemplate->m_bWithFace )
	{
		// 캐릭터의 원래 얼굴을 붙인다.
		pcmCharacter->SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
	}
	// Case 2-2. 캐릭터의 원래 얼굴을 제거하고 아이템만 나와야 하는 경우
	else
	{
		// Case 2-2-1. 이 아이템을 착용하려 하는 경우
		if( bIsEquip )
		{
			// 캐릭터에 붙어 있는 얼굴 제거
			m_pcsAgcmCharacter->DetachFace( pcdCharacter );

			// 캐릭터에 얼굴을 붙일수 없다고 명시
			pcdCharacter->m_bAttachableFace = FALSE;
		}
		// Case 2-2-2. 이 아이템을 착용헤제하는 경우
		else
		{
			// 제거한 얼굴을 다시 붙여준다.
			pcmCharacter->SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
		}
	}

	return TRUE;
}

BOOL AgcmItem::OnUpdateViewHelmet( AgcdCharacter* pcdCharacter )
{
	if( !pcdCharacter ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
	if( !ppmItem || !pcmCharacter || !ppmFactor ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetCharacter( pcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// 서머너는 패스
	AuRaceType eRace = ( AuRaceType )ppmFactor->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_csFactor );
	if( eRace == AURACE_TYPE_DRAGONSCION && eClass == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	// 현재 착용중인 아이템들 중에서 HEAD 파츠에 해당하는 아이템을 찾는다.
	AgpdGridItem* ppdGridItem = ppmItem->GetEquipItem( ppdCharacter, AGPMITEM_PART_HEAD );
	if( !ppdGridItem ) return FALSE;

	AgpdItem* ppdItem = ppmItem->GetItem( ppdGridItem->m_lItemID );
	if( !ppdItem ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ppdItem->m_pcsItemTemplate;
	AgcdItemTemplate* pcdItemTemplate = GetTemplateData( ppdItemTemplate );
	if( !pcdItemTemplate ) return FALSE;
	if( ppmItem->IsAvatarItem( ppdItem->m_pcsItemTemplate ) ) return FALSE;
	if( _IsWearAvatarItem( ppdCharacter, TRUE, TRUE ) ) return FALSE;

	// Case 1. 헬멧을 보여주지 말아야 하는 경우
	if( !pcdCharacter->m_bViewHelmet )
	{
		// 헬멧아이템을 UnEquip 해준다.
		UnEquipItem( ppdCharacter, ppdItem, FALSE, pcdCharacter->m_bViewHelmet );

		// HEAD 파츠 아이템이 머리를 날려버리나?
		if( !pcdItemTemplate->m_bWithHair )
		{
			// 날아간 머리를 복구해준다.
			pcmCharacter->SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
		}

		// HEAD 파츠 아이템이 얼굴을 날려버리나?
		if( !pcdItemTemplate->m_bWithFace )
		{
			// 날아간 얼굴을 복구 해준다.
			pcmCharacter->SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
		}
	}
	// Case 2. 헬멧을 보여줘야 하는 경우
	else
	{
		EquipItem( ppdCharacter, ppdItem, FALSE, TRUE );
		OnUpdateHairAndFace( ppdItemTemplate, pcdCharacter, TRUE );
	}

	return TRUE;
}

AgpdItemTemplateEquip* AgcmItem::_GetDragonScionItemAgpdTemplate( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItem || !ppdCharacter ) return NULL;

	AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplateEquip ) return NULL;

	return _GetDragonScionItemAgpdTemplate( ppdItemTemplateEquip, ppdCharacter, eClassType );
}

AgpdItemTemplateEquip* AgcmItem::_GetDragonScionItemAgpdTemplate( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItemTemplate || !ppdCharacter ) return NULL;

	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION )
	{
		return ( AgpdItemTemplateEquip* )ppdItemTemplate;
	}

	INT32 nEvolutionTID = m_ItemEvolutionTable.GetEvolutionTID( ppdItemTemplate->m_lID, eClassType );
	if( nEvolutionTID < 0 )
	{
		return ( AgpdItemTemplateEquip* )ppdItemTemplate;
	}

	return ( AgpdItemTemplateEquip* )m_pcsAgpmItem->GetItemTemplate( nEvolutionTID );
}

AgcdItemTemplate* AgcmItem::_GetDragonScionItemAgcdTemplate( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	AgpdItemTemplateEquip* pItemTemplate = _GetDragonScionItemAgpdTemplate( ppdItem, ppdCharacter, eClassType );
	if( !pItemTemplate ) return NULL;
	return GetTemplateData( ( AgpdItemTemplate* )pItemTemplate );
}

AgcdItemTemplate* AgcmItem::_GetDragonScionItemAgcdTemplate( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItemTemplate || !ppdCharacter ) return NULL;
	return GetTemplateData( ( AgpdItemTemplate* )ppdItemTemplate );
}

AgcdItem* AgcmItem::_GetDragonScionItemAgcdItem( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItemTemplate ) return NULL;
	AgpdItem* ppdItem = m_pcsAgpmItem->GetInventoryItemByTID( ppdCharacter, ppdItemTemplate->m_lID );
	
	// 아이템이 아직 만들어지지 않았으면 생성
	if( !ppdItem )
	{
		INT32 nItemID = 100000 + ppdItemTemplate->m_lID;
		ppdItem = m_pcsAgpmItem->AddItem( nItemID, ppdItemTemplate->m_lID, 1, FALSE );
		m_pcsAgpmItem->InitItem( ppdItem );

		ppdItem->m_pcsCharacter = ppdCharacter;
		ppdItem->m_eStatus = AGPDITEM_STATUS_EQUIP;
		MakeItemClump( ppdItem );
	}

	return GetItemData( ppdItem );
}

BOOL AgcmItem::_IsMatrixValid( RwMatrix* pTM )
{
	if( !pTM ) return FALSE;

	float fLengthAt = RwV3dLength( &pTM->at );
	float fLengthRight = RwV3dLength( &pTM->right );
	float fLengthUp = RwV3dLength( &pTM->up );

	if( fLengthAt == 0.0f || fLengthRight == 0.0f || fLengthUp == 0.0f ) return FALSE;
	return TRUE;
}

RpAtomic* AgcmItem::_AttachItemArmor( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart )
{
	if( !pAtomic || !pEquipSet ) return NULL;

	RpClump				*pstCharacterClump				= pEquipSet->m_pstBaseClump;
	RpHAnimHierarchy	*pstCharacterHierarchy			= pEquipSet->m_pstBaseHierarchy;
	RpHAnimHierarchy	*pstItemHierarchy				= pEquipSet->m_pstEquipmentHierarchy;
	INT16				nKind							= pEquipSet->m_nKind;
	RwInt16				nCBCount						= pEquipSet->m_nCBCount;

	LockFrame();

	RpAtomic* pstAtomic = RpAtomicClone(pAtomic);
	if (!pstAtomic)
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() Cloning Error!!!");
		UnlockFrame();
		return NULL;
	}

#ifdef _DEBUG
	pstAtomic->pvApBase = RpAtomicGetClump(pAtomic) ? RpAtomicGetClump(pAtomic)->pvApBase : pAtomic->pvApBase;
#endif

	RpGeometry* pstGeom		= RpAtomicGetGeometry(pstAtomic);
	RwFrame* pstFrame		= RpClumpGetFrame(pstCharacterClump);
	RwFrame* pstAtomicFrame	= RwFrameCreate();

	RpAtomic* pstAtomic2 = NULL;
	AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(pEquipSet->m_pvCustData);

	RpAtomicSetFrame(pstAtomic, pstAtomicFrame);
	RwFrameAddChild(pstFrame, pstAtomicFrame);
	RpClumpAddAtomic(pstCharacterClump, pstAtomic);

	UnlockFrame();

	AcuObject::SetAtomicRenderUDA( pstAtomic, pstAtomic->stRenderInfo.renderType,
									(RpAtomicCallBackRender)(pstAtomic->stRenderInfo.backupCB),
									pstAtomic->stRenderInfo.blendMode, 0, 0 );

	if (pEquipSet->m_pstTransformInfo)
	{
		//ASSERT(!"Not Used Code...");
		RwMatrix* pstMatrix = pEquipSet->m_pstTransformInfo->m_astTransform + nCBCount;
		RpGeometryTransform( pstGeom, pstMatrix );
	}

	if( pcsAgcdItemTemplate && pcsAgcdItemTemplate->m_bEquipTwoHands )
	{			
		pstAtomic2 = RpAtomicClone(pAtomic);
		if( pstAtomic2 )
		{
			LockFrame();
#ifdef _DEBUG
			pstAtomic2->pvApBase = RpAtomicGetClump(pAtomic) ? RpAtomicGetClump(pAtomic)->pvApBase : pAtomic->pvApBase;
#endif
			RpGeometry* pstGeom		= RpAtomicGetGeometry(pstAtomic2);
			RwFrame* pstFrame		= RpClumpGetFrame(pstCharacterClump);
			RwFrame* pstAtomicFrame	= RwFrameCreate();
			RpAtomicSetFrame(pstAtomic2, pstAtomicFrame);
			RwFrameAddChild(pstFrame, pstAtomicFrame);
			RpClumpAddAtomic(pstCharacterClump, pstAtomic2);

			UnlockFrame();

			AcuObject::SetAtomicRenderUDA( pstAtomic2, pstAtomic2->stRenderInfo.renderType,
											(RpAtomicCallBackRender)(pstAtomic2->stRenderInfo.backupCB),
											pstAtomic2->stRenderInfo.blendMode, 0, 0 );

			if( pEquipSet->m_pstTransformInfo )
			{
				RwMatrix* pstMatrix = pEquipSet->m_pstTransformInfo->m_astTransform + 1;
				RpGeometryTransform( pstGeom, pstMatrix );
			}
		}			
	}

	if (m_pWorld && (pEquipSet->m_pstBaseClump->ulFlag & RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		if(m_pcsAgcmRender)
			m_pcsAgcmRender->AddAtomicToWorld(pstAtomic, ONLY_ALPHA,AGCMRENDER_ADD_NONE, true);
		else
			RpWorldAddAtomic(m_pWorld, pstAtomic);

		// shadow영역 갱신
		if( m_pcsAgcmShadow2 )
		{
			RwFrame*	pFrame = RpClumpGetFrame(pstCharacterClump);
			if( pFrame )
			{
				RwV3d* pPos = RwMatrixGetPos( RwFrameGetLTM(pFrame) );
				if( m_pcsAgcmShadow2->GetShadowBuffer( pPos ) )
					m_pcsAgcmRender->CustomizeLighting(pstCharacterClump, LIGHT_IN_SHADOW);
			}
		}
	}

	if (pEquipSet->m_pcsAttachedAtomics)
	{
		if (pstAtomic)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic);

		if (pstAtomic2)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic2);
	}

	return pstAtomic;
}

RpAtomic* AgcmItem::_AttachItemWeapon( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart )
{
	if( !pAtomic || !pEquipSet ) return NULL;

	RpClump				*pstCharacterClump				= pEquipSet->m_pstBaseClump;
	RpHAnimHierarchy	*pstCharacterHierarchy			= pEquipSet->m_pstBaseHierarchy;
	RpHAnimHierarchy	*pstItemHierarchy				= pEquipSet->m_pstEquipmentHierarchy;

	INT32 lNodeID	= AGCMITEM_NODE_ID_RIGHT_HAND;
	INT32 lNodeID2	= AGCMITEM_NODE_ID_LEFT_HAND;

	if (nPart == AGPMITEM_PART_HAND_LEFT)
	{
		lNodeID		= AGCMITEM_NODE_ID_LEFT_HAND;
		lNodeID2	= AGCMITEM_NODE_ID_RIGHT_HAND;
	}

	ItemTransformInfo* pTransformInfo = pEquipSet->m_pstTransformInfo;
	RwMatrix* pAttachTM = NULL;

	if( pTransformInfo )
	{
		pAttachTM = pTransformInfo->m_astTransform;
		if( !_IsMatrixValid( pAttachTM ) )
		{
			pAttachTM = NULL;
		}
	}

	RpAtomic* pstAtomic = AttachNodeIndex( pstCharacterClump, pstCharacterHierarchy, pstItemHierarchy, pAtomic,
		lNodeID, nPart,	pAttachTM, &pEquipSet->m_pcsAttachedAtomics->m_pstHierarchy, R_ALPHAFUNC );

	RpAtomic* pstAtomic2 = NULL;
	AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(pEquipSet->m_pvCustData);

	if (pcsAgcdItemTemplate->m_bEquipTwoHands)
	{
		ItemTransformInfo* pTransformInfo1 = pTransformInfo + 1;
		RwMatrix* pAttachTM1 = NULL;

		if( pTransformInfo && pTransformInfo1 )
		{
			pAttachTM1 = pTransformInfo->m_astTransform + 1;
			if( !_IsMatrixValid( pAttachTM1 ) )
			{
				pAttachTM1 = NULL;
			}
		}

		pstAtomic2 = AttachNodeIndex( pstCharacterClump, pstCharacterHierarchy, pstItemHierarchy, pAtomic,
			lNodeID2, nPart, pAttachTM1, &pEquipSet->m_pcsAttachedAtomics->m_pstHierarchy, R_ALPHAFUNC );
	}

	// shadow영역 갱신
	if(m_pcsAgcmShadow2 && (pstCharacterClump->ulFlag &RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		RwFrame* pFrame = RpClumpGetFrame(pstCharacterClump);
		if( pFrame )
		{
			RwV3d* pPos = RwMatrixGetPos( RwFrameGetLTM( pFrame ) );
			if(m_pcsAgcmShadow2->GetShadowBuffer(pPos))
				m_pcsAgcmRender->CustomizeLighting( pstCharacterClump, LIGHT_IN_SHADOW );
		}
	}

	if (pEquipSet->m_pcsAttachedAtomics)
	{
		if (pstAtomic)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic);

		if (pstAtomic2)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic2);
	}

	return pstAtomic;
}

RpAtomic* AgcmItem::_AttachItemShield( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart )
{
	if( !pAtomic || !pEquipSet ) return NULL;

	RpClump				*pstCharacterClump				= pEquipSet->m_pstBaseClump;
	RpHAnimHierarchy	*pstCharacterHierarchy			= pEquipSet->m_pstBaseHierarchy;
	RpHAnimHierarchy	*pstItemHierarchy				= pEquipSet->m_pstEquipmentHierarchy;

	INT32 lNodeID = nPart == AGPMITEM_PART_HAND_RIGHT ? AGCMITEM_NODE_ID_RIGHT_HAND : AGCMITEM_NODE_ID_LEFT_HAND;

	RpAtomic* pstAtomic = AttachNodeIndex( pstCharacterClump, pstCharacterHierarchy, pstItemHierarchy, pAtomic,
		lNodeID, nPart,	pEquipSet->m_pstTransformInfo ? pEquipSet->m_pstTransformInfo->m_astTransform : NULL,
		&pEquipSet->m_pcsAttachedAtomics->m_pstHierarchy );

	// shadow영역 갱신
	if(m_pcsAgcmShadow2 && (pstCharacterClump->ulFlag & RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		RwFrame* pFrame = RpClumpGetFrame( pstCharacterClump );
		if( pFrame )
		{
			RwV3d* pPos = RwMatrixGetPos( RwFrameGetLTM(pFrame) );
			if( m_pcsAgcmShadow2->GetShadowBuffer( pPos ) )
				m_pcsAgcmRender->CustomizeLighting( pstCharacterClump, LIGHT_IN_SHADOW );
		}
	}

	if (pEquipSet->m_pcsAttachedAtomics)
	{
		if (pstAtomic)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic);
	}

	return pstAtomic;
}

BOOL AgcmItem::_EquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckDefaultArmour, BOOL bViewHelmet )
{
	AgcdItem				*pstAgcdItem				= GetItemData(pcsAgpdItem);

	AgcdCharacter			*pstAgcdCharacter			= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= pstAgcdCharacter->m_pstAgcdCharacterTemplate;
	AgcdItemTemplate		*pstAgcdItemTemplate		= GetTemplateData((AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate));
	AgpdItemTemplateEquip	*pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate);

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	//말을 탄상태에서 로그인되면 이곳에서 말을 태워야한다.
	if( pstAgpdCharacter->m_bRidable && pstAgcdCharacter->m_pRide == NULL &&
		pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_RIDE )
	{
		AgcmRide::CBRideAck( pstAgpdCharacter, m_pcsAgcmRide, &pcsAgpdItem->m_lID );
		return TRUE;
	}

	/*
	//탈것을 타고있는 상태에선 마상무기외의 무기는 장착되지 않는다.
	if( pstAgpdCharacter->m_bRidable && 
		pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON && 
		pcsAgpdItemTemplateEquip->GetPartIndex() != AGPMITEM_PART_LANCER ) 
	{
		return TRUE;
	}
	*/

	//방패도 장착할수 없다.
	if( pstAgpdCharacter->m_bRidable &&
		pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD )
	{
		return TRUE;
	}

	//탈것을 안탄상태에선 마상무기는 장착되지 않는다.
	if( pstAgpdCharacter->m_bRidable == FALSE && pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_LANCER )
	{
		return TRUE;
	}

	// 내구도가 0이하이면 착용할 수 없다. (반지, 목걸이 제외)
	if (pcsAgpdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_NECKLACE && 
		pcsAgpdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_RING)
	{
		if (m_pcsAgpmCharacter->IsPC(pstAgpdCharacter))
		{
			INT32	lCurrentDurability	= 0;
			m_pcsAgpmFactors->GetValue(&pcsAgpdItem->m_csFactor, &lCurrentDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
			
			#ifdef USE_MFC
				if( lCurrentDurability <= 0 )
					lCurrentDurability = 1;			
			#else
			if( lCurrentDurability <= 0 && !pcsAgpdItemTemplateEquip->IsAvatarEquip() )
				return TRUE;		
			#endif
		}
	}

	if( pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD )
	{
		if( !bViewHelmet )
			pstAgcdCharacter->m_bEquipHelmet = TRUE;

		if( !pcsAgpdItemTemplateEquip->IsAvatarEquip() && !pstAgcdCharacter->m_bViewHelmet )
			return TRUE;
	}

	if( !pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	{
		AgpdGridItem * pGrid = m_pcsAgpmItem->GetEquipItem( pstAgpdCharacter , pcsAgpdItemTemplateEquip->GetAvatarPartIndex() );
		if( pGrid )		// 아바타 아이템이 이미 존재한다.
			return TRUE;
	}

	// 이하의 작업은 드래곤시온 서머너의 경우 처리하지 않는다.
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);
		EnumCallback(AGCMITEM_CB_ID_EQUIP_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

		// 아이템 종류에 따라 추가로 처리해야할 것들을 처리한다.
		switch( pcsAgpdItemTemplateEquip->m_nKind )
		{
		case AGPMITEM_EQUIP_KIND_NECKLACE :
			{
				// 목걸이의 경우 망토기원석이 박혀 있는지 검사하여 이를 처리한다.
				OnEquipItemNecklace( pstAgpdCharacter, pcsAgpdItem, pstAgcdItem );
			}
			break;
		};

		return TRUE;
	}

	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
	if( pcmUIItem )
	{
		if( !pcmUIItem->IsUsableItem( pstAgpdCharacter, pcsAgpdItem, eClassType ) )
		{
			// 착용불가능한 아이템이라면 이하의 모델링작업은 처리하지 않는다.
			return FALSE;
		}
	}

	// 이미 Attach된 Item이 여기에 들어오면 안된다.
//	ASSERT(!(pstAgcdItem->m_lStatus & AGCDITEM_STATUS_ATTACH));

	// Valid 하지 않은 케릭터는 여기까지 들어오면 안된다.
	// Valid 하지 않은 케릭터는 다음의 2가지 경우이다.
	// 1. 아직 Init가 다 끝나지 않았다. (이 경우는 ResourceLoader를 통해서 InitCharacter가 된 다음 들어와야 한다.)
	// 2. 이미 Remove된 케릭터이다. (이 경우는 먼저 UnEquip이 되기 때문에 ResourceLoader의 List에서 제외되야 한다.)
	ASSERT(m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter));
	ASSERT(pstAgcdCharacter->m_pClump && pstAgcdCharacter->m_pInHierarchy);		// 당연히 Valid 한 Character라면 Clump는 있어야 된다.
	if( !pstAgcdCharacter->m_pClump )
		return TRUE;

	// Record the 'countEnd' value of an atomic in order to use it later in case of the 'invisible' mode.
	RwInt16 countEnd = 0;
	if( pstAgcdCharacter->m_pClump && pstAgcdCharacter->m_pClump->atomicList )
		countEnd = pstAgcdCharacter->m_pClump->atomicList->stRenderInfo.countEnd;

	AgcdItemTemplateEquipSet stAgcdItemTemplateEquipSet;
	if( /*(bCheckDefaultArmour) && */(pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) && (pstAgcdCharacterTemplate->m_pDefaultArmourClump) )
	{
		ZeroMemory(&stAgcdItemTemplateEquipSet, sizeof(AgcdItemTemplateEquipSet));

		stAgcdItemTemplateEquipSet.m_pvClass					= this;
		stAgcdItemTemplateEquipSet.m_nCBCount					= 0;
		stAgcdItemTemplateEquipSet.m_nPart						= pcsAgpdItemTemplateEquip->GetPartIndex();	// 지울려고 하는 아템의 part와 동일...
		stAgcdItemTemplateEquipSet.m_nKind						= pcsAgpdItemTemplateEquip->m_nKind;	// 지울려고 하는 아템의 kind와 동일...
		stAgcdItemTemplateEquipSet.m_pstBaseClump				= pstAgcdCharacter->m_pClump;
		stAgcdItemTemplateEquipSet.m_pstBaseHierarchy			= pstAgcdCharacter->m_pInHierarchy;

		//. 2006. 1. 13. Nonstopdj
		//. pstAgcdCharacter->m_pClump가 NULL인 경우에도 들어올 수 있으므로 윗부분에 NULL체크 추가. by Crash Report.
		RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, DetachItemCB, (PVOID)(&stAgcdItemTemplateEquipSet));
	}

	// 머리카락과 얼굴처리..
	// 기존 아이템이 얼굴과 헤어가 붙어있는지 확인해야한다.
	OnUpdateHairAndFace( pcsAgpdItem->m_pcsItemTemplate, pstAgcdCharacter, TRUE );
	
	if( !AttachItem( pstAgpdCharacter->m_lTID1, pstAgcdCharacter, pcsAgpdItemTemplateEquip, pstAgcdItem, pstAgcdItemTemplate, eClassType ) )
	{
		// 어떤 이유에서든 실패할 경우.. 디폴트 아이템을 설치한다.
		AttachedAtomics csAttachedAtomics;
		if( pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR	&& pstAgcdCharacterTemplate->m_pDefaultArmourClump )
		{
			if(!EquipDefaultArmour(
				pstAgcdCharacter->m_pClump,
				pstAgcdCharacterTemplate->m_pDefaultArmourClump,
				pstAgcdCharacter->m_pInHierarchy,
				pcsAgpdItemTemplateEquip->GetPartIndex(),
				&csAttachedAtomics ) ) return FALSE;

			AttachedAtomicList* pcsCurrent = csAttachedAtomics.pcsList;
			while (pcsCurrent)
			{
				m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
				pcsCurrent	= pcsCurrent->m_pcsNext;
			}

			RemoveAllAttachedAtomics(&csAttachedAtomics);
		}
	}

	// LOD
	if (m_pcsAgcmLODManager)
	{
		AgcdLODData	*pstCharacterLODData = m_pcsAgcmLODManager->GetLODData(&pstAgcdCharacterTemplate->m_stLOD, 0, FALSE);
		if( pstCharacterLODData && pstCharacterLODData->m_ulMaxLODLevel)
		{
			AgcdLODData *pstItemLODData	= m_pcsAgcmLODManager->GetLODData(&pstAgcdItemTemplate->m_stLOD, 0, FALSE);
			if( pstItemLODData && pstItemLODData->m_ulMaxLODLevel )
			{
				m_pcsAgcmLODManager->SetLODCallback(pstAgcdCharacter->m_pClump);
				AcuObject::SetClumpDInfo_LODLevel( pstAgcdCharacter->m_pClump, pstCharacterLODData->m_ulMaxLODLevel );
			}
		}
	}
	SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);

	EnumCallback(AGCMITEM_CB_ID_EQUIP_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

	if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_HALF)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 77, countEnd);
	else if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_FULL)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 0, countEnd);

	// 아이템 종류에 따라 추가로 처리해야할 것들을 처리한다.
	switch( pcsAgpdItemTemplateEquip->m_nKind )
	{
	case AGPMITEM_EQUIP_KIND_NECKLACE :
		{
			// 목걸이의 경우 망토기원석이 박혀 있는지 검사하여 이를 처리한다.
			OnEquipItemNecklace( pstAgpdCharacter, pcsAgpdItem, pstAgcdItem );
		}
		break;
	};

	// 지지고 볶고 끝났으면 애니메이션 세팅
	SetEquipAnimType( pstAgpdCharacter, pstAgcdCharacter );
	return TRUE;
}

BOOL AgcmItem::_UnEquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem, BOOL bViewHelmet )
{
	AgcdCharacter			*pstAgcdCharacter			= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= pstAgcdCharacter->m_pstAgcdCharacterTemplate;
	AgpdItemTemplateEquip	*pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate);
	AgcdItem				*pstAgcdItem				= GetItemData(pcsAgpdItem);
	AgcdItemTemplate		*pstAgcdItemTemplate		= pstAgcdItem->m_pstAgcdItemTemplate;

	// 드래곤시온종족 서머너 클래스에 대해서는 적용하지 않는다.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	if( pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD && bViewHelmet == FALSE )
		pstAgcdCharacter->m_bEquipHelmet = FALSE;

	//if( !(pstAgcdItem->m_lStatus & AGCDITEM_STATUS_ATTACH ))
	//	return TRUE;

	// Valid 하지 않은 Character로 여기까지 들어오면 안된다.
	// Valid 하지 않다는 것은 다음의 2가지 경우이다.
	// 1. 아직 InitCharacter()가 덜 된 케릭터 (이 경우에는 위의 AGCDITEM_STATUS_ATTACH Flag가 없을 것이므로 위에서 return된다.)
	// 2. Remove된 케릭터 (Module 순서상 Remove Flag가 켜지기 전에 UnEquip 이 된다.)
	//ASSERT(m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter));
	if( !m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter))
	{
		MD_SetErrorMessage("AgcmItem::UnEquipItem() pstAgcdCharacter is Invalid.");
		return TRUE;
	}

	if( !pstAgcdCharacter->m_pClump && !pstAgcdCharacter->m_pClump->atomicList )
	{
		MD_SetErrorMessage("AgcmItem::UnEquipItem() pstAgcdCharacter->m_pClump was removed.");
		return TRUE;
	}

	if( !pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	{
		AgpdGridItem * pGrid = m_pcsAgpmItem->GetEquipItem( pstAgpdCharacter , pcsAgpdItemTemplateEquip->GetAvatarPartIndex() );
		if( pGrid )
			return TRUE;
	}

	RwInt16 countEnd = pstAgcdCharacter->m_pClump->atomicList->stRenderInfo.countEnd;

	EnumCallback(AGCMITEM_CB_ID_UNEQUIP_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

	OnUpdateHairAndFace( pcsAgpdItem->m_pcsItemTemplate, pstAgcdCharacter, FALSE );

	//. 2006. 3. 16. nonstopdj
	m_pcsAgcmCharacter->GeneralizeAttachedAtomicsNormals(pstAgcdCharacter);

	AuCharClassType eCurrentClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &pstAgpdCharacter->m_csFactor );
	DetachItem(pstAgcdCharacter, pcsAgpdItemTemplateEquip, pstAgcdItem, pstAgcdItemTemplate, eCurrentClassType );

	if( pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	{
		// 아바타 장비를 떼는경우
		// 아래에 입은 장비가 다시 장착 돼어야한다.
		AgpdGridItem * pGrid = m_pcsAgpmItem->GetEquipItem( pstAgpdCharacter , pcsAgpdItemTemplateEquip->GetPartIndex() );
		if( pGrid )
		{
			AgpdItem				*pcsUnderWearItem		= m_pcsAgpmItem->GetItem( pGrid );
			AgpdItemTemplateEquip	*pcsUnderWearTemplate	= (AgpdItemTemplateEquip *)(pcsUnderWearItem->m_pcsItemTemplate);

			AgcdItem				*pstAgcdUnderWearItem			= GetItemData(pcsUnderWearItem);
			AgcdItemTemplate		*pstAgcdUnderWearItemTemplate	= pstAgcdUnderWearItem->m_pstAgcdItemTemplate;
			
			if( pcsUnderWearTemplate->m_nPart == AGPMITEM_PART_HEAD && !pstAgcdCharacter->m_bViewHelmet )
			{
				bEquipDefaultItem = FALSE;
				SetItemStatus(pstAgcdUnderWearItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);
			}
			else
			{
				bEquipDefaultItem = FALSE;
				AttachItem( pstAgpdCharacter->m_lTID1, pstAgcdCharacter, pcsUnderWearTemplate, pstAgcdUnderWearItem, pstAgcdUnderWearItemTemplate, eClassType );
				SetItemStatus(pstAgcdUnderWearItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);
			}

			SetEquipAnimType( pstAgpdCharacter, pstAgcdCharacter );
			pstAgcdCharacter->m_bEquipHelmet = TRUE;
		}
	}

	if( bEquipDefaultItem )
	{
		AttachedAtomics csAttachedAtomics;
		if ((pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) && (pstAgcdCharacterTemplate->m_pDefaultArmourClump))
		{
			if(!EquipDefaultArmour(
				pstAgcdCharacter->m_pClump,
				pstAgcdCharacterTemplate->m_pDefaultArmourClump,
				pstAgcdCharacter->m_pInHierarchy,
				pcsAgpdItemTemplateEquip->GetPartIndex(),
				&csAttachedAtomics									)	) return FALSE;

			AttachedAtomicList	*pcsCurrent	= csAttachedAtomics.pcsList;
			while (pcsCurrent)
			{
				m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
				pcsCurrent	= pcsCurrent->m_pcsNext;
			}

			RemoveAllAttachedAtomics(&csAttachedAtomics);
		}
	}

	ResetItemStatus(pstAgcdItem, AGCDITEM_STATUS_ATTACH);

	EnumCallback(AGCMITEM_CB_ID_DETACHED_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgpdCharacter));

	if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_HALF)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 77, countEnd);
	else if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_FULL)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 0, countEnd);

	// 아이템 종류에 따라 추가로 처리해야할 것들을 처리한다.
	switch( pcsAgpdItemTemplateEquip->m_nKind )
	{
	case AGPMITEM_EQUIP_KIND_NECKLACE :
		{
			// 목걸이의 경우 망토기원석이 박혀 있는지 검사하여 이를 처리한다.
			OnUnEquipItemNecklace( pstAgpdCharacter, pcsAgpdItem, pstAgcdItem );
		}
		break;
	};

	// 지지고 볶고 끝났으면 애니메이션 타입 세팅
	SetEquipAnimType( pstAgpdCharacter, pstAgcdCharacter );
	return TRUE;
}

BOOL AgcmItem::_SetEquipAnimType( AgcdCharacter* pcdCharacter, INT32 nWeaponType )
{
	INT32	lAnimType2	= pcdCharacter->m_pstAgcdCharacterTemplate->m_lAnimType2;
	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetCharacter( pcdCharacter );
	BOOL bRidable = ppdCharacter ? ppdCharacter->m_bRidable : FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !ppmCharacter || !pcmCharacter ) return FALSE;

	BOOL bIsPCCharacter = ppmCharacter->IsPC( ppdCharacter );
	if( bIsPCCharacter )
	{
		AuRace race = ppmCharacter->GetCharacterRace( ppdCharacter );

		// 드레곤시온이 아닌 경우 탈것을 탔을때는 마상무기를 착용하고 있는지만 체크한다.
		if( bRidable && race.detail.nRace != AURACE_TYPE_DRAGONSCION )
		{
			switch( nWeaponType )
			{
				// 랜스를 착용한경우
			case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE :
				pcdCharacter->m_lCurAnimType2 = GetAnimWeaponRide( lAnimType2 );
				break;				
				// 다른종류의 마상무기가 있는듯
			case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD :
				pcdCharacter->m_lCurAnimType2 = GetAnimStandardRide( lAnimType2 );
				break;
				// 마상무기가 없는경우
			default:
				pcdCharacter->m_lCurAnimType2 = GetAnimDefaultRide( lAnimType2 );
				break;
			}
		}
		else // 드레곤 시온이거나 탈것을 타지 않은경우 모든 경우를 체크한다.
		{
			AuCharClassType eClassType = pcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :
				{
					switch( nWeaponType )
					{
					case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WARRIR_DEFAULT_RIDE : AGCMCHAR_AT2_WARRIR_DEFAULT;			break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_SLASH;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_AXE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_BLUNT;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SWORD:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_SLASH;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_AXE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_BLUNT;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_POLEARM:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_POLEARM;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SCYTHE:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_SCYTHE;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE:			pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WARRIR_WEAPON_RIDE : pcdCharacter->m_lCurAnimType2;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD:		pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WARRIR_STANDARD_RIDE : pcdCharacter->m_lCurAnimType2;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_RAPIERDAGGER:	pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_BLUNT;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_CHARON;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_CHARON;	break;
					default:												return FALSE;
					}		
				}
				break;

			case AUCHARCLASS_TYPE_RANGER :
				{
					switch( nWeaponType )
					{
					case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE : AGCMCHAR_AT2_ARCHER_DEFAULT;			break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_BOW;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_CROSSBOW;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE:			pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_WEAPON_RIDE : pcdCharacter->m_lCurAnimType2;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_KATARIYA:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_KATARIYA;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD:		pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_STANDARD_RIDE : pcdCharacter->m_lCurAnimType2;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_ONE_HAND_ZENNON;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_ZENNON;	break;
					default:												return FALSE;
					}
				}
				break;

			case AUCHARCLASS_TYPE_MAGE :
				{
					if( race.detail.nRace == AURACE_TYPE_DRAGONSCION )
					{
						// 드레곤 시온 서머너인 경우는 무조건 AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON
						pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON;
					}
					else
					{
						switch( nWeaponType )
						{
						case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WIZARD_DEFAULT_RIDE : AGCMCHAR_AT2_WIZARD_DEFAULT;			break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STAFF:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_STAFF;				break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_TROPHY:
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WAND:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_TROPHY;				break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE:			pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WIZARD_WEAPON_RIDE : pcdCharacter->m_lCurAnimType2;		break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHAKRAM:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_TWO_HAND_CHAKRAM;	break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD:		pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WIZARD_STANDARD_RIDE : pcdCharacter->m_lCurAnimType2;	break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
						case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON;	break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_TWO_HAND_CHARON;	break;
						default:												return FALSE;
						}
					}
				}
				break;

			case AUCHARCLASS_TYPE_SCION :
				{
					switch( nWeaponType )
					{
					case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE : AGCMCHAR_AT2_ARCHER_DEFAULT;			break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_ONE_HAND_ZENNON;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON;	break;
					default:												return FALSE;
					}
				}
				break;

			case AUCHARCLASS_TYPE_MAX :
				{
					switch( lAnimType2 )
					{
					case AGCMCHAR_AT2_BASE:
					case AGCMCHAR_AT2_BOSS_MONSTER:							pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_BASE_DEFAULT;				break;
					case AGCMCHAR_AT2_ARCHLORD :							pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHLORD_DEFAULT;			break;
					default :												return FALSE;
					}
				}
				break;
			}
		}
	}
	else
	{
		switch (lAnimType2)
		{
		case AGCMCHAR_AT2_BASE:
		case AGCMCHAR_AT2_BOSS_MONSTER:									pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_BASE_DEFAULT;				break;
		case AGCMCHAR_AT2_ARCHLORD :									pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHLORD_DEFAULT;			break;
		}
	}

	return TRUE;
}

BOOL AgcmItem::_IsWearAvatarItem( AgpdCharacter* ppdCharacter, BOOL bIsCheckFace, BOOL bIsCheckHair )
{
	// 이 캐릭터가 현재 아바타 아이템을 착용중인가를 검사한다.
	if( !ppdCharacter ) return FALSE;

	// 이 캐릭터가 착용중인 아이템을 다 뒤진다.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	if( !ppmItem || !ppmGrid || !pcmItem ) return FALSE;

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// 해당 그리드의 아이템을 찾는다.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		// 아바타 아이템이면.. 
		if( ppmItem->IsAvatarItem( ppdItemCurrent->m_pcsItemTemplate ) )
		{
			AgcdItemTemplate* pcdItemTemplate = pcmItem->GetTemplateData( ppdItemCurrent->m_pcsItemTemplate );
			if( pcdItemTemplate )
			{
				// bIsCheckFace 값을 체크하여 이 아바타 아이템이 얼굴을 변경시키는지 알아본다.
				if( bIsCheckFace && !pcdItemTemplate->m_bWithFace )
				{
					return TRUE;
				}

				// bIsCheckHair 값을 체크하여 이 아바타 아이템이 얼굴을 변경시키는지 알아본다.
				if( bIsCheckHair && !pcdItemTemplate->m_bWithHair )
				{
					return TRUE;
				}
			}
		}

		// 다음거..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return FALSE;
}

RpAtomic* CallBack_FindAtomicArms( RpAtomic* pAtomic, void* pData )
{
	if( pAtomic )
	{
		RpAtomic** ppTargetAtomic = ( RpAtomic** )pData;
		RpAtomic* pTargetAtomic = *ppTargetAtomic;
		if( pAtomic->iPartID == AGPMITEM_PART_ARMS )
		{
			pTargetAtomic = pAtomic;
			return NULL;
		}
	}

	return pAtomic;
}