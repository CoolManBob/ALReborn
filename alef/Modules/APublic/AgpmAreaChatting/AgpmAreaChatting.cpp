#include "AgpmAreaChatting.h"

AgpmAreaChatting::AgpmAreaChatting()
{
	SetModuleName("AgpmAreaChatting");

	SetPacketType(AGPMAREACHATTING_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(CHAR));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,		// operation
							AUTYPE_INT32,			1,		// cid
							AUTYPE_INT8,			1,		// if character is administrator, 1 else 0;
							AUTYPE_INT8,			1,		// [외치기] 추가할지 여부 (1이면 추가, 0이면 추가안함)
							AUTYPE_UINT32,			1,		// 텍스트 칼라
							AUTYPE_MEMORY_BLOCK,	1,		// chatting message
							AUTYPE_END,				0
							);
}

AgpmAreaChatting::~AgpmAreaChatting()
{
}

BOOL AgpmAreaChatting::OnAddModule()
{
	return TRUE;
}

BOOL AgpmAreaChatting::OnInit()
{
	return TRUE;
}

BOOL AgpmAreaChatting::OnDestroy()
{
	return TRUE;
}

BOOL AgpmAreaChatting::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1 || !pstCheckArg)
		return FALSE;

	INT8	cOperation			= (-1);
	INT32	lCID				= AP_INVALID_CID;
	INT8	cIsAdmin			= 0;
	INT8	cIsAddHeader		= 1;
	UINT32	ulTextColorRGB		= 0;

	CHAR*	szMessage		= NULL;
	UINT16	unMessageLength	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
							&cOperation,
							&lCID,
							&cIsAdmin,
							&cIsAddHeader,
							&ulTextColorRGB,
							&szMessage, &unMessageLength);

	switch (cOperation) {
	case AGPMAREACHATTING_OPERATION_SEND_MESSAGE:
		{
			if (unMessageLength + sizeof(PACKET_HEADER) + m_csPacket.GetFlagLength() > (UINT16) nSize)
				return FALSE;

			PVOID	pvBuffer[5];
			pvBuffer[0]	= (PVOID) cIsAdmin;
			pvBuffer[1]	= (PVOID) szMessage;
			pvBuffer[2]	= (PVOID) unMessageLength;
			pvBuffer[3]	= (PVOID) cIsAddHeader;
			pvBuffer[4]	= (PVOID) ulTextColorRGB;

			return EnumCallback(AGPMAREACHATTING_CB_RECV_SEND_MESSAGE, &lCID, pvBuffer);
		}
		break;
	}

	return TRUE;
}

BOOL AgpmAreaChatting::SetCallbackRecvSendMessage(ApModuleDefaultCallBack fnCallback, PVOID pClass)
{
	return SetCallback(AGPMAREACHATTING_CB_RECV_SEND_MESSAGE, fnCallback, pClass);;
}

PVOID AgpmAreaChatting::MakePacketSendMessage(INT32 lCID, INT8 cIsAdmin, INT8 cIsAddHeader, UINT32 ulTextColorRGB, CHAR *szMessage, UINT16 unMessageLength, INT16 *pnPacketLength)
{
	if (!szMessage || !pnPacketLength || unMessageLength < 1 /* || lCID == AP_INVALID_CID */)
		return NULL;

	INT8	cOperation	= AGPMAREACHATTING_OPERATION_SEND_MESSAGE;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMAREACHATTING_PACKET_TYPE,
								&cOperation,
								&lCID,
								&cIsAdmin,
								&cIsAddHeader,
								(ulTextColorRGB > 0) ? &ulTextColorRGB : NULL,
								szMessage, &unMessageLength);
}