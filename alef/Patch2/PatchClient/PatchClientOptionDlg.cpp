// PatchClientOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PatchClient.h"
#include "PatchClientOptionDlg.h"

#include "d3d9.h"
#include "cpu_info.h"
#include ".\patchclientoptiondlg.h"

#include "PatchClientDlg.h"
#include "LangControl.h"
#include "AutoDetectMemoryLeak.h"

#include "BasePatchClient.h"

#define REGIKEY_LANGUAGE			_T("Language")
#define REGIKEY_ARCHLORD			_T("SOFTWARE\\Webzen\\Archlord")


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchClientOptionDlg dialog


CPatchClientOptionDlg::CPatchClientOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchClientOptionDlg::IDD, pParent)
	, m_iSelectSlider(-1) , m_iBeforeSelectSlider(-2)
{
	//{{AFX_DATA_INIT(CPatchClientOptionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPatchClientOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchClientOptionDlg)
	DDX_Control(pDX, IDC_Option_Combo_Texture, m_cTextureCombo);
	DDX_Control(pDX, IDC_Option_Combo_Resolution, m_cResolutionCombo);
	DDX_Control(pDX, IDC_Option_Combo_WindowedMode, m_cWindowedModeCombo);

#ifdef _ENG
	DDX_Control(pDX, IDC_Option_Combo_LanguageSelect, m_cLanguageCombo);
#endif

	DDX_Control(pDX, IDC_Option_Button_Save, m_cKbcSaveButton );
	DDX_Control(pDX, IDC_Option_Button_Cancel, m_cKbcCancelButton );
	//}}AFX_DATA_MAP
	DDX_Slider(pDX, IDC_OPTION_SLIDER, m_iSelectSlider);
	DDX_Control(pDX, IDC_OPTION_SLIDER, m_ctrlSlider);
	
}


BEGIN_MESSAGE_MAP(CPatchClientOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CPatchClientOptionDlg)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_Option_Button_Save, OnOptionButtonSave)
	ON_BN_CLICKED(IDC_Option_Button_Cancel, OnOptionButtonCancel)
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_Option_Combo_Resolution, OnSelchangeOptionComboResolution)
	ON_CBN_SELCHANGE(IDC_Option_Combo_Texture, OnSelchangeOptionComboTexture)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_Option_Combo_WindowedMode, OnCbnSelchangeOptionComboWindowedMode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchClientOptionDlg message handlers
BOOL CPatchClientOptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

