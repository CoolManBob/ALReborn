// RoughDisplayDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "RoughDisplayDlg.h"

#include "MyEngine.h"

// CRoughDisplayDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRoughDisplayDlg, CDialog)
CRoughDisplayDlg::CRoughDisplayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoughDisplayDlg::IDD, pParent)
	, m_fSrcX(0)
	, m_fSrcZ(0)
	, m_fSrcHeight(0)
	, m_nRadius(0)
	, m_fCloudHeight(0)
	, m_fDstX(0)
	, m_fDstZ(0)
{
}

CRoughDisplayDlg::~CRoughDisplayDlg()
{
}

void CRoughDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SRC_X, m_fSrcX);
	DDX_Text(pDX, IDC_SRC_Z, m_fSrcZ);
	DDX_Text(pDX, IDC_SRC_HEIGHT, m_fSrcHeight);
	DDX_Text(pDX, IDC_RADIUS, m_nRadius);
	DDX_Text(pDX, IDC_CLOUD_HEIGHT, m_fCloudHeight);
	DDX_Text(pDX, IDC_DST_X, m_fDstX);
	DDX_Text(pDX, IDC_DST_Z, m_fDstZ);
}


BEGIN_MESSAGE_MAP(CRoughDisplayDlg, CDialog)
	ON_BN_CLICKED(IDC_SHOW, OnBnClickedShow)
	ON_BN_CLICKED(IDC_HIDE, OnBnClickedHide)
	ON_BN_CLICKED(IDC_SET_DEFAULT, OnBnClickedSetDefault)
END_MESSAGE_MAP()


void CRoughDisplayDlg::OnBnClickedShow()
{
	UpdateData( TRUE );

	AGCMMAP_THIS->LoadZoneRoughBuffer(
		m_fSrcX,
		m_fSrcZ,
		m_fSrcHeight,
		m_fDstX,
		m_fDstZ,
		m_nRadius );

	OnOK();
}

void CRoughDisplayDlg::OnBnClickedHide()
{
	AGCMMAP_THIS->FlushZoneRoughBuffer();
}

void CRoughDisplayDlg::OnBnClickedSetDefault()
{
	UpdateData( TRUE );
	RwV3d			*	camPos			= RwMatrixGetPos( RwFrameGetLTM( RwCameraGetFrame( g_pEngine->m_pCamera ) ) );

	INT32 nDiv			= GetDivisionIndexF( camPos->x , camPos->z );

	m_fDstX			= GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDiv ) ) ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
	m_fDstZ			= GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDiv ) ) ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
	UpdateData( FALSE );
}
