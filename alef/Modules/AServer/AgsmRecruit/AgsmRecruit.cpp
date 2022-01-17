#include "AgsmRecruit.h"

AgsmRecruit::AgsmRecruit()
{
	SetModuleName( "AgsmRecruit" );

	//만들어봐요~ 만들어봐요~ 자~자~ 어떤 패킷이 만들어졌을까나? by 치요스케~
	SetPacketType(AGSMRECRUIT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  연산
							AUTYPE_INT32,			1, // lLFPID  리쿠르트에 올린놈 ID
							AUTYPE_PACKET,			1, // LFP에 과한 세부정보.
							AUTYPE_PACKET,			1, // LFM에 과한 세부정보.
							AUTYPE_INT32,			1, // lResult
		                    AUTYPE_END,				0
							);

	m_csLFPInfo.SetFlagLength( sizeof(INT8) );
	m_csLFPInfo.SetFieldType(
								AUTYPE_INT32,		1, //TotalCount
		                        AUTYPE_INT32,		1, //lIndex
		                        AUTYPE_INT32,		1, //LV
		                        AUTYPE_INT32,		1, //Class
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1, // pstrPlayerID;
								AUTYPE_INT32,		1, // lPlayerID
		                        AUTYPE_END,			0
							);
	
	m_csLFMInfo.SetFlagLength( sizeof(INT16) );
	m_csLFMInfo.SetFieldType(
								AUTYPE_INT32,		1, //lTotalCount
		                        AUTYPE_INT32,		1, //lIndex
		                        AUTYPE_CHAR,		20, //Purpose
		                        AUTYPE_INT32,		1, //lRequire Member
		                        AUTYPE_INT32,		1, //Min LV
		                        AUTYPE_INT32,		1, //Max LV
		                        AUTYPE_INT32,		1, //LV
		                        AUTYPE_INT32,		1, //Class
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1, // pstrLeaderID;
								AUTYPE_INT32,		1, //lLeaderID
		                        AUTYPE_END,			0
							);
}

AgsmRecruit::~AgsmRecruit()
{
	;
}

//어디서나 볼 수 있는 OnAddModule()
BOOL AgsmRecruit::OnAddModule()
{
	m_papmMap = (ApmMap *)GetModule("ApmMap");
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmParty = (AgpmParty *) GetModule("AgpmParty");
	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pAgsmServerManager = (AgsmServerManager *)GetModule( "AgsmServerManager2" );

	if( !m_papmMap || !m_pagpmCharacter || !m_pagpmParty || !m_pagpmFactors || !m_pagpmItem || !m_pAgsmServerManager )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmRecruit::ProcessRegisterLFP( INT32 lLFPID )
{
	//lLFPID를 기반으로 접속 여부와 ID(String)을 얻어낸다.
	AgpdCharacter			*pcsAgpdCharacter;
	AgpdFactorCharStatus	*pcsAgpdFactorCharStatus;
	AgpdFactorCharType		*pcsAgpdFactorCharType;

	BOOL					bResult;
	char					strCharName[AGPACHARACTER_MAX_ID_STRING];
	INT32					lLevel;
	INT32					lClass;

	bResult = FALSE;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFPID );

	if( pcsAgpdCharacter )
	{
		memset( strCharName, 0, AGPACHARACTER_MAX_ID_STRING );

		//AccountID를 Copy한다.
		strcat( strCharName, pcsAgpdCharacter->m_szID );

		//알아낸 정보를 Recruit서버로 날린다.
		INT32	lLevel	= m_pagpmFactors->GetLevel(&pcsAgpdCharacter->m_csFactor);
		INT32	lClass	= m_pagpmFactors->GetClass(&pcsAgpdCharacter->m_csFactor);

		bResult = SendRegisterLFP( lLFPID, strCharName, lLevel, lClass, 0 );
	}

	return bResult;
}