#ifdef _KOR
    m_cDrawFont.CreateFont( 12, 0, 0, 0, 550, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
	m_cBMPBackground.Attach( ((CPatchClientDlg*)GetParent())->LoadBitmapResource( "images\\OptionBackground_k.bm1" ) );
#elif _ENG
	m_cDrawFont.CreateFont( 11, 0, 0, 0, 550, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, /*"Arial"*/0 );
	//m_cBMPBackground.LoadBitmap( MAKEINTRESOURCE(IDB_GLOBAL_OPTION) );
	m_cBMPBackground.Attach( ((CPatchClientDlg*)GetParent())->LoadBitmapResource( "images\\OptionBackground_e.bm1" ) );
#elif _JPN
	m_cDrawFont.CreateFont( 11, 0, 0, 0, 550, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "MS PGothic"  );
	m_cBMPBackground.LoadBitmap( MAKEINTRESOURCE(IDB_OPTIONBACKGROUND) );
#elif _CHN
	m_cDrawFont.CreateFont( 13, 0, 0, 0, 550, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "SimSun" );
	m_cBMPBackground.LoadBitmap( MAKEINTRESOURCE(IDB_OPTIONBACKGROUND) );
#elif _TIW
	m_cDrawFont.CreateFont( 13, 0, 0, 0, 550, 0, 0, 0, THAI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "PMing LiU" );
	m_cBMPBackground.Attach( ((CPatchClientDlg*)GetParent())->LoadBitmapResource( "images\\OptionBackground_e.bm1" ) );
#elif _TEST_SERVER_
	m_cDrawFont.CreateFont( 12, 0, 0, 0, 550, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "굴림" );
#endif

	m_cOldPoint.x = -1;
	m_cOldPoint.y = -1;
	

	RECT rect;
	CDialog::GetWindowRect(&rect);

#if defined(_KOR) || defined(_CHN) || defined(_TIW)
	CDialog::SetWindowPos(NULL, rect.left + (GetIntFromStringTable(IDS_WIDTHSIZE)/2) - 150 , 
		rect.top + (GetIntFromStringTable(IDS_HEIGHTSIZE)/2) - 244, 300, 488, SWP_SHOWWINDOW);
#elif _ENG
	CDialog::SetWindowPos(NULL, rect.left + (GetIntFromStringTable(IDS_WIDTHSIZE)/2) - 150 , 
		rect.top + (GetIntFromStringTable(IDS_HEIGHTSIZE)/2) - 244, 300, 536, SWP_SHOWWINDOW);
#else
	CDialog::SetWindowPos(NULL, rect.left + (GetIntFromStringTable(IDS_WIDTHSIZE)/2) - 150 , 
		rect.top + (GetIntFromStringTable(IDS_HEIGHTSIZE)/2) - 244, 300, 512, SWP_SHOWWINDOW);
#endif

	m_cKbcSaveButton.SetBmpButtonImage( IDB_Button_Option_Save_Set, RGB(255,255,255) );
	m_cKbcCancelButton.SetBmpButtonImage( IDB_Button_Option_Cancel_Set, RGB(255,255,255) );

	CWnd* pcTempWnd;
	pcTempWnd = GetDlgItem( IDC_Option_Combo_Resolution );
	pcTempWnd->SetWindowPos( NULL, 152, 128, 132, 8, SWP_SHOWWINDOW );
	pcTempWnd->ModifyStyleEx( WS_EX_CLIENTEDGE, 0, 0 );

	pcTempWnd = GetDlgItem( IDC_Option_Combo_Texture );
	pcTempWnd->SetWindowPos( NULL, 152, 155, 132, 8, SWP_SHOWWINDOW );

	pcTempWnd = GetDlgItem( IDC_Option_Select_Interface );
	pcTempWnd->SetWindowPos( NULL, 152, 182, 132, 8, SWP_SHOWWINDOW );

#ifdef _ENG
	pcTempWnd = GetDlgItem( IDC_Option_Combo_LanguageSelect );
	pcTempWnd->SetWindowPos( NULL , 152 , 209 , 132 , 8 , SWP_SHOWWINDOW );
#endif

	pcTempWnd = GetDlgItem( IDC_Option_Button_Save );
#ifdef _JPN
	pcTempWnd->SetWindowPos( NULL, 41, 478-6, 89, 26, SWP_SHOWWINDOW );
#elif _ENG
	pcTempWnd->SetWindowPos( NULL, 41, 478+21, 89, 26, SWP_SHOWWINDOW );
#else
	pcTempWnd->SetWindowPos( NULL, 39, 451, 89, 26, SWP_SHOWWINDOW );
#endif

	pcTempWnd = GetDlgItem( IDC_Option_Button_Cancel );
#ifdef _JPN
	pcTempWnd->SetWindowPos( NULL, 178, 478-6, 89, 26, SWP_SHOWWINDOW );
#elif _ENG
	pcTempWnd->SetWindowPos( NULL, 178, 478+21, 89, 26, SWP_SHOWWINDOW );
#else
	pcTempWnd->SetWindowPos( NULL, 176, 451, 89, 26, SWP_SHOWWINDOW );
#endif

	INT		nOffSet	=	0;

#ifdef _ENG
	nOffSet = 27;
#endif

	pcTempWnd = GetDlgItem( IDC_OPTION_SLIDER );
	pcTempWnd->SetWindowPos( NULL, 20, 241 + nOffSet , 261 , 20, SWP_SHOWWINDOW );
	pcTempWnd->SendMessage(TBM_SETRANGE,TRUE,MAKELPARAM(-1,8));

	InitResolution();
	InitTexture();
	InitWindowed();
	InitLanguage();

	CPUInfo cCPU;
	m_strCPU = cCPU.DoesCPUSupportCPUID() ? cCPU.GetExtendedProcessorName() : "Unknwon";
	m_iPhysicalRam = GetPhysicalMemory();
	GetGraphicCardName( m_strGraphicCard );

	// preset option비교 .. 슬라이더바 초기 위치 구하기
    for( int i=0; i<9; ++i )
	{
		if( m_pcPatchOptionFile->m_stPresetInfo[i].eCharacter == m_pcPatchOptionFile->m_eCharacter &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eMap == m_pcPatchOptionFile->m_eMap &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eNature == m_pcPatchOptionFile->m_eNature &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eEffect == m_pcPatchOptionFile->m_eEffect &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eShadow == m_pcPatchOptionFile->m_eShadow &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eWater == m_pcPatchOptionFile->m_eWater &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eAutoExposure == m_pcPatchOptionFile->m_eAutoExposure &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eHighTexture == m_pcPatchOptionFile->m_eHighTexture &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eImpact == m_pcPatchOptionFile->m_eImpact &&
			m_pcPatchOptionFile->m_stPresetInfo[i].eBloomType == m_pcPatchOptionFile->m_eBloomType)
		{
			m_pcPatchOptionFile->m_iCurPresetID = i;
			m_iSelectSlider = i;
		}
	}

	UpdateData(FALSE);
        
	return TRUE;
}

