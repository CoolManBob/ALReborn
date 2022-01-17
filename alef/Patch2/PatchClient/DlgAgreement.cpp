// DlgAgreement.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "DlgAgreement.h"
#include "PatchClientDlg.h"


// CDlgAgreement 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgAgreement, CDialog)

CDlgAgreement::CDlgAgreement(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAgreement::IDD, pParent)
{
	m_bIsChecked = TRUE;
}

CDlgAgreement::~CDlgAgreement()
{
}

void CDlgAgreement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAgreement, CDialog)
	ON_BN_CLICKED(IDB_AGREE, &CDlgAgreement::OnBnClickedAgree)
	ON_BN_CLICKED(IDC_CHECK_AGREE, &CDlgAgreement::OnBnClickedCheckAgree)
END_MESSAGE_MAP()


// CDlgAgreement 메시지 처리기입니다.
BOOL CDlgAgreement::OnInitDialog( void )
{
	// 문자열 세팅
	CWnd* pTextMsg = GetDlgItem( IDC_EDIT_AGREEMSG );
	if( pTextMsg )
	{
		CString strText;
		strText.LoadString( IDS_PVPAGREE_MSG );

		pTextMsg->SetWindowText( strText.GetBuffer() );
	}

	pTextMsg = GetDlgItem( IDC_CHECK_AGREE );
	if( pTextMsg )
	{
		CString strText;
		strText.LoadString( IDS_PVPAGREE_CHECK );

		pTextMsg->SetWindowText( strText.GetBuffer() );
	}

	// 기본적으로 체크상태로 있도록..
	CButton* pBtn = ( CButton* )GetDlgItem( IDC_CHECK_AGREE );
	if( pBtn )
	{
		pBtn->SetCheck( BST_CHECKED );
	}

	return CDialog::OnInitDialog();
}

void CDlgAgreement::OnBnClickedAgree()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 체크버튼에 동의체크를 안했으면 종료 아니면 걍 넘어간다.
	if( m_bIsChecked )
	{
		EndDialog( 0 );
	}
	else
	{
		EndDialog( 1 );
		_exit( 0 );
	}
}

void CDlgAgreement::OnBnClickedCheckAgree()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = ( CButton* )GetDlgItem( IDB_AGREE );
	if( !pButton ) return;

	m_bIsChecked = IsDlgButtonChecked( IDC_CHECK_AGREE ) ? TRUE : FALSE;
	int nID = m_bIsChecked ? IDS_PVPAGREE_OK : IDS_PVPAGREE_NO;

	CString strText;
	strText.LoadString( nID );

	pButton->SetWindowText( strText.GetBuffer() );
}
