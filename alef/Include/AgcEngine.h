// AgcEngine.h: INT32erface for the AgcEngine class.
// 클라이언트 엔진 클래스.
// 작성 : 정재욱
// 일자 : 2002/04/11
// -= Update Log =-
// 마고자 (2002-08-20 오후 4:56:23) : 라이브러리 형식으로 변경.. 상속받아서 메시지 오버라이딩 하는 방식을 사용한다.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCENGINE_H__8BCC5208_F2F4_4E6C_B3DE_B52CB449BB5C__INCLUDED_)
#define AFX_AGCENGINE_H__8BCC5208_F2F4_4E6C_B3DE_B52CB449BB5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

#include "AgcModule.h"
#include "ApModuleManager.h"
#include "AuList.h"

#include "AcUtils.h"
#include "MagDebug.h"

#include <skeleton.h>
#include <camera.h>
#include <menu.h>

#include "AcSocketManager.h"
#include "AuCryptManager.h"

#include "AcDefine.h"
#include "AcuTexture.h"
#include <queue>
#include "AuLua.h"
#include "AgcWindow.h"

#define	LUA_RETURN( result )	{ pLua->PushNumber( ( double ) result ); return 1; }

#define	MAX_CLIENT_MODULE			10

#define DEFAULT_SCREEN_WIDTH		(1024)
#define DEFAULT_SCREEN_HEIGHT		(768)
#define DEFAULT_ASPECTRATIO_WIDE	(16.0f/9.0f)
#define DEFAULT_ASPECTRATIO_NORMAL	(4.0f/3.0f)
#define DEFAULT_VIEWWINDOW			(0.7f)


typedef BOOL	(*AgcEngineDefaultCallBack) (PVOID pData, PVOID pClass, PVOID pCustData);		// AgcEngine Default Callback Function Type 

struct Im2DScreenData			// font를 위해 필요한 정보를 얻어오기 위해(DrawIm2DInWorld2에 쓰임)
{
	RwV3d	Screen_Pos;				// x,y 화면 좌표 z -  screenz
	float	Recip_Z;
};

class AgcEngine : public ApModuleManager
{
protected:
	ApMutualEx			m_csMutexRender;	// Render Lock
	ApCriticalSection	m_csMutexFrame;		// Frame Lock
	ApCriticalSection	m_csMutexTexture;	// Texture Lock
	ApCriticalSection	m_csMutexResArena;	// Resource Arena Lock
	ApCriticalSection	m_csMutexClump;		// Clump Lock

	BOOL				m_bReceivedMouseMessage;	// 마우스 입력 처리개선용..
	
	HWND				m_hWnd;				// 윈도우 핸들..

public:
	// RenderWare Datas
	enum ACTIVATION_FLAG
	{
		AGCENGINE_NORMAL	,
		AGCENGINE_SUSPENDED	,
		AGCENGINE_MINOR		
	};

	INT32		m_nActivated		;	// 액티베이션 플래그로 이용..
	RwBool		m_FPSOn				;	// FPS 표시 On/off			--> AgcmRender로 이동

	RwInt32		m_nFrameCounter		;
	RwInt32		m_nFramesPerSecond	;

	RwRGBA		m_rgbForegroundColor;
	RwRGBA		m_rgbBackgroundColor;

	RpWorld		*m_pWorld			;
	RwCamera	*m_pCamera			;

	RwReal		m_fNearClipPlane	;
	RwReal		m_fFarClipPlane		; // 절사면 위치.

	BOOL		m_bFogEnable		;

	RpLight		*m_pLightAmbient	;	// 광원
	RpLight		*m_pLightDirect		;

	AcuCamera	*m_clCamera			;
	BOOL		m_bWideScreenMode	;	// 와이드 모드

	AgcWindow	*m_pCurrentFullUIModule		;	// 현재 적용됀 Full UI Module

	
	AcSocketManager m_SocketManager;

	AgcWindow	*m_pFocusedWindow	;	// Focus를 가지고 있는 Window

	BOOL		m_bRenderWorld			;
	BOOL		m_bRenderWindow			;

	INT32		m_lIdleModal			;

	AuList<AgcWindowNode *>	m_listWindowToDelete;

