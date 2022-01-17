#include "CWebzenBilling.h"




CWebzenBilling::CWebzenBilling( void )
{
	memset( m_strServerAddress, 0, sizeof( char ) * 64 );
	m_nServerPort = 0;

	m_nGameServerType = 1;

	m_bIsStarted = FALSE;
	m_bIsLogined = FALSE;

	m_dwAccountGUID = -1;
	m_nBillingGUID = -1;

	m_dwEndDate = 0;
	m_dwEndDateReal = 0;

	m_dRemainPoint = 0;
	m_dRemainTime = 0;

	m_nUserType = 0;
}

CWebzenBilling::~CWebzenBilling( void )
{
}

BOOL CWebzenBilling::OnCreateSession( void )
{
	if( m_bIsStarted )
	{
		WriteLog( "-- Webzen Billing -- : Webzen Billing is already started\n" );
		return TRUE;
	}

	if( !CreateSession( fnErrorHandler ) )
	{
		WriteLog( "-- Webzen Billing -- : Failed to CBillEventHandler::CreateSession()!\n" );
		return FALSE;
	}

	m_bIsStarted = TRUE;
	return TRUE;
}

BOOL CWebzenBilling::OnWebzenBillingConnect( char* pServerAddress, int nPort )
{
	if( !pServerAddress || strlen( pServerAddress ) <= 0 )
	{
		WriteLog( "-- Webzen Billing -- : Billing Server address is missing\n" );
		return FALSE;
	}

	memset( m_strServerAddress, 0, sizeof( char ) * 64 );
	strcpy_s( m_strServerAddress, sizeof( char ) * 64, pServerAddress );
	m_nServerPort = nPort;

	Connect( m_strServerAddress, m_nServerPort );
	return TRUE;
}

BOOL CWebzenBilling::OnWebzenBillingDisConnect( void )
{
	WriteLog( "-- Webzen Billing -- : Request disconnect to Billing Server..." );
	SetKill();

	m_bIsStarted = FALSE;
	return TRUE;
}

