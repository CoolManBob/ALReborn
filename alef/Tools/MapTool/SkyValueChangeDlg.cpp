// SkyValueChangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "SkyValueChangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkyValueChangeDlg dialog


CSkyValueChangeDlg::CSkyValueChangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSkyValueChangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkyValueChangeDlg)
	m_fCircumstanceHeight	= 0.0f	;
	m_fCircumstanceRadius	= 0.0f	;
	m_fSkyHeight			= 0.0f	;
	m_fSkyRadius			= 0.0f	;
	m_nCircumstanceHeight	= 0		;
	m_nCircumstanceRadius	= 0		;
	m_nSkyHeight			= 0		;
	m_nSkyRadius			= 0		;
	//}}AFX_DATA_INIT
}


void CSkyValueChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkyValueChangeDlg)
	DDX_Text(pDX, IDC_CIRCUMSTANCE_HEIGHT, m_fCircumstanceHeight);
	DDX_Text(pDX, IDC_CIRCUMSTANCE_RANGE, m_fCircumstanceRadius);
	DDX_Text(pDX, IDC_SKY_HEIGHT, m_fSkyHeight);
	DDX_Text(pDX, IDC_SKY_RANGE, m_fSkyRadius);
	DDX_Slider(pDX, IDC_CIRCUMSTANCE_HEIGHT_SLIDER, m_nCircumstanceHeight);
	DDX_Slider(pDX, IDC_CIRCUMSTANCE_RANGE_SLIDER, m_nCircumstanceRadius);
	DDX_Slider(pDX, IDC_SKY_HEIGHT_SLIDER, m_nSkyHeight);
	DDX_Slider(pDX, IDC_SKY_RANGE_SLIDER, m_nSkyRadius);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSkyValueChangeDlg, CDialog)
	//{{AFX_MSG_MAP(CSkyValueChangeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkyValueChangeDlg message handlers
