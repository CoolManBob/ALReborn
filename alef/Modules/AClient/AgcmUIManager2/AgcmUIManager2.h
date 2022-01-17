// AgcmUIManager2.h: interface for the AgcmUIManager2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMUIMANAGER2_H__A3154F83_8751_4B6B_9D14_3174694FAABF__INCLUDED_)
#define AFX_AGCMUIMANAGER2_H__A3154F83_8751_4B6B_9D14_3174694FAABF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"
#include "AgcaUIManager2.h"
#include "AgcdUIManager2.h"
#include "AgcmSound.h"
#include "AgcmResourceLoader.h"
#include "AgcmUIControl.h"

#include "AgcmRender.h"

#include "AgcUIManager.h"
#include "AgcUIEffect.h"
#include "AcDefine.h"

#include "AgcmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIManager2D" )
#else
#pragma comment ( lib , "AgcmUIManager2" )
#endif
#endif

#define AGCMUIMANAGER2_MAX_UI_FUNCTIONS							2048
#define AGCDUIMANAGER2_MAX_UI_USER_DATA							2048
#define AGCDUIMANAGER2_MAX_UI_DISPLAYS							2048
#define AGCDUIMANAGER2_MAX_UI_EVENTS							2048
#define AGCDUIMANAGER2_MAX_UI_BOOLEANS							128
#define AGCDUIMANAGER2_MAX_UI_CUSTOM_CONTROL					32
#define AGCMUIMANAGER2_MAX_UI_WINDOWS							256

#define AGCMUIMANAGER2_MAX_REFRESH_USER_DATA					256

#define AGCMUIMANAGER2_MAX_CURSOR								20

#define AGCMUIMANAGER2_DEFAULT_RES_X							1024.0f
#define AGCMUIMANAGER2_DEFAULT_RES_Y							768.0f

#define	AGCMUIMANAGER2_VARIABLE_START							"{{"
#define	AGCMUIMANAGER2_VARIABLE_END								"}}"
#define	AGCMUIMANAGER2_VARIABLE_DELIMITER						"."

#define	AGCMUIMANAGER2_MAX_LINE_DELIMITER						8
#define	AGCMUIMANAGER2_MAX_UI_MESSAGE_DATA						384	// 독어가 엄청나게 길어서 256->384로 수정

#define AGCMUI_CHECK_RETURN( _ReturnValue, _Function )	\
	_ReturnValue = _Function;						\
	if( _ReturnValue < 0 ) return FALSE;


enum AgcmUIDataType
{
	AGCMUIMANAGER2_DATA_TYPE_UI,
	AGCMUIMANAGER2_DATA_TYPE_CONTROL,
};

enum // AgcmUIManager2 CB ID 
{	
	AGCMUIMANAGER2_CB_ID_OPEN_MAIN_UI							= 0,
	AGCMUIMANAGER2_CB_ID_CLOSE_ALL_UI,
	AGCMUIMANAGER2_CB_ID_SET_MAIN_WINDOW,
	AGCMUIMANAGER2_CB_ID_ACTION_MESSAGE_CHAT					// Chatting Window로 메세지를 찍어야 한다 
};


struct stAdminUIMsgEntry
{
	std::string	m_strIndex;
	std::string	m_strMsg;
};


class AgcmUICursor
{
public :
	INT32														m_lCursor;
	INT32														m_lCursorCurrent;
	ApModule*													m_pcsLockCursorModule;
	AgcdUICursor*												m_apstCursorInfo[ AGCMUIMANAGER2_MAX_CURSOR ];
	HCURSOR														m_hCursorHandle;
	BOOL														m_bShowCursor;

public :
	AgcmUICursor( void );
	~AgcmUICursor( void );

public :
	// Cursor 관련
	INT32						AddCursor						( HCURSOR hCursor , INT32 lSpeed );
	BOOL						AddCursorFrame					( INT32 lCursorID, HCURSOR hCursor , INT32 lFrameIndex );
	VOID						RemoveAllCursor					( void );

	VOID						UpdateCursor					( UINT32 nPrevTick, UINT32 nClockCount );

	AgcdUICursor *				GetCursor						( INT32 lCursorID );
	HCURSOR						GetCurrentCursorHandle			( void ) { return m_hCursorHandle; }

	BOOL						IsShowCursor					( void ) { return m_bShowCursor; }
	INT32						GetCurrentCursor				( void );

