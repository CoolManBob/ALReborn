#ifndef	__AGCMCHATTING2_H__
#define	__AGCMCHATTING2_H__

#include "AgpmChatting.h"
#include "AgpmGuild.h"
#include "AgpmAreaChatting.h"

#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgcmFont.h"
#include "AgcmEventEffect.h"
#include "AgcmGuild.h"

#include "CTextFilter.h"

const int		AGCMCHATTING_MAX_CHAT_MESSAGE_LENGTH		= 128;
const int		AGCMCHATTING_MAX_CHAT_LIST					= 100;
const int		AGCMCHATTING_MAX_CHAT_MESSAGE_PIXEL_SIZE	= 380;
const int		AGCMCHATTING_MAX_SYSTEM_MESSAGE_PIXEL_SIZE	= 220;
const int		AGCMCHATTING_MAX_CHAT_STRING_LENGTH			= 128;

const int		AGCMCHATTING_MAX_CHAT_FILTER_TEXT			= 32;

#define	UI_MESSAGE_ID_CHAT_PARTY_HEADER					"Chat_Party_Header"
#define	UI_MESSAGE_ID_CHAT_GUILD_HEADER					"Chat_Guild_Header"
#define	UI_MESSAGE_ID_CHAT_WHISPER_HEADER				"Chat_Whisper_Header"
#define	UI_MESSAGE_ID_CHAT_WHISPER_HEADER2				"Chat_WhisperSelf_header"
#define	UI_MESSAGE_ID_CHAT_NOTICE_HEADER				"Chat_Notice_Header"
#define	UI_MESSAGE_ID_CHAT_SYSTEM_HEADER				"Chat_System_Header"
#define	UI_MESSAGE_ID_CHAT_AREA_CHAT_HEADER				"Chat_Area_Header"
#define UI_MESSAGE_ID_CHAT_GUILD_JOINT_HEADER			"Chat_Guild_Joint_Header"

#define	UI_MESSAGE_ID_CHAT_BLOCK_WHISPER_MSG			"Chat_Block_Whisper_Msg"
#define	UI_MESSAGE_ID_CHAT_OFFLINE_WHISPER_TARGET_MSG	"Chat_Offline_Whisper_Target_Msg"

#define	UI_MESSAGE_ID_CHAT_FILTER_TEXT					"Chat_Filter_Text"

enum AgcmChattingType {
	AGCMCHATTING_TYPE_NORMAL				= 0,
	AGCMCHATTING_TYPE_PARTY,
	AGCMCHATTING_TYPE_GUILD,
	AGCMCHATTING_TYPE_WHISPER,
	AGCMCHATTING_TYPE_AREA,
	AGCMCHATTING_TYPE_NOTICE,
	AGCMCHATTING_TYPE_SYSTEM,
	AGCMCHATTING_TYPE_GUILD_JOINT,
	AGCMCHATTING_TYPE_EMPHASIS,
	AGCMCHATTING_TYPE_SIEGEWARINFO,
	AGCMCHATTING_TYPE_MAX,
};

enum AgcmChatting2CBID {
	AGCMCHATTING2_CB_INPUT_MESSAGE			= 0,
	AGCMCHATTING2_CB_COMMAND_CALLBACK,
	AGCMCHATTING2_CB_COMMAND_GUILD_LEAVE,
	AGCMCHATTING2_CB_INPUT_SIEGEWAR_MESSAGE,
};

class AgcuPathWork;
class AgcmTextBoardMng;

class AgcmChatting2 : public AgcModule, public CTextFilter
{
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmChatting		*m_pcsAgpmChatting;
	AgpmGuild			*m_pcsAgpmGuild;
	AgpmAreaChatting	*m_pcsAgpmAreaChatting;

	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmTextBoardMng	*m_pcsAgcmTextBoardMng;
	AgcmUIManager2		*m_pcsAgcmUIManager2;
	AgcmFont			*m_pcsAgcmFont;
	AgcmEventEffect		*m_pcsAgcmEventEffect;
	AgcmGuild			*m_pcsAgcmGuild;
	AgcmConnectManager	*m_pcsAgcmConnectManager;
	ApmMap				*m_pcsApmMap;

	CHAR				m_aszChatMessage[AGCMCHATTING_TYPE_MAX][AGCMCHATTING_MAX_CHAT_LIST][AGCMCHATTING_MAX_CHAT_MESSAGE_LENGTH + 1];
	UINT32				m_aulChatMessageColor[AGCMCHATTING_TYPE_MAX][AGCMCHATTING_MAX_CHAT_LIST];
	UINT32				m_aulChatBackgroundColor[AGCMCHATTING_TYPE_MAX][AGCMCHATTING_MAX_CHAT_LIST];
	BOOL				m_abChatMessageBold[AGCMCHATTING_TYPE_MAX][AGCMCHATTING_MAX_CHAT_LIST];
	BOOL				m_abChatMessageApplyColorAllText[AGCMCHATTING_TYPE_MAX][AGCMCHATTING_MAX_CHAT_LIST];
	UINT32				m_aulChatMessageTime[AGCMCHATTING_TYPE_MAX][AGCMCHATTING_MAX_CHAT_LIST];
	UINT32				m_aulChatMessageCount[AGCMCHATTING_TYPE_MAX];
	BOOL				m_abChatMessageEnable[AGCMCHATTING_TYPE_MAX];

