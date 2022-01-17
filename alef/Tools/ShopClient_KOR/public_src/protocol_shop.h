#pragma once
#pragma pack(1)

/**************************************************************************************************

작성일: 2010.03.25
작성자: 진혜진

설  명: 샵서버 프로토콜 정의

		조회, 구매, 선물 인터페이스 종류

		 In		  : 국내   - 뮤, 배터리, R2
					글로벌 - 뮤
				    글로벌 - 아크로드 전용
	     In / Out : HanCoin / IJJI.com / GameOn - 헉슬리 국내/북미, 뮤 일본
					GameChu - 뮤 일본

**************************************************************************************************/

#include "ShopDefine.h"

namespace ShopProtocol
{

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// 구조체 ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// 국내 캐시 종류 세부 정보
struct STCashDetail 
{
	STCashDetail()
	{
		ZeroMemory(Name, MAX_TYPENAME_LENGTH * sizeof(WCHAR));
		Value = 0;			
		Type = 0;			
	};
	
	WCHAR	Name[MAX_TYPENAME_LENGTH];	// 캐시유형 이나 포인트유형 이름
	double	Value;						// 캐시나 포인트 값
	char	Type;						// 캐시인지 포인트 인지 구분 (C:캐시, P:포인트 0:없음)
};

// 글로벌 캐시 종류 세부 정보
struct STCashDetail_GB
{
	STCashDetail_GB()
	{
		ZeroMemory(Name, MAX_TYPENAME_LENGTH * sizeof(WCHAR));	
		Value = 0;			
		Type = 0;			
		CashTypeCode = 0;
	};
	
	WCHAR	Name[MAX_TYPENAME_LENGTH];	// 캐시유형 이나 포인트유형 이름
	double	Value;						// 캐시나 포인트 값
	char	Type;						// 캐시인지 포인트 인지 구분 (C:캐시, P:포인트 0:없음)
	long	CashTypeCode;				// 캐시 타입 코드
};

// 포인트 종류 세부 정보
struct STPointDetail
{
	STPointDetail()
	{
		ZeroMemory(PointTypeName, MAX_POINTNAME_LENGTH);
		PointType = PointValue = 0;
	};

	char	PointTypeName[MAX_POINTNAME_LENGTH];
	long	PointType;
	long	PointValue;
};

// 아이템 속성
struct STItemProperty
{
	STItemProperty()
	{
		PropertySeq = 0;
		Value = 0;
	};

	long	PropertySeq;
	int		Value;
};

//보관함 내의 아이템 정보
struct STStorage 
{
	STStorage()
	{
		Seq = ItemSeq = GroupCode = ShareFlag = ProductSeq = PriceSeq = ProductType = 0;
		CashPoint = 0;
		ItemType = 0;
		RelationType = 0;
		ZeroMemory(CashName, MAX_TYPENAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendAccountID, MAX_ACCOUNTID_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendMessage, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
	};

	long	Seq;								// 보관함 순번
	long	ItemSeq;							// 보관함 상품 or 캐시 항목 순번
	long	GroupCode;							// 보관함 그룹코드
	long	ShareFlag;							// ServerType(서버유형) 간 보관함 항목 조회 공유 항목 여부
	long	ProductSeq;							// 단위상품 코드
	WCHAR	CashName[MAX_TYPENAME_LENGTH];		// 캐시 명
	double	CashPoint;							// 웹젠 캐시 포인트
	WCHAR	SendAccountID[MAX_ACCOUNTID_LENGTH];// 발신자 회원 아이디
	WCHAR	SendMessage[MAX_MESSAGE_LENGTH];	// 발신 메시지 - 선물일 경우 존재
	char	ItemType;							// 상품 캐시 구분 (P:상품, C:캐시)
	BYTE	RelationType;						// 보관함 유형 구분 (1:구매, 시리얼, 무료지급 , 2:선물)
	long	PriceSeq;
	long	ProductType;						// 단위 상품 유형 - 406인 경우 게임 내에서 보관함 사용하기 불가
};

//보관함 내의 아이템 정보
struct STStorageNoGiftMessage
{
	STStorageNoGiftMessage()
	{
		Seq = ItemSeq = GroupCode = ShareFlag = ProductSeq = PriceSeq = ProductType = 0;
		CashPoint = 0;
		ItemType = 0;
		RelationType = 0;
		ZeroMemory(CashName, MAX_TYPENAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendAccountID, MAX_ACCOUNTID_LENGTH * sizeof(WCHAR));
	};

	long	Seq;								// 보관함 순번
	long	ItemSeq;							// 보관함 상품 or 캐시 항목 순번
	long	GroupCode;							// 보관함 그룹코드
	long	ShareFlag;							// ServerType(서버유형) 간 보관함 항목 조회 공유 항목 여부
	long	ProductSeq;							// 단위상품 코드
	WCHAR	CashName[MAX_TYPENAME_LENGTH];		// 캐시 명
	double	CashPoint;							// 웹젠 캐시 포인트
	WCHAR	SendAccountID[MAX_ACCOUNTID_LENGTH];// 발신자 회원 아이디
	char	ItemType;							// 상품 캐시 구분 (P:상품, C:캐시)
	BYTE	RelationType;						// 보관함 유형 구분 (1:구매, 시리얼, 무료지급 , 2:선물)
	long	PriceSeq;
	long	ProductType;						// 단위 상품 유형 - 406인 경우 게임 내에서 보관함 사용하기 불가
};

// 결제 서비스 타입 - 자체서비스, 한코인, ijji.com ...
enum PaymentType
{
	Inbound				= 0,
	Outbound_HanCoin	= 1,
	Outbound_Ijji		= 2,
	Outbound_GameOn		= 3,
	Outbound_GameChu	= 4,
};


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// 메시지 베이스 ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// 메시지 베이스
template <class _subcls>
class MSG_REQUEST_BASE
{
public:
	MSG_REQUEST_BASE() {}
	MSG_REQUEST_BASE(DWORD dwID) 
		: dwProtocolID(dwID),
		  GameCode(0),
		  dwPacketSize(sizeof( _subcls ))
	{}

	long	GetGameCode()	{ return GameCode; }
	
