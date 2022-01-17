// TemplateNameEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeventnaturedlg.h"
#include "TemplateNameEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateNameEditDlg dialog


CTemplateNameEditDlg::CTemplateNameEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateNameEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateNameEditDlg)
	m_strName		= _T("");
	m_bUseCloud		= TRUE;
	m_bUseEffect	= TRUE;
	m_bUseFog		= TRUE;
	m_bUseLight		= TRUE;
	//}}AFX_DATA_INIT
}


void CTemplateNameEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateNameEditDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Check(pDX, IDC_USE_CLOUD, m_bUseCloud);
	DDX_Check(pDX, IDC_USE_EFFECT, m_bUseEffect);
	DDX_Check(pDX, IDC_USE_FOG, m_bUseFog);
	DDX_Check(pDX, IDC_USE_LIGHT, m_bUseLight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateNameEditDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateNameEditDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateNameEditDlg message handlers

void CTemplateNameEditDlg::OnOK() 
{
	UpdateData( TRUE );

	if( m_strName.GetLength() > 0 && m_strName.GetLength() < AGPDSKYSET_MAX_NAME )
	{
		CDialog::OnOK();
	}
	else
	{
		CString	str;
		str.Format( "템플릿 이름은 %d자 이내여야합니다." , AGPDSKYSET_MAX_NAME );
		MessageBox( str , "에러!" , MB_ICONERROR | MB_OK );
		return;
	}
}
