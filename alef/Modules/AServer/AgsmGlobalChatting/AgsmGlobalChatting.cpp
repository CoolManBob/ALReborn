#include "AgsmGlobalChatting.h"
#include "AgsmItem.h"

AgsmGlobalChatting::AgsmGlobalChatting()
{
	SetModuleName("AgsmGlobalChatting");
	SetPacketType(AGSMGLOBALCHATTING_PACKET_TYPE);
	SetModuleType(APMODULE_TYPE_SERVER);

	m_pcsAgsmServerManager2	= NULL;

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,									// operation
							AUTYPE_INT8,			1,									// Chat Type
							AUTYPE_MEMORY_BLOCK,	1,									// Sender Name
							AUTYPE_MEMORY_BLOCK,	1,									// Server Name
							AUTYPE_MEMORY_BLOCK,	1,									// chatting message
							AUTYPE_END,				0
							);

	m_Mutex.Init();

	m_nLoginServerIndex = -1;

	EnableIdle(TRUE);
}

AgsmGlobalChatting::~AgsmGlobalChatting()
{
	m_Mutex.Destroy();
}

BOOL AgsmGlobalChatting::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgsmItem			= (AgsmItem *)				GetModule("AgsmItem");
	m_pcsAgsmServerManager2	= (AgsmServerManager2 *)	GetModule("AgsmServerManager2");
	m_pcsAgsmChatting		= (AgsmChatting *)			GetModule("AgsmChatting");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgsmItem ||
		!m_pcsAgsmServerManager2 ||
		!m_pcsAgsmChatting)
		return FALSE;

	if (!m_pcsAgsmChatting->SetCallbackAreaChattingGlobal(CBAreaChattingGlobal, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmGlobalChatting::OnDestroy()
{
	return TRUE;
}

BOOL AgsmGlobalChatting::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

PVOID AgsmGlobalChatting::MakePacketSendMessage(AgsmGlobalChatType eChatType, CHAR *pszCharName, CHAR *pszServerName, CHAR *pszMessage, UINT16 unMessageLength, INT16 *pnPacketLength, BOOL bIsBroadCast)
{
	if (!pnPacketLength || !pszMessage || !pszMessage[0] || unMessageLength < 1)
		return NULL;

	INT8	cOperation;
	if (bIsBroadCast)
		cOperation	= AGSMGLOBALCHAT_OPERATION_BROADCAST_MESSAGE;
	else
		cOperation	= AGSMGLOBALCHAT_OPERATION_SEND_MESSAGE;

	INT8	cChatType	= (INT8)	eChatType;

	UINT16	unCharNameLength	= (UINT16) strlen(pszCharName);
	UINT16	unServerNameLength	= (UINT16) strlen(pszServerName);

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGLOBALCHATTING_PACKET_TYPE,
									&cOperation,
									&cChatType,
									pszCharName, &unCharNameLength,
									pszServerName, &unServerNameLength,
									pszMessage, &unMessageLength);
}

BOOL AgsmGlobalChatting::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < sizeof(PACKET_HEADER) || !pstCheckArg)
		return FALSE;

	//if (!pstCheckArg->bReceivedFromServer)
	//	return FALSE;

	INT8	cOperation	= 0;
	INT8	cChatType	= 0;

	CHAR	*pszSenderName			= NULL;
	CHAR	*pszServerName			= NULL;
	CHAR	*pszChatMessage			= NULL;

	UINT16	unSenderNameLength		= 0;
	UINT16	unServerNameLength		= 0;
	UINT16	unChatMessageLength		= 0;

	CHAR	szSenderName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR	szServerName[AGSM_MAX_SERVER_NAME + 1];

	ZeroMemory(szSenderName, sizeof(szSenderName));
	ZeroMemory(szServerName, sizeof(szServerName));

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&cChatType,
						&pszSenderName, &unSenderNameLength,
						&pszServerName, &unServerNameLength,
						&pszChatMessage, &unChatMessageLength);

	if (unSenderNameLength > 0 &&
		unSenderNameLength <= AGPDCHARACTER_MAX_ID_LENGTH)
		CopyMemory(szSenderName, pszSenderName, sizeof(CHAR) * unSenderNameLength);
	if (unServerNameLength > 0 &&
		unServerNameLength <= AGSM_MAX_SERVER_NAME)
		CopyMemory(szServerName, pszServerName, sizeof(CHAR) * unServerNameLength);

	AuAutoLock	lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	switch (cOperation) {
		case AGSMGLOBALCHAT_OPERATION_SEND_MESSAGE:
			OnOperationSendMessage((AgsmGlobalChatType) cChatType, szSenderName, szServerName, pszChatMessage, unChatMessageLength);
			break;

		case AGSMGLOBALCHAT_OPERATION_BROADCAST_MESSAGE:
			OnOperationBroadCastMessage((AgsmGlobalChatType) cChatType, szSenderName, szServerName, pszChatMessage, unChatMessageLength);
			break;
	}

	return TRUE;
}

