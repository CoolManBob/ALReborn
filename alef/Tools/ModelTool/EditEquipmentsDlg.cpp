// EditEquipmentsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "EditEquipmentsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEquipmentsDlg dialog


CEditEquipmentsDlg::CEditEquipmentsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditEquipmentsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditEquipmentsDlg)
	m_bCheckPickMode	= TRUE;
	m_fPositAt			= 0.0f;
	m_fPositRight		= 0.0f;
	m_fPositUp			= 0.0f;
	m_nRotateAxis		= 0;
	m_fRotateDegree		= 0.0f;
	m_fScaleRight		= 1.0f;
	m_fScaleUp			= 1.0f;
	m_fScaleAt			= 1.0f;
	m_bMouseMode		= FALSE;
	m_nMouseModeAxis	= 0;
	//}}AFX_DATA_INIT
}


void CEditEquipmentsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditEquipmentsDlg)
	DDX_Check(pDX, IDC_CHECK_EDIT_EQUIP_PICK_MODE, m_bCheckPickMode);
	DDX_Text(pDX, IDC_EDIT_POSIT_AT, m_fPositAt);
	DDX_Text(pDX, IDC_EDIT_POSIT_RIGHT, m_fPositRight);
	DDX_Text(pDX, IDC_EDIT_POSIT_UP, m_fPositUp);
	DDX_Radio(pDX, IDC_RADIO_ROTATE_RIGHT, m_nRotateAxis);
	DDX_Text(pDX, IDC_EDIT_ROTATE_DEGREE, m_fRotateDegree);
	DDX_Text(pDX, IDC_EDIT_SCALE_RIGHT, m_fScaleRight);
	DDX_Text(pDX, IDC_EDIT_SCALE_UP, m_fScaleUp);
	DDX_Text(pDX, IDC_EDIT_SCALE_AT, m_fScaleAt);
	DDX_Check(pDX, IDC_CHECK_EDIT_EQUIP_MOUSE_MODE, m_bMouseMode);
	DDX_Radio(pDX, IDC_RADIO_MOUSE_MODE_AXIS_RIGHT, m_nMouseModeAxis);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditEquipmentsDlg, CDialog)
	//{{AFX_MSG_MAP(CEditEquipmentsDlg)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_EQUIP_APPLY, OnButtonEditEquipApply)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_EDIT_EQUIP_RESET, OnButtonEditEquipReset)
	ON_BN_CLICKED(IDC_CHECK_EDIT_EQUIP_MOUSE_MODE, OnCheckEditEquipMouseMode)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_EQUIP_COPY, OnButtonEditEquipCopy)
	ON_CBN_SELCHANGE(IDC_COMBO_MOUSE_MODE, OnSelchangeComboMouseMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditEquipmentsDlg message handlers

void CEditEquipmentsDlg::OnButtonEditEquipApply() 
{
	// TODO: Add your control notification handler code here
	if(CModelToolApp::GetInstance())
	{
		UpdateData(TRUE);

		RwV3d v3dPos	= RwUtilMakeV3d(m_fPositRight, m_fPositUp, m_fPositAt);
		RwV3d v3dScale	= RwUtilMakeV3d(m_fScaleRight, m_fScaleUp, m_fScaleAt);
/*		RwV3d v3dAxis;

		if(m_nRotateAxis == 0)
		{
			RwV3dAssign(&v3dAxis, &RWUTIL_XAXIS);
		}
		else if(m_nRotateAxis == 1)
		{
			RwV3dAssign(&v3dAxis, &RWUTIL_YAXIS);
		}
		else
		{
			RwV3dAssign(&v3dAxis, &RWUTIL_ZAXIS);
		}*/

		CModelToolApp::GetInstance()->ApplyTransform(&v3dPos, m_nRotateAxis, m_fRotateDegree, &v3dScale);

		ResetValue();
	}
}

void CEditEquipmentsDlg::OnOK() 
{
	// TODO: Add extra validation here
	CModelToolDlg::GetInstance()->CloseEditEquipmentsDlg();

	if(!CModelToolApp::GetInstance()->SaveTransform())
		MessageBox("Save를 할 수 없습니다!", "ERROR");
	
//	CDialog::OnOK();
}

void CEditEquipmentsDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CModelToolDlg::GetInstance()->CloseEditEquipmentsDlg();

	if(CModelToolApp::GetInstance())
		CModelToolApp::GetInstance()->InitTransformPickedAtomic();

//	CDialog::OnCancel();
}

BOOL CEditEquipmentsDlg::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::Create(IDD, pParentWnd);
}

BOOL CEditEquipmentsDlg::IsPickMode()
{
	UpdateData(TRUE);

	return m_bCheckPickMode;
}

BOOL CEditEquipmentsDlg::IsMouseMode()
{
	UpdateData(TRUE);

	return m_bMouseMode;
}

