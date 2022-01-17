#include "LibClientSession.h"
#include <time.h>


CLibClientSession::CLibClientSession( void )
{
}

CLibClientSession::~CLibClientSession( void )
{
}

void CLibClientSession::OnNetConnect( bool success, DWORD error )
{
	if( success )
	{
		WriteLog( _T( "OnConnect succeed" ) );
	}
	else
	{
		WriteLog( _T( "OnConnect fali" ) );
	}
}

void CLibClientSession::OnNetSend( int size )
{
	//WriteLog("OnSend");
}

void CLibClientSession::OnNetClose( DWORD error )
{
	WriteLog( _T( "OnClose" ) );
}

void CLibClientSession::OnUserStatus( DWORD dwAccountGUID, long dwBillingGUID, DWORD RealEndDate, DWORD EndDate, double dRestPoint, double dRestTime, short nDeductType, short nAccessCheck, short nResultCode )
{
	struct tm tRealEndDate;
	struct tm tEndDate;
	
	tRealEndDate.tm_hour = 0;
	tRealEndDate.tm_isdst = 0;
	tRealEndDate.tm_mday = 0;
	tRealEndDate.tm_min = 0;
	tRealEndDate.tm_mon = 0;
	tRealEndDate.tm_sec = 0;
	tRealEndDate.tm_wday = 0;
	tRealEndDate.tm_yday = 0;
	tRealEndDate.tm_year = 0;

	tEndDate.tm_hour = 0;
	tEndDate.tm_isdst = 0;
	tEndDate.tm_mday = 0;
	tEndDate.tm_min = 0;
	tEndDate.tm_mon = 0;
	tEndDate.tm_sec = 0;
	tEndDate.tm_wday = 0;
	tEndDate.tm_yday = 0;
	tEndDate.tm_year = 0;

	_localtime32_s( &tRealEndDate, ( __time32_t* )&RealEndDate );
	_localtime32_s( &tEndDate, ( __time32_t* )&EndDate );

	char strEndTime[ 256 ] = { 0, };
	sprintf_s( strEndTime, sizeof( char ) * 256, "%d-%d-%d %d:%d:%d", tEndDate.tm_year + 1900, tEndDate.tm_mon + 1, tEndDate.tm_mday, tEndDate.tm_hour, tEndDate.tm_min, tEndDate.tm_sec );

	char strRealEndTime[ 256 ] = { 0, };
	sprintf_s( strRealEndTime, sizeof( char ) * 256, "%d-%d-%d %d:%d:%d", tRealEndDate.tm_year + 1900, tRealEndDate.tm_mon + 1, tRealEndDate.tm_mday, tRealEndDate.tm_hour, tRealEndDate.tm_min, tRealEndDate.tm_sec );

	char strMsg[ 1024 ] = { 0, };
	sprintf_s( strMsg, sizeof( char ) * 1024, "개인계정: %d\r\n빌링계정: %d\r\n차감유형: %d\r\n접근체크결과: %d\r\n처리결과코드: %d\r\n잔여포인트: %f\r\n잔여시간: %f\r\n종료시간: %s \r\n진짜종료시간: %s\r\n", dwAccountGUID, dwBillingGUID, nDeductType, nAccessCheck, nResultCode, dRestPoint, dRestTime, strEndTime, strRealEndTime );

	WriteLog( strMsg );
}

void CLibClientSession::OnInquireMultiUser( long AccountID, long RoomGUID, long Result )
{
	WriteFormattedLog( "계정 순번: %d, 피시방 GUID: %d, 결과: %d\r\n", AccountID, RoomGUID, Result );
}

void CLibClientSession::OnInquirePersonDeduct( long AccountID, long GameCode, long ResultCode )
{
	WriteFormattedLog( "AccountID: %d, GameCode: %d, ResultCode: %d\r\n", AccountID, GameCode, ResultCode );
}

void CLibClientSession::OnInquirePCRoomPoint( long AccountID, long RoomGUID, long GameCode, long ResultCode )
{
	WriteFormattedLog( "AccountID: %d, RoomGUID: %d, GameCode: %d, ResultCode: %d\r\n", AccountID, RoomGUID, GameCode, ResultCode );
}

void CLibClientSession::OnCheckLoginUser( long AccountID, long BillingGUID )
{
	WriteFormattedLog( "AccountID: %d, BillingGUID: %d\r\n", AccountID, BillingGUID );
}

void CLibClientSession::WriteLog( char* szMsg )
{
	TCHAR szModule[ MAX_PATH ] = { 0 };
	TCHAR szFilePath[ MAX_PATH ] = { 0 };

	::GetModuleFileName( NULL, szModule, MAX_PATH );
	*( _tcsrchr( szModule, '\\' ) ) = 0;

	StringCchPrintf( szFilePath, STRSAFE_MAX_CCH , _T( "%s\\Billing.log" ), szModule );
	WriteFileLog( szMsg, szFilePath );
}
