#include "AgcmUILogin.h"
#include "AgcmUIEventTeleport.h"
#include "AgcmReturnToLogin.h"
#include "AgcmUIItem.h"
#include "AgpmQuest.h"
#include "AgpmRefinery.h"
#include "AgcmUICharacter.h"
#include "AgcmEventPointLight.h"
#include "AgpmEventCharCustomize.h"
#include "AgcmGrass.h"
#include "AgcmUIOption.h"
#include "AgcmUIGuild.h"
#include "AgcmWorld.h"

#include "AgcmCamera2.h"
#include "AgcdEffGlobal.h"
#include "AuStrTable.h"
#include "AgcmTextBoardMng.h"
#include "AgcmEventNPCDialog.h"
#include "AgcmUIMailBox.h"
#include "AgcmTitle.h"
#include "AgcmRegistryManager.h"

	#ifdef _AREA_KOREA_
	#include "CWebzenAuth.h"
	#elif defined(_AREA_JAPAN_)
		#include "AuJapaneseClientAuth.h"
#endif

#include "AuIgnoringHelper.h"


//AuPOS posAgcmUILoginHumanMyCharacterPos[] = {
//	{-509985.687500f,7585.757813f,-731600.500000f},//{ -709662.875000f,-0.421406f,-709644.0f	},		//	HUMAN 0 Position
//	{-509989.781250f,7585.757813f,-731731.312500f},//{ -709754.500000f,-0.192181f,-709510.875000f	},		//	HUMAN 1 Position
//	{-509993.718750f,7585.757813f,-731460.625000f},//{ -709661.875000f,-0.422656f,-709770.312500f	},		//	HUMAN 2 Position
//	{ -709850.812500f,0.005504f,-709444.000000f		},		//	HUMAN 3 Position
//	{ -709662.875000f,100.0f,-709644.0f				},		//	HUMAN 0 Position
//	{ -709754.500000f,100.0f,-709510.875000f		},		//	HUMAN 1 Position
//	{ 223840.890625f, 203.0f, 317051.156250f		}			//  HUMAN 4 Position - 현재는 0 ~ 3 까지만 존재
//};
//
//
AuPOS posAgcmUILoginOrcMyCharacterPos[5] = {			
	{ 449631.6f,2.0f,677856.3f },						// ORC 0 Position
	{ 449741.3f,2.0f,677795.0f },						// ORC 1 Position
	{ 449894.9f,2.0f,677791.5f },						// ORC 2 Position
	{ 450016.4f,2.0f,677879.8f },						// ORC 3 Position
	{ 225126.015625f, 383.838654f, 331866.531250f }		// ORC 4 Position
};


//float	fAgcmUILoginHumanMyCharacterAngle	[] = { -121.0f, -91.0f, -60.0f , -142.0f, -258.0f , -99.0f, -123.0f };//{ -99.0f, -123.0f, -75.0f , -142.0f, -258.0f , -99.0f, -123.0f };
float	fAgcmUILoginOrcMyCharacterAngle		[5] = { 47.0f, 14.0f, -14.0f, -47.0f, -56.0f };

//. 2006. 10. 9. Nonstopdj
INT32 g_ISelectCID = 0;
static AgcmUILogin *	g_pcsAgcmUILogin = NULL;

/****************************************************/
/*		The Implementation of AccmUILogin class		*/
/****************************************************/
//
TCHAR AgcmUILogin::s_szEvent[AGCMUILOGIN_EVENT_MAX][50] =
{
	_T("LG_IDPASSWORD_INPUT_OPEN"),				// AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_OPEN = 0,
	_T("LG_IDPASSWORD_1280_INPUT_OPEN"),		// AGCMUILOGIN_EVENT_ID_PASSWORD_1280_INPUT_OPEN,
	_T("LG_IDPASSWORD_1600_INPUT_OPEN"),		// AGCMUILOGIN_EVENT_ID_PASSWORD_1600_INPUT_OPEN,
	_T("LG_IDPASSWORD_INPUT_CLOSE"),			// AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_CLOSE,
	_T("LG_2D_OPEN"),							// AGCMUILOGIN_EVENT_2D_OPEN,
	_T("LG_2D_1280_OPEN"),						// AGCMUILOGIN_EVENT_2D_1280_OPEN,
	_T("LG_2D_1600_OPEN"),						// AGCMUILOGIN_EVENT_2D_1600_OPEN,
	_T("LG_2D_CLOSE"),							// AGCMUILOGIN_EVENT_2D_CLOSE,
	_T("LG_UNION_SELECT_OPEN"),					// AGCMUILOGIN_EVENT_UNION_SELECT_OPEN,
	_T("LG_UNION_SELECT_CLOSE"),				// AGCMUILOGIN_EVENT_UNION_SELECT_CLOSE,
	_T("LG_CHAR_CREATE_OPEN"),					// AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN,
	_T("LG_CHAR_CREATE_OPEN_1280"),					// AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN,
	_T("LG_CHAR_CREATE_OPEN_1600"),					// AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN,
	_T("LG_CHAR_CREATE_CLOSE"),					// AGCMUILOGIN_EVENT_CHARACTER_CREATE_CLOSE,
	_T("LG_MAIN_BUTTONS_OPEN"),					// AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_OPEN,
	_T("LG_MAIN_BUTTONS_CLOSE"),				// AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_CLOSE,
	_T("LG_IDPASSWORD_FAILD"),					// AGCMUILOGIN_EVENT_ID_PASSWORD_FALIED,
	_T("LG_CHAR_CREATE_FAILD_SAME_NAME"),		// AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAILED_SAME_NAME,
	_T("LG_CONNECT_LOGIN_SERVER"),				// AGCMUILOGIN_EVENT_CONNECT_LOGIN_SERVER,
	_T("LG_LOADING_WINDOW_OPEN"),				// AGCMUILOGIN_EVENT_LOADING_WINDOW_OPEN,
	_T("LG_LOADING_WINDOW_CLOSE"),				// AGCMUILOGIN_EVENT_LOADING_WINDOW_CLOSE,
	_T("LG_GAME_SERVER_NOT_READY"),				// AGCMUILOGIN_EVENT_GAME_SERVER_NOT_READY,
	_T("LG_CHAR_CREATE_NAME_INVALID"),			// AGCMUILOGIN_EVENT_INVALID_CHAR_NAME_INPUT,
	_T("LG_CHAR_CREATE_HELP_OPEN"),				// AGCMUILOGIN_EVENT_CHARACTER_CREATE_HELP_OPEN,
	_T("LG_CHAR_CREATE_HELP_CLOSE"),			// AGCMUILOGIN_EVENT_CHARACTER_CREATE_HELP_CLOSE,
	_T("LG_CHAR_CREATE_FAILD_INVALID_NAME"),	// AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAILED_INVALID_NAME,
	_T("LG_DISCONNECT_BY_INVALID_PW"),			// AGCMUILOGIN_EVENT_DISCONNECT_BY_INVALID_PW,
	_T("LG_CANNOT_REMOVE_DUPLICATE_ACCOUNT"),	// AGCMUILOGIN_EVENT_CANNOT_REMOVE_DUPLICATE_ACCOUNT,
	_T("LG_DISCONNECT_BY_ANOTHER_USER"),		// AGCMUILOGIN_EVENT_DISCONNECT_BY_ANOTHER_USER,
	_T("EXIT_Open_Window"),						// AGCMUILOGIN_EVENT_OPEN_EXIT_WINDOW,
	_T("LG_CONNECT_FAILED_LOGIN_SERVER"),		// AGCMUILOGIN_EVENT_CONNECT_FAILED_LOGIN_SERVER,
	_T("LG_PASSWORD_INCORRECT"),				// AGCMUILOGIN_EVENT_PASSWORD_INCORRECT,
	_T("LG_OpenSelectServer"),					// AGCMUILOGIN_EVENT_OPEN_SELECT_SERVER_WINDOW,
	_T("LG_CloseSelectServer"),					// AGCMUILOGIN_EVENT_CLOSE_SELECT_SERVER_WINDOW,
	_T("LG_CHAR_CREATE_NOT_SELECTED"),			// AGCMUILOGIN_EVENT_CHARACTER_CREATE_NOT_SELECTED,
	_T("LG_EnterGameNotSelectedChar"),			// AGCMUILOGIN_EVENT_ENTER_GAME_NOT_SELECTED_CHAR,
	_T("LG_LoadingHumanBackground"),			// AGCMUILOGIN_EVENT_LOADING_HUMAN_BACKGROUND,
	_T("LG_LoadingOrcBackground"),				// AGCMUILOGIN_EVENT_LOADING_ORC_BACKGROUND,
	_T("LG_LoadingMoonElfBackground"),			// AGCMUILOGIN_EVENT_LOADING_MOONELF_BACKGROUND,
	_T("LG_OnCharMark"),						// AGCMUILOGIN_EVENT_ON_CHARACTER_MARK,	
	_T("LG_OffCharMark"),						// AGCMUILOGIN_EVENT_OFF_CHARACTER_MARK
	_T("LG_OnSelectServerBar"),					// AGCMUILOGIN_EVENT_ON_SELECT_SERVER_BAR
	_T("LG_OffSelectServerBar"),				// AGCMUILOGIN_EVENT_OFF_SELECT_SERVER_BAR
	_T("LG_ServerStatusGood"),					// AGCMUILOGIN_EVENT_SERVER_STATUS_GOOD
	_T("LG_ServerStatusNormal"),				// AGCMUILOGIN_EVENT_SERVER_STATUS_NORMAL
	_T("LG_ServerStatusBusy"),					// AGCMUILOGIN_EVENT_SERVER_STATUS_BUSY
	_T("LG_ServerStatusFull"),					// AGCMUILOGIN_EVENT_SERVER_STATUS_FULL
	_T("LG_ServerStatusClose"),					// AGCMUILOGIN_EVENT_SERVER_STATUS_CLOSE
	_T("LG_ServerRateNC15"),					// AGCMUILOGIN_EVENT_SERVER_RATE_15
	_T("LG_ServerRateNC17"),					// AGCMUILOGIN_EVENT_SERVER_RATE_17
	_T("LG_ServerRateNONE"),					// AGCMUILOGIN_EVENT_SERVER_RATE_NONE
	_T("LG_ServerOpenEvent"),					// AGCMUILOGIN_EVENT_SERVER_OPEN_EVENT
	_T("LG_ServerOpenNew"),						// AGCMUILOGIN_EVENT_SERVER_OPEN_NEW
	_T("LG_ServerOpenOld"),						// AGCMUILOGIN_EVENT_SERVER_OPEN_OLD
	_T("LG_TestServer"),						// AGCMUILOGIN_EVENT_SERVER_TEST
	_T("LG_ServerPlusOn"),					
	_T("LG_ServerPlusOff"),					
	_T("LG_ServerMinusOn"),					
	_T("LG_ServerMinusOff"),					
	_T("LG_ServerWorld"),					
	_T("LG_ServerServer"),					
	_T("LG_Slot01NotView"),						// AGCMUILOGIN_EVENT_SLOT01_NOT_VIEW
	_T("LG_Slot01UnSelect"),					// AGCMUILOGIN_EVENT_SLOT01_UNSELECT
	_T("LG_Slot01Select"),						// AGCMUILOGIN_EVENT_SLOT01_SELECT
	_T("LG_Slot02NotView"),						// AGCMUILOGIN_EVENT_SLOT02_NOT_VIEW
	_T("LG_Slot02UnSelect"),					// AGCMUILOGIN_EVENT_SLOT02_UNSELECT
	_T("LG_Slot02Select"),						// AGCMUILOGIN_EVENT_SLOT02_SELECT
	_T("LG_Slot03NotView"),						// AGCMUILOGIN_EVENT_SLOT03_NOT_VIEW
	_T("LG_Slot03UnSelect"),					// AGCMUILOGIN_EVENT_SLOT03_UNSELECT
	_T("LG_Slot03Select"),						// AGCMUILOGIN_EVENT_SLOT03_SELECT			
	_T("LG_ConfirmPassword"),					// AGCMUILOGIN_EVENT_CONFIRM_PASSWORD
	_T("LG_ConfirmPasswordFail"),				// AGCMUILOGIN_EVENT_CONFIRM_PASSWORD_FAIL
	_T("LG_DUPLICATE_ACCOUNT"),					// AGCMUILOGIN_EVENT_DUPLICATE_ACCOUNT
	_T("LG_GameServerIsFull"),					// AGCMUILOGIN_EVENT_GAME_SERVER_FULL
	_T("LG_CreateCharFailForBusy"),				// AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAIL_BUSY
	_T("LG_NotEnoughAge"),						// AGCMUILOGIN_EVENT_NOT_ENOUGH_AGE
	_T("LG_CharRenameOpen"),					// AGCMUILOGIN_EVENT_CHARACTER_RENAME_OPEN
	_T("LG_CharRenameClose"),					// AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE
	_T("LG_CharRenameFail"),					// AGCMUILOGIN_EVENT_CHARACTER_RENAME_FAIL
	_T("LG_CharRenameSameOldNewID"),			// AGCMUILOGIN_EVENT_CHARACTER_RENAME_FAIL_SAME_OLD_NEW_ID
	_T("LG_SHOW_COMPEN_01"),					// AGCMUILOGIN_EVENT_SHOW_COMPEN_CHAR01,
	_T("LG_SHOW_COMPEN_02"),					// AGCMUILOGIN_EVENT_SHOW_COMPEN_CHAR02,
	_T("LG_SHOW_COMPEN_03"),					// AGCMUILOGIN_EVENT_SHOW_COMPEN_CHAR03,
	_T("LG_HIDE_COMPEN"),						// AGCMUILOGIN_EVENT_HIDE_COMPEN,
	_T("LG____END"),							// AGCMUILOGIN_EVENT_END
	_T("LG_QUESTION_REMOVE_SELECTED_CHARACTER"),// AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION,
	_T("LG_EXIST_DUPLICATE_ACCOUNT"),			// AGCMUILOGIN_EVENT_EXIST_DUPLICATE_ACCOUNT
	_T("DisconnectedByServer"),					// AGCMUILOGIN_EVENT_DISCONNECTED_BY_SERVER
	_T("LG_ID_FAILED"),							// AGCMUILOGIN_EVENT_ID_FALIED,
	_T("LG_PASSWORD_FAILED"),					// AGCMUILOGIN_EVENT_PASSWORD_FALIED
	_T("LG_NOT_BETA_TESTER"),					// AGCMUILOGIN_EVENT_NOT_BETA_TESTER	
	_T("LG_NOT_APPLIED"),						// AGCMUILOGIN_EVENT_NOT_APPLIED
	_T("LG_MOVE_HANGAME"),						// AGCMUILOGIN_EVENT_OPEN_MOVE_HANGAME
};

AgcmUILogin::AgcmUILogin()
: m_bLoadSImpleUI( FALSE )
, m_pcmLogin( NULL )
, m_pIDControl( NULL )
, m_pPWControl( NULL )
{
	INT32 i = 0;

	SetModuleName("AgcmUILogin")					;
	EnableIdle(TRUE)								;

#ifdef _AREA_GLOBAL_
	m_lUnion			=		AUUNION_TYPE_RED_HEART	;
#else
	m_lUnion			=		AUUNION_TYPE_NONE	;
#endif

	m_lLoginMainSelectCID	=	AP_INVALID_CID				;

	ZeroMemory(m_szMyCharacterInfo, sizeof(m_szMyCharacterInfo));
	ZeroMemory(m_bMyCharacterFlag, sizeof(m_bMyCharacterFlag));

	m_lAddMyCharacterCount	=	0					;

	m_bHumanBaseCharacterAddRequest	=	FALSE		;
	m_bOrcBaseCharacterAddRequest	=	FALSE		;

	m_pcsPreSelectedCharacter		=	NULL		;


	m_iBackUpDrawRange = 0;
	m_pPrevSector = NULL;

	m_lCompenData = 0;
	m_lCharacterIndex = -1;
	ZeroMemory(m_szItemDetails, sizeof(m_szItemDetails));

	m_bRenameBlock = FALSE;

	CHAR* FileName = "INI\\LoginSettings.txt";
	GetUILoginSetting()->LoadSettingFromFile( FileName, FALSE );

	g_pcsAgcmUILogin = this;
}

AgcmUILogin::~AgcmUILogin()
{
	DestroyLoginSettingData();
}

