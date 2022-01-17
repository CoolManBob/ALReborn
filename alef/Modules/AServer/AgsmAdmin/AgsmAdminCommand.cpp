// AgsmAdminCommand.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 03. 29.
//
// Code 양이 너무 길어져서 관리를 위해서 분리

#include "AgsmAdmin.h"
#include <list>
#include "AuStrTable.h"
#include "ApUtil.h"
#include <atltime.h>
#include <AgsmBattleGround.h>
#include <AgsmConfig.h>
#include <AgsmNpcManager.h>
#include <AgpmPvP.h>
#include <AgspCashItemTimeExtend.h>
#include <AgpmEpicZone.h>
#include <AgsmEpicZone.h>
#include <AgpmBuddy.h>

#define AGSMADMIN_MSG_CONNECTABLE_NONE				"접속 제한을 '모두다' 로 바꾸었습니다"
#define AGSMADMIN_MSG_CONNECTABLE_CLOSE_BETA		"접속 제한을 '클로즈 베타' 로 바꾸었습니다"
#define AGSMADMIN_MSG_CONNECTABLE_DEVELOPER			"접속 제한을 '개발자' 로 바꾸었습니다"
#define AGSMADMIN_MSG_CONNECTABLE_ADMIN				"접속 제한을 '운영자' 로 바꾸었습니다"

extern BOOL					g_bPrintNumCharacter;
extern BOOL					g_bPrintActiveCellCount;
extern BOOL					g_bPrintRemovePoolTID;

namespace {	// local only function
	
	bool IsRenshpereArea(INT16 regionIndex)
	{
		return (regionIndex == 81 || regionIndex == 88);
	}
}

void AgsmAdmin::InitCommandString()
{
	struct CommandPair
	{
		std::string strcommands;
		_eAgsmAdminCommands ecommands;
	};

	CommandPair CommandsArray[] = 
	{
		{ "/cha_up",		ACOMMAND_cha_up},
		{ "/battleground",	ACOMMAND_battleground},
		{ "/bg_start",		ACOMMAND_bg_start	},
		{ "/ebg_start",		ACOMMAND_ebg_start	},
		{ "/bg_end",		ACOMMAND_bg_end },
		{ "/ebg_end",		ACOMMAND_ebg_end },
		{ "/expup",			ACOMMAND_expup },
		{ "/reload",		ACOMMAND_reload },
		{ "/crash_server",	ACOMMAND_crash },
		{ "/enable_pvp",	ACOMMAND_enable_pvp },
		{ "/disable_pvp",	ACOMMAND_disable_pvp },
		{ "/castskill",     ACOMMAND_cast_skill },
		{ "/nochat",		ACOMMAND_disable_chat},
		{ "/productup",		ACOMMAND_product_skill_up},
		{ "/murderpoint_up",ACOMMAND_murderpoint_up},
		{ "/cashmall",		ACOMMAND_cashmall},
		{ "/WinGuild",		ACOMMAND_WinGuild },
		{ "", ACOMMAND_MAX }
	};

	for(int i = 0; i != ACOMMAND_MAX; i++)
		m_AdminCommandString.insert(AdminCommandsMap::value_type(CommandsArray[i].strcommands, CommandsArray[i].ecommands));

}

///////////////////////////////////////////////////////////////////////////////
// Command

