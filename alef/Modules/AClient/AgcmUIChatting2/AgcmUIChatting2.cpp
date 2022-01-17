#include "AgcmUIChatting2.h"

#include "AgcmEventNature.h"
#include "ApUtil.h"
#include "AgcmPostFX.h"
#include "AgcmUIConsole.h"
#include "ApMemoryTracker.h"
#include "AgcmUIPartyOption.h"

#include "AuStrTable.h"
#include "AgcmUISearch.h"

#include "AgcuCamMode.h"

// 2005.03.10. steeple
// BGM File, Title List
const CHAR* BGM_FILE_LIST[AGCMUICHATTING2_MAX_BGM_COUNT] = {
	"MT_001.MP3",

	"HTT_001.MP3", "HTT_002.MP3",

	"HFT_001.MP3", "HFT_002.MP3", "HFT_003.MP3", "HFT_004.MP3",
	"HFT_005.MP3", "HFT_006.MP3", "HFT_007.MP3", "HFT_008.MP3",
	
	"OTT_001.MP3", "OTT_002.MP3",
	
	"OFT_001.MP3", "OFT_002.MP3", "OFT_003.MP3", "OFT_004.MP3",
	"OFT_005.MP3", "OFT_006.MP3", "OFT_007.MP3", "OFT_008.MP3",
	
	"BT_001.MP3",
	
	"NFT_001.MP3", "NFT_002.MP3", "NFT_003.MP3", "NFT_004.MP3",
	"NFT_005.MP3", "NFT_006.MP3", "NFT_007.MP3"
};

CHAR* BGM_TITLE_LIST[AGCMUICHATTING2_MAX_BGM_COUNT] = {
	NULL,
};

AgcmUISearch* g_AgcmUISearch	= NULL;

AgcmUIChatting2::AgcmUIChatting2()
{
	SetModuleName("AgcmUIChatting2");

	for (int i = 0; i < AGCMCHATTING_MAX_CHAT_LIST; ++i)
		m_alListIndex[i]	= i;

	m_alComboValue[0]	= AGCMCHATTING_TYPE_NORMAL;
	m_alComboValue[1]	= AGCMCHATTING_TYPE_PARTY;
	m_alComboValue[2]	= AGCMCHATTING_TYPE_GUILD;
	m_alComboValue[3]	= AGCMCHATTING_TYPE_WHISPER;

	m_lCurrentChattingModeIndex	= AGCMCHATTING_TYPE_NORMAL;

	m_pcsScrollControl		= NULL;

	m_pcsNormalEditControl	= NULL;
	m_pcsSystemEditControl	= NULL;
	//m_pcsInputIDEditControl	= NULL;
	//m_pcsInputMsgEditControl	= NULL;

	m_pstUDBGMTitle = NULL;
	m_lBGMTitle = 0;

	m_pcsAgcmSound = NULL;
	m_pcsAgcmEventNature = NULL;
	m_szNowBGM = NULL;

	m_bIsSetOptBase	= TRUE;

	BGM_TITLE_LIST[0] = ClientStr().GetStr(STI_AMUNESIA);
	BGM_TITLE_LIST[1] = ClientStr().GetStr(STI_THOULAN);
	BGM_TITLE_LIST[2] = ClientStr().GetStr(STI_DELPARASS);
	BGM_TITLE_LIST[3] = ClientStr().GetStr(STI_ELAN);
	BGM_TITLE_LIST[4] = ClientStr().GetStr(STI_JUNGLE);
	BGM_TITLE_LIST[5] = ClientStr().GetStr(STI_AMUNESIA);
	BGM_TITLE_LIST[6] = ClientStr().GetStr(STI_ELORR);	
	BGM_TITLE_LIST[7] = ClientStr().GetStr(STI_MEMBREATHEN);
	BGM_TITLE_LIST[8] = ClientStr().GetStr(STI_SHINEWOOD);
	BGM_TITLE_LIST[9] = ClientStr().GetStr(STI_HARIEL);
	BGM_TITLE_LIST[10] = ClientStr().GetStr(STI_LIMELITE);
	BGM_TITLE_LIST[11] = ClientStr().GetStr(STI_THRILGARD);
	BGM_TITLE_LIST[12] = ClientStr().GetStr(STI_KUN);
	BGM_TITLE_LIST[13] = ClientStr().GetStr(STI_WHISTLE);
	BGM_TITLE_LIST[14] = ClientStr().GetStr(STI_CIESELVA);
	BGM_TITLE_LIST[15] = ClientStr().GetStr(STI_XAILOK);
	BGM_TITLE_LIST[16] = ClientStr().GetStr(STI_EDGEOROCK);
	BGM_TITLE_LIST[17] = ClientStr().GetStr(STI_SANTUS);
	BGM_TITLE_LIST[18] = ClientStr().GetStr(STI_HELINK);
	BGM_TITLE_LIST[19] = ClientStr().GetStr(STI_GRIMROAR);
	BGM_TITLE_LIST[20] = ClientStr().GetStr(STI_SERENDO);
	BGM_TITLE_LIST[21] = ClientStr().GetStr(STI_BATTLE);
	BGM_TITLE_LIST[22] = ClientStr().GetStr(STI_WINDRILL);
	BGM_TITLE_LIST[23] = ClientStr().GetStr(STI_GORGOB);
	BGM_TITLE_LIST[24] = ClientStr().GetStr(STI_KUSKUT);
	BGM_TITLE_LIST[25] = ClientStr().GetStr(STI_ELKA);
	BGM_TITLE_LIST[26] = ClientStr().GetStr(STI_SHAMLUK);
	BGM_TITLE_LIST[27] = ClientStr().GetStr(STI_NUK);
	BGM_TITLE_LIST[28] = ClientStr().GetStr(STI_RAZMARQ);
}

AgcmUIChatting2::~AgcmUIChatting2()
{
}

BOOL AgcmUIChatting2::OnInit()
{
	m_csUIChattingEdit.m_pInputInfoTex = RwTextureRead("Chat_InputInfo.png",NULL);
	m_csUIChattingEdit.m_hKeyboardLayOut = GetKeyboardLayout(0);

	//@{ Jaewon 20051123
	// Now, you can input chat messages in the dev console.
	AS_REGISTER_TYPE_BEGIN(AgcmUIChatting2, AgcmUIChatting2);
	AS_REGISTER_METHOD1(void, ProcessChatMessage, string &in);
	AS_REGISTER_TYPE_END;
	//@} Jaewon

	OnVisibleWindowNormalChatting( FALSE );
	OnVisibleWindowSystemChatting( FALSE );
	return TRUE;
}

BOOL AgcmUIChatting2::OnDestroy()
{
	if(m_csUIChattingEdit.m_pInputInfoTex)
	{
		RwTextureDestroy(m_csUIChattingEdit.m_pInputInfoTex);
		m_csUIChattingEdit.m_pInputInfoTex = NULL;
	}

	return TRUE;
}

