/******************************************************************************
Module:  AgcmLoginMultiple.cpp
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 3. 18
******************************************************************************/
#include "AgcmLoginMultiple.h"

AgcmLoginMultiple::AgcmLoginMultiple()
{
	SetModuleName( "AgcmLogin" );

	//만들어봐요~ 만들어봐요~ 자~자~ 어떤 패킷이 만들어졌을까나? by 치요스케~
	SetPacketType(AGPMLOGIN_PACKET_TYPE);

	m_pstrEncryptCode = NULL;

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1, // Operation  연산
							AUTYPE_CHAR,			ENCRYPT_STRING_SIZE, //EncryptCode
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING+1,  // AccountID
							AUTYPE_INT8,			1, // AccountID Length
							AUTYPE_CHAR,			AGPACHARACTER_MAX_PW_STRING+1, // AccountPassword
							AUTYPE_INT8,			1, // Password Length
							AUTYPE_INT32,			1, // lCID
							AUTYPE_CHAR,			AGPMLOGIN_SERVERGROUPNAMESIZE,// pvServerGroupName ;
							AUTYPE_PACKET,			1, // pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
							AUTYPE_PACKET,			1, // pvDetailServerInfo; 서버군 상세 정보.
							AUTYPE_INT32,			1, // lResult
		                    AUTYPE_END,				0
							);

	m_csServerAddr.SetFlagLength(sizeof(INT8));
	m_csServerAddr.SetFieldType(AUTYPE_CHAR,		AGPMLOGIN_IPADDRSIZE, // GameServerAddr
								AUTYPE_CHAR,		AGPMLOGIN_IPADDRSIZE, // AuctionServerAddr
								AUTYPE_CHAR,		AGPMLOGIN_IPADDRSIZE, // RecruitServerAddr
								AUTYPE_END,			0
								);

	m_csDetailCharInfo.SetFlagLength(sizeof(INT8));
	m_csDetailCharInfo.SetFieldType(AUTYPE_INT32,	1,  //TID
									AUTYPE_CHAR,	AGPACHARACTER_MAX_ID_STRING, //CharName
									AUTYPE_INT32,	1,	//MaxRegisterChars
									AUTYPE_INT32,	1,	//Slot Index
									AUTYPE_INT32,	1,	//lUnion
									AUTYPE_INT32,	1,	//lRace
									AUTYPE_END,		0
									);

	m_csDetailServerInfo.SetFlagLength(sizeof(INT8));
	m_csDetailServerInfo.SetFieldType(	AUTYPE_CHAR,	AGPMLOGIN_IPADDRSIZE,  //IPAddress
										AUTYPE_END,		0
										);
	
	m_pcsMainWindow	=	NULL				;

	m_lLoginMode = AGCMLOGIN_PRE_LOGIN_MODE	;
}

AgcmLoginMultiple::~AgcmLoginMultiple()
{
	if( m_pstrEncryptCode != NULL )
	{
		delete [] m_pstrEncryptCode;
	}
}

BOOL AgcmLoginMultiple::OnAddModule()
{
	m_pcsAgcmCharacter = (AgcmCharacter *) GetModule("AgcmCharacter");
	m_pcsAgcmConnectManager = (AgcmConnectManager *) GetModule("AgcmConnectManager");
	m_pcsAgpmCharacter = (AgpmCharacter*) GetModule("AgpmCharacter");
	
	m_pstrEncryptCode = new char[ENCRYPT_STRING_SIZE];

	if( !m_pcsAgcmCharacter || !m_pcsAgcmConnectManager || !m_pcsAgpmCharacter )
	{
		return FALSE;
	}

	AgcmRender*		pAgcmRender = (AgcmRender *)GetModule("AgcmRender");
	if( !pAgcmRender ) return FALSE;
	
	return TRUE;
}

BOOL AgcmLoginMultiple::OnInit()
{
	return TRUE;
}

INT16 AgcmLoginMultiple::LoginToServer()
{
	BOOL			bResult;

	bResult = InitRegistry();

	INT16			ulDPNID = 0;
	if( bResult )
	{
		// Connect를 요구한다 - 클라이언트( DPNID등록 )
		if ( (ulDPNID = m_pcsAgcmConnectManager->Connect( m_pstrIPAddress , ACDP_SERVER_TYPE_LOGINSERVER,
			this, CBSocketOnConnect, CBSocketOnDisConnect, CBSocketOnError ) ) >= 0 )
		{
			;//m_ulNID = ulDPNID;
		}
	}
	
	return ulDPNID;
}

BOOL AgcmLoginMultiple::DisconnectLoginServer(INT16 ulNID)
	{
	return m_pcsAgcmConnectManager->Disconnect(ulNID);
	}

