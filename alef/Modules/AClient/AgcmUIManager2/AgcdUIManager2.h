#ifndef	_AGCDUIMANAGER2_H_
#define _AGCDUIMANAGER2_H_

#include "AuList.h"
#include "ApBase.h"
#include "AgcmUIControl.h"
#include "AgcUIWindow.h"

#define	AGCDUIMANAGER2_MAX_ACTION_MESSAGE					200
#define AGCDUIMANAGER2_MAX_NAME								64
#define AGCDUIMANAGER2_MAX_CONTROL							32
#define AGCDUIMANAGER2_MAX_ITEM_CONTROL						5
#define AGCDUIMANAGER2_MAX_DISPLAY_LENGTH					128
#define AGCDUIMANAGER2_MAX_ARGS								5
#define AGCDUIMANAGER2_MAX_STATUS							3
#define AGCDUIMANAGER2_MAX_CURSOR_FRAME						10

// UI Mode
typedef enum
{
	AGCDUI_MODE_1024_768									= 0,
	AGCDUI_MODE_1280_1024,
	AGCDUI_MODE_1600_1200,
	AGCDUI_MAX_MODE
} AgcdUIMode;

typedef enum
{
	AGCDUI_USERDATA_TYPE_NONE								= 0x00000000,
	AGCDUI_USERDATA_TYPE_INT32								= 0x00000001,
	AGCDUI_USERDATA_TYPE_FLOAT								= 0x00000002,
	AGCDUI_USERDATA_TYPE_CHARACTER							= 0x00000004,
	AGCDUI_USERDATA_TYPE_ITEM								= 0x00000008,
	AGCDUI_USERDATA_TYPE_SKILL								= 0x00000010,
	AGCDUI_USERDATA_TYPE_GRID								= 0x00000020,
	AGCDUI_USERDATA_TYPE_STRING								= 0x00000040,
	AGCDUI_USERDATA_TYPE_TELEPORT_POINT						= 0x00000080,
	AGCDUI_USERDATA_TYPE_UINT64								= 0x00000100,
	AGCDUI_USERDATA_TYPE_BOOL								= 0x00000200,
	AGCDUI_USERDATA_TYPE_POS								= 0x00000400,
	AGCDUI_USERDATA_TYPE_ETC								= 0x80000000,
	AGCDUI_USERDATA_TYPE_ALL								= 0xffffffff,
} AgcdUIDataType;

class AgcdUI;
class AgcdUIControl;

