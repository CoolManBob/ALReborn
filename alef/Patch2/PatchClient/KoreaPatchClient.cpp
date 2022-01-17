#include "StdAfx.h"

#ifdef _KOR

#include "KoreaPatchClient.h"
#include "Hyperlinks.h"
#include "PatchClientDlg.h"
#include "AuIgnoringHelper.h"
#include "../HttpDownloader/HttpDownloader.h"

#include "ImageHlp.h"
#pragma comment( lib, "Imagehlp.lib" )

static char const * _backupURL = "http://patch-arch.krweb.nefficient.com/";


KoreaPatchClient::KoreaPatchClient( CDialog* pDlg )
:	BasePatchClient( pDlg )
{
	m_ulClientFileCRC = 0;
	memset( m_strPatchCodeString, 0, sizeof( char ) * LENGTH_PATCH_CODE_STRING );
}

KoreaPatchClient::~KoreaPatchClient(void)
{
}

BOOL KoreaPatchClient::OnInitDialog( VOID )
{
	wchar_t		strMsg[ 256 ];
	BITMAP		bitmap;
	CHAR		szBackFileName[ MAX_PATH ];

	ZeroMemory( szBackFileName , MAX_PATH );

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

	m_pDlg->ModifyStyle(0 , WS_CLIPCHILDREN );

	// 창 이름 변경
	m_pDlg->SetWindowText( "Archlord Patch Client" );

	//폰트설정
	m_cDrawFont.CreateFont       ( 13, 0, 0, 0, FW_BOLD,   0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawTitle.CreateFont      ( 12, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawFontInfo.CreateFont   ( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawHyperLink.CreateFont  ( 12, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawProgressMsg.CreateFont( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );

	m_cOldPoint.x = -1;
	m_cOldPoint.y = -1;

	m_cBMPBackground.Attach( LoadBitmapResource( "images\\BaseBackground.bm1" ) );
	m_cBMPBackground.GetBitmap( &bitmap );

	// 4 개중에 하나를 임의로 보여주는 배경 그림 로드
	unsigned int nRandomNum = (timeGetTime() % 4)+1;
	sprintf_s( szBackFileName , MAX_PATH , "images\\RandomBackground%02d.bm1" , nRandomNum);
	m_cBmpBackgroundEx.Attach( LoadBitmapResource(szBackFileName) );

	

	m_cBMPBackground.GetBitmap( &bitmap );

	m_pDlg->MoveWindow( nPosX , nPosY, bitmap.bmWidth , bitmap.bmHeight );
	
	CString	strJoin;
	CString	strOption;
	CString	strHomepage;
	CString	strStart;

	strJoin.LoadString(IDS_MSG_JOIN);
	strOption.LoadString(IDS_MSG_OPTION);
	strHomepage.LoadString(IDS_MSG_HOMEPAGE);
	strStart.LoadString(IDS_MSG_START);

	//Button
	m_cKbcExitButton.SetBmpButtonImage( IDB_Button_Close_Set, RGB(0,0,255) );
	m_cKbcOptionButton.SetBmpButtonImage( IDB_BUTTON_OPTION, RGB(0,0,255) );
	m_cKbcStartgameButton.SetBmpButtonImage( IDB_BUTTON_START, RGB(0,0,255) );

	CWnd		*pcTempWnd = 0;
	RECT		rect;

	pcTempWnd = m_pDlg->GetDlgItem( IDC_EXitButton );
	pcTempWnd->SetWindowPos( NULL, 
		bitmap.bmWidth	- 22	,
		GetIntFromStringTable(IDS_EXITBTNPOSY), 
		16, 
		16, 
		SWP_SHOWWINDOW );
	

	m_cKbcOptionButton.GetClientRect( &rect );
	pcTempWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_OPTIONBTNPOSX), 
		GetIntFromStringTable(IDS_OPTIONBTNPOSY), 
		rect.right	,
		rect.bottom	,
		SWP_SHOWWINDOW );


	m_cKbcStartgameButton.GetClientRect( &rect );
	pcTempWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_STARTBTNPOSX), 
		GetIntFromStringTable(IDS_STARTBTNPOSY), 
		rect.right	,
		rect.bottom	,
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


	//. Hyperlink static text 위치와 링크설정
	pcTempWnd = m_pDlg->GetDlgItem( IDC_HOMEPAGE );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawHyperLink);
	pcTempWnd->SetWindowPos( NULL, 563, 231, 49, 12, SWP_SHOWWINDOW );
	ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_HOMEPAGE);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_JOIN );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 410, 27, 49, 12, SWP_SHOWWINDOW );
	//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_JOIN);

	pcTempWnd = m_pDlg->GetDlgItem( IDC_CUSTOMER );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawHyperLink);
	pcTempWnd->SetWindowPos( NULL, 627, 231, 49, 12, SWP_SHOWWINDOW );
	ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_CUSTOMER);

	pcTempWnd = m_pDlg->GetDlgItem( IDC_DRIVER );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawHyperLink);
	pcTempWnd->SetWindowPos( NULL, 691, 231, 49, 12, SWP_SHOWWINDOW );
	ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_DRIVER);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_LINE );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 400, 27, 200, 12, SWP_SHOWWINDOW );
	
	//ini\\COption.ini를 읽는다.
	m_cPatchOptionFile.Init();
	m_cPatchOptionFile.Load( PATCHCLIENT_OPTION_FILENAME );

	//m_staticMainBack.Create( "Main Back" , WS_CHILD | WS_VISIBLE | SS_BITMAP | WS_CLIPCHILDREN , rect , m_pDlg  );
	//m_staticMainBack.SetWindowPos( NULL , 0 , 0 , bitmap.bmWidth , bitmap.bmHeight , NULL );
	//m_staticMainBack.ShowWindow( SW_SHOW );

	HDC	hdc		=	GetDC( m_pDlg->m_hWnd );

	// 디스플레이 DC와 호환되는 비트맵과 DC 만들기
	m_hbmBackBuffer			= CreateCompatibleBitmap(hdc, bitmap.bmWidth,bitmap.bmHeight);
	m_hdcBack				= CreateCompatibleDC(hdc);

	// DC 내부 속성 설정
	m_hbmOld				= (HBITMAP)SelectObject(m_hdcBack, m_hbmBackBuffer);	

	//Html을 보인다.
	CCreateContext pContext;
	CWnd* pFrameWnd = m_pDlg;

	pContext.m_pCurrentDoc = NULL;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CTestHtmlView);

	m_pWeb =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);
	ASSERT(m_pWeb);

	//. 스크롤바를 없앤다.
	//m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL, 0, 0 );
	//m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR, 0, 0 );
	//m_pWeb->SetWindowPos(NULL,380,390,424,128,NULL);
	//m_pWeb->ShowWindow(SW_HIDE);
	//m_pWeb->Navigate(m_cPatchOptionFile.GetNoticeURL(), 0, NULL, NULL);		
	///////////////////////////////////////////////////////////////////////////////////

	m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL  , 0 , 0 );
	m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR , 0 , 0 );
	m_pWeb->SetWindowPos( NULL , 4 , 23 , 786 , 504 , NULL );
	m_pWeb->ShowWindow( SW_HIDE );
	//m_pWeb->Navigate( "http://archlord.webzen.co.kr/news/notice/list.aspx" );
	m_pWeb->Navigate( "http://archlord.webzen.co.kr/launcher/" );
	m_pWeb->SetParent( m_pDlg );

	m_staticBackColor.Create( "Back Rotate" , WS_CHILD | WS_VISIBLE | SS_BITMAP  , rect , m_pWeb );
	m_staticBackColor.SetBitmap( m_cBmpBackgroundEx );
	m_staticBackColor.SetWindowPos( NULL , 0 , 0 , 787 , 190 , NULL );
	m_staticBackColor.ShowWindow( SW_SHOW );

	/*m_pWebAdvertisement	=	(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
	ASSERT( m_pWebAdvertisement );
	m_pWebAdvertisement->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL , 0 , 0 );
	m_pWebAdvertisement->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR , 0 , 0 );
	m_pWebAdvertisement->SetWindowPos( NULL , 52 , 254 , 321 ,241 , NULL );
	m_pWebAdvertisement->ShowWindow( SW_HIDE );
	m_pWebAdvertisement->Navigate( ARCHLORD_ADVERTISEMENT );

	for( INT i = 0 ; i < 3 ; ++i )
	{
		m_pWebInside[ i ]	=	(CTestHtmlView*)((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
		ASSERT( m_pWebInside[ i ] );
		m_pWebInside[ i ]->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL , 0 , 0 );
		m_pWebInside[ i ]->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR , 0 , 0 );
		m_pWebInside[ i ]->SetWindowPos( NULL, 416 + 106*i , 287 , 97 , 60 , NULL );
		m_pWebInside[ i	]->ShowWindow( SW_HIDE );
		m_pWebInside[ i ]->Navigate( m_cPatchOptionFile.GetNoticeURL() );
	}*/

	//웹 페이지가 뜰 시간 0.5초간 기다려준다.
	Sleep( 500 );

	//TempDir을 세팅해준다.
	if( m_cPatchClientLib.SetTempDir( "Temp" ) == false )
	{
		::LoadStringW(g_resInst, IDS_ERROR_MAKE_PATCH_FOLDER, strMsg, 256);
		MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
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

	m_StaticStatus.ShowWindow( SW_HIDE );
	m_StaticDetailInfo.ShowWindow( SW_HIDE );


	//패치진행중에 프로그래스바를 그리기위해 받았다. -_-;
	m_cPatchClientLib.SetDialogInfo( m_pDlg->FromHandle( m_pDlg->GetSafeHwnd() ), &m_cCustomProgressRecvFile, &m_cCustomProgressRecvBlock, &m_StaticStatus, NULL, &m_StaticDetailInfo );

	m_pDlg->SetActiveWindow();
	m_pDlg->SetTimer( KOREA_TIMER, 180000, NULL );

	m_cPatchClientLib.ReadConfig("Ini\\PatchConfig.ini");

	PatchThreadStart();

	return TRUE;
}

