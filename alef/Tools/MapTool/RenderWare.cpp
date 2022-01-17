// RenderWare.cpp: implementation of the CRenderWare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(disable:4786) //disable linker key > 255 chars long (for stl map)
#include <map>
#include "RenderWare.h"

#include <rtpng.h>
#include <rtbmp.h>
#include <rphanim.h> 
#include <rpskin.h>

#include <win.h>
#include <skeleton.h>
#include <menu.h>
#include <camera.h>
#include <AgcEngine.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int GetKeyTableEnglish( int index );

extern psGlobalType PsGlobal;

namespace
{
	const float EPSILON     = 0.00001f;
	
	const float FARCLIP     = 5000.0f;
	const float NEARCLIP    = 0.01f;
	const float STARTZOOM   = .5;        //Default zoom
	const float CAMERAVIEWX = 0.01f;
	const float CAMERAVIEWY = 0.01f;
	const float CAMERAZ     = -30.0f;   //Camera offset on the world's z axis
	
	const float ANIM_ADDTIME= 1/25.0f;
	
	const long  ARENA_SIZE  = 32000000; //bytes
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
RwInt32	winTranslateKey(WPARAM wParam __RWUNUSED__, LPARAM lParam);

CRenderWare::CRenderWare():
	m_bInitialised	(false),
	m_hWnd			(0x0),
	m_bForegroundApp( TRUE ),
	m_bValidDelta	( FALSE )
{
	
}

//////////////////////////////////////////////////////////////////////
// Member functions
//////////////////////////////////////////////////////////////////////

//Initialises RenderWare with the passed window
bool CRenderWare::Initialize(HWND hWnd)
{
	//Initialise core RenderWare library
	if ( hWnd == 0x0)
	{
		OutputDebugString("RenderWare::Initialise: Invalid window specified.\n");
		return false;
	}
	
	m_hWnd = hWnd;

	PsGlobal.fullScreen	= false;
	PsGlobal.window		= m_hWnd;
	PsGlobal.lastMousePos.x	= 0.0f;
	PsGlobal.lastMousePos.y	= 0.0f;

	//Initialise the RenderWare engine
	if( !( RwEngineInit( NULL , NULL, 0x0, ARENA_SIZE) ) )
	{
		OutputDebugString("RenderWare::Initialise: RenderWare Engine failed to initialise.\n");
		return false;
	}

	// 야호..;;
	m_bInitialised = true;

	OnEventHandler( rsINITIALIZE , NULL );

	RsGlobal.maximumHeight	= 0;
	RsGlobal.maximumWidth	= 0;

	OnEventHandler(rsINITDEBUG, NULL);
    if( OnEventHandler(rsPLUGINATTACH, NULL) == rsEVENTERROR )
    {
		RwEngineClose();
		RwEngineTerm();
		m_bInitialised = false;
		return (FALSE);
	}

	// 잇풋 디바이스는 MFC에선 사용하지 않는다.
    // OnEventHandler(rsINPUTDEVICEATTACH, NULL);	
	
	//Open the library
	RwEngineOpenParams openParams;
	openParams.displayID = m_hWnd;
	if ( !(RwEngineOpen(&openParams)) )
	{
		OutputDebugString("\nRenderWare::Initialise: Failed to open library.\n");
		RwEngineTerm();
		m_bInitialised = false;
		return false;
	}
	
	// 창모드 설정..
	RwEngineSetSubSystem( 0 );
	RwEngineSetVideoMode( 0 );

	//Start the RenderWare engine
	if ( !( RwEngineStart() ) )
	{
		OutputDebugString("\nRenderWare::Initialise: Failed to start engine.\n");
		RwEngineClose();
		RwEngineTerm();
		m_bInitialised = false;
		return false;
	}

	//Format and output version information
	char sVersion[128];
	RwInt32 dwVer = RwEngineGetVersion();
	sprintf(sVersion,"RenderWare v%x.%x.%x initialised.\n", (dwVer & 0xffff0000) >> 16,
		(dwVer & 0x0000f000) >> 12,
		(dwVer & 0x00000f00) >> 8 );
	
	OutputDebugString(sVersion);


    OnEventHandler( rsREGISTERIMAGELOADER , NULL);

	OnEventHandler( rsRWINITIALIZE , hWnd);

	return true;
}

//---------------------------------------------------------------------
//Changes the viewport size to which the scene is rendered in the window
bool CRenderWare::SetCameraViewSize(int dx, int dy)
{
	RwRect r;
	
	r.x = 0;
	r.y = 0;
	r.w = dx;
	r.h = dy;
	
	//#ifdef RWMOUSE
	//        ClipMouseToWindow(window);
	//#endif  /* RWMOUSE */
	
	if ( m_bInitialised && r.h > 0 && r.w > 0)
	{
		OnEventHandler(rsCAMERASIZE, &r);
		
		if ( r.w != dx && r.h != dy )
		{
			//WINDOWPLACEMENT     wp;
	
			/* stop re-sizing */
			ReleaseCapture();
		}
	}
	
	return true;
}
//---------------------------------------------------------------------

void CRenderWare::Destroy( void)
{
	#ifndef _DEBUG
	try 
	#endif
	{

		// OnDestory
		OnEventHandler( rsRWTERMINATE , NULL );
		
		//Shutdown RenderWare if initialised
		if (m_bInitialised)
		{
			RwEngineStop();
			RwEngineClose();
			RwEngineTerm();
			OutputDebugString("RenderWare exited gracefully.\n");
		}
		
		//Catch everything just in case
	} 
	#ifndef _DEBUG
	catch (...) 
	{
		RwError	error;
		VERIFY( NULL != RwErrorGet( & error ) );
		OutputDebugString("CRenderWare::Destroy: Exception encountered on destruction.\n");
		TRACE( "Error plugin = %d , Error Code = %d\n" , error.pluginID , error.errorCode );
	}
	#endif
	
	return;
}

BOOL	CRenderWare::Idle	( void )
{
	// 라이들 들어가기전에 몇가지 처리.
	return OnEventHandler( rsIDLE , ( void * ) GetTickCount() );
}

//---------------------------------------------------------------------
//Renders the current world to the window specified in initilisation
void CRenderWare::Render()
{

}

BOOL			CRenderWare::MessageProc	( MSG* pMsg )
{
	if( !m_bInitialised ) return FALSE;

	// 메시지 처리!
	HWND	window	= pMsg->hwnd	;
	UINT	message	= pMsg->message	;
	WPARAM	wParam	= pMsg->wParam	;
	LPARAM	lParam	= pMsg->lParam	;

    POINTS points;
    static BOOL noMemory = FALSE;

    switch( message )
    {
    case WM_CREATE:
		{
			return 0L;
		}

	case WM_SIZE:
		{
			RwRect r;
			
			r.x = 0;
			r.y = 0;
			r.w = LOWORD(lParam);
			r.h = HIWORD(lParam);
			
			//#ifdef RWMOUSE
			//        ClipMouseToWindow(window);
			//#endif  /* RWMOUSE */


			if( r.w )
				OnEventHandler(rsACTIVATE, (void *)0);
			else
				OnEventHandler(rsACTIVATE, (void *)1);

		
			if (m_bInitialised && r.h > 0 && r.w > 0)
			{
				OnEventHandler(rsCAMERASIZE, &r);
				
				if (r.w != LOWORD(lParam) && r.h != HIWORD(lParam))
				{
					WINDOWPLACEMENT     wp;
					
					/* failed to create window of required size */
					noMemory = TRUE;
					
					/* stop re-sizing */
					ReleaseCapture();
					
					/* handle maximised window */
					GetWindowPlacement(window, &wp);
					if (wp.showCmd == SW_SHOWMAXIMIZED)
					{
						SendMessage(window, WM_WINDOWPOSCHANGED, 0, 0);
					}
				}
				else
				{
					noMemory = FALSE;
				}
				
			}
			
			return 0L;
		}
	case WM_SETFOCUS:
		{
			OnEventHandler(rsACTIVATE, (void *)0);
			return 0L;
		}
	case WM_KILLFOCUS:
		{
			OnEventHandler(rsACTIVATE, (void *)2);
			return 0L;
		}		
	case WM_SIZING:
		{
		/* 
		* Handle event to ensure window contents are displayed during re-size
		* as this can be disabled by the user, then if there is not enough 
		* memory things don't work.
			*/
			RECT               *newPos = (LPRECT) lParam;
			RECT                rect;
			
			/* redraw window */
			if (m_bInitialised)
			{
				OnEventHandler(rsIDLE, NULL);
			}
			
			/* Manually resize window */
			rect.left = rect.top = 0;
			rect.bottom = newPos->bottom - newPos->top;
			rect.right = newPos->right - newPos->left;
			
			SetWindowPos(window, HWND_TOP, rect.left, rect.top,
				(rect.right - rect.left),
				(rect.bottom - rect.top), SWP_NOMOVE);
			
			return 0L;
		}
		
	case WM_LBUTTONDOWN:
		{
			RsMouseStatus ms;
			
			points		= MAKEPOINTS( lParam )					;
			ms.pos.x	= points.x								;
			ms.pos.y	= points.y								;
			ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
			ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;
			
			SetCapture(window);
			
			OnMouseEventHandler(rsLEFTBUTTONDOWN, &ms);
			
			return 0L;
		}
	case WM_LBUTTONDBLCLK:
		{
			RsMouseStatus ms;
			
			points		= MAKEPOINTS( lParam )					;
			ms.pos.x	= points.x								;
			ms.pos.y	= points.y								;
			ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
			ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;
			
			SetCapture(window);
			
			OnMouseEventHandler(rsLEFTBUTTONDBLCLK, &ms);
			
			return 0L;
		}
	case WM_RBUTTONDBLCLK:
		{
			RsMouseStatus ms;
			
			points		= MAKEPOINTS( lParam )					;
			ms.pos.x	= points.x								;
			ms.pos.y	= points.y								;
			ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
			ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;
			
			SetCapture(window);
			
			OnMouseEventHandler(rsRIGHTBUTTONDBLCLK, &ms);
			
			return 0L;
		}

	case WM_RBUTTONDOWN:
		{
			RsMouseStatus ms;
			
			points		= MAKEPOINTS( lParam )					;
			ms.pos.x	= points.x								;
			ms.pos.y	= points.y								;
			ms.shift	= ( wParam & MK_SHIFT	) ? TRUE : FALSE;
			ms.control	= ( wParam & MK_CONTROL	) ? TRUE : FALSE;
			
			SetCapture(window);
			
			OnMouseEventHandler(rsRIGHTBUTTONDOWN, &ms);
			
			return 0L;
		}
		
	case WM_MOUSEWHEEL:
		{
			RwBool forward = FALSE;
			
			forward = ((short)HIWORD(wParam) < 0) ? FALSE : TRUE;

			RsMouseStatus	ms;
			ms.wheel = forward;
			OnMouseEventHandler(rsMOUSEWHEELMOVE, (void *)&ms);
			
			return 0L;
		}
		
	case WM_MOUSEMOVE:
		{
			if (m_bForegroundApp)
			{
				points = MAKEPOINTS(lParam);
				
				if (m_bValidDelta)
				{
					RsMouseStatus ms;
					
					ms.delta.x = points.x - m_lastMousePos.x;
					ms.delta.y = points.y - m_lastMousePos.y;
					ms.pos.x = points.x;
					ms.pos.y = points.y;
					/****** Parn님 작업 시작 *******/
					ms.shift = (wParam & MK_SHIFT) ? TRUE : FALSE;
					ms.control = (wParam & MK_CONTROL) ? TRUE : FALSE;
					/****** Parn님 작업 끝 *******/
					
					OnMouseEventHandler(rsMOUSEMOVE, &ms);
				}
				else
				{
					m_bValidDelta = TRUE;
				}
				
				m_lastMousePos.x = points.x;
				m_lastMousePos.y = points.y;
			}
			
			return 0L;
		}
		
	case WM_LBUTTONUP:
		{
			ReleaseCapture();

			RsMouseStatus ms;
			
			points		= MAKEPOINTS(lParam)					;
			ms.pos.x	= points.x								;
			ms.pos.y	= points.y								;
			ms.shift	= (wParam & MK_SHIFT	) ? TRUE : FALSE;
			ms.control	= (wParam & MK_CONTROL	) ? TRUE : FALSE;
			
			OnMouseEventHandler( rsLEFTBUTTONUP, &ms );
			
			return 0L;
		}
		
	case WM_RBUTTONUP:
		{
			ReleaseCapture();
			
			RsMouseStatus ms;
			
			points		= MAKEPOINTS(lParam)					;
			ms.pos.x	= points.x								;
			ms.pos.y	= points.y								;
			ms.shift	= (wParam & MK_SHIFT	) ? TRUE : FALSE;
			ms.control	= (wParam & MK_CONTROL	) ? TRUE : FALSE;

			OnMouseEventHandler( rsRIGHTBUTTONUP, &ms );
			
			return 0L;
		}
		
	case WM_KEYDOWN:
		{
			RsKeyStatus ks;
			
			if (!(lParam & 0x40000000)) /* ignore repeat events */
			{
				ks.keyScanCode = winTranslateKey(wParam, lParam);
				ks.keyCharCode = GetKeyTableEnglish( ks.keyScanCode );
				OnKeyboardEventHandler(rsKEYDOWN, &ks);
				
				if (ks.keyCharCode == rsESC)
				{
					/* Send a quit message - this allows app to do stuff */
					OnEventHandler(rsQUITAPP, NULL);
				}
			}
			
			return 0L;
		}
		
	case WM_KEYUP:
		{
			RsKeyStatus ks;
			
			ks.keyScanCode = winTranslateKey(wParam, lParam);
			ks.keyCharCode = GetKeyTableEnglish( ks.keyScanCode );
			
			OnKeyboardEventHandler(rsKEYUP, &ks);
			
			return 0L;
		}
		
	case WM_SYSKEYDOWN:
		{
			RsKeyStatus ks;
			
			if (!(lParam & 0x40000000)) /* ignore repeat events */
			{
				ks.keyScanCode = winTranslateKey(wParam, lParam);
				ks.keyCharCode = GetKeyTableEnglish( ks.keyScanCode );
				
				OnKeyboardEventHandler(rsKEYDOWN, &ks);
			}
			
			return 0L;
		}
		
	case WM_SYSKEYUP:
		{
			RsKeyStatus ks;
			
			ks.keyScanCode = winTranslateKey(wParam, lParam);
			ks.keyCharCode = GetKeyTableEnglish( ks.keyScanCode );
			
			OnKeyboardEventHandler(rsKEYUP, &ks);
			
			return 0L;
		}
		
	case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
#ifdef RWMOUSE
				ClipCursor(NULL);
#endif
				SetTimer(window, 1, 20, NULL);
				m_bForegroundApp = FALSE;
				
				OnEventHandler(rsACTIVATE, (void *)FALSE);
			}
			else
			{
#ifdef RWMOUSE
				ClipMouseToWindow(window);
#endif
				KillTimer(window, 1);
				m_bForegroundApp = TRUE;
				OnEventHandler(rsACTIVATE, (void *)TRUE);
			}
			
			return 0L;
		}
		
	case WM_TIMER:
		{
			if (m_bInitialised)
			{
				OnEventHandler(rsIDLE, NULL);
			}
			
			return 0L;
		}
		
	case WM_CLOSE:
	case WM_DESTROY:
		{
		/*
		* Quit message handling.
			*/

			ClipCursor(NULL);

			Destroy();
			
			PostQuitMessage(0);
			
			return 0L;
		}
	}
		
	return FALSE;// 윈도우 자체에서 처리하란 명령.
}

RsEventStatus	CRenderWare::OnEventHandler			( RsEvent event , void *param )
{
	if( !m_bInitialised ) return rsEVENTNOTPROCESSED;
	else 
		return g_pEngine->OnMessageSink( event , param );
	
}

RsEventStatus	CRenderWare::OnKeyboardEventHandler	( RsEvent event , void *param )
{
	if( !m_bInitialised ) return rsEVENTNOTPROCESSED;
	else 
		return g_pEngine->KeyboardHandler( event , param );
}

RsEventStatus	CRenderWare::OnMouseEventHandler	( RsEvent event , void *param )
{
	if( !m_bInitialised ) return rsEVENTNOTPROCESSED;
	else 
		return g_pEngine->MouseHandler( event , param );
}
