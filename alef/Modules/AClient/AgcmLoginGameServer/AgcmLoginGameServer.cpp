#include "AgcmLoginGameServer.h"

AgcmLoginGameServer::AgcmLoginGameServer()
{
	SetModuleName( "AgcmLoginGameServer" );

	//만들어봐요~ 만들어봐요~ 자~자~ 어떤 패킷이 만들어졌을까나? by 치요스케~
	SetPacketType(AGSMLOGIN_PACKET_TYPE);

	m_pcsAgcmLogin = NULL	;

	m_ulNID			= 0;
	
}

AgcmLoginGameServer::~AgcmLoginGameServer()
{
}

BOOL AgcmLoginGameServer::OnAddModule()
{
	m_pcsAgpmLogin = (AgpmLogin *) GetModule("AgpmLogin");
	m_pcsAgpmCharacter = (AgpmCharacter*) GetModule("AgpmCharacter");
	m_pcsAgcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pcsAgcmConnectManager = (AgcmConnectManager *) GetModule("AgcmConnectManager");
	m_pcsAgcmLogin	=	(AgcmLogin*)GetModule("AgcmLogin");
	
	if( !m_pcsAgpmLogin || !m_pcsAgcmCharacter || !m_pcsAgcmConnectManager || !m_pcsAgpmCharacter || !m_pcsAgcmLogin )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmLoginGameServer::SendDisconnectFromGameServer( char *pstrServerGroupName, char *pstrAccountID, char *pstrCharName, INT32 lCID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMLOGIN_DISCONNECT_FROM_GAMESERVER;
	INT16	nPacketLength;
	void	*pvDetailCharInfo;

	pvDetailCharInfo = m_pcsAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGSMLOGIN_PACKET_TYPE,
													NULL,			//TID
													pstrCharName,	//CharName
													NULL,			//MaxRegisterChars
													NULL,			//Slot Index
													NULL,			//Union Info
													NULL,
													NULL,
													NULL,
													NULL
													);

	PVOID	pvPacket = m_pcsAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													pstrServerGroupName,// pstrServerGroupName;
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if (pvDetailCharInfo)
		m_pcsAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength );

		m_pcsAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgcmLoginGameServer::SendReturnToLoginServer( char *pstrServerGroupName, char *pstrAccountID, char *pstrCharacterName, INT32 lCID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMLOGIN_RETURN_TO_LOGINSERVER;
	INT16	nPacketLength;
	void	*pvDetailCharInfo;

	pvDetailCharInfo = m_pcsAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGSMLOGIN_PACKET_TYPE,
													NULL,			//TID
													pstrCharacterName,	//CharName
													NULL,			//MaxRegisterChars
													NULL,			//Slot Index
													NULL,			//Union Info
													NULL,
													NULL,
													NULL,
													NULL
													);

	PVOID	pvPacket = m_pcsAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													pstrServerGroupName,// pstrServerGroupName;
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if (pvDetailCharInfo)
		m_pcsAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength );

		m_pcsAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgcmLoginGameServer::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
{
	//거래결과 혹은 Error메시지를 출력해준다~ 출력해봐요~ 출력해봐요~
	BOOL			bResult;

	INT8			nOperation;
	char			*pstrEncryptCode;
	char			*pstrAccountID;
	INT8			iIDLen;
	char			*pstrPassword;
	INT8			iPWLen;
	INT32			lCRequesterID;
	void			*pvServerGroupName;
	void			*pvCharDetailInfo;
	void			*pvDetailServerInfo;
	INT32			lResult;

	bResult = FALSE;
	m_ulNID	= ulNID;

	m_pcsAgpmLogin->m_csPacket.GetField(TRUE, pvPacket, nSize,
						&nOperation,
						&pstrEncryptCode,
						&pstrAccountID,
						&iIDLen,
						&pstrPassword,
						&iPWLen,
						&lCRequesterID,
						&pvServerGroupName,
						&pvCharDetailInfo,
						&pvDetailServerInfo,
						&lResult );

	if( nOperation == AGPMLOGIN_RETURN_TO_LOGINSERVER )
	{
		EnumCallback( AGCMLOGINGAMESERVER_CB_ID_RESPONSE_RETURN_TO_LOGINSERVER, NULL, NULL )	;
	}

	return TRUE;
}

BOOL	AgcmLoginGameServer::SetCallbackResponseReturnToLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGINGAMESERVER_CB_ID_RESPONSE_RETURN_TO_LOGINSERVER, pfCallback, pClass );
}