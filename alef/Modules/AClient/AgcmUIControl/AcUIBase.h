// AcUIBase.h: interface for the AcUIBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUIBASE_H__DF1EAA6C_C816_45EE_BC3D_4DE7C338BAA1__INCLUDED_)
#define AFX_ACUIBASE_H__DF1EAA6C_C816_45EE_BC3D_4DE7C338BAA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"
#include "AgcmSound.h"
#include "AgcmFont.h"
#include "AcuTextureList.h"

// UI 컨트롤의 베이스 모듈..
//UI컨트롤 처리..
//
//UI이미지는 따로 윈도우 처리를 하지는 않고..
//클래스만 해당 윈도우에 추가하는 방식..
//즉..
//
//버튼 클래스를 보면..
//1,윈도우 헤더에 추가..
//	CButton button;
//2,
//3,메시지 루프에 추가..
//	button.MessageProc( message , param1, param2 );
//4,렌더 사이클에 추가.. PostRender에다가..
//	buttne.Render( raster );
//
//이걸로 설치 끝..

//Ui컨트롤 구조..
//
//UiBase
//- UiButton
//- UiCheckBox
//- UiRadioButton
//
//- UiSliderControl
//- UiVerticalScroll
//           
//- UiTextBox
//- UiEditBox
//- UiListBox


// 사용자 시나리오
// AgcWindow Init 에서..
// AcUIBase * pControl;
// pControl = new AcUIBase;
// AddControl( pControl , type, id );
// ok?

#define	ACUIBASE_STATIC_STRING_START_X					10
#define	ACUIBASE_STATIC_STRING_START_Y					10
#define	ACUIBASE_STATUS_MAX_NUM							20
#define	ACUIBASE_STATUS_NAME_LENGTH						32
#define	ACUIBASE_STATUS_ID_BIT_CONSTANT					0x00020	// 반드시 ACUIBASE_STATUS_MAX_NUM 보다는 큰 값이 어야 한다 
#define	ACUIBASE_STATUS_ID_MAX							1000

typedef enum AgcdUIHotkeyType
{
	AGCDUI_HOTKEY_TYPE_NORMAL							= 0	,
	AGCDUI_HOTKEY_TYPE_SHIFT,
	AGCDUI_HOTKEY_TYPE_CONTROL,
	AGCDUI_HOTKEY_TYPE_ALT,
	AGCDUI_MAX_HOTKEY_TYPE,
} AgcdUIHotkeyType;

typedef enum
{
	ACUIBASE_HALIGN_LEFT								= 0,
	ACUIBASE_HALIGN_CENTER,
	ACUIBASE_HALIGN_RIGHT,
	ACUIBASE_MAX_HALIGN,
} AcUIBaseHAlign;

typedef enum
{
	ACUIBASE_VALIGN_TOP									= 0,
	ACUIBASE_VALIGN_CENTER,
	ACUIBASE_VALIGN_BOTTOM,
	ACUIBASE_MAX_VALIGN,
} AcUIBaseVAlign;

#define	ACUIBASE_GET_HALIGN( align )					( align & 0x0f )
#define	ACUIBASE_GET_VALIGN( align )					( align & 0xf0 )

struct	stDragInfomation
{
	RsMouseStatus										ms;	
	INT32												lMyControlID;	// Drag를 주도한 Window의 ControlID
	INT32												lID;			// Drag한 Item등의 ID
};

// DragDrop이 왔을때 정보를 저장해 놓는다 
struct AcUIDragInfo
{
	AcUIBase*											pSourceWindow;	// Drap된 Source Window
	AcUIBase*											pTargetWindow;	// DragDrop 된 Target Window 
	INT32												lX;				// DragDrop 된 Target Window위의 좌표
	INT32												lY;				// DragDrop 된 Target Window위의 좌표

	AcUIDragInfo( void )
	{
		pSourceWindow =	pTargetWindow =	NULL;
		lX = lY = 0;
	}
};

struct	AgcdUIBaseProperty
{
	BOOL												bRenderBlank;
	BOOL												bShrink;

	AgcdUIBaseProperty( void ) : bRenderBlank( FALSE ), bShrink( TRUE )
	{		
	}
};

// AcUIImage에서 이사온 짐들************************************/
enum 
{
	UICM_BASE_MESSAGE_SET_FOCUS							= 0,
	UICM_BASE_MESSAGE_KILL_FOCUS,
	UICM_BASE_MESSAGE_LCLICK,
	UICM_BASE_MESSAGE_RCLICK,
	UICM_BASE_MESSAGE_LDBLCLICK,
	UICM_BASE_MESSAGE_RDBLCLICK,
	UICM_BASE_MESSAGE_CLOSE,
	UICM_BASE_MESSAGE_INIT,
	UICM_BASE_MESSAGE_DRAG_DROP,
	UICM_BASE_MAX_MESSAGE								= 10
};

