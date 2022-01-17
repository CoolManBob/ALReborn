#ifndef _WEBZENBILLING_H
#define _WEBZENBILLING_H

#pragma pack(1)

#ifdef _EXPORT_WZBILL_DLL
	#define WZBILL_EXPORT	__declspec(dllexport)
#else
	#define WZBILL_EXPORT	__declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
// Shop
typedef struct  
{
	bool success;
	DWORD error;

} stOnNetConnect;

typedef struct  
{
	long ResultCode;
	DWORD AccountSeq;
	CHAR AccountID[21];
	double WCoinSum;
	double PCoinSum;
} stInquire;

typedef struct  
{
	long ResultCode;
	DWORD AccountSeq;
	CHAR AccountID[21];
	DWORD DeductCashSeq;
	DWORD InGamePurchaseSeq;
} stBuyProduct;

//JK_웹젠빌링
typedef struct
{
	long ResultCode;
	DWORD AccountSeq;
	long LeftProductCount;
} stBuyProduct_Union;

typedef struct  
{
	int GameCode;
	int SalesZone;
	int Year;
	int YaerIdentity;
	int ResultCode;
} stSalesZoneScriptVersion;

#define MAX_TYPENAME_LENGTH 21
#define MAX_ACCOUNTID_LENGTH 51

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

#define MAX_STORAGELISTCOUNT_PER_PAGE			15
#define STORAGELISTCOUNT_PER_PAGE				7


typedef struct  
{
	DWORD AccountID;
	DWORD ResultCode;
	char  StorageType;
	int   NowPage;
	int	  TotalPage;
	int   TotalCount;
	int   ListCount;
	STStorageNoGiftMessage StorageList[MAX_STORAGELISTCOUNT_PER_PAGE];

} stStorageListPageNoGiftMessage;

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
#define MAX_PROPERTYCOUNT		10
typedef struct  
{
	DWORD AccountSeq;
	DWORD ResultCode;
	CHAR  InGameProductID[20];
	BYTE  PropertyCount;
	DWORD ProductSeq;
	DWORD StorageSeq;
	DWORD StorageItemSeq;
	STItemProperty PropertyList[MAX_PROPERTYCOUNT];

} stUseStorage;

typedef struct  
{
	DWORD AccountSeq;
	DWORD ResultCode;
} stRollbackUseStorage;

///////////////////////

typedef void (*CFunction_OnInquireCash)(PVOID);
typedef void (*CFunction_OnBuyProduct)(PVOID);
typedef void (*CFunction_OnLog)(CHAR*);
//JK_웹젠빌링
typedef void (*CFunction_OnInquireSalesZoneScriptVersion)(PVOID);
typedef void (*CFunction_OnUpdateVersion)(PVOID);
typedef void (*CFunction_OnInquireStorageListPageNoGiftMessage)(PVOID);
typedef void (*CFunction_OnUseStorage)(PVOID);
typedef void (*CFunction_OnRollbackUseStorage)(PVOID);

typedef void (*CFunction_OnNetConnect)(PVOID);

//////////////////////////////////////////////////////////////////////////
// Billing
typedef struct
{
	long AccountGUID;
	long RoomGUID;
	long GameCode;
	long ResultCode;
} stInquirePCRoomPoint;

typedef struct  
{
	long AccountID;
	long RoomGUID;
	long Result;
} stInquireMultiUser;

typedef struct  
{
	DWORD dwAccountGUID;
	long dwBillingGUID;
	DWORD RealEndDate;
	DWORD EndDate;
	double dRestPoint;
	double dRestTime;
	short nDeductType;
	short nAccessCheck;
	short nResultCode;
} stUserStatus;

typedef struct  
{
	long AccountGUID;
	long GameCode;
	long ResultCode;
} stInquirePersonDeduct;

typedef void (*CFunction_OnInquirePCRoomPoint)(PVOID);
typedef void (*CFunction_OnInquireMultiUser)(PVOID);
typedef void (*CFunction_OnUserStatus)(PVOID);
typedef void (*CFunction_OnInquirePersonDeduct)(PVOID);


#pragma pack()


//////////////////////////////////////////////////////////////////////////
// common
enum eWZConnect
{
	eNotConnect,
	eConnecting,
	eConnected,
};

class WZBILL_EXPORT CWebzenShop
{
public:
	CWebzenShop();
	virtual ~CWebzenShop();

	BOOL Initialize();