	BOOL			m_bModalMessage		;

	
	CHAR			m_szLanguage[128];		// 하부에서 사용함.
	RsMouseStatus	m_stLastMouseMoveState; // 최근에 마우스 이동 상태 설정..

protected:
	// Alef Datas
	AgcModule*	m_apFullUIModule[ MAX_CLIENT_MODULE ];
	AgcModule*	m_apWindowUIModule[ MAX_CLIENT_MODULE ];
	AgcModule*	m_apNetworkModule[ MAX_CLIENT_MODULE ];
	AgcModule*	m_apDataModule[ MAX_CLIENT_MODULE ];

	INT16		m_nFullUIModuleCount;
	INT16		m_nWindowUIModuleCount;
	INT16		m_nNetworkModuleCount;
	INT16		m_nDataModuleCount;

	// 이 데이타 맴버들은 모듈들을 체계적으로 관리하기 위해
	// 종류별로 분류하여 포인터로 가지고 있기 위해서이다. 이 리스트는
	// 모듈 레지스터할때 자동으로 쌓이게 돼며 , 모듈은 제작될때
	// 타입을 지정해야한다.
	
	bool		m_bSuspendedFlag		; // 카메라 생성실패시 서스펜디드 플래그 삽입.
	UINT		m_uCurrentTick			; // 현재 틱.
	UINT		m_uPastTick				; // 전프레임에서 현 프레임 사이의 틱.

	bool		m_bNeedWindowListUpdate	;

	INT32		m_lOneTimePopCount;
	
	BOOL		m_bIMEComposingState	;
	BOOL		m_bCharCheckState		;

	void		WindowListUpdate()		;

	CHAR*		m_szImagePath;

	// Set Cursor Mouse Position 을 위한 Member
	AgcEngineDefaultCallBack	m_pSetCursorPositionCallback;
	PVOID						m_pSetCursorPositionCallbackClass;

	UINT32		m_uDebugFlag;
	FLOAT		m_fCameraProjection;

	INT32		m_lMaxPacketBufferSize;

	UINT32		m_ulPrevProcessClock;
	BOOL		m_bImmediateIdleTerminateFlag;

	static BOOL	m_bLCtrl;
	static BOOL	m_bRCtrl;
	static BOOL	m_bLAlt;
	static BOOL	m_bRAlt;
	static BOOL	m_bLShift;
	static BOOL	m_bRShift;
	static BOOL m_bMouseLeftBtn;
	static BOOL m_bMouseRightBtn;
	static BOOL m_bMouseMiddleBtn;

public:
	enum	DEBUGFLAG
	{
		SHOW_GRID				= 0x01,
		SHOW_GEOMETRY_COLLISION	= 0x02,
		SHOW_GEOMETRY_BLOCKING	= 0x04,
		SLEEP_MAIN_THREAD		= 0x08,
		SLEEP_BACK_THREAD		= 0x10,
		CAMERA_ANGLE_CHANGE		= 0x20,	// Ctrl + Wheel 돌리면 카메라 화각 변경.
		AUTO_HUNTING			= 0x40,
		OBJECT_LOADING_ERROR	= 0x80,	// 오브젝트 로딩에 실패하면..
	};

	AgcEngine();
	virtual ~AgcEngine();

	HWND	GethWnd()				{	return m_hWnd;				}
	BOOL	ShowFps( BOOL bShow	)	{	return m_FPSOn = bShow;		}
	BOOL	GetFpsMode()			{	return m_FPSOn; }
	RwInt32	GetFps()				{	return m_nFramesPerSecond;	}

	UINT32	SetDebugFlag( UINT32 uFlag )	{	return m_uDebugFlag = uFlag; }
	UINT32	GetDebugFlag()					{	return m_uDebugFlag;		}

	// 프로젝션 변경함수.
	BOOL	SetProjection( FLOAT fRate ); // 0.2~ 2.0 사이 값이 적당하다.
	BOOL	SetProjection()			{	return SetProjection( m_fCameraProjection );	}
	FLOAT	GetProjection()			{	return m_fCameraProjection;	}
	BOOL	SetWideScreen( BOOL bWide = TRUE );
	BOOL	GetWideScreen()			{	return m_bWideScreenMode;	}