VOID AgcmLoginMultiple::SetMainWindow(AgcWindow *pcsWindow)
	{
	m_pcsMainWindow = pcsWindow;
	}

BOOL AgcmLoginMultiple::InitRegistry()
{
	BOOL			bResult;
	HKEY			hRegKey;
	char			strBuffer[256];

	sprintf( strBuffer, "SOFTWARE\\ArchLord" );

/*	//레지스트리가 없다면 만들어준다.
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, strBuffer, 0, KEY_WRITE, &hRegKey ) )
	{
		//레지스트리가 없다. 만든다~ 아싸! 조쿠나!!
		RegCreateKey( HKEY_LOCAL_MACHINE, strBuffer, &hRegKey );

		char			strIP[256];
		int				iPort;

		sprintf( strIP, "%s", "220.75.250.237" );
		iPort = 9991;

		//키에 값을 넣는다.
		//IP
		RegSetValueEx( hRegKey, "LoginServerIP", 0, REG_SZ, (unsigned char*)strIP, strlen( strIP ) );
		//Port
		RegSetValueEx( hRegKey, "LoginServerPort", 0, REG_DWORD, (unsigned char*)&iPort, sizeof( iPort ) );
	}

	RegCloseKey(hRegKey); */

	//Key에서 값을 읽는다. 이번엔 읽어야한다. -_-;
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, strBuffer, 0, KEY_READ, &hRegKey ) )
	{
		bResult = false;
	}
	else
	{
		char			strTempIP[256];
		int				iTempPort;

		DWORD			iType;
		DWORD			iDataLen;

		//키에 값을 넣는다.
		//IP, String은 BufferSize를 정해줘야한다.
		iDataLen = sizeof( strTempIP );
		RegQueryValueEx( hRegKey, "LoginServerIP", 0, &iType, (unsigned char *)strTempIP, &iDataLen );
		//Port
		iDataLen = sizeof( iTempPort );
		RegQueryValueEx( hRegKey, "LoginServerPort", 0, &iType, (unsigned char*)&iTempPort, &iDataLen );

		memset( m_pstrIPAddress, 0, sizeof(m_pstrIPAddress) );
		sprintf( m_pstrIPAddress, "%s:%d", strTempIP, iTempPort );

		RegCloseKey(hRegKey); 

		bResult = true;
	}

	return bResult;
}

BOOL AgcmLoginMultiple::SendAddToLoginServer(INT16 ulNID)
	{
	BOOL			bResult;

	bResult = FALSE;
	INT8	lOperation = AGPMLOGIN_ADD_TO_LOGINSERVER;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Length
													NULL,				// AccountPassword
													NULL,				// AccountPassword Length
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
													);
	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID );
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::SetIDPassword( char *pstrID, char *pstrPassword )
{

	return TRUE;
}

BOOL AgcmLoginMultiple::SendAccount( char *pstrAccountID, char *pstrAccountPassword, INT16 ulNID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( (m_pstrEncryptCode != NULL) && (pstrAccountID != NULL) && (pstrAccountPassword != NULL) )
	{
		//ID,PW의 복사본을 만들고 Encrypt한다.
		char			*pstrTempID, *pstrTempPW;
		int				iIDLen,	iPWLen;

		iIDLen = strlen( pstrAccountID );
		iPWLen = strlen( pstrAccountPassword );

		if( (0 < iIDLen) && (iIDLen <= AGPACHARACTER_MAX_ID_STRING ) && (0 < iPWLen) && (iPWLen <= AGPACHARACTER_MAX_PW_STRING ) )
		{
			pstrTempID = new char [AGPACHARACTER_MAX_ID_STRING+1]; //NULL문자 포함
			pstrTempPW = new char [AGPACHARACTER_MAX_PW_STRING+1]; //NULL문자 포함

			memset( pstrTempID, 0, iIDLen );
			memset( pstrTempPW, 0, iPWLen );

			strcat( pstrTempID, pstrAccountID );
			strcat( pstrTempPW, pstrAccountPassword );

			if( m_csMD5Encrypt.EncryptString( m_pstrEncryptCode, pstrTempID, iIDLen ) == true )
			{
				if( m_csMD5Encrypt.EncryptString( m_pstrEncryptCode, pstrTempPW, iPWLen ) == true )
				{
					INT8	lOperation = AGPMLOGIN_CHECK_ACCOUNT;
					INT16	nPacketLength;

					PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
																	&lOperation,		// lOperation
																	NULL,				// EncryptCode
																	pstrTempID,			// AccountID
																	&iIDLen,				// AccountID Length
																	pstrTempPW,			// AccountPassword
																	&iPWLen,				// Password Length
																	NULL,				// lCID
																	NULL,				// pvServerGroupName;
																	NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
																	NULL,				// pvDetailServerInfo; 서버군 정보.
																	NULL				// lResult
																		);

					if( pvPacket != NULL )
					{
						bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID );
					
						m_csPacket.FreePacket(pvPacket);
					}
				}
			}

			delete [] pstrTempID;
			delete [] pstrTempPW;
		}
	}

	return bResult;
}

