// RotationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeffectdlg.h"
#include "RotationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRotationDlg dialog


CRotationDlg::CRotationDlg(AgcdUseEffectSetDataRotation **ppstRotation, CWnd* pParent /*=NULL*/)
	: CDialog(CRotationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRotationDlg)
	m_fAngleX = 0.0f;
	m_fAngleY = 0.0f;
	m_fAngleZ = 0.0f;
	//}}AFX_DATA_INIT

	m_ppstRotation	= ppstRotation;

	//@{ burumal 2006/01/18
	m_bFirstChanged = FALSE;
	//@}
}


void CRotationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotationDlg)
	DDX_Text(pDX, IDC_EDIT_X, m_fAngleX);
	DDX_Text(pDX, IDC_EDIT_Y, m_fAngleY);
	DDX_Text(pDX, IDC_EDIT_Z, m_fAngleZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRotationDlg, CDialog)
	//{{AFX_MSG_MAP(CRotationDlg)
	ON_BN_CLICKED(IDC_BUUTON_APPLY, OnBuutonApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotationDlg message handlers
BOOL CRotationDlg::ApplyData()
{
	UpdateData(TRUE);

	AME_MATRIX	stTempMatrix;
	BOOL		bSetRotation	= FALSE;

	if (m_fAngleX != 0.0f)
	{
		AuMathEx::MatrixRotate(&stTempMatrix, &GV_AME_V3D_X_AXIS, m_fAngleX);
		bSetRotation	= TRUE;
	}

	if (m_fAngleY != 0.0f)
	{
		AuMathEx::MatrixRotate(&stTempMatrix, &GV_AME_V3D_Y_AXIS, m_fAngleY);
		bSetRotation	= TRUE;
	}

	if (m_fAngleZ != 0.0f)
	{
		AuMathEx::MatrixRotate(&stTempMatrix, &GV_AME_V3D_Z_AXIS, m_fAngleZ);
		bSetRotation	= TRUE;
	}

	if ( bSetRotation && (m_bFirstChanged == FALSE) )
	{
		//@{ burumal 2006/01/18
		m_bFirstChanged = TRUE;
		//@}
	}

	if (bSetRotation)
	{
		if (!(*m_ppstRotation))
		{
			*m_ppstRotation	= (AgcdUseEffectSetDataRotation *)(malloc(sizeof(AgcdUseEffectSetDataRotation)));
			if (!(*m_ppstRotation))
			{
				MessageBox("내부 ERROR입니다!");
				return FALSE;
			}
		}
	}
	else
	{
		if (*m_ppstRotation)
		{
			free(*m_ppstRotation);
			*m_ppstRotation = NULL;
		}

		return TRUE;
	}

	(*m_ppstRotation)->m_stRight.x	= stTempMatrix.right.x;
	(*m_ppstRotation)->m_stRight.y	= stTempMatrix.right.y;
	(*m_ppstRotation)->m_stRight.z	= stTempMatrix.right.z;

	(*m_ppstRotation)->m_stUp.x		= stTempMatrix.up.x;
	(*m_ppstRotation)->m_stUp.y		= stTempMatrix.up.y;
	(*m_ppstRotation)->m_stUp.z		= stTempMatrix.up.z;

	(*m_ppstRotation)->m_stAt.x		= stTempMatrix.at.x;
	(*m_ppstRotation)->m_stAt.y		= stTempMatrix.at.y;
	(*m_ppstRotation)->m_stAt.z		= stTempMatrix.at.z;

	if (AgcmEffectDlg::GetInstance())
		AgcmEffectDlg::GetInstance()->UpdateEffectScript();

	return TRUE;
}

void CRotationDlg::OnBuutonApply() 
{
	// TODO: Add your control notification handler code here
	ApplyData();
}

void CRotationDlg::OnOK() 
{
	// TODO: Add extra validation here
//	ApplyData();

	CDialog::OnOK();
}