BOOL CPatchClientOptionDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(wParam == IDC_OPTION_SLIDER)
	{
		UpdateData();
		if(m_iBeforeSelectSlider != m_iSelectSlider)
		{
			m_iBeforeSelectSlider = m_iSelectSlider;
			InvalidateRect(NULL);

			m_pcPatchOptionFile->m_iCurPresetID = m_iSelectSlider;
		}	
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}



void CPatchClientOptionDlg::OnPaint() 
{	
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CDC		memdc;
	wchar_t	strMsg[256];

	memdc.CreateCompatibleDC( &dc );
	memdc.SelectObject( CBitmap::FromHandle(m_cBMPBackground) );

#ifdef _ENG
	dc.BitBlt( 0, 0, 300, 536, &memdc, 0, 0, SRCCOPY );
#else
	dc.BitBlt( 0, 0, 300, 512, &memdc, 0, 0, SRCCOPY );
#endif
	
	//폰트처리
	//Text 관련 세팅.
	dc.SetBkMode(TRANSPARENT);              
	CFont* pOldFont = (CFont*)dc.SelectObject(&m_cDrawFont);
	
	dc.SetTextColor( RGB(220,220,220) );

	::LoadStringW(g_resInst, IDS_OPTIONTITLE, strMsg, 256);
#ifdef _JPN
	::TextOutW( dc.m_hDC, 125 - 14, 5, strMsg, (int) wcslen(strMsg) );
#else
	::TextOutW( dc.m_hDC, 136, 5, strMsg, (int) wcslen(strMsg) );
#endif

	::LoadStringW(g_resInst, IDS_USERSPEC, strMsg, 256);
#ifdef _DE
	::TextOutW( dc.m_hDC, 125-19, 30, strMsg, (int) wcslen(strMsg) );
#elif _ENG
	::TextOutW( dc.m_hDC, 125-9, 30, strMsg, (int) wcslen(strMsg) );
#elif _JPN
	::TextOutW( dc.m_hDC, 125 - 14, 30, strMsg, (int) wcslen(strMsg) );
#elif _TIW
	::TextOutW( dc.m_hDC, 124, 28, strMsg, (int) wcslen(strMsg) );
#else
	::TextOutW( dc.m_hDC, 125, 30, strMsg, (int) wcslen(strMsg) );
#endif

	dc.SetTextColor( RGB(137,137,137) );
	::LoadStringW(g_resInst, IDS_SCREENSIZE, strMsg, 256);
	::TextOutW( dc.m_hDC, 31, 132, strMsg, (int) wcslen(strMsg) );
	::LoadStringW(g_resInst, IDS_TETUREQUALITY, strMsg, 256);
	::TextOutW( dc.m_hDC, 31, 159, strMsg, (int) wcslen(strMsg) );
	::LoadStringW(g_resInst, IDS_WINDOWED_MODE, strMsg, 256);
	::TextOutW( dc.m_hDC, 31, 186, strMsg, (int) wcslen(strMsg) );

#ifdef _ENG
	::LoadStringW(g_resInst, IDS_LANGUAGE_SELECT, strMsg, 256);
	::TextOutW( dc.m_hDC, 31, 213, strMsg, (int) wcslen(strMsg) );
#endif
	
	// 사양 표시 
	wchar_t		OutputString[256];
	swprintf(OutputString, L"CPU : ");
	dc.SetTextColor( RGB(137,137,137) );
	::TextOutW( dc.m_hDC, 18, 57, OutputString, (int) wcslen(OutputString) );
	MultiByteToWideChar( CP_ACP, 0, (LPCSTR) m_strCPU, m_strCPU.GetLength() + 1, OutputString, 256);
	dc.SetTextColor( RGB(197,218,173) );
	::TextOutW( dc.m_hDC, 55, 57, OutputString, (int) wcslen(OutputString) );

	swprintf(OutputString, L"RAM : ");
	dc.SetTextColor( RGB(137,137,137) );
	::TextOutW( dc.m_hDC, 18, 75, OutputString, (int) wcslen(OutputString) );
	swprintf(OutputString, L"%d", m_iPhysicalRam);
	dc.SetTextColor( RGB(197,218,173) );
	::TextOutW( dc.m_hDC, 55, 75, OutputString, (int) wcslen(OutputString) );

	wcscpy(OutputString, L"VGA : "); 
	dc.SetTextColor( RGB(137,137,137) );
	::TextOutW( dc.m_hDC, 18, 95, OutputString, (int) wcslen(OutputString) );
	MultiByteToWideChar( CP_ACP, 0, (LPCSTR) m_strGraphicCard, m_strGraphicCard.GetLength() + 1, OutputString, 256);
	dc.SetTextColor( RGB(197,218,173) );
	::TextOutW( dc.m_hDC, 55, 95, OutputString, (int) wcslen(OutputString) );


	dc.SetTextColor( RGB(160,160,160) );
	::LoadStringW(g_resInst, IDS_QUICKOPTIONSPEED, strMsg, 256);
#ifdef _ENG
	::TextOutW( dc.m_hDC, 25, 314, L"X", (int) wcslen(L"X") );
	::TextOutW( dc.m_hDC, 48, 314, strMsg, (int) wcslen(strMsg) );
#else
	::TextOutW( dc.m_hDC, 25, 287, L"X", (int) wcslen(L"X") );
	::TextOutW( dc.m_hDC, 48, 287, strMsg, (int) wcslen(strMsg) );
#endif
	::LoadStringW(g_resInst, IDS_QUICKOPTIONQUALITY, strMsg, 256);
#ifdef _ENG
	::TextOutW( dc.m_hDC, 246-4, 314, strMsg, (int) wcslen(strMsg) );
#elif _JPN
	::TextOutW( dc.m_hDC, 246 - 16, 287, strMsg, (int) wcslen(strMsg) );
#else
	::TextOutW( dc.m_hDC, 246, 287, strMsg, (int) wcslen(strMsg) );
#endif

	dc.SetTextColor( RGB(220,220,220) );
	::LoadStringW(g_resInst, IDS_OPTIONTITLE2, strMsg, 256);
#ifdef _ENG
	::TextOutW( dc.m_hDC, 110+16, 247, strMsg, (int) wcslen(strMsg) );
#elif _JPN
	::TextOutW( dc.m_hDC, 110 - 4, 220, strMsg, (int) wcslen(strMsg) );
#elif _TIW
	::TextOutW( dc.m_hDC, 110, 218, strMsg, (int) wcslen(strMsg) );
#else
	::TextOutW( dc.m_hDC, 110, 220, strMsg, (int) wcslen(strMsg) );
#endif

	// 옵션 정보 표시
	COLORREF		colorMsg = RGB(137,137,137);

	wchar_t	strViewRangeBuffer1[24];
	::LoadStringW(g_resInst, IDS_RANGENEAR, strViewRangeBuffer1, 24);
	wchar_t	strViewRangeBuffer2[24];
	::LoadStringW(g_resInst, IDS_RANGEMIDDLE, strViewRangeBuffer2, 24);
	wchar_t	strViewRangeBuffer3[24];
	::LoadStringW(g_resInst, IDS_RANGEFAR, strViewRangeBuffer3, 24);
	wchar_t	strViewRangeBuffer4[24];
	::LoadStringW(g_resInst, IDS_RANGEALL, strViewRangeBuffer4, 24);

	const wchar_t *		strViewRange[4] = {strViewRangeBuffer1, strViewRangeBuffer2, strViewRangeBuffer3, strViewRangeBuffer4};
	const	COLORREF	colorViewRange[4] = {RGB(137,137,137),RGB(197,218,173),RGB(197,218,173),RGB(197,218,173)};
	
	wchar_t	strSettingBuffer1[12];
	::LoadStringW(g_resInst, IDS_OPTIONOFF, strSettingBuffer1, 12);
	wchar_t	strSettingBuffer2[12];
	::LoadStringW(g_resInst, IDS_LOW, strSettingBuffer2, 12);
	wchar_t	strSettingBuffer3[12];
	::LoadStringW(g_resInst, IDS_MIDDLE, strSettingBuffer3, 12);
	wchar_t	strSettingBuffer4[12];
	::LoadStringW(g_resInst, IDS_HIGH, strSettingBuffer4, 12);

	const wchar_t *		strSetting[4] = {strSettingBuffer1, strSettingBuffer2, strSettingBuffer3, strSettingBuffer4};
	const	COLORREF	colorSetting[4] = {RGB(137,137,137),RGB(197,218,173),RGB(197,218,173),RGB(197,218,173)};

	wchar_t	strOnOffBuffer2[12];
	::LoadStringW(g_resInst, IDS_OPTIONOFF, strOnOffBuffer2, 12);
	const wchar_t *		strOnOff[2] = {strSettingBuffer1, strOnOffBuffer2};
	const	COLORREF	colorOnOff[2] = {RGB(137,137,137),RGB(197,218,173)};

	wchar_t	strBloomBuffer1[12];
	::LoadStringW(g_resInst, IDS_BROOMTYPE1, strBloomBuffer1, 12);
	wchar_t	strBloomBuffer2[12];
	::LoadStringW(g_resInst, IDS_BROOMTYPE2, strBloomBuffer2, 12);

	const wchar_t *		strBloom[3] = {strSettingBuffer1,strBloomBuffer1, strBloomBuffer2};
	const	COLORREF	colorBloom[3] = {RGB(137,137,137),RGB(197,218,173),RGB(197,218,173)};

	if(m_iSelectSlider == -1)
	{
#ifdef _TIW
		int	y = 327;
		int x1 = 30;
		int x2 = 30;
		int x3 = 30;
#else
		int	y = 367;
		int x1 = 57;
		int x2 = 34;
		int x3 = 65;
#endif

		::LoadStringW(g_resInst, IDS_OPTIONDESC1, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );

		y += 12+12;

		::LoadStringW(g_resInst, IDS_OPTIONDESC2, strMsg, 256);
		::TextOutW( dc.m_hDC, x2, y, strMsg, (int) wcslen(strMsg) );

		y += 12+12;

		::LoadStringW(g_resInst, IDS_OPTIONDESC3, strMsg, 256);
		::TextOutW( dc.m_hDC, x3, y, strMsg, (int) wcslen(strMsg) );
	}
	else 
	{
#ifdef _ENG
		int		y = 340;
		int		x1 = 22;
		
		const int x2 = 200;
		const int cy = 14;
		
		dc.SetTextColor( colorMsg );
		
		::LoadStringW(g_resInst, IDS_CHARACTERRANGE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );

		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );

		swprintf(OutputString, L"%s",strViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eCharacter]);
		dc.SetTextColor( colorViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eCharacter] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_MAPRANGE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eMap]);
		dc.SetTextColor( colorViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eMap] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_NATUREOBJECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eNature]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eNature] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_SPECIALEFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eEffect]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eEffect] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_SHADOW, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eShadow]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eShadow] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		//y = 317;
		//x1 = 165;
		//x2 = 249;
		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_WATEREFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eWater]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eWater] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_SPECIALTEXTURE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eHighTexture]);
		dc.SetTextColor( colorOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eHighTexture] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_GLOWEFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strBloom[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eBloomType]);
		dc.SetTextColor( colorBloom[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eBloomType] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_AUTOEXPOSURE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eAutoExposure]);
		dc.SetTextColor( colorOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eAutoExposure] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += cy;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_IMPACTEFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eImpact]);
		dc.SetTextColor( colorOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eImpact] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );
