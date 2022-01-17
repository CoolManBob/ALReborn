#if !defined(AFX_UITWPUIDIALOG_H__76B6250F_4946_4382_BC13_F216EBEEB50E__INCLUDED_)
#define AFX_UITWPUIDIALOG_H__76B6250F_4946_4382_BC13_F216EBEEB50E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPUIDialog.h : header file
//

#include "AgcmUIManager2.h"
#include "UITPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPUIDialog dialog

class UITWPUIDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPUIDialog)

private:
	AgcdUI *	m_pcsUI;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPUIDialog();
	~UITWPUIDialog();

// Dialog Data
	//{{AFX_DATA(UITWPUIDialog)
	enum { IDD = IDD_UIT_WP_UI };
	CComboBox	m_csKillFocus;
	CComboBox	m_csSetFocus;
	CComboBox	m_csUIType;
	CComboBox	m_csCoordSystems;
	CComboBox	m_csParentUI;
	BOOL	m_bUseGroupPosition;
	BOOL	m_bMainUI;
	BOOL	m_bInitStatus;
	BOOL	m_bMode_1024;
	BOOL	m_bMode_1280;
	BOOL	m_bMode_1600;
	BOOL	m_bAutoClose;
	BOOL	m_bEventUI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPUIDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPUIDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bTransparentUI;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPUIDIALOG_H__76B6250F_4946_4382_BC13_F216EBEEB50E__INCLUDED_)
