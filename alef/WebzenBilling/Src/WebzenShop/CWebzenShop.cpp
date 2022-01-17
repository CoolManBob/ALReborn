#include "CWebzenShop.h"



CWebzenShop::CWebzenShop( void )
{
	memset( m_strServerAddress, 0, sizeof( char ) * 64 );
	m_nServerPort = 0;

	m_bIsStarted = FALSE;

	m_nPageCur = 0;
	m_nPageMax = 0;
	m_nViewCountPerPage = 0;

	m_nGameCode = 0;
	m_nSalesZoneCode = 0;
}

CWebzenShop::~CWebzenShop( void )
{
}

BOOL CWebzenShop::OnCreateSession( void )
{
	if( m_bIsStarted )
	{
		WriteLog( "-- Webzen Shop -- : Webzen Shop is already started\n" );
		return TRUE;
	}

	if( !CreateSession( fnErrorHandler ) )
	{
		WriteLog( "-- Webzen Shop -- : Failed to CShopEventHandler::CreateSession()!\n" );
		return FALSE;
	}

	m_bIsStarted = TRUE;
	return TRUE;
}

BOOL CWebzenShop::OnWebzenShopConnect( char* pServerAddress, int nPort )
{
	if( !pServerAddress || strlen( pServerAddress ) <= 0 )
	{
		WriteLog( "-- Webzen Shop -- : Shop Server address is missing\n" );
		return FALSE;
	}

	memset( m_strServerAddress, 0, sizeof( char ) * 64 );
	strcpy_s( m_strServerAddress, sizeof( char ) * 64, pServerAddress );
	m_nServerPort = nPort;

	Connect( m_strServerAddress, m_nServerPort );
	return TRUE;
}

BOOL CWebzenShop::OnWebzenShopDisConnect( void )
{
	WriteLog( "-- Webzen Shop -- : Request disconnect to Shop Server..." );
	SetKill();
	return TRUE;
}

void CWebzenShop::fnErrorHandler( DWORD dwLastError, TCHAR* pErrorMsg )
{
}

void CWebzenShop::OnNetConnect( bool bIsSuccess, DWORD dwError )
{
	if( !bIsSuccess )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Failed to connect Shop Server, IP = %s, Port = %d, ErrorCode = %d\n", m_strServerAddress, m_nServerPort, dwError );
		return;
	}

	WriteLog( "-- Webzen Shop -- : OnConnect Succeed." );
}

void CWebzenShop::OnNetClose( DWORD dwError )
{
	WriteLog( "-- Webzen Shop -- : Connection refused." );

	if( dwError > 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Connection refused by Error, ErrorCode = %d\n", dwError );
	}
}

void CWebzenShop::OnNetSend( int nSendSize )
{
	WriteFormattedLog( "-- Webzen Shop -- : Complete to Send Packet to Shop Server, Send Size = %d\n", nSendSize );
}

void CWebzenShop::WriteLog( char* szMsg )
{
	TCHAR szModule[ MAX_PATH ] = { 0 };
	TCHAR szFilePath[ MAX_PATH ] = { 0 };

	::GetModuleFileName( NULL, szModule, MAX_PATH );
	*( _tcsrchr( szModule, '\\' ) ) = 0;

	StringCchPrintf( szFilePath, STRSAFE_MAX_CCH , _T( "%s\\Shop.log" ), szModule );
	WriteFileLog( szMsg, szFilePath );
}

void CWebzenShop::OnInquireCash( DWORD dwAccountID, double dCashSum, double dPointSum, double dMileageSum, int nDetailCount, ShopProtocol::STCashDetail Detail[], long nResultCode )
{
	if( nResultCode != 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnInquireCash(), AccountID = %d, ErrorCode = %d\n", dwAccountID, nResultCode );
		return;
	}

	// 아크로드에는 마일리지 뭐 그런건 없으니 캐쉬만 체크

	if( nDetailCount <= 0 )
	{
		m_mapCash.Clear();
		return;
	}

	for( int nCount = 0 ; nCount < nDetailCount ; nCount++ )
	{
		if( Detail[ nCount ].Type != 0 )
		{
			char strNameBuffer[ 32 ] = { 0, };
			ConvertWideToMBCS( Detail[ nCount ].Name, strNameBuffer, 32 );
			_UpdateCashData( Detail[ nCount ].Type, Detail[ nCount ].Value, strNameBuffer );
		}
	}
}

void CWebzenShop::OnBuyProduct( long nAccountID, long nResultCode, long nRemainProductCount )
{
	// 아크로드의 판매상품들은 상품수량 제한이 걸린것은 없으니 nRemainProductCount 값은 개무시해준다.
	if( !nResultCode )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnBuyProduct(), AccountID = %d, ErorCode = %d\n", nAccountID, nResultCode );
	}
}

