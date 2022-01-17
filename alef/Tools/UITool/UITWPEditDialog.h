#if !defined(AFX_UITWPEDITDIALOG_H__23A2EB2A_91F7_4205_A326_9DC4BC10B512__INCLUDED_)
#define AFX_UITWPEDITDIALOG_H__23A2EB2A_91F7_4205_A326_9DC4BC10B512__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPEditDialog.h : header file
//

#include "UITPropertyPage.h"

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPEditDialog dialog

class UITWPEditDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPEditDialog)

private:
	AcUIEdit *		m_pcsEdit;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPEditDialog();
	~UITWPEditDialog();

// Dialog Data
	//{{AFX_DATA(UITWPEditDialog)
	enum { IDD = IDD_UIT_WP_EDIT };
	BOOL	m_bPassword;
	UINT	m_ulLength;
	BOOL	m_bMultiLine;
	BOOL	m_bReadOnly;
	BOOL	m_bAutoLF;
	BOOL	m_bUseTag;
	BOOL	m_bForHotkey;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPEditDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPEditDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bFilterCase;
	BOOL m_bReleaseEditInputEnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPEDITDIALOG_H__23A2EB2A_91F7_4205_A326_9DC4BC10B512__INCLUDED_)
