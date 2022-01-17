// SkySettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "SkySettingDlg.h"
#include "clipplanesetdlg.h"
#include "EffectEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkySettingDlg dialog


CSkySettingDlg::CSkySettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSkySettingDlg::IDD, pParent)
	, m_bWeaLensFlare	(FALSE)
	, m_bWeaMoon		(FALSE)
	, m_bWeaBreath		(FALSE)
	, m_bWeaStar		(FALSE)
	, m_bWeaPLightOff	(FALSE)
	, m_bWeaNEDisable	(FALSE)
	, m_bWea_Reserved1	(FALSE)
	, m_bWea_Reserved2	(FALSE)
	, m_bWea_Reserved3	(FALSE)
{
	//{{AFX_DATA_INIT(CSkySettingDlg)
	m_fSkyRadius	= 0.0f;
	m_fCloudSpeed1 = 0.0f;
	m_fCloudSpeed2 = 0.0f;
	m_nEffectInterval = 0;
	m_bUseCloud	= FALSE;
	m_bUseColor	= FALSE;
	m_bUseEffect= FALSE;
	m_bUseFog	= FALSE;
	m_bUseBGM	= FALSE;
	m_bUseWeather = FALSE;
	m_strBackgroundMusic = _T("");
	m_fFogDistance = 0.0f;
	m_fFogFarClip = 0.0f;
	//}}AFX_DATA_INIT

	m_nTemplateID		= -1	;	// 템플릿 아이디..
	m_pAgpdSkySet		= NULL	;
	m_pAgcdSkySet		= NULL	;
	m_pEventNatureDlg	= NULL	;
}


void CSkySettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkySettingDlg)
	DDX_Control(pDX, IDC_CLOUD2_COLOR, m_ctlCloud2Color);
	DDX_Control(pDX, IDC_CLOUD1_COLOR, m_ctlCloud1Color);
	DDX_Control(pDX, IDC_CIRCUMSTANCE_COLOR, m_ctlCircumstanceColor);
	DDX_Control(pDX, IDC_CIRCUMSTANCE_ALPHA, m_ctlCircumstanceAlpha);
	DDX_Control(pDX, IDC_CLOUD2_ALPHA, m_ctlCloud2Alpha);
	DDX_Control(pDX, IDC_CLOUD1_ALPHA, m_ctlCloud1Alpha);
	DDX_Control(pDX, IDC_AMBIENT_EFFECT_LIST, m_ctlEffectList);
	DDX_Control(pDX, IDC_CIRCUMSTANCE_TEXTURE, m_ctlCircumstanceTexture);
	DDX_Control(pDX, IDC_CLOUD_TEXTURE1, m_ctlCloud1ImagePreview);
	DDX_Control(pDX, IDC_CLOUD_TEXTURE2, m_ctlCloud2ImagePreview);
	DDX_Control(pDX, IDC_HSV_SKYTOP, m_ctlSkyTop);
	DDX_Control(pDX, IDC_HSV_SKYBOTTOM, m_ctlSkyBottom);
	DDX_Control(pDX, IDC_HSV_FOG, m_ctlFog);
	DDX_Control(pDX, IDC_HSV_DIRECTIONAL, m_ctlDirectional);
	DDX_Control(pDX, IDC_HSV_AMBIENT, m_ctlAmbient);
	DDX_Control(pDX, IDC_TEXTURELIST, m_ctlTextureList);
	DDX_Control(pDX, IDC_TIMEZONE, m_ctlTimeSelectStatic);
	DDX_Text(pDX, IDC_SKYRADIUS, m_fSkyRadius);
	DDX_Text(pDX, IDC_CLOUDSPEED1, m_fCloudSpeed1);
	DDX_Text(pDX, IDC_CLOUDSPEED2, m_fCloudSpeed2);
	DDX_Text(pDX, IDC_EFFECTINTERVAL, m_nEffectInterval);
	DDX_Check(pDX, IDC_USE_CLOUD, m_bUseCloud);
	DDX_Check(pDX, IDC_USE_COLOR, m_bUseColor);
	DDX_Check(pDX, IDC_USE_EFFECT, m_bUseEffect);
	DDX_Check(pDX, IDC_USE_FOG, m_bUseFog);
	DDX_Check(pDX, IDC_BGM, m_bUseBGM);
	DDX_Check(pDX, IDC_WEATHER, m_bUseWeather);
	DDX_Text(pDX, IDC_BACKGROUNDMUSIC, m_strBackgroundMusic);
	DDX_Text(pDX, IDC_FOG_DISTANCE, m_fFogDistance);
	DDX_Text(pDX, IDC_FOG_FARCLIP, m_fFogFarClip);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_WEA_LENSFLARE	, m_bWeaLensFlare	);
	DDX_Check(pDX, IDC_WEA_MOON			, m_bWeaMoon		);
	DDX_Check(pDX, IDC_WEA_SHOWBREATH	, m_bWeaBreath		);
	DDX_Check(pDX, IDC_WEA_SHOWSTAR		, m_bWeaStar		);
	DDX_Check(pDX, IDC_WEA_PLIGHT_OFF	, m_bWeaPLightOff	);
	DDX_Check(pDX, IDC_WEA_DISABLE_WEATHER_EFFECT	, m_bWeaNEDisable	);
	DDX_Check(pDX, IDC_WEA_RESERVED_1	, m_bWea_Reserved1	);
	DDX_Check(pDX, IDC_WEA_RESERVED_2	, m_bWea_Reserved2	);
	DDX_Check(pDX, IDC_WEA_RESERVED_3	, m_bWea_Reserved3	);
	DDX_Control(pDX, IDC_SUN_TEXTURE, m_ctlSunTexture);
	DDX_Control(pDX, IDC_SUN_COLOR, m_ctlSunColor);
	DDX_Control(pDX, IDC_SUN_ALPHA, m_ctlSunAlpha);
}


BEGIN_MESSAGE_MAP(CSkySettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSkySettingDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SETFOG, OnSetfog)
	ON_BN_CLICKED(IDC_EFFECT_ADD, OnEffectAdd)
	ON_BN_CLICKED(IDC_EFFECT_DELETE, OnEffectDelete)
	ON_BN_CLICKED(IDC_EFFECT_EDIT, OnEffectEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_AMBIENT_EFFECT_LIST, OnDblclkEffectList)
	ON_BN_CLICKED(IDC_USE_COLOR		, OnChangeDataFlag)
	ON_BN_CLICKED(IDC_BROWSE, OnBackgroundMusicBrowse)
	ON_BN_CLICKED(IDC_USE_CLOUD		, OnChangeDataFlag)
	ON_BN_CLICKED(IDC_USE_FOG		, OnChangeDataFlag)
	ON_BN_CLICKED(IDC_USE_EFFECT	, OnChangeDataFlag)
	ON_BN_CLICKED(IDC_BGM			, OnChangeDataFlag)
	ON_BN_CLICKED(IDC_WEATHER		, OnChangeDataFlag)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SET_WEATHER, OnBnClickedSetWeather)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkySettingDlg message handlers

