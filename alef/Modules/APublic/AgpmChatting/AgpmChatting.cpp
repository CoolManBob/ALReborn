//	AgpmChatting.cpp
////////////////////////////////////////////////////////////////

#include "AgpmChatting.h"

AgpmChatting::AgpmChatting()
{
	SetModuleName("AgpmChatting");

	SetPacketType(AGPMCHATTING_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,									// Operation
							AUTYPE_INT32,			1,									// CID
							AUTYPE_INT8,			1,									// Chat Type
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Sender ID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Target ID
							AUTYPE_MEMORY_BLOCK,	1,									// chatting message
							AUTYPE_END,				0
							);

	m_lIndexADChar	= 0;
	m_lMaxCommands	= AGPMCHAT_MAX_COMMANDS;
}

AgpmChatting::~AgpmChatting()
{
}

BOOL AgpmChatting::OnAddModule()
{
	m_pcsAgpmCharacter	= (AgpmCharacter *)	GetModule("AgpmCharacter");
	m_pcsAgpmParty		= (AgpmParty *)		GetModule("AgpmParty");

	if (!m_pcsAgpmCharacter || !m_pcsAgpmParty)
		return FALSE;

	m_lIndexADChar = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdChattingADChar), ConAgpdCharacter, DesAgpdCharacter);
	if (m_lIndexADChar < 0)
		return FALSE;

	return TRUE;
}

BOOL AgpmChatting::OnInit()
{
	if (!m_csChatCommands.InitializeObject(sizeof(AgpdChatCommand), m_lMaxCommands))
		return FALSE;

	return TRUE;
}

BOOL AgpmChatting::OnDestroy()
{
	return TRUE;
}

