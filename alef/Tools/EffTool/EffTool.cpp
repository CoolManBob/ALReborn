#include "stdafx.h"
#include "EffTool.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "EffToolDoc.h"
#include "EffToolView.h"

#include "BrowserView.h"

#include "./Dlg/DlgNewEffSet.h"

#include "AgcdEffGlobal.h"
#include "ToolOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CEffToolApp, CWinApp)
	//{{AFX_MSG_MAP(CEffToolApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_PACKING, OnPacking)
	ON_UPDATE_COMMAND_UI(ID_PACKING, OnUpdatePacking)
	ON_COMMAND(ID_STOP_CALCBOUND, OnStopCalcbound)
	ON_COMMAND(ID_CALCBOUND_ALLEFF, OnCalcboundAlleff)
	ON_COMMAND(ID_AMBIENT_LIGHT, OnAmbientLight)
	ON_COMMAND(ID_ENGINE_LIGHT, OnEngineLight)
END_MESSAGE_MAP()

CEffToolApp::CEffToolApp() : 
 m_bExistToolView(FALSE),
 m_pDocTemplateToolView(NULL)
{
}

CEffToolApp theApp;

BOOL CEffToolApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pDocTemplateToolView = new CMultiDocTemplate(
		IDR_EFFTOOTYPE,
		RUNTIME_CLASS(CEffToolDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CEffToolView));
	AddDocTemplate(m_pDocTemplateToolView);

	
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

//------------------------ CAboutDlg -------------------------
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CEffToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CEffToolApp::OnFileNew() 
{
	// TODO: Add your command handler code here
	if( !m_bExistToolView )
	{
		//CEffToolDoc* pDoc = (CEffToolDoc*)m_pDocTemplateBrowserView->OpenDocumentFile(NULL);
		//POSITION	pos	= pDoc->GetFirstViewPosition();
		//CBrowserView* pView = (CBrowserView*)pDoc->GetNextView(pos);
		//CString strURL("http://www.naver.com");
		//pView->Navigate(strURL);
		//pDoc->SetTitle( _T("BLOG") );
		
		CEffToolDoc* pDoc = (CEffToolDoc*)m_pDocTemplateToolView->OpenDocumentFile(NULL);
		pDoc->SetTitle( _T("TOOL VIEW") );
		m_bExistToolView	= TRUE;
	}
	else
	{
		if( !CMainFrame::bGetThis()->m_dlgListEffSet.bInit() )
		{
			ToWnd("새로운 이펙트를 작성하기 전에, 먼저 이펙트셋 리스트창을 활성화 시켜 주십시오.");
			return;
		}

		CDlgNewEffSet	dlgNewEffSet(CMainFrame::bGetThis());
		if( IDOK == dlgNewEffSet.DoModal() )
		{
			LPEFFSET pNewEffSet	=  AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet(dlgNewEffSet.m_dwEffSetID);
			if( !pNewEffSet )
			{
				ErrToWnd( "!pNewEffSEt" );
			}
			else
			{
				++pNewEffSet->m_nRefCnt;

				CMainFrame::bGetThis()->bInsEffSetFromListToTree( pNewEffSet );
				CMainFrame::bGetThis()->m_dlgTreeEffSet.bOnNewEffSet(dlgNewEffSet.m_dwEffSetID);

				--pNewEffSet->m_nRefCnt;

				ToWnd( Eff2Ut_FmtMsg("[id : %d, title : %s] 새로 추가되었습니다.", pNewEffSet->bGetID(), pNewEffSet->bGetTitle()) );
			}
		}
	}
}


void CEffToolApp::OnPacking()
{
	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_MakeEffFile();
}

void CEffToolApp::OnUpdatePacking(CCmdUI *pCmdUI)
{
}