BOOL AgcmLoginMultiple::SendSelectServerGroup(char *pstrServerGroupName, INT16 ulNID)
	{
	BOOL			bResult;

	bResult = FALSE;
	INT8	lOperation = AGPMLOGIN_SELECT_SERVER;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID);
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

//아직 CID가 없으니 Account로 구분한다.
BOOL AgcmLoginMultiple::SendSelectCharacter(char *pstrServerGroupName, char *pstrAccountID, INT32 lTID, char *pstrCharName, INT16 ulNID)
	{
	BOOL			bResult;

	INT8			lOperation = AGPMLOGIN_SELECT_CHARACTER;
	INT16			nPacketLength;
	void			*pvDetailCharInfo;

	bResult = FALSE;
	pvDetailCharInfo = m_csDetailCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lTID,				//TID
													pstrCharName,		//CharName
													NULL,				//MaxRegisterChars
													NULL,				//Slot Index
													NULL,				//Union Info
													NULL
													);
													
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													pstrServerGroupName,// pvServerGroupName;
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if (pvDetailCharInfo)
		m_csDetailCharInfo.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID);
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

//아직 CID가 없으니 Account로 구분한다.
BOOL AgcmLoginMultiple::SendCreateCharacter( char *pstrAccountID, char *pstrServerGroupName, INT32 lTID, char *pstrCharName, INT32 lIndex, INT32 lUnion, INT16 ulNID)
	{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMLOGIN_CREATE_CHARACTER;
	INT16	nPacketLength;
	void	*pvDetailCharInfo;

	pvDetailCharInfo = m_csDetailCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lTID,			//TID
													pstrCharName,	//CharName
													NULL,			//MaxRegisterChars
													&lIndex,		//Slot Index
													&lUnion,			//Union Info
													NULL
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													pstrServerGroupName,// pstrServerGroupName;
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if (pvDetailCharInfo)
		m_csPacket.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID);
		m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}

//아직 CID가 없으니 Account로 구분한다.
BOOL AgcmLoginMultiple::SendRemoveCharacter( char *pstrAccountID, char *pstrServerGroupName, char *pstrCharName, INT16 ulNID)
	{
	BOOL			bResult;

	bResult = FALSE;
	INT8	lOperation = AGPMLOGIN_REMOVE_CHARACTER;
	INT16	nPacketLength;
	void	*pvDetailCharInfo;

	pvDetailCharInfo = m_csDetailCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													NULL,			//TID
													pstrCharName,	//CharName
													NULL,			//MaxRegisterChars
													NULL,			//Slot Index
													NULL,			//Union Info
													NULL
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if (pvDetailCharInfo)
		m_csPacket.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID );
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::SendRemoveDuplicateCharacter( char *pstrAccountID, char *pstrServerGroupName, char *pstrCharName, INT16 ulNID)
	{
	BOOL			bResult;

	bResult = FALSE;
	INT8	lOperation = AGPMLOGIN_REMOVE_DUPLICATE_CHARACTER;
	INT16	nPacketLength;
	void	*pvDetailCharInfo;

	pvDetailCharInfo = m_csDetailCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													NULL,			//TID
													pstrCharName,	//CharName
													NULL,			//MaxRegisterChars
													NULL,			//Slot Index
													NULL,			//Union Info
													NULL
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if (pvDetailCharInfo)
		m_csPacket.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID);
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::SendGetUnion( char *pstrServerGroupName, char *pstrAccountID, INT16 ulNID )
	{
	BOOL			bResult;

	bResult = FALSE;
	INT8	lOperation = AGPMLOGIN_GET_UNION;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID );
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::SendRemoveDuplicateAccount( char *pstrAccountID, INT16 ulNID)
	{
	BOOL			bResult;

	bResult = FALSE;
	INT8	lOperation = AGPMLOGIN_REMOVE_DUPLICATE_ACCOUNT;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID );
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::SendGetChars( char *pstrServerGroupName, char *pstrAccountID, INT16 ulNID )
	{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMLOGIN_GET_CHARACTERS;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													NULL,				// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID );
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::SendGetCreateCharacter( char *pstrServerGroupName, char *pstrAccount, char *pstrCharacterName, INT16 ulNID )
{
	BOOL			bResult;

	bResult = FALSE;

	void	*pvDetailCharInfo;
	INT8	lOperation = AGPMLOGIN_ADD_CREATED_CHARACTER;
	INT8	lAccountLen;
	INT16	nPacketLength;

	lAccountLen = strlen( pstrAccount ) + 1 ;
	pvDetailCharInfo = m_csDetailCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														pstrCharacterName,			//CharName
														NULL,			//MaxRegisterChars
														NULL,			//Slot Index
														NULL,			//UnionInfo
														NULL			//race info
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccount,		// AccountID
													&lAccountLen,		// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;각종 서버의 주소를 넣어둔 패킷
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL		// lResult
													);

	if (pvDetailCharInfo)
		m_csDetailCharInfo.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID );
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::SendRequestRaceCharacterInfo( char *pstrAccountID, INT32 lRace, INT16 ulNID )
	{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMLOGIN_REQUEST_CREATE_CHARACTER_INFO;
	INT16	nPacketLength;

	PVOID	pvDetailCharInfo = m_csDetailCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													NULL,			//TID
													NULL,	//CharName
													NULL,			//MaxRegisterChars
													NULL,			//Slot Index
													NULL,			//Union Info
													&lRace
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&lOperation,		// lOperation
													NULL,				// EncryptCode
													pstrAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;
													pvDetailCharInfo,	// pvCharDetailInfo; 캐릭생성시 상세데이터를 넣는 패킷
													NULL,				// pvDetailServerInfo; 서버군 정보.
													NULL				// lResult
														);

	if (pvDetailCharInfo)
		m_csPacket.FreePacket(pvDetailCharInfo);

	if( pvPacket != NULL )
		{
		bResult = SendPacket(pvPacket, nPacketLength, ACDP_SEND_LOGINSERVER, ulNID);
		m_csPacket.FreePacket(pvPacket);
		}
	return bResult;
	}

