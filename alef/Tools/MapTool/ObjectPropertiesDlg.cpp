// ObjectPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ObjectPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDlg dialog


CObjectPropertiesDlg::CObjectPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectPropertiesDlg)
	m_bStatic		= FALSE	;
	m_fPositionX	= 0.0f	;
	m_fPositionY	= 0.0f	;
	m_fPositionZ	= 0.0f	;
	m_fScaleX		= 0.0f	;
	m_fScaleY		= 0.0f	;
	m_fScaleZ		= 0.0f	;
	m_fDegreeX		= 0.0f	;
	m_fDegreeY		= 0.0f	;
	m_bBridge		= FALSE	;
	m_bWall			= FALSE	;
	//}}AFX_DATA_INIT
}


void CObjectPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectPropertiesDlg)
	DDX_Check(pDX, IDC_OBJECT_STATIC, m_bStatic);
	DDX_Text(pDX, IDC_POSITION_X, m_fPositionX);
	DDX_Text(pDX, IDC_POSITION_Y, m_fPositionY);
	DDX_Text(pDX, IDC_POSITION_Z, m_fPositionZ);
	DDX_Text(pDX, IDC_SCALE_X, m_fScaleX);
	DDX_Text(pDX, IDC_SCALE_Y, m_fScaleY);
	DDX_Text(pDX, IDC_SCALE_Z, m_fScaleZ);
	DDX_Text(pDX, IDC_ROTATE_DEGREEX, m_fDegreeX);
	DDX_Text(pDX, IDC_ROTATE_DEGREEY, m_fDegreeY);
	DDX_Check(pDX, IDC_BRIDGE, m_bBridge);
	DDX_Check(pDX, IDC_WALL, m_bWall);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectPropertiesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertiesDlg message handlers

void CObjectPropertiesDlg::SetObject(ApdObject *pcsObject, AgcdObject *pstAgcdObject)
{
	m_pcsObject = pcsObject;
	m_pstAgcdObject = pstAgcdObject;

	m_bStatic		= pcsObject->m_bStatic			;

	m_fPositionX	= pcsObject->m_stPosition	.x	;
	m_fPositionY	= pcsObject->m_stPosition	.y	;
	m_fPositionZ	= pcsObject->m_stPosition	.z	;
	m_fScaleX		= pcsObject->m_stScale		.x	;
	m_fScaleY		= pcsObject->m_stScale		.y	;
	m_fScaleZ		= pcsObject->m_stScale		.z	;
	m_fDegreeX		= pcsObject->m_fDegreeX			;
	m_fDegreeY		= pcsObject->m_fDegreeY			;

	if( ACUOBJECT_TYPE_RIDABLE	& AcuObject::GetProperty( pstAgcdObject->m_lObjectType ) )
		m_bBridge	= TRUE;
	if( ACUOBJECT_TYPE_BLOCKING	& AcuObject::GetProperty( pstAgcdObject->m_lObjectType ) )
		m_bWall		= TRUE;
}

void CObjectPropertiesDlg::OnOK() 
{
	UpdateData(TRUE);

	m_pcsObject->m_bStatic			= m_bStatic			;
	
	m_pcsObject->m_stPosition	.x	= m_fPositionX		;
	m_pcsObject->m_stPosition	.y	= m_fPositionY		;
	m_pcsObject->m_stPosition	.z	= m_fPositionZ		;
	m_pcsObject->m_stScale		.x	= m_fScaleX			;
	m_pcsObject->m_stScale		.y	= m_fScaleY			;
	m_pcsObject->m_stScale		.z	= m_fScaleZ			;
	m_pcsObject->m_fDegreeX			= m_fDegreeX		;
	m_pcsObject->m_fDegreeY			= m_fDegreeY		;

	m_pstAgcdObject->m_lObjectType		= ACUOBJECT_TYPE_OBJECT;
	if( m_bWall		)
	{
		m_pcsObject->m_nObjectType	|=	APDOBJECT_TYPE_BLOCKING	;
		m_pstAgcdObject->m_lObjectType	|=	ACUOBJECT_TYPE_BLOCKING	;
	}
	if( m_bBridge	)
	{
		m_pcsObject->m_nObjectType	|=	APDOBJECT_TYPE_RIDABLE	;
		m_pstAgcdObject->m_lObjectType	|=	ACUOBJECT_TYPE_RIDABLE	;
	}

	g_pcsAgcmObject->SetupObjectClump( m_pcsObject );

	// 오브젝트 업데이트..

	CDialog::OnOK();
}