void CWebzenShop::OnInquireStorageListPageNoGiftMessage( DWORD dwAccountID, long nResultCode, char cInventoryType, int nPageNumber, int nTotalPageCount, int nTotalProductCount, long nItemCount, ShopProtocol::STStorageNoGiftMessage ItemData[] )
{
	if( nResultCode != 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnInquireStorageListPageNoGiftMessage(), AccountID = %d, ErorCode = %d\n", dwAccountID, nResultCode );
		return;
	}

	m_nPageCur = nPageNumber;
	m_nPageMax = nTotalPageCount;

	for( int nCount = 0 ; nCount < nItemCount ; nCount++ )
	{
		char strCashName[ 20 ] = { 0, };
		ConvertWideToMBCS( ItemData[ nCount ].CashName, strCashName, 20 );

		char strBuyerName[ 50 ] = { 0, };
		ConvertWideToMBCS( ItemData[ nCount ].SendAccountID, strBuyerName, 50 );

		_UpdateCashItemData(	ItemData[ nCount ].Seq,				// 보관함 고유코드
								ItemData[ nCount ].ItemSeq,			// 아이템 고유코드
								ItemData[ nCount ].GroupCode,		// 그룹 코드
								ItemData[ nCount ].ShareFlag,		// 서버 유형
								ItemData[ nCount ].ProductSeq,		// 상품 고유코드
								strCashName,						// 뭘로 샀냐
								ItemData[ nCount ].CashPoint,		// 얼마냐
								strBuyerName,						// 누가 샀냐
								ItemData[ nCount ].ItemType,		// 물건이냐 상품권이냐
								ItemData[ nCount ].RelationType,	// 직접 산거냐 선물받은 거냐
								ItemData[ nCount ].ProductType		// 상품 타입
							);
	}
}

void CWebzenShop::OnUseStorage( DWORD dwAccountID, long nResultCode, char strInGameProductID[ MAX_TYPENAME_LENGTH ], BYTE bPropertyCount, long nProductID, long nIndex, long nItemIndex, ShopProtocol::STItemProperty ItemProperty[] )
{
	if( nResultCode != 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnUseStorage(), AccountID = %d, ErorCode = %d\n", dwAccountID, nResultCode );
		return;
	}

	for( int nCount = 0 ; nCount < bPropertyCount ; nCount++ )
	{
		_UpdateCashItemProperty( nIndex, nItemIndex, nProductID, ItemProperty[ nCount ].PropertySeq, ItemProperty[ nCount ].Value );
	}
}

void CWebzenShop::OnRollbackUseStorage( DWORD dwAccountID, long nResultCode )
{
	if( nResultCode != 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnRollbackUseStorage(), AccountID = %d, ErorCode = %d\n", dwAccountID, nResultCode );
		return;
	}
}

void CWebzenShop::OnThrowStorage( DWORD dwAccountID, long nResultCode )
{
	if( nResultCode != 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnThrowStorage(), AccountID = %d, ErorCode = %d\n", dwAccountID, nResultCode );
		return;
	}
}

void CWebzenShop::OnItemSerialUpdate( DWORD dwAccountID, long nResultCode )
{
	if( nResultCode != 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnItemSerialUpdate(), AccountID = %d, ErorCode = %d\n", dwAccountID, nResultCode );
		return;
	}
}

void CWebzenShop::OnInquireSalesZoneScriptVersion( unsigned short nSalesZoneCode, unsigned short nYear, unsigned short nYearIdentity, long nResultCode )
{
	if( nResultCode != 0 )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error result from OnInquireSalesZoneScriptVersion(), SalesZone = %d, ErorCode = %d\n", nSalesZoneCode, nResultCode );
		return;
	}
}

void CWebzenShop::OnUpdateVersion( long nGameCode, unsigned short nSalesZoneCode, unsigned short nYear, unsigned short nYearIdentity )
{
	m_nGameCode = nGameCode;
	m_nSalesZoneCode = nSalesZoneCode;

	// 받은 GameCode, ZoneCode 를 클라로 전송
}