	BOOL	GetIMEComposingState()	{	return m_bIMEComposingState;	}

	void	SetCharCheckOut()		{	m_bCharCheckState = TRUE;	}
	void	SetCharCheckIn()		{	m_bCharCheckState = FALSE;	}
	BOOL	GetCharCheckState()		{	return m_bCharCheckState;	}

	// 윈도우 관리 Operations!
	INT32	SetActivate( INT32 nActivated );
	INT32	GetActivate( void );

	void	SetImmediateIdleTerminateFlag()	{	m_bImmediateIdleTerminateFlag = TRUE;	}
	BOOL	GetImmediateIdleTerminateFlag()	{
		BOOL	bRet = m_bImmediateIdleTerminateFlag;
		m_bImmediateIdleTerminateFlag = FALSE;
		return bRet;
	}

	BOOL	AddChild( AgcWindow* pModule, INT32 nid = 0	);
	BOOL	CloseWindowUIModule( AgcWindow* pModule ); // 탑레벨만 체크함.
	BOOL	SetFullUIModule( AgcWindow* pModule	);
	
	// Idle Time용 함수들
	UINT	GetTickCount()			{	return m_uCurrentTick;	}
	UINT	GetPastTime()			{	return m_uPastTick;		}
	UINT	SetTickCount( UINT tick );

	INT32	GetOneTimePopCount()	{	return m_lOneTimePopCount;	}
	VOID	SetOneTimePopCount(INT32 lOneTimePopCount)	{	m_lOneTimePopCount = lOneTimePopCount;	}

	// Initialize Operations
	BOOL		RegisterModule		( AgcModule * pModule	);	// 모듈을 등록함.
	RwCamera	*CreateCamera		( RpWorld *world		);
	RpWorld		*CreateWorld		();	// Empty World 생성.
	void		CreateLights		(void);
	void		DestroyLights		(void);

	AgcWindow *	WindowFromPoint		( INT32 x , INT32 y, BOOL bCheckUseInput = TRUE );

	// Focussing!
	BOOL		SetFocus( AgcWindow * pWindow	);
	BOOL		SetCapture( AgcWindow * pWindow	);
	BOOL		ReleaseCapture();
	
	// Client Socket 최대 연결 갯수 설정
	BOOL		SetMaxConnection( INT32 lMaxConnection	, INT32 lMaxPacketBufferSize	);
	
	// 서버 접속 관련
	INT16	connect( CHAR* pszServerAddr , INT16 nServerType	, PVOID pClass, ApModuleDefaultCallBack fpConnect, ApModuleDefaultCallBack fpDisconnect, ApModuleDefaultCallBack fpError);
	BOOL	disconnect( INT16 nNID );

	// Render Lock/Unlock (Lock 하면 OnRender가 안불린다.)
	inline VOID		LockRender	()		{		}
	inline VOID		UnlockRender()		{		}

	// Render Lock/Unlock (Lock 하고 RwFrame 연산해야한다.)
	inline	VOID	LockFrame()			{		}
	inline	VOID	UnlockFrame()		{		}

	// 2004.11.20	gemani
	static	void 	RwLockFrame();
	static	void 	RwUnlockFrame();

	static	void	RwLockTexture();
	static	void	RwUnlockTexture();

	static	void	RwLockGeometry();
	static	void	RwUnlockGeometry();

	static	void*	RwAllocFreeListCB();
	static	void	RwFreeFreeListCB(void*	pCriticalSection);
	static	void	RwLockFreeList(void*	pCriticalSection);
	static	void	RwUnlockFreeList(void*	pCriticalSection);

	static	void	RwLockResArena();
	static	void	RwUnlockResArena();

	static	void	RwLockClump();
	static	void	RwUnlockClump();

	static	void*	RwResEntryCreateCB();
	static	void	RwResEntryFreeCB(void*	pCriticalSection);
	static	void	RwLockResEntry(void*	pCriticalSection);
	static	void	RwUnlockResEntry(void*	pCriticalSection);

