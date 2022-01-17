#if !defined(AFX_EVENTBUTTON_H__B1DC5440_A59D_4C62_BE14_CB2FCC929FBF__INCLUDED_)
#define AFX_EVENTBUTTON_H__B1DC5440_A59D_4C62_BE14_CB2FCC929FBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EventButton.h : header file
//

typedef BOOL (*EventButtonCallback)(PVOID pvClass, PVOID pvData1, PVOID pData2, PVOID pData3);

/////////////////////////////////////////////////////////////////////////////
// CEventButton window

class CEventButton : public CButton
{
protected:
	PVOID					m_pvClass;
	PVOID					m_pvData1;
	PVOID					m_pvData2;
	PVOID					m_pvData3;

	EventButtonCallback		m_pfCallback;
// Construction
public:
	CEventButton(CWnd *pcsParent, RECT &stInitRect,	LPCSTR lpszCaption, BOOL bFlat = FALSE, EventButtonCallback pfCallback = NULL, PVOID pvClass = NULL, PVOID pvData1 = NULL, PVOID pvData2 = NULL, PVOID pvData3 = NULL);

// Attributes
public:

// Operations
public:
	VOID _ShowButton();
	VOID _HideButton();

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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTBUTTON_H__B1DC5440_A59D_4C62_BE14_CB2FCC929FBF__INCLUDED_)
