#ifndef __CLASS_WEBZEN_SHOP_H__
#define __CLASS_WEBZEN_SHOP_H__




#include "CWebzenUtility.h"
#include <WBANetwork.h>
#include "ShopEventHandler.h"
#include "ContainerUtil.h"



enum eWebzenCashType
{
	WebzenCash_TotalAll = 0,
	WebzenCash_CashAll,
	WebzenCash_PointAll,
	WebzenCash_PointBonus,
	WebzenCash_PointGame,
	WebzenCash_CashNormal,
	WebzenCash_CashEvent,
	WebzenCash_Mileage,
};

struct stCashDataEntry
{
	eWebzenCashType														m_eType;
	double																m_dValue;
	char																m_strTypeName[ 32 ];

	stCashDataEntry( void )
	{
		m_eType = WebzenCash_TotalAll;
		m_dValue = 0;
		memset( m_strTypeName, 0, sizeof( char ) * 32 );
	}
};

// 유료보관함 데이터 항목 정의.. ㅡ.ㅡ.. 사실 요게 젤 중요..
struct stCashItemInventoryData
{
	long																m_nIndex;					// 보관함 고유번호
	long																m_nItemIndex;				// 아이템의 고유번호

	long																m_nGroupCode;				// 보관함 그룹코드 : 아크로드에서는 안쓰.. 지만 일단 놔둠;;
	long																m_nProductType;				// 상품타입
	char																m_cItemType;				// 아이템 타입 : C일경우 캐쉬, P일경우 상품.. 상품권처럼 캐쉬충전하는 아이템을 구매한 경우가 있으니..
	long																m_nProductID;				// 상품 고유코드
	double																m_dPrice;					// 상품 가격

	long																m_nShareFlag;				// 서버유형... 이라는데 이벤트서버인지 일반서버인지등등인듯 한데.. 아크로드에서는 안쓸듯?
	BYTE																m_bRelationType;			// 보관함유형구분.. 이라 함.. 산거냐 공짜로 받은거냐 선물받은 거냐 등등을 구분한다 함..

	char																m_strCashName[ 20 ];		// 이 아이템을 구매하는데 사용한 캐쉬이름.. 아마 아크로드에서는 안쓸듯..
	char																m_strBuyerAccount[ 50 ];	// 구매자 계정ID 보통은 자기자신.. 선물받은 경우 보낸 사람

	ContainerMap< long, int >											m_mapItemProperty;			// 아이템 속성값

	stCashItemInventoryData( void )
	{
		m_nIndex = -1;
		m_nItemIndex = -1;

		m_nGroupCode = -1;
		m_nProductType = -1;
		m_cItemType = -1;
		m_nProductID = -1;
		m_dPrice = 0;

		m_nShareFlag = -1;
		m_bRelationType = 0;

		memset( m_strCashName, 0, sizeof( char ) * 20 );
		memset( m_strBuyerAccount, 0, sizeof( char ) * 50 );
	}
};

class CWebzenShop : public CShopEventHandler, public CWebzenUtility
{
private :
	char																m_strServerAddress[ 64 ];
	int																	m_nServerPort;

	BOOL																m_bIsStarted;

	ContainerMap< eWebzenCashType, stCashDataEntry >					m_mapCash;
	ContainerMap< long, stCashItemInventoryData >						m_mapCashItemInventory;		// 유료보관함 데이터

	int																	m_nPageCur;
	int																	m_nPageMax;
	int																	m_nViewCountPerPage;

	long																m_nGameCode;
	unsigned short														m_nSalesZoneCode;

public :
	CWebzenShop( void );
	virtual ~CWebzenShop( void );

public :
	BOOL						OnCreateSession							( void );
	BOOL						OnWebzenShopConnect						( char* pServerAddress, int nPort );
	BOOL						OnWebzenShopDisConnect					( void );

public :
	static void					fnErrorHandler							( DWORD dwLastError, TCHAR* pErrorMsg );

public :
	virtual void				OnNetConnect							( bool bIsSuccess, DWORD dwError );
	virtual void				OnNetClose								( DWORD dwError );
	virtual void				OnNetSend								( int nSendSize );