extern MyEngine		g_MyEngine;
#define	FIRST_BEGIN	static BOOL b1st = TRUE; \
	if( b1st )	\
	{			\
		b1st	= FALSE;	
#define FIRST_END	}
		
//extern RwBBox*		CALCBOUNDINFO_BOX;
//extern RwSphere*	CALCBOUNDINFO_SPHERE;

struct	STSAVE_BOUND
{
	STSAVE_BOUND() : m_ulEffID(-1), m_pEffSet(NULL), m_bAfterUseEffSet(TRUE)	{	};

	void End()	{	m_ulEffID=-1; m_pEffSet=NULL; m_bAfterUseEffSet=TRUE;	}
	void Init()	{	m_ulEffID=0LU; m_pEffSet=NULL; m_bAfterUseEffSet=FALSE;	}

	unsigned int	m_ulEffID;
	LPEFFSET		m_pEffSet;
	BOOL			m_bAfterUseEffSet;
};

std::vector<UINT>	G_EFFSETQUEUE;
BOOL G_BEFFEND	= TRUE;
BOOL G_BSAVE	= TRUE;
BOOL CB_END( stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass=NULL )
{
	if( stProcessData.lEffectProcessType == AGCMEFF2_PROCESS_TYPE_EFFECT_END )
	{
		G_EFFSETQUEUE.pop_back();
		G_BEFFEND	= TRUE;
	}
	return TRUE;	
}


BOOL CB_NEXT( stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass=NULL )
{
	static	STSAVE_BOUND	stSavedInfo;

	if( !AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_CALCBOUND ) )
		return TRUE;

	if( !stSavedInfo.m_bAfterUseEffSet )
		return TRUE;

	////save
	//if( stSavedInfo.m_pEffSet )
	//{
	//	FIRST_BEGIN;
	//	Eff2Ut_TimeStampToFile("eff_자동출력된 바운딩 정보.txt");
	//	FIRST_END;

	//	if( CALCBOUNDINFO_SPHERE && CALCBOUNDINFO_BOX )
	//	{
	//		if( CALCBOUNDINFO_BOX->inf.x == 9999.f )
	//		{
	//			FIRST_BEGIN;
	//			Eff2Ut_TimeStampToFile("eff_바운딩정보_계산안함.txt");
	//			FIRST_END;

	//			Eff2Ut_TOFILE( "eff_바운딩정보_계산안함.txt", Eff2Ut_FmtMsg("%d, <%s>\n", stSavedInfo.m_ulEffID, "CALCBOUNDINFO_BOX->inf.x == 9999.f" ) );
	//		}
	//		else if( G_BSAVE )
	//		{
	//			Eff2Ut_TOFILE( "eff_자동출력된 바운딩 정보.txt"
	//				, Eff2Ut_FmtMsg("ID : %d\n"
	//				"sphere=%f:%f:%f:%f\n"
	//				"box=%f:%f:%f:%f:%f:%f\n"
	//				, stSavedInfo.m_ulEffID

	//				, CALCBOUNDINFO_SPHERE->center.x
	//				, CALCBOUNDINFO_SPHERE->center.y
	//				, CALCBOUNDINFO_SPHERE->center.z
	//				, CALCBOUNDINFO_SPHERE->radius

	//				, CALCBOUNDINFO_BOX->sup.x
	//				, CALCBOUNDINFO_BOX->sup.y
	//				, CALCBOUNDINFO_BOX->sup.z
	//				, CALCBOUNDINFO_BOX->inf.x
	//				, CALCBOUNDINFO_BOX->inf.y
	//				, CALCBOUNDINFO_BOX->inf.z
	//				)
	//				);

	//			stSavedInfo.m_pEffSet->bSetBBox( *CALCBOUNDINFO_BOX );
	//			stSavedInfo.m_pEffSet->bSetBSphere( *CALCBOUNDINFO_SPHERE );

	//			stSavedInfo.m_pEffSet->bAddRef();
	//			AgcdEffGlobal::bGetInst().bGetPtrEffIniMng()->bWrite_txt( stSavedInfo.m_pEffSet );
	//			stSavedInfo.m_pEffSet->bRelease();
	//		}
	//		else
	//		{
	//			ToWnd( Eff2Ut_FmtMsg("ID : %d\n"
	//								"sphere=%f:%f:%f:%f\n"
	//								"box=%f:%f:%f:%f:%f:%f\n"
	//								, stSavedInfo.m_ulEffID

	//								, CALCBOUNDINFO_SPHERE->center.x
	//								, CALCBOUNDINFO_SPHERE->center.y
	//								, CALCBOUNDINFO_SPHERE->center.z
	//								, CALCBOUNDINFO_SPHERE->radius

	//								, CALCBOUNDINFO_BOX->sup.x
	//								, CALCBOUNDINFO_BOX->sup.y
	//								, CALCBOUNDINFO_BOX->sup.z
	//								, CALCBOUNDINFO_BOX->inf.x
	//								, CALCBOUNDINFO_BOX->inf.y
	//								, CALCBOUNDINFO_BOX->inf.z
	//								)
	//								);
	//			stSavedInfo.m_pEffSet->bSetBBox( *CALCBOUNDINFO_BOX );
	//			stSavedInfo.m_pEffSet->bSetBSphere( *CALCBOUNDINFO_SPHERE );
	//		}
	//	}
	//}

	//끝!
	if( G_EFFSETQUEUE.empty() )
	{
		AgcdEffGlobal::bGetInst().bFlagOff( E_GFLAG_CALCBOUND );
		stSavedInfo.End();
		return TRUE;
	}

	//init SavedInfo
	stSavedInfo.Init();

	//struct setting
	stEffUseInfo	useInfo;
	useInfo.m_pFrmParent						= CGlobalVar::bGetInst().bGetPtrFrame(CGlobalVar::e_frm_blue);
	useInfo.m_stMissileTargetInfo.m_pFrmTarget	= CGlobalVar::bGetInst().bGetPtrFrame(CGlobalVar::e_frm_red);
	useInfo.m_pFrmTarget						= CGlobalVar::bGetInst().bGetPtrFrame(CGlobalVar::e_frm_red);
	useInfo.m_fptrNoticeCB						= CB_END;
	useInfo.m_ulEffID							= G_EFFSETQUEUE.back();
	//useInfo.m_ulDelay							= 1000UL;

	////pop EffSetID
	//G_EFFSETQUEUE.pop_back();

	//show info to windowtitle
	{
		char	szInfo[128]	= "";
		sprintf( szInfo, "%d / %d, id : %d", G_EFFSETQUEUE.size(), AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_GetEffSetMng().size(), useInfo.m_ulEffID );
		::AfxGetMainWnd()->SetWindowText( szInfo );
	}

	//filltering
	LPEFFSET	pEffSet	= AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet( useInfo.m_ulEffID );
	if( !pEffSet													||
		!pEffSet->bGetVarSizeInfo().m_nNumOfBase					||

		DEF_FLAG_CHK(pEffSet->bGetFlag(),FLAG_EFFSET_MISSILE)	||
		DEF_FLAG_CHK(pEffSet->bGetFlag(),FLAG_EFFSET_ONLYTAIL)	||
		DEF_FLAG_CHK(pEffSet->bGetFlag(),FLAG_EFFSET_ONLYSOUND)	)
	{
		const char* szExtraInfo	= "FLAG_EFFSET_MISSILE";
		if( !pEffSet )
			szExtraInfo	= "AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet FAILED";
		else if( !pEffSet->bGetVarSizeInfo().m_nNumOfBase )
			szExtraInfo	= "pEffSet->bGetVarSizeInfo().m_nNumOfBase == 0";
		else if( DEF_FLAG_CHK(pEffSet->bGetFlag(),FLAG_EFFSET_ONLYTAIL) )
			szExtraInfo	= "FLAG_EFFSET_ONLYTAIL";
		else if( DEF_FLAG_CHK(pEffSet->bGetFlag(),FLAG_EFFSET_ONLYSOUND) )
			szExtraInfo	= "FLAG_EFFSET_ONLYSOUND";

		
		FIRST_BEGIN;
		Eff2Ut_TimeStampToFile("eff_바운딩정보_계산안함.txt");
		FIRST_END;

		Eff2Ut_TOFILE( "eff_바운딩정보_계산안함.txt", Eff2Ut_FmtMsg("%d, <%s>\n", useInfo.m_ulEffID, szExtraInfo ) );
		stAgcmEffectNoticeEffectProcessData	processData;
		processData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_END;
		stSavedInfo.m_bAfterUseEffSet = TRUE;

		G_EFFSETQUEUE.pop_back();
		return CB_NEXT( processData );
	}

	//infinit effect -> finit effect
	if( pEffSet->bGetLoopOpt() == e_TblDir_infinity )
		useInfo.m_ulLife	= 5000LU;	//5초로 셋팅.



	//start effect
	if( !AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->UseEffSet( &useInfo ) )
	{
		FIRST_BEGIN;
		Eff2Ut_TimeStampToFile("eff_바운딩정보_계산실패.txt");
		FIRST_END;

		Eff2Ut_TOFILE( "eff_바운딩정보_계산실패.txt", Eff2Ut_FmtMsg("%d\n", useInfo.m_ulEffID) );

		stAgcmEffectNoticeEffectProcessData	processData;
		processData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_END;
		stSavedInfo.m_bAfterUseEffSet = TRUE;

		G_EFFSETQUEUE.pop_back();
		return CB_NEXT( processData );
	}
	else
	{
		stSavedInfo.m_ulEffID	= useInfo.m_ulEffID;
		stSavedInfo.m_pEffSet	= pEffSet;
		stSavedInfo.m_bAfterUseEffSet	= TRUE;

		G_BEFFEND	= FALSE;
	}

	return TRUE;
}