	// Message
	virtual BOOL		OnInit()				{	return TRUE;	}
	virtual	BOOL		OnAttachPlugins()		{	return TRUE;	}
	virtual	BOOL		OnRegisterModulePreCreateWorld()	{	return TRUE;	}
	virtual	RpWorld *	OnCreateWorld(); // 월드를 생성해서 리턴해준다.

	virtual	BOOL		OnPreRegisterModule()	{	return TRUE;	}
	virtual	BOOL		OnRegisterModule()		{	return TRUE;	}
	virtual BOOL		OnPostRegisterModule()	{	return TRUE;	}
	virtual	BOOL		OnPostInitialize()		{	return TRUE;	}

	virtual	RwCamera *	OnCreateCamera( RpWorld * pWorld );	// 메인 카메라를 Create해준다
	virtual BOOL		OnCameraResize( RwRect * pRect );
	virtual BOOL		OnDestroyCamera();
	
	virtual	BOOL		OnCreateLight();	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
	virtual	void		OnDestroyLight();
	virtual	AgcWindow *	OnSetMainWindow()		{	return NULL;	}

	// 패킷 관련
	virtual BOOL		SendPacket(PVOID pvPacket, INT16 nLength, UINT32 nFlag = ACDP_SEND_GAMESERVERS, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 ulNID = 0);

	static BOOL			SendPacket( PACKET_HEADER& pPacket);

	// 엔진에서 처리하는 메시지들.
	// 이쪽은 큰일 없으면 오버라이드 하지 않는다.
	virtual	BOOL			OnInitialize3D( PVOID param ); // 그래픽 모듈 초기화
	virtual	BOOL			OnAttachInputDevices();	// 입력기 연결은 여기서
	virtual	BOOL			OnIdle(); // 엔진단위 컨트롤을 위한 idle타임처리
	virtual BOOL			OnRenderPreCameraUpdate()	{	return TRUE;	}
	virtual void			OnRender();	// 렌더링 과정 
	virtual	void			OnWorldRender()				{					}
	virtual	BOOL			OnInitialize();	//엔진 자체 , 그래픽 들어가기전 초기화
	virtual	void			OnTerminate();	// 프로그램 끝나기전
	virtual BOOL			OnSelectDevice(); // 디스플레이 디바이스와 해상도를 선택하는것..


	// near,far,viewwindow변경 등 카메라 설정이 변경되었을 경우
	// 0 - View Window size변경시
	// 1 - Near변경시
	// 2 - Far변경시
	virtual void			OnCameraStateChange		(CAMERASTATECHANGETYPE ChangeType);	
	
	virtual RsEventStatus	OnMessageSink			( RsEvent event , PVOID param ); // 메시지를 처리하여 모듈들에게 넘겨줌
	virtual UINT32			OnWindowEventHandler	(HWND window, UINT message, WPARAM wParam, LPARAM lParam );

	static RsEventStatus	KeyboardHandler	( RsEvent event , PVOID param ); // 키보드 메시지 받음
	static RsEventStatus	MouseHandler	( RsEvent event , PVOID param ); // 마우스 메시지 받음;

	virtual BOOL SetSelfCID		( INT32 lCID			);	// SelfCID를 DP Module에 등록.

	virtual VOID OnEndIdle		(						){return;}	// Idle Tick 이 한번 끝나면 불른다. 

	// Camera설정 함수
	void				SetWorldCameraNearClipPlane	(float	fNear);
	void				SetWorldCameraFarClipPlane	(float	fFar);
	void				SetWorldCameraViewWindow	(int w,int h);

	// Cursor Mouse Position Set Call back funciton
	void				SetCallbackSetCursorMousePosition( AgcEngineDefaultCallBack pfCallback, PVOID pClass );

	VOID				SetRenderMode	( BOOL bRenderWorld = TRUE	, BOOL bRenderWindow = TRUE	)
	{
		m_bRenderWorld	= bRenderWorld	;
		m_bRenderWindow	= bRenderWindow	;
	}

protected:
	void	DisplayOnScreenInfo		();	// Display Frame Rate
	void	ProcessDeleteWindow		();

public:
	// Modal Window처리.. 따로 호출해주면 혼난다.
	AuList< AgcModalWindow * >	m_listModalStack;

