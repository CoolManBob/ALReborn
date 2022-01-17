#ifndef __AGCMUIPARTYOPTION_H__
#define __AGCMUIPARTYOPTION_H__

#include "AuPacket.h"

#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmParty.h"
#include "AgcmParty.h"
#include "AgcmChatting2.h"
#include "AgcmUIManager2.h"
#include "AgcmUIItem.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIPartyOptionD" )
#else
#pragma comment ( lib , "AgcmUIPartyOption" )
#endif
#endif


typedef struct _AgcmUIPartyOptionGetItemPartyMemberMessageData {
	INT32		lTID;
	INT32       lCID;
	INT32       lCount;
	INT8		cNumPhysicalConvert;
	INT8		cNumSocket;
	INT8		cNumConvertedSocket;
	INT8		cOptionCount;
} AgcmUIPartyOptionGetItemPartyMemberMessageData;


//	Begin 2005.04.13 by SungHoon
//	Party Option UserData Name
#define	AGCMUI_PARTYOPTION_FUNCTION_APLY					"PartyOptionUI_Aply"
#define	AGCMUI_PARTYOPTION_FUNCTION_INVITE_MEMBER			"PartyOptionUI_InviteMember"
#define	AGCMUI_PARTYOPTION_FUNCTION_OPEN_UI					"PartyOptionUI_OpenUI"
#define	AGCMUI_PARTYOPTION_FUNCTION_OPEN_UI_CLICK			"PartyOptionUI_OpenUI_Click"
//	Finish 2005.04.13 by SungHoon


//	Begin 2005.04.13 by SungHoon
//	Party Option UserData Name

#define AGCMUI_PARTYOPTION_DIVISION_ITEM					"PartyOptionUI_DivisionItem"
#define AGCMUI_PARTYOPTION_DIVISION_EXP						"PartyOptionUI_DivisionExp"

#define AGCMUI_PARTYOPTION_USERDATA_ITEM_GET_PARTY_MEMBER					"GetItemPartyMemeber"

#define AGCMUI_PARTYOPTION_USERDATA_ENABLE_DIVISION_ITEM					"PO_EnableDivisionItem"
#define AGCMUI_PARTYOPTION_USERDATA_ENABLE_DIVISION_EXP						"PO_EnableDivisionExp"
#define AGCMUI_PARTYOPTION_USERDATA_ENABLE_OPTION_APLY						"PO_EnableAply"
#define AGCMUI_PARTYOPTION_USERDATA_ENABLE_INVITE_MEMBER					"PO_EnableInviteMember"

#define	AGCMUI_PARTYOPTION_EVENT_RECEIVE_INVITE_NO_LOGIN_MEMBER				"PartyOptionUI_RecvInviteNoLoginMember"	// 2005.04.21. By SungHoon
#define	AGCMUI_PARTYOPTION_EVENT_RECEIVE_GET_ITEM_MEMBER					"PartyOptionUI_GetItemMember"			// 2005.04.21. By SungHoon
#define	AGCMUI_PARTYOPTION_EVENT_RECEIVE_DIVISION_ITEM_UPDATE				"PartyOptionUI_DivisionItemUpdate"		// 2005.04.21. By SungHoon
#define	AGCMUI_PARTYOPTION_EVENT_OPEN_UI									"PartyOptionUI_Open"		// 2005.04.21. By SungHoon
#define	AGCMUI_PARTYOPTION_EVENT_OPEN_UI_CLICK								"PartyOptionUI_Open_Click"		// 2005.04.21. By SungHoon
#define	AGCMUI_PARTYOPTION_EVENT_RECEIVE_DIVISION_EXP_UPDATE				"PartyOptionUI_DivisionExpUpdate"		// 2005.05.17. By SungHoon


#define	AGCMUI_PARTYOPTION_DIVISION_ITEM_DAMAGE				0		// ±â¿©µµ È¹µæ
#define	AGCMUI_PARTYOPTION_DIVISION_ITEM_SEQUENCE			1		// ¼øÂ÷Àû È¹µæ
#define	AGCMUI_PARTYOPTION_DIVISION_ITEM_FREE				2		// ÀÚÀ¯È¹µæ

#define	AGCMUI_PARTYOPTION_DIVISION_EXP_DAMAGE				0		// ±â¿©µµ È¹µæ
#define	AGCMUI_PARTYOPTION_DIVISION_EXP_LEVEL				1		// ·¹º§  È¹µæ

#define	AGCMUI_MAX_PARTYOPTION_DIVISION_ITEM				3

#define	AGCMUI_MAX_PARTYOPTION_DIVISION_EXP					2

//	Finish 2005.04.13 by SungHoon
#define UI_MESSAGE_PARTYOPTION_DIVISION_ITEM_SEQUENCE		"¼øÂ÷ÀûÈ¹µæ"
#define UI_MESSAGE_PARTYOPTION_DIVISION_ITEM_FREE			"ÀÚÀ¯È¹µæ"
#define UI_MESSAGE_PARTYOPTION_DIVISION_ITEM_DAMAGE			"±â¿©µµÈ¹µæ"

#define UI_MESSAGE_PARTYOPTION_DIVISION_EXP_DAMAGE			"±â¿©µµºÐ¹è"
#define UI_MESSAGE_PARTYOPTION_DIVISION_EXP_LEVEL			"·¹º§±ÕµîºÐ´ë"

#define	UI_MESSAGE_ID_INVITE_FAILED_SELF					"Party_Invite_Failed_Self"
#define	UI_MESSAGE_ID_INVITE_FAILED_NO_LEADER				"Party_Invite_Failed_No_Leader"
#define	UI_MESSAGE_ID_INVITE_FAILED_COLOR					"Positive_Text_Color"

