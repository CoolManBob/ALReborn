#include "AlefAdminManager.h"
#include "CDataManager.h"
#include "AgcmAdmin.h"
#include "AgcmTargeting.h"
#include "AlefAdminAPI.h"


#define INTERVAL_REQUEST_CURRENT_USER_COUNT				2000


CDataManager::CDataManager( void )
{
	m_pAgpmAdmin = NULL;
	m_pAgcmAdmin = NULL;

	m_bIsUseAdmin = FALSE;
	m_bIsDialogOpen = FALSE;

	memset( &m_stSelfAdminInfo, 0, sizeof( stAgpdAdminInfo ) );
	memset( &m_stOldSearchData, 0, sizeof( stAgpdAdminSearch ) );
	memset( &m_stReceivedCharacterData, 0, sizeof( stAgpdAdminCharData ) );
	memset( &m_stSelectCharacterData, 0, sizeof( stAgpdAdminPickingData ) );

	m_dwLastRequestCurrentUserTickCount = 0;
}

CDataManager::~CDataManager( void )
{
	m_pAgpmAdmin = NULL;
	m_pAgcmAdmin = NULL;

	m_dwLastRequestCurrentUserTickCount = 0;
}

BOOL CDataManager::OnRegisterCallBack( void )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;
	if( !ppmAdmin->SetCallbackAddAdmin( CBAddAdmin, this ) ) return FALSE;
	if( !ppmAdmin->SetCallbackSearchResult( CBReceiveSearchResult, this ) ) return FALSE;
	if( !ppmAdmin->SetCallbackCharData( CBReceiveCharacterDataSub, this ) ) return FALSE;
	if( !ppmAdmin->SetCallbackCharEdit( CBReceiveCharacterModifiied, this ) ) return FALSE;
	if( !ppmAdmin->SetCallbackServerInfo( CBReceiveServerInfo, this ) ) return FALSE;
	if( !ppmAdmin->SetCallbackNumCurrentUser( CBNumCurrentUser, this ) ) return FALSE;
	if( !ppmAdmin->SetCallbackCustom( CBReceiveCustom, this ) )	return FALSE;
	if( !ppmAdmin->SetCallbackItemResult( CBReceiveItemOperationResult, this ) ) return FALSE;

	AgcmTargeting* pcmTargeting = ( AgcmTargeting* )pcmAdmin->GetModule( "AgcmTargeting" );
	if( !pcmTargeting ) return FALSE;
	if( !pcmTargeting->SetCallbackLButtonDownPickSomething( CBSelectCharacter, this ) ) return FALSE;
	if( !pcmTargeting->SetCallbackLButtonDownPickNothing( CBSelectGround, this ) ) return FALSE;

	AgpmSkill* ppmSkill = ( AgpmSkill* )pcmAdmin->GetModule( "AgpmSkill" );
	if( !ppmSkill ) return FALSE;
	if( !ppmSkill->SetCallbackInitSkill( CBReceiveCharacterSkill, this ) ) return FALSE;
	if( !ppmSkill->SetCallbackUpdateSkill( CBReceiveCharacterSkill, this ) ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmAdmin->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;
	if( !ppmCharacter->SetCallbackInitChar( CBReceiveCharacterData, this ) ) return FALSE;
	if( !ppmCharacter->SetCallbackUpdateStatus( CBReceiveCharacterData, this ) ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )pcmAdmin->GetModule( "AgpmItem" );
	if( !ppmItem ) return FALSE;
	if( !ppmItem->SetCallbackInventory( CBReceiveCharacterItem, this ) ) return FALSE;
	if( !ppmItem->SetCallbackInventoryForAdmin( CBReceiveCharacterItem, this ) ) return FALSE;
	if( !ppmItem->SetCallbackEquipForAdmin( CBReceiveCharacterItem, this ) ) return FALSE;

	AlefAdminManager::Instance()->m_pMainDlg->SetCBSearch( CBDlgSendSearchCharacter );
	return TRUE;
}

BOOL CDataManager::OpenMainDlg( void )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;
	//if( !ppmAdmin->IsAdminCharacter( ppdCharacter ) ) return FALSE;

	if( !m_bIsDialogOpen )
	{
		AlefAdminManager::Instance()->OpenMainDlg();
		m_bIsDialogOpen = TRUE;
	}

	return TRUE;
}

BOOL CDataManager::CloseMainDlg( void )
{
	m_bIsDialogOpen = FALSE;
	return TRUE;
}

BOOL CDataManager::OnIdle( void )
{
	ProcessRequestCurrentUserCount();
	return TRUE;
}