BOOL AgcmUIChatting2::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgpmParty			= (AgpmParty *)			GetModule("AgpmParty");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsAgcmChatting2		= (AgcmChatting2 *)		GetModule("AgcmChatting2");
	m_pcsAgcmSound			= (AgcmSound *)			GetModule("AgcmSound");
	m_pcsAgcmEventNature	= (AgcmEventNature *)	GetModule("AgcmEventNature");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmChatting2)
	{
		return FALSE;
	}

	if (!m_pcsAgcmChatting2->SetCallbackInputMessage(CBInputMessage, this))
	{
		MessageBox(NULL, GetModuleName(), "AgcmUIChatting2::OnAddModule() Error (2)", MB_OK);
		return FALSE;
	}

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
	{
		MessageBox(NULL, GetModuleName(), "AgcmUIChatting2::OnAddModule() Error (4)", MB_OK);
		return FALSE;
	}
	if (!m_pcsAgcmCharacter->SetCallbackUpdateOptionFlag(CBUpdateOptionFlag, this))
	{
		MessageBox(NULL, GetModuleName(), "AgcmUIChatting2::OnAddModule() Error (6)", MB_OK);
		return FALSE;
	}
	if (m_pcsAgcmEventNature)
	{
		if(!m_pcsAgcmEventNature->SetCallbackBGMUpdate(CBBGMUpdate, this))
			return FALSE;
	}

	m_pcsAgcmUIManager2->AddCustomControl("ChattingEdit",&m_csUIChattingEdit);

	// add user data
	m_pcsUDChattingList		= m_pcsAgcmUIManager2->AddUserData("Chat_ChattingList",
															   m_alListIndex,
															   sizeof(INT32),
															   1,
															   AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUDChattingList)
	{
		return FALSE;
	}

	m_pcsUDChattingList->m_lSelectedIndex = 0;

	// add user data
	m_pcsUDSystemMsgList	= m_pcsAgcmUIManager2->AddUserData("Chat_SystemList",
															   m_alListIndex,
															   sizeof(INT32),
															   1,
															   AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUDSystemMsgList)
	{
		return FALSE;
	}

	m_pcsUserDataComboTitle		= m_pcsAgcmUIManager2->AddUserData("Chat_ComboTitle",
															   &m_lCurrentChattingModeIndex,
															   sizeof(INT32),
															   1,
															   AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataComboTitle)
	{
		return FALSE;
	}

	m_pcsUDChattingMode			= m_pcsAgcmUIManager2->AddUserData("Chat_ChatMode",
															   m_alComboValue,
															   sizeof(INT32),
															   AGCMUICHATTING2_MAX_COMBO_BUTTON,
															   AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUDChattingMode)
	{
		return FALSE;
	}

	m_pstUDBGMTitle				= m_pcsAgcmUIManager2->AddUserData("Chat_BGMTitle",
																NULL,
																0,
																0,
																AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pstUDBGMTitle)
		return FALSE;

	// add event
	m_lEventChatInputNormalUIOpen	= m_pcsAgcmUIManager2->AddEvent("Chat_InputNormalUIOpen");
	if (m_lEventChatInputNormalUIOpen < 0)
		return FALSE;
	m_lEventGetScrollControl		= m_pcsAgcmUIManager2->AddEvent("Chat_GetScrollControl");
	if (m_lEventGetScrollControl < 0)
		return FALSE;
	m_lEventGetNormalEditControl	= m_pcsAgcmUIManager2->AddEvent("Chat_GetNormalEditControl");
	if (m_lEventGetNormalEditControl < 0)
		return FALSE;
	m_lEventGetSystemEditControl	= m_pcsAgcmUIManager2->AddEvent("Chat_GetSystemEditControl");
	if (m_lEventGetSystemEditControl < 0)
		return FALSE;
	m_lEventGetInputIDEditControl	= m_pcsAgcmUIManager2->AddEvent("Chat_GetInputIDEditControl");
	if (m_lEventGetInputIDEditControl < 0)
		return FALSE;

	m_lEventOpenInputPrivateUI		= m_pcsAgcmUIManager2->AddEvent("Chat_OpenInputPrivateUI");
	if (m_lEventOpenInputPrivateUI < 0)
		return FALSE;

	m_lEventSetFocusNormalChat		= m_pcsAgcmUIManager2->AddEvent("Chat_SetFocusNormalChat");
	if (m_lEventSetFocusNormalChat < 0)
		return FALSE;
	m_lEventKillFocusNormalChat		= m_pcsAgcmUIManager2->AddEvent("Chat_KillFocusNormalChat");
	if (m_lEventKillFocusNormalChat < 0)
		return FALSE;

	m_lEventSetFocusSystemChat		= m_pcsAgcmUIManager2->AddEvent("Chat_SetFocusSystemChat");
	if (m_lEventSetFocusSystemChat < 0)
		return FALSE;
	m_lEventKillFocusSystemChat		= m_pcsAgcmUIManager2->AddEvent("Chat_KillFocusSystemChat");
	if (m_lEventKillFocusSystemChat < 0)
		return FALSE;

	m_lEventSetFocusNormalChatOptBase		= m_pcsAgcmUIManager2->AddEvent("Chat_SetFocusNormalChatOptBase");
	if (m_lEventSetFocusNormalChatOptBase < 0)
		return FALSE;
	m_lEventKillFocusNormalChatOptBase		= m_pcsAgcmUIManager2->AddEvent("Chat_KillFocusNormalChatOptBase");
	if (m_lEventKillFocusNormalChatOptBase < 0)
		return FALSE;

	m_lEventSetFocusNormalChatOptSmallBase		= m_pcsAgcmUIManager2->AddEvent("Chat_SetFocusNormalChatOptSmallBase");
	if (m_lEventSetFocusNormalChatOptSmallBase < 0)
		return FALSE;
	m_lEventKillFocusNormalChatOptSmallBase		= m_pcsAgcmUIManager2->AddEvent("Chat_KillFocusNormalChatOptSmallBase");
	if (m_lEventKillFocusNormalChatOptSmallBase < 0)
		return FALSE;

	// add display
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Chat_Message", 0, CBDisplayChatMessage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Chat_MessageSystem", 1, CBDisplayChatMessage, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Chat_ComboTitle", 1, CBDisplayComboTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Chat_ComboList", 2, CBDisplayComboList, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Chat_BGMTitle", 0, CBDisplayBGMTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	// Add Function
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_ComboSelectChange", CBChangeChatMode, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_InputEndMessage", CBInputEndChatMessage, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_InputEndPrivateMessage", CBInputEndPrivateMessage, 1, "TargetIDControl"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_GetScrollControl", CBGetScrollControl, 1, "Scroll Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_InputStart", CBInputStart, 1, "Input Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_CheckNCloseInput", CBCheckNCloseInput, 5, "Edit Control", "Edit Control", "Edit Control", "Edit Control", "Edit Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_GetNormalEditControl", CBGetNormalChatEditControl, 1, "Normal Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_GetSystemEditControl", CBGetSystemChatEditControl, 1, "System Edit Control"))
		return FALSE;

	//if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_GetInputIDEditControl", CBGetInputIDEditControl, 2, "InputID Edit Control", "InputMessage Edit Control"))
	//	return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetNormalChat", CBSetNormalChat, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_DisableNormalChat", CBDisableNormalChat, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetGuildChat", CBSetGuildChat, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_DisableGuildChat", CBDisableGuildChat, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetPartyChat", CBSetPartyChat, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_DisablePartyChat", CBDisablePartyChat, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetWhisperChat", CBSetWhisperChat, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_DisableWhisperChat", CBDisableWhisperChat, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetAreaChat", CBSetAreaChat, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_DisableAreaChat", CBDisableAreaChat, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SendWhisper", CBSendWhisper, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetFocusNormalChat", CBSetFocusNormalChat, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_KillFocusNormalChat", CBKillFocusNormalChat, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetFocusSystemChat", CBSetFocusSystemChat, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_KillFocusSystemChat", CBKillFocusSystemChat, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetFocusNormalChatOptBase", CBSetFocusNormalChatOptBase, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_SetFocusNormalChatOptSmallBase", CBSetFocusNormalChatOptSmallBase, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_ReplyImmediate", CBReplyImmedite, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_StartNormal", CBStartChattingNormal, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_StartGuild", CBStartChattingGuild, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_StartParty", CBStartChattingParty, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_StartWhisper", CBStartChattingWhisper, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Chat_StartShout", CBStartChattingShout, 0))
		return FALSE;

	//if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsActiveWhisperMemu", CBIsActiveWhisperMemu, AGCDUI_USERDATA_TYPE_STRING))
	//	return FALSE;

	return TRUE;
}

BOOL AgcmUIChatting2::CBDisplayChatMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData || !szDisplay || !pcsSourceControl)
		return FALSE;

	AgcmUIChatting2 *	pThis		= (AgcmUIChatting2 *)	pClass;
	INT32 *				plIndex		= (INT32 *) pData;
	AgcmChattingType	lChatMode;

	if (lID == 0)
		lChatMode = (AgcmChattingType) pThis->m_lCurrentChattingModeIndex;
	else
		lChatMode = AGCMCHATTING_TYPE_SYSTEM;

	CHAR				*szMessage		= pThis->m_pcsAgcmChatting2->GetChatMessage(lChatMode, pcsSourceControl->m_lUserDataIndex);
	if (!szMessage || !szMessage[0])
		return FALSE;

	UINT32				ulColor			= pThis->m_pcsAgcmChatting2->GetChatMessageColor(lChatMode, pcsSourceControl->m_lUserDataIndex);

	sprintf(szDisplay, "%s", szMessage);

	pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor	= ulColor;

	return TRUE;
}

BOOL AgcmUIChatting2::CBDisplayComboTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !szDisplay)
		return FALSE;

	AgcmUIChatting2		*pThis			= (AgcmUIChatting2 *)	pClass;

	switch (pThis->m_lCurrentChattingModeIndex) {
	case AGCMCHATTING_TYPE_PARTY:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_PARTY));
		break;

	case AGCMCHATTING_TYPE_GUILD:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_GUILD));
		break;

	case AGCMCHATTING_TYPE_WHISPER:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_WHISPER));
		break;

	default:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_NORMAL));
		break;
	}

	return TRUE;
}

BOOL AgcmUIChatting2::CBDisplayComboList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData || !szDisplay)
		return FALSE;

	AgcmUIChatting2		*pThis			= (AgcmUIChatting2 *)	pClass;
	INT32				*plChatType		= (INT32 *)				pData;

	switch (*plChatType) {
	case AGCMCHATTING_TYPE_PARTY:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_PARTY));
		break;

	case AGCMCHATTING_TYPE_GUILD:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_GUILD));
		break;

	case AGCMCHATTING_TYPE_WHISPER:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_WHISPER));
		break;

	default:
		sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_TAB_NORMAL));
		break;
	}

	return TRUE;
}

