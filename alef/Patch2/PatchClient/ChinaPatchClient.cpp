#include "StdAfx.h"
#include "ChinaPatchClient.h"
#include "PatchClientDlg.h"

ChinaPatchClient::ChinaPatchClient( CDialog* pDlg )
:	BasePatchClient( pDlg )
{
	
}

ChinaPatchClient::~ChinaPatchClient(void)
{
}

VOID ChinaPatchClient::DoDataExchange( CDataExchange* pDX )
{
	DDX_Control(pDX, IDC_Static_Status, m_StaticStatus);
	DDX_Control(pDX, IDC_Static_DetailInfo, m_StaticDetailInfo);
	DDX_Control(pDX, IDC_EXitButton, m_cKbcExitButton );
	DDX_Control(pDX, IDC_RegisterButton, m_cKbcRegisterButton );
	DDX_Control(pDX, IDC_OptionButton, m_cKbcOptionButton );
	DDX_Control(pDX, IDC_HomepageButton, m_cKbcHomepageButton );
	DDX_Control(pDX, IDC_START_GAME, m_cKbcStartgameButton );
	DDX_Control(pDX, IDC_MenuBtn1, m_ckbcMenu1Button );
	DDX_Control(pDX, IDC_MenuBtn2, m_ckbcMenu2Button );
	DDX_Control(pDX, IDC_MenuBtn3, m_ckbcMenu3Button );
	DDX_Control(pDX, IDC_MenuBtn4, m_ckbcMenu4Button );
	DDX_Control(pDX, IDC_MenuBtn5, m_ckbcMenu5Button );
	DDX_Control(pDX, IDC_SeverSelect1, m_ckbcServerSelect1Btn );
	DDX_Control(pDX, IDC_SeverSelect2, m_ckbcServerSelect2Btn );
	DDX_Control(pDX, IDC_SeverSelect3, m_ckbcServerSelect3Btn );
	DDX_Control(pDX, IDC_ServerList, m_cServerListBox );
	DDX_Control(pDX, IDC_ScrollThumbBtn, m_cScrollThumbBtn);

}
BOOL ChinaPatchClient::OnInitDialog( VOID )
{
	wchar_t strMsg[ 256 ];

	m_bIsWaitPatch		=	TRUE;

	m_cPatchClientLib.LoadReferenceFile();

	//. Force setting Dialog size/position
	int nPosX = GetSystemMetrics ( SM_CXSCREEN );
	int nPosY = GetSystemMetrics ( SM_CYSCREEN );

	nPosX = ( nPosX / 2 ) - 296;
	nPosY = ( nPosY / 2 ) - 199;

	// 창 크기 변경 및 이동
	m_pDlg->MoveWindow( nPosX,	nPosY, 
						GetIntFromStringTable(IDS_WIDTHSIZE), 
						GetIntFromStringTable(IDS_HEIGHTSIZE));

	// 창 이름 변경
	m_pDlg->SetWindowText( "Archlord Patch Client" );

	// font 설정
	m_cDrawFont.CreateFont       ( 13, 0, 0, 0, FW_BOLD,   0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimSun" );
	m_cDrawTitle.CreateFont      ( 15, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimSun" );
	m_cDrawFontInfo.CreateFont   ( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimSun" );
	m_cDrawHyperLink.CreateFont  ( 13, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimSun" );
	m_cDrawProgressMsg.CreateFont( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimSun" );

	m_cOldPoint.x = -1;
	m_cOldPoint.y = -1;

	m_cBMPBackground.LoadBitmap( IDB_BASEBACKGROUND );

	CWnd *pcTempWnd = 0;

	m_ckbcMenu1Button.SetBmpButtonImage( IDB_MenuBtn1, RGB(0,0,255) );
	m_ckbcMenu2Button.SetBmpButtonImage( IDB_MenuBtn2, RGB(0,0,255) );
	m_ckbcMenu3Button.SetBmpButtonImage( IDB_MenuBtn3, RGB(0,0,255) );
	m_ckbcMenu4Button.SetBmpButtonImage( IDB_MenuBtn4, RGB(0,0,255) );
	m_ckbcMenu5Button.SetBmpButtonImage( IDB_MenuBtn5, RGB(0,0,255) );

	m_ckbcServerSelect1Btn.SetBmpButtonImage( IDB_SelectServer1, RGB(0,0,255) );
	m_ckbcServerSelect1Btn.m_bToggle = TRUE;
	m_ckbcServerSelect1Btn.m_bClicked = TRUE;	//. 처음 시작시 선택된 패치서버에어리어는 첫번째 것
	m_ckbcServerSelect2Btn.SetBmpButtonImage( IDB_SelectServer2, RGB(0,0,255) );
	m_ckbcServerSelect2Btn.m_bToggle = TRUE;
	m_ckbcServerSelect3Btn.SetBmpButtonImage( IDB_SelectServer3, RGB(0,0,255) );
	m_ckbcServerSelect3Btn.m_bToggle = TRUE;

	m_cKbcExitButton.SetBmpButtonImage( IDB_Button_Close_Set, RGB(0,0,255) );
	m_cKbcOptionButton.SetBmpButtonImage( IDB_BUTTON_OPTION_SET, RGB(0,0,255) );
	m_cKbcStartgameButton.SetBmpButtonImage( IDB_BUTTON_STARTGAME_SET, RGB(0,0,255) );

	m_cScrollThumbBtn.SetBmpButtonImage(IDB_ScrollThumb, RGB(0,0,255));

	pcTempWnd = m_pDlg->GetDlgItem( IDC_MenuBtn1 );
	pcTempWnd->SetWindowPos( NULL, 82, 250, 66, 23, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_MenuBtn2 );
	pcTempWnd->SetWindowPos( NULL, 189, 250, 66, 23, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_MenuBtn3 );
	pcTempWnd->SetWindowPos( NULL, 298, 250, 66, 23, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_MenuBtn4 );
	pcTempWnd->SetWindowPos( NULL, 405, 250, 66, 23, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_MenuBtn5 );
	pcTempWnd->SetWindowPos( NULL, 513, 250, 66, 23, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_SeverSelect1 );
	pcTempWnd->SetWindowPos( NULL, 166, 286, 36, 19, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_SeverSelect2 );
	pcTempWnd->SetWindowPos( NULL, 203, 286, 36, 19, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_SeverSelect3 );
	pcTempWnd->SetWindowPos( NULL, 240, 286, 36, 19, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_ScrollThumbBtn );
	pcTempWnd->SetWindowPos( NULL, 263, 310, 66, 23, SWP_SHOWWINDOW );

	//. 패치서버 리스트박스 초기 설정.
	m_cServerListBox.SetFont(11,"SimSun",RGB(202,205,152),RGB(202,205,152),TRUE,1);
	m_cServerListBox.SetItemBaseImage(IDB_ServerSelectSet, RGB(202,205,152));

	pcTempWnd = m_pDlg->GetDlgItem( IDC_EXitButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_EXITBTNPOSX), 
		GetIntFromStringTable(IDS_EXITBTNPOSY), 
		16, 
		16, 
		SWP_SHOWWINDOW );

	//pcTempWnd = GetDlgItem( IDC_RegisterButton );
	//pcTempWnd->SetWindowPos( NULL, 31, 456, 87, 22, SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_OPTIONBTNPOSX), 
		GetIntFromStringTable(IDS_OPTIONBTNPOSY), 
		GetIntFromStringTable(IDS_OPTIONBTNWIDTH), 
		GetIntFromStringTable(IDS_OPTIONBTNHEIGHT), 
		SWP_SHOWWINDOW );

	//pcTempWnd = GetDlgItem( IDC_HomepageButton );
	//pcTempWnd->SetWindowPos( NULL, 480, 291, 87, 22, SWP_HIDEWINDOW ); //더 사용하지 않기에 숨긴다.

	pcTempWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_STARTBTNPOSX), 
		GetIntFromStringTable(IDS_STARTBTNPOSY), 
		GetIntFromStringTable(IDS_STARTBTNWIDTH), 
		GetIntFromStringTable(IDS_STARTBTNHEIGHT), 
		SWP_SHOWWINDOW );

	//. Static text위치와 폰트를 설정한다.
	pcTempWnd = m_pDlg->GetDlgItem( IDC_Static_DetailInfo );
	CStatic* pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawProgressMsg);
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_PROGRESSMSG1X), 
		GetIntFromStringTable(IDS_PROGRESSMSG1Y), 
		GetIntFromStringTable(IDS_PROGRESSMSGWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSMSGHEIGHT), 
		SWP_SHOWWINDOW );

	m_cPatchClientLib.InvalidateStatic( pStaticText , L"请选择游戏区进行登陆或更新" , FALSE );

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

	//	ini\\COption.ini를 읽는다.
	m_cPatchOptionFile.Init();
	m_cPatchOptionFile.Load( PATCHCLIENT_OPTION_FILENAME );

	//Html을 보인다.
	CCreateContext pContext;
	CWnd* pFrameWnd = m_pDlg;

	//pContext.m_pCurrentDoc = new CTemplateDocument;
	pContext.m_pCurrentDoc = NULL;

	pContext.m_pNewViewClass = RUNTIME_CLASS(CTestHtmlView);

	// Notification HTML
	{
		m_pWeb =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);

		ASSERT(m_pWeb);

		//. 스크롤바를 없앤다.
		m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL, 0, 0 );
		m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR, 0, 0 );

		m_pWeb->SetWindowPos(NULL,307,290,310,150,NULL);

		m_pWeb->ShowWindow(SW_HIDE);
		m_pWeb->Navigate(m_cPatchOptionFile.GetNoticeURL(), 0, NULL, NULL);		
	}
	

	m_pWebTitle =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);

	ASSERT(m_pWebTitle);

	CRect rect;
	m_pDlg->GetClientRect(&rect);

	//보더옵션을 없애기 위해서 추가한다.
	//. 2005. 11. 30. Nonstopdj
	//. 스크롤바를 없앤다.
	m_pWebTitle->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR, 0, 0 );
	m_pWebTitle->SetWindowPos(NULL,22,30,605,208,NULL);
	m_pWebTitle->ShowWindow(SW_HIDE);
	m_pWebTitle->Navigate("http://ac.sdo.com/ad/ad.htm",0,NULL,NULL); //. http://ac.sdo.com/ad/ad.swf 


	//웹 페이지가 뜰 시간 0.5초간 기다려준다.
	Sleep( 500 );

	//TempDir을 지운다.
	//m_cPatchClientLib.RemoveDir( "Temp" );

	//TempDir을 세팅해준다.
	if( m_cPatchClientLib.SetTempDir( "Temp" ) == false )
	{
		::LoadStringW(g_resInst, IDS_ERROR_MAKE_PATCH_FOLDER, strMsg, 256);
		MessageBoxW( m_pDlg->GetSafeHwnd() , strMsg , MESSAGE_TITLE , MB_OK);
		return FALSE;
	}

	//. update/download Progress bar 
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

	//패치진행중에 프로그래스바를 그리기위해 받았다
	m_cPatchClientLib.SetDialogInfo( m_pDlg->FromHandle( m_pDlg->m_hWnd ), &m_cCustomProgressRecvFile, &m_cCustomProgressRecvBlock, &m_StaticStatus, NULL, &m_StaticDetailInfo );

	m_pDlg->SetActiveWindow();

	m_pDlg->SetTimer( CHINA_TIMER , 200 , NULL );

	if (m_cPatchClientLib.ReadConfig("Ini\\PatchConfig.ini"))
	{
		m_cPatchClientLib.ReadAreaConfig();
		SetServerAreaList(0);
		m_pWebTitle->ShowWindow(SW_SHOW);
		m_pWebTitle->Refresh();

		PingThreadStart();
	}


	return TRUE;
}

