#include "AgcmItem.h"
#include "AgcCharacterUtil.h"
#include "RtPITexD.h"
#include "ApModuleStream.h"
#include "AgpmEventQuest.h"
#include "AgcmEventManager.h"
#include "AgpmSystemMessage.h"
#include "AgcmUIItem.h"

void _ReallocCopyString( char** szDst, const char* szSrc )
{
	if( !szSrc )		return;
	if( !szSrc[0] )		return;

	if( *szDst )
		delete [] *szDst;

	int size = strlen( szSrc );
	*szDst = new char [size + 1];
	strcpy( *szDst, szSrc );
	(*szDst)[size] = NULL;
}

//--------------------------- Lua Script -------------------------
LuaGlue	LG_EquipItem( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , -1 );
	INT32	nItemTID	= ( INT32 ) pLua->GetNumberArgument( 2 , -1 );

	if( -1 == nCID )
	{
		LUA_RETURN( FALSE );
	}
	if( -1 == nItemTID )
	{
		LUA_RETURN( FALSE );
	}

	AgpmCharacter	* pcsAgpmCharacter	= ( AgpmCharacter	* ) g_pEngine->GetModule( "AgpmCharacter"	);
	AgpmItem		* pcsAgpmItem		= ( AgpmItem		* ) g_pEngine->GetModule( "AgpmItem"		);

	AgpdCharacter * pCharacter = pcsAgpmCharacter->GetCharacter( nCID );

	if( !pCharacter )
	{
		g_pEngine->LuaErrorMessage( "CID not found" );
		LUA_RETURN( FALSE );
	}

	static INT32	_snItemSequence = 0x0FF00000;
	AgpdItem * pstAgpdItem = pcsAgpmItem->AddItem( _snItemSequence++ , nItemTID , 1);
	if( !pstAgpdItem )
	{
		g_pEngine->LuaErrorMessage( "Item Creation Fail" );
		LUA_RETURN( FALSE );
	}

	/*
	if( !pcsAgpmItem->AddItemToInventory( pCharacter , pstAgpdItem ) )
	{
		// 인벤토리가 꽉 찼나?
		g_pEngine->LuaErrorMessage( "AddItemToInvectory Fail" );
		pcsAgpmItem->DestroyItem( pstAgpdItem );
		LUA_RETURN( FALSE );
	}
	*/

	if(!pcsAgpmItem->InitItem(pstAgpdItem))
	{
		g_pEngine->LuaErrorMessage( "InitItem Error");

		pcsAgpmItem->DestroyItem( pstAgpdItem );
		LUA_RETURN( FALSE );
	}

	// 포인터 강제설정.
	pstAgpdItem->m_pcsCharacter	= pCharacter;

	if( pcsAgpmItem->EquipItem( pCharacter , pstAgpdItem , FALSE) )
	{
		// do nothing..
	}
	else
	{
		g_pEngine->LuaErrorMessage( "Equip Item Fail" );
		LUA_RETURN( FALSE );
	}

	LUA_RETURN( TRUE );
}

LuaGlue	LG_SetAvatarItem( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nItemTID		= ( INT32 ) pLua->GetNumberArgument( 1 , -1 );

	if( -1 == nItemTID )
	{
		LUA_RETURN( FALSE );
	}

	AgpmItem		* pcsAgpmItem		= ( AgpmItem		* ) g_pEngine->GetModule( "AgpmItem"		);

	AgpdItemTemplate *pcsItemTemplate = pcsAgpmItem->GetItemTemplate(nItemTID);

	if( pcsItemTemplate )
	{
		// 템플릿을 강제로 아바타 파츠로 설정한다.
		AgpdItemTemplateEquip * pcsItemTemplateEquip= ( AgpdItemTemplateEquip * )pcsItemTemplate;
		pcsItemTemplateEquip->m_nPart	= pcsItemTemplateEquip->GetAvatarPartIndex();
	}

	LUA_RETURN( TRUE );
}

LuaGlue	LG_UnEquipItem( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , -1 );
	INT32	nPartIndex	= ( INT32 ) pLua->GetNumberArgument( 2 , -1 );
	
	AgpmCharacter	* pcsAgpmCharacter	= ( AgpmCharacter	* ) g_pEngine->GetModule( "AgpmCharacter"	);
	AgpmItem		* pcsAgpmItem		= ( AgpmItem		* ) g_pEngine->GetModule( "AgpmItem"		);

	if( -1 == nCID )
	{
		LUA_RETURN( FALSE );
	}

	AgpdCharacter * pCharacter = pcsAgpmCharacter->GetCharacter( nCID );

	if( !pCharacter )
	{
		g_pEngine->LuaErrorMessage( "CID not found" );
		LUA_RETURN( FALSE );
	}

	if( -1 != nPartIndex && ( nPartIndex <= AGPMITEM_PART_NONE || nPartIndex >= AGPMITEM_PART_NUM ))
	{
		g_pEngine->LuaErrorMessage( "Invalid Part Index" );
		LUA_RETURN( FALSE );
	}

	if( -1 == nPartIndex )
	{
		// 전부 해제
		for( int i = AGPMITEM_PART_BODY ; i < AGPMITEM_PART_NUM ; i ++ )
		{
			AgpdGridItem	* pcsItemGrid	= pcsAgpmItem->GetEquipItem( pCharacter , i );
			AgpdItem		* pcsItem		= pcsItemGrid ? pcsAgpmItem->GetItem( pcsItemGrid ) : NULL;
			if( pcsItem )
			{
				pcsAgpmItem->UnEquipItem( pCharacter , pcsItem );
				pcsAgpmItem->DestroyItem( pcsItem );
			}
		}
	}
	else
	{
		AgpdGridItem	* pcsItemGrid	= pcsAgpmItem->GetEquipItem( pCharacter , nPartIndex );
		AgpdItem		* pcsItem		= pcsItemGrid ? pcsAgpmItem->GetItem( pcsItemGrid ) : NULL;

		if( pcsItem )
		{
			pcsAgpmItem->UnEquipItem( pCharacter , pcsItem );
			pcsAgpmItem->DestroyItem( pcsItem );
		}
	}

	LUA_RETURN( TRUE );
}


LuaGlue	LG_EquipAvatarSet( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , -1 );
	INT32	nItemTID	= ( INT32 ) pLua->GetNumberArgument( 2 , -1 );

	if( -1 == nItemTID || nCID == -1)
	{
		LUA_RETURN( FALSE );
	}

	AgpmItem		* pcsAgpmItem		= ( AgpmItem		* ) g_pEngine->GetModule( "AgpmItem"		);

	pcsAgpmItem->Debug_EquipAvatarSet( nCID , nItemTID );

	LUA_RETURN( TRUE );
}

LuaGlue	LG_UnEquipAvatarSet( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , -1 );
	INT32	nItemTID	= ( INT32 ) pLua->GetNumberArgument( 2 , -1 );

	if( -1 == nItemTID || nCID == -1)
	{
		LUA_RETURN( FALSE );
	}

	AgpmItem		* pcsAgpmItem		= ( AgpmItem		* ) g_pEngine->GetModule( "AgpmItem"		);

	pcsAgpmItem->Debug_UnEquipAvatarSet( nCID , nItemTID );

	LUA_RETURN( TRUE );
}

void	AgcmItem::OnLuaInitialize( AuLua * pLua )
{
	luaDef MVLuaGlue[] = 
	{
		{"EquipItem"		,	LG_EquipItem	},
		{"UnEquipItem"		,	LG_UnEquipItem	},
		{"SetAvatarItem"	,	LG_SetAvatarItem},
		{"EquipAvatarSet"	,	LG_EquipAvatarSet},
		{"UnEquipAvatarSet"	,	LG_UnEquipAvatarSet},
		{NULL				,	NULL			},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}

//--------------------------- AgcmItem -------------------------

#define AGPMITEM_LOTTERY_BOX_CONVERSION_DURATION_MSEC	3000

AgcmItem *		AgcmItem::m_pcsThis = NULL;

CHAR*	g_aszWeaponTypeName[AGPMITEM_EQUIP_WEAPON_TYPE_NUM] = 
{
	"OneHand_Sword",
	"OneHand_Axe",
	"OneHand_Mace",
	"OneHand_Hammer",
	"TwoHand_Sword",
	"TwoHand_Axe",
	"TwoHand_Mace",
	"TwoHand_Hammer",
	"TwoHand_Polearm",
	"TwoHand_Scythe",
	"TwoHand_Bow",
	"TwoHand_Crossbow",
	"TwoHand_Claw",
	"TwoHand_Wing",
	"TwoHand_Staff",
	"OneHand_Trophy",
	"OneHand_Wand",
	"OneHand_Lance",
	"OneHand_Katariya",
	"OneHand_Chakram",
	"OneHand_Standard",
	"OneHand_Rapier",
	"OneHand_Dagger",
	"TwoHand_RapierDagger", // for 문엘프 전사.
	"OneHand_Chanon",
	"TwoHand_Chanon",
	"OneHand_Zenon",
	"TwoHand_Zenon",
};

char* g_aszWeaponKindName[AGPMITEM_PART_V_BODY] = {
	"",
	"None",
	"Body",
	"Head",
	"Arms",
	"Hands",
	"Legs",
	"Foot",
	"Arms2",
	"Hand(Left)",
	"Hand(Right)",
	"Ring1",
	"Ring2",
	"Necklace",
	"Ride",
	"Lancer",
};

#ifdef _DEBUG
DWORD	g_dwThreadID = 0;
#endif

AgcmItem::AgcmItem() 
{
	SetModuleName("AgcmItem");

	m_pcsThis	= this;

	m_pWorld    = NULL;

	m_nItemAttachIndex         = 0;
	m_nItemTemplateAttachIndex = 0;

	m_bUseTexDict = FALSE;

	m_szTexDict[0]	= '\0';
	m_pstTexDict	= NULL;

	m_nMaxItemClump	= 0;

	ZeroMemory(m_szClumpPath, sizeof(CHAR) * AGCMITEM_MAX_PATH_CLUMP);

	m_lLoaderLoadID	= 0;
	m_lLoaderSetID	= 0;

	m_pcsAgpmFactors		= NULL;
	m_pcsAgpmItem			= NULL;
	m_pcsAgpmSkill			= NULL;
	m_pcsAgpmItemConvert	= NULL;
	m_pcsAgcmCharacter		= NULL;
	m_pcsAgpmCharacter		= NULL;
	m_pcsAgcmRender			= NULL;
	m_pcsAgcmLODManager		= NULL;
	m_pcsAgcmShadow			= NULL;
	m_pcsAgcmUIControl		= NULL;
	m_pcsAgpmGrid			= NULL;
	m_pcsAgcmMap			= NULL;
	m_pcsAgcmResourceLoader	= NULL;
	m_pcsApmMap				= NULL;
	m_pAgcmUICooldown		= NULL;
	m_pcsAgpmSystemMessage	= NULL;

	m_ulNextAutoPickupItemTimeMSec	= 0;

	m_szTexturePath[0] = 0;
	m_szIconTexturePath[0] = 0;

	m_ulModuleFlags			= E_AGCM_ITEM_FLAGS_NONE;

	m_lCooldownID			= AGCMUICOOLDOWN_INVALID_ID;

	m_bIsNowWaitingForUseItem = FALSE;
	m_nCoolDownWaitUseItemID = AGCMUICOOLDOWN_INVALID_ID;
	m_nCoolDownWaitUseItemReturnID = AGCMUICOOLDOWN_INVALID_ID;
	m_nWaitingItemID = -1;

#ifdef _DEBUG
	g_dwThreadID	= GetCurrentThreadId();
#endif
}

BOOL AgcmItem::OnInit()
{
	if ( !m_csClump.Initialize(m_nMaxItemClump) )
		return FALSE;

	if( !m_ItemEvolutionTable.OnLoadTable( "INI\\ItemEvolutionTable.txt" ) )
	{
#ifdef _DEBUG
		::MessageBox( NULL, "Cannot load ItemEvolutionTable.txt.", "File Error", MB_OK );
#endif
		return TRUE;
	}

	return TRUE;
}

BOOL AgcmItem::OnAddModule()
{
	// 각종 필요한 모듈의 포인터를 가져온다.
	m_pcsAgpmFactors	= (AgpmFactors*)(GetModule("AgpmFactors"));
	m_pcsAgpmItem		= (AgpmItem*)(GetModule("AgpmItem"));
	m_pcsAgpmCharacter	= (AgpmCharacter *)(GetModule("AgpmCharacter"));
	m_pcsAgcmCharacter	= (AgcmCharacter*)(GetModule("AgcmCharacter"));
	m_pcsAgpmGrid		= (AgpmGrid *)(GetModule("AgpmGrid"));
	m_pcsAgpmSkill		= (AgpmSkill *) GetModule("AgpmSkill");
	m_pcsAgpmItemConvert	= (AgpmItemConvert*)(GetModule("AgpmItemConvert"));
	m_pcsAgcmShadow		= (AgcmShadow *)(GetModule("AgcmShadow"));
	m_pcsAgcmRender		= (AgcmRender *)(GetModule("AgcmRender"));
	m_pcsAgcmUIControl	= (AgcmUIControl *)(GetModule("AgcmUIControl"));
	m_pcsAgcmMap		= (AgcmMap *)(GetModule("AgcmMap"));
	m_pcsAgcmLODManager	= (AgcmLODManager*)GetModule("AgcmLODManager");
	m_pcsAgcmResourceLoader	= (AgcmResourceLoader*)GetModule("AgcmResourceLoader");
	m_pcsApmMap			= (ApmMap *) GetModule("ApmMap");
	m_pcsAgcmShadow2	= (AgcmShadow2*) GetModule("AgcmShadow2");
	m_pcsAgcmRide		= NULL;
	m_pAgcmUICooldown	= (AgcmUICooldown *) GetModule("AgcmUICooldown");
	m_pcsAgpmSystemMessage	= (AgpmSystemMessage*)GetModule("AgpmSystemMessage");

	m_csClump.m_pcsAgcmResourceLoader = m_pcsAgcmResourceLoader;
	if ( !m_pcsAgcmResourceLoader )
		return FALSE;

	// setting attached data in item module
	m_nItemAttachIndex = m_pcsAgpmItem->AttachItemData(this, sizeof(AgcdItem), ItemConstructorCB, ItemDestructorCB);
	if( m_nItemAttachIndex < 0 ) return FALSE;

	m_nItemTemplateAttachIndex = m_pcsAgpmItem->AttachItemTemplateData(this, sizeof(AgcdItemTemplate), ItemTemplateConstructorCB, ItemTemplateDestructorCB);
	if( m_nItemTemplateAttachIndex < 0 ) return FALSE;

	// Callback을 등록한다.
	if(!m_pcsAgpmItem->SetCallbackInit(ItemInitCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackAdd(ItemAddCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackRemove(ItemRemoveCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackEquip(ItemEquipCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUnEquip(ItemUnequipCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackField(ItemFieldCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackInventory(ItemInventoryCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackBank(CBUpdateBank, this )) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackRemoveBank(CBUpdateBank, this)) return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackInitCharacter(InitCharacterCB, this)) return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackPreRemoveCharacter(PreRemoveCharacterCB, this))	return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackEvolution(CBEvolutionAppear, this)) return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRestoreEvolution(CBRestoreEvolution, this)) return FALSE;
	if ( !m_pcsAgpmItem->SetCallbackInventory( CBUpdateInventory, this ) ) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUseItemSuccess(ItemUseSuccessCB, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUnuseItem(CBUnuseItem, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackPauseCashItem(CBPauseItem, this)) return FALSE;
	if ( !m_pcsAgpmItem->SetCallbackUseItemByTID(ItemUseByTIDCB, this)) return FALSE;
	if ( !m_pcsAgpmCharacter->SetCallbackUISetMoney( CBUpdateMoney, this ) ) return FALSE;
	if ( !m_pcsAgpmCharacter->SetCallbackUpdateBankMoney( CBUpdateBankMoney, this ) ) return FALSE;
	if ( !m_pcsAgpmItem->SetCallbackRemoveFromInventory( CBUpdateInventory, this ) ) return FALSE;
	if ( !m_pcsAgcmCharacter->SetCallbackSetSelfCharacter( CBSetSelfCharacter, (PVOID)this ) ) return FALSE;
	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorDurability(CBUpdateFactorDurability, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUpdateFactor(CBUpdateFactor, this)) return FALSE;
	if (!m_pcsAgpmItem->SetCallbackAddItemExtraData(CBAddItemExtraData, this)) return FALSE;
	

	m_pcsAgpmItem->SetCallbackEquip( CBUpdateEquipment, this );
	m_pcsAgpmItem->SetCallbackUnEquip( CBUpdateEquipment, this );
	
	// Stream 콜백 등록
	if(!m_pcsAgpmItem->AddStreamCallback(AGPMITEM_DATA_TYPE_ITEM, ItemStreamReadCB, ItemStreamWriteCB, this)) return FALSE;
	if(!m_pcsAgpmItem->AddStreamCallback(AGPMITEM_DATA_TYPE_TEMPLATE, ItemTemplateStreamReadCB, ItemTemplateStreamWriteCB, this)) return FALSE;

	m_lLoaderLoadID = m_pcsAgcmResourceLoader->RegisterLoader(this, CBLoadTemplate, NULL);
	m_lLoaderSetID = m_pcsAgcmResourceLoader->RegisterLoader(this, CBMakeItemClump, CBSetItem);

	if (m_lLoaderLoadID < 0 || m_lLoaderSetID < 0)
		return FALSE;

	AgcaItemClump::m_fnDefaultTextureReadCB = RwTextureGetReadCallBack();

	SetWorld(GetWorld());
	
	if (m_pAgcmUICooldown)
	{
		m_lCooldownID = m_pAgcmUICooldown->RegisterCooldown(CBAfterCooldown, this);
		if (AGCMUICOOLDOWN_INVALID_ID == m_lCooldownID)
			return FALSE;

		m_nCoolDownWaitUseItemID = m_pAgcmUICooldown->RegisterCooldown( CBAfterCoolDownWaitUseItem, this );
		if( AGCMUICOOLDOWN_INVALID_ID == m_nCoolDownWaitUseItemID ) return FALSE;

		m_nCoolDownWaitUseItemReturnID = m_pAgcmUICooldown->RegisterCooldown( CBAfterCoolDownWaitUseItemReturn, this );
		if( AGCMUICOOLDOWN_INVALID_ID == m_nCoolDownWaitUseItemReturnID ) return FALSE;
	}

	return TRUE;
}

BOOL AgcmItem::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgcmItem::OnDestroy()
{
	return m_csClump.RemoveAllItemClump();
}

AgpdItem* AgcmItem::GetItem(AgcdItem *pstItem)
{
	return (AgpdItem *)(m_pcsAgpmItem->GetParentModuleData( m_nItemAttachIndex, (void *)(pstItem)) );
}

AgcdItem* AgcmItem::GetItemData(AgpdItem *pstItem)
{
	return (AgcdItem *)(m_pcsAgpmItem->GetAttachedModuleData( m_nItemAttachIndex, (void *)(pstItem)) );
}

PVOID AgcmItem::GetTemplate(AgcdItemTemplate *pstTemplate)
{
	return (m_pcsAgpmItem->GetParentModuleData( m_nItemTemplateAttachIndex, (void *)(pstTemplate)) );
}

AgcdItemTemplate* AgcmItem::GetTemplateData(AgpdItemTemplate *pstTemplate)
{
	return (AgcdItemTemplate *)(m_pcsAgpmItem->GetAttachedModuleData( m_nItemTemplateAttachIndex, (void *)(pstTemplate)) );
}

BOOL AgcmItem::EquipDefaultArmour(RpClump *pstCharClump, RpClump *pstArmorClump, RpHAnimHierarchy *pstBaseHierarchy, INT16 nPartID, AttachedAtomics *pcsAttachedAtomics)
{
	AgcdItemTemplateEquipSet stAgcdItemTemplateEquipSet;
	memset( &stAgcdItemTemplateEquipSet, 0, sizeof(AgcdItemTemplateEquipSet) );
	stAgcdItemTemplateEquipSet.m_pvClass			= this;
	stAgcdItemTemplateEquipSet.m_nCBCount			= 0;
	stAgcdItemTemplateEquipSet.m_nPart				= nPartID;
	stAgcdItemTemplateEquipSet.m_nKind				= AGPMITEM_EQUIP_KIND_ARMOUR;
	stAgcdItemTemplateEquipSet.m_pstBaseClump		= pstCharClump;
	stAgcdItemTemplateEquipSet.m_pstBaseHierarchy	= pstBaseHierarchy;
	stAgcdItemTemplateEquipSet.m_pcsAttachedAtomics	= pcsAttachedAtomics;
	RpClumpForAllAtomics( pstArmorClump, nPartID ? AttachItemCheckPartIDCB : AttachItemCB, (PVOID)(&stAgcdItemTemplateEquipSet) );

	return TRUE;
}

BOOL AgcmItem::InitCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmItem				*pThis						= (AgcmItem *)(pClass);	
	AgcdCharacter			*pstAgcdCharacter			= (AgcdCharacter *)(pCustData);

	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= pstAgcdCharacter->m_pstAgcdCharacterTemplate;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate	= pThis->m_pcsAgcmCharacter->GetTemplate(pstAgcdCharacterTemplate);

	AttachedAtomics			csAttachedAtomics;
	AttachedAtomicList		*pcsList;
	
	if (pstAgcdCharacter->m_pClump && pstAgcdCharacterTemplate->m_pDefaultArmourClump)
	{
		RpHAnimHierarchy* pHierarchy = (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY) ? (pstAgcdCharacter->m_pInHierarchy) : (NULL);
		if( !pThis->EquipDefaultArmour( pstAgcdCharacter->m_pClump, pstAgcdCharacterTemplate->m_pDefaultArmourClump, pHierarchy, 0, &csAttachedAtomics ) )
		{
			ASSERT(!"AgcmItem::InitCharacterCB() EquipDefaultArmour Error!!!");
			return FALSE;
		}

		pcsList	= csAttachedAtomics.pcsList;
		while (pcsList)
		{
			pThis->m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsList->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
			pcsList = pcsList->m_pcsNext;
		}

		pThis->RemoveAllAttachedAtomics(&csAttachedAtomics);

		if(pThis->m_pcsAgcmShadow)
		{
			pThis->m_pcsAgcmShadow->UpdateBoundingSphere(pstAgcdCharacter->m_pClump);
		}

		if (pstAgcdCharacterTemplate->m_pcsPreData)
		{
			if (pstAgcdCharacterTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
				AcuObject::SetClumpPreLitLim(pstAgcdCharacter->m_pClump, &pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight);
		}

		if (pThis->m_pcsAgcmLODManager)
		{
			AgcdLODData			*pstLODData = pThis->m_pcsAgcmLODManager->GetLODData(&pstAgcdCharacterTemplate->m_stLOD, 0, FALSE);
			if((pstLODData) && (pstLODData->m_ulMaxLODLevel))
			{
				pThis->m_pcsAgcmLODManager->SetLODCallback(pstAgcdCharacter->m_pClump);
				AcuObject::SetClumpDInfo_LODLevel(pstAgcdCharacter->m_pClump, pstLODData->m_ulMaxLODLevel);
			}
		}

		pThis->EnumCallback(AGCMITEM_CB_ID_EQUIP_DEFAULT_ARMOUR, (PVOID)(pstAgpdCharacterTemplate), (PVOID)(pThis->GetAgcmCharacter()->GetClumpPath()));

		pstAgcdCharacter->m_lStatus |= AGCMCHAR_STATUS_FLAG_EQUIP_DEFAULT_ARMOUR;
	}

	return TRUE;
}

BOOL AgcmItem::PreRemoveCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	return ((AgcmItem *)(pClass))->ReleaseEquipItems((AgpdCharacter *)(pData));
}

BOOL AgcmItem::ItemInitCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::ItemInitCB");

	if((!pClass) || (!pData))
		return FALSE;

	AgcmItem *	pThis       = (AgcmItem *)(pClass);
	AgpdItem *	pcsAgpdItem = (AgpdItem *)(pData);
	AgcdItem *	pstAgcdItem	= pThis->GetItemData(pcsAgpdItem);

	if(!pcsAgpdItem->m_pcsItemTemplate)
	{
		ASSERT(!"AgcmItem::ItemInitCB() No Template Error!!!");

		return FALSE;
	}

	pstAgcdItem->m_pstAgcdItemTemplate			= pThis->GetTemplateData((AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate));
	pstAgcdItem->m_pvClass						= (PVOID)(pThis);

	ASSERT(pstAgcdItem->m_pstAgcdItemTemplate->m_lRefCount >= 0);
	++(pstAgcdItem->m_pstAgcdItemTemplate->m_lRefCount);

	ASSERT(pcsAgpdItem->m_eType == APBASE_TYPE_ITEM);

	TRACE("AgcmItem::ItemAddCB() AddLoadEntry %d, %x, %x", pThis->m_lLoaderLoadID, pcsAgpdItem, 0);

	if (pstAgcdItem->m_pstAgcdItemTemplate->m_bLoaded && pstAgcdItem->m_pstAgcdItemTemplate->m_lRefCount >= 2)
	{
		pThis->SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_LOADED);
		pThis->SetGridItemAttachedTexture(pcsAgpdItem);
		return TRUE;
	}

	// 일단 무조건 Loading한다.
	BOOL	bForceImmediate = FALSE;

	// 기본적으로 이미 Loading이 되어있는 ItemTemplate은 ForeThread에서 LoadTemplateData를 한다.
	// 다른 케릭터의 Equip Item 이거나 Field에 떨어진 Item은 Back Thread로 처리한다.

	//. 2006. 2. 14. 종석이형
	//. CID가 0이거나 자기자신이면 main thread에서 처리하고 나머지는 모두 Back Thread에서 처리
	if (pstAgcdItem->m_pstAgcdItemTemplate->m_bLoaded)
		bForceImmediate	= TRUE;
	else if (pcsAgpdItem->m_ulCID == pThis->m_pcsAgcmCharacter->GetSelfCID() || pcsAgpdItem->m_ulCID == AP_INVALID_IID)
		bForceImmediate = TRUE;
	else if (pcsAgpdItem->m_eNewStatus == AGPDITEM_STATUS_TRADE_GRID ||
		pcsAgpdItem->m_eNewStatus == AGPDITEM_STATUS_CLIENT_TRADE_GRID ||
		pcsAgpdItem->m_eNewStatus == AGPDITEM_STATUS_BANK ||
		pcsAgpdItem->m_eNewStatus == AGPDITEM_STATUS_NPC_TRADE ||
		pcsAgpdItem->m_eNewStatus == AGPDITEM_STATUS_CASH_MALL)
		bForceImmediate = TRUE;
	else
		bForceImmediate = FALSE;

	return pThis->m_pcsAgcmResourceLoader->AddLoadEntry(pThis->m_lLoaderLoadID, pcsAgpdItem, NULL, bForceImmediate);
}

BOOL AgcmItem::ItemAddCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData))
		return FALSE;
	
	return TRUE;
}