VOID KoreaPatchClient::OnPaint( VOID )
{
	BITMAP		bitmap;
	BITMAP		bitmapBack;
	CDC			memDC;
	CDC			BackDC;
	CRect		rect;
	memDC.Attach( CreateCompatibleDC( m_hdcBack ) );
	
	HBITMAP		hbmOld	=	(HBITMAP)memDC.SelectObject( m_cBMPBackground );

	m_cBMPBackground.GetBitmap( &bitmap );

	BitBlt(m_hdcBack, 0, 0, bitmap.bmWidth, bitmap.bmHeight , memDC, 0 , 0 , SRCCOPY);

	BackDC.Attach( m_hdcBack );
	m_cCustomProgressRecvBlock.Draw( &BackDC  );
	m_cCustomProgressRecvFile.Draw( &BackDC   );

	CStringW strWindowText;
	

	BackDC.SetBkMode( TRANSPARENT );
	BackDC.SetTextColor( RGB(255,255,255) );

	(CFont*)BackDC.SelectObject( m_StaticDetailInfo.GetFont() );
	
	strWindowText	=	m_StaticDetailInfo.GetText();
	DrawTextW( BackDC , strWindowText , strWindowText.GetLength() , m_cCustomProgressRecvBlock.GetRect() , DT_CENTER );

	strWindowText	=	m_StaticStatus.GetText();
	DrawTextW( BackDC , strWindowText	, strWindowText.GetLength() , m_cCustomProgressRecvFile.GetRect() , DT_CENTER );

	memDC.SelectObject(hbmOld);

	HDC hdc = GetDC(m_pDlg->m_hWnd);
	BitBlt(hdc, 0 , 0 ,  bitmap.bmWidth ,bitmap.bmHeight, BackDC,  0 , 0 , SRCCOPY);

	BackDC.Detach();
	memDC.Detach();

	ReleaseDC(m_pDlg->m_hWnd, hdc);
	
}

