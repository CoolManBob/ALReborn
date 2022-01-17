#ifndef __HanBillingForSvr__
#define __HanBillingForSvr__

#ifdef HANBILLINGFORSVR_EXPORTS
#define HANBILLING_API __declspec(dllexport)
#else
#define HANBILLING_API __declspec(dllimport)
	#ifdef _DEBUG
		#ifdef _WIN64
			#pragma comment(lib, "HanBillingForSvrD_x64.lib")
		#else
			#pragma comment(lib, "HanBillingForSvrD.lib")
		#endif
	#else
		#ifdef _WIN64
			#pragma comment(lib, "HanBillingForSvr_x64.lib") 
		#else
			#pragma comment(lib, "HanBillingForSvr.lib") 
		#endif
	#endif
#endif

#define SERVICE_NATION  0x000000FF
#define SERVICE_KOR		0x00000001
#define SERVICE_USA		0x00000002
#define SERVICE_JPN		0x00000003
#define SERVICE_CHN		0x00000004

#define SERVICE_TYPE    0x00000F00
#define SERVICE_ALPHA	0x00000100
#define SERVICE_REAL	0x00000200
#define SERVICE_BETA	0x00000300

#define SERVICE_SITE	0x0000F000
#define SERVICE_HANGAME 0x00001000
#define SERVICE_ASOBLOG 0x00002000

/*
Return value Info:
0 : Ok
- : fault from  function / this system 
-1 : general fault.
+ : fault from billing server
*/
#define HAN_BILLING_OK						0
#define HAN_BILLING_GAMECODE_NOTEXIST		1
#define HAN_BILLING_USERNO_NOTEXIST			2
#define HAN_BILLING_CMD_NOTEXIST			3
#define HAN_BILLING_ORDNO_NOTEXIST			4
#define HAN_BILLING_SCD_NOTEXIST			5
#define HAN_BILLING_SNAME_NOTEXIST			6
#define HAN_BILLING_QTY_NOTEXIST			7
#define HAN_BILLING_AMT_NOTEXIST			8
#define HAN_BILLING_IP_NOTEXIST				9
#define HAN_BILLING_USERNO_INVALID		   10
#define HAN_BILLING_BUY_FAIL			   11
#define HAN_BILLING_GAMECODE_INVALID	   12
#define HAN_BILLING_BALANCE_INSUFFICIENT   13
#define HAN_BILLING_CMD_UNEXPECTED		   14
#define HAN_BILLING_DB_CONNECTFAIL		   15
#define HAN_BILLING_SVR_UNEXPECTED		   16

//
// HanBillingGetBillNo() specific codes
//
#define HAN_BILLING_GETBILLNO_NEED_PARAM       1
#define HAN_BILLING_GETBILLNO_IP_MISMATCH      2
#define HAN_BILLING_GETBILLNO_USERID_MISMATCH  3
#define HAN_BILLING_GETBILLNO_DB_CONN_ERROR    4
#define HAN_BILLING_GETBILLNO_USER_NOTEXIST    5
#define HAN_BILLING_GETBILLNO_COOKIE_ERROR     8
#define HAN_BILLING_GETBILLNO_COOKIE_IS_EMPTY  9
#define HAN_BILLING_GETBILLNO_COOKIE_TIMEOUT   10


#define HAN_BILLING_ARGUMENT_INVALID	   -1
#define HAN_BILLING_INITED_ALREADY		   -2
#define HAN_BILLING_INITED_NOT			   -3
#define HAN_BILLING_INITED_FAIL			   -4
#define HAN_BILLING_INITED_FAILBALANCE	   -5
#define HAN_BILLING_INITED_FAILBUY		   -6
#define HAN_BILLING_INITED_FAILCONFIRM	   -7
#define HAN_BILLING_GETCONN_FAILBALANCE	   -8
#define HAN_BILLING_GETCONN_FAILBUY		   -9
#define HAN_BILLING_GETCONN_FAILCONFIRM	  -10
#define HAN_BILLING_OPENREQ_FAIL		  -11
#define HAN_BILLING_SENDREQ_FAIL		  -12
#define HAN_BILLING_RECVRESULT_FAIL		  -13
#define HAN_BILLING_RECVRESULT_INVALID	  -14

