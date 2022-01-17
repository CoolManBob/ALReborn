//	AgsmChatting.cpp file
//
//////////////////////////////////////////////////////////

#include "AgsmChatting.h"
#include "AuStrTable.h"
#include "AuStringConv.h"
#include "AgpmLog.h"
#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgpmBattleGround.h"
#include "AgsmGuild.h"
#include "ApmMap.h"

AgsmChatting::AgsmChatting()
{
	SetModuleName("AgsmChatting");

	m_pChatEventData	= NULL;
	m_pagpmBattleGround	= NULL;
	m_pagsmGuild		= NULL;
	m_papmMap			= NULL;
	
	m_lEventDataCount	= 0;
}

AgsmChatting::~AgsmChatting()
{
	if (m_pChatEventData)
	{
		delete [] m_pChatEventData;

		m_pChatEventData	= NULL;
	}
}

BOOL AgsmChatting::OnAddModule()
{
	m_pagpmLog				= (AgpmLog *)			GetModule("AgpmLog");
	m_pagpmFactors			= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pagpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pagpmItem				= (AgpmItem *)			GetModule("AgpmItem");
	m_pagpmChatting			= (AgpmChatting *)		GetModule("AgpmChatting");
	m_pagpmParty			= (AgpmParty *)			GetModule("AgpmParty");
	m_pagpmSkill			= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pagpmPvP				= (AgpmPvP *)			GetModule("AgpmPvP");
	m_pagpmAdmin			= (AgpmAdmin*)			GetModule("AgpmAdmin");

	m_pagsmAOIFilter		= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");
	m_pagsmFactors			= (AgsmFactors *)		GetModule("AgsmFactors");
	m_pagsmCharacter		= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pagsmItem				= (AgsmItem *)			GetModule("AgsmItem");
	m_pagsmParty			= (AgsmParty *)			GetModule("AgsmParty");
	m_pAgsmServerManager	= (AgsmServerManager *)	GetModule("AgsmServerManager2");
	m_pagsmDeath			= (AgsmDeath *)			GetModule("AgsmDeath");

	m_pagpmConfig			= (AgpmConfig *)		GetModule("AgpmConfig");
	m_papmMap				= (ApmMap *)			GetModule("ApmMap");

	if (!m_pagpmLog || !m_pagpmFactors || !m_pagpmCharacter || !m_pagpmChatting || !m_pagsmAOIFilter || !m_pagpmSkill ||
		!m_pagpmPvP || !m_pagpmAdmin ||
		!m_pagsmCharacter || !m_pagsmParty || !m_pAgsmServerManager || !m_pagsmDeath || !m_pagsmFactors || !m_pagpmConfig ||
		!m_pagsmItem || !m_pagpmItem || !m_papmMap)
		return FALSE;

	if (!m_pagpmChatting->SetCallbackRecvNormalMsg(CBRecvNormalMsg, this))
		return FALSE;
	//if (!m_pagpmChatting->SetCallbackRecvUnionMsg(CBRecvUnionMsg, this))
	//	return FALSE;
	if (!m_pagpmChatting->SetCallbackRecvPartyMsg(CBRecvPartyMsg, this))
		return FALSE;
	if (!m_pagpmChatting->SetCallbackRecvGuildMsg(CBRecvGuildMsg, this))
		return FALSE;
	if (!m_pagpmChatting->SetCallbackRecvWhisperMsg(CBRecvWhisperMsg, this))
		return FALSE;
	if (!m_pagpmChatting->SetCallbackRecvSystemLevel1Msg(CBRecvSystemLevel1Msg, this))
		return FALSE;
	if (!m_pagpmChatting->SetCallbackRecvSystemLevel2Msg(CBRecvSystemLevel2Msg, this))
		return FALSE;
	if (!m_pagpmChatting->SetCallbackRecvSystemLevel3Msg(CBRecvSystemLevel3Msg, this))
		return FALSE;
	if (!m_pagpmChatting->SetCallbackCheckEnable(CBCheckEnable, this))
		return FALSE;

	/*
	if (!m_pagpmCharacter->SetCallbackUpdateLevel(CBUpdateLevel, this))
		return FALSE;
	*/

	return TRUE;
}

BOOL AgsmChatting::OnInit()
{
	m_pagpmBattleGround = (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagsmGuild		= (AgsmGuild*)GetModule("AgsmGuild");
	
	if(!m_pagpmBattleGround || !m_pagsmGuild)
		return FALSE;
		
	return TRUE;
}

BOOL AgsmChatting::OnDestroy()
{
	return TRUE;
}

// 2003.11.18. steeple
BOOL AgsmChatting::SetCallbackCheckAdminCommand(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_CHECK_ADMIN_COMMAND, pfCallback, pClass);
}

BOOL AgsmChatting::CBRecvNormalMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmChatting		*pThis			= (AgsmChatting *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	// 2006.07.12. steeple
	// 2006.10.25. parn 중국은 chatting 로그 빼기
	if (g_eServiceArea != AP_SERVICE_AREA_CHINA)
	{
		if(pThis->m_pagpmLog->m_bWriteChattingLog &&
			pstChatData->pcsSenderBase && pstChatData->pcsSenderBase->m_eType == APBASE_TYPE_CHARACTER)
		{
			AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pstChatData->pcsSenderBase->m_lID);
			if(pcsCharacter)
				pThis->m_pagpmLog->WriteLog_Chatting(0, pcsCharacter->m_szID, pstChatData->szMessage, pstChatData->lMessageLength);
		}
	}

	// '/' command가 있는지 보고 처리한다.
	if (!pThis->ParseCommand(pstChatData))
	{
		// 없다면 걍 type 대로 보낸다.
		return pThis->SendMessageNormal(pstChatData->pcsSenderBase, pstChatData->szMessage, pstChatData->lMessageLength);
	}

	return TRUE;
}

