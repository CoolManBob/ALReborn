#pragma once

#include "../resource.h"

class CItemListDlg : public CDialog
{
public:
	CItemListDlg(INT32 *plTID, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CItemListDlg)
	enum { IDD = IDD_ITEM_LIST };
	CListBox	m_csItemList;
	//}}AFX_DATA

protected:
	INT32 *m_plTID;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CItemListDlg)
	afx_msg void OnDblclkListItemList();

	DECLARE_MESSAGE_MAP()
};