// AgsmChatting 의 ParseCommand 에서 불러준다.\
//
// 현재 명령어.
// 1. Help (모든 사용자가 사용가능하다.)
// 2. Notice (Admin Level 을 체크해서 Admin 일 때만 가능하다.)
//
// TRUE 를 리턴하게 되면, Admin Command 로 인식 되어서 채팅 메시지가 뿌려지지 않는다.
// FALSE 를 리턴하게 되면, 일반 채팅 메시지로 분류한다.
BOOL AgsmAdmin::ParseCommand(AgpdChatData * pstChatData, BOOL bCheckAdmin)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstChatData)
		return FALSE;

	AgpdCharacter* pcsAgpdCharacter = NULL;
	AgsdCharacter* pcsAgsdCharacter = NULL;

	if (bCheckAdmin)
	{
		if (!pstChatData->pcsSenderBase)
			return FALSE;

		// Sender 는 이미 Lock 되어 있다.
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(pstChatData->pcsSenderBase->m_lID);
		if(!pcsAgpdCharacter)
			return FALSE;

		pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

		AgpdAdmin* pcsAttachedAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
		if(!pcsAttachedAdmin)
			return FALSE;
	}

	// 문자열 검사
	if(!pstChatData->szMessage || pstChatData->lMessageLength == 0)
		return FALSE;

	INT32	i = 0, j = 0, k = 0;
	BOOL	bExistArg = FALSE;
	CHAR	szArg[256] = {0,};
	//INT32	lArgLen;

	for(i; i < (INT32) pstChatData->lMessageLength; i++)
		if(pstChatData->szMessage[i] != ' ')
			break;

	// '/' 검사
	if(pstChatData->szMessage[i] != '/')
		return FALSE;

	// '/' 로부터 첫번째 공백 문자 인덱스를 찾자.
	for(j = i + 1, k = i + 1; j < (INT32) pstChatData->lMessageLength; ++j, ++k)
	{
		if(pstChatData->szMessage[j] == ' ')
		{
			bExistArg = TRUE;

			if (pstChatData->lMessageLength > 255)
				memcpy(szArg, pstChatData->szMessage + j + 1, 255);
			else
				memcpy(szArg, pstChatData->szMessage + j + 1, pstChatData->lMessageLength - (j + 1));

			++j;
			break;
		}
	}

	// Admin Command에서 ' ' 없어도 되게
	//if(j == pstChatData->lMessageLength || j - i < 2)
	if(k - i < 2)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	// arycoat(2008.06.09)
	CHAR szCommans[1024] = { 0, };
	memcpy(szCommans, pstChatData->szMessage, pstChatData->lMessageLength);
	CGetArg2 argCommands(szCommans);
	
	AdminCommandsMap::iterator it = m_AdminCommandString.find(argCommands.GetParam(0));
	if(it != m_AdminCommandString.end())
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		switch(it->second)
		{
			case ACOMMAND_cha_up:
			{
				if(argCommands.GetArgCount() < 2)
					return FALSE;

				INT32 lPoint = atoi(argCommands.GetParam(1));
				
				m_pagpmCharacter->UpdateCharismaPoint(pcsAgpdCharacter, lPoint);
			} break;
			case ACOMMAND_battleground:
			{
				if(argCommands.GetArgCount() < 6)
					return FALSE;
				
				INT32 nDay		=  atoi(argCommands.GetParam(1));
				INT32 nHour		=  atoi(argCommands.GetParam(2));
				INT32 nMin		=  atoi(argCommands.GetParam(3));
				INT32 nDuring	=  atoi(argCommands.GetParam(4));
				BOOL  bEvent	=  atoi(argCommands.GetParam(5)) == 1 ? TRUE : FALSE; 

				if( m_pagsmBattleGround )
					m_pagsmBattleGround->SetBattleGroundTime(TRUE, nDay, nHour, nMin, nDuring, bEvent);
			} break;
			case ACOMMAND_ebg_start:
			case ACOMMAND_bg_start:
			{
				CTime bg_start = CTime::GetCurrentTime() + CTimeSpan( 0, 0, 2, 0);

				INT32 nDay		=  bg_start.GetDayOfWeek();
				INT32 nHour		=  bg_start.GetHour();
				INT32 nMin		=  bg_start.GetMinute();
				INT32 nDuring	=  60;
				BOOL  bEvent	=  (it->second == ACOMMAND_bg_start) ? FALSE : TRUE; 

				if( m_pagsmBattleGround )
					m_pagsmBattleGround->SetBattleGroundTime(TRUE, nDay, nHour, nMin, nDuring, bEvent);
			} break;
			case ACOMMAND_ebg_end:
			{
				if( m_pagsmBattleGround )
					m_pagsmBattleGround->SetBattleGroundTime(TRUE, 0, 0, 0, 0, TRUE);
			} break;
			case ACOMMAND_bg_end:
			{
				if( m_pagsmBattleGround )
					m_pagsmBattleGround->SetBattleGroundTime(TRUE, 0, 0, 0, 0, FALSE);
			} break;
			case ACOMMAND_expup:
				{
					if(argCommands.GetArgCount() < 2)
						return FALSE;

					INT32 lPoint = atoi(argCommands.GetParam(1));

					m_pagsmDeath->AddBonusExpToChar(pcsAgpdCharacter, NULL, lPoint, FALSE, FALSE, FALSE);
				} break;

			case ACOMMAND_reload :
				{
					if(argCommands.GetArgCount() < 2)
						return FALSE;

					CHAR* strParam = argCommands.GetParam(1);
					if(!strcmp(strParam, "event"))
					{
						AuAutoLock pLock(m_pagsmConfig->m_Mutex);
						if(pLock.Result())
						{
							m_pagsmConfig->m_bReloadConfigLua = TRUE;
						}
					}
					else if(!strcmp(strParam, "npclua"))
					{
						m_pagsmNpcManager->LoadXmlFile();
					}
					else if(!strcmp(strParam, "checkcharacter"))
					{
						m_pagsmCharacter->m_bReloadLua = TRUE;
					}
					else if(!strcmp(strParam, "epiczone"))
					{
						m_pagsmEpicZone->LoadXml();
					}
					else if(!strcmp(strParam, "adminclientip"))
					{
						LoadAllowedAdminIPList();
					}
					else if(!strcmp(strParam, "region"))
					{
						m_papmMap->LoadRegionPerculiarity( TRUE );
					}
					else if(!strcmp(strParam, "tpack"))
					{
						m_pagsmConfig->LoadTPackConfig();
					}
					else if(!strcmp(strParam, "buddyconstrict"))
					{
						if(m_pagpmBuddy)
							m_pagpmBuddy->ReadMentorConstrinctFile("Ini\\BuddyConstrict.xml");
					}
				} break;

			case ACOMMAND_crash:
				{
					CHAR *crash = NULL;
					*crash = 'a';
				} break;

			case ACOMMAND_enable_pvp:
				{
					INT32 lMinLevel = 0;

					if (argCommands.GetArgCount() < 2)
					{
						lMinLevel = AGPMPVP_MINIMUM_ABLE_LEVEL;
					}
					else
					{
						lMinLevel = atoi(argCommands.GetParam(1));
					}
					
					m_pagpmConfig->SetEnablePvP(TRUE);
					m_pagpmPvP->SetMinPvPLevel(lMinLevel);
				} break;

			case ACOMMAND_disable_pvp:
				{
					m_pagpmConfig->SetEnablePvP(FALSE);
				} break;

			case ACOMMAND_cast_skill:
				{
					if(argCommands.GetArgCount() < 3)
						return FALSE;
					
					CHAR  *TargetName = argCommands.GetParam(1);
					INT32 lSkillID    = atoi(argCommands.GetParam(2));
									
					AgpdCharacter	  *pcsTargetCharacter = m_pagpmCharacter->GetCharacter(TargetName);
					AgpdCharacter	  *pcsCastCharacter = m_pagpmCharacter->GetCharacter(TargetName);
					if(pcsTargetCharacter)
					{
						m_pagsmSkill->CastSkill(pcsCastCharacter, lSkillID, 1, (ApBase*)pcsTargetCharacter, TRUE);
					}
					
				} break;
			case ACOMMAND_disable_chat:
				{
					if(argCommands.GetArgCount() < 4)
						return FALSE;

					CHAR *TargetName		= argCommands.GetParam(1);
					CHAR *szReason			= argCommands.GetParam(3);
					UINT32 lSkillDuration	= atoi(argCommands.GetParam(2));
					UINT32 lSkillLevel		= 1;

					// 분단위를 변환해준다.
					UINT32 lSkillDurationMillisecond		= lSkillDuration * 60 * 1000;

					AgpdCharacter *pcsTargetCharacter = m_pagpmCharacter->GetCharacter(TargetName);
					AgpdCharacter *pcsCastCharacter = m_pagpmCharacter->GetCharacter(TargetName);
					if(pcsTargetCharacter && pcsTargetCharacter->m_pcsCharacterTemplate)
					{
						AgsdCastSkill pcsAgsdCastSkill;
						pcsAgsdCastSkill.pcsBase			= static_cast<ApBase*>(pcsCastCharacter);
						pcsAgsdCastSkill.lSkillTID			= AGSM_ADMIN_CAST_SKILL_NOCHAT;
						pcsAgsdCastSkill.lSkillLevel		= lSkillLevel;
						pcsAgsdCastSkill.pcsTargetBase		= static_cast<ApBase*>(pcsTargetCharacter);
						pcsAgsdCastSkill.bForceAttack		= TRUE;
						pcsAgsdCastSkill.bMonsterUse		= FALSE;
						pcsAgsdCastSkill.lSkillScrollIID	= AP_INVALID_IID;

						AgpdSkillTemplate *pcsTemplate		= m_pagpmSkill->GetSkillTemplate(pcsAgsdCastSkill.lSkillTID);
						if(pcsTemplate)
						{
							if(m_pagpmSkill->SetSkillTemplateDurationTime(pcsTemplate, lSkillDurationMillisecond, lSkillLevel))
							{
								if(m_pagsmSkill->CastSkill(pcsAgsdCastSkill))
								{
									AgsdCharacter *pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsTargetCharacter);
									if(pcsAgsdCharacter)
									{
										TCHAR szDesc[AGPDLOG_MAX_DESCRIPTION + 1];
										ZeroMemory(szDesc, sizeof(szDesc));
										sprintf(szDesc, "%d분 제재 / 이유 : %s", lSkillDuration, szReason);

										INT32 lLevel = m_pagpmCharacter->GetLevel(pcsTargetCharacter);
										INT64 lExp	 = m_pagpmCharacter->GetExp(pcsTargetCharacter); 

										m_pagpmLog->WriteLog_NoChat(0, &pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szAccountID,
																	pcsAgsdCharacter->m_szServerName, pcsTargetCharacter->m_szID, 
																	((AgpdCharacterTemplate*)pcsTargetCharacter->m_pcsCharacterTemplate)->m_lID,
																	lLevel, lExp, pcsTargetCharacter->m_llMoney, pcsTargetCharacter->m_llBankMoney,
																	AGSM_ADMIN_CAST_SKILL_NOCHAT, szDesc, 0, pcsAgpdCharacter->m_szID);
									}
								}
							}
						}
					}
				} break;

			case ACOMMAND_product_skill_up:
				{
					if(argCommands.GetArgCount() < 3)
						return FALSE;

					CHAR	*szProductSkill = argCommands.GetParam(1);
					INT32	lSkillLevel = atoi(argCommands.GetParam(2));
					if(NULL == szProductSkill || lSkillLevel < 1)
						return FALSE;

					if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) || 
						m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
						return FALSE;

					INT32 lProductSkillTID = 0;
					if(strcmp(szProductSkill, "gather") == 0)
					{
						lProductSkillTID = 663;
					}
					else if(strcmp(szProductSkill, "cook") == 0)
					{
						lProductSkillTID = 203;
					}
					else if(strcmp(szProductSkill, "alchemist") == 0)
					{
						lProductSkillTID = 285;
					}
					else
						return FALSE;

					m_pagpmProduct->SetSkillLevel(pcsAgpdCharacter, lProductSkillTID, lSkillLevel);

					char strCharBuff[256] = { 0, };
					sprintf_s(strCharBuff, sizeof(strCharBuff), "[productup] %s(%s) : %s\n",
						bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
						bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
						szArg);
					AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

				} break;

			case ACOMMAND_murderpoint_up:
				{
					if(argCommands.GetArgCount() < 3)
						return FALSE;

					CHAR	*szCharName			= argCommands.GetParam(1);
					INT32	lMurderPoint		= atoi(argCommands.GetParam(2));
					if(NULL == szCharName || lMurderPoint < 0)
						return FALSE;

					if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) || 
						m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
						return FALSE;

					AgpdCharacter *pcsCharacter  = m_pagpmCharacter->GetCharacter(szCharName);
					if(NULL == pcsCharacter)
						return FALSE;

					m_pagpmCharacter->UpdateMurdererPoint(pcsCharacter, lMurderPoint);

					char strCharBuff[256] = { 0, };
					sprintf_s(strCharBuff, sizeof(strCharBuff), "[murderpoint_up] %s(%s) : %s\n",
						bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
						bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
						szArg);
					AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	
				} break;

			case ACOMMAND_cashmall:
				{
					if(argCommands.GetArgCount() < 2 )
						return FALSE;

					if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) || 
						m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
						return FALSE;

					BOOL bCashMallUse   = TRUE;
					CHAR *pCashMallUse  = argCommands.GetParam(1);
					if(!pCashMallUse)
						return FALSE;

					if(!strcmp(pCashMallUse, "ON"))
					{
						bCashMallUse	= TRUE;	
					}
					else if(!strcmp(pCashMallUse, "OFF"))
					{
						bCashMallUse	= FALSE;
					}
					else
						return FALSE;

					// CashMall On / Off 설정
					m_pagpmCashMall->SetUseCashMall(bCashMallUse);

				} break;
			case ACOMMAND_WinGuild:
				{
					if(argCommands.GetArgCount() < 3 )
						return FALSE;

					INT32 lRanking = (INT32)atoi(argCommands.GetParam(1));

					CHAR *p2dnWinnerGuild  = argCommands.GetParam(2);
					if(!p2dnWinnerGuild)
						return FALSE;

					m_pagsmGuild->SetWinnerGuild(p2dnWinnerGuild, lRanking);
					m_pagsmGuild->SaveWinnerGuild();
				} break;
		}

		return TRUE;
	}

	// Notice
	else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_ENG), k - i) == 0 ||
			strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_LOCALIZE), k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[NOTICE] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return ProcessCommandNotice(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	// Notice Repeat
	else if((ServerStr().GetStr(STI_CHAT_NOTICE_REPEAT_ENG) && strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_REPEAT_ENG), k - i) == 0) ||
			(ServerStr().GetStr(STI_CHAT_NOTICE_REPEAT_LOCALIZE) &&strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_REPEAT_LOCALIZE), k - i) == 0))
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[NOTICE REPEAT] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return ProcessCommandNoticeRepeat(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	// Notice Repeat List
	else if((ServerStr().GetStr(STI_CHAT_NOTICE_LIST_ENG) && strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_LIST_ENG), k - i) == 0) ||
			(ServerStr().GetStr(STI_CHAT_NOTICE_LIST_LOCALIZE) && strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_LIST_LOCALIZE), k - i) == 0))
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[NOTICE LIST] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return ProcessCommandNoticeList(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	// Notice Repeat Delete
	else if((ServerStr().GetStr(STI_CHAT_NOTICE_DELETE_ENG) && strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_DELETE_ENG), k - i) == 0) ||
			(ServerStr().GetStr(STI_CHAT_NOTICE_DELETE_LOCALIZE) && strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_DELETE_LOCALIZE), k - i) == 0))
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[NOTICE DELETE] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return ProcessCommandNoticeDelete(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	// Notice Repeat Clear
	else if((ServerStr().GetStr(STI_CHAT_NOTICE_CLEAR_ENG) && strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_CLEAR_ENG), k - i) == 0) ||
			(ServerStr().GetStr(STI_CHAT_NOTICE_CLEAR_LOCALIZE) && strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_NOTICE_CLEAR_LOCALIZE), k - i) == 0))
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[NOTICE CLEAR] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return ProcessCommandNoticeClear(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_CREATE_ENG), k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
			return FALSE;

		return ProcessCommandCreate(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j), 10);
	}
	else if(strncmp(pstChatData->szMessage + i, "/createone", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
			return FALSE;

		return ProcessCommandCreate(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j), 1);
	}
	//else if (strncmp(pstChatData->szMessage + i, "/createcash", k - i) == 0)
	//{
	//	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
	//		return FALSE;

	//	return ProcessCommandCreateCashItem(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	//}
	else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_RIDE_ENG), k - i) == 0)
	{
		if (!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandRide(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_SUMMONS), k - i) == 0)
	{
		if (!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandSummons(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	//else if(strncmp(pstChatData->szMessage + i, AGPMCHAT_COMMAND_QCREATE_ENG, k - i) == 0)
	//{
	//	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
	//		return FALSE;

	//	return ProcessCommandQCreate(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	//}
	// Server Open (Connectable Account Level)
	else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_OPEN_ENG), k - i) == 0 ||
			strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_OPEN_LOCALIZE), k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandOpen(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_QUEST_CLEAR), k - i) == 0)
	{
		if (!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandQuest(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	// Guild - 임시임
	else if(strncmp(pstChatData->szMessage + i, "/Guild", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandGuild(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	// 무적 커맨드
	else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_INVINCIBLE), k - i) == 0
		||strncmp(pstChatData->szMessage + i, "/invincible", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[INVINCIBLE] %s(%s) : %s\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
		
		if (pcsAgsdCharacter->m_bIsSuperMan)
			pcsAgsdCharacter->m_bIsSuperMan		= FALSE;
		else
			pcsAgsdCharacter->m_bIsSuperMan		= TRUE;

		return TRUE;
	}
	// Max Defence 커맨드
	else if(strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_INVINCIBLE), k - i) == 0
		||strncmp(pstChatData->szMessage + i, "/maxdefence", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[MAX Defence] %s(%s) : %s\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

		if (pcsAgsdCharacter->m_bIsMaxDefence)
			pcsAgsdCharacter->m_bIsMaxDefence		= FALSE;
		else
			pcsAgsdCharacter->m_bIsMaxDefence		= TRUE;

		return TRUE;
	}
	/*
	// PvP 열기, 닫기
	else if(strncmp(pstChatData->szMessage + i, "/pvpOpen", k - i) == 0)
	{
		if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		m_pagpmCharacter->PvPModeOn();

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/pvpClose", k - i) == 0)
	{
		if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		m_pagpmCharacter->PvPModeOff();

		return TRUE;
	}
	*/
	else if(strncmp(pstChatData->szMessage + i, "/move", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandMove(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j));
	}
	else if (strncmp(pstChatData->szMessage + i, "/send", k - i) == 0)
	{
		if (!pcsAgpdCharacter)
			return FALSE;

		if (m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		{
			// 케릭터 날려 보내기..
			// 커맨드는
			// "/send charactername index"
			// "/send charactername posx posz"
			// 두가지로 사용이 가능하다.
			AuPOS	stPos;
			ZeroMemory(&stPos, sizeof(AuPOS));

			if (!bExistArg)
				return TRUE;

			CGetArg2	arg( szArg );

			AgpdCharacter* pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock( arg.GetParam(0) );
			if (!pcsTargetCharacter)
			{
				return FALSE;
			}

			int nCount = arg.GetArgCount();
			switch( nCount )
			{
			case 2:
				{
					// 지정 인덱스로 이동
					AuPOS	posZero = { 0 , 0 , 0 };
					AuPOS	arrayPos[] = {
						{808400,0,630200},
						{808400,0,628400},
						{808400,0,626600},
						{808400,0,624800},
						{803600,0,624800},
						{803600,0,626600},
						{803600,0,628400},
						{803600,0,630200},
						posZero
					};
					// 인덱스 갯수 계산해둠.
					int nPosCount = 0;
					for( int i = 0 ; i < 256 /*MAX*/ ; i++ )
					{
						if( arrayPos[i] == posZero ) break;
						nPosCount ++;
					}

					// 제로 베이스로 변경..
					int nIndex = atoi( arg.GetParam(1) ) - 1;
					if( nIndex >= 0 && nIndex < nPosCount )
					{
						stPos = arrayPos[ nIndex ];
					}
					else
					{
						// 인덱스 오류
						pcsTargetCharacter->m_Mutex.Release();
						return FALSE;
					}
				}
				break;
			case 3:
				{
					// 지정 좌표로 이동.
					stPos.x = (FLOAT)atof(arg.GetParam(1));
					stPos.z = (FLOAT)atof(arg.GetParam(2));
				}
				break;
			default:
			case 1:
				{
					// 인자 오류
					pcsTargetCharacter->m_Mutex.Release();
					return FALSE;
				}
			}

			m_pagpmCharacter->StopCharacter( pcsTargetCharacter, NULL);
			m_pagpmCharacter->UpdatePosition( pcsTargetCharacter, &stPos, FALSE, TRUE);

			// 2006.11.22. steeple
			// 소환수도 옮겨준다.
			if(m_pagpmSummons)
				m_pagpmSummons->UpdateAllSummonsPosToOwner( pcsTargetCharacter );

			pcsTargetCharacter->m_Mutex.Release();

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[SEND] %s(%s) : %s\n",
				pcsTargetCharacter->m_szID,
				pcsAgsdCharacter->m_szAccountID,
				szArg);
			AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
		}
	}
	else if (strncmp(pstChatData->szMessage + i, "/go", k - i) == 0)
	{
		if (!pcsAgpdCharacter)
			return FALSE;
		else
		{
			// 2006.09.24. steeple
			// 캐시아템이 사용중인지 본다.
			// 사용중인지 아닌지 체크 하지 않고 있으면 바로 까준다.

			AgpdCharacter* pcsCharacter = (AgpdCharacter*)pstChatData->pcsSenderBase;
			if(!pcsCharacter)
				return FALSE;

			AgsdCharacter *pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (NULL == pagsdCharacter)
				return FALSE;

			// 정상이 아닐 땐 사용불가.
			if(!pcsCharacter->m_bIsAddMap)
				return FALSE;

			if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				return FALSE;

			if (m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
				return FALSE;

			if( m_pagpmCharacter->IsCombatMode(pcsCharacter) 
				|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE)
				|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK)
				|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
				|| (m_pagpmCharacter->IsActionBlockCondition(pcsCharacter)))
			{
				if(m_pagsmSystemMessage)
					m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
				return FALSE;
			}

			// 렌스피어 지역에 있다면 사용 할 수 없다.
			if ( IsRenshpereArea(pcsCharacter->m_nBindingRegionIndex) )
				return FALSE;

			// 2007.05.04. steeple
			if(m_pagpmCharacter->IsInJail(pcsCharacter))
			{
				if(m_pagsmSystemMessage)
					m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
				return FALSE;
			}

			AgpdItemADChar* pAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
			if(!pAgpdItemADChar)
				return FALSE;

			// 점프 아이템을 찾는다.
			AgpdGridItem* pAgpdGridItem = m_pagpmItem->GetCashItemUsableByType(pcsCharacter, AGPMITEM_USABLE_TYPE_JUMP, 0);
			if (NULL == pAgpdGridItem)
				return FALSE;
			
			AgpdItem* pAgpdItem = m_pagpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if (NULL == pAgpdItem)
				return FALSE;

			UINT32 ulClockCount = GetClockCount();

			// 재사용 시간 검사한다.
			if (pAgpdItemADChar->m_ulUseJumpTime > 0 &&
				pAgpdItemADChar->m_ulUseJumpTime + ((AgpdItemTemplateUsable*)pAgpdItem->m_pcsItemTemplate)->m_ulUseInterval > ulClockCount)
			{
				if(m_pagsmSystemMessage)
					m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_REUSE_INTERVAL);
				return FALSE;
			}

			if (!bExistArg)
				return TRUE;

			AgpdCharacter* pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(szArg);
			if (!pcsTargetCharacter)
				return TRUE;

			ApmMap::RegionTemplate *pcsTargetRegionTemplate = m_papmMap->GetTemplate(pcsTargetCharacter->m_nBindingRegionIndex);
			ApmMap::RegionTemplate *pcsBaseRegionTemplate = m_papmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
			if(NULL == pcsTargetRegionTemplate || NULL == pcsBaseRegionTemplate)
				return FALSE;

			AgpdSiegeWar	*pcsSiegeWar	= m_pagpmSiegeWar->GetSiegeWarInfo(pcsCharacter);
			AgpdSiegeWar	*pcsSiegeWarTarget	= m_pagpmSiegeWar->GetSiegeWarInfo(pcsTargetCharacter);
			if (m_pagpmSiegeWar->IsStarted(pcsSiegeWar) ||
				m_pagpmSiegeWar->IsStarted(pcsSiegeWarTarget))
			{
				pcsTargetCharacter->m_Mutex.Release();
				return FALSE;
			}

			// 공성 지역 점검
			if (m_pagpmSiegeWar)
			{
				// 공성중이라면 이동할 수 없다.
				AgpdSiegeWar	*pcsSiegeWar	= m_pagpmSiegeWar->GetSiegeWarInfo(pcsTargetCharacter);
				if (pcsSiegeWar && pcsSiegeWar->m_eCurrentStatus >= AGPD_SIEGE_WAR_STATUS_START && pcsSiegeWar->m_eCurrentStatus <= AGPD_SIEGE_WAR_STATUS_OPEN_EYES)
				{
					pcsTargetCharacter->m_Mutex.Release();
					return FALSE;
				}

				// 렌스피어로는 무조건 못간다.
				if ( IsRenshpereArea(pcsTargetCharacter->m_nBindingRegionIndex) )
				{
					pcsTargetCharacter->m_Mutex.Release();
					return FALSE;
				}
			}

			// 타겟이 PC 가 아니면 못 간다. 2007.02.22. steeple
			if(!m_pagpmCharacter->IsPC(pcsTargetCharacter) || _tcslen(pcsTargetCharacter->m_szID) == 0)
			{
				pcsTargetCharacter->m_Mutex.Release();
				return FALSE;
			}

			// 운영자에게도 못 날라간다. 2007.02.22. steeple
			// 아크로드에게도 못 날라간다. 2007.02.22. steeple
			if( (!m_pagpmAdmin->IsAdminCharacter(pcsCharacter) && m_pagpmAdmin->IsAdminCharacter(pcsTargetCharacter)) || 
				m_pagpmCharacter->IsArchlord(pcsTargetCharacter))
			{
				pcsTargetCharacter->m_Mutex.Release();
				return FALSE;
			}

			// 타겟캐릭터가 감옥에 있어도 가지 못한다. 2009.03.02. iluvs
			// 타겟캐릭터가 상담소에 있어도 가지 못한다. 2009.03.04. iluvs
			if(m_pagpmCharacter->IsInJail(pcsTargetCharacter) || m_pagpmCharacter->IsInConsulationOffice(pcsTargetCharacter) ||
			   pcsTargetRegionTemplate->ti.stType.bRecallItem || pcsBaseRegionTemplate->ti.stType.bRecallItem)
			{
				pcsTargetCharacter->m_Mutex.Release();
				if(m_pagsmSystemMessage)
					m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
				return FALSE;
			}

			// 타겟캐릭터가 게임중이 아니라면 가지 못한다. [ 2011-08-16 silvermoo ]
			if(pcsTargetCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD)
			{
				pcsTargetCharacter->m_Mutex.Release();
				if(m_pagsmSystemMessage)
					m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
				return FALSE;
			}

			// 레벨 제한이 있는지 확인한다.
			if( pcsTargetCharacter->m_nBindingRegionIndex != pcsCharacter->m_nBindingRegionIndex )
			{
				//ApmMap::RegionTemplate	*pcsRegionTemplate	= m_papmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
				ApmMap::RegionTemplate	*pcsTargetRegion	= m_papmMap->GetTemplate(pcsTargetCharacter->m_nBindingRegionIndex);
				// NULL 체크 추가 2008.05.21. steeple
				if(pcsTargetRegion)
				{
					INT32	nLevelOriginal = m_pagpmCharacter->GetLevelOriginal( pcsCharacter );

					//  최저 레벨보다 높아야 한다
					if( pcsTargetRegion->nLevelMin && nLevelOriginal < pcsTargetRegion->nLevelMin )
					{
						if(m_pagsmSystemMessage)
							m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_GO_COMMAND_LACK_OF_LEVEL );

						pcsTargetCharacter->m_Mutex.Release();
						return FALSE;
					}

					if( m_pagpmCharacter->IsArchlord(pcsCharacter) && pcsTargetRegion->nLevelLimit )
					{
						// 아크로드는 레벨제한 지역에 못가요.
						pcsTargetCharacter->m_Mutex.Release();
						return FALSE;
					}

					if( pcsCharacter->m_bIsTrasform && pcsTargetRegion->nLevelLimit )
					{
						// 변신한 놈은 레벨 제한 지역에 못 들어간다.
						pcsTargetCharacter->m_Mutex.Release();
						return FALSE;
					}

					if(m_pagpmCharacter->IsRideOn(pcsCharacter) && pcsTargetRegion->nLevelLimit)
					{
						// 탈것에 타고있는 상태로는 레벨 제한 지역에 못 들어간다.
						pcsTargetCharacter->m_Mutex.Release();
						return FALSE;
					}
				}

				//////////////////////////////////////////////////////////////////////////
				//
				AuPOS stTeleportPos = pcsTargetCharacter->m_stPos;
				AuPOS stCharacterPos = pcsCharacter->m_stPos;

				if( m_papmMap->GetTargetPositionLevelLimit( stTeleportPos ) 
					&& m_papmMap->GetTargetPositionLevelLimit( stCharacterPos ) )
				{
					if(m_pagsmSystemMessage)
						m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );
					
					return FALSE;
				}
			}

			////////////////////////////////////////////////////////
			/// 여기서부터 케릭터 이동 처리.
			////////////////////////////////////////////////////////
			m_pagpmCharacter->SetActionBlockTime(pcsCharacter, 3000);
			pagsdCharacter->m_bIsTeleportBlock = TRUE;

			m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);
			m_pagpmCharacter->UpdatePosition(pcsCharacter, &pcsTargetCharacter->m_stPos, FALSE, TRUE);

			pagsdCharacter->m_bIsTeleportBlock = FALSE;

			pcsTargetCharacter->m_Mutex.Release();

			// 2006.11.22. steeple
			// 소환수도 옮겨준다.
			if(m_pagpmSummons)
				m_pagpmSummons->UpdateAllSummonsPosToOwner(pcsCharacter);

			// 성공하였다.
			// 시간 새로 세팅한다.
			pAgpdItemADChar->m_ulUseJumpTime = ulClockCount;

			// send message
			if(m_pagsmSystemMessage)
				m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_JUMP_ITEM_USED, -1, -1,
										((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_szName);

			// log
			m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsCharacter->m_lID, pAgpdItem, 1);

			// sub item
			++pAgpdItem->m_lCashItemUseCount;
			m_pagpmItem->SubItemStackCount(pAgpdItem, 1);
		}

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/goadmin", k - i) == 0)
	{
		if (!pcsAgpdCharacter)
			return FALSE;

		if (m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		{
			AuPOS	stPos;
			ZeroMemory(&stPos, sizeof(AuPOS));

			if (!bExistArg)
				return TRUE;

			AgpdCharacter* pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(szArg);
			if (!pcsTargetCharacter)
			{
				AgpdCharacterTemplate	*pcsCharacterTemplate	= m_pagpmCharacter->GetCharacterTemplate(szArg);
				if (!pcsCharacterTemplate)
					return FALSE;

				INT32			lIndex	= 0;
				pcsTargetCharacter = m_pagpmCharacter->GetCharSequence(&lIndex);

				while (pcsTargetCharacter)
				{
					if (pcsTargetCharacter->m_pcsCharacterTemplate == pcsCharacterTemplate)
						break;

					pcsTargetCharacter = m_pagpmCharacter->GetCharSequence(&lIndex);
				}

				if (!pcsTargetCharacter)
					return FALSE;

				if (!pcsTargetCharacter->m_Mutex.WLock())
					return FALSE;
			}

			//			m_pagpmCharacter->UpdatePosition((AgpdCharacter *) pstChatData->pcsSenderBase, &stPos, FALSE, TRUE);
			m_pagpmCharacter->StopCharacter((AgpdCharacter *) pstChatData->pcsSenderBase, NULL);
			m_pagpmCharacter->UpdatePosition(((AgpdCharacter *) pstChatData->pcsSenderBase), &pcsTargetCharacter->m_stPos, FALSE, TRUE);

			pcsTargetCharacter->m_Mutex.Release();

			// 2006.11.22. steeple
			// 소환수도 옮겨준다.
			if(m_pagpmSummons)
				m_pagpmSummons->UpdateAllSummonsPosToOwner((AgpdCharacter*)pstChatData->pcsSenderBase);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[GOADMIN] %s(%s) : %s\n",
				((AgpdCharacter *) pstChatData->pcsSenderBase)->m_szID,
				pcsAgsdCharacter->m_szAccountID,
				szArg);
			AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
		}
	}
	else if (strncmp(pstChatData->szMessage + i, "/goadmincid", k - i) == 0)
	{
		if (!pcsAgpdCharacter)
			return FALSE;

		if (m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		{
			AuPOS	stPos;
			ZeroMemory(&stPos, sizeof(AuPOS));

			if (!bExistArg)
				return TRUE;

			INT32 lCID = atoi(szArg);

			AgpdCharacter* pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
			if (!pcsTargetCharacter)
				return FALSE;

			m_pagpmCharacter->StopCharacter((AgpdCharacter *) pstChatData->pcsSenderBase, NULL);
			m_pagpmCharacter->UpdatePosition(((AgpdCharacter *) pstChatData->pcsSenderBase), &pcsTargetCharacter->m_stPos, FALSE, TRUE);

			pcsTargetCharacter->m_Mutex.Release();

			// 2006.11.22. steeple
			// 소환수도 옮겨준다.
			if(m_pagpmSummons)
				m_pagpmSummons->UpdateAllSummonsPosToOwner((AgpdCharacter*)pstChatData->pcsSenderBase);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[GOADMINCID] %s(%s) : %s\n",
				((AgpdCharacter *) pstChatData->pcsSenderBase)->m_szID,
				pcsAgsdCharacter->m_szAccountID,
				pcsTargetCharacter->m_pcsCharacterTemplate->m_szTName);
			AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
		}
	}
	else if (strncmp(pstChatData->szMessage + i, "/come", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
			return FALSE;

		if (!bExistArg)
			return TRUE;

		AgpdCharacter* pcsAgpdCharacter = m_pagpmCharacter->GetCharacterLock(szArg);
		if (!pcsAgpdCharacter) return FALSE;

//			m_pagpmCharacter->UpdatePosition((AgpdCharacter *) pstChatData->pcsSenderBase, &stPos, FALSE, TRUE);
		m_pagpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
		if (pcsAgsdCharacter)
			pcsAgsdCharacter->m_stComePosition	= pcsAgpdCharacter->m_stPos;

		m_pagpmCharacter->UpdatePosition(pcsAgpdCharacter, &((AgpdCharacter *) pstChatData->pcsSenderBase)->m_stPos, FALSE, TRUE);

		// 2006.11.22. steeple
		// 소환수도 옮겨준다.
		if(m_pagpmSummons)
			m_pagpmSummons->UpdateAllSummonsPosToOwner(pcsAgpdCharacter);

		pcsAgpdCharacter->m_Mutex.Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[COME] %s(%s) : %s\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_NEAR_TOWN), k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
			return FALSE;

		if (!bExistArg)
			return TRUE;

		AgpdCharacter* pcsAgpdCharacter = m_pagpmCharacter->GetCharacterLock(szArg);
		if (!pcsAgpdCharacter) return FALSE;

		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
		if (pcsAgsdCharacter &&
			!(pcsAgsdCharacter->m_stComePosition.x == 0 &&
			  pcsAgsdCharacter->m_stComePosition.y == 0 &&
			  pcsAgsdCharacter->m_stComePosition.z == 0))
		{
			m_pagpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

			m_pagpmCharacter->UpdatePosition(pcsAgpdCharacter, &pcsAgsdCharacter->m_stComePosition, FALSE, TRUE);

			ZeroMemory(&pcsAgsdCharacter->m_stComePosition, sizeof(AuPOS));
		}

		pcsAgpdCharacter->m_Mutex.Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[Return] %s(%s) : %s\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/levelup", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if (!bExistArg)
			return TRUE;

		INT32	lLevel	= atoi(szArg);
		if (lLevel > 0 && lLevel < AGPMCHAR_MAX_LEVEL)
			m_pagsmCharacter->SetCharacterLevel(pcsAgpdCharacter, lLevel, FALSE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[LEVELUP] %s(%s) : %d\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			lLevel);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITLEVEL, pcsAgpdCharacter, pcsAgpdCharacter);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/show me the money", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
			return FALSE;

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[SHOWMETHEMONEY] %s(%s)\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		WriteLog_Gheld(AGPDLOGTYPE_GHELD_GMEDIT, pcsAgpdCharacter, pcsAgpdCharacter);

		return m_pagpmCharacter->AddMoney(pcsAgpdCharacter, 100000);
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CHAT_CHARPOINT_FULL), k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		// HP를 Full로 채운다.
		PVOID	pvPacketFactor	= NULL;

		if (!m_pagsmFactors->Resurrection(&((AgpdCharacter *) pstChatData->pcsSenderBase)->m_csFactor, &pvPacketFactor))
			return FALSE;

		// factor 패킷을 보낸다.
		BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, (AgpdCharacter *) pstChatData->pcsSenderBase, PACKET_PRIORITY_2);
		m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[Helpme] %s(%s)\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	//else if(strncmp(pstChatData->szMessage + i, "/levelupto", k - i) == 0)
	//{
	//	// ex. levelupto 때이헌터 99
	//	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
	//		return FALSE;

	//	INT32 lStart = j + 1;
	//	INT32 lEnd = lStart + pstChatData->lMessageLength - (j + 1);

	//	if(lEnd > pstChatData->lMessageLength)
	//		return TRUE;

	//	INT32 lIndex = 0, i = 0;
	//	for(i = lStart; i < lEnd; i++)
	//	{
	//		if(pstChatData->szMessage[i] == ' ')
	//		{
	//			lIndex = i;
	//			break;
	//		}
	//	}

	//	if(lIndex > lStart && i < lEnd - 1 && lEnd - lStart <= 100)
	//	{
	//		CHAR szCharID[50], szLevel[50];
	//		memset(szCharID, 0, sizeof(CHAR) * 50);
	//		memset(szLevel, 0, sizeof(CHAR) * 50);

	//		memcpy(szCharID, &pstChatData->szMessage[lStart], lIndex - lStart);
	//		memcpy(szLevel, &pstChatData->szMessage[lIndex+1], lEnd - lIndex - 1);

	//		INT32 lLevel = atoi(szLevel);
	//		if(lLevel > 0 && lLevel < AGPMCHAR_MAX_LEVEL)
	//		{
	//			if(strcmp(pcsAgpdCharacter->m_szID, szCharID) == 0)
	//			{
	//				m_pagsmCharacter->SetCharacterLevel(pcsAgpdCharacter, lLevel, FALSE);
	//			}
	//			else
	//			{
	//				AgpdCharacter* pcsTarget = m_pagpmCharacter->GetCharacterLock(szCharID);
	//				if(pcsTarget)
	//				{
	//					m_pagsmCharacter->SetCharacterLevel(pcsTarget, lLevel, FALSE);
	//					pcsTarget->m_Mutex.Release();

	//					AuLogFile(AGSMADMIN_COMMAND_LOG_FILE, "[LEVELUPTO] %s(%s) : %s %d\n",
	//						pcsAgpdCharacter->m_szID,
	//						pcsAgsdCharacter->m_szAccountID,
	//						szCharID,
	//						lLevel);
	//				}
	//			}
	//		}
	//	}
	//}
	//else if(strncmp(pstChatData->szMessage + i, "/moneyto", k - i) == 0)
	//{
	//	// ex. moneyto 때이헌터 100000000
	//	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
	//		return FALSE;

	//	INT32 lStart = j + 1;
	//	INT32 lEnd = lStart + pstChatData->lMessageLength - (j + 1);

	//	INT32 lIndex = 0, i = 0;
	//	for(i = lStart; i < lEnd; i++)
	//	{
	//		if(pstChatData->szMessage[i] == ' ')
	//		{
	//			lIndex = i;
	//			break;
	//		}
	//	}

	//	if(lIndex > lStart && i < lEnd - 1 && lEnd - lStart <= 100)
	//	{
	//		CHAR szCharID[50], szMoney[50];
	//		memset(szCharID, 0, sizeof(CHAR) * 50);
	//		memset(szMoney, 0, sizeof(CHAR) * 50);

	//		memcpy(szCharID, &pstChatData->szMessage[lStart], lIndex - lStart);
	//		memcpy(szMoney, &pstChatData->szMessage[lIndex+1], lEnd - lIndex - 1);

	//		INT32 lMoney = atoi(szMoney);
	//		if(lMoney > 0)
	//		{
	//			if(strcmp(pcsAgpdCharacter->m_szID, szCharID) == 0)
	//			{
	//				m_pagpmCharacter->AddMoney(pcsAgpdCharacter, lMoney);
	//			}
	//			else
	//			{
	//				AgpdCharacter* pcsTarget = m_pagpmCharacter->GetCharacterLock(szCharID);
	//				if(pcsTarget)
	//				{
	//					m_pagpmCharacter->AddMoney(pcsTarget, lMoney);
	//					pcsTarget->m_Mutex.Release();

	//					AuLogFile(AGSMADMIN_COMMAND_LOG_FILE, "[MONEYTO] %s(%s) : %s %d\n",
	//						pcsAgpdCharacter->m_szID,
	//						pcsAgsdCharacter->m_szAccountID,
	//						szCharID,
	//						lMoney);
	//				}
	//			}
	//		}
	//	}
	//}
	else if(strncmp(pstChatData->szMessage + i, "/createto", k - i) == 0)
	{
		// ex. createto 때이헌터 TID1 TID2 TID3 ....
		//
		// 2007.06.17. steeple 일본 클베용 하드코딩
		//
		// 온라인인 캐릭터에게는 그냥 되지만,
		// 오프라인인 캐릭터에게는 한번만 해야 한다.
		// 여러번 하면 아이템이 겹쳐서 들어가게 된다.
		//
		//

		if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if(!bExistArg)
			return TRUE;

		CGetArg2 arg(szArg);

		if(!arg.GetParam(0))
			return FALSE;

		int nCount = arg.GetArgCount();
		--nCount;

		// 한번에 넣을 수 있는 개수 체크한다.
		if(nCount < 1 || nCount > AGPMITEM_INVENTORY_ROW * AGPMITEM_INVENTORY_COLUMN * AGPMITEM_MAX_INVENTORY - AGPMITEM_INVENTORY_COLUMN)
			return FALSE;

		stAgpdAdminItemOperation stItemOperation;
		memset(&stItemOperation, 0, sizeof(stItemOperation));

		BOOL bIsOnline = m_pagpmCharacter->GetCharacter(stItemOperation.m_szCharName) ? TRUE : FALSE;
		CHAR* szItemTID = NULL;

		for(int i = 0; i < nCount; ++i)
		{
			szItemTID = arg.GetParam(i + 1);
			if(!szItemTID)
				continue;

			INT32 lItemTID = atoi(szItemTID);
			if(!lItemTID)
				continue;

			memset(&stItemOperation, 0, sizeof(stItemOperation));

			_tcsncpy(stItemOperation.m_szCharName, arg.GetParam(0), AGPACHARACTER_MAX_ID_STRING);

			stItemOperation.m_cOperation = AGPMADMIN_ITEM_CREATE;
			stItemOperation.m_lTID = lItemTID;
			stItemOperation.m_cReason = 1;
			stItemOperation.m_lCount = 1;		// 임시로 1개

			if(bIsOnline)
				ProcessItemOperation(&stItemOperation, pcsAgpdCharacter);
			else
			{
				// 오프라인일 경우에는 ItemPosition 을 지정해줘야 한다.
				// 첫 생성된 캐릭터에게만 이 작업을 수행해야 한다.
				// 시작은 0, 1, 0 으로 한다.
				INT32 lIndex = i + 4;
				INT32 lLayer = lIndex / (AGPMITEM_INVENTORY_ROW * AGPMITEM_INVENTORY_COLUMN);
				INT32 lRow = ((INT32)(lIndex / AGPMITEM_INVENTORY_ROW)) % AGPMITEM_MAX_INVENTORY;
				INT32 lCol = lIndex % AGPMITEM_INVENTORY_COLUMN;

				stItemOperation.m_lLayer = lLayer;
				stItemOperation.m_lRow = lRow;
				stItemOperation.m_lColumn = lCol;

				ProcessItemOperation(&stItemOperation, pcsAgpdCharacter);
			}

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[CREATETO] %s(%s) : %s %d\n",
				pcsAgpdCharacter->m_szID,
				pcsAgsdCharacter->m_szAccountID,
				stItemOperation.m_szCharName,
				stItemOperation.m_lTID);
			AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
		}

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/spawn", k - i) == 0)
	{
		// ex. spawn 개수 몬스터 이름(또는 TID)
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandSpawn(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j)); 
	}
	else if(strncmp(pstChatData->szMessage + i, "/kick", k - i) == 0)
	{
		// 해당 캐릭터를 짜른다.
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
			return FALSE;

		if (!bExistArg)
			return TRUE;

		// 어드민이다. 장난치지 마셈!! 2005.06.15. steeple
		if(strcmp(szArg, pcsAgpdCharacter->m_szID) == 0)
			return FALSE;

		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szArg);
		if(!pcsCharacter)
			return FALSE;

		UINT32 ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);
		INT32 lCID = pcsCharacter->m_lID;

		//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
		//	AuLogFile("RemoveNPC.log", "Removed by /KICK Command\n");

		pcsCharacter->m_Mutex.Release();

		if(ulNID != 0)
			DestroyClient(ulNID);
		else if(lCID != 0)
			m_pagpmCharacter->RemoveCharacter(lCID);
		else
			m_pagpmCharacter->RemoveCharacter(szArg);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[KICK] %s(%s) : %s\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/endbuff", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		m_pagsmSkill->EndAllBuffedSkillExceptDebuff((ApBase*)pcsAgpdCharacter, TRUE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ENDBUFF] %s(%s)\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/invisible", k - i) == 0)
	{
		// 해당 캐릭터를 짜른다.
		if(!pcsAgpdCharacter || m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
			return FALSE;

		INT32 lDuration = 0;
		if(bExistArg)
		{
			lDuration = atoi(szArg);
		}

		if(lDuration > 0)
		{
			lDuration = 24 * 60 * 60 * 1000;	// 하루
			m_pagsmCharacter->SetSpecialStatusTime(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT, (UINT32)lDuration);
		}
		else
			m_pagpmCharacter->UpdateUnsetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[Invisible] %s(%s) : %s\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	// 2005.10.30. steeple
	else if(strncmp(pstChatData->szMessage + i, _T("/counsel"), k - i) == 0)
	{
		if(!pcsAgpdCharacter || m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
			return FALSE;

		if(!(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_COUNSEL))
		{
			// 시간 체크는 하지 않기 때문에 그냥 Set 해준다.
			m_pagpmCharacter->UpdateSetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_COUNSEL);
		}
		else
		{
			m_pagpmCharacter->UpdateUnsetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_COUNSEL);
		}

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[Counsel] %s(%s) : %s\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/printlog", k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		g_bPrintNumCharacter	= 1;
		g_bPrintActiveCellCount	= 1;
		g_bPrintRemovePoolTID	= 1;

		ApMemoryManager::GetInstance().ReportMemoryInfo();

		m_pagpmFactors->PrintDataTypeLog();
	}
	else if(strncmp(pstChatData->szMessage + i, "/printheap", k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		m_pagsmCharacter->m_csHeap.Report();
	}
	else if(strncmp(pstChatData->szMessage + i, "/refreshmalllist", k - i) == 0)
	{
		if(!m_pagpmCashMall)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmCashMall->RefreshMallList();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[RefreshMallList] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/expdropon", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetEventResurrect(TRUE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ExpDropOn] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/expdropoff", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetEventResurrect(FALSE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ExpDropOff] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/buffeventstart", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		m_pagpmConfig->SetEventChatting(TRUE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[BuffEventStart] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/buffeventstop", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		m_pagpmConfig->SetEventChatting(FALSE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[BuffEventStop] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/itemdropon", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetEventItemDrop(FALSE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ItemDropOn] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/itemdropoff", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetEventItemDrop(TRUE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ItemDropOff] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/ExpPenaltyOn", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetExpPenaltyOnDeath(TRUE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ExpPenaltyOn] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/ExpPenaltyOff", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetExpPenaltyOnDeath(FALSE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ExpPenaltyOff] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/removespawnedmob", k - i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		INT32			lIndex	= 0;
		AgpdCharacter	*pcsCharacter = m_pagpmCharacter->GetCharSequence(&lIndex);

		while (pcsCharacter)
		{
			if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_ADMIN_SPAWNED_CHAR)
			{
				//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
				//	AuLogFile("RemoveNPC.log", "Removed by /removespawnedmob command\n");

				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
			}

			pcsCharacter = m_pagpmCharacter->GetCharSequence(&lIndex);
		}

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[RemoveSpanedMob] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/startsiegewar", k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		EnumCallback(AGSMADMIN_CB_START_SIEGEWAR, pcsAgpdCharacter, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[StartSiegeWar] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/endsiegewar", k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		EnumCallback(AGSMADMIN_CB_END_SIEGEWAR, pcsAgpdCharacter, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[EndSiegeWar] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/endarchlordbattle", k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		EnumCallback(AGSMADMIN_CB_END_ARCHLORD_BATTLE, pcsAgpdCharacter, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[EndArchlordBattle] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, "/autopickup", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagsmCharacter->SetAutoPickup(pcsAgpdCharacter, (m_pagsmCharacter->IsAutoPickup(pcsAgpdCharacter)) ? FALSE : TRUE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[AutoPickUp] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_SET_GUARD), k - i) == 0)
	{
		if (strlen(szArg) > 32)
			return TRUE;

		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		AgpdCharacter *pcsTargetChar = m_pagpmCharacter->GetCharacter(szArg);

		if (NULL == pcsTargetChar)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_SET_GUARD, pcsAgpdCharacter, pcsTargetChar);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[SetGuard] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_CANCEL_GUARD), k - i) == 0)
	{
		if (strlen(szArg) > 32)
			return TRUE;

		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		AgpdCharacter *pcsTargetChar = m_pagpmCharacter->GetCharacter(szArg);

		if (NULL == pcsTargetChar)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_CANCEL_GUARD, pcsAgpdCharacter, pcsTargetChar);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[CancelGuard] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_GUARD_INFO), k - i) == 0)
	{
		if(bCheckAdmin && !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_GUARD_INFO, pcsAgpdCharacter, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[GuardInfo] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_SET_ARCHLORD), k - i) == 0
		||strncmp(pstChatData->szMessage + i, "/setarchlord", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if (strlen(szArg) > 32)
			return TRUE;

		AgpdCharacter *pcsTargetChar = m_pagpmCharacter->GetCharacter(szArg);

		if (NULL == pcsTargetChar)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_SET_ARCHLORD, pcsTargetChar, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[SetArchlord] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_CANCEL_ARCHLORD), k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if (strlen(szArg) > 32)
			return TRUE;

		AgpdCharacter *pcsTargetChar = m_pagpmCharacter->GetCharacter(szArg);

		if (NULL == pcsTargetChar)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_CANCEL_ARCHLORD, pcsTargetChar, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[CancelArchlord] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_START_ARCHLORD), k - i) == 0
		|| strncmp(pstChatData->szMessage + i, "/startarchlordbattle", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_START, NULL, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[StartArchlordBattle] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_END_ARCHLORD), k - i) == 0
		||strncmp(pstChatData->szMessage + i, "/endarchlordbattle", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_END, NULL, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[EndArchlordbattle] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_NEXT_STEP_ARCHLORD), k - i) == 0
		||strncmp(pstChatData->szMessage + i, "/nextsteparchlordbattle", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_ARCHLORD_NEXT_STEP, NULL, NULL);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[NextStepArchlordBattle] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	// 성주설정
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_SET_CASTLE_OWNER), k - i) == 0
		||strncmp(pstChatData->szMessage + i, "/setcastleowner", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_SET_CASTLE_OWNER, pcsAgpdCharacter, szArg);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[SetCastleOwner] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	// 성주취소
	else if (strncmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_CANCEL_CASTLE_OWNER), k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		EnumCallback(AGSMADMIN_CB_CANCEL_CASTLE_OWNER, pcsAgpdCharacter, szArg);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[CancelCastleOwner] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strnicmp(pstChatData->szMessage + i, "/ExpRatio", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetExpAdjustmentRatio( (float) atof(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ExpRatio] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strnicmp(pstChatData->szMessage + i, "/DropRatio", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetDropAdjustmentRatio( (float) atof(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[DropRatio] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strnicmp(pstChatData->szMessage + i, "/Drop2Ratio", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetDrop2AdjustmentRatio( (float) atof(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[Drop2Ratio] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strnicmp(pstChatData->szMessage + i, "/GheldDropRatio", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetGheldDropAdjustmentRatio( (float) atof(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[GheldDropRatio] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if(strnicmp(pstChatData->szMessage + i, "/CharismaDropRatio", k - i) == 0)
	{
		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetCharismaDropAdjustmentRatio( (float) atof(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[CharismaDropRatio] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strnicmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_NICKNAME), k - i) == 0)
	{	
		// 2007.03.03. laki
		if (!pcsAgpdCharacter)
			return FALSE;

		if (!bExistArg || !strlen(szArg))
			strcpy_s(szArg, AGPACHARACTER_MAX_CHARACTER_NICKNAME, " ");

		/*if (m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		{	
			if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
				return FALSE;	

			if (!m_pagpmCharacter->CheckFilterText(szArg))
				return FALSE;
				
			ZeroMemory(pcsAgpdCharacter->m_szNickName, sizeof(pcsAgpdCharacter->m_szNickName));
			strncpy(pcsAgpdCharacter->m_szNickName, szArg, AGPACHARACTER_MAX_CHARACTER_NICKNAME);
			m_pagsmCharacter->SendPacketNickName(pcsAgpdCharacter);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[NickName] %s(%s) : %s\n",
				pcsAgpdCharacter->m_szID,
				pcsAgsdCharacter->m_szAccountID,
				szArg);
			AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
		}*/ // 해깔리니까 운영자도 예외없이 아이템 소모하도록 수정.
		
		AgpdCharacter* pcsCharacter = (AgpdCharacter*)pstChatData->pcsSenderBase;
		if(!pcsCharacter)
			return FALSE;

		// 정상이 아닐 땐 사용불가.
		if(!pcsCharacter->m_bIsAddMap)
			return FALSE;
		if(pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_NORMAL &&
			pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_NOT_ACTION &&
			pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_MOVE)
		{
			if(m_pagsmSystemMessage)
				m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
			return FALSE;
		}

		AgpdItemADChar* pAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
		if(!pAgpdItemADChar)
			return FALSE;

		// 아이템을 찾는다.
		AgpdGridItem* pAgpdGridItem = m_pagpmItem->GetCashItemUsableByType(pcsCharacter, AGPMITEM_USABLE_TYPE_NICK, 0);
		if (NULL == pAgpdGridItem)
			return FALSE;
		
		AgpdItem* pAgpdItem = m_pagpmItem->GetItem(pAgpdGridItem->m_lItemID);
		if (NULL == pAgpdItem)
			return FALSE;

		if (!m_pagpmCharacter->CheckFilterText(szArg))
			return FALSE;

		ZeroMemory(pcsCharacter->m_szNickName, sizeof(pcsCharacter->m_szNickName));
		strncpy(pcsCharacter->m_szNickName, szArg, AGPACHARACTER_MAX_CHARACTER_NICKNAME);
		m_pagsmCharacter->SendPacketNickName(pcsCharacter);

		// send message. AGPMSYSTEMMESSAGE_CODE_JUMP_ITEM_USED와 포맷이 같으므로 그냥 사용한다.
		if(m_pagsmSystemMessage)
			m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_JUMP_ITEM_USED, -1, -1,
									((AgpdItemTemplate*)pAgpdItem->m_pcsItemTemplate)->m_szName);

		// log
		m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsCharacter->m_lID, pAgpdItem, 1);

		// sub item
		++pAgpdItem->m_lCashItemUseCount;
		m_pagpmItem->SubItemStackCount(pAgpdItem, 1);
	}
	//else if (strnicmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_SUNDRY), k - i) == 0)
	//{
	//	return FALSE;
	//
	//	if (!pcsAgpdCharacter)
	//		return FALSE;
	//		
	//	if (!m_pagsmEventNPCTrade)
	//		return FALSE;
	//		
	//	m_pagsmEventNPCTrade->ProcessOpenAnywhere_Sundry(pcsAgpdCharacter);

	//	AuLogFile(AGSMADMIN_COMMAND_LOG_FILE, "[CancelArchlord] %s(%s) : %s\n",
	//		bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
	//		bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
	//		szArg);
	//}
	//else if (strnicmp(pstChatData->szMessage + i, ServerStr().GetStr(STI_COMMAND_BANK), k - i) == 0)
	//{
	//	if (!pcsAgpdCharacter)
	//		return FALSE;
	//		
	//	if (!m_pagsmEventBank)
	//		return FALSE;

	//	// check cash item
	//	AgpdGridItem *pcsAgpdGridItem = m_pagpmItem->GetUsingCashItemUsableByType(pcsAgpdCharacter, AGPMITEM_USABLE_TYPE_ANY_BANK, 0);
	//	if (NULL == pcsAgpdGridItem)
	//		return FALSE;
	//	
	//	AgpdItem *pcsAgpdItem = m_pagpmItem->GetItem(pcsAgpdGridItem->m_lItemID);
	//	if (NULL == pcsAgpdItem)
	//		return FALSE;
	//		
	//	m_pagsmEventBank->OpenAnywhere(pcsAgpdCharacter);

	//	AuLogFile(AGSMADMIN_COMMAND_LOG_FILE, "[BankAnywhereOpen] %s(%s) : %s\n",
	//		bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
	//		bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
	//		szArg);
	//}
	else if (strnicmp(pstChatData->szMessage + i, "/gather", k - i) == 0)
	{
		if (!bExistArg || !pcsAgpdCharacter || !m_pagpmProduct)
			return FALSE;
			
		if (!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) ||
			m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
			return FALSE;	
		
		m_pagpmProduct->SetSkillLevel(pcsAgpdCharacter, 663, atoi(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[Gather] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if (strnicmp(pstChatData->szMessage + i, "/reconnectlogin", k - i) == 0)
	{
		if (!pcsAgpdCharacter)
			return FALSE;
			
		if (!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) ||
			m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
			return FALSE;	
		
		if (IsGameServer() && m_pAgsmInterServerLink)
		{
			m_pAgsmInterServerLink->SetDisconnectLoginServer(TRUE);
		}

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ReconnectLogin] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if(strncmp(pstChatData->szMessage + i, "/setlevellimit", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		if (!bExistArg)
			return TRUE;

		INT32	lLevel	= atoi(szArg);
		if (lLevel > 0 && lLevel < AGPMCHAR_MAX_LEVEL)
			m_pagsmCharacter->SetCharacterLevelLimit(pcsAgpdCharacter, lLevel);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[SetLevelLimit] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/releaselevellimit", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		m_pagsmCharacter->ReleaseCharacterLevelLimit(pcsAgpdCharacter);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ReleaseLevelLimit] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage + i, "/loadeventitem", k - 1) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		LoadEventItem();
		m_pagpmConfig->SetIsAdminEventItem(TRUE);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[LoadEventItem] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if(strncmp(pstChatData->szMessage + i, "/seteventitem", k - 1) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		m_pagpmConfig->SetIsAdminEventItem((BOOL)atoi(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[SetEventItem] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	else if(strncmp(pstChatData->szMessage + i, "/processeventitem", k - 1) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		if(m_pagpmConfig->GetIsAdminEventItem() == FALSE)
			return FALSE;

		ProcessEventItem(atoi(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ProcessEventItem] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}
	
	// Enable Auction
	else if(strncmp(pstChatData->szMessage + i, "/Auction", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) || !m_pagpmConfig)
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		if(atoi(szArg) != 0 && atoi(szArg) != 1)
			return FALSE;
		
		m_pagpmConfig->SetEnableAuction((BOOL)atoi(szArg));	

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ProcessAuction] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}

	// Disable EventItemEffect
	else if (strncmp(pstChatData->szMessage+i, "/EventEffect", k-i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) || !m_pagpmConfig)
			return FALSE;

		if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		if(atoi(szArg) != 0 && atoi(szArg) != 1)
			return FALSE;

		m_pagpmConfig->SetEnableEventItemEffect((BOOL)atoi(szArg));

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[ProcessEventItemEffect] %s(%s) : %s\n",
			bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
			bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
			szArg);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

		return TRUE;
	}
	else if(strncmp(pstChatData->szMessage+i, "/CashItemTimeExtend", k-i) == 0)
	{
		if(!m_pagpmConfig)
			return FALSE;

		if(bCheckAdmin && m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_5)
			return FALSE;

		return ProcessCommandCashItemTimeExtend(pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j), bCheckAdmin);
	}
	else if(strncmp(pstChatData->szMessage + i, "/durability", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		INT32	lItemType = 0;
		INT32	lModifyDurability = 0;
		INT32	lCurrentDurability = 0;
		INT32	lMaxDurability = 0;

		lItemType			= atoi(argCommands.GetParam(1));
		lModifyDurability	= atoi(argCommands.GetParam(2));

		AgpdItem* pcsItem = m_pagpmItem->GetEquipSlotItem(pcsAgpdCharacter, (AgpmItemPart)lItemType);

		if ( !pcsItem || !pcsItem->m_pcsItemTemplate )
			return FALSE;

		lCurrentDurability	= m_pagpmItem->GetItemDurabilityCurrent(pcsItem);
		lMaxDurability		= m_pagpmItem->GetItemDurabilityMax(pcsItem);

		// 수정될 내구도가 0보다 작으면 안된다
		if ( lCurrentDurability + lModifyDurability < 0 )
			lModifyDurability = -lCurrentDurability;

		// 수정될 내구도가 최대 내구도보다 크면 안된다
		if ( lCurrentDurability + lModifyDurability > lMaxDurability )
			lModifyDurability = lMaxDurability - lCurrentDurability;

		// 업데이트 적용~
		return m_pagsmItem->UpdateItemDurability(pcsItem, lModifyDurability);
	}
	else if(strncmp(pstChatData->szMessage + i, "/clearinventory", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		UINT32 ulInvenLayerNum = 0;

		int nArgCnt = argCommands.GetArgCount();

		if ( 1 != nArgCnt )
		{
			// 매개변수가 있으면 해당 인벤토리만 삭제
			ulInvenLayerNum = (UINT32)atoi(argCommands.GetParam(1));
		}

		return ProcessCommandClearInventory(pcsAgpdCharacter, ulInvenLayerNum);
	}
	else if(strncmp(pstChatData->szMessage + i, "/clearcashinventory", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandClearCashInventory(pcsAgpdCharacter);
	}
	else if(strncmp(pstChatData->szMessage + i, "/openbox", k - i) == 0)
	{
		if(!pcsAgpdCharacter || !m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
			return FALSE;

		return ProcessCommandOpenLotteryBox( pcsAgpdCharacter, pstChatData->szMessage + j, pstChatData->lMessageLength - (j) );
	}

	return FALSE;
}

// 공지사항 날린다.
BOOL AgsmAdmin::ProcessCommandNotice(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!m_pagsmChatting)
		return FALSE;

	if(!szMessage || lMessageLength <= 0)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= NULL;
	
	if (pcsAgpdCharacter)
		pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	CHAR	*pszBuffer	= new CHAR [lMessageLength + 1];
	ZeroMemory(pszBuffer, sizeof(CHAR) * (lMessageLength + 1));

	CopyMemory(pszBuffer, szMessage, sizeof(CHAR) * lMessageLength);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[NOTICE] %s(%s) : %s\n",
		pcsAgpdCharacter ? pcsAgpdCharacter->m_szID : "<Console>",
		pcsAgsdCharacter ? pcsAgsdCharacter->m_szAccountID : "",
		pszBuffer);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	delete [] pszBuffer;

	// 모두에게 보낸다.
	m_pagsmChatting->SendMessageAll(AGPDCHATTING_TYPE_WHOLE_WORLD, pcsAgpdCharacter ? pcsAgpdCharacter->m_lID : -1, szMessage, lMessageLength);

	return TRUE;
}

// 공지사항 날린다.
BOOL AgsmAdmin::ProcessCommandNoticeRepeat(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!m_pagsmChatting)
		return FALSE;

	if(!szMessage || lMessageLength <= 0)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= NULL;
	
	if (pcsAgpdCharacter)
		pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	CHAR	*pszBuffer	= new CHAR [lMessageLength + 1];
	ZeroMemory(pszBuffer, sizeof(CHAR) * (lMessageLength + 1));

	CopyMemory(pszBuffer, szMessage, sizeof(CHAR) * lMessageLength);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[NOTICE Repeat] %s(%s) : %s\n",
		pcsAgpdCharacter ? pcsAgpdCharacter->m_szID : "<Console>",
		pcsAgsdCharacter ? pcsAgsdCharacter->m_szAccountID : "",
		pszBuffer);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	delete [] pszBuffer;

	AssmNoticeRepeat	csNotice;
	//CHAR				szTemp[16];
	CHAR *				szMessageStart = szMessage;
	INT32				lLength;

	while (TRUE)
	{
		if (*szMessageStart == 0) return FALSE;
		if (*szMessageStart != ' ') break;
		++szMessageStart;
	}
	csNotice.m_lID = atoi(szMessageStart);

	szMessageStart = strchr(szMessageStart, ' ');
	if (!szMessageStart) return FALSE;
	while (TRUE)
	{
		if (*szMessageStart == 0) return FALSE;
		if (*szMessageStart != ' ') break;
		++szMessageStart;
	}
	csNotice.m_ulDuration = atoi(szMessageStart);

	szMessageStart = strchr(szMessageStart, ' ');
	if (!szMessageStart) return FALSE;
	while (TRUE)
	{
		if (*szMessageStart == 0) return FALSE;
		if (*szMessageStart != ' ') break;
		++szMessageStart;
	}
	csNotice.m_ulGap = atoi(szMessageStart);

	szMessageStart = strchr(szMessageStart, ' ');
	if (!szMessageStart) return FALSE;
	while (TRUE)
	{
		if (*szMessageStart == 0) return FALSE;
		if (*szMessageStart != ' ') break;
		++szMessageStart;
	}
	lLength = ( INT32 ) ( lMessageLength - sizeof(CHAR) * (szMessageStart - szMessage) );
	csNotice.m_szMessage = new CHAR [lLength + 1];
	strncpy(csNotice.m_szMessage, szMessageStart, lLength);
	csNotice.m_szMessage[lLength] = 0;

	csNotice.m_ulStartTime = GetClockCount();
	csNotice.m_ulLastNoticeTime = csNotice.m_ulStartTime;

	AuAutoLock lock(m_csNoticeCS);
	if (lock.Result())
		m_vtNotice.push_back(csNotice);

	// 모두에게 보낸다.
	m_pagsmChatting->SendMessageAll(AGPDCHATTING_TYPE_WHOLE_WORLD, -1, csNotice.m_szMessage, lLength);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandNoticeList(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!m_pagsmChatting)
		return FALSE;

	AssmNoticeIter	iter;
	CHAR			szTemp[256];

	sprintf(szTemp, "[Notice] Notice List -----------");
	m_pagsmChatting->SendMessageSystem((ApBase *) pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, szTemp, (INT32) strlen(szTemp));

	AuAutoLock lock(m_csNoticeCS);
	if (!lock.Result()) return FALSE;

	for (iter = m_vtNotice.begin(); iter != m_vtNotice.end(); ++iter)
	{
		sprintf(szTemp, "%d %s", (*iter).m_lID, (*iter).m_szMessage);
		m_pagsmChatting->SendMessageSystem((ApBase *) pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, szTemp, (INT32) strlen(szTemp));
	}

	sprintf(szTemp, "--------------------------------");
	m_pagsmChatting->SendMessageSystem((ApBase *) pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, szTemp, (INT32) strlen(szTemp));

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandNoticeDelete(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!m_pagsmChatting)
		return FALSE;

	if(!szMessage || lMessageLength <= 0)
		return FALSE;

	AssmNoticeIter	iter;
	INT32			lMessageID = atoi(szMessage);
	CHAR			szTemp[256];

	AuAutoLock lock(m_csNoticeCS);
	if (!lock.Result()) return FALSE;

	iter = m_vtNotice.begin();
	while (iter != m_vtNotice.end())
	{
		if (!lMessageID || (*iter).m_lID == lMessageID)
		{
			sprintf(szTemp, "[Notice] Notice %d Deleted", lMessageID);
			m_pagsmChatting->SendMessageSystem((ApBase *) pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, szTemp, (INT32) strlen(szTemp));

			if ((*iter).m_szMessage)
			{
				delete [] (*iter).m_szMessage;
				(*iter).m_szMessage = NULL;
			}

			m_vtNotice.erase(iter);
			iter = m_vtNotice.begin();
			continue;
		}

		++iter;
	}

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandNoticeClear(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!m_pagsmChatting)
		return FALSE;

	CHAR			szTemp[256];

	sprintf(szTemp, "[Notice] Cleared");
	m_pagsmChatting->SendMessageSystem((ApBase *) pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, szTemp, (INT32) strlen(szTemp));

	ClearNoticeRepeat();

	return TRUE;
}

// 아이템을 만든다.
// 초대박 임시 땜빵 급조 코드. 2004.02.02
BOOL AgsmAdmin::ProcessCommandCreate(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength, INT32 lCount)
{
	if(!m_pagpmItem || !m_pagsmItemManager || lCount < 1 || lCount > 99)
		return FALSE;

	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 32)
		return FALSE;

	//if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
	//	return FALSE;

	CHAR szBuf[32];
	memset(szBuf, 0, 32);

	memcpy(szBuf, szMessage, lMessageLength);
	szBuf[lMessageLength] = '\0';

	// Item Template ID
	INT32 lTID = atoi(szBuf);

	AgpdItemTemplate* pItemTemplate = m_pagpmItem->GetItemTemplate(lTID);
	if (!pItemTemplate)
	{
		pItemTemplate	= m_pagpmItem->GetItemTemplate(szBuf);
		if (!pItemTemplate)
			return FALSE;
	}

	AgpdItem* pcsAgpdItem = NULL;
	if (pItemTemplate->m_bStackable)
		pcsAgpdItem = m_pagsmItemManager->CreateItem(pItemTemplate->m_lID, pcsAgpdCharacter, lCount);
	else
		pcsAgpdItem = m_pagsmItemManager->CreateItem(pItemTemplate->m_lID, pcsAgpdCharacter);

	if(!pcsAgpdItem)
		return FALSE;

	// 귀속아이템이라면 귀속과 주인을 세팅해준다. 2005.11.02. steeple
	if(m_pagpmItem->GetBoundType(pcsAgpdItem->m_pcsItemTemplate) == E_AGPMITEM_BIND_ON_ACQUIRE)
	{
		m_pagpmItem->SetBoundType(pcsAgpdItem, E_AGPMITEM_BIND_ON_ACQUIRE);
		m_pagpmItem->SetBoundOnOwner(pcsAgpdItem, pcsAgpdCharacter);
	}

	// Skill Plus 해준다. 2007.02.05. steeple
	m_pagsmItem->ProcessItemSkillPlus(pcsAgpdItem, pcsAgpdCharacter);

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[CREATE] %s(%s) : %s\n",
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		pItemTemplate->m_szName);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	WriteLog_Item(AGPDLOGTYPE_ITEM_GM_MAKE, pcsAgpdCharacter, pcsAgpdCharacter, pcsAgpdItem, lCount);

	m_pagpmItem->AddItemToInventory(pcsAgpdCharacter, pcsAgpdItem);
	
	return TRUE;
}


// 2004.05.21.
// Server 접속 권한을 설정한다.
// Login Server 로 패킷이 보내지게 된다.
BOOL AgsmAdmin::ProcessCommandOpen(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!m_pAgsmServerManager || !m_pagsmChatting)
		return FALSE;

	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 32)
		return FALSE;

	CHAR szBuf[32];
	memset(szBuf, 0, 32);

	memcpy(szBuf, szMessage, lMessageLength);
	szBuf[lMessageLength] = '\0';

	if(strcmp(szBuf, "모두") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_NONE);
	else if(strcmp(szBuf, "모두다") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_NONE);
	else if(strcmp(szBuf, "전부") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_NONE);
	else if(strcmp(szBuf, "테스터") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_CLOSE_BETA);
	else if(strcmp(szBuf, "클로즈베타") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_CLOSE_BETA);
	else if(strcmp(szBuf, "베타") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_CLOSE_BETA);
	else if(strcmp(szBuf, "개발") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_DEVELOPER);
	else if(strcmp(szBuf, "개발자") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_DEVELOPER);
	else if(strcmp(szBuf, "운영") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_ADMIN);
	else if(strcmp(szBuf, "운영자") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_ADMIN);
	else if(strcmp(szBuf, "GM") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_ADMIN);
	else if(strcmp(szBuf, "ADMIN") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_ADMIN);
	else if(strcmp(szBuf, "admin") == 0)
		SetConnectableAccountLevel(ACCOUNT_LEVEL_ADMIN);

	// 나중에는 LoginServer 가 여러개가 된다.
	// 그때에는 루프를 돌면서 다 뿌려야 한다.
	INT16	nIndex	= 0;
	for(AgsdServer* pcsLoginServer = m_pAgsmServerManager->GetLoginServers(&nIndex); pcsLoginServer;
			pcsLoginServer = m_pAgsmServerManager->GetLoginServers(&nIndex))
	{
		if(!pcsLoginServer)
			break;

		SendConnectableAccountLevel(GetConnectableAccountLevel(), pcsAgpdCharacter->m_lID, pcsLoginServer->m_dpnidServer);
	}
	//SendConnectableAccountLevel(GetConnectableAccountLevel(), pcsAgpdCharacter->m_lID, GetLoginServerNID());

	// 운영자에게 시스템 메시지 뿌린다.
	switch(GetConnectableAccountLevel())
	{
		case ACCOUNT_LEVEL_NONE:
			m_pagsmChatting->SendMessageSystem((ApBase*)pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, AGSMADMIN_MSG_CONNECTABLE_NONE,
				(INT32)_tcslen(AGSMADMIN_MSG_CONNECTABLE_NONE));
			break;

		case ACCOUNT_LEVEL_CLOSE_BETA:
			m_pagsmChatting->SendMessageSystem((ApBase*)pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, AGSMADMIN_MSG_CONNECTABLE_CLOSE_BETA,
				(INT32)_tcslen(AGSMADMIN_MSG_CONNECTABLE_CLOSE_BETA));
			break;

		case ACCOUNT_LEVEL_DEVELOPER:
			m_pagsmChatting->SendMessageSystem((ApBase*)pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, AGSMADMIN_MSG_CONNECTABLE_DEVELOPER,
				(INT32)_tcslen(AGSMADMIN_MSG_CONNECTABLE_DEVELOPER));
			break;

		case ACCOUNT_LEVEL_ADMIN:
			m_pagsmChatting->SendMessageSystem((ApBase*)pcsAgpdCharacter, AGPDCHATTING_TYPE_SYSTEM_LEVEL1, AGSMADMIN_MSG_CONNECTABLE_ADMIN,
				(INT32)_tcslen(AGSMADMIN_MSG_CONNECTABLE_ADMIN));
			break;
	}

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[OPEN] %s(%s) : %d\n",
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		GetConnectableAccountLevel());
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	return TRUE;
}

// Guild 기능 임시로 커맨드로 했음
// 즉 나중엔 다 빠질 놈임.-0-;;;
BOOL AgsmAdmin::ProcessCommandGuild(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	// 2004.07.18. 
	// UI 작업 완료하면서 빠져버림~
	return TRUE;

	/*
	INT16 nIndex = -1;
	for(int i = 0; i < lMessageLength; i++)
	{
		if(szMessage[i] == ' ')
		{
			nIndex = i;
			break;
		}
	}

	CHAR szCommand[64], szValue[64];
	memset(szCommand, 0, 64);
	memset(szValue, 0, 64);

	if(nIndex == -1)
	{	// 공백이 없을 때
	}
	else
	{	// 공백이 있을 때
		if(lMessageLength - nIndex > 64)
			return TRUE;

		if(nIndex >= 64)
			return TRUE;

		memcpy(szCommand, szMessage, nIndex);
		memcpy(szValue, szMessage+nIndex+1, lMessageLength - nIndex - 1);
	}

	if(strlen(szCommand) == 0)
		return TRUE;

	PVOID pvPacket = NULL;
	INT16 nPacketLength = 0;

	if(strcmp(szCommand, "생성") == 0)
	{
		pvPacket = m_pagpmGuild->MakeGuildCreatePacket(&nPacketLength, &pcsAgpdCharacter->m_lID, szValue, pcsAgpdCharacter->m_szID,
															NULL, NULL, NULL, NULL, NULL);
	}
	else if(strcmp(szCommand, "해체") == 0)
	{
		AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsAgpdCharacter);
		if(pcsAttachedGuild)
			pvPacket = m_pagpmGuild->MakeGuildDestroyPacket(&nPacketLength, &pcsAgpdCharacter->m_lID, pcsAttachedGuild->m_szGuildID,
															pcsAgpdCharacter->m_szID);
	}
	else if(strcmp(szCommand, "초대") == 0)
	{
		AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsAgpdCharacter);
		if(pcsAttachedGuild && strlen(pcsAttachedGuild->m_szGuildID) > 0)
			pvPacket = m_pagpmGuild->MakeGuildJoinRequestPacket(&nPacketLength, &pcsAgpdCharacter->m_lID, pcsAttachedGuild->m_szGuildID,
															pcsAgpdCharacter->m_szID, szValue);
	}
	else if(strcmp(szCommand, "가입") == 0)
	{
		pvPacket = m_pagpmGuild->MakeGuildJoinPacket(&nPacketLength,
															&pcsAgpdCharacter->m_lID,
															szValue,
															pcsAgpdCharacter->m_szID,
															NULL, NULL, NULL, NULL, NULL);
	}
	else if(strcmp(szCommand, "탈퇴") == 0)
	{
		AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsAgpdCharacter);
		if(pcsAttachedGuild && strlen(pcsAttachedGuild->m_szGuildID) > 0)
			pvPacket = m_pagpmGuild->MakeGuildLeavePacket(&nPacketLength,
															&pcsAgpdCharacter->m_lID,
															pcsAttachedGuild->m_szGuildID,
															pcsAgpdCharacter->m_szID);
	}
	else if(strcmp(szCommand, "강퇴") == 0)
	{
		AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsAgpdCharacter);
		if(pcsAttachedGuild && strlen(pcsAttachedGuild->m_szGuildID) > 0)
			pvPacket = m_pagpmGuild->MakeGuildForcedLeavePacket(&nPacketLength,
															&pcsAgpdCharacter->m_lID,
															pcsAttachedGuild->m_szGuildID,
															pcsAgpdCharacter->m_szID,
															szValue);
	}

	if(pvPacket && nPacketLength > 0)
	{
		pcsAgpdCharacter->m_Mutex.Release();

		m_pagpmGuild->OnReceive(0, pvPacket, nPacketLength, 0, FALSE);
		m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

		pcsAgpdCharacter->m_Mutex.WLock();
	}

	return TRUE;
	*/
}

// 2004.08.25. steeple
// Move 급조
BOOL AgsmAdmin::ProcessCommandMove(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 32)
		return FALSE;

	if(lMessageLength < 1)
		return FALSE;

	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		return FALSE;

	CHAR szBuf[255], szTmp[255];
	memset(szBuf, 0, sizeof(CHAR) * 255);
	memset(szTmp, 0, sizeof(CHAR) * 255);

	memcpy(szBuf, szMessage, lMessageLength);
	szBuf[lMessageLength] = '\0';

	AuPOS auPos;
	memset(&auPos, 0, sizeof(auPos));
	
	BOOL bMove = FALSE;

	int j = 0;
	for(int i = 0; i < lMessageLength; i++)
	{
		if(szBuf[i] == ',')
		{
			memcpy(szTmp, szBuf, i);
			auPos.x = (FLOAT)atof(szTmp);

			if(szBuf[i+1] == ' ' && (i+2) < lMessageLength)
			{
				auPos.z = (FLOAT)atof(&szBuf[i+2]);
			}
			else if((i+1) < lMessageLength)
				auPos.z = (FLOAT)atof(&szBuf[i+1]);

			bMove = TRUE;
			break;
		}

		if(szBuf[i] == ' ')
		{
			memcpy(szTmp, szBuf, i);
			auPos.x = (FLOAT)atof(szTmp);

			if((i+1) >= lMessageLength)
				break;
			
			auPos.z = (FLOAT)atof(&szBuf[i+1]);
			
			bMove = TRUE;
			break;
		}
	}

	if(bMove)
	{
		m_pagpmCharacter->UpdatePosition(pcsAgpdCharacter, &auPos, FALSE, TRUE);

		// 2006.11.22. steeple
		// 소환수도 옮겨준다.
		if(m_pagpmSummons)
			m_pagpmSummons->UpdateAllSummonsPosToOwner(pcsAgpdCharacter);

		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[MOVE] %s(%s) : %f,%f\n",
			pcsAgpdCharacter->m_szID,
			pcsAgsdCharacter->m_szAccountID,
			auPos.x,
			auPos.z);
		AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	}

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandSpawn(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 64)
		return TRUE;

	if(!m_pagpmEventSpawn || !m_pagsmEventSpawn || !m_pagsmCharManager)
		return TRUE;

	CHAR szToken[2][32];
	memset(szToken, 0, sizeof(CHAR) * 2 * 32);

	// 앞의 공백 비우기
	for(INT32 i = 0; i < lMessageLength; i++)
	{
		if(szMessage[i] == ' ')
			szMessage++;
		else
			break;
	}

	INT32 j = 0;
	for(INT32 i = 0; i <lMessageLength; i++)
	{
		if(szMessage[i] == ' ' && i + 1 < lMessageLength)
		{
			memcpy(szToken[0], &szMessage[j], i - j);
			memcpy(szToken[1], &szMessage[i + 1], lMessageLength - i - 1);
			break;
		}
	}

	AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(szToken[1]);
	if(!pcsTemplate)
		pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(atoi(szToken[1]));

	if(!pcsTemplate)
		return TRUE;

	INT32 lSpawnCount = atoi(szToken[0]);
	if(lSpawnCount > 20)
		lSpawnCount = 20;

	INT32 lIndex = 0;
	AgpdSpawn* pstSpawn = m_pagpmEventSpawn->GetSpawnByMonsterTID(pcsTemplate->m_lID, &lIndex);
	if(!pstSpawn)
		return TRUE;

	INT32 lAITID = pstSpawn->m_stSpawnConfig[lIndex].m_lAITID;

	for(INT32 i = 0; i < lSpawnCount; ++i)
	{
		if(!m_pagsmEventSpawn->SpawnCharacter(pcsTemplate->m_lID, lAITID, pcsAgpdCharacter->m_stPos, pstSpawn, NULL, TRUE))
			return TRUE;
	}

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[SPAWN] %s(%s) : %s %d\n",
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		pcsTemplate->m_szTName,
		lSpawnCount);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandCreateItemAll(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 64)
		return TRUE;

	CHAR szToken[2][32];
	memset(szToken, 0, sizeof(CHAR) * 2 * 32);

	// TT.......... String Tokenizer 괜히 만들었다. ㅜㅜ
	// 몬스터 이름의 공백 때문에 낭패 ㅜㅜ
	//
	//CHAR* pszToken[2];
	//pszToken[0] = szToken[0];
	//pszToken[1] = szToken[1];

	//ProcessStringToken(szMessage, lMessageLength, pszToken, 2, ' ');

	// 앞의 공백 비우기
	for(INT32 i = 0; i < lMessageLength; i++)
	{
		if(szMessage[i] == ' ')
			szMessage++;
		else
			break;
	}

	INT32 j = 0;
	for(INT32 i = 0; i <lMessageLength; i++)
	{
		if(szMessage[i] == ' ' && i + 1 < lMessageLength)
		{
			memcpy(szToken[0], &szMessage[j], i - j);
			memcpy(szToken[1], &szMessage[i + 1], lMessageLength - i - 1);
			break;
		}
	}

	INT32	lLowLevel	= _ttoi(szToken[0]);
	INT32	lHighLevel	= _ttoi(szToken[1]);

	if (lLowLevel > lHighLevel)
		return FALSE;

	for(AgpaItemTemplate::iterator it = m_pagpmItem->csTemplateAdmin.begin(); it != m_pagpmItem->csTemplateAdmin.end(); ++it)
	{
		AgpdItemTemplate* pcsTemplate = it->second;

		if (pcsTemplate->m_nType == AGPMITEM_TYPE_EQUIP &&
			m_pagpmItem->CheckUseItem(pcsAgpdCharacter, pcsTemplate))
		{
			INT32	lItemLevel = m_pagpmFactors->GetLevel(&pcsTemplate->m_csRestrictFactor);

			if (lItemLevel >= lLowLevel && lItemLevel <= lHighLevel)
			{
				AgpdItem* pcsAgpdItem = m_pagsmItemManager->CreateItem(pcsTemplate->m_lID, pcsAgpdCharacter);

				if(pcsAgpdItem)
				{
					// 귀속아이템이라면 귀속과 주인을 세팅해준다. 2005.11.02. steeple
					if(m_pagpmItem->GetBoundType(pcsAgpdItem->m_pcsItemTemplate) == E_AGPMITEM_BIND_ON_ACQUIRE)
					{
						m_pagpmItem->SetBoundType(pcsAgpdItem, E_AGPMITEM_BIND_ON_ACQUIRE);
						m_pagpmItem->SetBoundOnOwner(pcsAgpdItem, pcsAgpdCharacter);
					}

					AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

					char strCharBuff[256] = { 0, };
					sprintf_s(strCharBuff, sizeof(strCharBuff), "[CREATE] %s(%s) : %s\n",
						pcsAgpdCharacter->m_szID,
						pcsAgsdCharacter->m_szAccountID,
						pcsTemplate->m_szName);
					AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

					m_pagpmItem->AddItemToInventory(pcsAgpdCharacter, pcsAgpdItem);
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandRide(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if (!m_pagsmRide) 
		return TRUE;

	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 64)
		return TRUE;

	if(!m_pagpmEventSpawn || !m_pagsmEventSpawn)
		return TRUE;

	CHAR szBuf[32];
	memset(szBuf, 0, 32);

	memcpy(szBuf, szMessage, lMessageLength);
	szBuf[lMessageLength] = '\0';

	if(0 == _stricmp(szBuf, "on"))
	{
		m_pagsmRide->RideReq(pcsAgpdCharacter, 10);
	}
	else if(0 == _stricmp(szBuf, "off"))
	{
		m_pagsmRide->DismountReq(pcsAgpdCharacter, 10);
	}

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[RIDE] %s(%s) : %s\n", 
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		szBuf);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandQuest(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!m_pagsmQuest || !pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 32)
		return TRUE;

	CHAR szBuf[32];
	memset(szBuf, 0, 32);

	memcpy(szBuf, szMessage, lMessageLength);
	szBuf[lMessageLength] = '\0';

	INT32 lQuestTID = atoi(szBuf);
	if (0 == lQuestTID) 
		return FALSE;

	m_pagsmQuest->CompleteQuestByChatting(pcsAgpdCharacter, lQuestTID);

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[QUEST] %s(%s) : %d\n",
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		lQuestTID);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandSummons(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 64)
		return TRUE;

	if(!m_pagsmSummons)
		return TRUE;

	CHAR szBuf[32];
	memset(szBuf, 0, 32);

	memcpy(szBuf, szMessage, lMessageLength);
	szBuf[lMessageLength] = '\0';

	AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(szBuf);
	if(!pcsTemplate)
		pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(atoi(szBuf));

	if(!pcsTemplate)
		return TRUE;

	UINT32 ulDuration = 60 * 1000;	// 시간은 일단 1분.
	m_pagsmSummons->ProcessSummons(pcsAgpdCharacter, pcsTemplate->m_lID, ulDuration);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[SUMMONS] %s(%s) : %s\n",
		pcsAgpdCharacter->m_szID,
		m_pagsmCharacter->GetAccountID(pcsAgpdCharacter),
		pcsTemplate->m_szTName);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandCashItemTimeExtend(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength, BOOL bCheckAdmin)
{
	if(!szMessage || lMessageLength <= 0 || lMessageLength >= 64)
		return TRUE;

	AgsdServer* pGameServer = m_pAgsmServerManager->GetThisServer();

	if(!pGameServer)
		return FALSE;

	AgsdServer* pRelayServer = m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	CHAR szBuf[32];
	memset(szBuf, 0, 32);

	memcpy(szBuf, szMessage, lMessageLength);
	szBuf[lMessageLength] = '\0';

	UINT32 ulExtendTime = atoi(szBuf);

	PACKET_AGSMCASHITEMTIMEEXTEND_REQUEST_RELAY pRelayPacket(ulExtendTime);

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[CashItemTimeExtend] %s(%s) : %s\n",
		bCheckAdmin ? pcsAgpdCharacter->m_szID : _T("From Console"),
		bCheckAdmin ? pcsAgsdCharacter->m_szAccountID : _T(""),
		szBuf);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandClearInventory(AgpdCharacter* pcsAgpdCharacter, UINT32 ulInvenLayerNum)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	AgpdGridItem* pcsGridItem = NULL;
	AgpdItem* pcsItem = NULL;
	AgpdGrid* pcsGrid = NULL;

	pcsGrid = m_pagpmItem->GetInventory(pcsAgpdCharacter);
	if (!pcsGrid)
		return FALSE;

	if ( ulInvenLayerNum < 0 || pcsGrid->m_nLayer < ulInvenLayerNum )
		return FALSE;

	// 인벤토리를 뒤지면서 체크와 삭제
	for(INT32 lLayer = 0; lLayer < pcsGrid->m_nLayer; lLayer++)
	{
		// 원하는 Layer만 삭제하기 위한 체크 (ulInvenLayerNum가 0이면 모두 삭제)
		if (ulInvenLayerNum != 0 && ulInvenLayerNum-1 != lLayer)
			continue;

		for(INT32 lRow = 0; lRow < pcsGrid->m_nRow; lRow++)
		{
			for(INT32 lColumn = 0; lColumn < pcsGrid->m_nColumn; lColumn++)
			{
				pcsGridItem = m_pagpmGrid->GetItem(pcsGrid, lLayer, lRow, lColumn);
				if(pcsGridItem)
				{
					pcsItem = m_pagpmItem->GetItem(pcsGridItem);
					if (pcsItem)
					{
						// 아이템이 있으면 삭제
						m_pagpmItem->RemoveItem(pcsItem, TRUE);
						pcsItem = NULL;
					}
					pcsGridItem = NULL;
				}	
			}
		}
	}

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandClearCashInventory(AgpdCharacter* pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	AgpdGridItem* pcsGridItem = NULL;
	AgpdItem* pcsItem = NULL;
	AgpdGrid* pcsGrid = NULL;

	pcsGrid = m_pagpmItem->GetCashInventoryGrid(pcsAgpdCharacter);
	if (!pcsGrid)
		return FALSE;

	// 인벤토리를 뒤지면서 체크와 삭제

	while(1)
	{
		pcsGridItem = m_pagpmGrid->GetItem(pcsGrid, 0, 0, 0);

		if(!pcsGridItem)
			break;

		pcsItem = m_pagpmItem->GetItem(pcsGridItem);

		if (!pcsItem)
			break;

		m_pagpmItem->RemoveItem(pcsItem, TRUE);

		pcsItem = NULL;
		pcsGridItem = NULL;
	}

	return TRUE;
}

BOOL AgsmAdmin::ProcessCommandOpenLotteryBox(AgpdCharacter* pcsAgpdCharacter, CHAR* szMessage, INT32 lMessageLength)
{
	if(!pcsAgpdCharacter || !szMessage || lMessageLength <= 0 || lMessageLength >= 64)
		return FALSE;

	CHAR szToken[2][32];
	memset(szToken, 0, sizeof(CHAR) * 2 * 32);

	// 앞의 공백 비우기
	for(INT32 i = 0; i < lMessageLength; i++)
	{
		if(szMessage[i] == ' ')
			szMessage++;
		else
			break;
	}

	INT32 j = 0;
	for(INT32 i = 0; i <lMessageLength; i++)
	{
		if(szMessage[i] == ' ' && i + 1 < lMessageLength)
		{
			memcpy(szToken[0], &szMessage[j], i - j);
			memcpy(szToken[1], &szMessage[i + 1], lMessageLength - i - 1);
			break;
		}
	}

	// Item Template ID
	INT32 lTID = atoi(szToken[1]);

	AgpdItemTemplate* pItemTemplate = m_pagpmItem->GetItemTemplate(lTID);
	if (!pItemTemplate)
	{
		pItemTemplate	= m_pagpmItem->GetItemTemplate(szToken[1]);
		if (!pItemTemplate)
			return FALSE;
	}

	// 로터리 박스인지 체크
	AgpdItemTemplateUsable	*pcsItemTemplateUsable = (AgpdItemTemplateUsable *) pItemTemplate;
	if ( !pcsItemTemplateUsable )
		return FALSE;
	
	if ( AGPMITEM_USABLE_TYPE_LOTTERY_BOX != pcsItemTemplateUsable->m_nUsableItemType )
		return FALSE;

	// 상자를 만들고
	AgpdItem *pcsAgpdItem = NULL;
	pcsAgpdItem = m_pagsmItemManager->CreateItem(pItemTemplate->m_lID, pcsAgpdCharacter);
	if (!pcsAgpdItem)
		return FALSE;

	// 개수 확인
	UINT32 ulCount = (UINT32)atoi(szToken[0]);
	if ( ulCount <= 0 || 1000 < ulCount )
		return FALSE;
		
	// 연다
	for ( int i = 0 ; i < ulCount; i++ )
	{
		if( FALSE == m_pagsmItem->UseItemLotteryBox( pcsAgpdItem, FALSE ) )
			break;
	}

	return TRUE;
}

INT32 AgsmAdmin::ProcessStringToken(CHAR* szMessage, INT32 lMessageLength, CHAR** szToken, INT32 lTokenNum, CHAR cTokener)
{
	if(!szMessage || lMessageLength <= 0 || lMessageLength >=255)
		return 0;

	CHAR szMsg[255];
	memset(szMsg, 0, sizeof(CHAR) * 255);
	memcpy(szMsg, szMessage, lMessageLength);

	return ProcessStringToken(szMsg, szToken, lTokenNum, cTokener);
}

INT32 AgsmAdmin::ProcessStringToken(CHAR* szMessage, CHAR** szToken, INT32 lTokenNum, CHAR cTokener)
{
	if(!szMessage || lTokenNum <= 0)
		return 0;

	INT32 lCount = 0, j = 0;
	INT32 lLength = (INT32)_tcslen(szMessage);
	
	// szMessage 앞에 cTokener 가 있으면 뒤로 민다.
	for(int i = 0; i < lLength; i++)
	{
		if(szMessage[i] == cTokener)
			szMessage++;
		else
			break;
	}

	// 길이를 다시 구한다.
	lLength = (INT32)_tcslen(szMessage);
	for(int i = 0; i < lLength; i++)
	{
		if(szMessage[i] == cTokener)
		{
			if(szMessage[i - 1] == cTokener)	// cTokener 가 연속으로 온 경우
			{
				j = i + 1;
				if(j >= lLength)
					break;
                
				continue;
			}

			if(i + 1 == lLength)				// 마지막 문자라면
				break;

			memcpy(szToken[lCount], &szMessage[j], i - j);
			szToken[lCount][i - j] = '\0';

			// 필요한 토큰 개수를 다 구했으면 나간다.
			if(++lCount >= lTokenNum)
				break;

			j = i + 1;
			if(j >= lLength)
				break;
		}
		else if(i + 1 == lLength)
		{
			memcpy(szToken[lCount], &szMessage[j], lLength - j);
			szToken[lCount][lLength - j] = '\0';

			lCount++;
			break;
		}
	}

	return lCount;
}









//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgsmAdmin::CBRecvNormalMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdChatData * pstChatData = (AgpdChatData *)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pstChatData || !pThis)
		return FALSE;

	// Admin 명령어 인지 파싱한다.
	pThis->ParseCommand(pstChatData);
	
	return TRUE;
}

// From AgsmChatting
BOOL AgsmAdmin::CBCheckAdminCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdChatData * pstChatData = (AgpdChatData *)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!pstChatData || !pThis)
		return FALSE;

	return pThis->ParseCommand(pstChatData);
}

BOOL AgsmAdmin::CBReceiveEventMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmAdmin		*pThis			= (AgsmAdmin *)		pClass;
	AgpdChatData	*pstChatData	= (AgpdChatData *)	pData;
	INT32			*plSkillTID		= (INT32 *)			pCustData;

	if (pThis->m_pagpmConfig->IsEventChatting() &&
		pstChatData->pcsSenderBase)
	{
		// 초대형 회피의 물약
		pThis->m_pagsmSkill->CastSkill((ApBase *) pstChatData->pcsSenderBase, *plSkillTID, 1, NULL, FALSE, FALSE);
	}

	return TRUE;
}