BOOL AgsmChatting::CBRecvGuildMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmChatting		*pThis			= (AgsmChatting *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	// 2006.07.12. steeple
	// 2006.10.25. parn 중국은 chatting 로그 빼기
	if (g_eServiceArea != AP_SERVICE_AREA_CHINA)
	{
		if(pThis->m_pagpmLog->m_bWriteChattingLog &&
			pstChatData->pcsSenderBase && pstChatData->pcsSenderBase->m_eType == APBASE_TYPE_CHARACTER)
		{
			AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pstChatData->pcsSenderBase->m_lID);
			if(pcsCharacter)
				pThis->m_pagpmLog->WriteLog_Chatting(0, pcsCharacter->m_szID, pstChatData->szMessage, pstChatData->lMessageLength);
		}
	}

	// '/' command가 있는지 보고 처리한다.
	if (!pThis->ParseCommand(pstChatData))
	{
		// 없다면 걍 type 대로 보낸다.
		return pThis->SendMessageGuild(pstChatData->pcsSenderBase, pstChatData->szMessage, pstChatData->lMessageLength);
	}

	return TRUE;
}

BOOL AgsmChatting::CBRecvPartyMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmChatting		*pThis			= (AgsmChatting *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	// 2006.07.12. steeple
	// 2006.10.25. parn 중국은 chatting 로그 빼기
	if (g_eServiceArea != AP_SERVICE_AREA_CHINA)
	{
		if(pThis->m_pagpmLog->m_bWriteChattingLog &&
			pstChatData->pcsSenderBase && pstChatData->pcsSenderBase->m_eType == APBASE_TYPE_CHARACTER)
		{
			AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pstChatData->pcsSenderBase->m_lID);
			if(pcsCharacter)
				pThis->m_pagpmLog->WriteLog_Chatting(0, pcsCharacter->m_szID, pstChatData->szMessage, pstChatData->lMessageLength);
		}
	}

	// '/' command가 있는지 보고 처리한다.
	if (!pThis->ParseCommand(pstChatData))
	{
		// 없다면 걍 type 대로 보낸다.
		return pThis->SendMessageParty(pstChatData->pcsSenderBase, pstChatData->szMessage, pstChatData->lMessageLength);
	}

	return TRUE;
}

BOOL AgsmChatting::CBRecvWhisperMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmChatting		*pThis			= (AgsmChatting *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	// 2006.07.12. steeple
	// 2006.10.25. parn 중국은 chatting 로그 빼기
	if (g_eServiceArea != AP_SERVICE_AREA_CHINA)
	{
		if(pThis->m_pagpmLog->m_bWriteChattingLog &&
			pstChatData->pcsSenderBase && pstChatData->pcsSenderBase->m_eType == APBASE_TYPE_CHARACTER)
		{
			AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pstChatData->pcsSenderBase->m_lID);
			if(pcsCharacter)
				pThis->m_pagpmLog->WriteLog_Chatting(0, pcsCharacter->m_szID, pstChatData->szMessage, pstChatData->lMessageLength);
		}
	}

	// '/' command가 있는지 보고 처리한다.
	if (!pThis->ParseCommand(pstChatData))
	{
		// 없다면 걍 type 대로 보낸다.

		if (!pstChatData->pcsTargetBase)
		{
			// 귓속말을 보내려구 하는데 타겟이 없다. 에러 메시지를 날려준다.
			pThis->SendMessageCommandFailed(pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pstChatData->pcsSenderBase), AGSMCHAT_COMMAND_FAIL_NO_EXIST_TARGET, pstChatData->lTargetID);
		}
		else
		{
			pThis->SendMessageWhisper(pstChatData->pcsSenderBase, pstChatData->pcsTargetBase, pstChatData->szMessage, pstChatData->lMessageLength);
		}

//		for (int i = 0; i < 3; i++)
//		{
//			if (!pstChatData->pcsTargetBase[i])
//			{
//				// 귓속말을 보내려구 하는데 타겟이 없다. 에러 메시지를 날려준다.
//
//				if (i == 0)
//					pThis->SendMessageCommandFailed(pThis->m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pstChatData->pcsSenderBase), AGSMCHAT_COMMAND_FAIL_NO_EXIST_TARGET, pstChatData->lTargetID[i]);
//			}
//			else
//			{
//				pThis->SendMessageWhisper(pstChatData->pcsSenderBase, pstChatData->pcsTargetBase[i], pstChatData->szMessage, pstChatData->lMessageLength);
//			}
//		}

		return TRUE;
	}

	return TRUE;
}

BOOL AgsmChatting::CBRecvSystemLevel1Msg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmChatting		*pThis			= (AgsmChatting *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	return TRUE;
}

BOOL AgsmChatting::CBRecvSystemLevel2Msg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmChatting		*pThis			= (AgsmChatting *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	return TRUE;
}

BOOL AgsmChatting::CBRecvSystemLevel3Msg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmChatting		*pThis			= (AgsmChatting *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	return TRUE;
}

BOOL AgsmChatting::CBCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsSender = (AgpdCharacter*)pData;
	AgsmChatting* pThis = (AgsmChatting*)pClass;
	INT8* pcChatEnable = (INT8*)pCustData;

	BOOL bEnable = pThis->CheckEnable(pcsSender, pcChatEnable[0]);
	if(!bEnable && pcsSender)
	{
		pThis->SendMessageSystem(pcsSender, AGPDCHATTING_TYPE_SYSTEM_LEVEL3, AGSDCHARACTER_BAN_CHAT_SYSTEM_MESSAGE,
			(INT32)_tcslen(AGSDCHARACTER_BAN_CHAT_SYSTEM_MESSAGE), pThis->m_pagsmCharacter->GetCharDPNID(pcsSender));
	}

	pcChatEnable[1] = (INT8)bEnable;

	return TRUE;
}