BOOL CDataManager::OnSelectCharacter( INT32 nType, INT32 nCID )
{
	if( nType == ACUOBJECT_TYPE_CHARACTER )
	{
		AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
		AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
		if( !ppmAdmin || !pcmAdmin ) return FALSE;	

		AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmAdmin->GetModule( "AgpmCharacter" );
		if( !ppmCharacter ) return FALSE;

		AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( nCID );
		if( ppdCharacter )
		{
			memset( &m_stSelectCharacterData, 0, sizeof( stAgpdAdminPickingData ) );

			m_stSelectCharacterData.m_eType = nType;
			m_stSelectCharacterData.m_lCID = nCID;
			strcpy( m_stSelectCharacterData.m_szName, ppdCharacter->m_szID );

			// 원래는 캐릭터를 선택하면 먼가 행동을 했던듯 하지만.. 왠일인지 주석처리가 되어 있다.. 왜일까..
			//ADMIN_Main_AddObject(&stPickingData);
		}
	}

	return TRUE;
}

BOOL CDataManager::OnSelectGround( void* pPos )
{
	AuPOS* puPos = ( AuPOS* )pPos;
	if( !puPos ) return FALSE;

	// 원래는 지면을 찍으면 그곳으로 이동.. 하도록 되어 있었던듯 하지만.. 주석처리되어 있다.. 
	//ADMIN_Main_SetLButtonDownPos( puPos );
	//ADMIN_CharacterMove_SetLButtonDownPos( puPos );
	return TRUE;
}

BOOL CDataManager::OnAddAdmin( void* pCharacter, void* pAdminInfo )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	stAgpdAdminInfo* ppdAdminInfo = ( stAgpdAdminInfo* )pAdminInfo;
	if( !ppdCharacter || !ppdAdminInfo ) return FALSE;

	if( ppdSelfCharacter->m_lID == ppdAdminInfo->m_lCID )
	{
		AgpdAdmin* ppdAttachedData = ppmAdmin->GetADCharacter( ppdCharacter );
		if( !ppdAttachedData ) return FALSE;

		ppdAttachedData->m_lAdminCID = ppdCharacter->m_lID;
		ppdAttachedData->m_lAdminLevel = ppdAdminInfo->m_lAdminLevel;

		SetSelfAdminInfo( ppdAdminInfo );
		//OpenMainDlg();

		// DLL Dialog 에도 해준다. 
		AlefAdminManager::Instance()->SetSelfAdminInfo( ppdAdminInfo );
	}

	return TRUE;
}

BOOL CDataManager::OnAddCharacterData( void* pCharacter )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter || !ppdCharacter->m_pcsCharacterTemplate ) return FALSE;
	if( !strlen( ppdCharacter->m_szID ) ) return TRUE;
	if( strcmp( m_stOldSearchData.m_szSearchName, ppdCharacter->m_szID ) ) return TRUE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmAdmin->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;
	if( !ppmCharacter->IsPC( ppdCharacter ) ) return TRUE;

	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceive( ppdCharacter );
}

BOOL CDataManager::OnAddCharacterDataSub( void* pCharacterSub )
{
	stAgpdAdminCharDataSub* ppdDataSub = ( stAgpdAdminCharDataSub* )pCharacterSub;
	if( !ppdDataSub ) return FALSE;
	if( strcmp( m_stOldSearchData.m_szSearchName, ppdDataSub->m_szCharName ) ) return TRUE;

	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceiveSub( ppdDataSub );
}

BOOL CDataManager::OnAddCharacterSkill( void* pSkill )
{
	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->m_pSkillView->OnReceive( ( AgpdSkill* )pSkill );
}

BOOL CDataManager::OnAddCharacterItem( void* pItem )
{
	AgpdItem* ppdItem = ( AgpdItem* )pItem;
	if( !ppdItem ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ( AgpdItemTemplate* )ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplate ) return FALSE;

	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmAdmin->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( ppdItem->m_ulCID );
	if( !ppdCharacter ) return FALSE;
    if( strcmp( m_stOldSearchData.m_szSearchName, ppdCharacter->m_szID ) ) return TRUE;

	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->m_pItemView->OnReceive( ppdItem );
}

BOOL CDataManager::OnAddServerInfo( void* pServerInfo )
{
	stAgpdAdminServerInfo* ppdServerInfo = ( stAgpdAdminServerInfo* )pServerInfo;
	if( !ppdServerInfo ) return FALSE;

	if( FindServerInfo( ppdServerInfo ) ) return TRUE;

	stAgpdAdminServerInfo NewServerInfo;
	memcpy( &NewServerInfo, ppdServerInfo, sizeof( stAgpdAdminServerInfo ) );
	NewServerInfo.m_lNID = -1;

	m_vecServerInfo.Add( NewServerInfo );
	return TRUE;
}

