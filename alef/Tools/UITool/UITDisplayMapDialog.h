#if !defined(AFX_UITDISPLAYMAPDIALOG_H__DEAFC59E_8502_4D2F_B414_84BB67185869__INCLUDED_)
#define AFX_UITDISPLAYMAPDIALOG_H__DEAFC59E_8502_4D2F_B414_84BB67185869__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITDisplayMapDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITDisplayMapDialog dialog

class UITDisplayMapDialog : public CDialog
{
private:
	AgcdUIControl *		m_pcsControl;

// Construction
public:
	UITDisplayMapDialog(AgcdUIControl *pcsControl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UITDisplayMapDialog)
	enum { IDD = IDD_UIT_DISPLAY_MAP };
	CComboBox	m_csFontVAlign;
	CComboBox	m_csFontHAlign;
	CComboBox	m_csFontList;
	CComboBox	m_csUserDataList;
	CComboBox	m_csVariableList;
	CEdit	m_csCaption;
	CComboBox	m_csDisplayType;
	CString	m_strCaption;
	BYTE	m_cRed;
	BYTE	m_cGreen;
	BYTE	m_cBlue;
	float	m_fScale;
	BOOL	m_bShadow;
	BYTE	m_cAlpha;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITDisplayMapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITDisplayMapDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnUITUpdateDisplayMap();
	afx_msg void OnSelchangeUITDisplaymapType();
	afx_msg void OnSelchangeUITUserDataList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bNumberComma;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITDISPLAYMAPDIALOG_H__DEAFC59E_8502_4D2F_B414_84BB67185869__INCLUDED_)