VOID KoreaPatchClient::OnTimer( UINT nIDEvent )
{
	if( nIDEvent == 5 )
	{
		m_pDlg->InvalidateRect( m_cCustomProgressRecvFile.GetRect(), FALSE );
		m_pDlg->InvalidateRect( m_cCustomProgressRecvBlock.GetRect(), FALSE );
	}
}

VOID KoreaPatchClient::DoDataExchange( CDataExchange* pDX )
{
	DDX_Control(pDX, IDC_Static_Status		, m_StaticStatus		);
	DDX_Control(pDX, IDC_Static_DetailInfo	, m_StaticDetailInfo	);
	DDX_Control(pDX, IDC_EXitButton			, m_cKbcExitButton		);
	DDX_Control(pDX, IDC_RegisterButton		, m_cKbcRegisterButton	);
	DDX_Control(pDX, IDC_OptionButton		, m_cKbcOptionButton	);
	DDX_Control(pDX, IDC_HomepageButton		, m_cKbcHomepageButton	);
	DDX_Control(pDX, IDC_START_GAME			, m_cKbcStartgameButton );
}

VOID KoreaPatchClient::OnStartGame( VOID )
{
	bool checkCRC = true;

	if( AuIgnoringHelper::IgnoreToFile( "NotCheckCRC.arc" ) )
	{
		checkCRC = false;
	}

	if( checkCRC )
	{
		// 실행파일의 CRC 체크를 위해 서버로 CRC 를 요청하고 기다린다.
		if( !_GetClientFileCRCFromPatchServer() )
		{
			char strMessage[ 256 ] = { 0, };
			::LoadString( NULL, IDS_ERROR_PATCH_CONNECTION, strMessage, sizeof( char ) * 256 );
			::MessageBox( NULL, strMessage, "Network Error", MB_OK );

			// 패치클라이언트는 종료시킨다.
			DoCloseDialog();
		}
	}
	else
	{
		DoStartGame();
		DoCloseDialog();
	}
}

