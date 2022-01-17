#pragma once

/**************************************************************************************************

작성일: 2008-07-08
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 샵 서버를 사용하는 클라이언트에게 수신된 패킷을 전달해 주기 위한 이벤트 객체 
      이벤트를 받는쪽(샵클라이언트)에서 상속받아 사용해야 한다.

**************************************************************************************************/

#include "ClientSession.h"
#include "protocol_shop.h"

class CShopEventHandler : public CClientSession
{
public:
	CShopEventHandler(void);
	virtual  ~CShopEventHandler(void);

	// 초기화 함수 (프로그램 실행하고 한번만 사용해야 한다.)
	bool CreateSession(WBANetwork::LPFN_ErrorHandler errhandler = NULL);

	// 상품 샵 스크립트 최신 버전 정보 조회
	bool InquireSalesZoneScriptVersion(long GameCode, long SalesZone);

	// 배너 스크립트 최신 버전 정보 조회
	bool InquireBannerZoneScriptVersion(long GameCode, long BannerZone);

	// 캐시 및 포인트를 조회한다.
	// MU KR, Battery, R2
	bool InquireCash(long	GameCode,
					 BYTE	ViewType, 
					 DWORD	AccountSeq, 
					 bool	SumOnly, 
					 long	MileageSection);
	// MU Global
	bool InquireCash(BYTE	ViewType, 
					 DWORD	AccountSeq, 
					 long	GameCode,
					 bool	SumOnly, 
					 long	MileageSection);
	// MU JP - GameChu, GameOn 
	bool InquireCash(long	GameCode,
					 ShopProtocol::PaymentType	PaymentType,
					 long	AccountSeq, 
					 int	USN, 
					 BYTE	ViewType,
					 long	MileageSection,
					 bool	SumOnly);
	// Hx
	bool InquireCash(long	GameCode,
					 ShopProtocol::PaymentType	PaymentType,
					 DWORD	AccountSeq, 
					 char	AccountID[MAX_ACCOUNTID_LENGTH],
					 BYTE	ViewType,
					 long	MileageSection,
					 bool	SumOnly);

	// 물품을 구매한다.
	// MU KR, Battery, R2
	bool BuyProduct(long	GameCode,
					DWORD	AccountSeq, 
					long	PackageProductSeq, 
					long	ProductDisplaySeq, 
					long	SalesZone, 
					long	PriceSeq, 
					long	Class, 
					long	Level, 
					WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					WCHAR	Rank[MAX_RANK_LENGTH], 
					long	ServerIndex);
	// MU Global
	bool BuyProduct(DWORD	AccountSeq, 
					long	GameCode,
					long	PackageProductSeq, 
					long	PriceSeq, 
					long	SalesZone, 
					long	ProductDisplaySeq, 
					long	Class, 
					long	Level, 
					WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					WCHAR	Rank[MAX_RANK_LENGTH], 
					long	ServerIndex,
					int		CashTypeCode);
	// MU JP - GameChu, GameOn 
	bool BuyProduct(long	GameCode,
					ShopProtocol::PaymentType	PaymentType,
					DWORD	AccountSeq, 
					int		USN, 
					long	PackageProductSeq, 
					long	PriceSeq, 
					long	SalesZone, 
					long	ProductDisplaySeq, 
					long	Class, 
					long	Level, 
					WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					WCHAR	Rank[MAX_RANK_LENGTH], 
					long	ServerIndex,
					DWORD	dwIPAddress, 
					bool	DeductMileageFlag);
	// Hx
	bool BuyProduct(long	GameCode,
					ShopProtocol::PaymentType	PaymentType,
					DWORD	AccountSeq, 
					char	AccountID[MAX_ACCOUNTID_LENGTH], 
					long	PackageProductSeq, 
					long	PriceSeq, 
					long	SalesZone, 
					long	ProductDisplaySeq, 
					long	Class, 
					long	Level, 
					WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					WCHAR	Rank[MAX_RANK_LENGTH], 
					long	ServerIndex,
					DWORD	dwIPAddress, 
					char	RefKey[MAX_TYPENAME_LENGTH], 
					bool	DeductMileageFlag);

