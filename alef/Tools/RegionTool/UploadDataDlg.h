#pragma once


// CUploadDataDlg 대화 상자입니다.

class CUploadDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CUploadDataDlg)

public:
	CUploadDataDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUploadDataDlg();


	enum { IDD = IDD_UPLOAD_DATA };

	CString m_strTitle;
	INT32	DoModal( BOOL bUpload );

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAI2Template;
	BOOL m_bArtist;
	BOOL m_bCharacterTemplate;
	BOOL m_bDesign;
	BOOL m_bObjectTemplate;
	BOOL m_bRegionTool;
	BOOL m_bSkySet;
	BOOL m_bSpawnGroup;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
