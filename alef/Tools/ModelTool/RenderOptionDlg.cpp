// RenderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "modeltool.h"
#include "RenderOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenderOptionDlg dialog

CRenderOptionDlg::CRenderOptionDlg(CRenderOption *pcsOption, CWnd* pParent /*=NULL*/)
	: CDialog(CRenderOptionDlg::IDD, pParent)
	, m_bMatD3DFx(FALSE)
	, m_bShowTextBoarder(FALSE)
	, m_iKeyFrameRate(0)
	, m_bCheckSaveFile(FALSE)
{
	m_CurrentRenderOption		= *pcsOption;
	m_pcsDestRenderOption		= pcsOption;
}

void CRenderOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_SHOW_CAMERA_POS,			m_CurrentRenderOption.m_bShowCameraPos);
	DDX_Check(pDX, IDC_SHOW_COLLISION_CLUMP,	m_CurrentRenderOption.m_bShowCollisionClump);
	DDX_Check(pDX, IDC_SHOW_FPS,				m_CurrentRenderOption.m_bShowFPS);
	DDX_Check(pDX, IDC_SHOW_BLOCKING,			m_CurrentRenderOption.m_bShowBlocking);
	DDX_Check(pDX, IDC_SHOW_CLUMP_POLYGONS,		m_CurrentRenderOption.m_bShowClumpPolygons);
	DDX_Check(pDX, IDC_SHOW_CLUMP_VERTICES,		m_CurrentRenderOption.m_bShowClumpVertices);
	DDX_Check(pDX, IDC_SHOW_PICK_CLUMP,			m_CurrentRenderOption.m_bShowPickClump);
	DDX_Check(pDX, IDC_SHOW_HIT_RANGE,			m_CurrentRenderOption.m_bShowHitRange);
	DDX_Check(pDX, IDC_SHOW_SIEGEWAR_COLL_OBJ,	m_CurrentRenderOption.m_bShowSiegeWarCollObj);
	DDX_Check(pDX, IDC_SHOW_WIREFRAME,			m_CurrentRenderOption.m_bShowWireFrame);
	DDX_Check(pDX, IDC_SHOW_BOUNDING_SPHERE,	m_CurrentRenderOption.m_bShowBoundingSphere);
	DDX_Check(pDX, IDC_RENDER_MATD3DFX,			m_CurrentRenderOption.m_bMatD3DFx);
	DDX_Check(pDX, IDC_SHOW_TEXT_BOARDER,		m_CurrentRenderOption.m_bShowTextBoarder);
	DDX_Check(pDX, IDC_SAVE_CUSTOM_ANIMFILE,	m_CurrentRenderOption.m_bCheckSaveFile);
	DDX_Check(pDX, IDC_ENABLE_SOUND,			m_CurrentRenderOption.m_bEnableSound);
	DDX_Check(pDX, IDC_RENDER_DETAILINFO,		m_CurrentRenderOption.m_bShowTemp);
	DDX_CBIndex(pDX, IDC_ANIMATIONRATECOMBO,	m_CurrentRenderOption.m_iKeyFrameRate);
	DDX_Text(pDX, IDC_TERRAIN_TEXTURE,			m_CurrentRenderOption.m_strTerrain);
	DDX_Text(pDX, IDC_RESOUCE_TREE_RECT,		m_CurrentRenderOption.m_dwResourceTreeWidth);
}

BEGIN_MESSAGE_MAP(CRenderOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CRenderOptionDlg)
	ON_BN_CLICKED(IDC_BTN_TERRAIN_TEXTURE, OnBtnClickedTerrainTexture)
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenderOptionDlg message handlers

void CRenderOptionDlg::OnOK() 
{
	UpdateData(TRUE);

	m_pcsDestRenderOption->m_bShowBlocking			= m_CurrentRenderOption.m_bShowBlocking;
	m_pcsDestRenderOption->m_bShowCameraPos			= m_CurrentRenderOption.m_bShowCameraPos;
	m_pcsDestRenderOption->m_bShowCollisionClump	= m_CurrentRenderOption.m_bShowCollisionClump;
	m_pcsDestRenderOption->m_bShowPickClump			= m_CurrentRenderOption.m_bShowPickClump;	
	m_pcsDestRenderOption->m_bShowFPS				= m_CurrentRenderOption.m_bShowFPS;
	m_pcsDestRenderOption->m_bShowClumpPolygons		= m_CurrentRenderOption.m_bShowClumpPolygons;
	m_pcsDestRenderOption->m_bShowClumpVertices		= m_CurrentRenderOption.m_bShowClumpVertices;
	m_pcsDestRenderOption->m_bShowWireFrame			= m_CurrentRenderOption.m_bShowWireFrame ;
	m_pcsDestRenderOption->m_bShowBoundingSphere	= m_CurrentRenderOption.m_bShowBoundingSphere;
	m_pcsDestRenderOption->m_bMatD3DFx				= m_CurrentRenderOption.m_bMatD3DFx;
	m_pcsDestRenderOption->m_bShowTextBoarder		= m_CurrentRenderOption.m_bShowTextBoarder;
	m_pcsDestRenderOption->m_iKeyFrameRate			= m_CurrentRenderOption.m_iKeyFrameRate;
	m_pcsDestRenderOption->m_bCheckSaveFile			= m_CurrentRenderOption.m_bCheckSaveFile;
	m_pcsDestRenderOption->m_bShowHitRange			= m_CurrentRenderOption.m_bShowHitRange;		
	m_pcsDestRenderOption->m_bEnableSound			= m_CurrentRenderOption.m_bEnableSound;
	m_pcsDestRenderOption->m_bShowTemp				= m_CurrentRenderOption.m_bShowTemp;
	m_pcsDestRenderOption->m_bShowSiegeWarCollObj	= m_CurrentRenderOption.m_bShowSiegeWarCollObj;
	m_pcsDestRenderOption->m_dwResourceTreeWidth	= m_CurrentRenderOption.m_dwResourceTreeWidth;

	RwD3D9SetRenderState( D3DRS_FILLMODE , m_CurrentRenderOption.m_bShowWireFrame ? D3DFILL_WIREFRAME : D3DFILL_SOLID );

	CDialog::OnOK();
}

void CRenderOptionDlg::OnBtnClickedTerrainTexture()
{
	static char* szFilter = "DFF(*.DFF)|*.DFF||";
	CFileDialog		cDlg( TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter );
	if( IDOK == cDlg.DoModal() )
	{
		m_CurrentRenderOption.m_strTerrain	= cDlg.GetPathName();
		m_pcsDestRenderOption->m_strTerrain = cDlg.GetPathName();
		UpdateData( FALSE );

		CModelToolApp::GetInstance()->LoadTerrain( TRUE );
	}
}