	DWORD 	dwPacketSize; 					// 패킷의 전체 사이즈
	DWORD	dwProtocolID;					// 패킷 아이디
	long	GameCode;						// 게임 코드
};

// 리턴 메시지 베이스
template <class _subcls>
class MSG_RESPONSE_BASE : public MSG_REQUEST_BASE<_subcls>	
{
public:
	MSG_RESPONSE_BASE(DWORD dwProtocol) 
		: MSG_REQUEST_BASE<_subcls>(dwProtocol)
	{
		ResultCode = 0;
	}
	
public:
	long ResultCode;
};


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// 아크로드 - 글로벌 ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// 캐시 조회 /////////////////////////////////

// C->S : 아크로드 - 글로벌
class MSG_CLIENT_INQUIRE_CASH_AR : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_AR>
{
public:
	MSG_CLIENT_INQUIRE_CASH_AR(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_AR>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		ViewType = 0;
		AccountSeq = 0;
		SumOnly = false;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
	}

public:
	BYTE	ViewType;							//조회 유형 (0:전체(캐시+포인트), 1:캐시(일반+이벤트), 2:포인트(보너스+일반), 3:보너스포인트, 4:게임포인트, 5:캐시(일반))
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;							//계정순번
	bool	SumOnly;							//총 합계만 조회 여부 (true: 유형별 종합 값만, false: 유형별 종합 + 캐시 유형별 결과(STDetail 값 출력))
};

// S->C : 아크로드 - 글로벌
class MSG_SERVER_INQUIRE_CASH_AR : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_AR>
{
public:
	MSG_SERVER_INQUIRE_CASH_AR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_AR>(PROTOCOL_SERVER_INQUIRE_CASH)	  
	{
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		sum[0] = sum[1] = 0;
		ListCount = 0;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
	}

public:
	long	PackHeader;
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;							// 계정순번
	double	sum[2];								// 유형별 종합 정보 (0: 캐시 총합, 1: 포인트 총합)
	int		ListCount;
};	

///////////////////////////////// 상품 구매 /////////////////////////////////

// C->S : 아크로드 - 글로벌
class MSG_CLIENT_BUYPRODUCT_AR : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_AR>
{
public:
	MSG_CLIENT_BUYPRODUCT_AR(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_AR>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		ProductSeq = Class = Level = ServerIndex = SalesZone = CashTypeCode = 0;
		DeductPrice = 0;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
		ZeroMemory(CharName, MAX_CHARNAME_LENGTH * sizeof(WCHAR));	
		ZeroMemory(ProductName, MAX_PRODUCTNAME_LENGTH * sizeof(WCHAR));	
	}

public:
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;								// 계정 순번
	int		ProductSeq;								// 상품 순번
	WCHAR	ProductName[MAX_PRODUCTNAME_LENGTH];	// 상품 이름
	int		InGamePurchaseSeq;						// 게임 내 상품 구매번호
	int		Class;									// 클래스 코드
	int		Level;									// 레벨 코드
	WCHAR	CharName[MAX_CHARNAME_LENGTH];			// 캐릭터 명
	int		ServerIndex;							// 서버 인덱스
	int		SalesZone;								// 판매 영역
	double	DeductPrice;							// 차감 캐시 가격
	char	DeductType;								// 차감 유형
	int		CashTypeCode;							// 캐시 유형 코드
};

// S->C : 아크로드 - 글로벌
class MSG_SERVER_BUYPRODUCT_AR : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_AR>
{
public:
	MSG_SERVER_BUYPRODUCT_AR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_AR>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		DeductCashSeq = 0;
		InGamePurchaseSeq = 0;
		ZeroMemory(AccountID, MAX_ACCOUNT_LENGTH);
	}

public:
	char	AccountID[MAX_ACCOUNT_LENGTH];
	DWORD	AccountSeq;							// 계정순번
	long	DeductCashSeq;						// 웹 상품 구매에 대한 차감 번호 (ResultCode != 이면 -1 값 전달)	
	int		InGamePurchaseSeq;					// 게임 내 상품 구매번호
};

///////////////////////////////// 상품 선물 /////////////////////////////////

// C->S : 아크로드 - 글로벌
class MSG_CLIENT_GIFTPRODUCT_AR : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_AR>
{
public:
	MSG_CLIENT_GIFTPRODUCT_AR(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_AR>(PROTOCOL_CLIENT_GIFTPRODUCT)		
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = ProductSeq = InGamePurchaseSeq = SalesZone = 0;
		SendClass = SendLevel = SendServerIndex = RevClass = RevLevel = RevServerIndex = CashTypeCode = 0;
		DeductPrice = 0;
		DeductType = 0;
		ZeroMemory(SenderID, MAX_ACCOUNT_LENGTH); 
		ZeroMemory(ReceiverID, MAX_ACCOUNT_LENGTH);
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendCharName, MAX_CHARNAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(RevCharName, MAX_CHARNAME_LENGTH * sizeof(WCHAR));
		ZeroMemory(ProductName, MAX_PRODUCTNAME_LENGTH * sizeof(WCHAR));
	}

public:
	char	SenderID[MAX_ACCOUNT_LENGTH];
	DWORD	SenderSeq;								// 발신자 순번
	char	ReceiverID[MAX_ACCOUNT_LENGTH];
	DWORD	ReceiverSeq;							// 수신자 순번
	WCHAR	Message[MAX_MESSAGE_LENGTH];			// 메시지
	DWORD	ProductSeq;								// 상품 순번
	WCHAR	ProductName[MAX_PRODUCTNAME_LENGTH];	// 상품 명
	DWORD	InGamePurchaseSeq;						// 게임 내 상품 구매번호
	int		SendClass;								// 발신자 클래스 코드
	int		SendLevel;								// 발신자 레벨 코드
	WCHAR	SendCharName[MAX_CHARNAME_LENGTH];		// 발신자 캐릭터명
	int		SendServerIndex;						// 보낸 사람 서버 인덱스
	int		RevClass;								// 수신자 클래스 코드
	int		RevLevel;								// 수신자 레벨 코드
	WCHAR	RevCharName[MAX_CHARNAME_LENGTH];		// 수신자 캐릭터명
	int		RevServerIndex;							// 받는 사람 서버 인덱스
	DWORD	SalesZone;								// 판매 영역
	double	DeductPrice;							// 차감 캐시 가격
	char	DeductType;								// 차감 유형
	int		CashTypeCode;							// 캐시 유형 코드
};

// S->C : 아크로드 - 글로벌
class MSG_SERVER_GIFTPRODUCT_AR : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_AR>
{
public:
	MSG_SERVER_GIFTPRODUCT_AR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_AR>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = 0;
		DeductCashSeq = 0;
		ZeroMemory(SenderID, MAX_ACCOUNT_LENGTH); 
		ZeroMemory(ReceiverID, MAX_ACCOUNT_LENGTH);
	}

public:
	char	SenderID[MAX_ACCOUNT_LENGTH];
	char	ReceiverID[MAX_ACCOUNT_LENGTH];
	DWORD	SenderSeq;							// 계정순번
	DWORD	ReceiverSeq;						// 계정순번
	long	DeductCashSeq;						// 웹 상품 구매에 대한 차감 번호 (ResultCode != 이면 -1 값 전달)
};


//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// 국내 - 뮤, 배터리, R2 ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// 캐시 조회 /////////////////////////////////