	// 상품을 선물한다.
	// MU KR, Battery, R2
	bool GiftProduct(long	GameCode,
					 DWORD	SenderSeq, 
					 long	SenderServerIndex, 
					 WCHAR	SenderCharName[MAX_CHARACTERID_LENGTH], 
					 DWORD	ReceiverSeq, 
					 long	ReceiverServerIndex, 
					 WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH], 
					 WCHAR	Message[MAX_MESSAGE_LENGTH], 
					 long	PackageProductSeq, 
					 long	PriceSeq, 
					 long	SalesZone, 
					 long	ProductDisplaySeq);
	// MU Global
	bool GiftProduct(DWORD	SenderAccountSeq, 
					WCHAR	SenderCharName[MAX_CHARACTERID_LENGTH], 
					long	SenderServerIndex,
					DWORD	ReceiverAccountSeq, 
					WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH], 
					long	ReceiverServerIndex,
					WCHAR	SendMessage[MAX_MESSAGE_LENGTH], 
					long	GameCode,
					long	PackageProductSeq, 
					long	PriceSeq, 
					long	SalesZone, 
					long	ProductDisplaySeq, 
					int		CashTypeCode);
	// MU JP - GameChu, GameOn 
	bool GiftProduct(long	GameCode,
					 ShopProtocol::PaymentType	PaymentType,
					 DWORD	AccountSeq, 
					 int	USN, 
					 WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					 long	ServerIndex, 
					 DWORD	ReceiverAccountSeq, 
					 WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH], 
					 long	ReceiverServerIndex, 
					 WCHAR	Message[MAX_MESSAGE_LENGTH], 
					 long	PackageProductSeq, 
					 long	PriceSeq, 
					 long	SalesZone, 
					 long	ProductDisplaySeq, 
					 DWORD	dwIPAddress, 
					 bool	DeductMileageFlag);
	// Hx
	bool GiftProduct(long	GameCode,
					 ShopProtocol::PaymentType	PaymentType,
					 DWORD	AccountSeq, 
					 char	AccountID[MAX_ACCOUNTID_LENGTH], 
					 WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					 long	ServerIndex, 
					 DWORD	ReceiverAccountSeq, 
					 char	ReceiverAccountID[MAX_ACCOUNTID_LENGTH], 
					 WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH], 
					 long	ReceiverServerIndex, 
					 WCHAR	Message[MAX_MESSAGE_LENGTH], 
					 long	PackageProductSeq, 
					 long	PriceSeq, 
					 long	SalesZone, 
					 long	ProductDisplaySeq, 
					 DWORD	dwIPAddress, 
					 char	RefKey[MAX_TYPENAME_LENGTH],
					 bool	DeductMileageFlag);

	// 캐시를 선물한다.
	bool GiftCash(DWORD		SenderAccountSeq, 
				  long		SendServerIndex, 
				  WCHAR		SendCharName[MAX_CHARACTERID_LENGTH], 
				  DWORD		ReceiverAccountID, 
				  long		ReceiverServerIndex, 
				  WCHAR		ReceiverCharName[MAX_CHARACTERID_LENGTH], 
				  WCHAR		Message[MAX_MESSAGE_LENGTH], 
				  double	CashValue, 
				  long		GameCode);
		
	// 상품 구매/선물 가능 여부를 조회한다.
	bool InquireBuyGiftPossibility(DWORD AccountSeq, long GameCode);	

	// 이벤트 상품 목록을 조회한다.
	bool InquireEventProductList(long GameCode, DWORD AccountSeq, long SalesZone, long ProductDisplaySeq);

	// 전시 상품 잔여 개수 조회
	bool InquireProductLeftCount(long GameCode, long PackageSeq);

	// 보관함 리스트를 조회한다.
	bool InquireStorageList(DWORD AccountSeq, 
							long GameCode, 
							long SalesZone);

	// 보관함 리스트 페이지를 조회한다.
	bool InquireStorageListPage(DWORD AccountSeq, 
								int GameCode, 
								int SalesZone, 
								char StorageType, 
								int NowPage, 
								int PageSize);

	// 보관함 리스트 페이지를 조회한다. - 선물 메시지 제외
	bool InquireStorageListPageNoGiftMessage(DWORD AccountSeq, 
											 int GameCode, 
											 int SalesZone, 
											 char StorageType, 
											 int NowPage, 
											 int PageSize);

	// 보관함에 상품을 사용한다.
	bool UseStorage(DWORD	AccountSeq, 
					long	GameCode, 
					DWORD	IPAddress, 
					long	StorageSeq, 
					long	StorageItemSeq, 
					char	StorageItemType, 
					long	Class, 
					long	Level, 
					WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					WCHAR	Rank[MAX_RANK_LENGTH], 
					long	ServerIndex);

	// 보관함 상품 사용하기 롤백
	bool RollbackUseStorage(DWORD	AccountSeq, 
							long	GameCode, 
							long	StorageSeq, 
							long	StorageItemSeq);

	// 보관함에 상품을 버린다.
	bool ThrowStorage(DWORD	AccountSeq, 
					  long	GameCode, 
					  long	StorageSeq, 
					  long	StorageItemSeq, 
					  char	StorageItemType);

	// 마일리지 소진
	bool MileageDeduct(DWORD	AccountSeq, 
					   long		GameCode, 
					   long		DeductCategory, 
					   long		MileageSection, 
					   long		MileageDeductPoint, 
					   long		Class, 
					   long		Level, 
					   WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					   WCHAR	Rank[MAX_RANK_LENGTH], 
					   long		ServerIndex);

	// 마일리지 적립
	bool MileageSave(DWORD	AccountSeq, 
					 long	GameCode, 
					 long	MileageType, 
					 long	MileageSection, 
					 long	MileagePoint, 
					 long	Class, 
					 long	Level, 
					 WCHAR	CharName[MAX_CHARACTERID_LENGTH], 
					 WCHAR	Rank[MAX_RANK_LENGTH], 
					 long	ServerIndex);

	// 실시간 마일리지 적립
	bool MileageLiveSaveUp(DWORD AccountSeq, 
						   long	 GameCode, 
						   long	 MileageSection, 
						   long	 SourceType);

	// 아이템 시리얼 코드 업데이트
	bool ItemSerialUpdate(DWORD	AccountSeq, 
						  long	GameCode, 
						  long	StorageSeq, 
						  long	StorageItemSeq, 
						  INT64 InGameUseCode);
	
	// 인게임 포인트 유형별 지급 퍼센트 조회
	bool InquireInGamePointValue(long  GameCode,
								 long  ServerType,
								 long  AccessType);


