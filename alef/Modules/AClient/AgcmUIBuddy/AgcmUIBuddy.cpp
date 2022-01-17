#include "AgcmUIBuddy.h"

#include "AgpmBuddy.h"
#include "AgcmCharacter.h"
#include "AgcmUIConsole.h"
#include "AgcmChatting2.h"
#include "AgcmParty.h"
#include "AgcmUIChatting2.h"

#include "AuStrTable.h"
#include "AuRegionNameConvert.h"

//. UI Common Message Define.
#define UI_MESSAGE_BUDDYLIST_SELECT_NONE				"BuddyList_Select_None"
#define UI_MESSAGE_BANLIST_SELECT_NONE					"BanList_Select_None"
#define UI_MESSAGE_BUDDYLIST_ADD_USER					"BuddyList_Add_User"
#define UI_MESSAGE_BUDDYLIST_REMOVE_USER				"BuddyList_Remove_User"
#define	UI_MESSAGE_BUDDYLIST_INVITE_SEND				"BuddyList_Invite_Event_Send"

#define UI_MESSAGE_BANLIST_TRADEOPTION_ENABLE			"BanList_TradeOption_Enable"
#define UI_MESSAGE_BANLIST_TRADEOPTION_DISABLE			"BanList_TradeOption_Disable"
#define UI_MESSAGE_BANLIST_WHISPEROPTION_ENABLE			"BanList_WhisperOption_Enable"
#define UI_MESSAGE_BANLIST_WHISPEROPTION_DISABLE		"BanList_WhisperOption_Disable"
#define UI_MESSAGE_BANLIST_INVITEOPTION_ENABLE			"BanList_InviteOption_Enable"
#define UI_MESSAGE_BANLIST_INVITEOPTION_DISABLE			"BanList_InviteOption_Disable"

#define	UIMESSAGE_MENTOR_REQUEST_CONFIRM				"MentorRequestConfirm"
#define	UIMESSAGE_MENTOR_DELETE_CONFIRM					"MentorDeleteConfirm"

#define UIMESSAGE_MENTOR_REQUEST_ACCEPT					"MentorRequestAccept"
#define UIMESSAGE_MENTOR_DELETE							"MentorDelete"

#define UIMESSAGE_MENTOR_NOT_INVITE_BUDDY				"MentorNotInviteBuddy"
#define UIMESSAGE_MENTOR_INVALID_PROCESS				"MentorInvalidProcess"

AgcmUIBuddy::AgcmUIBuddy( void )
{
	// property initialize
	m_pcsAgpmBuddy					= NULL;
	m_pcsAgcmCharacter				= NULL;
	m_pcsAgpmCharacter				= NULL;
	m_pcsAgcmUIMain					= NULL;
	m_pcsAgcmUIManager2 			= NULL;
	m_pcsAgpmChatting				= NULL;
	m_pcsAgcmChatting2				= NULL;
	m_pcsApmMap						= NULL;
	m_pcsAgcmParty					= NULL;
	m_pcsAgcmUIChatting2			= NULL;

	m_pstUDFriendList				= NULL;
	m_pstUDBanList					= NULL;	
	m_pstUDBuddyDetailInfo			= NULL;

	m_pstUDBlockWhisper				= NULL;		
	m_pstUDBlockTrade				= NULL;		
	m_pstUDBlockInvitation			= NULL;
	m_bBlockWhisper					= FALSE;
	m_bBlockTrade					= FALSE;
	m_bBlockInvitation				= FALSE;

	m_lEventMainUIOpen				= 0;
	m_lEventMainUIClose				= 0;
	m_lEventAddBuddyOpen			= 0;
	m_lEventBanUIOpen				= 0;
	m_lEventBanUIClose				= 0;
	m_lEventBuddyBanAddOpen 		= 0;
	m_IEventBuddyDeleteQueryOpen	= 0;
	m_IEventBuddyBanDeleteQueryOpen	= 0;
	m_IMsgBoxInvalidUserIDLength	= 0;
	m_lEventAddFriendDlgSetFocus	= 0;
	m_lEventAddBanDlgSetFocus		= 0;
	m_lEventAddRequestOpen			= 0;
	m_lEventAddRejectOpen			= 0;

	m_lMsgBoxAlreayExist			= 0;
	m_lMsgBoxFullFriendList			= 0;	
	m_lMsgBoxFullBanList			= 0;
	m_lMsgBoxOfflineOrNotExist		= 0;	
	m_lMsgBoxBanUser				= 0;;		

	m_ICurrentBuddySelectIndex		= -1;
	m_ICurrentBanSelectIndex		= -1;
	m_lCurrentMentorSelectIndex		= -1;

	m_bInitEnd = FALSE;

	memset( m_szIDBuffer, 0, sizeof( m_szIDBuffer ) );
	memset( m_szRegionBuffer, 0, sizeof( m_szRegionBuffer ) );

	for( INT32 nListCount = 0 ; nListCount < AGPD_BUDDY_MAX ; ++nListCount )
	{
		m_arFriend[ nListCount ] = nListCount;
		m_arBan[ nListCount ] = nListCount;
		m_arMentor[ nListCount ] = nListCount;
	}

	// module initialize
	SetModuleName( "AgcmUIBuddy" );
	EnableIdle2( FALSE );
}

AgcmUIBuddy::~AgcmUIBuddy( void )
{
}