void CEffToolApp::OnStopCalcbound()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_CALCBOUND ) )
		AgcdEffGlobal::bGetInst().bFlagOff( E_GFLAG_CALCBOUND );
	else
	{
		stAgcmEffectNoticeEffectProcessData stProcessData;
		stProcessData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_END;

		// FLAG ON
		AgcdEffGlobal::bGetInst().bFlagOn( E_GFLAG_CALCBOUND );
		CB_NEXT(stProcessData);
	}
}

void CEffToolApp::OnCalcboundAlleff()
{
	if( !AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2() )
		return;

	if( !G_EFFSETQUEUE.empty() )
	{
		ToWnd( Eff2Ut_FmtMsg("바운딩 정보계산중인 이펙트들이 아직 %d개 남아있습니다.", G_EFFSETQUEUE.size()) );
		return;
	}
	
	G_BSAVE	= TRUE;

	LPEffectSetMapCItr	it_curr = AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_GetEffSetMng().begin();
	LPEffectSetMapCItr	it_last = AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_GetEffSetMng().end();

	G_EFFSETQUEUE.resize( AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_GetEffSetMng().size() );
	for( int i=0; it_curr != it_last; ++it_curr, ++i )
		G_EFFSETQUEUE[G_EFFSETQUEUE.size()-i-1] = (*it_curr).first;

	stAgcmEffectNoticeEffectProcessData stProcessData;
	stProcessData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_END;

	AgcdEffGlobal::bGetInst().bFlagOn( E_GFLAG_CALCBOUND );
	AgcdEffGlobal::bGetInst().bSetCurrTime( g_MyEngine.AgcEngine::GetClockCount() );

	CB_NEXT(stProcessData);
}

