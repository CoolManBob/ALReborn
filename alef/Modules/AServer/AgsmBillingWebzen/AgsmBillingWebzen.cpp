
#include <winsock2.h>
#include "AgsmBillingWebzen.h"


#ifdef _WEBZEN_BILLING_

AgsmBillingWebzen::AgsmBillingWebzen(void) : m_bConnect(FALSE)
{
}

AgsmBillingWebzen::~AgsmBillingWebzen(void)
{
}

void AgsmBillingWebzen::OnNetConnect(bool success, DWORD error)
{
	/*
	if(success)
	{
		LOG_BILLING("Billing System Network Connect SUCCESS !");

		USES_CONVERSION;

		// 스크립트 버전을 최초로 요청한다.
		InquireSalesZoneScriptVersion(GetGameCodeBilling(), GetSalesZoneBilling());
		SetConnect(TRUE);
	}
	else 
	{
		LOG_BILLING("Billing System Network Connect FAIL !");

		// 연결이 근어지면 재접속 시도
		AddTimer(BILLING_RECONNECT_TIME_GAP, TIMER_ID_BILLING_RECONNECT);
		SetConnect(FALSE);
	}
	*/
}

void AgsmBillingWebzen::OnNetSend( int size )
{
	/*
	char szLog[ARRY_BILLING_LOG];
	::sprintf_s(szLog, ARRY_BILLING_LOG, "Sending Byte From Billing System [ %d ]", size );
	LOG_BILLING(szLog);
	*/
}

void AgsmBillingWebzen::OnNetClose( DWORD error )
{
	/*
	char szLog[ARRY_BILLING_LOG];
	::sprintf_s(szLog, ARRY_BILLING_LOG, "Billing System Net Closed [ %d ]", error );
	LOG_BILLING(szLog);

	// 연결이 근어지면 재접속 시도
	AddTimer(BILLING_RECONNECT_TIME_GAP, TIMER_ID_BILLING_RECONNECT);
	*/
	SetConnect(FALSE);
}

void AgsmBillingWebzen::WriteLog(char* szMsg)
{
}

void AgsmBillingWebzen::Start( BOOL bFirstStarUp )
{
	// 시스템을 지원하는가 ?
//	if( !( SYSTEM_BILLING & CONFIG_SYSTEM) )
//		return;

	// 처음으로 시작하는가?
	if( bFirstStarUp )
		CreateSession(NULL);

//	Connect((TCHAR*)CONFIG_BILLINGIP, (unsigned short)CONFIG_BILLINGPORT);
//	LOG_BILLING( "Billing System try to Connect [ IP : %s - PORT : %d ]", CONFIG_BILLINGIP, CONFIG_BILLINGPORT );
}

void AgsmBillingWebzen::Stop()
{
//	if( !( SYSTEM_BILLING & CONFIG_SYSTEM) )
//		return;

	Close();
}


