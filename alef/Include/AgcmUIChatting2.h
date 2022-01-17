#ifndef	__AGCMUICHATTING2_H__
#define	__AGCMUICHATTING2_H__

#include "AgcmChatting2.h"
#include "AcUIChattingEdit.h"
#include "AgpmParty.h"
#include "AgcmEventNature.h"

#include <string>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIChatting2D" )
#else
#pragma comment ( lib , "AgcmUIChatting2" )
#endif
#endif

const int	AGCMUICHATTING2_MAX_COMBO_BUTTON		= 4;
const int	AGCMUICHATTING2_MAX_BGM_COUNT			= 29;		// BGM 개수 (2005.03.10. steeple)

#define	UI_MESSAGE_ID_CHAT_TAB_NORMAL			"Chat_Tab_Normal"
#define	UI_MESSAGE_ID_CHAT_TAB_PARTY			"Chat_Tab_Party"
#define	UI_MESSAGE_ID_CHAT_TAB_GUILD			"Chat_Tab_Guild"
#define	UI_MESSAGE_ID_CHAT_TAB_WHISPER			"Chat_Tab_Whisper"

#define	UI_MESSAGE_ID_CHAT_OPTION_PARTY_OFF		"Option_Party_Off"
#define	UI_MESSAGE_ID_CHAT_OPTION_PARTY_ON		"Option_Party_On"
#define	UI_MESSAGE_ID_CHAT_OPTION_TRADE_OFF		"Option_Trade_Off"
#define	UI_MESSAGE_ID_CHAT_OPTION_TRADE_ON		"Option_Trade_On"
#define	UI_MESSAGE_ID_CHAT_OPTION_GUILD_OFF		"Option_Guild_Off"
#define	UI_MESSAGE_ID_CHAT_OPTION_GUILD_ON		"Option_Guild_On"
#define	UI_MESSAGE_ID_CHAT_OPTION_GBATTLE_OFF	"Option_GBattle_Off"
#define	UI_MESSAGE_ID_CHAT_OPTION_GBATTLE_ON	"Option_GBattle_On"
#define	UI_MESSAGE_ID_CHAT_OPTION_BATTLE_OFF	"Option_Battle_Off"
#define	UI_MESSAGE_ID_CHAT_OPTION_BATTLE_ON		"Option_Battle_On"
#define	UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR		"Positive_Text_Color"
#define	UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR		"Negative_Text_Color"

#define	UI_MESSAGE_ID_CHAR_OPTION_ALEADY_SET	"Option_Aleady_Set"

#define	UI_MESSAGE_ID_CHAT_OPTION_SKILL_EFFECT_OFF			"Option_SkillEffect_Off"
#define	UI_MESSAGE_ID_CHAT_OPTION_SKILL_EFFECT_ON			"Option_SkillEffect_On"
#define	UI_MESSAGE_ID_CHAT_OPTION_OTHER_SKILL_EFFECT_OFF	"Option_OtherSkillEffect_Off"
#define	UI_MESSAGE_ID_CHAT_OPTION_OTHER_SKILL_EFFECT_ON		"Option_OtherSkillEffect_On"


class AgcmUIChatting2 : public AgcModule {
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgcmCharacter		*m_pcsAgcmCharacter;
	AgpmParty			*m_pcsAgpmParty;
	AgcmChatting2		*m_pcsAgcmChatting2;
	AgcmUIManager2		*m_pcsAgcmUIManager2;
	AgcmSound			*m_pcsAgcmSound;
	AgcmEventNature		*m_pcsAgcmEventNature;

	INT32				m_alListIndex[AGCMCHATTING_MAX_CHAT_LIST];

	INT32				m_lCurrentChattingModeIndex;
	INT32				m_alComboValue[AGCMUICHATTING2_MAX_COMBO_BUTTON];

	INT32				m_lEventChatInputNormalUIOpen;

