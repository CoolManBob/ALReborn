#pragma once
#include "afxwin.h"


// 2006.11.30. steeple
// 해당 캐릭터의 현상수배 정보를 관리


// AlefAdminWC 폼 뷰입니다.

class AlefAdminWC : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminWC)

protected:
	AlefAdminWC();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminWC();

public:
	enum { IDD = IDD_WC };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnInitialUpdate();

	void ClearContent();
	BOOL OnReceiveWCInfo(stAgpdAdminCharDataMoney* pstWantedCriminal);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CXTButton m_csRefreshBtn;
	CXTButton m_csDeleteBtn;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
public:
	afx_msg void OnBnClickedBWcRefresh();
public:
	afx_msg void OnBnClickedBWcDelete();
};


