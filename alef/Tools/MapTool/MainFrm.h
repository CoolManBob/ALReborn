// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__2B55D17D_3143_40DE_A293_61DF470CF205__INCLUDED_)
#define AFX_MAINFRM_H__2B55D17D_3143_40DE_A293_61DF470CF205__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"
#include "AlefMapDocument.h"

#include "UIMiniMapWnd.h"
#include "UIOutputWnd.h"
#include "UITileListWnd.h"
#include "AlefToolBar.h"
#include "HtmlWnd.h"
#include "DungeonWnd.h"



#define MAX_CONSOLE_COMMAND_ALIAS	3
class CConsoleCommand
{
public:
	CString	strAlias[ MAX_CONSOLE_COMMAND_ALIAS ];
	INT32	nCommandIndex	;
	INT32	nArgumentCount	;	// 인자갯수. 0이 없는것.
	CString	strHelp			;	// 헬프..
	BOOL	bHide			;	// 비밀커맨드.

	CConsoleCommand()
	{
		nArgumentCount	= 0		;
		bHide			= FALSE	;
	}
};

class CExportDlg;

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)
			
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	CChildView* GetChildView();
	virtual     ~CMainFrame();
	bool        CheckMenuItem(CMenu* pMenu, UINT uiMenuID);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void		OnToolBarMenuSelected( int index );
	
	WINDOWPLACEMENT	m_WPPrev				;
	CRect			m_FullScreenWindowRect	;

public:
	// Operations
	CAlefMapDocument * GetDocument();	// 현재 상태 얻기.

	RwRGBA				GetVertexColor	();
	void				SetWindowSize	( int cx , int cy );
	UINT32				Save			();
	UINT32				Load			();

	BOOL				LoadDivision	( INT32 nDivision );
	BOOL				ExportDivision	( INT32 nDivision , CExportDlg * pDlg , char * pDestinationDirectory = NULL );

	BOOL				ExportTile		();
	BOOL				ExportDivisions	( int *pDivisions , int nDivisionCount
										, BOOL bMapDetail	
										, BOOL bMapRough		
										, BOOL bTile			
										, BOOL bCompactData	
										, BOOL bObject		
										, BOOL bMinimapExport	
										, BOOL bServerDataExport	);

public:
	// 마고자 (2004-05-11 오후 4:27:28) : Console 커맨드 관련..
	AuList< CConsoleCommand >	m_listCommand;
	BOOL	AddConsoleCommand( INT32 nIndex , char *pAlias1 , char *pAlias2 , char *pAlias3 , INT32 nArgument , char * pHelp , BOOL bHide = FALSE );
	
public:  // control bar embedded members
	// Attributes
	enum	ViewMode
	{
		MT_VM_NORMAL		,
		MT_VM_FULLSCREEN	,
		MT_VM_DUNGEON		,
		MT_VM_MAX
	};

	UINT32				m_bFullScreen	;

	CUIMiniMapWnd	*	m_pMiniMap		;
	CUIOutputWnd	*	m_pOutputWnd	;
	CUITileListWnd	*	m_pTileList		;
	CAlefToolBar	*	m_pToolBar		;
	CHtmlWnd		*	m_pHtmlWnd		;
	CStatusBar			m_wndStatusBar	;
	CToolBar			m_wndToolBar	;
	
	CDungeonWnd		*	m_pDungeonWnd	;
	CDungeonToolBar	*	m_pDungeonToolbar;

	// 메인뷰창.
	CChildView			m_wndView		;
	