BOOL AgcmItem::ItemRemoveCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::ItemRemoveCB");

	if((!pClass) || (!pData))
		return FALSE;

	AgcmItem			*pThis					= (AgcmItem *)(pClass);
	AgpdItem			*pcsAgpdItem			= (AgpdItem *)(pData);

	TRACE("AgcmItem::ItemRemoveCB() Item Remove %d (%s)\n", pcsAgpdItem->m_lID, ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_szName);

	if(!pcsAgpdItem->m_pcsItemTemplate)
	{
		ASSERT(!"AgcmItem::ItemRemoveCB() No Template Error!!!");

		return FALSE;
	}

	AgcdItem			*pstAgcdItem			= pThis->GetItemData(pcsAgpdItem);
	AgcdItemTemplate	*pstAgcdItemTemplate	= pstAgcdItem->m_pstAgcdItemTemplate;
	if((!pstAgcdItem) || (!pstAgcdItemTemplate))
		return FALSE;

	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderLoadID, pcsAgpdItem, NULL);
	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);

	pThis->SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_REMOVED);

	pThis->EnumCallback(AGCMITEM_CB_ID_PRE_REMOVE_DATA, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

	if (pstAgcdItem->m_lStatus & AGCDITEM_STATUS_WORLD_ADDED)
	{
		if (!pThis->RemoveClumpFromWorld(pstAgcdItem->m_pstClump)) return FALSE;
	}

	return pThis->ReleaseTemplateData((AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate), pstAgcdItemTemplate);
}

BOOL AgcmItem::ItemConstructorCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData))
		return FALSE;

	AgcmItem	*pcsThis		= (AgcmItem *)(pClass);
	AgpdItem	*pcsAgpdItem	= (AgpdItem *)(pData);

	AgcdItem	*pstAgcdItem	= pcsThis->GetItemData(pcsAgpdItem);
	if(!pstAgcdItem)
		return FALSE;

	memset(pstAgcdItem, 0, sizeof(AgcdItem));

	pstAgcdItem->m_lStatus			= AGCDITEM_STATUS_INIT;
	pstAgcdItem->m_ulLastUseTime	= 0;

	pstAgcdItem->m_rwLock.Initialize();

	return TRUE;
}

BOOL AgcmItem::ItemDestructorCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmItem	*pcsThis		= (AgcmItem *)(pClass);
	AgpdItem	*pcsAgpdItem	= (AgpdItem *)(pData);

	TRACE("AgcmItem::ItemDestructorCB() Item Destroy %d (%s)\n", pcsAgpdItem->m_lID, ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_szName);

	return pcsThis->ReleaseItemData(pcsAgpdItem);
}

BOOL AgcmItem::ItemTemplateConstructorCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmItem                       *pThis			= (AgcmItem *)(pClass);
	AgpdItemTemplate	  *pcsAgpdItemTemplate		= (AgpdItemTemplate *)(pData);
	AgcdItemTemplate	  *pstAgcdItemTemplate		= pThis->GetTemplateData(pcsAgpdItemTemplate);

	memset(pstAgcdItemTemplate, 0, sizeof (AgcdItemTemplate));
	pstAgcdItemTemplate->m_pItemTransformInfo = new CItemTransformInfo;

	if (!(pThis->m_ulModuleFlags & E_AGCM_ITEM_FLAGS_EXPORT))
		pstAgcdItemTemplate->m_pcsPreData			= new AgcdPreItemTemplate();

	return TRUE;
}

BOOL AgcmItem::DeleteTemplatePreData(AgcdItemTemplate *pstAgcdItemTemplate)
{
	if (!pstAgcdItemTemplate->m_pcsPreData)
		return TRUE; // skip
	
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszLabel );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName );
	DEF_SAFEDELETEARRAY( pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName );
	
	delete pstAgcdItemTemplate->m_pcsPreData;
	pstAgcdItemTemplate->m_pcsPreData	= NULL;

	return TRUE;
}

BOOL AgcmItem::ItemTemplateDestructorCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmItem				*pThis						= (AgcmItem *)(pClass);
	AgpdItemTemplate		*pcsAgpdItemTemplate		= (AgpdItemTemplate *)(pData);
	AgcdItemTemplate		*pstAgcdItemTemplate		= pThis->GetTemplateData(pcsAgpdItemTemplate);
	AgpdItemTemplateEquip	*pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)(pThis->GetTemplate(pstAgcdItemTemplate));

	pThis->m_pcsAgcmRender->m_csRenderType.DeleteClumpRenderType(&pstAgcdItemTemplate->m_csClumpRenderType);

	DEF_SAFEDELETE( pstAgcdItemTemplate->m_pItemTransformInfo );

	if(pstAgcdItemTemplate->m_pstFieldClumpTransform)
	{
		if(!RwMatrixDestroy(pstAgcdItemTemplate->m_pstFieldClumpTransform)) return FALSE;
	}

	if(pstAgcdItemTemplate->m_stLOD.m_pstList)
	{
		if (pThis->m_pcsAgcmLODManager)
			pThis->m_pcsAgcmLODManager->m_csLODList.RemoveAllLODData(&pstAgcdItemTemplate->m_stLOD);

		pstAgcdItemTemplate->m_stLOD.m_pstList = NULL;
	}

	if ( pstAgcdItemTemplate->m_pTexture )
	{
		RwTextureDestroy( pstAgcdItemTemplate->m_pTexture );
		pstAgcdItemTemplate->m_pTexture = NULL;
	}
	if (pstAgcdItemTemplate->m_pDurabilityZeroTexture)
	{
		RwTextureDestroy(pstAgcdItemTemplate->m_pDurabilityZeroTexture);
		pstAgcdItemTemplate->m_pDurabilityZeroTexture = NULL;
	}
	if (pstAgcdItemTemplate->m_pDurability5UnderTexture)
	{
		RwTextureDestroy(pstAgcdItemTemplate->m_pDurability5UnderTexture);
		pstAgcdItemTemplate->m_pDurability5UnderTexture = NULL;
	}
	if ( pstAgcdItemTemplate->m_pSmallTexture )
	{
		RwTextureDestroy( pstAgcdItemTemplate->m_pSmallTexture );
		 pstAgcdItemTemplate->m_pSmallTexture = NULL;
	}

	pThis->DeleteTemplatePreData(pstAgcdItemTemplate);

	return TRUE;
}

BOOL AgcmItem::ItemStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	return TRUE;
}

BOOL AgcmItem::ItemStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	return TRUE;
}

BOOL AgcmItem::ParseValueFieldTransform(AgcdItemTemplate *pItem, ApModuleStream *pcsStream)
{
	if( pItem->m_pstFieldClumpTransform )		return FALSE;

	pItem->m_pstFieldClumpTransform = RwMatrixCreate();
	if( !pItem->m_pstFieldClumpTransform )		return FALSE;

	char value_buffer[AGCMITEM_MAX_PATH_VALUE_BUFFER];
	pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);
	sscanf(value_buffer, "%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",	&pItem->m_pstFieldClumpTransform->right.x,
																&pItem->m_pstFieldClumpTransform->right.y,
																&pItem->m_pstFieldClumpTransform->right.z,
																&pItem->m_pstFieldClumpTransform->up.x,
																&pItem->m_pstFieldClumpTransform->up.y,
																&pItem->m_pstFieldClumpTransform->up.z,
																&pItem->m_pstFieldClumpTransform->at.x,
																&pItem->m_pstFieldClumpTransform->at.y,
																&pItem->m_pstFieldClumpTransform->at.z,
																&pItem->m_pstFieldClumpTransform->pos.x,
																&pItem->m_pstFieldClumpTransform->pos.y,
																&pItem->m_pstFieldClumpTransform->pos.z		);

	return TRUE;
}