VOID ChinaPatchClient::OnPaint( VOID )
{
	CPaintDC dc( m_pDlg ); 

	CDC			memdc;
	CString		strNotice;
	CFont*		pOldFont;

	strNotice.LoadString(IDS_MSG_NOTICE);

	memdc.CreateCompatibleDC( &dc );
	memdc.SelectObject( CBitmap::FromHandle(m_cBMPBackground) );

	int cX		=	GetIntFromStringTable( IDS_WIDTHSIZE );
	int cY		=	GetIntFromStringTable( IDS_HEIGHTSIZE );

	dc.BitBlt( 0, 0, cX, cY, &memdc, 0, 0, SRCCOPY );

	wchar_t	strMsg[256];

	dc.SetBkMode(TRANSPARENT);		         
	pOldFont = (CFont*)dc.SelectObject(&m_cDrawHyperLink);
	dc.SetTextColor( RGB(255,255,255) );	//RGB(C3C3C3)

	::LoadStringW(g_resInst, IDS_BOTTOMCMSTR1, strMsg, 256);
	::TextOutW( dc.m_hDC,  253, 535, strMsg, (int) wcslen(strMsg) );

	::LoadStringW(g_resInst, IDS_BOTTOMCMSTR2, strMsg, 256);
	::TextOutW( dc.m_hDC,  253, 552, strMsg, (int) wcslen(strMsg) );

	dc.SelectObject(pOldFont);

	//. Progress draw
	m_cCustomProgressRecvBlock.Draw( &dc );
	m_cCustomProgressRecvFile.Draw( &dc );

	//AuIniManagerA   csManager;

	//csManager.SetPath( "./Archlord.ini" );
	//csManager.ReadFile( 0 , TRUE );

	//csManager.GetValue(		"Patch"		,	"Code"				);
	//csManager.GetValue(		"Patch"		,	"Version"			);
	//csManager.GetValue(		"Patch"		,	"LoginServerIP"		);
	//csManager.GetValue(		"Patch"		,	"LoginServerPort"	);
	//csManager.GetValue(		"Patch"		,	"PatchServerIP"		);
	//csManager.GetValue(		"Patch"		,	"PatchServerPort"	);
	//csManager.GetValue(		"Patch"		,	"SelfPatchFlag"		);
	//csManager.GetValue(		"Patch"		,	"File"				);
	//csManager.GetValue(		"Patch"		,	"Folder"			);

}

