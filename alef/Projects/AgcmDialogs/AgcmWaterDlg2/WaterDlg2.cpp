// WaterDlg2.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmWaterDlg2.h"
#include "WaterDlg2.h"
#include ".\waterdlg2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WaterDlg2 dialog


WaterDlg2::WaterDlg2(CWnd* pParent /*=NULL*/)
	: CDialog(WaterDlg2::IDD, pParent)
	//@{ Jaewon 20050706
	// ;)
	, m_fBaseR(0)
	, m_fBaseG(0)
	, m_fBaseB(0)
	, m_iShader14ID(FALSE)
	, m_iShader11ID(FALSE)
	//@} Jaewon
{
	//{{AFX_DATA_INIT(WaterDlg2)
	m_fAlphaMax = 0.0f;
	m_fAlphaMin = 0.0f;
	m_strBump = _T("");
	m_fBlue = 0.0f;
	m_fGreen = 0.0f;
	m_fRed = 0.0f;
	m_fDirX0 = 0.0f;
	m_fDirX1 = 0.0f;
	m_fDirX2 = 0.0f;
	m_fDirX3 = 0.0f;
	m_fDirY0 = 0.0f;
	m_fDirY1 = 0.0f;
	m_fDirY2 = 0.0f;
	m_fDirY3 = 0.0f;
	m_strFresnel = _T("");
	m_fHeight0 = 0.0f;
	m_fHeight1 = 0.0f;
	m_fHeight2 = 0.0f;
	m_fHeight3 = 0.0f;
	m_iHwID = 0;
	m_fReflectionWeight = 0.0f;
	m_fSpeed0 = 0.0f;
	m_fSpeed1 = 0.0f;
	m_fSpeed2 = 0.0f;
	m_fSpeed3 = 0.0f;
	m_fTexX0 = 0.0f;
	m_fTexX1 = 0.0f;
	m_fTexY0 = 0.0f;
	m_fTexY1 = 0.0f;
	m_iVertexPerTile = 0;
	m_fAlphaDecHeight = 0.0f;
	m_fWaveDecHeight = 0.0f;
	m_fWaveMax = 0.0f;
	m_fWaveMin = 0.0f;
	//}}AFX_DATA_INIT
	m_bDataChange = false;
}


void WaterDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WaterDlg2)
	DDX_Text(pDX, IDC_HWATER_EDIT_ALPHA_MAX, m_fAlphaMax);
	DDX_Text(pDX, IDC_HWATER_EDIT_ALPHA_MIN, m_fAlphaMin);
	DDX_Text(pDX, IDC_HWATER_EDIT_BUMPMAP, m_strBump);
	DDX_Text(pDX, IDC_HWATER_EDIT_COLOR_BLUE, m_fBlue);
	DDX_Text(pDX, IDC_HWATER_EDIT_COLOR_GREEN, m_fGreen);
	DDX_Text(pDX, IDC_HWATER_EDIT_COLOR_RED, m_fRed);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRX0, m_fDirX0);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRX1, m_fDirX1);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRX2, m_fDirX2);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRX3, m_fDirX3);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRY0, m_fDirY0);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRY1, m_fDirY1);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRY2, m_fDirY2);
	DDX_Text(pDX, IDC_HWATER_EDIT_DIRY3, m_fDirY3);
	DDX_Text(pDX, IDC_HWATER_EDIT_FRESNELMAP, m_strFresnel);
	DDX_Text(pDX, IDC_HWATER_EDIT_HEIGHT0, m_fHeight0);
	DDX_Text(pDX, IDC_HWATER_EDIT_HEIGHT1, m_fHeight1);
	DDX_Text(pDX, IDC_HWATER_EDIT_HEIGHT2, m_fHeight2);
	DDX_Text(pDX, IDC_HWATER_EDIT_HEIGHT3, m_fHeight3);
	DDX_Text(pDX, IDC_HWATER_EDIT_ID, m_iHwID);
	DDX_Text(pDX, IDC_HWATER_EDIT_REFLECTION_WEIGHT, m_fReflectionWeight);
	DDX_Text(pDX, IDC_HWATER_EDIT_SPEED0, m_fSpeed0);
	DDX_Text(pDX, IDC_HWATER_EDIT_SPEED1, m_fSpeed1);
	DDX_Text(pDX, IDC_HWATER_EDIT_SPEED2, m_fSpeed2);
	DDX_Text(pDX, IDC_HWATER_EDIT_SPEED3, m_fSpeed3);
	DDX_Text(pDX, IDC_HWATER_EDIT_TEX_X0, m_fTexX0);
	DDX_Text(pDX, IDC_HWATER_EDIT_TEX_X1, m_fTexX1);
	DDX_Text(pDX, IDC_HWATER_EDIT_TEX_Y0, m_fTexY0);
	DDX_Text(pDX, IDC_HWATER_EDIT_TEX_Y1, m_fTexY1);
	DDX_Text(pDX, IDC_HWATER_EDIT_VERTEXPERTILE, m_iVertexPerTile);
	DDX_Text(pDX, IDC_HWATER_EDIT_ALPHA_DEC_HEIGHT, m_fAlphaDecHeight);
	DDX_Text(pDX, IDC_HWATER_EDIT_WAVE_DEC_HEIGHT, m_fWaveDecHeight);
	DDX_Text(pDX, IDC_HWATER_EDIT_WAVE_MAX, m_fWaveMax);
	DDX_Text(pDX, IDC_HWATER_EDIT_WAVE_MIN, m_fWaveMin);
	//}}AFX_DATA_MAP
	//@{ Jaewon 20050706
	// ;)
	DDX_Text(pDX, IDC_BASE_COLOR_R, m_fBaseR);
	DDX_Text(pDX, IDC_BASE_COLOR_G, m_fBaseG);
	DDX_Text(pDX, IDC_BASE_COLOR_B, m_fBaseB);
	DDX_Radio(pDX, IDC_RADIO1, m_iShader14ID);
	DDX_Radio(pDX, IDC_RADIO3, m_iShader11ID);
	//@} Jaewon
}