BOOL AgcmItem::ItemFieldCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::ItemFieldCB");

	if((!pClass) || (!pData))
		return FALSE;

	AgcmItem			*pThis					= (AgcmItem *)(pClass);
	AgpdItem			*pcsAgpdItem			= (AgpdItem *)(pData);
	AgcdItem			*pstAgcdItem			= pThis->GetItemData(pcsAgpdItem);

	pThis->ReleaseItemData(pcsAgpdItem);

	BOOL bForceImmediate = pThis->IsItemReady( pcsAgpdItem );
	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry( pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL );		// 해당 Item에 대한 다른 작업을 지운다.

	return pThis->m_pcsAgcmResourceLoader->AddLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL, bForceImmediate);
}

BOOL AgcmItem::SetFieldItem(AgpdItem *pcsAgpdItem)
{
	PROFILE("AgcmItem::SetItemField");

	AgcdItem			*pstAgcdItem			= GetItemData(pcsAgpdItem);
	AgcdItemTemplate	*pstAgcdItemTemplate	= pstAgcdItem->m_pstAgcdItemTemplate;

	if ((!pstAgcdItem) || (!pstAgcdItemTemplate)) return FALSE;
	if (!pstAgcdItem->m_pstClump) return FALSE;
	if (!RpClumpGetFrame(pstAgcdItem->m_pstClump)) return FALSE;

	if (m_pcsAgcmMap)
	{
		pcsAgpdItem->m_posItem.y = m_pcsAgcmMap->GetHeight( pcsAgpdItem->m_posItem.x , pcsAgpdItem->m_posItem.z , pcsAgpdItem->m_posItem.y );
	}

	LockFrame();

	RwFrameTranslate(RpClumpGetFrame(pstAgcdItem->m_pstClump), (RwV3d *)(&pcsAgpdItem->m_posItem), rwCOMBINEREPLACE);

	UnlockFrame();

	BOOL bBillboard = FALSE;
	if (!AddClumpToWorld(pstAgcdItem->m_pstClump, bBillboard)) return FALSE;

	SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_WORLD_ADDED);

	EnumCallback(AGCMITEM_CB_ID_FIELD_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

	return TRUE;
}

BOOL AgcmItem::ItemUseSuccessCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmItem		*pThis			= (AgcmItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	AgpdItem		*pcsItem		= (AgpdItem *)		pCustData;

	return pThis->ReceiveUseItem(pcsItem);		//	원래 사용되지 않았으나, 캐쉬아이템을 위해 사용한다.	By SungHoon
}

BOOL AgcmItem::ItemUseByTIDCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmItem			*pThis				= (AgcmItem *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)	pData;

	if (pcsCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return TRUE;

	PVOID				*ppvBuffer			= (PVOID *)			pCustData;

	AgpdItemTemplate	*pcsItemTemplate	= pThis->m_pcsAgpmItem->GetItemTemplate((INT32) ppvBuffer[0]);
	if (!pcsItemTemplate)
	{
		ASSERT(!"AgcmItem::ItemUseByTIDCB() No Template Error!!!");

		return FALSE;
	}

	// 현재 사용되는 아템은 물약 뿐이다. 나중에 스킬북이나 이런넘들 추가되면 그때 차례로 하나씩 추가한다.
	switch (pcsItemTemplate->m_nType) {
	case AGPMITEM_TYPE_USABLE:
		{
			AgpdItemTemplateUsable	*pcsTemplateUsable = (AgpdItemTemplateUsable *) pcsItemTemplate;

			switch (pcsTemplateUsable->m_nUsableItemType) {
			case AGPMITEM_USABLE_TYPE_POTION:
				{
					AgpdItemADChar	*pcsAgpdItemADChar	= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);

					if ((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT] &&
						((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT])->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] > 0)
					{
						pcsAgpdItemADChar->m_ulUseHPPotionTime = pThis->GetClockCount();
					}
					else if ((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT] &&
						((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT])->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP] > 0)
					{
						pcsAgpdItemADChar->m_ulUseMPPotionTime = pThis->GetClockCount();
					}
					else if ((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT] &&
						((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT])->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP] > 0)
					{
						pcsAgpdItemADChar->m_ulUseSPPotionTime = pThis->GetClockCount();
					}
				}
				break;

			case AGPMITEM_USABLE_TYPE_SKILL_SCROLL:
				{
					AgpdItemADChar	*pcsItemADChar	= pThis->m_pcsAgpmItem->GetADCharacter(pcsCharacter);
					if (!pcsItemADChar)
						return NULL;

					for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
					{
						for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
						{
							for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
							{
								AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j);
								if (!pcsGridItem)
									continue;

								AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pcsGridItem->m_lItemID);
								if (pcsItem && pcsItem->m_pcsItemTemplate &&
									pcsItemTemplate->m_lID == ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID)
								{
									AgcdItem	*pcsAgcdItem	= pThis->GetItemData(pcsItem);
									if (pcsAgcdItem)
									{
										pcsAgcdItem->m_ulLastUseTime	= pThis->GetClockCount();
									}
								}
							}
						}
					}
				}
				break;
			}
		}
		break;
	}

	return TRUE;
}

AgpdItem *AgcmItem::FindCharacterDefaultItem(INT32 lCID, INT32 lTID)
{
	INT32 lIndex = 0;
	for(AgpdItem *pcsAgpdItem = m_pcsAgpmItem->GetItemSequence(&lIndex); pcsAgpdItem; pcsAgpdItem = m_pcsAgpmItem->GetItemSequence(&lIndex))
		if( pcsAgpdItem->m_ulCID == lCID && pcsAgpdItem->m_lTID == lTID )
			return pcsAgpdItem;

	return NULL;
}

BOOL AgcmItem::AddClumpToWorld(RpClump *pstClump, BOOL bBillboard)
{
	if ( !m_pWorld || !pstClump )		return FALSE;
		

	TRACE("AgcmItem::ClumpAddWorld() Add Item Clump %x\n", pstClump);

#ifdef _DEBUG
	if (g_dwThreadID != GetCurrentThreadId())
		ASSERT(!"No proper thread process!!!");
#endif

	if (m_pcsAgcmRender)	m_pcsAgcmRender->AddClumpToWorld(pstClump);
	else					RpWorldAddClump(m_pWorld, pstClump);

	return TRUE;
}

BOOL AgcmItem::RemoveClumpFromWorld(RpClump *pstClump)
{
	if( !pstClump )		return FALSE;

#ifdef _DEBUG
	if (g_dwThreadID != GetCurrentThreadId())
		ASSERT(!"No proper thread process!!!");
#endif

	if (pstClump->ulFlag & RWFLAG_RENDER_ADD)
	{
		if (m_pcsAgcmRender)	m_pcsAgcmRender->RemoveClumpFromWorld(pstClump);
		else if (m_pWorld)		RpWorldRemoveClump(m_pWorld, pstClump);
	}

	return TRUE;
}

RpClump *AgcmItem::GetClumpAdminData(CHAR *szFile, BOOL bMake)
{
	CHAR	szFullPath[AGCMCHAR_MAX_PATH];
	sprintf(szFullPath, "%s%s", m_szClumpPath, szFile);
	
	RpClump* pstClump = m_csClump.GetItemClump(szFullPath);
	if( !pstClump && bMake )
		pstClump = m_csClump.AddItemClump(szFullPath);

	return pstClump;
}

BOOL AgcmItem::RemoveClumpAdminData(CHAR *szName)
{
	CHAR	szFullPath[AGCMCHAR_MAX_PATH];
	sprintf(szFullPath, "%s%s", m_szClumpPath, szName);

	return m_csClump.RemoveItemClump(szFullPath);
}

RwTexture	*AgcmItem::GetTexture(CHAR *szFile)
{
	#ifdef _DEBUG
	if (g_dwThreadID != GetCurrentThreadId())
		OutputDebugString("AgcmItem::GetTexture() - No proper thread process!!! Item Texture stream load from main thread.");
	#endif

	RwTexture*	pTexture = 	m_pcsAgcmResourceLoader->LoadTexture(szFile, NULL, NULL, NULL, -1, m_szIconTexturePath);
	if( !pTexture )		return NULL;

	RwTextureSetFilterMode( pTexture, rwFILTERNEAREST );
	RwTextureSetAddressing( pTexture, rwTEXTUREADDRESSCLAMP );
	return pTexture;
}

BOOL AgcmItem::DropItem(AgpdItem *pcsItem)
{
	return TRUE;

	if (!pcsItem)
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	pcsItem->m_posItem = pcsSelfCharacter->m_stPos;

	INT32	lRandomX	= m_csRandom.randInt(100);
	INT32	lRandomZ	= m_csRandom.randInt(100);

	if (lRandomX > 50)
		pcsItem->m_posItem.x	+= lRandomX;
	else
		pcsItem->m_posItem.x	-= (lRandomX + 50);

	if (lRandomZ > 50)
		pcsItem->m_posItem.z	+= lRandomZ;
	else
		pcsItem->m_posItem.z	-= (lRandomZ + 50);

	return SendFieldInfo(pcsItem->m_lID, pcsSelfCharacter->m_lID, &pcsItem->m_posItem);
}

BOOL AgcmItem::PickupItem(AgpdItem *pcsItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsItem)				return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter )		return FALSE;
	if( m_pcsAgpmCharacter->IsActionBlockCondition(pcsSelfCharacter) )	return FALSE;
		
	
	return SendPickupItem(pcsItem->m_lID, pcsSelfCharacter->m_lID);
}

BOOL AgcmItem::AutoPickupItem()
{
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter )		return FALSE;
	if( m_pcsAgpmCharacter->IsActionBlockCondition( pcsSelfCharacter ) )	return FALSE;
	if( m_ulNextAutoPickupItemTimeMSec > m_pcsAgcmCharacter->m_ulCurTick )	return TRUE;		// 이전에 처리했던 시간을 체크한다.
		

	m_ulNextAutoPickupItemTimeMSec	= m_pcsAgcmCharacter->m_ulCurTick + AGCDITEM_AUTO_LOOTING_INTERVAL_TIME;

	// 현재 위치에서 가장 가까운 거리에 있는 아템을 가져와서 집어든다.

	// 일단 맵에서 아템 리스트를 가져온다.
	INT32	lItemList[64];
	ZeroMemory( lItemList, sizeof(INT32) * 64 );

	INT32 lItemCount	= m_pcsApmMap->GetItemList( pcsSelfCharacter->m_nDimension , pcsSelfCharacter->m_stPos, AGCDITEM_MAX_AUTO_LOOTING_RANGE, lItemList, 64);

	// 젤 가까운 아템을 가져온다.
	AgpdItem* pcsNearestItem = NULL;

	FLOAT fNearestItemDistance	= (FLOAT) AGCDITEM_MAX_AUTO_LOOTING_RANGE;

	for (int i = 0; i < lItemCount; ++i)
	{
		AgpdItem* pcsItem = m_pcsAgpmItem->GetItem(lItemList[i]);
		if (pcsItem)
		{
			FLOAT	fx = pcsSelfCharacter->m_stPos.x - pcsItem->m_posItem.x;
			FLOAT	fz = pcsSelfCharacter->m_stPos.z - pcsItem->m_posItem.z;

			FLOAT	fDistance = (FLOAT) sqrt(fx * fx + fz * fz);
			if (fDistance < fNearestItemDistance)
			{
				fNearestItemDistance	= fDistance;
				pcsNearestItem			= pcsItem;
			}
		}
	}

	return pcsNearestItem ? PickupItem(pcsNearestItem) : TRUE;
}