VOID ChinaPatchClient::OnTimer( UINT nIDEvent )
{
	static DWORD	dwAccumulationtime		= 0;
	static DWORD	dwDelaytime				= 0;
	DWORD			dwCurrentTime			= timeGetTime();
	BOOL			bScrollBarForceUpdate	= FALSE;

	//. server list refreshing then resist current top index.
	int				nPreTopIndex = m_cServerListBox.GetTopIndex();

	if(dwDelaytime > 0)
		dwAccumulationtime += dwCurrentTime - dwDelaytime;

	if(dwAccumulationtime > PATCH_AREA_LIST_REFRESH_DELAY)
	{
		if(m_nCurrentServerListCategory < 2)
			SetServerAreaList( m_nCurrentServerListCategory );
		else
			SetServerAreaList( 0, TRUE );

		dwAccumulationtime		= 0;
		dwDelaytime				= 0;
		bScrollBarForceUpdate	= TRUE;
	}

	dwDelaytime = dwCurrentTime;

	//. scrollbar animation.
	if(m_cServerListBox.GetCount() && (m_cScrollThumbBtn.m_bLButtonDown || bScrollBarForceUpdate))
	{
		CWnd			*pcTempWnd;
		POINT			point;

		GetCursorPos(&point);
		m_pDlg->ScreenToClient(&point);

		static int	nPrePosY	= 310;
		static int	nPreIndex	= 0;
		int			nIndex;
		int			CurPosY;

		if(!bScrollBarForceUpdate)
		{
			CurPosY = nPrePosY + (int)((point.y - nPrePosY) * 0.1f);

			if(CurPosY < LISTSCROLL_MIN)
				CurPosY = LISTSCROLL_MIN;

			if(CurPosY > LISTSCROLL_MAX)
				CurPosY = LISTSCROLL_MAX;

			pcTempWnd = m_pDlg->GetDlgItem( IDC_ScrollThumbBtn );
			pcTempWnd->SetWindowPos( NULL, LISTSCROLL_POSTION_X, CurPosY, 14, 14, SWP_SHOWWINDOW );
		}
		else
		{
			CurPosY = nPrePosY;
		}

		nIndex = (CurPosY - LISTSCROLL_MIN)/ (LISTSCROLL_SIZE /m_cServerListBox.GetCount());

		if(nIndex <= 0 ) 
			m_cServerListBox.SetScrollPos(SB_VERT, 0);

		else if(nIndex > m_cServerListBox.GetCount())
			m_cServerListBox.SetScrollPos(SB_VERT, m_cServerListBox.GetCount());
		else
		{
			if( nPreIndex > nIndex)
			{
				//. Scroll UP.
				m_cServerListBox.ScrollUp();
				m_cServerListBox.SetScrollPos(SB_VERT, nIndex);
			}
			else if( nPreIndex < nIndex)
			{
				//. Scroll Down.
				m_cServerListBox.ScrollDown();
				m_cServerListBox.SetScrollPos(SB_VERT, nIndex);
			}
			else if( nPreIndex == nIndex)
				m_cServerListBox.SetTopIndex(nPreTopIndex);
		}

		nPrePosY	= CurPosY;
		nPreIndex	= nIndex;
	}	
}

