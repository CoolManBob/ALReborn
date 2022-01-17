// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DORIAN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DORIAN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef __DORIAN_CONNECTOR_H__
#define __DORIAN_CONNECTOR_H__

 #if (defined(_WIN64) || defined(_WIN32)) 
	#ifdef DORIAN_EXPORTS
		#define DORIAN_EXTERN __declspec(dllexport)
	#else
		#define DORIAN_EXTERN __declspec(dllimport)		

		#ifndef DORIAN_NOSTUBLIB
			#ifdef _DEBUG
				#ifdef _WIN64
					#pragma comment(lib, "DORIAN3_ConnectorD_x64.lib")
				#else
					#pragma comment(lib, "DORIAN3_ConnectorD.lib")
				#endif // _WIN64
			#else
				#ifdef _WIN64
					#pragma comment(lib, "DORIAN3_Connector_x64.lib")
				#else
					#pragma comment(lib, "DORIAN3_Connector.lib")
				#endif // _WIN64
			#endif // _DEBUG
		#endif // DORIAN_NOSTUBLIB
	#endif // DORIAN_EXPORTS	
#elif defined(__linux__)
	#define DORIAN_EXTERN
	#define __stdcall
#else
	#error OS not supported.
#endif // _WINDOWS

#ifndef IN
	#define IN
#endif

#ifndef OUT
	#define OUT
#endif

#ifndef INOUT
	#define INOUT
#endif

#if (defined(_WIN64) || defined(_WIN32))
	typedef __int64 int64;
#else
	#include <stdint.h>
	typedef int64_t int64;
#endif

namespace Dorian
{
	//====================================================================
	// Billing notification info define
	// 자동변환 관련 정의가 추가되어야 함.
	const unsigned int BILLING_EXPIRE					= 0;	// 상품제 만료
	const unsigned int BILLING_REMAINING_TIME_ALRAM		= 1;	// 상품제 잔여 시간이 5분 이하
	const unsigned int CONFIRM_USER_EXIST				= 3;	// 사용자 존재유무 확인
	const unsigned int BILLING_GAME_SPECIFIC			= 1000; // 게임별 특정 상품제
	//--------------------------------------------------------------------

	//====================================================================
	// 체크인/상품제 조회 API result code define
	const unsigned int RESULT_OK						= 0;	// 성공
	const unsigned int RESULT_NO_PRODUCT				= 1;	// 상품제 없음
	const unsigned int RESULT_FAILED_OPERATION			= -1;	// 연산 수행 실패
	//--------------------------------------------------------------------

	//====================================================================
	// 문자열 최대 길이 정의
	enum MAX_LENGTH
	{
		// 게임 서버 정보 관련
		eMAX_GAME_ID = 20,
		eMAX_WORLD_ID = 20,
		eMAX_SERVER_ID = 20,
		eMAX_RECV_SERVER_ID = 20,

		// 사용자 정보 관련
		eMAX_USER_IP = 15,
		eMAX_USER_ID = 22,
		eMAX_CHAR_ID = 20,
		eMAX_RECV_USER_ID = 22,
		eMAX_RECV_CHAR_ID = 20,

		// 상품제 통지내용 확장 정보 관련
		eMAX_OPTION = 1024,

		// 상품제 만기날짜 정보 관련
		eMAX_TIME = 14,

		// 상품제 피씨방 코드 정보 관련
		eMAX_CRM = 15,

		// 에러 메시지 정보 관련
		eMAX_ERROR_MSG = 1024,

		// 메뉴 정보 관련
		eMAX_BRANCH_ID = 20,
		eMAX_BRANCH_NAME = 40,
		eMAX_SUPER_BRANCH_ID = 20,

		// 상품 정보 관련
		eMAX_PROD_ID = 20,
		eMAX_PROD_NAME = 40,
		eMAX_PROD_DESC = 500,
		eMAX_PROD_DETAIL_DESC = 2000,
		eMAX_CHILD_PROD_ID = 20,
		eMAX_DATE = 14,
		eMAX_IMG_URL = 100,
		eMAX_MOVIE_URL = 200,
		eMAX_MONEY_CODE = 20,
		eMAX_UNIT_NAME = 50,
		eMAX_DIRECTION = 200,
		eMAX_CLASS_NAME = 40,

		// 쿠폰 정보 관련
		eMAX_COUPON_USE_ID = 20,
		eMAX_COUPON_CODE = 20,
		eMAX_COUPON_NAME = 40,
		eMAX_USE_END_DATE = 8,

		// 스페셜 노출 상품 관련
		eMAX_EXPOSE_CODE = 20,
		eMAX_EXPOSE_NAME = 40,
		eMAX_BANNER_URL = 100,

		// 아이템 정보 관련
		eMAX_ITEM_ID = 20,
		eMAX_ITEM_NAME = 40,
		eMAX_VALID_TERM = 14,

		// 아이템 옵션 정보 관련
		eMAX_PROD_OPTION_CODE = 20,
		eMAX_PROD_OPTION_VALUE = 20,
		eMAX_PROD_OPTION_NAME = 20,
		eMAX_ITEM_OPTION_CODE = 100,
		eMAX_ITEM_OPTION_VALUE = 100,

		// 태그 정보 관련
		eMAX_TAG_NAME = 100,

		// 스토리지 정보 관련
		eMAX_SET_NAME = 40,

		// 구매 정보 관련
		eMAX_GIFT_MESSAGE = 200,
		eMAX_PASSWD = 20,

		// 구매/할인 조건 관련
		eMAX_CONDITION_VALUE = 100,
		eMAX_DISCOUNT_INFO = 20,

