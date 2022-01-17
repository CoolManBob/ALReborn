#if !defined(AFX_UITWPMAPDIALOG_H__648DBE55_4708_40FE_855F_6412638E1031__INCLUDED_)
#define AFX_UITWPMAPDIALOG_H__648DBE55_4708_40FE_855F_6412638E1031__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPMapDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPMapDialog dialog

class UITWPMapDialog : public CPropertyPage
{
	DECLARE_DYNCREATE(UITWPMapDialog)

private:
	AcUIBase *	m_pcsBase;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPMapDialog();
	~UITWPMapDialog();

// Dialog Data
	//{{AFX_DATA(UITWPMapDialog)
	enum { IDD = IDD_UIT_WP_MAP_DIALOG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPMapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPMapDialog)
	afx_msg void OnUITEditMessagemap();
	afx_msg void OnUITEditDisplay();
	afx_msg void OnUITSetHandler();
	afx_msg void OnUITEditStatus();
	afx_msg void OnUITEditUserData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPMAPDIALOG_H__648DBE55_4708_40FE_855F_6412638E1031__INCLUDED_)