BOOL	AgcmUILogin::OnAddModule()
{
	AgcmUIAccountCheck::OnInitialize( this );
	AgcmUIServerSelect::OnInitialize( this );
	AgcmUICharacterCreate::OnInitialize( this );

	m_ControlPostFX.OnInitialzie( this );
	m_LoadingWindow.OnInitialize( this );

	if (!AddEvent())
		return FALSE;

	if (!AddFunction())
		return FALSE;	

	if (!AddUserData())
		return FALSE;

	if (!AddDisplay())
		return FALSE;

	// set module callback
	AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
	if (pcmLogin)
	{
		// enter game
		if (!pcmLogin->SetCallbackEncryptCodeSuccess(CBEncryptCode, this)) return FALSE;
		if (!pcmLogin->SetCallbackInvalidClientVersion(CBInvalidClientVersion, this)) return FALSE;
		if (!pcmLogin->SetCallbackUnionInfo(CBGetUnionInfo, this)) return FALSE;
		if (!pcmLogin->SetCallbackCharacterName(CBGetMyCharacterName, this)) return FALSE;
		if (!pcmLogin->SetCallbackCharacterNameFinish(CBGetMyCharacterNameFinish, this)) return FALSE;
		if (!pcmLogin->SetCallbackCharacterInfoFinish(CBGetMyCharacterInfoFinish, this)) return FALSE;
		if (!pcmLogin->SetCallbackEnterGameEnd(CBEnterGameEnd, this)) return FALSE;
		// create/remove
		if (!pcmLogin->SetCallbackNewCharacterName(CBGetNewCharacterName, this)) return FALSE;
		if (!pcmLogin->SetCallbackNewCharacterInfoFinish(CBGetNewCharacterInfoFinish, this)) return FALSE;
		if (!pcmLogin->SetCallbackRemoveCharacter(CBRemoveCharacter, this)) return FALSE;
		// common login result
		if (!pcmLogin->SetCallbackLoginResult(CBLoginResult, this)) return FALSE;
		if (!pcmLogin->SetCallbackCompensationInfo(CBCompensationInfo, this)) return FALSE;
	}

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if (ppmCharacter)
	{
		if (!ppmCharacter->SetCallbackAddCharPost(CBAddCharacter, this)) return FALSE;
		if (!ppmCharacter->SetCallbackDisconnectByAnotherUser( CBResultDisconnectByAnotherUser, this )) return FALSE;
	}

	AgcmMap* pcmMap = ( AgcmMap* )GetModule( "AgcmMap" );
	if (pcmMap)
	{
		if (!pcmMap->SetCallbackSectorChangePre(CBMapSectorChangePreCallback, this)) return FALSE;
		if (!pcmMap->SetCallbackSectorChange(CBMapSectorChangeCallback, this)) return FALSE;
		if (!pcmMap->SetCallbackLoadMap(CBMapLoadSector, this)) return FALSE;
	}

	AgcmUIMain* pcmUIMain = ( AgcmUIMain* )GetModule( "AgcmUIMain" );
	if( !pcmUIMain ) return FALSE;
	if( !pcmUIMain->SetCallbackKeydownCloseAllUIExceptMain( CBKeydownCloseAllUI, this ) ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( pcmCharacter )
	{
		if (!pcmCharacter->SetCallbackGameServerError(CBGameServerError, this))	return FALSE;
		if (!pcmCharacter->SetCallbackGameServerDisconnect(CBDisconnectGameServer, this)) return FALSE;
		if (!pcmCharacter->SetCallbackLoadingComplete(CBLoadingComplete, this))	return FALSE;
		if (!pcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this)) return FALSE;
	}

	AgpmWorld* ppmWorld = ( AgpmWorld* )GetModule( "AgpmWorld" );
	if( !ppmWorld ) return FALSE;
	if (!ppmWorld->SetCallbackResultGetWorldAll(CBGetWorldAll, this)) return FALSE;
	if (!ppmWorld->SetCallbackUpdateWorld(CBUpdateWorld, this))	return FALSE;

	AgcmWorld* pcmWorld = ( AgcmWorld* )GetModule( "AgcmWorld" );
	if( !pcmWorld ) return FALSE;
	if( !pcmWorld->SetCallbackGetCharCount( CBGetCharCount, this ) ) return FALSE;

	AgcmReturnToLogin* pcmReturnToLogin = ( AgcmReturnToLogin* )GetModule( "AgcmReturnToLogin" );
	if( !pcmReturnToLogin ) return FALSE;
	if( !pcmReturnToLogin->SetCallbackEndProcess( CBReturnToLoginEndProcess, this ) ) return FALSE;

	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )GetModule( "AgpmAdmin" );
	if( !ppmAdmin ) return FALSE;
	if( !ppmAdmin->SetCallbackPing( CBPing, this ) ) return FALSE;

	AgcmUIAccountCheck::OnAddCallBack( pcmLogin );
	AgcmUICharacterCreate::OnAddCallBack( pcmLogin );
	return TRUE;
}


BOOL AgcmUILogin::OnInit()
{	
	//@{ kday 20050225
	//초기화에서 한번만 할것들.
	AgcmCamera2* pAgcmCamera2 = (AgcmCamera2*)GetModule("AgcmCamera2");
	ASSERT(pAgcmCamera2);

	pAgcmCamera2->bSetIdleType( AgcmCamera2::e_idle_login );

	AgcmEventNature* pcmEventNature = ( AgcmEventNature* )GetModule( "AgcmEventNature" );
	AgcmEventEffect* pcmEventEffect = ( AgcmEventEffect* )GetModule( "AgcmEventEffect" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );

	m_ControlCamera.SetCameraAndNature( pAgcmCamera2, pcmEventNature );	
	m_ControlCharacter.SetExternalClassPointers( pcmCharacter, ppmCharacter, pcmEventEffect );

	if( pcmEventNature )
	{
		pcmEventNature->SetTimeForce( pcmEventNature->GetTime() );
	}

	m_CompenTooltip.m_Property.bTopmost = TRUE;

	m_pcsAgcmUIManager2 = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	m_pcsAgcmUIManager2->AddWindow((AgcWindow *)(&m_CompenTooltip));
	m_CompenTooltip.ShowWindow(FALSE);

	m_pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );

	m_pcsAgcmUIManager2->m_bCameraControl = FALSE;

	m_LoginSelect.Initialize( m_pcsAgcmUIManager2 , m_pcmLogin);
	return TRUE;
}


BOOL AgcmUILogin::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmUILogin::OnIdle");

	if( (m_pIDControl && m_pIDControl->m_bChangeText) || (m_pPWControl && m_pPWControl->m_bChangeText) )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_pcsAgcmUIManager2->m_nEventTextSound );
		m_pIDControl->m_bChangeText = FALSE;
		m_pPWControl->m_bChangeText = FALSE;
	}

	static float prevTime = 0;
	float curTime = float(clock())/CLOCKS_PER_SEC;
	float elapsedTime = min( curTime - prevTime, 0.2f );

	m_ControlPostFX.OnUpdate( elapsedTime );
	prevTime = curTime;

	AgcmMap* pcmMap = ( AgcmMap* )GetModule( "AgcmMap" );
	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )GetModule( "AgcmResourceLoader" );
	if (pcmResourceLoader)
	{
		static BOOL	bR	= FALSE;
		if( KEYDOWN(VK_SHIFT) && KEYDOWN(VK_CONTROL) && KEYDOWN_ONECE('R', bR) )
		{
			CHAR* FileName = "INI\\LoginSettings.txt";
			GetUILoginSetting()->LoadSettingFromFile( FileName, FALSE );

			MyCharacterAllUpdatePosition();
			m_ControlCamera.OnChangeMode( m_ControlCamera.GetLoginMode() );
		}

		INT32				lCurrentRemain	= pcmResourceLoader->GetRemainCount();
		INT32				lLoginMode		= GetLoginMode();
		INT32				lSectorsToLoad	= pcmMap ? pcmMap->GetSectorsToLoad() : 0;

		AgcmUIEventTeleport* pcmUIEventTeleport = ( AgcmUIEventTeleport* )GetModule( "AgcmUIEventTeleport" );
		if( pcmUIEventTeleport )
		{
			if( pcmUIEventTeleport->IsStartedMapLoading() && !m_LoadingWindow.GetIsLoadingWindowOpen() )
			{
				// 2007/06/21 마고자 
				// 텔레포트시 이벤트 로딩맵을 띄운후
				// 섹터 체인지가 돼지 않으면 맵 로딩이 완료되는 콜백이 불리지않아
				// 로딩 유아이가 사라지지 않는 현상이 있다.
				pcmUIEventTeleport->CloseEventLoadingWindow();
				m_LoadingWindow.SetIsLoadingWindowOpen( TRUE );
			}
		}
		//2005.2.28 gemani .. shadow2 섹터 수동 지정
		if(lLoginMode == AGCMLOGIN_MODE_ID_PASSWORD_INPUT ||
			lLoginMode == AGCMLOGIN_MODE_SERVER_SELECT)
		{
			AgcmRender* pcmRender = ( AgcmRender* )GetModule( "AgcmRender" );
			if( pcmRender )
			{
				RwFrame*	pMainFrame = pcmRender->m_pFrame;
				if(pMainFrame)
				{
					RwMatrix*	pLTM = RwFrameGetLTM(pMainFrame);
					RwV3d*		pPos = RwMatrixGetPos(pLTM);

					if( pcmMap )
					{
						ApWorldSector* pCurSector = pcmMap->m_pcsApmMap->GetSector(pPos->x,pPos->z);
						if(pCurSector != m_pPrevSector)
						{
							AgcmShadow2* pcmShadow2 = ( AgcmShadow2* )GetModule( "AgcmShadow2" );
							if( pcmShadow2 )
							{
								pcmShadow2->CB_SECTOR_CAHNGE((PVOID)pCurSector,(PVOID)pcmShadow2,NULL);
							}

							m_pPrevSector = pCurSector;
						}
					}
				}
			}
		}

		{
			m_ControlCamera.OnUpdate( AgcdEffGlobal::bGetInst().bGetDiffTimeS() );
			if( AGCMLOGIN_MODE_CHARACTER_SELECT == lLoginMode || 
				AGCMLOGIN_MODE_CHARACTER_CREATE == lLoginMode )
			{
				m_ControlCharacter.OnUpdate( AgcdEffGlobal::bGetInst().bGetDiffTimeS() );
			}
		}

		// ResourceLoader에 남은 개수로 처리하지 않고, m_bForceImmediate가 FALSE로 바뀌면 
		if ((!lCurrentRemain || !lSectorsToLoad) && m_LoadingWindow.GetIsLoadingWindowOpen() )
		{
			AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );

			if (
				(
				lLoginMode != AGCMLOGIN_MODE_WAIT_MY_CHARACTER &&					// IDPW 화면에서 넘어가는게 아니고
				lLoginMode != AGCMLOGIN_MODE_WAIT_MY_NEW_CHARACTER &&
				lLoginMode != AGCMLOGIN_MODE_SERVER_SELECT && 
				lLoginMode != AGCMLOGIN_MODE_POST_LOGIN_LOADING
				) ||
				(
				lLoginMode == AGCMLOGIN_MODE_POST_LOGIN_LOADING &&
				m_LoadingWindow.GetIsPostLoadingWindow() &&											// 게임에 들어갈때 Loading이 나오면
				pcmCharacter->GetSelfCharacter()
				)
				)
			{
				if( AGCMLOGIN_MODE_CHARACTER_CREATE == lLoginMode )
					SetLoginMode( AGCMLOGIN_MODE_CHARACTER_CREATE );

				CloseLoadingWindow();

				// Login 과정 중이라면 다시한번 My Character들 Update Position 해준다 
				if ( AGCMLOGIN_MODE_POST_LOGIN != lLoginMode && AGCMLOGIN_MODE_PRE_LOGIN != lLoginMode )
					MyCharacterAllUpdatePosition();

				AgcmEventPointLight* pcmUIEventPointLight = ( AgcmEventPointLight* )GetModule( "AgcmEventPointLight" );
				if( pcmUIEventPointLight )
				{
					if (m_LoadingWindow.GetIsPostLoadingWindow() && !pcmUIEventPointLight->m_bReturnLoginLobby)
					{
						AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
						pcmUIManager->m_bCameraControl = TRUE;

						SetLoginMode(AGCMLOGIN_MODE_POST_LOGIN);
						pcmResourceLoader->m_bForceImmediate = FALSE;

						AgcmUIOption* pcmUIOption = ( AgcmUIOption* )GetModule( "AgcmUIOption" );
						if( pcmUIOption )
						{
							pcmUIOption->UpdateBloom( FALSE );
						}

						AgcmEventEffect* pcmEventEffect = ( AgcmEventEffect* )GetModule( "AgcmEventEffect" );
						if( pcmEventEffect )
						{
							pcmEventEffect->SetCommonCharacterEffect(pcmCharacter->GetSelfCharacter(), E_COMMON_CHAR_TYPE_SPAWN);
						}

						AgcmUIMailBox* pcmUIMailBox = ( AgcmUIMailBox* )GetModule( "AgcmUIMailBox" );
						if( pcmUIMailBox )
						{
							pcmUIMailBox->CheckHaveNotReadMail();
						}
					}
				}
			}
		}
	}

	AgcmUIServerSelect::OnUpdateServerInfo( ulClockCount );

	#ifndef USE_MFC
	if( ( GetIsServerSelectDialogOpen() ) 
		|| AGCMLOGIN_MODE_SERVER_SELECT == GetLoginMode() )
		OnLoadServerList();
	#endif

	return TRUE;
}

BOOL AgcmUILogin::AddEvent()
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	// register event
	for (INT32 i = 0; i < AGCMUILOGIN_EVENT_END; i++)
	{
		m_lEvent[i] = pcmUIManager->AddEvent(s_szEvent[i]);
		if ( 0 > m_lEvent[i] )
			return FALSE;
	}

	// event requires callback function
	m_lEvent[AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION] =
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION], CBLoginReturnRemoveCharacter, this);

	m_lEvent[AGCMUILOGIN_EVENT_EXIST_DUPLICATE_ACCOUNT] =
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_EXIST_DUPLICATE_ACCOUNT], CBLoginReturnRemoveDuplicateAccount, this);

	m_lEvent[AGCMUILOGIN_EVENT_DISCONNECTED_BY_SERVER] =
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_DISCONNECTED_BY_SERVER], CBLoginReturnDisconnect, this);

	m_lEvent[AGCMUILOGIN_EVENT_ID_FALIED] =
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_ID_FALIED], CBIDFailed, this);

	m_lEvent[AGCMUILOGIN_EVENT_PASSWORD_FALIED] =
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_PASSWORD_FALIED], CBPasswordFailed, this);

	m_lEvent[AGCMUILOGIN_EVENT_NOT_BETA_TESTER]	=
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_NOT_BETA_TESTER], CBReturnNotTester, this);

	m_lEvent[AGCMUILOGIN_EVENT_NOT_APPLIED] =
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_NOT_APPLIED], CBResultNotApplied, this);

	m_lEvent[AGCMUILOGIN_EVENT_OPEN_MOVE_HANGAME] =
		pcmUIManager->AddEvent(s_szEvent[AGCMUILOGIN_EVENT_OPEN_MOVE_HANGAME], CBOpenHangameWeb, this);

	for (INT32 i = AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION; i <= AGCMUILOGIN_EVENT_NOT_APPLIED; i++)
	{
		if (0 > m_lEvent[i])
			return FALSE;
	}

	AgcmUIAccountCheck::OnAddEvent( pcmUIManager );
	AgcmUICharacterCreate::OnAddEvent( pcmUIManager );
	return TRUE;
}

BOOL AgcmUILogin::AddFunction()
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	pcmUIManager->AddFunction( this, "LG_LoginMainCreateButtonClick", CBLoginMainCreateButtonClick, 1, "CharCreate UI" );
	pcmUIManager->AddFunction( this, "LG_LoginMainCancelButtonClick", CBLoginMainCancelButtonClick, 0 );

	if ( !pcmUIManager->AddFunction( this, "LG_UnionSelectReturnIDPassworldInput", CBUnionSelectWindowReturnIDPassworldInput, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_LoginMainSelectButtonClick", CBLoginMainSelectButtonClick, 0 ))	return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_LoginMainDeleteButtonClick", CBLoginMainDeleteButtonClick, 0 ))	return FALSE;
	if ( !pcmUIManager->AddFunction( this, "EXIT_ReturnLoginCharacterSelect", CBReturnLoginCharacterSelectMode, 0 )) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "EXIT_ReturnGame", CBReturnGame, 0 )) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "EXIT_Open_Window", CBOpenExitWindow, 0 )) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_ConfirmPassword", CBConfirmPassword, 1, "Edit Control")) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_ConfirmDeleteChar", CBConfirmDeleteChar, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CancelDeleteConfirm", CBCancelDeleteConfirm, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CharRenameOk", CBCharRenameOkButtonClick, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CharRenameCancel", CBCharRenameCancelButtonClick, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CharInfoClick01", CBCharInfoClick01, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CharInfoClick02", CBCharInfoClick02, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CharInfoClick03", CBCharInfoClick03, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CompenClick01", CBCompensationClick01, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CompenClick02", CBCompensationClick02, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CompenClick03", CBCompensationClick03, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CompenSetFocus", CBCompensationSetFocus, 0)) return FALSE;
	if ( !pcmUIManager->AddFunction( this, "LG_CompenKillFocus", CBCompensationKillFocus, 0)) return FALSE;

	AgcmUIAccountCheck::OnAddFunction( pcmUIManager );
	AgcmUIServerSelect::OnAddFunction( pcmUIManager );
	AgcmUICharacterCreate::OnAddFunction( pcmUIManager );
	return TRUE;
}

