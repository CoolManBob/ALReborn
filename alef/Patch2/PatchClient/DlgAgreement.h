#pragma once


// CDlgAgreement 대화 상자입니다.

class CDlgAgreement : public CDialog
{
	DECLARE_DYNAMIC(CDlgAgreement)

private :
	BOOL									m_bIsChecked;

public:
	CDlgAgreement(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgAgreement();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_PVP_AGREEMENT };

public :
	virtual BOOL OnInitDialog( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAgree();
public:
	afx_msg void OnBnClickedCheckAgree();

public :
	BOOL IsChecked( void ) { return m_bIsChecked; }
};
