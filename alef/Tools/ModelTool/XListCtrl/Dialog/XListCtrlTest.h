// XListCtrlTest.h : main header file for the XLISTCTRLTEST application
//

#ifndef XLISTCTRLTEST_H
#define XLISTCTRLTEST_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

///////////////////////////////////////////////////////////////////////////////
// CXListCtrlTestApp:
// See XListCtrlTest.cpp for the implementation of this class
//

class CXListCtrlTestApp : public CWinApp
{
public:
	CXListCtrlTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrlTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXListCtrlTestApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XLISTCTRLTEST_H
