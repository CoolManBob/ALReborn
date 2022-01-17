#include <stdio.h>
#include "AgsmRecruitClient.h"

AgsmRecruitClient::AgsmRecruitClient()
{
	SetModuleName( "AgsmRecruitClient" );

	//만들어봐요~ 만들어봐요~ 자~자~ 어떤 패킷이 만들어졌을까나? by 치요스케~
	SetPacketType(AGPMRECRUIT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));

	m_csPacket.SetFieldType(AUTYPE_INT8,			1, // Operation 연산 
							AUTYPE_INT32,			1, // lControlServerID
							AUTYPE_INT32,			1, // lPlayerID
							AUTYPE_PACKET,			1, // pvLFPInfo
							AUTYPE_PACKET,			1, // pvLFMInfo
							AUTYPE_PACKET,			1, // pvSearchInfo
							AUTYPE_INT32,			1, // lResult    Result
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

	m_csSearchInfo.SetFlagLength(sizeof(INT8));
	m_csSearchInfo.SetFieldType(
								 AUTYPE_INT8,		1,  //내 레벨을 기준으로 찾는다?
								 AUTYPE_INT32,		1,  //Level은?
								 AUTYPE_INT32,		1,  //몇번째 페이지 인가?
								 AUTYPE_INT32,		1,  //Class BitFlag
		                         AUTYPE_END,		0
								 );
}

AgsmRecruitClient::~AgsmRecruitClient()
{
	;
}


BOOL AgsmRecruitClient::OnAddModule()
{
	m_csCIDManager.InitializeObject( sizeof(CLoginCharInfo **), MAX_RECRUIT_CONNECTION_COUNT );

	m_paAuOLEDBManager = (AuOLEDBManager *)GetModule( "AuOLEDBManager" );
	m_pagsmRecruitServer = (AgsmRecruitServer *)GetModule( "AgsmRecruitServer" );

	m_pagsmRecruitServer->SetCallbackRegisterLFP( CBRegisterLFP, this );
	m_pagsmRecruitServer->SetCallbackCancelLFP( CBCancelLFP, this );
	m_pagsmRecruitServer->SetCallbackSearchLFP( CBSearchLFP, this );
	m_pagsmRecruitServer->SetCallbackRegisterLFM( CBRegisterLFM, this );
	m_pagsmRecruitServer->SetCallbackCancelLFM( CBCancelLFM, this );
	m_pagsmRecruitServer->SetCallbackSearchLFM( CBSearchLFM, this );
	m_pagsmRecruitServer->SetCallbackResult( CBResult, this );

	if( !m_paAuOLEDBManager || !m_pagsmRecruitServer )
		return FALSE;

	return TRUE;
}

UINT32 AgsmRecruitClient::GetulNID( INT32 lCID )
{
	CLoginCharInfo		**ppcLoginCharInfo;
	
	ppcLoginCharInfo = (CLoginCharInfo **)m_csCIDManager.GetObject( lCID );

	if( ppcLoginCharInfo != NULL )
	{
		return (*ppcLoginCharInfo)->m_ulNID;
	}
	else
	{
		return 0;
	}
}

BOOL AgsmRecruitClient::CBRegisterLFP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//결과 패킷을 보낸다.
	AgsmRecruitClient		*pThis;
	INT32					*plCID;
	INT32					*plResult;

	pThis = (AgsmRecruitClient *)pClass;
	plCID = (INT32 *)pData;
	plResult = (INT32 *)pCustData;

	return pThis->SendResult( *plCID, *plResult );
}

BOOL AgsmRecruitClient::CBCancelLFP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//결과 패킷을 클라이언트로 보낸다.
	AgsmRecruitClient		*pThis;
	INT32					*plCID;
	INT32					*plResult;

	pThis = (AgsmRecruitClient *)pClass;
	plCID = (INT32 *)pData;
	plResult = (INT32 *)pCustData;

	return pThis->SendResult( *plCID, *plResult );
}

