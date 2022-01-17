#if !defined(AFX_AGCMAISCREAMEDITDLG_H__EE5500A3_B92C_411A_BAAE_C8EB214247DE__INCLUDED_)
#define AFX_AGCMAISCREAMEDITDLG_H__EE5500A3_B92C_411A_BAAE_C8EB214247DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAIScreamEditDlg.h : header file
//

#include "AgpmCharacter.h"
#include "AgpmAI.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAIScreamEditDlg dialog

class AgcmAIScreamEditDlg : public CDialog
{
// Construction
public:
	AgcmAIScreamEditDlg(CWnd* pParent = NULL);   // standard constructor

	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpdAITemplate		*m_pstTemplate;


// Dialog Data
	//{{AFX_DATA(AgcmAIScreamEditDlg)
	enum { IDD = IDD_DIALOG_AI_YELL };
	CComboBox	m_cHelpScreamCharTypeCombo;
	CListBox	m_cHelpScreamCharTypeList;
	CEdit	m_cHelpScreamEdit3;
	CEdit	m_cHelpScreamEdit2;
	CEdit	m_cHelpScreamEdit1;
	CEdit	m_cFearScreamEdit3;
	CEdit	m_cFearScreamEdit2;
	CEdit	m_cFearScreamEdit1;
	BOOL	m_cHelpScreamAllCharTypeCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAIScreamEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAIScreamEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAiHelpScreamCharAddButton();
	afx_msg void OnAiHelpScreamCharRemoveButton();
	afx_msg void OnAiHelpScreamAllCharTypeCheck();
	afx_msg void OnAiScreamOkButton();
	afx_msg void OnAiScreamCancelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAISCREAMEDITDLG_H__EE5500A3_B92C_411A_BAAE_C8EB214247DE__INCLUDED_)
