#include "StdAfx.h"

#ifdef _JPN

#include "JapanPatchClient.h"

#ifdef _AREA_JAPAN_
	#include "AuJapaneseClientAuth.h"
#endif

#include "Hyperlinks.h"

JapanPatchClient::JapanPatchClient( CDialog* pDlg )
:	BasePatchClient( pDlg )
{
}

JapanPatchClient::~JapanPatchClient(void)
{
}

BOOL JapanPatchClient::OnInitDialog( VOID )
{
	wchar_t strMsg[ 256 ];


	m_cPatchClientLib.LoadReferenceFile();

	//. Force setting Dialog size/position
	int nPosX = GetSystemMetrics ( SM_CXSCREEN );
	int nPosY = GetSystemMetrics ( SM_CYSCREEN );

	nPosX = ( nPosX / 2 ) - 296;
	nPosY = ( nPosY / 2 ) - 199;

	//. window size locale string table resource.
	m_pDlg->MoveWindow( nPosX , nPosY, 
						GetIntFromStringTable(IDS_WIDTHSIZE), 
						GetIntFromStringTable(IDS_HEIGHTSIZE));

	m_cDrawFont.CreateFont       ( 13, 0, 0, 0, FW_BOLD,   0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS PGothic" );
	m_cDrawTitle.CreateFont      ( 15, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS PGothic" );
	m_cDrawFontInfo.CreateFont   ( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS PGothic" );
	m_cDrawHyperLink.CreateFont  ( 13, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS PGothic" );
	m_cDrawProgressMsg.CreateFont( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS PGothic" );


	m_pDlg->SetWindowText( "Archlord Patch Client" );

	m_cOldPoint.x = -1;
	m_cOldPoint.y = -1;

	m_cBMPBackground.LoadBitmap( IDB_BASEBACKGROUND );

	// 3개중에 하나를 임의로 보여주는 배경 그림 로드
	unsigned int	nRandomNum					= (timeGetTime() % 3) + 1;
	char			backImgName[_MAX_PATH]		= {0, };

	switch( nRandomNum )
	{
	case 1 :	m_cBmpBackgroundEx.LoadBitmap( IDB_EXTRABACKGROUND01 );		break;
	case 2 :	m_cBmpBackgroundEx.LoadBitmap( IDB_EXTRABACKGROUND02 );		break;
	case 3 :	m_cBmpBackgroundEx.LoadBitmap( IDB_EXTRABACKGROUND03 );		break;
	}

	m_cBMPBackgroundEx = ( HBITMAP )MAKEINTRESOURCE( IDB_EXTRABACKGROUND01 );

	//CString	strJoin;
	CString	strOption;
	//CString	strHomepage;
	CString	strStart;

	//strJoin.LoadString(IDS_MSG_JOIN);
	strOption.LoadString(IDS_MSG_OPTION);
	//strHomepage.LoadString(IDS_MSG_HOMEPAGE);
	strStart.LoadString(IDS_MSG_START);


	m_cKbcExitButton.SetBmpButtonImage( IDB_Button_Close_Set, RGB(0,0,255) );
	m_cKbcOptionButton.SetBmpButtonImage( IDB_BUTTON_OPTION_SET, RGB(0,0,255) );
	m_cKbcStartgameButton.SetBmpButtonImage( IDB_BUTTON_STARTGAME_SET, RGB(0,0,255) );

	CWnd *pcTempWnd = 0;

	pcTempWnd = m_pDlg->GetDlgItem( IDC_EXitButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_EXITBTNPOSX), 
		GetIntFromStringTable(IDS_EXITBTNPOSY), 
		16, 
		16, 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_OPTIONBTNPOSX), 
		GetIntFromStringTable(IDS_OPTIONBTNPOSY), 
		GetIntFromStringTable(IDS_OPTIONBTNWIDTH), 
		GetIntFromStringTable(IDS_OPTIONBTNHEIGHT), 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_STARTBTNPOSX), 
		GetIntFromStringTable(IDS_STARTBTNPOSY), 
		GetIntFromStringTable(IDS_STARTBTNWIDTH), 
		GetIntFromStringTable(IDS_STARTBTNHEIGHT), 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_Static_DetailInfo );
	CStatic* pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawProgressMsg);
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_PROGRESSMSG1X), 
		GetIntFromStringTable(IDS_PROGRESSMSG1Y), 
		GetIntFromStringTable(IDS_PROGRESSMSGWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSMSGHEIGHT), 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem(	IDC_Static_Status );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawProgressMsg);
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_PROGRESSMSG1X), 
		GetIntFromStringTable(IDS_PROGRESSMSG2Y), 
		GetIntFromStringTable(IDS_PROGRESSMSGWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSMSGHEIGHT), 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_Static_Progress_Percent);
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawProgressMsg);
	pcTempWnd->SetWindowPos( NULL, 2000, 4000, 20, 10, SWP_SHOWWINDOW);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_HOMEPAGE );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 284, 27, 79, 12, SWP_SHOWWINDOW );
	//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_HOMEPAGE);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_JOIN );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 373, 27, 52, 12, SWP_SHOWWINDOW );
	//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_JOIN);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_CUSTOMER );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 442, 27, 73, 12, SWP_SHOWWINDOW );
	//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_CUSTOMER);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_DRIVER );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 528, 27, 60, 12, SWP_SHOWWINDOW );
	//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_DRIVER);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_LINE );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 364, 27, 200, 12, SWP_SHOWWINDOW );

	//ini\\COption.ini를 읽는다.
	m_cPatchOptionFile.Init();
	m_cPatchOptionFile.Load( PATCHCLIENT_OPTION_FILENAME );

	CCreateContext pContext;
	CWnd* pFrameWnd = m_pDlg;

	pContext.m_pCurrentDoc = NULL;

	pContext.m_pNewViewClass = RUNTIME_CLASS(CTestHtmlView);

	m_pWeb =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);

	ASSERT(m_pWeb);

	m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL, 0, 0 );
	m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR, 0, 0 );
	m_pWeb->SetWindowPos(NULL,20,200,424,128,NULL);
	m_pWeb->ShowWindow(SW_HIDE);
	m_pWeb->Navigate(m_cPatchOptionFile.GetNoticeURL(), 0, NULL, NULL);		


	//웹 페이지가 뜰 시간 0.5초간 기다려준다.
	Sleep( 500 );

	//TempDir을 세팅해준다.
	if( m_cPatchClientLib.SetTempDir( "Temp" ) == false )
	{
		::LoadStringW(g_resInst, IDS_ERROR_MAKE_PATCH_FOLDER, strMsg, 256);
		MessageBoxW( m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
		return FALSE;
	}

	//. update/dowinload Progress bar 
	CRect			cTempRect;

	//. update progress bar.
	::SetRect( &cTempRect, 
		GetIntFromStringTable(IDS_PROGRESSBAR1X), 
		GetIntFromStringTable(IDS_PROGRESSBAR1Y),
		GetIntFromStringTable(IDS_PROGRESSBAR1X) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSBAR1Y) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH) );
	m_cCustomProgressRecvBlock.Init( &cTempRect, 0 );

	// download progress bar.
	::SetRect( &cTempRect, 
		GetIntFromStringTable(IDS_PROGRESSBAR1X), 
		GetIntFromStringTable(IDS_PROGRESSBAR2Y),
		GetIntFromStringTable(IDS_PROGRESSBAR1X) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSBAR2Y) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH) );
	m_cCustomProgressRecvFile.Init( &cTempRect, 0 );

	//패치진행중에 프로그래스바를 그리기위해 받았다. -_-;
	m_cPatchClientLib.SetDialogInfo( m_pDlg->FromHandle( m_pDlg->GetSafeHwnd() ), &m_cCustomProgressRecvFile, &m_cCustomProgressRecvBlock, &m_StaticStatus, NULL, &m_StaticDetailInfo );

	m_pDlg->SetActiveWindow();
	m_pDlg->SetTimer( JAPAN_COOKIE_TIMER, 180000, NULL );

	m_cPatchClientLib.ReadConfig("Ini\\PatchConfig.ini");

	PatchThreadStart();

	return TRUE; 
}

