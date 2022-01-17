// AlefAdminOption.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminOption.h"
#include ".\alefadminoption.h"


// AlefAdminOption 대화 상자입니다.

IMPLEMENT_DYNAMIC(AlefAdminOption, CDialog)
AlefAdminOption::AlefAdminOption(CWnd* pParent /*=NULL*/)
	: CDialog(AlefAdminOption::IDD, pParent)
{
	m_bUseAlpha = m_bUseAlpha2 = FALSE;
	m_lAlpha = m_lAlpha2 = 70;

	m_bSaveWhisper = TRUE;
}

AlefAdminOption::~AlefAdminOption()
{
}

void AlefAdminOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SL_ALPHA, m_csAlphaSlider);
	DDX_Control(pDX, IDC_CH_USE_LAYERED, m_csAlphaBtn);
	DDX_Control(pDX, IDC_CH_SAVE_WHISPER, m_csSaveWhisperCheckBox);
}


BEGIN_MESSAGE_MAP(AlefAdminOption, CDialog)
	ON_BN_CLICKED(IDC_CH_USE_LAYERED, OnBnClickedChUseLayered)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// AlefAdminOption 메시지 처리기입니다.

BOOL AlefAdminOption::OnInitDialog()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CDialog::OnInitDialog();

	// Slider 초기화
	m_csAlphaSlider.SetRange(5, 95);
	m_csAlphaSlider.SetPos(m_lAlpha);
	m_csAlphaSlider.SetTicFreq(1);

	CHAR szTmp[8];
	sprintf(szTmp, "%d%%", m_lAlpha);
	SetDlgItemText(IDC_S_ALPHA, szTmp);

	m_csAlphaBtn.SetCheck(m_bUseAlpha ? BST_CHECKED : BST_UNCHECKED);
	OnBnClickedChUseLayered();

	m_csSaveWhisperCheckBox.SetCheck(m_bSaveWhisper ? BST_CHECKED : BST_UNCHECKED);

	return FALSE;
}

void AlefAdminOption::OnBnClickedChUseLayered()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UINT uState = m_csAlphaBtn.GetCheck();
	if(uState == BST_CHECKED)
	{
		m_bUseAlpha2 = TRUE;
		m_csAlphaSlider.EnableWindow();
	}
	else
	{
		m_bUseAlpha2 = FALSE;
		m_csAlphaSlider.EnableWindow(FALSE);
	}
}

void AlefAdminOption::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	int iPos = m_csAlphaSlider.GetPos();
	m_lAlpha2 = iPos;

	CHAR szTmp[8];
	sprintf(szTmp, "%d%%", m_lAlpha2);
	SetDlgItemText(IDC_S_ALPHA, szTmp);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void AlefAdminOption::OnBnClickedOk()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_bUseAlpha = m_bUseAlpha2;
	m_lAlpha = m_lAlpha2;
	OnOK();
}