VOID ChinaPatchClient::OnBnClickedServer1( VOID )
{
	m_ckbcServerSelect1Btn.m_bClicked = TRUE;
	m_ckbcServerSelect2Btn.m_bClicked = FALSE;
	m_ckbcServerSelect3Btn.m_bClicked = FALSE;
	m_ckbcServerSelect1Btn.RedrawWindow();
	m_ckbcServerSelect2Btn.RedrawWindow();
	m_ckbcServerSelect3Btn.RedrawWindow();

	m_nCurrentServerListCategory = 0;
	SetServerAreaList( m_nCurrentServerListCategory );
	ResetScrollPos();
}

VOID ChinaPatchClient::OnBnClickedServer2( VOID )
{
	m_ckbcServerSelect1Btn.m_bClicked = FALSE;
	m_ckbcServerSelect2Btn.m_bClicked = TRUE;
	m_ckbcServerSelect3Btn.m_bClicked = FALSE;
	m_ckbcServerSelect1Btn.RedrawWindow();
	m_ckbcServerSelect2Btn.RedrawWindow();
	m_ckbcServerSelect3Btn.RedrawWindow();

	m_nCurrentServerListCategory = 1;
	SetServerAreaList( m_nCurrentServerListCategory );
	ResetScrollPos();
}

