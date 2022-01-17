#ifndef __HAN_nEVER_BILL_FOR_SVR_H__
#define __HAN_nEVER_BILL_FOR_SVR_H__

#if defined(_WIN32) || defined(_WIN64)

	#ifdef HANNEVERBILLFORSVR_EXPORTS
		#define HANNEVERBILL_API __declspec(dllexport)
	#else
		#define HANNEVERBILL_API __declspec(dllimport)
		#ifdef _DEBUG
			#ifdef _WIN64
				#pragma comment(lib, "HannEverBillForSvrD_x64.lib")
			#else				
				#pragma comment(lib, "HannEverBillForSvrD.lib")
			#endif
		#else
			#ifdef _WIN64
				#pragma comment(lib, "HannEverBillForSvr_x64.lib") 
			#else
				#pragma comment(lib, "HannEverBillForSvr.lib") 
			#endif
		#endif
	#endif

	#pragma comment(lib, "ws2_32.lib")

#elif defined(__linux__)

	#define HANNEVERBILL_API
	#define __stdcall

#else

	#error OS not supported.

#endif

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

// Constants
#define SERVICE_NATION									0x000000FF
#define SERVICE_KOR										0x00000001
#define SERVICE_USA										0x00000002
#define SERVICE_JPN										0x00000003
#define SERVICE_CHN										0x00000004

#define SERVICE_TYPE									0x00000F00
#define SERVICE_ALPHA									0x00000100
#define SERVICE_REAL									0x00000200
#define SERVICE_BETA									0x00000300

#define SERVICE_SITE									0x0000F000
#define SERVICE_HANGAME									0x00001000
#define SERVICE_ASOBLOG									0x00002000

#define nEB_USERKEY_LEN									20
#define nEB_CPID_LEN									10
#define nEB_ITEMID_LEN									20
#define nEB_TITLE_LEN									30
#define nEB_MEMO_LEN									30
#define nEB_MANAGERID_LEN								16
#define nEB_VIPFG_LEN									16
#define nEB_SUBSFG_LEN									16
#define nEB_SETTLEDT_LEN								8
#define nEB_REMOTEIP_LEN								16
#define nEB_SERVICEINF_LEN								16
#define nEB_SETTLERCTDT_LEN								8
#define nEB_RTNMSG_LEN									200

#define MAX_MULTIPLE_PURCHASE							10

// @brief : REQUEST_USECOIN
typedef struct
{
	int		nPayment;									// 결제금액
	char	szUserKey[nEB_USERKEY_LEN+1];				// userkey(=userid) 유니크한 고객 key	
	char	szCPID[nEB_CPID_LEN+1];						// 상점 아이디(빌링 인프라팀에서 미리 발급 받아야 함)
	char	szItemID[nEB_ITEMID_LEN+1];					// 아이템 아이디(빌링 인프라팀에서 미리 발급 받아야 함)
	char	szTitle[nEB_TITLE_LEN+1];					// 노출 상품명
	char	szMemo[nEB_MEMO_LEN+1];						// 메모
	char	szManagerID[nEB_MANAGERID_LEN+1];			// 센터관리자 아이디
	char	szVIP_FG[nEB_VIPFG_LEN+1];					// VIP 한도 사용여부
	char	szSUBS_FG[nEB_SUBSFG_LEN+1];				// 자동결제 여부
	char	szSettleStartDate[nEB_SETTLEDT_LEN+1];		// 정산시작일 (yyyymmdd)
	char	szSettleEndDate[nEB_SETTLEDT_LEN+1];		// 정산만료일 (yyyymmdd)
	char	szRemoteIP[nEB_REMOTEIP_LEN+1];				// 아이피
	char	szServiceINF[nEB_SERVICEINF_LEN+1];			// 서비스 주문번호
	char	szSettlerCountDate[nEB_SETTLERCTDT_LEN+1];	// 정산일
} REQUEST_USECOIN;

// @brief : RESULT_USECOIN
typedef struct 
{
	int			nRtnCode;								// return code
	char		szRtnMsg[nEB_RTNMSG_LEN+1];				// return message
} RESULT_USECOIN;

// @brief : RESULT_MULTI_USECOIN
typedef struct 
{
	int			nRtnCode;								// return code
	char*		pRtnMsgBuff;							// return message buffer
	int			nRtnMsgBuffLen;							// return message buffer len
} RESULT_MULTI_USECOIN;