#else
		int		y = 317;
		int		x1 = 14;
		int		x2 = 112;
		
		dc.SetTextColor( colorMsg );
		
		::LoadStringW(g_resInst, IDS_CHARACTERRANGE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );

		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eCharacter]);
		dc.SetTextColor( colorViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eCharacter] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_MAPRANGE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eMap]);
		dc.SetTextColor( colorViewRange[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eMap] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_NATUREOBJECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eNature]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eNature] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_SPECIALEFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eEffect]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eEffect] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_SHADOW, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eShadow]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eShadow] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y = 317;
#ifdef _TIW
		x1 = 160;
		x2 = 244;
#else
		x1 = 165;
		x2 = 249;
#endif
		
		
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_WATEREFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eWater]);
		dc.SetTextColor( colorSetting[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eWater] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_SPECIALTEXTURE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eHighTexture]);
		dc.SetTextColor( colorOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eHighTexture] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_GLOWEFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strBloom[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eBloomType]);
		dc.SetTextColor( colorBloom[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eBloomType] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_AUTOEXPOSURE, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eAutoExposure]);
		dc.SetTextColor( colorOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eAutoExposure] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );

		y += 12 + 12;
		dc.SetTextColor( colorMsg );
		::LoadStringW(g_resInst, IDS_IMPACTEFFECT, strMsg, 256);
		::TextOutW( dc.m_hDC, x1, y, strMsg, (int) wcslen(strMsg) );
		swprintf(OutputString, L":");
		::TextOutW( dc.m_hDC, x2, y, OutputString, (int) wcslen(OutputString) );
		swprintf(OutputString, L"%s",strOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eImpact]);
		dc.SetTextColor( colorOnOff[m_pcPatchOptionFile->m_stPresetInfo[m_iSelectSlider].eImpact] );
		::TextOutW( dc.m_hDC, x2 + 8, y, OutputString, (int) wcslen(OutputString) );
