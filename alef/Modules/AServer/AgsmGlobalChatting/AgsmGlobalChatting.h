#pragma once

#include "AgpmCharacter.h"
#include "AgsEngine.h"
#include "AgsmServerManager2.h"
#include "AgsmChatting.h"


typedef enum	AgsmGlobalChattingOperation {
	AGSMGLOBALCHAT_OPERATION_SEND_MESSAGE			= 1,
	AGSMGLOBALCHAT_OPERATION_BROADCAST_MESSAGE,
};

typedef enum	_AgsmGlobalChatType {
	AGSMGLOBALCHAT_TYPE_NOTIFY						= 1,
	AGSMGLOBALCHAT_TYPE_NORMAL,
} AgsmGlobalChatType;

typedef enum	AgsmGlobalChatCB {
	AGSMGLOBALCHAT_CB_RECEIVE_BROADCAST_MESSAGE		= 0,
};


class AgsmGlobalChatting : public AgsModule {
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgsmItem			*m_pcsAgsmItem;
	AgsmServerManager2	*m_pcsAgsmServerManager2;
	AgsmChatting		*m_pcsAgsmChatting;

	AuPacket	m_csPacket;
	ApMutualEx	m_Mutex;

	INT16		m_nLoginServerIndex;

public:
	AgsmGlobalChatting();
	virtual ~AgsmGlobalChatting();

	BOOL	OnAddModule();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);

	static BOOL	CBAreaChattingGlobal(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL	SetCallbackReceiveBroadCastMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SendGlobalChatMessage(AgpdCharacter *pcsCharacter, CHAR *pszMessage, UINT16 unMessageLength);
	BOOL	SendGlobalNotifyMessage(CHAR *pszMessage, UINT16 unMessageLength);

	AgsdServer2* NextLoginServer();

private:
	PVOID	MakePacketSendMessage(AgsmGlobalChatType eChatType, CHAR *pszCharName, CHAR *pszServerName, CHAR *pszMessage, UINT16 nMessageLength, INT16 *pnPacketLength, BOOL bIsBroadCast = FALSE);

	BOOL	OnOperationSendMessage(AgsmGlobalChatType eChatType, CHAR *pszSenderName, CHAR *pszServerName, CHAR *pszChatMessage, UINT16 unChatMessageLength);
	BOOL	OnOperationBroadCastMessage(AgsmGlobalChatType eChatType, CHAR *pszSenderName, CHAR *pszServerName, CHAR *pszChatMessage, UINT16 unChatMessageLength);
};