	BOOL						ChangeCursor					( INT32 lCursorID, ApModule *pcsChangeModule );
	VOID						ShowCursor						( BOOL bShow );

	BOOL						LockCursor						( ApModule *pcsLockModule );
	BOOL						UnLockCursor					( ApModule *pcsLockModule );
};




class AgcmUIManager2 : public AgcModule, public AgcmUICursor
{
private:
	typedef		std::map< std::string, stAdminUIMsgEntry* >					MapUIMessage;
	typedef		std::map< std::string, stAdminUIMsgEntry* >::iterator		MapUIMessageIter;

private:
	INT32														m_lMaxUIID;
	AgcWindow*													m_pcsMainWindow;

	INT32														m_lMaxUIWindows;
	AgcaUIManager2												m_csUIWindows;

	INT32														m_lMaxUIFunctions;
	INT32														m_lMaxUIFunctionID;
	ApAdmin														m_csUIFunctions;

	INT32														m_lMaxUIUserData;
	INT32														m_lMaxUIUserDataID;
	ApAdmin														m_csUIUserData;

	INT32														m_lMaxUIDisplays;
	INT32														m_lMaxUIDisplayID;
	ApAdmin														m_csUIDisplays;

	INT32														m_lMaxUIEvents;
	INT32														m_lMaxUIEventID;
	ApAdmin														m_csUIEvents;

	INT32														m_lMaxUIBooleans;
	INT32														m_lMaxUIBooleanID;
	ApAdmin														m_csUIBooleans;

	INT32														m_lMaxUICControls;
	INT32														m_lMaxUICControlID;
	ApAdmin														m_csUICControls;

	INT32														m_lRefreshUserData;
	AgcdUIUserData*												m_apstRefreshUserData[ AGCMUIMANAGER2_MAX_REFRESH_USER_DATA ];
	AgcdUIHotkey*												m_apstHotkeyTable[ AGCDUI_MAX_HOTKEY_TYPE ][ 256 ];

	UINT32														m_ulPrevTick;
	FLOAT														m_fRecipZ;			
	My2DVertex													m_vtBoxFan[ 4 ];

	RwCamera*													m_pstCamera;
	ApBase*														m_pcsTarget;

	CHAR														m_szSoundPath[ AGCDUIMANAGER2_MAX_NAME ];
	CHAR														m_szTexturePath[ AGCDUIMANAGER2_MAX_NAME ];

	static AgcdUIMode											m_eMode;
	MapUIMessage												m_mapAdminUIMsg;

	CHAR														m_szLineDelimiter[ AGCMUIMANAGER2_MAX_LINE_DELIMITER ];
	CHAR														m_szQuotationMark;
	CHAR														m_szComma;
	CHAR*														m_aszMessageData;

	AgcdUI*														m_pcsMouseOnUI;
	AgcdUI*														m_pcsMouseOnUIPrev;
	BOOL														m_bBeingDestroyed;

	AuList< AgcdUI* >											m_listUIProcessing;
	BOOL														m_bSmoothUI;

	// 화면내 공지 사항 관련
	AcUIToolTip													m_csNotification;
	UINT32														m_ulNotificationCloseTime;
	AgcdUI*														m_pcsUIDummy;

	AuList< AgcdUIActionBuffer* >								m_listActionBuffers;
	ApAdmin														m_csControlCopyHistory;

public:
	INT32														m_nEventToggleUIOpen;
	INT32														m_nEventToggleUIClose;
	INT32														m_nEventTabUIOpen;
	INT32														m_nEventEatSound;
	INT32														m_nEventClickSound;
	INT32														m_nEventWeaponEquipSound;
	INT32														m_nEventArmourEquipSound;
	INT32														m_nEventAccessorySound;
	INT32														m_nEventTextSound;
	INT32														m_nEventServerSelectMouseOverSound;
	INT32														m_nEventDeadSound;

	INT32														m_nEventCriticalSound;
	INT32														m_nEventMissSound;
	INT32														m_nEventBlockSound;

public:
	AgcmFont*													m_pcsAgcmFont;
	AgcmRender*													m_pcsAgcmRender;
	AgcmSound*													m_pcsAgcmSound;
	AgcmResourceLoader* 										m_pcsAgcmResourceLoader;
	AgcmUIControl*												m_pcsAgcmUIControl;
	AgcmCharacter*												m_pcsAgcmCharacter;

	static INT32												m_alControlMessages[ AcUIBase::TYPE_COUNT ];
	RwV2d														m_v2dCurMousePos;

