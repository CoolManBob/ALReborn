// FullScanOptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "FullScanOptionDlg.h"


// CFullScanOptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFullScanOptionDlg, CDialog)

CFullScanOptionDlg::CFullScanOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFullScanOptionDlg::IDD, pParent)
	, m_strLastPatch(_T(""))
{

}

CFullScanOptionDlg::~CFullScanOptionDlg()
{
}

void CFullScanOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LAST_PATCH, m_staticLastPatch);
	DDX_Control(pDX, IDC_MONTHCALENDAR1, m_CalendarMin);
	DDX_Control(pDX, IDC_MONTHCALENDAR2, m_CalendarMax);
	DDX_Text(pDX, IDC_STATIC_LAST_PATCH, m_strLastPatch);
}


BEGIN_MESSAGE_MAP(CFullScanOptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFullScanOptionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_FULLSCAN1, &CFullScanOptionDlg::OnBnClickedRadioFullscan1)
	ON_BN_CLICKED(IDC_RADIO_FULLSCAN2, &CFullScanOptionDlg::OnBnClickedRadioFullscan2)
	ON_BN_CLICKED(IDC_RADIO_FULLSCAN3, &CFullScanOptionDlg::OnBnClickedRadioFullscan3)
END_MESSAGE_MAP()


// CFullScanOptionDlg 메시지 처리기입니다.

void CFullScanOptionDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CTime			MinTime;
	CTime			MaxTime;

	m_CalendarMin.GetCurSel( MinTime );
	m_CalendarMax.GetCurSel( MaxTime );

	m_nMaxYear		=	MaxTime.GetYear();
	m_nMaxMonth		=	MaxTime.GetMonth();
	m_nMaxDay		=	MaxTime.GetDay();

	m_nMinYear		=	MinTime.GetYear();
	m_nMinMonth		=	MinTime.GetMonth();
	m_nMinDay		=	MinTime.GetDay();

	OnOK();
}

void CFullScanOptionDlg::OnBnClickedRadioFullscan1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelType	=	0;
}

void CFullScanOptionDlg::OnBnClickedRadioFullscan2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelType	=	1;
}

void CFullScanOptionDlg::OnBnClickedRadioFullscan3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelType	=	2;
}