BOOL AgcmUIBuddy::OnAddModule( void )
{
	m_pcsAgpmBuddy			= ( AgpmBuddy*			)GetModule( "AgpmBuddy" );
	m_pcsAgcmCharacter		= ( AgcmCharacter*		)GetModule( "AgcmCharacter" );
	m_pcsAgpmCharacter		= ( AgpmCharacter*		)GetModule( "AgpmCharacter" );
	m_pcsAgcmUIMain			= ( AgcmUIMain*			)GetModule( "AgcmUIMain" );
	m_pcsAgcmUIManager2 	= ( AgcmUIManager2* 	)GetModule( "AgcmUIManager2" );
	m_pcsAgpmChatting		= ( AgpmChatting*		)GetModule( "AgpmChatting" );
	m_pcsAgcmChatting2		= ( AgcmChatting2*		)GetModule( "AgcmChatting2" );
	m_pcsApmMap				= ( ApmMap*				)GetModule( "ApmMap" );
	m_pcsAgcmParty			= ( AgcmParty*			)GetModule( "AgcmParty" );
	m_pcsAgcmUIChatting2	= ( AgcmUIChatting2*	)GetModule( "AgcmUIChatting2" );

	if( !m_pcsAgpmBuddy || !m_pcsAgcmCharacter || !m_pcsAgcmUIManager2 || !m_pcsAgpmChatting || !m_pcsAgcmChatting2 ||
		!m_pcsApmMap || !m_pcsAgcmParty || !m_pcsAgcmUIMain || !m_pcsAgpmCharacter || !m_pcsAgcmUIChatting2 ) return FALSE;

	if( !m_pcsAgpmBuddy->SetCallbackAdd( CBAddBuddy, this ) ) return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackAddRequest( CBAddBuddyRequest, this ) )	return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackAddReject( CBAddBuddyReject, this ) ) return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackRemove( CBRemoveBuddy, this ) ) return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackOptions( CBOptionsBuddy, this ) ) return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackOnline( CBOnlineBuddy, this ) ) return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackMsgCode( CBMsgCodeBuddy, this ) ) return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackInitEnd( CBInitEnd, this ) ) return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackRegion( CBRegionInfo, this ) ) return FALSE;
	if( !m_pcsAgpmChatting->SetCallbackCheckBlockWhisper( CBCheckBlockWhisper, this) ) return FALSE;


	// Mentor
	if( !m_pcsAgpmBuddy->SetCallbackMentorRequest		( CBMentorRequestConfirm,	this ) )	return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackMentorUIOpen		( CBMentorUIOpen,			this ) )	return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackMentorDelete		( CBMentorDelete,	this ) )	return FALSE;
	if( !m_pcsAgpmBuddy->SetCallbackMentorRequestAccept	( CBMentorRequestAccept,	this ) )	return FALSE;



	if( !AddEvent() ) return FALSE;
	if( !AddFunction())	return FALSE;
	if( !AddUserData() ) return FALSE;
	if( !AddDisplay() )	return FALSE;

	if( !m_pcsAgcmUIManager2->AddBoolean( this, "NotInBuddyList", CBNotInBuddyList, 0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddBoolean( this, "NotInBanList", CBNotInBanList, 0 ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIBuddy::OnInit( void )
{
	AS_REGISTER_TYPE_BEGIN( AgcmUIBuddy, AgcmUIBuddy );
		AS_REGISTER_METHOD0( void, AddFriend );
		AS_REGISTER_METHOD0( void, RemoveFriend );
		AS_REGISTER_METHOD0( void, AddBan );
		AS_REGISTER_METHOD0( void, RemoveBan );
		AS_REGISTER_METHOD0( void, OpenAddDialog );
		AS_REGISTER_METHOD0( void, OpenFriendList );
		AS_REGISTER_METHOD0( void, OpenBanList );
		AS_REGISTER_METHOD0( void, RefreshFriendList );
		AS_REGISTER_METHOD0( void, RefreshBanList );
		AS_REGISTER_METHOD0( void, OperationMentorUIOpen );
	AS_REGISTER_TYPE_END;
	return TRUE;
}

BOOL AgcmUIBuddy::OnDestroy( void )
{
	return TRUE;
}

BOOL AgcmUIBuddy::OnIdle( UINT32 ulClockCount )
{
	return TRUE;
}

BOOL AgcmUIBuddy::AddEvent( void )
{
	AGCMUI_CHECK_RETURN( m_lEventMainUIOpen,			m_pcsAgcmUIManager2->AddEvent( "BuddyMain_Open" ) );
	AGCMUI_CHECK_RETURN( m_lEventMainUIClose,			m_pcsAgcmUIManager2->AddEvent( "BuddyMain_Close" ) );
	AGCMUI_CHECK_RETURN( m_lEventAddBuddyOpen,			m_pcsAgcmUIManager2->AddEvent( "BuddyAddDlg_Open" ) );
	AGCMUI_CHECK_RETURN( m_lEventBanUIOpen,				m_pcsAgcmUIManager2->AddEvent( "BanList_Open" ) );
	AGCMUI_CHECK_RETURN( m_lEventBanUIClose,			m_pcsAgcmUIManager2->AddEvent( "BanList_Close" ) );
	AGCMUI_CHECK_RETURN( m_lEventBuddyBanAddOpen,		m_pcsAgcmUIManager2->AddEvent( "BuddyBanAddDlg_Open" ) );
	AGCMUI_CHECK_RETURN( m_lEventAddRejectOpen,			m_pcsAgcmUIManager2->AddEvent( "AddRejectDlg_Open" ) );
	AGCMUI_CHECK_RETURN( m_lMsgBoxAlreayExist,			m_pcsAgcmUIManager2->AddEvent( "BuddyAlreayExist_MsgBox" ) );
	AGCMUI_CHECK_RETURN( m_lMsgBoxFullFriendList,		m_pcsAgcmUIManager2->AddEvent( "BuddyFullFriendList_MsgBox" ) );
	AGCMUI_CHECK_RETURN( m_lMsgBoxFullBanList,			m_pcsAgcmUIManager2->AddEvent( "BuddyFullBanList_MsgBox" ) );
	AGCMUI_CHECK_RETURN( m_lMsgBoxOfflineOrNotExist,	m_pcsAgcmUIManager2->AddEvent( "BuddyOfflineOrNotExist_MsgBox" ) );
	AGCMUI_CHECK_RETURN( m_lMsgBoxBanUser,				m_pcsAgcmUIManager2->AddEvent( "BuddyBanUser_MsgBox" ) );
	AGCMUI_CHECK_RETURN( m_lMsgBoxInvalidUser,			m_pcsAgcmUIManager2->AddEvent( "BuddyInvalidUser_MsgBox" ) );
	AGCMUI_CHECK_RETURN( m_IMsgBoxInvalidUserIDLength,	m_pcsAgcmUIManager2->AddEvent( "BuddyIDLengthWarning_Open" ) );
	AGCMUI_CHECK_RETURN( m_lEventAddFriendDlgSetFocus,	m_pcsAgcmUIManager2->AddEvent( "FriendAddDlg_SetFocus" ) );
	AGCMUI_CHECK_RETURN( m_lEventAddBanDlgSetFocus,		m_pcsAgcmUIManager2->AddEvent( "BanAddDlg_SetFocus" ) );

	// Mentor
	AGCMUI_CHECK_RETURN( m_lEventMentorUIOpen,			m_pcsAgcmUIManager2->AddEvent( "MentorUI_Open" ) );
	AGCMUI_CHECK_RETURN( m_lEventMentorUIClose,			m_pcsAgcmUIManager2->AddEvent( "MentorUI_Close" ) );


	//. 2005. 11. 15. Nonstopdj
	//. delete confirm dialog event
	m_IEventBuddyDeleteQueryOpen = m_pcsAgcmUIManager2->AddEvent( "BuddyDeleteBuddyUserConfirm_MsgBox", CBReturnBuddyOkCancelDelete, this );
	if( m_IEventBuddyDeleteQueryOpen < 0 ) return FALSE;

	m_IEventBuddyBanDeleteQueryOpen = m_pcsAgcmUIManager2->AddEvent( "BuddyDeleteBanUserConfirm_MsgBox", CBReturnBanOkCancelDelete, this );
	if( m_IEventBuddyBanDeleteQueryOpen < 0 ) return FALSE;

	m_lEventAddRequestOpen = m_pcsAgcmUIManager2->AddEvent( "BuddyAddRequestConfirm_Open", CBReturnOkCancelBuddyAddRequest, this );
	if( m_lEventAddRequestOpen < 0 ) return FALSE;

	return TRUE;
}

BOOL AgcmUIBuddy::AddFunction( void )
{
	//. BuddyList UI Functions
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyList_Clear",				CBBuddyListMemberClear,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyAddFriend_Click",		CBBtnAddBuddyClick,				0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyAddDlgOpen_Click", 		CBBtnAddDlgOpenClick,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyDeleteList_Click", 		CBBtnDeleteBuddyClick,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyInviteFriend_Click",		CBBtnInviteBuddyClick,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyWhiperMsgFriend_Click",	CBBtnWhisperMsgBuddyClick,		0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyList_Item_Selected",		CBBuddySelect,					0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BuddyAddDlg_SetFocus",		CBBuddyAddDlgSetFocus,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanList_Clear",				CBBanListMemberClear,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanAdd_Click",				CBBtnAddRejectionListClick, 	0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanAddDlgOpen_Click",			CBBtnAddBanDlgOpenClick,		0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanBlockWhisper_Click",		CBBtnBlockWhisperClick,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanBlockTrade_Click",			CBBtnBlockTradeClick,			0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanBlockInvitation_Click",	CBBtnBlockInvitationClick,		0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanDeleteList_Click",			CBBtnDeleteRejectionListClick,	0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanList_Item_Selected",		CBBanListSelect,				0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "BanAddDlg_SetFocus",			CBBanAddDlgSetFocus,			0 ) ) return FALSE;

	// Mentor System
	if( !m_pcsAgcmUIManager2->AddFunction( this , "Mentor_AddAndDel" ,			CBMentorAddAndDel,				0 ) ) return FALSE;
	//if( !m_pcsAgcmUIManager2->AddFunction( this , "Mentor_Del" ,				CBDelMentor,					0 ) ) return FALSE;

	if( !m_pcsAgcmUIManager2->AddFunction( this , "Mentor_Item_Selected",		CBMentorSelect,					0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this , "MentorList_Refresh",			CBMentorListRefresh,			0 ) ) return FALSE;

	//. KeyDownESC callback
	if( !m_pcsAgcmUIMain->SetCallbackKeydownESC( CBBuddyUIKeydownESC, this ) ) return FALSE;
	return TRUE;
}


BOOL AgcmUIBuddy::AddUserData( void )
{
	if( !( m_pstUDFriendList		= m_pcsAgcmUIManager2->AddUserData( "BuddyFriendList_UD",		&m_arFriend[ 0 ],		sizeof( INT32 ), 				AGPD_BUDDY_MAX, AGCDUI_USERDATA_TYPE_INT32 	) ) ) return FALSE;
	if( !( m_pstUDBanList			= m_pcsAgcmUIManager2->AddUserData( "BanList_UD",				&m_arBan[ 0 ],			sizeof( INT32 ), 				AGPD_BUDDY_MAX, AGCDUI_USERDATA_TYPE_INT32 	) ) ) return FALSE;
	if( !( m_pstUDAddRequestBuddy	= m_pcsAgcmUIManager2->AddUserData( "AddRequest_UD",			m_szIDBuffer,			AGPDCHARACTER_NAME_LENGTH + 1,	1, 				AGCDUI_USERDATA_TYPE_STRING ) ) ) return FALSE;
	if( !( m_pstUDBuddyDetailInfo	= m_pcsAgcmUIManager2->AddUserData( "BuddyDetailInfo_UD",		&m_lBuddyDetailInfo,	sizeof( INT32 ), 				1, 				AGCDUI_USERDATA_TYPE_INT32	) ) ) return FALSE;
	if( !( m_pstUDBlockWhisper		= m_pcsAgcmUIManager2->AddUserData( "BanBlockWhisper_UD",		&m_bBlockWhisper,		sizeof( INT32 ), 				1, 				AGCDUI_USERDATA_TYPE_BOOL 	) ) ) return FALSE;
	if( !( m_pstUDBlockTrade		= m_pcsAgcmUIManager2->AddUserData( "BanBlockTrade_UD",			&m_bBlockTrade,			sizeof( INT32 ), 				1, 				AGCDUI_USERDATA_TYPE_BOOL 	) ) ) return FALSE;
	if( !( m_pstUDBlockInvitation	= m_pcsAgcmUIManager2->AddUserData( "BanBlockInvitation_UD",	&m_bBlockInvitation,	sizeof( INT32 ), 				1, 				AGCDUI_USERDATA_TYPE_BOOL 	) ) ) return FALSE;

	// Mentor
	if( !( m_pstUDMentorList		= m_pcsAgcmUIManager2->AddUserData( "MentorList_UD" ,			&m_arMentor[0],			sizeof( INT32 ),				AGPD_BUDDY_MAX, AGCDUI_USERDATA_TYPE_INT32	) ) ) return FALSE;

	m_pstMentorBtnID = m_pcsAgcmUIManager2->AddUserData( "Mentor_Btn" , m_szMentorBtnID , sizeof( CHAR ), MENTOR_BUTTON_MAX_LENGTH , AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pstMentorBtnID )
		return FALSE;

	m_pstEnableInviteBtn = m_pcsAgcmUIManager2->AddUserData( "Mentor_Enable_Invite_Button" , &m_bEnableInviteButton , sizeof(m_bEnableInviteButton) , 1 , AGCDUI_USERDATA_TYPE_BOOL );
	if( !m_pstEnableInviteBtn )
		return FALSE;

    return TRUE;
}

BOOL AgcmUIBuddy::AddDisplay( void )
{
	//. Buddylist UI Display Funtions
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "BuddyFriend_Display",				0, CBDisplayFriendList,			AGCDUI_USERDATA_TYPE_INT32	) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "BuddyFriendStatus_Display",		0, CBDisplayFriendStatus,		AGCDUI_USERDATA_TYPE_INT32	) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "BuddyFriendDetaulInfo_Display",	0, CBDisplayFriendDetailInfo,	AGCDUI_USERDATA_TYPE_INT32	) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "BuddyFriendSelect_Display",		0, CBDisplayFriendSelect,		AGCDUI_USERDATA_TYPE_INT32	) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "AddRequestBuddy_Display",			0, CBDisplayAddRequestBuddy,	AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Ban_Display",						0, CBDisplayBanList,			AGCDUI_USERDATA_TYPE_INT32	) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Ban_Display_Select",				0, CBDisplayBanSelect,			AGCDUI_USERDATA_TYPE_INT32	) ) return FALSE;

	if( !m_pcsAgcmUIManager2->AddDisplay( this , "Mentor_BtnText" , 0 , CBDisplayMentorBtnText , AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	if( !m_pcsAgcmUIManager2->AddDisplay( this , "MentorSelect_Display", 0 , CBDisplayMentorSelect, AGCDUI_USERDATA_TYPE_INT32 ))
		return FALSE;

	if( !m_pcsAgcmUIManager2->AddDisplay( this , "Mentor_Relation", 0 , CBDisplayMentorRelation, AGCDUI_USERDATA_TYPE_STRING ))
		return FALSE;



	return TRUE;
}

BOOL AgcmUIBuddy::CBMentorListRefresh( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass )
		return FALSE;

	AgcmUIBuddy*	pThis		=	static_cast< AgcmUIBuddy* >(pClass);

	pThis->RefreshMentorList();

	return TRUE;
}


