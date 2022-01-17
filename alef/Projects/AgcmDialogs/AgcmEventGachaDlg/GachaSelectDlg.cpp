// GachaSelectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../resource.h"		// main symbols
#include "AgcmEventGachaDlg.h"

#include "GachaSelectDlg.h"


const char g_strFormat[] = "%d,%s";

// CGachaSelectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGachaSelectDlg, CDialog)

CGachaSelectDlg::CGachaSelectDlg(AgcmEventGachaDlg * pParent , INT32 nIndex )
	: CDialog(CGachaSelectDlg::IDD, NULL) , m_pParentModule( pParent ) , m_nIndex( nIndex ),
	m_pMap ( NULL )
	, m_strSelectedGacha(_T(""))
{

}

CGachaSelectDlg::~CGachaSelectDlg()
{
}

void CGachaSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GACHA, m_listGacha);
	DDX_Text(pDX, IDC_SELECTED_GACHA, m_strSelectedGacha);
}


BEGIN_MESSAGE_MAP(CGachaSelectDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_GACHA, &CGachaSelectDlg::OnLbnSelchangeListGacha)
END_MESSAGE_MAP()


// CGachaSelectDlg 메시지 처리기입니다.

BOOL CGachaSelectDlg::OnInitDialog()
{
	UpdateData( FALSE );

	// 리스트박스에 데이타 추가
	m_pMap = m_pParentModule->m_pcsAgpmEventGacha->GetGachaTypeMap();
	map< INT32	, AgpdGachaType	>::iterator iter;

	for( iter = m_pMap->begin();
		iter != m_pMap->end();
		iter ++ )
	{
		INT32				nGachaIndex	= iter->first	;
		AgpdGachaType *		pGachaType	= &iter->second	;

		CString	str;

		str.Format( g_strFormat , nGachaIndex , pGachaType->strName.c_str() );

		m_listGacha.AddString( str );
	}

	if( m_nIndex )
	{
		AgpdGachaType * pGachaType = m_pParentModule->m_pcsAgpmEventGacha->GetGachaTypeInfo( m_nIndex );

		if( pGachaType )
		{
			m_strSelectedGacha.Format( g_strFormat , m_nIndex , pGachaType->strName.c_str() );
		}
		else
		{
			m_strSelectedGacha.Format( g_strFormat , m_nIndex , "해당인덱스에 가챠 타입이 없어요" );
			m_nIndex = 0;
		}
	}
	else
	{
		m_strSelectedGacha.Format( g_strFormat , m_nIndex , "선택한 가차가 없어요" );
	}

	UpdateData( FALSE );

	return TRUE;
}

// 리스트박스 선택 이벤트 오버라이딩
// On OK 오버라이딩
void CGachaSelectDlg::OnLbnSelchangeListGacha()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData( TRUE );

	int	nIndex = m_listGacha.GetCurSel();
	CString	str;
	m_listGacha.GetText( nIndex , str );

	INT32	nGachaIndex;
	nGachaIndex = atoi( (LPCTSTR) str );

	m_nIndex = nGachaIndex;

	AgpdGachaType * pGachaType = m_pParentModule->m_pcsAgpmEventGacha->GetGachaTypeInfo( nGachaIndex );

	if( pGachaType )
	{
		CString	str;
		m_strSelectedGacha.Format( g_strFormat , m_nIndex , pGachaType->strName.c_str() );
	}
	else
	{
		m_strSelectedGacha.Format( g_strFormat , m_nIndex , "해당인덱스에 가챠 타입이 없어요" );
		m_nIndex = 0;
	}

	UpdateData( FALSE );
}
