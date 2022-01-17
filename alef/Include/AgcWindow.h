#if !defined _AGCWINDOW_H_
#define _AGCWINDOW_H_

#include <list>

using namespace std;

class	AgcWindowProperty
{
public:
	BOOL	bUseInput	;	// 입력 수용
	BOOL	bMovable	;	// 이동 가능
	BOOL	bModal		;	// 이 윈도우 에만 키&마우스 입력이 들어간다.
							// 하위 차일드는 종속됨;
	BOOL	bTopmost	;	// 차일드 내에서 탑 모스트 기능

	BOOL	bVisible	;	// 보이는지 안보이는지 설정.

	AgcWindowProperty() : bUseInput(TRUE), bMovable(FALSE), bModal(FALSE), bTopmost( FALSE ), bVisible(TRUE)		{		}
};

class AgcWindow;
struct	AgcWindowNode
{
	AgcWindowNode*		next;
	AgcWindowNode*		prev;

	AgcWindow*			pWindow;
};

struct	AgcWindowList
{
	AgcWindowNode*		head;
	AgcWindowNode*		tail;
};

class AgcWindow;
class CMessageMng
{
	// Message Structure
	struct	Message
	{
		Message( INT32 id, PVOID param1, PVOID param2 ) : message(id), pParam1(param1), pParam2(param2)		{		}

		INT32		message;
		PVOID		pParam1;
		PVOID		pParam2;
	};
	typedef list<Message>			MessageList;
	typedef MessageList::iterator	MessageListItr;

public:
	CMessageMng( AgcWindow* pAgcWindow );
	~CMessageMng();

	void	MessagePush( INT32 id, PVOID param1, PVOID param2 );
	void	MessageUpdate();

public:
	static const INT32	s_MessageMax = 100;

protected:
	AgcWindow*		m_pAgcWindow;	//상호참조이다 ㅜ.ㅜ
	MessageList		m_listMessage;
};

class AcUIBase;
class AgcmUIManager2;
class AgcmTargeting;
class AgcWindow : public CMessageMng
{
public:
	friend class AgcEngine;
	friend class AgcmUIManager2;
	friend class AgcmUIControl;
	friend class AgcmTargeting;

	enum
	{
		// 메시지 인덱스			// pParam				// 설명.
		MESSAGE_INIT			,	// void
		MESSAGE_CLOSE			,	// void
		MESSAGE_PRERENDER		,	// RwRaster *
		MESSAGE_RENDER			,	// RwRaster *
		MESSAGE_POSTRENDER		,	// RwRaster *
		MESSAGE_KEYDOWN			,	// RsKeyStatus *
		MESSAGE_KEYUP			,	// RsKeyStatus *
		MESSAGE_CHAR			,	// char
		MESSAGE_IMECOMPOSING	,	//
		MESSAGE_LBUTTONDOWN		,	// RsMouseStatus *
		MESSAGE_LBUTTONDBLCLK	,	// RsMouseStatus *
		MESSAGE_LBUTTONUP		,	// RsMouseStatus *
		MESSAGE_RBUTTONDOWN		,	// RsMouseStatus *
		MESSAGE_RBUTTONDBLCLK	,	// RsMouseStatus *
		MESSAGE_MBUTTONUP		,	// RsMouseStatus *
		MESSAGE_MBUTTONDOWN		,	// RsMouseStatus *
		MESSAGE_MBUTTONDBLCLK	,	// RsMouseStatus *
		MESSAGE_RBUTTONUP		,	// RsMouseStatus *		
		MESSAGE_MOUSEMOVE		,	// RsMouseStatus *
		MESSAGE_MOUSEWHEEL		,	// BOOL
		MESSAGE_IDLE			,	// 

		MESSAGE_SETFOCUS		,
		MESSAGE_KILLFOCUS		,

		MESSAGE_MOUSEACTIVATE	,	// 마우스가 윈도우 영역내로 들어온다.
		MESSAGE_MOUSELEAVE		,	// 마우스가 윈도우 영역을 벗어난다.
		
		MESSAGE_COMMAND			,	//  컨트롤 커맨드 메시지.. 인자는 마음대로.;;
		MESSAGE_DRAG_DROP		,	//  Drag를 놨다는 메세지 

		MESSAGE_IME_COMPOSING	,	// IME 처리..

		MESSAGE_POSTINIT		,	// Init을 수행한후 넘어옴..Init가 강제로 막힌것도 무조건 열릴때 체크함..

		MESSAGE_EDIT_ACTIVE		,	// ActiveEdit가 될때
		MESSAGE_EDIT_DEACTIVE	,	// ActiveEdit에서 해제될때

		MESSAGE_COUNT
	};

	INT32			m_nType;				// 컨트롤의 타입..(0일경우 Control이 아니다)

	BOOL			m_bUIWindow			;	// AcUIBase를 상속받은 UI Window인가?

	BOOL			m_bOpened			;
	BOOL			m_bUseDrag			;	// 마우스로 윈도우를 드레그 할 수 있는지 여부

