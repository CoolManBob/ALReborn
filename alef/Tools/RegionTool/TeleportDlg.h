#pragma once


// CTeleportDlg 대화 상자입니다.

class CTeleportDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeleportDlg)

public:
	CTeleportDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTeleportDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GOTO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMessage;
	BOOL m_bShow;
	afx_msg void OnBnClickedShowthisboxagain();
};