	INT32				m_lEventGetScrollControl;
	INT32				m_lEventGetNormalEditControl;
	INT32				m_lEventGetSystemEditControl;
	INT32				m_lEventGetInputIDEditControl;

	INT32				m_lEventOpenInputPrivateUI;

	INT32				m_lEventSetFocusNormalChat;
	INT32				m_lEventKillFocusNormalChat;

	INT32				m_lEventSetFocusSystemChat;
	INT32				m_lEventKillFocusSystemChat;

	INT32				m_lEventSetFocusNormalChatOptBase;
	INT32				m_lEventKillFocusNormalChatOptBase;
	INT32				m_lEventSetFocusNormalChatOptSmallBase;
	INT32				m_lEventKillFocusNormalChatOptSmallBase;

	AgcdUIUserData		*m_pcsUDChattingList;
	AgcdUIUserData		*m_pcsUDSystemMsgList;
	AgcdUIUserData		*m_pcsUDChattingMode;
	AgcdUIUserData		*m_pcsUserDataComboTitle;

	AgcdUIControl		*m_pcsScrollControl;

	AgcdUIControl		*m_pcsNormalEditControl;
	AgcdUIControl		*m_pcsSystemEditControl;

	AcUIChattingEdit	m_csUIChattingEdit;

	INT32				ParseNormalChat(CHAR *szMessage, INT32 lLength);

	// 2005.03.10. steeple
	AgcdUIUserData*		m_pstUDBGMTitle;
	INT32				m_lBGMTitle;	// Dummy
	CHAR*				m_szNowBGM;

	BOOL				m_bIsSetOptBase;

	
	// Now, you can input chat messages in the dev console.
	bool				ProcessChatMessage(const std::string &message);
	
public:
	BOOL				SetNormalChatMessage();
	BOOL				SetSystemChatMessage();

	BOOL				CheckOptionFlag(const CHAR *szMessage);
	BOOL				SetChattingMessage(const TCHAR *szMessage);
	void				ChatInputNormalUIOpen();

	VOID				SetWhisperTo(CHAR *szName);

public :
	void				OnVisibleWindowNormalChatting( BOOL bIsVisible );
	void				OnVisibleWindowSystemChatting( BOOL bIsVisible );

public:
	AgcmUIChatting2();
	virtual ~AgcmUIChatting2();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	static BOOL			CBDisplayChatMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL			CBDisplayComboTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL			CBDisplayComboList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL			CBChangeChatMode(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBInputEndChatMessage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBInputEndPrivateMessage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBGetScrollControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBInputStart(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBCheckNCloseInput(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBGetNormalChatEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBGetSystemChatEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

//	static BOOL			CBGetInputIDEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBDisableNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetGuildChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBDisableGuildChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetPartyChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBDisablePartyChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBDisableWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetAreaChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBDisableAreaChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSendWhisper(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetFocusNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBKillFocusNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBSetFocusSystemChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBKillFocusSystemChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBSetFocusNormalChatOptBase(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBSetFocusNormalChatOptSmallBase(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBInputMessage(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	// 2005.03.10. steeple
	static BOOL			CBDisplayBGMTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL			CBUpdateOptionFlag( PVOID pData, PVOID pClass, PVOID pCustData );	//	2005.06.01 By SungHoon

	// BGM 이 바뀔 때 호출됨
	static BOOL			CBBGMUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	const CHAR*			GetBGMTitleByFileName(CHAR* szBGMFileName);

	// 즉시 회신 버튼
	static BOOL			CBReplyImmedite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// 채팅창 리뉴얼에 의해 채팅종류별로 버튼을 누르면 자동으로 해당 명령어를 채팅창에 입력해주기 위한 콜백
	static BOOL			CBStartChattingNormal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBStartChattingGuild(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBStartChattingParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBStartChattingWhisper(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBStartChattingShout(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	// boolean callback functions
	///////////////////////////////////////////////////////////////////////
//	static BOOL			CBIsActiveWhisperMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
};

#endif	//__AGCMUICHATTING2_H__