	INT32														m_lWindowWidth;
	INT32														m_lWindowHeight;

	BOOL														m_bOnCameraSizing;			// 현재 Camera Sizing 중인가?
	BOOL														m_bUseAbsoluteOnly;			// 절대좌표체계만 쓰나?

	AgcUIManager												m_csManagerWindow;
	AgcUIEffect													m_csEffectWindow;
	AgcUIEffect													m_csEffectBGMWindow;		// 2005.03.13. steeple
	AcUIToolTip													m_csTooltip;

	AgcUIEffect													m_csEffectGuildBattleWindow1;		// 2005.04.19. steeple
	AgcUIEffect													m_csEffectGuildBattleWindow2;		// 2005.04.19. steeple
	AgcUIEffect													m_csEffectGuildBattleWindow3;		// 2005.04.19. steeple
	AgcUIEffect													m_csEffectGuildBattleWindow4;		// 2005.04.19. steeple
	AgcUIEffect													m_csEffectGuildBattleWindow5;		// 2005.04.19. steeple
	AgcUIEffect													m_csEffectGuildBattleWindow6;		// 2005.04.20. steeple
	AgcUIEffect													m_csEffectGuildBattleWindow7;		// 2005.04.20. steeple

	BOOL														m_bCameraControl;
	BOOL														m_bUIFocusProcessed;

	AgcdUI*														m_pcsUIOpenedPopup;
	std::string													m_strMouseOnUI;

public:
	AgcmUIManager2( void );
	virtual ~AgcmUIManager2( void );

	BOOL						OnAddModule						( void );
	BOOL						OnInit							( void );

	BOOL						OnIdle							( UINT32 ulClockCount );
	BOOL						OnDestroy						( void );
	VOID						OnCameraStateChange				( CAMERASTATECHANGETYPE	ctype );

	VOID						CameraStatusChange				( void );
	VOID						SetCamera						( RwCamera *pstCamera );

	INT16						AttachUIData					( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor );
	INT16						AttachControlData				( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor );

	VOID						SetMaxUI						( INT32 lMaxCount );
	VOID						SetMainWindow					( AgcWindow *pcsWindow );
	BOOL						SetMaxUIMessage					( INT32 lCount );

	VOID						ResizeManagerWindow				( INT32 x, INT32 y, INT32 w, INT32 h );
	VOID						SetMaxMaps						( INT32 lFunction, INT32 lUserData, INT32 lDisplay, INT32 lEvents, INT32 lBooleans = AGCDUIMANAGER2_MAX_UI_BOOLEANS, INT32 lCControls = AGCDUIMANAGER2_MAX_UI_CUSTOM_CONTROL );
	BOOL						MakeValidName					( CHAR *szName );

	// UI 관련
	AgcdUI*						CreateUI						( void );
	BOOL						DestroyUI						( AgcdUI *pcsUI );
	AgcdUI*						AddUI							( AgcdUI *pcsUI );
	AgcdUI*						GetUI							( CHAR *szName );
	AgcdUI*						GetUI							( INT32 lID );
	BOOL						RemoveUI						( AgcdUI *pcsUI );
	VOID						RemoveAllUI						( void );
	BOOL						ChangeUIName					( AgcdUI *pcsUI, CHAR *szName );
	AgcdUI *					GetSequenceUI					( INT32 *plIndex );

	BOOL						OpenUI							( AgcdUI *pcsUI, BOOL bGroup = FALSE, BOOL bEditMode = FALSE, BOOL bSmooth = TRUE );
	BOOL						CloseUI							( AgcdUI *pcsUI, BOOL bDeleteMemory = FALSE, BOOL bGroup = FALSE, BOOL bForce = FALSE, BOOL m_bSmooth = TRUE );
	BOOL						OpenMainUI						( void );
	BOOL						CloseAllUI						( void );					// 모든 UI를 다 닫는다 
	BOOL						CloseAllUIExceptMainUI			( BOOL *pbIsCloseWindow = NULL );		// Main UI를 빼고 나머지 UI들을 다 닫는다.
	BOOL						CloseAllEventUI					( void );											// 모든 Event UI를 다 닫는다.
	AgcdUI *					CopyUI							( AgcdUI *pcsUI, CHAR *szName, BOOL bTransControl = TRUE );

	VOID						SetUIData						( AgcdUI *pcsUI, INT32 lUIData );
	INT32						GetUIData						( AgcdUI *pcsUI );

