// WaterDlg.cpp : implementation file
//

#include "stdafx.h"

#include "WaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaterDlg dialog


CWaterDlg::CWaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaterDlg)
	m_iStatusID = -1;
	m_iWaterType = 0;		
	m_iSelBOP_L1 = 0;
	m_iSelBMode_L1 = 0;
	m_strWaterFile_L1 = _T("");
	m_iRed_L1 = 0;
	m_iGreen_L1 = 0;
	m_iBlue_L1 = 0;
	m_iAlpha_L1 = 0;
	m_fU_L1 = 0.0f;
	m_fV_L1 = 0.0f;
	m_iTileSize_L1 = 0;
	m_iSelBMode_L2 = 0;
	m_strWaterFile_L2 = _T("");
	m_fU_L2 = 0.0f;
	m_fV_L2 = 0.0f;
	m_strWaveFile = _T("");
	m_iWaveWidth = 0;
	m_iWaveHeight = 0;
	m_fWaveLifeTime = 0.0f;
	m_fWaveScaleX = 1.0f;
	m_fWaveScaleZ = 1.0f;
	m_iWaveRed = 0;
	m_iWaveGreen = 0;
	m_iWaveBlue = 0;
	m_iWaveMinNum = 0;
	m_iWaveMaxNum = 0;
	//}}AFX_DATA_INIT
	m_bDataChange = false;
}

void CWaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaterDlg)
	DDX_Text(pDX, IDC_WATER_EDIT_STATUSID, m_iStatusID);
	DDX_Radio(pDX, IDC_WATERTYPE_RADIO0, m_iWaterType);
	DDX_Radio(pDX, IDC_BELNDOP_RADIO0, m_iSelBOP_L1);
	DDX_Radio(pDX, IDC_BMODE_RADIO1, m_iSelBMode_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1FILE, m_strWaterFile_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1RED, m_iRed_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1GREEN, m_iGreen_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1BLUE, m_iBlue_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1ALPHA, m_iAlpha_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1U, m_fU_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1V, m_fV_L1);
	DDX_Text(pDX, IDC_WATER_EDIT_L1TSIZE, m_iTileSize_L1);
	DDX_Radio(pDX, IDC_BMODE_RADIO7, m_iSelBMode_L2);
	DDX_Text(pDX, IDC_WATER_EDIT_L2FILE, m_strWaterFile_L2);
	DDX_Text(pDX, IDC_WATER_EDIT_L2U, m_fU_L2);
	DDX_Text(pDX, IDC_WATER_EDIT_L2V, m_fV_L2);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVEFILE, m_strWaveFile);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVEWIDTH, m_iWaveWidth);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVEHEIGHT, m_iWaveHeight);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVELIFETIME, m_fWaveLifeTime);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVESCALEX, m_fWaveScaleX);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVESCALEZ, m_fWaveScaleZ);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVERED, m_iWaveRed);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVEGREEN, m_iWaveGreen);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVEBLUE, m_iWaveBlue);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVEMINNUM, m_iWaveMinNum);
	DDX_Text(pDX, IDC_WATER_EDIT_WAVEMAXNUM, m_iWaveMaxNum);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWaterDlg, CDialog)
	//{{AFX_MSG_MAP(CWaterDlg)
	ON_BN_CLICKED(IDC_WATERTYPE_RADIO0, OnWaterTypeRadio0)
	ON_BN_CLICKED(IDC_WATERTYPE_RADIO1, OnWaterTypeRadio1)
	ON_BN_CLICKED(IDC_WATER_BUTTON_L1FILE, OnButton_L1File)
	ON_BN_CLICKED(IDC_WATER_BUTTON_L1COLOR, OnButton_L1Color)
	ON_BN_CLICKED(IDC_WATER_BUTTON_L2FILE, OnButton_L2File)
	ON_BN_CLICKED(IDC_WATER_BUTTON_WAVE, OnButton_WaveFile)
	ON_BN_CLICKED(IDC_WATER_BUTTON_WAVECOLOR, OnButton_WaveColor)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1FILE, OnChangeEdit_L1WaterFile)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1RED, OnChangeEdit_L1Red)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1GREEN, OnChangeEdit_L1Green)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1BLUE, OnChangeEdit_L1Blue)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1ALPHA, OnChangeEdit_L1Alpha)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1U, OnChangeEdit_L1U)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1V, OnChangeEdit_L1V)
	ON_EN_CHANGE(IDC_WATER_EDIT_L1TSIZE, OnChangeEdit_L1TileSize)
	ON_EN_CHANGE(IDC_WATER_EDIT_L2FILE, OnChangeEdit_L2WaterFile)
	ON_EN_CHANGE(IDC_WATER_EDIT_L2U, OnChangeEdit_L2U)
	ON_EN_CHANGE(IDC_WATER_EDIT_L2V, OnChangeEdit_L2V)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVEFILE, OnChangeEdit_WaveFile)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVEWIDTH, OnChangeEdit_WaveWidth)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVEHEIGHT, OnChangeEdit_WaveHeight)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVELIFETIME, OnChangeEdit_WaveLifeTime)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVESCALEX, OnChangeEdit_WaveScaleX)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVESCALEZ, OnChangeEdit_WaveScaleZ)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVERED, OnChangeEdit_WaveRed)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVEGREEN, OnChangeEdit_WaveGreen)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVEBLUE, OnChangeEdit_WaveBlue)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVEMINNUM, OnChangeEdit_WaveMinNum)
	ON_EN_CHANGE(IDC_WATER_EDIT_WAVEMAXNUM, OnChangeEdit_WaveMaxNum)
	ON_BN_CLICKED(IDC_BMODE_RADIO1, OnBmodeRadio1)
	ON_BN_CLICKED(IDC_BMODE_RADIO2, OnBmodeRadio2)
	ON_BN_CLICKED(IDC_BMODE_RADIO3, OnBmodeRadio3)
	ON_BN_CLICKED(IDC_BMODE_RADIO4, OnBmodeRadio4)
	ON_BN_CLICKED(IDC_BMODE_RADIO5, OnBmodeRadio5)
	ON_BN_CLICKED(IDC_BMODE_RADIO6, OnBmodeRadio6)
	ON_BN_CLICKED(IDC_BMODE_RADIO7, OnBmodeRadio7)
	ON_BN_CLICKED(IDC_BMODE_RADIO8, OnBmodeRadio8)
	ON_BN_CLICKED(IDC_BMODE_RADIO9, OnBmodeRadio9)
	ON_BN_CLICKED(IDC_BMODE_RADIO10, OnBmodeRadio10)
	ON_BN_CLICKED(IDC_BMODE_RADIO11, OnBmodeRadio11)
	ON_BN_CLICKED(IDC_BELNDOP_RADIO0, OnBelndopRadio0)
	ON_BN_CLICKED(IDC_BELNDOP_RADIO1, OnBelndopRadio1)
	ON_BN_CLICKED(IDC_BELNDOP_RADIO2, OnBelndopRadio2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaterDlg message handlers

void CWaterDlg::OnButton_L1File() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE,"Image file",NULL,OFN_NOCHANGEDIR,"Texture Files(*.bmp,*.tif,*.png)|*.bmp;*.tif;*.tiff;*.png||");
	if(dlg.DoModal() == IDOK)
	{
		CString mystr;
		mystr = dlg.GetFileName();
		
		m_strWaterFile_L1 = mystr;
		UpdateData(FALSE);

		m_bDataChange = true;
	}
}