//. clear functions
BOOL AgcmUIBuddy::CBBuddyListMemberClear( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;

	AgcmUIBuddy* pThis = (AgcmUIBuddy*)pClass;
	pThis->m_ICurrentBuddySelectIndex = -1;

	memset( pThis->m_szIDBuffer, 0, sizeof( pThis->m_szIDBuffer ) );
	memset( pThis->m_szRegionBuffer, 0, sizeof( pThis->m_szRegionBuffer ) );

	pThis->RefreshFriendList();
	pThis->RefreshMentorList();
	pThis->RefreshBuddyDetailInfo();
	return TRUE;
}

BOOL AgcmUIBuddy::CBBanListMemberClear( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;

	pThis->m_ICurrentBanSelectIndex	= -1;
	pThis->RefreshBanList();
	pThis->m_bBlockWhisper = FALSE;
	pThis->m_bBlockTrade = FALSE;
	pThis->m_bBlockInvitation = FALSE;
	pThis->RefreshBlockOptions();

	return TRUE;
}

BOOL AgcmUIBuddy::CBBuddyUIKeydownESC( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	
	pThis->m_ICurrentBuddySelectIndex = -1;
	pThis->m_ICurrentBanSelectIndex	= -1;
	pThis->m_lCurrentMentorSelectIndex = -1;
	pThis->RefreshFriendList();
	pThis->RefreshMentorList();
	pThis->RefreshBuddyDetailInfo();
	pThis->RefreshBanList();
	pThis->m_bBlockWhisper = FALSE;
	pThis->m_bBlockTrade = FALSE;
	pThis->m_bBlockInvitation = FALSE;
	pThis->RefreshBlockOptions();

	memset( pThis->m_szIDBuffer, 0, sizeof( pThis->m_szIDBuffer ) );
	memset( pThis->m_szRegionBuffer, 0, sizeof( pThis->m_szRegionBuffer ) );
	return TRUE;
}

BOOL AgcmUIBuddy::CBAddBuddy( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgpdCharacter* pcsCharacter = ( AgpdCharacter* )pData;
	AgpdBuddyElement* pcsBuddyElement = ( AgpdBuddyElement* )pCustData;

	return pThis->OperationAddBuddy( pcsCharacter, pcsBuddyElement );
}

BOOL AgcmUIBuddy::CBAddBuddyRequest( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgpdCharacter* pcsCharacter = ( AgpdCharacter* )pData;
	AgpdBuddyElement* pcsBuddyElement = ( AgpdBuddyElement* )pCustData;

	return pThis->OperationAddRequest( pcsCharacter, pcsBuddyElement );
}

BOOL AgcmUIBuddy::CBAddBuddyReject( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgpdCharacter* pcsCharacter = ( AgpdCharacter* )pData;
	AgpdBuddyElement* pcsBuddyElement = ( AgpdBuddyElement* )pCustData;

	return pThis->OperationAddReject( pcsCharacter, pcsBuddyElement );
}

BOOL AgcmUIBuddy::CBRemoveBuddy( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;

	AgpdCharacter* pcsCharacter = ( AgpdCharacter* )pData;
	AgpdBuddyElement* pcsBuddyElement = ( AgpdBuddyElement* )pCustData;
	
	//. 선택된 Item이 제거되면 최근에 저장하고 있던 Index멤버도 초기화.
	( pcsBuddyElement->IsFriend() ) ? pThis->m_ICurrentBuddySelectIndex = -1 : pThis->m_ICurrentBanSelectIndex = -1;
	return pThis->OperationRemoveBuddy( pcsCharacter, pcsBuddyElement );
}

BOOL AgcmUIBuddy::CBOptionsBuddy( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgpdCharacter* pcsCharacter = ( AgpdCharacter* )pData;
	AgpdBuddyElement* pcsBuddyElement = ( AgpdBuddyElement* )pCustData;

	return pThis->OperationOptionsBuddy( pcsCharacter, pcsBuddyElement );
}

BOOL AgcmUIBuddy::CBOnlineBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pCustData)
		return FALSE;

	AgcmUIBuddy* pThis = (AgcmUIBuddy*)pClass;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

    return pThis->OperationOnlineBuddy( pcsBuddyElement );
}

BOOL AgcmUIBuddy::CBMsgCodeBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass ) 
		return FALSE;

	AgcmUIBuddy*			pThis		= static_cast< AgcmUIBuddy* >(pClass);
	EnumAgpmBuddyMsgCode*	pMsgCode	= static_cast< EnumAgpmBuddyMsgCode* >(pCustData);

	return pThis->OperationMsgCodeBuddy( *pMsgCode );
}

BOOL AgcmUIBuddy::CBRegionInfo( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgpdCharacter* pcsCharacter = ( AgpdCharacter* )pData;
	AgpdBuddyRegion* pcsBuddyRegion = ( AgpdBuddyRegion* )pCustData;

	return pThis->OperationRegionInfo( pcsCharacter, pcsBuddyRegion );
}

BOOL AgcmUIBuddy::CBInitEnd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	pThis->m_bInitEnd = TRUE;
	return TRUE;
}

BOOL AgcmUIBuddy::CBMentorUIOpen( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass )
		return FALSE;

	AgcmUIBuddy*	pThis	=	static_cast< AgcmUIBuddy* >(pClass);

	pThis->OperationMentorUIOpen();

	return TRUE;

}

BOOL AgcmUIBuddy::CBMentorRequestConfirm( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass )
		return FALSE;

	AgcmUIBuddy*		pThis				=	static_cast< AgcmUIBuddy* >(pClass);
	AgpdCharacter*		pcsCharacter		=	static_cast< AgpdCharacter* >(pData);
	AgpdBuddyElement*	pcsBuddyElement		=	static_cast< AgpdBuddyElement* >(pCustData);

	pThis->OperationMentorRequestConfirm( pcsCharacter , pcsBuddyElement );

	return TRUE;
}

BOOL AgcmUIBuddy::CBMentorDelete( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass )
		return FALSE;

	AgcmUIBuddy*		pThis				=	static_cast< AgcmUIBuddy* >(pClass);
	AgpdCharacter*		pcsCharacter		=	static_cast< AgpdCharacter* >(pData);
	AgpdBuddyElement*	pcsBuddyElement		=	static_cast< AgpdBuddyElement* >(pCustData);

	pThis->OperationMentorDelete( pcsCharacter , pcsBuddyElement );

	return TRUE;
}

BOOL AgcmUIBuddy::CBMentorRequestAccept( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass )
		return FALSE;

	AgcmUIBuddy*		pThis				=	static_cast< AgcmUIBuddy* >(pClass);
	AgpdCharacter*		pcsCharacter		=	static_cast< AgpdCharacter* >(pData);
	AgpdBuddyElement*	pcsBuddyElement		=	static_cast< AgpdBuddyElement* >(pCustData);

	pThis->OperationMentorRequestAccept( pcsCharacter , pcsBuddyElement );

	return TRUE;
}