BOOL AgcmItem::SendInventoryInfo(INT32 lIID, INT32 lCID, INT16 nInvIdx, INT16 nInvRow, INT16 nInvCol)
{
	//-----------------------------------------------------------------------
	// 2010.01.06 kdi
	// 드래곤 시온의 장착해제 불가 아이템을 비정상적으로 해제하여 스텟을 어뷰징하는 버그를 클라이언트에서 차단합니다.
	if( IsDisableUnEquipItem( m_pcsAgpmItem->GetItem( lIID ) ) ) 
		return FALSE;
	//-----------------------------------------------------------------------

	// 서버에게 (장착 슬롯에 있던)아이템이 인벤토리의 어느 위치에 옮겨졌는지 알린다.	
	PVOID		pvPacket;
	PVOID       pvInventoryPacket;
	INT16		nSize;
	INT8		cOperation = AGPMITEM_PACKET_OPERATION_UPDATE;
	INT8        cStatus    = AGPDITEM_STATUS_INVENTORY;

	// 상태를 인벤토리에 추가로 바꾼다.
	pvInventoryPacket = m_pcsAgpmItem->m_csPacketInventory.MakePacket(FALSE, &nSize, AGPMITEM_PACKET_TYPE, 
																	 nInvIdx == -1 ? NULL : &nInvIdx, // inventory tab
																	 nInvIdx == -1 ? NULL : &nInvRow, // inventory row
																	 nInvIdx == -1 ? NULL : &nInvCol  // inventory column
																	  );

	if(!pvInventoryPacket) return FALSE;

	pvPacket          = m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nSize, AGPMITEM_PACKET_TYPE, 
		                                                    &cOperation,			// operation
															&cStatus,				// status
															&lIID,					// item i.d.
															 NULL,					// item template i.d.
															 &lCID,					// Item Owner ID
															 NULL,					// Item Count
															 NULL,					// field(packet)
															 pvInventoryPacket,	// inventory(packet)
															 NULL,					//Bank
															 NULL,					// equip(packet)
															 NULL,					// factor
															 NULL,
															 NULL,
															 NULL,
															 NULL,
															 NULL,
															 NULL,					// Quest
															 NULL,
															 NULL,
															 NULL,
															 NULL,
															 NULL,					// Skill Plus
															 NULL,
															 NULL,					// CashInformation
															 NULL
															 );

	m_pcsAgpmItem->m_csPacketInventory.FreePacket(pvInventoryPacket);
	if(!pvPacket) return FALSE;

	if(!SendPacket(pvPacket, nSize))
	{ // error
		m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmItem::SendSubInventoryInfo(INT32 lIID, INT32 lCID, INT16 nInvIdx, INT16 nInvRow, INT16 nInvCol)
{
	//-----------------------------------------------------------------------
	// 2010.01.06 kdi
	// 드래곤 시온의 장착해제 불가 아이템을 비정상적으로 해제하여 스텟을 어뷰징하는 버그를 클라이언트에서 차단합니다.
	if( IsDisableUnEquipItem( m_pcsAgpmItem->GetItem( lIID ) ) ) 
		return FALSE;
	//-----------------------------------------------------------------------

	// 서버에게 (장착 슬롯에 있던)아이템이 인벤토리의 어느 위치에 옮겨졌는지 알린다.	
	PVOID		pvPacket;
	PVOID       pvInventoryPacket;
	INT16		nSize;
	INT8		cOperation = AGPMITEM_PACKET_OPERATION_UPDATE;
	INT8        cStatus    = AGPDITEM_STATUS_SUB_INVENTORY;

	// 상태를 인벤토리에 추가로 바꾼다.
	pvInventoryPacket = m_pcsAgpmItem->m_csPacketInventory.MakePacket(FALSE, &nSize, AGPMITEM_PACKET_TYPE, 
																	 nInvIdx == -1 ? NULL : &nInvIdx, // inventory tab
																	 nInvIdx == -1 ? NULL : &nInvRow, // inventory row
																	 nInvIdx == -1 ? NULL : &nInvCol  // inventory column
																	  );

	if(!pvInventoryPacket) return FALSE;

	pvPacket          = m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nSize, AGPMITEM_PACKET_TYPE, 
		                                                    &cOperation,			// operation
															&cStatus,				// status
															&lIID,					// item i.d.
															 NULL,					// item template i.d.
															 &lCID,					// Item Owner ID
															 NULL,					// Item Count
															 NULL,					// field(packet)
															 pvInventoryPacket,	// inventory(packet)
															 NULL,					//Bank
															 NULL,					// equip(packet)
															 NULL,					// factor
															 NULL,
															 NULL,
															 NULL,
															 NULL,
															 NULL,
															 NULL,					// Quest
															 NULL,
															 NULL,
															 NULL,
															 NULL,
															 NULL,					// Skill Plus
															 NULL,
															 NULL,					// CashInformation
															 NULL
															 );

	m_pcsAgpmItem->m_csPacketInventory.FreePacket(pvInventoryPacket);
	if(!pvPacket) return FALSE;

	if(!SendPacket(pvPacket, nSize))
	{
		m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 100102. Bob Jung
******************************************************************************/
BOOL AgcmItem::SendEquipInfo(INT32 lIID, INT32 lCID)
{
	// 서버에게 (인벤토리에 잇던)아이템이 장착 슬롯으로 옮겨졌다고 알린다.
	PVOID		pvPacket;
	PVOID       pvEquipPacket;
	INT16		nSize;
	INT8		cOperation = AGPMITEM_PACKET_OPERATION_UPDATE;
 	INT8        cStatus    = AGPDITEM_STATUS_EQUIP;

	pvEquipPacket = m_pcsAgpmItem->m_csPacketEquip.MakePacket(FALSE, &nSize, AGPMITEM_PACKET_TYPE
		                                                     );

	if(!pvEquipPacket) return FALSE;

	pvPacket = m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nSize, AGPMITEM_PACKET_TYPE, 
		                                           &cOperation,                                        // operation
												   &cStatus,											// status
												   &lIID,												// item i.d.
												    NULL,												// item template i.d.
													&lCID,												// Item Owner ID
													NULL,												// Item Count
													NULL,												// field(packet)
													NULL,												// inventory(packet)
													NULL,												// bank
													pvEquipPacket,										// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL,												// CashInformation
													NULL
													);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvEquipPacket);
	if(!pvPacket) return FALSE;

	if(!SendPacket(pvPacket, nSize))
	{ // error
		m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmItem::SendBankInfo(INT32 lIID, INT32 lCID, INT16 lLayer, INT16 lRow, INT16 lColumn)
{
	if (lIID == AP_INVALID_IID &&
		lCID == AP_INVALID_CID)
		return FALSE;

	INT16	nPacketLength = 0;

	PVOID	pvPacketBank = m_pcsAgpmItem->m_csPacketBank.MakePacket(FALSE, &nPacketLength, 0,
													&lLayer,
													&lRow,
													&lColumn);

	if (!pvPacketBank)
		return FALSE;

	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_UPDATE;
 	INT8    cStatus		= AGPDITEM_STATUS_BANK;

	PVOID	pvPacket	= m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
		                                           &cOperation,                                        // operation
												   &cStatus,											// status
												   &lIID,												// item i.d.
												    NULL,												// item template i.d.
													&lCID,												// Item Owner ID
													NULL,												// Item Count
													NULL,												// field(packet)
													NULL,												// inventory(packet)
													pvPacketBank,										// bank
													NULL,												// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL,												// CashInformation
													NULL
													);

	m_pcsAgpmItem->m_csPacketBank.FreePacket(pvPacketBank);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	// packet을 보낸다.
	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendQuestInfo(INT32 lIID, INT32 lCID, INT16 lLayer, INT16 lRow, INT16 lColumn)
{
	if (lIID == AP_INVALID_IID &&
		lCID == AP_INVALID_CID)
		return FALSE;

	INT16	nPacketLength = 0;

	PVOID	pvPacketQuest = m_pcsAgpmItem->m_csPacketQuest.MakePacket(FALSE, &nPacketLength, 0,
													&lLayer,
													&lRow,
													&lColumn);

	if (!pvPacketQuest)
		return FALSE;

	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_UPDATE;
 	INT8    cStatus		= AGPDITEM_STATUS_QUEST;

	PVOID	pvPacket	= m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
		                                           &cOperation,                                        // operation
												   &cStatus,											// status
												   &lIID,												// item i.d.
												    NULL,												// item template i.d.
													&lCID,												// Item Owner ID
													NULL,												// Item Count
													NULL,												// field(packet)
													NULL,												// inventory(packet)
													NULL,												// bank
													NULL,												// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													pvPacketQuest,										// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL,												// CashInformation
													NULL
													);

	m_pcsAgpmItem->m_csPacketQuest.FreePacket(pvPacketQuest);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	// packet을 보낸다.
	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendFieldInfo(INT32 lIID, INT32 lCID, AuPOS *ppcsPos)
{
	if (lIID == AP_INVALID_IID ||
		lCID == AP_INVALID_CID)
		return FALSE;

	INT16	nPacketLength	= 0;

	PVOID	pvPacketField	= m_pcsAgpmItem->m_csPacketField.MakePacket(FALSE, &nPacketLength, 0,
																		ppcsPos);
	if (!pvPacketField)
		return FALSE;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_UPDATE;
	INT8	cStatus			= AGPDITEM_STATUS_FIELD;
	PVOID	pvPacket		= m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
		                                           &cOperation,                                        // operation
												   &cStatus,											// status
												   &lIID,												// item i.d.
												    NULL,												// item template i.d.
													&lCID,												// Item Owner ID
													NULL,												// Item Count
													pvPacketField,										// field(packet)
													NULL,												// inventory(packet)
													NULL,												// bank
													NULL,												// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL, 
													NULL,												// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL,												// CashInformation
													NULL
													);

	m_pcsAgpmItem->m_csPacketField.FreePacket(pvPacketField);
	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nPacketLength))
	{
		m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmItem::SendPickupItem(INT32 lIID, INT32 lCID)
{
	if (lIID == AP_INVALID_IID ||
		lCID == AP_INVALID_CID)
		return FALSE;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_PICKUP_ITEM;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
		                                           &cOperation,                                        // operation
												    NULL,												// status
												   &lIID,												// item i.d.
												    NULL,												// item template i.d.
													&lCID,												// Item Owner ID
													NULL,												// Item Count
													NULL,												// field(packet)
													NULL,												// inventory(packet)
													NULL,												// bank
													NULL,												// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL,												// CashInformation
													NULL
													);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendBuyBankSlot(INT32 lCID)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->MakePacketRequestBuyBankSlot(lCID, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmItem::SendUseReturnScroll()
{
	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (m_pcsAgpmCharacter->IsCombatMode(pcsSelfCharacter))
		return FALSE;

	AgpdItemADChar	*pcsItemADChar		= m_pcsAgpmItem->GetADCharacter(pcsSelfCharacter);
	if (!pcsItemADChar->m_bUseReturnTeleportScroll)
		return FALSE;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_USE_RETURN_SCROLL;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
		                                           &cOperation,                                        // operation
												    NULL,											// status
												    NULL,												// item i.d.
												    NULL,												// item template i.d.
													&pcsSelfCharacter->m_lID,												// Item Owner ID
													NULL,												// Item Count
													NULL,										// field(packet)
													NULL,												// inventory(packet)
													NULL,												// bank
													NULL,												// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL,												// CashInformation
													NULL
													);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendCancelReturnScroll()
{
	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdItemADChar	*pcsItemADChar		= m_pcsAgpmItem->GetADCharacter(pcsSelfCharacter);
	if (!pcsItemADChar->m_bUseReturnTeleportScroll)
		return FALSE;

	INT8	cOperation		= AGPMITEM_PACKET_OPERATION_CANCEL_RETURN_SCROLL;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMITEM_PACKET_TYPE,
		                                           &cOperation,                                        // operation
												    NULL,											// status
												    NULL,												// item i.d.
												    NULL,												// item template i.d.
													&pcsSelfCharacter->m_lID,												// Item Owner ID
													NULL,												// Item Count
													NULL,										// field(packet)
													NULL,												// inventory(packet)
													NULL,												// bank
													NULL,												// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Quest
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL,												// CashInformation
													NULL
													);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendSplitItem(INT32 lIID, INT32 lSplitStackCount, AgpdItemStatus eTargetStatus, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->MakePacketSplitItem(m_pcsAgpmItem->GetItem(lIID), lSplitStackCount, eTargetStatus, nLayer, nRow, nColumn, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmItem::SendSplitItem(INT32 lIID, INT32 lSplitStackCount, AuPOS *pstDestPos)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->MakePacketSplitItem(m_pcsAgpmItem->GetItem(lIID), lSplitStackCount, pstDestPos, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmItem::SendRequestConvert(INT32 lIID, INT32 lCID, INT32 lSpiritStoneID)
{
	if (lIID == AP_INVALID_IID ||
		lCID == AP_INVALID_CID ||
		lSpiritStoneID == AP_INVALID_IID ||
		!m_pcsAgpmItemConvert)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket = m_pcsAgpmItemConvert->MakePacketRequestSpiritStoneConvert(lCID, lIID, lSpiritStoneID, &nPacketLength);

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendDropMoneyToField(INT32 lCID, INT32 lMoneyCount )
{
	if( lMoneyCount <= 0 || lCID == AP_INVALID_IID )
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacter(lCID);
	if (pcsCharacter)
	{
		INT64	llCurrentMoney	= 0;
		if (!m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llCurrentMoney))
			return FALSE;

		if (llCurrentMoney < lMoneyCount)
			lMoneyCount	= (INT32) llCurrentMoney;
	}

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pcsAgpmItem->MakePacketDropMoneyToField( lCID, lMoneyCount, &nPacketLength );

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendRequestDestroyItem(INT32 lCID, INT32 lIID)
{
	if (lCID == AP_INVALID_CID ||
		lIID == AP_INVALID_IID)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pcsAgpmItem->MakePacketRequestDestroyItem( lCID, lIID, &nPacketLength );

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::SendAutoPickItem(INT32 llID, INT32 lCID, INT16 *lSwitch)
{
	if(lCID == AP_INVALID_CID ||
	   llID == AP_INVALID_IID ||
	   !(*lSwitch == 1 || *lSwitch == 0))
	   return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pcsAgpmItem->MakePacketChangeAutoPickItem(lCID, llID, lSwitch, &nPacketLength);

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

/******************************************************************************
* Purpose :
*
* 090802. Bob Jung
******************************************************************************/
BOOL AgcmItem::UpdateItem(AgpdItem *pcsAgpdItem)
{
	return TRUE;
}

RpAtomic *AgcmItem::UtilCopyAtomic(RpAtomic *pstAtomic)
{
	RwMemory	stMemory;
	RwStream	*pstReadStream;
	RwStream	*pstWriteStream;
	RpAtomic	*pstNewAtomic;

	stMemory.length = RpAtomicStreamGetSize(pstAtomic);

	pstWriteStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &stMemory);
	if(!pstWriteStream)
	{
		return NULL;
	}

	if(!RpAtomicStreamWrite(pstAtomic, pstWriteStream))
	{
		return NULL;
	}

	RwStreamClose(pstWriteStream, &stMemory);

	pstReadStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemory);
	if(!pstReadStream)
	{
		RwFree(stMemory.start);
		return NULL;
	}

	if(!RwStreamFindChunk(pstReadStream, rwID_ATOMIC, NULL, NULL))
	{
		RwFree(stMemory.start);
		return NULL;
	}

	pstNewAtomic = RpAtomicStreamRead(pstReadStream);

	RwStreamClose(pstReadStream, &stMemory);

	RwFree(stMemory.start);

	return pstNewAtomic;
}

RpAtomic *AgcmItem::UtilSetAtomicCB(RpAtomic *atomic, void *data)
{
	RpAtomic **pstDestAtomic = (RpAtomic **)(data);
	*(pstDestAtomic) = atomic;

	return atomic;
}

RpAtomic *AgcmItem::CreateAtomic(CHAR *szDFF)
{
	return m_pcsAgcmResourceLoader->LoadAtomic(szDFF, "Item.txd", NULL, -1, m_szTexturePath);
}

BOOL AgcmItem::LoadTemplateClump(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate, BOOL bArmour)
{
	CHAR	*pszBaseClump		= NULL;
	CHAR	*pszSecondClump		= NULL;
	CHAR	*pszFieldClump		= NULL;
	CHAR	*pszPickingClump	= NULL;

	if( pstAgcdItemTemplate->m_pcsPreData )
	{
		if( !pstAgcdItemTemplate->m_pstBaseClump && pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName )
			_ReallocCopyString( &pszBaseClump, pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );
		if( !pstAgcdItemTemplate->m_pstSecondClump && pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName )
			_ReallocCopyString( &pszSecondClump, pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName );
		if( !pstAgcdItemTemplate->m_pstFieldClump && pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName )
			_ReallocCopyString( &pszFieldClump, pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName );
		if( !pstAgcdItemTemplate->m_pstPickingAtomic && pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName )
			_ReallocCopyString( &pszPickingClump, pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName );
	}
	else
	{
		CHAR	szTemp[256];
		memset(szTemp, 0, sizeof (CHAR) * 256);

		if( pstAgcdItemTemplate->GetBaseDFFID() > 0 )
		{
			pstAgcdItemTemplate->GetBaseDFFName( szTemp );
			_ReallocCopyString( &pszBaseClump, szTemp );
		}

		if( pstAgcdItemTemplate->GetSecondDFFID() > 0 )
		{
			pstAgcdItemTemplate->GetSecondDFFName( szTemp );
			_ReallocCopyString( &pszSecondClump, szTemp );
		}

		if( pstAgcdItemTemplate->GetFieldDFFID() > 0 )
		{
			pstAgcdItemTemplate->GetFieldDFFName( szTemp );
			_ReallocCopyString( &pszFieldClump, szTemp );
		}

		if( pstAgcdItemTemplate->GetPickDFFID() > 0 )
		{
			pstAgcdItemTemplate->GetPickDFFName( szTemp );
			_ReallocCopyString( &pszPickingClump, szTemp );
		}
	}

	if (pszBaseClump)
	{
		pstAgcdItemTemplate->m_pstBaseClump = bArmour ? CreateArmourPartClump( GetClumpAdminData(pszBaseClump), ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->GetPartIndex() ) : GetClumpAdminData(pszBaseClump);
		if( !pstAgcdItemTemplate->m_pstBaseClump && pstAgcdItemTemplate->m_pcsPreData )
			return FALSE;

		//. access violation when AgcdItemTemplate has no base clump.
		if( pstAgcdItemTemplate->m_pstBaseClump && pstAgcdItemTemplate->m_csClumpRenderType.m_lSetCount > 0 )
		{
			UtilSetUDAInt32Params	csParams;
			csParams.m_lCBCount		= 0;
			strcpy(csParams.m_szUDAName, AGCMLOD_ATOMIC_INDEX);
			RpClumpForAllAtomics( pstAgcdItemTemplate->m_pstBaseClump, UtilSetUDAInt32CB, (void *)(&csParams) );

			if ( csParams.m_nValidCount == 0 )
			{
				csParams.Clear();
				strcpy(csParams.m_szUDAName, AGCMLOD_ATOMIC_INDEX_EXTRA);
				RpClumpForAllAtomics(pstAgcdItemTemplate->m_pstBaseClump, UtilSetUDAInt32CB, (void *)(&csParams));
			}

			m_pcsAgcmRender->ClumpSetRenderTypeCheckCustData( pstAgcdItemTemplate->m_pstBaseClump, &pstAgcdItemTemplate->m_csClumpRenderType, csParams.m_alUDAInt32 );
		}
	}

	if (pszSecondClump)
	{
		pstAgcdItemTemplate->m_pstSecondClump = bArmour ? CreateArmourPartClump( GetClumpAdminData(pszSecondClump), ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->GetPartIndex() ) : GetClumpAdminData(pszSecondClump);
		if( !pstAgcdItemTemplate->m_pstSecondClump && pstAgcdItemTemplate->m_pcsPreData )
			return FALSE;

		//. access violation when AgcdItemTemplate has no second clump.
		if( pstAgcdItemTemplate->m_pstSecondClump && pstAgcdItemTemplate->m_csClumpRenderType.m_lSetCount > 0 )
		{
			UtilSetUDAInt32Params	csParams;
			csParams.m_lCBCount		= 0;
			strcpy( csParams.m_szUDAName, AGCMLOD_ATOMIC_INDEX );
			RpClumpForAllAtomics( pstAgcdItemTemplate->m_pstSecondClump, UtilSetUDAInt32CB, (void *)(&csParams)	);

			if ( csParams.m_nValidCount == 0 )
			{
				csParams.Clear();
				strcpy(csParams.m_szUDAName, AGCMLOD_ATOMIC_INDEX_EXTRA);
				RpClumpForAllAtomics(pstAgcdItemTemplate->m_pstSecondClump, UtilSetUDAInt32CB, (void *)(&csParams));
			}
			
			m_pcsAgcmRender->ClumpSetRenderTypeCheckCustData( pstAgcdItemTemplate->m_pstSecondClump, &pstAgcdItemTemplate->m_csClumpRenderType, csParams.m_alUDAInt32 );
		}
	}

	if (pszFieldClump)
	{
		pstAgcdItemTemplate->m_pstFieldClump = GetClumpAdminData(pszFieldClump);

		if (!pstAgcdItemTemplate->m_pstFieldClump && pstAgcdItemTemplate->m_pcsPreData)
		{
			ASSERT(!"AgcmItem::LoadTemplateClump() Error No FieldClump !!!");
			return FALSE;
		}
	}

	if (pszPickingClump)
	{
		pstAgcdItemTemplate->m_pstPickingAtomic	= CreateAtomic(pszPickingClump);

		if (!pstAgcdItemTemplate->m_pstPickingAtomic && pstAgcdItemTemplate->m_pcsPreData)
		{
			ASSERT(!"AgcmItem::LoadTemplateClump() Error No PickingClump !!!");
			return FALSE;
		}
	}


	DEF_SAFEDELETEARRAY( pszBaseClump );
	DEF_SAFEDELETEARRAY( pszSecondClump );
	DEF_SAFEDELETEARRAY( pszFieldClump );
	DEF_SAFEDELETEARRAY( pszPickingClump );

	return TRUE;
}

BOOL AgcmItem::LoadTemplateTexture(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate)
{
	CHAR	*pszTexture				= NULL;
	CHAR	*pszSmallTexture		= NULL;
	CHAR	*pszDurZeroTexture		= NULL;
	CHAR	*pszDur5UnderTexture	= NULL;


	if( pstAgcdItemTemplate->m_pcsPreData )
	{
		if( !pstAgcdItemTemplate->m_pTexture && pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName )
			_ReallocCopyString( &pszTexture, pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName );
		if( !pstAgcdItemTemplate->m_pSmallTexture && pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName )
			_ReallocCopyString( &pszSmallTexture, pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName );
		if( !pstAgcdItemTemplate->m_pDurabilityZeroTexture && pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName )
			_ReallocCopyString( &pszDurZeroTexture, pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName );
		if( !pstAgcdItemTemplate->m_pDurability5UnderTexture && pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName )
			_ReallocCopyString( &pszDur5UnderTexture, pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName );
	}
	else
	{
		CHAR	szTemp[256];
		memset(szTemp, 0, sizeof (CHAR) * 256);

		if( pstAgcdItemTemplate->GetTextureID() > 0 )
		{
			pstAgcdItemTemplate->GetTextureName( szTemp );
			_ReallocCopyString( &pszTexture, szTemp );
		}

		if( pstAgcdItemTemplate->GetSmallTextureID() > 0 )
		{
			pstAgcdItemTemplate->GetSmallTextureName(szTemp);
			_ReallocCopyString( &pszSmallTexture, szTemp );
		}

		if( pstAgcdItemTemplate->GetDurZeroTextureID() > 0 )
		{
			pstAgcdItemTemplate->GetDurZeroTextureName(szTemp);
			_ReallocCopyString( &pszDurZeroTexture, szTemp );
		}

		if( pstAgcdItemTemplate->GetDur5UnderTextureID() > 0 )
		{
			pstAgcdItemTemplate->GetDur5UnderTextureName(szTemp);
			_ReallocCopyString( &pszDur5UnderTexture, szTemp );
		}
	}

	if (pszTexture)
	{
		pstAgcdItemTemplate->m_pTexture	= GetTexture(pszTexture);
		if( !pstAgcdItemTemplate->m_pTexture )
			pstAgcdItemTemplate->m_pTexture	= GetTexture(pszTexture);

		if (m_pcsAgcmUIControl)
		{
			RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdItemTemplate->m_pcsGridItem);
			if( ppRwTexture && (*ppRwTexture) )
				*ppRwTexture = pstAgcdItemTemplate->m_pTexture;
		}
	}

	if (pszSmallTexture)
	{
		pstAgcdItemTemplate->m_pSmallTexture = GetTexture(pszSmallTexture);
		if (!pstAgcdItemTemplate->m_pSmallTexture)
		{
			ASSERT(!"AgcmItem::LoadTemplateTexture() Error No SmallTexture!!!");
//			return FALSE;
		}
		
		if (m_pcsAgcmUIControl)
		{
			if( !pcsAgpdItemTemplate->m_pcsGridItemSmall )
				pcsAgpdItemTemplate->m_pcsGridItemSmall	= m_pcsAgpmGrid->CreateGridItem();

			RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdItemTemplate->m_pcsGridItemSmall);
			if( ppRwTexture && (*ppRwTexture) )
				*ppRwTexture = pstAgcdItemTemplate->m_pSmallTexture;
		}
	}

	if (pszDurZeroTexture)
	{
		pstAgcdItemTemplate->m_pDurabilityZeroTexture = GetTexture(pszDurZeroTexture);
		if (!pstAgcdItemTemplate->m_pDurabilityZeroTexture && pstAgcdItemTemplate->m_pcsPreData)
		{
			ASSERT(!"AgcmItem::LoadTemplateTexture() Error No DurabilityZeroTexture!!!");
//			return FALSE;
		}
	}

	if (pszDur5UnderTexture)
	{
		pstAgcdItemTemplate->m_pDurability5UnderTexture = GetTexture(pszDur5UnderTexture);
		if (!pstAgcdItemTemplate->m_pDurability5UnderTexture && pstAgcdItemTemplate->m_pcsPreData)
		{
			ASSERT(!"AgcmItem::LoadTemplateTexture() Error No Durability5UnderTexture!!!");
//			return FALSE;
		}
	}

	DEF_SAFEDELETEARRAY( pszTexture );
	DEF_SAFEDELETEARRAY( pszSmallTexture );
	DEF_SAFEDELETEARRAY( pszDurZeroTexture );
	DEF_SAFEDELETEARRAY( pszDur5UnderTexture );

	return TRUE;
}

BOOL AgcmItem::LoadTemplateData(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate, BOOL bCheckArmour)
{
	ASSERT(pstAgcdItemTemplate->m_lRefCount >= 0);

	// 여기서 Template의 Item Clump를 만들어 준다. 없으면...
	if (!pstAgcdItemTemplate->m_bLoaded)
	{
		AuAutoLock	lock(pcsAgpdItemTemplate->m_Mutex);
		if (lock.Result())
		{
			BOOL	bArmour = FALSE;

			// Armor일때는 PartID로 Clump를 새로 만든다.
			if( bCheckArmour &&
				pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP &&
				((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR )
				bArmour = TRUE;

			if (!LoadTemplateClump(pcsAgpdItemTemplate, pstAgcdItemTemplate, bArmour))
			{
				ASSERT(!"AgcmItem::LoadTemplateData() LoadTemplateClump Error!!!");
				return FALSE;
			}

			if (!LoadTemplateTexture(pcsAgpdItemTemplate, pstAgcdItemTemplate))
			{
				ASSERT(!"AgcmItem::LoadTemplateData() LoadTemplateTexture Error!!!");
				return FALSE;
			}

			EnumCallback(AGCMITEM_CB_ID_LOAD_CLUMP, (PVOID)(pcsAgpdItemTemplate), m_szClumpPath);

			pstAgcdItemTemplate->m_bLoaded = TRUE;
		}
	}

	return TRUE;
}

BOOL AgcmItem::ReleaseTemplateClump(AgcdItemTemplate *pstAgcdItemTemplate, BOOL bArmour)
{
	RpClump		*pstClump	= NULL;
	RpAtomic	*pstAtomic	= NULL;

	if (pstAgcdItemTemplate->m_pstBaseClump)
	{
		pstClump = pstAgcdItemTemplate->m_pstBaseClump;

		pstAgcdItemTemplate->m_pstBaseClump = NULL;

		if (bArmour)
		{
			m_pcsAgcmResourceLoader->AddDestroyClump(pstClump);
		}
	}

	if (pstAgcdItemTemplate->m_pstSecondClump)
	{
		pstClump = pstAgcdItemTemplate->m_pstSecondClump;

		pstAgcdItemTemplate->m_pstSecondClump = NULL;

		if (bArmour)
		{
			m_pcsAgcmResourceLoader->AddDestroyClump(pstClump);
		}
	}

	if (pstAgcdItemTemplate->m_pstFieldClump)
	{
		pstClump = pstAgcdItemTemplate->m_pstFieldClump;

		pstAgcdItemTemplate->m_pstFieldClump = NULL;
	}

	if (pstAgcdItemTemplate->m_pstPickingAtomic)
	{		
		pstAtomic = pstAgcdItemTemplate->m_pstPickingAtomic;

		pstAgcdItemTemplate->m_pstPickingAtomic = NULL;

		m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);	
	}

	if (pstAgcdItemTemplate->m_pcsPreData)
	{
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName)
		{
			RemoveClumpAdminData(pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName);
		}

		if (pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName)
		{
			RemoveClumpAdminData(pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName);
		}

		if (pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName)
		{
			RemoveClumpAdminData(pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName);
		}
	}

	return TRUE;
}

BOOL AgcmItem::ReleaseTemplateTexture(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate)
{

	if (	(pstAgcdItemTemplate->m_pTexture) &&
			(!pstAgcdItemTemplate->m_lAttachedToGrid)	)
	{
		RwTextureDestroy(pstAgcdItemTemplate->m_pTexture);
		pstAgcdItemTemplate->m_pTexture = NULL;

		if (m_pcsAgcmUIControl)
		{
			RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdItemTemplate->m_pcsGridItem);
			if (	(ppRwTexture) &&
					(*ppRwTexture)		)
				*ppRwTexture	= NULL;
		}
	}

	if (pstAgcdItemTemplate->m_pSmallTexture)
	{
		RwTextureDestroy(pstAgcdItemTemplate->m_pSmallTexture);
		pstAgcdItemTemplate->m_pSmallTexture = NULL;

		if (	(m_pcsAgcmUIControl) &&
				(pcsAgpdItemTemplate->m_pcsGridItemSmall)	)
		{
			RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdItemTemplate->m_pcsGridItemSmall);
			if (	(ppRwTexture) &&
					(*ppRwTexture)		)
				*ppRwTexture	= NULL;
		}
	}

	return TRUE;
}

BOOL AgcmItem::ReleaseTemplateData(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate, BOOL bCheckArmour)
{
	--(pstAgcdItemTemplate->m_lRefCount);

	ASSERT(pstAgcdItemTemplate->m_lRefCount >= 0);

	if(pstAgcdItemTemplate->m_lRefCount < 1 && pstAgcdItemTemplate->m_bLoaded)
	{
		AuAutoLock	lock(pcsAgpdItemTemplate->m_Mutex);
		if (lock.Result())
		{
			BOOL		bArmour = FALSE;

			// Armor 일때는 직접 Clump를 만들기 때문에 Clump 들도 Destroy해야한다.
			if(	(bCheckArmour) &&
				(pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP) &&
				(((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)	)
				bArmour = TRUE;

			ReleaseTemplateClump(pstAgcdItemTemplate, bArmour);
			ReleaseTemplateTexture(pcsAgpdItemTemplate, pstAgcdItemTemplate);

			pstAgcdItemTemplate->m_bLoaded = FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmItem::LoadAllTemplateData()
{
	for(AgpaItemTemplate::iterator it = m_pcsAgpmItem->csTemplateAdmin.begin(); it != m_pcsAgpmItem->csTemplateAdmin.end(); ++it)
	{
		AgpdItemTemplate* pcsAgpdItemTemplate = it->second;
		AgcdItemTemplate* pstAgcdItemTemplate = GetTemplateData(pcsAgpdItemTemplate);
		if( !pstAgcdItemTemplate )		return FALSE;

		++pstAgcdItemTemplate->m_lRefCount;
		if( !LoadTemplateData( pcsAgpdItemTemplate, pstAgcdItemTemplate ) )
			return FALSE;
	}

	return TRUE;
}

RpClump *AgcmItem::CopyClump(RpClump *pstClump)
{
	RwMemory	stMemory;
	RwStream	*pstReadStream;
	RwStream	*pstWriteStream;
	RpClump		*pstNewClump;
	RwUInt32	ulClumpSize = RpClumpStreamGetSize(pstClump);

	stMemory.length = ulClumpSize;

	pstWriteStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &stMemory);
	if (!pstWriteStream) return NULL;
	if (!RpClumpStreamWrite(pstClump, pstWriteStream)) return NULL;

	RwStreamClose(pstWriteStream, &stMemory);

	pstReadStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemory);
	if (!pstReadStream)
	{
		RwFree(stMemory.start);
		return NULL;
	}

	if (!RwStreamFindChunk(pstReadStream, rwID_CLUMP, NULL, NULL))
	{
		RwFree(stMemory.start);
		return NULL;
	}

	pstNewClump = RpClumpStreamRead(pstReadStream);
	RwStreamClose(pstReadStream, &stMemory);
	RwFree(stMemory.start);
	return pstNewClump;
}

BOOL AgcmItem::RemoveClumpData(AgpdItem *pcsAgpdItem, AgcdItem *pcsAgcdItem)
{
	EnumCallback(AGCMITEM_CB_ID_PRE_REMOVE_DATA, (PVOID)(pcsAgpdItem), (PVOID)(pcsAgcdItem));
	
	if (pcsAgcdItem->m_lStatus & AGCDITEM_STATUS_WORLD_ADDED)
	{
		if (!RemoveClumpFromWorld(pcsAgcdItem->m_pstClump))	return FALSE;
	}

	if (RpClumpGetFrame(pcsAgcdItem->m_pstClump))
	{
		if (RwFrameGetParent(RpClumpGetFrame(pcsAgcdItem->m_pstClump)))
		{
			LockFrame();

			RwFrameRemoveChild(RpClumpGetFrame(pcsAgcdItem->m_pstClump));

			UnlockFrame();
		}
	}

	m_pcsAgcmResourceLoader->AddDestroyClump(pcsAgcdItem->m_pstClump);

	pcsAgcdItem->m_pstClump = NULL;

	return TRUE;
}

BOOL AgcmItem::ReleaseItemData(AgpdItem *pcsAgpdItem)
{
	AgcdItem *pstAgcdItem = GetItemData(pcsAgpdItem);

	ApAutoWriterLock	csLock( pstAgcdItem->m_rwLock );
	if(!pstAgcdItem) return FALSE;

	RemoveAllAttachedAtomics(&pstAgcdItem->m_csAttachedAtomics);

	if (pstAgcdItem->m_pstHierarchy)
	{
		m_pcsAgcmResourceLoader->AddDestroyHierarchy(pstAgcdItem->m_pstHierarchy);

		pstAgcdItem->m_pstHierarchy = NULL;
	}

	if (pstAgcdItem->m_pstClump)
	{
		if(!RemoveClumpData(pcsAgpdItem, pstAgcdItem)) return FALSE;
	}

	if (pstAgcdItem->m_pstPickAtomic)
	{
		m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAgcdItem->m_pstPickAtomic);

		pstAgcdItem->m_pstPickAtomic = NULL;
	}

	pstAgcdItem->m_lStatus &= ~(AGCDITEM_STATUS_MAKE_ITEM_CLUMP | AGCDITEM_STATUS_ATTACH | AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_REMOVED);

	return TRUE;
}

BOOL AgcmItem::MakeItemClump(AgpdItem *pcsAgpdItem)
{
	AgpdItemTemplate *pcsAgpdItemTemplate = (AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate);
	if (!pcsAgpdItemTemplate)
	{
		ASSERT(!"AgcmItem::MakeItemClump() No Template !!!\n");
		return FALSE;
	}

	AgcdItem *pstAgcdItem = GetItemData(pcsAgpdItem);
	if ((!pstAgcdItem) || (pstAgcdItem->m_pstClump))
	{
		ASSERT(!"AgcmItem::MakeItemClump() Clump Exist !!!\n");
		return FALSE;
	}

	AgcdItemTemplate *pstAgcdItemTemplate = pstAgcdItem->m_pstAgcdItemTemplate;
	if (!pstAgcdItemTemplate)
	{
		ASSERT(!"AgcmItem::MakeItemClump() No Template !!!\n");
		return FALSE;
	}

	if( !pstAgcdItemTemplate->m_pstBaseClump ) return FALSE;

	switch(pcsAgpdItem->m_eStatus)
	{
	case AGPDITEM_STATUS_EQUIP:
		{
			if (	(pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP) &&
				(	((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR	) ||
				(	((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON	) ||
				(	((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD	)	)
			{
				AgpdCharacter	*pstAgpdOwner	= pcsAgpdItem->m_pcsCharacter;
				if (!pstAgpdOwner)
				{
					ASSERT(!"AgcmItem::MakeItemClump() No Owner !!!\n");
					return FALSE;
				}

				INT32			lGender			= 0;
				m_pcsAgpmFactors->GetValue(&pstAgpdOwner->m_pcsCharacterTemplate->m_csFactor, &lGender, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_GENDER);

				RpClump			*pstCloneClump	= pstAgcdItemTemplate->m_pstBaseClump;
				if (lGender == 2)
				{
					if(pstAgcdItemTemplate->m_pstSecondClump)
						pstCloneClump			= pstAgcdItemTemplate->m_pstSecondClump;
				}

				if (!pstCloneClump)
				{
					ASSERT(!"AgcmItem::MakeItemClump() CloneClump Error!!!\n");
					return FALSE;
				}

				LockFrame();
				pstAgcdItem->m_pstClump			= RpClumpClone(pstCloneClump);
				UnlockFrame();

				if (!pstAgcdItem->m_pstClump)
				{
					ASSERT(!"AgcmItem::MakeItemClump() Cloning Error !!!\n");
					return FALSE;
				}

#ifdef _DEBUG
				pstAgcdItem->m_pstClump->pvApBase = pcsAgpdItem;
				RpClumpSetName(pstAgcdItem->m_pstClump, ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_szName);
#endif

				RwFrame	*pstItemClumpFrame		= RpClumpGetFrame(pstAgcdItem->m_pstClump);
				pstAgcdItem->m_pstHierarchy		= m_pcsAgcmCharacter->GetHierarchy(pstAgcdItem->m_pstClump);

				if (pstAgcdItemTemplate->m_csClumpRenderType.m_lSetCount)
				{
					m_pcsAgcmRender->ClumpSetRenderType(pstAgcdItem->m_pstClump, &pstAgcdItemTemplate->m_csClumpRenderType);
				}
			}
		}
		break;

	case AGPDITEM_STATUS_FIELD:
		{
			if (pstAgcdItemTemplate->m_pstFieldClump)
			{
				LockFrame();
				pstAgcdItem->m_pstClump		= RpClumpClone(pstAgcdItemTemplate->m_pstFieldClump);
				UnlockFrame();
			}
			else if (pstAgcdItemTemplate->m_pstBaseClump)
			{
				if (	(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP) ||
						(((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR)	)
				{
					LockFrame();
					pstAgcdItem->m_pstClump	= RpClumpClone(pstAgcdItemTemplate->m_pstBaseClump);
					UnlockFrame();

					//. 2006. 4. 25. nonstopdj. 
					//. null check 추가.
					if (!pstAgcdItem->m_pstClump)
					{
						ASSERT(!"AgcmItem::MakeItemClump() Cloning Error !!!\n");
						return FALSE;
					}
				}

				if (pstAgcdItemTemplate->m_csClumpRenderType.m_lSetCount && pstAgcdItem->m_pstClump)
				{
					m_pcsAgcmRender->ClumpSetRenderType(pstAgcdItem->m_pstClump, &pstAgcdItemTemplate->m_csClumpRenderType);
				}
			}

			if (pstAgcdItemTemplate->m_pstPickingAtomic)
			{
				LockFrame();

				pstAgcdItem->m_pstPickAtomic = RpAtomicClone(pstAgcdItemTemplate->m_pstPickingAtomic);
				RpAtomicSetFrame(pstAgcdItem->m_pstPickAtomic, RpClumpGetFrame(pstAgcdItem->m_pstClump));

				UnlockFrame();
			}

			if (pstAgcdItem->m_pstClump)
			{
#ifdef _DEBUG
				pstAgcdItem->m_pstClump->pvApBase = pcsAgpdItem;
				RpClumpSetName(pstAgcdItem->m_pstClump, ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_szName);
#endif

				pstAgcdItem->m_pstHierarchy		= m_pcsAgcmCharacter->GetHierarchy(pstAgcdItem->m_pstClump);
				if (pstAgcdItem->m_pstHierarchy)
				{
					LockFrame();
					RpHAnimHierarchyAttach(pstAgcdItem->m_pstHierarchy);
					UnlockFrame();
				}
			}
		}
		break;

	case AGPDITEM_STATUS_INVENTORY: break;
	default: return FALSE;
	}

	if (pstAgcdItem->m_pstClump)
	{
		AgcdLODData	*pstLODData = NULL;
		
		if (m_pcsAgcmLODManager)
			pstLODData = m_pcsAgcmLODManager->GetLODData(&pstAgcdItemTemplate->m_stLOD, 0, FALSE);

		RwSphere	*BS	= &pstAgcdItem->m_pstAgcdItemTemplate->m_stBSphere;

		if (BS)
		{
			// BBOX 임시로 구하자
			float	ty = BS->center.y;//BS->center.y + BS->radius; // radius를 더하면 칼끝이 땅을 향할경우 땅밑에 가려진다.
			float	x1 = BS->center.x - BS->radius;
			float	x2 = BS->center.x + BS->radius;
			float	z1 = BS->center.z - BS->radius;
			float	z2 = BS->center.z + BS->radius;

			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].x	= x1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].z	= z1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].x	= x1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].z	= z2;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].x	= x2;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].z	= z1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].x	= x2;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].z	= z2;
		}
		else
		{
			float	ty = 0.0f;
			float	x1 = -50.0f;
			float	x2 = 50.0f;
			float	z1 = -50.0f;
			float	z2 = 50.0f;

			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].x	= x1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].z	= z1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].x	= x1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].z	= z2;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].x	= x2;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].z	= z1;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].x	= x2;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].y	= ty;
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].z	= z2;
		}

		AcuObject::SetClumpType(pstAgcdItem->m_pstClump,
								pstAgcdItemTemplate->m_lObjectType | ACUOBJECT_TYPE_ITEM,
								pcsAgpdItem->m_lID,
								pcsAgpdItem,
								pcsAgpdItemTemplate,
								((pstLODData) && (pstLODData->m_ulMaxLODLevel)) ? (pstLODData) : (NULL),
								pstAgcdItem->m_pstPickAtomic,
								BS,
								NULL,
								&pstAgcdItemTemplate->m_stOcTreeData);
	}
	
	return TRUE;
}

