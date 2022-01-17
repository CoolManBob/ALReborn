// WorldMapCalcDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "WorldMapCalcDlg.h"
#include "ApmMap.h"


// CWorldMapCalcDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWorldMapCalcDlg, CDialog)
CWorldMapCalcDlg::CWorldMapCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldMapCalcDlg::IDD, pParent)
	, nDivision1	(0)
	, nDivision2	(0)
	, nX1			(0)
	, nX2			(0)
	, nY1			(0)
	, nY2			(0)
{
}

CWorldMapCalcDlg::~CWorldMapCalcDlg()
{
}

void CWorldMapCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DIVISION1, nDivision1);
	DDX_Text(pDX, IDC_DIVISION2, nDivision2);
	DDX_Text(pDX, IDC_X1, nX1);
	DDX_Text(pDX, IDC_X2, nX2);
	DDX_Text(pDX, IDC_Y1, nY1);
	DDX_Text(pDX, IDC_Y2, nY2);
}


BEGIN_MESSAGE_MAP(CWorldMapCalcDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CWorldMapCalcDlg 메시지 처리기입니다.

void	GetWorldPos( INT32 nDivision1, INT32 nDivision2, float fStartX, float fStartY , float fEndX, float fEndY )
{
	FLOAT fDivX1 = GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision1 ) ) );
	FLOAT fDivX2 = GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision2 ) ) );

	FLOAT fDivY1 = GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision1 ) ) );
	FLOAT fDivY2 = GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision2 ) ) );

	float fStepSize = ( fEndX - fStartX ) / 800.f;

	
	float fX1 = ( fDivX1 - fStartX ) / fStepSize;
	float fY1 = ( fDivY1 - fStartY ) / fStepSize;

	float fX2 = ( fDivX2 - fDivX1 )  / fStepSize + fX1;
	float fY2 = ( fDivY2 - fDivY1 )  / fStepSize + fY1;
}

void CWorldMapCalcDlg::OnBnClickedOk()
{
	UpdateData( TRUE );
	// 월드맵 좌표 계산...

	FLOAT fDivTmp1 = GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision1 ) ) );
	FLOAT fDivTmp2 = GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision2 ) ) );
	FLOAT fDivisionStartX = GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision1 ) ) );
	FLOAT fDivisionStartZ = GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision1 ) ) );

	FLOAT fStepSize	=  ( fDivTmp2 - fDivTmp1 ) / (FLOAT)( nX2 - nX1 );

	m_fStartX	= fDivisionStartX - (FLOAT)nX1 * fStepSize; 
	m_fStartZ	= fDivisionStartZ - (FLOAT)nY1 * fStepSize;

	m_fEndX		= m_fStartX + 800.0f * fStepSize;
	m_fEndZ		= m_fStartZ + 600.0f * fStepSize;


	// 확인..
	{
		FLOAT	fStepSizeNew = ( m_fEndX - m_fStartX ) / 800.0f;
		INT32	nX2New = (( fDivTmp2 - m_fStartX ) / fStepSizeNew );

		TRACE( "X2 = %d , X2New = %d\n" , nX2 , nX2New );
	}

	OnOK();
}
