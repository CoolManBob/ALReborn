// NatureEffectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MyEngine.h"
#include "AgpmEventNature.h"
#include "AgcmNatureEffect.h"
#include "NatureEffectDlg.h"

extern	AgcmNatureEffect			*g_pcsAgcmNatureEffect;

// TemplateSelectDlg.h 에 똑같이 정의돼어있다 고칠라면 같이 고쳐야함
#define	TEMPLATE_LIST_FORMAT	"%03d , %s"

// NatureEffectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(NatureEffectDlg, CDialog)
NatureEffectDlg::NatureEffectDlg( INT32 nEffectID , CWnd* pParent /*=NULL*/)
	: CDialog(NatureEffectDlg::IDD, pParent)
	, m_ID(0)
	, m_strTextureFile(_T(""))
	, m_fSizeMin(0)
	, m_fSizeMax(0)
	, m_fSpeedMin(0)
	, m_fSpeedMax(0)
	, m_fVibMin(0)
	, m_fVibMax(0)
	, m_fDensityMin(0)
	, m_fDensityMax(0)
	, m_iRedMin(0)
	, m_iRedMax(0)
	, m_iGreenMin(0)
	, m_iGreenMax(0)
	, m_iBlueMin(0)
	, m_iBlueMax(0)
	, m_iAlphaMin(0)
	, m_iAlphaMax(0)
	, m_iSelType(0)
	, m_nSkySet( 0 )
{
	 m_nEffectID = nEffectID;
}

NatureEffectDlg::~NatureEffectDlg()
{
}

void NatureEffectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NEFFECT_EDIT_ID, m_ID);
	DDX_Text(pDX, IDC_NEFFECT_TEXTURE, m_strTextureFile);
	DDX_Text(pDX, IDC_NEFFECT_SIZE_MIN, m_fSizeMin);
	//DDV_MinMaxFloat(pDX, m_fSizeMin, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_SIZE_MAX, m_fSizeMax);
	//DDV_MinMaxFloat(pDX, m_fSizeMax, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_SPEED_MIN, m_fSpeedMin);
	//DDV_MinMaxFloat(pDX, m_fSpeedMin, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_SPEED_MAX, m_fSpeedMax);
	//DDV_MinMaxFloat(pDX, m_fSpeedMax, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_VIB_MIN, m_fVibMin);
	//DDV_MinMaxFloat(pDX, m_fVibMin, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_VIB_MAX, m_fVibMax);
	//DDV_MinMaxFloat(pDX, m_fVibMax, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_DENSITY_MIN, m_fDensityMin);
	//DDV_MinMaxFloat(pDX, m_fDensityMin, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_DENSITY_MAX, m_fDensityMax);
	//DDV_MinMaxFloat(pDX, m_fDensityMax, 0.0, 1.0);
	DDX_Text(pDX, IDC_NEFFECT_RED_MIN, m_iRedMin);
	DDV_MinMaxUInt(pDX, m_iRedMin, 0, 255);
	DDX_Text(pDX, IDC_NEFFECT_RED_MAX, m_iRedMax);
	DDV_MinMaxUInt(pDX, m_iRedMax, 0, 255);
	DDX_Text(pDX, IDC_NEFFECT_GREEN_MIN, m_iGreenMin);
	DDV_MinMaxUInt(pDX, m_iGreenMin, 0, 255);
	DDX_Text(pDX, IDC_NEFFECT_GREEN_MAX, m_iGreenMax);
	DDV_MinMaxUInt(pDX, m_iGreenMax, 0, 255);
	DDX_Text(pDX, IDC_NEFFECT_BLUE_MIN, m_iBlueMin);
	DDV_MinMaxUInt(pDX, m_iBlueMin, 0, 255);
	DDX_Text(pDX, IDC_NEFFECT_BLUE_MAX, m_iBlueMax);
	DDV_MinMaxUInt(pDX, m_iBlueMax, 0, 255);
	DDX_Text(pDX, IDC_NEFFECT_ALPHA_MIN, m_iAlphaMin);
	DDV_MinMaxUInt(pDX, m_iAlphaMin, 0, 255);
	DDX_Text(pDX, IDC_NEFFECT_ALPHA_MAX, m_iAlphaMax);
	DDV_MinMaxUInt(pDX, m_iAlphaMax, 0, 255);
	DDX_Control(pDX, IDC_NEFFECT_COMBO, m_ctrlComboBox);
	DDX_CBIndex(pDX, IDC_NEFFECT_COMBO, m_iSelType);
	DDX_Control(pDX, IDC_SKYSETCOMBO, m_ctlSkySetCombo);
}


