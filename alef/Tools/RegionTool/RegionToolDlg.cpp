// RegionToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"

#include "GoToDlg.h"
#include "TemplateEditDlg.h"
#include "PathDialog.h"
#include "LoginDlg.h"
#include "UploadDataDlg.h"

#include "AgpmBillInfo.h"

#define POSITIONLOG

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MyEngine	g_MyEngine;

ApmMap			* g_pcsApmMap			= NULL;
ApmObject		* g_pcsApmObject		= NULL;
ApmEventManager	* g_pcsApmEventManager	= NULL;
AgpmEventNature	* g_pcsAgpmEventNature	= NULL;

AgpmEventSpawn		* g_pcsAgpmEventSpawn	= NULL;
AgcmEventSpawnDlg	* g_pcsAgcmEventSpawnDlg= NULL;

AgpmTimer		* g_pcsAgpmTimer		= NULL;
AgpmCharacter	* g_pcsAgpmCharacter	= NULL;
ApmEventManagerDlg	* g_pcsApmEventManagerDlg = NULL;

AgpmItem		*	g_pcsAgpmItem		= NULL;
AgpmSkill		*	g_pcsAgpmSkill		= NULL;
AgpmDropItem	*	g_pcsAgpmDropItem	= NULL;
AgcmFactorsDlg	*	g_pcsAgcmFactorsDlg	= NULL;
AgpmAI2			*	g_pcsAgpmAI2		= NULL;
AgpmGrid		*	g_pcsAgpmGrid		= NULL;	
AgpmFactors		*	g_pcsAgpmFactors	= NULL;
AgpmPathFind	*	g_pcsAgpmPathFind	= NULL;	
AgpmBillInfo	*	g_pcsAgpmBillInfo	= NULL;	


//AgcmEventNature		*	g_pcsAgcmEventNature	= NULL;
//AgcmEventNatureDlg	*	g_pcsAgcmEventNatureDlg	= NULL;

BOOL	g_bTeleportOnlyMode = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About


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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionToolDlg dialog

CRegionToolDlg::CRegionToolDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRegionToolDlg::IDD, pParent) , m_pRegionMenu( NULL )
{
	//{{AFX_DATA_INIT(CRegionToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	strcpy( m_strWorkingFolder , "" );
}

void CRegionToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegionToolDlg)
	DDX_Control(pDX, IDC_REGIONVIEW, m_ctlRegion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegionToolDlg, CDialog)
	//{{AFX_MSG_MAP(CRegionToolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionToolDlg message handlers