	// 리스트만 관리하는 함수이다.. 조심!..
	BOOL	OpenModalWindow		( AgcModalWindow * pWindow );
	BOOL	CloseModalWindow	( AgcModalWindow * pWindow );
	BOOL	IsModalWindow		( AgcModalWindow * pWindow );
	AgcModalWindow *	GetTopModalWindow();

	// Seong Yon - jun
	//AgcWindow		*m_pMonopolyUI			;	// Monopoly Window ( 각종 메세지를 독점한다 ) 의 Pointer
	vector< AgcWindow * >	m_vecMonopolyUI	;
	BOOL			m_bDraging				;	// 어떤 윈도우가 Draging 중임
	AgcWindow		*m_pMouseOverUI			;	// Focused Window의 Pointer - 게임내에서는 보통 마우스가 위로 가는 윈도우가 Focus임 
	AgcModalWindow	*m_pWaitingDialog		;	// Waiting Dialog
	AgcModalWindow	*m_pMessageDialog		;	// Message Dialog
	AgcModalWindow	*m_pMessageDialog2		;	// Message Dialog
	AgcModalWindow	*m_pOkCancelDialog		;	// OK Cancel Dialog
	AgcModalWindow	*m_pEditOkDialog		;	// Edit + OK Dialog
	AgcWindow		*m_pActiveEdit			;	// 현재 Active인 Edit Control

	void	SetSoundAlertMSGFile( char* pszFileName );	// Alert MSG파일 이름을 Setting한다
	CHAR	m_szSoundAlertMSG[256]			;			// Alert MSG Sound 파일 이름 - 이곳에는 특별히 Path까지 같이 넣어준다 

	AgcWindow*	GetMonopolyWindow()
	{
		if( m_vecMonopolyUI.size() )
		{
			vector< AgcWindow * >::reverse_iterator	rIter = m_vecMonopolyUI.rbegin();
			AgcWindow * pMonopolyUI	= *rIter;		
			return pMonopolyUI;
		}
		else return NULL;
	}

	BOOL	PushMonopolyWindow( AgcWindow* pWindow )	;	// Monopoly Window로 지정한다
	BOOL	PopMonopolyWindow	( AgcWindow* pWindow )	;	// Monopoly Window에서 해제한다 

	INT32	WaitingDialog( AgcWindow* pWindow, char* pszmessage )		;	// Waiting Dialog를 띄어준다
	INT32	WaitingDialogEnd()											;	// Waiting Dialog를 닫는다.
	INT32	MessageDialog( AgcWindow* pWindow, char* pszmessage )		;	// Message Dialog를 띄어준다
	INT32	MessageDialog2( AgcmUIManager2* pWindow, char* pszmessage)		;	// No Blocking Messsage Dialog
	INT32	EditMessageDialog( AgcWindow* pWindow, char* pszmessage, CHAR* pEditString)	;	// Messsage Dialog width EditBox
	INT32	EditMessageDialog2( AgcWindow* pWindow, char* pszmessage, CHAR* pEditString)	;	// Messsage Dialog width EditBox
	INT32	OkCancelDialog( AgcWindow* pWindow, char* pszmessage )		;	// Ok Cancel Dialog를 띄어준다
	INT32	OkCancelDialog2( AgcWindow* pWindow, char* pszmessage )		;	// Ok Cancel Dialog를 띄어준다

	BOOL	SetMeActiveEdit( AgcWindow*	pEdit	)						;	// 이 Edit가 Active Edit가 된다.
	BOOL	ReleaseMeActiveEdit( AgcWindow* pEdit )						;	// 이 Edit를 Deactive한다.

	AgcWindow*	GetFocusedWindow	()	{ return m_pFocusedWindow; }	// 현재 Focus를 가진 Window을 가져온다.

	My2DVertex		m_vBoxFan[4];
	RwReal			m_fRecipZ;

	BOOL			m_bDrawFPS;
	BOOL			m_bSaveScreen;

	IDirect3DDevice9*	m_pCurD3D9Device;
	bool		GetVideoMemoryState( DWORD * pTotal , DWORD * pFree );

	void		ScreenShotSave();
	void		ScreenShotSaveGDI();
	int			SaveToBmp(HANDLE file,LPDIRECT3DSURFACE9 pSurf);

