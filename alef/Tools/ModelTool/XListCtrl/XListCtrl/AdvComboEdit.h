// AdvComboEdit.h : header file
//

#ifndef ADVCOMBOEDIT_H
#define ADVCOMBOEDIT_H

/////////////////////////////////////////////////////////////////////////////
// CAdvComboEdit window

class CAdvComboEdit : public CEdit
{
// Construction
public:
	CAdvComboEdit();
	virtual ~CAdvComboEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvComboEdit)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CAdvComboEdit)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //ADVCOMBOEDIT_H
