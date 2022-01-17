// WorldMapEditDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"
#include "WorldMapEditDlg.h"
#include "ItemSelectDlg.h"
#include "WorldMapCalcDlg.h"
#include "WorldPosExportDlg.h"

// CWorldMapEditDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CWorldMapEditDlg, CDialog)
CWorldMapEditDlg::CWorldMapEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldMapEditDlg::IDD, pParent)
	, m_strComment(_T(""))
	, m_fEndX(0)
	, m_fEndZ(0)
	, m_nIndex(0)
	, m_fStartX(0)
	, m_fStartZ(0)
	, m_strItemID(_T(""))
{
	m_pstWorldMap	= NULL;
}

CWorldMapEditDlg::~CWorldMapEditDlg()
{
}

void CWorldMapEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COMMENT, m_strComment);
	DDX_Text(pDX, IDC_ENDX, m_fEndX);
	DDX_Text(pDX, IDC_ENDZ, m_fEndZ);
	DDX_Text(pDX, IDC_INDEX, m_nIndex);
	DDX_Text(pDX, IDC_STARTX, m_fStartX);
	DDX_Text(pDX, IDC_STARTZ, m_fStartZ);
	DDX_Text(pDX, IDC_ITEMID, m_strItemID);
}


BEGIN_MESSAGE_MAP(CWorldMapEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SELECT_ITEM_ID, OnBnClickedSelectItemId)
	ON_BN_CLICKED(IDC_CALCCOODINATE, OnBnClickedCalccoodinate)
	ON_BN_CLICKED(IDC_CALCCOODINATE2, &CWorldMapEditDlg::OnBnClickedCalccoodinate2)
END_MESSAGE_MAP()


// CWorldMapEditDlg 메시지 처리기입니다.

BOOL CWorldMapEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT( NULL != m_pstWorldMap );
	if( m_pstWorldMap )
	{
		m_nIndex		= m_pstWorldMap->nMID		;
		m_strComment	= m_pstWorldMap->strComment	;
		m_fStartX		= m_pstWorldMap->xStart		;
		m_fStartZ		= m_pstWorldMap->zStart		;
		m_fEndX			= m_pstWorldMap->xEnd		;
		m_fEndZ			= m_pstWorldMap->zEnd		;

		if( m_pstWorldMap->nMapItemID == -1)
		{
			m_strItemID.Format( "%d , 아이템 필요없음" , m_pstWorldMap->nMapItemID );
		}
		else
		{
			AgpdItemTemplate * pItemTemplate = g_pcsAgpmItem->GetItemTemplate( m_pstWorldMap->nMapItemID );
			if( pItemTemplate )
			{
				m_strItemID.Format( "%04d , %s" , m_pstWorldMap->nMapItemID , pItemTemplate->m_szName );
			}
			else
			{
				m_strItemID.Format( "%04d , 아이템없음에러!" , m_pstWorldMap->nMapItemID );
			}
		}

		UpdateData( FALSE );
	}
	else
	{
		MessageBox( "-_-???????????" );
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CWorldMapEditDlg::OnBnClickedOk()
{
	UpdateData( TRUE );

	if( m_pstWorldMap )
	{
		strncpy( m_pstWorldMap->strComment , (LPCTSTR) m_strComment , WORLDMAP_COMMENT_LENGTH - 1 ); 
		m_pstWorldMap->xStart		= m_fStartX		;
		m_pstWorldMap->zStart		= m_fStartZ		;
		m_pstWorldMap->xEnd			= m_fEndX		;
		m_pstWorldMap->zEnd			= m_fEndZ		;
		m_pstWorldMap->nMapItemID	= atoi( (LPCTSTR) m_strItemID );
	}
	
	OnOK();
}

void CWorldMapEditDlg::OnBnClickedSelectItemId()
{
	// 아이템 선택창 구현 여기

	CItemSelectDlg	dlg;

	if( IDOK == dlg.DoModal() )
	{
		UpdateData( TRUE );

		if( dlg.m_nItemID == -1)
		{
			m_strItemID.Format( "%d , 아이템 필요없음" , dlg.m_nItemID );
		}
		else
		{
			AgpdItemTemplate * pItemTemplate = g_pcsAgpmItem->GetItemTemplate( dlg.m_nItemID );
			if( pItemTemplate )
			{
				m_strItemID.Format( "%04d , %s" , dlg.m_nItemID , pItemTemplate->m_szName );
			}
			else
			{
				m_strItemID.Format( "%04d , 아이템없음에러!" , dlg.m_nItemID );
			}
		}
		UpdateData( FALSE );
	}
}

void CWorldMapEditDlg::OnBnClickedCalccoodinate()
{
	CWorldMapCalcDlg	dlg;

	if( IDOK == dlg.DoModal() )
	{
		// 갱신!
		UpdateData( TRUE );

		m_fStartX	= dlg.m_fStartX	;
		m_fStartZ	= dlg.m_fStartZ	;
		m_fEndX		= dlg.m_fEndX	;
		m_fEndZ		= dlg.m_fEndZ	;

		UpdateData( FALSE );
	}
}

void CWorldMapEditDlg::OnBnClickedCalccoodinate2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWorldPosExportDlg		dlg;

	dlg.DoModal();

}
