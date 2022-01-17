// 마고자 (2002-06-07 오전 10:46:32) : 렌더웨어 라이브러리 MFC 포팅 클래스.

// 설치과정

// 1) WinApp에 렌더웨어 클래스 등록
//		CRenderware m_Renderware;
// 2) WinApp 에 OnIdle 오버라이드후 m_Renderware.Idle() 호출
// 3) 타겟윈도우에 PreTraslateMessage 에 엔진 연결 추가
//      return theApp.m_Renderware.MessageProc( message , wParam , lParam );

#if !defined(_RW_FOR_MFC_)
#define _RW_FOR_MFC_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <rwcore.h>
#include <skeleton.h>

#define RWUTIL_MAX_VIDEOMODE					50

//using namespace RenderWare;
using namespace std;

class CRenderWare  
{
public:     //Construction/destruction
	CRenderWare();
	
protected:  //Representation
	bool	m_bInitialised;     //If the library has been initialised
	HWND	m_hWnd;             //Member window which initialised to
	RwBool	m_bForegroundApp;
	RwBool	m_bValidDelta;

	// Global
    RwV2d	m_lastMousePos	;

public:     //Public Methods
	bool	Initialize   ( HWND hWnd  );
	void	Destroy      (void);
	
	bool	SetCameraViewSize (int dx, int dy);
	
	void	Render	( void );
	BOOL	Idle	( void );
	BOOL	MessageProc	( MSG* pMsg );	// 다겟윈도우의 윈도우 프로시져에 붙인다.
	
protected:     //Protected interface

public:
	// 이벤트들.
	RsEventStatus	OnEventHandler			( RsEvent event , void *param ); // 메시지를 처리하여 모듈들에게 넘겨줌
	RsEventStatus	OnKeyboardEventHandler	( RsEvent event , void *param ); // 메시지를 처리하여 모듈들에게 넘겨줌
	RsEventStatus	OnMouseEventHandler		( RsEvent event , void *param ); // 메시지를 처리하여 모듈들에게 넘겨줌
};

#endif // !defined(_RW_FOR_MFC_)