BOOL AgsmChatting::ProcessChatEvent(AgpdChatData * pstChatData)
{
	if (!m_pagpmConfig->IsEventChatting())
		return FALSE;

	for (int i = 0; i < m_lEventDataCount; ++i)
	{
		if (strncmp(pstChatData->szMessage, m_pChatEventData[i].strChatMessage.GetBuffer(), strlen(m_pChatEventData[i].strChatMessage.GetBuffer())) == 0)
		{
			EnumCallback(AGSMCHATTING_CB_EVENT_MESSAGE, pstChatData, &m_pChatEventData[i].lSkillTID);

			return TRUE;
		}
	}

	return FALSE;
}

//		ParseCommand
//	Functions
//		- chat message에 '/' command 가 있는기 검사한다.
//		- '/' command가 나왔다면 처리한다.
//	Arguments
//		- pstChatData : chat data를 담고 있는 AgpdChatData * pointer
//	Return value
//		- TRUE : '/' command가 있고 정상적으로 처리를 했다.
//		- FALSE : '/' command가 없거나 정상적인 처리가 안되었다.
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmChatting::ParseCommand(AgpdChatData * pstChatData)
{
	if (!pstChatData || !pstChatData->szMessage || !pstChatData->szMessage[0] || pstChatData->lMessageLength < 1)
		return FALSE;

	// 불량스런 대화 내용이 있는지 검사한다. 그런 내용이 있다면...
	// 
	//
	//
	//

	if (ProcessChatEvent(pstChatData))
		return TRUE;

	// '/' command 가 있는지 검사한다. 있다면 기존의 AgpdChattingType은 무시하고 command를 실행한다.
	//	   command 가 있다면 처리하고 TRUE를, 그렇지 않음 FALSE를 리턴한다.

	int i = 0;
	for (i = 0; i < (int) pstChatData->lMessageLength; i++)
	{
		if (pstChatData->szMessage[i] != ' ')
			break;
	}

	if (i == (int) pstChatData->lMessageLength)
		return FALSE;

	if (pstChatData->szMessage[i] == '/')
	{
		///////////////////////////////////////////////////////////////////////////////
		// 명령어 뒤에 파라미터가 필요없는 명령어부터 처리한다.
		// 2005.02.16. steeple
		// 2005.02.17. steeple 클라이언트에서 확인/취소 UI 를 띄우게끔 변경해서 필요없어졌다.
		//if(strncmp(pstChatData->szMessage + i, AGPMCHAT_COMMAND_GUILD_LEAVE, pstChatData->lMessageLength - i) == 0 ||
		//	strncmp(pstChatData->szMessage + i, AGPMCHAT_COMMAND_GUILD_LEAVE_ENG, pstChatData->lMessageLength - i) == 0)
		//{
		//	return ProcessCommandGuildLeave(pstChatData);
		//}

		///////////////////////////////////////////////////////////////////////////////
		// 명령어 뒤에 파라미터가 오는 놈들
		int j = i + 1;
		for (j; j < (int) pstChatData->lMessageLength; j++)
		{
			if ( AP_SERVICE_AREA_JAPAN != g_eServiceArea && pstChatData->szMessage[j] == ' ')
				break;
			else
			{
				if (pstChatData->szMessage[j] == ' ')
					break;
					
				if (pstChatData->szMessage[j] == char(0x81) && pstChatData->szMessage[j+1] == 0x40)
					break;
			}
		}

		if (j - i < 2)
			return FALSE;

		// 파싱한 문자와 command를 비교해서 같으면 (올바른넘이면) 처리해준다.

		// 말풍선을 써라.
		if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_WORD_BALLOON_LOWER), max(strlen(ServerStr().GetStr(STI_CHAT_WORD_BALLOON_LOWER)), j - i)) == 0 ||
			strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_WORD_BALLOON_UPPER), max(strlen(ServerStr().GetStr(STI_CHAT_WORD_BALLOON_UPPER)), j - i)) == 0 ||
			strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_WORD_BALLOON_LOCALIZE), max(strlen(ServerStr().GetStr(STI_CHAT_WORD_BALLOON_LOCALIZE)), j - i)) == 0)
		{
			return ProcessCommandUseWordBalloon(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		// 귓속말이다.
		else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_WHISPER_LOWER), max(strlen(ServerStr().GetStr(STI_CHAT_WHISPER_LOWER)), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_WHISPER_UPPER), max(strlen(ServerStr().GetStr(STI_CHAT_WHISPER_UPPER)), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_WHISPER_LOCALIZE), max(strlen(ServerStr().GetStr(STI_CHAT_WHISPER_LOCALIZE)), j - i)) == 0)
		{
			return ProcessCommandWhisper(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		// 마지막으로 나테 귓속말 한 넘한테 대답하는거당.
		else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_REPLY_LOWER), max(strlen(ServerStr().GetStr(STI_CHAT_REPLY_LOWER)), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_REPLY_UPPER), max(strlen(ServerStr().GetStr(STI_CHAT_REPLY_UPPER)), j - i)) == 0)
		{
			return ProcessCommandReply(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		// 내가 속한 길드에게만 보내라.
		else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_SEND_GUILD_LOWER), max(strlen(ServerStr().GetStr(STI_CHAT_SEND_GUILD_LOWER)), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_SEND_GUILD_UPPER), max(strlen(ServerStr().GetStr(STI_CHAT_SEND_GUILD_UPPER)), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_SEND_GUILD_LOCALIZE), max(strlen(ServerStr().GetStr(STI_CHAT_SEND_GUILD_LOCALIZE)), j - i)) == 0)
		{
			return ProcessCommandSendGuild(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		// 내 파티원들에게 보내랑.
		else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_SEND_PARTY_LOWER), max(strlen(ServerStr().GetStr(STI_CHAT_SEND_PARTY_LOWER)), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_SEND_PARTY_UPPER), max(strlen(ServerStr().GetStr(STI_CHAT_SEND_PARTY_UPPER)), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_SEND_PARTY_LOCALIZE), max(strlen(ServerStr().GetStr(STI_CHAT_SEND_PARTY_LOCALIZE)), j - i)) == 0)
		{
			return ProcessCommandSendParty(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		// 연대 채팅 2006.08.11. steeple
		else if (strncmp(pstChatData->szMessage + i, AGPMCHAT_LOWER_COMMAND_SEND_GUILD_JOINT, max(strlen(AGPMCHAT_LOWER_COMMAND_SEND_GUILD_JOINT), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, AGPMCHAT_UPPER_COMMAND_SEND_GUILD_JOINT, max(strlen(AGPMCHAT_UPPER_COMMAND_SEND_GUILD_JOINT), j - i)) == 0 ||
				 strncmp(pstChatData->szMessage + i, AGPMCHAT_COMMAND_SEND_GUILD_JOINT_KOR, max(strlen(AGPMCHAT_COMMAND_SEND_GUILD_JOINT_KOR), j - i)) == 0)
		{
			return ProcessCommandSendGuildJoint(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		/*
		else if (strncmp(pstChatData->szMessage + i, AGPMCHAT_COMMAND_MOVE_SPEED_UP, j - i) == 0)
		{
			if (pstChatData->pcsSenderBase &&
				pstChatData->pcsSenderBase->m_eType == APBASE_TYPE_CHARACTER &&
				((AgpdCharacter *) pstChatData->pcsSenderBase)->m_pcsCharacterTemplate)
			{
				m_pagpmCharacter->StopCharacter((AgpdCharacter *) pstChatData->pcsSenderBase, NULL);

				INT32	lMoveSpeed	= 0;
				m_pagpmFactors->GetValue(&((AgpdCharacter *) pstChatData->pcsSenderBase)->m_pcsCharacterTemplate->m_csFactor, &lMoveSpeed, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

				m_pagpmFactors->SetValue(&((AgpdCharacter *) pstChatData->pcsSenderBase)->m_csFactor, lMoveSpeed * 5, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
				//m_pagpmFactors->SetValue(&((AgpdCharacter *) pstChatData->pcsSenderBase)->m_csFactor, lMoveSpeed * 5, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

				m_pagsmCharacter->ReCalcCharacterResultFactors(((AgpdCharacter *) pstChatData->pcsSenderBase), TRUE);
			}

			return TRUE;
		}
		else if (strncmp(pstChatData->szMessage + i, AGPMCHAT_COMMAND_MOVE_SPEED_DOWN, j - i) == 0)
		{
			if (pstChatData->pcsSenderBase &&
				pstChatData->pcsSenderBase->m_eType == APBASE_TYPE_CHARACTER &&
				((AgpdCharacter *) pstChatData->pcsSenderBase)->m_pcsCharacterTemplate)
			{
				m_pagpmCharacter->StopCharacter((AgpdCharacter *) pstChatData->pcsSenderBase, NULL);

				INT32	lMoveSpeed	= 0;
				m_pagpmFactors->GetValue(&((AgpdCharacter *) pstChatData->pcsSenderBase)->m_pcsCharacterTemplate->m_csFactor, &lMoveSpeed, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

				m_pagpmFactors->SetValue(&((AgpdCharacter *) pstChatData->pcsSenderBase)->m_csFactor, lMoveSpeed, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
				//m_pagpmFactors->SetValue(&((AgpdCharacter *) pstChatData->pcsSenderBase)->m_csFactor, lMoveSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

				m_pagsmCharacter->ReCalcCharacterResultFactors(((AgpdCharacter *) pstChatData->pcsSenderBase), TRUE);
			}

			return TRUE;
		}*/
		else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_INVITE_GUILD_MEMBER_ENG), max(strlen(ServerStr().GetStr(STI_CHAT_INVITE_GUILD_MEMBER_ENG)), j - i)) == 0 ||
				strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_INVITE_GUILD_MEMBER_LOCALIZE), max(strlen(ServerStr().GetStr(STI_CHAT_INVITE_GUILD_MEMBER_LOCALIZE)), j - i)) == 0)
		{
			return ProcessCommandGuildMemberInvite(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_INVITE_PARTY_MEMBER_ENG), j - i) == 0 ||
				strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_INVITE_PARTY_MEMBER_LOCALIZE), j - i) == 0)
		{
			return ProcessCommandPartyMemberInvite(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}
		else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_AREA_CHAT_TYPE_RACE), j - i) == 0)
		{
			int spaceSize = pstChatData->szMessage[j] == ' ' ? 1 : 2;

			pstChatData->szMessage		= pstChatData->szMessage + j + spaceSize;
			pstChatData->lMessageLength	= pstChatData->lMessageLength - (j + spaceSize);

			EnumCallback(AGSMCHATTING_CB_AREA_CHATTING_RACE, pstChatData, NULL);
			return TRUE;
		}
		else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_AREA_CHAT_TYPE_ALL), j - i) == 0)
		{
			// 감옥에 있는 녀석은 정령의 외침을 사용할 수 없다.
			AgpdCharacter *pcsSendCharacter = NULL;
			pcsSendCharacter = m_pagpmCharacter->GetCharacter(pstChatData->szSenderName);

			if(pcsSendCharacter != NULL)
			{
				if(m_pagpmCharacter->IsInJail(pcsSendCharacter))
					return FALSE;
			}

			int spaceSize = pstChatData->szMessage[j] == ' ' ? 1 : 2;
			
			pstChatData->szMessage		= pstChatData->szMessage + j + spaceSize;
			pstChatData->lMessageLength	= pstChatData->lMessageLength - (j + spaceSize);

			EnumCallback(AGSMCHATTING_CB_AREA_CHATTING_ALL, pstChatData, NULL);
			return TRUE;
		}
		else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_AREA_CHAT_TYPE_GLOBAL), j - i) == 0)
		{
			// 감옥에 있는 녀석은 정령의 외침을 사용할 수 없다.
			AgpdCharacter *pcsSendCharacter = NULL;
			pcsSendCharacter = m_pagpmCharacter->GetCharacter(pstChatData->szSenderName);

			if(pcsSendCharacter != NULL)
			{
				if(m_pagpmCharacter->IsInJail(pcsSendCharacter))
					return FALSE;
			}

			int spaceSize = pstChatData->szMessage[j] == ' ' ? 1 : 2;

			pstChatData->szMessage		= pstChatData->szMessage + j + 1;
			pstChatData->lMessageLength	= pstChatData->lMessageLength - (j + 1);

			EnumCallback(AGSMCHATTING_CB_AREA_CHATTING_GLOBAL, pstChatData, NULL);
			return TRUE;
		}
		else if(ServerStr().GetStr(STI_CHAT_EMPHASIS) &&
			strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_EMPHASIS), j - i) == 0)
		{
			return ProcessCommandEmphasis(pstChatData, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));
		}

		// 2006.09.29. steeple
		// 길이도 비교한다.
		else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_ARCHLORD_NOTICE), j - i) == 0 &&
				_tcslen(ServerStr().GetStr(STI_COMMAND_ARCHLORD_NOTICE)) == j - i)
		{
			pstChatData->szMessage		= pstChatData->szMessage + j + 1;
			pstChatData->lMessageLength	= pstChatData->lMessageLength - (j + 1);

			EnumCallback(AGSMCHATTING_CB_ARCHLORD_MESSAGE, pstChatData, NULL);
			return TRUE;
		}
		else if(!strncmp(pstChatData->szMessage + i, "/flag", 5))
		{
			INT32 nFlagType = 0;
			CHAR* strFlag = pstChatData->szMessage + j + 1;

			if(!strncmp(strFlag, "ribbon", 6))
				nFlagType = AGPDCHAR_EVENTFLAG_BLACKRIBBON;
			else if(!strncmp(strFlag, "korea", 5))
				nFlagType = AGPDCHAR_EVENTFLAG_KOREA;
			else
				nFlagType = AGPDCHAR_EVENTFLAG_NONE;

			AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(pstChatData->szSenderName);

			if( m_pagpmCharacter->SetNationFlag(pcsCharacter, nFlagType) )
				m_pagsmCharacter->SendPacketUpdateEventStatusFlag(pcsCharacter);

			return TRUE;
		}
		else
		{
			// 이도 저도 아니면 Admin 명령어인지 체크해본다. 2003.11.18. steeple
			return EnumCallback(AGSMCHATTING_CB_CHECK_ADMIN_COMMAND, pstChatData, NULL);
		}
	}
	else if (ServerStr().GetStr(STI_CHAT_WHISPER_SPECIAL) &&
		pstChatData->szMessage[i] == ServerStr().GetStr(STI_CHAT_WHISPER_SPECIAL)[0])
	{
		int	j = i + 1;

		if (i + 1 < (int) pstChatData->lMessageLength && pstChatData->szMessage[i + 1] == ' ')
		{
			for (j = i + 1; j < (int) pstChatData->lMessageLength; j++)
			{
				if (pstChatData->szMessage[j] != ' ')
					break;
			}
		}

		if (j == pstChatData->lMessageLength)
			return FALSE;

		return ProcessCommandWhisper(pstChatData, pstChatData->szMessage + j, pstChatData->lMessageLength - j);
	}
	else if (ServerStr().GetStr(STI_CHAT_SEND_GUILD_SPECIAL) &&
		pstChatData->szMessage[i] == ServerStr().GetStr(STI_CHAT_SEND_GUILD_SPECIAL)[0])
	{
		return ProcessCommandSendGuild(pstChatData, pstChatData->szMessage + i + 1, pstChatData->lMessageLength - (i + 1));
	}
	else if (ServerStr().GetStr(STI_CHAT_SEND_PARTY_SPECIAL) &&
		pstChatData->szMessage[i] == ServerStr().GetStr(STI_CHAT_SEND_PARTY_SPECIAL)[0])
	{
		return ProcessCommandSendParty(pstChatData, pstChatData->szMessage + i + 1, pstChatData->lMessageLength - (i + 1));
	}

	return FALSE;
}

