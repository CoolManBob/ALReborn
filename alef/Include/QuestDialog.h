#if !defined(AFX_QUESTDIALOG_H__6F0EABE5_739B_43DB_8F6E_96371348F106__INCLUDED_)
#define AFX_QUESTDIALOG_H__6F0EABE5_739B_43DB_8F6E_96371348F106__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuestDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQuestDialog dialog

class CQuestDialog : public CDialog
{
// Construction
public:
	CQuestDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuestDialog)
	enum { IDD = IDD_DIALOG1 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuestDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuestDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUESTDIALOG_H__6F0EABE5_739B_43DB_8F6E_96371348F106__INCLUDED_)