BOOL AgcmUIBuddy::OperationAddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if( !m_pcsAgpmBuddy->AddBuddy(pcsCharacter, pcsBuddyElement ) )	return FALSE;
	if( m_bInitEnd )
	{
		ApString< 100 > strTemp;
		strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BUDDYLIST_ADD_USER ), pcsBuddyElement->m_szName.GetBuffer() );
		AddSystemMessage( strTemp );
	}

	return BuddyListUpdate( pcsBuddyElement );
}

// 2007.07.12. steeple
BOOL AgcmUIBuddy::OperationAddRequest( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement )
{
	if( !pcsCharacter || !pcsBuddyElement )	return FALSE;

	// 거부 상태인지 체크
	if( pcsCharacter->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_BUDDY ) return FALSE;

	_tcscpy( m_szIDBuffer, pcsBuddyElement->m_szName );
	
	// Confirm Dialog 띄운다.
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventAddRequestOpen );
	return TRUE;
}

// 2007.07.12. steeple
BOOL AgcmUIBuddy::OperationAddReject( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement )
{
	if( !pcsCharacter || !pcsBuddyElement )	return FALSE;

	m_pcsAgcmUIManager2->ThrowEvent( m_lEventAddRejectOpen );
	return TRUE;
}

BOOL AgcmUIBuddy::OperationRemoveBuddy( AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement )
{
	if( !m_pcsAgpmBuddy->RemoveBuddy( pcsCharacter, pcsBuddyElement ) )	return FALSE;
	if( m_bInitEnd )
	{
		ApString< 100 > strTemp;
		strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BUDDYLIST_REMOVE_USER ), pcsBuddyElement->m_szName.GetBuffer() );
		AddSystemMessage( strTemp );
	}

	return BuddyListUpdate( pcsBuddyElement );
}

BOOL AgcmUIBuddy::OperationOptionsBuddy( AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement )
{
	if( !m_pcsAgpmBuddy->OptionsBuddy( pcsCharacter, pcsBuddyElement) )	return FALSE;
	return BlockOptionUpdate( pcsBuddyElement );
}


//. BanListUI의 check box 옵션 Update & Refresh
BOOL AgcmUIBuddy::BlockOptionUpdate( AgpdBuddyElement* pcsBuddyElement )
{
	//. 2006. 1. 6. Nonstopdj
	//. 거부옵션변경 시스템메세지 출력추가
	ApString<128> strTemp;
	//. 귓말차단 옵션
	if( m_bBlockWhisper != pcsBuddyElement->IsBlockWhisper() )
	{
		if( pcsBuddyElement->IsBlockWhisper() )
		{
			strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_WHISPEROPTION_ENABLE ), pcsBuddyElement->m_szName.GetBuffer() );
		}
		else
		{
			strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_WHISPEROPTION_DISABLE ), pcsBuddyElement->m_szName.GetBuffer() );
		}

		AddSystemMessage( strTemp );
	
		//. 멤버변수 옵션 업데이트.
		m_bBlockWhisper = pcsBuddyElement->IsBlockWhisper();
	}
	//. 거래차단 옵션
	if( m_bBlockTrade != pcsBuddyElement->IsBlockTrade() )
	{
		if( pcsBuddyElement->IsBlockTrade() )
		{
			strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_TRADEOPTION_ENABLE ), pcsBuddyElement->m_szName.GetBuffer() );
		}
		else
		{
			strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_TRADEOPTION_DISABLE ), pcsBuddyElement->m_szName.GetBuffer() );
		}

		AddSystemMessage( strTemp );	
		m_bBlockTrade = pcsBuddyElement->IsBlockTrade();
	}
	//. 초대차단 옵션
	if( m_bBlockInvitation != pcsBuddyElement->IsBlockInvitation() )
	{
		if( pcsBuddyElement->IsBlockInvitation() )
		{
			strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_INVITEOPTION_ENABLE ), pcsBuddyElement->m_szName.GetBuffer() );
		}
		else
		{
			strTemp.Format( m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_INVITEOPTION_DISABLE ), pcsBuddyElement->m_szName.GetBuffer() );
		}

		AddSystemMessage( strTemp );	
		m_bBlockInvitation = pcsBuddyElement->IsBlockInvitation();
	}

	return RefreshBlockOptions();
}

BOOL AgcmUIBuddy::RefreshBlockOptions( void )
{
	if( !m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDBlockWhisper ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDBlockTrade ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDBlockInvitation ) ) return FALSE;
	return  TRUE;
}

BOOL AgcmUIBuddy::OperationOnlineBuddy( AgpdBuddyElement* pcsBuddyElement )
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter ) return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	ASSERT( NULL != pcsBuddyADChar );

	ApVectorBuddy::iterator iter = std::find( pcsBuddyADChar->m_vtFriend.begin(), pcsBuddyADChar->m_vtFriend.end(), pcsBuddyElement->m_szName.GetBuffer() );

	if( iter != pcsBuddyADChar->m_vtFriend.end() )
	{
		iter->SetOnline( pcsBuddyElement->IsOnline() );
		BuddyListUpdate( iter );
		return TRUE;
	}

	iter = std::find( pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), pcsBuddyElement->m_szName.GetBuffer() );

	if( iter != pcsBuddyADChar->m_vtBan.end() )
	{
		iter->SetOnline( pcsBuddyElement->IsOnline() );
		BuddyListUpdate( iter );
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIBuddy::OperationMsgCodeBuddy( EnumAgpmBuddyMsgCode eMsgCode )
{
	ASSERT( AGPMBUDDY_MSG_CODE_NONE != eMsgCode );

	switch( eMsgCode )
	{
	case AGPMBUDDY_MSG_CODE_ALREADY_EXIST :			m_pcsAgcmUIManager2->ThrowEvent( m_lMsgBoxAlreayExist );										break;
	case AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST:	m_pcsAgcmUIManager2->ThrowEvent( m_lMsgBoxOfflineOrNotExist);									break;
	case AGPMBUDDY_MSG_CODE_INVALID_USER:			
	case AGPMBUDDY_MSG_CODE_REFUSE:					AddSystemMessage( ClientStr().GetStr( STI_MAX_CANNOT_REGISTER_BUDDY ) );						break;
	case AGPMBUDDY_MSG_CODE_INVALID_PROCESS:		AddSystemMessage( m_pcsAgcmUIManager2->GetUIMessage( UIMESSAGE_MENTOR_INVALID_PROCESS ) );		break;
	case AGPMBUDDY_MSG_CODE_NOT_INVITE_BUDDY:		AddSystemMessage( m_pcsAgcmUIManager2->GetUIMessage( UIMESSAGE_MENTOR_NOT_INVITE_BUDDY ) );		break;


	default:										ASSERT(!"Msg code Error");													break;
	};

	return TRUE;
}

BOOL AgcmUIBuddy::OperationRegionInfo( AgpdCharacter *pcsCharacter, AgpdBuddyRegion *pcsBuddyRegion )
{
	//. Detail Info에 들어가는 ID, region string 구성하기
	memset( m_szIDBuffer, 0, sizeof( m_szIDBuffer ) );
	memset( m_szRegionBuffer, 0, sizeof( m_szRegionBuffer ) );

	ApmMap::RegionTemplate* pTemplate = m_pcsApmMap->GetTemplate( pcsBuddyRegion->m_lRegionIndex );
	if( pTemplate )
	{
		sprintf( m_szIDBuffer, "%s", pcsBuddyRegion->m_BuddyElement.m_szName.GetBuffer() );

		std::string point = RegionLocalName().GetStr( pTemplate->pStrName );
		sprintf( m_szRegionBuffer, "%s : %s", ClientStr().GetStr( STI_CUR_LOC ), point == "" ? pTemplate->pStrName : point.c_str() );
	}

	return RefreshBuddyDetailInfo();
}

BOOL AgcmUIBuddy::CBCheckBlockWhisper( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pData || !pClass )	return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	CHAR* szSenderName = ( CHAR* )pData;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter ) return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	ASSERT( NULL != pcsBuddyADChar );

	ApVectorBuddy::iterator iter = std::find( pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), szSenderName );
	if( iter != pcsBuddyADChar->m_vtBan.end() )
	{
		return iter->IsBlockWhisper();
	}

	return FALSE;
}

BOOL AgcmUIBuddy::CBCheckBlockTrade( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pData || !pClass )	return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	CHAR* szSenderName = ( CHAR* )pData;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	ASSERT( NULL != pcsBuddyADChar );

	ApVectorBuddy::iterator iter = std::find( pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), szSenderName );
	if( iter != pcsBuddyADChar->m_vtBan.end() )
	{
		return iter->IsBlockTrade();
	}

	return FALSE;
}

BOOL AgcmUIBuddy::CBCheckBlockInvitation( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pData || !pClass )	return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	CHAR* szSenderName = ( CHAR* )pData;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	ASSERT( NULL != pcsBuddyADChar );

	ApVectorBuddy::iterator iter = std::find( pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), szSenderName );
	if( iter != pcsBuddyADChar->m_vtBan.end() )
	{
		return iter->IsBlockInvitation();
	}

	return FALSE;
}

BOOL AgcmUIBuddy::BuddyListUpdate( AgpdBuddyElement* pcsBuddyElement )
{
	if( pcsBuddyElement->IsFriend() ) 
	{
		RefreshMentorList();
		return RefreshFriendList();
	}
	else 
		return RefreshBanList();
}