BOOL AgsmGlobalChatting::OnOperationSendMessage(AgsmGlobalChatType eChatType, CHAR *pszSenderName, CHAR *pszServerName, CHAR *pszChatMessage, UINT16 unChatMessageLength)
{
	if (!pszChatMessage || !pszChatMessage[0])
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketSendMessage(eChatType, pszSenderName, pszServerName, pszChatMessage, unChatMessageLength, &nPacketLength, TRUE);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	AgsdServer* pcsAgsdServer = NULL;
	INT16 lIndex = 0;

	while((pcsAgsdServer = m_pcsAgsmServerManager2->GetGameServers(&lIndex)) != NULL)
		SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmGlobalChatting::OnOperationBroadCastMessage(AgsmGlobalChatType eChatType, CHAR *pszSenderName, CHAR *pszServerName, CHAR *pszChatMessage, UINT16 unChatMessageLength)
{
	if (!pszChatMessage || !pszChatMessage[0])
		return FALSE;

	// 받은 메시지를 채팅 쓰레드로 넘겨준다.
	PVOID	pvBuffer[5];
	pvBuffer[0]	= (PVOID) eChatType;
	pvBuffer[1]	= (PVOID) pszSenderName;
	pvBuffer[2]	= (PVOID) pszServerName;
	pvBuffer[3]	= (PVOID) pszChatMessage;
	pvBuffer[4]	= (PVOID) unChatMessageLength;

	EnumCallback(AGSMGLOBALCHAT_CB_RECEIVE_BROADCAST_MESSAGE, pvBuffer, NULL);

	return TRUE;
}

BOOL AgsmGlobalChatting::SetCallbackReceiveBroadCastMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGLOBALCHAT_CB_RECEIVE_BROADCAST_MESSAGE, pfCallback, pClass);
}

BOOL AgsmGlobalChatting::SendGlobalChatMessage(AgpdCharacter *pcsCharacter, CHAR *pszMessage, UINT16 unMessageLength)
{
	if (!pcsCharacter || !pszMessage || !pszMessage[0] || unMessageLength < 1)
		return FALSE;

	AgsdServer2 *pcsLoginServer = NextLoginServer();

	if(!pcsLoginServer)
		return FALSE;

	AgsdServer2	*pcsThisServer		= m_pcsAgsmServerManager2->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketSendMessage(AGSMGLOBALCHAT_TYPE_NORMAL, pcsCharacter->m_szID, pcsThisServer->m_szGroupName, pszMessage, unMessageLength, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	// 필요한 유료템이 있다면 여기서 소모시킨다.
	if (!m_pcsAgsmItem->UseItemAreaChattingGlobal(pcsCharacter))
		return FALSE;

	BOOL	bSendResult	= SendPacketSvr(pvPacket, nPacketLength, pcsLoginServer->m_dpnidServer);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmGlobalChatting::SendGlobalNotifyMessage(CHAR *pszMessage, UINT16 unMessageLength)
{
	if (!pszMessage || !pszMessage[0] || unMessageLength < 1)
		return FALSE;

	AgsdServer2	*pcsLoginServer	= NextLoginServer();
	if (!pcsLoginServer)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketSendMessage(AGSMGLOBALCHAT_TYPE_NORMAL, NULL, NULL, pszMessage, unMessageLength, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacketSvr(pvPacket, nPacketLength, pcsLoginServer->m_dpnidServer);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmGlobalChatting::CBAreaChattingGlobal(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return TRUE;

	AgsmGlobalChatting	*pThis			= (AgsmGlobalChatting *)	pClass;
	AgpdChatData		*pcsChatData	= (AgpdChatData *)			pData;

	if (!pThis->m_pcsAgsmItem->GetItemAreaChattingGlobal((AgpdCharacter *) pcsChatData->pcsSenderBase))
		return TRUE;

	return pThis->SendGlobalChatMessage((AgpdCharacter *) pcsChatData->pcsSenderBase, pcsChatData->szMessage, pcsChatData->lMessageLength);
}

AgsdServer2* AgsmGlobalChatting::NextLoginServer()
{
	AgsdServer2	*pcsLoginServer	= NULL;
	AgsdServer2 *pcsTempLoginServer = NULL;

	INT16 lIndex = 0;
	INT16 ltempIndex = 0;

	while((pcsLoginServer = m_pcsAgsmServerManager2->GetLoginServers(&lIndex)) != NULL)
	{
		if(pcsLoginServer->m_bIsConnected)
		{
			if(lIndex > m_nLoginServerIndex)
			{
				m_nLoginServerIndex = lIndex;
				break;
			}
			else if(pcsTempLoginServer == NULL)
			{
				pcsTempLoginServer = pcsLoginServer;
				ltempIndex = lIndex;
			}
		}
	}

	if(!pcsLoginServer)
	{
		pcsLoginServer = pcsTempLoginServer;
		m_nLoginServerIndex = ltempIndex;
	}

	return pcsLoginServer;
}