BOOL AgpmChatting::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation		= 0;
	INT32	lCID			= 0;
	INT8	cChatType		= AGPDCHATTING_TYPE_MAX;
	CHAR*	szSenderName	= NULL;
	CHAR*	szTargetID		= NULL;
	CHAR*	szMessage		= NULL;
	UINT16	unMessageLength	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&cChatType,
						&szSenderName,
						&szTargetID,
						&szMessage, &unMessageLength);

	if (!pstCheckArg->bReceivedFromServer &&
		pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		lCID != AP_INVALID_CID &&
		pstCheckArg->lSocketOwnerID != lCID)
		return FALSE;

	if (unMessageLength + sizeof(PACKET_HEADER) + m_csPacket.GetFlagLength() > (UINT16) nSize)
		return FALSE;

	if (unMessageLength > 128)
		return FALSE;

	/*
	DWORD	dwThreadID	= GetCurrentThreadId();
	CHAR	szFileName[32];
	ZeroMemory(szFileName, sizeof(szFileName));

#ifdef	__PROFILE__
	sprintf(szFileName, "ChatPacket_%d.dat", dwThreadID);
	FILE	*fpChatPacket	= fopen(szFileName, "wb");
	if (fpChatPacket)
	{
		fwrite(pvPacket, sizeof(CHAR), nSize, fpChatPacket);

		fclose(fpChatPacket);
	}
#endif	//__PROFILE__
	*/

	AgpdCharacter *pcsCharacter= m_pcsAgpmCharacter->GetCharacterLock(lCID);

	// Chatting 불가능 캐릭터라면 채팅을 아예 원천봉쇄한다.
	if(m_pcsAgpmCharacter->IsDisableChattingCharacter(pcsCharacter) == TRUE)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation) {
	case AGPDCHATTING_OPERATION_CHAT:
		OnOperationChat(pcsCharacter, cChatType, szSenderName, szTargetID, szMessage, unMessageLength);
		break;

	case AGPDCHATTING_OPERATION_SET_BLOCK_WHISPER:
		OnOperationSetBlockWhisper(pcsCharacter);
		break;

	case AGPDCHATTING_OPERATION_RELEASE_BLOCK_WHISPER:
		OnOperationReleaseBlockWhisper(pcsCharacter);
		break;

	case AGPDCHATTING_OPERATION_REPLY_BLOCK_WHISPER:
		OnOperationReplyBlockWhisper(pcsCharacter, szTargetID);
		break;

	case AGPDCHATTING_OPERATION_OFFLINE_WHISPER_TARGET:
		OnOperationOfflineWhisperTarget(pcsCharacter, szTargetID);
		break;
	}

	// 2004.01.16. steeple - NULL Check
	if(pcsCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL	AgpmChatting::OperationChat( AgpdCharacter *pcsCharacter , char * pMessage )
{
	if( !pcsCharacter ) return FALSE;

	AgpdChatData	stChatData;
	ZeroMemory(&stChatData, sizeof(AgpdChatData));

	stChatData.eChatType		= AGPDCHATTING_TYPE_NORMAL;
	stChatData.lSenderID		= (pcsCharacter) ? pcsCharacter->m_lID : AP_INVALID_CID;
	stChatData.szMessage		= pMessage;
	stChatData.lMessageLength	= (UINT32) strlen( pMessage );
	stChatData.pcsSenderBase	= (ApBase *) pcsCharacter;
	stChatData.szSenderName		= pcsCharacter->m_szID;

	EnumCallback(AGPMCHATTING_CB_RECV_NORMAL_MSG, &stChatData, NULL);

	return TRUE;
}


BOOL AgpmChatting::OnOperationChat(AgpdCharacter *pcsCharacter, INT8 cChatType, CHAR *szSenderName, CHAR *szTargetID, CHAR *szMessage, UINT16 unMessageLength)
{
	if (!szMessage || unMessageLength < 1)
		return FALSE;

	AgpdChatData	stChatData;
	ZeroMemory(&stChatData, sizeof(AgpdChatData));

	stChatData.eChatType		= (AgpdChattingType) cChatType;
	stChatData.lSenderID		= (pcsCharacter) ? pcsCharacter->m_lID : AP_INVALID_CID;
	stChatData.szSenderName		= szSenderName;
	stChatData.szMessage		= szMessage;
	stChatData.lMessageLength	= (UINT32) unMessageLength;
	
	// 2003.11.18. 공지사항이라면 Character 를 얻을 필요가 없다.
	// Client 만 처리하게 된다.
	if(cChatType == AGPDCHATTING_TYPE_WHOLE_WORLD)
	{
		EnumCallback(AGPMCHATTING_CB_RECV_WHOLE_WORLD_MSG, &stChatData, NULL);
		return TRUE;
	}

	// 채팅 Ban 상태인지 확인한다. - 2004.03.31. steeple
	//INT8 cChatEnable[2];
	ApSafeArray<INT8, 2>	cChatEnable;

	cChatEnable[0] = cChatType; cChatEnable[1] = 1;
	EnumCallback(AGPMCHATTING_CB_CHECK_ENABLE, pcsCharacter, &cChatEnable[0]);
	if(!cChatEnable[1])
	{
		if(pcsCharacter)
			pcsCharacter->m_Mutex.Release();
		return TRUE;
	}

	stChatData.pcsSenderBase = (ApBase *) pcsCharacter;

	if (pcsCharacter)
		stChatData.szSenderName	= pcsCharacter->m_szID;

	switch (cChatType) {
	case AGPDCHATTING_TYPE_NORMAL:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_NORMAL_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_WORD_BALLOON:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_WORD_BALLOON_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_UNION:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_UNION_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_GUILD:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_GUILD_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_PARTY:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_PARTY_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_WHISPER:
		{
			if (TRUE == EnumCallback(AGPMCHATTING_CB_CHECK_BLOCK_WHISPER_MSG, szSenderName, NULL))
				break;

			if (pcsCharacter)
			{
				AgpdCharacter *pcsTargetChar = m_pcsAgpmCharacter->GetCharacter(szTargetID);
				if (pcsTargetChar && pcsTargetChar != pcsCharacter)
				{
					stChatData.lTargetID		= pcsTargetChar->m_lID;
					stChatData.pcsTargetBase	= (ApBase *) pcsTargetChar;
				}

				stChatData.szTargetName		= szTargetID;
			}

			EnumCallback(AGPMCHATTING_CB_RECV_WHISPER_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_WHOLE_WORLD:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_WHOLE_WORLD_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_SYSTEM_LEVEL1:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_SYSTEM_LEVEL1_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_SYSTEM_LEVEL2:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_SYSTEM_LEVEL2_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_SYSTEM_LEVEL3:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_SYSTEM_LEVEL3_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_GUILD_JOINT:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_GUILD_JOINT_MSG, &stChatData, NULL);
		}
		break;

	case AGPDCHATTING_TYPE_EMPHASIS:
		{
			EnumCallback(AGPMCHATTING_CB_RECV_EMPHASIS_MSG, &stChatData, NULL);
		}
		break;
	}

	return TRUE;
}

BOOL AgpmChatting::OnOperationSetBlockWhisper(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;
	
	AgpdChattingADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	pcsAttachData->m_bIsBlockWhisper	= TRUE;

	return TRUE;
}

BOOL AgpmChatting::OnOperationReleaseBlockWhisper(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdChattingADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	pcsAttachData->m_bIsBlockWhisper	= FALSE;

	return TRUE;
}

BOOL AgpmChatting::OnOperationReplyBlockWhisper(AgpdCharacter *pcsCharacter, CHAR *szTargetID)
{
	if (!pcsCharacter || !szTargetID || !szTargetID[0])
		return FALSE;

	return EnumCallback(AGPMCHATTING_CB_RECV_REPLY_BLOCK_WHISPER, pcsCharacter, szTargetID);
}

BOOL AgpmChatting::OnOperationOfflineWhisperTarget(AgpdCharacter *pcsCharacter, CHAR *szTargetID)
{
	if (!pcsCharacter || !szTargetID || !szTargetID[0])
		return FALSE;

	return EnumCallback(AGPMCHATTING_CB_RECV_OFFLINE_WHISPER_TARGET, pcsCharacter, szTargetID);
}

//		MakePacketChatting
//	Functions
//		- chatting packet을 맹근다.
//			eChatType 에 따라 lTargetID에 해당 타입의 타겟 아뒤를 넘겨준다.
//	Arguments
//		- lCID : message를 보내는 넘 아뒤
//		- eChatType : 	AGPDCHATTING_TYPE_NORMAL
//						AGPDCHATTING_TYPE_UNION
//						AGPDCHATTING_TYPE_GUILD
//						AGPDCHATTING_TYPE_PARTY
//						AGPDCHATTING_TYPE_WHISPER
//						AGPDCHATTING_TYPE_WHOLE_WORLD
//						AGPDCHATTING_TYPE_SYSTEM_LEVEL1
//						AGPDCHATTING_TYPE_SYSTEM_LEVEL2
//						AGPDCHATTING_TYPE_SYSTEM_LEVEL3
//						AGPMCHATTING_CB_RECV_GUILD_JOINT_MSG
//		- szSenderName : 보내는 넘 이름
//		- szTargetID : 위 chat type에 따른 대상 아뒤 (귀속말 등등...)
//		- szMessage : message string
//		- lMessageLength : message string length
//		- pnPacketLength : 만들어진 패킷 길이 리턴
//	Return value
//		- PVOID : 만들어진 패킷 (NULL 이면 실패)
///////////////////////////////////////////////////////////////////////////////
PVOID AgpmChatting::MakePacketChatting(INT32 lCID, AgpdChattingType eChatType, CHAR *szSenderName,
									   CHAR *szTargetID,
									   CHAR *szMessage, INT32 lMessageLength, INT16 *pnPacketLength)
{
	// validation check
	if (lCID == AP_INVALID_CID || !szMessage || lMessageLength < 1 || !strlen(szMessage) || lMessageLength >= 256)
		return NULL;
	if (eChatType < 0 || eChatType >= AGPDCHATTING_TYPE_MAX)
		return NULL;

	INT8	cOperation	= AGPDCHATTING_OPERATION_CHAT;
	INT8	cChatType = (INT8) eChatType;

	// make packet and return packet
	return  m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHATTING_PACKET_TYPE,
											&cOperation,
											&lCID,
											&cChatType,
											szSenderName,
											szTargetID,
											szMessage,	&lMessageLength);
}

