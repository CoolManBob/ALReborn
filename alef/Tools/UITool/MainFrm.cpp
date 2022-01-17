#include "..\maptool\mainfrm.h"
#include "..\maptool\mainfrm.h"
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "UITool.h"
#include "MyEngine.h"

#include "MainFrm.h"

#include "UITEventMapDialog.h"
#include "UITHotkeyDialog.h"

#include "UITModeDialog.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSize	g_acsViewSize[UIT_MAX_VIEW] =
{
	CSize(0, 0),
	CSize(1024, 768),
	CSize(1280, 1024),
	CSize(1600, 1200)
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_UIT_MENU_NEW, OnNewUI)
	ON_COMMAND(ID_CONTROL_BUTTON, OnControlButton)
	ON_COMMAND(ID_CONTROL_COMBO, OnControlCombo)
	ON_COMMAND(ID_CONTROL_EDITBOX, OnControlEdit)
	ON_COMMAND(ID_CONTROL_LIST, OnControlList)
	ON_COMMAND(ID_CONTROL_STATIC, OnControlStatic)
	ON_WM_SIZE()
	ON_COMMAND(ID_UIT_SAVE, OnUITSave)
	ON_COMMAND(ID_UIT_LOAD, OnUITLoad)
	ON_COMMAND(ID_MODE_EDIT, OnModeEdit)
	ON_COMMAND(ID_MODE_RUN, OnModeRun)
	ON_COMMAND(ID_UIT_EDIT_EVENT, OnUITEditEvent)
	ON_COMMAND(ID_MODE_AREA, OnModeArea)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, OnUpdateModeEdit)
	ON_WM_DESTROY()
	ON_COMMAND(ID_CONTROL_GRID, OnControlGrid)
	ON_COMMAND(ID_CONTROL_BAR, OnControlBar)
	ON_COMMAND(ID_CONNECT_SERVER, OnConnectServer)
	ON_UPDATE_COMMAND_UI(ID_MODE_AREA, OnUpdateModeArea)
	ON_UPDATE_COMMAND_UI(ID_MODE_RUN, OnUpdateModeRun)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_SERVER, OnUpdateConnectServer)
	ON_COMMAND(ID_CONTROL_SKILLTREE, OnControlSkilltree)
	ON_COMMAND(ID_UIT_EDIT_HOTKEY, OnUITEditHotkey)
	ON_COMMAND(ID_UIT_MODE_VIEW_GRID, OnUITModeViewGrid)
	ON_COMMAND(ID_CONTROL_SCROLL, OnControlScroll)
	ON_COMMAND(ID_CONTROL_CUSTOM, OnControlCustom)
	ON_COMMAND(ID_SET_UI_MODE, OnSetUIMode)
	ON_COMMAND(ID_CONTROL_TREE, OnControlTree)
	ON_COMMAND(ID_CONTROL_CLOCK, OnControlClock)
	//}}AFX_MSG_MAP
	//ON_COMMAND(IDM_FINDOBJECT, OnFindobject)
	ON_COMMAND(ID_VIEW_FITTOWINDOWSIZE, OnViewFittowindowsize)
	ON_COMMAND(ID_VIEW_1024, OnView1024)
	ON_COMMAND(ID_VIEW_1280, OnView1280)
	ON_COMMAND(ID_VIEW_1600, OnView1600)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FITTOWINDOWSIZE, OnUpdateViewFittowindowsize)
	ON_UPDATE_COMMAND_UI(ID_VIEW_1024, OnUpdateView1024)
	ON_UPDATE_COMMAND_UI(ID_VIEW_1280, OnUpdateView1280)
	ON_UPDATE_COMMAND_UI(ID_VIEW_1600, OnUpdateView1600)
	ON_COMMAND(ID_UIT_COPY_UI, OnUitCopyUi)
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
	g_pMainFrame	= this;
	m_bConnected	= FALSE;

	m_eView			= UIT_VIEW_WINDOW;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame

	if (!m_wndView.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL ,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	m_pwndUIList = new CUIListView;
	if (!m_pwndUIList->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
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
	
	if (!m_wndToolBarControl.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBarControl.LoadToolBar(IDR_CONTROLS))
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
	m_wndToolBarControl.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndToolBarControl);

	TBBUTTONINFO	stBInfo;

	stBInfo.cbSize = sizeof(stBInfo);
	stBInfo.dwMask = TBIF_STYLE;
	stBInfo.fsStyle = TBSTYLE_CHECKGROUP;

	m_wndToolBarControl.GetToolBarCtrl().SetButtonInfo(ID_MODE_EDIT, &stBInfo);
	m_wndToolBarControl.GetToolBarCtrl().SetButtonInfo(ID_MODE_RUN, &stBInfo);
	m_wndToolBarControl.GetToolBarCtrl().SetButtonInfo(ID_MODE_AREA, &stBInfo);

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


void CMainFrame::OnNewUI() 
{
	g_MainWindow.m_pdlgWinProperty->UpdateDataToDialog(NULL, NULL, NULL);

	if (AfxMessageBox("Remove previous UIs?", MB_YESNO) == IDYES)
	{
		g_MainWindow.m_pcsUI = NULL;
		g_pcsAgcmUIManager2->RemoveAllUI();
	}

	g_MainWindow.ChangeMode(g_MainWindow.m_eMode);

	g_MainWindow.NewUI();
	UpdateUIList();
}

void CMainFrame::OnControlButton() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_BUTTON);
}

