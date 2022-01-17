#include <string.h>
#include <stdio.h>
#include "AgsmRecruitServer.h"

AgsmRecruitServer::AgsmRecruitServer()
{
	SetModuleName( "AgsmRecruitServer" );

	//만들어봐요~ 만들어봐요~ 자~자~ 어떤 패킷이 만들어졌을까나? by 치요스케~
	SetPacketType(AGSMRECRUIT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  연산
							AUTYPE_INT32,			1, // lLFPID  Recruit에 올린놈 ID
							AUTYPE_PACKET,			1, // LFP에 관한 세부정보.
							AUTYPE_PACKET,			1, // LFM에 관한 세부정보.
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

	m_csSearchInfo.SetFlagLength(sizeof(INT8));
	m_csSearchInfo.SetFieldType(
								 AUTYPE_INT8,		1,  //내 레벨을 기준으로 찾는다?
								 AUTYPE_INT32,		1,  //Level은?
								 AUTYPE_INT32,		1,  //몇번째 페이지 인가?
								 AUTYPE_INT32,		1,  //Class BitFlag
		                         AUTYPE_END,		0
								 );

}

AgsmRecruitServer::~AgsmRecruitServer()
{
}

//어디서나 볼 수 있는 OnAddModule()
BOOL AgsmRecruitServer::OnAddModule()
{
	m_csRegisterCID.InitializeObject( sizeof(INT32), MAX_RECRUIT_CONNECTION_COUNT );

	m_paAuOLEDBManager = (AuOLEDBManager *)GetModule( "AuOLEDBManager" );
	m_pagsmServerManager = (AgsmServerManager *)GetModule( "AgsmServerManager2" );

	if( !m_paAuOLEDBManager || !m_pagsmServerManager )
	{
		return FALSE;
	}

	return TRUE;
}

//GameServer(ControlServer)에 내가 있는지 확인하고 내 정보를 받아온다.
BOOL AgsmRecruitServer::SendRegisterLFP( INT32 lControlServerID, INT32 lLFPID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFP;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													NULL,				//LFPInfo
													NULL,				//LFMInfo
													NULL				//lResult
													);

	if( pvPacket != NULL )
	{
		AgsdServer			*pcsAgsdServer;

		//당신을 컨트롤 서버로 임명합니다.
		pcsAgsdServer = m_pagsmServerManager->GetServer( lControlServerID );

		//컨트롤 서버가 있으면 보내주자!! 아햏햏~
		if( pcsAgsdServer != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;	
}

BOOL AgsmRecruitServer::SendRegisterLFM( INT32 lControlServerID, INT32 lLFPID, void *pvLFMInfo )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													NULL,				//LFPInfo
													pvLFMInfo,			//LFMInfo
													NULL				//lResult
													);

	if( pvPacket != NULL )
	{
		AgsdServer			*pcsAgsdServer;

		//당신을 컨트롤 서버로 임명합니다.
		pcsAgsdServer = m_pagsmServerManager->GetServer( lControlServerID );

		//컨트롤 서버가 있으면 보내주자!! 아햏햏~
		if( pcsAgsdServer != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;	
}

BOOL AgsmRecruitServer::PreProcessRegisterLFP( INT32 lControlServerID, INT32 lLFPID )
{
	BOOL			bResult;
	void			*pvResult;

	//등록된 놈인지 확인한다.
	pvResult = m_csRegisterCID.GetObject( lLFPID );

	//등록이 된 놈이군. 또 등록? 드랍해준다.
	if( pvResult != NULL )
	{
	}
	//아니라면 넣을수 있다. GameServer로 정보를 요청한다.
	else
	{
		bResult = SendRegisterLFP( lControlServerID, lLFPID );
	}

	return bResult;
}

BOOL AgsmRecruitServer::PreProcessRegisterLFM( INT32 lControlServerID, INT32 lLFPID, void *pvLFMInfo )
{
	BOOL			bResult;
	void			*pvResult;

	//등록된 놈인지 확인한다.
	pvResult = m_csRegisterCID.GetObject( lLFPID );

	//등록이 된 놈이군. 또 등록? 드랍해준다.
	if( pvResult != NULL )
	{
	}
	//아니라면 넣을수 있다. GameServer로 정보를 요청한다.
	else
	{
		bResult = SendRegisterLFM( lControlServerID, lLFPID, pvLFMInfo );
	}

	return bResult;
}

//DB Insert 한다.
BOOL AgsmRecruitServer::ProcessRegisterLFP( INT32 lLFPID, void *pvLFPInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFPInfo != NULL )
	{
		char			*pstrCharName;

		INT32			lLevel;
		INT32			lClass;

		pstrCharName = NULL;

		m_csLFPInfo.GetField( FALSE, pvLFPInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								&lLevel,			//lLevel
								&lClass,			//lClass
								&pstrCharName,		//CharName
								NULL				//lPlaerID
								);

		if( pstrCharName != NULL )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			int				lCharNameLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_INSERT_LFP;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			pcQueryQueue->m_lLV = lLevel;

			//클래스 정의가 없어서 아래와 같이 가라로 변경!
			//또 가라로 만들었으니 갈아 엎어야겠지?
			if( lClass == 1 )
			{
				lClass = 1;
			}
			else if( lClass == 2 )
			{
				lClass = 2;
			}
			else if( lClass == 3 )
			{
				lClass = 4;
			}
			else if( lClass == 4 )
			{
				lClass = 8;
			}

			pcQueryQueue->m_lClass = lClass;

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

//DB Insert 한다.
BOOL AgsmRecruitServer::ProcessRegisterLFM( INT32 lLFMID, void *pvLFMInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFMInfo != NULL )
	{
		char			*pstrCharName;
		char			*pstrPurpose;

		INT32			lRequireMember;
		INT32			lMinLV;
		INT32			lMaxLV;
		INT32			lLevel;
		INT32			lClass;
		INT32			lLeaderID;

		pstrCharName = NULL;

		m_csLFMInfo.GetField( FALSE, pvLFMInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								&pstrPurpose,		//purpose
								&lRequireMember,	//lRequireMember
								&lMinLV,			//MinLV
								&lMaxLV,			//MaxLV
								&lLevel,			//Level
								&lClass,			//Class
								&pstrCharName,		//CharName
								&lLeaderID			//lLeaderID
								);

		if( (pstrCharName != NULL) && (pstrPurpose != NULL) )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			INT32				lCharNameLen;
			INT32				lPurposeLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;
			lPurposeLen = strlen( pstrPurpose ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_INSERT_LFM;
			pcQueryQueue->m_lCID = lLFMID;

			pcQueryQueue->m_pstrPurpose = new char[lPurposeLen];
			memset( pcQueryQueue->m_pstrPurpose, 0, lPurposeLen );
			strcat( pcQueryQueue->m_pstrPurpose, pstrPurpose );

			pcQueryQueue->m_lRequireMember = lRequireMember;
			pcQueryQueue->m_lMinLV = lMinLV;
			pcQueryQueue->m_lMaxLV = lMaxLV;
			pcQueryQueue->m_lLV = lLevel;

			//클래스 정의가 없어서 아래와 같이 가라로 변경!
			//또 가라로 만들었으니 갈아 엎어야겠지?
			if( lClass == 1 )
			{
				lClass = 1;
			}
			else if( lClass == 2 )
			{
				lClass = 2;
			}
			else if( lClass == 3 )
			{
				lClass = 4;
			}
			else if( lClass == 4 )
			{
				lClass = 8;
			}

			pcQueryQueue->m_lClass = lClass;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			pcQueryQueue->m_lLeaderID = lLeaderID;

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessCancelLFP( INT32 lLFPID, void *pvLFPInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFPInfo != NULL )
	{
		char			*pstrCharName;

		pstrCharName = NULL;

		m_csLFPInfo.GetField( FALSE, pvLFPInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								NULL,				//lLevel
								NULL,				//lClass
								&pstrCharName,		//CharName
								NULL				//lPlaerID
								);

		if( pstrCharName != NULL )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			INT32			lCharNameLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_DELETE_LFP;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessCancelLFM( INT32 lLFMID, void *pvLFMInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFMInfo )
	{
		char			*pstrCharName;

		pstrCharName = NULL;

		m_csLFMInfo.GetField(FALSE, pvLFMInfo, 0,
									NULL,			//lTotalCount
									NULL,			//lIndex
									NULL,			//Purpose
									NULL,			//lRequireMemeber
									NULL,			//MinLV
									NULL,			//MaxLV
									NULL,			//LV
									NULL,			//Class
									&pstrCharName,	//CharName
									NULL			//lLeaderID
									);

		if( pstrCharName != NULL )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			INT32			lCharNameLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_DELETE_LFM;
			pcQueryQueue->m_lCID = lLFMID;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessSearchFromLFP( INT32 lLFPID, void *pvSearchInfo, BOOL bCheckQueue )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvSearchInfo != NULL )
	{
		AgsmRecruitQueueInfo		*pcQueryQueue;

		bool				bSearchByLevel;
		INT32				lLevel;
		INT32				lPage;
		INT32				lClass;

		m_csSearchInfo.GetField( FALSE, pvSearchInfo, 0,
								&bSearchByLevel,
								&lLevel,
								&lPage,
								&lClass
								);

		pcQueryQueue = new AgsmRecruitQueueInfo;
		pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_SEARCH_LFP;
		pcQueryQueue->m_bSearchByLevel = bSearchByLevel;
		pcQueryQueue->m_lLV = lLevel;
		pcQueryQueue->m_lPage = lPage;
		pcQueryQueue->m_lCID = lLFPID;
		pcQueryQueue->m_lClass = lClass;

		if( bCheckQueue )
		{
			bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
		else
		{
			bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessSearchFromLFM( INT32 lLFMID, void *pvSearchInfo, BOOL bCheckQueue )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvSearchInfo != NULL )
	{
		AgsmRecruitQueueInfo		*pcQueryQueue;

		bool				bSearchByLevel;
		INT32				lLevel;
		INT32				lPage;
		INT32				lClass;

		m_csSearchInfo.GetField( FALSE, pvSearchInfo, 0,
								&bSearchByLevel,
								&lLevel,
								&lPage,
								&lClass
								);

		pcQueryQueue = new AgsmRecruitQueueInfo;
		pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_SEARCH_LFM;
		pcQueryQueue->m_bSearchByLevel = bSearchByLevel;
		pcQueryQueue->m_lCID = lLFMID;
		pcQueryQueue->m_lClass = lClass;
		pcQueryQueue->m_lLV = lLevel;
		pcQueryQueue->m_lPage = lPage;

		if( bCheckQueue )
		{
			bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
		else
		{
			bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::SetCallbackRegisterLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_REGISTER_LFP, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackCancelLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_CANCEL_LFP, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackSearchLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_SEARCH_LFP, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackRegisterLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_REGISTER_LFM, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackCancelLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_CANCEL_LFM, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackSearchLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_SEARCH_LFM, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_RESULT, pfCallback, pClass);
}


BOOL AgsmRecruitServer::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
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

	if( lOperation == AGPMRECRUIT_REGISTER_LFP )
	{
		bResult = ProcessRegisterLFP( lCID, pvLFPInfo, FALSE );
	}
	else if( lOperation == AGPMRECRUIT_REGISTER_LFM )
	{
		bResult = ProcessRegisterLFM( lCID, pvLFMInfo, FALSE );
	}
	else if( lOperation == AGPMRECRUIT_UPDATE_LFP )
	{
		//bResult = ProcessUpdateLFP( lCID, pvLFPInfo );
	}
	//DB에 올린 PlayerID를 지운다.
	else if( lOperation == AGPMRECRUIT_CANCEL_LFP )
	{
		bResult = ProcessCancelLFP( lCID, pvLFPInfo, FALSE );
	}
	else if( lOperation == AGPMRECRUIT_CANCEL_LFM )
	{
		bResult = ProcessCancelLFM( lCID, pvLFPInfo, FALSE );
	}
	//비정상적인 패킷을 받았다. 잘못 오면? 방법해준다. 내가 잘못한건데? 허허~
	else
	{
		
	}

	return bResult;
}
