#if !defined(AFX_UITEVENTMAPDIALOG_H__7011EA0C_2BE8_4A92_98C0_982A6EACA0FF__INCLUDED_)
#define AFX_UITEVENTMAPDIALOG_H__7011EA0C_2BE8_4A92_98C0_982A6EACA0FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITEventMapDialog.h : header file
//

#include "AgcdUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITEventMapDialog dialog

class UITEventMapDialog : public CDialog
{
private:
	AgcdUI *			m_pcsUI;
	AgcdUIEventMap *	m_pstEventMap;

// Construction
public:
	UITEventMapDialog(AgcdUI *pcsUI, CWnd* pParent = NULL);   // standard constructor
	VOID RefreshActionList();

// Dialog Data
	//{{AFX_DATA(UITEventMapDialog)
	enum { IDD = IDD_UIT_EVENT_MAPS };
	CListCtrl	m_csActionList;
	CListCtrl	m_csEventList;
	CString	m_strEventName;
	CString	m_strEventDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITEventMapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITEventMapDialog)
	afx_msg void OnUITAddAction();
	afx_msg void OnUITEditAction();
	afx_msg void OnUITDeleteAction();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedUITEventList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITEVENTMAPDIALOG_H__7011EA0C_2BE8_4A92_98C0_982A6EACA0FF__INCLUDED_)