BOOL CDataManager::SetAdminModule( void* pModule )
{
	m_pAgcmAdmin = pModule;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !pcmAdmin ) return FALSE;

	m_pAgpmAdmin = pcmAdmin->GetModule( "AgpmAdmin" );
	if( !m_pAgpmAdmin ) return FALSE;

	OnRegisterCallBack();
	return TRUE;
}

BOOL CDataManager::ProcessRequestCurrentUserCount( void )
{
	// 자기 정보를 받지 않은 상태에선 보내지 않는다.
	if( m_stSelfAdminInfo.m_lCID == 0) return FALSE;

	DWORD dwNowTickCount = ::GetTickCount();
	
	// 처음 들어왔으면 요청하고 나간다.
	if( m_dwLastRequestCurrentUserTickCount == 0 )
	{
		SendRequestCurrentUserCount();
		m_dwLastRequestCurrentUserTickCount = dwNowTickCount;
		return TRUE;
	}

	if( dwNowTickCount - m_dwLastRequestCurrentUserTickCount >= INTERVAL_REQUEST_CURRENT_USER_COUNT )
	{
		SendRequestCurrentUserCount();
		m_dwLastRequestCurrentUserTickCount = dwNowTickCount;
	}

	return TRUE;
}

void CDataManager::SetSelfAdminInfo( void* pSelfAdminInfo )
{
	if( pSelfAdminInfo )
	{
		memcpy( &m_stSelfAdminInfo, pSelfAdminInfo, sizeof( stAgpdAdminInfo ) );
	}
	else
	{
		memset( &m_stSelfAdminInfo, 0, sizeof( stAgpdAdminInfo ) );
	}
}

void CDataManager::SetOldSearchData( stAgpdAdminSearch* pSearchData )
{
	if( !pSearchData )
	{
		memset( &m_stOldSearchData, 0, sizeof( stAgpdAdminSearch ) );
	}
	else
	{
		memcpy( &m_stOldSearchData, pSearchData, sizeof( stAgpdAdminSearch ) );
	}
}

void CDataManager::SetReceivedCharData( stAgpdAdminCharData* pCharData )
{
	if(pCharData)
	{
		memcpy( &m_stReceivedCharacterData.m_stBasic,	&pCharData->m_stBasic,	sizeof( stAgpdAdminCharDataBasic ) );
		memcpy( &m_stReceivedCharacterData.m_stStat,	&pCharData->m_stStat,	sizeof( stAgpdAdminCharDataStat ) );
		memcpy( &m_stReceivedCharacterData.m_stStatus,	&pCharData->m_stStatus, sizeof( stAgpdAdminCharDataStatus ) );
		memcpy( &m_stReceivedCharacterData.m_stCombat,	&pCharData->m_stCombat, sizeof( stAgpdAdminCharDataCombat ) );
		memcpy( &m_stReceivedCharacterData.m_stMoney,	&pCharData->m_stMoney,	sizeof( stAgpdAdminCharDataMoney ) );
	}
	else
	{
		memset( &m_stReceivedCharacterData.m_stBasic,	0, sizeof( stAgpdAdminCharDataBasic ) );
		memset( &m_stReceivedCharacterData.m_stStat,	0, sizeof( stAgpdAdminCharDataStat ) );
		memset( &m_stReceivedCharacterData.m_stStatus,	0, sizeof( stAgpdAdminCharDataStatus ) );
		memset( &m_stReceivedCharacterData.m_stCombat,	0, sizeof( stAgpdAdminCharDataCombat ) );
		memset( &m_stReceivedCharacterData.m_stMoney,	0, sizeof( stAgpdAdminCharDataMoney ) );
	}
}

void CDataManager::SetReceivedCharDataSub( stAgpdAdminCharDataSub* pCharDataSub )
{
	if( pCharDataSub )
	{
		memcpy( &m_stReceivedCharacterData.m_stSub, pCharDataSub, sizeof( stAgpdAdminCharDataSub ) );
	}
	else
	{
		memset( &m_stReceivedCharacterData.m_stSub, 0, sizeof( stAgpdAdminCharDataSub ) );
	}
}