struct stWindowRenderInfo
{
	float												fStartU;
	float												fStartV;
	float												fEndU;
	float												fEndV;

	FLOAT												fScale;

	DWORD												lRenderColor;		// ( DrawIm2d ) Render Color
	UINT8												cRenderAlpha;		// ( DrawIm2d ) Render Alpha
	INT32												lRenderID;			// ( DrawIm2d ) Render될 Texture ID
	INT32												lDefaultRenderID;	// ( DrawIm2d ) Default로 Render될 Texture ID
};

struct stStatusInfo
{
	RwRect												m_rectStatusPos;	// Status별 Pos
	INT32												m_lStatusImageID;	// Status별 Image ID 

	CHAR												m_szStatusName[ ACUIBASE_STATUS_NAME_LENGTH ];
	INT32												m_lStatusID;		// Status의 고유 ID 
	BOOL												m_bSetting;			// Setting 된 Status인지 여부 

	BOOL												m_bVisible;			// Visible ?
	DWORD												m_lStringColor;		// Static String Color
};

typedef struct
{
	PVOID												m_pvData;			// User Data
	INT32												m_lDataSize;		// Data Size
	INT32												m_lCount;			// Data Count
} AcUIBaseData;

struct AcUIBaseAnimation
{
	UINT32												m_ulClockCount;		// TickCount
	INT32												m_lTextureID;		// Texture ID
	BOOL												m_bRenderString;	// Render Text
	BOOL												m_bRenderBaseTexture;	// Render Base Texture
	struct AcUIBaseAnimation*							m_pNext;			// Next Animation

	AcUIBaseAnimation( void )
	{
		m_ulClockCount = 0;
		m_lTextureID = 0;
		m_bRenderString	= TRUE;
		m_bRenderBaseTexture = TRUE;
		m_pNext = NULL;
	}
};

typedef struct AcUIBaseAnimation AcUIBaseAnimation;

/////////////////////////////////////////////////////////////////

typedef BOOL ( *AcUIDefalutCallback )( PVOID pClass, PVOID pData_1, PVOID pData_2, PVOID pData_3 );

class AgcmUIControl;

class AcUIBase  : public AgcWindow
{
public:
	AcUIBase( void );
	virtual ~AcUIBase( void );

public:
	enum
	{
	TYPE_NONE,
	TYPE_BASE,
	TYPE_EDIT,
	TYPE_BUTTON,
	TYPE_CHECKBOX,
	TYPE_COMBO,
	TYPE_NUMBER,
	TYPE_TOOLTIP,
	TYPE_GRID,
	TYPE_BAR,
	TYPE_LIST,
	TYPE_LIST_ITEM,
	TYPE_SKILL_TREE,
	TYPE_GRID_ITEM,
	TYPE_SCROLL,
	TYPE_SCROLL_BUTTON,
	TYPE_CUSTOM,
	TYPE_TREE,
	TYPE_TREE_ITEM,
	TYPE_CLOCK,
	TYPE_COUNT,	
	};

	
	BOOL												m_bPassPostMessage;		// Post Message를 Parent에게 넘길것인가?
	BOOL												m_bClipImage;

	static AgcmUIControl*								m_pAgcmUIControl;
	static AgcmSound*									m_pAgcmSound;
	static AgcmFont*									m_pAgcmFont;
	static RwTexture*									m_pBaseTexture;

	stWindowRenderInfo									m_stRenderInfo;

	// Static Control용 파라메터
	INT32												m_lStaticStringLength;	// Static String Length
	CHAR *												m_szStaticString;		// Static String
	FLOAT												m_fScale;				// Static String Scale
	INT32												m_lFontType;			// Static String Font Type 
	DWORD												m_lColor;				// Static String Color
	BOOL												m_bShadow;				// Static String Shadow
	BOOL												m_bDrawStaticString;
	BOOL												m_bAutoFitString;		// Static String이 범위를 벗어나면 '...'로 처리?
	BOOL												m_bStringNumberComma;	// String에 Comma 표시를 해줄것인가?

	BOOL												m_bBlinkText;			// 텍스트 블링크 설정.
	DWORD												m_lColorSecond;
	DWORD												m_lBlinkGap;
	// Static Control용 파라메터 끝 

	// Tooltip 내용
	CHAR *												m_szTooltip;			// Tooltip
	BOOL												m_bTooltipForFitString;

	// Texture List
	AcuTextureList										m_csTextureList;

	BOOL												m_bActiveStatus;		// 이게 TRUE이면 PreTranslateMessage에서 Message를 걸른다. (Tool에서 Edit할때 사용)
	BOOL												m_bDrawOutline;			// 이게 TRUE이면, Outline을 그린다.
	BOOL												m_bShrinkMode;			// Shrink가 어떤 모드 인가 