BOOL AgcmUIChatting2::CBChangeChatMode(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIChatting2 *	pThis				= (AgcmUIChatting2 *)	pClass;
	AgcdUIControl *		pcsScrollControl	= (AgcdUIControl *)	pData1;
	INT32				lSelectIndex;

	lSelectIndex	= pThis->m_pcsUDChattingMode->m_lSelectedIndex;

	if (lSelectIndex >= 0 && lSelectIndex < AGCMUICHATTING2_MAX_COMBO_BUTTON)
		pThis->m_lCurrentChattingModeIndex	= pThis->m_alComboValue[lSelectIndex];

	if (pcsScrollControl)
		((AcUIScroll *) pcsScrollControl->m_pcsBase)->SetScrollValue(1.0f);

	pThis->m_pcsUDChattingList->m_stUserData.m_lCount	= pThis->m_pcsAgcmChatting2->GetChatMessageCount((AgcmChattingType) pThis->m_lCurrentChattingModeIndex) + 1;
	pThis->m_pcsUDChattingList->m_lStartIndex = pThis->m_pcsUDChattingList->m_stUserData.m_lCount;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUDChattingMode);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUDChattingList);

	return TRUE;
}

BOOL AgcmUIChatting2::CBInputMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2		*pThis				= (AgcmUIChatting2 *)	pClass;
	AgpdChatData *		pstChatData			= (AgpdChatData *)		pData;
	BOOL				*pbIsNormalMessage	= (BOOL *)				pCustData;

//	if (!pThis->m_pcsScrollControl)
//		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetScrollControl);

//	BOOL	bIsBottom	= FALSE;

//	if (pThis->m_pcsScrollControl)
//	{
//		FLOAT	fScrollValue	= ((AcUIScroll *) pThis->m_pcsScrollControl->m_pcsBase)->GetScrollValue();
//
//		if (fScrollValue == 1.0f)
//			bIsBottom	= TRUE;
//	}

	pThis->SetNormalChatMessage();
	pThis->SetSystemChatMessage();

	/*
	if (pbIsNormalMessage && *pbIsNormalMessage)
		pThis->SetNormalChatMessage();
	if (!pstChatData ||
		
		pstChatData->eChatType == AGPDCHATTING_TYPE_SYSTEM_LEVEL1 ||
		pstChatData->eChatType == AGPDCHATTING_TYPE_SYSTEM_LEVEL2 ||
		pstChatData->eChatType == AGPDCHATTING_TYPE_SYSTEM_LEVEL3 ||

		pstChatData->eChatType == AGPDCHATTING_TYPE_NOTICE_LEVEL1 ||
		pstChatData->eChatType == AGPDCHATTING_TYPE_NOTICE_LEVEL2 ||

		pstChatData->eChatType == AGPDCHATTING_TYPE_WHOLE_WORLD)
		pThis->SetSystemChatMessage();
	else
		pThis->SetNormalChatMessage();
	*/

//	if (!pstChatData)
//	{
//		pThis->m_pcsUDSystemMsgList->m_stUserData.m_lCount	= pThis->m_pcsAgcmChatting2->GetChatMessageCount(AGCMCHATTING_TYPE_SYSTEM) + 1;
//		pThis->m_pcsUDSystemMsgList->m_lStartIndex = pThis->m_pcsUDSystemMsgList->m_stUserData.m_lCount;
//		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUDSystemMsgList);
//	}
//	else if (pstChatData->eChatType == AGCMCHATTING_TYPE_SYSTEM)
//	{
//		pThis->m_pcsUDSystemMsgList->m_stUserData.m_lCount	= pThis->m_pcsAgcmChatting2->GetChatMessageCount(AGCMCHATTING_TYPE_SYSTEM) + 1;
//		pThis->m_pcsUDSystemMsgList->m_lStartIndex = pThis->m_pcsUDSystemMsgList->m_stUserData.m_lCount;
//		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUDSystemMsgList);
//	}
//	else
//	{
//		pThis->m_pcsUDChattingList->m_stUserData.m_lCount	= pThis->m_pcsAgcmChatting2->GetChatMessageCount((AgcmChattingType) pThis->m_lCurrentChattingModeIndex) + 1;
//		pThis->m_pcsUDChattingList->m_lStartIndex = pThis->m_pcsUDChattingList->m_stUserData.m_lCount;
//		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUDChattingList);
//	}
//
//	if (bIsBottom && pThis->m_pcsScrollControl)
//		((AcUIScroll *) pThis->m_pcsScrollControl->m_pcsBase)->SetScrollValue(1.0f);

	if (pstChatData && pstChatData->eChatType == AGPDCHATTING_TYPE_WHOLE_WORLD)
	{
		CHAR *	szMessage = new CHAR[pstChatData->lMessageLength + 1];

		memcpy(szMessage, pstChatData->szMessage, pstChatData->lMessageLength);
		szMessage[pstChatData->lMessageLength] = 0;

		pThis->m_pcsAgcmUIManager2->Notice(szMessage, -1, NULL, -1, -1, 0.09f);

		delete [] szMessage;
	}

	return TRUE;
}

