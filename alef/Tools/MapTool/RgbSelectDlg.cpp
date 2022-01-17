// RgbSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "RgbSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRgbSelectDlg dialog


CRgbSelectDlg::CRgbSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRgbSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRgbSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRgbSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRgbSelectDlg)
	DDX_Control(pDX, IDC_DIRECTIONAL_RGB, m_ctlDirectionalRGB);
	DDX_Control(pDX, IDC_AMBIANT_RGB, m_ctlAmbiantRGB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRgbSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CRgbSelectDlg)
	ON_BN_CLICKED(IDC_BACKCOLORSET, OnBackcolorset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRgbSelectDlg message handlers

BOOL CRgbSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	COLORREF	intAmbiant , intDirectional;

	intAmbiant		= RGB(	( int ) ( 255.0f * m_AmbiantColor.red		) ,
							( int ) ( 255.0f * m_AmbiantColor.green		) ,
							( int ) ( 255.0f * m_AmbiantColor.blue		) );
	intDirectional	= RGB(	( int ) ( 255.0f * m_DirectionalColor.red	) ,
							( int ) ( 255.0f * m_DirectionalColor.green	) ,
							( int ) ( 255.0f * m_DirectionalColor.blue	) );

	m_ctlAmbiantRGB		.Init( CHSVControlStatic::CIRCLETYPE , intAmbiant		);
	m_ctlDirectionalRGB	.Init( CHSVControlStatic::CIRCLETYPE , intDirectional	);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRgbSelectDlg::OnOK() 
{
	// int 값을 float로 변경.
	m_AmbiantColor.alpha		=	0.0f;
	m_AmbiantColor.red			=	( float ) m_ctlAmbiantRGB.m_nR / 255.0f;
	m_AmbiantColor.green		=	( float ) m_ctlAmbiantRGB.m_nG / 255.0f;
	m_AmbiantColor.blue			=	( float ) m_ctlAmbiantRGB.m_nB / 255.0f;

	m_DirectionalColor.alpha	=	0.0f;
	m_DirectionalColor.red		=	( float ) m_ctlDirectionalRGB.m_nR / 255.0f;
	m_DirectionalColor.green	=	( float ) m_ctlDirectionalRGB.m_nG / 255.0f;
	m_DirectionalColor.blue		=	( float ) m_ctlDirectionalRGB.m_nB / 255.0f;

	CDialog::OnOK();
}

void CRgbSelectDlg::OnBackcolorset() 
{
	// TODO: Add your control notification handler code here
	// int 값을 float로 변경.

	switch ( MessageBox( "현재 Directional Light 의 설정값을 이용해서 배경 이미지를 바꿉니다. 괜찮습니까?..-_-+" ,
		"배아제" , MB_YESNOCANCEL ) )
	{
	case IDYES:
		m_DirectionalColor.alpha	=	0.0f;
		m_DirectionalColor.red		=	( float ) m_ctlDirectionalRGB.m_nR / 255.0f;
		m_DirectionalColor.green	=	( float ) m_ctlDirectionalRGB.m_nG / 255.0f;
		m_DirectionalColor.blue		=	( float ) m_ctlDirectionalRGB.m_nB / 255.0f;

		EndDialog( IDC_BACKCOLORSET );
		break;
	default:
		break;
	}
}