BOOL AgcmUIBuddy::RefreshFriendList( void )
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	m_pstUDFriendList->m_stUserData.m_lCount = pcsBuddyADChar->m_vtFriend.size();
	return m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDFriendList );
}

BOOL AgcmUIBuddy::RefreshMentorList( void )
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )
		return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar )
		return FALSE;

	m_pstUDMentorList->m_stUserData.m_lCount = pcsBuddyADChar->m_vtFriend.size();

	return m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDMentorList );

}


BOOL AgcmUIBuddy::RefreshBanList( void )
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	m_pstUDBanList->m_stUserData.m_lCount = pcsBuddyADChar->m_vtBan.size();
	return m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDBanList );
}

BOOL AgcmUIBuddy::RefreshBuddyDetailInfo( void )
{
	//. AddUserData()에서 등록한 Dummy UD를 이용하여 Buddy Detail Info Refresh
	return m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDBuddyDetailInfo );
}

//. 친구 리스트에서 해당 ID Delete
BOOL AgcmUIBuddy::CBReturnBuddyOkCancelDelete( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	if( !pClass ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( lTrueCancel )
	{
		//. 친구리스트에서 삭제
		AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if( !pcsCharacter )	return FALSE;

		AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
		if( !pcsBuddyADChar ) return FALSE;

		pThis->SendRemoveBuddy( &pcsBuddyADChar->m_vtFriend[ pThis->m_ICurrentBuddySelectIndex ] );

		//. 지워진 Index와 관련된 member 초기화.
		pThis->m_ICurrentBuddySelectIndex = -1;
		memset( pThis->m_szIDBuffer, 0, sizeof( pThis->m_szIDBuffer ) );
		memset( pThis->m_szRegionBuffer, 0, sizeof( pThis->m_szRegionBuffer ) );
		pThis->RefreshBuddyDetailInfo();
	}

	return TRUE;
}

//. 거부 리스트에서 해당 ID Delete
BOOL AgcmUIBuddy::CBReturnBanOkCancelDelete( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	if( !pClass ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;

	if( lTrueCancel )
	{
		//. 거부리스트에서 삭제
		AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if( !pcsCharacter )	return FALSE;

		AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
		if( !pcsBuddyADChar ) return FALSE;

		pThis->SendRemoveBuddy( &pcsBuddyADChar->m_vtBan[ pThis->m_ICurrentBanSelectIndex ] );

		//. 지워진 Index와 관련된 member 초기화.
		pThis->m_ICurrentBanSelectIndex = -1;
		pThis->m_bBlockWhisper = FALSE;
		pThis->m_bBlockTrade = FALSE;
		pThis->m_bBlockInvitation = FALSE;
		pThis->RefreshBlockOptions();
	}

	return TRUE;
}

// 친구 초대 요청이 와서 컨펌하는 중.
BOOL AgcmUIBuddy::CBReturnOkCancelBuddyAddRequest( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	if( !pClass ) return FALSE;

	AgcmUIBuddy* pThis = static_cast< AgcmUIBuddy* >( pClass );
	if( lTrueCancel )
	{
		// 친구 초대를 한다.
		AgpdBuddyElement buddy;
		buddy.m_szName.SetText( pThis->m_szIDBuffer );
		buddy.SetFriend( TRUE );	//친구/거부리스트에 등록한다.
		pThis->SendAddBuddy( &buddy );
	}
	else
	{
		// 거절을 한다.
		AgpdBuddyElement buddy;
		buddy.m_szName.SetText( pThis->m_szIDBuffer );
		pThis->SendAddReject( &buddy );
	}

	return TRUE;
}

//. 친구 리스트 Display
BOOL AgcmUIBuddy::CBDisplayFriendList( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	INT32 Index = *( INT32* )pData;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter ) return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	if( pcsBuddyADChar->m_vtFriend.size() > pcsSourceControl->m_lUserDataIndex )
	{
		strncpy( szDisplay, pcsBuddyADChar->m_vtFriend[ Index ].m_szName.GetBuffer(), AGPDCHARACTER_NAME_LENGTH );
	}

	return TRUE;
}

//. BuddyList의 item을 선택했을 때의 Display callback.
BOOL AgcmUIBuddy::CBDisplayFriendSelect( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl )	return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	INT32 Index = *( INT32* )pData;

	// 넘어온 인덱스가 범위에 맞지 않는다.
	if( Index >= AGPD_BUDDY_MAX || Index < 0 )
	{
		szDisplay[ 0 ] = NULL;
		return TRUE;
	}

	if( pThis->m_ICurrentBuddySelectIndex == pcsSourceControl->m_lUserDataIndex )
	{
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[ 0 ].m_lStatusID );
	}
	else
	{
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[ 1 ].m_lStatusID );
	}

	return TRUE;
}

BOOL AgcmUIBuddy::CBDisplayMentorSelect( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl )
		return FALSE;

	AgcmUIBuddy*	pThis	=	static_cast< AgcmUIBuddy* >(pClass);
	INT32			nIndex	=	*static_cast< INT32* >(pData);

	if( nIndex >= AGPD_BUDDY_MAX || nIndex < 0 )
	{
		szDisplay[ 0 ]	=	NULL;
		return TRUE;
	}

	if( pThis->m_lCurrentMentorSelectIndex == pcsSourceControl->m_lUserDataIndex )
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[0].m_lStatusID );

	else
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[1].m_lStatusID );
	

	return TRUE;
}

BOOL AgcmUIBuddy::CBDisplayMentorRelation( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pClass )
		return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter ) return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	if( pcsBuddyADChar->m_vtFriend[ pcsSourceControl->m_lUserDataIndex ].GetMentorStatus() == AGSMBUDDY_MENTOR_NONE  )
		sprintf( szDisplay , "X" );

	else
		sprintf( szDisplay , "O" );

	return TRUE;
}


// 친구 등록을 요청한 사람 Display
BOOL AgcmUIBuddy::CBDisplayAddRequestBuddy( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !szDisplay ) return FALSE;

	AgcmUIBuddy* pThis = static_cast< AgcmUIBuddy* >( pClass );
	_tcscpy( szDisplay, pThis->m_szIDBuffer );

	return TRUE;
}

//. 거부 리스트 Display
BOOL AgcmUIBuddy::CBDisplayBanList( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	INT32 Index = *( INT32* )pData;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	if( pcsBuddyADChar->m_vtBan.size() > pcsSourceControl->m_lUserDataIndex )
	{
		strncpy( szDisplay, pcsBuddyADChar->m_vtBan[ Index ].m_szName, AGPDCHARACTER_NAME_LENGTH );
	}

	return TRUE;
}

//. List에 등록된 Buddy의 접속상태(on/off)를 display하는 callback
BOOL AgcmUIBuddy::CBDisplayFriendStatus( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	INT32 Index = *( INT32* )pData;

	// 넘어온 인덱스가 범위에 맞지 않는다.
	if( Index >= AGPD_BUDDY_MAX || Index < 0 )
	{
		szDisplay[ 0 ] = NULL;
		return TRUE;
	}

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	//. 등록한 Buddy가 접속한 상태
	if( pcsBuddyADChar->m_vtFriend[pcsSourceControl->m_lUserDataIndex].IsOnline() )
	{
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[ 0 ].m_lStatusID );
	}
	else
	{
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[ 1 ].m_lStatusID );
	}

	return TRUE;
}

//. List에 등록된 Buddy를 선택했을 경우 표시되어야 하는 세부정보 Display하는 callback
BOOL AgcmUIBuddy::CBDisplayFriendDetailInfo( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl )	return FALSE;

	AgcmUIBuddy *pThis = ( AgcmUIBuddy* )pClass;

	//. 선택된 Index가 있을때만.
	if( pThis->m_ICurrentBuddySelectIndex < AGPD_BUDDY_MAX || pThis->m_ICurrentBuddySelectIndex >= 0 )
	{
		CHAR szBuffer[ 256 ];
		memset( szBuffer, 0, sizeof( CHAR ) * 256 );
		wsprintf( szBuffer, "%s\\n%s", pThis->m_szIDBuffer, pThis->m_szRegionBuffer );
		( ( AcUIEdit* )pcsSourceControl->m_pcsBase )->SetText( szBuffer );
	}
	else
	{
		//. 종료나 초기화 시 Edit Control의 내용을 비운다.
		( ( AcUIEdit* )pcsSourceControl->m_pcsBase )->SetText( "" );
	}

	return TRUE;
}


//. BanList의 item을 선택했을 때의 Display callback.
BOOL AgcmUIBuddy::CBDisplayBanSelect( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl )	return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	INT32 Index = *( INT32* )pData;

	// 넘어온 인덱스가 범위에 맞지 않는다.
	if( Index >= AGPD_BUDDY_MAX || Index < 0 )
	{
		szDisplay[ 0 ] = NULL;
		return TRUE;
	}

	if( pThis->m_ICurrentBanSelectIndex == pcsSourceControl->m_lUserDataIndex )
	{
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[ 0 ].m_lStatusID );
	}
	else
	{
		pcsSourceControl->m_pcsBase->SetStatus( pcsSourceControl->m_pcsBase->m_astStatus[ 1 ].m_lStatusID );
	}

	return TRUE;
}