void CMainFrame::OnControlCombo() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_COMBO);
}

void CMainFrame::OnControlEdit() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_EDIT);
}

void CMainFrame::OnControlList() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_LIST);
}

void CMainFrame::OnControlStatic() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_BASE);
}

void CMainFrame::OnControlGrid() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_GRID);
}

void CMainFrame::OnControlBar() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_BAR);
}

void CMainFrame::OnControlSkilltree() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_SKILL_TREE);
}

void CMainFrame::OnControlScroll() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_SCROLL);
}

void CMainFrame::OnControlTree() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_TREE);
}

void CMainFrame::OnControlCustom() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_CUSTOM);
}

void CMainFrame::OnControlClock() 
{
	g_MainWindow.NewControl(AcUIBase::TYPE_CLOCK);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
}

void CMainFrame::UpdateUIList()
{
	INT32		lIndex = 0;
	INT32		lListIndex;
	AgcdUI *	pcsUI;
	CListCtrl &	csList = m_pwndUIList->GetListCtrl();

	csList.DeleteAllItems();

	for (pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex); pcsUI; pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex))
	{
		lListIndex = csList.InsertItem(lIndex, pcsUI->m_szUIName);
		if (lListIndex == -1)
			break;

		csList.SetItemData(lListIndex, (DWORD) pcsUI);
	}
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

void CMainFrame::OnUITSave() 
{
	CFileDialog	dlgFile(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, "|*.ini", this);
    //CFileDialog	dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, "|*.ini", this);

	if (dlgFile.DoModal() == IDOK)
	{
		INT32	eMode = g_MainWindow.m_eMode;

		g_MainWindow.ChangeMode(UIT_MODE_RUN);

		if (!g_pcsAgcmUIManager2->StreamWrite((LPSTR) (LPCTSTR) dlgFile.GetPathName(), FALSE))
			AfxMessageBox("Error INI Saving...!!!");

		g_MainWindow.ChangeMode(UIT_MODE_EDIT);
	}
}

void CMainFrame::OnUITLoad() 
{
	CFileDialog	dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, "|*.ini", this);

	if (dlgFile.DoModal() == IDOK)
	{
		g_MainWindow.m_pdlgWinProperty->UpdateDataToDialog(NULL, NULL, NULL);

		if (AfxMessageBox("Remove previous UIs?", MB_YESNO) == IDYES)
		{
			g_MainWindow.m_pcsUI = NULL;
			g_pcsAgcmUIManager2->RemoveAllUI();

			g_pcsAgcmUIManager2->RemoveAllEventActions();
			g_pcsAgcmUIManager2->RemoveAllHotkey();
		}

		UITMode eMode = g_MainWindow.m_eMode;

		g_MainWindow.ChangeMode(UIT_MODE_RUN, FALSE);

		g_pcsAgcmUIManager2->StreamRead((LPSTR) (LPCTSTR) dlgFile.GetPathName(), FALSE);

		UpdateUIList();

		g_MainWindow.ChangeMode(UIT_MODE_EDIT, TRUE);

		if (eMode != UIT_MODE_EDIT)
			g_MainWindow.ChangeMode(eMode, TRUE);
	}
}

void CMainFrame::OnModeEdit() 
{
	g_MainWindow.ChangeMode(UIT_MODE_EDIT);
}

void CMainFrame::OnModeRun() 
{
	g_MainWindow.ChangeMode(UIT_MODE_RUN);
}

void CMainFrame::OnModeArea() 
{
	g_MainWindow.ChangeMode(UIT_MODE_AREA);
}

void CMainFrame::OnUITEditEvent() 
{
	UITEventMapDialog	dlgEventMap(g_MainWindow.m_pcsUI);

	if (dlgEventMap.DoModal() == IDOK)
	{
	}
}