BEGIN_MESSAGE_MAP(NatureEffectDlg, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_NEFFECT_TEXTURE_BUTTON, OnBnClickedNeffectTextureButton)
	ON_EN_CHANGE(IDC_NEFFECT_SIZE_MIN, OnEnChangeNeffectSizeMin)
	ON_EN_CHANGE(IDC_NEFFECT_SIZE_MAX, OnEnChangeNeffectSizeMax)
	ON_EN_CHANGE(IDC_NEFFECT_SPEED_MIN, OnEnChangeNeffectSpeedMin)
	ON_EN_CHANGE(IDC_NEFFECT_SPEED_MAX, OnEnChangeNeffectSpeedMax)
	ON_EN_CHANGE(IDC_NEFFECT_VIB_MIN, OnEnChangeNeffectVibMin)
	ON_EN_CHANGE(IDC_NEFFECT_VIB_MAX, OnEnChangeNeffectVibMax)
	ON_EN_CHANGE(IDC_NEFFECT_DENSITY_MIN, OnEnChangeNeffectDensityMin)
	ON_EN_CHANGE(IDC_NEFFECT_DENSITY_MAX, OnEnChangeNeffectDensityMax)
	ON_EN_CHANGE(IDC_NEFFECT_RED_MIN, OnEnChangeNeffectRedMin)
	ON_EN_CHANGE(IDC_NEFFECT_RED_MAX, OnEnChangeNeffectRedMax)
	ON_EN_CHANGE(IDC_NEFFECT_GREEN_MIN, OnEnChangeNeffectGreenMin)
	ON_EN_CHANGE(IDC_NEFFECT_GREEN_MAX, OnEnChangeNeffectGreenMax)
	ON_EN_CHANGE(IDC_NEFFECT_BLUE_MIN, OnEnChangeNeffectBlueMin)
	ON_EN_CHANGE(IDC_NEFFECT_BLUE_MAX, OnEnChangeNeffectBlueMax)
	ON_EN_CHANGE(IDC_NEFFECT_ALPHA_MIN, OnEnChangeNeffectAlphaMin)
	ON_EN_CHANGE(IDC_NEFFECT_ALPHA_MAX, OnEnChangeNeffectAlphaMax)
	ON_BN_CLICKED(ID_NEFFECT_OK, OnBnClickedNeffectOk)
	ON_BN_CLICKED(ID_NEFFECT_CANCEL, OnBnClickedNeffectCancel)
	ON_CBN_SELCHANGE(IDC_NEFFECT_COMBO, OnCbnSelchangeNeffectCombo)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
END_MESSAGE_MAP()


// NatureEffectDlg 메시지 처리기입니다.

int NatureEffectDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
		
	return 0;
}

void NatureEffectDlg::OnBnClickedNeffectTextureButton()
{
	// file dialog 열자
	CFileDialog dlg(TRUE,"Image file",NULL,OFN_NOCHANGEDIR,"Texture Files(*.bmp,*.tif,*.png)|*.bmp;*.tif;*.tiff;*.png||");
	if(dlg.DoModal() == IDOK)
	{
		CString mystr;
		mystr = dlg.GetFileName();
		
		m_strTextureFile = mystr;
		UpdateData(FALSE);

		m_bDataChange = TRUE;
	}
}