BOOL AgcmUILogin::AddUserData()
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	m_pstUserDataDummy			= pcmUIManager->AddUserData("LG_DummyData", &m_lDummyData, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	m_pstUserDataOldCharID		= pcmUIManager->AddUserData("LG_RenameOldCharID", &m_lOldCharID, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	m_pstCompen					= pcmUIManager->AddUserData("LG_Compen", &m_lCompenData, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);

	m_LoadingWindow.OnAddUserData( pcmUIManager );

	AgcmUIAccountCheck::OnAddUserData( pcmUIManager );
	AgcmUIServerSelect::OnAddUserData( pcmUIManager );
	AgcmUICharacterCreate::OnAddUserData( pcmUIManager );

	AgcmUIServerSelect::OnAddBoolean( pcmUIManager );
	return TRUE;
}

BOOL AgcmUILogin::AddDisplay()
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	if (!pcmUIManager->AddDisplay(this, "Login01SlotCharName", 0, CBDisplaySlotCharName, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login01SlotCharLevel", 0, CBDisplaySlotCharLevel, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login01SlotCharExp", 0, CBDisplaySlotCharExp, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login01SlotCharGuild", 0, CBDisplaySlotCharGuild, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login01SlotCharVillian", 0, CBDisplaySlotCharVillian, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login02SlotCharName", 0, CBDisplaySlotCharName, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login02SlotCharLevel", 0, CBDisplaySlotCharLevel, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login02SlotCharExp", 0, CBDisplaySlotCharExp, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login02SlotCharGuild", 0, CBDisplaySlotCharGuild, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login02SlotCharVillian", 0, CBDisplaySlotCharVillian, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login03SlotCharName", 0, CBDisplaySlotCharName, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login03SlotCharLevel", 0, CBDisplaySlotCharLevel, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login03SlotCharExp", 0, CBDisplaySlotCharExp, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login03SlotCharGuild", 0, CBDisplaySlotCharGuild, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "Login03SlotCharVillian", 0, CBDisplaySlotCharVillian, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "LoginGameServerName", 0, CBDisplayGameServerName, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "LoginOldCharID", 0, CBDisplayOldCharID, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "ConnRename", 0, CBDisplayConnectRename, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;
	if (!pcmUIManager->AddDisplay(this, "LG_CompenCaption", 0, CBDisplayCompenCaption, AGCDUI_USERDATA_TYPE_INT32)) return FALSE;

	AgcmUIAccountCheck::OnAddDisplay( pcmUIManager );
	AgcmUIServerSelect::OnAddDisplay( pcmUIManager );
	AgcmUICharacterCreate::OnAddDisplay( pcmUIManager );
	return TRUE;
}

void AgcmUILogin::InitAgcmUILogin()
{
	ZeroMemory(m_szMyCharacterInfo, sizeof(m_szMyCharacterInfo));
	ZeroMemory(m_bMyCharacterFlag, sizeof(m_bMyCharacterFlag));

	DeleteAllMyCharacter();

	m_bHumanBaseCharacterAddRequest	=	FALSE;
	m_bOrcBaseCharacterAddRequest	=	FALSE;
}

BOOL AgcmUILogin::LoadTemplates(TCHAR *szPath)
{
	if (!szPath || _tcslen(szPath) <= 0)
		return FALSE;

	ApString<128>	csString;

	csString.AppendFormat("%s\\TeleportFee.txt", szPath);

	AgpmEventTeleport* ppmEventTeleport = ( AgpmEventTeleport* )GetModule( "AgpmEventTeleport" );
	if (ppmEventTeleport)
	{
		ppmEventTeleport->RemoveFee();
		VERIFY(ppmEventTeleport	->StreamReadFee		(csString.GetBuffer(), TRUE));
	}

	CHAR	szBuffer[512];

	csString.Clear();
	csString.AppendFormat("%s\\CharacterDataTable.txt", szPath);

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if( ppmCharacter )
	{
		VERIFY(ppmCharacter	->StreamReadImportData		(csString.GetBuffer(), szBuffer, TRUE));
	}

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	if (ppmItem)
	{
		csString.Clear();
		csString.AppendFormat("%s\\ItemDataTable.txt", szPath);
		VERIFY(ppmItem		->StreamReadImportData		(csString.GetBuffer(), szBuffer, TRUE));

		csString.Clear();
		csString.AppendFormat("%s\\BankSlotPrice.txt", szPath);
		VERIFY(ppmItem			->StreamReadBankSlotPrice	(csString.GetBuffer(), TRUE));

		csString.Clear();
		csString.AppendFormat("%s\\ItemOptionTable.txt", szPath);
		VERIFY(ppmItem		->StreamReadOptionData		(csString.GetBuffer(), TRUE));
	}

	csString.Clear();
	csString.AppendFormat("%s\\ItemTooltipDesc.txt", szPath);

	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
	if (pcmUIItem)
		VERIFY(pcmUIItem->StreamReadTooltipDescData(csString.GetBuffer(), TRUE));

	csString.Clear();
	csString.AppendFormat("%s\\QuestTemplate.ini", szPath);

	AgpmQuest* ppmQuest = ( AgpmQuest* )GetModule( "AgpmQuest" );
	if (ppmQuest)
		VERIFY(ppmQuest		->StreamReadTemplate		(csString.GetBuffer(), TRUE));

	AgpmGuild* ppmGuild = ( AgpmGuild* )GetModule( "AgpmGuild" );
	if( ppmGuild )
	{
		csString.Clear();
		csString.AppendFormat("%s\\GuildMaxMember.txt", szPath);
		VERIFY(ppmGuild->ReadRequireItemIncreaseMaxMember(csString.GetBuffer(), TRUE));

		csString.Clear();
		csString.AppendFormat("%s\\guildmark.txt", szPath);
		VERIFY(ppmGuild->ReReadGuildMarkTemplate	(csString.GetBuffer(), TRUE));
	}

	AgpmRefinery* ppmRefinery = ( AgpmRefinery* )GetModule( "AgpmRefinery" );
	if( ppmRefinery )
	{
		csString.Clear();
		csString.AppendFormat("%s\\RefineryItem.txt", szPath);
	}

	AgpmItemConvert* ppmItemConvert = ( AgpmItemConvert* )GetModule( "AgpmItemConvert" );
	if( ppmItemConvert )
	{
		csString.Clear();
		csString.AppendFormat("%s\\ItemConvertTable.txt", szPath);
		VERIFY(ppmItemConvert->StreamReadConvertTable(csString.GetBuffer(), TRUE));

		csString.Clear();
		csString.AppendFormat("%s\\ItemRuneAttributeTable.txt", szPath);
		VERIFY(ppmItemConvert->StreamReadRuneAttribute(csString.GetBuffer(), TRUE));
	}

	csString.Clear();
	csString.AppendFormat("%s\\CharacterCustomizeList.txt", szPath);

	AgpmEventCharCustomize* ppmEventCharCustomize = ( AgpmEventCharCustomize* )GetModule( "AgpmEventCharCustomize" );
	if (ppmEventCharCustomize)
		VERIFY(ppmEventCharCustomize->StreamReadCustomizeList(csString.GetBuffer(), TRUE));

	return TRUE;
}

void AgcmUILogin::SetLoginMode(INT32 lLoginMode)
{
	AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
	if ( pcmLogin )
	{
		if(lLoginMode != AGCMLOGIN_MODE_POST_LOGIN && lLoginMode != AGCMLOGIN_MODE_POST_LOGIN_LOADING )
		{
			m_ControlPostFX.OnStartPostFX();
		}

		pcmLogin->SetLoginMode( lLoginMode )	;
		m_ControlCharacter.OnChangeMode( lLoginMode );
		INT32 nPrevMode = m_ControlCamera.OnChangeMode( lLoginMode );

		if( m_bLoadSImpleUI )
		{
			AgcmCamera2* pAgcmCamera2 = (AgcmCamera2*)GetModule("AgcmCamera2");
			ASSERT( pAgcmCamera2 );

			AgcmEventNature* pcmEventNature = ( AgcmEventNature* )GetModule( "AgcmEventNature" );
			switch( lLoginMode )
			{
			case AGCMLOGIN_MODE_CHARACTER_CREATE:
				if( nPrevMode != AGCMLOGIN_MODE_CHARACTER_CREATE )
				{
					LoadCreateMap();				
					if(pcmEventNature->SetCharacterPosition( *(const AuPOS*)pAgcmCamera2->bGetPtrEye(), TRUE ))
					{
						pcmEventNature->ApplySkySetting();
					}
				}
				break;
			default:
				break;
			}
		}

		AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );

		// Character Data Loading 시 부하줄이기 위해 LoginMode 변경
		if (lLoginMode == AGCMLOGIN_MODE_WAIT_MY_NEW_CHARACTER)
			pcmCharacter->SetTemplateReadType(AGCMCHAR_READ_TYPE_WAIT_ANIM_ONLY);
		if (lLoginMode == AGCMLOGIN_MODE_CHARACTER_CREATE)
		{
			pcmCharacter->SetTemplateReadType(AGCMCHAR_READ_TYPE_ALL);

			//AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
			//AgpdCharacter* pChar = ppmCharacter->AddCharacter( 337337, 1724, "시온" );
			//if( pChar )
			//	pChar->m_unCurrentStatus = AGPDCHAR_STATUS_IN_GAME_WORLD;
		}

		// Character Mode 변경시 Character Select Mode 면 TextBoard 에서 NickName 보이지 않도록 설정
		AgcmTextBoardMng* pTextBoardMng = ( AgcmTextBoardMng* )GetModule( "AgcmTextBoardMng" );
		if( pTextBoardMng )
		{
#ifdef _AREA_CHINA_
			if( lLoginMode == AGCMLOGIN_MODE_CHARACTER_SELECT || lLoginMode == AGCMLOGIN_MODE_CHARACTER_CREATE || lLoginMode == AGCMLOGIN_MODE_POST_LOGIN_LOADING )
#else
			if( lLoginMode == AGCMLOGIN_MODE_CHARACTER_SELECT || lLoginMode == AGCMLOGIN_MODE_POST_LOGIN_LOADING )
#endif
			{
				pTextBoardMng->SetDrawID( FALSE );
			}
			else
			{
				pTextBoardMng->SetDrawID( TRUE );
			}
		}
	}
}


INT32 AgcmUILogin::GetLoginMode()
{
	AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
	return pcmLogin ? pcmLogin->GetLoginMode() : -1;
}


INT32 AgcmUILogin::LoadLoginMap(UINT32 ulDivision)
{
	// 로그인 지형 전체를 읽어들임.
	INT32 i,j;
	INT32	nX1 , nX2 , nZ1 , nZ2;
	nX1	= GetFirstSectorXInDivision( ulDivision );
	nZ1	= GetFirstSectorZInDivision( ulDivision );
	nX2 = nX1 + MAP_DEFAULT_DEPTH;
	nZ2 = nZ1 + MAP_DEFAULT_DEPTH;

	ApWorldSector * pSector;

	INT32	llSectorsToLoad	= (nZ2 - nZ1 + 1) * (nX2 - nX1 + 1);

	AgcmMap* pcmMap = ( AgcmMap* )GetModule( "AgcmMap" );
	if( !pcmMap ) return 0;

	pcmMap->SetSectorsToLoad(llSectorsToLoad);
	OpenLoadingWindow(AURACE_TYPE_HUMAN);

	for( i = nZ1 ; i <= nZ2 ; i++)
	{
		for( j = nX1 ; j <= nX2 ; j++)
		{
			pSector =  pcmMap->m_pcsApmMap->GetSectorByArrayIndex( j , i );
			if( NULL == pSector )
			{
				pSector = pcmMap->LoadSector( ArrayIndexToSectorIndexX( j ), 0,	ArrayIndexToSectorIndexZ( i ) );
			}

			if( pSector )
			{
#ifdef _LOAD_ROUGH_ONLY_
				pcmMap->OnLoadRough( pSector );
				pcmMap->SetCurrentDetail( pSector , SECTOR_LOWDETAIL );
#else
				pcmMap->OnLoadRough( pSector );
				pcmMap->OnLoadDetail( pSector );
				pcmMap->SetCurrentDetail( pSector , SECTOR_HIGHDETAIL );
#endif
			}

			pcmMap->SetSectorsToLoad(--llSectorsToLoad);
		}
	}

	// 게임내에서 캐릭터 신규생성 걸리는 랙을 줄이기 위해서 이 시점에서 미리 모든 캐릭터 타입을 로딩해둔다.
	PreLoadAllCharacterTypes();
	return 0;
}

INT32	AgcmUILogin::LoadSelectMap	()
{
	// 케릭터 선택화면만 읽어들임.
	POINT	aUseSector[] = 
	{
		{ 275 , 280 } ,
		{ 276 , 280 } ,
		{ 277 , 280 }
	};

	INT32	nUseSector = 3;
	INT32	llSectorsToLoad	= nUseSector;

	AgcmMap* pcmMap = ( AgcmMap* )GetModule( "AgcmMap" );
	if( pcmMap )
	{
		pcmMap->SetSectorsToLoad(llSectorsToLoad);
	}

	OpenLoadingWindow(AURACE_TYPE_HUMAN);

	ApWorldSector * pSector;
	for( int i = 0 ; i < nUseSector ; i++ )
	{
		pSector = pcmMap->m_pcsApmMap->GetSectorByArrayIndex( aUseSector[ i ].x , aUseSector[ i ].y );
		if( NULL == pSector )
		{
			pSector = pcmMap->LoadSector( ArrayIndexToSectorIndexX( aUseSector[ i ].x ) , 0 , ArrayIndexToSectorIndexZ( aUseSector[ i ].y ) );
		}

		if( pSector )
		{
			pcmMap->OnLoadRough( pSector );
			pcmMap->OnLoadDetail( pSector );
			pcmMap->SetCurrentDetail( pSector , SECTOR_HIGHDETAIL );
		}

		pcmMap->SetSectorsToLoad(--llSectorsToLoad);
	}

	return TRUE;
}

INT32	AgcmUILogin::LoadCreateMap	()
{
	// 케릭터 생성 배경만 읽어들임..
	POINT	aUseSector[] = 
	{
		{ 275 , 281 } ,	{ 276 , 281 } ,
		{ 275 , 282 } ,	{ 276 , 282 } ,
		{ 275 , 283 } ,	{ 276 , 283 } ,
		{ 275 , 284 } ,	{ 276 , 284 } ,
		{ 275 , 285 } ,	{ 276 , 285 } ,
		{ 275 , 286 } ,	{ 276 , 286 } ,
		{ 275 , 287 } ,	{ 276 , 287 }
	};

	INT32	nUseSector = 14;
	INT32	llSectorsToLoad	= nUseSector;

	AgcmMap* pcmMap = ( AgcmMap* )GetModule( "AgcmMap" );
	if( pcmMap )
	{
		pcmMap->SetSectorsToLoad(llSectorsToLoad);
	}

	OpenLoadingWindow(AURACE_TYPE_HUMAN);
	ApWorldSector * pSector;

	for( int i = 0 ; i < nUseSector ; i++ )
	{
		pSector = pcmMap->m_pcsApmMap->GetSectorByArrayIndex( aUseSector[ i ].x , aUseSector[ i ].y );
		if( !pSector )
		{
			pSector = pcmMap->LoadSector( ArrayIndexToSectorIndexX( aUseSector[ i ].x ) , 0 , ArrayIndexToSectorIndexZ( aUseSector[ i ].y ) );
		}

		if( pSector )
		{
			pcmMap->OnLoadRough( pSector );
			pcmMap->OnLoadDetail( pSector );
			pcmMap->SetCurrentDetail( pSector , SECTOR_HIGHDETAIL );
		}

		pcmMap->SetSectorsToLoad(--llSectorsToLoad);
	}

	return TRUE;
}

//
//====================================================
//
BOOL AgcmUILogin::SetLoginProcess()
{
	// 마고자 (2005-09-07 오후 6:53:23) : 
	// 초기 UI로딩 지역 변경
	// 카메라 워킹 파일 업데이트가 안됐을경우
	// 아무것도 안나올지도 모름 ;
	//const INT32 LOGINDIVISION	= 1717;
	//const INT32 LOGINDIVISION	= 1817;

	//@{ Jaewon 20051027
	// Moved here from the bottom of the function.
	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )GetModule( "AgcmResourceLoader" );
	if (pcmResourceLoader)
		pcmResourceLoader->m_bForceImmediate = TRUE;

	//@{ kday 20050225
	//const UINT32 LOGINDIVISION	= 2017;
	if( m_bLoadSImpleUI )
	{
		// 선택하는 부분만 읽음..
		LoadSelectMap();
	}
	else
	{
		LoadLoginMap( GetUILoginSetting()->GetLoginDivision() );
	}

	// DrawRange set
	AgcmRender* pcmRender = ( AgcmRender* )GetModule( "AgcmRender" );
	if( pcmRender )
	{
		m_iBackUpDrawRange = pcmRender->m_iDrawRange;
		pcmRender->m_iDrawRange = 10;
	}

	AgcdUI* pcdUI = m_pcsAgcmUIManager2->GetUI( "UI_LG_IDPASSWORD" );
	m_pcsAgcmUIManager2->m_bCameraControl = FALSE;
	AgcdUIControl* pcdControl = (m_pcsAgcmUIManager2->GetControl( pcdUI, "CT_LG_EDIT_ID" ));
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )AgcmUIAccountCheck::m_pcmUILogin;
	if( pcdControl && pcmUILogin )
		pcmUILogin->m_pIDControl = (AcUIEdit*)pcdControl->m_pcsBase;

	pcdControl = (m_pcsAgcmUIManager2->GetControl( pcdUI, "CT_LG_EDIT_PASSWORD" ));
	if( pcdControl && pcmUILogin )
		pcmUILogin->m_pPWControl = (AcUIEdit*)pcdControl->m_pcsBase;


	return TRUE;
}


BOOL AgcmUILogin::StartLoginProcess()
{
	SetLoginProcess();

	// 시작하자 마자 ID, Password 입력창이 필요합니다. 
	SetLoginMode(AGCMLOGIN_MODE_ID_PASSWORD_INPUT);

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	if ( m_bLoadSImpleUI )
	{
		switch (pcmUIManager->GetUIMode())
		{
		case AGCDUI_MODE_1024_768:
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_2D_OPEN]);
			break;
		case AGCDUI_MODE_1280_1024:
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_2D_1280_OPEN]);
			break;
		case AGCDUI_MODE_1600_1200:
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_2D_1600_OPEN]);
			break;
		}
	}
#ifdef _AREA_JAPAN_
	if( g_jAuth.GetAutoLogin() )	
		return TRUE;
#elif defined(_AREA_KOREA_) && !defined( USE_MFC )
	CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( pWebzenAuth && pWebzenAuth->IsAutoLogin() )	return TRUE;
#endif
	pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CONNECT_LOGIN_SERVER]);
	switch (pcmUIManager->GetUIMode())
	{
	case AGCDUI_MODE_1024_768:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_OPEN]);
		break;
	case AGCDUI_MODE_1280_1024:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_ID_PASSWORD_1280_INPUT_OPEN]);
		break;
	case AGCDUI_MODE_1600_1200:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_ID_PASSWORD_1600_INPUT_OPEN]);
		break;
	}

	return TRUE;
}

void AgcmUILogin::OpenMoveToHangame()
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_OPEN_MOVE_HANGAME]);
}

void AgcmUILogin::CharacterClick(INT32 lCID)
{
	ASSERT( AGCMLOGIN_MODE_CHARACTER_SELECT == GetLoginMode() );
	if( AGCMLOGIN_MODE_CHARACTER_SELECT != GetLoginMode() )		return;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );

	AgpdCharacter*	pcsCharacter;
	AgcdCharacter*	pstCharacter;

	pcsCharacter = ppmCharacter->GetCharacter( m_lLoginMainSelectCID );
	if( pcsCharacter )
	{
		pstCharacter = pcmCharacter->GetCharacterData(pcsCharacter);
		if( pstCharacter && pstCharacter->m_pClump )
			pcmUIManager->m_pcsAgcmRender->ResetLighting( pstCharacter->m_pClump );
	}

	pcsCharacter = ppmCharacter->GetCharacter( lCID );
	if( !pcsCharacter )		return;

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )GetModule( "AgcmUICharacter" );
	if( pcmUICharacter )
	{
		pcmUICharacter->SetCharacter(pcsCharacter);
		pstCharacter = pcmCharacter->GetCharacterData(pcsCharacter);
		if( pstCharacter && pstCharacter->m_pClump )
			pcmUIManager->m_pcsAgcmRender->CustomizeLighting( pstCharacter->m_pClump, LIGHT_LOGIN_SELECT );
	}


	m_ControlCharacter.OnDoubleClickCharacter( lCID );

	m_lLoginMainSelectCID	= lCID;
	g_ISelectCID			= lCID;

	UpdateCompenInfo();
	UpdateSlotCharInfo();

	// 이름변경창이 떠 있을수 있으니 닫아준다.
	pcmUIManager->ThrowEvent( m_lEvent[ AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE ] );
	pcmUIManager->SetUserDataRefresh( m_pstUserDataDummy );
}


