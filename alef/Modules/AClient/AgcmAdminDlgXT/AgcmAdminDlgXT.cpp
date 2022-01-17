// AgcmAdminDlgXT.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
//#include "AgcmAdminDlgXT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAgcmAdminDlgXTApp

BEGIN_MESSAGE_MAP(CAgcmAdminDlgXTApp, CWinApp)
	//{{AFX_MSG_MAP(CAgcmAdminDlgXTApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgcmAdminDlgXTApp construction

CAgcmAdminDlgXTApp::CAgcmAdminDlgXTApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	// Singleton 초기화
	AgcmAdminDlgXT_Manager::Instance();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAgcmAdminDlgXTApp object

CAgcmAdminDlgXTApp theApp;




///////////////////////////////////////////////////////
// Export Function API
// Common
ADMIN_DLL BOOL ADMIN_Common_IsDialogMessage(LPMSG lpMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(AgcmAdminDlgXT_Manager::Instance()->IsDialogMessage(lpMsg))
		return TRUE;

	// 2004.12.01.
	if(AgcmAdminDlgXT_Manager::Instance()->IsDLGMSG(lpMsg))
		return TRUE;

	return FALSE;
}

// Common
ADMIN_DLL BOOL ADMIN_Common_SetCallbackClass(PVOID pClass)
{
	AgcmAdminDlgXT_Manager::Instance()->SetCBClass(pClass);
	return TRUE;
}

