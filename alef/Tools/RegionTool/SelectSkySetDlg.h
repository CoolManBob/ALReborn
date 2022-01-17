#pragma once
#include "afxwin.h"


// CSelectSkySetDlg 대화 상자입니다.

class CSelectSkySetDlg : public CDialog
{
	DECLARE_DYNCREATE(CSelectSkySetDlg)

public:
	CSelectSkySetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSelectSkySetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SELECTSKYSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnDblclkList();

public:
	int			m_nItemID;
	CListBox	m_ctlList;
};