#endif
 	}
	
	dc.SelectObject(pOldFont);

	// Do not call CDialog::OnPaint() for painting messages
}

void CPatchClientOptionDlg::OnOptionButtonSave() 
{
	// TODO: Add your control notification handler code here
	//Save를 한다.
	m_pcPatchOptionFile->Save( PATCHCLIENT_OPTION_FILENAME );

	HKEY			hRegKey;
	DWORD			dwType;
	DWORD			dwLength = 256;

	TCHAR szLanguage[ MAX_PATH ];
	ZeroMemory( szLanguage, sizeof(szLanguage) );

	RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGIKEY_ARCHLORD, 0, KEY_WRITE, &hRegKey );
	RegQueryValueEx(hRegKey, REGIKEY_LANGUAGE, 0, &dwType, (LPBYTE) szLanguage, &dwLength );

	CString		strLanguage;
	INT			nIndex = m_cLanguageCombo.GetCurSel();

	if( nIndex != -1 )
	{
		m_cLanguageCombo.GetLBText( nIndex , strLanguage );
		RegSetValueEx( hRegKey, REGIKEY_LANGUAGE, 0, REG_SZ, (BYTE*)strLanguage.GetString() , strLanguage.GetLength() ); 
	}

	RegCloseKey(hRegKey); 

	OnOK();
}

