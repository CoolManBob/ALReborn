// AgcmUIQuest2.h: interface for the AgcmUIQuest2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMUIQUEST2_H__F4BB8378_259F_475E_BB65_A530D7B46346__INCLUDED_)
#define AFX_AGCMUIQUEST2_H__F4BB8378_259F_475E_BB65_A530D7B46346__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcModule.h"
#include "AgpmQuest.h"
#include "AgcmQuest.h"
#include "ApmEventManager.h"
#include "AgcmEventQuest.h"
#include "AgcmCharacter.h"
#include "AgcmUIControl.h"
#include "AgcmUIManager2.h"
#include "AgcmEventEffect.h"
#include "AgcmChatting2.h"
#include "AgpmFactors.h"
#include "AgpmGrid.h"
#include "ApmMap.h"
#include "AgcmUICharacter.h"
#include "AgcmObject.h"
#include "ApmObject.h"
#include "AgcmUICooldown.h"
#include "AgcmUIMain.h"

class AgcmTextBoardMng;

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIQuest2D" )
#else
#pragma comment ( lib , "AgcmUIQuest2" )
#endif
#endif

#define ACCEPTLIST_MAX_COUNT 50
#define QUESTMAIN_MAX_COUNT 100

enum EnumAcceptUIStatus
{
	ACCEPTUI_STATUS_NONE			= 0,
	ACCEPTUI_STATUS_NEW,					// 새로운 퀘스트
	ACCEPTUI_STATUS_INCOMPLETE,				// 부여 NPC의 관점에서 미완료 퀘스트
	ACCEPTUI_STATUS_INCOMPLETE_GRANT,		// 완료 NPC의 관점에서 미완료 퀘스트
	ACCEPTUI_STATUS_COMPLETE,				// 완료 퀘스트
};

enum EnumMainUIStatus
{
	MAINUI_NONE		= 0,
	MAINUI_CURRENT,					// 현재 진행중인 퀘스트
	MAINUI_COMPLETE,				// 완료한 퀘스트
	MAINUI_UNKNOWN,					// 아직 시작하지 않은 퀘스트
	MAINUI_SCENARIO,				// 시나리오 퀘스트
	MAINUI_SCENARIO_COMPLETE,		// 선행완료된 시나리오 퀘스트
	MAINUI_AREA,					// 지역 이름
};

const INT32 TREE_ROOT = 0;
const INT32 TREE_DEPTH_1 = 1;
const INT32 AGCMUIEVENTQUEST_CLOSE_UI_DISTANCE = 100;

struct AgcdUIQuestList
{
	INT32 lQuestTID;
	EnumAcceptUIStatus eStatus;
};

class AgcmUIQuest2 : public AgcModule
{
private:
	// Modules
	AgpmQuest			*m_pcsAgpmQuest;
	AgcmQuest			*m_pcsAgcmQuest;
	ApmEventManager		*m_pcsApmEventManager;
	AgcmUIManager2		*m_pcsAgcmUIManager2;
	AgcmUIControl		*m_pcsAgcmUIControl;
	AgcmEventQuest		*m_pcsAgcmEventQuest;
	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmEventEffect		*m_pcsAgcmEventEffect;
	AgcmTextBoardMng	*m_pcsAgcmTextBoard;
	AgcmChatting2		*m_pcsAgcmChatting;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmItem			*m_pcsAgpmItem;
	ApmMap				*m_pcsApmMap;
	AgcmUICharacter		*m_pcsAgcmUICharacter;
	AgcmObject			*m_pcsAgcmObject;
	ApmObject			*m_pcsApmObject;
	AgpmSkill			*m_pcsAgpmSkill;
	AgcmSkill			*m_pcsAgcmSkill;
	AgcmUICooldown		*m_pcsAgcmUICooldown;
	AgcmUIMain			*m_pcsAgcmUIMain;