VOID JapanPatchClient::OnPaint( VOID )
{
	CPaintDC dc( m_pDlg ); // device context for painting

	CDC			memdc, memdc1;
	CString		strNotice;
	CFont*		pOldFont;

	strNotice.LoadString( IDS_MSG_NOTICE );

	memdc.CreateCompatibleDC( &dc );
	memdc.SelectObject( CBitmap::FromHandle(m_cBMPBackground) );

	memdc1.CreateCompatibleDC( &dc );
	memdc1.SelectObject( CBitmap::FromHandle(m_cBmpBackgroundEx) );	

	int cX		=	GetIntFromStringTable( IDS_WIDTHSIZE );
	int cY		=	GetIntFromStringTable( IDS_HEIGHTSIZE );

	dc.BitBlt( 0, 0, cX, cY, &memdc, 0, 0, SRCCOPY );
	dc.BitBlt( 4, 20, cX, cY, &memdc1, 0, 0, SRCCOPY );

	strNotice.LoadString(IDS_PATCH_INFO);
	dc.SetBkMode(TRANSPARENT);              
	pOldFont = (CFont*)dc.SelectObject(&m_cDrawFontInfo);
	dc.SetTextColor( RGB(137,137,137) );	//RGB(898989)
	dc.TextOut( 16, 340, strNotice, strNotice.GetLength());
	dc.SelectObject(pOldFont);

	//. 'Total now' print
	strNotice.LoadString(IDS_PATCH_TOTAL);
	dc.SetBkMode(TRANSPARENT);              
	pOldFont = (CFont*)dc.SelectObject(&m_cDrawFontInfo);
	dc.SetTextColor( RGB(137,137,137) );	//RGB(898989)
	dc.TextOut( 16, 358, strNotice, strNotice.GetLength());
	dc.SelectObject(pOldFont);

	strNotice.LoadString(IDS_REAL_TITLE);

	dc.SetBkMode(TRANSPARENT);              
	pOldFont = (CFont*)dc.SelectObject(&m_cDrawTitle);
	dc.SetTextColor( RGB(195,195,195) );	//RGB(C3C3C3)
	dc.TextOut( 33, 5, strNotice, strNotice.GetLength());
	dc.SelectObject(pOldFont);

	//. Progress draw
	m_cCustomProgressRecvBlock.Draw( &dc );
	m_cCustomProgressRecvFile.Draw( &dc );
}

