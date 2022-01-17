// RenderWare.cpp: implementation of the CRenderWare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "RenderWare.h"

#include <rtpng.h>
#include <rtbmp.h>
#include <rtanim.h>
#include <rphanim.h> 
#include <rpskin.h>
#include <rtfsyst.h>


#include "Camera.h"

#include <win.h>

#include "MyEngine.h"

#ifndef _D3D9_H_
#define _D3D9_H_
#endif


namespace
{
	const float EPSILON     = 0.00001f;
	
	const float FARCLIP     = 15000.0f;
	const float NEARCLIP    = 0.01f;
	const float STARTZOOM   = .5;        //Default zoom
	const float CAMERAVIEWX = 0.01f;
	const float CAMERAVIEWY = 0.01f;
	const float CAMERAZ     = -30.0f;   //Camera offset on the world's z axis
	
	const float ANIM_ADDTIME= 1/25.0f;
	
	const long  ARENA_SIZE  = 32000000; //bytes

	/*
	필요하면 AgcmPostFX 참조해서 하자..
	static	rwD3D9DeviceRestoreCallBack	CBPrevRestore = NULL;
	static	rwD3D9DeviceReleaseCallBack	CBPrevRelease = NULL;

	void CBRestorDevice()
	{
	};
	void CBReleaseDevice()
	{
	};/**/
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderWare::CRenderWare()
: m_pd3dDevice(NULL)
, m_pd3dLine(NULL)
, m_bInitialised(false)
{	
}

//////////////////////////////////////////////////////////////////////
// Member functions
//////////////////////////////////////////////////////////////////////


//Initialises RenderWare with the passed window
bool CRenderWare::Initialise(HWND hWnd)
{
	//Initialise core RenderWare library
	if ( hWnd == 0x0)
	{
		OutputDebugString("RenderWare::Initialise: Invalid window specified.\n");
		return false;
	}

	PsGlobal.fullScreen	= false;
	PsGlobal.window		= hWnd;
	PsGlobal.lastMousePos.x	= 
	PsGlobal.lastMousePos.y	= 0.f;
	
	//Initialise the RenderWare engine
	if( !( RwEngineInit(0x0, NULL, 0x0, ARENA_SIZE) ) )
	{
		OutputDebugString("RenderWare::Initialise: RenderWare Engine failed to initialise.\n");
		return false;
	}

	m_bInitialised = true;
	OnEventHandler( rsINITDEBUG, NULL );
	if( OnEventHandler( rsPLUGINATTACH, NULL ) == rsEVENTERROR )
	{
		RwEngineClose();
		RwEngineTerm();
		m_bInitialised = false;
		return false;
	}
	
	//Open the library
	RwEngineOpenParams openParams;
	openParams.displayID = hWnd;
	if ( !(RwEngineOpen(&openParams)) )
	{
		OutputDebugString("\nRenderWare::Initialise: Failed to open library.\n");
		RwEngineTerm();
		m_bInitialised = false;
		return false;
	}
	
	//Start the RenderWare engine
	if ( !( RwEngineStart() ) )
	{
		OutputDebugString("\nRenderWare::Initialise: Failed to start engine.\n");
		RwEngineClose();
		RwEngineTerm();
		m_bInitialised = false;
		return false;
	}
	
	m_pd3dDevice	= (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
	g_MyEngine.bSetDevice( m_pd3dDevice );
	D3DCAPS9*	lpDevCaps	= (D3DCAPS9*)RwD3D9GetCaps();

	D3DXCreateLine( m_pd3dDevice, &m_pd3dLine );


	m_bInitialised	= true;

	RwEngineSetSubSystem(0);
	
	//Format and output version information
	char sVersion[128];
	RwInt32 dwVer = RwEngineGetVersion();
	sprintf(sVersion,"RenderWare v%x.%x.%x initialised.\n", (dwVer & 0xffff0000) >> 16,
		(dwVer & 0x0000f000) >> 12,
		(dwVer & 0x00000f00) >> 8 );
	
	OnEventHandler(rsREGISTERIMAGELOADER, NULL);

	OnEventHandler(rsRWINITIALIZE, NULL);
	//---->
	CCamera::bGetInst().bGetPtrAxis()->bSetupSize( 50.f, 40.f, 4.f );
	CCamera::bGetInst().bGetPtrAxis()->bSetupSemi( TRUE );
	//<----

	return true;
}


//---------------------------------------------------------------------

void CRenderWare::Destroy( void)
{
	try {       
		OnEventHandler( rsRWTERMINATE, NULL );
		
		//Shutdown RenderWare if initialised
		if (m_bInitialised)
		{
			if( m_pd3dLine )
				m_pd3dLine->Release();
			/* Close the file system manager */
			RtFSManagerClose();
			
			RwEngineStop();
			RwEngineClose();
			RwEngineTerm();
			OutputDebugString("RenderWare exited gracefully.\n");
		}
		
		//Catch everything just in case
	} 
	catch (...) 
	{
		OutputDebugString("CRenderWare::Destroy: Exception encountered on destruction.\n");
	}

	
	m_bInitialised	= false;	
	return;
}



//---------------------------------------------------------------------
//Renders the current world to the window specified in initilisation
void CRenderWare::Render()
{
	return;
}

//////////////////////////////////////////////////////////////////////
// Camera functions
//////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
//Changes the viewport size to which the scene is rendered in the window
bool CRenderWare::SetCameraViewSize(int dx, int dy)
{
	RwRect r = { 0, 0, dx, dy };
	if ( m_bInitialised && r.h > 0 && r.w > 0)
	{
		if( m_pd3dLine )
			m_pd3dLine->OnLostDevice();

		OnEventHandler( rsCAMERASIZE, &r );
		if ( r.w != dx && r.h != dy )
			ReleaseCapture();

		RwV2d pView = { (float)0.5f , 0.5f*(float)dy/(float)dx };
		RwCameraSetViewWindow ( g_MyEngine.m_pCamera, &pView );
		
		if( m_pd3dLine )
			m_pd3dLine->OnResetDevice();
	}

	return true;
}

void CRenderWare::bRenderTo2ndWnd(HWND hWnd)
{
	if( m_pd3dDevice )
	{
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x0, 1.f, 0L );

		if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
		{
			m_pd3dDevice->EndScene();
		}
		m_pd3dDevice->Present(0,0,hWnd,0);
	}
};


BOOL CRenderWare::Idle( void )
{
	// 라이들 들어가기전에 몇가지 처리.
	return OnEventHandler( rsIDLE , NULL );
}

BOOL CRenderWare::MsgProc( MSG* pMsg )
{
	if( !m_bInitialised ) return FALSE;

	// 메시지 처리!
	HWND	window	= pMsg->hwnd	;
	UINT	message	= pMsg->message	;
	WPARAM	wParam	= pMsg->wParam	;
	LPARAM	lParam	= pMsg->lParam	;

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
		return g_MyEngine.OnMessageSink( event , param );
	
}

RsEventStatus	CRenderWare::OnKeyboardEventHandler	( RsEvent event , void *param )
{
	if( !m_bInitialised ) return rsEVENTNOTPROCESSED;
	else 
		return g_MyEngine.KeyboardHandler( event , param );
}

RsEventStatus	CRenderWare::OnMouseEventHandler	( RsEvent event , void *param )
{
	if( !m_bInitialised ) return rsEVENTNOTPROCESSED;
	else 
		return g_MyEngine.MouseHandler( event , param );
}