// Common
ADMIN_DLL BOOL ADMIN_Common_SetSelfAdminInfo(stAgpdAdminInfo* pstAdminInfo)
{
	AgcmAdminDlgXT_Manager::Instance()->SetSelfAdminInfo(pstAdminInfo);
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_OpenDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// Main Window 는 바로 보여주고
	AgcmAdminDlgXT_Manager::Instance()->OpenMainDlg();

	AgcmAdminDlgXT_Manager::Instance()->OpenGameDlg();

	// 다른 윈도우는 만든다음 HIDE 시켜놓는다. - Z-Position 때문에.. 2004.04.21.
	//AgcmAdminDlgXT_Manager::Instance()->OpenAllWindows();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_AddObject(stAgpdAdminPickingData* pstPickingData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->AddObject(pstPickingData);
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetInfoCharData(stAgpdAdminCharData* pstCharData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->SetInfoCharData(pstCharData);
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetInfoCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->SetInfoCharDataSub(pstCharDataSub);
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetCallbackSearch(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->SetCBSearch(pfCallback);
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetCallbackHelpProcess(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->SetCBHelpProcess(pfCallback);
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetCallbackHelpRequest(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->SetCBHelpRequest(pfCallback);
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_AddHelp(stAgpdAdminHelp* pstHelp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->HelpLock();
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->AddHelp(pstHelp);
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->HelpUnlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_RemoveHelp(stAgpdAdminHelp* pstHelp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->HelpLock();
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->RemoveHelp(pstHelp);
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->HelpUnlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetLButtonDownPos(AuPOS* pstPosition)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Lock();
	//AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Main Dialog
ADMIN_DLL BOOL ADMIN_Main_SetCurrentUserCount(INT32 lCurrentUserCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->SetCurrentUserCount(lCurrentUserCount);
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Unlock();
	return TRUE;
}

// Search Dialog
ADMIN_DLL BOOL ADMIN_Search_SetCallbackSearch(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetSearchDlg()->SetCBSearch(pfCallback);
	return TRUE;
}

// Search Dialog
ADMIN_DLL BOOL ADMIN_Search_SearchResult(stAgpdAdminSearch* pstSearch, stAgpdAdminSearchResult* pstSearchResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	AgcmAdminDlgXT_Manager::Instance()->GetSearchDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetSearchDlg()->SetResult(pstSearch, pstSearchResult);
	AgcmAdminDlgXT_Manager::Instance()->GetSearchDlg()->Unlock();
	return TRUE;
}

// Character Info Dialog
ADMIN_DLL BOOL ADMIN_CharacterData_SetCallbackEdit(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCBEditCharacter(pfCallback);
}

// Character Info Dialog
ADMIN_DLL BOOL ADMIN_CharacterData_SetCharData(BOOL bResult, stAgpdAdminCharData* pstCharData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCharData(pstCharData);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Info Dialog
ADMIN_DLL BOOL ADMIN_CharacterData_SetCharDataSub(BOOL bResult, stAgpdAdminCharDataSub* pstCharDataSub)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCharDataSub(pstCharDataSub);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

ADMIN_DLL BOOL ADMIN_CharacterData_SetCharSkillPoint(CHAR* szCharName, INT32 lTotalSP, INT32 lRemainSP)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCharSkillPoint(szCharName, lTotalSP, lRemainSP);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

ADMIN_DLL BOOL ADMIN_CharacterData_ReceiveEditResult(stAgpdAdminCharEdit* pstCharEdit)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMessageQueue()->PushQueueRecvCharEdit(pstCharEdit);

	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->ReceiveEditResult(pstCharEdit);
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Item Dialog
ADMIN_DLL BOOL ADMIN_CharacterItem_SetCallback(ADMIN_CB pfCallback)
{
	return TRUE;
}

// Character Item Dialog
ADMIN_DLL BOOL ADMIN_CharacterItem_SetItem(INT16 lType, stAgpdAdminItemData* pstItem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetItem(lType, pstItem);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	AgcmAdminDlgXT_Manager::Instance()->GetCharItemDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharItemDlg()->SetItem(lType, pstItem);
	AgcmAdminDlgXT_Manager::Instance()->GetCharItemDlg()->Unlock();
	
	return TRUE;
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackSearchSkill(ADMIN_CB pfCallback)
{
	return TRUE;
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackRollback(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCBSkillRollback(pfCallback);
	return TRUE;
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetCallbackSave(ADMIN_CB pfCallback)
{
	return TRUE;
}

// Character Skill Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkill_SetMasteryList(BOOL bResult, PVOID pList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetMasteryList(pList);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackSearchSkill(ADMIN_CB pfCallback)
{
	return TRUE;
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackRollback(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCBSkillRollback(pfCallback);
	return TRUE;
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetCallbackSave(ADMIN_CB pfCallback)
{
	return TRUE;
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetSkill(stAgpdAdminSkillMastery* pstMastery, stAgpdAdminSkillData* pstSkill)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetSkill(pstMastery, pstSkill);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Skill Sub Dialog
ADMIN_DLL BOOL ADMIN_CharacterSkillSub_SetSkillList(stAgpdAdminSkillMastery* pstMastery, PVOID pList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetSkill(pstMastery, pList);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetCallbackSearchParty(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCBSearchParty(pfCallback);

	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetCallbackReset(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetCBReset(pfCallback);

	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMember(stAgpdAdminCharParty* pstParty)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetMember(pstParty);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMemberSub(stAgpdAdminCharDataSub* pstMemberSub)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetMember(pstMemberSub);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Party Dialog
ADMIN_DLL BOOL ADMIN_CharacterParty_SetMemberList(PVOID pList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->SetMember(pList);
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

// Character Move Dialog
ADMIN_DLL BOOL ADMIN_CharacterMove_SetCallbackMove(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->SetCBMove(pfCallback);
	return TRUE;
}

// Character Move Dialog
ADMIN_DLL BOOL ADMIN_CharacterMove_SetLButtonDownPos(AuPOS* pstPosition)
{
	if(!pstPosition)
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->SetPosition(pstPosition->x, pstPosition->y, pstPosition->z);
	AgcmAdminDlgXT_Manager::Instance()->GetMoveDlg()->Unlock();
	
	// Main Window 에도 해준다.
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->SetPosition(pstPosition->x, pstPosition->y, pstPosition->z);
	AgcmAdminDlgXT_Manager::Instance()->GetMainDlg()->Unlock();

	return TRUE;
}

// Ban Dialog
ADMIN_DLL BOOL ADMIN_Ban_SetCallbackBan(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetBanDlg()->SetCBBan(pfCallback);
	return TRUE;
}

// Set Ban Data
ADMIN_DLL BOOL ADMIN_Ban_SetBanData(stAgpdAdminBan* pstBan)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetBanDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetBanDlg()->SetBanData(pstBan);
	AgcmAdminDlgXT_Manager::Instance()->GetBanDlg()->Unlock();

	return TRUE;
}


// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_OpenDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AgcmAdminDlgXT_Manager::Instance()->OpenHelpDlg();
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackRefresh(ADMIN_CB pfCallback)
{
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackComplete(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->SetCallbackHelpComplete(pfCallback);
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackDefer(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->SetCallbackHelpDefer(pfCallback);
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackMemo(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->SetCallbackHelpMemo(pfCallback);
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackCharInfo(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->SetCallbackCharInfo(pfCallback);
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCallbackChatting(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->SetCallbackChatting(pfCallback);
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_AddHelp(stAgpdAdminHelp* pstHelp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->AddHelp(pstHelp);
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_HelpCompleteResult(stAgpdAdminHelp* pstHelp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//AgcmAdminDlgXT_Manager::Instance()->GetMessageQueue()->PushQueueRecvHelpResult(pstHelp);

	// Help Dialog 안에서 부르게끔 변경 - 2004.04.05.
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->ReceiveHelpCompleteResult(pstHelp);
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_ReceiveHelpDefer(stAgpdAdminHelp* pstHelp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->ReceiveHelpDefer(pstHelp);
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_ReceiveHelpMemo(stAgpdAdminHelp* pstHelp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//AgcmAdminDlgXT_Manager::Instance()->GetMessageQueue()->PushQueueRecvHelpResult(pstHelp);

	// Help Dialog 안에서 부르게끔 변경 - 2004.04.05.
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->ReceiveHelpMemoResult(pstHelp);
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCharInfo(stAgpdAdminCharData* pstCharData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->SetCharInfo(pstCharData);
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_SetCharSubInfo(stAgpdAdminCharDataSub* pstCharDataSub)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->SetCharSub(pstCharDataSub);
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Help Dialog
ADMIN_DLL BOOL ADMIN_Help_ReceiveChatMsg(stAgpdAdminChatData* pstAdminChatData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->ReceiveChatMsg(pstAdminChatData);
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();
	return TRUE;
}

// Item Dialog
ADMIN_DLL BOOL ADMIN_Item_OpenDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->OpenDlg();
}

// Item Dialog
ADMIN_DLL BOOL ADMIN_Item_SetCallbackCreate(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->SetCallbackItemCreate(pfCallback);
}

// Item Dialog
ADMIN_DLL BOOL ADMIN_Item_SetCallbackGetInventoryItem(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->SetCallbackGetInventoryItem(pfCallback);
}

// Item Dialog
ADMIN_DLL BOOL ADMIN_Item_AddItemTemplate(stAgpdAdminItemTemplate* pstItemTemplate)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->AddItemTemplate(pstItemTemplate);
	AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Unlock();
	return TRUE;
}

// Item Dialog
ADMIN_DLL BOOL ADMIN_Item_ReceiveResult(stAgpdAdminItemOperation* pstItemOperation)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetMessageQueue()->PushQueueRecvItemResult(pstItemOperation);

	//AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Lock();
	//AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->ReceiveResult(pstItemOperation);
	//AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Unlock();
	return TRUE;
}

// Item Dialog
ADMIN_DLL BOOL ADMIN_Item_RefreshInventoryItemList()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->RefreshInventoryItemList();
	AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Unlock();
	return TRUE;
}

// Admin List Dialog
ADMIN_DLL BOOL ADMIN_List_OpenDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AgcmAdminDlgXT_Manager::Instance()->OpenAdminListDlg();
}

// Admin List Dialog
ADMIN_DLL BOOL ADMIN_List_AddAdmin(stAgpdAdminInfo* pstAdminInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetAdminListDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetAdminListDlg()->AddAdmin(pstAdminInfo);
	AgcmAdminDlgXT_Manager::Instance()->GetAdminListDlg()->Unlock();
	return TRUE;
}

// Admin List Dialog
ADMIN_DLL BOOL ADMIN_List_RemoveAdmin(stAgpdAdminInfo* pstAdminInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetAdminListDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetAdminListDlg()->RemoveAdmin(pstAdminInfo);
	AgcmAdminDlgXT_Manager::Instance()->GetAdminListDlg()->Unlock();
	return TRUE;
}

// Notice Dlg
ADMIN_DLL BOOL ADMIN_Notice_OpenDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AgcmAdminDlgXT_Manager::Instance()->OpenNoticeDlg();
}

// Notice Dlg
ADMIN_DLL BOOL ADMIN_Notice_SetCallbackNotice(ADMIN_CB pfCallback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AgcmAdminDlgXT_Manager::Instance()->GetNoticeDlg()->SetCBNotice(pfCallback);
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////
// 2004.12.01. Renewal
ADMIN_DLL BOOL ADMIN_Game_OpenDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	AgcmAdminDlgXT_Manager::Instance()->OpenGameDlg();
	return TRUE;
}