VOID JapanPatchClient::OnTimer( UINT nIDEvent )
{
#ifdef _AREA_JAPAN_
	g_jAuth.Refresh();
#endif
}

VOID JapanPatchClient::DoDataExchange( CDataExchange* pDX )
{
	DDX_Control(pDX, IDC_Static_Status, m_StaticStatus);
	DDX_Control(pDX, IDC_Static_DetailInfo, m_StaticDetailInfo);
	DDX_Control(pDX, IDC_EXitButton, m_cKbcExitButton );
	DDX_Control(pDX, IDC_RegisterButton, m_cKbcRegisterButton );
	DDX_Control(pDX, IDC_OptionButton, m_cKbcOptionButton );
	DDX_Control(pDX, IDC_HomepageButton, m_cKbcHomepageButton );
	DDX_Control(pDX, IDC_START_GAME, m_cKbcStartgameButton );
}


VOID JapanPatchClient::StartPatch( VOID )
{
	wchar_t strMsg[256] = {0, };

	if( m_cPatchClientLib.m_cPatchReg.InitRegistry() )
	{		
		//Server로 연결.
		char* pstrIP	= m_cPatchClientLib.m_cPatchReg.GetIP();
		int	iPort		= m_cPatchClientLib.m_cPatchReg.GetPort();
		bool bConnect	= m_cPatchClientLib.initConnect( 0x0202, false, pstrIP, iPort );

		if( m_cPatchClientLib.m_lErrorCode & 1 )
		{
			::MessageBoxW(m_pDlg->GetSafeHwnd(), L"StartupWinsock Error", MESSAGE_TITLE, MB_OK);
		}
		if( m_cPatchClientLib.m_lErrorCode & 2 )
		{
			::MessageBoxW(m_pDlg->GetSafeHwnd(), L"CreateSocket Error", MESSAGE_TITLE, MB_OK);
		}
		if( m_cPatchClientLib.m_lErrorCode & 4 )
		{
			::LoadStringW(g_resInst, IDS_ERROR_DNS, strMsg, 256);
			::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
		}
		if( m_cPatchClientLib.m_lErrorCode & 8 )
		{
			::LoadStringW(g_resInst, IDS_ERROR_CONNECTION, strMsg, 256);
			::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
		}

		if( bConnect )
		{
			bool bResult = m_cPatchClientLib.ExecuteIOLoop( false );

			if( m_cPatchClientLib.IsPatchClient() )
			{
				::LoadStringW(g_resInst, IDS_RUN_AGAIN, strMsg, 256);
				::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);

				exit(1);
			}

			if( bResult )
			{
				//패치성공사실을 알린다.
				::LoadStringW(g_resInst, IDS_MSG_COMPLETE, strMsg, 256);
				m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticDetailInfo, strMsg );

				::LoadStringW(g_resInst, IDS_RUN_GAME, strMsg, 256);
				m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticStatus, strMsg );

				if ( m_cPatchOptionFile.m_bSaveLoad )
				{
					CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
					pcWnd->EnableWindow();
				}

				//if ( g_jAuth.IsRealService() )
				{
					CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
					pcWnd->EnableWindow();
				}


				//if( m_bForceExit )
				//{
				//	exit(0);
				//}
			}
			else
			{
				if( m_cPatchClientLib.m_lResult == g_lAlreadyPatched )
				{
					//패치성공사실을 알린다.
					m_cPatchClientLib.m_pcCustomProgressRecvBlock->SetMaxCount( 1 );
					m_cPatchClientLib.m_pcCustomProgressRecvBlock->SetCurrentCount( 1 );
					m_pDlg->InvalidateRect( m_cPatchClientLib.m_pcCustomProgressRecvBlock->GetRect(), FALSE );

					m_cPatchClientLib.m_pcCustomProgressRecvFile->SetMaxCount( 1 );
					m_cPatchClientLib.m_pcCustomProgressRecvFile->SetCurrentCount( 1 );
					m_pDlg->InvalidateRect( m_cPatchClientLib.m_pcCustomProgressRecvFile->GetRect(), FALSE );

					::LoadStringW(g_resInst, IDS_MSG_COMPLETE, strMsg, 256);
					m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticDetailInfo, strMsg );

					::LoadStringW(g_resInst, IDS_RUN_GAME, strMsg, 256);
					m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticStatus, strMsg );

					//pcPatchClientDlg->m_cPatchReg.BackupRegistry();
					if ( m_cPatchOptionFile.m_bSaveLoad )
					{
						CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
						pcWnd->EnableWindow();
					}

					//if ( g_jAuth.IsRealService() )
					{
						CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
						pcWnd->EnableWindow();
					}

				}
				else if( m_cPatchClientLib.m_lResult == g_lNotEnoughDiskSpace )
				{
					wchar_t			strBuffer[256];

					::LoadStringW(g_resInst, IDS_INSUFFICENT_DISK, strMsg, 256);
					swprintf( strBuffer, strMsg, m_cPatchClientLib.m_iRequireDiskSpace/1000000.0f );

					::MessageBoxW(m_pDlg->GetSafeHwnd(), strBuffer, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lCannotConnectToDownloadServer )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_CONNECTION, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lDisconnectFromDownloadServer )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_DISCONNECTED, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lCannotOpenFileFromDownload )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_OPEN, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lCannotDownloadFileFromDownload )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_RECEIVE, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else
				{
					::LoadStringW(g_resInst, IDS_ERROR_FAILED, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
			}
		}
		else
		{
			::LoadStringW(g_resInst, IDS_ERROR_PATCH_CONNECTION, strMsg, 256);
			::MessageBoxW(m_pDlg->m_hWnd, strMsg, MESSAGE_TITLE, MB_OK);
		}		

		//윈속을 초기화한다.
		m_cPatchClientLib.cleanupWinsock();
	}
	else
	{
		//MessageBoxW( "레지스트리에 패치 정보가 없습니다." );
	}
}

#endif