		// 조회 정보 관련
		eMAX_SEARCH_VALUE = 22,
	};
	//--------------------------------------------------------------------

	// DORIAN server 접속 정보.
	typedef struct _GAMESERVER_INFO
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szGameServerID[eMAX_SERVER_ID + 1];
		char	szWorldID[eMAX_WORLD_ID + 1];
	}GAMESERVER_INFO, *LPGAMESERVER_INFO;

	// 가변길이 조회결과를 위한 핸들
	typedef struct _DATA_HANDLE_EX
	{
	} *DATA_HANDLE_EX;

	//====================================================================
	// 정액/정량 상품제 관련 인자 선언
	// 상품제 관련 통지 정보
	typedef struct _BILLING_NOTI_INFO
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
		int		nEventCode;
		int		nSpecificCode;
		char	szOptions[eMAX_OPTION + 1];
	} BILLING_NOTI_INFO, *LPBILLING_NOTI_INFO;

	// 상품제 정보
	typedef struct _BILL_INFO
	{
		int		nBillType;
		int		nProductID;
		int64	nRemainMinutes;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
	} BILL_INFO, *LPBILL_INFO;

	// UserMoveTo/UserMoveFrom 관련 상품제 정보
	typedef struct _BILL_INFO_FOR_USER_MOVE
	{
		int		nBillType;
		int		nProductID;
		int		nSeqNo;
		bool	bFirstYN;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
		char	chFreeYN;
	} BILL_INFO_FOR_USER_MOVE, *LPBILL_INFO_FOR_USER_MOVE;

	// 체크인
	typedef struct _REQUEST_CHECKIN
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
	} REQUEST_CHECKIN, *LPREQUEST_CHECKIN;

	typedef struct _RESPONSE_CHECKIN
	{
		int		nResult;
		bool	bAlreadyUsedPCCafeIP;
		char	szErrMsg[eMAX_ERROR_MSG + 1];

		int		nBillType;
		int		nProductID;
		int64	nRemainMinutes;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
	} RESPONSE_CHECKIN, *LPRESPONSE_CHECKIN;

	typedef struct _RESPONSE_CHECKIN_EX
	{
		int				nResult;
		bool			bAlreadyUsedPCCafeIP;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		int				nBillType;
		int				nProductID;
		int64			nRemainMinutes;
		char			szCRM[eMAX_CRM + 1];
		char			szEndDate[eMAX_TIME + 1];
		DATA_HANDLE_EX	handle;
	} RESPONSE_CHECKIN_EX, *LPRESPONSE_CHECKIN_EX;

	// 체크아웃
	typedef struct _REQUEST_CHECKOUT
	{
		char	szUserID[eMAX_USER_ID + 1];
	} REQUEST_CHECKOUT, *LPREQUEST_CHECKOUT;

	typedef struct _RESPONSE_CHECKOUT
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_CHECKOUT, *LPRESPONSE_CHECKOUT;

	// 사용자 이동(to/from)
	typedef struct _REQUEST_USERMOVE_TO
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
	} REQUEST_USERMOVE_TO, *LPREQUEST_USERMOVE_TO, REQUEST_USERMOVE_FROM, *LPREQUEST_USERMOVE_FROM;

	typedef struct _RESPONSE_USERMOVE_TO
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_USERMOVE_TO, *LPRESPONSE_USERMOVE_TO, RESPONSE_USERMOVE_FROM, *LPRESPONSE_USERMOVE_FROM;

	// 상품제 조회
	typedef struct _REQUEST_BILL_INFO
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
	} REQUEST_BILL_INFO, *LPREQUEST_BILL_INFO;

	typedef struct _RESPONSE_BILL_INFO
	{
		int		nResult;
		bool	bAlreadyUsedPCCafeIP;
		char	szErrMsg[eMAX_ERROR_MSG + 1];

		int		nBillType;
		int		nProductID;
		int64	nRemainMinutes;
		char	szCRM[eMAX_CRM + 1];
		char	szEndDate[eMAX_TIME + 1];
		bool	bPCBang;
	} RESPONSE_BILL_INFO, *LPRESPONSE_BILL_INFO;

	typedef struct _RESPONSE_BILL_INFO_EX
	{
		int				nResult;
		bool			bAlreadyUsedPCCafeIP;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		int				nBillType;
		int				nProductID;
		int64			nRemainMinutes;
		char			szCRM[eMAX_CRM + 1];
		char			szEndDate[eMAX_TIME + 1];
		DATA_HANDLE_EX	handle;
	} RESPONSE_BILL_INFO_EX, *LPRESPONSE_BILL_INFO_EX;

	// 과금(for debugging)
	typedef struct _RESPONSE_CHARGE
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_CHARGE, *LPRESPONSE_CHARGE;
	//--------------------------------------------------------------------

	//====================================================================
	// 아이템 관련 인자 선언
	// 메뉴 리스트 조회
	typedef struct _REQUEST_MENU_LIST
	{
		char	szGameID[eMAX_GAME_ID + 1];
	} REQUEST_MENU_LIST, *LPREQUEST_MENU_LIST;

	typedef struct _RESPONSE_MENU_LIST
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1]; // 전송 실패 or 수신 실패 or 조회 실패

		DATA_HANDLE_EX	handle;
	} RESPONSE_MENU_LIST, *LPRESPONSE_MENU_LIST;

	typedef struct _MENU_INFO
	{
		char	szBranchID[eMAX_BRANCH_ID + 1];
		char	szBranchName[eMAX_BRANCH_NAME + 1];
		char	szSuperBranchID[eMAX_SUPER_BRANCH_ID + 1];
		int		nBranchDepth;
		int		nSortOrder;
		int		nProdCnt;
	} MENU_INFO, *LPMENU_INFO;

	// 상품 리스트 조회
	typedef struct _REQUEST_PROD_LIST
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szBranchID[eMAX_BRANCH_ID + 1];
		char	chSearchType;
		char	szSearchValue[eMAX_SEARCH_VALUE + 1];
		int		nRow;
		int		nPage;
	} REQUEST_PROD_LIST, *LPREQUEST_PROD_LIST;

	typedef struct _RESPONSE_PROD_LIST
	{ 
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		int				nPageCnt;
		int				nCurrPage;
		int				nTotalCnt;
		DATA_HANDLE_EX	handle;
	} RESPONSE_PROD_LIST, *LPRESPONSE_PROD_LIST;

	typedef struct _PROD_LIST_INFO
	{
		char	szBranchID[eMAX_BRANCH_ID + 1];
		char	szProdID[eMAX_PROD_ID + 1];
		char	szProdName[eMAX_PROD_NAME + 1];
		char	szMoneyCode[eMAX_MONEY_CODE + 1];
		int		nPrice;
		int		nDiscountRate;
		char	szRegDate[eMAX_DATE + 1];
		char	szStartDate[eMAX_DATE + 1];
		char	szEndDate[eMAX_DATE + 1];
		char	szProdDesc[eMAX_PROD_DESC + 1];
		bool	bGiftYN;
		char	szImgUrl[eMAX_IMG_URL + 1];
		int		nSellTotalCnt;
		int		nLevel;
		bool	bDuplicateSell;
		char	szUnitName[eMAX_UNIT_NAME + 1];
		char	szIconImgUrl[eMAX_IMG_URL + 1];
		int		nDispOrder;
		char	szDiscountInfo[eMAX_DISCOUNT_INFO + 1];
		char	szClassName[eMAX_CLASS_NAME + 1];
	} PROD_LIST_INFO, *LPPROD_LIST_INFO;

	// 상품 상세 정보 조회
	typedef struct _REQUEST_PROD_INFO
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szProdID[eMAX_PROD_ID + 1];
	} REQUEST_PROD_INFO, *LPREQUEST_PROD_INFO;

	typedef struct _RESPONSE_PROD_INFO
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1]; // 전송 실패 or 수신 실패 or 조회 실패

		char			szBranchID[eMAX_BRANCH_ID + 1];
		char			szProdID[eMAX_PROD_ID + 1];
		char			szProdName[eMAX_PROD_NAME + 1];
		char			szProdDetailDesc[eMAX_PROD_DETAIL_DESC + 1];
		char			szImgUrl[eMAX_IMG_URL + 1];
		char			szMovieUrl[eMAX_MOVIE_URL + 1];
		char			szDirection[eMAX_DIRECTION + 1];
		char			chCouponUseType;
		char			chDiscountType;

		DATA_HANDLE_EX	childProdHandle;
		DATA_HANDLE_EX	conditionHandle;
		DATA_HANDLE_EX	optionHandle;
	} RESPONSE_PROD_INFO, *LPRESPONSE_PROD_INFO;

	typedef struct _CHILDPROD_INFO
	{
		char	szChildProdID[eMAX_CHILD_PROD_ID + 1];
		int		nPrice;
		char	szUnitName[eMAX_UNIT_NAME + 1];
		int		nValidTerm;
		int		nPossibleLevel;
		int		nUseCnt;
		int		nPeriod;
		int		nHanCoinPrice;
		int		nGameMoneyPrice;
		char	szClassName[eMAX_CLASS_NAME + 1];
	} CHILDPROD_INFO, *LPCHILDPROD_INFO;

	typedef struct _CONDITION_INFO
	{
		char	chConditionType;
		char	szConditionValue[eMAX_CONDITION_VALUE + 1];
	} CONDITION_INFO, *LPCONDITION_INFO;

	typedef struct _OPTION_INFO
	{
		char	szOptionCode[eMAX_PROD_OPTION_CODE + 1];
		char	szOptionValue[eMAX_PROD_OPTION_VALUE + 1];
		char	szOptionName[eMAX_PROD_OPTION_NAME + 1];
	} OPTION_INFO, *LPOPTION_INFO;

	// 할인 정보 조회
	typedef struct _REQUEST_DISCOUNT_LIST
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
		char	szProdID[eMAX_PROD_ID + 1];
		int		nLevel;
	} REQUEST_DISCOUNT_LIST, *LPREQUEST_DISCOUNT_LIST;

	typedef struct _RESPONSE_DISCOUNT_LIST
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1]; // 전송 실패 or 수신 실패 or 조회 실패

		int				nPrice;
		DATA_HANDLE_EX	handle;
	} RESPONSE_DISCOUNT_LIST, *LPRESPONSE_DISCOUNT_LIST;

	typedef struct _DISCOUNT_INFO
	{
		char	szConditionValue[eMAX_CONDITION_VALUE + 1];
		int		nDiscountRate;
		int		nDiscountAmt;
		bool	bUseYN;
	} DISCOUNT_INFO, *LPDISCOUNT_INFO;

	// 태그 목록 조회
	typedef struct _REQUEST_TAG_LIST
	{
		char	szGameID[eMAX_GAME_ID + 1];
	} REQUEST_TAG_LIST, *LPREQUEST_TAG_LIST;

	typedef struct _RESPONSE_TAG_LIST
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1]; // 전송 실패 or 수신 실패 or 조회 실패

		DATA_HANDLE_EX	handle;
	} RESPONSE_TAG_LIST, *LPRESPONSE_TAG_LIST;

	typedef struct _TAG_INFO
	{
		char	szTagName[eMAX_TAG_NAME + 1];
	} TAG_INFO, *LPTAG_INFO;

	// 쿠폰 리스트 조회
	typedef struct _REQUEST_COUPON_LIST
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szUserIP[eMAX_USER_IP + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
		char	szProdID[eMAX_PROD_ID + 1];
		int		nLevel;
	} REQUEST_COUPON_LIST, *LPREQUEST_COUPON_LIST;

	typedef struct _RESPONSE_COUPON_LIST
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		DATA_HANDLE_EX	handle;
	} RESPONSE_COUPON_LIST, *LPRESPONSE_COUPON_LIST;

	typedef struct _COUPON_INFO
	{
		char	szCouponCode[eMAX_COUPON_CODE + 1];
		char	szCouponUseID[eMAX_COUPON_USE_ID + 1];
		char	szCouponName[eMAX_COUPON_NAME + 1];
		int		nDiscountRate;
		int		nDiscountAmt;
		char	szUseEndDate[eMAX_USE_END_DATE + 1];
	} COUPON_INFO, *LPCOUPON_INFO;

	// 스페셜 노출 상품 목록 조회
	typedef struct _REQUEST_SPECIAL_PRODUCT_LIST
	{
		char	szGameID[eMAX_GAME_ID + 1];
	} REQUEST_SPECIAL_PRODUCT_LIST, *LPREQUEST_SPECIAL_PRODUCT_LIST;

	typedef struct _RESPONSE_SPECIAL_PRODUCT_LIST
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		DATA_HANDLE_EX	handle;
	} RESPONSE_SPECIAL_PRODUCT_LIST, *LPRESPONSE_SPECIAL_PRODUCT_LIST;

	typedef struct _SPECIAL_PRODUCT_LIST_INFO
	{
		char	szExposeCode[eMAX_EXPOSE_CODE + 1];
		char	szExposeName[eMAX_EXPOSE_NAME + 1];
		char	szIconUrl[eMAX_IMG_URL + 1];
		char	szBannerUrl[eMAX_BANNER_URL + 1];
	} SPECIAL_PRODUCT_LIST_INFO, *LPSPECIAL_PRODUCT_LIST_INFO;

	// 스페셜 노출 상품 조회
	typedef struct _REQUEST_SPECIAL_PRODUCT_INFO
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szExposeCode[eMAX_EXPOSE_CODE + 1];
	} REQUEST_SPECIAL_PRODUCT_INFO, *LPREQUEST_SPECIAL_PRODUCT_INFO;

	typedef struct _RESPONSE_SPECIAL_PRODUCT_INFO
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1];

		DATA_HANDLE_EX	handle;
	} RESPONSE_SPECIAL_PRODUCT_INFO, *LPRESPONSE_SPECIAL_PRODUCT_INFO;

	typedef struct _SPECIAL_PRODUCT_INFO
	{
		char	szExposeCode[eMAX_EXPOSE_CODE + 1];
		char	szProdID[eMAX_PROD_ID + 1];
		char	szImgUrl[eMAX_IMG_URL + 1];
		int		nSortOrder;
	} SPECIAL_PRODUCT_INFO, *LPSPECIAL_PRODUCT_INFO;

	// 구매 요청
	typedef struct _USE_COUPON_INFO
	{
		char	szCouponCode[eMAX_COUPON_CODE + 1];
		char	szCouponUseID[eMAX_COUPON_USE_ID + 1];
	} USE_COUPON_INFO, *LPUSE_COUPON_INFO;

	typedef struct _USE_OPTION_INFO
	{
		char	szOptionCode[eMAX_PROD_OPTION_CODE + 1];
		char	szOptionValue[eMAX_PROD_OPTION_VALUE + 1];
	} USE_OPTION_INFO, *LPUSE_OPTION_INFO;

	typedef struct _REQUEST_PURCHASE_PROD
	{
		char			szGameID[eMAX_GAME_ID + 1];
		char			szProdID[eMAX_PROD_ID + 1];
		char			szUserID[eMAX_USER_ID + 1];
		char			szServerID[eMAX_SERVER_ID + 1];
		char			szCharID[eMAX_CHAR_ID + 1];
		char			szRecvUserID[eMAX_RECV_USER_ID + 1];
		char			szRecvServerID[eMAX_RECV_SERVER_ID + 1];
		char			szRecvCharID[eMAX_RECV_CHAR_ID + 1];
		int				nLevel;
		int				nCnt;
		char			szGiftMessage[eMAX_GIFT_MESSAGE + 1];
		char			szUserIP[eMAX_USER_IP + 1];
		char			szPassWD[eMAX_PASSWD + 1];

		DATA_HANDLE_EX	useCouponHandle;
		DATA_HANDLE_EX	useOptionHandle;
	} REQUEST_PURCHASE_PROD, *LPREQUEST_PURCHASE_PROD;

	typedef struct _RESPONSE_PURCHASE_PROD
	{
		bool	bIsOK; // STATE 항목 값이 0이면 true, 0이 아니면 false 해 주자. 물론 통신 실패의 경우도 마찬가지이다.
		char	szErrMsg[eMAX_ERROR_MSG + 1]; // 구매 결과 메시지

		int		nOrderSeq;
		int		nBuyPrice;
	} RESPONSE_PURCHASE_PROD, *LPRESPONSE_PURCHASE_PROD;

	// 한코인 조회
	typedef struct _REQUEST_HANCOIN_INFO
	{
		char	szUserID[eMAX_USER_ID + 1];
	} REQUEST_HANCOIN_INFO, *LPREQUEST_HANCOIN_INFO;

	typedef struct _RESPONSE_HANCOIN_INFO
	{
		bool	bIsOK; // HANCOIN 항목 값이 -1이면 false, -1이 아니면 true 해 주자. 물론 통신 실패의 경우도 마찬가지
		char	szErrMsg[eMAX_ERROR_MSG + 1]; // 전송 실패 or 수신 실패 or 조회 실패

		int		nHanCoin;
	} RESPONSE_HANCOIN_INFO, *LPRESPONSE_HANCOIN_INFO;

	// 게임머니 조회
	typedef struct _REQUEST_GAMEMONEY_INFO
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
	} REQUEST_GAMEMONEY_INFO, *LPREQUEST_GAMEMONEY_INFO;

	typedef struct _RESPONSE_GAMEMONEY_INFO
	{
		bool	bIsOK; // 조회 성공 시 true, 실패 시 false
		char	szErrMsg[eMAX_ERROR_MSG + 1]; // 조회 실패 시, Dorian connector logging 용도

		int		nGameMoney; // 조회가 성공한 경우 게임 머니값을 가짐
	} RESPONSE_GAMEMONEY_INFO, *LPRESPONSE_GAMEMONEY_INFO;

	// 게임머니 소진
	typedef struct _REQUEST_REDUCE_GAMEMONEY
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
		int		nGameMoney;
	} REQUEST_REDUCE_GAMEMONEY, *LPREQUEST_REDUCE_GAMEMONEY;

	typedef struct _RESPONSE_REDUCE_GAMEMONEY
	{
		bool	bIsOK; // 소진 성공 시 true, 실패 시 false
		char	szErrMsg[eMAX_ERROR_MSG + 1]; // 소진 결과 메시지

		int		nGameMoney;
	} RESPONSE_REDUCE_GAMEMONEY, *LPRESPONSE_REDUCE_GAMEMONEY;

	// 인벤토리 아이템 정보
	typedef struct _INVENTORY_ITEM_INFO
	{
		char	szItemID[eMAX_ITEM_ID + 1];
		int		nCnt;
		int		nUseCnt;
		int		nPeriod;
		int		nPossibleLevel;
		char	szValidTerm[eMAX_VALID_TERM + 1];
		char	chUseStatus;
		char	szOptionCode[eMAX_ITEM_OPTION_CODE + 1];
		char	szOptionValue[eMAX_ITEM_OPTION_VALUE + 1];
	} INVENTORY_ITEM_INFO, *LPINVENTORY_ITEM_INFO, SAVE_ITEM_INFO, *LPSAVE_ITEM_INFO;

	// 인벤토리 조회
	typedef struct _REQUEST_INVENTORY_INFO
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
	} REQUEST_INVENTORY_INFO, *LPREQUEST_INVENTORY_INFO;

	typedef struct _RESPONSE_INVENTORY_INFO
	{
		bool			bIsOK; // 조회 성공 시 true, 실패 시 false
		char			szErrMsg[eMAX_ERROR_MSG + 1]; // 조회 실패 시, Dorian connector logging 용도
		DATA_HANDLE_EX	handle;
	} RESPONSE_INVENTORY_INFO, *LPRESPONSE_INVENTORY_INFO;

	// 인벤토리 저장
	typedef struct _REQUEST_SAVE_INVENTORY
	{
		char			szGameID[eMAX_GAME_ID + 1];
		int				nOrderSeq;
		char			szUserID[eMAX_USER_ID + 1];
		char			szServerID[eMAX_SERVER_ID + 1];
		char			szCharID[eMAX_CHAR_ID + 1];
		char			szRecvUserID[eMAX_RECV_USER_ID + 1];
		char			szRecvServerID[eMAX_RECV_SERVER_ID + 1];
		char			szRecvCharID[eMAX_RECV_CHAR_ID + 1];
		char			szGiftMessage[eMAX_GIFT_MESSAGE + 1];
		int				nPrice;
		DATA_HANDLE_EX	handle;
	} REQUEST_SAVE_INVENTORY, *LPREQUEST_SAVE_INVENTORY;

	typedef struct _RESPONSE_SAVE_INVENTORY
	{
		bool	bIsOK; // 저장 성공 시 true, 실패 시 false
		char	szErrMsg[eMAX_ERROR_MSG + 1]; // 저장 결과 메시지
	} RESPONSE_SAVE_INVENTORY, *LPRESPONSE_SAVE_INVENTORY;

	// 캐릭터 조회
	typedef struct _REQUEST_CHARACTER_INFO
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	chSearchType;
		char	szSearchValue[eMAX_SEARCH_VALUE + 1];
	} REQUEST_CHARACTER_INFO, *LPREQUEST_CHARACTER_INFO;

	typedef struct _RESPONSE_CHARACTER_INFO
	{
		bool			bIsOK; // 조회 성공 시 true, 실패 시 false
		char			szErrMsg[eMAX_ERROR_MSG + 1]; // 조회 실패 시, Dorian connector logging 용도
		DATA_HANDLE_EX	handle;
	} RESPONSE_CHARACTER_INFO, *LPRESPONSE_CHARACTER_INFO;

	typedef struct _CHARACTER_INFO
	{
		char	szUserID[eMAX_USER_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
		int		nLevel;
	} CHARACTER_INFO, *LPCHARACTER_INFO;
	//--------------------------------------------------------------------

	//====================================================================
	// 스토리지 관련 인자 선언
	// 스토리지 신규 통지 정보
	typedef struct _STORAGE_NOTI_INFO
	{
		char	szRecvUserID[eMAX_RECV_USER_ID + 1];
		char	szRecvCharID[eMAX_CHAR_ID + 1];
		int		nItemCnt;
	} STORAGE_NOTI_INFO, *LPSTORAGE_NOTI_INFO;

	// 스토리지 조회
	typedef struct _REQUEST_SEARCH_STORAGE
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
		char	chSearchType;
		char	szSearchValue[eMAX_SEARCH_VALUE + 1];
	} REQUEST_SEARCH_STORAGE, *LPREQUEST_SEARCH_STORAGE;

	typedef struct _RESPONSE_SEARCH_STORAGE
	{
		bool			bIsOK;
		char			szErrMsg[eMAX_ERROR_MSG + 1]; // 전송 실패 or 수신 실패 or 조회 실패

		DATA_HANDLE_EX	handle;
	} RESPONSE_SEARCH_STORAGE, *LPRESPONSE_SEARCH_STORAGE;

	typedef struct _STORAGE_INFO
	{
		int		nStorageSeq;
		int		nOrderSeq;
		char	szItemID[eMAX_ITEM_ID + 1];
		char	szItemName[eMAX_ITEM_NAME + 1];
		bool	bBonusYN;
		char	szSenderID[eMAX_USER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
		char	szGiftMessage[eMAX_GIFT_MESSAGE + 1];
		int		nCnt;
		int		nRemainCnt;
		char	szValidTerm[eMAX_VALID_TERM + 1];
		int		nPossibleLevel;
		int		nUseCnt;
		int		nPeriod;
		char	szOptionCode[eMAX_ITEM_OPTION_CODE + 1];
		char	szOptionValue[eMAX_ITEM_OPTION_VALUE + 1];
		char	szUseStartDate[eMAX_DATE + 1];
		char	szValidDate[eMAX_DATE + 1];
		char	szBuyDate[eMAX_DATE + 1];
		char	szSetName[eMAX_SET_NAME + 1];
		char	szListImgUrl[eMAX_IMG_URL + 1];
		char	szIconImgUrl[eMAX_IMG_URL + 1];
		char	szDirection[eMAX_DIRECTION + 1];
		bool	bReadYN;
		bool	bNewYN;
	} STORAGE_INFO, *LPSTORAGE_INFO;

	// 스토리지 사용
	typedef struct _REQUEST_USE_STORAGE
	{
		char			szGameID[eMAX_GAME_ID + 1];
		char			szServerID[eMAX_SERVER_ID + 1];
		char			szUserID[eMAX_USER_ID + 1];
		char			szCharID[eMAX_CHAR_ID + 1];
		DATA_HANDLE_EX	handle;
	} REQUEST_USE_STORAGE, *LPREQUEST_USE_STORAGE;

	typedef struct _RESPONSE_USE_STORAGE
	{
		bool	bIsOK; // STATE 항목 값이 0이면 true, 0이 아니면 false 해 주자. 물론 통신 실패의 경우도 마찬가지
		char	szErrMsg[eMAX_ERROR_MSG + 1]; // 전송 실패 or 수신 실패 or 이동 결과 메시지
	} RESPONSE_USE_STORAGE, *LPRESPONSE_USE_STORAGE;

	typedef struct _USE_STORAGE_INFO
	{
		int		nStorageSeq;
		int		nCnt;
		char	szItemID[eMAX_ITEM_ID + 1];
	} USE_STORAGE_INFO, *LPUSE_STORAGE_INFO;

	// 스토리지 읽음 확인
	typedef struct _REQUEST_READ_STORAGE
	{
		char	szGameID[eMAX_GAME_ID + 1];
		char	szServerID[eMAX_SERVER_ID + 1];
		char	szUserID[eMAX_USER_ID + 1];
		char	szCharID[eMAX_CHAR_ID + 1];
		int		nStorageSeq;
		bool	bDelYN;
	} REQUEST_READ_STORAGE, *LPREQUEST_READ_STORAGE;

	typedef struct _RESPONSE_READ_STORAGE
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_READ_STORAGE, *LPRESPONSE_READ_STORAGE;

	// 스토리지 사용 승인/삭제
	typedef struct _REQUEST_CONFIRM_USE_STORAGE
	{
		char			szGameID[eMAX_GAME_ID + 1];
		char			szServerID[eMAX_SERVER_ID + 1];
		char			szUserID[eMAX_USER_ID + 1];
		char			szCharID[eMAX_CHAR_ID + 1];
		DATA_HANDLE_EX	handle;
	} REQUEST_CONFIRM_USE_STORAGE, *LPREQUEST_CONFIRM_USE_STORAGE, REQUEST_CANCEL_USE_STORAGE, *LPREQUEST_CANCEL_USE_STORAGE;

	typedef struct _RESPONSE_CONFIRM_USE_STORAGE
	{
		bool	bIsOK;
		char	szErrMsg[eMAX_ERROR_MSG + 1];
	} RESPONSE_CONFIRM_USE_STORAGE, *LPRESPONSE_CONFIRM_USE_STORAGE, RESPONSE_CANCEL_USE_STORAGE, *LPRESPONSE_CANCEL_USE_STORAGE;

	typedef struct _CONFIRM_USE_STORAGE_INFO
	{
		int		nStorageSeq;
	} CONFIRM_USE_STORAGE_INFO, *LPCONFIRM_USE_STORAGE_INFO, CANCEL_USE_STORAGE_INFO, *LPCANCEL_USE_STORAGE_INFO;
	//--------------------------------------------------------------------

	//====================================================================
	// 함수 포인터 타입 정의
	// 상품제 이벤트 통지
	typedef int (*FP_NotifyBillingEvent) (const LPBILLING_NOTI_INFO pInfo);

	// 스토리지 신규 통지
	typedef int (*FP_NotifyStorageInfo) (const LPSTORAGE_NOTI_INFO pInfo);

	// 게임머니 조회
	typedef void (*FP_SearchGameMoney) (const LPREQUEST_GAMEMONEY_INFO pReq, LPRESPONSE_GAMEMONEY_INFO pRes);

	// 게임머니 소진
	typedef void (*FP_ReduceGameMoney) (const LPREQUEST_REDUCE_GAMEMONEY pReq, LPRESPONSE_REDUCE_GAMEMONEY pRes);

	// 인벤토리 조회
	typedef void (*FP_SearchInventory) (const LPREQUEST_INVENTORY_INFO pReq, LPRESPONSE_INVENTORY_INFO pRes);

	// 인벤토리 저장
	typedef void (*FP_SaveInventory) (const LPREQUEST_SAVE_INVENTORY pReq, LPRESPONSE_SAVE_INVENTORY pRes);

	// 캐릭터 조회
	typedef void (*FP_SearchCharacter) (const LPREQUEST_CHARACTER_INFO pReq, LPRESPONSE_CHARACTER_INFO pRes);

	typedef struct _FUNCTION_POINTER_INFO
	{
		FP_NotifyBillingEvent	fpNotiBillInfo;
		FP_NotifyStorageInfo	fpNotiStorageInfo;
		FP_SearchGameMoney		fpSearchGameMoney;
		FP_ReduceGameMoney		fpReduceGameMoney;
		FP_SearchInventory		fpSearchInventory;
		FP_SaveInventory		fpSaveInventory;
		FP_SearchCharacter		fpSearchCharacter;
	} FUNCTION_POINTER_INFO, *LPFUNCTION_POINTER_INFO;
	//--------------------------------------------------------------------

#if defined(__cplusplus)
	extern "C" {
#endif
	//====================================================================
	// 모듈 초기화, 해제 관련 API
	DORIAN_EXTERN bool
		__stdcall InitModule(IN GAMESERVER_INFO gameServerInfo, IN FUNCTION_POINTER_INFO fpPointers, OUT char* errorMsg, IN int bufferSize);

	DORIAN_EXTERN void
		__stdcall UninitModule();
	//--------------------------------------------------------------------


	//====================================================================
	// 정액/정량 상품제 관련 API
	DORIAN_EXTERN void
		__stdcall Checkin(IN REQUEST_CHECKIN *pRequest, OUT RESPONSE_CHECKIN *pResponse);

	DORIAN_EXTERN void
		__stdcall CheckinEx(IN REQUEST_CHECKIN *pRequest, OUT RESPONSE_CHECKIN_EX *pResponse);

	DORIAN_EXTERN void
		__stdcall Checkout(IN REQUEST_CHECKOUT* pRequest, OUT RESPONSE_CHECKOUT *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryBillingInfo(IN REQUEST_BILL_INFO *pRequest, OUT RESPONSE_BILL_INFO *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryBillingInfoEx(IN REQUEST_BILL_INFO *pRequest, OUT RESPONSE_BILL_INFO_EX *pResponse);

	DORIAN_EXTERN void
		__stdcall UserMoveTo(IN REQUEST_USERMOVE_TO *pRequest, OUT RESPONSE_USERMOVE_TO *pResponse);

	DORIAN_EXTERN void
		__stdcall UserMoveFrom(IN REQUEST_USERMOVE_FROM *pRequest, IN BILL_INFO_FOR_USER_MOVE *pBillInfo, OUT RESPONSE_USERMOVE_FROM *pResponse);

	DORIAN_EXTERN bool
		__stdcall GetBillingInfoForUserMove(IN const char* pszKey, OUT BILL_INFO_FOR_USER_MOVE *pBillInfo);

	DORIAN_EXTERN const char*
		__stdcall GetBillingName(IN const int nPID);

	DORIAN_EXTERN const char*
		__stdcall GetEventName(IN const int nEventCode);

	// for debug
	//////////////////////////////////////////////////////
	DORIAN_EXTERN void
		__stdcall Charge(OUT RESPONSE_CHARGE *pResponse);
	//////////////////////////////////////////////////////
	// for debug
	//--------------------------------------------------------------------

	//====================================================================
	// 아이템 관련 API
	DORIAN_EXTERN void
		__stdcall QueryMenuList(IN REQUEST_MENU_LIST *pRequest, OUT RESPONSE_MENU_LIST *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryProductList(IN REQUEST_PROD_LIST *pRequest, OUT RESPONSE_PROD_LIST *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryProductDetailInfo(IN REQUEST_PROD_INFO *pRequest, OUT RESPONSE_PROD_INFO *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryDiscountList(IN REQUEST_DISCOUNT_LIST *pRequest, OUT RESPONSE_DISCOUNT_LIST *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryTagList(IN REQUEST_TAG_LIST *pRequest, OUT RESPONSE_TAG_LIST *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryHanCoinInfo(IN REQUEST_HANCOIN_INFO *pRequest, OUT RESPONSE_HANCOIN_INFO *pResponse);

	DORIAN_EXTERN void
		__stdcall QueryCouponList(IN REQUEST_COUPON_LIST *pRequest, OUT RESPONSE_COUPON_LIST *pResponse);

	DORIAN_EXTERN void
		__stdcall QuerySpecialProductList(IN REQUEST_SPECIAL_PRODUCT_LIST *pRequest, OUT RESPONSE_SPECIAL_PRODUCT_LIST *pResponse);

	DORIAN_EXTERN void
		__stdcall QuerySpecialProductInfo(IN REQUEST_SPECIAL_PRODUCT_INFO *pRequest, OUT RESPONSE_SPECIAL_PRODUCT_INFO *pResponse);

	DORIAN_EXTERN void
		__stdcall PurchaseProduct(IN REQUEST_PURCHASE_PROD *pRequest, OUT RESPONSE_PURCHASE_PROD *pResponse);
	//--------------------------------------------------------------------

	//====================================================================
	// Storage 관련 API
	DORIAN_EXTERN void
		__stdcall QueryStorageInfo(IN REQUEST_SEARCH_STORAGE *pRequest, OUT RESPONSE_SEARCH_STORAGE *pResponse);

	DORIAN_EXTERN void
		__stdcall ReadStorageInfo(IN REQUEST_READ_STORAGE *pRequest, OUT RESPONSE_READ_STORAGE *pResponse);

	DORIAN_EXTERN void
		__stdcall StorageToInventory(IN REQUEST_USE_STORAGE *pRequest, OUT RESPONSE_USE_STORAGE *pResponse);

	DORIAN_EXTERN void
		__stdcall ConfirmStorageToInventory(IN REQUEST_CONFIRM_USE_STORAGE *pRequest, OUT RESPONSE_CONFIRM_USE_STORAGE *pResponse);

	DORIAN_EXTERN void
		__stdcall CancelStorageToInventory(IN REQUEST_CANCEL_USE_STORAGE *pRequest, OUT RESPONSE_CANCEL_USE_STORAGE *pResponse);
	//--------------------------------------------------------------------

	//====================================================================
	// Data handle 관련 API
	DORIAN_EXTERN size_t
		__stdcall GetDataSize(IN DATA_HANDLE_EX handle, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetBillingData(IN DATA_HANDLE_EX handle, IN size_t index, OUT BILL_INFO* pBillInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetMenuData(IN DATA_HANDLE_EX handle, IN size_t index, OUT MENU_INFO* pMenuInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetProductData(IN DATA_HANDLE_EX handle, IN size_t index, OUT PROD_LIST_INFO* pProductListInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetChildProductData(IN DATA_HANDLE_EX handle, IN size_t index, OUT CHILDPROD_INFO* pChildProductInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetConditionData(IN DATA_HANDLE_EX handle, IN size_t index, OUT CONDITION_INFO* pConditionInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetOptionData(IN DATA_HANDLE_EX handle, IN size_t index, OUT OPTION_INFO* pOptionInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetDiscountData(IN DATA_HANDLE_EX handle, IN size_t index, OUT DISCOUNT_INFO* pDiscountInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetTagData(IN DATA_HANDLE_EX handle, IN size_t index, OUT TAG_INFO* pTagInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetCouponData(IN DATA_HANDLE_EX handle, IN size_t index, OUT COUPON_INFO* pCouponListInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetSpecialProductData(IN DATA_HANDLE_EX handle, IN size_t index, OUT SPECIAL_PRODUCT_LIST_INFO* pSpecialProductListInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetSpecialProductDetailData(IN DATA_HANDLE_EX handle, IN size_t index, OUT SPECIAL_PRODUCT_INFO* pSpecialProductInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetStorageItemData(IN DATA_HANDLE_EX handle, IN size_t index, OUT STORAGE_INFO* pStorageInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall GetSaveItemData(IN DATA_HANDLE_EX handle, IN size_t index, OUT SAVE_ITEM_INFO* pSaveItemInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN DATA_HANDLE_EX
		__stdcall GetHandleForUseCouponInfo(OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN DATA_HANDLE_EX
		__stdcall GetHandleForUseOptionInfo(OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN DATA_HANDLE_EX
		__stdcall GetHandleForUseStorageItemInfo(OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN DATA_HANDLE_EX
		__stdcall GetHandleForConfirmUseStorageItemInfo(OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN DATA_HANDLE_EX
		__stdcall GetHandleForCancelUseStorageItemInfo(OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall SetInventoryItemData(IN DATA_HANDLE_EX handle, IN INVENTORY_ITEM_INFO inventoryItemInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall SetCharacterData(IN DATA_HANDLE_EX handle, IN CHARACTER_INFO characterInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall SetUseCouponData(IN DATA_HANDLE_EX handle, IN USE_COUPON_INFO useCouponInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall SetUseOptionData(IN DATA_HANDLE_EX handle, IN USE_OPTION_INFO useOptionInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall SetUseStorageItemData(IN DATA_HANDLE_EX handle, IN USE_STORAGE_INFO useStorageItemInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall SetConfirmUseStorageItemData(IN DATA_HANDLE_EX handle, IN CONFIRM_USE_STORAGE_INFO confirmUseStorageItemInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN bool
		__stdcall SetCancelUseStorageItemData(IN DATA_HANDLE_EX handle, IN CANCEL_USE_STORAGE_INFO cancelUseStorageItemInfo, OUT char* errorMsg, IN size_t bufferSize);

	DORIAN_EXTERN void
		__stdcall DestroyHandle(IN DATA_HANDLE_EX handle);
	//--------------------------------------------------------------------
#if defined(__cplusplus)
	}
#endif

}//namespace Dorian

#endif // __DORIAN_CONNECTOR_H__