BOOL AgcmUILogin::UpdateCompenInfo()
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	pcmUIManager->ThrowEvent( m_lEvent[AGCMUILOGIN_EVENT_HIDE_COMPEN] );

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgpdCharacter *pcsCharacter = ppmCharacter->GetCharacter(m_lLoginMainSelectCID);
	if( !pcsCharacter )							return FALSE;
	if( !m_AgcdLoginCompenMaster.m_lCompenID )	return FALSE;
		
	INT32 lCharacterIndex = -1;
	for( INT32 i = 0; i < AGCMUILOGIN_MY_CHARACTER_NUM; i++ )
	{
		if( !_tcscmp(m_szMyCharacterInfo[i], pcsCharacter->m_szID))
		{
			lCharacterIndex = i;
			break;
		}
	}

	if( lCharacterIndex < 0 )
		return FALSE;

	if( _tcslen(m_AgcdLoginCompenMaster.m_szSelectedCharID) > 0 && 
		_tcscmp( m_AgcdLoginCompenMaster.m_szSelectedCharID, pcsCharacter->m_szID ) )
		return FALSE;

	BOOL bShowCompensationButton = FALSE;
	if (AGPMLOGINDB_COMPEN_TYPE_CHAR == m_AgcdLoginCompenMaster.m_eType || AGPMLOGINDB_COMPEN_TYPE_ALLC == m_AgcdLoginCompenMaster.m_eType)
	{
		if( 0 == _tcscmp(pcsCharacter->m_szID, m_AgcdLoginCompenMaster.m_szCharID ) )
			bShowCompensationButton = TRUE;
		if (0 == _tcscmp(pcsCharacter->m_szID, m_AgcdLoginCompenMaster.m_szCharID1))
			bShowCompensationButton = TRUE;
		if (0 == _tcscmp(pcsCharacter->m_szID, m_AgcdLoginCompenMaster.m_szCharID2))
			bShowCompensationButton = TRUE;
	}
	else
	{
		bShowCompensationButton = TRUE;
	}

	if (bShowCompensationButton)
	{
		m_lCharacterIndex = lCharacterIndex;
		pcmUIManager->ThrowEvent(m_lEvent[ AGCMUILOGIN_EVENT_SHOW_COMPEN_CHAR01 + lCharacterIndex ]);
	}

	pcmUIManager->SetUserDataRefresh(m_pstCompen);
	return TRUE;
}


void AgcmUILogin::CharacterDoubleClick(INT32 lCID)
{
	ASSERT( AGCMLOGIN_MODE_CHARACTER_SELECT == GetLoginMode() );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )GetModule( "AgcmUICharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );

	AgpdCharacter*	pcsCharacter = ppmCharacter->GetCharacter( lCID );
	if( !pcsCharacter )		return;

	pcmUICharacter->ReleaseCharacter();

	m_ControlCharacter.OnDoubleClickCharacter( lCID );

	m_lLoginMainSelectCID	= lCID;
	g_ISelectCID			= lCID;

	if( SelectCharacterAtLoginMain( lCID ) )
	{
		m_lLoginMainSelectCID = AP_INVALID_CID;
		g_pEngine->WaitingDialog( NULL, ClientStr().GetStr( STI_CONNECTING_SERVER ) );

		AgcmEventPointLight* pcmEventPointLight = ( AgcmEventPointLight* )GetModule( "AgcmEventPointLight" );
		if( pcmEventPointLight )
		{
			pcmEventPointLight->m_bReturnLoginLobby = FALSE;
		}

		AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
		if( !pcmCharacter ) return;

		AgcdCharacter*	pstCharacter = pcmCharacter->GetCharacterData( pcsCharacter );
		if( pstCharacter && pstCharacter->m_pClump )
			pcmUIManager->m_pcsAgcmRender->ResetLighting( pstCharacter->m_pClump );

		// 변신 및 진화해제 버튼 활성여부 초기화
		pcmUICharacter->OnToggleBtnRestoreTransform( FALSE );
		pcmUICharacter->OnToggleBtnRestoreEvolution( FALSE );
	}
}

BOOL AgcmUILogin::SelectCharacterAtLoginMain(INT32 lCID)
{
	if (GetLoginMode() != AGCMLOGIN_MODE_CHARACTER_SELECT)
		return FALSE;

	if ( lCID == AP_INVALID_CID ) return FALSE;

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )GetModule( "AgcmUICharacter" );
	if( pcmUICharacter )
	{
		pcmUICharacter->ReleaseCharacter();
	}

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgpmFactors* ppmFactors = ( AgpmFactors* )GetModule( "AgpmFactors" );
	AgpdCharacter	*pcsCharacter	= ppmCharacter->GetCharacter(lCID);
	if (pcsCharacter)
	{
		m_lUnion	= ppmFactors->GetRace(&pcsCharacter->m_csFactor);
	}

	BOOL bRename = FALSE;
	AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
	pcmLogin->SelectCharacter( lCID, bRename )									;
	if (bRename)
	{
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_RENAME_OPEN]);
		pcmUIManager->SetUserDataRefresh(m_pstUserDataOldCharID);
		return FALSE;
	}

	// 선택된 캐릭터를 사용한다고 지정
	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
	if( pcmUIItem )
	{
		pcmUIItem->m_AutoPickUp.UseCharacter( pcsCharacter->m_szID );
	}

	SetLoginMode( AGCMLOGIN_MODE_POST_LOGIN_LOADING );
	return TRUE;
}

void AgcmUILogin::CreateCharacter( AgpdCharacter* pcsCharacter, INT32 lID, INT32 lRace, INT32 lClass )
{
	if( !pcsCharacter )		return;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgpmFactors* ppmFactors = ( AgpmFactors* )GetModule( "AgpmFactors" );

	stPositionSet* pPosition = GetUILoginSetting()->GetCreatePosition( ( AuRaceType )ppmFactors->GetRace( &pcsCharacter->m_pcsCharacterTemplate->m_csFactor ) );
	if( pPosition )
	{			
		ppmCharacter->UpdatePosition( pcsCharacter, ( AuPOS* )( &pPosition->m_vPos ), FALSE );	//position
		ppmCharacter->TurnCharacter( pcsCharacter, 0.f, pPosition->m_rRotate );	//rotate

		m_ControlCamera.OnChangeRace( ( AuRaceType )lRace, ( AuCharClassType )lClass );

		AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );	
		AgcdCharacter* pstCharacter = pcmCharacter->GetCharacterData( pcsCharacter );

		float fMoveOffset = GetUILoginSetting()->GetMoveOffset( ( AuRaceType )lRace, ( AuCharClassType )lClass );
		m_ControlCharacter.InitializeCreateCharacter( lID, pcsCharacter, pstCharacter, pPosition->m_vPos, fMoveOffset );
	}
}


//	My Character
//====================================================================
//
BOOL AgcmUILogin::AddMyCharacterInfo(TCHAR *pszName, INT32 lIndex)
{
	if (m_bMyCharacterFlag[lIndex])
	{
		for (int i = 0; i < AGCMUILOGIN_MY_CHARACTER_NUM; ++i)
		{
			if (!m_bMyCharacterFlag[i])
			{
				lIndex	= i;
				break;
			}
		}
	}

	ZeroMemory(m_szMyCharacterInfo[lIndex], sizeof(TCHAR) * AGCMUILOGIN_MY_CHARACTER_INFO);
	_tcsncpy(m_szMyCharacterInfo[lIndex], pszName, AGCMUILOGIN_MY_CHARACTER_INFO);

	m_bMyCharacterFlag[lIndex] = TRUE;	

	return TRUE;
}


BOOL AgcmUILogin::RemoveMyCharacterInfo(TCHAR *pszName)
{
	INT32 lIndex = GetMyCharacterInfoIndex(pszName);

	if (lIndex < 0 || lIndex >= AGCMUILOGIN_MY_CHARACTER_NUM)
		return FALSE;

	if (m_bMyCharacterFlag[lIndex])
	{
		ZeroMemory(m_szMyCharacterInfo[lIndex], sizeof(TCHAR) * AGCMUILOGIN_MY_CHARACTER_INFO);
		m_bMyCharacterFlag[lIndex]	=	FALSE	;
	}

	return TRUE;
}


BOOL AgcmUILogin::UpdateSlotCharInfo()
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );

	if (!m_bMyCharacterFlag[0])
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT01_NOT_VIEW]);
	else
	{
		AgpdCharacter	*pcsCharacter	= ppmCharacter->GetCharacter(m_szMyCharacterInfo[0]);
		if (pcsCharacter)
		{
			if (m_lLoginMainSelectCID == pcsCharacter->m_lID)
				pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT01_SELECT]);
			else
				pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT01_UNSELECT]);
		}
		else
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT01_NOT_VIEW]);
		}
	}

	if (!m_bMyCharacterFlag[1])
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT02_NOT_VIEW]);
	else
	{
		AgpdCharacter	*pcsCharacter	= ppmCharacter->GetCharacter(m_szMyCharacterInfo[1]);
		if (pcsCharacter)
		{
			if (m_lLoginMainSelectCID == pcsCharacter->m_lID)
				pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT02_SELECT]);
			else
				pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT02_UNSELECT]);
		}
		else
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT02_NOT_VIEW]);
		}
	}

	if (!m_bMyCharacterFlag[2])
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT03_NOT_VIEW]);
	else
	{
		AgpdCharacter	*pcsCharacter	= ppmCharacter->GetCharacter(m_szMyCharacterInfo[2]);
		if (pcsCharacter)
		{
			if (m_lLoginMainSelectCID == pcsCharacter->m_lID)
				pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT03_SELECT]);
			else
				pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT03_UNSELECT]);
		}
		else
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_SLOT03_NOT_VIEW]);
		}
	}

	return TRUE;
}


INT32 AgcmUILogin::GetMyCharacterCount()
{
	INT32 lMyCharacterCount  = 0	;

	for ( INT32 i = 0 ; i < AGCMUILOGIN_MY_CHARACTER_NUM ; ++i )
		if ( m_bMyCharacterFlag[i] != FALSE ) lMyCharacterCount++;

	return lMyCharacterCount;
}


INT32 AgcmUILogin::GetMyCharacterInfoIndex(TCHAR *pszName)
{
	for (INT32 i = 0 ; i < AGCMUILOGIN_MY_CHARACTER_NUM ; ++i)
	{
		if(FALSE != m_bMyCharacterFlag[i])	
		{
			if (0 == _tcscmp(pszName, m_szMyCharacterInfo[i]))
				return i;
		}
	}

	return -1;
}


INT32 AgcmUILogin::GetMyCharacterInfoIndex(INT32 lID)
{
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgpdCharacter *	pcsCharacter = ppmCharacter->GetCharacter(lID);
	if (!pcsCharacter)
		return -1;

	return GetMyCharacterInfoIndex(pcsCharacter->m_szID);
}

void AgcmUILogin::MyCharacterAllUpdatePosition()
{
	AgpdCharacter* pdCharacter = NULL;

	for ( UINT32 i = 0 ; i < 5 ; ++i )
	{
		pdCharacter = NULL;

		if ( m_bMyCharacterFlag[i] )
		{
			AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
			AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
			AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );

			pdCharacter = ppmCharacter->GetCharacter(m_szMyCharacterInfo[i]);
			if ( pdCharacter )
			{
				if ( m_lUnion == AUUNION_TYPE_RED_HEART )
				{
					stPositionSet* pPosition = GetUILoginSetting()->GetSelectPosition( i );
					if( pPosition )
					{
						ppmCharacter->UpdatePosition( pdCharacter, ( AuPOS* )( &pPosition->m_vPos ), FALSE);	//position
						ppmCharacter->TurnCharacter( pdCharacter, 0.f, pPosition->m_rRotate );	//rotate
					}
				}
				else if ( m_lUnion == AUUNION_TYPE_BLUE_BLOOD )
				{
					// Position Update 
					ppmCharacter->UpdatePosition( pdCharacter, &(posAgcmUILoginOrcMyCharacterPos[i]), FALSE );

					// Character Rotete 
					ppmCharacter->TurnCharacter( pdCharacter, 0.0f, fAgcmUILoginOrcMyCharacterAngle[i] );
				}

				AuRaceType eRaceType = ( AuRaceType )ppmFactor->GetRace( &pdCharacter->m_csFactor );
				AuCharClassType eClassType = ( AuCharClassType )ppmFactor->GetClass( &pdCharacter->m_csFactor );
				if( eRaceType == AURACE_TYPE_DRAGONSCION )
				{
					if( eClassType != AUCHARCLASS_TYPE_MAGE )
					{
						pcmCharacter->OnChagneEquipItemByEvolution( pdCharacter, eClassType );
					}
					//else
					//{
					//	pcmCharacter->StartCharEvolutionByTID( pdCharacter, pdCharacter->m_lTID1 );
					//}
					else
					{
						// 서머너의 경우 특정 몇몇 아이템만 처리해준다.. 카론만..
						pcmCharacter->OnEquipCharonForSummoner( pdCharacter );
					}
				}
			}
		}
	}
}


void AgcmUILogin::DeleteAllMyCharacter()
{
	for ( INT32 i = 0 ; i < AGCMUILOGIN_MY_CHARACTER_NUM ; ++i )
	{
		if ( m_bMyCharacterFlag[i] )
		{
			AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
			AgpdCharacter* pdCharacter = ppmCharacter->GetCharacter(m_szMyCharacterInfo[i]);
			if(!pdCharacter)
				continue;

			ppmCharacter->RemoveCharacterFromMap(pdCharacter);
		
			if ( ppmCharacter->GetCharacter(g_ISelectCID) == pdCharacter )
				ppmCharacter->RemoveCharacter(pdCharacter->m_lID, FALSE, TRUE);
			else
				ppmCharacter->RemoveCharacter(m_szMyCharacterInfo[i]);

			ZeroMemory(m_szMyCharacterInfo[i], sizeof(TCHAR) * AGCMUILOGIN_MY_CHARACTER_INFO);
			m_bMyCharacterFlag[i] = FALSE ;
		}
	}

	m_ControlCharacter.OnClearCharacter();
}


void AgcmUILogin::RemoveSelectedMyCharacter()
{
	if (AP_INVALID_CID == m_lLoginMainSelectCID)
		return;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgpdCharacter *pAgpdCharacter = ppmCharacter->GetCharacter(m_lLoginMainSelectCID);
	if (NULL == pAgpdCharacter)
	{
		m_lLoginMainSelectCID = AP_INVALID_CID;
		return;
	}

	AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
	if(pcmLogin)
	{
		// 비밀번호를 함께 넘겨준 뒤 NULL로 초기화 시킨다.
		//pcmLogin->SendRemoveCharacter(pcmLogin->m_szAccount, pcmLogin->m_szWorldName, pAgpdCharacter->m_szID, pcmLogin->m_szConfirmPassword);
		pcmLogin->SendRemoveCharacter(pcmLogin->m_szAccount, pcmLogin->m_szWorldName, pAgpdCharacter->m_szID, 0);
		ZeroMemory(pcmLogin->m_szConfirmPassword, sizeof(pcmLogin->m_szConfirmPassword));
	}
	m_ControlCharacter.UnSelectCharacter();
	m_lLoginMainSelectCID = AP_INVALID_CID;
	UpdateCompenInfo();
	UpdateSlotCharInfo();
}

//	Create mode helper
//==========================================================
//

//	Loading window
//===========================================================
//
BOOL AgcmUILogin::OpenLoadingWindow(INT32 lRace)
{
	OutputDebugString("======== Loading Window Open\n");

	g_pEngine->WaitingDialogEnd();
	m_ControlPostFX.OnStopPostFX();

	if (g_pEngine && g_pEngine->m_pCurD3D9Device)						// video ram managed texture flush
		g_pEngine->m_pCurD3D9Device->EvictManagedResources();

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_LOADING_WINDOW_OPEN], 0, TRUE);

	switch ((AuRaceType) lRace) {
	case AURACE_TYPE_HUMAN:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_LOADING_HUMAN_BACKGROUND]);
		break;

	case AURACE_TYPE_ORC:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_LOADING_HUMAN_BACKGROUND]);
		break;

	case AURACE_TYPE_MOONELF:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_LOADING_HUMAN_BACKGROUND]);
		break;

	case AURACE_TYPE_DRAGONSCION:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_LOADING_HUMAN_BACKGROUND]);
		break;
	}

	m_LoadingWindow.OnOpen( pcmUIManager );
	return TRUE;
}


void AgcmUILogin::CloseLoadingWindow()
{
	OutputDebugString("======== Loading Window Close\n");

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	pcmUIManager->ShowCursor(TRUE);
	pcmUIManager->ThrowEvent( m_lEvent[AGCMUILOGIN_EVENT_LOADING_WINDOW_CLOSE], 0, TRUE );

	g_pEngine->SetRenderMode(TRUE, TRUE);
	m_LoadingWindow.OnClose( pcmUIManager );
}




//	Login Result
BOOL AgcmUILogin::OnLoginResult(INT32 lResult, CHAR *psz)
{
	g_pEngine->WaitingDialogEnd();
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	switch (lResult)
	{
	case AGPMLOGIN_RESULT_ACCOUNT_BLOCKED :
		{
			CHAR *pszFormat = pcmUIManager->GetUIMessage(UI_MESSAGE_ID_ACCOUNT_BLOCKED);
			CHAR sz[200]; ZeroMemory(sz, sizeof(sz));
			if (pszFormat && psz)
			{
				sprintf(sz, pszFormat, psz);
				pcmUIManager->ActionMessageOKDialog(sz);
			}
		}
		break;

	case AGPMLOGIN_RESULT_INVALID_ACCOUNT :
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_ID_FALIED]);


#ifdef _AREA_KOREA_
		// 로그인에 실패하면 종료시킨다.
		PostQuitMessage(0);
		g_pEngine->OnTerminate();
		return FALSE;