	AgcdUIBaseProperty									m_clProperty;			// AcUIBase 의 Property 

	AcUIBaseHAlign										m_eHAlign;				// StaticString의 HAlign
	AcUIBaseVAlign										m_eVAlign;				// StaticString의 VAlign

	INT32												m_lStringX;				// StaticString이 쓰여지는 X 좌표
	INT32												m_lStringY;				// StaticString이 쓰여지는 Y 좌표

	INT32												m_lStringOffsetX;		// StaticString이 쓰여지는 X Offset
	INT32												m_lStringOffsetY;		// StaticString이 쓰여지는 Y Offset

	INT32												m_lStringWidth;
	INT32												m_lStringHeight;

	BOOL												m_bImageNumber;			// StaticString이 Image Number로 쓰여질 것인가?
	FLOAT*												m_pfAlpha;				// Global Alpha

	INT32												m_lImageOffsetX;
	INT32												m_lImageOffsetY;

	UINT32												m_ulClockCount;
	UINT32												m_ulPrevClockCount;
	BOOL												m_bAnimation;
	BOOL												m_bAnimationDefault;
	AcUIBaseAnimation*									m_pAnimationData;
	AcUIBaseAnimation*									m_pCurrentAnim;

	INT32												m_lAbsolute_x;
	INT32												m_lAbsolute_y;

protected:
	AcUIBaseData										m_stUserData;			// User Data
	AcUIDragInfo										m_stDragDropInfo;
	BOOL												m_bDragDropMessageExist;

public:
	VOID				SetStaticStringExt				( char* szString, float fScale = 1.0f, INT32 lFontType = 0,  
															DWORD lColor = 0xffffffff, bool bShadow = true,
															AcUIBaseHAlign eHAlign = ACUIBASE_HALIGN_LEFT, AcUIBaseVAlign = ACUIBASE_VALIGN_TOP,
															BOOL bImageNumber = FALSE );
	VOID				SetStaticString					( char* szString );

	BOOL				OnCommand						( INT32	nID, PVOID pParam );
	VOID				PlayUISound						( char* pszFileName	);

	INT32				AddImage						( char* filename, BOOL bLoad = TRUE	);	// 해당 인덱스의 이미지 레스터를 리턴함..
	INT32				AddOnlyThisImage				( char* filename, BOOL bLoad = TRUE );	//List에 모든 이미지를 지우고 넘어온 이미지만 담는다. 

	BOOL				RenderTexture					( INT32 lRenderID );
	
	VOID				SetRenderColor					( DWORD lColor );	// Render시 사용될 Color를 바꾼다
	VOID				SetRenderAlpha					( UINT8 cAlpha );	// Render시 사용될 Alpha를 바꾼다
	BOOL				SetRednerTexture				( INT32 lID = -1 );	// Render Texture Index를 바꾼다 (-이면 Default, 0이면 NULL Image) - 성공하면 TRUE를 리턴
	VOID				SetRenderScale					( FLOAT fScale );
	VOID				SetDefaultRenderTexture			( INT32 lID );		// Default Render Texture Index를 바꾼다.

	DWORD				GetRenderColor					( void );
	UINT8				GetRenderAlpha					( void );
	INT32				GetRenderTexture				( void );
	FLOAT				GetRenderScale					( void );
	INT32				GetDefaultRenderTexture			( void );

	VOID				SetRenderDefaultImage			( BOOL bRenderBlank ) { m_clProperty.bRenderBlank = bRenderBlank; }
	BOOL				GetRenderDefaultImage			( void ) { return m_clProperty.bRenderBlank; }

	VOID				SetActive						( BOOL bActive ) { m_bActiveStatus = bActive; }
	VOID				SetDrawOutline					( BOOL bDrawOutline ) { m_bDrawOutline = bDrawOutline; }

	VOID				DrawOutline						( void );
	void				CloseUI							( void );
	
	BOOL				PreTranslateInputMessage		( RsEvent event , PVOID param );
	AcUIDragInfo*		GetDragDropMessage				( void );

	BOOL				SetUserData						( PVOID pvData, INT32 lDataSize, INT32 lDataCount );
	PVOID				GetUserData						( INT32 *plDataSize = NULL, INT32 *plDataCount = NULL );

	DWORD				GetColor						( void );
	BOOL				SetBlink						( BOOL bBlink , DWORD dColor = 0 , DWORD dGap = 300 ) { m_lColorSecond = dColor; m_lBlinkGap = dGap; return m_bBlinkText = bBlink; }

	virtual BOOL		OnSetUserData					( void ) { return TRUE; }

