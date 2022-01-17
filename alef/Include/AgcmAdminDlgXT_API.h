// AgcmAdminDlgXT_API.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 12. 11.

// 다른 프로젝트에서 참조하기 때문에..
// 대략 API 비스므리한 것이 온다.

//#pragma once

#ifdef _ADMIN_EXPORT_
	#define ADMIN_DLL extern "C" __declspec(dllexport)
#else
	#define ADMIN_DLL extern "C" __declspec(dllimport)
#endif

// Callback Functions Define
typedef BOOL (*ADMIN_CB)(PVOID pData, PVOID pClass, PVOID pCustData);

////////////////////////////////////////////////////////////////////////
// API
// Common
ADMIN_DLL BOOL ADMIN_Common_IsDialogMessage(LPMSG lpMsg);
ADMIN_DLL BOOL ADMIN_Common_SetCallbackClass(PVOID pClass);
ADMIN_DLL BOOL ADMIN_Common_SetSelfAdminInfo(stAgpdAdminInfo* pstInfo);

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_OpenDlg();
ADMIN_DLL BOOL ADMIN_Main_AddObject(stAgpdAdminPickingData* pstPickingData);
ADMIN_DLL BOOL ADMIN_Main_SetInfoCharData(stAgpdAdminCharData* pstCharData);
ADMIN_DLL BOOL ADMIN_Main_SetInfoCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub);
ADMIN_DLL BOOL ADMIN_Main_SetInfoText(LPCTSTR szName, LPCTSTR pszText);
ADMIN_DLL BOOL ADMIN_Main_SetCallbackSearch(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Main_SetCallbackHelpProcess(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Main_SetCallbackHelpRequest(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Main_AddHelp(stAgpdAdminHelp* pstHelp);
ADMIN_DLL BOOL ADMIN_Main_RemoveHelp(stAgpdAdminHelp* pstHelp);
ADMIN_DLL BOOL ADMIN_Main_SetLButtonDownPos(AuPOS* pstPosition);
ADMIN_DLL BOOL ADMIN_Main_SetCurrentUserCount(INT32 lCurrentUserCount);

// Search Dialog
ADMIN_DLL BOOL ADMIN_Search_SetCallbackSearch(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Search_SearchResult(stAgpdAdminSearch* pstSearch, stAgpdAdminSearchResult* stSearchResult);
ADMIN_DLL BOOL ADMIN_Search_SearchResultChar(BOOL bResult, stAgpdAdminCharData* pstCharData);

// Character Info Dialog
ADMIN_DLL BOOL ADMIN_CharacterData_SetCallbackEdit(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterData_SetCharData(BOOL bResult, stAgpdAdminCharData* pstCharData);
ADMIN_DLL BOOL ADMIN_CharacterData_SetCharDataSub(BOOL bResult, stAgpdAdminCharDataSub* pstCharDataSub);
ADMIN_DLL BOOL ADMIN_CharacterData_SetCharSkillPoint(CHAR* szCharName, INT32 lTotalSP, INT32 lRemainSP);
ADMIN_DLL BOOL ADMIN_CharacterData_ReceiveEditResult(stAgpdAdminCharEdit* pstCharEdit);

// Character Item Dialog
ADMIN_DLL BOOL ADMIN_CharacterItem_SetCallback(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterItem_SetItem(INT16 lType, stAgpdAdminItemData* pstItem);

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackSearchSkill(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackRollback(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackSave(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetMasteryList(BOOL bResult, PVOID pList);

// Character SkillSub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackSearchSkill(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackRollback(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackSave(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetSkill(stAgpdAdminSkillMastery* pstMastery, stAgpdAdminSkillData* pstSkill);
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetSkillList(stAgpdAdminSkillMastery* pstMastery, PVOID pList);

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetCallbackSearchParty(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterParty_SetCallbackReset(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMember(stAgpdAdminCharParty* pstParty);
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMemberSub(stAgpdAdminCharDataSub* pstMemberSub);
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMemberList(PVOID pList);

// Character Move
ADMIN_DLL BOOL ADMIN_CharacterMove_SetCallbackMove(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_CharacterMove_SetLButtonDownPos(AuPOS* pstPosition);

// Ban
ADMIN_DLL BOOL ADMIN_Ban_SetCallbackBan(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Ban_SetBanData(stAgpdAdminBan* pstBan);

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_OpenDlg();
ADMIN_DLL BOOL ADMIN_Help_SetCallbackRefresh(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Help_SetCallbackComplete(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Help_SetCallbackDefer(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Help_SetCallbackMemo(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Help_SetCallbackCharInfo(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Help_SetCallbackChatting(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Help_AddHelp(stAgpdAdminHelp* pstHelp);
ADMIN_DLL BOOL ADMIN_Help_HelpCompleteResult(stAgpdAdminHelp* pstHelp);
ADMIN_DLL BOOL ADMIN_Help_ReceiveHelpDefer(stAgpdAdminHelp* pstHelp);
ADMIN_DLL BOOL ADMIN_Help_ReceiveHelpMemo(stAgpdAdminHelp* pstHelp);
ADMIN_DLL BOOL ADMIN_Help_SetCharInfo(stAgpdAdminCharData* pstCharData);
ADMIN_DLL BOOL ADMIN_Help_SetCharSubInfo(stAgpdAdminCharDataSub* pstCharDataSub);
ADMIN_DLL BOOL ADMIN_Help_ReceiveChatMsg(stAgpdAdminChatData* pstAdminChatData);

// Item Dialog
ADMIN_DLL BOOL ADMIN_Item_OpenDlg();
ADMIN_DLL BOOL ADMIN_Item_SetCallbackCreate(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Item_SetCallbackGetInventoryItem(ADMIN_CB pfCallback);
ADMIN_DLL BOOL ADMIN_Item_AddItemTemplate(stAgpdAdminItemTemplate* pstItemTemplate);
ADMIN_DLL BOOL ADMIN_Item_ReceiveResult(stAgpdAdminItemOperation* pstItemOperation);
ADMIN_DLL BOOL ADMIN_Item_RefreshInventoryItemList();

// Admin List Dialog
ADMIN_DLL BOOL ADMIN_List_OpenDlg();
ADMIN_DLL BOOL ADMIN_List_AddAdmin(stAgpdAdminInfo* pstAdminInfo);
ADMIN_DLL BOOL ADMIN_List_RemoveAdmin(stAgpdAdminInfo* pstAdminInfo);

// Admin List Dialog
ADMIN_DLL BOOL ADMIN_Notice_OpenDlg();
ADMIN_DLL BOOL ADMIN_Notice_SetCallbackNotice(ADMIN_CB pfCallback);



//////////////////////////////////////////////////////////////////////////
// 2004.12.01. Renewal
// Game Dialog
ADMIN_DLL BOOL ADMIN_Game_OpenDlg();