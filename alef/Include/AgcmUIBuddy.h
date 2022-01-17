#pragma once

#include "AgcModule.h"
#include "AgcmUIMain.h"
#include "AgcmUIManager2.h"
#include "AgpmBuddy.h"
#include "AgpmChatting.h"


#define		MENTOR_BUTTON_MAX_LENGTH			10

class AgcmCharacter;
class AgcmChatting2;
class AgcmParty;
class AgcmUIChatting2;

class AgcmUIBuddy : public AgcModule
{
private:
	AgpmBuddy*														m_pcsAgpmBuddy;
	AgcmCharacter*													m_pcsAgcmCharacter;
	AgpmCharacter*													m_pcsAgpmCharacter;
	AgcmUIMain*														m_pcsAgcmUIMain;
	AgcmUIManager2*													m_pcsAgcmUIManager2;
	AgpmChatting*													m_pcsAgpmChatting;
	AgcmChatting2*													m_pcsAgcmChatting2;
	AgcmUIChatting2*												m_pcsAgcmUIChatting2;
	ApmMap*															m_pcsApmMap;
	AgcmParty*														m_pcsAgcmParty;

	AgcdUIUserData* 												m_pstUDFriendList;
	AgcdUIUserData* 												m_pstUDBanList;
	AgcdUIUserData* 												m_pstUDBuddyDetailInfo;		//. Buddy Detail Info User Data (Dummy)
	AgcdUIUserData* 												m_pstUDBlockWhisper;		//. Rejection List Block Whisper Option 
	AgcdUIUserData* 												m_pstUDBlockTrade;			//. Rejection List Block Trade Option 
	AgcdUIUserData* 												m_pstUDBlockInvitation;		//. Rejection List Block Invitation Option 
	AgcdUIUserData* 												m_pstUDAddRequestBuddy;		//. Buddy request to me
	BOOL															m_bBlockWhisper;			//. Block Option User Data
	BOOL															m_bBlockTrade;				//. Block Option User Data
	BOOL															m_bBlockInvitation;			//. Block Option User Data

	// Mentor
	AgcdUIUserData*													m_pstUDMentorList;



	INT32															m_lBuddyDetailInfo;			//. Buddy Detail Info ID


	INT32															m_lEventMainUIOpen;				// Main UI(Buddy List) Open
	INT32															m_lEventMainUIClose;			// Main UI(Buddy List) Close
	INT32															m_lEventAddBuddyOpen;			// 친구 추가 UI Open
	INT32															m_lEventBanUIOpen;				//. 거부리스트 Open
	INT32															m_lEventBanUIClose;				//. 거부리스트 Close
	INT32															m_lEventBuddyBanAddOpen;		//. 거부 친구 추가 UI open
	INT32															m_IEventBuddyDeleteQueryOpen;	//. 친구 삭제 쿼리 UI Open
	INT32															m_IEventBuddyBanDeleteQueryOpen;//. 거부친구 삭제 쿼리 UI Open
	INT32															m_lEventAddFriendDlgSetFocus;	// 친구 등록 UI에서 캐럿 초기 위치 설정
	INT32															m_lEventAddBanDlgSetFocus;		// 거부 등록 UI에서 캐럿 초기 위치 설정

	INT32															m_lEventAddRequestOpen;			// 친구 등록 신청 UI Open
	INT32															m_lEventAddRejectOpen;			// 친구 등록 거절 UI Open

	INT32															m_lMsgBoxAlreayExist;		// 이미 등록된 캐릭터를 등록하려는 경우
	INT32															m_lMsgBoxFullFriendList;	// 친구로 등록 가능한 수량이 가득 찼다.
	INT32															m_lMsgBoxFullBanList;		// 거부로 등록 가능한 수량이 가득 찼다.
	INT32															m_lMsgBoxOfflineOrNotExist;	// 오프라인이거나 존재하지 않음
	INT32															m_lMsgBoxInvalidUser;		// 등록할 수 없는 캐릭터
	INT32															m_lMsgBoxBanUser;			// 차단된 대상
	INT32															m_IMsgBoxInvalidUserIDLength;	//. 등록하려는 아이디가 기본 ID의 길이보다 클 경우 뜨는 dlg