//. 친구 리스트UI의 '친구 등록' Button Click Function
BOOL AgcmUIBuddy::CBBtnAddDlgOpenClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;

	//. 현재 추가할 타겟리스트는 친구리스트
	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	
	//. throwEvent로 Buddy Add Dialog를 Open한다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventAddBuddyOpen );
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventAddFriendDlgSetFocus );
	return TRUE;
}

//. 친구 등록 Dialog의 '등록' Button Click Function
BOOL AgcmUIBuddy::CBBtnAddBuddyClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	CHAR* szID = NULL;
	AcUIEdit* pUIEdit = NULL;

	if( pData1 )
	{
		AgcdUIControl* pUIControl = ( AgcdUIControl* )pData1;
		if( AcUIBase::TYPE_EDIT != pUIControl->m_lType ) return FALSE;

		pUIEdit = ( AcUIEdit* )pUIControl->m_pcsBase;
		szID = ( CHAR* )pUIEdit->GetText();
	}
	else
	{
		AgpdCharacter* pcsTargetCharacter = ( AgpdCharacter* )pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;
		if( pcsTargetCharacter )
		{
			szID = pcsTargetCharacter->m_szID;
		}
	}

	if( szID && strlen( szID ) < AGPDCHARACTER_NAME_LENGTH )
	{
		AgpdBuddyElement buddy;
		buddy.m_szName.SetText( szID );
		buddy.SetFriend( TRUE );	//친구/거부리스트에 등록한다.
		pThis->SendAddRequest( &buddy );
	}
	else
	{
		if( pUIEdit )
		{
			pUIEdit->SetText( "" );
		}

		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_IMsgBoxInvalidUserIDLength );
	}

	return TRUE;
}

//. 친구삭제 Button Click Function
BOOL AgcmUIBuddy::CBBtnDeleteBuddyClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pThis->m_ICurrentBuddySelectIndex >= AGPD_BUDDY_MAX || pThis->m_ICurrentBuddySelectIndex < 0 )
	{
		//. 친구리스트에서 최소한개의 아이템을 선택 -> 시스템메세지
		pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BUDDYLIST_SELECT_NONE ) );
		return FALSE;
	}

	return pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_IEventBuddyDeleteQueryOpen );
}

//. '파티초대' Button Click Function 
BOOL AgcmUIBuddy::CBBtnInviteBuddyClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pThis->m_ICurrentBuddySelectIndex >= AGPD_BUDDY_MAX || pThis->m_ICurrentBuddySelectIndex < 0 )
	{
		//. 친구리스트에서 최소한개의 아이템을 선택 -> 시스템메세지
		pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BUDDYLIST_SELECT_NONE ) );
		return FALSE;
	}

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter ) return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;
	if( pcsBuddyADChar->m_vtFriend[ pThis->m_ICurrentBuddySelectIndex ].m_szName.IsEmpty() ) return FALSE;

	if( pThis->m_pcsAgcmParty->SendPacketInviteByCharID( pcsCharacter->m_lID, pcsBuddyADChar->m_vtFriend[ pThis->m_ICurrentBuddySelectIndex ].m_szName.GetBuffer() ) )
	{
		//. 2005. 12. 19. Nonstopdj
		//. 메세지 추가 : "%s님에게 파티초대를 하였습니다."
		ApString< 96 > strTemp;
		strTemp.Format( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BUDDYLIST_INVITE_SEND ), pcsBuddyADChar->m_vtFriend[ pThis->m_ICurrentBuddySelectIndex ].m_szName.GetBuffer() );
		pThis->AddSystemMessage( strTemp );
	}

	return TRUE;
}

//. '귓말' Button Click Function 
BOOL AgcmUIBuddy::CBBtnWhisperMsgBuddyClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pThis->m_ICurrentBuddySelectIndex >= AGPD_BUDDY_MAX || pThis->m_ICurrentBuddySelectIndex < 0 )
	{
		//. 친구리스트에서 최소한개의 아이템을 선택 -> 시스템메세지
		pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BUDDYLIST_SELECT_NONE ) );
		return FALSE;
	}

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;
	if( pcsBuddyADChar->m_vtFriend[ pThis->m_ICurrentBuddySelectIndex ].m_szName.IsEmpty() ) return FALSE;

	ApString< 64 > strWhisperMsg;
	strWhisperMsg.Format( _T( "%s %s " ), ClientStr().GetStr( STI_CHAT_WHISPER_LOWER ), pcsBuddyADChar->m_vtFriend[ pThis->m_ICurrentBuddySelectIndex ].m_szName.GetBuffer() );

	// Chat UI를 Open후 귓속말 메세지를 추가한다.
	pThis->m_pcsAgcmUIChatting2->ChatInputNormalUIOpen();
	return pThis->m_pcsAgcmUIChatting2->SetChattingMessage( strWhisperMsg );
}

//. '등록된' Buddy Click Function
BOOL AgcmUIBuddy::CBBuddySelect( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= AGPD_BUDDY_MAX ) return FALSE;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	if( pThis->m_ICurrentBuddySelectIndex != pcsSourceControl->m_lUserDataIndex )
	{
		//. 이전에 선택되었던 ItemIndex가 현재의 선택Index와 다르다면.
		pThis->m_ICurrentBuddySelectIndex = pcsSourceControl->m_lUserDataIndex;

		//. 선택이 된 상태로 Refresh한다.
		pThis->RefreshFriendList();

		//. Packet를 보낸다.
		pThis->SendRegion( &pcsBuddyADChar->m_vtFriend[ pcsSourceControl->m_lUserDataIndex ] );
	}

	return TRUE;
}

BOOL AgcmUIBuddy::CBMentorSelect( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl )
		return FALSE;

	AgcmUIBuddy* pThis		=	static_cast< AgcmUIBuddy* >(pClass);
	if( pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= AGPD_BUDDY_MAX )
		return FALSE;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )
		return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar	=	pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar )
		return FALSE;

	if( pThis->m_lCurrentMentorSelectIndex != pcsSourceControl->m_lUserDataIndex )
	{
		//. 이전에 선택되었던 ItemIndex가 현재의 선택Index와 다르다면.
		pThis->m_lCurrentMentorSelectIndex = pcsSourceControl->m_lUserDataIndex;

		//. 선택이 된 상태로 Refresh한다.
		pThis->RefreshMentorList();
	}

	// 초대 / 해제 버튼을 설정한다

	if( pcsBuddyADChar->m_vtFriend[ pThis->m_lCurrentMentorSelectIndex ].GetMentorStatus() == AGSMBUDDY_MENTOR_NONE )
	{
		sprintf_s( pThis->m_szMentorBtnID , MENTOR_BUTTON_MAX_LENGTH , ClientStr().GetStr( STI_MENTOR_INVITE ) );
	}

	else
	{
		sprintf_s( pThis->m_szMentorBtnID , MENTOR_BUTTON_MAX_LENGTH , ClientStr().GetStr( STI_MENTOR_RELEASE ) );
		pThis->m_bEnableInviteButton	=	TRUE;
	}

	pThis->m_bEnableInviteButton	=	TRUE;
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstEnableInviteBtn );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstMentorBtnID );

	return TRUE;

}

BOOL AgcmUIBuddy::CBBuddyAddDlgSetFocus( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgcdUIControl* pUIControl = ( AgcdUIControl* )pData1;
	if( AcUIBase::TYPE_EDIT != pUIControl->m_lType ) return FALSE;

	AcUIEdit* pUIEdit = ( AcUIEdit* )pUIControl->m_pcsBase;
	pUIEdit->SetText( _T( "" ) );
	pUIEdit->SetMeActiveEdit();
	return TRUE;
}

//. '대화차단' Button Click funtion
BOOL AgcmUIBuddy::CBBtnBlockWhisperClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pThis->m_ICurrentBanSelectIndex >= AGPD_BUDDY_MAX || pThis->m_ICurrentBanSelectIndex < 0 )
	{
		//. 거부리스트에서 최소한개의 아이템을 선택 -> 시스템메세지
		pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_SELECT_NONE ) );
		pThis->m_bBlockWhisper = FALSE;
		pThis->RefreshBlockOptions();
		return FALSE;
	}

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	BOOL bIsCurrentStatus;
	if( pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON )
	{
		bIsCurrentStatus = ( ( ( AcUIButton* )pcsSourceControl->m_pcsBase )->GetButtonMode() == ACUIBUTTON_MODE_CLICK );
	}
	
	pcsBuddyADChar->m_vtBan[ pThis->m_ICurrentBanSelectIndex ].SetBlockWhisper( bIsCurrentStatus );

	//. Packet를 보낸다.
	return pThis->SendOption( &pcsBuddyADChar->m_vtBan[ pThis->m_ICurrentBanSelectIndex ] );
}

//. '거래차단' Button Click funtion
BOOL AgcmUIBuddy::CBBtnBlockTradeClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pThis->m_ICurrentBanSelectIndex >= AGPD_BUDDY_MAX || pThis->m_ICurrentBanSelectIndex < 0 )
	{
		//. 거부리스트에서 최소한개의 아이템을 선택 -> 시스템메세지
		pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_SELECT_NONE ) );
		pThis->m_bBlockTrade = FALSE;
		pThis->RefreshBlockOptions();
		return FALSE;
	}

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	BOOL bIsCurrentStatus;
	if( pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON )
	{
		bIsCurrentStatus = ( ( ( AcUIButton* )pcsSourceControl->m_pcsBase )->GetButtonMode() == ACUIBUTTON_MODE_CLICK );
	}

	pcsBuddyADChar->m_vtBan[ pThis->m_ICurrentBanSelectIndex ].SetBlockTrade( bIsCurrentStatus );

	//. Packet를 보낸다.
	return pThis->SendOption( &pcsBuddyADChar->m_vtBan[ pThis->m_ICurrentBanSelectIndex ] );
}