BOOL CDataManager::SetItemData( void* pItem, void* pItemData )
{
	AgpdItem* ppdItem = ( AgpdItem* )pItem;
	stAgpdAdminItemData* ppdItemData = ( stAgpdAdminItemData* )pItemData;
	if( !ppdItem || !ppdItemData ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ( AgpdItemTemplate* )ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplate ) return FALSE;

	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pcmAdmin->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( ppdItem->m_ulCID );
	if( !ppdCharacter ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )pcmAdmin->GetModule( "AgpmItem" );
	if( !ppmItem ) return FALSE;

	AgpmItemConvert* ppmItemConvert = ( AgpmItemConvert* )pcmAdmin->GetModule( "AgpmItemConvert" );
	if( !ppmItemConvert ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )pcmAdmin->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return FALSE;

	// 아이템 소유자
	ppdItemData->m_lOwnerCID = ppdItem->m_ulCID;
	strcpy( ppdItemData->m_szCharName, ppdCharacter->m_szID );

	// 아이템 종류
	ppdItemData->m_lKind = ( ( AgpdItemTemplateEquip* )ppdItemTemplate )->m_nKind;

	// 인벤토리에서의 아이템 위치
	ppdItemData->m_lPos = ppdItem->m_eStatus;
	ppdItemData->m_lLayer = ppdItem->m_anGridPos[ AGPDITEM_GRID_POS_TAB ];
	ppdItemData->m_lRow = ppdItem->m_anGridPos[ AGPDITEM_GRID_POS_ROW ];
	ppdItemData->m_lCol = ppdItem->m_anGridPos[ AGPDITEM_GRID_POS_COLUMN ];

	// 아이템 기본정보.. 명칭, IID, TID, 갯수
	strcpy( ppdItemData->m_szItemName, ppdItemTemplate->m_szName );
	ppdItemData->m_lItemID = ppdItem->m_lID;
	ppdItemData->m_lItemTID = ppdItemTemplate->m_lID;
	ppdItemData->m_lCount = ppdItem->m_nCount;

	// 아이템 판매가격
	ppdItemData->m_lPrice = (INT32)( ppmItem->GetNPCPrice( ppdItem ) * ppdItem->m_nCount );

	// 아이템 등급
	ppdItemData->m_lRank = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK );

	// 아이템 내구도
	ppdItemData->m_lDurability = ppmItem->GetItemDurabilityPercent( ppdItem );

	// 물리강화 회수, 소켓 갯수 및 사용된 소켓 갯수
	ppdItemData->m_lNumPhysicalConvert = ppmItemConvert->GetNumPhysicalConvert( ppdItem );
	ppdItemData->m_lNumConvertedSocket = ppmItemConvert->GetNumConvertedSocket( ppdItem );
	ppdItemData->m_lNumSocket = ppmItemConvert->GetNumSocket( ppdItem );
	
	// 공격 범위
	ppdItemData->m_lAttackRange = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE );

	// 물리공격력
	ppdItemData->m_lPhyMinDmg = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );
	ppdItemData->m_lPhyMaxDmg = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

	// 마법공격력
	ppdItemData->m_lMagicMinDmg = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC );
	ppdItemData->m_lMagicMaxDmg = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC );

	// 공격속도
	ppdItemData->m_lAttackSpeed = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED );

	// 물리 방어력 - 갑빠에 적용
	ppdItemData->m_lPhyDefense = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

	// 블록 - 방패에 적용
	ppdItemData->m_lPhyDefenseRate = _GetFactorValue( &ppdItem->m_csFactor, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL );

	// Race Restrict
	INT32 nRestrictRace = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE );
	if( nRestrictRace > 0 )
	{
		CHAR* szTmp = ppmFactor->GetCharacterRaceName( nRestrictRace );
		if( szTmp && strlen( szTmp ) <= AGPACHARACTER_MAX_ID_STRING )
		{
			strcpy( ppdItemData->m_szRaceName, szTmp );
		}
	}

	// Gender Restrict
	INT32 nRestrictGender = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_GENDER );
	if( nRestrictGender > 0 )
	{
		CHAR* szTmp = ppmFactor->GetCharacterGenderName( nRestrictGender );
		if( szTmp && strlen( szTmp ) <= AGPACHARACTER_MAX_ID_STRING )
		{
			strcpy( ppdItemData->m_szGenderName, szTmp );
		}
	}

	// Class Restrict
	INT32 nRestrictClass = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS );
	if( nRestrictClass > 0 )
	{
		CHAR* szTmp = ppmFactor->GetCharacterClassName( nRestrictRace, nRestrictClass );
		if( szTmp && strlen( szTmp ) <= AGPACHARACTER_MAX_ID_STRING )
		{
			strcpy( ppdItemData->m_szClassName, szTmp );
		}
	}

	// 착용요구치
	ppdItemData->m_lNeedLevel = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );
	ppdItemData->m_lNeedStr = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR );
	ppdItemData->m_lNeedCon = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON );
	ppdItemData->m_lNeedInt = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT );
	ppdItemData->m_lNeedWis = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS );
	ppdItemData->m_lNeedDex = _GetFactorValue( &ppdItem->m_csRestrictFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX );
	return TRUE;
}