BOOL CWebzenShop::SendGetCash( DWORD dwAccountID, eWebzenCashType eType, long nMileageType, BOOL bOnlyTotal )
{
	if( !InquireCash( m_nGameCode, ( BYTE )eType, dwAccountID, bOnlyTotal ? true : false, nMileageType ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet GetCash, AccountID = %d, eType = %d\n", dwAccountID, eType );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenShop::SendBuyProduct( DWORD dwAccountID, long nPackageID, long nCategoryID, long nSalesZoneCode, long nPriceID, long nCharacterClass, long nCharacterLevel, long nServerID, char* pCharacterName )
{
	WCHAR strCharacterName[ 64 ] = { 0, };
	if( !ConvertMBCSToWide( pCharacterName, strCharacterName, 64 ) ) return FALSE;

	// 캐릭터 랭크는 아크로드에는 없으니 "" 처리
	if( !BuyProduct( m_nGameCode, dwAccountID, nPackageID, nCategoryID, nSalesZoneCode, nPriceID, nCharacterClass, nCharacterLevel, strCharacterName, L"", nServerID ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet BuyProduct, AccountID = %d, PackageID = %d\n", dwAccountID, nPackageID );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenShop::SendGetCashItemInventoryData( DWORD dwAccountID, int nPageNumber )
{
	// 구매보관함과 선물보관함을 따로 구분하지 않으므로 전체 'A' 로 해서 보낸다.
	if( !InquireStorageListPageNoGiftMessage( dwAccountID, m_nGameCode, m_nSalesZoneCode, 'A', nPageNumber, m_nViewCountPerPage ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet GetCashItemInventoryData, AccountID = %d, PageNumber = %d\n", dwAccountID, nPageNumber );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenShop::SendUseCashItem( DWORD dwAccountID, DWORD dwIPAddress, long nIndex, int nItemIndex, char cItemType, long nCharacterClass, long nCharacterLevel, long nServerIndex, char* pCharacterName )
{
	WCHAR strCharacterName[ 32 ] = { 0, };
	ConvertMBCSToWide( pCharacterName, strCharacterName, 32 );

	// 캐릭터 랭크는 아크로드에는 없으니 "" 처리
	if( !UseStorage( dwAccountID, m_nGameCode, dwIPAddress, nIndex, nItemIndex, cItemType, nCharacterClass, nCharacterLevel, strCharacterName, L"", nServerIndex ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet UseCashItem, AccountID = %d, InventoryIndex = %d, ItemIndex = %d\n", dwAccountID, nIndex, nItemIndex );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenShop::SendUseCashItemCancel( DWORD dwAccountID, long nIndex, long nItemIndex )
{
	if( !RollbackUseStorage( dwAccountID, m_nGameCode, nIndex, nItemIndex ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet UseCashItemCancel, AccountID = %d, InventoryIndex = %d, ItemIndex = %d\n", dwAccountID, nIndex, nItemIndex );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenShop::SendDeleteCashItem( DWORD dwAccountID, long nIndex, long nItemIndex, char cItemType )
{
	if( !ThrowStorage( dwAccountID, m_nGameCode, nIndex, nItemIndex, cItemType ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet DeleteCashItem, AccountID = %d, InventoryIndex = %d, ItemIndex = %d\n", dwAccountID, nIndex, nItemIndex );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenShop::SendUpdateItemCode( DWORD dwAccountID, long nIndex, long nItemIndex, __int64 nItemCode )
{
	if( !ItemSerialUpdate( dwAccountID, m_nGameCode, nIndex, nItemIndex, nItemCode ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet UpdateItemCode, AccountID = %d, InventoryIndex = %d, ItemIndex = %d, ItemCode = %i64d\n", dwAccountID, nIndex, nItemIndex, nItemCode );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenShop::SendCheckShopScriptVersion( void )
{
	if( !InquireSalesZoneScriptVersion( m_nGameCode, m_nSalesZoneCode ) )
	{
		WriteFormattedLog( "-- Webzen Shop -- : Error in Send Packet CheckShopScriptVersion, GameCode = %d, SalesZone = %d\n", m_nGameCode, m_nSalesZoneCode );
		return FALSE;
	}

	return TRUE;
}

void CWebzenShop::_UpdateCashData( char cCashType, double dValue, char* pName )
{
	eWebzenCashType eType = WebzenCash_TotalAll;
	switch( cCashType )
	{
	case 'C' :	eType = WebzenCash_CashAll;		break;
	case 'P' :	eType = WebzenCash_PointAll;	break;
	default :	return;							break;
	}

	stCashDataEntry* pEntry = m_mapCash.Get( eType );
	if( !pEntry )
	{
		// 없으면 추가해야징~
		stCashDataEntry NewEntry;

		NewEntry.m_eType = eType;
		NewEntry.m_dValue = dValue;

		if( pName && strlen( pName ) > 0 )
		{
			strcpy_s( NewEntry.m_strTypeName, sizeof( char ) * 32, pName );
		}

		m_mapCash.Add( eType, NewEntry );
		WriteFormattedLog( "-- Webzen Shop -- : Update Cash, TypeName = %s, NewValue = %d\n", NewEntry.m_strTypeName, NewEntry.m_dValue );
	}
	else
	{
		// 있으면 수정해야징~
		pEntry->m_dValue = dValue;
		WriteFormattedLog( "-- Webzen Shop -- : Update Cash, TypeName = %s, NewValue = %d\n", pEntry->m_strTypeName, pEntry->m_dValue );
	}
}

void CWebzenShop::_UpdateCashItemData( long nIndex, long nItemIndex, long nGroupCode, long nShareFlag, long nProductID, char* pCashName, double dPrice, char* pBuyerName, char cItemType, BYTE bRelationType, long nProductType )
{
	stCashItemInventoryData* pData = m_mapCashItemInventory.Get( nIndex );
	if( !pData )
	{
		// 없으면 뭐 추가해야지
		stCashItemInventoryData NewData;

		NewData.m_nIndex = nIndex;
		NewData.m_nItemIndex = nItemIndex;

		NewData.m_nGroupCode = nGroupCode;
		NewData.m_nProductType = nProductType;
		NewData.m_cItemType = cItemType;
		NewData.m_nProductID = nProductID;
		NewData.m_dPrice = dPrice;

		NewData.m_nShareFlag = nShareFlag;
		NewData.m_bRelationType = bRelationType;

		if( pCashName && strlen( pCashName ) > 0 )
		{
			strcpy_s( NewData.m_strCashName, sizeof( char ) * 20, pCashName );
		}

		if( pBuyerName && strlen( pBuyerName ) > 0 )
		{
			strcpy_s( NewData.m_strBuyerAccount, sizeof( char ) * 50, pBuyerName );
		}

		m_mapCashItemInventory.Add( nIndex, NewData );
		WriteFormattedLog( "-- Webzen Shop -- : Add CashInventoryItem, CashInventoryIndex = %d, CashItemIndex = %d, ProductID = %d\n", nIndex, nItemIndex, nProductID );
	}
	else
	{
		// 있으면 업데이트 해줘야지
		pData->m_nItemIndex = nItemIndex;

		pData->m_nGroupCode = nGroupCode;
		pData->m_nProductType = nProductType;
		pData->m_cItemType = cItemType;
		pData->m_nProductID = nProductID;
		pData->m_dPrice = dPrice;

		pData->m_nShareFlag = nShareFlag;
		pData->m_bRelationType = bRelationType;

		if( pCashName && strlen( pCashName ) > 0 )
		{
			memset( pData->m_strCashName, 0, sizeof( char ) * 20 );
			strcpy_s( pData->m_strCashName, sizeof( char ) * 20, pCashName );
		}

		if( pBuyerName && strlen( pBuyerName ) > 0 )
		{
			memset( pData->m_strBuyerAccount, 0, sizeof( char ) * 50 );
			strcpy_s( pData->m_strBuyerAccount, sizeof( char ) * 50, pBuyerName );
		}

		WriteFormattedLog( "-- Webzen Shop -- : Update CashInventoryItem, CashInventoryIndex = %d, CashItemIndex = %d, ProductID = %d\n", nIndex, nItemIndex, nProductID );
	}
}

void CWebzenShop::_UpdateCashItemProperty( long nIndex, long nItemIndex, long nProductID, long nPropertyID, int nValue )
{
	stCashItemInventoryData* pItemData = m_mapCashItemInventory.Get( nIndex );
	if( !pItemData ) return;

	int* pValue = pItemData->m_mapItemProperty.Get( nPropertyID );
	if( !pValue )
	{
		pItemData->m_mapItemProperty.Add( nPropertyID, nValue );
	}
	else
	{
		*pValue = nValue;
	}
}

stCashItemInventoryData* CWebzenShop::_GetCashInventoryItem( int nIndex )
{
	return m_mapCashItemInventory.Get( nIndex );
}

double CWebzenShop::GetCurrentCash( void )
{
	stCashDataEntry* pCash = m_mapCash.Get( WebzenCash_CashAll );
	if( !pCash ) return 0;
	return pCash->m_dValue;
}

long CWebzenShop::GetCashItemProductID( int nIndex )
{
	stCashItemInventoryData* pItemData = m_mapCashItemInventory.Get( nIndex );
	if( !pItemData ) return -1;
	return pItemData->m_nProductID;
}

double CWebzenShop::GetCashItemPrice( int nIndex )
{
	stCashItemInventoryData* pItemData = m_mapCashItemInventory.Get( nIndex );
	if( !pItemData ) return -1;
	return pItemData->m_dPrice;
}