// @brief : RESULT_QUERYCOIN
typedef struct 
{
	long			lCoin;								// coin
	long			lGiftCoin;							// gift coin
	long			lExEvntCoin;						// external event coin
	long			lInEvntCoin;						// internal event coin
} RESULT_QUERYCOIN;

// @brief : Clear Macro
#define CLEAR_REQ_UC(p)									if(p){memset((p),0,sizeof(REQUEST_USECOIN));}
#define CLEAR_RST_UC(p)									if(p){memset((p),0,sizeof(RESULT_USECOIN));(p)->nRtnCode=-1;}
#define CLEAR_RST_MUC(p)								if(p){memset((p),0,sizeof(RESULT_MULTI_USECOIN));(p)->nRtnCode=-1;}
#define CLEAR_RST_QC(p)									if(p){memset((p),0,sizeof(RESULT_QUERYCOIN));}

#define DELEMETER										"|"

/*
Return value Info:
0 : Ok
- : fault from  function / this system 
*/
#define HAN_NEVERBILL_OK								0
#define HAN_NEVERBILL_ARGUMENT_INVALID					-1
#define HAN_NEVERBILL_INITED_NOT						-2
#define HAN_NEVERBILL_INITED_FAIL						-3
#define HAN_NEVERBILL_RPC_CLIENT_CREATEION_FAILED		-4
#define HAN_NEVERBILL_RPC_CONNECTOR_CREATEION_FAILED	-5
#define HAN_NEVERBILL_CONNECTION_FAILED					-6
#define HAN_NEVERBILL_EXEC_FAILED						-7
#define HAN_NEVERBILL_LOAD_LOCAL_DIRECTORY_FILE_FAILED	-8
#define HAN_NEVERBILL_INVALID_HANDLE					-9
#define HAN_NEVERBILL_PARAMS_CREATION_FAILED			-10
#define HAN_NEVERBILL_EXEC_RESULT_LIST_INVALID			-11
#define HAN_NEVERBILL_EXEC_RESULT_USER_NOT_EXIST		-12
#define HAN_NEVERBILL_TXN_BEGIN_FAILED					-13
#define HAN_NEVERBILL_TXN_COMMIT_FAILED					-14
#define HAN_NEVERBILL_TXN_ROLLBACK_FAILED				-15
#define HAN_NEVERBILL_RPCCLIENT_INVALLID				-16
#define HAN_NEVERBILL_QC_RPC_CLIENT_INVALID				-17
#define HAN_NEVERBILL_UC_RPC_CLIENT_INVALID				-18
#define HAN_NEVERBILL_MUC_RPC_CLIENT_INVALID			-19
#define HAN_NEVERBILL_NOT_ENOUGH_MEMORY					-20
#define HAN_NEVERBILL_UNEXPECTED						-100

#if defined(__linux) && defined(__cplusplus)
extern "C"
{
#endif // __linux__&&__cplusplus

// @brief: Init
HANNEVERBILL_API int
	__stdcall	HannEverBillInit(IN const char* szGameID);

// @brief: Query Coin
HANNEVERBILL_API int
	__stdcall	HannEverBillQueryCoin(OUT RESULT_QUERYCOIN* pResult, IN const char* szUserID);

// @brief: Use Coin
HANNEVERBILL_API int
	__stdcall	HannEverBillUseCoin(OUT RESULT_USECOIN* pResult, IN const REQUEST_USECOIN* pReqUseCoin);

HANNEVERBILL_API int
	__stdcall	HannEverBillMultiUseCoin(OUT RESULT_MULTI_USECOIN* pResult,
										IN const REQUEST_USECOIN* pReqUseCoinArray,
										IN const int nCount);

HANNEVERBILL_API int
	__stdcall	HannEverBillMultiUseCoinPA(OUT RESULT_MULTI_USECOIN* pResult,
										IN const REQUEST_USECOIN** ppReqUseCoinArray,
										IN const int nCount);

// @brief: Error...
HANNEVERBILL_API int 
	__stdcall	HannEverBillGetLastError();

HANNEVERBILL_API const char* 
	__stdcall	HannEverBillErrorString(int nErrorCode);

#if defined(__linux) && defined(__cplusplus)
}
#endif // __linux__&&__cplusplus

#endif // __HAN_nEVER_BILL_FOR_SVR_H__