BOOL CDataManager::FindServerInfo( void* pServerInfo )
{
	stAgpdAdminServerInfo* ppdServerInfo = ( stAgpdAdminServerInfo* )pServerInfo;
	if( !ppdServerInfo ) return FALSE;

	INT32 nServerCount = m_vecServerInfo.GetSize();
	for( INT32 nCount = 0 ; nCount < nServerCount ; nCount++ )
	{
		stAgpdAdminServerInfo* pEntry = m_vecServerInfo.Get( nCount );
		if( pEntry )
		{
			if( ( strcmp( pEntry->m_szServerIP, ppdServerInfo->m_szServerIP ) == 0 ) && ( pEntry->m_lPort == ppdServerInfo->m_lPort ) )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

CHAR* CDataManager::GetUIMessageString( CHAR* pKeyString )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return NULL;	

	return pcmAdmin->GetUIMessageString( pKeyString );
}

BOOL CDataManager::CBSelectCharacter( void* pData, void* pClass, void* pCustData )
{
	INT32* pType = ( INT32* )pData;
	INT32* pCID = ( INT32* )pCustData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !pType || !pCID || !pDataManager ) return FALSE;

	INT32 nType = *pType;
	INT32 nCID = *pCID;
	return pDataManager->OnSelectCharacter( nType, nCID );
}

BOOL CDataManager::CBSelectGround( void* pData, void* pClass, void* pCustData )
{
	AuPOS* pPos = ( AuPOS* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !pPos || !pDataManager ) return FALSE;

	return pDataManager->OnSelectGround( pPos );
}

BOOL CDataManager::CBAddAdmin( void* pData, void* pClass, void* pCustData )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	stAgpdAdminInfo* ppdAdminInfo = ( stAgpdAdminInfo* )pCustData;
	if( !ppdCharacter || !pDataManager || !ppdAdminInfo ) return FALSE;

	return pDataManager->OnAddAdmin( ppdCharacter, ppdAdminInfo );
}

BOOL CDataManager::CBReceiveSearchResult( void* pData, void* pClass, void* pCustData )
{
	stAgpdAdminSearch* ppdSearch = ( stAgpdAdminSearch* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	stAgpdAdminSearchResult* ppdSearchResult = ( stAgpdAdminSearchResult* )pCustData;
	if( !ppdSearch || !pDataManager || !ppdSearchResult ) return FALSE;

	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->OnReceiveSearchResult( ppdSearchResult );
}

BOOL CDataManager::CBReceiveCharacterData( void* pData, void* pClass, void* pCustData )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !pDataManager ) return FALSE;

	return pDataManager->OnAddCharacterData( ppdCharacter );
}

BOOL CDataManager::CBReceiveCharacterDataSub( void* pData, void* pClass, void* pCustData )
{
	stAgpdAdminCharDataSub* ppdDataSub = ( stAgpdAdminCharDataSub* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !pDataManager ) return FALSE;

	return pDataManager->OnAddCharacterDataSub( ppdDataSub );
}

BOOL CDataManager::CBReceiveCharacterModifiied( void* pData, void* pClass, void* pCustData )
{
	stAgpdAdminCharEdit* ppdDataModified = ( stAgpdAdminCharEdit* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !ppdDataModified || !pDataManager ) return FALSE;

	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceiveEditResult( ppdDataModified );
}

BOOL CDataManager::CBReceiveCharacterSkill( void* pData, void* pClass, void* pCustData )
{
	AgpdSkill* ppdSkill = ( AgpdSkill* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !ppdSkill || !pDataManager ) return FALSE;

	return pDataManager->OnAddCharacterSkill( ppdSkill );
}

BOOL CDataManager::CBReceiveCharacterItem( void* pData, void* pClass, void* pCustData )
{
	AgpdItem* ppdItem = ( AgpdItem* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !ppdItem || !pDataManager ) return FALSE;

	return pDataManager->OnAddCharacterItem( ppdItem );
}

BOOL CDataManager::CBReceiveServerInfo( void* pData, void* pClass, void* pCustData )
{
	stAgpdAdminServerInfo* ppdServerInfo = ( stAgpdAdminServerInfo* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !ppdServerInfo || !pDataManager ) return FALSE;

	return pDataManager->OnAddServerInfo( ppdServerInfo );
}