PVOID AgpmChatting::MakePacketSetBlockWhisper(INT32 lCID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDCHATTING_OPERATION_SET_BLOCK_WHISPER;

	return  m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHATTING_PACKET_TYPE,
											&cOperation,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmChatting::MakePacketReleaseBlockWhisper(INT32 lCID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDCHATTING_OPERATION_RELEASE_BLOCK_WHISPER;

	return  m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHATTING_PACKET_TYPE,
											&cOperation,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL);
}

PVOID AgpmChatting::MakePacketReplyBlockWhisper(INT32 lCID, CHAR *szTargetID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !szTargetID || !szTargetID[0] || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDCHATTING_OPERATION_REPLY_BLOCK_WHISPER;

	return  m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHATTING_PACKET_TYPE,
											&cOperation,
											&lCID,
											NULL,
											NULL,
											szTargetID,
											NULL);
}

PVOID AgpmChatting::MakePacketOfflineWhisperTarget(INT32 lCID, CHAR *szTargetID, INT16 *pnPacketLength)
{
	if (lCID == AP_INVALID_CID || !szTargetID || !szTargetID[0] || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGPDCHATTING_OPERATION_OFFLINE_WHISPER_TARGET;

	return  m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHATTING_PACKET_TYPE,
											&cOperation,
											&lCID,
											NULL,
											NULL,
											szTargetID,
											NULL);
}

