#pragma once

#include "dlglist.h"

class CDlgListEffSet : public CDlgList  
{
public:
	CDlgListEffSet(CWnd* pParent = NULL);

protected:
	// Overrides
	virtual BOOL OnInitDialog();

	// Message
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	INT		bInsItem(DWORD dwEffSetID, LPCTSTR szEffSetTitle, INT nBaseNum, UINT ulLife);
	INT		bReloadAllItem();

private:
	INT		vUpload();
	INT		vInitColum();

private:
	LPEFFSET	m_pEffSet;
	INT			m_nColumn;
	BOOL		m_bSortedByID;
	BOOL		m_bSortedByTitle;
	BOOL		m_bSortedByBaseNum;
	BOOL		m_bSortedByBaseLife;
};