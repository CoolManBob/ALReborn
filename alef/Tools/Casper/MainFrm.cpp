// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Casper.h"

#include "MainFrm.h"

#include "MyEngine.h"
#include "TestEnvDialog.h"
#include "ConCurrencyMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL	g_bIgnorePacket;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_LOGIN_SEQUENCE, OnLoginSequence)
	ON_COMMAND(ID_RENDERING_TOGGLE, OnRenderingToggle)
	ON_WM_TIMER()
	ON_COMMAND(ID_USER_LIST_REFRESH, OnUserListRefresh)
	ON_COMMAND(ID_TEST_ACTION, OnTestAction)
	ON_COMMAND(ID_LOGIN_CONCURRENCY, OnLoginConcurrency)
	ON_COMMAND(ID_TEST_ACTION_STOP, OnTestActionStop)
	ON_COMMAND(ID_LOGIN_DIRECT, OnLoginDirect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CMainFrame *	g_pMainFrame	= NULL;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	g_pMainFrame = this;
	m_bConnected = FALSE;

	m_strHigherID	= "smith";
	m_lBeginID		= 50;
	m_lEndID		= 80;
	m_lCurrent		= 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	m_pwndUserList = new CUserListView;
	if (!m_pwndUserList->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 1, NULL))
	{
		TRACE0("Failed to create ui list window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnLoginSequence() 
{
	// TODO: Add your command handler code here
	if (SetTestEnv())
	{
		g_MyEngine.SetSequenceLogin();
		ConnectToServer();
	}
}

void CMainFrame::ConnectToServer()
{
	//g_csAgcmLogin.LoginToServer();		
	g_pcsAgcmCasper->ConnectDirectToGameServer();
}

BOOL CMainFrame::SetTestEnv()
{
	CTestEnvDialog EnvDialog;
	EnvDialog.m_strHigherID = m_strHigherID;
	EnvDialog.m_lID_Begin = m_lBeginID;
	EnvDialog.m_lID_End = m_lEndID;
	m_lCurrent = 0;

	if (IDOK == EnvDialog.DoModal())
	{
		m_strHigherID = EnvDialog.m_strHigherID;
		m_lBeginID = EnvDialog.m_lID_Begin;
		m_lEndID = EnvDialog.m_lID_End;

		g_pcsAgcmCasper->SetMaxUsers(abs(m_lEndID - m_lBeginID) + 1);
		g_pcsAgcmCasper->SetServerInfo(EnvDialog.m_strServerIP, EnvDialog.m_ulServerPort);

		return TRUE;
	}
	else return FALSE;
}

void CMainFrame::UpdateUserList(BOOL bBottom)
{
	INT32	lListIndex;
	INT32	lCharNameCount =  g_vtCharName.size();
	CListCtrl &	csList = m_pwndUserList->GetListCtrl();
	csList.DeleteAllItems();
	
	for (INT32 i = 0; i < lCharNameCount; i++)
	{
		lListIndex = csList.InsertItem(i, g_vtCharName[i]);
		csList.SetItemData(lListIndex, (DWORD)i);
	}

	if (bBottom) csList.SendMessage(WM_VSCROLL, SB_BOTTOM);
/*
	while (_iter == g_vtCharName.end())
	{
		lListIndex = csList.InsertItem(lIndex, (*_iter));
		csList.SetItemData(lListIndex, (DWORD)pcsChar);
		_iter++;
	}
*/
/*
	INT32		lIndex = 0;
	INT32		lListIndex;
	AgpdCharacter * pcsChar;
	CListCtrl &	csList = m_pwndUserList->GetListCtrl();

	csList.DeleteAllItems();

	for (pcsChar = g_pcsAgcmCharacter->GetAgpmCharacter()->GetCharSequence(&lIndex); pcsChar; pcsChar = g_pcsAgcmCharacter->GetAgpmCharacter()->GetCharSequence(&lIndex))
	{
		lListIndex = csList.InsertItem(lIndex, pcsChar->m_szID);
		if (lListIndex == -1)
			break;

		csList.SetItemData(lListIndex, (DWORD)pcsChar);
	}	
*/
}

void CMainFrame::AdduserList(LPCTSTR lpCharName)
{
	CListCtrl &	csList = m_pwndUserList->GetListCtrl();
	csList.InsertItem(csList.GetItemCount(), lpCharName);
	csList.SendMessage(WM_VSCROLL, SB_BOTTOM);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN		||
		pMsg->message == WM_KEYUP		)
	{
		if( m_wndView.PreTranslateMessage( pMsg ) ) return TRUE;
	}
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	CFrameWnd::RecalcLayout(bNotify);

	if (m_wndView.m_hWnd)
	{
		// 각 윈도우 크기 조절.

		CRect	rectChildView;
		CRect	rectUIList;

		m_wndView.GetWindowRect(&rectChildView);
		ScreenToClient(&rectChildView);

		rectChildView.right -= UIT_UILIST_WIDTH;
		m_wndView.MoveWindow(&rectChildView);

		rectUIList = CRect(rectChildView.right, rectChildView.top, rectChildView.right + UIT_UILIST_WIDTH, rectChildView.bottom);
		m_pwndUserList->MoveWindow(&rectUIList);
	}
}

void CMainFrame::OnRenderingToggle() 
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
	case TIMER_SEQUENCE_LOGIN:
		{
			// Sequence Login
			TimerSequenceLogin();
		}
		break;
	case TIMER_TEST_ACTION:
		{
			// Concurrency Login
			TimerTestAction();
		}
		break;

	default:
		{
			if (nIDEvent >= 1000)
			{
				KillTimer(nIDEvent);
				g_pcsAgcmCasper->SendRequestInit(g_pcsAgcmCasper->m_astUsers + nIDEvent - 1000);
			}
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}
void CMainFrame::TimerSequenceLogin()
{
	OutputDebugString("\nKillTimer\n");
	KillTimer(TIMER_SEQUENCE_LOGIN);
	ConnectToServer();
}

void CMainFrame::TimerTestAction()
{
	g_pcsAgcmCasper->ActionAllUsers();
	/*
	INT32	lCharNameCount =  g_vtCharName.size();
	CListCtrl &	csList = m_pwndUserList->GetListCtrl();

	static BOOL bToggle = TRUE;
	bToggle = !bToggle;

	for (INT32 i = 0; i < lCharNameCount; i++)
	{
		AgpdCharacter *pcsAgpdCharacter = g_pcsAgpmCharacter->GetCharacter((CHAR*)g_vtCharName[i].operator LPCTSTR());

		if (pcsAgpdCharacter)	// NULL이 아닐 경우에만 실행
		{
			AuPOS stTarget;

			stTarget = pcsAgpdCharacter->m_stPos;

			stTarget.x += (bToggle ? 1000 : -1000);
			stTarget.z += (bToggle ? 1000 : -1000);

//			g_pcsAgcmCharacter->MoveSelfCharacter( &stTarget, 0, FALSE, FALSE );
		}
	}
	*/
}

void CMainFrame::OnUserListRefresh() 
{
	// TODO: Add your command handler code here
	UpdateUserList();
}

void CMainFrame::OnTestAction() 
{
	g_bIgnorePacket = TRUE;

	// 게임서버에서 테스트 시작
	SetTimer(TIMER_TEST_ACTION, TIMER_TEST_ACTION_ELAPSE, NULL);
}

void CMainFrame::OnTestActionStop()
{
	g_bIgnorePacket = FALSE;

	KillTimer(TIMER_TEST_ACTION);
}

void CMainFrame::OnLoginConcurrency() 
{
	INT16 vtNIDs[50];

	::ZeroMemory(vtNIDs, sizeof(vtNIDs));

	if (SetTestEnv())
	{
		g_MyEngine.SetConcurrencyLogin();

		for ( INT i = 0; m_lEndID > m_lBeginID + i; i++ )
		{
			ConCurrencyUser *pstUser = new ConCurrencyUser;
			//pstUser->m_lNID = g_csAgcmLogin.LoginToServer();
		}
	}
}


void CMainFrame::OnLoginDirect() 
{
	CTestEnvDialog EnvDialog;

	if (EnvDialog.DoModal() == IDOK)
	{
		g_pcsAgcmCasper->SetMaxUsers(EnvDialog.m_lClientNumber);
		g_pcsAgcmCasper->SetServerInfo(EnvDialog.m_strServerIP, EnvDialog.m_ulServerPort);
		g_pcsAgcmCasper->ConnectDirectToGameServer();
	}
}