void CEffToolApp::bCalcBound(UINT32 ulEffID)
{
	if( !AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2() )
		return;

	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_CALCBOUND ) )
	{
		ToWnd( "다른 이펙트의 바운딩을 계산하고 있습니다." );
		return;
	}
	if( !G_EFFSETQUEUE.empty() )
	{
		ToWnd( Eff2Ut_FmtMsg("아직 %d개 남아있습니다.", G_EFFSETQUEUE.size()) );
		return;
	}

	LPEFFSET	pEffSet	= AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->bForTool_FindEffSet( ulEffID );
	if( !pEffSet )
	{
		ToWnd( Eff2Ut_FmtMsg("ID : %d 를 찾을 수 없습니다.\n", ulEffID) );
		return;
	}

	G_BSAVE	= FALSE;
	
	G_EFFSETQUEUE.push_back(ulEffID);

	stAgcmEffectNoticeEffectProcessData stProcessData;
	stProcessData.lEffectProcessType = AGCMEFF2_PROCESS_TYPE_EFFECT_END;

	AgcdEffGlobal::bGetInst().bFlagOn( E_GFLAG_CALCBOUND );
	AgcdEffGlobal::bGetInst().bSetCurrTime( g_MyEngine.AgcEngine::GetClockCount() );

	CB_NEXT(stProcessData);
};

void setRwLightColr(RpLight* light)
{
	if( !light )
		return;

	COLORREF clrInit
	(((DWORD)(RpLightGetColor(light)->red * 255.f))			|
	(((DWORD)(RpLightGetColor(light)->green * 255.f)) << 8)	|
	(((DWORD)(RpLightGetColor(light)->blue * 255.f)) << 16)	);	

	CColorDialog	dlgColr(clrInit, CC_FULLOPEN);//0x00bbggrr 
	if(dlgColr.DoModal() == IDOK )
	{
		COLORREF	colr = dlgColr.GetColor();
		FLOAT	blue	= (float)( ( colr & 0x00ff0000 ) >> 16  ) / 255.f;
		FLOAT	green	= (float)( ( colr & 0x0000ff00 ) >> 8   ) / 255.f;
		FLOAT	red		= (float)( ( colr & 0x000000ff )		) / 255.f;

		RwRGBAReal	rwcolr	= { red, green, blue, 1.f };

		RpLightSetColor(light, &rwcolr);
	}
}
void CEffToolApp::OnAmbientLight()
{
	setRwLightColr(g_MyEngine.m_pLightAmbient);
}

void CEffToolApp::OnEngineLight()
{
	setRwLightColr(g_MyEngine.m_pLightDirect);
}

BOOL CEffToolApp::OnIdle(LONG lCount) 
{
	m_cRenderWare.Idle();

	if( AgcdEffGlobal::bGetInst().bFlagChk( E_GFLAG_CALCBOUND ) )
	{
		if( G_BEFFEND )
		{
			stAgcmEffectNoticeEffectProcessData stProcessData;
			CB_NEXT( stProcessData, NULL );
		}

		if( G_EFFSETQUEUE.empty() )
		{
			AgcdEffGlobal::bGetInst().bFlagOff( E_GFLAG_CALCBOUND );
		}
	}

	return CWinApp::OnIdle(lCount/*lCount*/);
}