	INT32															m_ICurrentBuddySelectIndex; //. 현재 선택된 Buddy의 Index
	INT32															m_ICurrentBanSelectIndex;	//. 현재 선택된 거부Buddy의 Index
	INT32															m_lCurrentMentorSelectIndex; // Current Mentor Index

	BOOL															m_bInitEnd;					// 초기화 데이터를 모두다 전송했는지 여부를 확인 TRUE면 전송완료

	CHAR															m_szIDBuffer[AGPDCHARACTER_NAME_LENGTH + 1];
	CHAR															m_szRegionBuffer[128 + 1];

	INT32															m_lEventMentorUIOpen;		// Mentor UI Open
	INT32															m_lEventMentorUIClose;		// Mentor UI Close

	AgcdUIUserData*													m_pstMentorBtnID;
	CHAR															m_szMentorBtnID	[ MENTOR_BUTTON_MAX_LENGTH ];

	AgcdUIUserData*													m_pstEnableInviteBtn;
	BOOL															m_bEnableInviteButton;
    
	ApSafeArray< INT32, AGPD_BUDDY_MAX >							m_arMentor;
	ApSafeArray< INT32, AGPD_BUDDY_MAX > 							m_arFriend;
	ApSafeArray< INT32, AGPD_BUDDY_MAX > 							m_arBan;

public:
	AgcmUIBuddy( void );
	virtual ~AgcmUIBuddy( void );

	BOOL 						OnAddModule							( void );
	BOOL 						OnInit								( void );
	BOOL 						OnDestroy							( void );
	BOOL 						OnIdle								( UINT32 ulClockCount );

	BOOL 						AddEvent							( void );
	BOOL 						AddFunction							( void );
	BOOL 						AddUserData							( void );
	BOOL 						AddDisplay							( void );

	BOOL						AddSystemMessage					( CHAR* szMsg );

	//. Buddylist, Banlist의 종료시점에서 사용하는 몇몇변수를 초기화한다.
	static BOOL 				CBBuddyListMemberClear				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBanListMemberClear				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBuddyUIKeydownESC					( PVOID pData, PVOID pClass, PVOID pCustData );


	///////////////////////////////////////////////////////////////////////////////////////////
	// refresh 
	BOOL 						BuddyListUpdate						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						RefreshFriendList					( void );
	BOOL 						RefreshBanList						( void );
	BOOL						RefreshMentorList					( void );

	//. 2005. 11. 16. Nonstopdj
	//. 친구리스트를 클릭했을 경우 선택된 아이디의 유저세부정보를 갱신
	BOOL						RefreshBuddyDetailInfo				( void );

	//. 2005. 11. 17. Nonstopdj
	//. BanListUI의 check box 옵션 Update & Refresh
	BOOL						BlockOptionUpdate					( AgpdBuddyElement* pcsBuddyElement );
	BOOL						RefreshBlockOptions					( void );

	///////////////////////////////////////////////////////////////////////////////////////////
	// option callback
	static BOOL 				CBCheckBlockWhisper					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCheckBlockTrade					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCheckBlockInvitation				( PVOID pData, PVOID pClass, PVOID pCustData );