	void		PrintRenderState();

	BOOL		DrawRwTexture( RwTexture* pTexture, INT32 nX, INT32 nY, DWORD dwColor = 0xffffffff/*ARGB*/ );
	bool		DrawIm2D( RwTexture* pTex, 
							float x, float y, float w, float h,
							float st_u = 0.f, float st_v = 0.f, float off_u = 1.f, float off_v = 1.f,
							DWORD color = 0xffffffff, UINT8 alpha = 255 );
	BOOL		DrawIm2DPixel( RwTexture* pTexture,
							INT32 nX, INT32 nY, INT32 nWidth = -1, INT32 nHeight = -1,
							INT32 nUStart = 0, INT32 nVStart = 0, INT32 nUEnd = -1, INT32 nVEnd	= -1,
							DWORD uColor = 0xffffffff, UINT8 uAlpha = 255 );
	BOOL		DrawIm2DPixelMask( RwTexture* pTexture, RwTexture* pMask , 
							INT32 nX, INT32 nY, INT32 nWidth = -1, INT32 nHeight = -1,
							INT32 nUStart = 0, INT32 nVStart = 0, INT32 nUEnd = -1, INT32 nVEnd	= -1,
							DWORD uColor = 0xffffffff, UINT8 uAlpha = 255 );
	BOOL		DrawIm2DPixelRaster( RwRaster*	pRaster , RwTexture* pTexture ,
							INT32 nX, INT32 nY, INT32 nWidth = -1, INT32 nHeight = -1,
							INT32 nUStart = 0, INT32 nVStart = 0, INT32 nUEnd = -1, INT32 nVEnd	= -1,
							DWORD uColor = 0xffffffff, UINT8 uAlpha = 255 );
	bool		DrawIm2DRotate( RwTexture* pTex,
								float x, float y, float w, float h,
								float st_u = 0.f, float st_v = 0.f, float off_u = 1.f, float off_v = 1.f,
								float fCenterX = 0.f, float fCenterY = 0.f, float fDegree = 0.f,
								DWORD color = 0xffffffff, UINT8 alpha = 255 );

	// In World Space Render
	// offset은 월드 좌표로 변환한 화면 좌표에서의 상대적 이동치이다. (ex. offset.x = -10 이라면 화면에 찍을때 x - 10 한후 찍는다
	bool		DrawIm2DInWorld(RwTexture* pTex,RwV3d* pWPos,RwV2d* offset,float w,float h,
								float st_u = 0.0f, float st_v = 0.0f, float off_u = 1.0f, float off_v = 1.0f,
								DWORD color=0xffffffff,UINT8	alpha = 255);

	bool		DrawIm2DInWorld(RwTexture* pTex, RwV3d* pCamPos, RwV3d* pScreenPos, float recipZ, RwV2d* offset, float w,float h,
								float st_u = 0.0f, float st_v = 0.0f, float off_u = 1.0f, float off_v = 1.0f,
								DWORD color=0xffffffff,UINT8	alpha = 255);

	// Im2DScreenData 로 agcmfont DrawTextIMMode2DWorld에 전달한 인자들을 얻을수 있다
	bool		DrawIm2DInWorld2(Im2DScreenData* pData,RwTexture* pTex,RwV3d* pWPos,RwV2d* offset,float w,float h,
								float st_u = 0.0f, float st_v = 0.0f, float off_u = 1.0f, float off_v = 1.0f,
								DWORD color=0xffffffff,UINT8	alpha = 255);
		
	void		GetWorldPosToScreenPos		( RwV3d * pWorldPos ,RwV3d* pCameraPos, RwV3d* pScreenPos , float* recip_z);	

	// Message Dialog
	void	SetWaitingDialog( AgcModalWindow* pWindow )		{	m_pWaitingDialog = pWindow;		}
	void	SetMessageDialog( AgcModalWindow* pWindow )		{	m_pMessageDialog = pWindow;		}
	void	SetMessageDialog2( AgcModalWindow* pWindow )	{	m_pMessageDialog2	= pWindow;	}
	void	SetOkCancelDialog( AgcModalWindow* pWindow )	{	m_pOkCancelDialog = pWindow;	}
	void	SetEditOkDialog( AgcModalWindow* pWindow )		{	m_pEditOkDialog = pWindow;	}

