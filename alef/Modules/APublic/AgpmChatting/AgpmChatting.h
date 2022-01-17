//	AgpmChatting.h
////////////////////////////////////////////////////////////////

#ifndef	__AGPMCHATTING_H__
#define	__AGPMCHATTING_H__

#include "ApAdmin.h"
#include "AgpmCharacter.h"
#include "AgpmParty.h"
#include "AuPacket.h"
#include "AgpdChatting.h"


#define AGPMCHAT_LOWER_COMMAND_SEND_GUILD_JOINT			"/u"
#define AGPMCHAT_UPPER_COMMAND_SEND_GUILD_JOINT			"/U"
#define AGPMCHAT_COMMAND_SEND_GUILD_JOINT_KOR			"/연대"

#define	AGPMCHAT_MAX_COMMANDS		500

typedef enum _AgpdChattingPacketOperation {
	AGPDCHATTING_OPERATION_CHAT					= 0,
	AGPDCHATTING_OPERATION_SET_BLOCK_WHISPER,
	AGPDCHATTING_OPERATION_RELEASE_BLOCK_WHISPER,
	AGPDCHATTING_OPERATION_REPLY_BLOCK_WHISPER,
	AGPDCHATTING_OPERATION_OFFLINE_WHISPER_TARGET,
} AgpdChattingPacketOperation;

// chatting type setting.........
typedef enum
{
	AGPDCHATTING_TYPE_NORMAL					= 0,
	AGPDCHATTING_TYPE_WORD_BALLOON,

	AGPDCHATTING_TYPE_UNION,
	AGPDCHATTING_TYPE_GUILD,
	AGPDCHATTING_TYPE_PARTY,

	AGPDCHATTING_TYPE_WHISPER,			// 받은 귓속말
	AGPDCHATTING_TYPE_WHISPER2,			// 보낸 귓속말

	AGPDCHATTING_TYPE_WHOLE_WORLD,

	AGPDCHATTING_TYPE_SYSTEM_LEVEL1,
	AGPDCHATTING_TYPE_SYSTEM_LEVEL2,
	AGPDCHATTING_TYPE_SYSTEM_LEVEL3,	// 경고 메세지 (붉은색)

	AGPDCHATTING_TYPE_NOTICE_LEVEL1,
	AGPDCHATTING_TYPE_NOTICE_LEVEL2,

	AGPDCHATTING_TYPE_GUILD_NOTICE,

	AGPDCHATTING_TYPE_GUILD_JOINT,

	AGPDCHATTING_TYPE_EMPHASIS,
	AGPDCHATTING_TYPE_SIEGEWARINFO,

	AGPDCHATTING_TYPE_MAX,
	
} AgpdChattingType;

// callback function id
typedef enum
{
	AGPMCHATTING_CB_RECV_NORMAL_MSG				= 0,
	AGPMCHATTING_CB_RECV_WORD_BALLOON_MSG,
	AGPMCHATTING_CB_RECV_UNION_MSG,
	AGPMCHATTING_CB_RECV_GUILD_MSG,
	AGPMCHATTING_CB_RECV_PARTY_MSG,
	AGPMCHATTING_CB_RECV_WHISPER_MSG,
	AGPMCHATTING_CB_RECV_WHOLE_WORLD_MSG,
	AGPMCHATTING_CB_RECV_SYSTEM_LEVEL1_MSG,
	AGPMCHATTING_CB_RECV_SYSTEM_LEVEL2_MSG,
	AGPMCHATTING_CB_RECV_SYSTEM_LEVEL3_MSG,
	AGPMCHATTING_CB_RECV_NOTICE_LEVEL1_MSG,
	AGPMCHATTING_CB_RECV_NOTICE_LEVEL2_MSG,
	AGPMCHATTING_CB_CHECK_ENABLE,
	AGPMCHATTING_CB_CHECK_BLOCK_WHISPER_MSG,

	AGPMCHATTING_CB_RECV_REPLY_BLOCK_WHISPER,
	AGPMCHATTING_CB_RECV_OFFLINE_WHISPER_TARGET,

	AGPMCHATTING_CB_RECV_GUILD_JOINT_MSG,

	AGPMCHATTING_CB_RECV_EMPHASIS_MSG,
} AgpmChattingCB;