//@{ Jaewon 20051123
// Now, you can input chat messages in the dev console.
bool AgcmUIChatting2::ProcessChatMessage(const std::string &message)
{
	const CHAR			*szMessage			= message.c_str();

	BOOL				bDoNotClear			= FALSE;

	// moved here from the AgcmChatting project
	if(szMessage[0] == '/')
	{
		char command[256];

		CGetArg2 arg;
		arg.SetParam((char*)szMessage);
		arg.GetParam(0, command);

		if(!strcmp(command,"/fps"))
		{
			g_pEngine->m_bDrawFPS = !g_pEngine->m_bDrawFPS;
		}
		else if(!strncmp(command,"/3dsoundtest",12))
		{
			int room_index = atoi(&command[12]);

			if(room_index >= 0 && room_index < 26)
			{
				m_pcsAgcmUIManager2->m_pcsAgcmSound->SetRoom( ( eSoundEffectRoomType )room_index);
			}
		}
		else if(!strncmp(command, "/reloadcam", 10))
		{
			AgcmCamera2*	pcmCamera		= (AgcmCamera2*)g_pEngine->GetModule("AgcmCamera2");
			AgcmCharacter*	pcmCharacter	= (AgcmCharacter*)g_pEngine->GetModule( "AgcmCharacter" );
			AgpmCharacter*	ppmCharacter	= (AgpmCharacter*)g_pEngine->GetModule( "AgpmCharacter" );

			if(!pcmCamera || !pcmCharacter || !ppmCharacter)
				return FALSE;

			AgpdCharacter*	pstAgpdCharacter	= pcmCharacter->GetSelfCharacter();

			if(!pstAgpdCharacter)
				return FALSE;

			if( pstAgpdCharacter->m_bRidable )	// 탈것에 타고 있는지 확인.
			{
				AgpdCharacter	*pstAgpdCharacterRide = ppmCharacter->GetCharacter( pstAgpdCharacter->m_nRideCID );

				if( pstAgpdCharacterRide )
				{
					pstAgpdCharacter = pstAgpdCharacterRide;
				}
			}

			AgcuCamMode::bGetInst().ReloadCamSetting();
			AgcuCamMode::bGetInst().SetCameraInfo( pcmCamera , pstAgpdCharacter );
		}
		#ifdef _DEBUG
		else if(!strcmp(command, "/time"))
		{
			if( arg.GetArgCount() >= 2 )
			{
				int	nTime = atoi( arg.GetParam( 1 ) );

				// 마고자 (2004-06-30 오전 10:47:21) : Clamp,.
				if( nTime < 0	) nTime = 0	;
				if( nTime >= 24	) nTime = 23;

				// nTime 으로 설정..
				char	str[ 256 ];
				wsprintf( str , "%d시로 시간 변경합니다~", nTime );
				m_pcsAgcmChatting2->AddSystemMessage(str);

				// 시간 변경 커맨드는 여기에..
				AgcmEventNature	* pEventNature = ( AgcmEventNature * ) m_pcsAgcmChatting2->GetModule( "AgcmEventNature" );
				if( NULL == pEventNature )
				{
					m_pcsAgcmChatting2->AddSystemMessage("EventNature 모듈이 없어서 작동이 안돼요~");
				}
				else
				{
					// 모듈에 호출함..
					AgcmEventNature::__SetTime( pEventNature , nTime , 1 );
				}
			}
			else
			{
				m_pcsAgcmChatting2->AddSystemMessage("시간을 넣어요 시간을! 후야아저씨! ex) /time 12");
			}

		}
		//@{ Jaewon 20041012
		// add a command for matd3dfx toggle.
		else if(!strcmp(command, "/matd3dfx"))
		{
			AgcmRender* pRender = (AgcmRender*)m_pcsAgcmChatting2->GetModule("AgcmRender");
			if(pRender)
			{
				if(pRender->IsMatD3DFxDisabled())
				{
					pRender->EnableMatD3DFx();
					m_pcsAgcmChatting2->AddSystemMessage("enable matd3dfx.");
				}
				else
				{
					pRender->DisableMatD3DFx();
					m_pcsAgcmChatting2->AddSystemMessage("disable matd3dfx.");
				}
			}
		}
		//. 2006. 6. 29. Nonstopdj
		//. Find 명령어 추가
		//. 중국버젼에서는 지원안함.
		else if(!stricmp(command, "/find") && arg.GetArgCount() > 1 && g_eServiceArea != AP_SERVICE_AREA_CHINA)
		{
			if(!g_AgcmUISearch)
				g_AgcmUISearch	= (AgcmUISearch *)	GetModule("AgcmUISearch");

			BOOL bParseSuccess = TRUE;

			//. 검색조건(argument 갯수) 문제있음.
			if(arg.GetArgCount() - 1 > 3)
				bParseSuccess = FALSE;

			//. /find	유저이름
			//. /find	레벨(범위)  
			//. /find	클래스
			//. 각각의 command로 검색이 가능하고 조합으로의 검색도 가능하다.
			AgpdSearch stSearch;
			g_AgcmUISearch->ClearTransferData(stSearch);

			int nCount = 0;
			while(nCount < arg.GetArgCount() - 1 && bParseSuccess)
			{
				//. 키워드 판독()
				//. 
				//. 1. 읽어들인 문자열의 첫번째값이 0~9이면 Level
				//. 2. '1'의 경우가 아닌경우 클래스 이름에 등록되어 있는 문자열이면 Class
				//. 3. 1과 2의 경우가 아닌 것은 모두 캐릭터이름으로 분류.
				nCount++;

				CHAR	szArg[256] = { 0 };
				INT32	lTID = 0;
				INT		iAsciiCode = 0;
				arg.GetParam(nCount, szArg);


				if( strlen(szArg) > AGPACHARACTER_MAX_ID_STRING)
				{
					//. 검색조건 문제있음
					bParseSuccess = FALSE;
					break;
				}

				iAsciiCode = (INT)szArg[0];
				lTID = g_AgcmUISearch->FindClassName((TCHAR*)szArg);
				//. 클래스이름에 등록된 이름인지 검사.
				//. 클래스이름 키워드
				if(lTID > 0)
				{
					stSearch.m_lTID = lTID;
					continue;
				}
				//. ascii 값 '0'~'9' : 
				//. Level 키워드.
				//. 30, 30-38 두가지방법으로 검색가능.
				else if(iAsciiCode > 0)		//. Block crt Assert.
				{
					if(isdigit(iAsciiCode))
					{
						int nIndex = 0;
						int IMinLevel = 0;
						int IMaxLevel = 0;

						string str(szArg);

						nIndex = str.find('-');
						if(nIndex > 0)
						{
							char szLevel[8];

							strncpy(szLevel, &szArg[0], nIndex);
							IMinLevel = atoi(szLevel);
							strncpy(szLevel, &szArg[nIndex+1], strlen(szArg) - nIndex);
							IMaxLevel = atoi(szLevel);

							if(  (IMinLevel > 0 && IMinLevel < 100 && IMinLevel <= IMaxLevel) && //. lv1 ~ lv99
								( IMaxLevel > 0 && IMaxLevel < 100 ))
							{
								stSearch.m_lMinLevel = IMinLevel;
								stSearch.m_lMaxLevel = IMaxLevel;

								continue;
							}
							else
								bParseSuccess = FALSE;
						}

						IMinLevel = atoi(szArg);

						if( IMinLevel > 0 && IMinLevel < 100)
						{
							stSearch.m_lMinLevel = IMinLevel;
							stSearch.m_lMaxLevel = IMinLevel;
							continue;
						}
						else
							bParseSuccess = FALSE;
					}
				}
				//. 캐릭터이름 키워드
				else if(strlen(szArg) <= AGPACHARACTER_MAX_ID_STRING)
				{
					strcpy(stSearch.m_szName, szArg);
					continue;
				}
			}

			if(bParseSuccess)
			{
				BOOL ret = FALSE;

				//. Send Search Condition Packet;
				if(strlen(stSearch.m_szName))
					ret = g_AgcmUISearch->SendSearchCondition(APGMSEARCH_PACKET_SEARCH_NAME, stSearch);
				else if(stSearch.m_lMinLevel != 0 && stSearch.m_lMaxLevel != 0 && stSearch.m_lTID == 0)
					ret = g_AgcmUISearch->SendSearchCondition(AGPMSEARCH_PACKET_SEARCH_LEVEL, stSearch);
				else if(stSearch.m_lMinLevel != 0 && stSearch.m_lMaxLevel != 0 && stSearch.m_lTID != 0)
					ret = g_AgcmUISearch->SendSearchCondition(AGPMSEARCH_PACKET_SEARCH_TID_LEVEL, stSearch);
				else if(stSearch.m_lMinLevel == 0 && stSearch.m_lMaxLevel == 0 && stSearch.m_lTID != 0)
					ret = g_AgcmUISearch->SendSearchCondition(AGPMSEARCH_PACKET_SEARCH_TID, stSearch);

				//. if packet send condtion is sucess then search list clear.
				if(ret)
					g_AgcmUISearch->SearchListClear();
			}
		}
#endif
		// add a command for postfx pipeline test.
		else if(!strcmp(command, "/postfx"))
		{
			AgcmPostFX* pPostFX = (AgcmPostFX*)m_pcsAgcmChatting2->GetModule("AgcmPostFX");
			if(NULL == pPostFX)
			{
				m_pcsAgcmChatting2->AddSystemMessage("there is no AgcmPostFX module!");
			}

			pPostFX->OnDestroy();
			pPostFX->OnInit();

			if(arg.GetArgCount() >= 2)
			{
				char buf[256];
				if(arg.GetParam(1)[0] == '-')
				{
					if(!strcmp(arg.GetParam(1), "-all"))
						// list all registered post fx techniques.
					{
						if(pPostFX->m_technics.size() == 0)
						{
							m_pcsAgcmChatting2->AddSystemMessage("no techniques are available.");
						}
						else
						{
							for(int i=0; i< ( int ) pPostFX->m_technics.size(); ++i)
							{
								m_pcsAgcmChatting2->AddSystemMessage(pPostFX->m_technics[i]->m_name);
							}
						}
					}
					else if(!strcmp(arg.GetParam(1), "-cur"))
						// print the current post fx pipeline.
					{
						if(pPostFX->m_pipeline.size() == 0)
						{
							m_pcsAgcmChatting2->AddSystemMessage("no pipeline is active.");
						}
						else
						{
							for(int i=0; i< ( int ) pPostFX->m_pipeline.size(); ++i)
							{
								if(i==0)
									strcpy(buf, pPostFX->m_technics[pPostFX->m_pipeline[i]->m_nFxIndex]->m_name);
								else
								{
									strcat(buf, "-");
									strcat(buf, pPostFX->m_technics[pPostFX->m_pipeline[i]->m_nFxIndex]->m_name);
								}
							}
							m_pcsAgcmChatting2->AddSystemMessage(buf);
						}
					}
					//@{ Jaewon 20041008
					// now can change post fx parameters.
					else if(!strcmp(arg.GetParam(1), "-p"))
					{
						if(arg.GetArgCount() < 4)
							m_pcsAgcmChatting2->AddSystemMessage("not enough arguments.");
						else
						{
							D3DXHANDLE handle = pPostFX->m_pd3dEffect->GetParameterByName(NULL, arg.GetParam(2));
							if(!handle)
							{
								sprintf(buf, "there is no parameter of the name \"%s\".", arg.GetParam(2));
								m_pcsAgcmChatting2->AddSystemMessage(buf);
							}
							else
							{
								float value = (float)atof(arg.GetParam(3));
								HRESULT hr = pPostFX->m_pd3dEffect->SetFloat(handle, value);

								if(SUCCEEDED(hr))
								{
									sprintf(buf, "set \"%s\" to %f successfully.", arg.GetParam(2), value);
									m_pcsAgcmChatting2->AddSystemMessage(buf);
								}
								else
								{
									sprintf(buf, "cannot set \"%s\" to %f.", arg.GetParam(2), value);
									m_pcsAgcmChatting2->AddSystemMessage(buf);
								}
							}
						}
					}
					//@} Jaewon
					else
					{
						sprintf(buf, "option %s is unknown.", arg.GetParam(1));
						m_pcsAgcmChatting2->AddSystemMessage(buf);
					}
				}
				else
				{
					sprintf(buf, "set the post fx pipeline to %s.", arg.GetParam(1));
					m_pcsAgcmChatting2->AddSystemMessage(buf);
					if(pPostFX->setPipeline(arg.GetParam(1))==FALSE)
					{
						sprintf(buf, "pipeline %s is invalid!", arg.GetParam(1));
						m_pcsAgcmChatting2->AddSystemMessage(buf);
					}
					else
						pPostFX->On();
				}
			}
			else
			{
				m_pcsAgcmChatting2->AddSystemMessage("turn the post fx off.");
				pPostFX->Off();
			}
		}
		//@} Jaewon
		//그외의 경우 서버로 보낸다. /create, /go가 안먹어서 추가했음. -ashulam-
		else
		{
			// 2055.05.18 By SungHoon
			//	파티 초대 메세지의 경우 파티장만 가능하고 자기 자신을 초대할수는 없다.
			BOOL bSendChatMessage = TRUE;

			if((strcmp(command, ClientStr().GetStr(STI_CHAT_INVITE_PARTY_MEMBER_ENG)) == 0 ||
				strcmp(command, ClientStr().GetStr(STI_CHAT_INVITE_PARTY_MEMBER_LOCALIZE)) == 0) && arg.GetArgCount() == 2 )
			{
				CHAR szTargetID[256] = { 0 };
				arg.GetParam(1, szTargetID);
				if (strlen(szTargetID) <= 0) bSendChatMessage = FALSE;
				else
				{
					//pThis->m_pcsAgpmCharacter->MakeGameID(szTargetID, szTargetID);

					AgpdCharacter *pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
					AgpdParty *pcsSelfParty = NULL;
					CHAR *szFailedColor = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVITE_FAILED_COLOR);
					DWORD dwColor = 0x00FF0000;

					if (pcsSelfCharacter) pcsSelfParty = m_pcsAgpmParty->GetParty(pcsSelfCharacter);
					if (pcsSelfParty) 
					{
						if(szFailedColor ) dwColor = atol(szFailedColor);
						if(m_pcsAgpmParty->GetLeaderCID(pcsSelfParty) != pcsSelfCharacter->m_lID)
						{
							CHAR *szBuffer = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVITE_FAILED_NO_LEADER);
							if (szBuffer)
							{
								m_pcsAgcmChatting2->AddSystemMessage(szBuffer, dwColor);
								m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
							}
							bSendChatMessage = FALSE;
						}
					}

					if (((m_pcsAgpmCharacter->CheckCharName(szTargetID, FALSE) == FALSE) 
						|| (!strcmp(szTargetID, pcsSelfCharacter->m_szID))) && bSendChatMessage )
					{
						CHAR *szBuffer = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_INVITE_FAILED_SELF);
						if (szBuffer)
						{
							m_pcsAgcmChatting2->AddSystemMessage(szBuffer, dwColor);
							m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
						}

						bSendChatMessage = FALSE;
					}
				}
			}
			if (CheckOptionFlag(szMessage)) bSendChatMessage = FALSE;
			if (bSendChatMessage)
			{
				m_pcsAgcmChatting2->SendChatMessage(AGPDCHATTING_TYPE_NORMAL, (char *) szMessage, strlen(szMessage), NULL);
				
				bDoNotClear	= TRUE;
			}
		}
	}
	//@} Jaewon
	else
	{
		switch (m_lCurrentChattingModeIndex) {
		case AGCMCHATTING_TYPE_PARTY:
			m_pcsAgcmChatting2->SendChatMessage(AGPDCHATTING_TYPE_PARTY, (char *) szMessage, strlen(szMessage), NULL);
			break;

		case AGCMCHATTING_TYPE_GUILD:
			m_pcsAgcmChatting2->SendChatMessage(AGPDCHATTING_TYPE_GUILD, (char *) szMessage, strlen(szMessage), NULL);
			break;

		default:
			m_pcsAgcmChatting2->SendChatMessage(AGPDCHATTING_TYPE_NORMAL, (char *) szMessage, strlen(szMessage), NULL);

			bDoNotClear	= TRUE;

			break;
		}
	}

	return bDoNotClear ? true : false;
}
//@} Jaewon

