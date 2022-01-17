// AgcmAdminDlg.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 16.

#include "AgcmAdminDlgInclude.h"
#include "AgcmAdminDlg.h"

HINSTANCE g_hInst;

//////////////////////////////////////////////////////
// DLLMain
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpRes)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			g_hInst = hInst;

			// 초기화
			AgcmAdminDlgManager::Instance();
			AgcmAdminDlgManager::Instance()->SetInst(g_hInst);

			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}

///////////////////////////////////////////////////////
// Export Function API
// Common
ADMIN_DLL BOOL ADMIN_Common_IsDialogMessage(LPMSG lpMsg)
{
	return AgcmAdminDlgManager::Instance()->IsDialogMessage(lpMsg);
}

// Common
ADMIN_DLL BOOL ADMIN_Common_SetCallbackClass(PVOID pClass)
{
	return AgcmAdminDlgManager::Instance()->SetCBClass(pClass);
}

// Common
ADMIN_DLL BOOL ADMIN_Common_SetSelfAdminInfo(stAgpdAdminInfo* pstAdminInfo)
{
	return AgcmAdminDlgManager::Instance()->SetSelfAdminInfo(pstAdminInfo);
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_OpenDlg()
{
	return AgcmAdminDlgManager::Instance()->OpenMainDlg();
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_AddObject(stAgpdAdminPickingData* pstPickingData)
{
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetMainDlg()->AddObject(pstPickingData);
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetInfoCharData(stAgpdAdminCharData* pstCharData)
{
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetMainDlg()->SetInfoCharData(pstCharData);
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetInfoCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub)
{
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetMainDlg()->SetInfoCharDataSub(pstCharDataSub);
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetInfoText(LPCTSTR szName, LPCTSTR pszText)
{
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetMainDlg()->SetInfoText(szName, pszText);
	AgcmAdminDlgManager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetCallbackSearch(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetMainDlg()->SetCBSearch(pfCallback);
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetCallbackHelpProcess(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetMainDlg()->SetCBHelpProcess(pfCallback);
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetCallbackHelpRequest(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetMainDlg()->SetCBHelpRequest(pfCallback);
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_AddHelp(stAgpdAdminHelp* pstHelp)
{
	AgcmAdminDlgManager::Instance()->GetMainDlg()->HelpLock();
	AgcmAdminDlgManager::Instance()->GetMainDlg()->AddHelp(pstHelp);
	AgcmAdminDlgManager::Instance()->GetMainDlg()->HelpUnlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_ProcessHelp(stAgpdAdminHelp* pstHelp)
{
	AgcmAdminDlgManager::Instance()->GetMainDlg()->HelpLock();
	AgcmAdminDlgManager::Instance()->GetMainDlg()->ProcessHelp(pstHelp);
	AgcmAdminDlgManager::Instance()->GetMainDlg()->HelpUnlock();
	return TRUE;
}

// Search Dialog
ADMIN_DLL BOOL ADMIN_Search_SetCallbackSearch(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetSearchDlg()->SetCBSearch(pfCallback);
}

// Search Dialog
ADMIN_DLL BOOL ADMIN_Search_SearchResult(stAgpdAdminSearch* pstSearch, stAgpdAdminSearchResult* pstSearchResult)
{
	AgcmAdminDlgManager::Instance()->GetSearchDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetSearchDlg()->SetResult(pstSearch, pstSearchResult);
	AgcmAdminDlgManager::Instance()->GetSearchDlg()->Unlock();
	return TRUE;
}

// Character Info Dialog
ADMIN_DLL BOOL ADMIN_CharacterData_SetCallbackEdit(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->SetCBEditCharacter(pfCallback);
}

// Character Info Dialog
ADMIN_DLL BOOL ADMIN_CharacterData_SetCharData(BOOL bResult, stAgpdAdminCharData* pstCharData)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->SetCharData(bResult ? pstCharData : NULL);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->Unlock();

	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->SetCharData(bResult, pstCharData);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->Unlock();

	return TRUE;
}

// Character Info Dialog
ADMIN_DLL BOOL ADMIN_CharacterData_SetCharDataSub(BOOL bResult, stAgpdAdminCharDataSub* pstCharDataSub)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->SetCharDataSub(pstCharDataSub);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->Unlock();

	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->SetCharDataSub(bResult, pstCharDataSub);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetDataPage()->Unlock();
	return TRUE;
}

ADMIN_DLL BOOL ADMIN_CharacterData_SetCharSkillPoint(CHAR* szCharName, INT32 lTotalSP, INT32 lRemainSP)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->SetCharSkillPoint(szCharName, lTotalSP, lRemainSP);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->Unlock();

	return TRUE;
}

// Character Item Dialog
ADMIN_DLL BOOL ADMIN_CharacterItem_SetCallback(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetItemPage()->SetCBSearchItem(pfCallback);
}

// Character Item Dialog
ADMIN_DLL BOOL ADMIN_CharacterItem_SetItem(INT16 lType, stAgpdAdminItemData* pstItem)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetItemPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetItemPage()->SetItem(lType, pstItem);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetItemPage()->Unlock();
	return TRUE;
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackSearchSkill(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->SetCBSearchSkill(pfCallback);
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackRollback(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->SetCBRollback(pfCallback);
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackSave(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->SetCBSave(pfCallback);
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetMasteryList(BOOL bResult, PVOID pList)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->SetMasteryList(bResult, pList);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillPage()->Unlock();

	// Sub Dialog 의 Mastery 도 세팅해준다.
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->SetMasteryList(bResult, pList);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->Unlock();

	return TRUE;
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackSearchSkill(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->SetCBSearchSkill(pfCallback);
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackRollback(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->SetCBRollback(pfCallback);
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackSave(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->SetCBSave(pfCallback);
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetSkill(stAgpdAdminSkillMastery* pstMastery, stAgpdAdminSkillData* pstSkill)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->SetSkill(pstMastery, pstSkill);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->Unlock();
	return TRUE;
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetSkillList(stAgpdAdminSkillMastery* pstMastery, PVOID pList)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->SetSkill(pstMastery, pList);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetSkillSubPage()->Unlock();
	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetCallbackSearchParty(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->SetCBSearchParty(pfCallback);
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetCallbackReset(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->SetCBReset(pfCallback);
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMember(stAgpdAdminCharPartyMember* pstMember)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->SetMember(pstMember);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->Unlock();
	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMemberSub(stAgpdAdminCharDataSub* pstMemberSub)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->SetMember(pstMemberSub);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->Unlock();
	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMemberList(PVOID pList)
{
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->Lock();
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->SetMember(pList);
	AgcmAdminDlgManager::Instance()->GetCharacterDlg()->GetPartyPage()->Unlock();
	return TRUE;
}

// Character Move Dialog
ADMIN_DLL BOOL ADMIN_CharacterMove_SetCallbackMove(ADMIN_CB pfCallback)
{
	AgcmAdminDlgManager::Instance()->GetMoveDlg()->SetCBMove(pfCallback);
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_OpenDlg()
{
	return AgcmAdminDlgManager::Instance()->OpenHelpDlg();
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackRefresh(ADMIN_CB pfCallback)
{
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackComplete(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetHelpDlg()->SetCallbackHelpComplete(pfCallback);
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackCharInfo(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetHelpDlg()->SetCallbackCharInfo(pfCallback);
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackChatting(ADMIN_CB pfCallback)
{
	return AgcmAdminDlgManager::Instance()->GetHelpDlg()->SetCallbackChatting(pfCallback);
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_AddHelp(stAgpdAdminHelp* pstHelp)
{
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->AddHelp(pstHelp);
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_HelpCompleteResult(stAgpdAdminHelp* pstHelp)
{
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->ReceiveHelpCompleteResult(pstHelp);
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCharInfo(stAgpdAdminCharData* pstCharData)
{
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->SetCharInfo(pstCharData);
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_ReceiveChatMsg(stAgpdAdminChatData* pstAdminChatData)
{
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->ReceiveChatMsg(pstAdminChatData);
	AgcmAdminDlgManager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}