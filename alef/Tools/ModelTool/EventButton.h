#pragma once

typedef BOOL (*EventButtonCallback)(PVOID pvClass);

/////////////////////////////////////////////////////////////////////////////
// CEventButton window

class CEventButton : public CButton
{
protected:
	PVOID				m_pvClass;
	EventButtonCallback m_pfCallback;

	HBRUSH				m_hBackBrush;

// Construction
public:
	CEventButton(CWnd *pcsParent, RECT &stInitRect,	LPCSTR lpszCaption, BOOL bFlat = FALSE, EventButtonCallback pfCallback = NULL, PVOID pvClass = NULL);

// Attributes
public:

// Operations
public:
	VOID _ShowButton();
	VOID _HideButton();

	VOID _SetWindowText(LPCTSTR lpszString); 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEventButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEventButton)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};