BOOL CDataManager::CBDlgSendSearchCharacter( void* pData, void* pClass, void* pCustData )
{
	stAgpdAdminSearch* ppdSearchData = ( stAgpdAdminSearch* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !pDataManager ) return FALSE;
	if( !ppdSearchData )
	{
		pDataManager->SetOldSearchData( NULL );
		return TRUE;
	}

	return pDataManager->SendSearchCharacter( ppdSearchData );;
}

BOOL CDataManager::CBNumCurrentUser( void* pData, void* pClass, void* pCustData )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	INT32* pCurrentUser = ( INT32* )pCustData;
	if( !ppdCharacter || !pDataManager || !pCurrentUser ) return FALSE;

	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->OnReceiveCurrentUser( *pCurrentUser );
}

BOOL CDataManager::CBReceiveCustom( void* pData, void* pClass, void* pCustData )
{
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !pDataManager || !pData || !pCustData ) return FALSE;

	INT32 nCustomType = *( ( INT32* )pData );
	void** ppBuffer = static_cast< void** >( pCustData );
	void* pCustomData = ppBuffer[ 0 ];
	INT16 nLength = *static_cast< INT16* >( ppBuffer[ 1 ] );

	switch( nCustomType )
	{
	case AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT :
		{
			stAgpdAdminSearchResult* ppdResult = static_cast< stAgpdAdminSearchResult* >( pCustomData );
			if( ppdResult )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->OnReceiveSearchResult( ppdResult );
			}
		}
		break;

	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_BASIC :
		{
			stAgpdAdminCharDataBasic* ppdBasic = static_cast< stAgpdAdminCharDataBasic* >( pCustomData );
			if( ppdBasic )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceiveBasic( ppdBasic );
			}			
		}
		break;

	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STAT :
		{
			stAgpdAdminCharDataStat* ppdStat = static_cast< stAgpdAdminCharDataStat* >( pCustomData );
			if( ppdStat )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceiveStat( ppdStat );
			}			
		}
		break;

	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STATUS :
		{
			stAgpdAdminCharDataStatus* ppdStatus = static_cast< stAgpdAdminCharDataStatus* >( pCustomData );
			if( ppdStatus )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceiveStatus( ppdStatus );
			}
		}
		break;

	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_MONEY :
		{
			stAgpdAdminCharDataMoney* ppdMoney = static_cast< stAgpdAdminCharDataMoney* >( pCustomData );
			if( ppdMoney )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceiveMoney( ppdMoney );
			}			
		}
		break;

	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SUB :
		{
			stAgpdAdminCharDataSub* ppdSub = static_cast< stAgpdAdminCharDataSub* >( pCustomData );
			if( ppdSub )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->OnReceiveSub( ppdSub );
			}
		}
		break;

	case AGPMADMIN_CUSTOM_DATA_TYPE_ITEM :
		{
			stAgpdAdminItem* ppdItem = static_cast< stAgpdAdminItem* >( pCustomData );
			if( ppdItem )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pItemView->OnReceive( ppdItem );
			}			
		}
		break;

	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SKILL :
		{
			stAgpdAdminSkillString* ppdSkillString = static_cast< stAgpdAdminSkillString* >( pCustomData );
			if( ppdSkillString )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pSkillView->OnReceiveSkillString( ppdSkillString );
			}
		}
		break;

		// 2006.11.30. steeple
	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_WC :
		{
			// 얘는 Money 데이터를 그냥 사용한다.
			stAgpdAdminCharDataMoney* ppdWantedCriminal = static_cast< stAgpdAdminCharDataMoney* >( pCustomData );
			if( ppdWantedCriminal )
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_pWCView->OnReceiveWCInfo( ppdWantedCriminal );
			}
		}
		break;
	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_QUEST:
		{
			stAgpdAdminCharQuest* ppdQuest = static_cast< stAgpdAdminCharQuest* >( pCustomData );
			if(ppdQuest)
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pCharView->m_pCharQuest->OnReceiveQuestInfo( ppdQuest );
			}
		}
		break;
	case AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE:
		{
			stAgpdAdminCharTitle* ppdTitle = static_cast< stAgpdAdminCharTitle* >( pCustomData );
			if(ppdTitle)
			{
				if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
				return AlefAdminManager::Instance()->m_pMainDlg->m_pTitleView->OnReceiveTitleInfo( ppdTitle );
			}
		}
		break;
	}

	return TRUE;
}