void CMainFrame::OnUITEditHotkey() 
{
	UITHotkeyDialog		dlgHotkey(g_MainWindow.m_pcsUI);

	if (dlgHotkey.DoModal() == IDOK)
	{
	}
}

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	CFrameWnd::RecalcLayout(bNotify);

	if (m_wndView.m_hWnd)
	{
		// 각 윈도우 크기 조절.

		CRect	rectChildView;
		CRect	rectUIList;
		SCROLLINFO	stScroll;

		m_wndView.GetWindowRect(&rectChildView);
		ScreenToClient(&rectChildView);

		rectChildView.right -= UIT_UILIST_WIDTH;
		m_wndView.MoveWindow(&rectChildView);

		stScroll.cbSize	= sizeof(SCROLLINFO);
		stScroll.fMask	= SIF_POS | SIF_RANGE | SIF_PAGE;
		stScroll.nMin	= 0;
		stScroll.nPos	= 0;
		stScroll.nMax	= g_acsViewSize[m_eView].cx;
		stScroll.nPage	= rectChildView.right - rectChildView.left;
		m_wndView.SetScrollInfo(SB_HORZ, &stScroll, TRUE);

		stScroll.nMax	= g_acsViewSize[m_eView].cy;
		stScroll.nPage	= rectChildView.bottom - rectChildView.top;
		m_wndView.SetScrollInfo(SB_VERT, &stScroll, TRUE);

		rectUIList = CRect(rectChildView.right, rectChildView.top, rectChildView.right + UIT_UILIST_WIDTH, rectChildView.bottom);
		m_pwndUIList->MoveWindow(&rectUIList);
	}
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnConnectServer() 
{
	g_pcsAgcmLogin->ConnectLoginServer();

	m_bConnected = TRUE;
}

void CMainFrame::OnUpdateModeEdit(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_MainWindow.m_eMode == UIT_MODE_EDIT);
}

void CMainFrame::OnUpdateModeArea(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_MainWindow.m_eMode == UIT_MODE_AREA);
}

void CMainFrame::OnUpdateModeRun(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_MainWindow.m_eMode == UIT_MODE_RUN);
}

void CMainFrame::OnUpdateConnectServer(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bConnected);
}

void CMainFrame::OnUITModeViewGrid() 
{
	g_MainWindow.SetGridView(!g_MainWindow.m_bDrawGrid);
}

void CMainFrame::OnSetUIMode() 
{
	UITModeDialog	dlgUIMode;

	if (dlgUIMode.DoModal() == IDOK)
	{
		g_pcsAgcmUIManager2->SetUIMode(dlgUIMode.m_eMode);
	}
}

//void CMainFrame::OnFindobject()
//{
//	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//}
//
//void CMainFrame::OnFindobjectnext()
//{
//	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//}

void CMainFrame::OnViewFittowindowsize()
{
	m_eView	= UIT_VIEW_WINDOW;
}

void CMainFrame::OnView1024()
{
	m_eView	= UIT_VIEW_1024_768;
	g_MainWindow.MoveWindow(g_MainWindow.x, g_MainWindow.y, g_acsViewSize[m_eView].cx, g_acsViewSize[m_eView].cy);
	RecalcLayout(TRUE);
}

void CMainFrame::OnView1280()
{
	m_eView	= UIT_VIEW_1280_1024;
	g_MainWindow.MoveWindow(g_MainWindow.x, g_MainWindow.y, g_acsViewSize[m_eView].cx, g_acsViewSize[m_eView].cy);
	RecalcLayout(TRUE);
}

void CMainFrame::OnView1600()
{
	m_eView	= UIT_VIEW_1600_1200;
	g_MainWindow.MoveWindow(g_MainWindow.x, g_MainWindow.y, g_acsViewSize[m_eView].cx, g_acsViewSize[m_eView].cy);
	RecalcLayout(TRUE);
}

void CMainFrame::OnUpdateViewFittowindowsize(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_eView == UIT_VIEW_WINDOW);
}

void CMainFrame::OnUpdateView1024(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_eView == UIT_VIEW_1024_768);
}

void CMainFrame::OnUpdateView1280(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_eView == UIT_VIEW_1280_1024);
}

void CMainFrame::OnUpdateView1600(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_eView == UIT_VIEW_1600_1200);
}

void CMainFrame::OnUitCopyUi()
{
	if (!g_MainWindow.m_pcsUI)
		return;

	AgcdUI *	pcsUI;
	CHAR		szName[128];

	g_MainWindow.ChangeMode(UIT_MODE_RUN);

	sprintf(szName, "UIClone_%d", rand());
	pcsUI = g_pcsAgcmUIManager2->CopyUI(g_MainWindow.m_pcsUI, szName);

	g_MainWindow.ChangeMode(UIT_MODE_EDIT);

	if (!pcsUI)
		return;

	g_pcsAgcmUIManager2->AddUI(pcsUI);
	UpdateUIList();
}