#endif
		break;

	case AGPMLOGIN_RESULT_INVALID_PASSWORD :
		{
			OnAddLoginRetryCount( pcmUIManager );
		}
		break;	

	case AGPMLOGIN_RESULT_INVALID_PASSWORD_LIMIT_EXCEED :
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_DISCONNECT_BY_INVALID_PW]);
			RsEventHandler(rsQUITAPP, NULL);

			if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
				::ShellExecute( NULL, "open", "http://reg.sdo.com", NULL, NULL, SW_SHOW );
			else if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
				::ShellExecute( NULL, "open", "http://www.codemasters.com/cog", NULL, NULL, SW_SHOW );
			else
				::ShellExecute( NULL, "open", "http://archlord.webzen.co.kr", NULL, NULL, SW_SHOW );

			Sleep(1000);
		}
		break;

	case AGPMLOGIN_RESULT_NOT_APPLIED :
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_NOT_APPLIED]);
		break;

	case AGPMLOGIN_RESULT_NOT_ENOUGH_AGE :
		{
			if( AP_SERVICE_AREA_KOREA == g_eServiceArea)
			{
				AgcmWorld* pcmWorld = ( AgcmWorld* )GetModule( "AgcmWorld" );
				if( pcmWorld )
				{
					if( !pcmWorld->m_pAgpdWorldSelected || !pcmWorld->m_pAgpdWorldSelected->IsNC17() )
						pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_NOT_ENOUGH_AGE]);
					else
						pcmUIManager->ActionMessageOKDialog("선택된 서버는 만18세 미만의 청소년이 이용하실 수 없습니다.");
				}

				SetIsServerSelectComplete( FALSE );
			}
		}
		break;

	case AGPMLOGIN_RESULT_NOT_BETA_TESTER :
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_NOT_BETA_TESTER]);
		break;

	case AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT :
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_DUPLICATE_ACCOUNT]);
			RsEventHandler(rsQUITAPP, NULL);
		}
		break;

	case AGPMLOGIN_RESULT_UNMAKABLE_CHAR_NAME :
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAIL_INVALID_NAME]);

			if (FALSE == m_bRenameBlock)
			{
				SetLoginMode(AGCMLOGIN_MODE_CHARACTER_CREATE); // set mode to create mode
			}

			m_bRenameBlock = FALSE;
		}
		break;

	case AGPMLOGIN_RESULT_CHAR_NAME_ALREADY_EXIST :
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAIL_SAME_NAME]);

		// if not rename mode
		if (FALSE == m_bRenameBlock)
			SetLoginMode(AGCMLOGIN_MODE_CHARACTER_CREATE); // set mode to create mode

		m_bRenameBlock = FALSE;
		break;

	case AGPMLOGIN_RESULT_FULL_SLOT :
		break;

	case AGPMLOGIN_RESULT_RENAME_SUCCESS :
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE]);
			m_bRenameBlock = TRUE;
			m_lLoginMainSelectCID = AP_INVALID_CID;
		}
		break;

	case AGPMLOGIN_RESULT_RENAME_FAIL :
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_RENAME_FAIL]);
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE]);
			m_bRenameBlock = FALSE;
		}
		break;

	case AGPMLOGIN_RESULT_RENAME_SAME_OLD_NEW_ID :
		{
			pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_RENAME_FAIL_SAME_OLD_NEW_ID]);
			m_bRenameBlock = FALSE;
		}
		break;

	case AGPMLOGIN_RESULT_GAMESERVER_NOT_READY :
		pcmUIManager->Notice("", 0);
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_GAME_SERVER_NOT_READY]);
		break;

	case AGPMLOGIN_RESULT_GAMESERVER_FULL :
		SetIsServerSelectComplete( FALSE );
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_GAME_SERVER_FULL]);
		break;

	case AGPMLOGIN_RESULT_CANT_DELETE_CHAR_B4_1DAY :
		pcmUIManager->ActionMessageOKDialog(pcmUIManager->GetUIMessage(UI_MESSAGE_ID_CANT_DELETE_1DAY));
		break;

	case AGPMLOGIN_RESULT_CANT_DELETE_CHAR_GUILD_MASTER:
		pcmUIManager->ActionMessageOKDialog(pcmUIManager->GetUIMessage(UI_MESSAGE_ID_CANT_DELETE_MASTER));
		break;

	case AGPMLOGIN_RESULT_MIDNIGHTSHUTDOWN_UNDERAGE :
		pcmUIManager->ActionMessageOKDialog(pcmUIManager->GetUIMessage("MidnightShutdown"));
		break;

	default :
		break;
	}

	return TRUE;
}

INT32 AgcmUILogin::CalcMurdererLevelByPoint( INT32 nPoint )
{
	switch( nPoint )
	{
	case AGPMCHAR_MURDERER_LEVEL3_POINT :	return 3;	break;
	case AGPMCHAR_MURDERER_LEVEL2_POINT :	return 2;	break;
	case AGPMCHAR_MURDERER_LEVEL1_POINT :	return 1;	break;
	}

	return 0;
}




//	UI Function Callbacks
//===========================================================
//

BOOL AgcmUILogin::CBLoginMainCreateButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	// 이름변경창이 떠 있을수 있으니 닫아준다.
	pcmUIManager->ThrowEvent( pThis->m_lEvent[ AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE ] );

	// 이벤트 서버인경우 메세지박스를 출력하고 리턴
	AgpdWorld* ppdCurrentWorld = ( AgpdWorld* )pThis->GetCurrentApgdWorld();
	if( !ppdCurrentWorld ) return FALSE;

	if( ppdCurrentWorld->IsAimEvent() )
	{
		pcmUIManager->ActionMessageOKDialog( "이벤트서버에서는 신규캐릭터를 생성할 수 없습니다." );
		return FALSE;
	}	

	if (pThis->GetLoginMode() != AGCMLOGIN_MODE_CHARACTER_SELECT)
		return FALSE;

	if (pThis->GetMyCharacterCount() >= AGCMUILOGIN_MY_CHARACTER_NUM)
	{
		return FALSE;
	}

	pThis->AgcmUICharacterCreate::OnInitialize( pThis );
	pThis->SetLoginMode( AGCMLOGIN_MODE_WAIT_MY_NEW_CHARACTER );

	// Loading characters takes too long, so display a loading screen.
	pcmUIManager->ShowCursor(FALSE);
	// Turn off UI fade-in temporarily for the immediate display.
	pcmUIManager->EnableSmoothUI(FALSE);

	pThis->OpenLoadingWindow(AURACE_TYPE_HUMAN);

	pcmUIManager->EnableSmoothUI(TRUE);
	g_pEngine->SetRenderMode(FALSE, TRUE);

	pThis->m_LoadingWindow.OnRefresh( pcmUIManager, 0, 8 );
	g_pEngine->OnRender();

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pThis->GetModule( "AgcmUICharacter" );
	if( pcmUICharacter )
	{
		pcmUICharacter->ReleaseCharacter();
	}

	pcmUIManager->ThrowEvent( pThis->m_lEvent[AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_CLOSE] );

	AgcmLogin* pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );
	if ( pThis->m_lUnion == AUUNION_TYPE_RED_HEART )
	{
		if ( FALSE == pThis->m_bHumanBaseCharacterAddRequest )
		{
			// Character 요청하기 
			pcmLogin->SendGetBaseCharacterOfRace( pcmLogin->m_szAccount, AURACE_TYPE_HUMAN );
			pThis->m_bHumanBaseCharacterAddRequest = TRUE;
		}
		else
			pThis->SetLoginMode( AGCMLOGIN_MODE_CHARACTER_CREATE );

		//pcmUIManager->ThrowEvent( pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN] );

		switch (pcmUIManager->GetUIMode())
		{
		case AGCDUI_MODE_1024_768:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN]);
			break;
		case AGCDUI_MODE_1280_1024:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN_1280]);
			break;
		case AGCDUI_MODE_1600_1200:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN_1600]);
			break;
		}
	}
	else if ( pThis->m_lUnion == AUUNION_TYPE_BLUE_BLOOD )
	{
		if ( FALSE == pThis->m_bOrcBaseCharacterAddRequest )
		{
			// Character 요청하기 
			pcmLogin->SendGetBaseCharacterOfRace( pcmLogin->m_szAccount, AUUNION_TYPE_BLUE_BLOOD );
			pThis->m_bOrcBaseCharacterAddRequest = TRUE;
		}
		else
			pThis->SetLoginMode( AGCMLOGIN_MODE_CHARACTER_CREATE );

		//pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN]);

		switch (pcmUIManager->GetUIMode())
		{
		case AGCDUI_MODE_1024_768:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN]);
			break;
		case AGCDUI_MODE_1280_1024:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN_1280]);
			break;
		case AGCDUI_MODE_1600_1200:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN_1600]);
			break;
		}
	}

	//pThis->OnLoadSelectableCharacter();
	pThis->OnGetBtnControls( pcmUIManager );
	pThis->OnSelectRace( pThis->m_lUnion );

	return TRUE;
}

BOOL AgcmUILogin::CBLoginMainCancelButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	// 이름변경창이 떠 있을수 있으니 닫아준다.
	pcmUIManager->ThrowEvent( pThis->m_lEvent[ AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE ] );

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pThis->GetModule( "AgcmUICharacter" );
	if( pcmUICharacter )
	{
		pcmUICharacter->ReleaseCharacter();
	}

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	if( ppmCharacter )
	{
		ppmCharacter->RemoveAllCharacters();
	}
	pThis->InitAgcmUILogin();

	AgcmLogin* pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );
	pcmLogin->SendReturnToSelectServer(pcmLogin->m_szAccount);

	pThis->SetLoginMode(AGCMLOGIN_MODE_SERVER_SELECT);

	// already have world list
	if (pThis->m_bLoadSImpleUI)
	{
		switch (pcmUIManager->GetUIMode())
		{
		case AGCDUI_MODE_1024_768:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_2D_OPEN]);
			break;

		case AGCDUI_MODE_1280_1024:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_2D_1280_OPEN]);
			break;

		case AGCDUI_MODE_1600_1200:
			pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_2D_1600_OPEN]);
			break;
		}
	}

	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE]);
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_OPEN_SELECT_SERVER_WINDOW]);

	pThis->m_lLoginMainSelectCID = AP_INVALID_CID;
	pThis->SetIsServerSelectDialogOpen( TRUE );
	pThis->SetIsServerSelectComplete( FALSE );
	pThis->OnLoadServerList();

	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_CLOSE]);
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_HIDE_COMPEN]);

	// 현재 로드되어 있는 길드정보들을 삭제한다. 서버 이동시 동일길드명에 의한 오류를 방지하기 위해서이다.
	AgpmGuild* ppmGuild = ( AgpmGuild* )g_pEngine->GetModule( "AgpmGuild" );
	if( ppmGuild )
	{
		ppmGuild->OnInit();
	}

	return TRUE;
}

BOOL AgcmUILogin::CBLoginMainSelectButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;

	if (AP_INVALID_CID != pThis->m_lLoginMainSelectCID)
	{
		if (pThis->SelectCharacterAtLoginMain(pThis->m_lLoginMainSelectCID))
		{
			pThis->m_lLoginMainSelectCID = AP_INVALID_CID;
			g_pEngine->WaitingDialog( NULL, ClientStr().GetStr(STI_CONNECTING_SERVER) );

			AgcmEventPointLight* pcmEventPointLight = ( AgcmEventPointLight* )pThis->GetModule( "AgcmEventPointLight" );
			if( pcmEventPointLight )
			{
				pcmEventPointLight->m_bReturnLoginLobby = FALSE;
			}

			pThis->m_pcsPreSelectedCharacter	= NULL;
			pThis->m_lAddMyCharacterCount		= 0;
			pThis->m_lLoginMainSelectCID		= 0;

			ZeroMemory(pThis->m_szMyCharacterInfo, sizeof(pThis->m_szMyCharacterInfo));
			ZeroMemory(pThis->m_bMyCharacterFlag, sizeof(pThis->m_bMyCharacterFlag));

			pThis->DeleteAllMyCharacter();
		}
	}
	else
	{
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
		pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_ENTER_GAME_NOT_SELECTED_CHAR]);
	}

	return TRUE;
}


BOOL AgcmUILogin::CBLoginMainDeleteButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	// 이름변경창이 떠 있을수 있으니 닫아준다.
	pcmUIManager->ThrowEvent( pThis->m_lEvent[ AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE ] );

	// 이벤트 서버인경우 메세지박스를 출력하고 리턴
	AgpdWorld* ppdCurrentWorld = ( AgpdWorld* )pThis->GetCurrentApgdWorld();
	if( !ppdCurrentWorld ) return FALSE;

	if( ppdCurrentWorld->IsAimEvent() )
	{
		pcmUIManager->ActionMessageOKDialog( "이벤트서버에서는 캐릭터를 삭제할 수 없습니다." );
		return FALSE;
	}	

	AgpdCharacter	*pcsSelectCharacter	= ppmCharacter->GetCharacter(pThis->m_lLoginMainSelectCID);
	if (!pcsSelectCharacter)
		return TRUE;

	if (pData1)
	{
		AgcdUI	*pcsUI	= (AgcdUI *) pData1;

		INT32	lIndex	= 0;

		AgcdUIControl	*pcsUIControl	= pcmUIManager->GetSequenceControl(pcsUI, &lIndex);
		while (pcsUIControl)
		{
			if (pcsUIControl->m_pcsBase &&
				pcsUIControl->m_pcsBase->m_nType == AcUIBase::TYPE_EDIT)
			{
				if (((AcUIEdit *) pcsUIControl->m_pcsBase)->m_bReadOnly)
				{
					TCHAR	szBuffer[512];
					ZeroMemory(szBuffer, sizeof(szBuffer));

					_stprintf(szBuffer, _T("%s %s"), pcsSelectCharacter->m_szID,
						pcmUIManager->GetUIMessage(UI_MESSAGE_ID_LOGIN_CONFIRM_PASSWORD));

					((AcUIEdit *) pcsUIControl->m_pcsBase)->SetText(szBuffer);
				}
				else
				{
					((AcUIEdit *) pcsUIControl->m_pcsBase)->ClearText();
				}
			}

			pcsUIControl	= pcmUIManager->GetSequenceControl(pcsUI, &lIndex);
		}
	}
#if defined( _AREA_KOREA_ ) && !defined( USE_MFC )
	CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
	if( pWebzenAuth && pWebzenAuth->IsAutoLogin() )
	{
		pcmUIManager->ThrowEvent( pThis->m_lEvent[ AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION ] );
		// 국내도 비밀번호 물어보도록 변경
		//pcmUIManager->ThrowEvent( pThis->m_lEvent[ AGCMUILOGIN_EVENT_CONFIRM_PASSWORD ] );
	}
#elif defined(_AREA_KOREA_)
	//pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CONFIRM_PASSWORD]);
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION]);
#else
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CONFIRM_PASSWORD]);
#endif

	return TRUE;
}

BOOL AgcmUILogin::CBCharInfoClick01(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );

	if (0 >= _tcslen(pThis->m_szMyCharacterInfo[0]))
		return FALSE;

	AgpdCharacter *pAgpdCharacter = ppmCharacter->GetCharacter(pThis->m_szMyCharacterInfo[0]);
	if (NULL == pAgpdCharacter)	return FALSE;

	pThis->CharacterClick(pAgpdCharacter->m_lID);
	return TRUE;
}

BOOL AgcmUILogin::CBCharInfoClick02(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );

	if (0 >= _tcslen(pThis->m_szMyCharacterInfo[1]))
		return FALSE;

	AgpdCharacter *pAgpdCharacter = ppmCharacter->GetCharacter(pThis->m_szMyCharacterInfo[1]);
	if (NULL == pAgpdCharacter)
		return FALSE;

	pThis->CharacterClick(pAgpdCharacter->m_lID);
	return TRUE;
}

BOOL AgcmUILogin::CBCharInfoClick03(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );

	if (0 >= _tcslen(pThis->m_szMyCharacterInfo[2]))
		return FALSE;

	AgpdCharacter *pAgpdCharacter = ppmCharacter->GetCharacter(pThis->m_szMyCharacterInfo[2]);
	if (NULL == pAgpdCharacter)	return FALSE;

	pThis->CharacterClick(pAgpdCharacter->m_lID);
	return TRUE;
}


BOOL AgcmUILogin::CBCompensationClick01(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	return pThis->OnCompensationClick(0);
}


BOOL AgcmUILogin::CBCompensationClick02(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	return pThis->OnCompensationClick(1);
}


BOOL AgcmUILogin::CBCompensationClick03(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	return pThis->OnCompensationClick(2);	
}

BOOL AgcmUILogin::OnCompensationClick(INT32 lIndex)
{
	if (0 >= _tcslen(m_szMyCharacterInfo[lIndex])) return FALSE;
	if (0 == m_AgcdLoginCompenMaster.m_lCompenID) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgpdCharacter *pAgpdCharacter = ppmCharacter->GetCharacter(m_szMyCharacterInfo[lIndex]);
	if (NULL == pAgpdCharacter)	return FALSE;

	CHAR *pszFormat = NULL;
	BOOL bSelect = TRUE;

	if (_tcslen(m_AgcdLoginCompenMaster.m_szSelectedCharID) > 0)
	{
		if (0 == _tcscmp(m_AgcdLoginCompenMaster.m_szSelectedCharID, pAgpdCharacter->m_szID))
		{
			pszFormat = pcmUIManager->GetUIMessage(UI_MESSAGE_ID_COMPENSATION_CANCEL);
			bSelect = FALSE;
		}
	}
	else
	{
		pszFormat =  pcmUIManager->GetUIMessage(UI_MESSAGE_ID_COMPENSATION_CONFIRM);
	}

	if (!pszFormat) return FALSE;

	CHAR sz[200];
	sprintf(sz, pszFormat, m_szMyCharacterInfo[lIndex]);

	if (IDOK == pcmUIManager->ActionMessageOKCancelDialog(sz))
	{
		AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
		pcmLogin->SendCharacter4Compensation(pcmLogin->m_szAccount, NULL, m_szMyCharacterInfo[lIndex], bSelect);
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_HIDE_COMPEN]);
		if (bSelect)
		{
			_tcscpy(m_AgcdLoginCompenMaster.m_szSelectedCharID, m_szMyCharacterInfo[lIndex]);
			UpdateCompenInfo();
		}
		else
		{
			ZeroMemory(m_AgcdLoginCompenMaster.m_szSelectedCharID, sizeof(m_AgcdLoginCompenMaster.m_szSelectedCharID));
			UpdateCompenInfo();
		}
	}

	return TRUE;
}

BOOL AgcmUILogin::CBCompensationSetFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	if (0 == pThis->m_AgcdLoginCompenMaster.m_lCompenID ||
		0 >= _tcslen(pThis->m_AgcdLoginCompenMaster.m_szDescription))
		return FALSE;

	pThis->m_CompenTooltip.DeleteAllStringInfo();
	pThis->m_CompenTooltip.MoveWindow((INT32) pcmUIManager->m_v2dCurMousePos.x + 30,
		(INT32) pcmUIManager->m_v2dCurMousePos.y + 30,
		(INT32) pThis->m_CompenTooltip.w,
		(INT32) pThis->m_CompenTooltip.h);

	DWORD dwColor = 0xffffffff;
	pThis->m_CompenTooltip.AddString(pThis->m_AgcdLoginCompenMaster.m_szDescription, 14, dwColor);
	pThis->m_CompenTooltip.AddNewLine(14);
	pThis->m_CompenTooltip.ShowWindow( TRUE );

	return TRUE;
}