typedef struct	AgpdChatData {
	AgpdChattingType	eChatType;				// chatting message type
	INT32				lSenderID;				// message sender id
	CHAR*				szSenderName;			// message sender name
	ApBase*				pcsSenderBase;			// message sender base pointer

	INT32				lTargetID;
	CHAR*				szTargetName;
	ApBase *			pcsTargetBase;
	
	CHAR*				szMessage;				// chat message
	UINT32				lMessageLength;			// chat message length
} AgpmChatData;

class AgpmChatting : public ApModule
{
private:
	ApAdmin			m_csChatCommands;
	INT32			m_lMaxCommands;

	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmParty		*m_pcsAgpmParty;

	INT32			m_lIndexADChar;

	BOOL			OnOperationChat(AgpdCharacter *pcsCharacter, INT8 cChatType, CHAR *szSenderName, CHAR *szTargetID, CHAR *szMessage, UINT16 unMessageLength);
	BOOL			OnOperationSetBlockWhisper(AgpdCharacter *pcsCharacter);
	BOOL			OnOperationReleaseBlockWhisper(AgpdCharacter *pcsCharacter);
	BOOL			OnOperationReplyBlockWhisper(AgpdCharacter *pcsCharacter, CHAR *szTargetID);
	BOOL			OnOperationOfflineWhisperTarget(AgpdCharacter *pcsCharacter, CHAR *szTargetID);

public:
	AuPacket		m_csPacket;

public:
	AgpmChatting();
	~AgpmChatting();

	BOOL			OnAddModule();
	BOOL			OnInit();
	BOOL			OnDestroy();

	VOID			SetMaxCommands(INT32 lMax)	{ m_lMaxCommands = lMax;	}
	BOOL			AddCommand(CHAR *szCommand, ApModuleDefaultCallBack fnCallback, PVOID pvClass);
	AgpdChatCommand *	GetCommand(CHAR *szCommand);
	BOOL			RemoveCommand(CHAR *szCommand);

	BOOL			DispatchCommand(CHAR *szChatMessage);

	BOOL			OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	PVOID			MakePacketChatting(INT32 lCID, AgpdChattingType eChatType, CHAR *szSenderName,
									   CHAR *szTargetID, 
									   CHAR *szMessage, INT32 lMessageLength, INT16 *pnPacketLength);
	PVOID			MakePacketSetBlockWhisper(INT32 lCID, INT16 *pnPacketLength);
	PVOID			MakePacketReleaseBlockWhisper(INT32 lCID, INT16 *pnPacketLength);
	PVOID			MakePacketReplyBlockWhisper(INT32 lCID, CHAR *szTargetID, INT16 *pnPacketLength);
	PVOID			MakePacketOfflineWhisperTarget(INT32 lCID, CHAR *szTargetID, INT16 *pnPacketLength);

	// callback functions
	BOOL			SetCallbackRecvNormalMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvWordBalloonMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvUnionMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvGuildMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvPartyMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvWhisperMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvWholeWorldMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvSystemLevel1Msg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvSystemLevel2Msg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvSystemLevel3Msg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackCheckEnable(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackCheckBlockWhisper(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackRecvReplyBlockWhisper(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRecvOfflineWhisperTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackRecvGuildJointMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetCallbackRecvEmphasisMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL		ConAgpdCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		DesAgpdCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdChattingADChar*	GetADCharacter(AgpdCharacter *pcsCharacter);

	INT32			GetLastWhisperSenderID(AgpdCharacter *pcsCharacter);

	BOOL			OperationChat( AgpdCharacter *pcsCharacter , char * pMessage );
};

#endif	//__AGPMCHATTING_H__