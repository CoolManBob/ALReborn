#pragma once

class AgcmCustomizeRender;

// CCustomizePreviewDlg dialog
class CCustomizePreviewDlg : public CDialog
{
	DECLARE_DYNAMIC(CCustomizePreviewDlg)

public:
	enum {
		KEY_UP,
		KEY_DOWN,
		KEY_A,
		KEY_Z,
		KEY_MAX,
	};

	BOOL		Create(CWnd* pParentWnd = NULL);
	void		Start();
	void		End();

	void		OnIdle();

	void		KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void		KeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
	AgcmCustomizeRender*	m_pAgcmCustomizeRender;
	bool					m_bKey[KEY_MAX];
	float					m_fCameraY, m_fCameraZ;
	float					m_fTargetY;

public:
	CCustomizePreviewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCustomizePreviewDlg();

// Dialog Data
	enum { IDD = IDD_CUSTOMIZE_PREVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnClose();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCustomizePreviewPreview();
	afx_msg void OnBnClickedCustomizePreviewSetting();
	afx_msg void OnBnClickedCustomizePreviewSetnear();
	afx_msg void OnBnClickedCustomizePreviewSetfar();

	DECLARE_MESSAGE_MAP()
};