BOOL AgcmUILogin::CBCompensationKillFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;

	pThis->m_CompenTooltip.ShowWindow(FALSE);
	pThis->m_CompenTooltip.DeleteAllStringInfo();
	return TRUE;	
}

BOOL AgcmUILogin::CBOpenHangameWeb(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if( !pClass )		return FALSE;

	if( lTrueCancel )
		::ShellExecute( NULL, "open", "http://archlord.naver.com/hangameMove.nhn", NULL, NULL, SW_SHOW );
	return TRUE;
}

BOOL AgcmUILogin::CBUnionSelectWindowReturnIDPassworldInput(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)	return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );

	// disconnect and reconnect
	if( ppmCharacter )
	{
		ppmCharacter->RemoveAllCharacters();
	}

	pThis->InitAgcmUILogin();

	AgcmLogin* pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );
	pcmLogin->DisconnectLoginServer();
	pThis->ConnectLoginServer();
	return TRUE;
}

BOOL AgcmUILogin::CBConfirmPassword(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUILogin		*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	AgcdUIControl	*pcsEditControl	= (AgcdUIControl *)	pData1;
	AgcmLogin*		pcmLogin = (AgcmLogin*)pThis->GetModule("AgcmLogin");

	if (!pcsEditControl->m_pcsBase)
		return FALSE;

	TCHAR *pszPassword = (TCHAR *) ((AcUIEdit *) pcsEditControl->m_pcsBase)->GetText();
	if (!pszPassword)
		return FALSE;

	// 임시 변수에 캐릭터삭제시 입력한 비밀번호 저장(전송 후 초기화됩니다.)
	if(NULL != pcmLogin)
		sprintf(pcmLogin->m_szConfirmPassword, "%s", pszPassword);

	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION]);
	return TRUE;
}


BOOL AgcmUILogin::CBConfirmDeleteChar(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;

	pThis->RemoveSelectedMyCharacter();

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pThis->GetModule( "AgcmUICharacter" );
	if( pcmUICharacter )
	{
		pcmUICharacter->ReleaseCharacter();
	}
	return TRUE;
}


BOOL AgcmUILogin::CBCancelDeleteConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	return TRUE;
}

BOOL AgcmUILogin::CBCharRenameOkButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin		*pThis	= (AgcmUILogin *)	pClass;
	AgcdUIControl	*pIDControl			= (AgcdUIControl *) pData1;

	if (NULL == pIDControl || AcUIBase::TYPE_EDIT != pIDControl->m_lType)
		return FALSE;

	if (pThis->m_bRenameBlock)
		return FALSE;

	TCHAR* pszNewID = (TCHAR *) ((AcUIEdit *)(pIDControl->m_pcsBase ))->GetText();

	if (NULL == pszNewID || _T('\0') == *pszNewID)
		return FALSE;

	pThis->m_bRenameBlock = TRUE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );
	return pcmLogin->RenameCharacter(pThis->m_lLoginMainSelectCID, pszNewID);
}


BOOL AgcmUILogin::CBCharRenameCancelButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	pThis->m_bRenameBlock = FALSE;
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE]);
	return TRUE;
}


BOOL AgcmUILogin::CBOpenExitWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)	return FALSE;
	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	return TRUE;
}


BOOL AgcmUILogin::CBReturnGame(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass) return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );	
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );

	AgpdCharacter	*pcsSelfCharacter	= pcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return FALSE;

	INT8	cOperation		= AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_TOWN;
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= ppmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
		&cOperation,								// Operation
		&pcsSelfCharacter->m_lID,					// Character ID
		NULL,										// Character Template ID
		NULL,										// Game ID
		NULL,										// Character Status
		NULL,										// Move Packet
		NULL,										// Action Packet
		NULL,										// Factor Packet
		NULL,										// llMoney
		NULL,										// bank money
		NULL,										// cash
		NULL,										// character action status
		NULL,										// character criminal status
		NULL,										// attacker id (정당방위 설정에 필요)
		NULL,										// 새로 생성되서 맵에 들어간넘인지 여부
		NULL,										// region index
		NULL,										// social action index
		NULL,										// special status
		NULL,										// is transform status
		NULL,										// skill initialization text
		NULL,										// face index
		NULL,										// hair index
		NULL,										// Option Flag
		NULL,										// bank size
		NULL,										// event status flag
		NULL,										// remained criminal status time
		NULL,										// remained murderer point time
		NULL,										// nick name
		NULL,										// gameguard
		NULL										// last killed time in battlesquare
		);

	if (!pvPacket || nPacketLength < 1)	return FALSE;

	BOOL bSendResult = pThis->SendPacket(pvPacket, nPacketLength);
	ppmCharacter->m_csPacket.FreePacket(pvPacket);
	return bSendResult;
}


BOOL AgcmUILogin::CBReturnLoginCharacterSelectMode(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin *	pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	AgcmCharacter*	pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
	AgpmCharacter*	ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	AgcmLogin*		pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );
	AgcmUIMain*		pcsUIMain	= ( AgcmUIMain* )pThis->GetModule( "AgcmUIMain" );

	// Valid Check 	
	if (!pcmCharacter || !pcmCharacter->m_pcsSelfCharacter || !pcmLogin || !pcmLogin->m_szAccount )
		return FALSE;

	if( ppmCharacter->IsCombatMode( pcmCharacter->GetSelfCharacter() ) )
	{
		char* pMessage = pcmUIManager->GetUIMessage( "BattleNotUse" );
		if( pMessage )
			SystemMessage.ProcessSystemMessage( pMessage );
		return TRUE;
	}

	if( pcsUIMain )
		pcsUIMain->OnClearSystemGrid();

	//UI_Guild Clear
	AgcmUIGuild* pcmUIGuild = ( AgcmUIGuild* )pThis->GetModule( "AgcmUIGuild" );
	if( pcmUIGuild )
	{
		//나의 길드전은 강제로 없는걸로 셋팅~~~
		AgcmGuild* pcmGuild = ( AgcmGuild* )pThis->GetModule( "AgcmGuild" );
		if( pcmGuild )
		{
			AgpdGuild* ppdGuild = pcmGuild->GetSelfGuildLock();
			if( ppdGuild )
			{
				ppdGuild->m_cStatus = AGPMGUILD_STATUS_NONE;
				ppdGuild->m_Mutex.Release();
			}
		}

		pcmUIGuild->DrawBattleInfo( NULL, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_INIT );
	}

	// 거래창이 열려있는 경우 거래중지 패킷 전송
	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )g_pEngine->GetModule( "AgcmUIItem" );
	if( !pcmUIItem ) return FALSE;

	pcmUIItem->OnSendPrivateTradeCancel();

	// 지능형 NPC 다이얼로그 닫기
	AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )pThis->GetModule( "AgcmEventNPCDialog" );
	if( !pcmEventNPCDialog ) return FALSE;

	CExNPC* pExNPC = pcmEventNPCDialog->GetExNPC();
	if( !pExNPC ) return FALSE;

	pExNPC->OnHideNPCDialog();

	// 타이틀 임시정보 클리어
	AgcmTitle* pcmTitle = GetAgcmTitle();
	if( pcmTitle )
	{
		// 캐릭터 선택창으로 넘어가게 되므로 이 시점에서 현재까지 사용된 타이틀 정보를 초기화한다.
		pcmTitle->OnClearTitleState();
	}

	// 캐릭터 변신해제 및 진화해제 버튼 숨김
	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( pcmUICharacter )
	{
		pcmUICharacter->OnToggleBtnRestoreTransform( FALSE );
		pcmUICharacter->OnToggleBtnRestoreEvolution( FALSE );
	}

	AgcmReturnToLogin* pcmReturnToLogin = ( AgcmReturnToLogin* )pThis->GetModule( "AgcmReturnToLogin" );
	if( !pcmReturnToLogin ) return FALSE;

	return pcmReturnToLogin->SendPacketRequest();
}




//	Event(dialog) Callbacks
//============================================================
//
BOOL AgcmUILogin::CBLoginReturnRemoveCharacter(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (NULL == pClass)	return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;

	if (lTrueCancel)
	{
		pThis->RemoveSelectedMyCharacter();

		AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )pThis->GetModule( "AgcmUICharacter" );
		if( pcmUICharacter )
		{
			pcmUICharacter->ReleaseCharacter();
		}
	}

	return TRUE;
}


BOOL AgcmUILogin::CBLoginReturnRemoveDuplicateAccount(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (NULL == pClass)	return FALSE;

	AgcmUILogin *pThis = (AgcmUILogin *) pClass;
	return TRUE;
}


BOOL AgcmUILogin::CBLoginReturnDisconnect(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	RsEventHandler(rsQUITAPP, NULL);
	return TRUE;
}


BOOL AgcmUILogin::CBReturnNotTester(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	RsEventHandler(rsQUITAPP, NULL);
	return TRUE;
}


BOOL AgcmUILogin::CBResultNotApplied(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	RsEventHandler(rsQUITAPP, NULL);

	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
		::ShellExecute(NULL, "open", "http://reg.sdo.com", NULL, NULL, SW_SHOW);
	else
		::ShellExecute(NULL, "open", "http://archlord.naver.com/joinForm.nhn", NULL, NULL, SW_SHOW);

	Sleep(1000);
	return TRUE;
}


BOOL AgcmUILogin::CBIDFailed(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	return TRUE;
}


BOOL AgcmUILogin::CBPasswordFailed(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	RsEventHandler(rsQUITAPP, NULL);

	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
		::ShellExecute(NULL, "open", "http://reg.sdo.com", NULL, NULL, SW_SHOW);
	else
		::ShellExecute(NULL, "open", "http://archlord.webzen.co.kr", NULL, NULL, SW_SHOW);

	Sleep(1000);
	return TRUE;
}

//	Module Callback
//=============================================================

BOOL AgcmUILogin::CBEncryptCode(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	AgcmLogin* pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );

	pcmLogin->SendAccount(pcmLogin->m_szAccount, pcmLogin->m_szPassword, pcmLogin->m_nNID);
	pcmUIManager->SetLoginMode(TRUE);
	return TRUE;
}

BOOL AgcmUILogin::CBInvalidClientVersion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)	return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	g_pEngine->MessageDialog(NULL, ClientStr().GetStr(STI_WRONG_CLIENT));
	return TRUE;
}

BOOL AgcmUILogin::CBGetUnionInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	INT32		lUnion = *((INT32 *)pData);

	if (AUUNION_TYPE_NONE == lUnion)
	{
		pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_CLOSE]);
		pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_UNION_SELECT_OPEN]);
		pThis->SetLoginMode(AGCMLOGIN_MODE_UNION_SELECT);
	}
	else
		pThis->m_lUnion = lUnion;

	ZeroMemory(&pThis->m_AgcdLoginCompenMaster, sizeof(pThis->m_AgcdLoginCompenMaster));
	ZeroMemory(pThis->m_szItemDetails, sizeof(pThis->m_szItemDetails));
	return TRUE;
}


BOOL AgcmUILogin::CBGetMyCharacterName(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData) return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	INT32		lIndex = *((INT32 *) pData);
	TCHAR*		pszCharacterName = (TCHAR *) pCustData;

	// My Character Info Setting 
	if (AGCMUILOGIN_MY_CHARACTER_NUM <= lIndex) return FALSE;
	pThis->AddMyCharacterInfo(pszCharacterName, lIndex );
	return TRUE;
}


BOOL AgcmUILogin::CBGetMyCharacterNameFinish(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	pThis->SetLoginMode(AGCMLOGIN_MODE_WAIT_MY_CHARACTER); // ready to add character

	//@{ Jaewon 20051027
	// Moved here from CBGetUnion().
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CLOSE_SELECT_SERVER_WINDOW]);
	if (pThis->m_bLoadSImpleUI)
		pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_2D_CLOSE]);
	//@} Jaewon

	//@{ Jaewon 20051024
	// Loading characters takes too long, so display a loading screen.
	pcmUIManager->ShowCursor(FALSE);
	//@{ Jaewon 20051027
	// Turn off UI fade-in temporarily for the immediate display.
	pcmUIManager->EnableSmoothUI(FALSE);
	pThis->OpenLoadingWindow(AURACE_TYPE_HUMAN);
	pcmUIManager->EnableSmoothUI(TRUE);
	//@} Jaewon
	g_pEngine->SetRenderMode(FALSE, TRUE);
	pThis->m_LoadingWindow.OnRefresh( pcmUIManager, 0, pThis->GetMyCharacterCount() );
	g_pEngine->OnRender();
	//@} Jaewon

	pThis->m_lAddMyCharacterCount = 0;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );
	pcmLogin->SendGetCharacters(pcmLogin->m_szWorldName, pcmLogin->m_szAccount);
	return TRUE;
}


BOOL AgcmUILogin::CBGetMyCharacterInfoFinish(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	pcmUIManager->ShowCursor(TRUE);
	pcmUIManager->CloseAllUI();
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_OPEN]);

	pThis->SetLoginMode(AGCMLOGIN_MODE_CHARACTER_SELECT);

	//@{ Jaewon 20051024
	pThis->CloseLoadingWindow();
	pcmUIManager->ThrowEvent( pcmUIManager->m_nEventToggleUIOpen );
	pThis->MyCharacterAllUpdatePosition();
	//@} Jaewon

	pcmUIManager->SetUserDataRefresh(pThis->m_pstUserDataDummy);
	pThis->UpdateSlotCharInfo();
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_HIDE_COMPEN]);
	return TRUE;
}


BOOL AgcmUILogin::CBEnterGameEnd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	AgcmSound * pcmSound = ( AgcmSound* )pThis->GetModule( "AgcmSound" );

	pcmSound->CloseAll3DSample();
	pcmSound->CloseAllSample();

	// login process ended
	pThis->SetLoginMode(AGCMLOGIN_MODE_POST_LOGIN_LOADING);

	pcmUIManager->ShowCursor(FALSE);
	pThis->m_AgcdLoginCompenMaster.ClearCharID();

	// activate Map load end callback
	AgcmMap* pcmMap = ( AgcmMap* )pThis->GetModule( "AgcmMap" );
	if( pcmMap )
	{
		pcmMap->SetMapLoadEndCBActive();
	}

	pThis->OpenLoadingWindow(pThis->m_lUnion);

	// 원래대로 돌려놓기
	g_pEngine->SetProjection( DEFAULT_VIEWWINDOW );
	g_pEngine->SetRenderMode(FALSE, TRUE);

	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_CLOSE]);

	// Character Remove 
	pThis->DeleteAllMyCharacter();

	//. 2006. 10. 9. Nonstopdj
	g_ISelectCID = 0;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	if( ppmCharacter )
	{
		ppmCharacter->RemoveAllCharacters();
	}

	pThis->PreLoadAllCharacterTypes();

	// open main UI
	pcmUIManager->OpenMainUI();

	// init map character
	if( pcmMap )
	{
		pcmMap->DeleteMyCharacterCheck();
	}

	// set UI mode from login to game
	pcmUIManager->SetLoginMode(FALSE);

	if( pcmMap )
	{
		pcmMap->DeleteMyCharacterCheck();
	}

	pcmUIManager->m_pcsAgcmRender->m_bLogin = FALSE;

	pThis->InitAgcmUILogin();

	// restore draw range
	AgcmRender* pcmRender = ( AgcmRender* )pThis->GetModule( "AgcmRender" );
	if( pcmRender )
	{
		pcmRender->m_iDrawRange = pThis->m_iBackUpDrawRange;
	}
	
	//. 2006. 4. 17. nonstopdj
	//. call AgcmCamera2::InitEyeSubAtValue.
	AgcmCamera2* pAgcmCamera2 = (AgcmCamera2*)pThis->GetModule("AgcmCamera2");
	ASSERT(pAgcmCamera2);
	pAgcmCamera2->InitEyeSubAtValue();

	if (0 != pThis->m_AgcdLoginCompenMaster.m_lCompenID
		&& 0 < _tcslen(pThis->m_AgcdLoginCompenMaster.m_szSelectedCharID))
		_tcscpy(pThis->m_szItemDetails, pThis->m_AgcdLoginCompenMaster.m_szItemDetails);
	ZeroMemory(&pThis->m_AgcdLoginCompenMaster, sizeof(pThis->m_AgcdLoginCompenMaster));
	pThis->m_lCharacterIndex = -1;

	AgcmUIGuild* pcmUIGuild = ( AgcmUIGuild* )pThis->GetModule( "AgcmUIGuild" );
	if( !pcmUIGuild ) return FALSE;

	pcmUIGuild->GetUIGuildBattleOffer().Clear();

	// 퀵 벨트를 0번으로 초기화
	AgcmUIMain* pcmUIMain = ( AgcmUIMain* )pThis->GetModule( "AgcmUIMain" );
	if( pcmUIMain )
	{
		pcmUIMain->OnInitQuickBeltGrid();
	}

	return TRUE;
}


BOOL AgcmUILogin::CBGetNewCharacterName(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	INT32		lIndex = *((INT32 *) pData);
	TCHAR*		pszCharacterName = (CHAR *) pCustData;

	if ( AGCMUILOGIN_MY_CHARACTER_NUM <= lIndex ) return FALSE;

	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_CLOSE]);
	pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_CHARACTER_CREATE_HELP_CLOSE]);

	pThis->AddMyCharacterInfo(pszCharacterName, lIndex);
	pThis->SetLoginMode(AGCMLOGIN_MODE_WAIT_MY_CHARACTER);
	pThis->UpdateSlotCharInfo();

	g_pEngine->WaitingDialogEnd();
	return TRUE;
}


BOOL AgcmUILogin::CBGetNewCharacterInfoFinish(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	pcmUIManager->ThrowEvent( pThis->m_lEvent[AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_OPEN]);

	pThis->SetLoginMode(AGCMLOGIN_MODE_CHARACTER_SELECT);
	pThis->UpdateSlotCharInfo();
	pThis->m_lCharacterIndex = -1;
	pThis->UpdateCompenInfo();
	return TRUE;
}


BOOL AgcmUILogin::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	TCHAR		*pszCharacterName = (TCHAR *) pData;

	// m_AutoPickUp 의 목록에서 이 캐릭터를 삭제한다.
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )pThis->GetModule( "AgcmUIItem" );
	if( ppmCharacter && pcmUIItem )
	{
		AgpdCharacter* ppdCharacter = ppmCharacter->GetCharacter( pszCharacterName );
		if( ppdCharacter )
		{
			pcmUIItem->m_AutoPickUp.DeleteCharacter( ppdCharacter->m_szID );
		}
	}

	pThis->RemoveMyCharacterInfo(pszCharacterName);

	--(pThis->m_lAddMyCharacterCount);

	pThis->UpdateSlotCharInfo();
	pThis->m_ControlCharacter.OnDeleteCharacter( pszCharacterName );
	return TRUE;
}