BOOL AgcmItem::DumpTexDict()
{
	if( !m_pstTexDict )		return FALSE;
	if( !m_pstTexDict || !m_szTexDict[0])		return FALSE;

	RwStream* stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, m_szTexDict );
	if( !stream )			return FALSE;

	RtPITexDictionaryStreamWrite( m_pstTexDict , stream );
	RwStreamClose( stream, NULL );

	return TRUE;
}

RwTexture*	AgcmItem::GetItemTexture( INT32 lItemID )	
{
	AgpdItem *pcsAgpdItem = m_pcsAgpmItem->GetItem(lItemID);
	if( !pcsAgpdItem )		return NULL;

	RwTexture** ppRwTexture = (RwTexture**)(m_pcsAgpmGrid->GetAttachedModuleData( m_pcsAgcmUIControl->m_lItemGridTextureADDataIndex, pcsAgpdItem->m_pcsGridItem  ));
	if( !ppRwTexture )		return NULL;

	return (RwTexture*) *ppRwTexture ;
}

BOOL	AgcmItem::UpdateInventoryForUI( INT32 lItemID, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	AgpdItem *pcsAgpdItem = m_pcsAgpmItem->GetItem(lItemID);
	if( !pcsAgpdItem )		return FALSE;

	return m_pcsAgpmItem->UpdateItemInInventory( m_pcsAgcmCharacter->m_pcsSelfCharacter, pcsAgpdItem, nLayer, nRow, nColumn );
}

