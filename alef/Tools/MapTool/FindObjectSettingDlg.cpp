// FindObjectSettingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "FindObjectSettingDlg.h"


// CFindObjectSettingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFindObjectSettingDlg, CDialog)
CFindObjectSettingDlg::CFindObjectSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindObjectSettingDlg::IDD, pParent)
	, m_nEventID(0)
{
}

CFindObjectSettingDlg::~CFindObjectSettingDlg()
{
}

void CFindObjectSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EVENTID, m_nEventID);
}


BEGIN_MESSAGE_MAP(CFindObjectSettingDlg, CDialog)
END_MESSAGE_MAP()


// CFindObjectSettingDlg 메시지 처리기입니다.