// C->S : 국내 - 뮤, 배터리, R2
class MSG_CLIENT_INQUIRE_CASH_KR : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_KR>	
{
public:
	MSG_CLIENT_INQUIRE_CASH_KR(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_KR>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{ 
		GameCode = nGameCode; 
		ViewType = 0;
		AccountSeq = 0;
		SumOnly = false;
		MileageSection = 0;
	}

public:
	BYTE	ViewType;					// 조회 유형
	DWORD	AccountSeq;					// 계정순번	
	bool	SumOnly;					// 총 합계만 조회 여부 (	true  : 유형별 종합 값만, false : 유형별 종합 + 캐시 유형별 결과(STDetail 값 출력))
	long	MileageSection;				// 마일리지 유형 (438:고블린, 439:QT)
};

// S->C : 국내 - 뮤, 배터리, R2
class MSG_SERVER_INQUIRE_CASH_KR : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_KR>
{
public:
	MSG_SERVER_INQUIRE_CASH_KR(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_KR>(PROTOCOL_SERVER_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = 0;
		sum[0] = 0;					// 캐시(일반, 이벤트) 총합
		sum[1] = 0;					// 포인트(보너스, 게임) 총합
		sum[2] = 0;					// 마일리지(고블린, QT) 총합
	}

public:
	DWORD	AccountSeq;				// 계정순번
	long	PackHeader;	
	int		ListCount;
	double	sum[3];					// 유형별 종합 정보 (0: 캐시 총합, 1: 포인트 총합, 2: 마일리지 총합)	
};	


///////////////////////////////// 상품 구매 /////////////////////////////////

// C->S : 국내 - 뮤, 배터리, R2
class MSG_CLIENT_BUYPRODUCT_KR : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_KR>
{
public:
	MSG_CLIENT_BUYPRODUCT_KR(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_KR>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		PackageSeq = PriceSeq = SalesZone = DisplaySeq = Class = Level = ServerIndex = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	AccountSeq;							// 계정순번	
	long	PackageSeq;							// 포장 상품 순번
	long	PriceSeq;							// 가격 순번	
	long	SalesZone;							// 판매영역
	long	DisplaySeq;							// 전시정보 순번
	long	Class;								// 클래스
	long	Level;								// 레벨
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// 캐릭터 명
	WCHAR	Rank[MAX_RANK_LENGTH];				// 랭크
	long	ServerIndex;						// 서버 인덱스
};

// S->C : 국내 - 뮤, 배터리, R2
class MSG_SERVER_BUYPRODUCT_KR : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_KR>
{
public:
	MSG_SERVER_BUYPRODUCT_KR(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_KR>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		LeftCount = 0;
	}

public:
	DWORD	AccountSeq;					// 계정순번
	long	LeftCount;					// 상품 판매 가능 잔여수량 (-1: 무제한) 상품 개수 정보를 갱신해야한다.
};

///////////////////////////////// 상품 선물 /////////////////////////////////

// C->S : 국내 - 뮤, 배터리, R2
class MSG_CLIENT_GIFTPRODUCT_KR : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_KR>
{
public:
	MSG_CLIENT_GIFTPRODUCT_KR(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_KR>(PROTOCOL_CLIENT_GIFTPRODUCT)
	{
		GameCode = nGameCode; 
		SenderSeq = ReceiverSeq = 0;
		SenderServerIndex = ReceiverServerIndex = PackageSeq = PriceSeq = SalesZone = DisplaySeq = 0;
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SenderCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	SenderSeq;									// 발신자 순번
	long	SenderServerIndex;							// 발신자 서버 번호
	WCHAR	SenderCharName[MAX_CHARACTERID_LENGTH];		// 발신자 캐릭터 명
	DWORD	ReceiverSeq;								// 수신자 순번
	long	ReceiverServerIndex;						// 수신자 서버 번호
	WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH];	// 수신자 캐릭터 명
	WCHAR	Message[MAX_MESSAGE_LENGTH];				// 메시지	
	long	PackageSeq;									// 포장 상품 순번
	long	PriceSeq;									// 가격 순번	
	long	SalesZone;									// 판매영역
	long	DisplaySeq;									// 전시정보 순번
};

// S->C : 국내 - 뮤, 배터리, R2
class MSG_SERVER_GIFTPRODUCT_KR : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_KR>
{
public:
	MSG_SERVER_GIFTPRODUCT_KR(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_KR>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = 0;
		LeftProductCount = 0;
		LimitedCash = 0;		
	}

public:
	DWORD	SenderSeq;					// 계정 순번
	DWORD	ReceiverSeq;				// 계정 순번
	double	LimitedCash;				// 선물 가능 캐시
	long	LeftProductCount;
};


//////////////////////////////////////////////////////////////////////////////////////
////////////////////// HanCoin, IJJI.com - 헉슬리 국내/북미 ///////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// 캐시 조회 /////////////////////////////////

// C->S : HanCoin, IJJI.com - 헉슬리 국내/북미
class MSG_CLIENT_INQUIRE_CASH_OB : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_OB>	
{
public:
	MSG_CLIENT_INQUIRE_CASH_OB(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_OB>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		SumOnly = true;
		PaymentType = Inbound;
		MileageSection = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
	}

public:
	PaymentType	PaymentType;						// 결제 타입
	DWORD		AccountSeq;							// 계정순번	
	char		AccountID[MAX_ACCOUNTID_LENGTH];	// 게임 계정
	BYTE		ViewType;							// 7:마일리지로 고정
	long		MileageSection;						// 마일리지 유형 (438:고블린, 439:QT)
	bool		SumOnly;							// true로 고정
};

// S->C : HanCoin, IJJI.com - 헉슬리 국내/북미
class MSG_SERVER_INQUIRE_CASH_OB : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_OB>
{
public:
	MSG_SERVER_INQUIRE_CASH_OB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_OB>(PROTOCOL_SERVER_INQUIRE_CASH)
	{	
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = 0;
		sum[0] = 0;								// 캐시(일반, 이벤트) 총합
		sum[1] = 0;								// 포인트(보너스, 게임) 총합
		sum[2] = 0;								// 마일리지(고블린, QT) 총합
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;							// 계정순번
	char	AccountID[MAX_ACCOUNTID_LENGTH];	// 게임 계정
	long	PackHeader;	
	int		ListCount;	
	double	sum[3];								// 유형별 종합 정보 (0: 캐시 총합, 1: 포인트 총합, 2: 마일리지 총합)	
	long	OutBoundResultCode;					// 외부 모듈 결과 코드
};	


///////////////////////////////// 상품 구매 /////////////////////////////////

// C->S : HanCoin, IJJI.com - 헉슬리 국내/북미
class MSG_CLIENT_BUYPRODUCT_OB : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_OB>
{
public:
	MSG_CLIENT_BUYPRODUCT_OB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_OB>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = dwIPAddress = 0;
		PaymentType = Inbound;
		PackageSeq = PriceSeq = SalesZone = ProductDisplaySeq = Class = Level = ServerIndex = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		ZeroMemory(RefKey, MAX_TYPENAME_LENGTH);
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
		DeductMileageFlag = false;
	}

public:
	PaymentType	PaymentType;						// 결제 타입
	DWORD		AccountSeq;							// 계정 순번	
	char		AccountID[MAX_ACCOUNTID_LENGTH];	// 게임 계정
	WCHAR		CharName[MAX_CHARACTERID_LENGTH];	// 캐릭터 명
	long		ServerIndex;						// 서버 인덱스
	DWORD		dwIPAddress;						// 아이피 정보
	long		PackageSeq;							// 포장 상품 순번
	long		PriceSeq;							// 가격 순번	
	long		SalesZone;							// 판매 영역	
	long		ProductDisplaySeq;
	long		Class;								// 클래스
	long		Level;								// 레벨
	WCHAR		Rank[MAX_RANK_LENGTH];				// 랭크
	char		RefKey[MAX_TYPENAME_LENGTH];		// 게임 시스템 발급 주문번호
	bool		DeductMileageFlag;					// 마일리지로 차감하는 상품인지 여부 (false : 일반, true : 마일리지 차감)
};

// S->C : HanCoin, IJJI.com - 헉슬리 국내/북미
class MSG_SERVER_BUYPRODUCT_OB : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_OB>
{
public:
	MSG_SERVER_BUYPRODUCT_OB(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_OB>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		LeftCount = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;						// 계정순번
	char	AccountID[MAX_ACCOUNTID_LENGTH];// 게임 계정
	long	LeftCount;						// 상품 판매 가능 잔여수량 (-1: 무제한) 상품 개수 정보를 갱신해야한다.
	long	OutBoundResultCode;				// 외부 모듈 결과 코드
	
	/*	결과 코드
		-1 ~ -100   : 한코인 모듈 에러 코드
		-996 ~ -999 : DB 처리 에러

		0 : 성공
		1 : 보유 캐시 부족
		2 : (나의)상품 구매 불가능
		3 : 나의선물하기한도초과
		4 : 잔여수량부족
		5 : 판매기간종료
		6 : 판매종료(상품정보가없는경우)
		7 : 상품선물불가능
		8 : 이벤트상품선물불가
		9 : 이벤트상품선물가능횟수초과

		***** 이하 한코인 모듈 결과 *****
		101	: 유효하지 않은 파라미터
		102	: 유효하지 않은 사용자 정보
		103	: 유효하지 않은 PG(Payment Gateway)
		104	: 유효하지 않은 아이템
		105	: 결제할 금액이 0보다 크지 않음
		106	: 계좌가 존재하지 않음
		107	: 잔액 부족
		108	: 잔액 수정 실패
		199	: 한코인 DB 에러 
	*/
};


///////////////////////////////// 상품 선물 /////////////////////////////////

// C->S : HanCoin, IJJI.com - 헉슬리 국내/북미

class MSG_CLIENT_GIFTPRODUCT_OB : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_OB>
{
public:
	MSG_CLIENT_GIFTPRODUCT_OB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_OB>(PROTOCOL_CLIENT_GIFTPRODUCT)
	{
		GameCode = nGameCode; 
		AccountSeq = ReceiverAccountSeq = 0;
		PaymentType = Inbound;
		ServerIndex = dwIPAddress = ReceiverServerIndex = PackageSeq = PriceSeq = SalesZone = ProductDisplaySeq = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		ZeroMemory(ReceiverAccountID, MAX_ACCOUNTID_LENGTH);
		ZeroMemory(RefKey, MAX_TYPENAME_LENGTH);
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		DeductMileageFlag = false;
	}

public:
	PaymentType	PaymentType;								// 결제 타입
	DWORD		AccountSeq;									// 발신자 순번
	char		AccountID[MAX_ACCOUNTID_LENGTH];			// 게임 계정
	WCHAR		CharName[MAX_CHARACTERID_LENGTH];			// 발신자 캐릭터 명
	long		ServerIndex;								// 발신자 서버 번호
	DWORD		ReceiverAccountSeq;							// 수신자 순번
	char		ReceiverAccountID[MAX_ACCOUNTID_LENGTH];	// 수신자 계정 명
	WCHAR		ReceiverCharName[MAX_CHARACTERID_LENGTH];	// 수신자 캐릭터 명
	long		ReceiverServerIndex;						// 수신자 서버 번호
	DWORD		dwIPAddress;								// 아이피정보
	long		PackageSeq;									// 포장 상품 순번
	long		PriceSeq;									// 가격 순번	
	long		SalesZone;									// 판매영역	
	long		ProductDisplaySeq;
	WCHAR		Message[MAX_MESSAGE_LENGTH];				// 메시지	
	char		RefKey[MAX_TYPENAME_LENGTH];				// 게임 시스템 발급 주문번호
	bool		DeductMileageFlag;							// 마일리지로 차감하는 상품인지 여부 (false : 일반, true : 마일리지 차감)
};
// S->C : HanCoin, IJJI.com - 헉슬리 국내/북미

class MSG_SERVER_GIFTPRODUCT_OB : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_OB>
{
public:
	MSG_SERVER_GIFTPRODUCT_OB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_OB>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = ReceiverAccountSeq = 0;
		LeftProductCount = 0;
		LimitedCash = 0;
		OutBoundResultCode = 0;
		ZeroMemory(AccountID, MAX_ACCOUNTID_LENGTH);
		ZeroMemory(ReceiverAccountID, MAX_ACCOUNTID_LENGTH);
	}

public:
	DWORD	AccountSeq;									// 계정순번
	char	AccountID[MAX_ACCOUNTID_LENGTH];			// 게임 계정
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];			// 발신자 캐릭터 명
	DWORD	ReceiverAccountSeq;							// 계정순번
	char	ReceiverAccountID[MAX_ACCOUNTID_LENGTH];	// 수신자 계정 명
	WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH];	// 수신자 캐릭터 명
	WCHAR	Message[MAX_MESSAGE_LENGTH];				// 메시지	
	double	LimitedCash;								// 선물 가능 캐시
	long	LeftProductCount;
	long	OutBoundResultCode;							// 외부 모듈 결과 코드
};


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// GameChu - 뮤 ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// 캐시 조회 /////////////////////////////////

// C->S : GameChu - 뮤
class MSG_CLIENT_INQUIRE_CASH_GAMECHU : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_GAMECHU>	
{
public:
	MSG_CLIENT_INQUIRE_CASH_GAMECHU(long nGameCode) 
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_GAMECHU>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		SumOnly = true;
		PaymentType = Inbound;
		MileageSection = 0;
		USN = 0;
	}

public:
	PaymentType	PaymentType;						// 결제 타입
	DWORD		AccountSeq;							// 계정 순번
	BYTE		ViewType;							// 7:마일리지로 고정
	long		MileageSection;						// 마일리지 유형
	bool		SumOnly;							// true로 고정
	int			USN;								// GameChu 계정 번호
};

// S->C : GameChu - 뮤
class MSG_SERVER_INQUIRE_CASH_GAMECHU : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_GAMECHU>
{
public:
	MSG_SERVER_INQUIRE_CASH_GAMECHU(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_GAMECHU>(PROTOCOL_SERVER_INQUIRE_CASH)
	{	
		GameCode = nGameCode;
		AccountSeq = 0;
		CashSum = MileageSum = 0;
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;							// 계정순번
	double	CashSum;
	double	MileageSum;
	long	OutBoundResultCode;					// 외부 모듈 결과 코드
};	


///////////////////////////////// 상품 구매 /////////////////////////////////

// C->S : GameChu - 뮤
class MSG_CLIENT_BUYPRODUCT_GAMECHU : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_GAMECHU>
{
public:
	MSG_CLIENT_BUYPRODUCT_GAMECHU(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_GAMECHU>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = dwIPAddress = 0;
		PaymentType = Inbound;
		PackageSeq = PriceSeq = SalesZone = ProductDisplaySeq = Class = Level = ServerIndex = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
		DeductMileageFlag = false;
		USN = 0;
	}

public:
	PaymentType	PaymentType;						// 결제 타입
	DWORD		AccountSeq;							// 계정 순번	
	WCHAR		CharName[MAX_CHARACTERID_LENGTH];	// 캐릭터 명
	long		ServerIndex;						// 서버 인덱스
	DWORD		dwIPAddress;						// 아이피 정보
	long		PackageSeq;							// 포장 상품 순번
	long		PriceSeq;							// 가격 순번	
	long		SalesZone;							// 판매 영역	
	long		ProductDisplaySeq;
	long		Class;								// 클래스
	long		Level;								// 레벨
	WCHAR		Rank[MAX_RANK_LENGTH];				// 랭크
	bool		DeductMileageFlag;					// 마일리지로 차감하는 상품인지 여부 (false : 일반, true : 마일리지 차감)
	int			USN;								// GameChu 계정 번호
};

// S->C : GameChu - 뮤
class MSG_SERVER_BUYPRODUCT_GAMECHU : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_GAMECHU>
{
public:
	MSG_SERVER_BUYPRODUCT_GAMECHU(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_GAMECHU>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		LeftProductCount = 0;
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;						// 계정순번
	long	LeftProductCount;				// 상품 판매 가능 잔여수량 (-1: 무제한) 상품 개수 정보를 갱신해야한다.
	long	OutBoundResultCode;				// 외부 모듈 결과 코드
};


///////////////////////////////// 상품 선물 /////////////////////////////////

// C->S : GameChu - 뮤

class MSG_CLIENT_GIFTPRODUCT_GAMECHU : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_GAMECHU>
{
public:
	MSG_CLIENT_GIFTPRODUCT_GAMECHU(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_GAMECHU>(PROTOCOL_CLIENT_GIFTPRODUCT)
	{
		GameCode = nGameCode; 
		AccountSeq = ReceiverAccountSeq = 0;
		PaymentType = Inbound;
		ServerIndex = dwIPAddress = ReceiverServerIndex = PackageSeq = PriceSeq = SalesZone = ProductDisplaySeq = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		DeductMileageFlag = false;
		USN = 0;
	}

public:
	PaymentType	PaymentType;								// 결제 타입
	DWORD		AccountSeq;									// 발신자 순번
	WCHAR		CharName[MAX_CHARACTERID_LENGTH];			// 발신자 캐릭터 명
	long		ServerIndex;								// 발신자 서버 번호
	DWORD		ReceiverAccountSeq;							// 수신자 순번
	WCHAR		ReceiverCharName[MAX_CHARACTERID_LENGTH];	// 수신자 캐릭터 명
	long		ReceiverServerIndex;						// 수신자 서버 번호
	DWORD		dwIPAddress;								// 아이피정보
	long		PackageSeq;									// 포장 상품 순번
	long		PriceSeq;									// 가격 순번	
	long		SalesZone;									// 판매영역	
	long		ProductDisplaySeq;
	WCHAR		Message[MAX_MESSAGE_LENGTH];				// 메시지	
	bool		DeductMileageFlag;							// 마일리지로 차감하는 상품인지 여부 (false : 일반, true : 마일리지 차감)
	int			USN;										// GameChu 계정 번호
};
// S->C : GameChu - 뮤

class MSG_SERVER_GIFTPRODUCT_GAMECHU : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_GAMECHU>
{
public:
	MSG_SERVER_GIFTPRODUCT_GAMECHU(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_GAMECHU>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = ReceiverAccountSeq = 0;
		LeftProductCount = 0;
		OutBoundResultCode = 0;
	}

public:
	DWORD	AccountSeq;									// 계정순번
	DWORD	ReceiverAccountSeq;							// 계정순번
	long	LeftProductCount;
	long	OutBoundResultCode;							// 외부 모듈 결과 코드
};


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// 글로벌 - 뮤 ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// 캐시 조회 /////////////////////////////////

// C->S : 글로벌- 뮤
class MSG_CLIENT_INQUIRE_CASH_GB : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_GB>
{
public:
	MSG_CLIENT_INQUIRE_CASH_GB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_CASH_GB>(PROTOCOL_CLIENT_INQUIRE_CASH)
	{ 
		GameCode = nGameCode; 
		ViewType = 0;
		AccountSeq = 0;
		SumOnly = false;
		MileageSection = 0;
	}

public:
	BYTE	ViewType;					// 조회 유형 (	0:전체(캐시+포인트), 
										//				1:캐시(일반+이벤트), 
										//				2:포인트(보너스+게임), 
										//				3:보너스포인트, 
										//				4:게임포인트, 
										//				5:캐시(일반), 
										//				6:캐시(이벤트),
										//				7:마일리지)
	DWORD	AccountSeq;					// 계정순번	
	bool	SumOnly;					// 총 합계만 조회 여부 (	true  : 유형별 종합 값만, 
										//							false : 유형별 종합 + 캐시 유형별 결과(STDetail 값 출력))
	long	MileageSection;				// 마일리지 유형 (438:고블린, 439:QT)
};

// S->C : 글로벌- 뮤
class MSG_SERVER_INQUIRE_CASH_GB : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_GB>
{
public:
	MSG_SERVER_INQUIRE_CASH_GB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_CASH_GB>(PROTOCOL_SERVER_INQUIRE_CASH)
	{
		GameCode = nGameCode;
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = 0;
		sum[0] = 0;					// 캐시(일반, 이벤트) 총합
		sum[1] = 0;					// 포인트(보너스, 게임) 총합
		sum[2] = 0;					// 마일리지(고블린, QT) 총합
	}

public:
	DWORD	AccountSeq;				// 계정순번
	long	PackHeader;	
	long	ListCount;	
	double	sum[3];					// 유형별 종합 정보 (0: 캐시 총합, 1: 포인트 총합, 2: 마일리지 총합)	
};

///////////////////////////////// 상품 구매 /////////////////////////////////

// C->S : 글로벌- 뮤
class MSG_CLIENT_BUYPRODUCT_GB : public MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_GB>
{
public:
	MSG_CLIENT_BUYPRODUCT_GB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_BUYPRODUCT_GB>(PROTOCOL_CLIENT_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		PackageSeq = PriceSeq = SalesZone = DisplaySeq = Class = Level = ServerIndex = CashTypeCode = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	AccountSeq;							// 계정순번	
	long	PackageSeq;							// 포장 상품 순번
	long	PriceSeq;							// 가격 순번	
	long	SalesZone;							// 판매영역
	long	DisplaySeq;							// 전시정보 순번
	long	Class;								// 클래스
	long	Level;								// 레벨
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// 캐릭터 명
	WCHAR	Rank[MAX_RANK_LENGTH];				// 랭크
	long	ServerIndex;						// 서버 인덱스
	long	CashTypeCode;						// 캐시 유형 코드
};

// S->C : 글로벌- 뮤
class MSG_SERVER_BUYPRODUCT_GB : public MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_GB>
{
public:
	MSG_SERVER_BUYPRODUCT_GB(long nGameCode)
		: MSG_RESPONSE_BASE<MSG_SERVER_BUYPRODUCT_GB>(PROTOCOL_SERVER_BUYPRODUCT)
	{
		GameCode = nGameCode;
		AccountSeq = 0;
		LeftProductCount = 0;
	}

public:
	DWORD	AccountSeq;					// 계정순번
	long	LeftProductCount;			// 상품 판매 가능 잔여수량 (-1: 무제한) 상품 개수 정보를 갱신해야한다.
	
	/*	결과 코드 
		-2 : DB 처리 실패(정상적으로 수행하지 못함)
		 0 : 성공
		 1 : 보유캐시부족
		 2 : 나의 상품 구매하기 불가능
		 3 : 잔여수량부족
		 4 : 판매기간종료
		 5 : 판매종료
		 6 : 구매 불가능
		 7 : 이벤트상품 구매 불가
		 8 : 이벤트 상품 구매 가능 횟수 초과
		-1 : 오류 발생
	*/
};


///////////////////////////////// 상품 선물 /////////////////////////////////

// C->S : 글로벌- 뮤
class MSG_CLIENT_GIFTPRODUCT_GB : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_GB>
{
public:
	MSG_CLIENT_GIFTPRODUCT_GB(long nGameCode)
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTPRODUCT_GB>(PROTOCOL_CLIENT_GIFTPRODUCT)
	{
		GameCode = nGameCode; 
		SenderSeq = ReceiverSeq = 0;
		SenderServerIndex = ReceiverServerIndex = PackageSeq = PriceSeq = SalesZone = DisplaySeq = CashTypeCode = 0;
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SenderCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	SenderSeq;									// 발신자 순번
	long	SenderServerIndex;							// 발신자 서버 번호
	WCHAR	SenderCharName[MAX_CHARACTERID_LENGTH];		// 발신자 캐릭터 명
	DWORD	ReceiverSeq;								// 수신자 순번
	long	ReceiverServerIndex;						// 수신자 서버 번호
	WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH];	// 수신자 캐릭터 명
	WCHAR	Message[MAX_MESSAGE_LENGTH];				// 메시지	
	long	PackageSeq;									// 포장 상품 순번
	long	PriceSeq;									// 가격 순번	
	long	SalesZone;									// 판매영역
	long	DisplaySeq;									// 전시정보 순번
	long	CashTypeCode;								// 캐시 유형 코드
};

// S->C : 글로벌- 뮤
class MSG_SERVER_GIFTPRODUCT_GB : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_GB>
{
public:
	MSG_SERVER_GIFTPRODUCT_GB(long nGameCode) 
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTPRODUCT_GB>(PROTOCOL_SERVER_GIFTPRODUCT)
	{
		GameCode = nGameCode;
		SenderSeq = ReceiverSeq = 0;
		LeftProductCount = 0;
		GiftSendLimit = 0;		
	}

public:
	DWORD	SenderSeq;					// 계정 순번
	DWORD	ReceiverSeq;				// 계정 순번
	long	LeftProductCount;			// 선물가능 잔여 수량
	double	GiftSendLimit;				// 선물하기 한도 초과시 선물가능 캐시
};


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// 공통 ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////// 캐시 선물 /////////////////////////////////

// C->S : 캐시 선물
class MSG_CLIENT_GIFTCASH : public MSG_REQUEST_BASE<MSG_CLIENT_GIFTCASH>
{
public:
	MSG_CLIENT_GIFTCASH()
		: MSG_REQUEST_BASE<MSG_CLIENT_GIFTCASH>(PROTOCOL_CLIENT_GIFTCASH)
	{
		SenderAccountSeq = ReceiverAccountSeq = 0;
		SendServerIndex = ReceiverServerIndex = 0;
		CashValue = 0;
		ZeroMemory(Message, MAX_MESSAGE_LENGTH * sizeof(WCHAR));
		ZeroMemory(SendCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(ReceiverCharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
	}

public:
	DWORD	SenderAccountSeq;							// 보내는 계정 순번
	WCHAR	SendCharName[MAX_CHARACTERID_LENGTH];		// 발신자 캐릭터 명
	long	SendServerIndex;							// 발신자 서버 번호
	DWORD	ReceiverAccountSeq;							// 받는 계정 순번
	WCHAR	ReceiverCharName[MAX_CHARACTERID_LENGTH];	// 수신자 캐릭터 명
	long	ReceiverServerIndex;						// 수신자 서버 번호
	WCHAR	Message[MAX_MESSAGE_LENGTH];				// 선물 메시지
	double	CashValue;									// 선물 캐시 값
};

// S->C : 캐시 선물
class MSG_SERVER_GIFTCASH : public MSG_RESPONSE_BASE<MSG_SERVER_GIFTCASH>
{
public:
	MSG_SERVER_GIFTCASH()
		: MSG_RESPONSE_BASE<MSG_SERVER_GIFTCASH>(PROTOCOL_SERVER_GIFTCASH)
	{
		SenderSeq = ReceiverSeq = 0;
		GiftSendLimit = 0;
	}

public:
	DWORD	SenderSeq;					// 계정 순번
	DWORD	ReceiverSeq;				// 계정 순번
	double	GiftSendLimit;				// 보관함 목록 개수	
};


///////////////////////////////// 상품 구매/선물 가능여부 체크 /////////////////////////////////

// C->S : 상품 구매/선물 가능여부 조회
class MSG_CLIENT_PRODUCTBUYGIFTCHECK : public MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>
{
public:
	MSG_CLIENT_PRODUCTBUYGIFTCHECK()
		: MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTBUYGIFTCHECK>(PROTOCOL_CLIENT_PRODUCTBUYGIFTCHECK)
	{
		AccountSeq = 0;
	}

public:
	DWORD	AccountSeq;					// 계정 순번
};

// S->C : 상품 구매/선물 가능여부 조회
class MSG_SERVER_PRODUCTBUYGIFTCHECK : public MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>
{
public:
	MSG_SERVER_PRODUCTBUYGIFTCHECK() 
		: MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTBUYGIFTCHECK>(PROTOCOL_SERVER_PRODUCTBUYGIFTCHECK)
	{
		AccountSeq = 0;
		ItemBuy = Present = 0;
		MyPresentCash = MyPresentSendLimit = 0;
	}

public:
	DWORD	AccountSeq;					// 계정순번
	long	ItemBuy;					// 상품 구매 가능 여부 결과 (0: 불가, 1: 가능)
	long	Present;					// 상품/캐시 선물 가능 여부 결과 (1: 가능, 2:선물하기 불가, 3: 선물한도 초과)
	double	MyPresentCash;				// 한달 동안 내가 선물한 캐시 합계
	double	MyPresentSendLimit;			// 한달 동안 선물하기 캐시 한도

	/*	처리결과
		 0 : 불가
		 1 : 가능
		-1 : DB 처리 실패
	*/
};


///////////////////////////////// 이벤트 상품 목록 /////////////////////////////////

// C->S : 이벤트 상품 목록 조회
class MSG_CLIENT_PRODUCTEVENTLIST : public MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTEVENTLIST>
{
public:
	MSG_CLIENT_PRODUCTEVENTLIST()
		: MSG_REQUEST_BASE<MSG_CLIENT_PRODUCTEVENTLIST>(PROTOCOL_CLIENT_PRODUCTEVENTLIST)
	{
		AccountSeq = 0;
		SalesZone = ProductDisplaySeq = 0;
	}

public:	
	DWORD	AccountSeq;					// 계정 순번
	long	SalesZone;					// 판매 영역
	long	ProductDisplaySeq;			// 전시 상품 카테고리 순번
};

// S->C : 이벤트 상품 목록 조회
class MSG_SERVER_PRODUCTEVENTLIST : public MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTEVENTLIST>
{
public:
	MSG_SERVER_PRODUCTEVENTLIST()
		: MSG_RESPONSE_BASE<MSG_SERVER_PRODUCTEVENTLIST>(PROTOCOL_SERVER_PRODUCTEVENTLIST)
	{
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ProductDisplaySeq = PackagesCount = 0;
	}
		
public:
	long	PackHeader;
	DWORD	AccountSeq;					// 계정 순번
	long	ProductDisplaySeq;			// 전시 상품 카테고리 순번
	long	PackagesCount;		
};


///////////////////////////////// 패키지 잔여 수량 조회 /////////////////////////////////

// C->S : 전시 상품 잔여 개수 조회
class MSG_CLIENT_PACKAGELEFTCOUNT : public MSG_REQUEST_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>
{
public:
	MSG_CLIENT_PACKAGELEFTCOUNT()
		: MSG_REQUEST_BASE<MSG_CLIENT_PACKAGELEFTCOUNT>(PROTOCOL_CLIENT_PACKAGELEFTCOUNT)
	{
		PackageProductSeq = 0;
	}

public:	
	long	PackageProductSeq;
};

// S->C : 전시 상품 잔여 개수 조회
class MSG_SERVER_PACKAGELEFTCOUNT : public MSG_RESPONSE_BASE<MSG_SERVER_PACKAGELEFTCOUNT>
{
public:
	MSG_SERVER_PACKAGELEFTCOUNT() 
		: MSG_RESPONSE_BASE<MSG_SERVER_PACKAGELEFTCOUNT>(PROTOCOL_SERVER_PACKAGELEFTCOUNT)
	{
		PackageProductSeq = LeftCount = 0;
	}
		
public:
	long	PackageProductSeq;
	long	LeftCount;
};


///////////////////////////////// 전체 보관함 조회 /////////////////////////////////

// C->S : 전체 보관함 조회
class MSG_CLIENT_STORAGELIST : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST>
{
public:
	MSG_CLIENT_STORAGELIST() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST>(PROTOCOL_CLIENT_STORAGELIST)
	{
		AccountSeq = 0;
		SalesZone = 0;
	}

public:
	DWORD	AccountSeq;					// 계정 순번
	int		SalesZone;					// 판매영역	
};

// S->C : 전체 보관함 조회
class MSG_SERVER_STORAGELIST : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST>
{
public:
	MSG_SERVER_STORAGELIST()
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST>(PROTOCOL_SERVER_STORAGELIST)
	{
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = 0;
	}

public:
	long	PackHeader;
	DWORD	AccountSeq;					// 계정 순번
	int		ListCount;					// 보관함 목록 개수
};


///////////////////////////////// 한 페이지 보관함 조회 /////////////////////////////////

// C->S : 한 페이지 보관함 조회
class MSG_CLIENT_STORAGELIST_PAGE : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST_PAGE>
{
public:
	MSG_CLIENT_STORAGELIST_PAGE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGELIST_PAGE>(PROTOCOL_CLIENT_STORAGELIST_PAGE)
	{
		AccountSeq = 0;
		SalesZone = PageSize = NowPage = 0;
		GiftMsgFlag = true;
		StorageType = 'A';
	}

public:
	DWORD	AccountSeq;					// 계정 순번
	int		SalesZone;					// 판매영역
	char	StorageType;				// A : 전체, S : 보관함, G : 선물함
	bool	GiftMsgFlag;				// 0이면 선물 메시지 안보낸다.
	int		PageSize;					// 한 페이지에 들어갈 아이템 수
	int		NowPage;					// 요청할 페이지 번호
};

// S->C : 한 페이지 보관함 조회
class MSG_SERVER_STORAGELIST_PAGE : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST_PAGE>
{
public:
	MSG_SERVER_STORAGELIST_PAGE()
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGELIST_PAGE>(PROTOCOL_SERVER_STORAGELIST_PAGE)
	{
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ListCount = NowPage = TotalPage = TotalRecord = 0;
		GiftMsgFlag = true;
		StorageType = 'A';
	}

public:
	long	PackHeader;
	DWORD	AccountSeq;					// 계정 순번
	int		ListCount;					// 보관함 목록 개수
	int		NowPage;					// 요청할 페이지 번호
	char	StorageType;				// A : 전체, S : 보관함, G : 선물함
	bool	GiftMsgFlag;				// 0이면 선물 메시지 안보낸다.
	int		TotalPage;					// 전체 페이지 수
	int		TotalRecord;				// 전체 아이템 수
};

///////////////////////////////// 보관함 사용 /////////////////////////////////

// C->S : 보관함 아이템 사용하기
class MSG_CLIENT_STORAGEUSE : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE>
{
public:
	MSG_CLIENT_STORAGEUSE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE>(PROTOCOL_CLIENT_STORAGEUSE)
	{
		AccountSeq = IPAddress = 0;
		StorageSeq = StorageItemSeq = Class = Level = ServerIndex = 0;
		StorageItemType = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
	}

public:	
	DWORD	AccountSeq;							// 계정 순번
	DWORD	IPAddress;							// 접근 IP
	long	StorageSeq;							// 보관함 순번
	long	StorageItemSeq;						// 보관함 Or 상품 순번
	char	StorageItemType;					// 캐시 Or 상품 구분 (C: 캐시, P: 상품)
	long	Class;								// 클래스
	long	Level;								// 레벨
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// 캐릭터 명
	WCHAR	Rank[MAX_RANK_LENGTH];				// 랭크
	long	ServerIndex;						// 서버인덱스	
};

// S->C : 보관함 아이템 사용하기
class MSG_SERVER_STORAGEUSE : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE>
{
public:
	MSG_SERVER_STORAGEUSE() 
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE>(PROTOCOL_SERVER_STORAGEUSE)
	{
		PackHeader = dwPacketSize;
		AccountSeq = 0;
		ProductSeq = StorageSeq = StorageItemSeq = 0;
		PropertyCount = 0;
		ZeroMemory(InGameProductID, MAX_TYPENAME_LENGTH);
	}

public:
	DWORD	AccountSeq;						//	계정 순번
	char	InGameProductID[MAX_TYPENAME_LENGTH];
	long	PackHeader;
	long	ProductSeq;
	BYTE	PropertyCount;
	long	StorageSeq;							// 보관함 순번
	long	StorageItemSeq;						// 보관함 Or 상품 순번

	/*	처리결과
		 0 : 성공
		 1 : 보관함 사용하기 항목이 없음
		 2 : PC방에서만 받기 가능한 아이템
		 3 : 기간 내 사용중인 컬러 요금이 있는 경우
		 4 : 기간 내 사용중인 개인 정액 상품이 있는경우
		-1 : 오류발생
		-2 : DB 처리 실패(정상적으로 수행하지 못함)
	*/
};


///////////////////////////////// 보관함 사용 롤백 /////////////////////////////////

// C->S : 보관함 아이템 사용 롤백
class MSG_CLIENT_STORAGEUSE_ROLLBACK : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE_ROLLBACK>
{
public:
	MSG_CLIENT_STORAGEUSE_ROLLBACK() 
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGEUSE_ROLLBACK>(PROTOCOL_CLIENT_STORAGEUSE_ROLLBACK)
	{
		AccountSeq = 0;
		StorageSeq = StorageItemSeq = 0;
	}

public:	
	DWORD	AccountSeq;							// 계정 순번
	long	StorageSeq;							// 보관함 순번
	long	StorageItemSeq;						// 보관함 상품 순번
};

// S->C : 보관함 아이템 사용 롤백
class MSG_SERVER_STORAGEUSE_ROLLBACK : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE_ROLLBACK>
{
public:
	MSG_SERVER_STORAGEUSE_ROLLBACK() 
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGEUSE_ROLLBACK>(PROTOCOL_SERVER_STORAGEUSE_ROLLBACK)
	{
		AccountSeq = 0;
	}

public:
	DWORD	AccountSeq;						//	계정 순번

	/*	처리결과 
		 0 : 성공
		 1 : Rollback 대상 없음
		-1 : 오류발생
		-2 : DB 처리 실패(정상적으로 수행하지 못함)
	*/
};


///////////////////////////////// 보관함 버리기 /////////////////////////////////

// C->S : 보관함 아이템 버리기
class MSG_CLIENT_STORAGETHROW : public MSG_REQUEST_BASE<MSG_CLIENT_STORAGETHROW>
{
public:
	MSG_CLIENT_STORAGETHROW()
		: MSG_REQUEST_BASE<MSG_CLIENT_STORAGETHROW>(PROTOCOL_CLIENT_STORAGETHROW)
	{
		AccountSeq = 0;
		StorageSeq = StorageItemSeq = 0;
		StorageItemType = 0;
	}

public:
	DWORD	AccountSeq;					// 계정 순번
	long	StorageSeq;					// 보관함 순번
	long	StorageItemSeq;				// 보관함 Or 상품 순번
	char	StorageItemType;			// 캐시 Or 상품 구분 (C: 캐시, P: 상품)
};

// S->C : 보관함 아이템 버리기
class MSG_SERVER_STORAGETHROW : public MSG_RESPONSE_BASE<MSG_SERVER_STORAGETHROW>
{
public:
	MSG_SERVER_STORAGETHROW()
		: MSG_RESPONSE_BASE<MSG_SERVER_STORAGETHROW>(PROTOCOL_SERVER_STORAGETHROW)
	{
		AccountSeq = 0;
	}

public:
	DWORD	AccountSeq;					// 계정 순번

	/*	처리결과 
		 0 : 성공
		 1 : 보관함 버리기 항목이 없음
		-1 : 오류발생
		-2 : DB 처리 실패(정상적으로 수행하지 못함)
	*/
};


///////////////////////////////// 마일리지 차감 /////////////////////////////////

// C->S : 마일리지 소진
class MSG_CLIENT_MILEAGEDEDUCT : public MSG_REQUEST_BASE<MSG_CLIENT_MILEAGEDEDUCT>
{
public:
	MSG_CLIENT_MILEAGEDEDUCT() 
		: MSG_REQUEST_BASE<MSG_CLIENT_MILEAGEDEDUCT>(PROTOCOL_CLIENT_MILEAGEDEDUCT)
	{
		AccountSeq = 0;
		DeductCategory = MileageSection = MileageDeductPoint = Class = Level = ServerIndex = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
		GameCode = 0; /*xxxxx*/
	}

public:		
	DWORD	AccountSeq;							// 개인 회원 계정 순번
	long	GameCode;							// 게임 코드 /*xxxxx*/
	long	DeductCategory;						// 마일리지소진유형
	long	MileageSection;						// 마일리지유형
	long	MileageDeductPoint;					// 마일리지 소진 포인트
	long	Class;								// 클래스
	long	Level;								// 레벨
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// 캐릭터명
	WCHAR	Rank[MAX_RANK_LENGTH];				// 랭크
	long	ServerIndex;						// 서버 번호
};

// S->C : 마일리지 소진
class MSG_SERVER_MILEAGEDEDUCT : public MSG_RESPONSE_BASE<MSG_SERVER_MILEAGEDEDUCT>
{
public:
	MSG_SERVER_MILEAGEDEDUCT()
		: MSG_RESPONSE_BASE<MSG_SERVER_MILEAGEDEDUCT>(PROTOCOL_SERVER_MILEAGEDEDUCT)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD	AccountSeq;				// 개인 회원 계정 순번
	
	// 조회 결과 코드 	0 : 성공, -1 :  오류발생
};


///////////////////////////////// 마일리지 적립 /////////////////////////////////

// C->S : 마일리지 적립
class MSG_CLIENT_MILEAGESAVE : public MSG_REQUEST_BASE<MSG_CLIENT_MILEAGESAVE>
{
public:
	MSG_CLIENT_MILEAGESAVE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_MILEAGESAVE>(PROTOCOL_CLIENT_MILEAGESAVE)
	{
		AccountSeq = 0;
		MileageType = MileageSection = MileagePoint = Class = Level = ServerIndex = 0;
		ZeroMemory(CharName, MAX_CHARACTERID_LENGTH * sizeof(WCHAR));
		ZeroMemory(Rank, MAX_RANK_LENGTH * sizeof(WCHAR));
		GameCode = 0; /*xxxxx*/
	}

public:		
	DWORD	AccountSeq;							// 개인 회원 계정 순번
	long	GameCode;							// 게임 코드 /*xxxxx*/
	long	MileageType;						// 마일리지적립유형
	long	MileageSection;						// 마일리지유형
	long	MileagePoint;						// 마일리지 적립 포인트
	long	Class;								// 클래스
	long	Level;								// 레벨
	WCHAR	CharName[MAX_CHARACTERID_LENGTH];	// 캐릭터명
	WCHAR	Rank[MAX_RANK_LENGTH];				// 랭크
	long	ServerIndex;						// 서버 번호
};

// S->C : 마일리지 적립
class MSG_SERVER_MILEAGESAVE : public MSG_RESPONSE_BASE<MSG_SERVER_MILEAGESAVE>
{
public:
	MSG_SERVER_MILEAGESAVE()
		: MSG_RESPONSE_BASE<MSG_SERVER_MILEAGESAVE>(PROTOCOL_SERVER_MILEAGESAVE)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD	AccountSeq;			// 개인 회원 계정 순번
	
	// 조회 결과 코드 	0 : 성공, -1 :  오류발생
};


///////////////////////////////// 실시간 마일리지 적립 /////////////////////////////////

// C->S : 실시간 마일리지 적립
class MSG_CLIENT_MILEAGELIVESAVEUP : public MSG_REQUEST_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>
{
public:
	MSG_CLIENT_MILEAGELIVESAVEUP()
		: MSG_REQUEST_BASE<MSG_CLIENT_MILEAGELIVESAVEUP>(PROTOCOL_CLIENT_MILEAGELIVESAVEUP)
	{
		AccountSeq = 0;
		MileageSection = SourceType = 0;
	}

public:		
	DWORD	AccountSeq;			// 개인 회원 계정 순번
	long	MileageSection;		// 마일리지유형
	long	SourceType;			// 개인, PC방 구분
};

// S->C : 실시간 마일리지 적립
class MSG_SERVER_MILEAGELIVESAVEUP : public MSG_RESPONSE_BASE<MSG_SERVER_MILEAGELIVESAVEUP>
{
public:
	MSG_SERVER_MILEAGELIVESAVEUP()
		: MSG_RESPONSE_BASE<MSG_SERVER_MILEAGELIVESAVEUP>(PROTOCOL_SERVER_MILEAGELIVESAVEUP)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD AccountSeq;			// 개인 회원 계정 순번
	
	// 조회 결과 코드 	0 : 성공, -1 :  오류발생
};


///////////////////////////////// 아이템 고유코드 갱신 /////////////////////////////////

// C->S : 아이템 시리얼 업데이트
class MSG_CLIENT_ITEMSERIALUPDATE : public MSG_REQUEST_BASE<MSG_CLIENT_ITEMSERIALUPDATE>
{
public:
	MSG_CLIENT_ITEMSERIALUPDATE()
		: MSG_REQUEST_BASE<MSG_CLIENT_ITEMSERIALUPDATE>(PROTOCOL_CLIENT_ITEMSERIALUPDATE)
	{
		AccountSeq = 0;
		StorageSeq = StorageItemSeq = 0;
		InGameUseCode = 0;
	}

public:	
	DWORD	AccountSeq;			// 회원 계정 순번
	long	StorageSeq;			// 보관함 순번
	long	StorageItemSeq;		// 보관함 캐시 or 상품 순번
	INT64	InGameUseCode;		// 아이템 시리얼 코드	
};

// S->C : 아이템 시리얼 업데이트
class MSG_SERVER_ITEMSERIALUPDATE : public MSG_RESPONSE_BASE<MSG_SERVER_ITEMSERIALUPDATE>
{
public:
	MSG_SERVER_ITEMSERIALUPDATE() 
		: MSG_RESPONSE_BASE<MSG_SERVER_ITEMSERIALUPDATE>(PROTOCOL_SERVER_ITEMSERIALUPDATE)
	{
		AccountSeq = 0;
	}
		
public:
	DWORD	AccountSeq;			// 개인 회원 계정 순번
	
	// 조회 결과 코드 	0 : 성공, 1: 대상 항목 없음, -1 :  오류발생
};


/////////////////////// 인게임 포인트 유형별 지급 퍼센트 조회 ///////////////////////

// C->S
class MSG_CLIENT_INQUIRE_GAMEPOINTVALUE : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_GAMEPOINTVALUE>
{
public:
	MSG_CLIENT_INQUIRE_GAMEPOINTVALUE() 
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_GAMEPOINTVALUE>(PROTOCOL_CLIENT_INQUIRE_GAMEPOINTVALUE)
	{
		ServerType = AccessType = 0;
	}

public:	
	long	ServerType;
	long	AccessType;
};

// S->C
class MSG_SERVER_INQUIRE_GAMEPOINTVALUE : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_GAMEPOINTVALUE>
{
public:
	MSG_SERVER_INQUIRE_GAMEPOINTVALUE() 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_GAMEPOINTVALUE>(PROTOCOL_SERVER_INQUIRE_GAMEPOINTVALUE)
	{
		PackHeader = dwPacketSize;
		ListCount = 0;
	}
		
public:
	long	PackHeader;
	long	ListCount;
};


///////////////////////////////// 상품 버전 조회 /////////////////////////////////

// C->S : 상품 버전 조회
class MSG_CLIENT_INQUIRE_SALESVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_SALESVERSION>
{
public:
	MSG_CLIENT_INQUIRE_SALESVERSION()
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_SALESVERSION>(PROTOCOL_CLIENT_INQUIRE_SALESVERSION)
	{
		SalesZone = 0;
	}

public:
	long	SalesZone;						// 판매 영역 번호
};

// S->C : 상품 버전 조회
class MSG_SERVER_INQUIRE_SALESVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_SALESVERSION>
{
public:
	MSG_SERVER_INQUIRE_SALESVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_SALESVERSION>(PROTOCOL_SERVER_INQUIRE_SALESVERSION)
	{
		SalesZone = Year = YearIdentity = 0;
	}

public:
	unsigned short	SalesZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};


///////////////////////////////// 배너 버전 조회 /////////////////////////////////

// C->S : 배너 버전 조회
class MSG_CLIENT_INQUIRE_BANNERVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_BANNERVERSION>
{
public:
	MSG_CLIENT_INQUIRE_BANNERVERSION()
		: MSG_REQUEST_BASE<MSG_CLIENT_INQUIRE_BANNERVERSION>(PROTOCOL_CLIENT_INQUIRE_BANNERVERSION)
	{
		BannerZone = 0;
	}

public:
	long	BannerZone;						// 배너 영역 번호
};	

// S->C : 배너 버전 조회
class MSG_SERVER_INQUIRE_BANNERVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_BANNERVERSION>
{
public:
	MSG_SERVER_INQUIRE_BANNERVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_INQUIRE_BANNERVERSION>(PROTOCOL_SERVER_INQUIRE_BANNERVERSION)
	{
		BannerZone = Year = YearIdentity = 0;
	}

public:
	unsigned short	BannerZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};


///////////////////////////////// 버전 업데이트 /////////////////////////////////

// C->S : 버전 업데이트
class MSG_CLIENT_UPDATEVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_UPDATEVERSION>
{
public:
	MSG_CLIENT_UPDATEVERSION() 
		: MSG_REQUEST_BASE<MSG_CLIENT_UPDATEVERSION>(PROTOCOL_CLIENT_UPDATEVERSION)
	{
		SalesZone = Year = YearIdentity = 0;
	}

public:	
	unsigned short	SalesZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};

// S->C : 버전 업데이트
class MSG_SERVER_UPDATEVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_UPDATEVERSION>
{
public:
	MSG_SERVER_UPDATEVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_UPDATEVERSION>(PROTOCOL_SERVER_UPDATEVERSION)
	{
		SalesZone = Year = YearIdentity = 0;
	}
		
public:
	unsigned short	SalesZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};


///////////////////////////////// 배너 버전 업데이트 /////////////////////////////////

// C->S : 배너 버전 업데이트
class MSG_CLIENT_UPDATE_BANNERVERSION : public MSG_REQUEST_BASE<MSG_CLIENT_UPDATE_BANNERVERSION>
{
public:
	MSG_CLIENT_UPDATE_BANNERVERSION() 
		: MSG_REQUEST_BASE<MSG_CLIENT_UPDATE_BANNERVERSION>(PROTOCOL_CLIENT_UPDATE_BANNERVERSION)
	{
		BannerZone = Year = YearIdentity = 0;
	}

public:	
	unsigned short	BannerZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};

// S->C : 배너 버전 업데이트
class MSG_SERVER_UPDATE_BANNERVERSION : public MSG_RESPONSE_BASE<MSG_SERVER_UPDATE_BANNERVERSION>
{
public:
	MSG_SERVER_UPDATE_BANNERVERSION() 
		: MSG_RESPONSE_BASE<MSG_SERVER_UPDATE_BANNERVERSION>(PROTOCOL_SERVER_UPDATE_BANNERVERSION)
	{
		BannerZone = Year = YearIdentity = 0;
	}
		
public:
	unsigned short	BannerZone;
	unsigned short	Year;
	unsigned short	YearIdentity;
};

}

#pragma pack()