BOOL	AgcmItem::CBUpdateInventory( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmItem*		pThis = (AgcmItem*)pClass;
	AgpdItem*		pAgpdItem = (AgpdItem*)pData;

	return TRUE;	
}

BOOL	AgcmItem::CBUpdateBank( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmItem*	pThis = (AgcmItem*)pClass;
	AgpdItem*	pAgpdItem = (AgpdItem*)pData;

	return TRUE;
}

BOOL	AgcmItem::CBUpdateMoney( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmItem*		pThis = (AgcmItem*)pClass;
	AgpdCharacter* ppdCharacter = (AgpdCharacter*)pData;

	return TRUE;
}

BOOL	AgcmItem::CBUpdateBankMoney( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmItem*		pThis = (AgcmItem*)pClass;
	AgpdCharacter*	ppdCharacter = (AgpdCharacter*)pData;

	return TRUE;
}

BOOL	AgcmItem::CBUpdateEquipment( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmItem*		pThis = (AgcmItem*)pClass;
	AgpdItem*		pAgpdItem = (AgpdItem*)pData;

	return TRUE;
}

BOOL	AgcmItem::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmItem*		pThis = (AgcmItem*)pClass;

	return TRUE;
}

BOOL AgcmItem::UseItem(INT32 lIID, AgpdCharacter *pcsCharacter)
{
	return UseItem(m_pcsAgpmItem->GetItem(lIID), pcsCharacter);
}

BOOL AgcmItem::UseItem(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter)
{
	if (!pcsItem) return FALSE;
	if ( pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_SUB_INVENTORY &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY )
		return FALSE;

	//// 이동계열 문서( 왕복문서 or 이동문서 )면 로딩바를 띄워주고 넘어간다.
	//if( IsTeleportScroll( pcsItem ) )
	//{
	//	// 이미 대기상태라면 패스
	//	if( m_bIsNowWaitingForUseItem ) return FALSE;

	//	// 사용대기중인 아이템을 또 사용하려고 덤볐어도 패스
	//	if( m_nWaitingItemID == pcsItem->m_lID ) return FALSE;

	//	// 현재 사용가능한지 체크해서 사용불가능이면 패스
	//	if( !EnumCallback( AGCMITEM_CB_ID_CHECK_USE_ITEM, pcsItem, NULL ) )	return FALSE;
	//	if( !CheckReUseInterval( pcsItem ) ) return FALSE;

	//	// 로딩바를 띄워주고 대기상태에 들어갔으니 그대로 패스
	//	return OpenProgressBarWaiting( pcsItem, 5000 );
	//}

	if (IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType))
	{
		if (pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE)
		{
			BOOL bEnableStopCashItem = m_pcsAgpmItem->CheckEnableStopCashItem(pcsItem);
			if (bEnableStopCashItem)		//	사용중지가 가능하면
				return (UnuseItem(pcsItem));
		}
		else
		{
			AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
			if( pcmUIItem )
			{
				if( pcmUIItem->IsPetItem( pcsItem ) )
				{
					INT16 bAuto = ( INT16 )pcmUIItem->m_AutoPickUp.IsAutoPickUp();
					SendAutoPickItem( pcsItem->m_lID, pcsItem->m_ulCID, &bAuto );
				}
			}
		}
	}
	if( ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE )
	{
		switch( ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType )
		{
		case AGPMITEM_USABLE_TYPE_SKILL_BOOK:
			return UseItemSkillBook(pcsItem, TRUE);
		case AGPMITEM_USABLE_TYPE_MAP:
			// 맵아이템이면 콜백을 불러주고 종료시킨다.
			// 서버로는 어떤 메시지도 보내지 않는다.
			EnumCallback(AGCMITEM_CB_ID_USE_MAP_ITEM, pcsItem, NULL);
			return TRUE;
		case AGPMITEM_USABLE_TYPE_QUEST:
			// 퀘스트 아이템 처리..
			// 아이템에 이벤트를 체크함..
			{
				AgpdCharacter* pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
				static ApmEventManager* s_pcsApmEventManager = ( ApmEventManager * ) GetModule( "ApmEventManager" );
				s_pcsApmEventManager->GetEvent( pcsItem->m_eType , pcsItem->m_lID , APDEVENT_FUNCTION_QUEST );

				static AgcmEventManager* s_pcsAgcmEventManager = ( AgcmEventManager * ) GetModule( "AgcmEventManager" );
				s_pcsAgcmEventManager->CheckEvent( pcsItem , pcsSelfCharacter , APDEVENT_FUNCTION_QUEST );
			}

			return TRUE;
		}
	}

	if( !EnumCallback( AGCMITEM_CB_ID_CHECK_USE_ITEM, pcsItem, NULL ) )	return FALSE;
	if( !CheckReUseInterval( pcsItem ) )								return FALSE;
		
	INT32 lTargetID = pcsCharacter ? pcsCharacter->m_lID : AP_INVALID_CID;

	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
		((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL)
	{
		lTargetID	= m_pcsAgcmCharacter->GetSelectTarget(pcsItem->m_pcsCharacter);
	}

	BOOL	bSendResult = TRUE;

	if (m_pAgcmUICooldown && AGCMUICOOLDOWN_INVALID_ID != m_lCooldownID
		&& ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_LOTTERY_BOX)
	{
		
		return m_pAgcmUICooldown->StartCooldown(m_lCooldownID, AGPMITEM_LOTTERY_BOX_CONVERSION_DURATION_MSEC, NULL, TRUE, (PVOID)pcsItem->m_lID, AGCMUICOOLDOWN_EVENT_SOUND_ITEM);
	}
	else
	{
		INT16	nPacketLength = 0;
		PVOID	pvPacket = m_pcsAgpmItem->MakePacketItemUse(pcsItem, &nPacketLength, lTargetID);

		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		bSendResult	= SendPacket(pvPacket, nPacketLength);

		m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
	}

	return bSendResult;
}

BOOL AgcmItem::CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmItem *pThis = (AgcmItem *) pClass;

	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem( (INT32) pCustData );

	if (pcsItem == NULL)
	{
		return FALSE;
	}

	if (pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY)
	{
		return FALSE;
	}

	INT16	nPacketLength = 0;
	PVOID	pvPacket = pThis->m_pcsAgpmItem->MakePacketItemUse(pcsItem, &nPacketLength, 0);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength);

	pThis->m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::UseItemSkillBook(AgpdItem *pcsItem, BOOL bIsNeedConfirm)
{
	if (!pcsItem)
		return FALSE;

	if (bIsNeedConfirm)
	{
		EnumCallback(AGCMITEM_CB_ID_CONFIRM_USE_SKILL_BOOK, pcsItem, NULL);
		return TRUE;
	}

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pcsAgpmItem->MakePacketItemUse(pcsItem, &nPacketLength, 0);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgcmItem::UseItemHPPotionTest()
{
	return TRUE;
}

BOOL AgcmItem::CheckReUseInterval(AgpdItem *pcsItem)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	AgpdItemTemplate	*pcsItemTemplate	= (AgpdItemTemplate *)	pcsItem->m_pcsItemTemplate;
	if (!pcsItemTemplate)
		return FALSE;

	// 쿨다운이 멈춰있는 것은 일단 통과시킨다. 서버에서 체크한다.
	//if( 1054 == pcsItem->m_lTID )
	//{
	//	AgpdItemADChar* pcsAttachItemData = m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );
	//	if( pcsAttachItemData->m_ulUseReverseOrbTime != 0 &&
	//		((AgpdItemTemplateUsable*)pcsItemTemplate)->m_ulUseInterval != 0 &&
	//		pcsAttachItemData->m_ulUseReverseOrbTime + ((AgpdItemTemplateUsable*)pcsItemTemplate)->m_ulUseInterval >= GetClockCount() )
	//	{
	//		return FALSE;
	//	}
	//}
	//if( 1054 != pcsItem->m_lTID )		//일단 리버스 오브는 체크하지 않고 서버에서 Check하도록 봐꾼다.. 
	if(pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE && ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_REVERSE_ORB)
	{
		AgpdItemCooldownBase stCooldownBase = m_pcsAgpmItem->GetCooldownBase(m_pcsAgcmCharacter->GetSelfCharacter(), pcsItemTemplate->m_lID);
		if(stCooldownBase.m_lTID == 0 && pcsItem->m_pcsGridItem->m_ulUseItemTime + pcsItem->m_pcsGridItem->m_ulReuseIntervalTime > GetClockCount())
			return FALSE;
		else if(stCooldownBase.m_lTID != 0 && stCooldownBase.m_bPause == FALSE && pcsItem->m_pcsGridItem->m_ulUseItemTime + pcsItem->m_pcsGridItem->m_ulReuseIntervalTime > GetClockCount())
			return FALSE;
	}

	// 현재 사용되는 아템은 물약 뿐이다. 나중에 스킬북이나 이런넘들 추가되면 그때 차례로 하나씩 추가한다.
	switch (pcsItemTemplate->m_nType) {
	case AGPMITEM_TYPE_USABLE:
		{
			AgpdItemTemplateUsable	*pcsTemplateUsable = (AgpdItemTemplateUsable *) pcsItemTemplate;

			switch (pcsTemplateUsable->m_nUsableItemType) {
			case AGPMITEM_USABLE_TYPE_POTION:
				{
					AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
					if (!pcsSelfCharacter)
						return FALSE;

					AgpdItemADChar	*pcsAgpdItemADChar	= m_pcsAgpmItem->GetADCharacter(pcsSelfCharacter);
					if (!pcsAgpdItemADChar)
						return FALSE;

					if ((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT] &&
						((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT])->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] > 0)
					{
						INT32	lCurrentHP	= 0;
						INT32	lMaxHP		= 0;

						m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
						m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

						if (lMaxHP <= lCurrentHP)
							return FALSE;
					}
					else if ((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT] &&
						((AgpdFactorCharPoint *) pcsTemplateUsable->m_csEffectFactor.m_pvFactor[AGPD_FACTORS_TYPE_CHAR_POINT])->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP] > 0)
					{
						INT32	lCurrentMP	= 0;
						INT32	lMaxMP		= 0;

						m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
						m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

						if (lMaxMP <= lCurrentMP)
							return FALSE;
					}
				}
				break;

			case AGPMITEM_USABLE_TYPE_SKILL_SCROLL:
				{
					AgcdItem	*pcsAgcdItem	= GetItemData(pcsItem);
					if (!pcsAgcdItem)
						return FALSE;

					if (m_pcsAgpmSkill)
					{
						UINT32	ulRecastDelay	= m_pcsAgpmSkill->GetRecastDelay(m_pcsAgpmSkill->GetSkillTemplate(((AgpdItemTemplateUsableSkillScroll *) pcsItem->m_pcsItemTemplate)->m_lSkillTID), ((AgpdItemTemplateUsableSkillScroll *) pcsItem->m_pcsItemTemplate)->m_lSkillLevel);
						if (pcsAgcdItem->m_ulLastUseTime + ulRecastDelay > GetClockCount())
							return FALSE;
					}
				}
				break;
			}
		}
		break;
	}

	return TRUE;
}

BOOL AgcmItem::SplitItem(INT32 lIID, INT32 lSplitStackCount, AgpdItemStatus eTargetStatus, INT16 nLayer, INT16 nRow, INT16 nColumn)
{
	return SendSplitItem(lIID, lSplitStackCount, eTargetStatus, nLayer, nRow, nColumn);
}

BOOL AgcmItem::SplitItem(INT32 lIID, INT32 lSplitStackCount, AuPOS *pstDestPos)
{
	return SendSplitItem(lIID, lSplitStackCount, pstDestPos);
}

BOOL	AgcmItem::BeStackableItem( INT32 lIID )
{
	AgpdItem* ppdItem = ppdItem = m_pcsAgpmItem->GetItem( lIID );
	if( !ppdItem )		return FALSE;

	return ( ((AgpdItemTemplate*)(ppdItem->m_pcsItemTemplate))->m_bStackable );
}

