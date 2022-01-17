// UploadDataDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "UploadDataDlg.h"
#include ".\uploaddatadlg.h"


// CUploadDataDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUploadDataDlg, CDialog)
CUploadDataDlg::CUploadDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUploadDataDlg::IDD, pParent)
	, m_bAI2Template(FALSE)
	, m_bArtist(FALSE)
	, m_bCharacterTemplate(FALSE)
	, m_bDesign(FALSE)
	, m_bObjectTemplate(FALSE)
	, m_bRegionTool(FALSE)
	, m_bSkySet(FALSE)
	, m_bSpawnGroup(FALSE)
{
}

CUploadDataDlg::~CUploadDataDlg()
{
}

void CUploadDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_AI2TEMPLATE, m_bAI2Template);
	DDX_Check(pDX, IDC_ARTIST, m_bArtist);
	DDX_Check(pDX, IDC_CHARACTERTEMPLATE, m_bCharacterTemplate);
	DDX_Check(pDX, IDC_DESIGN, m_bDesign);
	DDX_Check(pDX, IDC_OBJECTTEMPLATE, m_bObjectTemplate);
	DDX_Check(pDX, IDC_REGIONTOOL, m_bRegionTool);
	DDX_Check(pDX, IDC_SKYSET, m_bSkySet);
	DDX_Check(pDX, IDC_SPAWNGROUP, m_bSpawnGroup);
}


BEGIN_MESSAGE_MAP(CUploadDataDlg, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()

INT32	CUploadDataDlg::DoModal( BOOL bUpload )
{
	if( bUpload )
	{
		m_strTitle.Format( "올려 놓을 데이타를 선택하세요" );
	}
	else
	{
		m_strTitle.Format( "다운 받을 데이타를 선택하세요" );
	}

	return CDialog::DoModal();
}

// CUploadDataDlg 메시지 처리기입니다.

int CUploadDataDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText( m_strTitle );

	return 0;
}
