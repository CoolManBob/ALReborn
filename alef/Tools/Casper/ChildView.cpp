// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Casper.h"
#include "ChildView.h"

#include "MainFrm.h"
#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView

extern CCasperApp theApp;

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}


BOOL CChildView::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN		||
		pMsg->message == WM_KEYUP		)
	{
		// 다음 키가 입력을 받지 않아서 ( 어딘가에서 먹어버리고 있는듯하다 )
		// 부득이하게 여기서 포워딩 시켜버림
		switch( pMsg->wParam )
		{
		case 'W'		:
		case 'A'		:
		case 'S'		:
		case 'D'		:
		case 'Q'		:
		case 'E'		:
		case VK_TAB		:
				/********************* Parn님이 여기다가 VK_DEL 넣었당. **********/
		case VK_DELETE	:
		case VK_LEFT	:
		case VK_UP		:
		case VK_RIGHT	:
		case VK_DOWN	:
			{
				theApp.GetRenderWare().MessageProc( pMsg );
			}
			return FALSE;
		case 'Z'		:
			{
				if( GetAsyncKeyState( VK_CONTROL ) < 0 ) 
					break;
				else 
					theApp.GetRenderWare().MessageProc( pMsg );
				return TRUE;
			}
		case VK_SPACE	:
			{
				theApp.GetRenderWare().MessageProc( pMsg );
			}
			return TRUE;
		default:
			// do no op
			break;
		}
		
	}
	return CWnd ::PreTranslateMessage(pMsg);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rcClient;
	GetClientRect(&rcClient);
	
	//Get the largest client dimension so the renderware client has a ratio of 1:1
	int iMaxDim;
	iMaxDim = ( rcClient.Width() > rcClient.Height() )? rcClient.Width() : rcClient.Height();
	
	//RenderWare initialisation===================================================
	theApp.GetRenderWare().Initialize( this->GetSafeHwnd() );
	
	return 0;
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	cx -= UIT_UILIST_WIDTH;

	CWnd ::OnSize(nType, cx, cy);
}

void CChildView::OnDestroy() 
{
	CWnd ::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

LRESULT CChildView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// 엔진으로 메시지 포워드..
	MSG messagestruct;
	messagestruct.hwnd		= GetSafeHwnd();
	messagestruct.message	= message;
	messagestruct.wParam	= wParam;
	messagestruct.lParam	= lParam;
	
	if( theApp.GetRenderWare().MessageProc( & messagestruct ) ) return TRUE;
	
	return CWnd ::WindowProc(message, wParam, lParam);
}
