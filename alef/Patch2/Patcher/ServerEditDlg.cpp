// ServerEditDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "ServerEditDlg.h"


// CServerEditDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerEditDlg, CDialog)

CServerEditDlg::CServerEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerEditDlg::IDD, pParent)
	, m_NameData(_T(""))
	, m_IPData(_T(""))
{

}

CServerEditDlg::~CServerEditDlg()
{
}

void CServerEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT	, m_NameData);
	DDX_Text(pDX, IDC_IP_EDIT	, m_IPData	);
}


BEGIN_MESSAGE_MAP(CServerEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CServerEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CServerEditDlg 메시지 처리기입니다.



void CServerEditDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );
	UpdateData( FALSE );


	OnOK();
}
