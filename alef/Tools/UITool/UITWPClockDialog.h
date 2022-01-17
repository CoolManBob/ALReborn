#if !defined(AFX_UITWPCLOCKDIALOG_H__AEAE4CE7_7171_4BEA_8B41_E0A3A519B00D__INCLUDED_)
#define AFX_UITWPCLOCKDIALOG_H__AEAE4CE7_7171_4BEA_8B41_E0A3A519B00D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPClockDialog.h : header file
//

#include "UITPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPClockDialog dialog

class UITWPClockDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPClockDialog)

private:
	AcUIClock *		m_pcsClock;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPClockDialog();
	~UITWPClockDialog();

// Dialog Data
	//{{AFX_DATA(UITWPClockDialog)
	enum { IDD = IDD_UIT_WP_CLOCK };
	CComboBox	m_csCurrentDisplay;
	CComboBox	m_csMaxDisplay;
	CComboBox	m_csImageAlpha;
	CComboBox	m_csCurrentPoint;
	CComboBox	m_csMaxPoint;
	BOOL	m_bCutHead;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPClockDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPClockDialog)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeUITClockMax();
	afx_msg void OnSelchangeUITClockCurrent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPCLOCKDIALOG_H__AEAE4CE7_7171_4BEA_8B41_E0A3A519B00D__INCLUDED_)