void CPatchClientOptionDlg::OnOptionButtonCancel() 
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CPatchClientOptionDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( nFlags == MK_LBUTTON )
	{
		SetCapture();

		RECT		cRect;
		int			iTempX, iTempY;

		GetWindowRect( &cRect );

		iTempX = 0;
		iTempY = 0;
		
		if( (m_cOldPoint.x == -1) && (m_cOldPoint.y == -1) )
		{
			//그냥 세팅이라 아무일도 없다.
		}
		else if( (m_cOldPoint.x == point.x) && (m_cOldPoint.y == point.y) )
		{
			//그냥 세팅이라 아무일도 없다.
		}
		else
		{
			iTempX = point.x - m_cOldPoint.x;
			iTempY = point.y - m_cOldPoint.y;

			SetWindowPos( NULL, cRect.left + iTempX, cRect.top + iTempY, cRect.right - cRect.left, cRect.bottom - cRect.top, SWP_SHOWWINDOW ); 
		}

		m_cOldPoint.x = point.x - iTempX;
		m_cOldPoint.y = point.y - iTempY;
	}
	else
	{
		ReleaseCapture();

 		m_cOldPoint.x = -1;
		m_cOldPoint.y = -1;
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

HBRUSH CPatchClientOptionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor( RGB( 255,255,255) );

	pDC->SetBkColor( RGB(50,50,50) );

	return CreateSolidBrush( RGB(50,50,50) );
}

BOOL CPatchClientOptionDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
		return TRUE;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CPatchClientOptionDlg::OnSelchangeOptionComboResolution() 
{
	// TODO: Add your control notification handler code here
	int iCurSel = m_cResolutionCombo.GetCurSel();
	if( iCurSel != CB_ERR )
	{
		void* pvData = m_cResolutionCombo.GetItemDataPtr( iCurSel );
		if( pvData )
			m_pcPatchOptionFile->SetResolution( (CPatchOptionResolution *)pvData );
	}
}

void CPatchClientOptionDlg::OnSelchangeOptionComboTexture() 
{
	// TODO: Add your control notification handler code here
	int iCurSel = m_cTextureCombo.GetCurSel();
	if( iCurSel != CB_ERR )
		m_pcPatchOptionFile->SetTextureQuality( iCurSel );
}

void CPatchClientOptionDlg::OnCbnSelchangeOptionComboWindowedMode()
{
	// TODO: Add your control notification handler code here
	int iCurSel = m_cWindowedModeCombo.GetCurSel();
	if( iCurSel != CB_ERR )
		m_pcPatchOptionFile->SetWindowedMode((eWindowedMode) iCurSel);
}