//@{ Jaewon 20051123
// Use 'ProcessChatMessage()'.
BOOL AgcmUIChatting2::CBInputEndChatMessage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIChatting2		*pThis				= (AgcmUIChatting2 *)	pClass;

	const CHAR			*szMessage			= ((AcUIEdit *) pcsSourceControl->m_pcsBase)->GetText();

	if (szMessage && szMessage[0])
	{
		if(!pThis->ProcessChatMessage(szMessage))
		{
			((AcUIEdit *) pcsSourceControl->m_pcsBase)->ClearText();
		}
		else
		{
			INT32	lParseIndex	= pThis->ParseNormalChat((CHAR *) szMessage, strlen(szMessage));
			if (lParseIndex > 0)
			{
				CHAR	szBuffer[256];
				ZeroMemory(szBuffer, sizeof(szBuffer));

				strncpy(szBuffer, szMessage, lParseIndex);

				((AcUIEdit *) pcsSourceControl->m_pcsBase)->ClearText();
				((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetText(szBuffer);
			}
			else
				((AcUIEdit *) pcsSourceControl->m_pcsBase)->ClearText();
		}
		((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetMeActiveEdit();
	}

	return TRUE;
}
//@} Jaewon

BOOL AgcmUIChatting2::CBInputEndPrivateMessage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData1)
		return FALSE;

	AgcmUIChatting2 *	pThis				= (AgcmUIChatting2 *)	pClass;
	AgcdUIControl *		pcsIDControl		= (AgcdUIControl *)		pData1;

	if (pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT || pcsIDControl->m_lType != AcUIBase::TYPE_EDIT)
		return FALSE;

	const CHAR *		szTargetID			= ((AcUIEdit *) pcsIDControl->m_pcsBase)->GetText();
	const CHAR *		szMessage			= ((AcUIEdit *) pcsSourceControl->m_pcsBase)->GetText();

	if (!szTargetID || !szTargetID[0])
		return CBInputEndChatMessage(pClass, pData1, pData2, pData3, pData4, pData5, pcsTarget, pcsSourceControl);

	if (szMessage && szMessage[0])
	{
		pThis->m_pcsAgcmChatting2->SendChatMessage(AGPDCHATTING_TYPE_WHISPER, (CHAR *) szMessage, strlen(szMessage), (CHAR *) szTargetID);

		((AcUIEdit *) pcsSourceControl->m_pcsBase)->ClearText();
		((AcUIEdit *) pcsSourceControl->m_pcsBase)->SetMeActiveEdit();
	}

	return TRUE;
}

BOOL AgcmUIChatting2::CBGetScrollControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChatting2		*pThis			= (AgcmUIChatting2 *)	pClass;
	AgcdUIControl		*pcsScrollControl	= (AgcdUIControl *)	pData1;

	pThis->m_pcsScrollControl	= pcsScrollControl;

	((AcUIScroll *) pcsScrollControl->m_pcsBase)->SetScrollValue(1.0f);

	return TRUE;
}

BOOL AgcmUIChatting2::CBInputStart(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChatting2		*pThis			= (AgcmUIChatting2 *)	pClass;
	AgcdUIControl		*pcsInputControl	= (AgcdUIControl *)	pData1;

	if (pcsInputControl->m_lType != AcUIBase::TYPE_EDIT)
		return FALSE;

	((AcUIEdit *) pcsInputControl->m_pcsBase)->SetMeActiveEdit();

	return TRUE;
}

BOOL AgcmUIChatting2::CBCheckNCloseInput(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2		*pThis			= (AgcmUIChatting2 *)	pClass;

	if (pData1)
	{
		if (((AcUIEdit *) ((AgcdUIControl *) pData1)->m_pcsBase)->GetActiveEdit())
			return FALSE;
	}

	if (pData2)
	{
		if (((AcUIEdit *) ((AgcdUIControl *) pData2)->m_pcsBase)->GetActiveEdit())
			return FALSE;
	}

	if (pData3)
	{
		if (((AcUIEdit *) ((AgcdUIControl *) pData3)->m_pcsBase)->GetActiveEdit())
			return FALSE;
	}

	if (pData4)
	{
		if (((AcUIEdit *) ((AgcdUIControl *) pData4)->m_pcsBase)->GetActiveEdit())
			return FALSE;
	}

	if (pData5)
	{
		if (((AcUIEdit *) ((AgcdUIControl *) pData5)->m_pcsBase)->GetActiveEdit())
			return FALSE;
	}

	if (pcsSourceControl)
		pThis->m_pcsAgcmUIManager2->CloseUI(pcsSourceControl->m_pcsParentUI);

	return TRUE;
}

BOOL AgcmUIChatting2::CBGetNormalChatEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChatting2		*pThis					= (AgcmUIChatting2 *)	pClass;

	pThis->m_pcsNormalEditControl	= (AgcdUIControl *)		pData1;

	AcUIEdit* pEdit = ( AcUIEdit* )pThis->m_pcsNormalEditControl->m_pcsBase;
	//pEdit->m_nOffSetFromTopEdge = 10;

	pThis->OnVisibleWindowNormalChatting( FALSE );

	return TRUE;
}

BOOL AgcmUIChatting2::CBGetSystemChatEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChatting2		*pThis					= (AgcmUIChatting2 *)	pClass;

	pThis->m_pcsSystemEditControl	= (AgcdUIControl *)		pData1;

	AcUIEdit* pEdit = ( AcUIEdit* )pThis->m_pcsSystemEditControl->m_pcsBase;
	//pEdit->m_nOffSetFromTopEdge = 10;

	pThis->OnVisibleWindowSystemChatting( FALSE );	

	return TRUE;
}

//BOOL AgcmUIChatting2::CBGetInputIDEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pData1 || !pData2)
//		return FALSE;
//
//	AgcmUIChatting2		*pThis					= (AgcmUIChatting2 *)	pClass;
//
//	pThis->m_pcsInputIDEditControl	= (AgcdUIControl *)		pData1;
//	pThis->m_pcsInputMsgEditControl	= (AgcdUIControl *)		pData2;
//
//	return TRUE;
//}

BOOL AgcmUIChatting2::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetNormalEditControl);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetSystemEditControl);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetInputIDEditControl);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusNormalChat, 0, TRUE);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusSystemChat, 0, TRUE);

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetFocusNormalChatOptBase, 0, TRUE);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusNormalChatOptSmallBase, 0, TRUE);

	return TRUE;
}