BOOL CWebzenBilling::OnWebzenBillingLogin( DWORD dwAccountID, long nGameCode, DWORD dwUserIP, DWORD dwPCBangID )
{
	if( !UserLogin( dwAccountID, dwUserIP, dwPCBangID, nGameCode, m_nGameServerType ) )
	{
		WriteFormattedLog( "-- Webzen Billing -- : Failed to login Billing Server, AccountID = %d, PCBangCode = %d\n", dwAccountID, dwUserIP );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenBilling::OnWebzenBillingLogout( void )
{
	if( !UserLogout( m_nBillingGUID ) )
	{
		BOOL bIsSuccess = FALSE;
		for( int nTryCount = 0 ; nTryCount < 20 ; nTryCount++ )
		{
			_sleep( 100000 );
			if( UserLogout( m_nBillingGUID ) )
			{
				bIsSuccess = TRUE;
				break;
			}
		}

		if( !bIsSuccess )
		{
			WriteFormattedLog( "-- Webzen Billing -- : Failed to logout Billing Server, Try Kill Connection.. AccountID = %d, BillingGUID = %d\n", m_dwAccountGUID, m_nBillingGUID );
			return OnWebzenBillingDisConnect();
		}
	}

	return TRUE;
}

BOOL CWebzenBilling::OnRequestUserInfo( void )
{
	if( !InquireUser( m_nBillingGUID ) )
	{
		WriteFormattedLog( "-- Webzen Billing -- : Failed to request user info, AccountID = %d, BillingGUID = %d\n", m_dwAccountGUID, m_nBillingGUID );
		return FALSE;
	}

	return TRUE;
}

BOOL CWebzenBilling::OnCheckLoginStatus( void )
{
	OnCheckLoginUser( m_dwAccountGUID, m_nBillingGUID );
	return TRUE;
}

void CWebzenBilling::OnNetConnect( bool bIsSuccess, DWORD dwError )
{
	if( !bIsSuccess )
	{
		WriteFormattedLog( "-- Webzen Billing -- : Failed to connect Billing Server, IP = %s, Port = %d, ErrorCode = %d\n", m_strServerAddress, m_nServerPort, dwError );
		return;
	}

	WriteLog( "-- Webzen Billing -- : OnConnect Succeed." );
}

void CWebzenBilling::OnNetClose( DWORD dwError )
{
	WriteLog( "-- Webzen Billing -- : Connection refused." );

	if( dwError > 0 )
	{
		WriteFormattedLog( "-- Webzen Billing -- : Connection refused by Error, ErrorCode = %d\n", dwError );
	}
}

void CWebzenBilling::OnNetSend( int nSendSize )
{
	WriteFormattedLog( "-- Webzen Billing -- : Complete to Send Packet to Billing Server, Send Size = %d\n", nSendSize );
}

void CWebzenBilling::OnUserStatus( DWORD dwAccountGUID, long dwBillingGUID, DWORD RealEndDate, DWORD EndDate, double dRestPoint, double dRestTime, short nDeductType, short nAccessCheck, short nResultCode )
{
	if( nResultCode != 0 )
	{
		_NotifyMessage( BillingNotice_Login, nResultCode );
	}

	if( nAccessCheck != 0 )
	{
		_NotifyMessage( BillingNotice_AccountCheck, nAccessCheck );
	}

	m_dwAccountGUID = dwAccountGUID;
	m_nBillingGUID = dwBillingGUID;

	m_dwEndDate = EndDate;
	m_dwEndDateReal = RealEndDate;

	m_dRemainPoint = dRestPoint;
	m_dRemainTime = dRestTime;

	m_nUserType = nDeductType;
	_NotifyMessage( BillingNotice_AccountType, m_nUserType );

	if( nResultCode == 0 && nAccessCheck == 0 && nDeductType != 0 )
	{
		if( !m_bIsLogined )
		{
			WriteLog( "-- Webzen Billing -- : Login Success." );
		}

		m_bIsLogined = TRUE;
	}
	else
	{
		if( m_bIsLogined )
		{
			WriteLog( "-- Webzen Billing -- : Log out." );
		}

		m_bIsLogined = FALSE;
	}
}

void CWebzenBilling::fnErrorHandler( DWORD dwLastError, TCHAR* pErrorMsg )
{
}

void CWebzenBilling::_NotifyMessage( eBillingNoticeType eType, short nNoticeCode )
{
	switch( eType )
	{
	case BillingNotice_Login :
		{
			switch( nNoticeCode )
			{
			case -1 :	WriteFormattedLog( "-- Webzen Billing -- : Login Failed, User Account Type UnKnown." );		break;		// 유저의 정액형태를 알수 없음
			case -2 :	WriteFormattedLog( "-- Webzen Billing -- : Login Failed, DB Query Fail." );					break;		// DB 작업 실패
			}
		}
		break;

	case BillingNotice_AccountCheck :
		{
			switch( nNoticeCode )
			{
			case 1 :	WriteFormattedLog( "-- Webzen Billing -- : Account Invalid, Have too Many products." );		break;		// 정액상품 접근제한 초과
			case 2 :	WriteFormattedLog( "-- Webzen Billing -- : Account Invalid, Cannot use this time." );		break;		// 정액 사용 불가한 시간대임
			}
		}
		break;

	case BillingNotice_AccountType :
		{
			switch( nNoticeCode )
			{
			case 1 :	WriteFormattedLog( "-- Webzen Billing -- : 개인 정액 사용자예욤." );				break;
			case 2 :	WriteFormattedLog( "-- Webzen Billing -- : PC방 정액 사용자예욤." );				break;
			case 3 :	WriteFormattedLog( "-- Webzen Billing -- : PC방 정량 사용자예욤." );				break;
			case 4 :	WriteFormattedLog( "-- Webzen Billing -- : 개인 정량 사용자예욤." );				break;
			case 5 :	WriteFormattedLog( "-- Webzen Billing -- : 개인 컬러요금제 사용자예욤." );			break;
			case 6 :	WriteFormattedLog( "-- Webzen Billing -- : PC방 컬러요금제 사용자예욤." );			break;
			default :	WriteFormattedLog( "-- Webzen Billing -- : 사용가능한 상품이 없는 사용자예욤." );	break;
			}
		}
		break;
	}
}