BOOL CSkySettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect	rect;
	m_ctlEffectList.GetClientRect( rect );
	// List Control 탭설정..
	LV_COLUMN	lvcolumn;
	lvcolumn.mask		=	LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt		=	LVCFMT_LEFT										;
	lvcolumn.iSubItem	=	0												;
	lvcolumn.pszText	=	"Effect"										;
	lvcolumn.cx			=	rect.Width() - 50								;

	m_ctlEffectList.InsertColumn( 0 , & lvcolumn );

	lvcolumn.iSubItem	=	1												;
	lvcolumn.pszText	=	"Rate"											;
	lvcolumn.cx			=	50												;

	m_ctlEffectList.InsertColumn( 1 , & lvcolumn );
	
	m_ctlCircumstanceAlpha	.Init( CHSVControlStatic::ALPHATYPE , RGB( 255 , 255 , 255 ) );
	m_ctlSunAlpha			.Init( CHSVControlStatic::ALPHATYPE , RGB( 255 , 255 , 255 ) );
	m_ctlCloud2Alpha		.Init( CHSVControlStatic::ALPHATYPE , RGB( 255 , 255 , 255 ) );
	m_ctlCloud1Alpha		.Init( CHSVControlStatic::ALPHATYPE , RGB( 255 , 255 , 255 ) );
	
	// 이전 타이머 설정 저장..
	m_bPrevTimerSetting	=	m_pEventNatureDlg->m_pcsAgpmEventNature->GetTimer		();
	m_uPrevSpeedRate	=	m_pEventNatureDlg->m_pcsAgpmEventNature->GetSpeedRate	();

	m_pEventNatureDlg->m_pcsAgpmEventNature->StopTimer();	// 그대로 멈춰라!..
	m_pEventNatureDlg->m_pcsAgpmEventNature->SetTime( 0 );
	m_pEventNatureDlg->m_pcsAgpmEventNature->SetSpeedRate( 1 );
	m_pEventNatureDlg->p__SetTimeFunction( m_pEventNatureDlg->m_pcsAgcmEventNature , 0 , 0 );

	SetTime( 0 );

	SetWindowText( "\"" + m_strTitle + "\" 의 설정." );

	OnChangeDataFlag();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CSkySettingDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_TIMESELECT_EVENT:
		{
			TRACE( "CSkySettingDlg::WindowProc 메시지 수신.. \n" );

			// 현재 데이타 백업..
			int	prevTime	= static_cast<int>( wParam );
			if( prevTime >= 0  && prevTime < ASkySetting::TS_MAX )
			{
				UpdateData( TRUE );

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyTop.red			= ( FLOAT ) m_ctlSkyTop			.m_nR / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyTop.green		= ( FLOAT ) m_ctlSkyTop			.m_nG / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyTop.blue			= ( FLOAT ) m_ctlSkyTop			.m_nB / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyBottom.red		= ( FLOAT ) m_ctlSkyBottom		.m_nR / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyBottom.green		= ( FLOAT ) m_ctlSkyBottom		.m_nG / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyBottom.blue		= ( FLOAT ) m_ctlSkyBottom		.m_nB / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbAmbient.red			= ( FLOAT ) m_ctlAmbient		.m_nR / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbAmbient.green		= ( FLOAT ) m_ctlAmbient		.m_nG / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbAmbient.blue		= ( FLOAT ) m_ctlAmbient		.m_nB / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbDirectional.red		= ( FLOAT ) m_ctlDirectional	.m_nR / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbDirectional.green	= ( FLOAT ) m_ctlDirectional	.m_nG / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbDirectional.blue	= ( FLOAT ) m_ctlDirectional	.m_nB / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbFog.red				= ( FLOAT ) m_ctlFog			.m_nR / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbFog.green			= ( FLOAT ) m_ctlFog			.m_nG / 255.0f;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbFog.blue			= ( FLOAT ) m_ctlFog			.m_nB / 255.0f;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.alpha	= m_ctlCloud1Alpha		.m_nR;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha	= m_ctlCloud2Alpha		.m_nR;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.alpha									= m_ctlCircumstanceAlpha.m_nR;
			
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.red		= m_ctlCloud1Color		.m_nR;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.green	= m_ctlCloud1Color		.m_nG;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.blue		= m_ctlCloud1Color		.m_nB;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.red		= m_ctlCloud2Color		.m_nR;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.green	= m_ctlCloud2Color		.m_nG;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.blue		= m_ctlCloud2Color		.m_nB;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.red									= m_ctlCircumstanceColor.m_nR;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.green									= m_ctlCircumstanceColor.m_nG;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.blue									= m_ctlCircumstanceColor.m_nB;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSun.red												= m_ctlSunColor.m_nR;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSun.green											= m_ctlSunColor.m_nG;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSun.blue											= m_ctlSunColor.m_nB;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].nCircumstanceTexture	= m_ctlCircumstanceTexture.m_nIndex;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].nSunTexture			= m_ctlSunTexture.m_nIndex;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].nTexture	= m_ctlCloud1ImagePreview.m_nIndex;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].nTexture	= m_ctlCloud2ImagePreview.m_nIndex;
				
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].fSpeed = m_fCloudSpeed1;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].fSpeed = m_fCloudSpeed2;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].fFogDistance		= m_fFogDistance	;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].fFogFarClip		= m_fFogFarClip		;

				m_pAgcdSkySet->m_aSkySetting[ prevTime ].nEffectInterval	= m_nEffectInterval	;

				strncpy( m_pAgcdSkySet->m_aSkySetting[ prevTime ].strBackMusic , (LPCTSTR) m_strBackgroundMusic , EFF2_FILE_NAME_MAX );

				//m_pAgcdSkySet->m_aSkySetting[ prevTime ].nWeatherEffect1	= m_nWeatherEffect1	;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].nWeatherEffect1 =
					( m_bWeaLensFlare	? ASkySetting::USE_LENS_FLARE		: 0 ) |
					( m_bWeaMoon		? ASkySetting::SHOW_MOON			: 0 ) |
					( m_bWeaBreath		? ASkySetting::SHOW_BREATH			: 0 ) |
					( m_bWeaStar		? ASkySetting::SHOW_STAR			: 0 ) |
					( m_bWeaPLightOff	? ASkySetting::DISABLE_POINT_LIGHT	: 0 ) |
					( m_bWeaNEDisable	? ASkySetting::DISABLE_WEATHER		: 0 ) |
					( m_bWea_Reserved1	? ASkySetting::RESERVED_1			: 0 ) |
					( m_bWea_Reserved2	? ASkySetting::RESERVED_2			: 0 ) |
					( m_bWea_Reserved3	? ASkySetting::RESERVED_3			: 0 ) ;
				m_pAgcdSkySet->m_aSkySetting[ prevTime ].nWeatherEffect2	= -1	;

				// 초기화..
				int i;
				for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
				{
					m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectArray		[ i ]	= 0	;
					m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectRateArray	[ i ]	= 0	;
				}

				//LV_ITEM	lvItem	;
				CString	str		;
				for( i  = 0 ; i < m_ctlEffectList.GetItemCount() ; ++ i )
				{
					str = m_ctlEffectList.GetItemText( i , 0 );
					m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectArray		[ i ]	= atoi( ( LPCTSTR ) str );
					str = m_ctlEffectList.GetItemText( i , 1 );
					m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectRateArray	[ i ]	= atoi( ( LPCTSTR ) str );
				}
			}

			SetTime( static_cast<int>( lParam ) );
		}
		break;

	case WM_HSVCONTROL_NOTIFY:
		{
			// 현재 설정을 저장함..
			
			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
			INT32	nTime	;
			INT32	i		;
			
			switch( wParam )
			{
			case IDC_HSV_SKYTOP			:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSkyTop.red			= ( FLOAT ) m_ctlSkyTop			.m_nR / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSkyTop.green		= ( FLOAT ) m_ctlSkyTop			.m_nG / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSkyTop.blue		= ( FLOAT ) m_ctlSkyTop			.m_nB / 255.0f;
				}
				break;
			case IDC_HSV_SKYBOTTOM		:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSkyBottom.red		= ( FLOAT ) m_ctlSkyBottom		.m_nR / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSkyBottom.green	= ( FLOAT ) m_ctlSkyBottom		.m_nG / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSkyBottom.blue		= ( FLOAT ) m_ctlSkyBottom		.m_nB / 255.0f;
				}
				break;
			case IDC_HSV_DIRECTIONAL	:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbDirectional.red	= ( FLOAT ) m_ctlDirectional	.m_nR / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbDirectional.green	= ( FLOAT ) m_ctlDirectional	.m_nG / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbDirectional.blue	= ( FLOAT ) m_ctlDirectional	.m_nB / 255.0f;
				}
				break;
			case IDC_HSV_AMBIENT		:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbAmbient.red		= ( FLOAT ) m_ctlAmbient		.m_nR / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbAmbient.green		= ( FLOAT ) m_ctlAmbient		.m_nG / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbAmbient.blue		= ( FLOAT ) m_ctlAmbient		.m_nB / 255.0f;
				}
				break;
			case IDC_HSV_FOG			:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbFog.red			= ( FLOAT ) m_ctlFog			.m_nR / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbFog.green			= ( FLOAT ) m_ctlFog			.m_nG / 255.0f;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbFog.blue			= ( FLOAT ) m_ctlFog			.m_nB / 255.0f;
				}
				break;
			case IDC_CLOUD1_COLOR		:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.red			= m_ctlCloud1Color	.m_nR;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.green		= m_ctlCloud1Color	.m_nG;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.blue			= m_ctlCloud1Color	.m_nB;
				}
				break;
			case IDC_CLOUD2_COLOR		:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.red			= m_ctlCloud2Color	.m_nR;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.green		= m_ctlCloud2Color	.m_nG;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.blue			= m_ctlCloud2Color	.m_nB;
				}
				break;
			case IDC_CIRCUMSTANCE_COLOR	:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbCircumstance.red		= m_ctlCircumstanceColor	.m_nR;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbCircumstance.green		= m_ctlCircumstanceColor	.m_nG;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbCircumstance.blue		= m_ctlCircumstanceColor	.m_nB;
				}
				break;
			case IDC_SUN_COLOR	:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSun.red		= m_ctlSunColor	.m_nR;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSun.green		= m_ctlSunColor	.m_nG;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSun.blue		= m_ctlSunColor	.m_nB;
				}
				break;
			case IDC_CLOUD1_ALPHA		:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.alpha			= m_ctlCloud1Alpha	.m_nR;
				}
				break;
			case IDC_CLOUD2_ALPHA		:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha			= m_ctlCloud2Alpha	.m_nR;
				}
				break;

			case IDC_CIRCUMSTANCE_ALPHA	:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbCircumstance.alpha			= m_ctlCircumstanceAlpha	.m_nR;
				}
				break;
			case IDC_SUN_ALPHA	:
				for( i = 0 ; i < nCount ; i ++ )
				{
					nTime	= m_ctlTimeSelectStatic.GetTime( i );

					m_pAgcdSkySet->m_aSkySetting[ nTime ].rgbSun.alpha			= m_ctlSunAlpha	.m_nR;
				}
				break;
			default:
				break;
			}

			if( nCount )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime();

				m_pEventNatureDlg->m_pcsAgpmEventNature->SetTime(
					ASkySetting::GetRealTime( nTime )	,
					!m_pEventNatureDlg->m_pcsAgpmEventNature->GetMinute( m_pEventNatureDlg->m_pcsAgpmEventNature->GetGameTime() ) );
				
				//m_pEventNatureDlg->m_pcsAgcmEventNature->ApplySkySetting();
				m_pEventNatureDlg->p__SetSkyTemplateID( m_pEventNatureDlg->m_pcsAgcmEventNature , m_pAgcdSkySet->m_aSkySetting , m_pAgcdSkySet->m_aSkySetting->nDataAvailableFlag );
				m_pEventNatureDlg->p__SetTimeFunction( m_pEventNatureDlg->m_pcsAgcmEventNature , ASkySetting::GetRealTime( nTime ) , 0 );
			}
		}
		break;

	case WM_TEXTURE_DROP_EVENT:
		{
			POINT	point;
			point.x	= static_cast<int>( wParam );
			point.y = static_cast<int>( lParam );
			ProcessLButtonUp( point );
		}
		break;
		
	case WM_IMAGEPREVIEW_NOTIFY_EVENT:
		{
			// 텍스쳐 삭제 이벤트..
			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
			INT32	nTime	;

			for( int i = 0 ; i < nCount ; i ++ )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( i );

				m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_FIRST	].nTexture	= m_ctlCloud1ImagePreview.	GetIndex();
				m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_SECOND	].nTexture	= m_ctlCloud2ImagePreview.	GetIndex();
				m_pAgcdSkySet->m_aSkySetting[ nTime ].nCircumstanceTexture							= m_ctlCircumstanceTexture.	GetIndex();
				m_pAgcdSkySet->m_aSkySetting[ nTime ].nSunTexture									= m_ctlSunTexture.			GetIndex();
			}

			if( nCount )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( 0 );
				SetTime( nTime	);
			}
		}
		break;
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL	CSkySettingDlg::SetTime( INT32 time )	// 0~23..
{
	ASSERT( 0 <= time && time < ASkySetting::TS_MAX );

	// 현재 시간의 스카이 설정으로 화면을변경한다..
	// m_pEventNatureDlg->m_pcsAgcmEventNature->ApplySkySetting();
	m_pEventNatureDlg->p__SetSkyTemplateID( m_pEventNatureDlg->m_pcsAgcmEventNature , m_pAgcdSkySet->m_aSkySetting , m_pAgcdSkySet->m_aSkySetting->nDataAvailableFlag );
	m_pEventNatureDlg->p__SetTimeFunction( m_pEventNatureDlg->m_pcsAgcmEventNature , ASkySetting::GetRealTime( time ) , 0 );

	// 컨트롤들의 값을 변경함..

	// Light 값 변경
	m_ctlSkyTop.SetRGB(
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbSkyTop.red			),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbSkyTop.green		),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbSkyTop.blue		));
	m_ctlSkyBottom.SetRGB(
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbSkyBottom.red		),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbSkyBottom.green	),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbSkyBottom.blue		));
	m_ctlAmbient.SetRGB(
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbAmbient.red		),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbAmbient.green		),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbAmbient.blue		));
	m_ctlDirectional.SetRGB(
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbDirectional.red	),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbDirectional.green	),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbDirectional.blue	));
	m_ctlFog.SetRGB(
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbFog.red			),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbFog.green			),
		( INT32 ) ( 255.0f * m_pAgcdSkySet->m_aSkySetting[ time ].rgbFog.blue			));

	// 텍스쳐 정보 변경..
	m_ctlCircumstanceTexture.	SetIndex( m_pAgcdSkySet->m_aSkySetting[ time ].nCircumstanceTexture	);
	m_ctlSunTexture.			SetIndex( m_pAgcdSkySet->m_aSkySetting[ time ].nSunTexture			);

	m_ctlCloud1ImagePreview	.SetIndex( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].nTexture	);
	m_ctlCloud2ImagePreview	.SetIndex( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].nTexture	);

	m_fCloudSpeed1	= m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].fSpeed;
	m_fCloudSpeed2	= m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].fSpeed;

	m_fFogDistance	= m_pAgcdSkySet->m_aSkySetting[ time ].fFogDistance	;
	m_fFogFarClip	= m_pAgcdSkySet->m_aSkySetting[ time ].fFogFarClip	;

	m_nEffectInterval	=	m_pAgcdSkySet->m_aSkySetting[ time ].nEffectInterval;

	m_strBackgroundMusic	= m_pAgcdSkySet->m_aSkySetting[ time ].strBackMusic;

	//m_nWeatherEffect1	= m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1;
	m_bWeaLensFlare	= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::USE_LENS_FLARE		) ? TRUE : FALSE;
	m_bWeaMoon		= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::SHOW_MOON			) ? TRUE : FALSE;
	m_bWeaBreath	= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::SHOW_BREATH			) ? TRUE : FALSE;
	m_bWeaStar		= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::SHOW_STAR			) ? TRUE : FALSE;
	m_bWeaPLightOff	= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::DISABLE_POINT_LIGHT	) ? TRUE : FALSE;
	m_bWeaNEDisable	= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::DISABLE_WEATHER		) ? TRUE : FALSE;
	m_bWea_Reserved1= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::RESERVED_1			) ? TRUE : FALSE;
	m_bWea_Reserved2= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::RESERVED_2			) ? TRUE : FALSE;
	m_bWea_Reserved3= ( m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect1 & ASkySetting::RESERVED_3			) ? TRUE : FALSE;


	//m_nWeatherEffect2	= m_pAgcdSkySet->m_aSkySetting[ time ].nWeatherEffect2;

	UpdateData( FALSE );

	// 리스트 컨트롤에 데이타 추가..

	m_ctlEffectList.DeleteAllItems();

	LV_ITEM	lvItem	;
	CString	str		;
	INT32	nCount	= 0;

	for( int i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
	{
		if( m_pAgcdSkySet->m_aSkySetting[ time ].aEffectArray[ i ] != 0 )
		{
			// 이펙트 스트링을 얻어야하는부분... 차후 수정..
			str.Format( "%d Effect" , m_pAgcdSkySet->m_aSkySetting[ time ].aEffectArray[ i ] );

			lvItem.mask		= LVIF_TEXT	;
			lvItem.iItem	= nCount	;
			lvItem.iSubItem	= 0			;
			lvItem.pszText	= (LPSTR)(LPCTSTR)	str;

			m_ctlEffectList.InsertItem( &lvItem );

			str.Format( "%d%%" , m_pAgcdSkySet->m_aSkySetting[ time ].aEffectRateArray[ i ] );

			lvItem.mask		= LVIF_TEXT	;
			lvItem.iItem	= nCount	;
			lvItem.iSubItem	= 1			;
			lvItem.pszText	= (LPSTR)(LPCTSTR)	str;

			m_ctlEffectList.SetItem( &lvItem );

			++nCount;			
		}
	}

	m_ctlCloud1Alpha.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].rgbColor.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].rgbColor.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].rgbColor.alpha	));
	m_ctlCloud2Alpha.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha	));
	m_ctlCircumstanceAlpha.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbCircumstance.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbCircumstance.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbCircumstance.alpha	));
	m_ctlSunAlpha.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbSun.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbSun.alpha	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbSun.alpha	));

	m_ctlCloud1Color.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].rgbColor.red		),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].rgbColor.green	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_FIRST		].rgbColor.blue		));
	m_ctlCloud2Color.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.red		),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.green	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.blue		));
	m_ctlCircumstanceColor.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbCircumstance.red		),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbCircumstance.green	),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbCircumstance.blue		));
	m_ctlSunColor.SetRGB(
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbSun.red		),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbSun.green		),
		( m_pAgcdSkySet->m_aSkySetting[ time ].rgbSun.blue		));

	return TRUE;
}

