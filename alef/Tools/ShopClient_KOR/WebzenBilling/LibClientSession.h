#pragma once

/**************************************************************************************************

작성일: 2008-07-10
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 라이브러리의 세션관련 작업을 하는 객체

**************************************************************************************************/

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_64_MB_d.lib")
	#else
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_64_MB.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_MB_d.lib")
	#else
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_MB.lib")
	#endif
#endif

#include <Winsock2.h>
#include <Windows.h>
#include <WBANetwork.h>
#include "../ClientStub/BillEventHandler.h"
#include "../ClientStub/ShopEventHandler.h"

#include "WebzenBilling_Kor.h"

#define WEBZEN_UNION_BILLING
//////////////////////////////////////////////////////////////////////////
// Shop
class CLibClientSessionShop : public CShopEventHandler
{
public:
	CLibClientSessionShop(void);
	virtual  ~CLibClientSessionShop(void);

	CFunction_OnInquireCash	WZSHOP_OnInquireCash;
	CFunction_OnBuyProduct	WZSHOP_OnBuyProduct;
	CFunction_OnLog			WZSHOP_OnLog;
	
	//JK_웹젠빌링
	CFunction_OnInquireSalesZoneScriptVersion WZSHOP_OnInquireSalesZoneScriptVersion;
	CFunction_OnUpdateVersion WZSHOP_OnUpdateVersion;
	CFunction_OnInquireStorageListPageNoGiftMessage WZSHOP_OnInquireStorageListPageNoGiftMessage;
	CFunction_OnUseStorage WZSHOP_OnUseStorage;
	CFunction_OnRollbackUseStorage WZSHOP_OnRollbackUseStorage;

	CFunction_OnNetConnect WZSHOP_OnNetConnect;
	
	eWZConnect	m_bConnect;
	DWORD		m_nLastError;

protected:
	void WriteLog(char* szMsg);
	void OnNetConnect(bool success, DWORD error);
	void OnNetSend( int size );
	void OnNetClose( DWORD error );	
	
	void OnGiftProduct(DWORD SenderSeq, DWORD ReceiverSeq, DWORD DeductCashSeq, long ResultCode);

#ifdef WEBZEN_UNION_BILLING
	
	virtual void OnInquireCash(DWORD AccountSeq, 
		double CashSum, 
		double PointSum, 
		double MileageSum, 
		int DetailCount, 
		ShopProtocol::STCashDetail Detail[], 
		long ResultCode);

	virtual void OnBuyProduct(DWORD AccountSeq, 
		long ResultCode, 
		long LeftProductCount);

#else
	void OnInquireCash(DWORD AccountSeq, CHAR* AccountID, double CashSum, double PointSum, ShopProtocol::STCashDetail_GB Detail[], long nCashInfoCount ,long ResultCode);	
	
	void OnBuyProduct(DWORD AccountSeq, CHAR* AccountID, DWORD DeductCashSeq, DWORD InGamePurchaseSeq, long ResultCode);
#endif
	// 상품 샵 스크립트 최신 버전 정보 조회
	virtual void OnInquireSalesZoneScriptVersion(unsigned short SalesZone, 
		unsigned short Year, 
		unsigned short YearIdentity, 
		long ResultCode);

	// 배너 스크립트 최신 버전 정보 조회
	virtual void OnInquireBannerZoneScriptVersion(unsigned short BannerZone, 
		unsigned short Year, 
		unsigned short YearIdentity, 
		long ResultCode);
	// 캐시 선물 결과
	virtual void OnGiftCash(DWORD SenderSeq, 
		DWORD ReceiverSeq, 
		long ResultCode, 
		double GiftSendLimit);

	// 상품 구매/선물 가능 조회 결과
	virtual void OnInquireBuyGiftPossibility(DWORD AccountID, 
		long ResultCode, 
		long ItemBuy, 
		long Present, 
		double MyPresentCash, 
		double MyPresentSendLimit);
	// 이벤트 상품 리스트 조회 결과
	virtual void OnInquireEventProductList(DWORD AccountSeq, 
		long ProductDisplaySeq, 
		int PackagesCount, 
		long Packages[]);

	// 전시 상품 잔여 개수 조회 결과
	virtual void OnInquireProductLeftCount(long PackageSeq, long LeftCount);

