// AgpmSystemMessage.cpp
// (C) NHN Games - ArchLord Development Team
// kelovon, 20050906

#include "AgpmSystemMessage.h"

AgpmSystemMessage::AgpmSystemMessage()
{
	SetModuleName("AgpmSystemMessage");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	EnableIdle2(FALSE);

	SetPacketType(AGPMSYSTEMMESSAGE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		AUTYPE_INT8,			1,											// Operation
		AUTYPE_INT32,			1,											// System Message Code
		AUTYPE_INT32,			1,											// Integer Param 1
		AUTYPE_INT32,			1,											// Integer Param 2
		AUTYPE_CHAR,			AGPMSYSTEMMESSAGE_MAX_STRING_LENGTH + 1,	// String Param 1
		AUTYPE_CHAR,			AGPMSYSTEMMESSAGE_MAX_STRING_LENGTH + 1,	// String Param 2
		AUTYPE_END,				0
		);
}

AgpmSystemMessage::~AgpmSystemMessage()
{
}

BOOL AgpmSystemMessage::OnAddModule()
{
	//m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");

	//if(!m_pagpmCharacter)
	//	return FALSE;

	return TRUE;
}

BOOL AgpmSystemMessage::OnInit()
{
	return TRUE;
}

BOOL AgpmSystemMessage::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmSystemMessage::OnDestroy()
{
	return TRUE;
}

BOOL AgpmSystemMessage::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmSystemMessage::OnReceive");

	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	INT32 lSystemMessageCode = -1;
	INT32 lIntParam1 = -1;
	INT32 lIntParam2 = -1;
	CHAR* szStringParam1 = NULL;
	CHAR* szStringParam2 = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lSystemMessageCode,
						&lIntParam1,
						&lIntParam2,
						&szStringParam1,
						&szStringParam2);

	ProcessSystemMessage( cOperation , lSystemMessageCode , lIntParam1 , lIntParam2 ,
							szStringParam1 , szStringParam2 );

	return TRUE;
}

BOOL AgpmSystemMessage::ProcessSystemMessage( INT32 eSysMsgType, INT32 lCode, INT32 lIntParam1 /* = -1 */, INT32 lIntParam2 /* = -1 */, CHAR* szStringParam1 /* = NULL */, CHAR* szStringParam2 /* = NULL */, AgpdCharacter* pcsCharacter /* = NULL */)
{
	AgpdSystemMessage stSystemMessage;
	ZeroMemory( &stSystemMessage , sizeof(AgpdSystemMessage) );

	stSystemMessage.m_lOperation	= eSysMsgType;
	stSystemMessage.m_lCode			= lCode;
	stSystemMessage.m_alData[0]		= lIntParam1;
	stSystemMessage.m_alData[1]		= lIntParam2;
	stSystemMessage.m_aszData[0]	= szStringParam1;
	stSystemMessage.m_aszData[1]	= szStringParam2;

	EnumCallback( AGPMSYSTEMMESSAGE_CB_SYSTEM_MESSAGE , &stSystemMessage, pcsCharacter );

	return TRUE;
}

BOOL AgpmSystemMessage::ProcessSystemMessage( CHAR* szMsg , DWORD dwColor , INT32 eSysMsgType )
{
	AgpdSystemMessage stSystemMessage;
	ZeroMemory( &stSystemMessage , sizeof(AgpdSystemMessage) );

	stSystemMessage.m_lOperation	=	eSysMsgType;
	stSystemMessage.m_lCode			=	-1;
	stSystemMessage.m_aszData[0]	=	szMsg;
	stSystemMessage.m_dwColor		=	dwColor;
	
	EnumCallback( AGPMSYSTEMMESSAGE_CB_SYSTEM_MESSAGE , &stSystemMessage , NULL );
	return TRUE;
	
}

PVOID AgpmSystemMessage::MakeSystemMessagePacket(INT16* pnPacketLength, INT32 lCode, INT32 lIntParam1, INT32 lIntParam2, CHAR* szStringParam1, CHAR* szStringParam2)
{
	INT8 cType = AGPMSYSTEMMESSAGE_PACKET_TYPE;
	INT8 cOperation = AGPMSYSTEMMESSAGE_PACKET_SYSTEM_MESSAGE;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, cType,
								&cOperation,							// Operation
								((lCode==-1)?NULL:&lCode),				// System Message Code
								((lIntParam1==-1)?NULL:&lIntParam1),	// Integer Param 1
								((lIntParam2==-1)?NULL:&lIntParam2),	// Integer Param 2
								szStringParam1,							// String Param 1
								szStringParam2							// String Param 2
								);
}
//JK_½É¾ß¼¦´Ù¿î
PVOID AgpmSystemMessage::MakeSystemMessagePacketWithType(INT16* pnPacketLength, INT8 cMsgType, INT32 lCode, INT32 lIntParam1, INT32 lIntParam2, CHAR* szStringParam1, CHAR* szStringParam2)
{
	INT8 cType = AGPMSYSTEMMESSAGE_PACKET_TYPE;
	INT8 cOperation = cMsgType;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, cType,
		&cOperation,							// Operation
		((lCode==-1)?NULL:&lCode),				// System Message Code
		((lIntParam1==-1)?NULL:&lIntParam1),	// Integer Param 1
		((lIntParam2==-1)?NULL:&lIntParam2),	// Integer Param 2
		szStringParam1,							// String Param 1
		szStringParam2							// String Param 2
		);
}

BOOL AgpmSystemMessage::SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSYSTEMMESSAGE_CB_SYSTEM_MESSAGE, pfCallback, pClass);
}