BOOL AgcmUIChatting2::SetNormalChatMessage()
{
	if (!m_pcsNormalEditControl || !m_pcsNormalEditControl->m_pcsBase)
		return FALSE;

	AcUIEdit	*pcsEdit	= (AcUIEdit *) m_pcsNormalEditControl->m_pcsBase;

	pcsEdit->SetLineDelimiter("\n");

	CHAR	szBuffer[18000];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	INT32	lMessageCount	= m_pcsAgcmChatting2->GetChatMessageCount(AGCMCHATTING_TYPE_NORMAL);

	for (int i = 0; i < lMessageCount; ++i)
	{
		CHAR	*szMessage			= m_pcsAgcmChatting2->GetChatMessage(AGCMCHATTING_TYPE_NORMAL, i);
		if (!szMessage || !szMessage[0])
			continue;

		if (strlen(szMessage) + strlen(szBuffer) >= 18000)
			break;

		UINT32	ulColor				= m_pcsAgcmChatting2->GetChatMessageColor(AGCMCHATTING_TYPE_NORMAL, i);
		UINT32	ulBGColor			= m_pcsAgcmChatting2->GetChatBackgroundColor(AGCMCHATTING_TYPE_NORMAL, i);
		BOOL	bBold				= m_pcsAgcmChatting2->GetChatMessageBold(AGCMCHATTING_TYPE_NORMAL, i);
		BOOL	bApplyColorAllText	= m_pcsAgcmChatting2->GetChatMessageApplyColorAllText(AGCMCHATTING_TYPE_NORMAL, i);

		if (!bApplyColorAllText &&
			strncmp(szMessage,
				m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_NOTICE_HEADER),
				strlen(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_NOTICE_HEADER))) != 0)
		{
			int j;

			for (j = 0; j < ( int ) strlen(szMessage); ++j)
			{
				if (szMessage[j] == ':')
				{
					CHAR	szHeader[129];
					ZeroMemory(szHeader, sizeof(szHeader));

					if (j < 128)
						CopyMemory(szHeader, szMessage, sizeof(CHAR) * (j + 1));

					sprintf(szBuffer + strlen(szBuffer),
						//"<C%dO%d%s>%s<C%d>%s%s<O0>\n", 0xffff9900, ulBGColor, bBold ? "B":"", szHeader, ulColor, szMessage + j + 1, bBold ? "<b>":"");
						"<C%d%s>%s%s%s<O0>\n", ulColor, bBold ? "B":"", szHeader, szMessage + j + 1, bBold ? "<b>":"");

					break;
				}
			}

			if (j == strlen(szMessage))
				sprintf(szBuffer + strlen(szBuffer), "<C%d>%s\n", ulColor, szMessage);
		}
		else
		{
			sprintf(szBuffer + strlen(szBuffer), "<C%d>%s\n", ulColor, szMessage);
		}
	}

	return pcsEdit->SetText(szBuffer , TRUE );
}

BOOL AgcmUIChatting2::SetSystemChatMessage()
{
	if (!m_pcsSystemEditControl || !m_pcsSystemEditControl->m_pcsBase)
		return FALSE;

	AcUIEdit	*pcsEdit	= (AcUIEdit *) m_pcsSystemEditControl->m_pcsBase;

	pcsEdit->SetLineDelimiter("\n");

	CHAR	szBuffer[10240];
	ZeroMemory(szBuffer, sizeof(CHAR) * 10240);

	INT32	lMessageCount	= m_pcsAgcmChatting2->GetChatMessageCount(AGCMCHATTING_TYPE_SYSTEM);

	for (int i = 0; i < lMessageCount; ++i)
	{
		CHAR	*szMessage		= m_pcsAgcmChatting2->GetChatMessage(AGCMCHATTING_TYPE_SYSTEM, i);
		if (!szMessage || !szMessage[0])
			continue;

		if (strlen(szMessage) + strlen(szBuffer) >= 10240)
			break;

		UINT32	ulColor			= m_pcsAgcmChatting2->GetChatMessageColor(AGCMCHATTING_TYPE_SYSTEM, i);

		sprintf(szBuffer + strlen(szBuffer), "<C%d>%s\n", ulColor, szMessage);
	}

	return pcsEdit->SetText(szBuffer);
}

BOOL AgcmUIChatting2::CBSetNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->ActiveChatMode(AGCMCHATTING_TYPE_NORMAL);
}

BOOL AgcmUIChatting2::CBDisableNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->DisableChatMode(AGCMCHATTING_TYPE_NORMAL);
}

BOOL AgcmUIChatting2::CBSetGuildChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->ActiveChatMode(AGCMCHATTING_TYPE_GUILD);
}

BOOL AgcmUIChatting2::CBDisableGuildChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->DisableChatMode(AGCMCHATTING_TYPE_GUILD);
}

BOOL AgcmUIChatting2::CBSetPartyChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->ActiveChatMode(AGCMCHATTING_TYPE_PARTY);
}

BOOL AgcmUIChatting2::CBDisablePartyChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->DisableChatMode(AGCMCHATTING_TYPE_PARTY);
}

BOOL AgcmUIChatting2::CBSetWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->ActiveChatMode(AGCMCHATTING_TYPE_WHISPER);
}

BOOL AgcmUIChatting2::CBDisableWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->DisableChatMode(AGCMCHATTING_TYPE_WHISPER);
}

BOOL AgcmUIChatting2::CBSetAreaChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->ActiveChatMode(AGCMCHATTING_TYPE_AREA);
}

BOOL AgcmUIChatting2::CBDisableAreaChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	return pThis->m_pcsAgcmChatting2->DisableChatMode(AGCMCHATTING_TYPE_AREA);
}

//BOOL AgcmUIChatting2::CBIsActiveWhisperMemu(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
//{
//	if (!pClass || !pData)
//		return FALSE;
//
//	AgcmUIChatting2			*pThis				= (AgcmUIChatting2 *)	pClass;
//	AgpdCharacter			*pcsTargetCharacter	= (AgpdCharacter *)		pData;
//
//	// pcsTargetCharacter가 PC인지 살펴본다.
//	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsTargetCharacter))
//		return FALSE;
//
//	return TRUE;
//}

BOOL AgcmUIChatting2::CBSendWhisper(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2			*pThis				= (AgcmUIChatting2 *)	pClass;

	if (pcsSourceControl &&
		pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl))
	{
		AgcdUIUserData *	pcsUserData = pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);
		if (pcsUserData->m_eType == AGCDUI_USERDATA_TYPE_CHARACTER)
		{
			AgpdCharacter *	pcsTargetCharacter = (AgpdCharacter *)		pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;
			if (pcsTargetCharacter)
			{
				pThis->SetWhisperTo(pcsTargetCharacter->m_szID);
				return TRUE;
			}
		}
	}

	if (pData1 && ((ApBase *) pData1)->m_eType == APBASE_TYPE_UI_CONTROL)
	{
		AgcdUIControl *	pcsControl = (AgcdUIControl *) pData1;

		if (pcsControl->m_pcsBase->m_nType == AcUIBase::TYPE_EDIT)
		{
			const CHAR *	szID = ((AcUIEdit *) pcsControl->m_pcsBase)->GetText();
			if (szID && strlen(szID) > 0)
			{
				pThis->SetWhisperTo((CHAR *) szID);
				return TRUE;
			}
		}
	}

	return TRUE;
}

BOOL AgcmUIChatting2::CBSetFocusNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	if (!pThis->m_bIsSetOptBase)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetFocusNormalChatOptBase, 0, TRUE);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusNormalChatOptSmallBase, 0, TRUE);
	}
	else
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetFocusNormalChatOptSmallBase, 0, TRUE);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusNormalChatOptBase, 0, TRUE);
	}
	
	pThis->OnVisibleWindowNormalChatting( TRUE );
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetFocusNormalChat, 0, TRUE);
}

BOOL AgcmUIChatting2::CBKillFocusNormalChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	pThis->OnVisibleWindowNormalChatting( FALSE );
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusNormalChat, 0, TRUE);
}

BOOL AgcmUIChatting2::CBSetFocusSystemChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	pThis->OnVisibleWindowSystemChatting( TRUE );
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetFocusSystemChat, 0, TRUE);
}

BOOL AgcmUIChatting2::CBKillFocusSystemChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	pThis->OnVisibleWindowSystemChatting( FALSE );
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusSystemChat, 0, TRUE);
}

BOOL AgcmUIChatting2::CBSetFocusNormalChatOptBase(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	pThis->m_bIsSetOptBase	= TRUE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetFocusNormalChatOptSmallBase, 0, TRUE);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusNormalChatOptBase, 0, TRUE);

	return TRUE;
}

BOOL AgcmUIChatting2::CBSetFocusNormalChatOptSmallBase(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIChatting2	*pThis	= (AgcmUIChatting2 *)	pClass;

	pThis->m_bIsSetOptBase	= FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSetFocusNormalChatOptBase, 0, TRUE);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventKillFocusNormalChatOptSmallBase, 0, TRUE);

	return TRUE;
}