BOOL CRegionToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	POSITIONLOG	

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// 상위 디렉토리로 이동
	{
		FILE * pFile;
		pFile = fopen( "Region.ini" , "rt" );
		if( pFile )
		{
			// 여기는 Regiontool 디렉토리다.
			// 상위 디렉토리로 이동.
			SetCurrentDirectory( ".." );
			fclose( pFile );
		}
	}

	POSITIONLOG

	if( g_MyEngine.OnRegisterModule() )
	{
	}
	else
	{
		TRACE( "모듈추가 실패\n" );
		return FALSE;
	}

	POSITIONLOG

	if( g_MyEngine.Initialize() )
	{
		TRACE( "모듈메니져 초기화 성공\n" );
	}
	else
	{
		TRACE( "모듈메니져 초기화 실패\n" );
		return FALSE;
	}

	POSITIONLOG

	g_bTeleportOnlyMode = FALSE;

	/*
	switch( MessageBox( "텔레포트 전용으로 구동할래요?" , "리젼툴" , MB_YESNOCANCEL ) )
	{
	case IDYES:
		g_bTeleportOnlyMode = TRUE;
		break;
	case IDNO:
		g_bTeleportOnlyMode = FALSE;
		break;
	default:
		ExitProcess( 1 );
		break;
	}
	*/


	/*
	if( FALSE == g_bTeleportOnlyMode )
	{
		//////////////////////////////////////////////////////////////////////////
		// 리소스키퍼 처리!
		//////////////////////////////////////////////////////////////////////////
		if( LoadInfoFile( m_strWorkingFolder ) )
		{
			// Do nothing.
		}
		else
		{
			// 워킹폴더 지정하는 인터페이스 필요함..
			// 

			CString	strCaption	= "워킹폴더를 지정하세요";
			CString	strTitle	= "리젼툴 리소스 키퍼";

			GetCurrentDirectory( MAX_PATH , m_strWorkingFolder );

			CPathDialog	dlg( strCaption , strTitle , m_strWorkingFolder );

			if( IDOK == dlg.DoModal() )
			{
				strcpy( m_strWorkingFolder , ( LPCTSTR ) dlg.GetPathName() );
			}
			else
			{
				MessageBox( "워킹폴더 지정안하면 작업 못해염!" , "리젼툴" );
				return FALSE;
			}

			SaveInfoFile( m_strWorkingFolder );
		}
		
		TRACE( "Working Folder = '%s'\n" , m_strWorkingFolder );

		// 로긴 과정..
		// 
		CLoginDlg	dlgLogin;

		dlgLogin.m_strWorkingFolder	= m_strWorkingFolder;
		dlgLogin.m_strRepository	= RM_RK_DIRECTORY	;

		NETRESOURCE NetInfo;
		NetInfo.dwType			= RESOURCETYPE_ANY;
		NetInfo.lpRemoteName	= const_cast<CHAR*>(RM_RK_DIRECTORY);
		NetInfo.lpLocalName		= NULL;
		NetInfo.lpProvider		= NULL;

		// 접속 끊어버림!
		WNetCancelConnection2( const_cast<CHAR*>(RM_RK_DIRECTORY) , CONNECT_UPDATE_PROFILE , TRUE );

		DWORD	dwReturn;
		switch( dwReturn = WNetAddConnection2(&NetInfo, RM_RK_USER_PASSWORD, RM_RK_USER_ID, CONNECT_UPDATE_PROFILE) )
		{
		case NO_ERROR:
			TRACE( "접속 성공.." );
			break;

		default:
			{
				CString	strError;
				switch( dwReturn )
				{
				case ERROR_ACCESS_DENIED				:	strError = "ERROR_ACCESS_DENIED				"; break;
				case ERROR_ALREADY_ASSIGNED				:	strError = "ERROR_ALREADY_ASSIGNED			"; break;
				case ERROR_BAD_DEV_TYPE					:	strError = "ERROR_BAD_DEV_TYPE				"; break;
				case ERROR_BAD_DEVICE					:	strError = "ERROR_BAD_DEVICE				"; break;
				case ERROR_BAD_NET_NAME					:	strError = "ERROR_BAD_NET_NAME				"; break;
				case ERROR_BAD_PROFILE					:	strError = "ERROR_BAD_PROFILE				"; break;
				case ERROR_CANNOT_OPEN_PROFILE			:	strError = "ERROR_CANNOT_OPEN_PROFILE		"; break;
				case ERROR_DEVICE_ALREADY_REMEMBERED	:	strError = "ERROR_DEVICE_ALREADY_REMEMBERED	"; break;
				case ERROR_EXTENDED_ERROR				:	strError = "ERROR_EXTENDED_ERROR			"; break;
				case ERROR_INVALID_PASSWORD				:	strError = "ERROR_INVALID_PASSWORD			"; break;
				case ERROR_NO_NET_OR_BAD_PATH			:	strError = "ERROR_NO_NET_OR_BAD_PATH		"; break;
				case ERROR_NO_NETWORK					:	strError = "ERROR_NO_NETWORK				"; break;

				default:									strError = "unknown Error!"; break;
				}

				CString	strMessage;
				strMessage.Format( "AlefServer 와 네트워크 접속 중에 '%s' 에러가 발생하였습니다. 문의바래요." ,  strError );
				MessageBox ( strMessage , "Region Tool" , MB_ICONERROR | MB_OK);
				return FALSE;
			}
			break;
		}

		CString strUserName;
		{
			CRegKey reg;
			CString str;
			str.Format( "Software\\RegionTool" );
			reg.Create( HKEY_LOCAL_MACHINE , str );

			char	strValue[ 1024 ];
			DWORD	size ;

			size = 1024;

			if( reg.QueryValue( strValue ,	"UserName" , & size ) == ERROR_SUCCESS )
			{
				strUserName = strValue;
			}
			else
			{
				strUserName = "유져이름 넣어주세요";
			}
		}

		dlgLogin.m_strID = strUserName;
		
		if( IDOK == dlgLogin.DoModal() )
		{
			TRACE( "로그인 성공..\n" );
			m_strUserName	= dlgLogin.m_strID;

			CRegKey reg;
			CString str;
			str.Format( "Software\\RegionTool" );
			reg.Create( HKEY_LOCAL_MACHINE , str );
			reg.SetValue( m_strUserName	, "UserName" );
		}
		else
		{
			// 로그인 실패..
			ExitProcess( 1 );
			return TRUE;
		}
	}
	*/

	// 타이틀 수정
	{
		CString	strTitle;
		
		if( FALSE == g_bTeleportOnlyMode )
			strTitle = "RegionTool";
		else
			strTitle = "RegionTool - 텔레포트 전용";

		#ifdef _DEBUG
		strTitle += " Debug Build";
		#else
		strTitle += " Release Build";
		#endif

		// 날자 삽입..
		char strFilename[ 1024 ];
		GetModuleFileName( NULL , strFilename , 1024 );
		CFileStatus	fs;
		if( CFile::GetStatus( strFilename , fs ) )
		{
			CString strDate;
			strDate.Format( " %04d/%02d/%02d" , fs.m_mtime.GetYear() , fs.m_mtime.GetMonth() , fs.m_mtime.GetDay() );
			strTitle += strDate;
		}

		SetWindowText( strTitle );
	}

	//////////////////////////////////////////////////////////////////////////
	// 데이타 로딩 과정

	CRect	rect;
	GetClientRect( rect );
	m_ctlRegion.MoveWindow( rect );

	// 시작위치를 튤란으로..
	m_ctlRegion.MoveTo( 2125 );

	m_pRegionMenu = new CRegionMenuDlg;
	if( m_pRegionMenu->Create( this ) )
	{
		// ok
	}
	else
	{
		TRACE( "-_-;;\n" );
	}

	// 풀스크린..
	ShowWindow( SW_MAXIMIZE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRegionToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegionToolDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRegionToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRegionToolDlg::OnOK() 
{		
	//CDialog::OnOK();
}

BOOL CRegionToolDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	if( WM_MBUTTONDOWN == pMsg->message )
	{
		CRTMenuBase::m_fScale = 1.0f;
		m_ctlRegion.SetZoom( CRTMenuBase::m_fScale );
	}
	else
	if( WM_KEYDOWN == pMsg->message )
	{
		char	nChar = ( char ) pMsg->wParam;

		switch( pMsg->wParam )
		{
		case VK_DELETE:
			{
				this->m_pRegionMenu->OnDeleteKeyDown();
			}
			break;
		case 'G':
			{
				CGoToDlg dlg;

				if( IDOK == dlg.DoModal() )
				{
					m_ctlRegion.MoveTo( ( int ) dlg.m_uDivision );
				}	

				return TRUE;
			} 
			
		case VK_ADD:
			{
				FLOAT	fScale = m_ctlRegion.GetZoom();
				fScale *= 2.0f;
				CRTMenuBase::m_fScale = fScale;
				m_ctlRegion.SetZoom( fScale );
			}
			break;
		case VK_SUBTRACT:
			{
				FLOAT	fScale = m_ctlRegion.GetZoom();
				fScale /= 2.0f;

				if( fScale < 0.5f ) break;

				CRTMenuBase::m_fScale = fScale;
				m_ctlRegion.SetZoom( fScale );
			}
			break;

		case VK_SHIFT:
		case VK_CONTROL:
			Invalidate( FALSE );
			break;
		default:
			{
				if( '1' <= nChar && nChar <= '9' &&
					nChar - '1' < ( int ) this->m_pRegionMenu->m_vectorMenuWnd.size() )
				{
					this->m_pRegionMenu->SelectMenu( nChar - '1' );
				}
			}
			break;
		}
	}
	else
	if( WM_KEYUP == pMsg->message )
	{
		switch( pMsg->wParam )
		{
		case VK_SHIFT:
		case VK_CONTROL:
			Invalidate( FALSE );
			break;
		}
	}

	
	return CDialog::PreTranslateMessage(pMsg);
}

