#if !defined(AFX_UITACTIONDIALOG_H__61CCE852_367A_4DFE_88A5_69CD898C8651__INCLUDED_)
#define AFX_UITACTIONDIALOG_H__61CCE852_367A_4DFE_88A5_69CD898C8651__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITActionDialog.h : header file
//

#include "AgcmUIManager2.h"

/////////////////////////////////////////////////////////////////////////////
// UITActionDialog dialog

class UITActionDialog : public CDialog
{
private:
	AgcdUI *			m_pcsUI;
	AgcdUIAction *		m_pstAction;

	BOOL				m_bUserChangeActionType;

	CComboBox *			m_apcsArguments[5];

// Construction
public:
	UITActionDialog(AgcdUI *pcsUI, AgcdUIAction *pstAction, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(UITActionDialog)
	enum { IDD = IDD_UIT_ACTION };
	CComboBox	m_csUIAttach;
	CEdit	m_csPositionY;
	CEdit	m_csPositionX;
	CButton	m_csUISmooth;
	CEdit	m_csSound;
	CComboBox	m_csDisplayList;
	CComboBox	m_csUserDataList;
	CEdit	m_csMessage;
	CEdit	m_csMessageColorR;
	CEdit	m_csMessageColorG;
	CEdit	m_csMessageColorB;
	CEdit	m_csMessageColorA;
	CComboBox	m_csMessageType;
	CComboBox	m_csStatusTarget3;
	CComboBox	m_csStatusTarget2;
	CComboBox	m_csStatusCurrent3;
	CComboBox	m_csStatusCurrent2;
	CComboBox	m_csStatusTarget1;
	CButton	m_csUIGroupAction;
	CButton	m_csSaveCurrentStatus;
	CComboBox	m_csArguments5;
	CComboBox	m_csArguments4;
	CComboBox	m_csArguments3;
	CComboBox	m_csArguments2;
	CComboBox	m_csArguments1;
	CComboBox	m_csFunctionList;
	CComboBox	m_csStatusCurrent1;
	CComboBox	m_csControlList;
	CComboBox	m_csUITypeList;
	CComboBox	m_csUIList;
	CComboBox	m_csActionTypeList;
	CString	m_strArgDescription;
	BOOL	m_bSaveCurrentStatus;
	BOOL	m_bGroupAction;
	CString	m_strMessage;
	CString	m_strSound;
	BOOL	m_bSmooth;
	long	m_lPosX;
	long	m_lPosY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(UITActionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(UITActionDialog)
	afx_msg void OnSelchangeUITActionType();
	afx_msg void OnSelchangeUITActionControl();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeUITFunctions();
	afx_msg void OnSelchangeUitActionMsType();
	afx_msg void OnUITActionAddVariable();
	afx_msg void OnSelchangeUITActionUDList();
	afx_msg void OnUITSoundBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITACTIONDIALOG_H__61CCE852_367A_4DFE_88A5_69CD898C8651__INCLUDED_)
