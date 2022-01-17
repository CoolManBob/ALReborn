// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "UITool.h"
#include "ChildView.h"

#include "MainFrm.h"

#include "MyEngine.h"
#include ".\childview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUIToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	m_pcsControlCopy = NULL;
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
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
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
//				theApp.GetRenderWare().MessageProc( pMsg );
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
//				theApp.GetRenderWare().MessageProc( pMsg );
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

LRESULT CChildView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// 엔진으로 메시지 포워드..
	MSG messagestruct;
	messagestruct.hwnd		= GetSafeHwnd();
	messagestruct.message	= message;
	messagestruct.wParam	= wParam;
	messagestruct.lParam	= lParam;
	
	if( theApp.GetRenderWare().MessageProc( & messagestruct ) ) return TRUE;

	CHAR	szTemp[128];

	sprintf(szTemp, "Message : %d\n", message);
	OutputDebugString(szTemp);

	return CWnd ::WindowProc(message, wParam, lParam);
}

void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	RwRect r;

	cx -= UIT_UILIST_WIDTH;

	r.x = 0;
	r.y = 0;
	r.w = cx;
	r.h = cy;

	CWnd ::OnSize(nType, cx, cy);

	RsEventHandler(rsCAMERASIZE, &r);
}

void CChildView::OnDestroy() 
{
	if (m_pcsControlCopy)
		g_pcsAgcmUIManager2->RemoveControl(m_pcsControlCopy);

	CWnd ::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CChildView::OnEditCopy() 
{
	if (g_MainWindow.m_eMode != UIT_MODE_EDIT)
		return;

	if (m_pcsControlCopy)
		g_pcsAgcmUIManager2->RemoveControl(m_pcsControlCopy);

	m_pcsControlCopy = NULL;

	if (g_MainWindow.m_pcsControl)
	{
		g_MainWindow.ChangeMode(UIT_MODE_RUN);
		m_pcsControlCopy = g_pcsAgcmUIManager2->CopyControl(g_MainWindow.m_pcsControl, NULL);
		g_MainWindow.ChangeMode(UIT_MODE_EDIT);
	}
}

void CChildView::OnEditPaste() 
{
	if (g_MainWindow.m_eMode != UIT_MODE_EDIT)
		return;

	if (!g_MainWindow.m_pcsUI)
		return;

	g_pcsAgcmUIManager2->EnableSmoothUI(FALSE);

	g_MainWindow.ChangeMode(UIT_MODE_RUN);

	if (g_MainWindow.m_pcsControl)
	{
		if (AfxMessageBox("Do you want embedded control?", MB_YESNO) == IDYES)
			g_MainWindow.m_pcsControl = g_pcsAgcmUIManager2->CopyControl(m_pcsControlCopy, g_MainWindow.m_pcsUI, g_MainWindow.m_pcsControl->m_pcsBase);
		else
			g_MainWindow.m_pcsControl = g_pcsAgcmUIManager2->CopyControl(m_pcsControlCopy, g_MainWindow.m_pcsUI, NULL);
	}
	else
		g_MainWindow.m_pcsControl = g_pcsAgcmUIManager2->CopyControl(m_pcsControlCopy, g_MainWindow.m_pcsUI, NULL);

	if (g_MainWindow.m_pcsControl)
	{
		sprintf(g_MainWindow.m_pcsControl->m_szName, "CONTROL_%d", rand());
	}

	g_MainWindow.ChangeMode(UIT_MODE_EDIT);

	g_pcsAgcmUIManager2->EnableSmoothUI(TRUE);
}

void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// Get the minimum and maximum scroll-bar positions.
	int minpos	= 0;
	int maxpos	= g_acsViewSize[((CMainFrame *) GetParentFrame())->m_eView].cx;

	// Get the current position of scroll box.
	int curpos = GetScrollPos(SB_HORZ);

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
	{
		// Get the page size. 
		SCROLLINFO   info;
		GetScrollInfo(SB_HORZ, &info, SIF_ALL);

		if (curpos > minpos)
		curpos = max(minpos, curpos - (int) info.nPage);
	}
		break;

	case SB_PAGERIGHT:      // Scroll one page right.
	{
		// Get the page size. 
		SCROLLINFO   info;
		GetScrollInfo(SB_HORZ, &info, SIF_ALL);

		if (curpos < maxpos)
			curpos = min(maxpos, curpos + (int) info.nPage);
	}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).
	SetScrollPos(SB_HORZ, curpos);

	g_MainWindow.MoveWindow(-curpos, g_MainWindow.y, g_MainWindow.w, g_MainWindow.h);

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// Get the minimum and maximum scroll-bar positions.
	int minpos = 0;
	int maxpos	= g_acsViewSize[((CMainFrame *) GetParentFrame())->m_eView].cy;

	// Get the current position of scroll box.
	int curpos = GetScrollPos(SB_VERT);

	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		curpos = minpos;
		break;

	case SB_RIGHT:      // Scroll to far right.
		curpos = maxpos;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (curpos > minpos)
			curpos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (curpos < maxpos)
			curpos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
	{
		// Get the page size. 
		SCROLLINFO   info;
		GetScrollInfo(SB_VERT, &info, SIF_ALL);

		if (curpos > minpos)
		curpos = max(minpos, curpos - (int) info.nPage);
	}
		break;

	case SB_PAGERIGHT:      // Scroll one page right.
	{
		// Get the page size. 
		SCROLLINFO   info;
		GetScrollInfo(SB_VERT, &info, SIF_ALL);

		if (curpos < maxpos)
			curpos = min(maxpos, curpos + (int) info.nPage);
	}
		break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		curpos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		curpos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).
	SetScrollPos(SB_VERT, curpos);

	g_MainWindow.MoveWindow(g_MainWindow.x, -curpos, g_MainWindow.w, g_MainWindow.h);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}