typedef BOOL ( *AgcUIEventReturnCB )	( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
typedef BOOL ( *AgcUICallBack )			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
typedef BOOL ( *AgcUIDisplayCB )		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
typedef BOOL ( *AgcUIDisplayOldCB )		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
typedef BOOL ( *AgcUIBooleanCB )		( PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourcecontrol );

class AuUITString
{
public:
	CHAR													m_szString[ AGCDUIMANAGER2_MAX_NAME ];

	AuUITString& operator=( const CHAR * szString )
	{
		INT32 lIndex;

		for( lIndex = 0 ; lIndex < AGCDUIMANAGER2_MAX_NAME - 1 ; ++lIndex )
		{
			if( szString[ lIndex ] == 0 ) break;
			if( szString[ lIndex ] == ' ' || szString[ lIndex ] == '\t' || szString[ lIndex ] == '\r' || szString[ lIndex ] == '\n' )
			{
				m_szString[ lIndex ] = '_';
			}
			else
			{
				m_szString[ lIndex ] = szString[ lIndex ];
			}
		}

		m_szString[ lIndex ] = 0;
		return *this;
	}

	operator LPCTSTR() const
	{
		return ( LPCTSTR )m_szString;
	}

	operator LPSTR() const
	{
		return ( LPSTR )m_szString;
	}

	CHAR operator[]( INT32 lIndex ) const
	{
		return m_szString[ lIndex ];
	}
};

// UI Function ===================================================================

typedef struct AgcdUIFunction								// UI Function (Client Module들이 등록하는 Format)
{
	AuUITString												m_szName;								// 이름
	AgcUICallBack											m_fnCallback;							// Callback Function
	INT32													m_lNumData;								// 필요한 Argument 개수
	CHAR *													m_aszData[ AGCDUIMANAGER2_MAX_ARGS ];	// 각 Argument에 대한 설명
	PVOID													m_pClass;								// Callback시 필요한 Class Pointer
} AgcdUIFunction;

// UI User Data =====================================================================

typedef struct AgcdUIUserData					// 사용자 Data (Client Module이 등록하는 Format)
{
	AuUITString												m_szName;			// 이름
	AgcdUIDataType											m_eType;			// User Data Type
	AcUIBaseData											m_stUserData;		// User Data
	INT32													m_lSelectedIndex;	// Selected User Data (If Multi-UserData)
	INT32													m_lStartIndex;		// List Start Index (-1 if not concerned)
	INT32													m_lControl;			// 이 User Data가 세팅된 Control 개수
	AgcdUIControl **										m_ppControls;		// 이 User Data가 세팅된 Control들
	BOOL													m_bNeedRefresh;		// 이 User Data를 Refresh 해야되나
	BOOL													m_bUpdateList;		// 이 User Data를 Refresh할때 List를 Update할 것인가

	AgcdUIUserData( void )
	{
		m_eType = AGCDUI_USERDATA_TYPE_NONE;
		memset( &m_stUserData, 0, sizeof( m_stUserData ) );
		m_lSelectedIndex = -1;
		m_lStartIndex = -1;
		m_lControl = 0;
		m_ppControls = NULL;
		m_bNeedRefresh = FALSE;
		m_bUpdateList = FALSE;
	}
} AgcdUIUserData;

// UI Boolean Callback ============================================================

typedef struct AgcdUIBoolean					// UI Boolean Callback (Client Module이 등록하는 Format)
{
	AuUITString												m_szName;		// 이름
	AgcUIBooleanCB											m_fnCallback;	// Callback Function
	PVOID													m_pvClass;		// Callback시 필요한 Class Pointer
	UINT32													m_ulDataType;	// Acceptable User Data Type

	AgcdUIBoolean( void )
	{
		m_fnCallback = NULL;
		m_pvClass = NULL;
		m_ulDataType = 0;
	}
} AgcdUIBoolean;

// UI Display =====================================================================

typedef struct AgcdUIDisplay
{
	AuUITString												m_szName;			// 화면에 표시할 Data의 내용 (Client Module이 등록하는 Format)
	INT32													m_lID;				// CB Function에서 받을 ID (Ex. HP, MP, SP 등)
	AgcUIDisplayCB											m_fnCallback;		// Callback Function
	AgcUIDisplayOldCB										m_fnOldCallback;	// Callback Function
	PVOID													m_pvClass;			// Callback시 필요한 Class Pointer
	UINT32													m_ulDataType;		// Acceptable User Data Type

	AgcdUIDisplay( void )
	{
		m_lID = 0;
		m_fnCallback = NULL;
		m_fnOldCallback = NULL;
		m_pvClass = NULL;
		m_ulDataType = 0;
	}
} AgcdUIDisplay;

// UI Custom Control

typedef struct AgcdUICControl
{
	AuUITString												m_szName;		// 이름
	AcUIBase*												m_pcsBase;

	AgcdUICControl( void )
	{
		m_pcsBase = NULL;
	}
} AgcdUICControl;

// UI Action ======================================================================

typedef enum								// UI상에서의 Action 종류
{
	AGCDUI_ACTION_NONE										= 0,
	AGCDUI_ACTION_UI,										// UI에 관한 Action (Open, Close 등)
	AGCDUI_ACTION_CONTROL,									// Control에 관한 Action (Status Change 등)
	AGCDUI_ACTION_FUNCTION,									// Callback Function Call
	AGCDUI_ACTION_MESSAGE,									// Message 관련 Action ( 각종 MessageDialg나 Chatting창 Message )
	AGCDUI_ACTION_SOUND,
	AGCDUI_MAX_ACTION,
} AgcdUIActionType;

// UI ActionUI ------------------------------------------------------

typedef enum								// UI관한 Action의 종류
{
	AGCDUI_ACTION_UI_NONE	= 0,
	AGCDUI_ACTION_UI_OPEN,					// Open UI
	AGCDUI_ACTION_UI_CLOSE,					// Close UI
	AGCDUI_ACTION_UI_TOGGLE,				// Toggle UI
	AGCDUI_ACTION_UI_MOVE,					// Move UI
	AGCDUI_ACTION_UI_CLOSE_EVENTUI,			// Close Event UIs
	AGCDUI_ACTION_UI_ATTACH_LEFT,			// Attach UI To Parent UI
	AGCDUI_ACTION_UI_ATTACH_RIGHT,			// Attach UI To Parent UI
	AGCDUI_ACTION_UI_ATTACH_TOP,			// Attach UI To Parent UI
	AGCDUI_ACTION_UI_ATTACH_BOTTOM,			// Attach UI To Parent UI
	AGCDUI_ACTION_UI_DETACH,
	AGCDUI_MAX_ACTION_UI,
} AgcdUIActionUIType;

typedef struct AgcdUIActionUI				// UI Action
{
	AgcdUIActionUIType										m_eType;								// UI Action Type
	AgcdUI*													m_pcsUI;								// Target UI
	CHAR													m_szUIName[ AGCDUIMANAGER2_MAX_NAME ];	// Target UI Name (if m_pcsUI is NULL, search UI with this)
	BOOL													m_bGroupAction;							// Group Action(Open/Close) ?
	BOOL													m_bSmooth;								// Smooth Action
	INT32													m_lPosX;								// UI position X
	INT32													m_lPosY;								// UI position Y
	AgcdUI*													m_pcsAttachUI;
	CHAR													m_szAttachUIName[ AGCDUIMANAGER2_MAX_NAME ];
} AgcdUIActionUI;

// UI ActionControl -------------------------------------------------

typedef struct AgcdUIActionStatus
{
	INT32													m_lCurrentStatus;	// Current Status (-1 : All Status)
	INT32													m_lTargetStatus;	// Target Status
} AgcdUIActionStatus;

typedef struct AgcdUIActionControl			// Control Action
{
	AgcdUI*													m_pcsUI;										// Target UI
	CHAR													m_szUIName[ AGCDUIMANAGER2_MAX_NAME ];			// Target UI Name (if m_pcsUI is NULL, search UI with this)
	AgcdUIControl*											m_pcsControl;									// Target Control
	CHAR													m_szControlName[ AGCDUIMANAGER2_MAX_NAME ];		// Target Control Name (if m_pcsControl is NULL, search Control with this)
	BOOL													m_bSaveCurrent;									// Save Current Status
	INT32													m_lStatusNum;									// Control Status Number
	AgcdUIActionStatus										m_astStatus[AGCDUIMANAGER2_MAX_STATUS];			// Control Status
} AgcdUIActionControl;

// UI ActionFunction ------------------------------------------------

typedef enum								// 실제 Call할 때 Argument Type
{
	AGCDUI_ARG_TYPE_NONE									= 0,
	AGCDUI_ARG_TYPE_CONTROL,				// Control의 값을 넘겨준다.
	AGCDUI_ARG_TYPE_UI,
	AGCDUI_ARG_MAX_TYPE
} AgcdUIArgType;

typedef struct AgcdUIArg					// 실제로 넘겨줄 Argument
{
	AgcdUIArgType											m_eType;									// Argument Type
	AgcdUI*													m_pcsUI;									// Target UI
	CHAR													m_szUIName[ AGCDUIMANAGER2_MAX_NAME ];		// Target UI Name (if m_pcsUI is NULL, search UI with this)
	AgcdUIControl*											m_pstControl;								// Control Pointer
	CHAR													m_szControlName[ AGCDUIMANAGER2_MAX_NAME ];	// Target Control Name (if m_pcsControl is NULL, search Control with this)
} AgcdUIArg;

typedef struct AgcdUIActionFunction			// Function Action
{
	AgcdUIFunction*											m_pstFunction;							// Target Function
	AgcdUIArg												m_astArgs[ AGCDUIMANAGER2_MAX_ARGS ];	// Argument List
} AgcdUIActionFunction;

// UI ActionMessage -------------------------------------------------

typedef enum								// Message 관한 Action의 종류
{
	AGCDUI_ACTION_MESSAGE_OK_DIALOG							= 0,
	AGCDUI_ACTION_MESSAGE_OKCANCEL_DIALOG,
	AGCDUI_ACTION_MESSAGE_INPUT_DIALOG,
	AGCDUI_ACTION_MESSAGE_CHATTING_WINDOW,
	AGCDUI_MAX_ACTION_MESSAGE

} AgcdUIActionMessageType;

typedef struct AgcdUIActionMessage
{
	AgcdUIActionMessageType									m_eMessageType;										// Message Action Type
	DWORD													m_lColor;											// Messaeg Color - Chatting 창에 출력할때 쓰임
	CHAR													m_szMessage[ AGCDUIMANAGER2_MAX_ACTION_MESSAGE ];	// Message String 
} AgcdUIActionMessage;

// UI ActionSound ---------------------------------------------------

typedef struct AgcdUIActionSound
{
	CHAR													m_szSound[ AGCDUIMANAGER2_MAX_NAME ];	// Sound File
} AgcdUIActionSound;

// UI Action --------------------------------------------------------

typedef struct AgcdUIAction	AgcdUIAction;
struct AgcdUIAction									// Action
{
	AgcdUIActionType										m_eType;				// Action Type (UI, Control, Function)
	union
	{
		AgcdUIActionUI										m_stUI;
		AgcdUIActionControl									m_stControl;
		AgcdUIActionFunction								m_stFunction;
		AgcdUIActionMessage									m_stMessage;
		AgcdUIActionSound									m_stSound;
	}														m_uoAction;

	bool operator==( AgcdUIAction stAction )
	{
		if( memcmp( ( PVOID ) this, ( PVOID )&stAction, sizeof( this ) ) ) return false;
		return true;
	}
} ;

// UI MessageMap ===============================================================================

typedef struct AgcdUIMessageMap								// 해당 Command Message에 대한 MessageMap
{
	AuList< AgcdUIAction >									m_listAction;			// Action List
} AgcdUIMessageMap;

// UI DisplayMap ===============================================================================

typedef enum												// Display Type
{
	AGCDUI_DISPLAYMAP_TYPE_NONE								= 0,
	AGCDUI_DISPLAYMAP_TYPE_STATIC,							// Static String
	AGCDUI_DISPLAYMAP_TYPE_VARIABLE,						// 변수를 표시
	AGCDUI_MAX_DISPLAYMAP
} AgcdUIDisplayMapType;

typedef struct AgcdUIFont
{
	INT32													m_lType;
	UINT32													m_ulColor;
	FLOAT													m_fScale;
	bool													m_bShadow;
	AcUIBaseHAlign											m_eHAlign;
	AcUIBaseVAlign											m_eVAlign;
	BOOL													m_bImageNumber;
} AgcdUIFont;

typedef struct AgcdUIDisplayMap								// Display (Static, Variable)
{
	AgcdUIDisplayMapType									m_eType;			// Display Type
	union													// Display Data
	{
		CHAR*												m_szStatic;
		struct
		{
			BOOL											m_bControlUserData;
			AgcdUIUserData*									m_pstUserData;
			AgcdUIDisplay*									m_pstDisplay;
		} m_stVariable;
	}														m_uoDisplay;

	AgcdUIFont												m_stFont;		// Font Type
} AgcdUIDisplayMap;

// UI Event =====================================================================================

typedef struct AgcdUIEventMap								// Module에서 발생하는 Event에 대한 Action을 정의
{
	AuUITString												m_szName;		// Event Name
	AuList< AgcdUIAction >									m_listAction;	// Action List
	AgcUIEventReturnCB										m_fnReturnCB;	// Event Map의 Return Callback
	PVOID													m_pCBClass;		// CB을 갖고 있는 Class의 Pointer

	AgcdUIEventMap( void )
	{
		m_fnReturnCB = NULL;
		m_pCBClass = NULL;
	}
} AgcdUIEventMap;

// UI Hotkey ====================================================================================
typedef enum
{
	AGCDUI_HOTKEY_MOUSE_LCLICK								= 248,
	AGCDUI_HOTKEY_MOUSE_RCLICK,
	AGCDUI_HOTKEY_MOUSE_MCLICK,
	AGCDUI_HOTKEY_MAX_MOUSE
} AgcdUIHotkeyMouseType;

typedef struct AgcdUIHotkey
{
	AgcdUIHotkeyType										m_eType;
	UINT8													m_cKeyCode;
	BOOL													m_bCustomizable;
	AuList< AgcdUIAction >									m_listAction;		// Action List
	CHAR*													m_szDescription;

	AgcdUIHotkey( void )
	{
		m_eType = AGCDUI_HOTKEY_TYPE_NORMAL;
		m_cKeyCode = 0;
		m_bCustomizable = FALSE;
		m_szDescription = NULL;
	}

	//@{ 2006/08/18 burumal
	~AgcdUIHotkey( void )
	{
		if( m_listAction.GetCount() > 0 )
		{
			m_listAction.RemoveAll();
		}
	}
	//@}
} AgcdUIHotkey;

// UI Control ===================================================================================

typedef enum
{
	AGCDUI_BUTTON_TYPE_NORMAL								= 0,
	AGCDUI_BUTTON_TYPE_SCROLL_DOWN,
	AGCDUI_BUTTON_TYPE_SCROLL_UP,
	AGCDUI_BUTTON_TYPE_TREE_CLOSE,
	AGCDUI_BUTTON_TYPE_TREE_OPEN,
	AGCDUI_BUTTON_MAX_TYPE
} AgcdUIButtonType;

// UI Control Data -----------------------------------------------------
typedef union
{
	struct
	{
		AgcdUIUserData*										m_pstGrid;
		BOOL												m_bControlUserData;
	}														m_stGrid;

	struct
	{
		AgcdUIUserData*										m_pstCurrent;
		AgcdUIDisplay*										m_pstDisplayCurrent;
		AgcdUIUserData*										m_pstMax;
		AgcdUIDisplay*										m_pstDisplayMax;
	}														m_stBar;

	struct
	{
		AgcdUIControl*										m_pcsListItemFormat;
	}														m_stList;

	struct
	{
		AgcdUIBoolean*										m_pstCheckBoolean;
		AgcdUIUserData*										m_pstCheckUserData;
		AgcdUIBoolean*										m_pstEnableBoolean;
		AgcdUIUserData*										m_pstEnableUserData;
		AgcdUIButtonType									m_eButtonType;
	}														m_stButton;

	struct
	{
		AgcdUIUserData*										m_pstCurrent;
		AgcdUIDisplay*										m_pstDisplayCurrent;
		AgcdUIUserData*										m_pstMax;
		AgcdUIDisplay*										m_pstDisplayMax;
	}														m_stScroll;

	struct
	{
		AgcdUIControl*										m_pcsListItemFormat;
		AgcdUIUserData*										m_pstDepthUserData;
	}														m_stTree;

	struct
	{
		AgcdUIUserData*										m_pstCurrent;
		AgcdUIDisplay*										m_pstDisplayCurrent;
		AgcdUIUserData*										m_pstMax;
		AgcdUIDisplay*										m_pstDisplayMax;
	}														m_stClock;
	struct
	{
		AgcdUICControl*										m_pstCustomControl;
	}														m_stCustom;
} AgcdUIControlData;

// UI Control ----------------------------------------------------------
class AgcdUIControl	: public ApBase				// UI Control
{
public:
	CHAR													m_szName[ AGCDUIMANAGER2_MAX_NAME ];	// Name
	INT32													m_lType;				// Control Type (Button, Edit, ...)
	AcUIBase*												m_pcsBase;				// Control Base Pointer
	BOOL													m_bCustomBase;			// Control Base is not allocated dynamically

	INT32													m_lMessageCount;
	AgcdUIMessageMap*										m_pstMessageMaps;		// MessageMaps
	AgcdUIDisplayMap										m_stDisplayMap;			// Display Map

	INT32													m_lUserDataIndex;		// User Data Index
	AgcdUIUserData*											m_pstUserData;			// User Data
	BOOL													m_bUseParentUserData;	// Use Parent UserData?

	AgcdUIControlData										m_uoData;				// Control Data
	AgcdUI*													m_pcsParentUI;			// Parent UI
	AgcdUIControl*											m_pcsParentControl;		// Parent Control
	AgcdUIControl*											m_pcsFormatControl;		// List에서 Item 일 경우 Format이 되는 Control

	AuList< AgcdUIControl* >								m_listChild;			// Child Controls
	AgcdUIUserData*											m_apstUserData[ ACUIBASE_STATUS_MAX_NUM ];	// Status UserData

	BOOL													m_bNeedRefresh;			// 이 Control을 Refresh 해야되나
	BOOL													m_bDynamic;				// UIManager가 필요에 의해서 만든것인가? TRUE이면, Streaming하지 않는다.
	BOOL													m_bAutoAlign;			// 차례로 정렬하여 보여준다.

	AgcdUIBoolean*											m_pstShowCB;			// Boolean Callback
	AgcdUIUserData*											m_pstShowUD;			// Show UserData (ShowCB으로 넘겨줄 UserData)
	CHAR*													m_szTooltip;			// Tooltip 내용
};

typedef struct AgcdUIActionBuffer
{
	AuList< AgcdUIAction >									m_listActions;
	AgcdUIControl*											m_pcsControl;
	AgcdUIEventMap*											m_pcsEvent;
	INT32													m_lDataIndex;
	ApBase*													m_pcsOwnerBase;

	AgcdUIActionBuffer( void )
	{
		m_pcsControl = NULL;
		m_pcsEvent = NULL;
		m_lDataIndex = 0;
		m_pcsOwnerBase = NULL;
	}
} AgcdUIActionBuffer;

// UI ===========================================================================================

enum AgcdUIType
{
	AGCDUI_TYPE_NORMAL										= 0,	// Normal Type
	AGCDUI_TYPE_POPUP_V,											// Auto Align Menus Vertically
	AGCDUI_TYPE_POPUP_H,											// Auto Align Menus Horizontally
	AGCDUI_MAX_TYPE
};

enum AgcdUIStatus					// UI의 현재 상태
{
	AGCDUI_STATUS_NONE										= 0,	// 아무 상태도 아니다. (Initial)
	AGCDUI_STATUS_ADDED,											// UI List에 Add되었다.
	AGCDUI_STATUS_CLOSING,											// UI가 닫히고 있다.
	AGCDUI_STATUS_OPENING,											// UI가 Open되고 있다.
	AGCDUI_STATUS_OPENED											// UI가 현재 Open되어있다.
};

enum AgcdUICoordSystem
{
	AGCDUI_COORDSYSTEM_ABSOLUTE								= 0,
	AGCDUI_COORDSYSTEM_RELATIVE,
	AGCDUI_COORDSYSTEM_CURSOR,
	AGCDUI_MAX_COORDSYSTEM
};

enum AgcdUIAttachType
{
	AGCDUI_ATTACH_TYPE_LEFT									= 0,
	AGCDUI_ATTACH_TYPE_RIGHT,
	AGCDUI_ATTACH_TYPE_TOP,
	AGCDUI_ATTACH_TYPE_BOTTOM,
	AGCDUI_ATTACH_MAX_TYPE
};

class AgcdUI : public ApBase					// UI
{
public:
	AgcdUIType												m_eUIType;					// UI의 Type
	AgcdUIStatus											m_eStatus;					// 현재 UI의 상태
	AgcdUICoordSystem										m_eCoordSystem;				// 좌표체계

	UINT32													m_ulModeFlag;				// 이 UI가 유효한 Mode Flag (Bitwise 연산)

	INT32													m_alX[ AGCDUI_MAX_MODE ];	// 해상도별 X좌표
	INT32													m_alY[ AGCDUI_MAX_MODE ];	// 해상도별 Y좌표

	CHAR													m_szUIName[ AGCDUIMANAGER2_MAX_NAME ];	// Name
	AgcUIWindow*											m_pcsUIWindow;				// Base UI Window
	
	INT32													m_lMaxChildID;				// Max Child ID
	AuList< AgcdUIControl* >								m_listChild;				// Child Controls

	AgcdUI*													m_pcsParentUI;				// Parent UI
	CHAR													m_szParentUIName[ AGCDUIMANAGER2_MAX_NAME] ;	// Parent UI Name
	BOOL													m_bUseParentPosition;		// Use Parent Position?

	AgcdUI*													m_pcsCurrentChildUI;		// Current Active Child UI

	BOOL													m_bMainUI;					// Main UI? (Initial)
	BOOL													m_bAutoClose;				// ESC 키로 닫을 수 있다. (MainUI 제외)
	BOOL													m_bEventUI;
	BOOL													m_bTransparent;				// UI위의 Control들을 Click한게 아니라면, UI 자체는 Click이 먹지 않게한다.
	
	BOOL													m_bInitControlStatus;		// Be Init Control Status At UI OPEN!

	FLOAT													m_fAlpha;
	UINT32													m_ulProcessingStartTime;

	AgcdUI*													m_pcsUIAttach;				// Attached parent UI
	AgcdUIAttachType										m_eAttachType;
	AuList< AgcdUI* >										m_listAttached;				// Attached child UIs

	AgcdUIAttachType										m_eAttachTypeToolTip;
	AcUIToolTip*											m_pcsTooltipAttach;

	AgcdUIFunction*											m_pstSetFocusFunction;
	AgcdUIFunction*											m_pstKillFocusFunction;

	INT32													m_lUIData;

	AgcdUI( void )
	{
		m_bInitControlStatus = TRUE;
		m_lUIData =	0;
	}
};

// Mouse Cursor =================================================================================

typedef struct AgcdUICursor
{
	HCURSOR													m_hCursor[ AGCDUIMANAGER2_MAX_CURSOR_FRAME ];
	INT32													m_lTotalFrame;				// Total Animation Frame
	INT32													m_lCurrentFrame;			// Current Animation Frame;
	INT32													m_lTick;					// Tickcount to next Frame
	INT32													m_lSpeed;					// Animation Speed (작을 수록 빠름)
} AgcdUICursor;


#endif //_AGCDUIMANAGER2_H_