	BOOL						AttachUI						( AgcdUI *pcsUI, AgcdUI *pcsUITarget, AgcdUIAttachType eType );
	BOOL						DetachUI						( AgcdUI *pcsUI );
	BOOL						AttachTooltip					( AgcdUI *pcsUI, AcUIToolTip *pcsTooltip, AgcdUIAttachType eType );
	BOOL						DetachTooltip					( AgcdUI *pcsUI );

	VOID						RefreshUI						( AgcdUI *pcsUI, BOOL bForce = FALSE );
	VOID						EnableSmoothUI					( BOOL bSmooth = TRUE ) { m_bSmoothUI = bSmooth; }

	// Control 관련
	AgcdUIControl*				AddControl						( AgcdUI *pcsUI, INT32 lType, INT32 lX, INT32 lY, AgcWindow *pcsWindow = NULL, AcUIBase *pcsBase = NULL, AgcdUICControl *pcsCustomControl = NULL );
	AgcdUIControl*				GetControl						( AgcdUI *pcsUI, AcUIBase *pcsUIBase );
	AgcdUIControl*				GetControl						( AgcdUI *pcsUI, CHAR *szName );
	AgcdUIControl*				GetControl						( AgcdUI *pcsUI, INT32 lID );
	AgcdUIControl*				GetControl						( AgcdUIControl *pcsParentControl, AcUIBase *pcsUIBase );
	AgcdUIControl*				GetControl						( AgcdUIControl *pcsParentControl, CHAR *szName );
	AgcdUIControl*				GetControl						( AgcdUIControl *pcsParentControl, INT32 lID );
	BOOL						RemoveControl					( AgcdUIControl *pcsControl, BOOL bTraverse = TRUE, BOOL bChildOnly = FALSE );
	BOOL						RemoveControl					( AgcdUI *pcsUI, AcUIBase *pcsUIBase, BOOL bTraverse = TRUE );

	VOID						CopyBaseProperty				( AcUIBase *pcsSource, AcUIBase *pscTarget );
	AgcdUIControl*				CopyControl						( AgcdUIControl *pcsControl, AgcdUI *pcsUI, AgcWindow *pcsParentWindow = NULL, BOOL bCopyChild = TRUE, INT32 lDataIndex = -1, AgcdUIControl *pcsDstControl = NULL, BOOL bDynamic = FALSE );
	AgcdUIControl*				GetSequenceControl				( AgcdUI *pcsUI, INT32 *plIndex );
	AgcdUIControl*				GetSequenceControl				( AgcdUIControl *pcsParentControl, INT32 *plIndex );

	VOID						RefreshControl					( AgcdUIControl *pcsControl, BOOL bForce = FALSE, BOOL bTraverse = TRUE, BOOL bUpdateList = TRUE, INT32 lUserDataIndex = -1 );
	VOID						SetControlAlpha					( AgcWindow *pcsWindow, FLOAT *pfAlpha );

	// Common API (Module에서 사용하는 가장 중요한 API들)
	AgcdUIFunction*				AddFunction						( PVOID pvClass, CHAR *szName, AgcUICallBack fnCallBack, INT32 lArgNum, CHAR *szArgDesc1 = NULL, CHAR *szArgDesc2 = NULL, CHAR *szArgDesc3 = NULL, CHAR *szArgDesc4 = NULL, CHAR *szArgDesc5 = NULL );
	AgcdUIUserData*				AddUserData						( CHAR *szName, PVOID pvData, INT32 lDataSize, INT32 lDataCount, AgcdUIDataType eDataType );
	AgcdUIDisplay*				AddDisplay						( PVOID pvClass, CHAR *szName, INT32 lID, AgcUIDisplayCB fnCallBack, UINT32 ulDataType );
	AgcdUIDisplay*				AddDisplay						( PVOID pvClass, CHAR *szName, INT32 lID, AgcUIDisplayOldCB fnCallBack, UINT32 ulDataType );
	AgcdUIBoolean*				AddBoolean						( PVOID pvClass, CHAR *szName, AgcUIBooleanCB fnCallBack, UINT32 ulDataType );
	INT32						AddEvent						( CHAR *szName, AgcUIEventReturnCB fnReturnCB = NULL, PVOID pClass = NULL );
	AgcdUICControl *			AddCustomControl				( CHAR *szName, AcUIBase *pcsBase );

