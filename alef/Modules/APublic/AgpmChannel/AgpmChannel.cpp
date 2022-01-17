#include "AgpmChannel.h"
#include "AgpmCharacter.h"
#include "ApmMap.h"
#include "AgpmSystemMessage.h"

AgpmChannel::AgpmChannel()
{
	SetModuleName("AgpmChannel");
	SetPacketType(AGPMCHANNEL_PACKET_TYPE);
	SetModuleData(sizeof(AgpdChannel), AGPMCHANNEL_DATA_CHANNEL);

	EnableIdle2(TRUE);

	m_csPacket.SetFlagLength(sizeof(INT32));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,											// Operation
							AUTYPE_INT32,			1,											// CID
							AUTYPE_CHAR,			ApStrChannelName::eBufferLength + 1,		// Channel Name
							AUTYPE_CHAR,			ApStrChannelPassword::eBufferLength + 1,	// password
							AUTYPE_CHAR,			ApStrCharacterName::eBufferLength + 1,		// user name
							AUTYPE_INT32,			1,											// Channel Type
							AUTYPE_INT32,			1,											// Message code
							AUTYPE_CHAR,			ApStrChannelChatMsg::eBufferLength + 1,		// Chatting Message
							AUTYPE_MEMORY_BLOCK,	1,											// user list
							AUTYPE_END,				0
							);

	m_pcsAgpmCharacter = NULL;
	m_papmMap = NULL;
	m_pcsAgpmSystemMessage = NULL;
	m_lIndexAttachData = -1;
}

AgpmChannel::~AgpmChannel()
{
}

BOOL AgpmChannel::SetMaxChannel(INT32 lMaxChannel)
{
	if (FALSE == m_csAdminChannel.InitializeObject(sizeof(AgpdChannel *), lMaxChannel))
		return FALSE;

	return TRUE;
}

BOOL AgpmChannel::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmSystemMessage	= (AgpmSystemMessage*)GetModule("AgpmSystemMessage");
	m_papmMap				= (ApmMap*)GetModule("ApmMap");

	if (NULL == m_pcsAgpmCharacter || NULL == m_papmMap || NULL == m_pcsAgpmSystemMessage)
		return FALSE;

	m_lIndexAttachData = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdJoinChannelADChar), 
																	AgpdJoinChannelConstructor, AgpdJoinChannelDestructor);
	if (m_lIndexAttachData < 0)
		return FALSE;

	return TRUE;
}

BOOL AgpmChannel::OnInit()
{
	return TRUE;
}

BOOL AgpmChannel::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmChannel::OnDestroy()
{
	return TRUE;
}

AgpdChannel* AgpmChannel::CreateChannelData()
{
   // 방을 생성
	AgpdChannel *pChannel = (AgpdChannel*)CreateModuleData(AGPMCHANNEL_DATA_CHANNEL);
	ASSERT(NULL != pChannel);

	if (NULL == pChannel)
		return NULL;

	// 생성자 호출
#ifdef new
#undef new
#endif
	new (pChannel) AgpdChannel;
#ifdef new
#undef new	
#define new DEBUG_NEW
#endif	

	return pChannel;
}

BOOL AgpmChannel::DestroyChannelData(AgpdChannel *pcsChannel)
{
	pcsChannel->~AgpdChannel();
	return DestroyModuleData(pcsChannel, AGPMCHANNEL_DATA_CHANNEL);
}