INT32	AgcmItem::GetItemCount( INT32 lIID )
{
	AgpdItem* ppdItem = m_pcsAgpmItem->GetItem( lIID );
	return ppdItem ? ppdItem->m_nCount : 0;
}

BOOL AgcmItem::SetCallbackInitItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_INIT_ITEM, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackEquipItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_EQUIP_ITEM, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackFieldItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_FIELD_ITEM, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackUnequipItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_UNEQUIP_ITEM, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackDetachedItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_DETACHED_ITEM, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackPreRemoveData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_PRE_REMOVE_DATA, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackSetupItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_SETUP_ITEM, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackReadTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_READ_TEMPLATE, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackCheckUseItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_CHECK_USE_ITEM, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackConfirmUseSkillBook(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_CONFIRM_USE_SKILL_BOOK, pfCallback, pClass);
}

BOOL	AgcmItem::SetCallbackUseMapItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_USE_MAP_ITEM, pfCallback, pClass);
}

BOOL	AgcmItem::SetGridItemAttachedTexture( AgpdItem* pcsAgpdItem )
{
	AgpdItemTemplate* pcsAgpdItemTemplate = (AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate);
	if ( !pcsAgpdItemTemplate )		return FALSE;

	AgcdItemTemplate* pcsAgcdItemTemplate = GetTemplateData(pcsAgpdItemTemplate);
	if( !pcsAgcdItemTemplate )		return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdItem->m_pcsGridItem);
	if( ppRwTexture )
		*ppRwTexture = pcsAgcdItemTemplate->m_pTexture;

	return TRUE;		
}

BOOL	AgcmItem::SetGridItemTemplateAttachedTexture( AgpdItemTemplate* pcsAgpdItemTemplate, AgpdGridItem *pcsGridItem )
{
	AgcdItemTemplate* pcsAgcdItemTemplate = GetTemplateData(pcsAgpdItemTemplate);
	if( !pcsAgcdItemTemplate )		return FALSE;

	if( !pcsAgcdItemTemplate->m_pTexture )
		LoadTemplateTexture(pcsAgpdItemTemplate, pcsAgcdItemTemplate);

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsGridItem);
	if( ppRwTexture )
		*ppRwTexture = pcsAgcdItemTemplate->m_pTexture;

	return TRUE;		
}

BOOL	 AgcmItem::SetGridItemAttachedSmallTexture( AgpdItemTemplate* pcsAgpdItemTemplate, AgpdGridItem *pcsGridItem )
{
	AgcdItemTemplate *pcsAgcdItemTemplate = GetTemplateData(pcsAgpdItemTemplate);
	if( !pcsAgcdItemTemplate )		return FALSE;

	if( !pcsAgcdItemTemplate->m_pSmallTexture )
		LoadTemplateTexture(pcsAgpdItemTemplate, pcsAgcdItemTemplate);

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsGridItem);
	if (ppRwTexture)
		*ppRwTexture = pcsAgcdItemTemplate->m_pSmallTexture;

	return TRUE;
}

BOOL AgcmItem::CBActionPickupItem( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmItem				*pThis				= (AgcmItem *)				pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pcsAction			= (AgpdCharacterAction *)	pCustData;

	AgpdCharacter			*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter != pcsCharacter)
		return TRUE;

	if (pcsAction->m_csTargetBase.m_eType != APBASE_TYPE_ITEM)
		return FALSE;

	return pThis->PickupItem(pThis->m_pcsAgpmItem->GetItem(pcsAction->m_csTargetBase.m_lID));
}

BOOL AgcmItem::CBTransformAppear( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmItem					*pThis				= (AgcmItem *)				pClass;
	AgpdCharacter				*pcsCharacter		= (AgpdCharacter *)			pData;
	AgcdCharacter				*pstCharacter		= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	if (pcsCharacter->m_lTID1 != pcsCharacter->m_lOriginalTID)
	{
		pThis->SetEquipAnimType(pcsCharacter, pstCharacter);
	}

	return TRUE;
}

BOOL AgcmItem::CBRestoreTransformAppear( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmItem					*pThis				= (AgcmItem *)				pClass;
	AgpdCharacter				*pcsCharacter		= (AgpdCharacter *)			pData;
	AgcdCharacter				*pstCharacter		= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);
	
	if (pcsCharacter->m_lTID1 != pcsCharacter->m_lOriginalTID)
	{
		pThis->SetEquipAnimType(pcsCharacter, pstCharacter);
	}

	pThis->RefreshEquipItems( pcsCharacter );
	return TRUE;
}

BOOL AgcmItem::CBEvolutionAppear( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmItem					*pThis				= (AgcmItem *)				pClass;
	AgpdCharacter				*pcsCharacter		= (AgpdCharacter *)			pData;
	AgcdCharacter				*pstCharacter		= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	if (pcsCharacter->m_lTID1 != pcsCharacter->m_lOriginalTID)
	{
		pThis->SetEquipAnimType(pcsCharacter, pstCharacter);
	}

	pThis->RefreshEquipItems( pcsCharacter );
	return TRUE;
}

BOOL AgcmItem::CBRestoreEvolution( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmItem					*pThis				= (AgcmItem *)				pClass;
	AgpdCharacter				*pcsCharacter		= (AgpdCharacter *)			pData;
	AgcdCharacter				*pstCharacter		= pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);
	
	if (pcsCharacter->m_lTID1 != pcsCharacter->m_lOriginalTID)
	{
		pThis->SetEquipAnimType(pcsCharacter, pstCharacter);
	}

	pThis->RefreshEquipItems( pcsCharacter );
	return TRUE;
}

BOOL AgcmItem::SetCallbackEquipDefaultArmour(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_EQUIP_DEFAULT_ARMOUR, pfCallback, pClass);
}

BOOL AgcmItem::SetCallbackLoadClump(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMITEM_CB_ID_LOAD_CLUMP, pfCallback, pClass);
}

BOOL AgcmItem::SetPreLightForAllItem(FLOAT fOffset)
{
	if ((fOffset < 0.0f) || (fOffset > 2.0f))
		return FALSE;

	INT32 lIndex = 0;
	for(AgpdItem* pstAgpdItem = m_pcsAgpmItem->GetItemSequence(&lIndex); pstAgpdItem; pstAgpdItem = m_pcsAgpmItem->GetItemSequence(&lIndex) )
	{
		AgcdItem* pstAgcdItem = GetItemData(pstAgpdItem);
		if( !pstAgcdItem )							return FALSE;

		AgcdItemTemplate* pstAgcdItemTemplate = pstAgcdItem->m_pstAgcdItemTemplate;
		if( !pstAgcdItemTemplate->m_pcsPreData )	return FALSE;

		if( pstAgcdItem->m_pstClump )
		{
			FLOAT fRed		= min( (FLOAT)(pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.red) * fOffset, 255.f );
			FLOAT fBlue		= min( (FLOAT)(pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.green) * fOffset, 255.f );
			FLOAT fGreen	= min( (FLOAT)(pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.blue) * fOffset, 255.f );
			FLOAT fAlpha	= min( (FLOAT)(pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.alpha) * fOffset, 255.f );

			RwRGBA	stRGBA;
			stRGBA.red		= (UINT8)(fRed);
			stRGBA.blue		= (UINT8)(fBlue);
			stRGBA.green	= (UINT8)(fGreen);
			stRGBA.alpha	= (UINT8)(fAlpha);

			AcuObject::SetClumpPreLitLim( pstAgcdItem->m_pstClump, &stRGBA );
		}
	}

	return TRUE;
}

BOOL AgcmItem::AddAttachedAtomic(AttachedAtomics *pcsAttachedAtomics, RpAtomic *pstAtomic)
{
	if (pcsAttachedAtomics->pcsList)
	{
		AttachedAtomicList	*pcsCurrent	= pcsAttachedAtomics->pcsList;
		while (pcsCurrent)
		{
			if (pcsCurrent->m_pstAtomic)
			{
				// 이미 리스트에 있다.
				if (pcsCurrent->m_pstAtomic == pstAtomic)
					return TRUE;
			}
			else
			{ // 할당된 공간이 있다.
				pcsCurrent->m_pstAtomic = pstAtomic;
				return TRUE;
			}

			if (!pcsCurrent->m_pcsNext)
			{ // 할당된 공간이 없다.
				pcsCurrent->m_pcsNext = new AttachedAtomicList();
				if (!pcsCurrent->m_pcsNext)
					return FALSE;

				pcsCurrent->m_pcsNext->m_pstAtomic	= pstAtomic;
				return TRUE;
			}

			pcsCurrent = pcsCurrent->m_pcsNext;
		}

		return FALSE;
	}
	else
	{ // 할당이 전혀 되어있질 않다.
		pcsAttachedAtomics->pcsList	= new AttachedAtomicList();
		if (!pcsAttachedAtomics->pcsList)
			return FALSE;

		pcsAttachedAtomics->pcsList->m_pstAtomic	= pstAtomic;
	}

	return TRUE;
}

BOOL AgcmItem::RemoveAllAttachedAtomics(AttachedAtomics *pcsAttachedAtomics)
{
	AttachedAtomicList	*pcsCurrent	= pcsAttachedAtomics->pcsList;
	AttachedAtomicList	*pcsNext;
	while (pcsCurrent)
	{
		pcsNext	= pcsCurrent->m_pcsNext;
		delete pcsCurrent;

		pcsCurrent	= pcsNext;
	}

	pcsAttachedAtomics->pcsList	= NULL;

	return TRUE;
}

INT32 AgcmItem::GetNumAttachedAtomics(AttachedAtomics *pcsAttachedAtomics)
{
	INT32	lNum = 0;

	AttachedAtomicList	*pcsCurrent	= pcsAttachedAtomics->pcsList;

	while (pcsCurrent)
	{
		++lNum;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return lNum;
}

RpAtomic *AgcmItem::CopyAtomic(RpAtomic *pstAtomic)
{
	RwMemory	stMemory;
	stMemory.length = RpAtomicStreamGetSize(pstAtomic);

	RwStream* pstWriteStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &stMemory);
	if( !pstWriteStream )		return NULL;
	if( !RpAtomicStreamWrite( pstAtomic, pstWriteStream ) )	return NULL;

	RwStreamClose(pstWriteStream, &stMemory);

	RwStream* pstReadStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &stMemory);
	if( !pstReadStream )
	{
		RwFree(stMemory.start);
		return NULL;
	}

	if( !RwStreamFindChunk( pstReadStream, rwID_ATOMIC, NULL, NULL ) )
	{
		RwFree(stMemory.start);
		return NULL;
	}

	RpAtomic* pstNewAtomic = RpAtomicStreamRead(pstReadStream);
	RwStreamClose( pstReadStream, &stMemory );

	RwFree(stMemory.start);

	return pstNewAtomic;
}

RpAtomic *AgcmItem::CreateArmourPartClumpCB(RpAtomic *atomic, void *data)
{
	AgcdItemCreateArmourPartParams	*pstParams			= (AgcdItemCreateArmourPartParams *)(data);
	AgcmItem						*pcsThis			= (AgcmItem *)(pstParams->m_pvClass);

	if( pstParams->m_lPartID == atomic->iPartID )
	{
		pcsThis->LockFrame();

		RpAtomic* pstNewAtomic = RpAtomicClone(atomic);
		if(!pstNewAtomic)
		{
			ASSERT(!"AgcmItem::CreateArmourPartClumpCB() Cloning Error!!!");

			pcsThis->UnlockFrame();
			return atomic;
		}

#ifdef _DEBUG
		pstNewAtomic->pvApBase = atomic->pvApBase;
#endif

		RwFrame* pstFrame = RpAtomicGetFrame(pstNewAtomic);
		if( !pstFrame )
		{
			pstFrame = RwFrameCreate();
			if( !pstFrame )
			{
				ASSERT(!"AgcmItem::CreateArmourPartClumpCB() Frame Error!!!");

				pcsThis->UnlockFrame();
				return atomic;
			}

			RpAtomicSetFrame(pstNewAtomic, pstFrame);
		}

		RpClumpAddAtomic(pstParams->m_pstBaseClump, pstNewAtomic);
		RwFrameAddChild(RpClumpGetFrame(pstParams->m_pstBaseClump), pstFrame);

		TRACE("AgcmItem::CreateArmourPartClumpCB() Clump(Src %x, Base %x), Atomic(Src %x, Clone %x)\n", RpAtomicGetClump(atomic), pstParams->m_pstBaseClump, atomic, pstNewAtomic);

		pcsThis->UnlockFrame();
	}

	return atomic;
}

/******************************************************************************
* Purpose :
*
* 051203. BOB
******************************************************************************/
RpClump *AgcmItem::CreateArmourPartClump(RpClump *pstArmourClump, INT32 lPartID)
{
	if (!pstArmourClump)
		return NULL;

	RwStream	*pstStream	= NULL;
	RpClump		*pstClump	= NULL;

	pstClump				= RpClumpCreate();
	if(!pstClump)
		return NULL;

	AgcdItemCreateArmourPartParams stParams;
	stParams.m_lPartID		= lPartID;
	stParams.m_pstBaseClump	= pstClump;
	stParams.m_pvClass		= this;	

	LockFrame();

	RpClumpSetFrame(pstClump, RwFrameCreate());

	UnlockFrame();

	RpClumpForAllAtomics(pstArmourClump, CreateArmourPartClumpCB, (PVOID)(&stParams));

	return pstClump;
}

/******************************************************************************
******************************************************************************/

BOOL	AgcmItem::CBLoadTemplate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmItem *			pcsThis = (AgcmItem *) pClass;
	AgpdItem *			pcsItem = (AgpdItem *) pData;
	AgcdItem *			pstAgcdItem = pcsThis->GetItemData(pcsItem);

	ASSERT(pcsItem->m_eType == APBASE_TYPE_ITEM);

	if (pstAgcdItem->m_lStatus & AGCDITEM_STATUS_REMOVED)
		return TRUE;

	if (!pcsThis->LoadTemplateData((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate, pstAgcdItem->m_pstAgcdItemTemplate))
	{
		ASSERT(!"AgcmItem::CBLoadTemplate() Error LoadTemplate()!!!");
		return FALSE;
	}

	pcsThis->SetGridItemAttachedTexture(pcsItem);

	// 여기는 완전 임시 코드( AgpdItem과 같이 뜯어 고쳐야 된다.) 일단은 되게만 하자...
	pcsThis->EnumCallback(AGCMITEM_CB_ID_SETUP_ITEM, pcsItem, NULL);

	AuAutoLock	lock(pcsItem->m_Mutex);
	if (lock.Result())
		pcsThis->SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_LOADED);

	return TRUE;
}

BOOL	AgcmItem::CBMakeItemClump(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::CBMakeItemClump");

	AgpdItem* pcsItem = (AgpdItem *) pData;
	if( !pcsItem )			return FALSE;

	AuAutoLock	lock(pcsItem->m_Mutex);
	if( !lock.Result() )	return FALSE;

	AgcmItem* pcsThis = (AgcmItem *) pClass;
	AgcdItem* pstAgcdItem = pcsThis->GetItemData(pcsItem);
	
	BOOL bRet = pcsThis->MakeItemClump( pcsItem );
	pcsThis->SetItemStatus( pstAgcdItem, AGCDITEM_STATUS_MAKE_ITEM_CLUMP );

	return bRet;
}

BOOL	AgcmItem::CBSetItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::CBSetItem");

	AgcmItem *			pcsThis = (AgcmItem *) pClass;
	AgpdItem *			pcsItem = (AgpdItem *) pData;
	if( !pcsItem )		return TRUE;
		
	pcsThis->SetGridItemAttachedTexture( pcsItem );

	AgcdItem* pstAgcdItem = pcsThis->GetItemData(pcsItem);
	ApAutoWriterLock csLock( pstAgcdItem->m_rwLock );

	// 만약 그사이에 아이템이 없어졌으면, return TRUE
	// 하지만 그사이에 아이템이 지워지면 여기까지 못와야 한다. Remove될때 ResourceLoader에서 RemoveEntry하기 때문에...
	ASSERT(pcsThis->IsItemReady(pcsItem, pstAgcdItem));
	if ( !pcsThis->IsItemReady(pcsItem, pstAgcdItem) )
		return TRUE;

	AgpdCharacter* pcsCharacter = pcsItem->m_pcsCharacter;
	if( pcsCharacter )
	{
		AgcdCharacter* pstAgcdCharacter = pcsThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

		// 이미 Remove된 케릭터라면 그냥 return
		// 하지만, 역시 케릭터가 지워지면 여기까지 오면 안된다. Remove될때 ResourceLoader에서 RemoveEntry하기 때문에...
		ASSERT(!(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED));
		if( (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED) )
			return TRUE;
	}

	BOOL bRet = FALSE;
	switch( pcsItem->m_eStatus )
	{
	case AGPDITEM_STATUS_FIELD:
		bRet = pcsThis->SetFieldItem(pcsItem);
		break;
	case AGPDITEM_STATUS_EQUIP:
		bRet = pcsThis->EquipItem(pcsItem->m_pcsCharacter, pcsItem);
		break;
	default:
		ASSERT(!"AgcmItem::CBSetItem() Error Invalid Item Status!!!");
		break;
	}

	return bRet;
}