BOOL AgsmRecruit::ProcessRegisterLFM( INT32 lLFMID, void *pvLFMInfo )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvLFMInfo && m_pagpmParty )
	{
		AgpdCharacter			*pcsAgpdCharacter;

		char			*pstrPurpose;
		char			*pstrCharName;

		INT32			lRequireMember;
		INT32			lMinLV;
		INT32			lMaxLV;

		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFMID );

		m_csLFMInfo.GetField( FALSE, pvLFMInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								&pstrPurpose,		//purpose
								&lRequireMember,	//lRequireMember
								&lMinLV,			//MinLV
								&lMaxLV,			//MaxLV
								NULL,				//Level
								NULL,				//Class
								&pstrCharName,		//CharName
								NULL				//lLeaderID
								);
								

		if( pcsAgpdCharacter )
		{
			//if( m_pagpmParty->GetLeaderCID( pcsAgpdCharacter ) == lLFMID )
			{
				if( (pstrCharName != NULL) && (pstrPurpose != NULL ) )
				{
					//lLFPID를 기반으로 접속 여부와 ID(String)을 얻어낸다.
					AgpdFactorCharStatus	*pcsAgpdFactorCharStatus;
					AgpdFactorCharType		*pcsAgpdFactorCharType;

					INT32					lLevel	= m_pagpmFactors->GetLevel(&pcsAgpdCharacter->m_csFactor);
					INT32					lClass	= m_pagpmFactors->GetClass(&pcsAgpdCharacter->m_csFactor);

					//알아낸 정보를 Recruit서버로 날린다.
					bResult = SendRegisterLFM( lLFMID, pstrPurpose, lRequireMember, lMinLV, lMaxLV, lLevel, lClass, pstrCharName, lLFMID, 1 );
				}
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruit::SendRegisterLFP( INT32 lLFPID, char *pstrLFPID, INT32 lLevel, INT32 lClass, INT32 lResult )
{
	AgsdServer		*pcsAgsdServer;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFP;
	INT16	nPacketLength;
	void	*pvLFPInfo;

	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	pvLFPInfo = m_csLFPInfo.MakePacket(FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													NULL,				//lTotalCount
													NULL,				//lIndex
													&lLevel,			//Level
													&lClass,			//Calss
													pstrLFPID,			//pstrLFPID
													NULL				//lPlayerID
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													pvLFPInfo,			//LFPInfo
													NULL,				//LFMInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
		
		m_csLFMInfo.FreePacket(pvLFPInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruit::SendRegisterLFM( INT32 lLFMID, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lLevel, INT32 lClass, char *pstrCharName, INT32 lLeaderID, INT32 lResult )
{
	AgsdServer		*pcsAgsdServer;
	void			*pvLFMInfo;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFM;
	INT16	nPacketLength;

	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	pvLFMInfo = m_csLFMInfo.MakePacket( FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
											NULL,				//lTotalCount
											NULL,				//Index
											pstrPurpose,		//Purpose
											&lRequireMember,	//Require Members
											&lMinLV,			//Min Lv
											&lMaxLV,			//Max Lv
											&lLevel,			//LV
											&lClass,			//lClass
											pstrCharName,		//pstrCharName
											&lLeaderID );		//LeaderID

	if( (pcsAgsdServer != NULL) && (pvLFMInfo != NULL) )
	{
		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														&lOperation,		//lOperation
														&lLFMID,			//lLFPID
														NULL,				//LFPInfo
														pvLFMInfo,			//LFMInfo
														&lResult			//lResult
														);

		if( pvPacket != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL AgsmRecruit::SendUpdateLFPInfo( INT32 lLFPID, char *pstrLFPID, INT32 lLevel, INT32 lClass, INT32 lResult )
{
	AgsdServer		*pcsAgsdServer;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_UPDATE_LFP;
	INT16	nPacketLength;
	void	*pvLFPInfo;

	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	pvLFPInfo = m_csLFPInfo.MakePacket(FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													NULL,				//lTotalCount
													NULL,				//lIndex
													&lLevel,			//Level
													&lClass,				//Class
													pstrLFPID,			//pstrLFPID
													NULL				//PlayerID
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													pvLFPInfo,			//pvLFPInfo
													NULL,				//pvLFMInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
		
		m_csLFMInfo.FreePacket(pvLFPInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruit::SendCancelLFP( INT32 lLFPID )
{
	AgsdServer		*pcsAgsdServer;
	AgpdCharacter	*pcsAgpdCharacter;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_CANCEL_LFP;
	INT16	nPacketLength;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFPID );
	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	if( pcsAgpdCharacter && pcsAgsdServer )
	{
		void	*pvLFPInfo = m_csLFPInfo.MakePacket( FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														NULL,						//lTotalCount
														NULL,						//lIndex
														NULL,						//lLevel
														NULL,						//Class
														pcsAgpdCharacter->m_szID,	//CharName
														NULL						//lPlayerID
														);

		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														&lOperation,		//lOperation
														&lLFPID,			//lLFPID
														pvLFPInfo,			//pvLFPInfo.
														NULL,				//pvLFMInfo
														NULL				//lResult
														);

		if( pvPacket != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL AgsmRecruit::SendCancelLFM( INT32 lLFMID )
{
	AgsdServer		*pcsAgsdServer;
	AgpdCharacter	*pcsAgpdCharacter;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_CANCEL_LFM;
	INT16	nPacketLength;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFMID );
	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	if( pcsAgpdCharacter && pcsAgsdServer )
	{
		void	*pvLFMInfo = m_csLFMInfo.MakePacket( FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														NULL,						//lTotalCount
														NULL,						//lIndex
														NULL,						//Purpose
														NULL,						//RequireMemeber
														NULL,						//MinLV
														NULL,						//MaxLV
														NULL,						//lLevel
														NULL,						//lClass
														pcsAgpdCharacter->m_szID,	//CharName
														NULL						//lLeaderID
														);

		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														&lOperation,		//lOperation
														&lLFMID,			//lLFPID
														NULL,				//pvLFPInfo.
														pvLFMInfo,			//pvLFMInfo
														NULL				//lResult
														);

		if( pvPacket != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

//패킷을 받고 파싱하자~
BOOL AgsmRecruit::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	//거래결과 혹은 Error메시지를 출력해준다~ 출력해봐요~ 출력해봐요~
	BOOL			bResult;

	INT8			lOperation;
	INT32			lCID;
	void			*pvLFPInfo;
	void			*pvLFMInfo;
	INT32			lResult;

	bResult = FALSE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&lOperation,
						&lCID,
						&pvLFPInfo,
						&pvLFMInfo,
						&lResult );

	//아이템을 판다.(Sales Box)에 올린다.
	if( lOperation == AGPMRECRUIT_REGISTER_LFP )
	{
		bResult = ProcessRegisterLFP( lCID );
	}
	else if( lOperation == AGPMRECRUIT_REGISTER_LFM )
	{
		bResult = ProcessRegisterLFM( lCID, pvLFMInfo );
	}
	//비정상적인 패킷을 받았다. 조치를쌔워주자!! 헉 그런데 이건 내 잘못이잖아? -_-; 아햏햏~
	else
	{
		//이미 bResult = FALSE이다. 그외의 뭔가를 하자!!
	}

	return bResult;
}
