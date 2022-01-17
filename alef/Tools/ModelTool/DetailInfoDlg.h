#pragma once

typedef BOOL (*DetailInfoDlgCallback)(PVOID pvClass, PVOID pvCustClass);

class CDetailInfoDlg : public CDialog
{
public:
	CDetailInfoDlg(DetailInfoDlgCallback pfCallback, PVOID pvCustClass = NULL, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CDetailInfoDlg)
	enum { IDD = IDD_DETAIL_INFO };
	CListCtrl	m_csDetailInfo;
	//}}AFX_DATA

protected:
	PVOID					m_pvCustClass;
	DetailInfoDlgCallback	m_pfCallback;
	INT16					m_nIndex;

public:
	VOID _Reset();
	VOID _InsertItem(CHAR *szMember, CHAR *szStatus);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};