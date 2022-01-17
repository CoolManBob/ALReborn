#pragma once


// CFindObjectSettingDlg 대화 상자입니다.

class CFindObjectSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindObjectSettingDlg)

public:
	CFindObjectSettingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFindObjectSettingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FINDOBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nEventID;
};