BEGIN_MESSAGE_MAP(WaterDlg2, CDialog)
	//{{AFX_MSG_MAP(WaterDlg2)
	ON_EN_CHANGE(IDC_HWATER_EDIT_COLOR_BLUE, OnChangeHwaterEditColorBlue)
	ON_EN_CHANGE(IDC_HWATER_EDIT_COLOR_GREEN, OnChangeHwaterEditColorGreen)
	ON_EN_CHANGE(IDC_HWATER_EDIT_COLOR_RED, OnChangeHwaterEditColorRed)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRX0, OnChangeHwaterEditDirx0)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRX1, OnChangeHwaterEditDirx1)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRX2, OnChangeHwaterEditDirx2)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRX3, OnChangeHwaterEditDirx3)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRY0, OnChangeHwaterEditDiry0)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRY1, OnChangeHwaterEditDiry1)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRY2, OnChangeHwaterEditDiry2)
	ON_EN_CHANGE(IDC_HWATER_EDIT_DIRY3, OnChangeHwaterEditDiry3)
	ON_EN_CHANGE(IDC_HWATER_EDIT_HEIGHT0, OnChangeHwaterEditHeight0)
	ON_EN_CHANGE(IDC_HWATER_EDIT_HEIGHT1, OnChangeHwaterEditHeight1)
	ON_EN_CHANGE(IDC_HWATER_EDIT_HEIGHT2, OnChangeHwaterEditHeight2)
	ON_EN_CHANGE(IDC_HWATER_EDIT_HEIGHT3, OnChangeHwaterEditHeight3)
	ON_EN_CHANGE(IDC_HWATER_EDIT_ID, OnChangeHwaterEditId)
	ON_EN_CHANGE(IDC_HWATER_EDIT_SPEED0, OnChangeHwaterEditSpeed0)
	ON_EN_CHANGE(IDC_HWATER_EDIT_SPEED1, OnChangeHwaterEditSpeed1)
	ON_EN_CHANGE(IDC_HWATER_EDIT_SPEED2, OnChangeHwaterEditSpeed2)
	ON_EN_CHANGE(IDC_HWATER_EDIT_SPEED3, OnChangeHwaterEditSpeed3)
	ON_EN_CHANGE(IDC_HWATER_EDIT_TEX_X0, OnChangeHwaterEditTexX0)
	ON_EN_CHANGE(IDC_HWATER_EDIT_TEX_X1, OnChangeHwaterEditTexX1)
	ON_EN_CHANGE(IDC_HWATER_EDIT_TEX_Y0, OnChangeHwaterEditTexY0)
	ON_EN_CHANGE(IDC_HWATER_EDIT_TEX_Y1, OnChangeHwaterEditTexY1)
	ON_BN_CLICKED(IDC_HWATER_BUTTON_BUMP, OnHwaterButtonBump)
	ON_BN_CLICKED(IDC_HWATER_BUTTON_FRESNEL, OnHwaterButtonFresnel)
	ON_EN_CHANGE(IDC_HWATER_EDIT_REFLECTION_WEIGHT, OnChangeHwaterEditReflectionWeight)
	ON_EN_CHANGE(IDC_HWATER_EDIT_WAVE_DEC_HEIGHT, OnChangeHwaterEditWaveDecHeight)
	ON_EN_CHANGE(IDC_HWATER_EDIT_WAVE_MAX, OnChangeHwaterEditWaveMax)
	ON_EN_CHANGE(IDC_HWATER_EDIT_WAVE_MIN, OnChangeHwaterEditWaveMin)
	ON_EN_CHANGE(IDC_HWATER_EDIT_ALPHA_DEC_HEIGHT, OnChangeHwaterEditAlphaDecHeight)
	ON_EN_CHANGE(IDC_HWATER_EDIT_ALPHA_MAX, OnChangeHwaterEditAlphaMax)
	ON_EN_CHANGE(IDC_HWATER_EDIT_ALPHA_MIN, OnChangeHwaterEditAlphaMin)
	ON_EN_CHANGE(IDC_HWATER_EDIT_VERTEXPERTILE, OnChangeHwaterEditVertexpertile)
	//}}AFX_MSG_MAP
	//@{ Jaewon 20050706
	// ;)
	ON_BN_CLICKED(IDC_SPECULAR_COLOR, OnBnClickedSpecularColor)
	ON_BN_CLICKED(IDC_BASE_COLOR, OnBnClickedBaseColor)
	ON_EN_CHANGE(IDC_BASE_COLOR_R, OnEnChangeBaseColorR)
	ON_EN_CHANGE(IDC_BASE_COLOR_G, OnEnChangeBaseColorG)
	ON_EN_CHANGE(IDC_BASE_COLOR_B, OnEnChangeBaseColorB)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedRadio4)
	//@} Jaewon
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WaterDlg2 message handlers

