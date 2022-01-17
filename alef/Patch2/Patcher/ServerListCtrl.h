#pragma once


// CServerListCtrl

class CServerListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CServerListCtrl)

public:
	CServerListCtrl();
	virtual ~CServerListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