protected:

	void OnReceive(PBYTE buffer, int size);
	void OnConnect(bool success, DWORD error);
	void OnSend(int size);
	void OnClose(DWORD error);	

	// 상품 샵 스크립트 최신 버전 정보 조회
	virtual void OnInquireSalesZoneScriptVersion(unsigned short SalesZone, 
												 unsigned short Year, 
												 unsigned short YearIdentity, 
												 long ResultCode) = 0;

	// 배너 스크립트 최신 버전 정보 조회
	virtual void OnInquireBannerZoneScriptVersion(unsigned short BannerZone, 
												  unsigned short Year, 
												  unsigned short YearIdentity, 
												  long ResultCode) = 0;

	// 캐시 조회의 결과
	// MU KR, Battery, R2
	virtual void OnInquireCash(DWORD AccountSeq, 
							   double CashSum, 
							   double PointSum, 
							   double MileageSum, 
							   int DetailCount, 
							   ShopProtocol::STCashDetail Detail[], 
							   long ResultCode);
	// MU Global
	virtual void OnInquireCash(DWORD AccountSeq, 
							   double CashSum, 
							   double PointSum, 
							   double MileageSum, 
							   int ListCount, 
							   ShopProtocol::STCashDetail_GB Detail[], 
							   long ResultCode);
	// MU JP - GameChu, GameOn 
	virtual void OnInquireCash(DWORD AccountSeq, 
							   long ResultCode,
							   long OutBoundResultCode,
							   double CashSum, 
							   double MileageSum);
	// Hx
	virtual void OnInquireCash(DWORD AccountSeq, 
							   char AccountID[MAX_ACCOUNTID_LENGTH], 
							   double CashSum, 
							   double PointSum, 
							   double MileageSum, 
							   int DetailCount, 
							   ShopProtocol::STCashDetail Detail[], 
							   long ResultCode,
							   long OutBoundResultCode);

	// 상품 구매의 결과
	// MU KR, MU Global, Battery, R2
	virtual void OnBuyProduct(DWORD AccountSeq, 
							  long ResultCode, 
							  long LeftProductCount);
	// MU JP - GameChu, GameOn 
	virtual void OnBuyProduct(DWORD AccountSeq, 
							  long ResultCode,
							  long OutBoundResultCode,
							  long LeftProductCount);
	// Hx
	virtual void OnBuyProduct(DWORD AccountSeq, 
							  char AccountID[MAX_ACCOUNTID_LENGTH], 
							  long LeftProductCount, 
							  long ResultCode,
							  long OutBoundResultCode);

	// 상품 선물의 결과
	// MU KR, MU Global, Battery, R2
	virtual void OnGiftProduct(DWORD SenderAccountSeq, 
							   DWORD ReceiverAccountSeq, 
							   double GiftSendLimit, 
							   long LeftProductCount, 
							   long ResultCode);
	// MU JP - GameChu, GameOn 
	virtual void OnGiftProduct(DWORD  SenderAccountSeq, 
							   long   ResultCode,
							   long   OutBoundResultCode,
							   DWORD  ReceiverAccountSeq, 
							   long   LeftProductCount);
	// Hx
	virtual void OnGiftProduct(DWORD  SenderAccountSeq, 
							   char   SenderAccountID[MAX_ACCOUNTID_LENGTH], 
							   WCHAR  SenderCharName[MAX_CHARACTERID_LENGTH], 
							   DWORD  ReceiverAccountSeq, 
							   char   ReceiverAccountID[MAX_ACCOUNTID_LENGTH], 
							   WCHAR  ReceiverCharName[MAX_CHARACTERID_LENGTH], 
							   WCHAR  Message[MAX_MESSAGE_LENGTH], 
							   double LimitedCash, 
							   long   LeftProductCount, 
							   long   ResultCode,
							   long   OutBoundResultCode);

	// 캐시 선물 결과
	virtual void OnGiftCash(DWORD SenderSeq, 
							DWORD ReceiverSeq, 
							long ResultCode, 
							double GiftSendLimit) = 0;

	// 상품 구매/선물 가능 조회 결과
	virtual void OnInquireBuyGiftPossibility(DWORD AccountID, 
											 long ResultCode, 
											 long ItemBuy, 
											 long Present, 
											 double MyPresentCash, 
											 double MyPresentSendLimit) = 0;	

	// 이벤트 상품 리스트 조회 결과
	virtual void OnInquireEventProductList(DWORD AccountSeq, 
										   long ProductDisplaySeq, 
										   int PackagesCount, 
										   long Packages[]) = 0;

	// 전시 상품 잔여 개수 조회 결과
	virtual void OnInquireProductLeftCount(long PackageSeq, long LeftCount) = 0;

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
										  ShopProtocol::STStorage StorageList[]) = 0;

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
							  ShopProtocol::STItemProperty PropertyList[]) = 0;

	// 보관함 상품 사용하기 롤백
	virtual void OnRollbackUseStorage(DWORD AccountSeq, long ResultCode) = 0;

	// 보관함 상품 버리기 결과
	virtual void OnThrowStorage(DWORD AccountSeq, long ResultCode) = 0;

	// 마일리지 소진
	virtual void OnMileageDeduct(DWORD AccountSeq, long ResultCode) = 0;
	
	// 마일리지 적립
	virtual void OnMileageSave(DWORD AccountSeq, long ResultCode) = 0;
	
	// 실시간 마일리지 적립
	virtual void OnMileageLiveSaveUp(DWORD AccountSeq, long ResultCode) = 0;
	
	// 아이템 시리얼 코드 업데이트
	virtual void OnItemSerialUpdate(DWORD AccountSeq, long ResultCode) = 0;
	
	// 샵 스크립트 버전 정보 업데이트
	virtual void OnUpdateVersion(long GameCode, 
								 unsigned short SalesZone, 
								 unsigned short year, 
								 unsigned short yearIdentity) = 0;

	// 배너 스크립트 버전 정보 업데이트
	virtual void OnUpdateBannerVersion(long GameCode, 
									   unsigned short BannerZone, 
									   unsigned short year, 
									   unsigned short yearIdentity) = 0;

	// 인게임 포인트 유형별 지급 퍼센트 조회
	//		현재 헉슬리에서만 사용하기 때문에 순수 가상함수로 만들지 않는다.
	//		다른 게임에도 모두 사용하게 되면 순수 가상함수로 바꾼다.
	virtual void OnInquireInGamePointValue(long ResultCode,
										   long PointCount,
										   ShopProtocol::STPointDetail PointList[]);
	
	
	// 서버에 연결한 결과를 알려줍니다.
	virtual void OnNetConnect(bool success, DWORD error) = 0;
	// 서버에 데이타를 전송한 결과를 알려줍니다.
	virtual void OnNetSend( int size ) = 0;
	// 서버와 접속이 종료되었을때 발생합니다.
	virtual void OnNetClose( DWORD error ) = 0;	
	// 로그를 써야할때 보내는 이벤트 메소드
	virtual void WriteLog(char* szMsg) = 0;

private:
	// 로그를 사용한다.
	void WriteLog(const char* szFormat, ...);
};
