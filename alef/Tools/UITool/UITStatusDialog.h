#if !defined(AFX_UITSTATUSDIALOG_H__F362E05F_316E_4A67_B22F_E41BC0A6553B__INCLUDED_)
#define AFX_UITSTATUSDIALOG_H__F362E05F_316E_4A67_B22F_E41BC0A6553B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITStatusDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITStatusDialog dialog

class UITStatusDialog : public CDialog
{
private:
	AcUIBase *			m_pcsBase;
	AgcdUIControl *		m_pcsControl;

	stStatusInfo		m_astStatus[ACUIBASE_STATUS_MAX_NUM];
	AgcdUIUserData *	m_apstUserData[ACUIBASE_STATUS_MAX_NUM];

// Construction
public:
	UITStatusDialog(AcUIBase *pcsBase, AgcdUIControl *pcsControl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UITStatusDialog)
	enum { IDD = IDD_UIT_STATUS };
	CComboBox	m_csUserDataList;
	CComboBox	m_csTextureList;
	CListCtrl	m_csStatusList;
	CString	m_strName;
	long	m_lX;
	long	m_lY;
	long	m_lW;
	long	m_lH;
	BOOL	m_bVisible;
	BYTE	m_cRed;
	BYTE	m_cGreen;
	BYTE	m_cBlue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITStatusDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITStatusDialog)
	afx_msg void OnUITAddStatus();
	afx_msg void OnUITUpdateStatus();
	afx_msg void OnUITDeleteStatus();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedUITStatusList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUITSetDefaultStatus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL RefreshData();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITSTATUSDIALOG_H__F362E05F_316E_4A67_B22F_E41BC0A6553B__INCLUDED_)
