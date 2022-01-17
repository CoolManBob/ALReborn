#include "stdafx.h"
#include "../resource.h"
#include "AgcmObjectTypeDlg.h"
#include "ObjectTypeDlg.h"
#include ".\objecttypedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectTypeDlg dialog

CObjectTypeDlg::CObjectTypeDlg(INT32 *plObjectType, RwRGBA *pstRGBA, CWnd* pParent /*=NULL*/)
	: CDialog(CObjectTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectTypeDlg)
	m_bAlpha			= (*plObjectType & ACUOBJECT_TYPE_USE_ALPHA)		? (TRUE) : (FALSE);
	m_bAmbient			= (*plObjectType & ACUOBJECT_TYPE_USE_AMBIENT)		? (TRUE) : (FALSE);
	m_bBlocking			= (*plObjectType & ACUOBJECT_TYPE_BLOCKING)			? (TRUE) : (FALSE);
	m_bLight			= (*plObjectType & ACUOBJECT_TYPE_USE_LIGHT)		? (TRUE) : (FALSE);
	m_bNoCameraAlpha	= (*plObjectType & ACUOBJECT_TYPE_NO_CAMERA_ALPHA)	? (TRUE) : (FALSE);
	m_bPreLight			= (*plObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)	? (TRUE) : (FALSE);
	m_bRidable			= (*plObjectType & ACUOBJECT_TYPE_RIDABLE)			? (TRUE) : (FALSE);
	m_unAlpha			= pstRGBA->alpha;
	m_unBlue			= pstRGBA->blue;
	m_unGreen			= pstRGBA->green;
	m_unRed				= pstRGBA->red;
	m_bFade				= (*plObjectType & ACUOBJECT_TYPE_USE_FADE_IN_OUT)	? (TRUE) : (FALSE);
	m_bIsSystemObject	= (*plObjectType & ACUOBJECT_TYPE_IS_SYSTEM_OBJECT)	? (TRUE) : (FALSE);
	m_bShadow			= (*plObjectType & ACUOBJECT_TYPE_OBJECTSHADOW)		? (TRUE) : (FALSE);
	m_bAlphaFunc		= (*plObjectType & ACUOBJECT_TYPE_USE_ALPHAFUNC)	? (TRUE) : (FALSE);
	m_bUDA				= (*plObjectType & ACUOBJECT_TYPE_RENDER_UDA)		? (TRUE) : (FALSE);
	m_bOccluder			= (*plObjectType & ACUOBJECT_TYPE_OCCLUDER)			? (TRUE) : (FALSE);
	m_bDonotCull		= (*plObjectType & ACUOBJECT_TYPE_DONOT_CULL)		? (TRUE) : (FALSE);
	//}}AFX_DATA_INIT

	m_plObjectType		= plObjectType;
	m_pstRGBA			= pstRGBA;
}

void CObjectTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectTypeDlg)
	DDX_Check(pDX, IDC_CHECK_ALPHA, m_bAlpha);
	DDX_Check(pDX, IDC_CHECK_AMBIENT, m_bAmbient);
	DDX_Check(pDX, IDC_CHECK_BLOCKING, m_bBlocking);
	DDX_Check(pDX, IDC_CHECK_LIGHT, m_bLight);
	DDX_Check(pDX, IDC_CHECK_NO_CAMERA_ALPHA, m_bNoCameraAlpha);
	DDX_Check(pDX, IDC_CHECK_PRE_LIGHT, m_bPreLight);
	DDX_Check(pDX, IDC_CHECK_RIDABLE, m_bRidable);
	DDX_Text(pDX, IDC_EDIT_PRE_LIGHT_ALPHA, m_unAlpha);
	DDX_Text(pDX, IDC_EDIT_PRE_LIGHT_BLUE, m_unBlue);
	DDX_Text(pDX, IDC_EDIT_PRE_LIGHT_GREEN, m_unGreen);
	DDX_Text(pDX, IDC_EDIT_PRE_LIGHT_RED, m_unRed);
	DDX_Check(pDX, IDC_CHECK_FADE, m_bFade);
	DDX_Check(pDX, IDC_CHECK_IS_SYSTEM_OBJECT, m_bIsSystemObject);
	DDX_Check(pDX, IDC_CHECK_SHADOW, m_bShadow);
	DDX_Check(pDX, IDC_CHECK_ALPHA_FUNC, m_bAlphaFunc);
	DDX_Check(pDX, IDC_CHECK_RENDER_UDA, m_bUDA);
	DDX_Check(pDX, IDC_CHECK_OCCLUDER, m_bOccluder);
	DDX_Check(pDX, IDC_DONOTCULL, m_bDonotCull);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBO_CAM_COLLISION, m_ctlCamCollision);
}