BOOL AgcmLoginMultiple::ProcessUnionInfo( INT32 *plUnion, void *pvCharDetailInfo, UINT16 ulNID)
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvCharDetailInfo != NULL )
	{
		m_csDetailCharInfo.GetField( FALSE, pvCharDetailInfo, 0,
									NULL,			//TID
									NULL,			//CharName
									NULL,			//MaxRegisterChars
									NULL,			//Slot Index
									plUnion,			//Union Info
									NULL
									);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmLoginMultiple::ProcessRegisterChar( char **ppstrCharName, INT32 *plRegisterCharCount, INT32 *plSlotIndex, void *pvCharDetailInfo, UINT16 ulNID)
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvCharDetailInfo != NULL )
	{
		m_csDetailCharInfo.GetField( FALSE, pvCharDetailInfo, 0,
									NULL,						//TID
									ppstrCharName,				//CharName
									plRegisterCharCount,		//MaxRegisterChars
									plSlotIndex,				//Slot Index
									NULL,						//Union Info
									NULL
									);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmLoginMultiple::ProcessLoginComplete( INT32 lCRequesterID, void *pvCharDetailInfo, void *pvDetailServerInfo, UINT16 ulNID )
{
	OutputDebugString("--------> ProcessLoginComplete\n");

	BOOL			bResult;
	bResult = FALSE;


	if( (pvCharDetailInfo != NULL) && (pvDetailServerInfo != NULL) )
	{
		char			*pstrCharName;
		char			*pstrIPAddress;
		INT32			lTID;

		m_csDetailCharInfo.GetField( FALSE, pvCharDetailInfo, 0,
									&lTID,				//TID
									&pstrCharName,		//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL
									);

		m_csDetailServerInfo.GetField( FALSE, pvDetailServerInfo, 0,
									&pstrIPAddress
									);

		if( (pstrCharName != NULL) && (pstrIPAddress != NULL) )
		{
			m_pcsAgpmCharacter->RemoveCharacter(pstrCharName);

			TRACE("Login End!!!!!!!!!!!!!!!!!!!!!!!!!!11\n");

			bResult = m_pcsAgcmCharacter->SendEnterGameWorld( pstrCharName, pstrIPAddress );
		}
	}	

	return bResult;
}

BOOL AgcmLoginMultiple::ProcessRemoveChar( void *pvCharDetailInfo, UINT16 ulNID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvCharDetailInfo != NULL )
	{
		char			*pstrCharName;

		m_csDetailCharInfo.GetField( FALSE, pvCharDetailInfo, 0,
									NULL,				//TID
									&pstrCharName,		//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL
									);

		if( pstrCharName != NULL )
		{
			EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_REMOVE_MYCHARACTER, pstrCharName, (PVOID) ulNID );
			RemoveCharacter( pstrCharName );
		}
	}	

	return bResult;	
}

