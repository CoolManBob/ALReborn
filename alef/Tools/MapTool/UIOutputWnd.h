#if !defined(AFX_UIOUTPUTWND_H__9FFEF9D3_7472_4461_819F_4381F6DA581E__INCLUDED_)
#define AFX_UIOUTPUTWND_H__9FFEF9D3_7472_4461_819F_4381F6DA581E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UIOutputWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUIOutputWnd window
#include <AuList.h>

class CUIOutputWnd : public CWnd
{
// Construction
public:
	CUIOutputWnd();

// Attributes
public:
	struct	stMessage
	{
		CString		message	;
		COLORREF	color	;
	};

	AuList< stMessage >	m_listMessage;

	int m_nMaxMessage;
	int m_cyChar;

	void SetUp();
	void AddMessage( CString str , COLORREF color = RGB( 0 , 0 , 0 ) );
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUIOutputWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUIOutputWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUIOutputWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UIOUTPUTWND_H__9FFEF9D3_7472_4461_819F_4381F6DA581E__INCLUDED_)