BOOL AgcmItem::ReleaseGridItemAttachedTexture( AgpdItemTemplate* pcsAgpdItemTemplate )
{
	if ( !pcsAgpdItemTemplate )				return FALSE;

	AgcdItemTemplate* pcsAgcdItemTemplate = GetTemplateData( pcsAgpdItemTemplate );
	if ( !pcsAgcdItemTemplate->m_pTexture )	return FALSE;

	pcsAgcdItemTemplate->m_lAttachedToGrid -= 1;

	if( !pcsAgcdItemTemplate->m_bLoaded )
	{
		RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdItemTemplate->m_pcsGridItem);

		RwTextureDestroy( pcsAgcdItemTemplate->m_pTexture );
		pcsAgcdItemTemplate->m_pTexture	= NULL;
		if( ppRwTexture )
			*ppRwTexture = NULL;
	}

	return TRUE;		
}

BOOL AgcmItem::SetGridItemAttachedTexture( AgpdItemTemplate* pcsAgpdItemTemplate )
{
	if( !pcsAgpdItemTemplate )		return FALSE;

	RwTexture**	ppRwTexture	= m_pcsAgcmUIControl->GetAttachGridItemTextureData(pcsAgpdItemTemplate->m_pcsGridItem);
	if( !ppRwTexture )				return FALSE;
	
	AgcdItemTemplate* pcsAgcdItemTemplate = GetTemplateData(pcsAgpdItemTemplate);

	if (!pcsAgcdItemTemplate->m_pTexture)
	{
		m_pcsAgcmResourceLoader->SetTexturePath(m_szIconTexturePath);

		if (!pcsAgcdItemTemplate->m_pTexture)
		{
			if (pcsAgcdItemTemplate->m_pcsPreData)
			{
				pcsAgcdItemTemplate->m_pTexture = RwTextureRead(pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName, NULL);
			}
			else
			{
				CHAR	szTemp[256];
				memset(szTemp, 0, sizeof (CHAR) * 256);

				if (pcsAgcdItemTemplate->GetTextureID() > 0)
				{
					pcsAgcdItemTemplate->GetTextureName(szTemp);

					if (szTemp && szTemp[0])
						pcsAgcdItemTemplate->m_pTexture	= GetTexture(szTemp);
				}
			}
		}
	}

	if( !pcsAgcdItemTemplate->m_pTexture )
		return FALSE;

	RwTextureSetFilterMode( pcsAgcdItemTemplate->m_pTexture, rwFILTERNEAREST );
	RwTextureSetAddressing( pcsAgcdItemTemplate->m_pTexture, rwTEXTUREADDRESSCLAMP );

	*ppRwTexture = pcsAgcdItemTemplate->m_pTexture;

	pcsAgcdItemTemplate->m_lAttachedToGrid += 1;

	return TRUE;		
}

BOOL AgcmItem::IsItemValid(AgpdItem *pcsItem)
{
	if (!pcsItem || pcsItem->m_eType != APBASE_TYPE_ITEM)
		return FALSE;

	return IsItemValid(pcsItem, GetItemData(pcsItem));
}

BOOL AgcmItem::IsItemValid(AgpdItem *pcsItem, AgcdItem *pstItem)
{
	if( !pcsItem || !pstItem || pcsItem->m_eType != APBASE_TYPE_ITEM )
		return FALSE;

	if (!IsItemReady(pcsItem, pstItem))
		return FALSE;

	// Field 나 Equip 인 경우에만 MakeItemClump를 한다.
	switch (pcsItem->m_eStatus)
	{
	case AGPDITEM_STATUS_EQUIP:
		{
			if (!(pstItem->m_lStatus & AGCDITEM_STATUS_ATTACH))
				return FALSE;
		}
		break;
	case AGPDITEM_STATUS_FIELD:
		{
			if (pstItem->m_lStatus & AGCDITEM_STATUS_MAKE_ITEM_CLUMP)
				return TRUE;
		}
		break;

	default:
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmItem::IsItemReady(AgpdItem *pcsItem)
{
	if (!pcsItem || pcsItem->m_eType != APBASE_TYPE_ITEM)
		return FALSE;

	return IsItemReady(pcsItem, GetItemData(pcsItem));
}

BOOL AgcmItem::IsItemReady(AgpdItem *pcsItem, AgcdItem *pstItem)
{
	if (!pcsItem || !pstItem || pcsItem->m_eType != APBASE_TYPE_ITEM)
		return FALSE;

	if ((pstItem->m_lStatus & AGCDITEM_STATUS_LOADED) && !(pstItem->m_lStatus & AGCDITEM_STATUS_REMOVED))
		return TRUE;

	return FALSE;
}

VOID AgcmItem::SetItemStatus(AgpdItem *pcsItem, INT32 lStatus)
{
	if( pcsItem )
		SetItemStatus(GetItemData(pcsItem), lStatus);
}

VOID AgcmItem::SetItemStatus(AgcdItem *pstItem, INT32 lStatus)
{
	if( pstItem )
		pstItem->m_lStatus |= lStatus;
}

VOID AgcmItem::ResetItemStatus(AgpdItem *pcsItem, INT32 lStatus)
{
	if( pcsItem )
		ResetItemStatus(GetItemData(pcsItem), lStatus);
}

VOID AgcmItem::ResetItemStatus(AgcdItem *pstItem, INT32 lStatus)
{
	if( pstItem )
		pstItem->m_lStatus &= ~lStatus;
}

VOID AgcmItem::SetAgcdRide( AgcmRide* pAgcmRide )
{
	m_pcsAgcmRide = pAgcmRide;
}

VOID AgcmItem::CancelCoolDown()
{
	m_pAgcmUICooldown->CancelCooldown(m_lCooldownID);
}

BOOL AgcmItem::CBUnuseItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmItem		*pThis			= (AgcmItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)	pData;

	AgpdCharacter	*pcsItemOwner	= pcsItem->m_pcsCharacter;

	if(pcsItemOwner == NULL) return FALSE;

	BOOL  bUnseResult = pThis->ReceiveUnuseItem(pcsItem);

	return bUnseResult;
}

BOOL AgcmItem::CBPauseItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pData || !pClass )		return FALSE;

	AgcmItem		*pThis			= (AgcmItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)	pData;

	AgpdCharacter*	pcsItemOwner	= pcsItem->m_pcsCharacter;
	if( !pcsItemOwner )		return FALSE;

	return pThis->ReceivePauseItem(pcsItem);
}


//실제 클라이언트로 사용중지가 인증되었을 경우 불린다.
BOOL AgcmItem::ReceiveUnuseItem( AgpdItem *pcsItem )
{
	m_pcsAgpmItem->StopCashItemTimer(pcsItem);

	// 캐쉬아이템이고 아바타 아이템이면 원래의 장비를 다시 입혀준다.


	m_pcsAgpmItem->ProcessRefreshUI( pcsItem->m_ulCID, AGPDITEM_STATUS_CASH_INVENTORY, AGPDITEM_STATUS_CASH_INVENTORY );
	return TRUE;
}

//사용중지 패킷을 서버로 전송한다.
BOOL AgcmItem::UnuseItem(AgpdItem *pcsItem)
{
	if( !pcsItem )		return FALSE;

	// 2006.02.06. steeple
	// 캐릭터가 액션블럭 상태라면 보내지 않는다.
	AgpdCharacter* pcsCharacter = pcsItem->m_pcsCharacter;
	if( !pcsCharacter )
		pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	if( !pcsCharacter )
		return FALSE;

	if(m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter, AGPDCHAR_ACTION_BLOCK_TYPE_SKILL))
	{
		if(m_pcsAgpmSystemMessage)
			SystemMessage.ProcessSystemMessage( 0 , AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_CANNOT_UNUSE_STATUS );

		return FALSE;
	}

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmItem->MakePacketItemUnuse(pcsItem, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult	= SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return (bSendResult);
}

//캐쉬 아이템이 사용될경우 불린다.
BOOL AgcmItem::ReceiveUseItem( AgpdItem *pcsItem )
{
	m_pcsAgpmItem->StartCashItemTimer(pcsItem);

	m_pcsAgpmItem->ProcessRefreshUI( pcsItem->m_ulCID, AGPDITEM_STATUS_CASH_INVENTORY, AGPDITEM_STATUS_CASH_INVENTORY );
	return TRUE;
}

// 캐쉬 아이템이 일시 정지 되었을 때 불린다.
BOOL AgcmItem::ReceivePauseItem(AgpdItem* pcsItem)
{
	if( !pcsItem )		return FALSE;

	m_pcsAgpmItem->ProcessRefreshUI(pcsItem->m_ulCID, AGPDITEM_STATUS_CASH_INVENTORY, AGPDITEM_STATUS_CASH_INVENTORY);
	return TRUE;
}

BOOL AgcmItem::IsTeleportScroll( AgpdItem* ppdItem )
{
	if( !ppdItem ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ( AgpdItemTemplate* )ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplate ) return FALSE;

	if( ppdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ) return FALSE;
	AgpdItemTemplateUsable* ppdItemTemplateUsable = ( AgpdItemTemplateUsable* )ppdItemTemplate;

	if( ppdItemTemplateUsable->m_nUsableItemType != AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL ) return FALSE;
	AgpdItemTemplateUsableTeleportScroll* ppdItemTemplateTeleportScroll = ( AgpdItemTemplateUsableTeleportScroll* )ppdItemTemplateUsable;

	return TRUE;
}

BOOL AgcmItem::OpenProgressBarWaiting( AgpdItem* ppdItem, INT32 nWaitDuration )
{
	if( !ppdItem || nWaitDuration <= 0 ) return FALSE;
	if( m_bIsNowWaitingForUseItem )
	{
		// 이동문서류의 아이템 사용대기중에는 다른 이동문서를 사용할수 없다는 메세지를 보여준다.
		return TRUE;
	}

	AgcmUICooldown* pcmUICoolDown = ( AgcmUICooldown* )GetModule( "AgcmUICooldown" );
	if( !pcmUICoolDown ) return FALSE;
	if( !ppdItem ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ( AgpdItemTemplate* )ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplate ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if( !pcmCharacter || !ppmCharacter ) return FALSE;
 
	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 이동중이었으면 스톱
	pcmCharacter->StopSelfCharacter();
	ppmCharacter->StopCharacter( ppdCharacter, NULL );
	pcmCharacter->SendStopCharacter();

	m_bIsNowWaitingForUseItem = pcmUICoolDown->StartCooldown( m_nCoolDownWaitUseItemID, nWaitDuration, NULL, TRUE, ppdItem );
	return m_bIsNowWaitingForUseItem;
}

BOOL AgcmItem::OpenProgressBarWaitingReturn( INT32 nWaitDuration )
{
	if( nWaitDuration <= 0 ) return FALSE;
	if( m_bIsNowWaitingForUseItem )
	{
		// 이동문서류의 아이템 사용대기중에는 다른 이동문서를 사용할수 없다는 메세지를 보여준다.
		return TRUE;
	}

	AgcmUICooldown* pcmUICoolDown = ( AgcmUICooldown* )GetModule( "AgcmUICooldown" );
	if( !pcmUICoolDown ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if( !pcmCharacter || !ppmCharacter ) return FALSE;
 
	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 이동중이었으면 스톱
	pcmCharacter->StopSelfCharacter();
	ppmCharacter->StopCharacter( ppdCharacter, NULL );
	pcmCharacter->SendStopCharacter();

	m_bIsNowWaitingForUseItem = pcmUICoolDown->StartCooldown( m_nCoolDownWaitUseItemReturnID, nWaitDuration, NULL, TRUE, NULL );
	return m_bIsNowWaitingForUseItem;
}

BOOL AgcmItem::IsNoneDurabilityItem( AgpdItem* ppdItem )
{
	if( !ppdItem ) return TRUE;

	AgpdItemTemplate* ppdItemTemplate = ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplate ) return TRUE;
	if( ppdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP ) return TRUE;

	AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemTemplate;
	switch( ppdItemTemplateEquip->m_nKind )
	{
	case AGPMITEM_EQUIP_KIND_NECKLACE :
	case AGPMITEM_EQUIP_KIND_RING :			return TRUE;		break;
	}

	// 일본의 경우 탈것도 내구도가 존재하지 않는 아이템이다.
	if( g_eServiceArea == AP_SERVICE_AREA_JAPAN )
	{
		if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_RIDE ) return TRUE;
	}

	return FALSE;
}

BOOL AgcmItem::CBAfterCoolDownWaitUseItem( void* pData, void* pClass, void* pCustomData )
{
	if( !pClass || !pCustomData ) return FALSE;

	AgcmItem* pcmItem = ( AgcmItem* )pClass;
	AgpdItem* ppdItem = ( AgpdItem* )pCustomData;

	// 이동문서류의 타겟ID는 물론.. 나 자신이다.
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmItem->GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmItem->GetModule( "AgpmCharacter" );
	if( !pcmCharacter || !ppmCharacter ) return FALSE;
	
	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 이동중이었으면 스톱
	pcmCharacter->StopSelfCharacter();
	ppmCharacter->StopCharacter( ppdCharacter, NULL );
	pcmCharacter->SendStopCharacter();

	INT32 nTargetID = ppdCharacter->m_lID;

	// 아이템 사용한다고 서버로 보낸댜.
	INT16 nPacketLength = 0;
	PVOID pPacket = pcmItem->m_pcsAgpmItem->MakePacketItemUse( ppdItem, &nPacketLength, nTargetID );
	if( !pPacket || nPacketLength < 1 )	return FALSE;

	BOOL bSendResult = pcmItem->SendPacket( pPacket, nPacketLength );
	pcmItem->m_pcsAgpmItem->m_csPacket.FreePacket( pPacket );

	// 대기중 사용했던 변수들 초기화
	pcmItem->m_bIsNowWaitingForUseItem = FALSE;
	pcmItem->m_nWaitingItemID = -1;

	return bSendResult;
}

BOOL AgcmItem::CBAfterCoolDownWaitUseItemReturn( void* pData, void* pClass, void* pCustomData )
{
	if( !pClass ) return FALSE;

	AgcmItem* pcmItem = ( AgcmItem* )pClass;

	// 이동문서류의 타겟ID는 물론.. 나 자신이다.
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmItem->GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmItem->GetModule( "AgpmCharacter" );
	if( !pcmCharacter || !ppmCharacter ) return FALSE;
	
	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 이동중이었으면 스톱
	pcmCharacter->StopSelfCharacter();
	ppmCharacter->StopCharacter( ppdCharacter, NULL );
	pcmCharacter->SendStopCharacter();

	pcmItem->SendUseReturnScroll();

	pcmItem->m_bIsNowWaitingForUseItem = FALSE;
	pcmItem->m_nWaitingItemID = -1;
	return TRUE;
}

BOOL AgcmItem::CBAddItemExtraData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass || !pData ) return FALSE;
	
	AgcmItem* pcmItem = ( AgcmItem* )pClass;
	AgpdItem* ppdItem = ( AgpdItem* )pData;
	AgpdSealData* pSealData = ( AgpdSealData* )pCustData;

	AgcdItem* pcdItem = pcmItem->GetItemData( ppdItem );
	if( pcdItem )
	{
		if( pSealData )
		{
			pcdItem->m_bHaveExtraData = TRUE;
			memcpy( &pcdItem->m_SealData, pSealData, sizeof( AgpdSealData ) );
		}
		else
		{
			pcdItem->m_bHaveExtraData = FALSE;
			memset( &pcdItem->m_SealData, 0, sizeof( AgpdSealData ) );
		}
	}

	return TRUE;
}

// 드래곤시온이면 변신상태에서 벗지 못하는 아이템이 있다. 그것을 확인하는 함수.
BOOL AgcmItem::IsDisableUnEquipItem( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter )
{
	if( !ppdItem || !ppdItem->m_pcsItemTemplate || ppdItem->m_eStatus != AGPDITEM_STATUS_EQUIP )
		return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmFactor || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdTargetCharacter = ppdCharacter;
	if( !ppdTargetCharacter )
	{
		// 매개변수가 NULL 인 경우는 자기 자신으로 가정한다.
		ppdTargetCharacter = pcmCharacter->GetSelfCharacter();
		if( !ppdTargetCharacter ) return FALSE;
	}

	AuRaceType eRaceType = ( AuRaceType )ppmFactor->GetRace( &ppdTargetCharacter->m_csFactor );
	AuCharClassType eClassType = pcmCharacter->GetClassTypeByTID( ppdTargetCharacter->m_lTID1 );

	// 드래곤시온에 대해 아래의 제한사항을 추가
	if( eRaceType == AURACE_TYPE_DRAGONSCION )
	{
		AgpdItemTemplate* ppdItemTemplate = ppdItem->m_pcsItemTemplate;
		if( !ppdItemTemplate || ppdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP ) return FALSE;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemTemplate;
		if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			// 무기의 경우...
			AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplateEquip;
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :
			case AUCHARCLASS_TYPE_MAGE :
				{
					if( ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON || 
						ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON )
					{
						// 슬레이어와 서머너는 제논을 벗을 수 없다.
						return TRUE;
					}
				}
				break;

			case AUCHARCLASS_TYPE_RANGER :
				{
					if( ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON || 
						ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON )
					{
						// 오비터는 카론을 벗을 수 없다.
						return TRUE;
					}
				}
				break;
			}			
		}
	}

	return FALSE;
}