//. '초대차단' Button Click funtion
BOOL AgcmUIBuddy::CBBtnBlockInvitationClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pThis->m_ICurrentBanSelectIndex >= AGPD_BUDDY_MAX || pThis->m_ICurrentBanSelectIndex < 0 )
	{
		//. 거부리스트에서 최소한개의 아이템을 선택 -> 시스템메세지
		pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_SELECT_NONE ) );
		pThis->m_bBlockInvitation = FALSE;
		pThis->RefreshBlockOptions();
		return FALSE;
	}

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	BOOL bIsCurrentStatus;
	if( pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON )
	{
		bIsCurrentStatus = ( ( ( AcUIButton* )pcsSourceControl->m_pcsBase )->GetButtonMode() == ACUIBUTTON_MODE_CLICK );
	}

	pcsBuddyADChar->m_vtBan[ pThis->m_ICurrentBanSelectIndex ].SetBlockInvitation( bIsCurrentStatus );

	//. Packet를 보낸다.
	return pThis->SendOption( &pcsBuddyADChar->m_vtBan[pThis->m_ICurrentBanSelectIndex ] );
}


//. 거부 리스트UI의 '거부등록' Button Click funtion
BOOL AgcmUIBuddy::CBBtnAddBanDlgOpenClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;

	//. 현재 추가할 타겟리스트는 거부리스트
	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;

	//. throwEvent로 Buddy Add Dialog를 Open한다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventBuddyBanAddOpen );
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventAddBanDlgSetFocus );
	return TRUE;
}

//. 거부 등록 Dialog의 '등록' Button Click Function
BOOL AgcmUIBuddy::CBBtnAddRejectionListClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	CHAR* szID = NULL;
	AcUIEdit* pUIEdit = NULL;

	if( pData1 )
	{
		AgcdUIControl* pUIControl = ( AgcdUIControl* )pData1;
		if( AcUIBase::TYPE_EDIT != pUIControl->m_lType ) return FALSE;

		pUIEdit = ( AcUIEdit* )pUIControl->m_pcsBase;
		szID = ( CHAR* )pUIEdit->GetText();
	}
	else
	{
		AgpdCharacter* pcsTargetCharacter = ( AgpdCharacter* )pcsSourceControl->m_pstUserData->m_stUserData.m_pvData;
		if( pcsTargetCharacter )
		{
			szID = pcsTargetCharacter->m_szID;
		}
	}

	if( szID && strlen( szID ) < AGPDCHARACTER_NAME_LENGTH )
	{
		AgpdBuddyElement buddy;
		buddy.m_szName.SetText( szID );
		buddy.SetFriend( FALSE );	//거부리스트에 등록한다.
		pThis->SendAddBuddy( &buddy );
	}
	else
	{
		if( pUIEdit )
		{
			pUIEdit->SetText( "" );
		}

		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_IMsgBoxInvalidUserIDLength );
	}

	return TRUE;
}

//. 거부 리스트UI의 '거부해제' Button Click Function
BOOL AgcmUIBuddy::CBBtnDeleteRejectionListClick( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pThis->m_ICurrentBanSelectIndex >= AGPD_BUDDY_MAX || pThis->m_ICurrentBanSelectIndex < 0 )
	{
		//. 거부리스트에서 최소한개의 아이템을 선택 -> 시스템메세지
		pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_BANLIST_SELECT_NONE ) );
		return FALSE;
	}

	return pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_IEventBuddyBanDeleteQueryOpen );
}

//. 등록된 BanList Selection Function
BOOL AgcmUIBuddy::CBBanListSelect( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	if( pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= AGPD_BUDDY_MAX ) return FALSE;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	if( pThis->m_ICurrentBanSelectIndex != pcsSourceControl->m_lUserDataIndex )
	{
		//. 이전에 선택되었던 ItemIndex가 현재의 선택Index와 다르다면.
		pThis->m_ICurrentBanSelectIndex = pcsSourceControl->m_lUserDataIndex;

		//. 선택이 된 상태로 Refresh한다.
		pThis->RefreshBanList();
		pThis->BlockOptionUpdate( &pcsBuddyADChar->m_vtBan[ pcsSourceControl->m_lUserDataIndex ] );
	}

	return TRUE;
}

BOOL AgcmUIBuddy::CBBanAddDlgSetFocus( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pData1 ) return FALSE;

	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgcdUIControl* pUIControl = ( AgcdUIControl* )pData1;
	if( AcUIBase::TYPE_EDIT != pUIControl->m_lType ) return FALSE;

	AcUIEdit* pUIEdit = ( AcUIEdit* )pUIControl->m_pcsBase;
	pUIEdit->SetText( _T( "" ) );
	pUIEdit->SetMeActiveEdit();
	return TRUE;
}

