#if !defined(AFX_AgcmAICharItemDialog_H__9F1980A4_50C6_4966_B79F_61738C0B3501__INCLUDED_)
#define AFX_AgcmAICharItemDialog_H__9F1980A4_50C6_4966_B79F_61738C0B3501__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmAICharItemDialog.h : header file
//

#include "AgpmItem.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmAICharItemDialog dialog

class AgcmAICharItemDialog : public CDialog
{
private:
	AgpmItem *			m_pcsAgpmItem;

public:
	AgpdItemTemplate *	m_pcsItemTemplate;
	VOID SetModule(AgpmItem *pcsAgpmItem);

// Construction
public:
	AgcmAICharItemDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AgcmAICharItemDialog)
	enum { IDD = IDD_DIALOG_AI_ITEM };
	CComboBox	m_csItemTemplate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmAICharItemDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmAICharItemDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AgcmAICharItemDialog_H__9F1980A4_50C6_4966_B79F_61738C0B3501__INCLUDED_)
