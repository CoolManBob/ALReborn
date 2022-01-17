// ObjectBrushSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ObjectBrushSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectBrushSettingDlg dialog


CObjectBrushSettingDlg::CObjectBrushSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectBrushSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectBrushSettingDlg)
	m_fDensity		= 1.0f		;
	m_fRotateMax	= 10.0f		;
	m_fRotateMin	= 0.0f		;
	m_fRotateYMin	= 0.0f		;
	m_fRotateYMax	= 5.0f		;
	m_fScaleMax		= 1.0f		;
	m_fScaleMin		= 1.0f		;
	//}}AFX_DATA_INIT
}


void CObjectBrushSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectBrushSettingDlg)
	DDX_Text(pDX, IDC_DENSITY, m_fDensity);
	DDX_Text(pDX, IDC_ROTATE_MAX, m_fRotateMax);
	DDX_Text(pDX, IDC_ROTATE_MIN, m_fRotateMin);
	DDX_Text(pDX, IDC_SCALE_MAX, m_fScaleMax);
	DDX_Text(pDX, IDC_SCALE_MIN, m_fScaleMin);
	DDX_Text(pDX, IDC_ROTATE_Y_MIN, m_fRotateYMin);
	DDX_Text(pDX, IDC_ROTATE_Y_MAX, m_fRotateYMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectBrushSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectBrushSettingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectBrushSettingDlg message handlers

void CObjectBrushSettingDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_fRotateMin > m_fRotateMax )
	{
		MessageBox( "Rotate 값의 Min이 Max보다 큽니다." );
		return;
	}
	if( m_fRotateYMin > m_fRotateYMax )
	{
		MessageBox( "Rotate 값의 Min이 Max보다 큽니다." );
		return;
	}
	if( m_fScaleMin > m_fScaleMax )
	{
		MessageBox( "Rotate 값의 Min이 Max보다 큽니다." );
		return;
	}
	
	CDialog::OnOK();
}