void WaterDlg2::OnChangeHwaterEditColorBlue() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditColorGreen() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditColorRed() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDirx0() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDirx1() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDirx2() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDirx3() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDiry0() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDiry1() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDiry2() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditDiry3() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditHeight0() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditHeight1() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditHeight2() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditHeight3() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditId() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditSpeed0() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditSpeed1() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditSpeed2() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditSpeed3() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditTexX0() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditTexX1() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditTexY0() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditTexY1() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnHwaterButtonBump() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE,"Image file",NULL,OFN_NOCHANGEDIR,"Texture Files(*.bmp,*.tif,*.png)|*.bmp;*.tif;*.tiff;*.png||");
	if(dlg.DoModal() == IDOK)
	{
		CString mystr;
		mystr = dlg.GetFileName();
		
		m_strBump = mystr;
		UpdateData(FALSE);

		m_bDataChange = true;
	}
}

void WaterDlg2::OnHwaterButtonFresnel() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE,"Image file",NULL,OFN_NOCHANGEDIR,"Texture Files(*.bmp,*.tif,*.png)|*.bmp;*.tif;*.tiff;*.png||");
	if(dlg.DoModal() == IDOK)
	{
		CString mystr;
		mystr = dlg.GetFileName();
		
		m_strFresnel = mystr;
		UpdateData(FALSE);

		m_bDataChange = true;
	}
}

void WaterDlg2::OnChangeHwaterEditReflectionWeight() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditWaveDecHeight() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditWaveMax() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditWaveMin() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditAlphaDecHeight() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditAlphaMax() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditAlphaMin() 
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnChangeHwaterEditVertexpertile() 
{
	UpdateData();
	m_bDataChange = true;
}

//@{ Jaewon 20050706
// ;)
void WaterDlg2::OnBnClickedSpecularColor()
{
	CColorDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		COLORREF color;
		color = dlg.GetColor();

		m_fRed = float(color & 0x000000ff)/255.0f;
		m_fGreen = float((color & 0x0000ff00) >> 8)/255.0f;
		m_fBlue = float((color & 0x00ff0000) >> 16)/255.0f;

		UpdateData(FALSE);
		m_bDataChange = true;
	}
}

void WaterDlg2::OnBnClickedBaseColor()
{
	CColorDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		COLORREF color;
		color = dlg.GetColor();

		m_fBaseR = float(color & 0x000000ff)/255.0f;
		m_fBaseG = float((color & 0x0000ff00) >> 8)/255.0f;
		m_fBaseB = float((color & 0x00ff0000) >> 16)/255.0f;

		UpdateData(FALSE);
		m_bDataChange = true;
	}
}

void WaterDlg2::OnEnChangeBaseColorR()
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnEnChangeBaseColorG()
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnEnChangeBaseColorB()
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnBnClickedRadio1()
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnBnClickedRadio2()
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnBnClickedRadio3()
{
	UpdateData();
	m_bDataChange = true;
}

void WaterDlg2::OnBnClickedRadio4()
{
	UpdateData();
	m_bDataChange = true;
}
//@} Jaewon
