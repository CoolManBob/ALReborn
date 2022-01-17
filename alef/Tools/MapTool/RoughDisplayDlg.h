#pragma once


// CRoughDisplayDlg 대화 상자입니다.

class CRoughDisplayDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoughDisplayDlg)

public:
	CRoughDisplayDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRoughDisplayDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ROUGHLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedShow();
	afx_msg void OnBnClickedHide();
	float m_fSrcX;
	float m_fSrcZ;
	float m_fSrcHeight;
	UINT m_uDiv;
	int m_nRadius;
	float m_fCloudHeight;
	afx_msg void OnBnClickedSetDefault();
	float m_fDstX;
	float m_fDstZ;
};