BOOL CDataManager::CBReceiveItemOperationResult( void* pData, void* pClass, void* pCustData )
{
	stAgpdAdminItemOperation* ppdItemOperation = ( stAgpdAdminItemOperation* )pData;
	CDataManager* pDataManager = ( CDataManager* )pClass;
	if( !ppdItemOperation || !pDataManager ) return FALSE;

	if( !AlefAdminManager::Instance()->IsInitialized() ) return FALSE;
	return AlefAdminManager::Instance()->m_pMainDlg->m_pItemView->OnReceiveResult( ppdItemOperation );
}

BOOL CDataManager::SendRequestCurrentUserCount( void )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter )	return FALSE;

	INT16 nPacketLength = 0;
	void* pLoginPacket = ppmAdmin->MakeLoginPacket( &nPacketLength, NULL, NULL,	NULL, NULL, NULL, NULL );
	if( !pLoginPacket ) return FALSE;

	void* pPacket = ppmAdmin->MakeNumCurrentUserPacket( &nPacketLength, &ppdSelfCharacter->m_lID, pLoginPacket );
	ppmAdmin->m_csLoginPacket.FreePacket( pLoginPacket );
	if( !pPacket || nPacketLength < 1 ) return FALSE;

	BOOL bResult = ppmAdmin->SendPacket( pPacket, nPacketLength );
	ppmAdmin->m_csPacket.FreePacket( pPacket );
	return bResult;
}

BOOL CDataManager::SendSearchCharacter( void* pSearchData )
{
	stAgpdAdminSearch* ppdSearchData = ( stAgpdAdminSearch* )pSearchData;
	if( !ppdSearchData ) return FALSE;

	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter )	return FALSE;

	PVOID pvPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CHARACTER;

	void* pSearchPacket = ppmAdmin->MakeSearchPacket( &nPacketLength, &ppdSearchData->m_iType, &ppdSearchData->m_iField, &ppdSearchData->m_lObjectCID, ppdSearchData->m_szSearchName, NULL );
	if( !pSearchPacket ) return FALSE;

	void* pPacket = ppmAdmin->MakeSearchPacket( &nPacketLength, &ppdSelfCharacter->m_lID, pSearchPacket );
	ppmAdmin->m_csSearchPacket.FreePacket( pSearchPacket );
	if( !pPacket || nPacketLength < 1 ) return FALSE;

	BOOL bResult = ppmAdmin->SendPacket( pPacket, nPacketLength );
	ppmAdmin->m_csPacket.FreePacket( pPacket );

	// 마지막에 검색하는 데이터를 세팅해준다.
	bResult ? SetOldSearchData( ppdSearchData ) : SetOldSearchData( NULL );
	return bResult;
}

BOOL CDataManager::SendCharacterModification( void* pModify, void* pString )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter )	return FALSE;

	stAgpdAdminCharEdit* ppdModify = ( stAgpdAdminCharEdit* )pModify;
	stAgpdAdminSkillString* ppdSkillString = ( stAgpdAdminSkillString* )pString;
	if( !ppdModify ) return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvCharEditPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_CHAREDIT;

	void* pCharEditPacket = ppmAdmin->MakeCharEditPacket( &nPacketLength, ppdModify, sizeof( stAgpdAdminCharEdit ), ppdSkillString, ppdSkillString ? sizeof( stAgpdAdminSkillString ) : 0 );
	if( !pCharEditPacket ) return FALSE;

	void* pPacket = ppmAdmin->MakeCharEditPacket( &nPacketLength, &ppdSelfCharacter->m_lID, pCharEditPacket );
	ppmAdmin->m_csCharEditPacket.FreePacket( pCharEditPacket );
	if( !pPacket || nPacketLength < 1 ) return FALSE;

	BOOL bResult = ppmAdmin->SendPacket( pPacket, nPacketLength );
	ppmAdmin->m_csPacket.FreePacket( pPacket );
	return bResult;
}

BOOL CDataManager::SendBan( void* pBan )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter )	return FALSE;

	stAgpdAdminBan* ppdBan = ( stAgpdAdminBan* )pBan;
	if( !ppdBan ) return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvBanPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_BAN;

	void* pBanPacket = ppmAdmin->MakeBanPacket( &nPacketLength,	&ppdSelfCharacter->m_lID, ppdBan->m_szCharName,	ppdBan->m_szAccName,
		&ppdBan->m_lLogout, &ppdBan->m_lBanFlag, &ppdBan->m_lChatBanStartTime, &ppdBan->m_lChatBanKeepTime, &ppdBan->m_lCharBanStartTime,
		&ppdBan->m_lCharBanKeepTime, &ppdBan->m_lAccBanStartTime, &ppdBan->m_lAccBanKeepTime );
	if( !pBanPacket ) return FALSE;

	void* pPacket = ppmAdmin->MakeBanPacket( &nPacketLength, &ppdSelfCharacter->m_lID, pBanPacket );
	ppmAdmin->m_csBanPacket.FreePacket( pBanPacket );
	if( !pPacket || nPacketLength < 1 ) return FALSE;

	BOOL bResult = ppmAdmin->SendPacket( pPacket, nPacketLength );
	ppmAdmin->m_csPacket.FreePacket( pPacket );
	return bResult;
}

