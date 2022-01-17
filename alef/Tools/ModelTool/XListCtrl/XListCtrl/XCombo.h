// XCombo.h : header file
//

#ifndef XCOMBO_H
#define XCOMBO_H

#include "AdvComboBox.h"
#include "XComboList.h"

extern UINT WM_XCOMBOLIST_COMPLETE;
//extern UINT WM_XCOMBOLIST_VK_ESCAPE;

/////////////////////////////////////////////////////////////////////////////
// CXCombo window

class CXCombo : public CAdvComboBox
{
// Construction
public:
	CXCombo(CWnd * pParent);
	virtual ~CXCombo();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXCombo)
protected:
	//}}AFX_VIRTUAL
	virtual void OnEscapeKey();
	virtual void OnComboComplete();

// Implementation
protected:
	CWnd * m_pParent;

	void SendRegisteredMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);

	// Generated message map functions
protected:
	//{{AFX_MSG(CXCombo)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XCOMBO_H
