#include "AgsmBillingJapan.h"
#include "HanBillingForSvr.h"
#include "AuGameEnv.h"
#include <stdio.h>

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

AgsmBillingJapan::AgsmBillingJapan( void )
:	m_orderSequence( 0 ),
	m_isDebugTest( false )
{
	memset( m_szBillNo, 0, sizeof(m_szBillNo));
}

bool AgsmBillingJapan::InitBillingModule()
{
#ifdef _AREA_JAPAN_
	int errCode = 0;
	GetGameEnv().InitEnvironment();

	// 실행 환경
	m_isDebugTest = GetGameEnv().IsDebugTest();
	if(GetGameEnv().IsAlpha())
		errCode = m_isDebugTest ? HAN_BILLING_OK : HanBillingInit( "J_ARCHLORD", SERVICE_JPN|SERVICE_ALPHA|SERVICE_HANGAME );
	else
		errCode = m_isDebugTest ? HAN_BILLING_OK : HanBillingInit( "J_ARCHLORD", SERVICE_JPN|SERVICE_REAL|SERVICE_HANGAME );

	if ( HAN_BILLING_OK != errCode )
		PrintError( errCode );

	return ( HAN_BILLING_OK == errCode );
#endif

	return true;
}
const char* AgsmBillingJapan::GetBillingNumber( const char* accountId )
{
#ifdef _AREA_JAPAN_
	char retMsg[256] = {0, };

	int errCode = m_isDebugTest ? HAN_BILLING_OK : HanBillingGetBillNo( accountId, m_szBillNo, retMsg, sizeof(retMsg));
	if ( HAN_BILLING_OK != errCode )
	{
		printf( retMsg );
		PrintError( errCode );
	}
#endif
	return m_szBillNo;
}
bool AgsmBillingJapan::GetBalance(const char* account, const char* billingNum, int& balance)
{
#ifdef _AREA_JAPAN_
	char retMsg[256] = {0, };
	
	HANBILL_BALANCEINFO hanBillInfo;
	memset( &hanBillInfo, 0, sizeof(hanBillInfo) );

	int errCode = m_isDebugTest ? HAN_BILLING_OK : HanBillingBalanceInquire(account, billingNum, retMsg, sizeof(retMsg), &hanBillInfo);
	if ( HAN_BILLING_OK == errCode )
	{
		balance = hanBillInfo.nTotalBalance;
		printf( "[Billing] ID: %s, Balance: %d\n", account, balance );
		//printf( "[Billing] ID: %s, Balance: %d\n", account, balance );
		//printf( "[Billing] ID: %s, Balance: %d\n", account, balance );
		//printf( "[Billing] ID: %s, Balance: %d\n", account, balance );
		if ( m_isDebugTest ) balance = 50000;
	}
	else
	{
		PrintError( errCode );
		printf(retMsg);
	}

	return ( HAN_BILLING_OK == errCode );
#endif

	return true;
}

bool AgsmBillingJapan::BuyItem(const char* account,		// Account
							   const char* billingNum,	// User billing Number
							   const char* itemCode,	// Item Code
							   const char* itemName,	// Item Name
							   int price,				// Item Price
							   const char* clientIp,	// Client IP
							   char* orderString)		// Order String
{
#ifdef _AREA_JAPAN_
	char retMsg[256] = {0, };
	_snprintf_s( orderString, 21, _TRUNCATE, "%s%06d", m_order.c_str(), InterlockedIncrement( &m_orderSequence ) );

	HANBILL_BUYINFO buyInfo;
	memset( &buyInfo, 0, sizeof(buyInfo) );

	int errCode = m_isDebugTest
				  ? HAN_BILLING_OK
				  : HanBillingBuy( account,						// 계정 이름
								   billingNum,					// 결제 번호
								   retMsg, sizeof(retMsg),		// 오류 메시지, 크기
								   itemCode,					// 아이템 코드
								   itemName,					// 아이템 이름
								   1,							// 개수, 아크로드엔 다중 구매 없음
								   price,						// 구매 가격
								   clientIp,					// 클라이언트 IP
								   orderString,					// 고유한 주문 스트링
								   &buyInfo );					// 구매 결과
	if ( HAN_BILLING_OK != errCode )
	{
		PrintError( errCode );
		printf( retMsg );
	}

	return ( HAN_BILLING_OK == errCode );
#endif

	return true;
}