void NatureEffectDlg::OnEnChangeNeffectSizeMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectSizeMax()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectSpeedMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectSpeedMax()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectVibMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectVibMax()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectDensityMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectDensityMax()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectRedMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectRedMax()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectGreenMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectGreenMax()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectBlueMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectBlueMax()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectAlphaMin()
{
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnEnChangeNeffectAlphaMax()
{
	m_bDataChange = TRUE;
}

BOOL NatureEffectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString	str;
	str.Format("Snow\n");
	m_ctrlComboBox.InsertString(0,str);
	str.Format("Rain\n");
	m_ctrlComboBox.InsertString(1,str);
	str.Format("HeavyRainWithThunder\n");
	m_ctrlComboBox.InsertString(2,str);
	str.Format("HeavyRainWithFog\n");
	m_ctrlComboBox.InsertString(3,str);
	str.Format("SandStorm\n");
	m_ctrlComboBox.InsertString(4,str);

	m_ctrlComboBox.SetCurSel(m_iSelType);

	{
		AuList< AgpdSkySet * >	* pList = g_pcsAgpmEventNature->GetSkySetList();
		AuNode< AgpdSkySet * >	* pNode	= pList->GetHeadNode();
		AgpdSkySet				* pSkySet	;

		CString					str;
		int						pos;
		while( pNode )
		{
			pSkySet	= pNode->GetData();
			str.Format( TEMPLATE_LIST_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );
			pos = m_ctlSkySetCombo.AddString( str );
			if( pSkySet->m_nIndex == m_nSkySet )
			{
				m_ctlSkySetCombo.SetCurSel( pos );
			}
			pList->GetNext( pNode );
		}
	}

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void NatureEffectDlg::OnBnClickedNeffectOk()
{
	UpdateData( TRUE );

	CString str;
	m_ctlSkySetCombo.GetLBText( m_ctlSkySetCombo.GetCurSel() , str );
	INT32 nPrevSkySet = m_nSkySet;
	m_nSkySet = atoi( ( LPCTSTR ) str );
	if( m_nSkySet != nPrevSkySet )	m_bDataChange = TRUE;

	CDialog::OnOK();
}

void NatureEffectDlg::OnBnClickedNeffectCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}

void NatureEffectDlg::OnCbnSelchangeNeffectCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bDataChange = TRUE;
}

void NatureEffectDlg::OnApply()
{
	// 텍스쳐는 적용 안돼염~
	UpdateData( TRUE );

	CString str;
	m_ctlSkySetCombo.GetLBText( m_ctlSkySetCombo.GetCurSel() , str );
	INT32 nPrevSkySet = m_nSkySet;
	m_nSkySet = atoi( ( LPCTSTR ) str );
	if( m_nSkySet != nPrevSkySet )	m_bDataChange = TRUE;

	// 바꿔서 네이쳐를 플레이 시킨다.
	NatureEffectInfo*	pInfo = &g_pcsAgcmNatureEffect->m_vectorstNEInfo[ m_nEffectID ];

	// 설정값 변경..
	pInfo->fSizeMin		= m_fSizeMin	;
	pInfo->fSizeMax		= m_fSizeMax	;
	pInfo->fSpeedMin	= m_fSpeedMin	;
	pInfo->fSpeedMax	= m_fSpeedMax	;
	pInfo->fSwingMin	= m_fVibMin		;
	pInfo->fSwingMax	= m_fVibMax		;
	pInfo->fDensityMin	= m_fDensityMin	;
	pInfo->fDensityMax	= m_fDensityMax	;
	pInfo->cRedMin		= m_iRedMin		;
	pInfo->cRedMax		= m_iRedMax		;
	pInfo->cGreenMin	= m_iGreenMin	;
	pInfo->cGreenMax	= m_iGreenMax	;
	pInfo->cBlueMin		= m_iBlueMin	;
	pInfo->cBlueMax		= m_iBlueMax	;
	pInfo->cAlphaMin	= m_iAlphaMin	;
	pInfo->cAlphaMax	= m_iAlphaMax	;
	pInfo->nSkySet		= m_nSkySet		;

	g_pcsAgcmNatureEffect->StopNatureEffect( TRUE );
	g_pcsAgcmNatureEffect->StartNatureEffect( m_nEffectID );	
	g_pcsAgcmEventNature->ApplySkySetting();
}