INT32 AgcmUIChatting2::ParseNormalChat(CHAR *szMessage, INT32 lLength)
{
	if (!szMessage || !szMessage[0] || lLength < 1)
		return 0;

	if (ClientStr().GetStr(STI_CHAT_WHISPER_SPECIAL) &&
		szMessage[0] == ClientStr().GetStr(STI_CHAT_WHISPER_SPECIAL)[0])
	{
		for (int i = 2; i < lLength; ++i)
		{
			if (szMessage[i] == ' ')
				return i + 1;
		}
	}
	else if (ClientStr().GetStr(STI_CHAT_SEND_GUILD_SPECIAL) && szMessage[0] == ClientStr().GetStr(STI_CHAT_SEND_GUILD_SPECIAL)[0] ||
		ClientStr().GetStr(STI_CHAT_SEND_PARTY_SPECIAL) && szMessage[0] == ClientStr().GetStr(STI_CHAT_SEND_PARTY_SPECIAL)[0])
		return 1;
	else if (szMessage[0] == '/')
	{
		if ((strncmp(szMessage, ClientStr().GetStr(STI_CHAT_WHISPER_LOWER), strlen(ClientStr().GetStr(STI_CHAT_WHISPER_LOWER))) == 0 ||
			 strncmp(szMessage, ClientStr().GetStr(STI_CHAT_WHISPER_UPPER), strlen(ClientStr().GetStr(STI_CHAT_WHISPER_UPPER))) == 0) &&
			szMessage[strlen(ClientStr().GetStr(STI_CHAT_WHISPER_LOWER))] == ' ')
		{
			for (int i = strlen(ClientStr().GetStr(STI_CHAT_WHISPER_LOWER)) + 1; i < lLength; ++i)
			{
				if (szMessage[i] == ' ')
					return i + 1;
			}
		}
		else if ((strncmp(szMessage, ClientStr().GetStr(STI_CHAT_WHISPER_LOCALIZE), strlen(ClientStr().GetStr(STI_CHAT_WHISPER_LOCALIZE))) == 0) &&
			szMessage[strlen(ClientStr().GetStr(STI_CHAT_WHISPER_LOCALIZE))] == ' ')
		{
			for (int i = strlen(ClientStr().GetStr(STI_CHAT_WHISPER_LOCALIZE)) + 1; i < lLength; ++i)
			{
				if (szMessage[i] == ' ')
					return i + 1;
			}
		}
		else if ((strncmp(szMessage, ClientStr().GetStr(STI_CHAT_SEND_GUILD_LOWER), strlen(ClientStr().GetStr(STI_CHAT_SEND_GUILD_LOWER))) == 0 ||
			 strncmp(szMessage, ClientStr().GetStr(STI_CHAT_SEND_GUILD_UPPER), strlen(ClientStr().GetStr(STI_CHAT_SEND_GUILD_UPPER))) == 0 ||
			 strncmp(szMessage, ClientStr().GetStr(STI_CHAT_SEND_PARTY_LOWER), strlen(ClientStr().GetStr(STI_CHAT_SEND_PARTY_LOWER))) == 0 ||
			 strncmp(szMessage, ClientStr().GetStr(STI_CHAT_SEND_PARTY_UPPER), strlen(ClientStr().GetStr(STI_CHAT_SEND_PARTY_UPPER))) == 0) &&
			szMessage[strlen(ClientStr().GetStr(STI_CHAT_WHISPER_LOWER))] == ' ')
		{
			return strlen(ClientStr().GetStr(STI_CHAT_SEND_GUILD_LOWER)) + 1;
		}
		else if ((strncmp(szMessage, ClientStr().GetStr(STI_CHAT_SEND_GUILD_LOCALIZE), strlen(ClientStr().GetStr(STI_CHAT_SEND_GUILD_LOCALIZE))) == 0 ||
			 strncmp(szMessage, ClientStr().GetStr(STI_CHAT_SEND_PARTY_LOCALIZE), strlen(ClientStr().GetStr(STI_CHAT_SEND_PARTY_LOCALIZE))) == 0) &&
			szMessage[strlen(ClientStr().GetStr(STI_CHAT_SEND_GUILD_LOCALIZE))] == ' ')
		{
			return strlen(ClientStr().GetStr(STI_CHAT_SEND_GUILD_LOCALIZE)) + 1;
		}
	}

	return 0;
}

// 2005.03.10. steeple
// BGM Title 을 뿌려준다.
BOOL AgcmUIChatting2::CBDisplayBGMTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	// 2005.03.11. steeple
	// 안쓰고, 시스템 메시지로 뿌리게끔 변경
	return TRUE;

	//if(!pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
	//	return FALSE;

	//if(pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT)
	//	return FALSE;

	//AgcmUIChatting2* pThis = (AgcmUIChatting2*)pClass;

	//CHAR szBuffer[255 + 16 + 1];
	//if(pThis->m_szNowBGM)
	//	sprintf(szBuffer, "BGM : %s", pThis->m_szNowBGM);
	//else
	//	sprintf(szBuffer, "");

	//((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);

	//return TRUE;
}

// 2005.03.10. steeple
// BGM Title 을 뿌려주기 위해서
BOOL AgcmUIChatting2::CBBGMUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	CHAR* szBGMFileName = (CHAR*)pData;
	AgcmUIChatting2* pThis = (AgcmUIChatting2*)pClass;

	if(strlen(szBGMFileName) == 0)
		return FALSE;

	// BGM 플레이 하지 않는 상태이면 걍 나간다.
	if(!pThis->m_pcsAgcmSound || pThis->m_pcsAgcmSound->GetBGMVolume() == 0.0f)
		return TRUE;

	pThis->m_szNowBGM = (CHAR*)pThis->GetBGMTitleByFileName(szBGMFileName);
	if(!pThis->m_szNowBGM)
		return TRUE;

	static	INT32 lFont	= 0;
	static	INT32 lOffsetX = 5;
	static	INT32 lOffsetY = 20;
	static	INT32 lDuration	= 10000;

	static	FLOAT fScale	= 1.0f;

	static	UINT32 uColor	= 0xFFFFFFFF;

	CHAR szMessage[255 + 4 + 1];
	if (g_eServiceArea == AP_SERVICE_AREA_KOREA)
		sprintf(szMessage, "♬ %s", pThis->m_szNowBGM);
	else
		sprintf(szMessage, "BGM : %s", pThis->m_szNowBGM);

	DWORD dwColor = 0xFFC0C0C0;

	pThis->m_pcsAgcmUIManager2->m_csEffectBGMWindow.SetEffectSubText(
		szMessage	,
		lFont		,
		lOffsetX	,
		lOffsetY	,
		FALSE		,
		uColor		,
		fScale		,
		lDuration	, 
		TRUE		);

	//pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
	//pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
	
	//pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDBGMTitle);

	return TRUE;
}

// 2005.03.10. steeple
// BGM Title 은 이 파일 맨 위에 하드코딩(!!!!) 되어 있다.
const CHAR*	AgcmUIChatting2::GetBGMTitleByFileName(CHAR* szBGMFileName)
{
	if(!szBGMFileName || strlen(szBGMFileName) == 0)
		return NULL;

	const CHAR* szBGMTitle = NULL;

	INT32 i = 0;
	for(i; i < AGCMUICHATTING2_MAX_BGM_COUNT; i++)
	{
		if(_stricmp(BGM_FILE_LIST[i], szBGMFileName) == 0)
		{
			szBGMTitle = BGM_TITLE_LIST[i];
			break;
		}
	}

	return szBGMTitle;
}

/*
	2005.05.31.
	거부 목록인지 확인한다.
*/
BOOL AgcmUIChatting2::CheckOptionFlag( const CHAR *szMessage )
{
	char szCommand[256];

	AgpdCharacter *pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (strlen(szMessage) >= 255 || !pcsSelfCharacter) return FALSE;

	size_t i;
	for (i = 0 ; i < strlen(szMessage);i++)
		szCommand[i] = tolower(szMessage[i]);
	szCommand[i] = '\0';

	INT32	lOptionFlag = 0;
	BOOL	bSetMode = TRUE;

	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_TRADEOFF_ENG)) == 0 ||		// 거래 거부
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_TRADEOFF_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_TRADE;
	}

	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_TRADEON_ENG)) == 0 ||		// 거래 가능
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_TRADEON_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_TRADE;
		bSetMode = FALSE;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_PARTYOFF_ENG)) == 0 ||		//파티 거부
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_PARTYOFF_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_PARTY_IN;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_PARTYON_ENG)) == 0 ||		//파티 가능
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_PARTYON_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_PARTY_IN;
		bSetMode = FALSE;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDBATTLEOFF_ENG)) == 0 ||		//길드전 거부
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDBATTLEOFF_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDBATTLEON_ENG)) == 0 ||		// 길드전 가능
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDBATTLEON_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE;
		bSetMode = FALSE;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_BATTLEOFF_ENG)) == 0 ||		//결투 거부
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_BATTLEOFF_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_BATTLE;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_BATTLEON_ENG)) == 0 ||		//결투 가능
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_BATTLEON_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_BATTLE;
		bSetMode = FALSE;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDINOFF_ENG)) == 0 ||		// 길드 초대 거부
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDINOFF_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_GUILD_IN;
	}
	if((strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDINON_ENG)) == 0 ||		// 길드 초대 가능
			strcmp(szCommand, ClientStr().GetStr(STI_CHAT_GUILDINON_LOCALIZE)) == 0))
	{
		lOptionFlag = AGPDCHAR_OPTION_REFUSE_GUILD_IN;
		bSetMode = FALSE;
	}

	if (lOptionFlag)
	{
		if(bSetMode)
			lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(pcsSelfCharacter, (AgpdCharacterOptionFlag)lOptionFlag);
		else
			lOptionFlag = m_pcsAgpmCharacter->UnsetOptionFlag(pcsSelfCharacter, (AgpdCharacterOptionFlag)lOptionFlag);
		if (lOptionFlag == pcsSelfCharacter->m_lOptionFlag)			//	 현재 적용된 거부 목록이면	2005.06.13 By SungHoon
		{
			DWORD dwColor = 0x00FF0000;
			CHAR *szColor = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
			if (szColor) dwColor = atol(szColor);
			CHAR *szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAR_OPTION_ALEADY_SET);
			if (szMessage)
			{
				m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
				m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
			}
		}
		else m_pcsAgcmCharacter->SendPacketOptionFlag(pcsSelfCharacter, lOptionFlag);
		return TRUE;
	}
	return FALSE;
}

