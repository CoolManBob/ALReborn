#include ".\agpmscript.h"

using namespace std;

AgpmScript::AgpmScript(void)
{
	SetModuleName("AgpmScript");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	EnableIdle(FALSE);
	EnableIdle2(FALSE);

	SetPacketType(AGPMSCRPIT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		AUTYPE_INT8,			1,			// Operation
		AUTYPE_INT32,			1,			// CID
		AUTYPE_MEMORY_BLOCK,	1,			// string
		AUTYPE_END,				0
		);

	{
		AuAutoLock lock(m_CriticalSection);
		ZeroMemory(m_Buffer, sizeof(m_Buffer));
	}
}

AgpmScript::~AgpmScript(void)
{
}

BOOL AgpmScript::OnAddModule()
{
	return TRUE;
}

BOOL AgpmScript::OnInit()
{
	return TRUE;
}

BOOL AgpmScript::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmScript::OnDestroy()
{
	return TRUE;
}

BOOL AgpmScript::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	INT8	cOperation		= -1;
	INT32	lCID			= -1;
	CHAR*	szMessage		= NULL;
	UINT16	unMessageLength	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&szMessage, &unMessageLength);

	szMessage[unMessageLength] = NULL;

	switch(cOperation)
	{
	case AGPMSCRIPT_OPERATION_COMMAND:	
		{
			// server가 아니어야만 한다.
			if (TRUE == pstCheckArg->bReceivedFromServer)
				return FALSE;

			// Admin 이어야만 Script를 실행할 수 있다.
			if (TRUE == EnumCallback(AGPMSCRIPT_CALLBACK_IS_ADMIN, &pstCheckArg->lSocketOwnerID, NULL))
				return (BOOL)m_ScriptEngine.execute(szMessage);
		}
	case AGPMSCRIPT_OPERATION_MESSAGE:
		{
			return EnumCallback(AGPMSCRIPT_CALLBACK_MESSAGE, &lCID, &szMessage); 
		}

	case AGPMSCRIPT_OPERATIION_KEYWORDS:
		{
			return EnumCallback(AGPMSCRIPT_CALLBACK_KEYWORDS, &szMessage, &unMessageLength);
		}
	};

	return TRUE;
}

PVOID AgpmScript::MakeScriptPacket(INT16 *pPacketLength, EnumAgpmScriptOperation eOperation, CHAR* szMessage, INT32 lCID)
{
	ASSERT(NULL != pPacketLength);
	
	INT8	cOperation = (INT8)eOperation;
	UINT16	unMessageLength	= (UINT16)strlen(szMessage);

	ASSERT(0 < unMessageLength);
	ASSERT(AGPMSCRIPT_MAX_MEMORY_LENGTH > unMessageLength);

	return m_csPacket.MakePacket(TRUE, pPacketLength, AGPMSCRPIT_PACKET_TYPE,
								&cOperation, 
								&lCID, 
								szMessage, &unMessageLength);
}

BOOL AgpmScript::SetCallbackMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSCRIPT_CALLBACK_MESSAGE, pfCallback, pClass);
}

BOOL AgpmScript::SetCallbackKeywords(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSCRIPT_CALLBACK_KEYWORDS, pfCallback, pClass);
}

BOOL AgpmScript::SetCallbackIsAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSCRIPT_CALLBACK_IS_ADMIN, pfCallback, pClass);
}

PVOID AgpmScript::MakeScriptKeywordPacket(INT16 *pPacketLength, INT32 lCID)
{
	AuAutoLock lock(m_CriticalSection);
	ZeroMemory(m_Buffer, sizeof(m_Buffer));
	INT32 lScriptKeywordLength = 0;

	for (vector<string>::iterator iter = m_Keywords.begin(); iter != m_Keywords.end(); iter++)
	{
		lScriptKeywordLength += sprintf(&m_Buffer[lScriptKeywordLength], "%s;", iter->c_str());
	}

	ASSERT(AGPMSCRIPT_MAX_MEMORY_LENGTH > lScriptKeywordLength);
	if (AGPMSCRIPT_MAX_MEMORY_LENGTH < lScriptKeywordLength)
		return FALSE;

	ASSERT(0 < lScriptKeywordLength);
	if (0 >= lScriptKeywordLength)
		return FALSE;

	return MakeScriptPacket(pPacketLength, AGPMSCRIPT_OPERATIION_KEYWORDS, m_Buffer, lCID);
}	

void AgpmScript::TestMessage()
{
	printf("Test Message : 테스트 함수\n");
//	printf("%s\n", szMessage);
}