BOOL AgcmLoginMultiple::ProcessLoginResult( void *pvCharDetailInfo, INT32 lResult, INT16 ulNID )
	{
	OutputDebugString("--------> ProcessLoginResult\n");

	BOOL				bResult;
	char				*pstrCharName;

	bResult = FALSE;
	pstrCharName = NULL;

	if( pvCharDetailInfo != NULL )
		{
		m_csDetailCharInfo.GetField( FALSE, pvCharDetailInfo, 0,
									NULL,				//TID
									&pstrCharName,		//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL
									);
		}

	switch (lResult)
		{
		//ID가 없는경우
		case AGPMLOGIN_RESULT_INVALID_ID :
			g_pEngine->MessageDialog( NULL , "ID가 존재하지 않습니다" );
			EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_ID_FALID, NULL, (PVOID) ulNID);
			break;
		//패스워드가 틀린경우
		case AGPMLOGIN_RESULT_INVALID_PW :
			g_pEngine->MessageDialog( NULL, "Password가 틀립니다" );
			EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_PASSWORD_FALID, NULL, (PVOID) ulNID);
			break;
		//이미 존재하는 캐릭 이름.
		case AGPMLOGIN_RESULT_ALREADEY_EXIST_CHAR_NAME :
			if( pstrCharName != NULL )
				{
				g_pEngine->MessageDialog( NULL , "이미 존재하는 케릭터이름입니다" );
				}
			break;
		// 만들수 없는 케릭터 네임 
		case AGPMLOGIN_RESULT_CAN_NOT_MAKE_CHAR_NAME :
			EnumCallback( AGCMLOGIN_CB_ID_RESULT_CAN_NOT_MAKE_CHAR_NAME, NULL, (PVOID) ulNID);
			break;
		// 3회 패스워드 오류 - Application 종료 상황
		case AGPMLOGIN_RESULT_DISCONNECT_BY_INVALID_PW :
			EnumCallback( AGCMLOGIN_CB_ID_RESULT_DISCONNECT_BY_INVALID_PW, NULL, (PVOID) ulNID);
			break;
		//캐릭터 슬롯이 차서 만들수 없는경우.
		case AGPMLOGIN_RESULT_FULL_SLOT :
			if( pstrCharName != NULL )
				{
				}
			break;
		case AGPMLOGIN_RESULT_EXIST_DUPLICATE_ACCOUNT :
			EnumCallback( AGCMLOGIN_CB_ID_RESULT_EXIST_DUPLICATE_ACCOUNT, NULL, (PVOID) ulNID);
			//bResult = SendRemoveDuplicateAccount( m_pstrID );
			break;
		//게임 서버에 해당 Account로 들어가있는 캐릭이있다고 알려준다.
		case AGPMLOGIN_RESULT_EXIST_DUPLICATE_CHAR :
			break;
		case AGPMLOGIN_RESULT_REMOVE_DUPLICATE_ACCOUNT :
			EnumCallback( AGCMLOGIN_CB_ID_RESULT_REMOVE_DUPLICATE_ACCOUNT, NULL, (PVOID) ulNID);
			//bResult = SendAccount( m_pstrID, m_pstrPassword, ulNID );
			break;
		//삭제 실패?
		case AGPMLOGIN_RESULT_CONNECT_TRY_LATER :
			EnumCallback( AGCMLOGIN_CB_ID_RESULT_CANNOT_REMOVE_DUPLICATE_ACCOUNT, NULL, (PVOID) ulNID);
			break;
		//삭제 상공!!
		case AGPMLOGIN_RESULT_REMOVE_DUPLICATE_CHAR :
			break;
		//같은 계정을 입력한 누군가에 의해 짤렸다. 방법 당했소!
		case AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT :
			break;
		//기타 알수 없는 오류
		case AGPMLOGIN_RESULT_UNKNOWN_ERROR :
		default:
			break;
		}
	return bResult;
	}

