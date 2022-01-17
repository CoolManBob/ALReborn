#include "stdafx.h"
#include "EffTool.h"
#include "MainFrm.h"
#include "ToolOption.h"
#include "AgcmEff2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum	tagEPane
{
	ePane_List_EffSet		= 0,
	ePane_Tree_EffSet,
	ePane_Property_Eff,
	ePane_Property_Option,
	ePane_Edit_EffTxt,
	ePane_Edit_Output,
	ePane_Num,
};

struct XTPPlanInfo
{
	DWORD					dwID;
	DWORD					dwIcon;
	char*					szCaption;
	XTPDockingPaneDirection	eDir;
};

static const XTPPlanInfo s_cPlanInfo[ePane_Num] = 
{
	{ 0, 8055 + 0, "List_EffSet", xtpPaneDockRight },
	{ 1, 8055 + 1, "Tree_EffSet", xtpPaneDockLeft },
	{ 2, 8055 + 2, "Property_Eff", xtpPaneDockRight },
	{ 3, 8055 + 3, "Property_Option", xtpPaneDockLeft },
	{ 4, 8055 + 4, "Edit_EffTxt", xtpPaneDockLeft },
	{ 5, 8055 + 5, "Edit_Output", xtpPaneDockBottom },
};

CXTPDockingPane* PANE_PTR[ ePane_Num ] = { NULL, };
CXTPDockingPane* GetPane(tagEPane ePane) { return PANE_PTR[ePane]; };

const unsigned int	PANE_ID_REF	= 8055;
const unsigned int	PANE_ID[ ePane_Num ] =
{
	PANE_ID_REF + ePane_List_EffSet,
	PANE_ID_REF + ePane_Tree_EffSet,
	PANE_ID_REF + ePane_Property_Eff,
	PANE_ID_REF + ePane_Property_Option,
	PANE_ID_REF + ePane_Edit_EffTxt,
	PANE_ID_REF + ePane_Edit_Output,
};

const char*	PANE_CAPTION[ ePane_Num ] =
{
	"List_EffSet",
	"Tree_EffSet",
	"Property_Eff",
	"Property_Option",
	"Edit_EffTxt",
	"Edit_Output",
};
	  
