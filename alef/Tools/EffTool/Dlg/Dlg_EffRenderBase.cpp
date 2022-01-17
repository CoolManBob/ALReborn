#include "stdafx.h"
#include "../EffTool.h"
#include "Dlg_EffRenderBase.h"

#include "AgcuEffPath.h"
#include "AgcuEffUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlg_EffRenderBase::CDlg_EffRenderBase(CWnd* pParent /*=NULL*/) : CDialog(CDlg_EffRenderBase::IDD, pParent) , m_nBlendType(4)
{
	m_fAngleX	= 0.0f;
	m_fAngleY	= 0.0f;
	m_fAngleZ	= 0.0f;
	m_szMask	= _T("");
	m_fPoxX		= 0.0f;
	m_fPoxY		= 0.0f;
	m_fPoxZ		= 0.0f;
	m_szTex		= _T("");
}

void CDlg_EffRenderBase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_EffRenderBase)
	DDX_Control(pDX, IDC_COMBO_BLEND, m_ctrlCombo_Blend);
	DDX_Text(pDX, IDC_EDIT_ANGLE_Z, m_fAngleZ);
	DDX_Text(pDX, IDC_EDIT_ANGLE_X, m_fAngleX);
	DDX_Text(pDX, IDC_EDIT_ANGLE_Y, m_fAngleY);
	DDX_Text(pDX, IDC_EDIT_MASK, m_szMask);
	DDV_MaxChars(pDX, m_szMask, 63);
	DDX_Text(pDX, IDC_EDIT_POS_X, m_fPoxX);
	DDX_Text(pDX, IDC_EDIT_POS_Y, m_fPoxY);
	DDX_Text(pDX, IDC_EDIT_POS_Z, m_fPoxZ);
	DDX_Text(pDX, IDC_EDIT_TEX, m_szTex);
	DDV_MaxChars(pDX, m_szTex, 63);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlg_EffRenderBase, CDialog)
	//{{AFX_MSG_MAP(CDlg_EffRenderBase)
	ON_BN_CLICKED(IDC_BUTTON_TEX, OnButtonTex)
	ON_BN_CLICKED(IDC_BUTTON_MASK, OnButtonMask)
	ON_CBN_SELCHANGE(IDC_COMBO_BLEND, OnSelchangeComboBlend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlg_EffRenderBase::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrlCombo_Blend.SetCurSel(m_nBlendType);
	UpdateData(FALSE);
	
	return TRUE;
}

void CDlg_EffRenderBase::OnButtonTex() 
{
	// TODO: Add your control notification handler code here
	
	LPCTSTR	TEXFILTER	= "(*.tif)|*.tif|(*.bmp)|*.bmp|(*.png)|*.png|(*.dds)|*dds|all (*.tif;*.bmp;*.png;*.dds)|*.tif; *.bmp; *.png; *.dds|";
	
	AuAutoSetDirectory	restoreCurrentDirectory;
	CString	strpath(restoreCurrentDirectory.bGetSavedPath());

	strpath += AgcuEffPath::bGetPath_Tex();
	strpath += _T("*.tif");
	CFileDialog dlgFile(TRUE,NULL,strpath,OFN_READONLY,TEXFILTER,this);

	if( IDOK == dlgFile.DoModal() )
	{
		ToWnd( dlgFile.GetFileName() );
		m_szTex = dlgFile.GetFileName();
		UpdateData(FALSE);
	}
}

void CDlg_EffRenderBase::OnButtonMask() 
{
	// TODO: Add your control notification handler code here
	
	LPCTSTR	TEXFILTER	= TEXFILTER = "(*.bmp)|*.bmp|";
	
	AuAutoSetDirectory	restoreCurrentDirectory;
	CString	strpath(restoreCurrentDirectory.bGetSavedPath());

	strpath += AgcuEffPath::bGetPath_Tex();
	strpath += _T("*.bmp");
	CFileDialog dlgFile(TRUE,NULL,strpath,OFN_READONLY,TEXFILTER,this);

	if( IDOK == dlgFile.DoModal() )
	{
		ToWnd( dlgFile.GetFileName() );
		m_szMask = dlgFile.GetFileName();
		UpdateData(FALSE);
	}
}

void CDlg_EffRenderBase::OnSelchangeComboBlend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(FALSE);
	m_nBlendType = m_ctrlCombo_Blend.GetCurSel();
}