	virtual void				WriteLog								( char* szMsg );

public :
	virtual void				OnInquireCash							( DWORD dwAccountID, double dCashSum, double dPointSum, double dMileageSum, int nDetailCount, ShopProtocol::STCashDetail Detail[], long nResultCode );
	virtual void				OnBuyProduct							( long nAccountID, long nResultCode, long nRemainProductCount );
	virtual void				OnInquireStorageListPageNoGiftMessage	( DWORD dwAccountID, long nResultCode, char cInventoryType, int nPageNumber, int nTotalPageCount, int nTotalProductCount, long nItemCount, ShopProtocol::STStorageNoGiftMessage ItemData[] = 0 );
	virtual void				OnUseStorage							( DWORD dwAccountID, long nResultCode, char strInGameProductID[ MAX_TYPENAME_LENGTH ], BYTE bPropertyCount, long nProductID, long nIndex, long nItemIndex, ShopProtocol::STItemProperty ItemProperty[] = 0 );
	virtual void				OnRollbackUseStorage					( DWORD dwAccountID, long nResultCode );
	virtual void				OnThrowStorage							( DWORD dwAccountID, long nResultCode );
	virtual void				OnItemSerialUpdate						( DWORD dwAccountID, long nResultCode );
	virtual void				OnInquireSalesZoneScriptVersion			( unsigned short nSalesZoneCode, unsigned short nYear, unsigned short nYearIdentity, long nResultCode );
	virtual void				OnUpdateVersion							( long nGameCode, unsigned short nSalesZoneCode, unsigned short nYear, unsigned short nYearIdentity );

public :
	BOOL						SendGetCash								( DWORD dwAccountID, eWebzenCashType eType, long nMileageType, BOOL bOnlyTotal = FALSE );
	BOOL						SendBuyProduct							( DWORD dwAccountID, long nPackageID, long nCategoryID, long nSalesZoneCode, long nPriceID, long nCharacterClass, long nCharacterLevel, long nServerID, char* pCharacterName );
	BOOL						SendGetCashItemInventoryData			( DWORD dwAccountID, int nPageNumber );
	BOOL						SendUseCashItem							( DWORD dwAccountID, DWORD dwIPAddress, long nIndex, int nItemIndex, char cItemType, long nCharacterClass, long nCharacterLevel, long nServerIndex, char* pCharacterName );
	BOOL						SendUseCashItemCancel					( DWORD dwAccountID, long nIndex, long nItemIndex );
	BOOL						SendDeleteCashItem						( DWORD dwAccountID, long nIndex, long nItemIndex, char cItemType );
	BOOL						SendUpdateItemCode						( DWORD dwAccountID, long nIndex, long nItemIndex, __int64 nItemCode );
	BOOL						SendCheckShopScriptVersion				( void );

private :
	void						_UpdateCashData							( char cCashType, double dValue, char* pName );
	void						_UpdateCashItemData						( long nIndex, long nItemIndex, long nGroupCode, long nShareFlag, long nProductID, char* pCashName, double dPrice, char* pBuyerName, char cItemType, BYTE bRelationType, long nProductType );
	void						_UpdateCashItemProperty					( long nIndex, long nItemIndex, long nProductID, long nPropertyID, int nValue );

	stCashItemInventoryData*	_GetCashInventoryItem					( int nIndex );

public :
	char*						GetShopServerAddress					( void ) { return m_strServerAddress; }
	int							GetShopServerPort						( void ) { return m_nServerPort; }

	BOOL						IsShopStarted							( void ) { return m_bIsStarted; }

	long						GetGameCode								( void ) { return m_nGameCode; }
	unsigned short				GetSalesZoneCode						( void ) { return m_nSalesZoneCode; }

	int							GetPageCurrent							( void ) { return m_nPageCur; }
	int							GetPageMax								( void ) { return m_nPageMax; }

public :
	void						SetViewCountPerPage						( int nCount ) { m_nViewCountPerPage = nCount; }
	double						GetCurrentCash							( void );
	long						GetCashItemProductID					( int nIndex );
	double						GetCashItemPrice						( int nIndex );
};




#endif