BOOL AgsmChatting::ProcessCommandUseWordBalloon(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	return SendMessageWordBalloon(pstChatData->pcsSenderBase, szParseMessage, lLength);
}

BOOL AgsmChatting::ProcessCommandWhisper(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	INT32			lNumTarget = 0;
//	AgpdCharacter	*pcsTarget[5];
//
//	ZeroMemory(pcsTarget, sizeof(AgpdCharacter *) * 5);

	ApSafeArray<AgpdCharacter *, 5>	pcsTarget;
	pcsTarget.MemSetAll();

	// szParseMessage에서 귓속말 대상을 꺼낸다.
	int i = 0;
	for (i = 0; i < lLength; ++i)
	{
		if (szParseMessage[i] == ' ')
			break;
	}

	// 타겟이 없는지 검사한다.
	if (i == lLength && lNumTarget == 0)
		return FALSE;

	CHAR	szTargetID[AGPACHARACTER_MAX_ID_STRING + 1];
	ZeroMemory(szTargetID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));

	if (i > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	strncpy(szTargetID, szParseMessage, i);

	ToLowerExceptFirst( szTargetID, static_cast<int>( strlen( szTargetID ) ) );

	//pcsTarget[lNumTarget] = m_pagpmCharacter->GetCharacterLock(szTargetID);
	pcsTarget[lNumTarget] = m_pagpmCharacter->GetCharacter(szTargetID);
	if (pcsTarget[lNumTarget])
		++lNumTarget;
	else
	{
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= m_pagpmChatting->MakePacketOfflineWhisperTarget(pstChatData->pcsSenderBase->m_lID, szTargetID, &nPacketLength);

		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pstChatData->pcsSenderBase));

		m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

		return TRUE;
	}

	for (int j = 0; j < lNumTarget; j++)
	{
		if (pcsTarget[j] == NULL)
			break;
			
		if(m_pagpmBattleGround->IsAttackable((AgpdCharacter*)pstChatData->pcsSenderBase, pcsTarget[j])) continue;

		SendMessageWhisper(pstChatData->pcsSenderBase, (ApBase *) pcsTarget[j], szParseMessage + i + 1, lLength - (i + 1));

		//pcsTarget[i]->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmChatting::ProcessCommandReply(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	INT32	lLastWhisperSenderID = m_pagpmChatting->GetLastWhisperSenderID(m_pagpmCharacter->GetCharacter(pstChatData->lSenderID));

	//AgpdCharacter	*pcsLastWhisperSender = m_pagpmCharacter->GetCharacterLock(lLastWhisperSenderID);
	AgpdCharacter	*pcsLastWhisperSender = m_pagpmCharacter->GetCharacter(lLastWhisperSenderID);
	if (!pcsLastWhisperSender)
		return FALSE;

	BOOL	bSendResult = SendMessageWhisper(pstChatData->pcsSenderBase, (ApBase *) pcsLastWhisperSender, szParseMessage, lLength);

	//pcsLastWhisperSender->m_Mutex.Release();

	return bSendResult;
}

