// Casper.h : main header file for the CASPER application
//

#if !defined(AFX_CASPER_H__2E1BAD25_9D3F_45C0_A521_E8859702477B__INCLUDED_)
#define AFX_CASPER_H__2E1BAD25_9D3F_45C0_A521_E8859702477B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCasperApp:
// See Casper.cpp for the implementation of this class
//
#include "RenderWare.h"

class CCasperApp : public CWinApp
{
public:
	CCasperApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCasperApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	// RenderWare MFC class
protected:
	CRenderWare         m_RenderWare	;
public:
	CRenderWare&		GetRenderWare()	{ return m_RenderWare; }

public:
	//{{AFX_MSG(CCasperApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CASPER_H__2E1BAD25_9D3F_45C0_A521_E8859702477B__INCLUDED_)