void AgsmBillingJapan::PrintError( int errCode )
{
#ifdef _AREA_JAPAN_
	switch( errCode )
	{
	case HAN_BILLING_GAMECODE_NOTEXIST:
		printf("Billing Module: GAMECODE_NOTEXIST\n");
		break;
	case HAN_BILLING_USERNO_NOTEXIST:
		printf("Billing Module: USERNO_NOEXIST\n");
		break;
	case HAN_BILLING_CMD_NOTEXIST:
		printf("Billing Module: CMD_NOTEXIST\n");
		break;
	case HAN_BILLING_ORDNO_NOTEXIST:
		printf("Billing Module: ORDNO_NOTEXIST\n");
		break;
	case HAN_BILLING_SCD_NOTEXIST:
		printf("Billing Module: SCD_NOTEXIST\n");
		break;
	case HAN_BILLING_SNAME_NOTEXIST:
		printf("Billing Module: SNAME_NOTEXIST\n");
		break;
	case HAN_BILLING_QTY_NOTEXIST:
		printf("Billing Module: QTY_NOTEXIST\n");
		break;
	case HAN_BILLING_AMT_NOTEXIST:
		printf("Billing Module: AMT_NOTEXIST\n");
		break;
	case HAN_BILLING_IP_NOTEXIST:
		printf("Billing Module: IP_NOTEXIST\n");
		break;
	case HAN_BILLING_USERNO_INVALID:
		printf("Billing Module: USERNO_INVALID\n");
		break;
	case HAN_BILLING_BUY_FAIL:
		printf("Billing Module: BUY_FAIL\n");
		break;
	case HAN_BILLING_GAMECODE_INVALID:
		printf("Billing Module: GAMECODE_INVALID\n");
		break;
	case HAN_BILLING_BALANCE_INSUFFICIENT:
		printf("Billing Module: BALANCE_INSUFFICIENT\n");
		break;
	case HAN_BILLING_CMD_UNEXPECTED:
		printf("Billing Module: CMD_UNEXPECTED\n");
		break;
	case HAN_BILLING_DB_CONNECTFAIL:
		printf("Billing Module: DB_CONNECTFAIL\n");
		break;
	case HAN_BILLING_SVR_UNEXPECTED:
		printf("Billing Module: SVR_UNEXPECTED\n");
		break;
	case HAN_BILLING_ARGUMENT_INVALID:
		printf("Billing Module: ARGUMENT_INVALID\n");
		break;
	case HAN_BILLING_INITED_ALREADY:
		printf("Billing Module: INITED_ALREADY\n");
		break;
	case HAN_BILLING_INITED_NOT:
		printf("Billing Module: INITED_NOT\n");
		break;
	case HAN_BILLING_INITED_FAIL:
		printf("Billing Module: INITED_FAIL\n");
		break;
	case HAN_BILLING_INITED_FAILBALANCE:
		printf("Billing Module: INITED_FAILBALANCE\n");
		break;
	case HAN_BILLING_INITED_FAILBUY:
		printf("Billing Module: INITED_FAILBUY\n");
		break;
	case HAN_BILLING_INITED_FAILCONFIRM:
		printf("Billing Module: INITED_FAILCONFIRM\n");
		break;
	case HAN_BILLING_GETCONN_FAILBALANCE:
		printf("Billing Module: GETCONN_FAILBALANCE\n");
		break;
	case HAN_BILLING_GETCONN_FAILBUY:
		printf("Billing Module: GETCONN_FAILBUY\n");
		break;
	case HAN_BILLING_GETCONN_FAILCONFIRM:
		printf("Billing Module: GETCONN_FAILCONFIRM\n");
		break;
	case HAN_BILLING_OPENREQ_FAIL:
		printf("Billing Module: OPENREQ_FAIL\n");
		break;
	case HAN_BILLING_SENDREQ_FAIL:
		printf("Billing Module: SENDREQ_FAIL\n");
		break;
	case HAN_BILLING_RECVRESULT_FAIL:
		printf("Billing Module: RECVRESULT_FAIL\n");
		break;
	case HAN_BILLING_RECVRESULT_INVALID:
		printf("Billing Module: RECVRESULT_INVALID\n");
		break;
	case HAN_BILLING_DLL_UNEXPECTED:
		printf("Billing Module: DLL_UNEXPECTED\n");
		break;
	default:
		printf("Billing Module: Unknown Error Code(%d)\n", errCode);
	}
#endif
}