BOOL	CSkySettingDlg::GetData()
{
	ASSERT( NULL != m_pEventNatureDlg );

//	for( int i = 0 ; i < ASkySetting::TS_MAX ; i ++ )
//	{
//		// 스카이 설정 카피..
//		m_aSkySetting[ i ] = * m_pEventNatureDlg->m_pcsAgcmEventNature->GetSkySetting( i );
//	}

	// 리스트는 SkyTextureList.ini 에 따로 저장함.. 이것은 맵툴 데이타..

	if( m_ctlTextureList.LoadScript( "map\\skytexture.ini" ) )
	{
		TRACE( "스카이텍스쳐 로드성공.\n" );
	}
	else
	{
		TRACE( "스카이텍스쳐 로드실패.\n" );
	}

	// 텍스쳐 리스트를 이 Dialog 에 추가함...

	return TRUE;
}

BOOL	CSkySettingDlg::SetData()
{
	ASSERT( NULL != m_pEventNatureDlg->m_pcsAgcmEventNature );

	// 리스트 카피..
	m_pEventNatureDlg->m_pcsAgcmEventNature->RemoveAllTexture();

	AuNode< CTextureListStatic::TextureElement * >	*	pNode			= m_ctlTextureList.m_listTextureDraw.GetHeadNode();
	CTextureListStatic::TextureElement *				pTextureElement	;

	char	strFilename [ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

	while( pNode )
	{
		pTextureElement	=	pNode->GetData();

		_splitpath( pTextureElement->strFilename , drive, dir, fname, ext );
		wsprintf( strFilename , "%s%s" , fname , ext );
		m_pEventNatureDlg->m_pcsAgcmEventNature->AddTexture( strFilename , pTextureElement->nIndex );

		m_ctlTextureList.m_listTextureDraw.GetNext( pNode );
	}

	VERIFY( m_pEventNatureDlg->m_pcsAgcmEventNature->SaveTextureScriptFile() );
	
	return TRUE;
}


BOOL	CSkySettingDlg::RestoreData()
{
	ASSERT( NULL != m_pEventNatureDlg->m_pcsAgcmEventNature );

	for( int i = 0 ; i < ASkySetting::TS_MAX ; i ++ )
	{
		// 스카이 설정 카피..
		m_pAgcdSkySet->m_aSkySetting[ i ] = m_aSkySetting[ i ];
	}

	// 리스트 카피..
	m_pEventNatureDlg->m_pcsAgcmEventNature->RemoveAllTexture();

	AuNode< CTextureListStatic::TextureElement * >	*	pNode			= m_ctlTextureList.m_listTextureDraw.GetHeadNode();
	CTextureListStatic::TextureElement *				pTextureElement	;

	char	strFilename [ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

	while( pNode )
	{
		pTextureElement	=	pNode->GetData();

		_splitpath( pTextureElement->strFilename , drive, dir, fname, ext );
		wsprintf( strFilename , "%s%s" , fname , ext );
		m_pEventNatureDlg->m_pcsAgcmEventNature->AddTexture( strFilename , pTextureElement->nIndex );

		m_ctlTextureList.m_listTextureDraw.GetNext( pNode );
	}

	return TRUE;
}

void CSkySettingDlg::OnOK() 
{
	// 현재 데이타 백업..
	int	prevTime	= m_ctlTimeSelectStatic.GetTime();
	if( prevTime >= 0  && prevTime < ASkySetting::TS_MAX )
	{
		UpdateData( TRUE );

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyTop.red			= ( FLOAT ) m_ctlSkyTop			.m_nR / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyTop.green		= ( FLOAT ) m_ctlSkyTop			.m_nG / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyTop.blue			= ( FLOAT ) m_ctlSkyTop			.m_nB / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyBottom.red		= ( FLOAT ) m_ctlSkyBottom		.m_nR / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyBottom.green		= ( FLOAT ) m_ctlSkyBottom		.m_nG / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSkyBottom.blue		= ( FLOAT ) m_ctlSkyBottom		.m_nB / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbAmbient.red			= ( FLOAT ) m_ctlAmbient		.m_nR / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbAmbient.green		= ( FLOAT ) m_ctlAmbient		.m_nG / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbAmbient.blue		= ( FLOAT ) m_ctlAmbient		.m_nB / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbDirectional.red		= ( FLOAT ) m_ctlDirectional	.m_nR / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbDirectional.green	= ( FLOAT ) m_ctlDirectional	.m_nG / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbDirectional.blue	= ( FLOAT ) m_ctlDirectional	.m_nB / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbFog.red				= ( FLOAT ) m_ctlFog			.m_nR / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbFog.green			= ( FLOAT ) m_ctlFog			.m_nG / 255.0f;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbFog.blue			= ( FLOAT ) m_ctlFog			.m_nB / 255.0f;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.alpha	= m_ctlCloud1Alpha		.m_nR;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha	= m_ctlCloud2Alpha		.m_nR;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.alpha									= m_ctlCircumstanceAlpha.m_nR;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSun.alpha											= m_ctlSunAlpha			.m_nR;
	
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.red		= m_ctlCloud1Color		.m_nR;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.green	= m_ctlCloud1Color		.m_nG;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.blue		= m_ctlCloud1Color		.m_nB;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.red		= m_ctlCloud2Color		.m_nR;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.green	= m_ctlCloud2Color		.m_nG;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.blue		= m_ctlCloud2Color		.m_nB;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.red									= m_ctlCircumstanceColor.m_nR;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.green									= m_ctlCircumstanceColor.m_nG;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbCircumstance.blue									= m_ctlCircumstanceColor.m_nB;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSun.red												= m_ctlSunColor.m_nR;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSun.green											= m_ctlSunColor.m_nG;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].rgbSun.blue											= m_ctlSunColor.m_nB;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].nCircumstanceTexture	= m_ctlCircumstanceTexture.m_nIndex	;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].nSunTexture			= m_ctlSunTexture.m_nIndex			;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].nTexture	= m_ctlCloud1ImagePreview.m_nIndex;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].nTexture	= m_ctlCloud2ImagePreview.m_nIndex;
		
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_FIRST	].fSpeed = m_fCloudSpeed1;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].aCloud[ ASkySetting::CLOUD_SECOND	].fSpeed = m_fCloudSpeed2;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].fFogDistance		= m_fFogDistance	;
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].fFogFarClip		= m_fFogFarClip		;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].nEffectInterval	= m_nEffectInterval	;

		strncpy( m_pAgcdSkySet->m_aSkySetting[ prevTime ].strBackMusic , (LPCTSTR) m_strBackgroundMusic , EFF2_FILE_NAME_MAX );

		// m_pAgcdSkySet->m_aSkySetting[ prevTime ].nWeatherEffect1	= m_nWeatherEffect1	;
	
		m_pAgcdSkySet->m_aSkySetting[ prevTime ].nWeatherEffect1 =
			( m_bWeaLensFlare	? ASkySetting::USE_LENS_FLARE		: 0 ) |
			( m_bWeaMoon		? ASkySetting::SHOW_MOON			: 0 ) |
			( m_bWeaBreath		? ASkySetting::SHOW_BREATH			: 0 ) |
			( m_bWeaStar		? ASkySetting::SHOW_STAR			: 0 ) |
			( m_bWeaPLightOff	? ASkySetting::DISABLE_POINT_LIGHT	: 0 ) |
			( m_bWeaNEDisable	? ASkySetting::DISABLE_WEATHER		: 0 ) |
			( m_bWea_Reserved1	? ASkySetting::RESERVED_1			: 0 ) |
			( m_bWea_Reserved2	? ASkySetting::RESERVED_2			: 0 ) |
			( m_bWea_Reserved3	? ASkySetting::RESERVED_3			: 0 ) ;

		m_pAgcdSkySet->m_aSkySetting[ prevTime ].nWeatherEffect2	= -1;//m_nWeatherEffect2	;
		// 초기화..
		int i;
		for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		{
			m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectArray		[ i ]	= 0	;
			m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectRateArray	[ i ]	= 0	;
		}

		//LV_ITEM	lvItem	;
		CString	str		;
		for( i  = 0 ; i < m_ctlEffectList.GetItemCount() ; ++ i )
		{
			str = m_ctlEffectList.GetItemText( i , 0 );
			m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectArray		[ i ]	= atoi( ( LPCTSTR ) str );
			str = m_ctlEffectList.GetItemText( i , 1 );
			m_pAgcdSkySet->m_aSkySetting[ prevTime ].aEffectRateArray	[ i ]	= atoi( ( LPCTSTR ) str );
		}

	}


	// 텍스쳐 설정 저장..
	SetData();
	
	CDialog::OnOK();
}

