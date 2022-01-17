//	AgsmChatting.h file
//
//////////////////////////////////////////////////////////

#ifndef	__AGSMCHATTING_H__
#define	__AGSMCHATTING_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmChattingD" )
#else
#pragma comment ( lib , "AgsmChatting" )
#endif
#endif

#include "AgpmChatting.h"
#include "AgsEngine.h"
class AgpmLog;
class AgpmFactors;
class AgpmCharacter;
class AgpmItem;
class AgpmParty;			
class AgpmChatting;
class AgpmSkill;			
class AgpmPvP;		
class AgpmAdmin;
class AgsmAOIFilter;
class AgsmFactors;	
class AgsmCharacter;
class AgsmServerManager2;
class AgsmItem;	
class AgsmParty;
class AgsmDeath;	
class AgpmConfig;
class AgpmBattleGround;
class AgsdChatEvent;
class AgsmGuild;
class ApmMap;

typedef enum	_eAgsmChatCommandFailType {
	AGSMCHAT_COMMAND_FAIL_NO_EXIST_TARGET			= 0,
} eAgsmChatCommandFailType;

typedef enum	_eAgsmChattingCB
{
	AGSMCHATTING_CB_CHECK_ADMIN_COMMAND = 0,
	AGSMCHATTING_CB_QUEST_COMPLETE,
	AGSMCHATTING_CB_GUILD_MEMBER_INVITE,
	AGSMCHATTING_CB_GUILD_LEAVE,
	AGSMCHATTING_CB_EVENT_MESSAGE,
	AGSMCHATTING_CB_AREA_CHATTING_RACE,
	AGSMCHATTING_CB_AREA_CHATTING_ALL,
	AGSMCHATTING_CB_AREA_CHATTING_GLOBAL,
	AGSMCHATTING_CB_GUILD_JOINT_MESSAGE,
	AGSMCHATTING_CB_ARCHLORD_MESSAGE,
} eAgsmChattingCB;

class AgsdChatEvent {
public:
	ApString<128>	strChatMessage;
	INT32			lSkillTID;
	INT32			lSkillLevel;

	AgsdChatEvent()
	{
		strChatMessage.Clear();
		lSkillTID	= AP_INVALID_SKILLID;
		lSkillLevel	= 0;
	}
};

class AgsmChatting : public AgsModule
{
private:
	AgpmLog*			m_pagpmLog;
	AgpmFactors*		m_pagpmFactors;
	AgpmCharacter*		m_pagpmCharacter;
	AgpmItem*			m_pagpmItem;
	AgpmParty*			m_pagpmParty;
	AgpmChatting*		m_pagpmChatting;
	AgpmSkill*			m_pagpmSkill;
	AgpmPvP*			m_pagpmPvP;
	AgpmAdmin*			m_pagpmAdmin;

	AgsmAOIFilter*		m_pagsmAOIFilter;
	AgsmFactors*		m_pagsmFactors;
	AgsmCharacter*		m_pagsmCharacter;
	AgsmItem*			m_pagsmItem;
	AgsmParty*			m_pagsmParty;
	AgsmServerManager2*	m_pAgsmServerManager;
	AgsmDeath*			m_pagsmDeath;

	AgpmConfig*			m_pagpmConfig;
	AgsmGuild*			m_pagsmGuild;
	AgpmBattleGround*	m_pagpmBattleGround;
	ApmMap*				m_papmMap;

	INT32				m_lEventDataCount;
	AgsdChatEvent*		m_pChatEventData;

public:
	AgsmChatting();
	~AgsmChatting();

	BOOL			OnAddModule();
	BOOL			OnInit();
	BOOL			OnDestroy();

	static BOOL		CBRecvNormalMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	//static BOOL		CBRecvUnionMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRecvGuildMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRecvPartyMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRecvWhisperMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRecvSystemLevel1Msg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRecvSystemLevel2Msg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRecvSystemLevel3Msg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			ParseCommand(AgpdChatData * pstChatData);

	BOOL			ProcessCommandUseWordBalloon(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength);
	BOOL			ProcessCommandWhisper(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength);
	BOOL			ProcessCommandReply(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength);
	BOOL			ProcessCommandSendGuild(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength);
	BOOL			ProcessCommandSendParty(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength);
	BOOL			ProcessCommandEmphasis(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength);
	BOOL			ProcessCommandGuildMemberInvite(AgpdChatData* pstChatData, CHAR* szParseMessage, INT32 lLength);
	BOOL			ProcessCommandPartyMemberInvite(AgpdChatData* pstChatData, CHAR* szParseMessage, INT32 lLength);
	BOOL			ProcessCommandSendGuildJoint(AgpdChatData * pstChatData, CHAR *szParseMessage, INT32 lLength);
	
	// 명령어 뒤에 파라미터가 없는 놈들
	BOOL			ProcessCommandGuildLeave(AgpdChatData* pstChatData);

	BOOL			CheckEnable(AgpdCharacter* pcsAgpdCharacter, INT8 cChatType);

	BOOL			SendMessageNormal(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength);
	BOOL			SendMessageWordBalloon(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength);
	BOOL			SendMessageWhisper(ApBase *pcsSender, ApBase *pcsTarget, CHAR *szMessage, INT32 lMessageLength);
	BOOL			SendMessageParty(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength);
	BOOL			SendMessageGuild(ApBase *pcsSender, CHAR *szMessage, INT32 lMessageLength);
	BOOL			SendMessageSystem(ApBase* pcsSender, INT32 lLevel, CHAR *szMessage, INT32 lMessageLength, UINT32 dpnID = 0);
	BOOL			SendMessageEmphasis(ApBase* pcsSender, CHAR *szMessage, INT32 lMessageLength);

	BOOL			SendMessage(UINT32 dpnid, AgpdChattingType eChatType, INT32 lSenderID, CHAR *szSenderName, CHAR *szTargetID, CHAR *szMessage, INT32 lMessageLength);
	BOOL			SendMessageAll(AgpdChattingType eChatType, INT32 lSendID, CHAR* szMessage, INT32 lMessageLength);

	BOOL			SendMessageCommandFailed(UINT32 dpnid, eAgsmChatCommandFailType eFailType, INT32 lTargetID = AP_INVALID_CID);

	//static BOOL		CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL SetCallbackCheckAdminCommand(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 2003.11.18. steeple
	BOOL SetCallbackQuestComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// 2004.10.09. 퀘스트 테스트를 위해 추가 kermi
	BOOL SetCallbackGuildMemberInvite(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 2004.11.15. steeple
	BOOL SetCallbackGuildLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);			// 2005.02.16. steeple
	BOOL SetCallbackEventMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAreaChattingRace(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAreaChattingAll(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAreaChattingGlobal(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildJointMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 2006.08.11. steeple
	BOOL SetCallbackArchlordMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);		// 2006.09.29. steeple

	BOOL			StreamReadChatEvent(CHAR *pszFile);

	BOOL			ProcessChatEvent(AgpdChatData * pstChatData);
};

#endif //__AGSMCHATTING_H__