	// 보관함 조회 결과
	virtual void OnInquireStorageList(DWORD AccountID, 
		long ListCount, 
		long ResultCode, 
		ShopProtocol::STStorage StorageList[]);

	// 보관함 페이지 조회 결과
	virtual void OnInquireStorageListPage(DWORD AccountID, 
		int	ResultCode, 
		char  StorageType,
		int	NowPage,
		int   TotalPage,
		int   TotalCount,
		int   ListCount, 
		ShopProtocol::STStorage StorageList[]);

	// 보관함 페이지 조회 결과 - 선물 메시지 제외
	virtual void OnInquireStorageListPageNoGiftMessage(DWORD AccountID, 
		int   ResultCode, 
		char  StorageType,
		int   NowPage,
		int   TotalPage,
		int   TotalCount,
		int   ListCount, 
		ShopProtocol::STStorageNoGiftMessage StorageList[]);

	// 보관함 상품 사용하기 결과
	virtual void OnUseStorage(DWORD AccountSeq, 
		long ResultCode, 
		char InGameProductID[MAX_TYPENAME_LENGTH], 
		BYTE PropertyCount, 
		long ProductSeq, 
		long StorageSeq, 
		long StorageItemSeq, 
		ShopProtocol::STItemProperty PropertyList[]);

	// 보관함 상품 사용하기 롤백
	virtual void OnRollbackUseStorage(DWORD AccountSeq, long ResultCode);

	// 보관함 상품 버리기 결과
	virtual void OnThrowStorage(DWORD AccountSeq, long ResultCode);

	// 마일리지 소진
	virtual void OnMileageDeduct(DWORD AccountSeq, long ResultCode);

	// 마일리지 적립
	virtual void OnMileageSave(DWORD AccountSeq, long ResultCode);

	// 실시간 마일리지 적립
	virtual void OnMileageLiveSaveUp(DWORD AccountSeq, long ResultCode);

	// 아이템 시리얼 코드 업데이트
	virtual void OnItemSerialUpdate(DWORD AccountSeq, long ResultCode);

	// 샵 스크립트 버전 정보 업데이트
	virtual void OnUpdateVersion(long GameCode, 
		unsigned short SalesZone, 
		unsigned short year, 
		unsigned short yearIdentity);

	// 배너 스크립트 버전 정보 업데이트
	virtual void OnUpdateBannerVersion(long GameCode, 
		unsigned short BannerZone, 
		unsigned short year, 
		unsigned short yearIdentity);

	// 인게임 포인트 유형별 지급 퍼센트 조회
	//		현재 헉슬리에서만 사용하기 때문에 순수 가상함수로 만들지 않는다.
	//		다른 게임에도 모두 사용하게 되면 순수 가상함수로 바꾼다.
	virtual void OnInquireInGamePointValue(long ResultCode,
		long PointCount,
		ShopProtocol::STPointDetail PointList[]);
};

//////////////////////////////////////////////////////////////////////////
// Billing
class CLibClientSessionBilling : public CBillEventHandler
{
public:
	CLibClientSessionBilling(void);
	virtual  ~CLibClientSessionBilling(void);

	eWZConnect	m_bConnect;
	DWORD		m_nLastError;

	CFunction_OnLog					WZBILLING_OnLog;
	CFunction_OnInquirePCRoomPoint	WZBILLING_OnInquirePCRoomPoint;
	CFunction_OnInquireMultiUser	WZBILLING_OnInquireMultiUser;
	CFunction_OnUserStatus			WZBILLING_OnUserStatus;
	CFunction_OnInquirePersonDeduct	WZBILLING_OnInquirePersonDeduct;

protected:		
	void WriteLog(char* szMsg);
	void OnNetConnect(bool success, DWORD error);
	void OnNetSend( int size );
	void OnNetClose( DWORD error );	

	void OnUserStatus(DWORD dwAccountGUID, long dwBillingGUID, DWORD RealEndDate, DWORD EndDate, double dRestPoint, double dRestTime, short nDeductType, short nAccessCheck, short nResultCode);
	void OnInquireMultiUser(long AccountID, long RoomGUID, long Result);
	void OnInquirePCRoomPoint(long AccountID, long RoomGUID, long GameCode, long ResultCode);
	void OnInquirePersonDeduct(long AccountGUID, long GameCode, long ResultCode);
	void OnCheckLoginUser(long AccountID, long BillingGUID);


private:
	CRITICAL_SECTION m_cs;

};