void	CSkySettingDlg::ProcessLButtonUp( POINT point )
{
	// 다른 윈도우인지 검색..

	CWnd	*pTarget	;

	pTarget = CWnd::WindowFromPoint( point );
	UINT	id = ::GetWindowLong( pTarget->GetSafeHwnd() , GWL_ID );

	switch( id )
	{
	case IDC_IMAGE_PREVIEW			:
		// do nothing..
		break;
	case IDC_CLOUD_TEXTURE1	:
		{
			m_ctlCloud1ImagePreview.SetIndex(
				m_ctlTextureList.m_pSelectedElement->nIndex		);

			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
			INT32	nTime	;

			for( int i = 0 ; i < nCount ; i ++ )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( i );
				m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_FIRST	].nTexture	= m_ctlTextureList.m_pSelectedElement->nIndex;
			}

			if( nCount )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( 0 );
				SetTime( nTime	);
			}
		}
		break;
	case IDC_CLOUD_TEXTURE2		:
		{
			m_ctlCloud2ImagePreview.SetIndex(
				m_ctlTextureList.m_pSelectedElement->nIndex		);

			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
			INT32	nTime	;

			for( int i = 0 ; i < nCount ; i ++ )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( i );
				m_pAgcdSkySet->m_aSkySetting[ nTime ].aCloud[ ASkySetting::CLOUD_SECOND].nTexture	= m_ctlTextureList.m_pSelectedElement->nIndex;
			}

			if( nCount )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( 0 );
				SetTime( nTime	);
			}
		}
		break;

	case IDC_CIRCUMSTANCE_TEXTURE	:
		{
			m_ctlCircumstanceTexture.SetIndex(
				m_ctlTextureList.m_pSelectedElement->nIndex		);

			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
			INT32	nTime	;

			for( int i = 0 ; i < nCount ; i ++ )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( i );
				m_pAgcdSkySet->m_aSkySetting[ nTime ].nCircumstanceTexture = m_ctlTextureList.m_pSelectedElement->nIndex;
			}

			if( nCount )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( 0 );
				SetTime( nTime	);
			}
		}
		break;
	case IDC_SUN_TEXTURE	:
		{
			m_ctlSunTexture.SetIndex(
				m_ctlTextureList.m_pSelectedElement->nIndex		);

			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
			INT32	nTime	;

			for( int i = 0 ; i < nCount ; i ++ )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( i );
				m_pAgcdSkySet->m_aSkySetting[ nTime ].nSunTexture = m_ctlTextureList.m_pSelectedElement->nIndex;
			}

			if( nCount )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( 0 );

				SetTime( nTime	);
			}
		}
		break;
	}

	m_ctlTextureList.Invalidate( FALSE );
}