BOOL AgpmChatting::SetCallbackRecvNormalMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_NORMAL_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvWordBalloonMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_WORD_BALLOON_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvUnionMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_UNION_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvGuildMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_GUILD_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvPartyMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_PARTY_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvWhisperMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_WHISPER_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvWholeWorldMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_WHOLE_WORLD_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvSystemLevel1Msg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_SYSTEM_LEVEL1_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvSystemLevel2Msg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_SYSTEM_LEVEL2_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvSystemLevel3Msg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_SYSTEM_LEVEL3_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackCheckEnable(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_CHECK_ENABLE, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackCheckBlockWhisper(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_CHECK_BLOCK_WHISPER_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvReplyBlockWhisper(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_REPLY_BLOCK_WHISPER, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvOfflineWhisperTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_OFFLINE_WHISPER_TARGET, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvGuildJointMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_GUILD_JOINT_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::SetCallbackRecvEmphasisMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHATTING_CB_RECV_EMPHASIS_MSG, pfCallback, pClass);
}

BOOL AgpmChatting::ConAgpdCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
	{
		// invalid constructor parameter
		return FALSE;
	}

	AgpmChatting		*pThis			= (AgpmChatting *)	pClass;
	AgpdCharacter		*pCharacter		= (AgpdCharacter *)	pData;

	AgpdChattingADChar *pAgsdCharacter	= pThis->GetADCharacter(pCharacter);

	if (pAgsdCharacter)
	{
		pAgsdCharacter->m_lLastWhisperSenderID	= AP_INVALID_CID;
		pAgsdCharacter->m_bIsBlockWhisper		= FALSE;
	}

	return TRUE;
}

BOOL AgpmChatting::DesAgpdCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

AgpdChattingADChar*	AgpmChatting::GetADCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	return (AgpdChattingADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexADChar, (PVOID) pcsCharacter);
}

INT32 AgpmChatting::GetLastWhisperSenderID(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return AP_INVALID_CID;

	AgpdChattingADChar *pcsAttachData = GetADCharacter(pcsCharacter);
	
	return pcsAttachData->m_lLastWhisperSenderID;
}

BOOL AgpmChatting::AddCommand(CHAR *szCommand, ApModuleDefaultCallBack fnCallback, PVOID pvClass)
{
	if (!szCommand)
		return FALSE;
	
	AgpdChatCommand	stChatCommand;
	INT32			lCommandLen = (INT32)strlen(szCommand);

	stChatCommand.m_szCommand	= new CHAR[lCommandLen + 1];
	strcpy(stChatCommand.m_szCommand, szCommand);
	stChatCommand.m_pvClass		= pvClass;
	stChatCommand.m_fnCallback	= fnCallback;

	if (!m_csChatCommands.AddObject(&stChatCommand, szCommand))
	{
		delete [] stChatCommand.m_szCommand;
		return FALSE;
	}

	return TRUE;
}

AgpdChatCommand * AgpmChatting::GetCommand(CHAR *szCommand)
{
	return (AgpdChatCommand *) m_csChatCommands.GetObject(szCommand);
}

BOOL AgpmChatting::RemoveCommand(CHAR *szCommand)
{
	return m_csChatCommands.RemoveObject(szCommand);
}

BOOL AgpmChatting::DispatchCommand(CHAR *szChatMessage)
{
	if (!szChatMessage)
		return FALSE;

	CHAR *				szCommandEnd;
	AgpdChatCommand *	pstCommand;

	szCommandEnd = strchr(szChatMessage, ' ');
	if (szCommandEnd)
	{
		*szCommandEnd = 0;
		pstCommand = GetCommand(szChatMessage);
		*szCommandEnd = ' ';
	}
	else
		pstCommand = GetCommand(szChatMessage);

	if (!pstCommand)
		return FALSE;

	if (pstCommand->m_fnCallback)
		return pstCommand->m_fnCallback(szChatMessage, pstCommand->m_pvClass, NULL);

	return TRUE;
}
