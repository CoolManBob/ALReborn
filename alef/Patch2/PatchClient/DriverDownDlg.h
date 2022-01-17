// DriverDownDlg 대화 상자입니다.

#include "afxwin.h"
#include "KbcButton.h"
#include "afxcmn.h"

class DriverDownDlg : public CDialog
{
	DECLARE_DYNAMIC(DriverDownDlg)

public:
	DriverDownDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DriverDownDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DRIVER_DOWN_DLG };

    CFont						m_cDrawFont;

	CPoint						m_cOldPoint;

	CBitmap						m_cBMPBackground;

	CKbcButton					m_cKbcOKButton;
	CKbcButton					m_cKbcNOButton;

	BOOL						m_bSelectResult;

	BOOL						m_bDownloadSuccess;
	CString						m_strDriverURL;
	CString						m_strFileName;

	BOOL						AutoDownloadAndExecDriver();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDriverOk();
	afx_msg void OnBnClickedButtonDriverNo();
	afx_msg void OnPaint();
	CProgressCtrl m_ctrlProgress;
	afx_msg void OnBnClickedCheckDonotusecheckdriver();
};