BOOL AgpmChannel::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	INT8 cOperation			= -1;
	INT32 lCID				= -1;
	TCHAR *szChannelName	= NULL;
	TCHAR *szPassword		= NULL;
	TCHAR *szUserName		= NULL;
	INT32 lChannelType		= -1;
	INT32 lMsgCode			= -1;
	TCHAR *szChatMsg		= NULL;
	PVOID pvUserList		= NULL;
	UINT16 unMessageLength	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,
						&lCID,
						&szChannelName,
						&szPassword,
						&szUserName,
						&lChannelType,
						&lMsgCode,
						&szChatMsg,
						&pvUserList, &unMessageLength);

	if (AGPMCHANNEL_OPERATION_MAX <= cOperation)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (NULL == pcsCharacter)
		return FALSE;

	AgpdChannelParam *pChannelParam = new AgpdChannelParam;
	if (szChannelName)
		pChannelParam->m_strName.SetText(szChannelName);

	if (szPassword)
		pChannelParam->m_strPassword.SetText(szPassword);

	if (szChatMsg)
		pChannelParam->m_strChatMsg.SetText(szChatMsg);

	if (szUserName)
		pChannelParam->m_strUserName.SetText(szUserName);

	pChannelParam->m_eType = (EnumChannelType)lChannelType;
	pChannelParam->m_eMsgCode = (EnumAgpmChannelMsgCode)lMsgCode;
	pChannelParam->m_pvUserList = pvUserList;
	pChannelParam->m_unByteLength = unMessageLength;

	// Nochat인 지역에서는 채팅방을 통한 대화도 막아준다.
	if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_CHATTING) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
	{
		if(m_pcsAgpmSystemMessage)
		{
			m_pcsAgpmSystemMessage->ProcessSystemMessage(0, AGPMSYSTEMMESSAGE_CODE_DISABLE_USE_THIS_REGION, -1, -1, NULL, NULL, pcsCharacter);
		}

		pcsCharacter->m_Mutex.Release();
		delete pChannelParam;
		return TRUE;
	}

	switch(cOperation)
	{
	case AGPMCHANNEL_OPERATION_CREATE:
		EnumCallback(AGPMCHANNEL_CB_CREATE, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_DESTROY:
		EnumCallback(AGPMCHANNEL_CB_DESTROY, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_JOIN:
		EnumCallback(AGPMCHANNEL_CB_JOIN, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_LEAVE:
		EnumCallback(AGPMCHANNEL_CB_LEAVE, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_ADD_USER:
		EnumCallback(AGPMCHANNEL_CB_ADD_USER, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_REMOVE_USER:
		EnumCallback(AGPMCHANNEL_CB_REMOVE_USER, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_INVITATION:
		EnumCallback(AGPMCHANNEL_CB_INVITATION, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_PASSWORD:
		EnumCallback(AGPMCHANNEL_CB_PASSWORD, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_CHATMSG:
		EnumCallback(AGPMCHANNEL_CB_CHATMSG, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_FIND_USER:
		EnumCallback(AGPMCHANNEL_CB_FIND_USER, pcsCharacter, pChannelParam);	break;

	case AGPMCHANNEL_OPERATION_MSG_CODE:
		EnumCallback(AGPMCHANNEL_CB_MSG_CODE, pcsCharacter, pChannelParam);	break;

	default:
		ASSERT(!"Invalid Channel Operation");
	};

	pcsCharacter->m_Mutex.Release();

	delete pChannelParam;

	return TRUE;
}

BOOL AgpmChannel::AgpdJoinChannelConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(NULL == pClass || NULL == pData)
		return FALSE;

	AgpmChannel *pThis = (AgpmChannel *) pClass;
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter *) pData;
	AgpdJoinChannelADChar *pcsJoinChannel = (AgpdJoinChannelADChar *)pThis->GetAttachAgpdJoinChannelData(pstAgpdCharacter);

#ifdef new
#undef new
#endif
	new (pcsJoinChannel) AgpdJoinChannelADChar;
#ifdef new
#undef new	
#define new DEBUG_NEW
#endif	

	return TRUE;
}

BOOL AgpmChannel::AgpdJoinChannelDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{

	return TRUE;
}

AgpdJoinChannelADChar* AgpmChannel::GetAttachAgpdJoinChannelData(AgpdCharacter *pcsCharacter)
{
	return (AgpdJoinChannelADChar*)m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexAttachData, (PVOID)pcsCharacter);
}

BOOL AgpmChannel::SetCallbackCreateChannel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_CREATE, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackDestroyChannel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_DESTROY, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackJoin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_JOIN, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_LEAVE, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_INVITATION, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackPassword(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_PASSWORD, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackChatMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_CHATMSG, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackFindUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_FIND_USER, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackAddUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_OPERATION_ADD_USER, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackRemoveUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_OPERATION_REMOVE_USER, pfCallback, pClass);
}

BOOL AgpmChannel::SetCallbackMsgCode(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHANNEL_CB_MSG_CODE, pfCallback, pClass);
}

PVOID AgpmChannel::MakePacketChannel(INT16 *pnPacketLength, EnumAgpmChannelOperation eOperation, INT32 lCID, TCHAR *szChannelName, 
							TCHAR *szPassword, TCHAR *szCharName, EnumChannelType eType, EnumAgpmChannelMsgCode eMsgCode, TCHAR *szChatMsg,
							PVOID pvUserList, UINT16 unMessageLength)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(AGPMCHANNEL_OPERATION_NONE != eOperation);
	ASSERT(0 < lCID);

	INT8 cOperation = (INT8)eOperation;
	INT32 lType = (INT32)eType;
	INT32 lMsgCode = (INT32)eMsgCode;

	if (NULL != pvUserList)
	{
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHANNEL_PACKET_TYPE,
										&eOperation, 
										&lCID,
										szChannelName,
										szPassword,
										szCharName,
										&lType,
										&lMsgCode,
										szChatMsg,
										pvUserList, &unMessageLength);
	}
	else
	{
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHANNEL_PACKET_TYPE,
										&eOperation, 
										&lCID,
										szChannelName,
										szPassword,
										szCharName,
										&lType,
										&lMsgCode,
										szChatMsg,
										NULL);
	}
}