//패킷을 받고 파싱하자~
BOOL AgcmLoginMultiple::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
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

	m_csPacket.GetField(TRUE, pvPacket, nSize,
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

	if( nOperation == AGPMLOGIN_ENCRYPT_CODE )
	{
		if( m_pstrEncryptCode != NULL )
		{
			memcpy( m_pstrEncryptCode, pstrEncryptCode, ENCRYPT_STRING_SIZE );

			EnumCallback( AGCMLOGIN_CB_ID_ENCRYPT_CODE_SUCCESS, NULL, (PVOID) ulNID);
		}
	}
	//ID,Password가 맞아 로그인 서버에 접속이 완료된 경우.
	else if( nOperation == AGPMLOGIN_LOGIN_SUCCEEDED )
	{
		//bResult = SendGetUnion( NULL, pstrAccountID, ulNID );
		bResult = EnumCallback( AGCMLOGIN_CB_LOGIN_SUCCEEDED , NULL, (PVOID) ulNID);
	}
	//Union 정보를 받아온다.
	else if( nOperation == AGPMLOGIN_GET_UNION )
	{
		INT32			lUnion;
	
		bResult = ProcessUnionInfo( &lUnion, pvCharDetailInfo, ulNID);

		//Union정보를 얻어냈다.
		if( bResult ) // bResult == TRUE
		{
			if ( AUUNION_TYPE_NONE == lUnion )
			{
				EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_DONT_GET_UNION, NULL, (PVOID) ulNID )		;
			}
			else 
			{
				EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_GET_UNION, (PVOID)lUnion, (PVOID) ulNID )			;
			}
		}
		//Union정보를 얻어내지 못했다??
		else
		{
		}
	}
	//계정에 등록된 캐릭터의 이름과 갯수, 몇번째 슬롯인지 읽어낸다.
	else if( nOperation == AGPMLOGIN_GET_CHARACTERS )
	{
		char			*pstrCharName;			//pstrCharName			<-등록된 캐릭의 이름
		INT32			lRegisterCharCount;		//lRegisterCharCount	<-등록된 캐릭의 수	
		INT32			lSlotIndex;				//lSlotIndex			<-인덱스( 제로 베이스 )

		bResult = ProcessRegisterChar( &pstrCharName, &lRegisterCharCount, &lSlotIndex, pvCharDetailInfo, ulNID );

		//등록된 캐릭터의 이름과 갯수, 슬롯 인덱스를 읽어냈다.
		if( bResult ) // bResult = TRUE
		{
			//########m_lMyResisterCharacter = lRegisterCharCount;

			EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_GET_MYCHARACTER_INFO, (PVOID)lSlotIndex, (PVOID)pstrCharName );	
		}
		//실패?
		else
		{

		}
	}
	else if ( nOperation == AGPMLOGIN_ADD_CREATED_CHARACTER )
	{
		char			*pstrCharName;			//pstrCharName			<-등록된 캐릭의 이름
		INT32			lRegisterCharCount;		//lRegisterCharCount	<-등록된 캐릭의 수	
		INT32			lSlotIndex;				//lSlotIndex			<-인덱스( 제로 베이스 )

		bResult = ProcessRegisterChar( &pstrCharName, &lRegisterCharCount, &lSlotIndex, pvCharDetailInfo, ulNID );

		//등록된 캐릭터의 이름과 갯수, 슬롯 인덱스를 읽어냈다.
		if( bResult ) // bResult = TRUE
		{
			//##########m_lMyResisterCharacter = lRegisterCharCount							;

			EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_CREATE_CHARACTER_INFO, (PVOID)lSlotIndex, (PVOID)pstrCharName );	
		}
		//실패?
		else
		{

		}
	}
	//로그인 과정이 모두 끝나고 게임서버에 접속만하면되는 상황
	else if( nOperation == AGPMLOGIN_LOGIN_COMPLETE )
	{
		// 제대로 선택했다 - UI 제어권을 AgcmUIManager로 넘겨준다 - Part II
		EnumCallback( AGCMLOGIN_CB_ID_LOGIN_PROCESS_END, NULL, (PVOID) ulNID);

		bResult = ProcessLoginComplete( lCRequesterID, pvCharDetailInfo, pvDetailServerInfo, ulNID );

		//게임 서버와 연결되었으니 로그인 서버와는 이제 더이상 볼일이 없군. 접속 해제!!
		if( bResult )
		{
			//m_pcsAgcmConnectManager->Disconnect( m_pcsAgcmConnectManager->GetLoginServerNID() );
		}
		//게임 서버와 연결실패. 로그인 화면에서 에러 메시지를 발생시키고 상황에 따른 대처를 한다.
		else //if( bResult == false )
		{
			
		}
	}
	//로그인과정에서 문제가 생긴경우.
	else if( nOperation == AGPMLOGIN_LOGIN_FAILED )
	{
		EnumCallback( AGCMLOGIN_CB_ID_RECEIVE_DONT_CERTIFY_ID, NULL, (PVOID) ulNID )		;
	}
	//로그인 서버에서 캐릭터가 삭제된 경우.
	else if( nOperation == AGPMLOGIN_REMOVE_CHARACTER )
	{
		bResult = ProcessRemoveChar( pvCharDetailInfo, ulNID );
	}
	else if( nOperation == AGPMLOGIN_LOGIN_RESULT )
	{
		bResult = ProcessLoginResult( pvCharDetailInfo, lResult, ulNID );
	}
	else if ( nOperation == AGPMLOGIN_RESPONSE_CREATE_CHARACTER_INFO )
	{
		EnumCallback( AGCMLOGIN_CB_ID_RESPONSE_CREATE_CHARACTER_INFO, NULL, (PVOID) ulNID );
	}
	else if ( nOperation == AGPMLOGIN_GET_CHARACTERS_FINISH)
	{
		EnumCallback( AGCMLOGIN_CB_ID_GET_CHARACTERS_FINISH, (PVOID)pstrAccountID, (PVOID) ulNID );		
	}
	else if ( nOperation == AGPMLOGIN_SEND_CHARACTER_FINISH)
	{
		PVOID pvBuffer[2];	// 0 : CID, 1 : AccountID
		pvBuffer[0] = (PVOID)lCRequesterID;
		pvBuffer[1] = (PVOID)pstrAccountID;
		EnumCallback( AGCMLOGIN_CB_ID_SEND_CHARACTER_FINISH, (PVOID)pvBuffer, (PVOID) ulNID );
	}
	else if ( nOperation == AGPMLOGIN_SEND_CREATE_CHARACTER_FINISH )
	{
		EnumCallback( AGCMLOGIN_CB_ID_SEND_CREATE_CHARACTER_FINISH, NULL, (PVOID) ulNID);
	}
	else if ( nOperation == AGPMLOGIN_GAME_SERVER_NOT_READY )
	{
		EnumCallback( AGCMLOGIN_CB_ID_GAME_SERVER_NOT_READY, NULL, (PVOID) ulNID );
	}
	else if ( nOperation == AGPMLOGIN_RETURN_TO_LOGINSERVER )
	{
		EnumCallback( AGCMLOGIN_CB_ID_RESPONSE_RETURN_TO_LOGINSERVER, NULL, (PVOID) ulNID );
	}
	//그외의 경우.
	else
	{

	}

	return bResult;
}