	// AcUIImage 용.
	virtual	VOID 		OnWindowRender					( void );
	virtual VOID 		OnAddImage						( RwTexture* pTexture );
	virtual	VOID 		OnClose							( void );	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close되는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
	virtual	BOOL 		OnLButtonDown					( RsMouseStatus *ms	);
	virtual BOOL 		OnLButtonUp						( RsMouseStatus *ms );
	virtual BOOL 		OnLButtonDblClk					( RsMouseStatus *ms );
	virtual	BOOL 		OnMouseMove						( RsMouseStatus *ms	);

	// New virtual Function from AcUIBase 
	virtual BOOL 		OnLButtonUpForHandler			( RsMouseStatus *ms );
	virtual BOOL 		OnLButtonDblClkForHandler		( RsMouseStatus *ms );
	virtual BOOL 		OnLButtonDownForHandler			( RsMouseStatus *ms );
	virtual BOOL 		OnRButtonUp						( RsMouseStatus *ms );
	virtual BOOL 		OnRButtonDblClk					( RsMouseStatus *ms );
	virtual	VOID 		OnMoveWindow					( void );
	virtual VOID 		OnSetFocus						( void );
	virtual VOID 		OnKillFocus						( void );

	virtual BOOL		OnInit							( void );

	virtual BOOL 		OnDragDrop						( PVOID pParam1, PVOID pParam2 );
	virtual void 		OnCloseUI						( void ) { }

	// Handler 관련 
public:
	BOOL												m_bExistHandler;
	RwRect												m_rectHandlerArea;
	RwRect												m_rectOriginalPos;	// Shrink Mode 이전의 Original Pos 
	
	VOID				EnableHandler					( BOOL bEnable );
	VOID				SetHandler						( INT32 lHandler_x, INT32 lHandler_y, INT32 lHandler_w, INT32 lHandler_h );
	BOOL				SetShrinkMode					( BOOL bShrink );
	BOOL				CheckInHandlerArea				( RsMouseStatus* ms );

	VOID				UpdateChildWindow				( void ) { WindowListUpdate(); }

private:
	VOID				MoveChildByShrink				( BOOL bShrink );	// Shrink 시 Child들을 다시 정돈한다 , Expand시에는 bShirink를 FALSE를 준다  
	void				_RenderBaseTexture				( BOOL bRenderBaseTexture );
	void				_UpdateAnimation				( BOOL* pbRenderBaseTexture, BOOL* pbDrawStaticString );
	void				_RenderText						( void );
	void				_DrawOutLine					( FLOAT fPosX, FLOAT fPosY, FLOAT fWidth, FLOAT fHeight );

	// Status 관련
public:
	INT32												m_lDefaultStatusID;
	INT32												m_lCurrentStatusID;
	stStatusInfo										m_astStatus[ ACUIBASE_STATUS_MAX_NUM ];

	// Set Status Info : Setting이 정상적으로 이루어지면 Status Index를 돌려준다. 실패시 -1을 리턴 
	INT32				SetStatusInfo					( INT32 lPosx, INT32 lPosy, INT32 lPosw, INT32 lPosh,
															INT32 lImageID, CHAR* pszName, INT32 lStatusID = -1,
															BOOL bVisible = TRUE, DWORD lStringColor = 0xffffffff);
	stStatusInfo*		GetStatusInfo_ID				( INT32 lStatusID );
	stStatusInfo*		GetStatusInfo_Index				( INT32 lStatusIndex );
	BOOL				DeleteStatusInfo_ID				( INT32 lStatusID );
	BOOL				DeleteStatusInfo_Index			( INT32 lStatusIndex );
	BOOL				DeleteAllStatusInfo				( void );
	
	BOOL				SetStatus						( INT32 lStatusID, BOOL bSaveCurrent = TRUE );
	VOID				SetDefaultStatus				( INT32 lStatusID ) { m_lDefaultStatusID = lStatusID; }
	INT32				GetDefaultStatus				( void ) { return m_lDefaultStatusID; }

	// Animation 관련
	AcUIBaseAnimation*	AddAnimation					( INT32 lTextureID, UINT32 ulClockCount, BOOL bRenderText, BOOL bRenderBaseTexture );
	BOOL				RemoveAnimation					( AcUIBaseAnimation *pAnimation );
	VOID				RemoveAllAnimation				( void );
	BOOL				StartAnimation					( void );
	VOID				StopAnimation					( void );

	VOID				SetTooltip						( CHAR *szTooltip, BOOL bForFitString = FALSE );
	VOID				MakeStringFit					( CHAR *szString = NULL, INT32 lWidth = -1 );

private:
	VOID				SetShrinkModeRenderInfo			( void );	// 현재 Texture 에 맞도록 Render Info를 Setting한다 
};

//@{ 2006/07/04 burumal
extern CHAR* g_aszHotkeyNameTable[ 256 ];
//@}

#endif // !defined(AFX_ACUIBASE_H__DF1EAA6C_C816_45EE_BC3D_4DE7C338BAA1__INCLUDED_)