VOID ChinaPatchClient::OnBnClickedServer3( VOID )
{
	m_ckbcServerSelect1Btn.m_bClicked = FALSE;
	m_ckbcServerSelect2Btn.m_bClicked = FALSE;
	m_ckbcServerSelect3Btn.m_bClicked = TRUE;
	m_ckbcServerSelect1Btn.RedrawWindow();
	m_ckbcServerSelect2Btn.RedrawWindow();
	m_ckbcServerSelect3Btn.RedrawWindow();

	//. if(m_nCurrentServerListCategory == 2) newArea flag is true;
	m_nCurrentServerListCategory = 2;
	SetServerAreaList( 0, TRUE);
	ResetScrollPos();
}

VOID ChinaPatchClient::OnBnClickedMenu1( VOID )
{
	char strMsg[ 256 ];
	::LoadString( g_resInst , IDS_SITEURL1 , strMsg , 256 );

	ShellExecute( NULL , _T("open") , _T("IEXPLORE.EXE") , strMsg , NULL , SW_SHOW );
}

VOID ChinaPatchClient::OnBnClickedMenu2( VOID )
{
	char strMsg[ 256 ];
	::LoadString( g_resInst , IDS_SITEURL2 , strMsg , 256 );

	ShellExecute( NULL , _T("open") , _T("IEXPLORE.EXE") , strMsg , NULL , SW_SHOW );
}