PVOID AgpmChannel::MakePacketCreateChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR* szChannelName, TCHAR* szPassword, EnumChannelType eType)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != szChannelName);

	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_CREATE, lCID, szChannelName, szPassword, NULL, eType, 
								AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketDestroyChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR* szChannelName, EnumChannelType eType)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != szChannelName);

	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_DESTROY, lCID, szChannelName, NULL, NULL, eType, 
								AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketMsgCode(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, 
									 EnumAgpmChannelMsgCode eMsgCode)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != szChannelName);
	ASSERT(AGPMCHANNEL_MSG_CODE_NONE != eMsgCode);

	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_MSG_CODE, lCID, szChannelName, NULL, NULL, eType, eMsgCode, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketJoinChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, 
										 PVOID pvUserList, UINT16 unMessageLength)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != szChannelName);

	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_JOIN, lCID, szChannelName, NULL, NULL, eType, 
								AGPMCHANNEL_MSG_CODE_NONE, NULL, pvUserList, unMessageLength);
}

PVOID AgpmChannel::MakePacketLeaveChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName)
{
	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_LEAVE, lCID, szChannelName, NULL, NULL, AGPDCHANNEL_TYPE_NONE, 
								AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketAddUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != szChannelName);

	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_ADD_USER, lCID, szChannelName, NULL, szUserName, 
								AGPDCHANNEL_TYPE_NONE, AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketRemoveUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName)
{
	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_REMOVE_USER, lCID, szChannelName, NULL, szUserName, 
								AGPDCHANNEL_TYPE_NONE, AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketChatMsg(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName, TCHAR *szChatMsg)
{
	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_CHATMSG, lCID, szChannelName, NULL, szUserName, AGPDCHANNEL_TYPE_NONE,
								AGPMCHANNEL_MSG_CODE_NONE, szChatMsg, NULL, 0);
}

PVOID AgpmChannel::MakePacketPassword(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szPassword)
{
	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_PASSWORD, lCID, szChannelName, szPassword, NULL, AGPDCHANNEL_TYPE_NONE, 
								AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketInvitation(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, TCHAR *szUserName)
{
	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_INVITATION, lCID, szChannelName, NULL, szUserName, eType,
								AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}

PVOID AgpmChannel::MakePacketFindUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szUserName)
{
	return MakePacketChannel(pnPacketLength, AGPMCHANNEL_OPERATION_FIND_USER, lCID, NULL, NULL, szUserName, AGPDCHANNEL_TYPE_NONE,
								AGPMCHANNEL_MSG_CODE_NONE, NULL, NULL, 0);
}