void	CSkySettingDlg::Export()
{
	// 냐냐냐;;;

	// 텍스쳐 디렉토리로..

	AuNode< CTextureListStatic::TextureElement * >	*	pNode			= m_ctlTextureList.m_listTextureDraw.GetHeadNode();
	//AuNode< CTextureListStatic::TextureElement * >	*	pNodeNext		;
	CTextureListStatic::TextureElement *				pTextureElement	;

	INT32			nIndex		= -1;

	char	strSource		[ 1024 ];
	char	strDestination	[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

	while( pNode )
	{
		pTextureElement	=	pNode->GetData();

		_splitpath( pTextureElement->strFilename , drive, dir, fname, ext );
		wsprintf( strSource			, "%s\\Map\\sky\\SKY%05d%s"	, m_pEventNatureDlg->m_strCurrentDirectory , pTextureElement->nIndex , ext );
		wsprintf( strDestination	, "%s\\Texture\\SKY%05d%s"	, m_pEventNatureDlg->m_strCurrentDirectory , pTextureElement->nIndex , ext );

		// 카피해넣는다..
		VERIFY( CopyFile( strSource , strDestination , FALSE ) );

		m_ctlTextureList.m_listTextureDraw.GetNext( pNode );
	}	
}

void CSkySettingDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	char	strFilename[ 1024 ];
	wsprintf( strFilename , "Map\\SkyTexture.ini" );
	VERIFY( m_ctlTextureList.SaveScript( strFilename ) );
	m_ctlTextureList.RemoveAll();
	
	if( m_bPrevTimerSetting )
		m_pEventNatureDlg->m_pcsAgpmEventNature->StopTimer();
	else
		m_pEventNatureDlg->m_pcsAgpmEventNature->StartTimer();

	m_pEventNatureDlg->m_pcsAgpmEventNature->SetSpeedRate( m_uPrevSpeedRate );
}