void CWaterDlg::OnButton_L2File() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE,"Image file",NULL,OFN_NOCHANGEDIR,"Texture Files(*.bmp,*.tif,*.png)|*.bmp;*.tif;*.tiff;*.png||");
	if(dlg.DoModal() == IDOK)
	{
		CString mystr;
		mystr = dlg.GetFileName();
		
		m_strWaterFile_L2 = mystr;
		UpdateData(FALSE);

		m_bDataChange = true;
	}
}

void CWaterDlg::OnButton_WaveFile() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE,"Image file",NULL,OFN_NOCHANGEDIR,"Texture Files(*.bmp,*.tif,*.png)|*.bmp;*.tif;*.tiff;*.png||");
	if(dlg.DoModal() == IDOK)
	{
		CString mystr;
		mystr = dlg.GetFileName();
		
		m_strWaveFile = mystr;
		UpdateData(FALSE);

		m_bDataChange = true;
	}
}

void CWaterDlg::OnButton_L1Color() 
{
	// TODO: Add your control notification handler code here
	CColorDialog	dlg;
	if(dlg.DoModal() == IDOK)
	{
		COLORREF	color;
		color = dlg.GetColor();

		m_iRed_L1 = color & 0x000000ff;
		m_iGreen_L1 = (color & 0x0000ff00) >> 8;
		m_iBlue_L1 = (color & 0x00ff0000) >> 16;

		UpdateData(FALSE);

		m_bDataChange = true;
	}
}

void CWaterDlg::OnButton_WaveColor() 
{
	// TODO: Add your control notification handler code here
	CColorDialog	dlg;
	if(dlg.DoModal() == IDOK)
	{
		COLORREF	color;
		color = dlg.GetColor();

		m_iWaveRed = color & 0x000000ff;
		m_iWaveGreen = (color & 0x0000ff00) >> 8;
		m_iWaveBlue = (color & 0x00ff0000) >> 16;

		UpdateData(FALSE);

		m_bDataChange = true;
	}
}

void CWaterDlg::OnOK() 
{
	// TODO: Add extra validation here
	// Validation Check
	
	CDialog::OnOK();
}

BOOL CWaterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaterDlg::OnChangeEdit_StatusID() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1WaterFile() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1Red() 
{
	UpdateData();	
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1Green() 
{
	UpdateData();	
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1Blue() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1Alpha() 
{
	UpdateData();	
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1U() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1V() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L1TileSize() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L2WaterFile() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L2U() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_L2V() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveFile() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveWidth() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveHeight() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveLifeTime() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveScaleX() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveScaleZ() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveRed() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveGreen() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveBlue() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveMinNum() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnChangeEdit_WaveMaxNum() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio1() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio2() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio3() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio4() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio5() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio6() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio7() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio8() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio9() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio10() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBmodeRadio11() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBelndopRadio0() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBelndopRadio1() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnBelndopRadio2() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnWaterTypeRadio0() 
{
	UpdateData();
	m_bDataChange = true;
}

void CWaterDlg::OnWaterTypeRadio1() 
{
	UpdateData();
	m_bDataChange = true;
}

