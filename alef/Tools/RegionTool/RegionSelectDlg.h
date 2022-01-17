#pragma once
#include "afxcmn.h"
#include "RTMenuRegion.h"


// CRegionSelectDlg 대화 상자입니다.

class CRegionSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegionSelectDlg)

public:
	CRegionSelectDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRegionSelectDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_REGIONSELECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrRegionSelectList;
	int		  m_nSelectedItem;
	CString	  m_strSelectParentIndex;

public:
	BOOL UpdateList();

public:
	afx_msg void OnBnClickedOk();

public:
	afx_msg void OnLvnItemchangedRegionselectList(NMHDR *pNMHDR, LRESULT *pResult);
};