public:
	CAlefMapDocument	m_Document		;	// 현 프로그램 설정을 저장함.

	// 유틸리티 펑션.
	void	RefreshObjectBlocking	();
	void	RoughTextureExport		();

	void	SetCurrentDetail		( int nDetail );
	// 디테일을 바꿀때 메뉴의 정보도 같이 변경된다.

	void	OnFileMinimapexportSmallSize();
		
	// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnNewWorld();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSave();
	afx_msg void OnRevert();
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnRevertCurrent();
	afx_msg void OnUpdateNormal();
	afx_msg void OnClose();
	afx_msg void OnAdjustGeometryHeight();
	afx_msg void OnModechangeGeometry();
	afx_msg void OnModechangeObject();
	afx_msg void OnModechangeSubdivision();
	afx_msg void OnModechangeTile();
	afx_msg void OnGridToggle();
	afx_msg void OnCameraFirstPerson();
	afx_msg void OnCameraReset();
	afx_msg void OnTileSpoidBack();
	afx_msg void OnTileSpoidAlpha();
	afx_msg void OnTileSpoidAll();
	afx_msg void OnBspExport();
	afx_msg void OnTongMapHapChiGi();
	afx_msg void OnHelpKeyreference();
	afx_msg void OnGenerateRoughmap();
	afx_msg void OnOptimizeAdjustEdge();
	afx_msg void OnCreatesectorDetail();
	afx_msg void OnCreatesector();
	afx_msg void OnToggleCameraProjectiontype();
	afx_msg void OnUpdateAll();
	afx_msg void OnEditCreateAlpha();
	afx_msg void OnExportTile();
	afx_msg void OnSmoothGeometry();
	afx_msg void OnEditCreateAllSector();
	afx_msg void OnBrushSizeDown();
	afx_msg void OnBrushSizeUp();
	afx_msg void OnFillTile();
	afx_msg void OnOpenOptionDialog();
	afx_msg void OnToggleFpView();
	afx_msg void OnRefreahObjectBlocking();
	afx_msg void OnViewToggleObject();
	afx_msg void OnViewClipplane();
	afx_msg void OnSkySetting();
	afx_msg void OnChangeFpsEditMode();
	afx_msg void OnOpenShortCut();
	afx_msg void OnToggleTimer();
	afx_msg void OnTongMapApply();
	afx_msg void OnObjectRefresh();
	afx_msg void OnAddWater();
	afx_msg void OnShowGridMenuSelect(UINT nID);
	afx_msg void OnShowBlocking(UINT nID);
	afx_msg void OnDWSectorControl(UINT nID);	
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFullScreenMode();
	afx_msg void OnModechangeGeometryReverse();
	afx_msg void OnModechangeTileReverse();
	afx_msg void OnViewSkyTemplate();
	afx_msg void OnViewWaterdlg();
	afx_msg void OnTimeFaster();
	afx_msg void OnTimeSlower();
	afx_msg void OnPrelightAdjust();
	afx_msg void OnEditCreateCollision();
	afx_msg void OnClientFpsView();
	afx_msg void OnSkyValueChange();
	afx_msg void OnTestAdd();
	afx_msg void OnTestSub();
	afx_msg void OnViewProjection();
	afx_msg void OnFileMinimapexport();
	afx_msg void OnFileClosetempfile();
	afx_msg void OnFileObjectExportForServer();
	afx_msg void OnFileDdsConvert();
	afx_msg void OnCleanupUnusedTexture();
	afx_msg void OnViewAtomicfilter();
	afx_msg void OnEventnatureFilter();
	afx_msg void OnEditUpdatecollision();
	afx_msg void OnModeDungeonToggle();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCameraPositionSave();
	afx_msg void OnCameraPositionLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CDungeonWnd;
public:

	INT32	m_nFindObjectEventIndex	;
	INT32	m_nLastSearchObject		;

	afx_msg void OnFindobject();
	afx_msg void OnFindobjectnext();
	afx_msg void OnObjectpopGroupsave();
	afx_msg void OnObjectpopGroupLoad( RwV3d * pPos );
	afx_msg void OnSelectAll();

	void	CleanUp();
	afx_msg void OnViewRoughdisplay();

	static BOOL	CBSeverDataExport( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	CBSeverDataLoadTest( DivisionInfo * pDivisionInfo , PVOID pData );

	BOOL	ExportServerData( INT32 nDivision , char * pDestinationDirectory = NULL ,
							ProgressCallback pfCallback = NULL , void * pData = NULL );

	int		CollectVertex(  ApWorldSector * pSector , vector< RwV3d > * pVertexVector , vector< INT32 > * pObjectList );
};


// 아웃풋 창에 메시지를 출력함.
void	DisplayMessage( char *pFormat , ... 					);
void	DisplayMessage( COLORREF color , char *pFormat , ...	);
void	DisplayMessage( CString str								);
void	DisplayMessage( COLORREF color , CString str			);

void	Maptool_ErrorCheck();

/////////////////////////////////////////////////////////////////////////////

BOOL _ProgressCallback( char * strMessage , int nCurrent , int nTarget , void *data );

BOOL	_CreateDirectory( char *pFormat , ... );

extern CMainFrame *		g_pMainFrame ;

// 환경 변수들.
#define	MAP_UI_MARGIN		5	
#define	MINIMAP_WIDTH		100	
#define	TILELIST_WIDTH		300	
#define TOOLBAR_HEIGHT		25	

INT32	GetLeftTopDivisionIndex();
#define	PROCESSIDLE()	{g_pEngine->SetTickCount( g_pEngine->GetTickCount() + 1 ); g_pEngine->Idle( g_pEngine->GetTickCount() ); }

#define	CRASH_SAFE_MESSAGE( strMessage ) catch( ... ) { \
	CString	str; \
	str.Format( "'%s' 작업시 크래시 발생! 보고요망 , 그리고 프로그램 죽이실래요? 작업에 문제 있을지 모름" , strMessage ); \
	if( IDYES == g_pMainFrame->MessageBox( str , "맵툴에러" , MB_ICONERROR | MB_YESNOCANCEL ) ) throw; }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__2B55D17D_3143_40DE_A293_61DF470CF205__INCLUDED_)
