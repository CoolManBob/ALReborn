#pragma once

#include "Resource.h"
#include "afxwin.h"


#define MAX_NOTICE_STRING				240
#define MAX_NOTICE_TEXT_LIST			20

class CNoticeItem
{
public:
	INT32 m_lDelay;
	DWORD m_dwColor;

	CString m_szNotice;
};

typedef enum _ePlayStatus
{
	NOTICE_STOP = 0,
	NOTICE_PLAY,
	NOTICE_PAUSE,
} ePlayStatus;



// AlefAdminNotice 폼 뷰입니다.

class AlefAdminNotice : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminNotice)

public:
	vector<CString> m_vectorNoticeFile;
	vector<CNoticeItem> m_vectorNoticeItem;

	INT32 m_lCurrentIndex;
	DWORD m_dwLastSendNoticeTick;

	ePlayStatus m_ePlayStatus;

protected:
	AlefAdminNotice();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminNotice();

public:
	enum { IDD = IDD_NOTICE };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void OnInitialUpdate();

	BOOL InitControls();

	BOOL SendNotice(LPCTSTR szMessage);

	BOOL LoadFile(LPCTSTR szPathName);
	BOOL CheckSetNoticeItem(CString& szLine, INT32* plDelay, DWORD* pdwColor);
	BOOL SaveFile(LPCTSTR szPathName);
	
	BOOL InitNotice();
	BOOL ProcessNotice();
	BOOL SetCurNotice();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CEdit m_csNoticeEdit;
	CEdit m_csNoticeTextEdit;
	CXTButton m_csSendBtn;
	CXTButton m_csPlayBtn;
	CXTButton m_csPauseBtn;
	CXTButton m_csStopBtn;
	CXTButton m_csHelpBtn;
	CXTButton m_csSaveBtn;
	CXTButton m_csSaveAsBtn;
	CXTButton m_csLoadBtn;
	CXTButton m_csDeleteBtn;

	CListBox m_csNoticeTextList;

	afx_msg void OnBnClickedBSend();
	afx_msg void OnBnClickedBLoad();
	afx_msg void OnBnClickedBDelete();
	afx_msg void OnLbnDblclkLNotice();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBPlay();
	afx_msg void OnBnClickedBPause();
	afx_msg void OnBnClickedBStop();
	afx_msg void OnBnClickedBSave();
	afx_msg void OnBnClickedBSaveAs();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


