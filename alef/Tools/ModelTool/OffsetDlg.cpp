// OffsetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "OffsetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COffsetDlg dialog


COffsetDlg::COffsetDlg(CCharacterOffset *pCharacterOffset, CCameraOffset *pCameraOffset, CEditOffset *pEditOffset, CWnd* pParent /*=NULL*/)
	: CDialog(COffsetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COffsetDlg)
	m_fCameraMove		= pCameraOffset			? pCameraOffset->m_fMove : 0.0f;
	m_fCameraRotate		= pCameraOffset			? pCameraOffset->m_fRotate : 0.0f;
	m_fCameraZoom		= pCameraOffset			? pCameraOffset->m_fZoom : 0.0f;
	m_fEditPosit		= pEditOffset			? pEditOffset->m_fPosit : 0.0f;
	m_fEditScale		= pEditOffset			? pEditOffset->m_fScale : 0.0f;
	m_fEditRotate		= pEditOffset			? pEditOffset->m_fRotate : 0.0f;
	m_bCharacterTarget	= pCharacterOffset		? pCharacterOffset->m_bCharacterTarget : FALSE;
	//}}AFX_DATA_INIT

	m_pcsCharacterOffset	= pCharacterOffset;
	m_pcsCameraOffset		= pCameraOffset;
	m_pcsEditOffset			= pEditOffset;
}


void COffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetDlg)
	DDX_Text(pDX, IDC_EDIT_OFFSET_CAMERA_MOVE, m_fCameraMove);
	DDX_Text(pDX, IDC_EDIT_OFFSET_CAMERA_ROTATE, m_fCameraRotate);
	DDX_Text(pDX, IDC_EDIT_OFFSET_CAMERA_ZOOM, m_fCameraZoom);
	DDX_Text(pDX, IDC_EDIT_OFFSET_EDIT_POSIT, m_fEditPosit);
	DDX_Text(pDX, IDC_EDIT_OFFSET_EDIT_SCALE, m_fEditScale);
	DDX_Text(pDX, IDC_EDIT_OFFSET_EDIT_ROTATE, m_fEditRotate);
	DDX_Check(pDX, IDC_CHECK_OFFSET_CHARACTER_TARGET, m_bCharacterTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetDlg, CDialog)
	//{{AFX_MSG_MAP(COffsetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetDlg message handlers

void COffsetDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if (m_pcsCameraOffset)
	{
		m_pcsCameraOffset->m_fRotate					= m_fCameraRotate;
		m_pcsCameraOffset->m_fZoom						= m_fCameraZoom;
		m_pcsCameraOffset->m_fMove						= m_fCameraMove;
	}

	if (m_pcsEditOffset)
	{
		m_pcsEditOffset->m_fPosit						= m_fEditPosit;
		m_pcsEditOffset->m_fRotate						= m_fEditRotate;
		m_pcsEditOffset->m_fScale						= m_fEditScale;
	}

	if (m_pcsCharacterOffset)
	{
		m_pcsCharacterOffset->m_bCharacterTarget		= m_bCharacterTarget;
	}

	if (CModelToolApp::GetInstance())
		CModelToolApp::GetInstance()->SetTarget();

	CDialog::OnOK();
}
