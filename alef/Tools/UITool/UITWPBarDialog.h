#if !defined(AFX_UITWPBARDIALOG_H__AEAE4CE7_7171_4BEA_8B41_E0A3A519B00D__INCLUDED_)
#define AFX_UITWPBARDIALOG_H__AEAE4CE7_7171_4BEA_8B41_E0A3A519B00D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITWPBarDialog.h : header file
//

#include "UITPropertyPage.h"

/////////////////////////////////////////////////////////////////////////////
// UITWPBarDialog dialog

class UITWPBarDialog : public UITPropertyPage
{
	DECLARE_DYNCREATE(UITWPBarDialog)

private:
	AcUIBar *		m_pcsBar;
	AgcdUI *		m_pcsUI;
	AgcdUIControl *	m_pcsControl;

// Construction
public:
	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();

	BOOL OnSetActive();

	UITWPBarDialog();
	~UITWPBarDialog();

// Dialog Data
	//{{AFX_DATA(UITWPBarDialog)
	enum { IDD = IDD_UIT_WP_BAR };
	CComboBox	m_csCurrentDisplay;
	CComboBox	m_csMaxDisplay;
	CComboBox	m_csImageBody;
	CComboBox	m_csImageEdge;
	CComboBox	m_csCurrentPoint;
	CComboBox	m_csMaxPoint;
	BOOL	m_bVertical;
	BOOL	m_bCutHead;
	BOOL	m_bSmooth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(UITWPBarDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(UITWPBarDialog)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeUITBarMax();
	afx_msg void OnSelchangeUITBarCurrent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITWPBARDIALOG_H__AEAE4CE7_7171_4BEA_8B41_E0A3A519B00D__INCLUDED_)