VOID ChinaPatchClient::OnBnClickedMenu3( VOID )
{
	char strMsg[ 256 ];
	::LoadString( g_resInst , IDS_SITEURL3 , strMsg , 256 );

	ShellExecute( NULL , _T("open") , _T("IEXPLORE.EXE") , strMsg , NULL , SW_SHOW );
}

VOID ChinaPatchClient::OnBnClickedMenu4( VOID )
{
	char strMsg[ 256 ];
	::LoadString( g_resInst , IDS_SITEURL5 , strMsg , 256 );

	ShellExecute( NULL , _T("open") , _T("IEXPLORE.EXE") , strMsg , NULL , SW_SHOW );
}

VOID ChinaPatchClient::OnBnClickedMenu5( VOID )
{
	char strMsg[ 256 ];
	::LoadString( g_resInst , IDS_SITEURL4 , strMsg , 256 );

	ShellExecute( NULL , _T("open") , _T("IEXPLORE.EXE") , strMsg , NULL , SW_SHOW );
}

BOOL ChinaPatchClient::SetServerAreaList( INT iCategory , BOOL bNewArea/* =FALSE */ )
{
	//. ListBox 초기화.
	CString		strSelected;

	if (m_cServerListBox.GetCurSel() >= 0)
		m_cServerListBox.GetText(m_cServerListBox.GetCurSel(), strSelected);

	m_cServerListBox.ClearAllListItem();
	int count=m_cServerListBox.GetCount();

	int nTotalArea = (bNewArea) ? m_cPatchClientLib.GetNewAreaCount() :
		m_cPatchClientLib.GetAreaCount(iCategory);
	int lIndex = 0;

	for(int nCount = 0; nCount < nTotalArea; nCount++)
	{
		AgpdPatchAreaInfo *pdAreaInfo = (bNewArea) ? m_cPatchClientLib.GetNextNewArea(&lIndex) :
			m_cPatchClientLib.GetNextArea(iCategory, &lIndex);

		if(pdAreaInfo)
		{
			CString Temp;
			Temp.Format("%s", pdAreaInfo->m_pszName);
			m_cServerListBox.AddString(Temp);

			if (m_bShowPingTime)
			{
				if (pdAreaInfo->m_lPingTime < 0)
					Temp.Format("> %dms", PATCHCLIENT_PING_MAX_WAIT_TIME);
				else if (pdAreaInfo->m_lPingTime < 100)
					Temp.Format("< 100ms", pdAreaInfo->m_lPingTime);
				else
					Temp.Format("%dms", pdAreaInfo->m_lPingTime);

				m_cServerListBox.AddSubString(Temp);
			}
		}
	}
	m_cServerListBox.SelectString(-1, strSelected);

	m_cServerListBox.RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);

	return TRUE;
}

void ChinaPatchClient::OnCheckPVPAgreement( void )
{
	m_DlgAgreement.OnInitDialog();
	if( m_DlgAgreement.DoModal() )
	{
		( ( CPatchClientDlg* )m_pDlg )->CloseDlg();
		if( !m_DlgAgreement.IsChecked() )
		{
			_exit( 0 );
		}
	}
}