	///////////////////////////////////////////////////////////////////////////////////////////
	// operation callback
	static BOOL 				CBAddBuddy							( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBAddBuddyRequest					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBAddBuddyReject					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBRemoveBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBOptionsBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBOnlineBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBMsgCodeBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBRegionInfo						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBInitEnd							( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL					CBMentorUIOpen						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBMentorRequestConfirm				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBMentorDelete				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBMentorRequestAccept				( PVOID pData, PVOID pClass, PVOID pCustData );

	///////////////////////////////////////////////////////////////////////////////////////////
	// packet operation
	BOOL 						OperationAddBuddy					( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationAddRequest					( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationAddReject					( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationRemoveBuddy				( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationOptionsBuddy				( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationOnlineBuddy				( AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationMsgCodeBuddy				( EnumAgpmBuddyMsgCode eMsgCode );
	BOOL 						OperationRegionInfo					( AgpdCharacter *pcsCharacter, AgpdBuddyRegion *pcsBuddyRegion );

	///////////////////////////////////////////////////////////////////////////////////////////
	// send packet
	BOOL 						SendAddBuddy						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendAddRequest						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendAddReject						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendRemoveBuddy						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendRegion							( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendOption							( AgpdBuddyElement* pcsBuddyElement );

	BOOL						SendMentorRequestAccept				( AgpdBuddyElement* pcsBuddyElement );
	BOOL						SendMentorRequestReject				( AgpdBuddyElement* pcsBuddyElement );
	BOOL						SendMentorDelete					( AgpdBuddyElement* pcsBuddyElement );
	BOOL						SendMentorRequest					( AgpdBuddyElement* pcsBuddyElement );

	///////////////////////////////////////////////////////////////////////////////////////////
	// event
	//. 2005. 11. 15. Nonstopdj
	//. 친구/거부 리스트에서 해당 캐릭터지우기 Event
	static BOOL 				CBReturnBuddyOkCancelDelete			( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
	static BOOL 				CBReturnBanOkCancelDelete			( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
	static BOOL 				CBReturnOkCancelBuddyAddRequest		( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );

	///////////////////////////////////////////////////////////////////////////////////////////
	// display : buddylist
	static BOOL					CBDisplayFriendList					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	//. 2005. 11. 15. Nonstopdj
	//. List에 등록된 Buddy의 접속상태(on/off)를 display하는 callback
	static BOOL					CBDisplayFriendStatus				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	//. List에 등록된 Buddy를 선택했을 경우 표시되어야 하는 세부정보 Display하는 callback
	static BOOL					CBDisplayFriendDetailInfo			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	//. item을 선택했을 때의 Display callback.
	static BOOL					CBDisplayFriendSelect				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	static BOOL					CBDisplayAddRequestBuddy			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	static BOOL					CBDisplayMentorBtnText				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR* szDIsplay, INT32* plValue, AgcdUIControl* pcsSourceControl );

	static BOOL					CBDisplayMentorSelect				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	static BOOL					CBDisplayMentorRelation				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	///////////////////////////////////////////////////////////////////////////////////////////
	// display : banlist
	static BOOL 				CBDisplayBanList					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBDisplayBanSelect					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );


	///////////////////////////////////////////////////////////////////////////////////////////
	// function : buddylist UI button control
	static BOOL 				CBBtnAddDlgOpenClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnAddBuddyClick					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 2005. 11. 15. Nonstopdj
	//. 친구삭제 Button Click Function
	static BOOL					CBBtnDeleteBuddyClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 파티초대 Button Click Function 
	static BOOL					CBBtnInviteBuddyClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 귓말 Button Click Function 
	static BOOL					CBBtnWhisperMsgBuddyClick			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 2005. 11. 16. Nonstopdj
	//. 등록된 Buddy Click Function
	static BOOL					CBBuddySelect						( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	// Buddy 리스트 등록창이 열렸을때 캐럿 위치 초기화
	static BOOL					CBBuddyAddDlgSetFocus				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );

	static BOOL					CBMentorSelect						( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// function : banlist UI button control
	//. 대화/거래/초대 차단
	static BOOL 				CBBtnBlockWhisperClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnBlockTradeClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnBlockInvitationClick			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 거부리스트에 추가
	static BOOL 				CBBtnAddBanDlgOpenClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnAddRejectionListClick			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 거부리스트에서 삭제
	static BOOL					CBBtnDeleteRejectionListClick		( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 등록된 Ban Click Function
	static BOOL					CBBanListSelect						( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	// Ban 리스트 등록창이 열렸을때 캐럿 위치 초기화
	static BOOL					CBBanAddDlgSetFocus					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	///////////////////////////////////////////////////////////////////////////////////////////

	// Boolean : 친구/거부 리스트에 등록되어있는지
	static BOOL 				CBNotInBuddyList					( PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBNotInBanList						( PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl );

	// Mentor
	static BOOL					CBMentorAddAndDel					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBMentorListRefresh					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );


	VOID						OperationMentorUIOpen						( VOID );
	VOID						OperationMentorUIClose						( VOID );
	VOID						OperationMentorRequestConfirm				( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement );
	VOID						OperationMentorDelete				( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement );
	VOID						OperationMentorRequestAccept				( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement );

	// Script
	void 						AddFriend							( void );
	void 						RemoveFriend						( void );
	void 						AddBan								( void );
	void 						RemoveBan							( void );
	void 						OpenAddDialog						( void );
	void 						OpenFriendList						( void );
	void 						OpenBanList							( void );
};
