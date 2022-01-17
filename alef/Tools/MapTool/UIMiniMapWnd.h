#if !defined(AFX_UIMINIMAPWND_H__BC878B3E_5BAE_4A2E_BF04_4D7649AD048A__INCLUDED_)
#define AFX_UIMINIMAPWND_H__BC878B3E_5BAE_4A2E_BF04_4D7649AD048A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UIMiniMapWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUIMiniMapWnd window

class CUIMiniMapWnd : public CWnd
{
// Construction
public:
	CUIMiniMapWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUIMiniMapWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUIMiniMapWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUIMiniMapWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UIMINIMAPWND_H__BC878B3E_5BAE_4A2E_BF04_4D7649AD048A__INCLUDED_)