	// Refresh Controls
	AgcdUIUserData		*m_pstUDAcceptList;
	AgcdUIUserData		*m_pstUDAcceptText;
	AgcdUIUserData		*m_pstUDAcceptLeftButton;
	AgcdUIUserData		*m_pstUDAcceptLeftButtonEnable;
	AgcdUIUserData		*m_pstUDMainText;
	AgcdUIUserData		*m_pstUDMainQuestCancel;
	AgcdUIUserData		*m_pstUDMainTree;
	AgcdUIUserData		*m_pstUDMainTreeDepth;
	AgcdUIUserData		*m_pstUDQuestStart;
	AgcdUIUserData		*m_pstUDQuestComplete;
	AgcdUIUserData		*m_pstUDQuestCount;

private:
	INT32 m_lEventAcceptUIClose;
	INT32 m_lEventAcceptUIOpen;
	INT32 m_lEventMainUIOpen;
	INT32 m_lEventQuestCancel;
	INT32 m_lEventQuestCount;

	INT32 m_lEventQuestAutoStart;
	INT32 m_lEventQuestAutoEnd;

	//Message 관련 Event
	INT32 m_lMessageBoxQuestTID;		// MessageBox에서 쓰일 Quest TID를 설정

	INT32 m_lMessageQuestDontHave;
	INT32 m_lMessageQuestStart;
	INT32 m_lMessageMainInventoryFull;
	INT32 m_lMessageQuestInventoryFull;
	INT32 m_lMessageQuestComplete;
	INT32 m_lMessageQuestFull;

	INT32 m_lSoundQuestStart;
	INT32 m_lSoundQuestFail;

	INT32 m_lLastCompleteTID;	// 마지막으로 완료한 퀘스트의 TID

	INT32 m_lClickIDAcceptUI;
	INT32 m_lClickIDMainUI;
	
	AuPOS m_stOpenPos;
	BOOL  m_bIsQuestUIOpen;

	INT32	m_lMainQuestTreeCount;
	BOOL	m_bAcceptLeftButtonEnable;
	UINT32	m_ulNotificationCloseTime;
	INT32	m_lAcceptListCount;

	BOOL	m_bHold;
	AgpdQuestCheckPoint m_CheckPointData;
	ApdEvent*			m_pCheckPointEvent;

	ApSafeArray<AgcdUIQuestList, ACCEPTLIST_MAX_COUNT> m_AcceptList;
	ApSafeArray<AgcdUIQuestList, ACCEPTLIST_MAX_COUNT> m_AcceptListAutoStart;
	ApSafeArray<INT32, QUESTMAIN_MAX_COUNT> m_lMainQuestIDList;
	ApSafeArray<INT32, QUESTMAIN_MAX_COUNT> m_lMainQuestIDListDepth;
	ApSafeArray<EnumMainUIStatus, QUESTMAIN_MAX_COUNT> m_eMainQuestIDStatus;

	AcUIToolTip		m_csNotification;

	INT32			m_lCooldownID;

	INT32			m_lNumAutoQuest;
	UINT32			m_ulAlertEndTime;

	AgpdGridItem *	m_pcsAutoStartIcon;
	BOOL			m_bOpenAutoStart;		// 자동부여 퀘스트가 남은것이 있으면 Button을 활성화하기 위해

	// test data
//	AgpdQuest		m_MyQuest;
	
public:
	AgcmUIQuest2();
	virtual ~AgcmUIQuest2();

//	void SetTestData();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	BOOL AddEvent();
	BOOL AddFunction();
	BOOL AddUserData();
	BOOL AddDisplay();

	// Refresh Functions
	BOOL RefreshAcceptList();
	BOOL RefreshAcceptText();
	BOOL RefreshAcceptLeftButton();
	BOOL RefreshAcceptLeftButtonEnable(BOOL bEnable);
	BOOL RefreshMainText();
	BOOL RefreshMainTree();
	BOOL RefreshMainCount();
	BOOL RefreshAcceptListAutoStart();

	// 
	BOOL UpdateQuestMainTree();