	AgcWindow*		pParent				;	// 상위 윈도우의 포인터 ( 최상위의 경우 NULL );
	AgcWindow*		m_pFocusedChild		;	// 하위 윈도우중 어느녀석이 포커스를 가지고 있는지 점검.

	// AuList구조에서 자체 list로 튜닝..(2004.10.27 gemani)
	// Windows list handling
	AgcWindowList	m_listChild			;	// 하위 윈도우의 포인터 ( 리스트 );
	
	AgcWindowList	m_listSetTop		;	// Set Top을 설정하기 위한 스택.
	AgcWindowList	m_listAdd			;	// 추가할 큐..

	INT32			m_lControlID		;	// ID!!! 윈도우가 필요할경우 따로 등록한다.

	INT16			x, y, w, h;
	INT32			m_lUserData_1;		// User가 자체적으로 사용한다. 

	// Window Movable을 위한..
	RwV2d			m_v2dDragPoint;		// Window의 포지션에서 클릭 위치 상대 좌표 
	BOOL			m_bMoveMovableWindow;		// Window를 옮기고 있는 중

	BOOL			m_bDeleteThis;
	
	BOOL			m_bVisibleChildWindowForShink		;		// Shink시 Visible된 Child였는지의 정보를 담는 임시공간 - default : FALSE

	BOOL			m_bActiveEdit			;		// 이 Window가 Active된 Edit 인가?

	BOOL			m_bUseModalMessage		;		// 다른 윈도우가 Modal로 떠있어도 메세지를 받아야 하는가?

	ApBase*			m_pcsOwnerBase;

	AgcWindowProperty	m_Property;	// 윈도우 속성 저장.

protected:
	bool	m_bNeedWindowListUpdate	;

	void	MessageQueueProcessing();

	void	WindowListUpdate();
	void	WindowListUnlink(AgcWindowList*		pList,AgcWindow*	pRemoveWindow);
	void	WindowListLinkTail(AgcWindowList*	pList,AgcWindow*	pAddWindow);
	void	WindowListLinkBefore(AgcWindowList*	pList,AgcWindow*	pBaseWindow, AgcWindow*	pAddWindow);
	
public:
	// Operations
	AgcWindow();
	virtual ~AgcWindow();

	virtual	BOOL OnInit()							{	return TRUE;	}	// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
	virtual BOOL OnPostInit()						{	return TRUE;	}
	virtual	void OnClose()							{					}
	virtual BOOL OnIdle( UINT32 ulClockCount )		{	return TRUE;	}

	virtual	void OnPreRender	( RwRaster *raster	)	{					}
	virtual	void OnRender		( RwRaster *raster	)	{					}
	virtual	void OnPostRender	( RwRaster *raster	)	{					}
	virtual	void OnWindowRender	()						{					}