/*****************************************************************
*   Function : AgpmCharacter를 통해 Character Remove한다
*   Comment  : RemoveCharacter
*   Date&Time : 2003-06-16, 오후 2:41
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmLoginMultiple::RemoveCharacter( char* szName )
{
	// Map에서 먼저 삭제 
	m_pcsAgpmCharacter->RemoveCharacterFromMap( m_pcsAgpmCharacter->GetCharacter( szName ));

	// Public Character Module 에서 삭제 
	return m_pcsAgpmCharacter->RemoveCharacter( szName );
}

/*****************************************************************
*   Function : SetCallbackConnectLoginServer
*   Comment  : SetCallbackConnectLoginServer
*   Date&Time : 2003-10-27, 오후 12:03
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmLoginMultiple::SetCallbackConnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMLOGIN_CB_ID_CONNECT_LOGIN_SERVER_SUCCESS, pfCallback, pClass );
}

/*****************************************************************
*   Function : SetCallbackLoginEnd
*   Comment  : Set Callback Login End 
*   Date&Time : 2003-08-12, 오전 11:18
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmLoginMultiple::SetCallbackLoginEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_LOGIN_PROCESS_END, pfCallback, pClass );
}

/*****************************************************************
*   Function : SetCallbackReceiveDontCertifyID
*   Comment  : Set Callback Don't Certify ID 
*   Date&Time : 2003-10-16, 오후 9:15
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmLoginMultiple::SetCallbackReceiveDontCertifyID(ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_DONT_CERTIFY_ID, pfCallback, pClass );	
}

/*****************************************************************
*   Function : SetCallbackReceiveGetUnion
*   Comment  : Set Callback Receive Get Union 
*   Date&Time : 2003-10-16, 오후 8:58
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmLoginMultiple::SetCallbackReceiveGetUnion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_GET_UNION, pfCallback, pClass );
}

/*****************************************************************
*   Function : SetCallbackReceiveDontGetUnion
*   Comment  : Set Callback Receive Don't Get Union
*   Date&Time : 2003-10-16, 오후 9:01
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmLoginMultiple::SetCallbackReceiveDontGetUnion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_DONT_GET_UNION, pfCallback, pClass );
}

/*****************************************************************
*   Function : SetCallbackReceiveGetCharacter
*   Comment  : Set Callback Receive Get Character 
*   Date&Time : 2003-10-16, 오후 8:58
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmLoginMultiple::SetCallbackReceiveGetMyCharacterInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_GET_MYCHARACTER_INFO, pfCallback, pClass );
}

/*****************************************************************
*   Function : SetCallbackEncryptCodeSuccess
*   Comment  : SetCallbackEncryptCodeSuccess
*   Date&Time : 2003-10-22, 오후 2:07
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL AgcmLoginMultiple::SetCallbackEncryptCodeSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_ENCRYPT_CODE_SUCCESS, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackReceiveIdFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass)		
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_ID_FALID, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackReceivePasswordFailed(ApModuleDefaultCallBack pfCalllback, PVOID pClass)	
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_PASSWORD_FALID, pfCalllback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackResponseCreateCharacterInfo(ApModuleDefaultCallBack pfCallback,PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_RESPONSE_CREATE_CHARACTER_INFO, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackReceiveRemoveMyCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_REMOVE_MYCHARACTER, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackReceiveGetCreateCharacterInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_RECEIVE_CREATE_CHARACTER_INFO, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackResultNotCreateSameNameCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_RESULT_NOT_CREATE_SAME_NAME_CHAR, pfCallback, pClass);
}

BOOL AgcmLoginMultiple::SetCallbackGetCharactersFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_GET_CHARACTERS_FINISH, pfCallback, pClass);
}

BOOL AgcmLoginMultiple::SetCallbackSendCharacterFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_SEND_CHARACTER_FINISH, pfCallback, pClass);
}

BOOL AgcmLoginMultiple::SetCallbackSendCreateCharacterFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_SEND_CREATE_CHARACTER_FINISH, pfCallback, pClass);
}

BOOL AgcmLoginMultiple::SetCallbackGameServerNotReady(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback( AGCMLOGIN_CB_ID_GAME_SERVER_NOT_READY, pfCallback, pClass);
}

BOOL AgcmLoginMultiple::SetCallbackResponseReturnToLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMLOGIN_CB_ID_RESPONSE_RETURN_TO_LOGINSERVER, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackResultCanNotMakeCharName(ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMLOGIN_CB_ID_RESULT_CAN_NOT_MAKE_CHAR_NAME, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackResultDisconnectByInvaludPassword(ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMLOGIN_CB_ID_RESULT_DISCONNECT_BY_INVALID_PW, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackResultExistDuplicateAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMLOGIN_CB_ID_RESULT_EXIST_DUPLICATE_ACCOUNT, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackResultRemoveDuplicateAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass )	
{
	return SetCallback( AGCMLOGIN_CB_ID_RESULT_REMOVE_DUPLICATE_ACCOUNT, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackResultCannotRemoveDuplicateAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMLOGIN_CB_ID_RESULT_CANNOT_REMOVE_DUPLICATE_ACCOUNT, pfCallback, pClass );
}

BOOL AgcmLoginMultiple::SetCallbackReceiveLoginSucceeded(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback( AGCMLOGIN_CB_LOGIN_SUCCEEDED, pfCallback, pClass );
	}

/*****************************************************************
*   Function : SelectCharacter
*   Comment  : Select Character 
*   Date&Time : 2003-08-22, 오후 5:53
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AgcmLoginMultiple::SelectCharacter( INT32 lCID, char* pszAccountID, INT16 ulNID )
{
	AgpdCharacter*	pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacter( lCID );

	if ( NULL == pcsAgpdCharacter ) return ;

	CHAR			pszCharacterID[AGPACHARACTER_MAX_ID_STRING + 1];

	memset( pszCharacterID, 0, sizeof( pszCharacterID ) );
	strcpy( pszCharacterID, pcsAgpdCharacter->m_szID );
	
	// 2. Select Info 보냄 
	SendSelectCharacter( NULL, pszAccountID, 0, pszCharacterID, ulNID);
}

/*****************************************************************
*   Function : CBMapLoad
*   Comment  : Map Load Callback
*   Date&Time : 2003-08-28, 오후 9:59
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL	AgcmLoginMultiple::CBMapLoad( PVOID pData, PVOID pClass, PVOID pCustData					)
{
	AgcmLoginMultiple* pThis = (AgcmLoginMultiple*)pClass;
	
	return TRUE;
}

AgpdCharacter*	AgcmLoginMultiple::GetCharacter( INT32 lCID )
{
	return ( m_pcsAgpmCharacter->GetCharacter( lCID ) );
}

BOOL	AgcmLoginMultiple::CBSocketOnConnect( PVOID pData, PVOID pClass, PVOID pCustData				)
	{
	AgcmLoginMultiple*	pThis = (AgcmLoginMultiple*)pClass		;
	AcClientSocket* pSocket = (AcClientSocket *) pData;

	pThis->SendAddToLoginServer(pSocket->GetIndex());
	pThis->EnumCallback( AGCMLOGIN_CB_ID_CONNECT_LOGIN_SERVER_SUCCESS, NULL, (PVOID)pSocket->GetIndex());

	return TRUE;
	}

BOOL	AgcmLoginMultiple::CBSocketOnDisConnect( PVOID pData, PVOID pClass, PVOID pCustData)
{
	// DisConnect 처리

	return TRUE;
}

BOOL	AgcmLoginMultiple::CBSocketOnError( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AcClientSocket* pSocket = (AcClientSocket*)pData	;
	
	if ( !pSocket ) return FALSE;

	pSocket->Close();

	return TRUE;
}