	static BOOL CBQuestGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBQuestAddEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInitBase(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRequireQuestResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCompleteQuestResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCancelQuestResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInventoryFull(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBQuestInventoryFull(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateQuest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRefreshAll(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBQuestItemNotice(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddItemInventory(PVOID pData, PVOID pClass, PVOID pCustData);

	//	Cooldown Callback
	static BOOL CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBClickAutoStartQuest(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL Notice(CHAR *szMessage);
	BOOL NoticeQuestItem(INT32 lQuestTID, INT32 lItemTID);
	BOOL AddSystemMessage(CHAR* szMsg);
	BOOL MakeAcceptList(AgcdUIQuestList *pAcceptList, INT32 *pListCount, ApdEvent *pApdEvent);
	BOOL MakeAcceptListAutoStart(AgcdUIQuestList *pAcceptList, INT32 *pListCount);
	BOOL SetCharacterEffect(ApBase* pApBase, ApdEvent* pApdEvent);
	BOOL RequireQuestResult(AgpdCharacter *pcsAgpdCharacter, INT32 lQuestTID, BOOL bResult, CHAR* szNPCName);
	BOOL RefreshBaseEffect();
	BOOL CompleteQuestResult(AgpdCharacter *pcsAgpdCharacter, INT32 lQuestTID, BOOL bResult);
	BOOL CancelQuestResult(AgpdCharacter *pcsAgpdCharacter, INT32 lQuestTID, BOOL bResult);
	BOOL CheckPointQuest(ApdEvent* pcsEvent);
	void AddMessageResult(AgpdElementItem* pItem);
	void AddMessageResult(AgpdElementSkill* pPRoductSkill);
	BOOL StartAnimation(AgpdCharacter* pcsAgpdCharacter, ApdEvent* pApdEvent);
	INT32 GetQuestTIDByItem(AgpdItem* pcsAgpdItem);
	BOOL NoticeNormalItem(AgpdItem *pcsAgpdItem);

	BOOL ComposeAcceptText(AgpdQuestTemplate *pQuestTemplate, char *pBuffer, INT32 lBufferSize);
	INT32 ComposeAcceptTextNew(AgpdQuestTemplate *pQuestTemplate, char *pBuffer, INT32 lBufferSize);
	INT32 ComposeAcceptTextIncomplete(AgpdQuestTemplate *pQuestTemplate, char *pBuffer, INT32 lBufferSize);
	INT32 ComposeAcceptTextComplete(AgpdQuestTemplate *pQuestTemplate, char *pBuffer, INT32 lBufferSize);
	INT32 ComposeAcceptItem(AgpdElementItem *pItem, char* pBuffer, INT32 lBufferSize);
	BOOL ComposeMainText(AgpdQuestTemplate *pQuestTemplate, char *pBuffer, INT32 lBufferSize);
	INT32 ComposeAcceptSummaryText(CHAR *pBuffer, INT32 lBufferSize, AgpdQuestTemplate *pQuestTemplate);	// return value : The number of characters written
	INT32 ComposeMainSummaryText(CHAR *pBuffer, INT32 lBufferSize, AgpdQuestTemplate *pQuestTemplate, AgpdQuest *pAgpdQuest);	// return value : The number of characters written
	INT32 ComposeCheckPointText(Enum_AGPDQUEST_SUMMARY_FACTOR eCheckPointFactor, char *pBuffer, AgpdQuestTemplate *pQuestTemplate, AgpdItemADChar* pcsAgpdItemADChar);

	// Display
	static BOOL CBDisplayQuestAcceptList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayQuestAcceptText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayQuestAcceptLeftButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayQuestMainText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayQuestMainCancel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayQuestMainTree(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayQuestStart(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayQuestComplete(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDispalyQuestCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	// Functions
	static BOOL CBButtonQuestAcceptListClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBButtonAcceptLeftClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBButtonQuestMainListClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCancelDialogOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBMainUIHotKeyOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBReturnCancelQuest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
};

#endif // !defined(AFX_AGCMUIQUEST2_H__F4BB8378_259F_475E_BB65_A530D7B46346__INCLUDED_)
