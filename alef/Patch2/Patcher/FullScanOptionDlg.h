#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// CFullScanOptionDlg 대화 상자입니다.

class CFullScanOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CFullScanOptionDlg)

public:
	CFullScanOptionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFullScanOptionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FULLSCAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg	VOID	OnBnClickedOk();

	INT				GetType			( VOID )		{	return m_nSelType;	}

	CStatic				m_staticLastPatch;
	CMonthCalCtrl		m_CalendarMin;
	CMonthCalCtrl		m_CalendarMax;

	INT					m_nSelType;

	INT					m_nMaxYear;
	INT					m_nMaxMonth;
	INT					m_nMaxDay;

	INT					m_nMinYear;
	INT					m_nMinMonth;
	INT					m_nMinDay;

public:
	afx_msg void OnBnClickedRadioFullscan1();
public:
	afx_msg void OnBnClickedRadioFullscan2();
public:
	afx_msg void OnBnClickedRadioFullscan3();
	CString m_strLastPatch;
};