#define HAN_BILLING_INITED_FAILBILLNO     -15  // 2009-01-12
#define HAN_BILLING_GETCONN_FAILBILLNO    -16  // 2009-01-12
#define HAN_BILLING_DLL_UNEXPECTED		 -100

//
// Initial values
//
#define INVALID_SERVICE_CODE				0xFFFFFFFF

HANBILLING_API int __stdcall HanBillingInit	(/*IN*/ const char* szGameId,
											 /*IN*/ unsigned int nServiceCode = INVALID_SERVICE_CODE);

#define MAX_BALANCENAMELEN  16
#define MAX_BALANCECNT      5
#define SIZE_RETBUFFER      1024

//
// HanBillingGetBillNo (for Japan only. 2009-01-09)
//
#define MAX_BILLNO_LEN      20

HANBILLING_API int __stdcall HanBillingGetBillNo(/*IN*/  const char *szMemberId,
                                                 /*OUT*/ char *szBillNo,
                                                 /*OUT*/ char *szRetMsg,
                                                 /*IN*/  int nRetMsg_size);

typedef struct hanbill_balance {  
	int	nBalance;  
	char szBalanceName[MAX_BALANCENAMELEN];  	
} HANBILL_BALANCE, *PHANBILL_BALANCE;

typedef struct hanbill_balanceinfo {  
	int	nTotalBalance;  
	int nBalanceCnt;
    HANBILL_BALANCE sBalace[MAX_BALANCECNT];	  	
} HANBILL_BALANCEINFO, *PHANBILL_BALANCEINFO;

HANBILLING_API int __stdcall HanBillingBalanceInquire(/*IN*/  const char *szMemberId,
													  /*IN*/  const char *nMemberNo,
													  /*OUT*/ char *szRetMsg,
													  /*IN*/  int	nRetMsg_size,
													  /*OUT*/ PHANBILL_BALANCEINFO pBalanceInfo);

#define MAX_CODELEN       20+1
typedef struct hanbill_buyinfo {  
	int  nBalance;
	char szOrderNo[MAX_CODELEN];  
	char szPaymentCode[MAX_CODELEN];	
} HANBILL_BUYINFO, *PHANBILL_BUYINFO;

// szOrderNo is always NULL , except a specified game..
HANBILLING_API int __stdcall HanBillingBuy(/*IN*/  const char *szMemberId,
										   /*IN*/  const char *szMemberNo,
										   /*OUT*/ char *szRetMsg,
										   /*IN*/  int	nRetMsg_size,
										   /*IN*/ const char *szSCD,
										   /*IN*/ const char *szSName,
										   /*IN*/ int nQty,
										   /*IN*/ int nAmt,
										   /*IN*/ const char *szClientIP,
										   /*IN*/ const char *szOrderNo,  
										   /*OUT*/ PHANBILL_BUYINFO pBuyInfo);

typedef struct hanbill_confirminfo {  
	char szOrderNo[MAX_CODELEN];  	
	char szPaymentCode[MAX_CODELEN];	
} HANBILL_CONFIRMINFO, *PHANBILL_CONFIRMINFO;

HANBILLING_API int __stdcall HanBillingConfirmPay(/*IN*/  const char *szMemberId,
												  /*IN*/  const char *szMemberNo,
												  /*OUT*/ char *szRetMsg,
												  /*IN*/  int	nRetMsg_size,
												  /*IN*/  const char *szOrderNo,
												  /*OUT*/ PHANBILL_CONFIRMINFO pConfirmInfo);

#endif // __HanBillingForSvr__