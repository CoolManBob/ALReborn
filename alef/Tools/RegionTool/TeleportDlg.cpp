// TeleportDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "TeleportDlg.h"
#include ".\teleportdlg.h"


// CTeleportDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTeleportDlg, CDialog)
CTeleportDlg::CTeleportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeleportDlg::IDD, pParent)
	, m_strMessage(_T(""))
	, m_bShow(TRUE)
{
}

CTeleportDlg::~CTeleportDlg()
{
}

void CTeleportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DISPLAY, m_strMessage);
	DDX_Check(pDX, IDC_SHOWTHISBOXAGAIN, m_bShow);
}


BEGIN_MESSAGE_MAP(CTeleportDlg, CDialog)
	ON_BN_CLICKED(IDC_SHOWTHISBOXAGAIN, OnBnClickedShowthisboxagain)
END_MESSAGE_MAP()


// CTeleportDlg 메시지 처리기입니다.

void CTeleportDlg::OnBnClickedShowthisboxagain()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_bShow = FALSE;
	OnOK();
}
