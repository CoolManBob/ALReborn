#include "FCMsdkBase.h"
#include "AgsmGameholic.h"
#include "AgsmCharacter.h"

// -========================================================================-

#ifdef WIN64
#pragma comment(lib, "fcmsdk_64.lib")
#else
#pragma comment(lib, "fcmsdk_32.lib")
#endif

// -========================================================================-

namespace 
{
	int GetAddictStatus( int iOnlineMinutes, int iOfflineMinutes )
	{
		if ( iOnlineMinutes < 180 )
			return AS_GREEN;
		else if ( iOnlineMinutes < 300 )
			return AS_YELLOW;
		else
			return AS_RED;
		return 0;
	}
}

// -========================================================================-

AgpmCharacter* AgsmGameholic::m_pAgpmCharacter = 0;
AgsmCharacter* AgsmGameholic::m_pAgsmCharacter = 0;

AgsmGameholic g_agsmGameholic;

// -========================================================================-


AgsmGameholic::AgsmGameholic()
:	m_pFcm( 0 )
{
}

AgsmGameholic::~AgsmGameholic()
{
}

//CFCMSDKBase* psdk = CFCMSDKBase::GetInInstance();

bool AgsmGameholic::Initialize( AgpmCharacter* agpmCharacter, AgsmCharacter* agsmCharacter )
{
	m_pAgpmCharacter = agpmCharacter;
	m_pAgsmCharacter = agsmCharacter;

	//m_pFcm = CreateSDKInstance();
	m_pFcm = CFCMSDKBase::GetInInstance();
	if ( 0 != m_pFcm )
	{
		//bool bRet = m_pFcm->initFCMSDK( 30, 999, 1, FcmCallback, SwitchCallback, QueryCallback );
		bool bRet = m_pFcm->initFCMSDKEASY( FcmCallback, SwitchCallback, QueryCallback );
		printf("FCM::initFCMSDK() return %d\n",bRet);
		//AuLogFile_s("FCMLog.txt", "FCM::initFCMSDK() return value %d\n", bRet);
		if( bRet )
			return TRUE;
	}
	return FALSE;
}

void AgsmGameholic::Online( const char* account, const char* charID, char* ip )
{
	if( strlen(account) <= 0 || strlen(charID) <= 0 || strlen(ip) <= 0 ) 
		return;
	if ( m_pFcm )
	{
		int iRet = m_pFcm->FCMIDOnline( account, charID, ip );
		printf("FCM::FCMIDOnline() return %d\n",iRet);
//		AuLogFile_s("FCMLog.txt", "FCM::FCMIDOnline() [%s:%s] return value %d\n", account, charID, iRet);
	}
}

void AgsmGameholic::Offline( const char* account, const char* charID, char* ip )
{
	if( strlen(account) <= 0 || strlen(charID) <= 0 || strlen(ip) <= 0 ) 
		return;
	if ( m_pFcm )
	{
		int iRet = m_pFcm->FCMIDOffline( account, charID, ip );
		printf("FCM::FCMIDOffline return %d\n",iRet);
//		AuLogFile_s("FCMLog.txt", "FCM::FCMIDOffline() [%s:%s] return value %d\n", account, charID, iRet);
	}
}

int AgsmGameholic::FcmCallback(const char* szUserID, const char* szRoleID,int iOnlineMinutes,int iOfflineMinutes)
{
	if ( AgpdCharacter* pcsCharacter = m_pAgpmCharacter->GetCharacter( const_cast<char*>(szRoleID) ) )
	{
		AuAutoLock cslock( pcsCharacter->m_Mutex );

		pcsCharacter->m_ulOnlineTime = iOnlineMinutes;
		pcsCharacter->m_lAddictStatus = GetAddictStatus( iOnlineMinutes, iOfflineMinutes );

		INT16 nPacketLength = 0;
		PVOID pvPacket = m_pAgpmCharacter->MakePacketOnlineTime(pcsCharacter, &nPacketLength);
		if(!pvPacket || nPacketLength < 1)
			return 0;

		m_pAgsmCharacter->SendPacket(pvPacket, nPacketLength, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));
		m_pAgpmCharacter->m_csPacket.FreePacket(pvPacket);

		printf( "FCM::FcmCallback() [%s:%s], Online: %d, Offline: %d user addict: %d\n", szUserID, szRoleID, iOnlineMinutes, iOfflineMinutes, pcsCharacter->m_lAddictStatus );
		//AuLogFile("FCMLog.txt", "FCM::FcmCallback() [%s:%s], Online: %d, Offline: %d user addict: %d\n", szUserID, szRoleID, iOnlineMinutes, iOfflineMinutes, pcsCharacter->m_lAddictStatus );
		return nPacketLength;
	}

	return 0;
}

void AgsmGameholic::SwitchCallback(int needFCM) //needFCM = 1需要防沉迷 ，needFCM = 0不需要防沉迷
{
}

void AgsmGameholic::QueryCallback(const char* szUserID, const char* szRoleID,int iState ,int iOnlineMinutes,int iOfflineMinutes)
{
}