BOOL AgcmUILogin::CBLoginResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	INT32		lResult	= *((INT32 *) pData);
	CHAR		*psz = (CHAR *) pCustData;

	return pThis->OnLoginResult(lResult, psz);
}

BOOL AgcmUILogin::CBCompensationInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgcmUILogin	*pThis = (AgcmUILogin *) pClass;
	TCHAR *pszAccount = (TCHAR *) pData;
	AgcdLoginCompenMaster *pAgcdLoginCompenMasterSrc = (AgcdLoginCompenMaster *) pCustData;

	pThis->m_AgcdLoginCompenMaster.m_lCompenID = pAgcdLoginCompenMasterSrc->m_lCompenID;
	pThis->m_AgcdLoginCompenMaster.m_eType = pAgcdLoginCompenMasterSrc->m_eType;
	
	if (0 == strlen(pThis->m_AgcdLoginCompenMaster.m_szCharID))
		_tcscpy(pThis->m_AgcdLoginCompenMaster.m_szCharID, pAgcdLoginCompenMasterSrc->m_szCharID);
	else if (0 == strlen(pThis->m_AgcdLoginCompenMaster.m_szCharID1))
		_tcscpy(pThis->m_AgcdLoginCompenMaster.m_szCharID1, pAgcdLoginCompenMasterSrc->m_szCharID);
	else
		_tcscpy(pThis->m_AgcdLoginCompenMaster.m_szCharID2, pAgcdLoginCompenMasterSrc->m_szCharID);
	
	_tcscpy(pThis->m_AgcdLoginCompenMaster.m_szItemDetails, pAgcdLoginCompenMasterSrc->m_szItemDetails);
	_tcscpy(pThis->m_AgcdLoginCompenMaster.m_szDescription, pAgcdLoginCompenMasterSrc->m_szDescription);
	return TRUE;
}


BOOL AgcmUILogin::CBMapSectorChangePreCallback(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUILogin *	pThis = (AgcmUILogin*)pClass	;
	ApWorldSector *	pSectorNext = (ApWorldSector *) pData;
	ApWorldSector *	pSectorPrev = (ApWorldSector *) pCustData;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	if (pThis->GetLoginMode() == AGCMLOGIN_MODE_POST_LOGIN_LOADING)
	{
		pThis->m_LoadingWindow.SetIsPostLoadingWindow( TRUE );
	}

	// Sector의 거리가 멀면
	if ( !pThis->m_LoadingWindow.GetIsLoadingWindowOpen() && 
		( !pSectorPrev || !pSectorNext || GetSectorDistance( pSectorPrev , pSectorNext ) > 2))
	{
		pcmUIManager->ShowCursor(FALSE);

		// Main Thread에서 읽게 하고
		AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )pThis->GetModule( "AgcmResourceLoader" );
		pcmResourceLoader->m_bForceImmediate = TRUE;

		// Loading Window 열고
		pcmUIManager->Notice("", -1, NULL, -1, 0, 0.5f);

		pThis->OpenLoadingWindow(pThis->m_lUnion);
		g_pEngine->SetRenderMode(FALSE, TRUE);
	}

	return TRUE;
}


BOOL AgcmUILogin::CBMapSectorChangeCallback(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUILogin *	pThis = (AgcmUILogin*)pClass	;
	ApWorldSector *	pSectorNext = (ApWorldSector *) pData;
	ApWorldSector *	pSectorPrev = (ApWorldSector *) pCustData;

	if ( pThis->GetLoginMode() == AGCMLOGIN_MODE_POST_LOGIN_LOADING )
	{
		pThis->m_LoadingWindow.SetIsLoadingWindowOpen( TRUE );
	}

	return TRUE;
}

BOOL AgcmUILogin::CBMapLoadSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUILogin *	pThis = (AgcmUILogin *)	pClass;
	AgcmMap* pcmMap = ( AgcmMap* )pThis->GetModule( "AgcmMap" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	if (pThis->m_LoadingWindow.GetIsLoadingWindowOpen() && pcmMap )
	{
		INT32	lSectorsToLoad	= pcmMap->GetSectorsToLoad();
		pThis->m_ControlPostFX.OnStopPostFX();

		INT32 nLoadingMax = pThis->m_LoadingWindow.GetMaxLoadingCount();
		if( nLoadingMax < lSectorsToLoad )
		{
			nLoadingMax = lSectorsToLoad + 1;
		}

		pThis->m_LoadingWindow.OnRefresh( pcmUIManager, nLoadingMax - lSectorsToLoad, nLoadingMax );
		pcmUIManager->OnIdle( pThis->GetClockCount() );

		AgcmGrass* pcmGrass = ( AgcmGrass* )pThis->GetModule( "AgcmGrass" );
		if( pcmGrass )
		{
			pcmGrass->OnIdle( pThis->GetClockCount() );
		}

		g_pEngine->OnRender();
	}

	return TRUE;
}



BOOL AgcmUILogin::CBAddCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmUILogin::CBAddCharacter");

	AgcmUILogin* pThis = (AgcmUILogin*)pClass					;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	AgpmFactors* ppmFactors = ( AgpmFactors* )pThis->GetModule( "AgpmFactors" );
	AgpdCharacter*	ppdCharacter = (AgpdCharacter *) pData	;

	// DB에 등록된 My Character Add를 기다리고 있다면 
	if ( pThis->GetLoginMode() == AGCMLOGIN_MODE_WAIT_MY_CHARACTER )
	{
		AuPOS vPos = { 0.0f, 0.0f, 0.0f };
		RwReal rRotate = 0.0f;

		stPositionSet* pPosition = GetUILoginSetting()->GetSelectPosition( pThis->GetMyCharacterInfoIndex( ppdCharacter->m_szID ) );
		if( pPosition )
		{
			vPos.x = pPosition->m_vPos.x;
			vPos.y = pPosition->m_vPos.y;
			vPos.z = pPosition->m_vPos.z;

			rRotate = pPosition->m_rRotate;

			if (!pThis->m_lAddMyCharacterCount)
				pThis->m_pcsPreSelectedCharacter = ppdCharacter;
			++(pThis->m_lAddMyCharacterCount);

			ppdCharacter->m_stPos = vPos;

			ppmCharacter->AddCharacterToMap( ppdCharacter )	;
			ppmCharacter->TurnCharacter( ppdCharacter, 0.f, rRotate );

			AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
			AgcdCharacter* pcdCharacter = pcmCharacter->GetCharacterData(ppdCharacter);
			ASSERT(pcdCharacter);
			ASSERT( pcdCharacter->m_pClump && RpClumpGetFrame(pcdCharacter->m_pClump) );

			if( pcdCharacter->m_pClump && RpClumpGetFrame(pcdCharacter->m_pClump) )
			{
				RwV3d	offset = *RwMatrixGetAt(RwFrameGetLTM(RpClumpGetFrame(pcdCharacter->m_pClump)));

				AuRaceType erace = (AuRaceType)ppmFactors->GetRace(&ppdCharacter->m_pcsCharacterTemplate->m_csFactor);
				AuCharClassType eclass = (AuCharClassType)ppmFactors->GetClass(&ppdCharacter->m_pcsCharacterTemplate->m_csFactor);

				RwReal	forward	= GetUILoginSetting()->GetMoveOffset( erace, eclass );

				RwV3dScale(&offset, &offset, forward);
				RwV3d	selectedPos;
				RwV3dAdd(&selectedPos, (RwV3d*)(&vPos), &offset);

				pcmCharacter->OnUpdateCharacterCustomize( ppdCharacter, pcdCharacter );
				pThis->m_ControlCharacter.OnAddCharacter( ppdCharacter->ApBase::m_lID
					, ppdCharacter
					, pcdCharacter
					, selectedPos
					, *(const RwV3d*)(&vPos) );
			}
		}

		// 추가된 캐릭터의 CID 를 AutoPickUp 정보에 등록
		AgcmUIItem* pcmUIItem = ( AgcmUIItem* )pThis->GetModule( "AgcmUIItem" );
		if( pcmUIItem )
		{
			pcmUIItem->m_AutoPickUp.AddCharacter( ppdCharacter->m_szID, TRUE );
		}
	}
	else if ( pThis->GetLoginMode() == AGCMLOGIN_MODE_WAIT_MY_NEW_CHARACTER )
	{
		AuPOS	origin	= {0.f, 0.f, 0.f};
		ppmCharacter->UpdatePosition( ppdCharacter, &origin, FALSE);//position
	}

	if (pThis->GetLoginMode() == AGCMLOGIN_MODE_CHARACTER_CREATE ||
		pThis->GetLoginMode() == AGCMLOGIN_MODE_WAIT_MY_NEW_CHARACTER ||
		pThis->GetLoginMode() == AGCMLOGIN_MODE_WAIT_MY_CHARACTER)
	{
		if( pThis->m_LoadingWindow.GetIsLoadingWindowOpen() )
		{
			pThis->m_LoadingWindow.OnUpdateAdd( pcmUIManager );
			g_pEngine->OnRender();
		}
	}
	//@} Jaewon
	return TRUE;
}


BOOL AgcmUILogin::CBGameServerError(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ( NULL == pClass ) return FALSE;

	AgcmUILogin* pThis = (AgcmUILogin*)pClass				;
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );

	if ( pThis->GetLoginMode() != AGCMLOGIN_MODE_WAIT_DISCONNECT ) return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= NULL;

	// World에 Map, Character와 Item을 모두 지워준다 - AgcmCharacter의 Self Character 정보를 초기화 한다 
	if( pcmCharacter )
	{
		pcsSelfCharacter	= pcmCharacter->GetSelfCharacter();
		pcmCharacter->ReleaseSelfCharacter()		;
	}

	AgcmMap* pcmMap = ( AgcmMap* )pThis->GetModule( "AgcmMap" );
	if( pcmMap )
	{
		if ( pcmMap->m_pcsApmMap )
		{
			pcmMap->m_pcsApmMap->DeleteAllSector();
		}
	}

	if( ppmCharacter )
	{
		if (pcsSelfCharacter)
		{
			ppmCharacter->RemoveAllCharactersExceptOne(pcsSelfCharacter->m_lID);
			if( pcmCharacter )
			{
				pcmCharacter->m_bIsDeletePrevSelfCharacterWhenNewSelfCharacterisSet	= TRUE;
				pcmCharacter->m_pcsPrevSelfCharacter = pcsSelfCharacter;
			}
		}
		else
		{
			ppmCharacter->RemoveAllCharacters();
		}
	}

	// Set Mode
	pThis->SetLoginMode(AGCMLOGIN_MODE_PRE_LOGIN)	;
	pThis->ConnectLoginServer();
	return TRUE;
}


BOOL AgcmUILogin::CBResultDisconnectByAnotherUser(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUILogin* pThis = (AgcmUILogin*)pClass	;
	if ( pThis == NULL ) return FALSE			;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	pcmUIManager->ThrowEvent( pThis->m_lEvent[AGCMUILOGIN_EVENT_DISCONNECT_BY_ANOTHER_USER] )	;
	RsEventHandler(rsQUITAPP, NULL);	// Application 종료 
	return TRUE;
}


BOOL AgcmUILogin::CBDisconnectGameServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmUILogin			*pThis				= (AgcmUILogin *)		pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	BOOL				m_bIsDestroyNormal	= *(BOOL *)				pCustData;

	if (!m_bIsDestroyNormal)
	{
		// 비정상 종료다. 서버랑 접속이 종료되었다고 알려주고 사용자가 확인하면 클라이언트를 죽인다.		
		pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_DISCONNECTED_BY_SERVER]);
	}

	return TRUE;
}


BOOL AgcmUILogin::CBLoadingComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUILogin *pThis = (AgcmUILogin *) pClass;
	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )pThis->GetModule( "AgcmResourceLoader" );
	if( !pcmResourceLoader ) return FALSE;

	pcmResourceLoader->m_bForceImmediate = FALSE;
	return TRUE;
}

BOOL AgcmUILogin::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUILogin *pThis = (AgcmUILogin *) pClass;

	if (0 >= _tcslen(pThis->m_szItemDetails))
		return TRUE;

	AgpmItem* ppmItem = ( AgpmItem* )pThis->GetModule( "AgpmItem" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	AgpdLoginCompenItemList ItemList;
	if (ItemList.Parse(pThis->m_szItemDetails))
	{
		for (INT32 i = 0; i < ItemList.m_lCount; i++)
		{
			CHAR *pszFormat = pcmUIManager->GetUIMessage(UI_MESSAGE_ID_COMPENSATION_ITEM_PAID);
			AgpdItemTemplate *pAgpdItemTemplate = ppmItem->GetItemTemplate(ItemList.m_Items[i].m_lItemTID);
			if (pszFormat && pAgpdItemTemplate)
			{
				CHAR szMessage[200];

				if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
					_stprintf(szMessage, pszFormat, ItemList.m_Items[i].m_lItemQty, pAgpdItemTemplate->m_szName);
				else
					_stprintf(szMessage, pszFormat, pAgpdItemTemplate->m_szName, ItemList.m_Items[i].m_lItemQty);

				SystemMessage.ProcessSystemMessage(szMessage);
			}
		}
	}

	ZeroMemory(pThis->m_szItemDetails, sizeof(pThis->m_szItemDetails));
	return TRUE;	
}

BOOL AgcmUILogin::CBReturnToLoginEndProcess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;

	//. 2006. 4. 7. Nonstopdj.
	AgcmEff2* pcmEffect = ( AgcmEff2* )pThis->GetModule( "AgcmEff2" );
	if( pcmEffect )
	{
		pcmEffect->RemoveAllLoadingEffsetList();
	}

	AgcmMap* pcmMap = ( AgcmMap* )pThis->GetModule( "AgcmMap" );
	if( pcmMap )
	{
		pcmMap->ClearAllSectors();
	}

	BOOL bPrevMode = FALSE;
	AgcmResourceLoader* pcmResourceLoader = ( AgcmResourceLoader* )pThis->GetModule( "AgcmResourceLoader" );
	if( pcmResourceLoader )
	{
		bPrevMode = pcmResourceLoader->m_bForceImmediate;
		pcmResourceLoader->m_bForceImmediate = TRUE;
	}

	pThis->SetLoginMode(AGCMLOGIN_MODE_SERVER_SELECT);

	AgcmCamera2* pAgcmCamera2 = (AgcmCamera2*)pThis->GetModule("AgcmCamera2");
	ASSERT(pAgcmCamera2);

	AgcmEventNature* pcmEventNature = ( AgcmEventNature* )pThis->GetModule( "AgcmEventNature" );
	pThis->m_ControlCamera.SetCameraAndNature( pAgcmCamera2, pcmEventNature );

	AgcmEventEffect* pcmEventEffect = ( AgcmEventEffect* )pThis->GetModule( "AgcmEventEffect" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	pThis->m_ControlCharacter.SetExternalClassPointers( pcmCharacter, ppmCharacter, pcmEventEffect );
	pAgcmCamera2->bSetIdleType( AgcmCamera2::e_idle_login );

	if( pcmEventNature )
	{
		pcmEventNature->SetTimeForce( 12 );
	}

	AgcmEventPointLight* pcmEventPointLight = ( AgcmEventPointLight* )pThis->GetModule( "AgcmEventPointLight" );
	if( pcmEventPointLight )
	{
		pcmEventPointLight->EnablePointLight();
		//. 2006. 3. 7. Nonstopdj
		//. ref. AgcmEventPointLight::CBSkyChange
		pcmEventPointLight->m_bReturnLoginLobby = TRUE;
	}

	if( pcmResourceLoader )
	{
		pcmResourceLoader->m_bForceImmediate = bPrevMode;
	}

	return pThis->SetLoginProcess();
}


BOOL AgcmUILogin::CBGetWorldAll(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass )		return FALSE;

	AgcmUILogin* pThis = (AgcmUILogin*) pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	if( pThis->GetIsServerSelectDialogOpen() )
	{
		pThis->AgcmUIServerSelect::OnRefresh();
	}
	else
	{
		pcmUIManager->ThrowEvent( pThis->m_lEvent[AGCMUILOGIN_EVENT_OPEN_SELECT_SERVER_WINDOW] );
#ifdef USE_MFC
#else
#ifdef _AREA_JAPAN_
		BOOL bIsOpenServerSelectDialog = AP_SERVICE_AREA_JAPAN == g_eServiceArea ? TRUE : FALSE;
#elif defined(_AREA_KOREA_)
		CWebzenAuth* pWebzenAuth = CWebzenAuth::GetInstance();
		BOOL bIsOpenServerSelectDialog = !pWebzenAuth || pWebzenAuth->IsAutoLogin() ? FALSE : TRUE;
#else
		BOOL bIsOpenServerSelectDialog = TRUE;
#endif
		pThis->SetIsServerSelectDialogOpen( bIsOpenServerSelectDialog ); 
		pThis->SetIsServerSelectComplete( FALSE );
		g_pEngine->WaitingDialogEnd();
#endif //_USE_MFC_
	}

	AgcmWorld* pcmWorld = ( AgcmWorld* )pThis->GetModule( "AgcmWorld" );
	pcmWorld->ResetWorldGroup();	//한번만 하는거야
	pThis->OnLoadServerList();
	return TRUE;
}


BOOL AgcmUILogin::CBUpdateWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass; 
	pThis->OnLoadServerList();
	return TRUE;
}


BOOL AgcmUILogin::CBGetCharCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	pThis->OnLoadServerList();
	return TRUE;
}


BOOL AgcmUILogin::CBKeydownCloseAllUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;

	AgcmUILogin	*pThis		= (AgcmUILogin *)	pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	return pcmUIManager->ThrowEvent(pThis->m_lEvent[AGCMUILOGIN_EVENT_OPEN_EXIT_WINDOW]);
}




//	Display Callbacks
//===============================================================
//

