// XEdit.h : header file
//

#ifndef XEDIT_H
#define XEDIT_H

extern UINT WM_XEDIT_KILL_FOCUS;
extern UINT WM_XEDIT_VK_ESCAPE;

/////////////////////////////////////////////////////////////////////////////
// CXEdit window

class CXEdit : public CEdit
{
// Construction
public:
	CXEdit(CWnd *pParent, LPCTSTR lpszText);
	virtual ~CXEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXEdit)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd *	m_pParent;
	CString	m_strText;
	BOOL	m_bMessageSent;

	void SendRegisteredMessage(UINT nMsg, WPARAM wParam = 0, LPARAM lParam = 0);


	// Generated message map functions
protected:
	//{{AFX_MSG(CXEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XEDIT_H