void CSkySettingDlg::OnSetfog() 
{
	CClipPlaneSetDlg	dlg;

	FLOAT	fFogDistanceBackup , fFogFarClipBackup ;

	INT32	time	= m_ctlTimeSelectStatic.GetTime();
	// 스카이 레인지로 사용함..
	dlg.m_fFogFarClip		= m_pAgcdSkySet->m_aSkySetting[ time ].fFogFarClip	;
	dlg.m_fFogDistance		= m_pAgcdSkySet->m_aSkySetting[ time ].fFogDistance	;
	
	fFogDistanceBackup		= dlg.m_fFogDistance	;
	fFogFarClipBackup		= dlg.m_fFogFarClip		;

	if( dlg.DoModal( this->m_pEventNatureDlg ) == IDOK )
	{
		// Fog 설정..

		INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
		INT32	nTime	;

		for( int i = 0 ; i < nCount ; i ++ )
		{
			nTime	= m_ctlTimeSelectStatic.GetTime( i );
			m_pAgcdSkySet->m_aSkySetting[ nTime ].fFogFarClip	=	dlg.m_fFogFarClip	;
			m_pAgcdSkySet->m_aSkySetting[ nTime ].fFogDistance	=	dlg.m_fFogDistance	;
		}

		UpdateData( TRUE );
		m_fFogDistance	=	dlg.m_fFogDistance	;
		m_fFogFarClip	=	dlg.m_fFogFarClip	;
		UpdateData( FALSE );
	}
	else
	{
		dlg.m_fFogFarClip	= fFogFarClipBackup		;
		dlg.m_fFogDistance	= fFogDistanceBackup	;
	}

	m_pEventNatureDlg->m_pcsAgpmEventNature->SetTime(
		ASkySetting::GetRealTime( m_ctlTimeSelectStatic.GetTime() )	,
		!m_pEventNatureDlg->m_pcsAgpmEventNature->GetMinute( m_pEventNatureDlg->m_pcsAgpmEventNature->GetGameTime() ) );

}

int	CSkySettingDlg::DoModal( int tid , AgcmEventNatureDlg * pEventNatureDlg )
{
	ASSERT( NULL != pEventNatureDlg );
	m_pEventNatureDlg							= pEventNatureDlg;
	m_ctlTextureList.m_pEventNatureDlg			= pEventNatureDlg;
	m_ctlCloud1ImagePreview.m_pEventNatureDlg	= pEventNatureDlg;
	m_ctlCloud2ImagePreview.m_pEventNatureDlg	= pEventNatureDlg;
	m_ctlCircumstanceTexture.m_pEventNatureDlg	= pEventNatureDlg;
	m_ctlSunTexture.m_pEventNatureDlg			= pEventNatureDlg;

	m_nTemplateID	= tid	;	// 템플릿 아이디..
	VERIFY( m_pAgpdSkySet	= pEventNatureDlg->m_pcsAgpmEventNature->GetSkySet				( tid			) );
	VERIFY( m_pAgcdSkySet	= pEventNatureDlg->m_pcsAgcmEventNature->GetSkySetClientData	( m_pAgpdSkySet ) );

	if( NULL == m_pAgpdSkySet || NULL == m_pAgcdSkySet)
	{
		MessageBox( "스카이데이타가 존재하지 않습니다." );
		return IDCANCEL;
	}

	for( int i = 0 ; i < ASkySetting::TS_MAX ; i ++ )
	{
		// 스카이 설정 카피..
		m_aSkySetting[ i ] = m_pAgcdSkySet->m_aSkySetting[ i ];
	}

	m_bUseColor		= ( m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_LIGHT	)	? TRUE : FALSE; 
	m_bUseFog		= ( m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_FOG		)	? TRUE : FALSE; 
	m_bUseCloud		= ( m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_CLOUD	)	? TRUE : FALSE; 
	m_bUseEffect	= ( m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_EFFECT	)	? TRUE : FALSE; 
	m_bUseBGM		= ( m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_MUSIC	)	? TRUE : FALSE; 
	m_bUseWeather	= ( m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_WEATHER	)	? TRUE : FALSE; 

	// 리스트는 SkyTextureList.ini 에 따로 저장함.. 이것은 맵툴 데이타..

	if( m_ctlTextureList.LoadScript( SKYSETTING_TEXTURE_FILE_NAME ) )
	{
		TRACE( "스카이텍스쳐 로드성공.\n" );
	}
	else
	{
		TRACE( "스카이텍스쳐 로드실패.\n" );
	}

	m_strTitle = m_pAgpdSkySet->m_strName;

	Export();

	return ( int ) CDialog::DoModal();
}


void CSkySettingDlg::OnCancel() 
{
	RestoreData();
	
	CDialog::OnCancel();
}

