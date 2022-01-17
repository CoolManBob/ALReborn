#if !defined(AFX_UITWPBUTTONDIALOG_H__EE3FEBD6_0264_457B_9E75_B1F638CD8CF8__INCLUDED_)
#define AFX_UITWPBUTTONDIALOG_H__EE3FEBD6_0264_457B_9E75_B1F638CD8CF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPButtonDialog.h : header file
//

#include "AgcmUIManager2.h"

#include "UITPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPButtonDialog dialog

class UITWPButtonDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPButtonDialog)

private:
	AcUIButton *	m_pcsButton;
	AgcdUIControl *	m_pcsControl;
	AgcdUI *		m_pcsUI;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPButtonDialog();
	~UITWPButtonDialog();

// Dialog Data
	//{{AFX_DATA(UITWPButtonDialog)
	enum { IDD = IDD_UIT_WP_BUTTON };
	CComboBox	m_csEnableUserData;
	CComboBox	m_csEnableBoolean;
	CComboBox	m_csCheckUserData;
	CComboBox	m_csCheckBoolean;
	CComboBox	m_csDisable;
	CComboBox	m_csClick;
	CComboBox	m_csOnMouse;
	CComboBox	m_csNormal;
	BOOL		m_bPushButton;
	BOOL		m_bStartOnClickStatus;
	long	m_lTextOffsetX;
	long	m_lTextOffsetY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPButtonDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPButtonDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnUITButtonApply();
	afx_msg void OnUITButtonCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPBUTTONDIALOG_H__EE3FEBD6_0264_457B_9E75_B1F638CD8CF8__INCLUDED_)