BOOL AgsmRecruitClient::CBSearchLFP(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//결과 패킷을 클라이언트로 보낸다.
	AgsmRecruitClient		*pThis;
	AgsmRecruitSearchInfo	*pcRecruitSearchInfo;
	INT32					*plCID;

	pThis = (AgsmRecruitClient *)pClass;
	plCID = (INT32 *)pData;
	pcRecruitSearchInfo = (AgsmRecruitSearchInfo *)pCustData;

	pThis->SendLFPInfo( *plCID, 
						pcRecruitSearchInfo->m_lTotalCount,
		                pcRecruitSearchInfo->m_lIndex, 
						pcRecruitSearchInfo->m_lLevel, 
						pcRecruitSearchInfo->m_lClass,
						pcRecruitSearchInfo->m_pstrCharName,
						0 );

	return TRUE;
}

BOOL AgsmRecruitClient::CBRegisterLFM(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRecruitClient		*pThis;
	INT32					*plCID;
	INT32					*plResult;

	pThis = (AgsmRecruitClient *)pClass;
	plCID = (INT32 *)pData;
	plResult = (INT32 *)pCustData;

	return pThis->SendResult( *plCID, *plResult );
}

BOOL AgsmRecruitClient::CBCancelLFM(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRecruitClient		*pThis;
	INT32					*plCID;
	INT32					*plResult;

	pThis = (AgsmRecruitClient *)pClass;
	plCID = (INT32 *)pData;
	plResult = (INT32 *)pCustData;

	return pThis->SendResult( *plCID, *plResult );
}

BOOL AgsmRecruitClient::CBSearchLFM(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRecruitClient		*pThis;
	AgsmRecruitSearchInfo	*pcRecruitSearchInfo;
	INT32					*plCID;

	pThis = (AgsmRecruitClient *)pClass;
	plCID = (INT32 *)pData;
	pcRecruitSearchInfo = (AgsmRecruitSearchInfo *)pCustData;

	return pThis->SendLFMInfo( *plCID,
		                pcRecruitSearchInfo->m_lTotalCount,
						pcRecruitSearchInfo->m_lIndex,
						pcRecruitSearchInfo->m_pstrPurpose,
						pcRecruitSearchInfo->m_lRequireMember,
						pcRecruitSearchInfo->m_lMinLV,
						pcRecruitSearchInfo->m_lMaxLV,
						pcRecruitSearchInfo->m_lLevel,
						pcRecruitSearchInfo->m_lClass,
						pcRecruitSearchInfo->m_pstrCharName,
						pcRecruitSearchInfo->m_lLeaderID );
}

BOOL AgsmRecruitClient::CBResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmRecruitClient		*pThis;

	BOOL			bResult;
	INT32			*plCID;
	INT32			*plResult;

	bResult = FALSE;

	plCID = (INT32 *)pData;
	plResult = (INT32 *)pCustData;
	pThis = (AgsmRecruitClient *)pClass;

	bResult = pThis->SendResult( *plCID, *plResult );

	return bResult;
}

BOOL AgsmRecruitClient::ProcessAddToRecruitServer( INT32 lPlayerID, UINT32 ulNID )
{
	BOOL			bResult;

	bResult = SetIDToPlayerContext( lPlayerID, ulNID );

	if( bResult == TRUE )
	{
		CLoginCharInfo		*pcLoginCharInfo;

		pcLoginCharInfo = new CLoginCharInfo;
		pcLoginCharInfo->m_ulNID = ulNID;

		m_csCIDManager.AddObject( (void **)&pcLoginCharInfo, lPlayerID );
	}

	return bResult;
}