BOOL AgcmUILogin::CBDisplaySlotCharName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if( !pClass ) return FALSE;
	if( !pcsSourceControl ) return FALSE;
	if( !pcsSourceControl->m_pcsParentControl ) return FALSE;

	INT32 nControlNumber = 0;
	sscanf( pcsSourceControl->m_pcsParentControl->m_szName, "CTL_BTN_CHARINFO%d", &nControlNumber );
	if( nControlNumber <= 0 || nControlNumber > AGCMUILOGIN_MY_CHARACTER_NUM ) return FALSE;

	AgcmUILogin	*pThis = ( AgcmUILogin* )pClass;
	BOOL bFlag = pThis->m_bMyCharacterFlag[ nControlNumber - 1 ];
	CHAR* pstrCharName = pThis->m_szMyCharacterInfo[ nControlNumber - 1 ];

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	if( bFlag )
	{
		AgpdCharacter* pcsCharacter	= ppmCharacter->GetCharacter( pstrCharName );
		if( pcsCharacter )
		{
			sprintf( szDisplay, "%s", pcsCharacter->m_szID );
		}
	}

	return TRUE;
}


BOOL AgcmUILogin::CBDisplaySlotCharLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if( !pClass ) return FALSE;
	if( !pcsSourceControl ) return FALSE;
	if( !pcsSourceControl->m_pcsParentControl ) return FALSE;

	INT32 nControlNumber = 0;
	sscanf( pcsSourceControl->m_pcsParentControl->m_szName, "CTL_BTN_CHARINFO%d", &nControlNumber );
	if( nControlNumber <= 0 || nControlNumber > AGCMUILOGIN_MY_CHARACTER_NUM ) return FALSE;

	AgcmUILogin	*pThis = ( AgcmUILogin* )pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	BOOL bFlag = pThis->m_bMyCharacterFlag[ nControlNumber - 1 ];
	CHAR* pstrCharName = pThis->m_szMyCharacterInfo[ nControlNumber - 1 ];

	//pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = 0xffb7dee8;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	if( bFlag )
	{
		AgpdCharacter* pcsCharacter	= ppmCharacter->GetCharacter( pstrCharName );
		if( pcsCharacter )
		{
			CHAR* pUIMsg = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_LOGIN_SLOT_CHAR_LEVEL );
			if( pUIMsg )
			{
				sprintf( szDisplay, "%s %d", pUIMsg, ppmCharacter->GetLevel( pcsCharacter ) );
			}
		}
	}

	return TRUE;
}


BOOL AgcmUILogin::CBDisplaySlotCharExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if( !pClass ) return FALSE;
	if( !pcsSourceControl ) return FALSE;
	if( !pcsSourceControl->m_pcsParentControl ) return FALSE;

	INT32 nControlNumber = 0;
	sscanf( pcsSourceControl->m_pcsParentControl->m_szName, "CTL_BTN_CHARINFO%d", &nControlNumber );
	if( nControlNumber <= 0 || nControlNumber > AGCMUILOGIN_MY_CHARACTER_NUM ) return FALSE;

	AgcmUILogin	*pThis = ( AgcmUILogin* )pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
	BOOL bFlag = pThis->m_bMyCharacterFlag[ nControlNumber - 1 ];
	CHAR* pstrCharName = pThis->m_szMyCharacterInfo[ nControlNumber - 1 ];

	//pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = 0xffb7dee8;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	AgpmFactors* ppmFactors = ( AgpmFactors* )pThis->GetModule( "AgpmFactors" );
	if( bFlag )
	{
		AgpdCharacter* pcsCharacter	= ppmCharacter->GetCharacter( pstrCharName );
		if( pcsCharacter )
		{
			INT64 llCurrentExp = ppmFactors->GetExp( &pcsCharacter->m_csFactor );
			INT64 llMaxExp = ppmFactors->GetMaxExp( &pcsCharacter->m_csFactor );

			if( llCurrentExp > llMaxExp )
			{
				llCurrentExp = llMaxExp;
			}

			CHAR* pUIMsg = pcmUIManager->GetUIMessage( UI_MESSAGE_ID_LOGIN_SLOT_CHAR_EXP );
			if( pUIMsg )
			{
				if( llCurrentExp > 0 && llMaxExp > 0 )
				{
					sprintf(szDisplay, "%s %.2f%%", pUIMsg, ( double )llCurrentExp / ( llMaxExp + 0.0 ) * 100 );
				}
				else
				{
					sprintf(szDisplay, "%s 0.00%%", pUIMsg );
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmUILogin::CBDisplaySlotCharGuild( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;
	if( !pcsSourceControl ) return FALSE;
	if( !pcsSourceControl->m_pcsParentControl ) return FALSE;

	INT32 nControlNumber = 0;
	sscanf( pcsSourceControl->m_pcsParentControl->m_szName, "CTL_BTN_CHARINFO%d", &nControlNumber );
	if( nControlNumber <= 0 || nControlNumber > AGCMUILOGIN_MY_CHARACTER_NUM ) return FALSE;

	AgcmUILogin	*pThis = ( AgcmUILogin* )pClass;
	BOOL bFlag = pThis->m_bMyCharacterFlag[ nControlNumber - 1 ];
	CHAR* pstrCharName = pThis->m_szMyCharacterInfo[ nControlNumber - 1 ];

	//pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = 0xffd7e4bd;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	if( bFlag )
	{
		AgpdCharacter* pcsCharacter	= ppmCharacter->GetCharacter( pstrCharName );
		AgpmGuild* pGuild = ( AgpmGuild* )pThis->GetModule( "AgpmGuild" );
		if( pGuild )
		{
			if( pGuild->IsAnyGuildMember( pcsCharacter ) )
			{
				// 길드 이름 찍고..
				CHAR* pGuildName = pGuild->GetJoinedGuildID( pcsCharacter );
				if( pGuildName )
				{
					sprintf( szDisplay, "[%s]", pGuildName );

					// 길드 아이콘 찍고..
					INT32 nGuildMarkID = pGuild->GetJointGuildMarkID( pGuildName );
				}
				else
				{
					// 길드없다고 표시해준다.
					CHAR* pStrNoGuild = ClientStr().GetStr( STI_STATICTEXT_NO_GUILD );
					if( pStrNoGuild && strlen( pStrNoGuild ) > 0 )
					{
						sprintf( szDisplay, "%s", pStrNoGuild );
					}
				}
			}
			else
			{
				// 길드없다고 표시해준다.
				CHAR* pStrNoGuild = ClientStr().GetStr( STI_STATICTEXT_NO_GUILD );
				if( pStrNoGuild && strlen( pStrNoGuild ) > 0 )
				{
					sprintf( szDisplay, "%s", pStrNoGuild );
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmUILogin::CBDisplaySlotCharVillian( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;
	if( !pcsSourceControl ) return FALSE;
	if( !pcsSourceControl->m_pcsParentControl ) return FALSE;

	INT32 nControlNumber = 0;
	sscanf( pcsSourceControl->m_pcsParentControl->m_szName, "CTL_BTN_CHARINFO%d", &nControlNumber );
	if( nControlNumber <= 0 || nControlNumber > AGCMUILOGIN_MY_CHARACTER_NUM ) return FALSE;

	AgcmUILogin	*pThis = ( AgcmUILogin* )pClass;
	BOOL bFlag = pThis->m_bMyCharacterFlag[ nControlNumber - 1 ];
	CHAR* pstrCharName = pThis->m_szMyCharacterInfo[ nControlNumber - 1 ];

	//pcsSourceControl->m_stDisplayMap.m_stFont.m_ulColor = 0xffe60fb8;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	if( bFlag )
	{
		AgpdCharacter* pcsCharacter	= ppmCharacter->GetCharacter( pstrCharName );
		if( pcsCharacter )
		{
			if( ppmCharacter )
			{
				INT32 nMurdererLevel = pThis->CalcMurdererLevelByPoint( ppmCharacter->GetMurdererLevel( pcsCharacter ) );
				INT32 nMurdererPoint = ppmCharacter->GetMurdererPoint( pcsCharacter );

				CHAR* pstrMurderer = ClientStr().GetStr( STI_VILLAIN );
				CHAR* pstrGrade = ClientStr().GetStr( STI_GRADE );
				CHAR* pstrPoint = ClientStr().GetStr( STI_UI_STRING_POINT );

				if( pstrMurderer && pstrGrade && pstrPoint )
				{
					sprintf( szDisplay, "%s%d%s %d%s", pstrMurderer, nMurdererLevel,
						pstrGrade, nMurdererPoint, pstrPoint );
				}
			}
		}
	}

	return TRUE;
}

BOOL AgcmUILogin::CBDisplayGameServerName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass )				return FALSE;
	AgcmUILogin* pThis	= (AgcmUILogin *)	pClass;
	AgcmWorld* pcmWorld = ( AgcmWorld* )pThis->GetModule( "AgcmWorld" );
	if( !pcmWorld ) return FALSE;

	AgpdWorld* pWorld = pcmWorld->m_pAgpdWorldSelected;
	if( !pWorld )				return FALSE;

	if( strlen( pWorld->m_szName ) > 0 )
		sprintf( szDisplay, "%s", pWorld->m_szName );

	return TRUE;
}


BOOL AgcmUILogin::CBDisplayOldCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;

	if (AP_INVALID_CID == pThis->m_lLoginMainSelectCID)
		return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	AgpdCharacter *pAgpdCharacter = ppmCharacter->GetCharacter(pThis->m_lLoginMainSelectCID);
	if (NULL == pAgpdCharacter)	return FALSE;

	sprintf(szDisplay, "%s", pAgpdCharacter->m_szID);
	return TRUE;
}

BOOL AgcmUILogin::CBDisplayCompenCaption(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	if (0 > pThis->m_lCharacterIndex || pThis->m_lCharacterIndex >= AGCMUILOGIN_MY_CHARACTER_NUM)
		return FALSE;

	CHAR *psz = NULL;
	if (0 == _tcscmp(pThis->m_szMyCharacterInfo[pThis->m_lCharacterIndex], pThis->m_AgcdLoginCompenMaster.m_szSelectedCharID))
		psz = pcmUIManager->GetUIMessage(UI_MESSAGE_ID_COMPEN_CAPTION_CANCEL);
	else
		psz = pcmUIManager->GetUIMessage(UI_MESSAGE_ID_COMPEN_CAPTION_PAY);

	if (psz)
		strcpy(szDisplay, psz);

	return TRUE;
}


BOOL AgcmUILogin::CBDisplayConnectRename(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUILogin	*pThis	= (AgcmUILogin *)	pClass;
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );

	CHAR *psz = pcmUIManager->GetUIMessage(UI_MESSAGE_ID_LOGIN_CONNECT);
	if (AP_INVALID_CID != pThis->m_lLoginMainSelectCID)
	{
		AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
		AgcmLogin* pcmLogin = ( AgcmLogin* )pThis->GetModule( "AgcmLogin" );
		AgpdCharacter *pAgpdCharacter = ppmCharacter->GetCharacter(pThis->m_lLoginMainSelectCID);
		if (NULL != pAgpdCharacter
			&& pcmLogin->m_pAgpmLogin->IsDuplicatedCharacterOfMigration(pAgpdCharacter->m_szID))
			psz = pcmUIManager->GetUIMessage(UI_MESSAGE_ID_LOGIN_RENAME);
	}

	sprintf(szDisplay, "%s", psz ? psz : _T(""));
	return TRUE;
}

BOOL AgcmUILogin::CBPing(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUILogin* pThis = static_cast<AgcmUILogin*>(pClass);
	stAgpdAdminPing* pstPing = static_cast<stAgpdAdminPing*>(pData);
	UINT32 ulNID = pCustData ? *static_cast<UINT32*>(pCustData) : 0;

	AgcEngine* pEngine = static_cast<AgcEngine*>( pThis->GetModuleManager() );
	if(!pEngine)
		return FALSE;

	AgpmAdmin* ppmAdmin = ( AgpmAdmin* )pThis->GetModule( "AgpmAdmin" );
	if( !ppmAdmin ) return FALSE;

	stConnectionList* pstConnection = pEngine->m_SocketManager.GetConnectionListPtr(ulNID);
	if(!pstConnection || pstConnection->Socket.GetState() == ACSOCKET_STATE_NULL)
		return FALSE;

	// 그냥 받은거 그대로 보내면 된다.
	INT16 nPacketLength = 0;
	PVOID pvPacket = ppmAdmin->MakePingPacket(&nPacketLength, pstPing, sizeof(stAgpdAdminPing));
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	ppmAdmin->m_csPacket.FreePacket(pvPacket);
	return pThis->SendPacket(pvPacket, nPacketLength, pstConnection->nServerType, PACKET_PRIORITY_NONE, ulNID);
}

void AgcmUILogin::PreLoadAllCharacterTypes( void )
{
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !ppmCharacter || !pcmCharacter ) return;

	// ㅡ.ㅡ.. 캐 하드코딩.. 나중에 이거 보는 사람은 꼭 고칩시다!!
	INT32 nTIDScion = 1722;
	INT32 nTIDSlayer = 1724;
	INT32 nTIDObiter = 1723;
	INT32 nTIDSummoner = 1732;

	AgpdCharacter* ppdCharacterScion = ppmCharacter->AddCharacter( 90000001, nTIDScion, "DummyScion" );
	AgpdCharacter* ppdCharacterSlayer = ppmCharacter->AddCharacter( 90000002, nTIDSlayer, "DummySlayer" );
	AgpdCharacter* ppdCharacterObiter = ppmCharacter->AddCharacter( 90000003, nTIDObiter, "DummyObiter" );
	AgpdCharacter* ppdCharacterSummoner = ppmCharacter->AddCharacter( 90000004, nTIDSummoner, "DummySummoner" );

	pcmCharacter->InitCharacter( ppdCharacterScion, FALSE );
	pcmCharacter->InitCharacter( ppdCharacterSlayer, FALSE );
	pcmCharacter->InitCharacter( ppdCharacterObiter, FALSE );
	pcmCharacter->InitCharacter( ppdCharacterSummoner, FALSE );

	//ppmCharacter->RemoveCharacter( 90000001, TRUE );
	//ppmCharacter->RemoveCharacter( 90000002, TRUE );
	//ppmCharacter->RemoveCharacter( 90000003, TRUE );
	//ppmCharacter->RemoveCharacter( 90000004, TRUE );
}

BOOL AgcmUILogin::ParsePatchCodeString( char* pString )
{
	// 아래의 파일이 존재하면 패치코드검사를 건너뛴다.

	if( AuIgnoringHelper::IgnoreToFile( "NotCheckPatchCode.arc" ) )
	{
		return TRUE;
	}

	if( !pString )
	{
		OutputDebugString( "Receive Invalid PatchCodeString\n" );
		return FALSE;
	}

	std::string patchStr = pString;

	size_t idx = patchStr.find( ":" );

	if( idx != std::string::npos )
	{
		patchStr.erase( 0, idx+1 );
		pString = const_cast<char*>( patchStr.c_str() );
	}

	int nPatchCodeLength = ( int )strlen( pString );
	if( nPatchCodeLength <= 0 || nPatchCodeLength >= 32 )
	{
#ifdef _DEBUG
		OutputDebugString( "Receive Invalid PatchCodeString\n" );
#endif
		return FALSE;
	}

	/*
	for( int nCount = 0 ; nCount < nPatchCodeLength ; nCount++ )
	{
		if( nCount != 0 && nCount % 2 == 0 )
		{
			char cTemp = pString[ nCount - 1 ];
			pString[ nCount - 1 ] = pString[ nCount ];
			pString[ nCount ] = cTemp;
		}
	}

	char strTimeLength[ 3 ] = { 0, };
	strTimeLength[ 0 ] = pString[ 0 ];
	strTimeLength[ 1 ] = pString[ 1 ];

	int nTimeLength = atoi( strTimeLength );
	char* pPatchCodePos = &pString[ nTimeLength + 2 ];
	__int64 nPatchCode = _atoi64( pPatchCodePos );
	DWORD dwPatchCode = ( DWORD )nPatchCode;
	*/

	DWORD dwPatchCode = ( DWORD )(_atoi64(pString));

	HKEY hKey = NULL;
	LONG nResult = NULL;
	switch( g_eServiceArea )
	{
	case AP_SERVICE_AREA_KOREA :	// 아크로드 이관하면 요건 분리되어야겠지..
		{
			LONG nResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, AgcmRegistryManager::RegPath(), 0, KEY_READ, &hKey );
			if( nResult != ERROR_SUCCESS ) return FALSE;
		}
		break;

	case AP_SERVICE_AREA_JAPAN :
	case AP_SERVICE_AREA_CHINA :
		{
			LONG nResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\ArchLord", 0, KEY_READ, &hKey );
			if( nResult != ERROR_SUCCESS ) return FALSE;
		}
		break;

	case AP_SERVICE_AREA_WESTERN :
		{
			LONG nResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Webzen\\ArchLord", 0, KEY_READ, &hKey );
			if( nResult != ERROR_SUCCESS ) return FALSE;
		}
		break;
	}

	DWORD dwValueType = 0;
	DWORD dwValue = 0;
	DWORD dwValueLength = sizeof( DWORD );

	nResult = ::RegQueryValueEx( hKey, "Code", 0, &dwValueType, ( LPBYTE )( &dwValue ), &dwValueLength );
	if( nResult != ERROR_SUCCESS ) return FALSE;

	RegCloseKey( hKey );
	
	BOOL bIsValidPatchCode = dwPatchCode == dwValue ? TRUE : FALSE;

#ifdef _DEBUG
	if( !bIsValidPatchCode )
	{
		char strDebug[ 256 ] = { 0, };
		sprintf_s( strDebug, sizeof( char ) * 256, "Invalid Patch Code : 0x%d ( VaildCode = 0x%d )\n", dwPatchCode, dwValue );
		OutputDebugString( strDebug );
	}
#endif

	return bIsValidPatchCode;
}

VOID	AgcmUILogin::ConnectLoginServer( VOID )
{
	AgcmLogin* pcmLogin = ( AgcmLogin* )GetModule( "AgcmLogin" );
	if( !pcmLogin )
		return;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	switch (pcmUIManager->GetUIMode())
	{
	case AGCDUI_MODE_1024_768:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_2D_OPEN]);
		break;
	case AGCDUI_MODE_1280_1024:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_2D_1280_OPEN]);
		break;
	case AGCDUI_MODE_1600_1200:
		pcmUIManager->ThrowEvent(m_lEvent[AGCMUILOGIN_EVENT_2D_1600_OPEN]);
		break;
	}

	m_LoginSelect.OpenLoginSelect();
}

BOOL	AgcmUILogin::ReConnectLoginServer( VOID )
{
	if( !m_LoginSelect.IsRetryLoginServer() )
		return FALSE;

	m_LoginSelect.ReConnectLoginServer();

	return TRUE;
}