BOOL AgcmUIChatting2::SetChattingMessage(const TCHAR *szMessage)
{
	m_csUIChattingEdit.SetText(const_cast<TCHAR*>(szMessage));
	//m_csUIChattingEdit.SetMeActiveEdit();
	return m_pcsAgcmUIManager2->ThrowEvent(m_lEventSetFocusNormalChat);
}

void AgcmUIChatting2::ChatInputNormalUIOpen()
{
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventChatInputNormalUIOpen);
}

/*
	2005.06.01 By SungHoon
	거부 마스크 변경이 되었을 경우 UI창에 뿌려준다.
*/
BOOL AgcmUIChatting2::CBUpdateOptionFlag( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIChatting2		*pThis				= (AgcmUIChatting2 *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	INT32				lOptionFlag			= *(INT32		 *)		pCustData;
	AgpmCharacter		*pcsAgpmCharacter	= pThis->m_pcsAgpmCharacter;

	if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_TRADE) 
		!= pcsAgpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_TRADE))
	{
		CHAR *szMessage = NULL;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
		DWORD dwColor = 0x00FF0000;

		if(pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_TRADE))
		{
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_TRADE_OFF);
			szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
		}
		else szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_TRADE_ON);
		if (szMessage)
		{
			if(szColor) dwColor = atol(szColor);

			pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
			pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
		}
	}
	if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_PARTY_IN) 
		!= pcsAgpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_PARTY_IN))
	{
		CHAR *szMessage = NULL;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
		DWORD dwColor = 0x00FF0000;

		if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_PARTY_IN))
		{
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_PARTY_OFF);
			szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
		}
		else szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_PARTY_ON);
		if (szMessage)
		{
			if(szColor) dwColor = atol(szColor);

			pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
			pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
		}
	}
	if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE) 
		!= pcsAgpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE))
	{
		CHAR *szMessage = NULL;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
		DWORD dwColor = 0x00FF0000;

		if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE))
		{
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_GBATTLE_OFF);
			szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
		}
		else szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_GBATTLE_ON);
		if (szMessage)
		{
			if(szColor) dwColor = atol(szColor);

			pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
			pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
		}
	}
	if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_BATTLE) 
		!= pcsAgpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_BATTLE))
	{
		CHAR *szMessage = NULL;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
		DWORD dwColor = 0x00FF0000;

		if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_BATTLE))
		{
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_BATTLE_OFF);
			szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
		}
		else szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_BATTLE_ON);
		if (szMessage)
		{
			if(szColor) dwColor = atol(szColor);

			pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
			pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
		}
	}
	if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_GUILD_IN) 
		!= pcsAgpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN))
	{
		CHAR *szMessage = NULL;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
		DWORD dwColor = 0x00FF0000;

		if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_GUILD_IN))
		{
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_GUILD_OFF);
			szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
		}
		else szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_GUILD_ON);
		if (szMessage)
		{
			if(szColor) dwColor = atol(szColor);

			pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
			pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
		}
	}

	// 2007.07.12. steeple
	if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_GUILD_RELATION) 
		!= pcsAgpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION))
	{
		CHAR *szMessage = NULL;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
		DWORD dwColor = 0x00FF0000;

		if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_GUILD_RELATION))
		{
			szMessage = ClientStr().GetStr(STI_REFUSE_GUILD_RELATION_ON);
			szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
		}
		else 
			szMessage = ClientStr().GetStr(STI_REFUSE_GUILD_RELATION_OFF);

		if (szMessage)
		{
			if(szColor) dwColor = atol(szColor);

			pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
			pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
		}
	}

	if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag ,AGPDCHAR_OPTION_REFUSE_BUDDY) 
		!= pcsAgpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_BUDDY))
	{
		CHAR *szMessage = NULL;
		CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
		DWORD dwColor = 0x00FF0000;

		if (pcsAgpmCharacter->IsOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_BUDDY))
		{
			szMessage = ClientStr().GetStr(STI_REFUSE_BUDDY_ON);
			szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
		}
		else 
			szMessage = ClientStr().GetStr(STI_REFUSE_BUDDY_OFF);

		if (szMessage)
		{
			if(szColor) dwColor = atol(szColor);

			pThis->m_pcsAgcmChatting2->AddSystemMessage(szMessage, dwColor);
			pThis->m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
		}
	}

	return TRUE;
}

BOOL AgcmUIChatting2::CBReplyImmedite(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIChatting2		*pThis				= (AgcmUIChatting2 *)	pClass;

	pThis->SetWhisperTo(pThis->m_pcsAgcmChatting2->GetLastWhisperCharacter());

	return TRUE;
}

BOOL AgcmUIChatting2::CBStartChattingNormal( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting2* pThis = ( AgcmUIChatting2* )pClass;
	if( !pThis ) return FALSE;

	pThis->ChatInputNormalUIOpen();
	return pThis->SetChattingMessage( "" );
}

BOOL AgcmUIChatting2::CBStartChattingGuild( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting2* pThis = ( AgcmUIChatting2* )pClass;
	if( !pThis ) return FALSE;

	CHAR strCommand[ 64 ] = { 0, };
	sprintf( strCommand, "%s ", ClientStr().GetStr( STI_CHAT_SEND_GUILD_LOWER ) );

	pThis->ChatInputNormalUIOpen();
	return pThis->SetChattingMessage( strCommand );
}

BOOL AgcmUIChatting2::CBStartChattingParty( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting2* pThis = ( AgcmUIChatting2* )pClass;
	if( !pThis ) return FALSE;

	CHAR strCommand[ 64 ] = { 0, };
	sprintf( strCommand, "%s ", ClientStr().GetStr( STI_CHAT_SEND_PARTY_LOWER ) );

	pThis->ChatInputNormalUIOpen();
	return pThis->SetChattingMessage( strCommand );
}

BOOL AgcmUIChatting2::CBStartChattingWhisper( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting2* pThis = ( AgcmUIChatting2* )pClass;
	if( !pThis ) return FALSE;

	CHAR strCommand[ 64 ] = { 0, };
	sprintf( strCommand, "%s ", ClientStr().GetStr( STI_CHAT_WHISPER_LOWER ) );

	pThis->ChatInputNormalUIOpen();
	return pThis->SetChattingMessage( strCommand );
}

BOOL AgcmUIChatting2::CBStartChattingShout( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIChatting2* pThis = ( AgcmUIChatting2* )pClass;
	if( !pThis ) return FALSE;

	CHAR strCommand[ 64 ] = { 0, };
	sprintf( strCommand, "%s ", ClientStr().GetStr( STI_CHAT_WORD_BALLOON_LOWER ) );

	pThis->ChatInputNormalUIOpen();
	return pThis->SetChattingMessage( strCommand );
}

VOID AgcmUIChatting2::SetWhisperTo(CHAR *szName)
{
	if (strlen(szName) > 64)
		return;

	CHAR				szReply[128];
	ZeroMemory(szReply, sizeof(szReply));

	sprintf(szReply, "%s %s ", ClientStr().GetStr(STI_CHAT_WHISPER_LOWER), szName);

	SetChattingMessage(szReply);

	ChatInputNormalUIOpen();
}

void AgcmUIChatting2::OnVisibleWindowNormalChatting( BOOL bIsVisible )
{
	if( !m_pcsNormalEditControl ) return;
	m_pcsNormalEditControl->m_pcsBase->SetStatus( bIsVisible ? 0 : 1 );
}

void AgcmUIChatting2::OnVisibleWindowSystemChatting( BOOL bIsVisible )
{
	if( !m_pcsSystemEditControl ) return;
	m_pcsSystemEditControl->m_pcsBase->SetStatus( bIsVisible ? 0 : 1 );
}