	VOID	SetGameServerID(INT32 lNID)						{	m_SocketManager.SetGameServerID(lNID);	}
	VOID	SetMaxPacketBufferSize(INT32 lMaxPacketBufferSize)	{	m_lMaxPacketBufferSize = lMaxPacketBufferSize;	}

	inline	BOOL	IsLCtrlDown()	{ return m_bLCtrl;	}
	inline	BOOL	IsRCtrlDown()	{ return m_bRCtrl;	}
	inline	BOOL	IsCtrlDown()	{ return m_bLCtrl || m_bRCtrl;	}

	inline	BOOL	IsLAltDown()	{ return m_bLAlt;	}
	inline	BOOL	IsRAltDown()	{ return m_bRAlt;	}
	inline	BOOL	IsAltDown()		{ return m_bLAlt || m_bRAlt;	}

	inline	BOOL	IsLShiftDown()	{ return m_bLShift;	}
	inline	BOOL	IsRShiftDown()	{ return m_bRShift;	}
	inline	BOOL	IsShiftDown()	{ return m_bLShift || m_bRShift;	}

	inline BOOL		IsMouseLeftBtnDown()		{ return m_bMouseLeftBtn; }
	inline BOOL		IsMouseRightBtnDown()		{ return m_bMouseRightBtn; }
	inline BOOL		IsMouseMiddleBtnDown()		{ return m_bMouseMiddleBtn; }

public:
	// 마고자 Lua 처리
	struct	stLuaWaitInfo
	{
		enum TYPE
		{
			NONE		,
			WAITTIME	,	// 일정 시간만 기다린다.
			WAITFORKEY	,	// 키입력이 있으면 트리거 됀다.
			WAITTIMEKEY		// 키입력이 있으면 트리거 돼고 , 일정 시간이 지나도 트리거 된다.
		};

		TYPE	eType;
		UINT32	uTime;

		stLuaWaitInfo():eType( NONE ),uTime(0){}

		BOOL	IsWaitTime( UINT32 uCurrentTick )
		{
			switch( eType )
			{
			case NONE:
				return FALSE;
			case WAITTIME:
				if( uCurrentTick < uTime )	return TRUE	;
				else						return FALSE;
			default:
				return FALSE;
			}
		}
	};
	
	std::queue< std::string >	m_queueLuaCommand	;
	stLuaWaitInfo				m_luaWaitInfo		;

	void	LuaInitialize();

	BOOL	Lua_RunScript( std::string strFilename , BOOL bEncrypted );
	BOOL	Lua_IsRunning();

	BOOL	Lua_EnQueue( std::string str = "" );
	BOOL	Lua_PushWait( UINT32 uWaitTime );
	void	Lua_EndWait() { m_luaWaitInfo.eType = stLuaWaitInfo::NONE; }
	void	Lua_Flush_Queue();

	void	ProcessIdleLuaCommand();
	static	void	LuaErrorHandler( const char * pStr );

	virtual	void	LuaErrorMessage( const char * pStr ){}

	BOOL	Debug_Idle(UINT32 ulClockCount);
	BOOL	Debug_Idle2(UINT32 ulClockCount);

	bool	IsCheckSlowIdle() { return m_uSlowIdleIndicator ? true : false; }
	UINT32	m_uSlowIdleIndicator;	// 이 틱 값이 있을경우
			// 위의 디버그 아이들 ( 로그 기능 포함 ) 이 실행이 되며
			// 이 값보다 큰 틱이 발생할경우
			// SlowIdle.txt 에 로그로 남는다.
};

// 엔진 인스턴스는 하나만 생성해야한다.
// 처음 생성돼는 인스턴스 포인터를 저장하여 각종 콜백에 사용한다.
// 고로 여러개 생성하면 뻗는다. - _-+
extern AgcEngine *	g_pEngine;

// 관리용..
RsEventStatus AppEventHandler(RsEvent event, PVOID param);

#endif // !defined(AFX_AGCENGINE_H__8BCC5208_F2F4_4E6C_B3DE_B52CB449BB5C__INCLUDED_)
