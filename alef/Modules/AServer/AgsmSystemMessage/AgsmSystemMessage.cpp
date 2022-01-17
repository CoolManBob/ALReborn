#include "AgsmSystemMessage.h"

AgsmSystemMessage::AgsmSystemMessage()
{
	SetModuleName("AgsmSystemMessage");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	EnableIdle2(FALSE);

	m_pcsAgpmSystemMessage	= NULL;
	m_pXmlRoot				= NULL;
}

AgsmSystemMessage::~AgsmSystemMessage()
{
}

BOOL AgsmSystemMessage::OnAddModule()
{
	m_pcsAgpmSystemMessage = (AgpmSystemMessage*)GetModule("AgpmSystemMessage");

	if(!m_pcsAgpmSystemMessage)
		return FALSE;

	if(!m_pcsAgpmSystemMessage->SetCallbackSystemMessage(CBProcessSystemMessage, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmSystemMessage::OnInit()
{
	if(!m_pXmlDoc.LoadFile("ini\\SystemMessage.xml"))
		return FALSE;

	//m_pXmlDoc.Print();

	m_pXmlRoot = m_pXmlDoc.FirstChild("SystemMessage");

	if(!m_pXmlRoot)
		return FALSE;

	return TRUE;
}

BOOL AgsmSystemMessage::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmSystemMessage::OnDestroy()
{
	return TRUE;
}

AuXmlNode* AgsmSystemMessage::GetRootNode()
{
	return m_pXmlRoot;
}

BOOL AgsmSystemMessage::SetCallbackGetDPNID(ApModuleDefaultCallBack pfCallback, PVOID pClass)	// AGSMSYSTEMMESSAGE_CB_GET_CHAR_NID
{
	return SetCallback(AGSMSYSTEMMESSAGE_CB_GET_CHAR_DPNID, pfCallback, pClass);
}

BOOL AgsmSystemMessage::SendSystemMessage(UINT32 ulNID, INT32 lCode, INT32 lIntParam1 /* = -1 */, INT32 lIntParam2 /* = -1 */, CHAR* szStringParam1 /* = NULL */, CHAR* szStringParam2 /* = NULL */, AgpdCharacter* pcsCharacter /* = NULL */)
{
	if(lCode < 0 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmSystemMessage->MakeSystemMessagePacket(&nPacketLength,
		lCode,
		lIntParam1,
		lIntParam2,
		szStringParam1,
		szStringParam2);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmSystemMessage->m_csPacket.FreePacket(pvPacket);

	return bResult;
}
//JK_½É¾ß¼¦´Ù¿î
BOOL AgsmSystemMessage::SendSystemMessageWithType(UINT32 ulNID, INT8 cMsgType, INT32 lCode, INT32 lIntParam1 /* = -1 */, INT32 lIntParam2 /* = -1 */, CHAR* szStringParam1 /* = NULL */, CHAR* szStringParam2 /* = NULL */, AgpdCharacter* pcsCharacter /* = NULL */)
{
	if(lCode < 0 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmSystemMessage->MakeSystemMessagePacketWithType(&nPacketLength,
		cMsgType,
		lCode,
		lIntParam1,
		lIntParam2,
		szStringParam1,
		szStringParam2);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmSystemMessage->m_csPacket.FreePacket(pvPacket);

	return bResult;
}
BOOL AgsmSystemMessage::SendSystemGeneralString(UINT32 ulNID, CHAR* szStringParam, AgpdCharacter* pcsCharacter)
{
	return SendSystemMessage(ulNID, AGPMSYSTEMMESSAGE_CODE_GENERAL_STRING, -1, -1, szStringParam, NULL, pcsCharacter);
}

BOOL AgsmSystemMessage::SendSystemDebugString(UINT32 ulNID, CHAR* szStringParam, AgpdCharacter* pcsCharacter)
{
	return SendSystemMessage(ulNID, AGPMSYSTEMMESSAGE_CODE_DEBUG_STRING, -1, -1, szStringParam, NULL, pcsCharacter);
}

BOOL AgsmSystemMessage::SendSystemMessage(AgpdCharacter* pcsCharacter, INT32 lCode, INT32 lIntParam1 /* = -1 */, INT32 lIntParam2 /* = -1 */, CHAR* szStringParam1 /* = NULL */, CHAR* szStringParam2 /* = NULL */)
{
	UINT_PTR ulNID = 0;

	EnumCallback(AGSMSYSTEMMESSAGE_CB_GET_CHAR_DPNID, pcsCharacter, &ulNID);

	if (ulNID == 0)
	{
		return FALSE;
	}

	return SendSystemMessage((UINT32)ulNID, lCode, lIntParam1, lIntParam2, szStringParam1, szStringParam2, NULL);
}

BOOL AgsmSystemMessage::SendSystemGeneralString(AgpdCharacter* pcsCharacter, CHAR* szStringParam)
{
	UINT32 ulNID = 0;

	EnumCallback(AGSMSYSTEMMESSAGE_CB_GET_CHAR_DPNID, pcsCharacter, &ulNID);

	if (ulNID == 0)
	{
		return FALSE;
	}

	return SendSystemGeneralString(ulNID, szStringParam);
}

BOOL AgsmSystemMessage::SendSystemDebugString(AgpdCharacter* pcsCharacter, CHAR* szStringParam)
{
	UINT32 ulNID = 0;

	EnumCallback(AGSMSYSTEMMESSAGE_CB_GET_CHAR_DPNID, pcsCharacter, &ulNID);

	if (ulNID == 0)
	{
		return FALSE;
	}

	return SendSystemDebugString(ulNID, szStringParam);
}

// 2007.01.23. steeple
BOOL AgsmSystemMessage::CBProcessSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmSystemMessage* pThis = static_cast<AgsmSystemMessage*>(pClass);
	AgpdSystemMessage* pstMessage = static_cast<AgpdSystemMessage*>(pData);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pCustData);

	return pThis->SendSystemMessage(pcsCharacter,
									pstMessage->m_lCode,
									pstMessage->m_alData[0],
									pstMessage->m_alData[1],
									pstMessage->m_aszData[0],
									pstMessage->m_aszData[1]
									);
}

BOOL AgsmSystemMessage::SendSystemMessageAllUser(INT32 lCode, INT32 lIntParam1, INT32 lIntParam2, CHAR* szStringParam1, CHAR* szStringParam2)
{
	if(lCode < 0) return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmSystemMessage->MakeSystemMessagePacket(&nPacketLength,
		lCode,
		lIntParam1,
		lIntParam2,
		szStringParam1,
		szStringParam2);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketAllUser(pvPacket, nPacketLength);
	m_pcsAgpmSystemMessage->m_csPacket.FreePacket(pvPacket);

	return bResult;
}
