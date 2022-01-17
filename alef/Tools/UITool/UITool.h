// UITool.h : main header file for the UITOOL application
//

#if !defined(AFX_UITOOL_H__E22638E2_E360_403B_A4A7_763228CDCF7B__INCLUDED_)
#define AFX_UITOOL_H__E22638E2_E360_403B_A4A7_763228CDCF7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "RenderWare.h"
#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CUIToolApp:
// See UITool.cpp for the implementation of this class
//

class CUIToolApp : public CWinApp
{
public:
	CUIToolApp();
	~CUIToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUIToolApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL


	// RenderWare MFC class
protected:
	CRenderWare         m_RenderWare	;
public:
	CRenderWare&		GetRenderWare()	{ return m_RenderWare; }

public:
	//{{AFX_MSG(CUIToolApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITOOL_H__E22638E2_E360_403B_A4A7_763228CDCF7B__INCLUDED_)