BOOL AgsmRecruitClient::PreProcessRegisterLFP( INT32 lControlServerID, INT32 lPlayerID, void *pvLFPInfo )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFPInfo != NULL )
	{
		char			*pstrCharName;

		pstrCharName = NULL;

		m_csLFPInfo.GetField( FALSE, pvLFPInfo, 0,
								NULL,
								NULL,
								NULL,
								NULL,
								&pstrCharName,
								NULL );

		if( pstrCharName != NULL )
		{
			CLoginCharInfo		**ppcLoginCharInfo;
			
			ppcLoginCharInfo = (CLoginCharInfo **)m_csCIDManager.GetObject( lPlayerID );

			if( ppcLoginCharInfo != NULL )
			{
				int				iCharNameLen;

				iCharNameLen = strlen( pstrCharName ) + 1;
				(*ppcLoginCharInfo)->m_pstrCharName = new char[iCharNameLen];
				memset( (*ppcLoginCharInfo)->m_pstrCharName, 0, iCharNameLen );
				strcat( (*ppcLoginCharInfo)->m_pstrCharName, pstrCharName );

				bResult = m_pagsmRecruitServer->PreProcessRegisterLFP( lControlServerID, lPlayerID );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitClient::PreProcessRegisterLFM( INT32 lControlServerID, INT32 lPlayerID, void *pvLFMInfo )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFMInfo != NULL )
	{
		char			*pstrCharName;

		pstrCharName = NULL;

		m_csLFMInfo.GetField( FALSE, pvLFMInfo, 0,
								NULL,				//TotalCount
								NULL,				//lIndex
								NULL,				//Purpose
								NULL,				//lRequireMembers
								NULL,				//lMinLV
								NULL,				//lMaxLV
								NULL,				//LV
								NULL,				//lClass
								&pstrCharName,		//CharName
								NULL );				//lLeaderID

		if( pstrCharName != NULL )
		{
			CLoginCharInfo		**ppcLoginCharInfo;
			
			ppcLoginCharInfo = (CLoginCharInfo **)m_csCIDManager.GetObject( lPlayerID );

			if( ppcLoginCharInfo != NULL )
			{
				int				iCharNameLen;

				iCharNameLen = strlen( pstrCharName ) + 1;
				(*ppcLoginCharInfo)->m_pstrCharName = new char[iCharNameLen];
				memset( (*ppcLoginCharInfo)->m_pstrCharName, 0, iCharNameLen );
				strcat( (*ppcLoginCharInfo)->m_pstrCharName, pstrCharName );

				//넣어본다.
				bResult = m_pagsmRecruitServer->PreProcessRegisterLFM( lControlServerID, lPlayerID, pvLFMInfo );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitClient::SendRegisterLFP( INT32 lPlayerID, INT32 lResult )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFP;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													NULL,				//lControlServerID
													NULL,				//lPlayerID
													NULL,				//pvLFPInfo
													NULL,				//pvLFMInfo
													NULL,				//pvSearchInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lPlayerID) );
		
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}


BOOL AgsmRecruitClient::SendCancelLFP( INT32 lPlayerID, INT32 lResult )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_CANCEL_LFP;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													NULL,				//lControlServerID
													NULL,				//lPlayerID
													NULL,				//pvLFPInfo
													NULL,				//pvLFMInfo
													NULL,				//pvSearchInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lPlayerID) );
		
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruitClient::SendLFPInfo( INT32 lPlayerID, INT32 lTotalCount, INT32 lIndex, INT32 lLevel, INT32 lClass, char *pstrLFPID, INT32 lLFPID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_SEARCH_LFP_BY_PAGE;
	INT16	nPacketLength;
	void	*pvLFPInfo;

	pvLFPInfo  = m_csLFPInfo.MakePacket(FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
										&lTotalCount,
										&lIndex,
										&lLevel,
										&lClass,
										pstrLFPID,
										&lLFPID );

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													NULL,				//lControlServerID
													NULL,				//lPlayerID
													pvLFPInfo,			//pvLFPInfo
													NULL,				//pvLFMInfo
													NULL,				//pvSearchInfo
													NULL				//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lPlayerID) );
		
		m_csLFPInfo.FreePacket(pvLFPInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruitClient::SendLFMInfo( INT32 lPlayerID, INT32 lTotalCount, INT32 lIndex, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lLevel, INT32 lClass, char *pstrCharName, INT32 lLeaderID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_SEARCH_LFM_BY_PAGE;
	INT16	nPacketLength;
	void	*pvLFMInfo;

	pvLFMInfo  = m_csLFMInfo.MakePacket(FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
										&lTotalCount,		//lTotalCount
										&lIndex,			//lIndex
										pstrPurpose,		//pstrPurpose
										&lRequireMember,	//lRequireMember
										&lMinLV,			//lMinLV
										&lMaxLV,			//lMaxLV
										&lLevel,			//lLevel
										&lClass,			//lClass
										pstrCharName,		//pstrLeaderID
										&lLeaderID			//lLeaderID
										);			

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													NULL,				//lControlServerID
													NULL,				//lPlayerID
													NULL,				//pvLFPInfo
													pvLFMInfo,			//pvLFMInfo
													NULL,				//pvSearchInfo
													NULL				//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lPlayerID) );
		
		m_csLFPInfo.FreePacket(pvLFMInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruitClient::SendResult( INT32 lPlayerID, INT32 lResult )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_RESULT;
	INT16	nPacketLength;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													NULL,				//lControlServerID
													NULL,				//lPlayerID
													NULL,				//pvLFPInfo
													NULL,				//pvLFMInfo
													NULL,				//pvSearchInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lPlayerID) );
		
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruitClient::SendResult( UINT32 ulNID, INT32 lResult )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_RESULT;
	INT16	nPacketLength;
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													NULL,				//lControlServerID
													NULL,				//lPlayerID
													NULL,				//pvLFPInfo
													NULL,				//pvLFMInfo
													NULL,				//pvSearchInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruitClient::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	BOOL			bResult;

	INT32			lControlServerID;
	INT8			lOperation;
	INT32			lPlayerID;
	void			*pvLFPInfo;
	void			*pvLFMInfo;
	void			*pvSearchInfo;
	INT32			lResult;

	bResult = FALSE;

	//큐가 충분한지 확인한다.
	if( m_paAuOLEDBManager->CheckQueueCount() )
	{
		m_csPacket.GetField(TRUE, pvPacket, nSize,
							&lOperation,
							&lControlServerID,
							&lPlayerID,
							&pvLFPInfo,
							&pvLFMInfo,
							&pvSearchInfo,
							&lResult );

		//Recruit Server에 나도 추가해줘~~~ ㅠ.ㅠ
		if( lOperation == AGPMRECRUIT_ADD_TO_RECRUIT )
		{
			bResult = ProcessAddToRecruitServer( lPlayerID, ulNID );
		}
		//DB에 LFP정보를 올린다.
		else if( lOperation == AGPMRECRUIT_REGISTER_LFP )
		{
			bResult = PreProcessRegisterLFP( lControlServerID, lPlayerID, pvLFPInfo );
		}
		//DB에 LFM정보를 올린다.
		else if( lOperation == AGPMRECRUIT_REGISTER_LFM )
		{
			bResult = PreProcessRegisterLFM( lControlServerID, lPlayerID, pvLFMInfo );
		}
		//DB에 올린 LFP를 지운다.
		else if( lOperation == AGPMRECRUIT_CANCEL_LFP )
		{
			bResult = m_pagsmRecruitServer->ProcessCancelLFP( lPlayerID, pvLFPInfo );
		}
		//DB에 올린 LFM을 지운다.
		else if( lOperation == AGPMRECRUIT_CANCEL_LFM )
		{
			bResult = m_pagsmRecruitServer->ProcessCancelLFM( lPlayerID, pvLFMInfo );
		}
		//LFP를 페이지 단위로 검색한다.
		else if( lOperation == AGPMRECRUIT_SEARCH_LFP_BY_PAGE )
		{
			bResult = m_pagsmRecruitServer->ProcessSearchFromLFP( lPlayerID, pvSearchInfo );
		}
		//LFM을 페이지 단위로 검색한다.
		else if( lOperation == AGPMRECRUIT_SEARCH_LFM_BY_PAGE )
		{
			bResult = m_pagsmRecruitServer->ProcessSearchFromLFM( lPlayerID, pvSearchInfo );
		}
		//비정상적인 패킷을 받았다. 잘못 오면? 방법해준다. 쌔워주자.
		else
		{
		}
	}
	//큐가 부족혀~~~
	else
	{
		INT32			lResult;

		lResult = AGPMRECRUIT_RESULT_QUEUE_FULL;

		bResult = SendResult( ulNID, lResult );
	}

	return bResult;
}

//접속이 끊기면 이리루온다. 아햏햏~
BOOL AgsmRecruitClient::OnDisconnect(INT32 lAccountID)
{
	//lAccountID는 AGPMRECRUIT_ADD_TO_RECRUIT를 받고 CID, dpnid를 받은뒤에 
	//SetIDToPlayerContext의 조합되에 발생된 값이다. AccountID == CID이다.
	return m_csCIDManager.RemoveObject( lAccountID );
}
