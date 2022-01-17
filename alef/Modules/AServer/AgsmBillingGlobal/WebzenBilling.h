#ifndef _WEBZENBILLING_H
#define _WEBZENBILLING_H

#ifdef _EXPORT_WZBILL_DLL
	#define WZBILL_EXPORT	__declspec(dllexport)
#else
	#define WZBILL_EXPORT	__declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
// Shop
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

typedef void (*CFunction_OnInquireCash)(PVOID);
typedef void (*CFunction_OnBuyProduct)(PVOID);
typedef void (*CFunction_OnLog)(CHAR*);

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
	BOOL BuyProduct(DWORD AccountSeq, CHAR* AccountID, DWORD GameCode, DWORD ProductSeq, CHAR* ProductName, DWORD InGamePurchaseSeq, int Class, int Level, CHAR* CharName, int ServerIndex, DWORD SalesZone, double DeductPrice, char DeductType, int MethodType);
	eWZConnect GetStatus();

	//////////////////////////////////////////////////////////////////////////
	//
	VOID SetCallbackOnInquireCash(CFunction_OnInquireCash fnCallback);
	VOID SetCallbackOnBuyProduct(CFunction_OnBuyProduct fnCallback);
	VOID SetCallbackOnLog(CFunction_OnLog fnCallback);
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