	virtual	BOOL OnKeyDown		( RsKeyStatus *ks	)		{	return FALSE;	}
	virtual	BOOL OnKeyUp		( RsKeyStatus *ks	)		{	return FALSE;	}
	virtual	BOOL OnLButtonDown	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnLButtonDblClk( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnLButtonUp	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnRButtonDown	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnRButtonDblClk( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMButtonUp	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMButtonDown	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMButtonDblClk( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnRButtonUp	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMouseMove	( RsMouseStatus *ms	)		{	return FALSE;	}
	virtual	BOOL OnMouseWheel	( INT32	lDelta		)		{	return FALSE;	}

	virtual	void OnSetFocus		(					)		{		}
	virtual	void OnKillFocus	(					)		{		}
	virtual void OnMouseActivate(					)		{		}
	virtual void OnMouseLeave	(					)		{		}

	virtual BOOL OnCommand		( INT32	nID , PVOID pParam	){ return TRUE; }
	virtual BOOL OnDragDrop		( PVOID pParam1 , PVOID pParam2  ){ return TRUE;	}

	virtual	BOOL OnChar			( char * pChar		, UINT lParam ) { return FALSE; }
	virtual	BOOL OnIMEComposing	( char * composing	, UINT lParam ) { return FALSE; }
	virtual BOOL OnIMEReading	(								  ) { return FALSE; }	// reading window 그리기
	virtual BOOL OnIMEHideReading(								  ) { return FALSE; }	// reading window 감추기
	virtual BOOL OnIMECandidate	(								  ) { return FALSE; }	// candidate window 그리기
	virtual BOOL OnIMEHideCandidate(							  ) { return FALSE; }	// candidate window 감추기

	virtual void OnMoveWindow()		{		}
	virtual	void OnPreRemoveThis()	{		}	// 페어런트 리스트에서 실제로 빠지는 시점의 버쳘함수.

	virtual void OnEditActive()		{		}
	virtual void OnEditDeactive()	{		}

public:	
	virtual void WindowRender();					// Window 를 실제로 Render하는 함수 
	virtual	BOOL HitTest( INT32 x , INT32 y	);	// 해당 포인트가 이 모듈에 속해있는지 점검. 창UI에서 중요함. x,y 는 무조건 Screen 좌표가 되겠음.

	// 다음은 키보드/마우스의 입력을 각 차일드 윈도우에 뿌려주기 위해서 , 엔진에서 사용하는 함수.
	// 따로 직접 컨트롤 하는 일은 없을듯?..
	virtual	RsEventStatus	UserInputMessageProc		( RsEvent event , PVOID param									); // 키보드&마우스 메시지 받음
	virtual	BOOL			WindowControlMessageProc	( INT32 message , PVOID pParam1 = NULL , PVOID pParam2	= NULL	);
	virtual BOOL			PreTranslateInputMessage	( RsEvent event , PVOID param									){ return FALSE; }
	virtual void			SetDialogMSGFuncClass( DIALOG_MSG_FUNC pfDialogMsgFunc, PVOID pClass );		// 모달리스 다이얼로그 메시지를 처리하기 위해서 사용한다. 2004.01.12. steeple

public:
	BOOL		Close();

	BOOL			IsFocused();
	virtual void	SetFocus();
	void			KillFocus();
	AgcWindow*		GetFocusedWindow();
	
	void		ClientToScreen( RsMouseStatus &ms );
	void		ScreenToClient( RsMouseStatus &ms );
	void		ClientToScreen( int *px , int *py );
	void		ScreenToClient( int *px , int *py );

	void		MousePositionToUVCoordinate( RwV2d* pPos , FLOAT* pU , FLOAT* pV );

	// 차일드 윈도우 붙이는 함수.
	BOOL		AddChild( AgcWindow * pWindow	, INT32 nid = 0			, BOOL bInit = TRUE );
	BOOL		DeleteChild( AgcWindow * pWindow	, BOOL bClose = TRUE	, BOOL bDeleteMemory = FALSE );
	BOOL		DeleteChild( INT32			nid		, BOOL bClose = TRUE	, BOOL bDeleteMemory = FALSE );
	// 여기에 사용돼는 포인터들은 윈도우 자체가 딜리트 해버린다.
	// 따라서 , 차일드로 들어가는 윈도우의 경우는 윈도우 포인터를 new로 생성하여
	// 페어런트에 넘겨준 후 , 신경을 쓰지 않아도 됀다.
	// 차일드 메시지 않아서는 호출해서는 안됀다.

	void		MoveWindow( INT32 x , INT32 y , INT32 w, INT32 h );
	BOOL		MovableOn( RsMouseStatus *ms );
	BOOL		MovableOff();
	
	// 윈도우를 엔진자체에서 렌더를 하지 않음. 입력도 받지 않음.
	void		ShowWindow( BOOL bVisible = TRUE )	{	m_Property.bVisible	= bVisible;	}
	
	AgcWindow*	WindowFromPoint( INT32 x , INT32 y, BOOL bCheckUseInput = TRUE );

	BOOL		PostMessage	( INT32 message , PVOID pParam1 = NULL , PVOID pParam2	= NULL	);
	BOOL		SendMessage	( INT32 message , PVOID pParam1 = NULL , PVOID pParam2	= NULL	);

	void		SetUseDrag( BOOL bUse )		{	m_bUseDrag = bUse;	}
	BOOL		BeDraged()					{	return m_bUseDrag;	}

	INT32		GetControlID()				{	return m_lControlID;	}
	
	BOOL		SetModal();
	BOOL		ReleaseModal();
	BOOL		BeInWindowRect( INT32 lScreenX, INT32 lScreenY );			// 단순히 Window안에 들어있는가를 판단해줌 - Visible여부와 상관없음
	
	BOOL		GetActiveEdit()		{	return m_bActiveEdit;	}
	ApBase*		GetOwnerBase()		{	return m_pcsOwnerBase;	}
	BOOL		SetOwnerBase(ApBase *pcsOwner)	{	return (m_pcsOwnerBase = pcsOwner) ? TRUE : FALSE;	}

	INT32		GetDepth();
	BOOL		ReculsiveWindowControlMessageProc( INT32 message, PVOID pParam1 = NULL, PVOID pParam2 = NULL );		// 메시지를 하위 윈도우 모두에게 전송함.

protected:
	void GetDepthRecurse(AgcWindow* pCurWindow, INT32* pDepth);
};

#define		AGCMODALWINDOW_MESSAGE_MAX			256

class AgcModalWindow : public AgcWindow
{
public:
	AgcModalWindow();

	virtual	BOOL	OnPreDoModal()					{	return TRUE;	}
	virtual	void	SetMessage( char* szMessage	);

	INT32			DoModal( AgcWindow* pParent );
	void			EndDialog( INT32 nRetValue );

protected:
	INT32		m_nModalReturnValue;
	CHAR		m_szMessage[AGCMODALWINDOW_MESSAGE_MAX];
	INT32		m_lModalCount;								// 현재 이 Modal Dialog가 Modal 된 Count (한 dialog가 여러번 DoModal() 되었을 경우 처리	

public :
	INT32			GetReturnValue( void )	{ return m_nModalReturnValue; }
};

#endif