void CEditEquipmentsDlg::OnDestroy() 
{
//	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CEditEquipmentsDlg::OnButtonEditEquipReset() 
{
	// TODO: Add your control notification handler code here
	if(CModelToolApp::GetInstance())
		CModelToolApp::GetInstance()->InitTransformPickedAtomic();
	
	ResetValue();
}

VOID CEditEquipmentsDlg::ResetValue()
{
	m_fPositAt		= 0.0f;
	m_fPositRight	= 0.0f;
	m_fPositUp		= 0.0f;
	m_fRotateDegree = 0.0f;
	m_fScaleRight	= 1.0f;
	m_fScaleUp		= 1.0f;
	m_fScaleAt		= 1.0f;

	UpdateData(FALSE);
}

void CEditEquipmentsDlg::OnCheckEditEquipMouseMode() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(m_bMouseMode)
	{
		GetDlgItem(IDC_BUTTON_EDIT_EQUIP_APPLY)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_EDIT_EQUIP_APPLY)->EnableWindow(TRUE);
	}

	EnableMouseModeAxisControl(m_bMouseMode);
}

void CEditEquipmentsDlg::OnButtonEditEquipCopy() 
{
}

BOOL CEditEquipmentsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CComboBox *pCombo = (CComboBox *)(GetDlgItem(IDC_COMBO_MOUSE_MODE));

	pCombo->InsertString(EED_POSIT,		"POSIT");
	pCombo->InsertString(EED_ROTATE,	"ROTATE");
	pCombo->InsertString(EED_SCALE,		"SCALE");

	GetDlgItem(IDC_EDIT_SCALE_RIGHT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SCALE_UP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SCALE_AT)->EnableWindow(FALSE);

	pCombo->SetCurSel(EED_POSIT);

	EnableMouseModeAxisControl(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

INT32 CEditEquipmentsDlg::GetMouseModeAxis()
{
	UpdateData(TRUE);

	return m_nMouseModeAxis;
}

INT32 CEditEquipmentsDlg::GetMouseMode()
{
	UpdateData(TRUE);

	CComboBox *pCombo = (CComboBox *)(GetDlgItem(IDC_COMBO_MOUSE_MODE));

	return pCombo->GetCurSel();
}

BOOL CEditEquipmentsDlg::EnableMouseModeAxisControl(BOOL bEnable)
{
	GetDlgItem(IDC_COMBO_MOUSE_MODE)->EnableWindow(bEnable);

	if(bEnable)
	{
		CheckMouseModeAxisControl();
	}
	else
	{
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_AT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_ALL)->EnableWindow(FALSE);
	}

	return TRUE;
}

BOOL CEditEquipmentsDlg::CheckMouseModeAxisControl()
{
	UpdateData(TRUE);

	CComboBox	*pCombo	= (CComboBox *)(GetDlgItem(IDC_COMBO_MOUSE_MODE));
	INT32		lMode	= pCombo->GetCurSel();

	if(lMode == EED_POSIT)
	{
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_AT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_ALL)->EnableWindow(FALSE);
	}
	else if(lMode == EED_ROTATE)
	{
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_RIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_UP)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_AT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_ALL)->EnableWindow(FALSE);
	}
	else if(lMode == EED_SCALE)
	{
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_RIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_AT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_MOUSE_MODE_AXIS_ALL)->EnableWindow(FALSE);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void CEditEquipmentsDlg::OnSelchangeComboMouseMode() 
{
	// TODO: Add your control notification handler code here
	CheckMouseModeAxisControl();
}

BOOL CEditEquipmentsDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
		KeyDown( pMsg->wParam );

	return CDialog::PreTranslateMessage(pMsg);
}

void CEditEquipmentsDlg::KeyDown( WPARAM wParam )
{
	RwV3d	vec = { 0.f, 0.f, 0.f };	
	switch( wParam )
	{
	case VK_NUMPAD8:	//y-up
		vec.y += 1.f;
		break;
	case VK_NUMPAD2:	//y-down
		vec.y -= 1.f;
		break;
	case VK_NUMPAD4:	//x-down
		vec.x -= 1.f;
		break;
	case VK_NUMPAD6:	//y-up
		vec.x += 1.f;
		break;
	case VK_NUMPAD7:	//z-up
		vec.z -= 1.f;
		break;
	case VK_NUMPAD9:	//z-down
		vec.z += 1.f;
		break;
	}

	UpdateData(TRUE);

	CComboBox* pCombo = (CComboBox *)(GetDlgItem(IDC_COMBO_MOUSE_MODE));
	INT32 lMode = pCombo->GetCurSel();
	if( lMode == EED_POSIT )
		CRwUtil::GetInst().TransAtomic( CModelToolApp::GetInstance()->GetPickedAtomic(), vec );
	else if( lMode == EED_ROTATE )
		CRwUtil::GetInst().RotAtomic( CModelToolApp::GetInstance()->GetPickedAtomic(), vec );
	else if( lMode == EED_SCALE )
		CRwUtil::GetInst().ScaleAtomic( CModelToolApp::GetInstance()->GetPickedAtomic(), vec );
}