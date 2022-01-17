#pragma once


// CFolderPointingDlg 대화 상자입니다.

class CFolderPointingDlg : public CDialog
{
	DECLARE_DYNAMIC(CFolderPointingDlg)

public:
	CFolderPointingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFolderPointingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FOLDER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CString m_strRealTimeFolder;

	afx_msg void OnBnClickedSearchrealtime();
};