// 상품 샵 스크립트 최신 버전 정보 조회
void AgsmBillingWebzen::OnInquireSalesZoneScriptVersion(unsigned short SalesZone, 
											  unsigned short Year, 
											  unsigned short YearIdentity, 
											  long ResultCode)
{
	/*
	CString strMsg;

	strMsg.Format(_T("상품 샵 스크립트 최신 버전 : %d.%d.%d, 결과: %d\r\n"), 
		SalesZone, Year, YearIdentity, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// 배너 스크립트 최신 버전 정보 조회
void AgsmBillingWebzen::OnInquireBannerZoneScriptVersion(unsigned short BannerZone, 
											   unsigned short Year, 
											   unsigned short YearIdentity, 
											   long ResultCode)
{
	/*
	CString strMsg;

	strMsg.Format(_T("배너 스크립트 최신 버전 : %d.%d.%d, 결과: %d\r\n"), 
		BannerZone, Year, YearIdentity, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// MU KR, Battery, R2
void AgsmBillingWebzen::OnInquireCash(DWORD AccountSeq, 
							double CashSum, 
							double PointSum, 
							double MileageSum, 
							int DetailCount, 
							ShopProtocol::STCashDetail Detail[], 
							long ResultCode)
{
	/*
	CString strMsg, strDetail;

	strMsg.Format(_T("캐시 조회 결과\r\n조회한 사람: %d, 캐시: %f, 포인트: %f, 마일리지: %f, 결과: %d, \r\n세부 정보: \r\n"), 
		AccountSeq, CashSum, PointSum, MileageSum, ResultCode);

	USES_CONVERSION;

	for(int i = 0 ; i < DetailCount ;i++)
	{
		if(0 != Detail[i].Type)
		{
			strDetail.Format(_T("[이름: %s, 종류: %c, 값:%f] \r\n"),
				W2A(Detail[i].Name), Detail[i].Type, Detail[i].Value);
			strMsg = strMsg + strDetail;
		}
	}

	strMsg += "\r\n";

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// MU GB
void AgsmBillingWebzen::OnInquireCash(DWORD AccountSeq, 
							double CashSum, 
							double PointSum, 
							double MileageSum, 
							int ListCount, 
							ShopProtocol::STCashDetail_GB Detail[], 
							long ResultCode)
{
	/*
	CString strMsg, strDetail;

	strMsg.Format(_T("캐시 조회 결과\r\n조회한 사람: %d, 캐시: %f, 포인트: %f, 마일리지: %f, 결과: %d, \r\n세부 정보: \r\n"), 
		AccountSeq, CashSum, PointSum, MileageSum, ResultCode);

	USES_CONVERSION;

	for(int i = 0 ; i < ListCount ;i++)
	{
		if(0 != Detail[i].Type)
		{
			strDetail.Format(_T("[이름: %s, 종류: %c, 값: %f, 캐시코드: %d] \r\n"), 
				W2A(Detail[i].Name), Detail[i].Type, Detail[i].Value, Detail[i].CashTypeCode);
			strMsg = strMsg + strDetail;
		}
	}

	strMsg += "\r\n";

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// MU JP - GameChu, GameOn
void AgsmBillingWebzen::OnInquireCash(DWORD AccountSeq, 
							long ResultCode,
							long OutBoundResultCode,
							double CashSum, 
							double MileageSum)
{
	/*
	CString strMsg, strDetail;

	strMsg.Format(_T("캐시 조회 결과\r\n조회한 사람: %d, 캐시: %f, 마일리지: %f, 결과: %d, 외부모듈결과: %d\r\n"), 
		AccountSeq, CashSum, MileageSum, ResultCode, OutBoundResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// Hx
void AgsmBillingWebzen::OnInquireCash(DWORD AccountSeq, 
							char AccoundID[MAX_ACCOUNTID_LENGTH], 
							double CashSum, 
							double PointSum, 
							double MileageSum, 
							int DetailCount, 
							ShopProtocol::STCashDetail Detail[], 
							long ResultCode,
							long OutBoundResultCode)
{
	/*
	CString strMsg, strDetail;

	strMsg.Format(_T("캐시 조회 결과\r\n조회한 사람: %d, %s, 캐시: %f, 포인트: %f, 마일리지: %f, 결과: %d, 외부모듈결과: %d, \r\n세부 정보: \r\n"), 
		AccountSeq, AccoundID, CashSum, PointSum, MileageSum, ResultCode, OutBoundResultCode);

	USES_CONVERSION;

	for(int i = 0 ; i < DetailCount ;i++)
	{
		if(0 != Detail[i].Type)
		{
			strDetail.Format(_T("[이름: %s, 종류: %c, 값:%f] \r\n"), W2A(Detail[i].Name), Detail[i].Type, Detail[i].Value);
			strMsg = strMsg + strDetail;
		}
	}

	strMsg += "\r\n";

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}


// MU KR, GB, Battery, R2
void AgsmBillingWebzen::OnBuyProduct(DWORD AccountSeq, 
						   long ResultCode, 
						   long LeftProductCount)
{
	/*
	CString strMsg;

	strMsg.Format(_T("상품 구매 결과\r\n구매한 사람: %d, 결과: %d, 상품 판매 가능 수량: %d\r\n"), 
		AccountSeq, ResultCode, LeftProductCount);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// MU JP - GameChu, GameOn 
void AgsmBillingWebzen::OnBuyProduct(DWORD AccountSeq, 
						   long ResultCode,
						   long OutBoundResultCode,
						   long LeftProductCount)
{
	/*
	CString strMsg;

	strMsg.Format(_T("상품 구매 결과\r\n구매한 사람: %d, 결과: %d, 외부모듈결과: %d, 상품 판매 가능 수량: %d\r\n"), 
		AccountSeq, ResultCode, OutBoundResultCode, LeftProductCount);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// Hx
void AgsmBillingWebzen::OnBuyProduct(DWORD AccountSeq, 
						   char AccoundID[MAX_ACCOUNTID_LENGTH], 
						   long LeftProductCount, 
						   long ResultCode,
						   long OutBoundResultCode)
{
	/*
	CString strMsg;

	strMsg.Format(_T("상품 구매 결과\r\n구매한 사람: %d(%s), 결과: %d, 외부모듈결과: %d, 상품 판매 가능 수량: %d\r\n"), 
		AccountSeq, AccoundID, ResultCode, OutBoundResultCode, LeftProductCount);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// MU KR, GB, Battery, R2
void AgsmBillingWebzen::OnGiftProduct(DWORD SenderSeq, 
							DWORD ReceiverSeq, 
							double LimitedCash, 
							long LeftProductCount, 
							long ResultCode)
{
	/*
	CString strMsg;

	strMsg.Format(_T("상품 선물 결과\r\n보낸사람: %d, 받는사람: %d, 결과: %d, 한도초과시 선물가능캐시: %f, 상품 판매 가능 수량: %d\r\n"), 
		SenderSeq, ReceiverSeq, ResultCode, LimitedCash, LeftProductCount);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// MU JP - GameChu, GameOn 
void AgsmBillingWebzen::OnGiftProduct(DWORD  SenderAccountSeq, 
							long   ResultCode,
							long   OutBoundResultCode,
							DWORD  ReceiverAccountSeq, 
							long   LeftProductCount)
{
	/*
	CString strMsg;
	USES_CONVERSION;

	strMsg.Format(_T("상품 선물 결과\r\n보낸사람: %d, 받는사람: %d, 결과: %d, 외부모듈결과: %d, 상품 판매 가능 수량: %d\r\n"), 
		SenderAccountSeq, ReceiverAccountSeq, ResultCode, OutBoundResultCode, LeftProductCount);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// Hx
void AgsmBillingWebzen::OnGiftProduct(DWORD  SenderAccountSeq, 
							char   SenderAccountID[MAX_ACCOUNTID_LENGTH], 
							WCHAR  SenderCharName[MAX_CHARACTERID_LENGTH], 
							DWORD  ReceiverAccountSeq, 
							char   ReceiverAccountID[MAX_ACCOUNTID_LENGTH], 
							WCHAR  ReceiverCharName[MAX_CHARACTERID_LENGTH], 
							WCHAR  Message[MAX_MESSAGE_LENGTH], 
							double LimitedCash, 
							long   LeftProductCount, 
							long   ResultCode,
							long   OutBoundResultCode)
{
	/*
	CString strMsg;
	USES_CONVERSION;

	strMsg.Format(_T("상품 선물 결과\r\n보낸사람: %d(%s:%s), 받는사람: %d(%s:%s), 메시지: %s, 결과: %d, 외부모듈결과: %d, 한도초과시 선물가능캐시: %f, 상품 판매 가능 수량: %d\r\n"), 
		SenderAccountSeq, SenderAccountID, W2A(SenderCharName), ReceiverAccountSeq, ReceiverAccountID, W2A(ReceiverCharName), W2A(Message), 
		ResultCode, OutBoundResultCode, LimitedCash, LeftProductCount);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnInquireStorageList(DWORD AccountID, 
								   long ListCount, 
								   long ResultCode, 
								   ShopProtocol::STStorage StorageList[])
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("보관함 요청 결과\r\n결과 코드: %d, 요청자: %d, 상품 개수: %d\r\n"), 
		ResultCode, AccountID, ListCount);

	USES_CONVERSION;

	for(int i = 0 ; i < ListCount ; i++)
	{
		strProDuct.Format(_T("%d, %d, %d, %d, %d, %s, %f, %s, %s, %c, %d, %d, %d\r\n"), 
			StorageList[i].Seq, 
			StorageList[i].ItemSeq, 
			StorageList[i].GroupCode, 
			StorageList[i].ShareFlag, 
			StorageList[i].ProductSeq, 
			W2A(StorageList[i].CashName), 
			StorageList[i].CashPoint, 		
			W2A(StorageList[i].SendAccountID), 
			W2A(StorageList[i].SendMessage), 	
			StorageList[i].ItemType, 
			StorageList[i].RelationType, 
			StorageList[i].PriceSeq,
			StorageList[i].ProductType);

		strMsg += strProDuct;
	}

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnInquireStorageListPage(DWORD AccountID, 
									   int	 ResultCode, 
									   char  StorageType,
									   int   NowPage,
									   int   TotalPage,
									   int   TotalCount,
									   int   ListCount, 
									   ShopProtocol::STStorage StorageList[])
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("보관함 페이지 요청 결과\r\n결과 코드: %d, 요청자: %d, 보관함타입:%c, 현재페이지: %d, 전체페이지수: %d, 전체상품수:%d, 현재페이지상품수: %d\r\n"), 
		ResultCode, AccountID, StorageType ? StorageType : ' ', 
		NowPage, TotalPage, TotalCount, ListCount);

	USES_CONVERSION;

	for(int i = 0 ; i < ListCount ; i++)
	{
		strProDuct.Format(_T("%d, %d, %d, %d, %d, %s, %f, %s, %s, %c, %d, %d, %d\r\n"), 
			StorageList[i].Seq, 
			StorageList[i].ItemSeq, 
			StorageList[i].GroupCode, 
			StorageList[i].ShareFlag, 
			StorageList[i].ProductSeq, 
			W2A(StorageList[i].CashName), 
			StorageList[i].CashPoint, 		
			W2A(StorageList[i].SendAccountID), 
			W2A(StorageList[i].SendMessage), 	
			StorageList[i].ItemType, 
			StorageList[i].RelationType, 
			StorageList[i].PriceSeq,
			StorageList[i].ProductType);

		strMsg += strProDuct;
	}

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnInquireStorageListPageNoGiftMessage(DWORD AccountID, 
													int	  ResultCode, 
													char  StorageType,
													int   NowPage,
													int   TotalPage,
													int   TotalCount,
													int   ListCount, 
													ShopProtocol::STStorageNoGiftMessage StorageList[])
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("보관함 페이지 요청 결과\r\n결과 코드: %d, 요청자: %d, 보관함타입:%c, 현재페이지: %d, 전체페이지수: %d, 전체상품수:%d, 현재페이지상품수: %d\r\n"), 
		ResultCode, AccountID, StorageType ? StorageType : ' ', 
		NowPage, TotalPage, TotalCount, ListCount);

	USES_CONVERSION;

	for(int i = 0 ; i < ListCount ; i++)
	{
		strProDuct.Format(_T("%d, %d, %d, %d, %d, %s, %f, %s, %c, %d, %d, %d\r\n"), 
			StorageList[i].Seq, 
			StorageList[i].ItemSeq, 
			StorageList[i].GroupCode, 
			StorageList[i].ShareFlag, 
			StorageList[i].ProductSeq, 
			W2A(StorageList[i].CashName), 
			StorageList[i].CashPoint, 		
			W2A(StorageList[i].SendAccountID), 
			StorageList[i].ItemType, 
			StorageList[i].RelationType, 
			StorageList[i].PriceSeq,
			StorageList[i].ProductType);

		strMsg += strProDuct;
	}

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnGiftCash(DWORD SenderSeq, 
						 DWORD ReceiverSeq, 
						 long ResultCode, 
						 double GiftSendLimit)
{
	/*
	CString strMsg;

	strMsg.Format(_T("캐시 선물 결과\r\n보낸사람: %d, 받는사람: %d, 결과: %d, 한도초과시 선물가능캐시: %f\r\n"), 
		SenderSeq, ReceiverSeq, ResultCode, GiftSendLimit);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnInquireBuyGiftPossibility(DWORD AccountID, 
										  long ResultCode, 
										  long ItemBuy, 
										  long Present, 
										  double MyPresentCash, 
										  double MyPresentSendLimit)
{
	/*
	CString strMsg;

	strMsg.Format(_T("상품 구매/선물 가능여부 체크\r\n요청자: %d, 결과: %d, 구매 가능 여부: %d, 선물 가능 여부: %d, 한달 동안 선물한 캐시:%f, 한달 동안 선물 캐시 한도:%f\r\n"), 
		AccountID, ResultCode, ItemBuy, Present, MyPresentCash, MyPresentSendLimit);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnInquireEventProductList(DWORD AccountSeq, 
										long ProductDisplaySeq, 
										int PackagesCount, 
										long Packages[])
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("이벤트 아이템 목록 요청 결과\r\n계정 순번: %d, Display 순번: %d, 패키지 개수: %d\r\n"), 
		AccountSeq, ProductDisplaySeq, PackagesCount);

	USES_CONVERSION;

	strMsg += "패키지 목록 \r\n";
	for(int i = 0 ; i < PackagesCount ; i++)
	{
		strProDuct.Format(_T("%d "), 	Packages[i]);

		strMsg += strProDuct;
	}

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnInquireProductLeftCount(long PackageSeq, long LeftCount)
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("전시 상품 잔여 개수 조회 결과\r\n패키지 순번: %d, 잔여수량: %d\r\n"), 
		PackageSeq, LeftCount);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnUpdateVersion(long GameCode, 
							  unsigned short SalesZone, 
							  unsigned short year, 
							  unsigned short yearIdentity)
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("버전 업데이트 정보\r\n게임 코드: %d, 판매 영역: %d, 연도: %d, 연도 Identity:%d\r\n"), 
		GameCode, SalesZone, year, yearIdentity);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnUpdateBannerVersion(long GameCode, 
									unsigned short BannerZone, 
									unsigned short year, 
									unsigned short yearIdentity)
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("배너 버전 업데이트 정보\r\n게임 코드: %d, 배너 영역: %d, 연도: %d, 연도 Identity:%d\r\n"), 
		GameCode, BannerZone, year, yearIdentity);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnUseStorage(DWORD AccountSeq, 
						   long ResultCode, 
						   char InGameProductID[MAX_TYPENAME_LENGTH], 
						   BYTE PropertyCount, 
						   long ProductSeq, 						   
						   long StorageSeq, 
						   long StorageItemSeq, 
						   ShopProtocol::STItemProperty PropertyList[])
{
	/*
	CString strMsg, strProperty;

	strMsg.Format(_T("보관함 사용\r\n요청자: %d, 결과: %d, 아이템코드: %s, 아이템속성 개수: %d, 제품시퀀스: %d, 보관함시퀀스: %d, 보관함아이템시퀀스: %d\r\n"), 
		AccountSeq, ResultCode, InGameProductID, PropertyCount, ProductSeq, StorageSeq, StorageItemSeq);

	for(int i = 0 ; i < PropertyCount ; i++)
	{
		strProperty.Format(_T("속성 시퀀스: %d, 값: %d\r\n"), PropertyList[i].PropertySeq, PropertyList[i].Value);
		strMsg += strProperty;
	}

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

// 보관함 상품 사용하기 롤백
void AgsmBillingWebzen::OnRollbackUseStorage(DWORD AccountSeq, long ResultCode)
{
	/*
	CString strMsg;

	strMsg.Format(_T("보관함 상품 사용하기 롤백\r\n요청자: %d, 결과: %d\r\n"), 
		AccountSeq, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnMileageDeduct(DWORD AccountSeq, long ResultCode)
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("마일리지 소진 결과\r\n계정 순번: %d, 결과: %d\r\n"), 
		AccountSeq, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnMileageSave(DWORD AccountSeq, long ResultCode)
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("마일리지 적립 결과\r\n계정 순번: %d, 결과: %d\r\n"), 
		AccountSeq, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnMileageLiveSaveUp(DWORD AccountSeq, long ResultCode)
{
	/*
	CString strMsg, strProDuct;

	strMsg.Format(_T("실시간 마일리지 적립 결과\r\n계정 순번: %d, 결과: %d\r\n"), 
		AccountSeq, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnThrowStorage(DWORD AccountSeq, long ResultCode)
{
	/*
	CString strMsg;

	strMsg.Format(_T("실시간 마일리지 적립\r\n요청자: %d, 결과: %d\r\n"), 
		AccountSeq, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnItemSerialUpdate(DWORD AccountSeq, long ResultCode)
{
	/*
	CString strMsg;

	strMsg.Format(_T("아이템 시리얼 코드 업데이트\r\n요청자: %d, 결과: %d\r\n"), 
		AccountSeq, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

void AgsmBillingWebzen::OnInquireInGamePointValue(long ResultCode,
										long PointCount,
										ShopProtocol::STPointDetail PointList[])
{
	/*
	CString strMsg, strProperty;

	strMsg.Format(_T("포인트 지급 퍼센트 요청 결과, 포인트 개수: %d\r\n"), PointCount);

	for(int i = 0 ; i < PointCount ; i++)
	{
		strProperty.Format(_T("포인트타입: %d, 포인트이름: %s, 포인트퍼센트: %d\r\n"), 
			PointList[i].PointType, PointList[i].PointTypeName, PointList[i].PointValue);
		strMsg += strProperty;
	}

	WriteLog((LPSTR)(LPCTSTR)strMsg);
	*/
}

#endif //_WEBZEN_BILLING_