void CSkySettingDlg::OnEffectAdd() 
{
	// Effect 추가..

	UINT32	unEffectIndex	= this->m_pEventNatureDlg->p__OpenEffectList();
	if( unEffectIndex != 0 )
	{
		UINT32	nLength			= this->m_pEventNatureDlg->p__GetSoundLength( unEffectIndex );

		LV_ITEM	lvItem	;
		CString	strEffect;
		INT32	nOffset	= m_ctlEffectList.GetItemCount();
		INT32	nRate	= 0;

		strEffect.Format( "%d Effect" , unEffectIndex );

		if( nOffset == 0 ) 
		{
			// 최초의 이펙트는 100으로..
			nRate	= 100;
		}

		lvItem.mask		= LVIF_TEXT	;
		lvItem.iItem	= nOffset	;
		lvItem.iSubItem	= 0			;
		lvItem.pszText	= (LPSTR)(LPCTSTR)	strEffect;

		m_ctlEffectList.InsertItem( &lvItem );

		strEffect.Format( "%d%%" , nRate );

		lvItem.mask		= LVIF_TEXT	;
		lvItem.iItem	= nOffset	;
		lvItem.iSubItem	= 1			;
		lvItem.pszText	= (LPSTR)(LPCTSTR)	strEffect;

		m_ctlEffectList.SetItem( &lvItem );

		UpdateData( TRUE );
		m_nEffectInterval	= ( INT32 ) nLength;
		UpdateData( FALSE );


		// 변경 적용~
		INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
		INT32	nTime	;

		for( int i = 0 ; i < nCount ; i ++ )
		{
			nTime	= m_ctlTimeSelectStatic.GetTime( i );
			// 초기화..
			int j;
			for( j = 0 ; j < AGCMEVENTNATURE_MAX_EVENT ; ++ j )
			{
				m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectArray		[ j ]	= 0	;
				m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectRateArray	[ j ]	= 0	;
			}

			m_pAgcdSkySet->m_aSkySetting[ nTime ].nEffectInterval = m_nEffectInterval;

			//LV_ITEM	lvItem	;
			CString	str		;
			for( j  = 0 ; j < m_ctlEffectList.GetItemCount() ; ++ j )
			{
				str = m_ctlEffectList.GetItemText( j , 0 );
				m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectArray		[ j ]	= atoi( ( LPCTSTR ) str );
				str = m_ctlEffectList.GetItemText( j , 1 );
				m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectRateArray	[ j ]	= atoi( ( LPCTSTR ) str );
			}
		}

	}
}

void CSkySettingDlg::OnEffectDelete() 
{
	POSITION pos = m_ctlEffectList.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		TRACE0("No items were selected!\n");
		MessageBox( "선택됀 녀석이 없어염.." );
	}
	else
	{
		while (pos)
		{
			int nItem = m_ctlEffectList.GetNextSelectedItem(pos);
			// you could do your own processing on nItem here

			VERIFY( m_ctlEffectList.DeleteItem( nItem ) );

			// 한개만 체크함..

			// 변경 적용~
			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
			INT32	nTime	;

			for( int i = 0 ; i < nCount ; i ++ )
			{
				nTime	= m_ctlTimeSelectStatic.GetTime( i );
				// 초기화..
				int j;
				for( j = 0 ; j < AGCMEVENTNATURE_MAX_EVENT ; ++ j )
				{
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectArray		[ j ]	= 0	;
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectRateArray	[ j ]	= 0	;
				}

				//LV_ITEM	lvItem	;
				CString	str		;
				for( j  = 0 ; j < m_ctlEffectList.GetItemCount() ; ++ j )
				{
					str = m_ctlEffectList.GetItemText( j , 0 );
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectArray		[ j ]	= atoi( ( LPCTSTR ) str );
					str = m_ctlEffectList.GetItemText( j , 1 );
					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectRateArray	[ j ]	= atoi( ( LPCTSTR ) str );
				}
			}
			return;
		}
	}
	
}

void CSkySettingDlg::OnEffectEdit() 
{
	if( m_ctlEffectList.GetItemCount() == 0 )
		return;

	CString	str		;
	CString	strName	;
	INT32	nRate	;

	CEffectEditDlg	dlg;

	int i;

	for( i  = 0 ; i < m_ctlEffectList.GetItemCount() ; ++ i )
	{
		strName	= m_ctlEffectList.GetItemText( i , 0 );
		str		= m_ctlEffectList.GetItemText( i , 1 );
		nRate	= atoi( ( LPCTSTR ) str );
		
		dlg.m_ctlPortion.AddElement( strName , nRate );
	}

	if( dlg.DoModal() == IDOK )
	{
		for( int i = 0 ; i < dlg.m_ctlPortion.GetCount() ; ++ i )
		{
			if( dlg.m_ctlPortion.GetElement( i )->nRate == -1 )
			{
				str.Format( "%d 지속" , dlg.m_ctlPortion.GetElement( i )->nRate );
			}
			else
			{
				str.Format( "%d%%" , dlg.m_ctlPortion.GetElement( i )->nRate );
			}
			m_ctlEffectList.SetItemText( i , 1 , str );
		}
	}


//	// TODO: Add your control notification handler code here
//	POSITION pos = m_ctlEffectList.GetFirstSelectedItemPosition();
//	if (pos == NULL)
//	{
//		TRACE0("No items were selected!\n");
//		MessageBox( "선택됀 녀석이 없어염.." );
//	}
//	else
//	{
//		while (pos)
//		{
//			int nItem = m_ctlEffectList.GetNextSelectedItem(pos);
//			// you could do your own processing on nItem here
//
//			CEffectEditDlg	dlg;
//			dlg.m_strEffect	= m_ctlEffectList.GetItemText( nItem , 0 );
//			dlg.m_nRate		= atoi( m_ctlEffectList.GetItemText( nItem , 1 ) );
//
//			if( IDOK == dlg.DoModal() )
//			{
//				// 변경..
//				CString	str;
//				str.Format( "%d%%" , dlg.m_nRate );
//				m_ctlEffectList.SetItemText( nItem , 1 , str );
//			}
//
//			// 한개만 체크함..
//			// 변경 적용~
//			INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
//			INT32	nTime	;
//
//			for( int i = 0 ; i < nCount ; i ++ )
//			{
//				nTime	= m_ctlTimeSelectStatic.GetTime( i );
//				// 초기화..
//				int j;
//				for( j = 0 ; j < AGCMEVENTNATURE_MAX_EVENT ; ++ j )
//				{
//					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectArray		[ j ]	= 0	;
//					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectRateArray	[ j ]	= 0	;
//				}
//
//				//LV_ITEM	lvItem	;
//				CString	str		;
//				for( i  = 0 ; i < m_ctlEffectList.GetItemCount() ; ++ i )
//				{
//					str = m_ctlEffectList.GetItemText( j , 0 );
//					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectArray		[ j ]	= atoi( ( LPCTSTR ) str );
//					str = m_ctlEffectList.GetItemText( j , 1 );
//					m_pAgcdSkySet->m_aSkySetting[ nTime ].aEffectRateArray	[ j ]	= atoi( ( LPCTSTR ) str );
//				}
//			}
//
//			return;
//		}
//	}
	
}

void CSkySettingDlg::OnDblclkEffectList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEffectEdit();
	*pResult = 0;
}