const XTPDockingPaneDirection paneDir[ ePane_Num ] =
{
	xtpPaneDockRight,
	xtpPaneDockLeft,
	xtpPaneDockRight,
	xtpPaneDockLeft,
	xtpPaneDockLeft,
	xtpPaneDockBottom,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

void ToWnd(LPCTSTR szInfo)
{
	if( CMainFrame::bGetThis() )
		CMainFrame::bGetThis()->bOutputWnd(szInfo);
}

void ErrToWnd(LPCTSTR szMsg, LPCTSTR szFunc, LPCTSTR szFile, INT nLine)
{
	CString	strInfo;
	strInfo.Format(
		_T("//----> ERROR\r\n")
		_T("emsg : %s\r\n")
		_T("func : %s\r\n")
		_T("file : %s\r\n")
		_T("line : %d\r\n")
		_T("//<----")
		, szMsg
		, szFunc
		, szFile
		, nLine );
		
	ToWnd(strInfo);
};

CMainFrame*	CMainFrame::PTHIS	= NULL;
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	PTHIS	= this;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if ( !m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if( !InitCommandBars() )
		return -1;

	//DocPane Setting
	GetDockingPaneManager()->InstallDockingPanes( this );
	GetDockingPaneManager()->SetTheme( xtpPaneThemeOffice2003 );
	CXTPPaintManager::SetTheme( xtpThemeOffice2003 );

	//CXTPDockingPaneLayout	layoutNormal( GetDockingPaneManager() );
	//if( layoutNormal.Load( _T( "EffToolNormalLayout" ) ) )
	//	GetDockingPaneManager()->SetLayout( &layoutNormal );
	//else
	{
		for( int i=0; i<ePane_Num; ++i )
		{
			PANE_PTR[i] = GetDockingPaneManager()->CreatePane( PANE_ID[i], CRect(0, 0, 210, 140), paneDir[i] );
			PANE_PTR[i]->SetTitle( PANE_CAPTION[i] );
			PANE_PTR[i]->SetOptions(xtpPaneNoCloseable);
		}

		::GetPane( ePane_Property_Option )->Hide();
		::GetPane( ePane_Edit_EffTxt )->Hide();
		GetDockingPaneManager()->AttachPane( GetPane(ePane_Property_Eff), GetPane(ePane_List_EffSet) );
	}


	int	nPaneIcon[] =
	{
		  PANE_ID[ePane_List_EffSet]
		, PANE_ID[ePane_Tree_EffSet]
		, PANE_ID[ePane_Property_Eff]
		, PANE_ID[ePane_Property_Option]
		, PANE_ID[ePane_Edit_EffTxt]
		, PANE_ID[ePane_Edit_Output]
		, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	GetDockingPaneManager()->SetIcons( IDB_BMP_PANE, nPaneIcon, sizeof( nPaneIcon ) / sizeof( nPaneIcon[0] ), RGB( 0, 255, 0 ) );
	VERIFY( m_MTIClientWnd.Attach( this ) );

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



void CMainFrame::OnEmptyCommand()
{
}
//void CMainFrame::OnEmptyCommandRange(UINT)
//{
//}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if( CGlobalVar::bGetInst().bIsInitEngin() )
			if (!pPane->IsValid())
			{
				switch (pPane->GetID())
				{
				case PANE_ID_REF + ePane_List_EffSet		:{pPane->Attach(CWndListEffSet());}break;
				case PANE_ID_REF + ePane_Tree_EffSet		:{pPane->Attach(CWndTreeEffSet());}break;
				case PANE_ID_REF + ePane_Property_Eff		:{pPane->Attach(CWndPropEffSet());}break;
				case PANE_ID_REF + ePane_Property_Option	:{pPane->Attach(CWndPropOption());}break;
				case PANE_ID_REF + ePane_Edit_EffTxt		:{pPane->Attach(CWndEditEffTxt());}break;
				case PANE_ID_REF + ePane_Edit_Output		:{pPane->Attach(CWndEditOutput());}break;
				}
			}

		return 1;
	}

	return 0;
}

void CMainFrame::OnDestroy() 
{
	CMDIFrameWnd::OnDestroy();
	
	m_MTIClientWnd.Detach();	
}


BOOL CMainFrame::ShowWindowEx(int nCmdShow)
{
#ifdef _XTP_INCLUDE_CONTROLS
	// Restore frame window size and position.
	CXTWindowPos wndPosition;
	wndPosition.LoadWindowPos(this);
	nCmdShow = wndPosition.showCmd;
#endif

	return ShowWindow(nCmdShow);
}

void CMainFrame::OnClose() 
{
	//왜 먼저 지워줘야 할까? MDI?
	m_dlgTreeEffSet.bClear();

	SaveCommandBars(_T("EffToolCommandBars"));

	//CXTPDockingPaneLayout layoutNormal(GetDockingPaneManager());
	//GetDockingPaneManager()->GetLayout(&layoutNormal);	
	//layoutNormal.Save(_T("EffToolNormalLayout"));

#ifdef _XTP_INCLUDE_CONTROLS
	CXTWindowPos wndPosition;
	wndPosition.SaveWindowPos(this);
#endif

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState) 
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	// Toggle CommandBars
	int nCount = pCommandBars->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPToolBar* pBar = pCommandBars->GetAt(i);
		pBar->OnSetPreviewMode(bPreview);	
	}

	// Toggle Docking Panes.
	if (bPreview)
	{
		ASSERT(m_pPreviewLayout == NULL);
		m_pPreviewLayout   = m_paneManager.CreateLayout();
		m_paneManager.GetLayout(m_pPreviewLayout);
		m_paneManager.CloseAll();
	}
	else
	{
		ASSERT(m_pPreviewLayout != NULL);		
		m_paneManager.SetLayout( m_pPreviewLayout );
		delete m_pPreviewLayout;
		m_pPreviewLayout = 0;
		
	}

	
	CMDIFrameWnd::OnSetPreviewMode(bPreview, pState);
}

