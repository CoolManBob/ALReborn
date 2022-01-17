// TargetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelTool.h"
#include "TargetDlg.h"
#include ".\targetdlg.h"


// CTargetDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTargetDlg, CDialog)
CTargetDlg::CTargetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTargetDlg::IDD, pParent)
	, m_lTargetTID(0)
	, m_lTargetNum(0)
	, m_fOffsetX(0)
	, m_fOffsetY(0)
	, m_fOffsetZ(0)
{
}

CTargetDlg::~CTargetDlg()
{
}

void CTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TARGET_TID, m_lTargetTID);
	DDX_Text(pDX, IDC_TARGET_NUM, m_lTargetNum);
	DDX_Text(pDX, IDC_TARGET_OFFSET_X, m_fOffsetX);
	DDX_Text(pDX, IDC_TARGET_OFFSET_Y, m_fOffsetY);
	DDX_Text(pDX, IDC_TARGET_OFFSET_Z, m_fOffsetZ);
}

BEGIN_MESSAGE_MAP(CTargetDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CTargetDlg::OnBnClickedOk()
{
	UpdateData();

	OnOK();
}
