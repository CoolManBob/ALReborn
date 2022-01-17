#pragma once
#include "afxwin.h"


// CItemSelectDlg 대화 상자입니다.

class CItemSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CItemSelectDlg)

public:
	CItemSelectDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CItemSelectDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMSELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnDblclkList();
	afx_msg void OnBnClickedNoneeditem();

public:
	CListBox	m_ctlList;
	int			m_nItemID;
	
	
};