CWnd* CMainFrame::CWndPropOption(void)
{
	if( !m_dlgGridOption.GetSafeHwnd() )
		m_dlgGridOption.Create(CDlgStatic::IDD, this);

	return static_cast<CWnd*>(&m_dlgGridOption);
}

CWnd* CMainFrame::CWndPropEffSet(void)
{
	if( !m_dlgGridEffSet.GetSafeHwnd() )
		m_dlgGridEffSet.Create(CDlgStatic::IDD, this);

	return static_cast<CWnd*>(&m_dlgGridEffSet);
}

CWnd* CMainFrame::CWndListEffSet(void)
{
	if( !m_dlgListEffSet.GetSafeHwnd() )
	{
		if( !m_dlgListEffSet.Create( CDlgListEffSet::IDD, this ) )
		{
			TRACE0( "m_wndListEffSet.Create failed.\n" );
			return NULL;
		}
	}

	return static_cast<CWnd*>(&m_dlgListEffSet);
}

CWnd* CMainFrame::CWndTreeEffSet(void)
{
	if( !m_dlgTreeEffSet.GetSafeHwnd() )
	{
		if( !m_dlgTreeEffSet.Create( CDlgTree::IDD, this ) )
		{
			TRACE0( "m_dlgTreeEffSet.Create failed.\n" );
			return NULL;
		}
	}

	return static_cast<CWnd*>(&m_dlgTreeEffSet);
}

CWnd* CMainFrame::CWndEditEffTxt(void)
{
	if( !m_dlgEditEffTxt.GetSafeHwnd() )
	{
		m_dlgEditEffTxt.Create(CDlgEdit::IDD, this);
		m_dlgEditEffTxt.m_ctrlWnd.ModifyStyleEx(0, WS_EX_STATICEDGE);
		m_dlgEditEffTxt.bAddString(_T("here~"));
		m_dlgEditEffTxt.bAddString(_T("effset ini file"));
		m_dlgEditEffTxt.bSetMargins(10, 5);
	}

	return static_cast<CWnd*>(&m_dlgEditEffTxt);
}

CWnd* CMainFrame::CWndEditOutput(void)
{
	if( !m_dlgEditOutput.GetSafeHwnd() )
	{
		m_dlgEditOutput.Create(CDlgEdit::IDD, this);
		m_dlgEditOutput.m_ctrlWnd.ModifyStyleEx(0, WS_EX_STATICEDGE);
		m_dlgEditOutput.bAddString(_T("------------------------------------"));
		m_dlgEditOutput.bAddString( Eff2Ut_GetTimeStamp() );
		m_dlgEditOutput.bAddString(_T("output window !!!"));
		m_dlgEditOutput.bAddString(_T("------------------------------------"));
		m_dlgEditOutput.bSetMargins(10, 5);
	}

	return static_cast<CWnd*>(&m_dlgEditOutput);
};

INT CMainFrame::bInsEffSetFromListToTree(AgcdEffSet* pEffSet)
{
	if( m_dlgTreeEffSet.GetSafeHwnd() )
	{
		CDlgTreeEffSet::Lparam	lparam(pEffSet);
		return m_dlgTreeEffSet.bInsLparam( lparam );
	}

	return -1;
};

void CMainFrame::bOutputWnd(LPCTSTR strInfo)
{
	if( !m_dlgEditOutput.GetSafeHwnd() )
		CWndEditOutput();
	
	m_dlgEditOutput.bAddString(strInfo);
};