	BOOL				AddMessage(AgcmChattingType eChatType, CHAR *szMessage, UINT32 ulColor, BOOL bAddForce = FALSE, BOOL bApplyAllTextColor = FALSE, UINT32 ulBGColor = 0x00000000, BOOL bBold = FALSE);

	BOOL				ParseSystemCommand(CHAR *szMessage, INT32 lLength);

	BOOL				SendSetBlockWhisper();
	BOOL				SendReleaseBlockWhisper();

	INT16				m_nNID;

	UINT32				m_ulLastSendAreaMessageMSec;

	//BOOL				m_bIsRetryConnectChatServer;
	//UINT32				m_ulRetryConnectTimeMSec;

	// 마고자 (2005-07-14 오후 5:22:00) : 
	// 피스웍 처리 
	AgcuPathWork*		m_pcamPathWork;

	CHAR				m_szLastWhisperCharacter[AGPDCHARACTER_MAX_ID_LENGTH];

public:
	BOOL				AddChatMessage(AgcmChattingType eChatType, AgpdChatData * pstChatData, BOOL bIsSendWhisper = FALSE, BOOL bTranslateMessage = FALSE);
	BOOL				AddSystemMessage(CHAR *szMessage, UINT32 ulColor);

public:
	AgcmChatting2();
	virtual ~AgcmChatting2();

	BOOL				OnAddModule();
	BOOL				OnIdle(UINT32 ulClockCount);
	BOOL				OnInit(VOID);
	void				OnLuaInitialize( AuLua * pLua );

	BOOL				SendChatMessage(AgpdChattingType eType, CHAR *szMessage, INT32 lLength,
										CHAR *szTargetID);

	VOID				ClearChatMessages();

	static BOOL			CBRecvNormalMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRecvWordBalloonMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRecvGuildMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRecvPartyMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRecvWhisperMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRecvWholeWorldMsg(PVOID pData, PVOID pClass, PVOID pCustData);	// 2003.11.18. steeple
	static BOOL			CBRecvSystemMsg(PVOID pData, PVOID pClass, PVOID pCustData);		// 2003.12.8. steeple

	static BOOL			CBRecvGuildJointMsg(PVOID pData, PVOID pClass, PVOID pCustData);	// 2006.08.16. steeple

	static BOOL			CBRecvReplyBlockWhisper(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRecvOfflineWhisperTarget(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBPutUIEventActionMsg(PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL			CBRecvAreaChatMsg(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL			CBChangeRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				SetCallbackInputMessage		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackInputSiegeWarMessage		(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// 마고자 (2004-11-03 오후 4:00:19) : 커맨드처리 콜백..
	BOOL				SetCallbackCommandProcess	(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// 2005.02.17. steeple. 길드 탈퇴 명령어 처리
	BOOL				SetCallbackCommandGuildLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	CHAR*				GetChatMessage(AgcmChattingType eChatType, INT32 lIndex);
	UINT32				GetChatMessageColor(AgcmChattingType eChatType, INT32 lIndex);
	UINT32				GetChatBackgroundColor(AgcmChattingType eChatType, INT32 lIndex);
	BOOL				GetChatMessageBold(AgcmChattingType eChatType, INT32 lIndex);
	BOOL				GetChatMessageApplyColorAllText(AgcmChattingType eChatType, INT32 lIndex);
	UINT32				GetChatMessageTime(AgcmChattingType eChatType, INT32 lIndex);
	UINT32				GetChatMessageCount(AgcmChattingType eChatType);

	BOOL				AddSystemMessage(CHAR *szMessage);

	BOOL				AddAreaChatMessage(CHAR *szMessage, INT32 lMessageLength, BOOL bIsAdminCharacter, BOOL bIsAddHeader, UINT32 ulTextColorRGB);
	BOOL				AddSiegeWarMessage(CHAR *szMessage);

	static BOOL			CBSocialAnimation(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				ActiveChatMode(AgcmChattingType eChatType);
	BOOL				DisableChatMode(AgcmChattingType eChatType);

	void				StopCameraWalking();

	void				SetLastWhisperCharacter(CHAR *szID);
	CHAR *				GetLastWhisperCharacter();

	void				TranslateChatMessage(CHAR *szDest, CHAR *szSrc, INT32 lLength);

	// 채팅 메세지를 받을수 있는 곳인지 확인한다
	BOOL				CheckChatRegion( VOID );
};

inline	void Display_System_Message( char * pStr , UINT32 ulColor = 0xffffff00 )
{
	AgcmChatting2 * pcsAgcmChatting2 = ( AgcmChatting2 * ) g_pEngine->GetModule( "AgcmChatting2" );
	if( pcsAgcmChatting2  )
	{
		pcsAgcmChatting2->AddSystemMessage( pStr , ulColor );
	}
}
#endif	//__AGCMCHATTING2_H__