#define	UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_SEQUENCE	"Party_Item_Division_Sequence"
#define	UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_FREE		"Party_Item_Division_Free"
#define	UI_MESSAGE_ID_PARTYOPTION_DIVISION_ITEM_DAMAGE		"Party_Item_Division_Damage"

#define	UI_MESSAGE_ID_PARTYOPTION_DIVISION_EXP_DAMAGE		"Party_Exp_Division_Damage"
#define	UI_MESSAGE_ID_PARTYOPTION_DIVISION_EXP_LEVEL		"Party_Exp_Division_Level"


typedef enum	_AgcmUIPartyOption_Display_ID {
	AGCMUI_PARTYOPTION_DISPLAY_ID_ITEM_PICKUP_NAME			= 0,
	AGCMUI_PARTYOPTION_DISPLAY_ID_DIVISION_ITEM_UPDATE,
	AGCMUI_PARTYOPTION_DISPLAY_ID_DIVISION_EXP_UPDATE,
	AGCMUI_PARTYOPTION_DISPLAY_ID_PARTY_MEMBER,
	AGCMUI_PARTYOPTION_DISPLAY_ID_NO_LOGIN_MEMBER,
} AgcmUIPartyOption_Display_ID ;


class AgcmUIPartyOption : public AgcModule
{
private:
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgpmParty				*m_pcsAgpmParty;
	AgcmParty				*m_pcsAgcmParty;
	AgcmUIManager2			*m_pcsAgcmUIManager2;
	AgcmUIItem				*m_pcsAgcmUIItem;
	AgcmChatting2			*m_pcsAgcmChatting;
	AgpmItem				*m_pcsAgpmItem;

//	Begin 2005.04.13 by SungHoon
//	Party Option UserData Name
	AgcdUIUserData			*m_pcsUserDivisionItem;
	AgcdUIUserData			*m_pcsUserDivisionExp;
	AgcdUIUserData			*m_pcsMessageData;

	AgcdUIUserData			*m_pcsEnableDivisionItem;
	AgcdUIUserData			*m_pcsEnableDivisionExp;
	AgcdUIUserData			*m_pcsEnableOptionAply;
	AgcdUIUserData			*m_pcsEnableOptionInviteMember;

	CHAR					*m_aszDivisionItem[AGCMUI_MAX_PARTYOPTION_DIVISION_ITEM];
	CHAR					*m_aszDivisionExp[AGCMUI_MAX_PARTYOPTION_DIVISION_EXP];

	BOOL					m_bEnableOptionForPartyLeader;
	BOOL					m_bEnableOptionInviteMember;
//	Finish 2005.04.13 by SungHoon


//	Begin 2005.04.14 by SungHoon
//	Party Option ÆÄ¶ó¹ÌÅÍ ÃÊ±âÈ­ ÇÔ¼ö
	void					SetPartyOption(AgpdParty *pcsParty);
//	Finish 2005.04.13 by SungHoon

public:
	AgcmUIPartyOption();
	virtual	~AgcmUIPartyOption();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	BOOL				AddEvent();
	BOOL				AddFunction();
	BOOL				AddDisplay();
	BOOL				AddUserData();
	BOOL				AddBoolean();

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

//	Begin 2005.04.13 by SungHoon
//	Party Option CallBack Funtion	È®ÀÎ ¹öÆ°À» ´­·¶À»¶§ ÇÊ¿äÁ¤º¸¿Í ÇÔ²² È£ÃâµÈ´Ù.
	static BOOL			CBPartyOptionAply(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBPartyOptionInviteMember(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
//	Finish 2005.04.13 by SungHoon
	static BOOL			CBPartyOptionOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL			CBPartyOptionOpenUIClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL			CBUpdateItemDivision(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.04.19 by SungHoon
	static BOOL			CBReceiveInviteFailedNoLoginMember(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.04.21 by SungHoon
	static BOOL			CBUpdateParty(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.05.10 by SungHoon
	static BOOL			CBAddParty(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.05.18 by SungHoon
	static BOOL			CBRemoveParty(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.05.10 by SungHoon
	static BOOL			CBUpdateExpDivision(PVOID pData, PVOID pClass, PVOID pCustData);	//	2005.05.17 by SungHoon

	static BOOL			CBDisplayPartyOptionGetItemMember(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);	//	2005.04.19 by SungHoon
	static BOOL			CBDisplayPartyOptionDivisionItemUpdate(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);	//	2005.04.19 by SungHoon
	static BOOL			CBDisplayPartyOptionNoLoginMember(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

	static BOOL			CBIsActivePartyOptionUIInviteMemberID(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	BOOL				AddSystemMessage(CHAR* szMsg, DWORD dwColor = 0XFFFFFF00);

	void				InitUIMessage();

	void				EnableUIForPartyLeader(AgpdParty *pcsParty);
	CHAR				*m_pszNoLoginMemberID;
private:
	AuPacket			m_csPacket;
	AgcmUIPartyOptionGetItemPartyMemberMessageData	m_stMessageData;

	INT32				m_lEventPartyOptionItemGetMemeber;
	INT32				m_lEventPartyOptionRecvNoLoginMember;
	INT32				m_lEventPartyOptionDivisionItemUpdate;
	INT32				m_lEventPartyOptionExpItemUpdate;


	INT32				m_lEventPartyOptionUIOpen;
	AgpmPartyOptionDivisionItem	m_ePartyOptionDivisionItem;
	AgpmPartyCalcExpType	m_ePartyCalExpType;
public:
	CHAR				m_szDefaultID[AGPACHARACTER_MAX_ID_STRING+1];
	INT32				m_lEventPartyOptionUIOpenClick;
};

#endif //__AGCMUIPARTY_H__
