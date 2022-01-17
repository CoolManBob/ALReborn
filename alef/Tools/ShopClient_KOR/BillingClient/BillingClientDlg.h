// BillingClientDlg.h : 헤더 파일
//

#pragma once

#include "LibClientSession.h"

// CBillingClientDlg 대화 상자
class CBillingClientDlg : public CDialog
{
// 생성입니다.
public:
	CBillingClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_BILLINGCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	//네트웍 라이브러리 콜백함수
	static void ErrorHandler( DWORD lastError, TCHAR* desc );	
private:
	CLibClientSession m_Client;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButConnect();
	afx_msg void OnBnClickedButLogin();
	afx_msg void OnBnClickedButLogout();
	afx_msg void OnBnClickedButInquire();	
	afx_msg void OnBnClickedButInquireMulti();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButInquirePcpoint();
	//afx_msg void OnBnClickedButInquireDeducttype();
public:
	afx_msg void OnBnClickedButton2();
};
