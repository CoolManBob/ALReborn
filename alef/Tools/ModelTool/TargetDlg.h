#pragma once

class CTargetDlg : public CDialog
{
	DECLARE_DYNAMIC(CTargetDlg)

public:
	CTargetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTargetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TARGET_OPTION };
	long	m_lTargetTID;
	long	m_lTargetNum;
	float	m_fOffsetX;
	float	m_fOffsetY;
	float	m_fOffsetZ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
};