	BOOL						SetUserDataRefresh				( AgcdUIUserData *pstUserData, BOOL bUpdateList = TRUE );

	// Action Message관련 
	BOOL						ParseMessage					( CHAR *szParsedMessage, CHAR *szMessage );
	INT32						ActionMessageOKDialog			( CHAR* szMessage );
	INT32						ActionMessageOKDialog2			( CHAR* szMessage );
	INT32						ActionMessageOKCancelDialog		( CHAR* szMessage );
	INT32						ActionMessageOKCancelDialog2	( CHAR* szMessage );
	INT32						ActionMessageEditOKDialog		( CHAR* szMessage, CHAR* pEditString );
	INT32						ActionMessageEditOKDialog2		( CHAR* szMessage, CHAR* pEditString );
#ifdef _AREA_GLOBAL_
	INT32						ActionMessageThreeBtnDialog( CHAR* szMessage ,CHAR* szBtn1Name ,CHAR* szBtn2Name ,CHAR* szBtn3Name);
#endif
	// 여기서 부터는 대부분 내부적으로 사용합니다.
	// Function 관련
	AgcdUIFunction *			CreateFunction					( void );
	BOOL						DestroyFunction					( AgcdUIFunction *pstFunction );
	INT32						AddFunction						( AgcdUIFunction *pstFunction );
	INT32						GetFunctionCount				( void ) { return m_lMaxUIFunctionID; }
	AgcdUIFunction *			GetFunction						( INT32 lIndex );
	AgcdUIFunction *			GetFunction						( CHAR *szName );
	VOID						RemoveAllFunction				( void );

	// User Data 관련
	AgcdUIUserData *			CreateUserData					( void );
	BOOL						DestroyUserData					( AgcdUIUserData *pstUserData );
	INT32						AddUserData						( AgcdUIUserData *pstUserData );
	INT32						GetUserDataCount				( void ) { return m_lMaxUIUserDataID; }
	AgcdUIUserData *			GetUserData						( INT32 lIndex );
	AgcdUIUserData *			GetUserData						( CHAR *szName );
	VOID						RemoveAllUserData				( void );
	AgcdUIUserData *			GetControlUserData				( AgcdUIControl *pcsControl );
	VOID						CountUserData					( void );
	VOID						CountUserData					( AgcdUIControl *pcsControl, BOOL bTraverse = TRUE, BOOL bUncount = FALSE );
	VOID						CountUserData					( AgcdUIUserData *pstUserData, AgcdUIControl *pcsControl, BOOL bUncount = FALSE );
	VOID						RefreshUserData					( AgcdUIUserData *pstUserData, BOOL bForce = FALSE );	// Internal Use

	// Boolean 관련
	AgcdUIBoolean *				CreateBoolean					( void );
	BOOL						DestroyBoolean					( AgcdUIBoolean *pstBoolean );
	INT32						AddBoolean						( AgcdUIBoolean *pstBoolean );
	INT32						GetBooleanCount					( void ) { return m_lMaxUIBooleanID; }
	AgcdUIBoolean *				GetBoolean						( INT32 lIndex );
	AgcdUIBoolean *				GetBoolean						( CHAR *szName );
	VOID						RemoveAllBoolean				( void );

	// Display 관련
	AgcdUIDisplay *				CreateDisplay					( void );
	BOOL						DestroyDisplay					( AgcdUIDisplay *pstDisplay );
	INT32						AddDisplay						( AgcdUIDisplay *pstDisplay );
	INT32						GetDisplayCount					( void ) { return m_lMaxUIDisplayID; }
	AgcdUIDisplay *				GetDisplay						( INT32 lIndex );
	AgcdUIDisplay *				GetDisplay						( CHAR *szName );
	VOID						RemoveAllDisplay				( void );
	BOOL						EnumDisplayCallback				( AgcdUIDisplay *pstDisplay, AgcdUIUserData *pstUserData, CHAR *szDisplay, INT32 *plValue, INT32 lIndex = 0, AgcdUIControl *pcsSourceControl = NULL );
	BOOL						ResetControlDisplayMap			( AgcdUIControl *pcsControl );
	BOOL						SetControlDisplayMap			( AgcdUIControl *pcsControl, CHAR *szDisplay );
	BOOL						SetControlDisplayMap			( AgcdUIControl *pcsControl, AgcdUIUserData *pstUserData, AgcdUIDisplay *pstDisplay, BOOL bControlUserData );
	BOOL						SetControlDisplayFont			( AgcdUIControl *pcsControl, INT32 lType, UINT32 ulColor, FLOAT fScale, bool bShadow, AcUIBaseHAlign eHAlign, AcUIBaseVAlign eVAlign, BOOL bImageNumber );