BOOL CDataManager::SendItemOperation( void* pItemOperation )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter )	return FALSE;

	stAgpdAdminItemOperation* ppdItemOperation = ( stAgpdAdminItemOperation* )pItemOperation;
	if( !ppdItemOperation ) return FALSE;

	INT16 nLength = sizeof( stAgpdAdminItemOperation );
	INT16 nPacketLength = 0;

	void* pItemPacket = ppmAdmin->MakeItemPacket( &nPacketLength, ppdItemOperation, nLength );
	if( !pItemPacket ) return FALSE;

	void* pPacket = ppmAdmin->MakeItemPacket( &nPacketLength, &ppdSelfCharacter->m_lID, pItemPacket );
	ppmAdmin->m_csItemPacket.FreePacket( pItemPacket );
	if( !pPacket ) return FALSE;

	BOOL bResult = ppmAdmin->SendPacket( pPacket, nPacketLength );
	ppmAdmin->m_csPacket.FreePacket( pPacket );
	return bResult;
}

BOOL CDataManager::SendGuildOperation( void* pModify )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter )	return FALSE;

	stAgpdAdminCharEdit* ppdModify = ( stAgpdAdminCharEdit* )pModify;
	if( !ppdModify ) return FALSE;

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_GUILD;

	void* pCharEditPacket = ppmAdmin->MakeCharEditPacket( &nPacketLength, ppdModify, sizeof( stAgpdAdminCharEdit ) );
	if( !pCharEditPacket ) return FALSE;

	void* pPacket = ppmAdmin->MakeAdminPacket( TRUE, &nPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation,
		&ppdSelfCharacter->m_lID, NULL,	NULL, NULL, pCharEditPacket, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	ppmAdmin->m_csCharEditPacket.FreePacket( pCharEditPacket );
	if( !pPacket || nPacketLength < 1 ) return FALSE;

	BOOL bResult = ppmAdmin->SendPacket( pPacket, nPacketLength );
	ppmAdmin->m_csPacket.FreePacket( pPacket );
	return bResult;
}

BOOL CDataManager::SendTitleOperation( void* pTitleOperation )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return FALSE;	

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmAdmin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter )	return FALSE;

	stAgpdAdminTitleOperation* ppdTitle = ( stAgpdAdminTitleOperation* )pTitleOperation;
	if( !ppdTitle ) return FALSE;

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_TITLE;

	void* pTitleEditPacket = ppmAdmin->MakeTitlePacket( &nPacketLength, ppdTitle, sizeof( stAgpdAdminTitleOperation ) );
	if( !pTitleEditPacket ) return FALSE;

	void* pPacket = ppmAdmin->MakeAdminPacket( TRUE, &nPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation,
		&ppdSelfCharacter->m_lID, NULL,	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pTitleEditPacket);
	ppmAdmin->m_csTitlePacket.FreePacket( pTitleEditPacket );
	if( !pPacket || nPacketLength < 1 ) return FALSE;

	BOOL bResult = ppmAdmin->SendPacket( pPacket, nPacketLength );
	ppmAdmin->m_csPacket.FreePacket( pPacket );
	return bResult;
}

INT32 CDataManager::_GetFactorValue( void* pFactor, INT32 nFactorType, INT32 nFactorSubType1, INT32 nFactorSubType2 )
{
	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )m_pAgpmAdmin;
	AgcmAdmin* pcmAdmin = ( AgcmAdmin* )m_pAgcmAdmin;
	if( !ppmAdmin || !pcmAdmin ) return 0;	

	AgpmFactors* ppmFactor = ( AgpmFactors* )pcmAdmin->GetModule( "AgpmFactors" );
	if( !ppmFactor ) return 0;

	AgpdFactor* ppdFactor = ( AgpdFactor* )pFactor;
	if( !ppdFactor ) return 0;

	INT32 nValue = 0;
	ppmFactor->GetValue( ppdFactor, &nValue, ( eAgpdFactorsType )nFactorType, nFactorSubType1, nFactorSubType2 );
	return nValue;
}
