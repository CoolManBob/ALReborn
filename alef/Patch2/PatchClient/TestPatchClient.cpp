#include "StdAfx.h"
#include "TestPatchClient.h"

TestPatchClient::TestPatchClient( CDialog* pDlg )
:	BasePatchClient( pDlg )
{
}

TestPatchClient::~TestPatchClient(void)
{
}

VOID TestPatchClient::DoDataExchange( CDataExchange* pDX )
{
	DDX_Control(pDX, IDC_Static_Status, m_StaticStatus);
	DDX_Control(pDX, IDC_Static_DetailInfo, m_StaticDetailInfo);
	DDX_Control(pDX, IDC_EXitButton, m_cKbcExitButton );
	DDX_Control(pDX, IDC_RegisterButton, m_cKbcRegisterButton );
	DDX_Control(pDX, IDC_OptionButton, m_cKbcOptionButton );
	DDX_Control(pDX, IDC_HomepageButton, m_cKbcHomepageButton );
	DDX_Control(pDX, IDC_START_GAME, m_cKbcStartgameButton );
}

BOOL TestPatchClient::OnInitDialog( VOID )
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
	/////////////////////////////////////////////////////////////////

	m_cDrawFont.CreateFont       ( 13, 0, 0, 0, FW_BOLD,   0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawTitle.CreateFont      ( 12, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawFontInfo.CreateFont   ( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawHyperLink.CreateFont  ( 12, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cDrawProgressMsg.CreateFont( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );

	m_pDlg->SetWindowText( "Archlord Patch Client" );

	m_cOldPoint.x = -1;
	m_cOldPoint.y = -1;

	//m_cBMPBackground	=(HBITMAP)::LoadImage(NULL, "./images/BaseBackground.bm1", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);

	unsigned int nRandomNum = (timeGetTime() % 3) + 1;
	char backImgName[_MAX_PATH] = {0, };
	sprintf(backImgName, "images\\RandomBackground0%d.bm1", nRandomNum);
	m_cBMPBackgroundEx = LoadBitmapResource( backImgName );

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

	//ini\\COption.ini를 읽는다.
	m_cPatchOptionFile.Init();
	m_cPatchOptionFile.Load( PATCHCLIENT_OPTION_FILENAME );

	//Html을 보인다.
	CCreateContext pContext;
	CWnd* pFrameWnd = m_pDlg;

	//pContext.m_pCurrentDoc = new CTemplateDocument;
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

	m_cPatchClientLib.ReadConfig("Ini\\PatchConfig.ini");

	PatchThreadStart();

	return TRUE; 
}

VOID TestPatchClient::OnPaint( VOID )
{
	CPaintDC dc( m_pDlg ); // device context for painting

	CDC			memdc;
	CString		strNotice;
	CFont*		pOldFont;

	strNotice.LoadString(IDS_MSG_NOTICE);

	memdc.CreateCompatibleDC( &dc );
	memdc.SelectObject( CBitmap::FromHandle(m_cBMPBackground) );

	int cX		=	GetIntFromStringTable( IDS_WIDTHSIZE );
	int cY		=	GetIntFromStringTable( IDS_HEIGHTSIZE );

	dc.BitBlt( 0, 0, cX, cY, &memdc, 0, 0, SRCCOPY );

	//. 'Update info' print
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

	strNotice.LoadString(IDS_TEST_TITLE);

	dc.SetBkMode(TRANSPARENT);              
	pOldFont = (CFont*)dc.SelectObject(&m_cDrawTitle);
	dc.SetTextColor( RGB(195,195,195) );	//RGB(C3C3C3)
	dc.TextOut( 33, 5, strNotice, strNotice.GetLength());
	dc.SelectObject(pOldFont);

	m_cCustomProgressRecvBlock.Draw( &dc );
	m_cCustomProgressRecvFile.Draw( &dc );

	dc.BitBlt( 4, 23, 585, 307, &memdc, 0, 0, SRCCOPY );

}