BOOL AgcmUIBuddy::SendAddBuddy( AgpdBuddyElement* pcsBuddyElement )
{
	INT16 nPacketLength = 0;
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT( NULL != pcsCharacter );
	if( !pcsCharacter )	return FALSE;

	PVOID pvPacket= m_pcsAgpmBuddy->MakePacketAddBuddy( &nPacketLength, pcsCharacter, pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL bResult = SendPacket( pvPacket, nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
	return bResult;
}

BOOL AgcmUIBuddy::SendAddRequest( AgpdBuddyElement* pcsBuddyElement )
{
	INT16 nPacketLength = 0;
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT( NULL != pcsCharacter );
	if( !pcsCharacter )	return FALSE;

	PVOID pvPacket= m_pcsAgpmBuddy->MakePacketAddRequest( &nPacketLength, pcsCharacter, pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL bResult = SendPacket( pvPacket, nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
	return bResult;
}

BOOL AgcmUIBuddy::SendAddReject( AgpdBuddyElement* pcsBuddyElement )
{
	INT16 nPacketLength = 0;
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT( NULL != pcsCharacter );
	if( !pcsCharacter )	return FALSE;

	PVOID pvPacket= m_pcsAgpmBuddy->MakePacketAddReject( &nPacketLength, pcsCharacter, pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL bResult = SendPacket( pvPacket, nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
	return bResult;
}

BOOL AgcmUIBuddy::SendRemoveBuddy( AgpdBuddyElement* pcsBuddyElement )
{
	INT16 nPacketLength = 0;
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT( NULL != pcsCharacter );
	if( !pcsCharacter ) return FALSE;

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketRemoveBuddy( &nPacketLength, pcsCharacter, pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL bResult = SendPacket( pvPacket, nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
	return bResult;
}

BOOL AgcmUIBuddy::SendRegion( AgpdBuddyElement* pcsBuddyElement )
{
	INT16 nPacketLength = 0;
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT( NULL != pcsCharacter );
	if( !pcsCharacter ) return FALSE;

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketRegion( &nPacketLength, pcsCharacter, pcsBuddyElement, 0 );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL bResult = SendPacket( pvPacket, nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
	return bResult;
}

BOOL AgcmUIBuddy::SendOption( AgpdBuddyElement* pcsBuddyElement )
{
	INT16 nPacketLength = 0;
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	ASSERT( NULL != pcsCharacter );
	if( !pcsCharacter )	return FALSE;

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketUpdateOptions( &nPacketLength, pcsCharacter, pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL bResult = SendPacket( pvPacket, nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
	return bResult;
}

BOOL AgcmUIBuddy::SendMentorRequestAccept( AgpdBuddyElement* pcsBuddyElement )
{
	INT16			nPacketLength	=	0;
	AgpdCharacter*	pcsCharacter	=	m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )
		return FALSE;

	PVOID			pvPacket		=	m_pcsAgpmBuddy->MakePacketMentorRequestAccept( &nPacketLength , pcsCharacter , pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL	bResult	=	SendPacket( pvPacket , nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );

	return bResult;
}

BOOL AgcmUIBuddy::SendMentorRequestReject( AgpdBuddyElement* pcsBuddyElement )
{
	INT16			nPacketLength	=	0;
	AgpdCharacter*	pcsCharacter	=	m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )
		return FALSE;

	PVOID			pvPacket		=	m_pcsAgpmBuddy->MakePacketMentorRequestReject( &nPacketLength , pcsCharacter , pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL	bResult	=	SendPacket( pvPacket , nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );

	return bResult;
}

BOOL AgcmUIBuddy::SendMentorDelete( AgpdBuddyElement* pcsBuddyElement )
{
	INT16			nPacketLength	=	0;
	AgpdCharacter*	pcsCharacter	=	m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )
		return FALSE;

	PVOID			pvPacket		=	m_pcsAgpmBuddy->MakePacketMentorDelete( &nPacketLength , pcsCharacter , pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL	bResult	=	SendPacket( pvPacket , nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );

	return bResult;
}

BOOL AgcmUIBuddy::SendMentorRequest( AgpdBuddyElement* pcsBuddyElement )
{
	INT16			nPacketLength	=	0;
	AgpdCharacter*	pcsCharacter	=	m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )
		return FALSE;

	PVOID			pvPacket		=	m_pcsAgpmBuddy->MakePacketMentorRequest( &nPacketLength , pcsCharacter , pcsBuddyElement );
	if( !pvPacket )
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );
		return FALSE;
	}

	BOOL	bResult	=	SendPacket( pvPacket , nPacketLength );
	m_pcsAgpmBuddy->m_csPacket.FreePacket( pvPacket );

	return bResult;
}

// script
void AgcmUIBuddy::AddFriend( void )
{
	AgpdBuddyElement buddy;
	buddy.m_szName.SetText( "꿍쓰" );
	buddy.SetFriend( TRUE );
	SendAddBuddy( &buddy );
}

void AgcmUIBuddy::RemoveFriend( void )
{
	AgpdBuddyElement buddy;
	buddy.m_szName.SetText( "꿍쓰" );
	buddy.SetFriend( TRUE );

	SendRemoveBuddy( &buddy );
}

void AgcmUIBuddy::AddBan( void )
{
	AgpdBuddyElement buddy;
	buddy.m_szName.SetText( "꿍쓰" );
	buddy.SetFriend( FALSE );

	SendAddBuddy( &buddy );
}

void AgcmUIBuddy::RemoveBan( void )
{
	AgpdBuddyElement buddy;
	buddy.m_szName.SetText( "꿍쓰" );
	buddy.SetFriend( FALSE );
	SendRemoveBuddy( &buddy );
}

void AgcmUIBuddy::OpenAddDialog( void )
{
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventAddBuddyOpen );
}

void AgcmUIBuddy::OpenFriendList( void )
{
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventMainUIOpen );
}

void AgcmUIBuddy::OpenBanList( void )
{
	m_pcsAgcmUIManager2->ThrowEvent( m_lEventBanUIOpen );
}

//. 시스템 메세지에 메세지 등록하기
BOOL AgcmUIBuddy::AddSystemMessage( CHAR* szMsg )
{
	if( !szMsg ) return FALSE;

	AgpdChatData stChatData;
	memset( &stChatData, 0, sizeof( stChatData ) );

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = szMsg;

	stChatData.lMessageLength = strlen( stChatData.szMessage );
		
	m_pcsAgcmChatting2->AddChatMessage( AGCMCHATTING_TYPE_SYSTEM, &stChatData );
	m_pcsAgcmChatting2->EnumCallback( AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL );
	return TRUE;
}

BOOL AgcmUIBuddy::CBNotInBuddyList( PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl )
{
	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgpdCharacter* pcsTargetCharacter = ( AgpdCharacter* )pData;
	if( !pThis->m_pcsAgpmCharacter->IsPC( pcsTargetCharacter ) ) return FALSE;

	AgpdCharacter* pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter || !pcsTargetCharacter ) return FALSE;

	return !pThis->m_pcsAgpmBuddy->IsExistBuddy( pcsSelfCharacter, pcsTargetCharacter->m_szID, TRUE, FALSE );
}

BOOL AgcmUIBuddy::CBNotInBanList( PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl )
{
	AgcmUIBuddy* pThis = ( AgcmUIBuddy* )pClass;
	AgpdCharacter* pcsTargetCharacter = ( AgpdCharacter* )pData;
	if( !pThis->m_pcsAgpmCharacter->IsPC( pcsTargetCharacter ) ) return FALSE;

	AgpdCharacter* pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter || !pcsTargetCharacter ) return FALSE;

	return !pThis->m_pcsAgpmBuddy->IsExistBuddy( pcsSelfCharacter, pcsTargetCharacter->m_szID, FALSE, TRUE );
}

BOOL AgcmUIBuddy::CBMentorAddAndDel( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{

	if( !pClass || !pcsSourceControl ) 
		return FALSE;

	AgcmUIBuddy*	pThis		=	static_cast< AgcmUIBuddy* >(pClass);
	INT				nAnswer		=	0;
	CHAR			szMessage[ MAX_PATH ];

	if( pThis->m_lCurrentMentorSelectIndex >= AGPD_BUDDY_MAX || pThis->m_lCurrentMentorSelectIndex < 0 )
	{
		// 선택해야 되는데 선택이 안되어 있다
		//pThis->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage(  ) );
		return FALSE;
	}

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )	return FALSE;

	AgpdBuddyADChar* pcsBuddyADChar = pThis->m_pcsAgpmBuddy->GetAttachAgpdBuddyData( pcsCharacter );
	if( !pcsBuddyADChar ) return FALSE;

	BOOL bIsCurrentStatus;
	if( pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON )
	{
		bIsCurrentStatus = ( ( ( AcUIButton* )pcsSourceControl->m_pcsBase )->GetButtonMode() == ACUIBUTTON_MODE_CLICK );
	}

	// Send
	if( pcsBuddyADChar->m_vtFriend[ pThis->m_lCurrentMentorSelectIndex ].GetMentorStatus() != AGSMBUDDY_MENTOR_NONE )
	{
		sprintf_s( szMessage , MAX_PATH , pThis->m_pcsAgcmUIManager2->GetUIMessage( UIMESSAGE_MENTOR_DELETE_CONFIRM ) , pcsBuddyADChar->m_vtFriend[ pThis->m_lCurrentMentorSelectIndex ].m_szName.GetBuffer() );
		nAnswer			=		pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szMessage );

		if( nAnswer )
			return pThis->SendMentorDelete( &pcsBuddyADChar->m_vtFriend[ pThis->m_lCurrentMentorSelectIndex ] );
	}

	else
	{	
		sprintf_s( szMessage , MAX_PATH , pThis->m_pcsAgcmUIManager2->GetUIMessage( UIMESSAGE_MENTOR_REQUEST_CONFIRM ) , pcsBuddyADChar->m_vtFriend[ pThis->m_lCurrentMentorSelectIndex ].m_szName.GetBuffer() );
		nAnswer			=		pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szMessage );

		if( nAnswer )
			return pThis->SendMentorRequest( &pcsBuddyADChar->m_vtFriend[ pThis->m_lCurrentMentorSelectIndex ] );
	}

	return TRUE;
}

VOID AgcmUIBuddy::OperationMentorUIOpen( VOID )
{
	if( !m_lEventMentorUIOpen )
		return;

	sprintf_s( m_szMentorBtnID , MENTOR_BUTTON_MAX_LENGTH , ClientStr().GetStr( STI_MENTOR_INVITE ) );
	m_bEnableInviteButton	=	FALSE;

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstEnableInviteBtn );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstMentorBtnID );

	m_lCurrentMentorSelectIndex		=	-1;

	m_pcsAgcmUIManager2->ThrowEvent( m_lEventMentorUIOpen );
}

VOID AgcmUIBuddy::OperationMentorUIClose( VOID )
{
	if( !m_lEventMentorUIClose )
		return;

	m_pcsAgcmUIManager2->ThrowEvent( m_lEventMentorUIClose );
}

VOID AgcmUIBuddy::OperationMentorRequestConfirm( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement )
{
	CHAR		szMessage[ MAX_PATH ];
	INT			nAnswer		=	0;

	ZeroMemory( szMessage , MAX_PATH );
	sprintf_s( szMessage , MAX_PATH , m_pcsAgcmUIManager2->GetUIMessage( UIMESSAGE_MENTOR_REQUEST_CONFIRM ) , pcsBuddyElement->m_szName.GetBuffer() );
	
	nAnswer			=		m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szMessage );

	if( nAnswer )
		SendMentorRequestAccept( pcsBuddyElement );

	else
		SendMentorRequestReject( pcsBuddyElement );

}

VOID AgcmUIBuddy::OperationMentorDelete( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement )
{
	if( !pcsCharacter || !pcsBuddyElement )
		return;

	CHAR		szMessage[ MAX_PATH ];
	ZeroMemory( szMessage , MAX_PATH );

	sprintf_s( szMessage , MAX_PATH , m_pcsAgcmUIManager2->GetUIMessage( UIMESSAGE_MENTOR_DELETE ) , pcsCharacter->m_szID , pcsBuddyElement->m_szName.GetBuffer() );
	AddSystemMessage( szMessage );

	m_pcsAgpmBuddy->MentorBuddy( pcsCharacter , pcsBuddyElement );

	RefreshMentorList();
}

VOID AgcmUIBuddy::OperationMentorRequestAccept( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement )
{
	if( !pcsCharacter || !pcsBuddyElement )
		return;

	CHAR		szMessage[ MAX_PATH ];
	ZeroMemory( szMessage , MAX_PATH );

	sprintf_s( szMessage , MAX_PATH , m_pcsAgcmUIManager2->GetUIMessage( UIMESSAGE_MENTOR_REQUEST_ACCEPT ) , pcsCharacter->m_szID , pcsBuddyElement->m_szName.GetBuffer() );

	AddSystemMessage( szMessage );
	m_pcsAgpmBuddy->MentorBuddy( pcsCharacter , pcsBuddyElement );

	RefreshMentorList();

}

BOOL AgcmUIBuddy::CBDisplayMentorBtnText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if( !pClass || !pcsSourceControl )
		return FALSE;

	AgcmUIBuddy* pThis = static_cast<AgcmUIBuddy*>(pClass);
	sprintf( szDisplay , "%s" , pThis->m_szMentorBtnID );

	return TRUE;
}