	// Event 관련
	INT32						GetEventCount					( void ) { return m_lMaxUIEventID; }
	AgcdUIEventMap*				GetEventMap						( INT32 lIndex );
	AgcdUIEventMap*				GetEventMap						( CHAR *szName );
	BOOL						ThrowEvent						( INT32 lEventID, INT32 lDataIndex = 0, BOOL bForce = FALSE, ApBase *pcsOwnerBase = NULL );
	VOID						RemoveAllEvent					( void );
	VOID						RemoveAllEventActions			( void );

	// Custom Control 관련
	INT32						GetCControlCount				( void ) { return m_lMaxUICControlID; }
	AgcdUICControl*				GetCControl						( INT32 lIndex );
	AgcdUICControl*				GetCControl						( CHAR *szName );
	VOID						RemoveAllCControl				( void );

	// Hotkey 관련
	AgcdUIHotkey*				AddHotkey						( AgcdUIHotkeyType eType, INT32 lKeyCode );
	AgcdUIHotkey*				GetHotkey						( AgcdUIHotkeyType eType, INT32 lKeyCode );
	VOID						RemoveAllHotkey					( void );
	VOID						RemoveAllHotkeyActions			( void );
	VOID						SetHotkeyDescription			( AgcdUIHotkey *pcsHotkey, CHAR *szDescription );
	VOID						RemoveHotkey					( AgcdUIHotkeyType eType, INT32 lKeyCode );
	VOID						RemoveHotkey					( AgcdUIHotkey* pHotkey );
	BOOL						CopyHotkey						( AgcdUIHotkey* pDest, AgcdUIHotkey* pSrc );

	// Control Data 관련
	BOOL						ResetControlGrid				( AgcdUIControl *pcsControl );
	BOOL						SetControlGrid					( AgcdUIControl *pcsControl, AgcdUIUserData *pstUserData, BOOL bControlUserData = FALSE );
	AgpdGrid *					GetControlGrid					( AgcdUIControl *pcsControl );

	// Action 관련
	BOOL						ProcessAction					( AuList< AgcdUIAction >* plistActions, AgcdUIControl* pcsControl = NULL, AgcdUIEventMap *pcsEvent = NULL, INT32 lDataIndex = 0, BOOL bForce = FALSE, ApBase *pcsOwnerBase = NULL );
	VOID						SetupActions					( void );
	VOID						SetupActions					( AgcdUIControl *pcsControl, BOOL bTraverse = TRUE );
	BOOL						SetupAction						( AuList <AgcdUIAction> *plistActions );
	VOID						RemoveFromActions				( AgcdUI *pcsUI, AgcdUIControl *pcsControl );
	VOID						RemoveFromActions				( AgcdUI *pcsUI, AgcdUIControl *pcsControl, AgcdUIControl *pcsSourceControl, BOOL bTraverse = TRUE );
	BOOL						RemoveFromAction				( AgcdUI *pcsUI, AgcdUIControl *pcsControl, AuList <AgcdUIAction> *plistActions );

	// Target 관련
	VOID						SetTarget						( ApBase *pcsTarget ) { m_pcsTarget = pcsTarget; }
	ApBase*						GetTarget						( void ) { return m_pcsTarget; }