void CPatchClientOptionDlg::InitResolution()
{
	DEVMODE curMode;
	EnumDisplaySettings( NULL , ENUM_CURRENT_SETTINGS , &curMode );	

	DEVMODE			cDevMode;
	bool			bSearchResolution;

	bSearchResolution = false;

	for( int iMode=0; ; iMode++ )
	{
		if( !EnumDisplaySettings( NULL, iMode, &cDevMode ) )		break;
			
#if defined( _KOR ) || defined( _TIW ) || defined( _ENG )
		if( cDevMode.dmPelsWidth >= 1024 && cDevMode.dmPelsHeight >= 768 && cDevMode.dmBitsPerPel >= 16
			&& cDevMode.dmPelsHeight <= curMode.dmPelsHeight && cDevMode.dmPelsWidth <= curMode.dmPelsWidth )
#else
		if( (cDevMode.dmPelsWidth == 1024 && cDevMode.dmPelsHeight == 768 && cDevMode.dmBitsPerPel >= 16) || 
			(cDevMode.dmPelsWidth == 1280 && cDevMode.dmPelsHeight == 1024 && cDevMode.dmBitsPerPel >= 16) || 
			(cDevMode.dmPelsWidth == 1600 && cDevMode.dmPelsHeight == 1200 && cDevMode.dmBitsPerPel >= 16) )
#endif
		{
			int	 iSameResuluionIndex = CB_ERR;
			bool bExist = false;

			for( int iCounter = 0; iCounter<m_cResolutionCombo.GetCount(); ++iCounter )
			{
				void* pvData = m_cResolutionCombo.GetItemDataPtr( iCounter );
				if( !pvData )		continue;

				//해상도, 비트가 같다면?
				if( (((CPatchOptionResolution*)pvData)->m_iWidth == (int)cDevMode.dmPelsWidth ) &&
					(((CPatchOptionResolution*)pvData)->m_iHeight == (int)cDevMode.dmPelsHeight ) )
				{
					iSameResuluionIndex = iCounter;

					if(((CPatchOptionResolution*)pvData)->m_iBit == (int)cDevMode.dmBitsPerPel )
						bExist = true;
				}
			}

			if( !bExist )
			{
				char	strBuffer[255];
				int		iIndex;

				CPatchOptionResolution* pcData = new CPatchOptionResolution( cDevMode.dmPelsWidth, cDevMode.dmPelsHeight, cDevMode.dmBitsPerPel );
				if( iSameResuluionIndex == CB_ERR )
				{
					sprintf( strBuffer, "%d*%d*%dbit", pcData->m_iWidth, pcData->m_iHeight, pcData->m_iBit );
					iIndex = m_cResolutionCombo.AddString(strBuffer);
				}
				else
				{
					sprintf( strBuffer, "%d*%d*%dbit", pcData->m_iWidth, pcData->m_iHeight, pcData->m_iBit );
					iIndex = m_cResolutionCombo.InsertString(iSameResuluionIndex+1,strBuffer);
				}

				m_cResolutionCombo.SetItemDataPtr( iIndex, pcData );

				CPatchOptionResolution* pcTempResolution = m_pcPatchOptionFile->GetResolution();

				if( (pcTempResolution->m_iWidth == pcData->m_iWidth) &&
					(pcTempResolution->m_iHeight == pcData->m_iHeight) &&
					(pcTempResolution->m_iBit == pcData->m_iBit) )
				{
					m_cResolutionCombo.SetCurSel( iIndex );
					bSearchResolution = true;
				}
			}
		}
	}

	if(	!bSearchResolution && m_cResolutionCombo.GetCount() > 0 )
		m_cResolutionCombo.SetCurSel( 0 );
}

void CPatchClientOptionDlg::InitTexture()
{
	m_cTextureCombo.ResetContent();

	wchar_t			strQualityW[32];
	char			strQuality[32];
	BOOL			bRet;

	::LoadStringW( g_resInst, IDS_HIGH, strQualityW, 32 );
	WideCharToMultiByte( CP_ACP, 0, strQualityW, (int) wcslen(strQualityW) + 1, strQuality, 32, "", &bRet);
	m_cTextureCombo.AddString( strQuality );

	::LoadStringW( g_resInst, IDS_MIDDLE, strQualityW, 32 );
	WideCharToMultiByte( CP_ACP, 0, strQualityW, (int) wcslen(strQualityW) + 1, strQuality, 32, "", &bRet);
	m_cTextureCombo.AddString( strQuality );

	::LoadStringW( g_resInst, IDS_LOW, strQualityW, 32 );
	WideCharToMultiByte( CP_ACP, 0, strQualityW, (int) wcslen(strQualityW) + 1, strQuality, 32, "", &bRet);
	m_cTextureCombo.AddString( strQuality );

	m_cTextureCombo.SetCurSel( m_pcPatchOptionFile->GetTextureQuality() );
}