BEGIN_MESSAGE_MAP(CObjectTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectTypeDlg)
	ON_BN_CLICKED(IDC_CHECK_PRE_LIGHT, OnCheckPreLight)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_CAM_COLLISION, OnCbnSelchangeComboCamCollision)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectTypeDlg message handlers

void CObjectTypeDlg::OnOK() 
{
	UpdateData(TRUE);

	*m_plObjectType		= 0;

	if(m_bAlpha)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_USE_ALPHA;
	}

	if(m_bAmbient)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_USE_AMBIENT;
	}

	if(m_bBlocking)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_BLOCKING;
	}

	if(m_bLight)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_USE_LIGHT;
	}

	if(m_bNoCameraAlpha)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_NO_CAMERA_ALPHA;
	}

	if(m_bPreLight)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_USE_PRE_LIGHT;
	}

	if(m_bRidable)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_RIDABLE;
	}

	if(m_bFade)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_USE_FADE_IN_OUT;
	}

	if(m_bIsSystemObject)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_IS_SYSTEM_OBJECT;
	}

	if(m_bShadow)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_OBJECTSHADOW;
	}

	if (m_bAlphaFunc)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_USE_ALPHAFUNC;
	}

	if (m_bUDA)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_RENDER_UDA;
	}

	if (m_bOccluder)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_OCCLUDER;
	}

	if (m_bDonotCull)
	{
		*m_plObjectType |= ACUOBJECT_TYPE_DONOT_CULL;
	}
	switch( m_ctlCamCollision.GetCurSel() )
	{
	case 1:
		*m_plObjectType |= ACUOBJECT_TYPE_CAM_ZOOM;
		break;
	case 2:
		*m_plObjectType |= ACUOBJECT_TYPE_CAM_ALPHA;
		break;
	};

	m_pstRGBA->alpha	= m_unAlpha;
	m_pstRGBA->red		= m_unRed;
	m_pstRGBA->green	= m_unGreen;
	m_pstRGBA->blue		= m_unBlue;

	CDialog::OnOK();
}

void CObjectTypeDlg::OnCheckPreLight() 
{
	if(IsDlgButtonChecked(IDC_CHECK_PRE_LIGHT))
	{
		GetDlgItem(IDC_EDIT_PRE_LIGHT_ALPHA)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_BLUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_GREEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_RED)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_PRE_LIGHT_ALPHA)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_BLUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_GREEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_RED)->EnableWindow(FALSE);
	}
}

BOOL CObjectTypeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	if(IsDlgButtonChecked(IDC_CHECK_PRE_LIGHT))
	{
		GetDlgItem(IDC_EDIT_PRE_LIGHT_ALPHA)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_BLUE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_GREEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_RED)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_PRE_LIGHT_ALPHA)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_BLUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_GREEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PRE_LIGHT_RED)->EnableWindow(FALSE);
	}

	m_ctlCamCollision.ResetContent();
	m_ctlCamCollision.AddString( "NONE" );
	m_ctlCamCollision.AddString( "CAM_ZOOM" );
	m_ctlCamCollision.SetCurSel( 0 );

	return TRUE;
}

void CObjectTypeDlg::OnCbnSelchangeComboCamCollision()
{
}