	// Stream 관련
	BOOL						StreamWriteUI					( AgcdUI* pUI, CHAR* szFile, BOOL bEncryption );
	BOOL						StreamWrite						( CHAR *szFile, BOOL bEncryption );
	BOOL						StreamRead						( CHAR *szFile, BOOL bDecryption );
	BOOL						StreamWriteControl				( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteBase					( AcUIBase *pcsBase, AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteAction				( AuList< AgcdUIAction > *plistActions, ApModuleStream *pStream );
	BOOL						StreamReadAction				( AuList< AgcdUIAction > *plistActions, ApModuleStream *pStream );
	BOOL						StreamWriteButton				( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteGrid					( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteBar					( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteList					( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteTree					( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteSkillTree			( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteEdit					( AgcdUIControl *pcsControl, ApModuleStream *pStream );	
	BOOL						StreamWriteScroll				( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteCombo				( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamWriteClock				( AgcdUIControl *pcsControl, ApModuleStream *pStream );
	BOOL						StreamReadUIMessage				( CHAR *szFile, BOOL bEncryption );

	static BOOL					StreamWriteCB					( PVOID pData, ApModule *pClass, ApModuleStream *pStream );
	static BOOL					StreamReadCB					( PVOID pData, ApModule *pClass, ApModuleStream *pStream );

	static BOOL					CBListItemConstructor			( AcUIList* pUIList, AcUIListItem* pUIListItem, PVOID pClass, PVOID pData, INT32 *plHeight );
	static BOOL					CBListItemDestructor			( AcUIList* pUIList, AcUIListItem* pUIListItem, PVOID pClass, PVOID pData, INT32 *plHeight );
	static BOOL					CBListItemRefresh				( AcUIList* pUIList, AcUIListItem* pUIListItem, PVOID pClass, PVOID pData, INT32 *plHeight );
	static BOOL					CB_POST_RENDER					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBIsCameraMoving				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBUpdateActionState				( PVOID pData, PVOID pClass, PVOID pCustData );

	BOOL						AddWindow						( AgcWindow* pWindow );
	BOOL						RemoveWindow					( AgcWindow* pWindow );

	BOOL						SetCallbackOpenMainUI			( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL						SetCallbackSetMainWindow		( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL						SetCallbackActionMessageChat	( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL						SetCallbackCloseAllUI			( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	BOOL						IsMainWindow					( AgcWindow *pWindow );
	void						SetLoginMode					( BOOL bLoginMode ) { m_csManagerWindow.SetLoginMode( bLoginMode );	}	// Login Mode 인지 Game Mode인지 Setting

	// UI Manager Window관련 
	BOOL						GetCameraControl				( void ) { return m_bCameraControl; }

	// Set Cursor Mouse Position Callback Function : g_pEngine 에 Set Callback
	static BOOL					CBSetCursorMousePosition		( PVOID pData, PVOID pClass, PVOID pCustData );

	// Sort PopupMenus
	VOID						SortPopupUI						( AgcdUI *pcsUI );

	// Set Sound File Path
	VOID						SetSoundPath					( CHAR *szPath );
	// Set Texture Path
	VOID						SetTexturePath					( CHAR *szPath );

	// Set/Get UI Mode
	VOID						SetUIMode						( AgcdUIMode eMode );
	static AgcdUIMode			GetUIMode						( void ) { return m_eMode; }
	static float				GetModeWidth					();
	static float				GetModeHeight					();

	CHAR*						GetLineDelimiter				( void );
	CHAR*						GetUIMessage					( CHAR *szMessageID );
	BOOL						GetSeparateLine					( CHAR *szMessageID, INT32 lLineIndex, CHAR *szBuffer, INT32 lBufferSize );
	VOID						SetMouseOnUI					( AgcdUI *pcsUI );
	AgcdUI*						GetMouseOnUI()					{ return m_pcsMouseOnUI; }

	VOID						CloseTooltip					( INT32 x, INT32 y, INT32 w, INT32 h, AgcUIWindow* pcsParentUIWindow = NULL );
	VOID						OpenTooltip						( CHAR *szTooltip, AgcWindow *pcsWindow = NULL, AgcUIWindow* pcsParentUIWindow = NULL );
	VOID						CloseTooltip					( void );
	VOID						SetControlTooltip				( AgcdUIControl *pcsControl, CHAR *szTooltip );

	// Notification
	BOOL						Notice							( CHAR *szMessage, DWORD dwColor = 0xffffffff, CHAR *szSubMessage = NULL, DWORD dwSubColor = 0xffffffff, INT32 lDuration = -1, FLOAT fRateY = 0.15f );		// Duration 이 -1 이면, szMessage의 길이에 비례
	inline	AgcdUIHotkeyType	GetKeyStatus					( void ) { return m_csManagerWindow.GetKeyStatus();	}

	// UIConsole functions
	VOID						UIClose							( std::string strUIName );
	VOID						ShowUIName						( void );
	void						SetUIMgrProcessHotkeys			( void );

	// 모달윈도우가 떠 있는가?
	BOOL						IsOpenModalWindow				( void );
	AgcWindow*					GetMainWindow					( void ) { return m_pcsMainWindow; }
};

#endif // !defined(AFX_AGCMUIMANAGER2_H__A3154F83_8751_4B6B_9D14_3174694FAABF__INCLUDED_)