void CPatchClientOptionDlg::InitWindowed()
{
	wchar_t			strTypeW[32];
	char			strType[32];
	BOOL			bRet;

	m_cWindowedModeCombo.ResetContent();


	::LoadStringW(g_resInst, IDS_OPTIONOFF, strTypeW, 32);	
	WideCharToMultiByte( CP_ACP, 0, strTypeW, (int) wcslen(strTypeW) + 1, strType, 32, "", &bRet);
	m_cWindowedModeCombo.AddString( strType );

	::LoadStringW(g_resInst, IDS_OPTIONON, strTypeW, 32);	
	WideCharToMultiByte( CP_ACP, 0, strTypeW, (int) wcslen(strTypeW) + 1, strType, 32, "", &bRet);
	m_cWindowedModeCombo.AddString( strType );

	m_cWindowedModeCombo.SetCurSel((int) m_pcPatchOptionFile->GetWindowedMode());


}

void CPatchClientOptionDlg::InitLanguage()
{

	wchar_t			strTypeW[32];
	char			strType[32];
	BOOL			bRet;

	m_cLanguageCombo.ResetContent();

	::LoadStringW(g_resInst, IDS_LANGUAGE_ENGLISH, strTypeW, 32);	
	WideCharToMultiByte( CP_ACP, 0, strTypeW, (int) wcslen(strTypeW) + 1, strType, 32, "", &bRet);
	m_cLanguageCombo.AddString( strType );

	::LoadStringW(g_resInst, IDS_LANGUAGE_FRENCH, strTypeW, 32);	
	WideCharToMultiByte( CP_ACP, 0, strTypeW, (int) wcslen(strTypeW) + 1, strType, 32, "", &bRet);
	m_cLanguageCombo.AddString( strType );

	::LoadStringW(g_resInst, IDS_LANGUAGE_GERMAN, strTypeW, 32);	
	WideCharToMultiByte( CP_ACP, 0, strTypeW, (int) wcslen(strTypeW) + 1, strType, 32, "", &bRet);
	m_cLanguageCombo.AddString( strType );

	::LoadStringW(g_resInst, IDS_LANGUAGE_PORTUGUESE, strTypeW, 32);	
	WideCharToMultiByte( CP_ACP, 0, strTypeW, (int) wcslen(strTypeW) + 1, strType, 32, "", &bRet);
	m_cLanguageCombo.AddString( strType );

	::LoadStringW(g_resInst, IDS_LANGUAGE_SPANISH, strTypeW, 32);	
	WideCharToMultiByte( CP_ACP, 0, strTypeW, (int) wcslen(strTypeW) + 1, strType, 32, "", &bRet);
	m_cLanguageCombo.AddString( strType );

	HKEY			hRegKey;
	DWORD			dwType;
	DWORD			dwLength = 256;
	
	TCHAR szLanguage[ MAX_PATH ];
	ZeroMemory( szLanguage, sizeof(szLanguage) );

	RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGIKEY_ARCHLORD, 0, KEY_READ, &hRegKey );
	RegQueryValueEx(hRegKey, REGIKEY_LANGUAGE, 0, &dwType, (LPBYTE) szLanguage, &dwLength );

	m_cLanguageCombo.SetCurSel( GetLanguageNumber( szLanguage ) );

	RegCloseKey(hRegKey);
}

INT	CPatchClientOptionDlg::GetLanguageNumber( const CString& strLanguage )
{
	if( !strLanguage )
		return -1;

	wchar_t			strTypeW[32];
	char			strType[32];
	BOOL			bRet;

	LoadString( g_resInst , IDS_LANGUAGE_ENGLISH , strType , 32 );
	if( !strLanguage.Compare( strType ) )
		return 0;

	LoadString(g_resInst, IDS_LANGUAGE_FRENCH, strType, 32);	
	if( !strLanguage.Compare( strType ) )
		return 1;

	LoadString(g_resInst, IDS_LANGUAGE_GERMAN, strType, 32);	
	if( !strLanguage.Compare( strType ) )
		return 2;

	LoadString(g_resInst, IDS_LANGUAGE_PORTUGUESE, strType, 32);	
	if( !strLanguage.Compare( strType ) )
		return 3;

	LoadString(g_resInst, IDS_LANGUAGE_SPANISH, strType, 32);	
	if( !strLanguage.Compare( strType ) )
		return 4;

	return 0;
}