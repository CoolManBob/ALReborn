#pragma once

#ifdef _WEBZEN_BILLING_

#ifdef _WIN64
#ifdef _DEBUG
#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB_d.lib")
#else
#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB.lib")
#endif
#else
#ifdef _DEBUG
#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB_d.lib")
#else
#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB.lib")
#endif
#endif


//#pragma comment(lib, "atls.lib")
//#include "ApBase.h"

//#include "Base.h"
#include "WBANetwork.h"
#include "util/Stream.h"
#include "ShopEventHandler.h"


//class CUser;
//class CPlayer;

class AgsmBillingWebzen : public CShopEventHandler
{
public:
	AgsmBillingWebzen(void);
	~AgsmBillingWebzen(void);

	void Start( BOOL bFirstStarUp = TRUE );
	void Stop();

	inline int		GetYear();
	inline int		GetYearIdentity();

	inline BOOL		IsNumberFromItemID( char* pData, int size );
	inline void		SetConnect(BOOL bConnect);
	inline BOOL		IsConnect();

protected:
	//빌링서버의 로그를 작성 이벤트 이다.
	//함수내부에서 로그를 남길수 있도록 해야한다.
	void WriteLog(char* szMsg);

	//빌링서버의 연결 성공 여부 (새로 연결되면 현재 차감이 필요한 모든 유저의 정보를 다시 로그인해야한다.)
	//		success: 연결 성공 여부
	//		error: 에러코드
	void OnNetConnect(bool success, DWORD error);

	//빌링서버로 보낸 데이타 전송 여부
	//		size: 전송된 패킷 사이즈
	void OnNetSend( int size );

	//빌링서버와 세션이 끊겼을때 (빌링서버가 죽은것으로 처리 해야한다.)	
	//		error: 에러코드
	void OnNetClose( DWORD error );	



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

	// 캐쉬 조회의 결과
	// MU
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
	// MU
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
	// MU
	virtual void OnGiftProduct(DWORD SenderSeq, 
		DWORD ReceiverSeq, 
		double LimitedCash, 
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

	// 상품 샵 스크립트 버전 정보 업데이트 알림
	virtual void OnUpdateVersion(long GameCode, 
		unsigned short SalesZone, 
		unsigned short year, 
		unsigned short yearIdentity);

	// 배너 스크립트 버전 정보 업데이트 알림
	virtual void OnUpdateBannerVersion(long GameCode, 
		unsigned short BannerZone, 
		unsigned short year, 
		unsigned short yearIdentity);

	// 인게임 포인트 유형별 지급 퍼센트 조회
	virtual void OnInquireInGamePointValue(long ResultCode,
		long PointCount,
		ShopProtocol::STPointDetail PointList[]);


	// 타이머
//	void			OnTimer( int nId );

private:
	// 빌링락
//	CCriticalSection					m_BillingLock;			

	int									m_nYear;
	int									m_nYearIdentity;
	BOOL								m_bConnect;
};

inline void	AgsmBillingWebzen::SetConnect(BOOL bConnect) {	/*CCriticalSectionLock theLock( &m_BillingLock ); */m_bConnect = bConnect;}
inline BOOL	AgsmBillingWebzen::IsConnect() { /*CCriticalSectionLock theLock( &m_BillingLock );*/ return m_bConnect; }

inline BOOL	AgsmBillingWebzen::IsNumberFromItemID( char* pData, int nSize )
{
	for(int i = 0 ; i < nSize; i++)
	{
		if(isdigit(pData[i]) == false)
			return FALSE;
	}
	return TRUE;
}

inline int AgsmBillingWebzen::GetYear()  { /*CCriticalSectionLock theLock( &m_BillingLock );*/ return m_nYear; }
inline int AgsmBillingWebzen::GetYearIdentity()  {	/*CCriticalSectionLock theLock( &m_BillingLock );*/	return m_nYearIdentity;}

/*
//////////////////////////////////////////////////////////////////////
//	GLOBAL VARIABLE.
extern	AgsmBillingWebzen	g_cBillingSystem;

//////////////////////////////////////////////////////////////////////
//	Nation Code For Billing

inline int GetSalesZoneBilling()
{
	// Nation Billing Code for USA 
	if(IsNation( SERVICE_USA )) 
		return PAYITEM_SALEZONE_USA;

	// Nation Billing Code for Korea 
	if(IsNation( SERVICE_KOR )) 
		return PAYITEM_SALEZONE_KOREA;
	return 0;
}

inline int GetGameCodeBilling()
{
	// Nation Billing Code for USA 
	if(IsNation( SERVICE_USA )) 
		return PAYITEM_GAMECODE_USA;

	// Nation Billing Code for Korea 
	if(IsNation( SERVICE_KOR )) 
		return PAYITEM_GAMECODE_KOREA;
	return 0;
}
*/


#endif //_WEBZEN_BILLING_