void CRegionToolDlg::OnCancel() 
{
	if( FALSE == g_bTeleportOnlyMode )
	{
		if( this->m_pRegionMenu )
		{
			if( this->m_pRegionMenu->IsAvailableUnSavedData() )
			{
				if( IDYES == MessageBox( "세이브돼지 않은 데이타가 있습니다. 종료하시겠습니까?" , "리젼툴" , MB_YESNOCANCEL) )
				{
					CDialog::OnCancel();
				}
			}
			else
			{
				if( MessageBox( "정말 닫을꺼슈?" , "RegionTool" , MB_YESNO) == IDYES )
					CDialog::OnCancel();
			}
		}
	}
	else
	{
		if( MessageBox( "정말 닫을꺼슈?" , "RegionTool" , MB_YESNO) == IDYES )
			CDialog::OnCancel();
	}
}

BOOL MyEngine::OnRegisterModule()
{
	// 모듈 추가 ..
	REGISTER_MODULE( g_pcsApmMap			, ApmMap			)
	REGISTER_MODULE( g_pcsApmObject			, ApmObject			)
	REGISTER_MODULE( g_pcsAgpmTimer			, AgpmTimer			)
	REGISTER_MODULE( g_pcsAgpmCharacter		, AgpmCharacter		)
	REGISTER_MODULE( g_pcsApmEventManager	, ApmEventManager	)
	REGISTER_MODULE( g_pcsAgpmEventNature	, AgpmEventNature	)

	REGISTER_MODULE( g_pcsAgpmGrid			, AgpmGrid			)
	REGISTER_MODULE( g_pcsAgpmFactors		, AgpmFactors		)

	
	REGISTER_MODULE( g_pcsAgpmBillInfo		, AgpmBillInfo		)
	REGISTER_MODULE( g_pcsAgpmItem			, AgpmItem			)
	REGISTER_MODULE( g_pcsAgpmSkill			, AgpmSkill			)
	REGISTER_MODULE( g_pcsAgpmDropItem		, AgpmDropItem		)
	REGISTER_MODULE( g_pcsAgcmFactorsDlg	, AgcmFactorsDlg	)

	REGISTER_MODULE( g_pcsAgpmEventSpawn	, AgpmEventSpawn	)
	REGISTER_MODULE( g_pcsAgpmPathFind		, AgpmPathFind		)
	
	REGISTER_MODULE( g_pcsAgpmAI2			, AgpmAI2			)

	REGISTER_MODULE( g_pcsApmEventManagerDlg, ApmEventManagerDlg)
	REGISTER_MODULE( g_pcsAgcmEventSpawnDlg	, AgcmEventSpawnDlg	)

//	REGISTER_MODULE( g_pcsAgcmEventNature		, AgcmEventNature	)
//	REGISTER_MODULE( g_pcsAgcmEventNatureDlg	, AgcmEventNatureDlg)

	if( g_pcsApmObject )
	{
		g_pcsApmObject->SetMaxObject			(100000);
		g_pcsApmObject->SetMaxObjectTemplate	(5000);
	}

	if( g_pcsAgpmCharacter )
	{
		g_pcsAgpmCharacter->SetMaxCharacterTemplate	( 1000	);
		g_pcsAgpmCharacter->SetMaxCharacter			( 1000	);
	}

	if( g_pcsAgpmItem )
	{
		g_pcsAgpmItem->SetMaxItem				( 3000	);
		g_pcsAgpmItem->SetMaxItemTemplate		( 20000	);
		g_pcsAgpmItem->SetMaxItemRemove			( 3000	);
		g_pcsAgpmItem->SetMaxItemOptionTemplate	( 2000	);
	}

	return TRUE;
}

void CRegionToolDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// 모듈 디스트로이 ..
	g_MyEngine.Destroy();	

	::PostQuitMessage( 1 );

	if( m_pRegionMenu )
	{
		m_pRegionMenu->DestroyWindow();
		delete m_pRegionMenu;
	}

}

void CRegionToolDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if( m_ctlRegion.GetSafeHwnd() )
		m_ctlRegion.MoveWindow( 0 , 0 , cx , cy );
}

void CRegionToolDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CRegionToolDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// 줌기능 사용하지 않음,
	if( zDelta > 0)
	{
		FLOAT	fScale = m_ctlRegion.GetZoom();
		fScale *= 1.2f;
		CRTMenuBase::m_fScale = fScale;
		m_ctlRegion.SetZoom( fScale );
	}
	else
	{
		FLOAT	fScale = m_ctlRegion.GetZoom();
		fScale /= 1.2f;

		if( fScale >= 0.5f )
		{
			CRTMenuBase::m_fScale = fScale;
			m_ctlRegion.SetZoom( fScale );
		}
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CRegionToolDlg::DownloadResource(CHAR *pszPath)
{
	/*
//	if (!CResourceManagerDlg::GetInstance())
//		return E_AC_RETURN_TYPE_FAILURE;
//
//	if (E_AC_RETURN_TYPE_FAILURE == Log_In())
//	{
//		CResourceManagerDlg::GetInstance()->AddOutput("Failed - Log_In()");
//		return E_AC_RETURN_TYPE_FAILURE;
//	}

	CList<FileInfo, FileInfo>	csUpdatedFileList, csNewFileList;
	POSITION					stPos;
//	CHAR						szFileName[256];
	CHAR						szOutput[256];

//	memset(szFileName, 0, sizeof (CHAR) * 256);
	memset(szOutput, 0, sizeof (CHAR) * 256);

	if (GetNewFileList2(&csNewFileList, pszPath))
	{
		stPos	= csNewFileList.GetHeadPosition();

		while (stPos)
		{
			FileInfo	stInfo = csNewFileList.GetNext(stPos);

//			if (E_AC_RETURN_TYPE_FAILURE == SearchWildCards((LPSTR)(LPCSTR)(stInfo.m_strFileName), pcsWildCards))
//				continue;

//			GetFileName((LPSTR)(LPCSTR)(stInfo.m_strFileName), szFileName);

			if (IsLatestFile(stInfo.m_strFileName))
				continue;

			if (FileDownLoad(stInfo.m_strFileName))
			{
				sprintf(szOutput, "Succeeded - %s\n", (LPSTR)(LPCSTR)(stInfo.m_strFileName) + 3);
				TRACE(szOutput);
			}
			else
			{
				sprintf(szOutput, "Failed - %s\n", (LPSTR)(LPCSTR)(stInfo.m_strFileName) + 3);
				TRACE(szOutput);
			}
		}
	}

	if (GetUpdatedFileList2(&csUpdatedFileList, pszPath))
	{
		stPos	= csUpdatedFileList.GetHeadPosition();

		while (stPos)
		{
			FileInfo	stInfo = csUpdatedFileList.GetNext(stPos);

//			if (E_AC_RETURN_TYPE_FAILURE == SearchWildCards((LPSTR)(LPCSTR)(stInfo.m_strFileName), pcsWildCards))
//				continue;

			if (FileDownLoad(stInfo.m_strFileName))
			{
				sprintf(szOutput, "Succeeded - %s\n", (LPSTR)(LPCSTR)(stInfo.m_strFileName) + 3);
				TRACE(szOutput);
			}
			else
			{
				sprintf(szOutput, "Failed - %s\n", (LPSTR)(LPCSTR)(stInfo.m_strFileName) + 3);
				TRACE(szOutput);
			}
		}
	}

	*/
	return TRUE;
}

BOOL	_CreateDirectory( char *pFormat , ... )
{
	char	strtmp[8192];
	va_list vParams;

	va_start	( vParams,		pFormat				);
	vsprintf	( strtmp	,	pFormat,	vParams	);
	va_end		( vParams							);

	return CreateDirectory( strtmp , NULL );
}