void CSkySettingDlg::OnChangeDataFlag() 
{
	UpdateData( TRUE );

	// 컨트롤들을 정리한다.

	GetDlgItem(	IDC_HSV_SKYTOP			)->ShowWindow( m_bUseColor );
	GetDlgItem(	IDC_HSV_SKYBOTTOM		)->ShowWindow( m_bUseColor );
	GetDlgItem(	IDC_HSV_DIRECTIONAL		)->ShowWindow( m_bUseColor );
	GetDlgItem(	IDC_HSV_AMBIENT			)->ShowWindow( m_bUseColor );

	GetDlgItem(	IDC_CLOUD_TEXTURE1		)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CLOUD_TEXTURE2		)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CIRCUMSTANCE_TEXTURE)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CLOUD1_COLOR		)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CLOUD2_COLOR		)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CIRCUMSTANCE_COLOR	)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CLOUD1_ALPHA		)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CLOUD2_ALPHA		)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CIRCUMSTANCE_ALPHA	)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CLOUDSPEED1			)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_CLOUDSPEED2			)->ShowWindow( m_bUseCloud );
	GetDlgItem(	IDC_TEXTURELIST			)->ShowWindow( m_bUseCloud );

	GetDlgItem(	IDC_HSV_FOG				)->ShowWindow( m_bUseFog );
	GetDlgItem(	IDC_SETFOG				)->ShowWindow( m_bUseFog );

	GetDlgItem(	IDC_EFFECTINTERVAL		)->ShowWindow( m_bUseEffect );
	GetDlgItem(	IDC_AMBIENT_EFFECT_LIST	)->ShowWindow( m_bUseEffect );
	GetDlgItem(	IDC_EFFECT_ADD			)->ShowWindow( m_bUseEffect );
	GetDlgItem(	IDC_EFFECT_DELETE		)->ShowWindow( m_bUseEffect );
	GetDlgItem(	IDC_EFFECT_EDIT			)->ShowWindow( m_bUseEffect );

	GetDlgItem(	IDC_BACKGROUND_TITLE	)->ShowWindow( m_bUseBGM	);
	GetDlgItem(	IDC_BACKGROUNDMUSIC		)->ShowWindow( m_bUseBGM	);
	GetDlgItem(	IDC_BROWSE				)->ShowWindow( m_bUseBGM	);

	//GetDlgItem(	IDC_WEATHER_EFFECT1		)->ShowWindow( m_bUseWeather);
	//GetDlgItem(	IDC_WEATHER_EFFECT2		)->ShowWindow( m_bUseWeather);

	GetDlgItem(	IDC_WEA_LENSFLARE		)->ShowWindow( m_bUseWeather);
	GetDlgItem(	IDC_WEA_SHOWSTAR		)->ShowWindow( m_bUseWeather);
	GetDlgItem(	IDC_WEA_SHOWBREATH		)->ShowWindow( m_bUseWeather);
	GetDlgItem(	IDC_WEA_MOON			)->ShowWindow( m_bUseWeather);
	GetDlgItem(	IDC_WEA_PLIGHT_OFF		)->ShowWindow( m_bUseWeather);
	
	for( int i = 0 ; i < ASkySetting::TS_MAX ; i ++ )
	{
		// 스카이 설정 카피..
		// 이것은 모두가 공유한다.
		
		m_pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag	= ASkySetting::DA_NONE;

		if( m_bUseColor		) m_pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_LIGHT	;
		if( m_bUseFog		) m_pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_FOG	; 
		if( m_bUseCloud		) m_pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_CLOUD	;
		if( m_bUseEffect	) m_pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_EFFECT;
		if( m_bUseBGM		) m_pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_MUSIC	;
		if( m_bUseWeather	) m_pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_WEATHER;
	}

}

void CSkySettingDlg::OnBackgroundMusicBrowse() 
{
	UpdateData( TRUE );

	char BASED_CODE szFilter[] = 
		"죄다 (*.*)|*.*|"
		"MP3|*.mp3|"		
		"|";
	char dir[ 1024 ];
	GetCurrentDirectory( 1024 , dir );
	CFileDialog OpenPS( TRUE , "*.*" , NULL , OFN_HIDEREADONLY , szFilter , this );// PS 는 Packed Sprite 를 말함
	
	if (OpenPS.DoModal() == IDOK)
	{
		char filename[ 1024 ];
		strncpy( filename , (LPCTSTR) OpenPS.GetPathName() , 1024 );
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
		_splitpath( filename , drive, dir, fname, ext );

		wsprintf( dir , "%s%s" , drive, dir );

		//SetCurrentDirectory( dir );

		wsprintf( filename , "%s%s" , fname , ext );

		if( strlen( filename ) >= EFF2_FILE_NAME_MAX )
		{
			MessageBox( "파일 이름길이 초과!!!!!!!!!!!!" );
			return;
		}

		m_strBackgroundMusic = filename;
		UpdateData( FALSE );

		// 데이타 업데이트

		INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
		INT32	nTime	;

		for( int i = 0 ; i < nCount ; i ++ )
		{
			nTime	= m_ctlTimeSelectStatic.GetTime( i );
			strncpy( m_pAgcdSkySet->m_aSkySetting[ nTime ].strBackMusic , filename , EFF2_FILE_NAME_MAX );
		}
	}
	else
	{
		// 취소인 경우에는 다 지워버럼..
		INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
		INT32	nTime	;

		for( int i = 0 ; i < nCount ; i ++ )
		{
			nTime	= m_ctlTimeSelectStatic.GetTime( i );
			strncpy( m_pAgcdSkySet->m_aSkySetting[ nTime ].strBackMusic , "" , EFF2_FILE_NAME_MAX );
		}

		m_strBackgroundMusic.Empty();
		UpdateData( FALSE );
	}
}

void CSkySettingDlg::OnBnClickedSetWeather()
{
	UpdateData( TRUE );

	INT32	nCount	= m_ctlTimeSelectStatic.GetSelectCount();
	INT32	nTime	;

	for( int i = 0 ; i < nCount ; i ++ )
	{
		nTime	= m_ctlTimeSelectStatic.GetTime( i );
		//m_pAgcdSkySet->m_aSkySetting[ nTime ].nWeatherEffect1	= m_nWeatherEffect1;

		m_pAgcdSkySet->m_aSkySetting[ nTime ].nWeatherEffect1 =
			( m_bWeaLensFlare	? ASkySetting::USE_LENS_FLARE		: 0 ) |
			( m_bWeaMoon		? ASkySetting::SHOW_MOON			: 0 ) |
			( m_bWeaBreath		? ASkySetting::SHOW_BREATH			: 0 ) |
			( m_bWeaStar		? ASkySetting::SHOW_STAR			: 0 ) |
			( m_bWeaPLightOff	? ASkySetting::DISABLE_POINT_LIGHT	: 0 ) |
			( m_bWeaNEDisable	? ASkySetting::DISABLE_WEATHER		: 0 ) |
			( m_bWea_Reserved1	? ASkySetting::RESERVED_1			: 0 ) |
			( m_bWea_Reserved2	? ASkySetting::RESERVED_2			: 0 ) |
			( m_bWea_Reserved3	? ASkySetting::RESERVED_3			: 0 ) ;

		m_pAgcdSkySet->m_aSkySetting[ nTime ].nWeatherEffect2	= -1;
	}
}