BOOL KoreaPatchClient::OnReceiveClientFileCRC( void* pPacket )
{
	DisConnect();

	CPacketTypeCS_0x05* ppdPacket = ( CPacketTypeCS_0x05* )pPacket;
	if( !ppdPacket ) return FALSE;

	m_ulClientFileCRC = ppdPacket->m_ulClientFileCRC;

	// 서버로부터 CRC 정보를 받았으니 CRC 검사를 수행한다.
	if( !AuIgnoringHelper::IgnoreToFile( "NotCheckCRC.arc" )
		&& !_CheckClientFileCRC( "AlefClient.exe", m_ulClientFileCRC ) )
	{
		char strMessage[ 256 ] = { 0, };
		::LoadString( NULL, IDS_ERROR_CLIENTFILE_CURRUPTED, strMessage, sizeof( char ) * 256 );
		::MessageBox( NULL, strMessage, "Client File Error!", MB_OK );

		// 패치클라이언트는 종료시킨다.
		DoCloseDialog();
		return TRUE;
	}

	// 패치클라이언트가 변경되었으면 다시 시작하라는 메세지를 보내고 종료한다.
	if( _IsUpdatePatchClientFile( "Archlord.bak" ) )
	{
		char strMessage[ 256 ] = { 0, };
		::LoadString( NULL, IDS_NOTICE_DO_RESTART, strMessage, sizeof( char ) * 256 );
		::MessageBox( NULL, strMessage, "Notice", MB_OK );

		// 패치클라이언트는 종료시킨다.
		DoCloseDialog();
		return TRUE;
	}

	// 실제로 게임 클라이언트를 기동시킨다.
	DoStartGame();
	return TRUE;
}

BOOL KoreaPatchClient::DoStartGame( void )
{
	// 실행파일에 넘겨줄 패치코드를 만들어서..
	if( !_MakePatchCodeString() ) return FALSE;

	// 실행파일에 패치코드를 넘겨서 실행시키고,..
	m_cPatchClientLib.StartGame( ( char* )m_strPatchCodeString );

	// 패치클라이언트는 종료시킨다.
	DoCloseDialog();
	return TRUE;
}

BOOL KoreaPatchClient::DoCloseDialog( void )
{
	( ( CPatchClientDlg* )m_pDlg )->CloseDlg();
	return TRUE;
}

BOOL KoreaPatchClient::_GetClientFileCRCFromPatchServer( void )
{
	if( !Connect() ) return FALSE;
	if( !SendRequestClientFileCRC() ) return FALSE;
	if( !Listen( false ) ) return FALSE;
	return TRUE;
}

