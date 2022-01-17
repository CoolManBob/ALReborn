// ObjectDialog.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ObjectDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectDialog dialog


CObjectDialog::CObjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectDialog)
	m_strName = _T("");
	m_strFile = _T("");
	m_nObjectRed = 127;
	m_nObjectGreen = 127;
	m_nObjectBlue = 127;
	m_bPreLight = FALSE;
	m_bForcePreLight = FALSE;
	m_bLight = FALSE;
	m_bMaterialColor = FALSE;
	m_bAmbient = FALSE;
	m_nObjectAlpha = 255;
	m_bAlpha = FALSE;
	m_bNoCameraAlpha = FALSE;
	m_bBlocking = FALSE;
	m_bRidable = FALSE;
	//}}AFX_DATA_INIT
}


void CObjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectDialog)
	DDX_Control(pDX, IDC_OBJECT_PRELIGHT_FORCE, m_ctlForcePreLight);
	DDX_Control(pDX, IDC_EDIT_OBJECT_ALPHA, m_ctlAlpha);
	DDX_Control(pDX, IDC_EDIT_OBJECT_BLUE, m_ctlBlue);
	DDX_Control(pDX, IDC_EDIT_OBJECT_GREEN, m_ctlGreen);
	DDX_Control(pDX, IDC_EDIT_OBJECT_RED, m_ctlRed);
	DDX_Text(pDX, IDC_EDIT1, m_strName);
	DDX_Text(pDX, IDC_EDIT2, m_strFile);
	DDX_Text(pDX, IDC_EDIT_OBJECT_RED, m_nObjectRed);
	DDX_Text(pDX, IDC_EDIT_OBJECT_GREEN, m_nObjectGreen);
	DDX_Text(pDX, IDC_EDIT_OBJECT_BLUE, m_nObjectBlue);
	DDX_Check(pDX, IDC_OBJECT_PRELIGHT, m_bPreLight);
	DDX_Check(pDX, IDC_OBJECT_PRELIGHT_FORCE, m_bForcePreLight);
	DDX_Check(pDX, IDC_OBJECT_LIGHT, m_bLight);
	DDX_Check(pDX, IDC_OBJECT_MATERIAL, m_bMaterialColor);
	DDX_Check(pDX, IDC_OBJECT_AMBIENT, m_bAmbient);
	DDX_Text(pDX, IDC_EDIT_OBJECT_ALPHA, m_nObjectAlpha);
	DDX_Check(pDX, IDC_OBJECT_ALPHA, m_bAlpha);
	DDX_Check(pDX, IDC_OBJECT_NO_CAMERA_ALPHA, m_bNoCameraAlpha);
	DDX_Check(pDX, IDC_OBJECT_BLOCKING, m_bBlocking);
	DDX_Check(pDX, IDC_OBJECT_RIDABLE, m_bRidable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectDialog, CDialog)
	//{{AFX_MSG_MAP(CObjectDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_OBJECT_PRELIGHT, OnObjectPrelight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectDialog message handlers

void CObjectDialog::OnButtonBrowse() 
{
	CFileDialog dlg(
		TRUE ,
		NULL ,
		NULL ,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , 
		"RWS파일 (*.rws)|*.rws|DFF 파일 (*.dff)|*.dff|죄다 (*.*)|*.*|" ,
		this );

	UpdateData();

	if( dlg.DoModal() == IDOK )
	{
		m_strFile = dlg.GetFileName();
		UpdateData( FALSE );
	}
}

void CObjectDialog::OnObjectPrelight() 
{
	UpdateData(TRUE);

	m_ctlRed.EnableWindow(m_bPreLight);
	m_ctlGreen.EnableWindow(m_bPreLight);
	m_ctlBlue.EnableWindow(m_bPreLight);
	m_ctlAlpha.EnableWindow(m_bPreLight);

	m_ctlForcePreLight.EnableWindow(m_bPreLight);
}