	//////////////////////////////////////////////////////////////////////////
	//
	eWZConnect Connect(char* dest, unsigned short port);
	BOOL InquireCash(BYTE ViewType, DWORD AccountSeq, CHAR* AccountID, DWORD GameCode, BOOL SumOnly);
	//JK_웹젠빌링
	BOOL InquireCash( DWORD GameCode, BYTE ViewType, DWORD AccountSeq, BOOL SumOnly, DWORD MileageSection );

	BOOL BuyProduct(DWORD AccountSeq, CHAR* AccountID, DWORD GameCode, DWORD ProductSeq, CHAR* ProductName, DWORD InGamePurchaseSeq, int Class, int Level, CHAR* CharName, int ServerIndex, DWORD SalesZone, double DeductPrice, char DeductType, int MethodType);
	//JK_웹젠빌링
	BOOL BuyProduct(DWORD GameCode, DWORD AccountSeq, DWORD ProductSeq, DWORD DisplaySeq, DWORD SalesZone, DWORD PriceSeq, int Class, int Level, CHAR* CharName, CHAR* Rank, int ServerIndex);

	//JK_웹젠빌링
	BOOL InquireSalesZoneScriptVersion(DWORD GameCode, DWORD SalesZone);
	//JK_웹젠빌링
	BOOL InquireStorageListPageNoGiftMessage(DWORD AccountSeq, DWORD GameCode, DWORD SalesZone, char StorageType,int NowPage, int PageSize);
	//JK_웹젠빌링
	BOOL UseStorage(DWORD AccountSeq, DWORD GameCode, DWORD IPAddress, DWORD StorageSeq, DWORD StorageItemSeq, char StorageItemType, int Class, int Level, CHAR* CharName, CHAR* Rank, int ServerIndex);
	//JK_웹젠빌링
	BOOL RollbackUseStorage(DWORD AccountSeq, DWORD GameCode, DWORD StorageSeq, DWORD StorageItemSeq);




	eWZConnect GetStatus();

	//////////////////////////////////////////////////////////////////////////
	//
	VOID SetCallbackOnInquireCash(CFunction_OnInquireCash fnCallback);
	VOID SetCallbackOnBuyProduct(CFunction_OnBuyProduct fnCallback);
	VOID SetCallbackOnLog(CFunction_OnLog fnCallback);

	//JK_웹젠빌링
	VOID SetCallbackOnInquireSalesZoneScriptVersion(CFunction_OnInquireSalesZoneScriptVersion fnCallback);
	VOID SetCallbackOnUpdateVersion(CFunction_OnUpdateVersion fnCallback);
	VOID SetCallbackOnInquireStorageListPageNoGiftMessage(CFunction_OnInquireStorageListPageNoGiftMessage fnCallback);
	VOID SetCallbackOnUseStorage(CFunction_OnUseStorage fnCallback);
	VOID SetCallbackOnRollbackUseStorage( CFunction_OnUseStorage fnCallback );


	VOID SetCallbackOnNetConnect(CFunction_OnNetConnect fnCallback);

};

class WZBILL_EXPORT CWebzenBilling
{
public:
	CWebzenBilling();
	virtual ~CWebzenBilling();

	BOOL Initialize();

	//////////////////////////////////////////////////////////////////////////
	//
	eWZConnect Connect(char* dest, unsigned short port);
	eWZConnect GetStatus();

	BOOL UserLogin(DWORD dwAccountGUID, DWORD dwIPAddress, DWORD dwRoomGUID, DWORD dwGameCode, DWORD dwServerType);
	BOOL UserLogout(long dwBillingGUID);
	BOOL InquireUser(long dwBillingGUID);
	BOOL InquireMultiUser(long AccountGUID, long RoomGUID);	
	BOOL InquirePCRoomPoint(long AccountGUID, long RoomGUID, long GameCode);
	BOOL InquirePersonDeduct(long AccountGUID, long GameCode);

	//////////////////////////////////////////////////////////////////////////
	//
	VOID SetCallbackOnLog(CFunction_OnLog fnCallback);
	VOID SetCallbackOnInquirePCRoomPoint(CFunction_OnInquirePCRoomPoint fnCallback);
	VOID SetCallbackOnInquireMultiUser(CFunction_OnInquireMultiUser fnCallback);
	VOID SetCallbackOnUserStatus(CFunction_OnUserStatus fnCallback);
	VOID SetCallbackOnInquirePersonDeduct(CFunction_OnInquirePersonDeduct fnCallback);
};



#endif //_WEBZENBILLING_H