BOOL KoreaPatchClient::_CheckClientFileCRC( char* pFileName, unsigned long ulCRC )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	char strFullPath[1024] = {0,};
	char strFullPathFileName[ 1024 ] = {0,};
	::GetCurrentDirectory( sizeof(strFullPath), strFullPath );
	sprintf_s( strFullPathFileName, "%s\\%s", strFullPath, pFileName );

	DWORD dwHeaderSum = 0;
	DWORD dwCheckSum = 0;

	DWORD dwResult = ::MapFileAndCheckSum( strFullPathFileName, &dwHeaderSum, &dwCheckSum );

	// CRC체크에 실패하면 실행파일을 새로 다운받아 다시 시도한다
	if( dwResult != CHECKSUM_SUCCESS || ulCRC != dwCheckSum && GetPatchClientLib() )
	{	
		char url[ 2048 ] = {0,};
		sprintf_s( url, sizeof(url) , "%s%s", _backupURL, pFileName );

		HttpDownLoader downloader( url );

		if( downloader.DownLoad( strFullPathFileName ) )
		{
			dwHeaderSum = 0;
			dwCheckSum = 0;
			if( ::MapFileAndCheckSum( strFullPathFileName, &dwHeaderSum, &dwCheckSum ) != CHECKSUM_SUCCESS )
				return FALSE;
		}
	}

	return ulCRC == dwCheckSum ? TRUE : FALSE;
}

BOOL KoreaPatchClient::_MakePatchCodeString( void )
{
	HKEY hKey = NULL;
	//LONG nResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\ArchLord_KR_Alpha", 0, KEY_READ, &hKey );
	//LONG nResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\ArchLord_KR", 0, KEY_READ, &hKey );
	LONG nResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, m_cPatchClientLib.m_cPatchReg.GetRegKeyBase(), 0, KEY_READ, &hKey );

	
	if( nResult != ERROR_SUCCESS ) return FALSE;

	DWORD dwValueType = 0;
	DWORD dwValue = 0;
	DWORD dwValueLength = sizeof( DWORD );

	nResult = ::RegQueryValueEx( hKey, "Code", 0, &dwValueType, ( LPBYTE )( &dwValue ), &dwValueLength );
	if( nResult != ERROR_SUCCESS ) return FALSE;

	RegCloseKey( hKey );

	/*
	DWORD dwTime = timeGetTime();
	memset( m_strPatchCodeString, 0, sizeof( char ) * LENGTH_PATCH_CODE_STRING );
	sprintf_s( m_strPatchCodeString, sizeof( char ) * LENGTH_PATCH_CODE_STRING, "%lu", dwTime );

	int nTimeLength = ( int )strlen( m_strPatchCodeString );
	memset( m_strPatchCodeString, 0, sizeof( char ) * LENGTH_PATCH_CODE_STRING );
	sprintf_s( m_strPatchCodeString, sizeof( char ) * LENGTH_PATCH_CODE_STRING, "%02d%lu%lu", nTimeLength, dwTime, dwValue );

	int nCodeLength = ( int )strlen( m_strPatchCodeString );
	for( int nCount = 0 ; nCount < nCodeLength ; nCount++ )
	{
		if( nCount != 0 && nCount % 2 == 0 )
		{
			char cTemp = m_strPatchCodeString[ nCount - 1 ];
			m_strPatchCodeString[ nCount - 1 ] = m_strPatchCodeString[ nCount ];
			m_strPatchCodeString[ nCount ] = cTemp;
		}
	}
	*/

	memset( m_strPatchCodeString, 0, sizeof(m_strPatchCodeString) );

	sprintf_s( m_strPatchCodeString, "%u", dwValue );

	return TRUE;
}

BOOL KoreaPatchClient::_IsUpdatePatchClientFile( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;
	BOOL bHaveNewPatchClientFile = FALSE;

	// 파일이 존재하는가
	FILE* pFile = ::fopen( pFileName, "rb" );
	if( pFile )
	{
		bHaveNewPatchClientFile = TRUE;
		::fclose( pFile );
	}

	if( IsUpdatePatchClient() && bHaveNewPatchClientFile ) return TRUE;
	return FALSE;
}

#endif