BOOL AgsmChatting::ProcessCommandSendGuild(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;
		
	return m_pagsmGuild->GuildMessage((AgpdCharacter*)pstChatData->pcsSenderBase, szParseMessage, lLength);
}

BOOL AgsmChatting::ProcessCommandSendGuildJoint(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	// 2006.08.11. steeple
	PVOID pvBuffer[2];
	pvBuffer[0] = szParseMessage;
	pvBuffer[1] = &lLength;

	return EnumCallback(AGSMCHATTING_CB_GUILD_JOINT_MESSAGE, pstChatData->pcsSenderBase, pvBuffer);
}

BOOL AgsmChatting::ProcessCommandSendParty(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	return SendMessageParty(pstChatData->pcsSenderBase, szParseMessage, lLength);
}

BOOL AgsmChatting::ProcessCommandEmphasis(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength)
{
	if (!pstChatData || !pstChatData->pcsSenderBase || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	AgpdItem *	pcsItem;
	
	pcsItem = m_pagpmItem->GetInventoryItemByTID((AgpdCharacter *) pstChatData->pcsSenderBase, m_pagpmItem->GetChattingEmphasisTID());
	if (!pcsItem)
	{
		pcsItem = m_pagpmItem->GetCashInventoryItemByTID((AgpdCharacter *) pstChatData->pcsSenderBase, m_pagpmItem->GetChattingEmphasisTID());
		if (!pcsItem)
			return FALSE;
	}

	if (!m_pagsmItem->UseItem(pcsItem))
		return FALSE;

	return SendMessageEmphasis(pstChatData->pcsSenderBase, szParseMessage, lLength);
}

// 2004.11.15. steeple
BOOL AgsmChatting::ProcessCommandGuildMemberInvite(AgpdChatData* pstChatData, CHAR* szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	// 2004.11.15. steeple
	PVOID pvBuffer[2];
	pvBuffer[0] = szParseMessage;
	pvBuffer[1] = &lLength;

	return EnumCallback(AGSMCHATTING_CB_GUILD_MEMBER_INVITE, pstChatData->pcsSenderBase, pvBuffer);
}

// 2005.01.19. steeple
BOOL AgsmChatting::ProcessCommandPartyMemberInvite(AgpdChatData* pstChatData, CHAR* szParseMessage, INT32 lLength)
{
	if (!pstChatData || !szParseMessage || !szParseMessage[0] || lLength < 1)
		return FALSE;

	if(lLength >= AGPACHARACTER_MAX_ID_STRING || lLength <= 0)
		return FALSE;

	szParseMessage[lLength]='\0';
	CHAR szTargetID[AGPACHARACTER_MAX_ID_STRING + 1];
	memset(szTargetID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	//m_pagpmCharacter->MakeGameID(szParseMessage,szTargetID);
	strncpy(szTargetID, szParseMessage, AGPACHARACTER_MAX_ID_STRING);

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pstChatData->pcsSenderBase;
	if(!pcsCharacter) return FALSE;
	UINT32 ulSenderNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);

//	파티초대 일원화를 위해 AgsmParty의 InviteMemever를 호출 2005.05.27. By SungHoon
	return m_pagsmParty->InvitePartyMember(ulSenderNID, pcsCharacter->m_lID, AP_INVALID_CID, szTargetID );
}

// 2005.02.16. steeple
BOOL AgsmChatting::ProcessCommandGuildLeave(AgpdChatData* pstChatData)
{
	if (!pstChatData)
		return FALSE;

	return EnumCallback(AGSMCHATTING_CB_GUILD_LEAVE, pstChatData->pcsSenderBase, NULL);
}

// 2004.03.31. steeple, return TRUE (채팅 가능), FALSE (채팅 불가능)
BOOL AgsmChatting::CheckEnable(AgpdCharacter* pcsAgpdCharacter, INT8 cChatType)
{
	// System Message 는 무조건 간다.
	if(cChatType == AGPDCHATTING_TYPE_SYSTEM_LEVEL1 ||
		cChatType == AGPDCHATTING_TYPE_SYSTEM_LEVEL2 ||
		cChatType == AGPDCHATTING_TYPE_SYSTEM_LEVEL3)
		return TRUE;

	// 귓말일때는 pcsAgpdCharacter 가 NULL 일 수도 있다.
	if(!pcsAgpdCharacter && cChatType == AGPDCHATTING_TYPE_WHISPER)
		return TRUE;

	if(!pcsAgpdCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// Chatting 불가능 지역이라면 채팅을 못하게 막는다. 
	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
	{
		if(m_papmMap->CheckRegionPerculiarity(pcsAgpdCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_CHATTING) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
		{
			return FALSE;
		}
	}

	// Chat Ban 상태가 아님 - 보통은 다 여기서 TRUE 로 리턴된다.
	if(pcsAgsdCharacter->m_stBan.m_lChatBanStartTime == 0 || pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime == 0)
		return TRUE;

	INT32 lKeepTime = 0;	// Second 단위
	switch(pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime)
	{
		case AGSDCHAR_BAN_KEEPTIME_5H:
			lKeepTime = (60 * 60) * 5;
			break;

		case AGSDCHAR_BAN_KEEPTIME_1D:
			lKeepTime = 60 * 60 * 24;
			break;

		case AGSDCHAR_BAN_KEEPTIME_5D:
			lKeepTime = (60 * 60 * 24) * 5;
			break;

		case AGSDCHAR_BAN_KEEPTIME_UNLIMIT:
			lKeepTime = -1;
			break;
	}

	// 무한대임
	if(lKeepTime == -1)
		return FALSE;
	
	// 현재 TimeStamp 를 구해서 뺀다.
	INT32 lCurrentTimeStamp = m_pagpmAdmin->GetCurrentTimeStamp();
	if((lCurrentTimeStamp - pcsAgsdCharacter->m_stBan.m_lChatBanStartTime) > lKeepTime)
	{
		// 여기로 왔다면, Ban 시간이 지난 것이므로 풀어줘야 된다.
		pcsAgsdCharacter->m_stBan.m_lChatBanStartTime = 0;
		pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime = 0;

		return TRUE;
	}
	else
		return FALSE;
}


//////////////////////////////////////////////////////////////////////////
// Send

BOOL AgsmChatting::SendMessageNormal(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength)
{
	if (!pcsSender || !szMessage || lMessageLength < 1 || !strlen(szMessage))
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmChatting->MakePacketChatting(pcsSender->m_lID, 
														   AGPDCHATTING_TYPE_NORMAL,
														   NULL,
														   NULL,
														   szMessage,
														   lMessageLength,
														   &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmChatting->m_csPacket.SetCID(pvPacket, nPacketLength, pcsSender->m_lID);

	BOOL bSendResult;
	AgpdCharacter* pcsCharacter =  (AgpdCharacter*)pcsSender;
	if(!pcsCharacter)
		return FALSE;
		
	if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		bSendResult = m_pagsmAOIFilter->SendPacketNearRace(pvPacket, nPacketLength, pcsCharacter->m_stPos, m_pagpmFactors->GetRace(&pcsCharacter->m_csFactor));
	else
		bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_6);

	m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmChatting::SendMessageWordBalloon(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength)
{
	if (!pcsSender || !szMessage || lMessageLength < 1 || !strlen(szMessage))
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmChatting->MakePacketChatting(pcsSender->m_lID, 
														   AGPDCHATTING_TYPE_WORD_BALLOON,
														   NULL,
														   NULL,
														   szMessage,
														   lMessageLength,
														   &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmChatting->m_csPacket.SetCID(pvPacket, nPacketLength, pcsSender->m_lID);

	BOOL bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, ((AgpdCharacter *) pcsSender)->m_stPos, PACKET_PRIORITY_6);

	m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

// 2004.04.06. steeple - 귓말을 보낸 사람에게도 같은 패킷을 그대로 보낸다.
BOOL AgsmChatting::SendMessageWhisper(ApBase *pcsSender, ApBase *pcsTarget, CHAR *szMessage, INT32 lMessageLength)
{
	if (!pcsSender || !pcsTarget || !szMessage || lMessageLength < 1 || !strlen(szMessage))
		return FALSE;

	// check block whisper status.

	AgpdChattingADChar	*pcsAttachData	= m_pagpmChatting->GetADCharacter((AgpdCharacter *) pcsTarget);

	if (pcsAttachData->m_bIsBlockWhisper)
	{
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= m_pagpmChatting->MakePacketReplyBlockWhisper(pcsSender->m_lID, ((AgpdCharacter *) pcsTarget)->m_szID, &nPacketLength);

		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsSender));

		m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

		return	bSendResult;
	}
	else
	{
		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter((AgpdCharacter *) pcsTarget);

		INT16	nPacketLength = 0;
		PVOID	pvPacket = m_pagpmChatting->MakePacketChatting(pcsSender->m_lID,
															   AGPDCHATTING_TYPE_WHISPER,
															   ((AgpdCharacter *) pcsSender)->m_szID,
															   ((AgpdCharacter *) pcsTarget)->m_szID,
															   szMessage,
															   lMessageLength,
															   &nPacketLength);

		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID((AgpdCharacter *) pcsTarget));

		// 2004.04.06. steeple - 귓말 보낸 사람에게도 보낸다.
		AgsdCharacter* pcsAgsdSender = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsSender);
		if (pcsAgsdSender)
		{
			SendPacket(pvPacket, nPacketLength, pcsAgsdSender->m_dpnidCharacter);
		}

		m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

		return bSendResult;
	}

	return TRUE;
}

BOOL AgsmChatting::SendMessageParty(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength)
{
	if (!pcsSender || !szMessage || lMessageLength < 1 || !strlen(szMessage))
		return FALSE;

	if (!m_pagpmParty)
		return FALSE;

	AgpdParty		*pcsParty	= m_pagpmParty->GetPartyLock((AgpdCharacter *) pcsSender);
	if (!pcsParty)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmChatting->MakePacketChatting(pcsSender->m_lID,
														   AGPDCHATTING_TYPE_PARTY,
														   NULL,
														   NULL,
														   szMessage,
														   lMessageLength,
														   &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
	{
		pcsParty->m_Mutex.Release();
		return FALSE;
	}

	BOOL	bSendResult = m_pagsmParty->SendPacketToParty(pcsParty, pvPacket, nPacketLength, PACKET_PRIORITY_6);

	pcsParty->m_Mutex.Release();

	m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmChatting::SendMessageGuild(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength)
{
	if (!pcsSender || !szMessage || lMessageLength < 1 || !strlen(szMessage))
		return FALSE;

	// 2004.07.18. steeple
	// 길드 메시지는 AgsmGuild 에서 AgsmChatting::SendMessage 를 부르는 식으로 된다.
	return TRUE;
}

// 2003.12.8. 김태희 수정. pcsSender 가 받게 된다.
BOOL AgsmChatting::SendMessageSystem(ApBase* pcsSender, INT32 lLevel, CHAR *szMessage, INT32 lMessageLength, UINT32 dpnID)
{
	if (!szMessage || lMessageLength < 1 || !strlen(szMessage))
		return FALSE;

	if(!pcsSender)
		return FALSE;

	// 먼저 Define 된 것이 아니라면, 제일 낮은 걸로 맞춘다.
	if(lLevel != AGPDCHATTING_TYPE_SYSTEM_LEVEL1 && lLevel != AGPDCHATTING_TYPE_SYSTEM_LEVEL2 &&
		lLevel != AGPDCHATTING_TYPE_SYSTEM_LEVEL3)
		lLevel = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;

	if(dpnID == 0)
		dpnID = m_pagsmCharacter->GetCharDPNID((AgpdCharacter*)pcsSender);

	return SendMessage(dpnID, (AgpdChattingType)lLevel, pcsSender->m_lID, ((AgpdCharacter *) pcsSender)->m_szID, NULL, szMessage, lMessageLength);
}

BOOL AgsmChatting::SendMessageEmphasis(ApBase* pcsSender, CHAR *szMessage, INT32 lMessageLength)
{
	if (!pcsSender || !szMessage || lMessageLength < 1 || !strlen(szMessage))
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmChatting->MakePacketChatting(pcsSender->m_lID, 
														   AGPDCHATTING_TYPE_EMPHASIS,
														   NULL,
														   NULL,
														   szMessage,
														   lMessageLength,
														   &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pagpmChatting->m_csPacket.SetCID(pvPacket, nPacketLength, pcsSender->m_lID);

	BOOL bSendResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, ((AgpdCharacter *) pcsSender)->m_stPos, PACKET_PRIORITY_6);

	m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmChatting::SendMessage(UINT32 dpnid, AgpdChattingType eChatType, INT32 lSenderID, CHAR *szSenderName, CHAR *szTargetID, CHAR *szMessage, INT32 lMessageLength)
{
	if (!szMessage || !szMessage[0] || lMessageLength < 1)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmChatting->MakePacketChatting(lSenderID,
														   eChatType,
														   szSenderName,
														   szTargetID,
														   szMessage,
														   lMessageLength,
														   &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, dpnid);

	m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

// 2003.11.18. 김태희 추가. 므흣
BOOL AgsmChatting::SendMessageAll(AgpdChattingType eChatType, INT32 lSenderID, CHAR* szMessage, INT32 lMessageLength)
{
	if (!szMessage || !szMessage[0] || lMessageLength < 1)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmChatting->MakePacketChatting(lSenderID,
														   eChatType,
														   NULL,
														   NULL,
														   szMessage,
														   lMessageLength,
														   &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	//BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, 0, APMODULE_SENDPACKET_ALL_PLAYERS);
	BOOL bSendResult = m_pagsmAOIFilter->SendPacketAllGroups(pvPacket, nPacketLength, PACKET_PRIORITY_6);

	m_pagpmChatting->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmChatting::SendMessageCommandFailed(UINT32 dpnid, eAgsmChatCommandFailType eFailType, INT32 lTargetID)
{
	if (dpnid == 0)
		return FALSE;

	switch (eFailType) {
	case AGSMCHAT_COMMAND_FAIL_NO_EXIST_TARGET:
		{
			// 귓속말을 날렸는데 대상이 없는 경우이당...
		}
		break;
	}

	return TRUE;
}

BOOL AgsmChatting::SetCallbackQuestComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_QUEST_COMPLETE, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackGuildMemberInvite(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_GUILD_MEMBER_INVITE, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackGuildLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_GUILD_LEAVE, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackEventMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_EVENT_MESSAGE, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackAreaChattingRace(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_AREA_CHATTING_RACE, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackAreaChattingAll(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_AREA_CHATTING_ALL, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackAreaChattingGlobal(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_AREA_CHATTING_GLOBAL, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackGuildJointMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_GUILD_JOINT_MESSAGE, pfCallback, pClass);
}

BOOL AgsmChatting::SetCallbackArchlordMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHATTING_CB_ARCHLORD_MESSAGE, pfCallback, pClass);
}

BOOL AgsmChatting::StreamReadChatEvent(CHAR *pszFile)
{
	if (!pszFile || !pszFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	if (nRow <= 0)
		return TRUE;

	if (nColumn < 3)
		return FALSE;

	m_pChatEventData	= new AgsdChatEvent [nRow];
	if (!m_pChatEventData)
		return FALSE;

	m_lEventDataCount	= nRow;

	for (int i = 0; i < nRow; ++i)
	{
		m_pChatEventData[i].strChatMessage.Append(csExcelTxtLib.GetData(0, i));
		m_pChatEventData[i].lSkillTID	= atoi(csExcelTxtLib.GetData(1, i));
		m_pChatEventData[i].lSkillLevel	= atoi(csExcelTxtLib.GetData(2, i));

		m_pagpmSkill->AddEventSkillTID(m_pChatEventData[i].lSkillTID);
	}

	return TRUE;
}
