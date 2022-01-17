// PatchClient.h : main header file for the PATCHCLIENT application
//

#if !defined(AFX_PATCHCLIENT_H__AD1A8A8E_6C7F_4010_8CB5_60119A5F8A7F__INCLUDED_)
#define AFX_PATCHCLIENT_H__AD1A8A8E_6C7F_4010_8CB5_60119A5F8A7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "PatchException.h"

/////////////////////////////////////////////////////////////////////////////
// CPatchClientApp:
// See PatchClient.cpp for the implementation of this class
//

class CPatchClientApp 
	:	public CWinApp	,
		public CPatchException
{
public:
	CPatchClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	void SetResource(void);

// Implementation

	//{{AFX_MSG(CPatchClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//@{ 2006/08/03 burumal
class CXmlLogger;
extern CXmlLogger* g_pLogger;
//@}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHCLIENT_H__AD1A8A8E_6C7F_4010_8CB5_60119A5F8A7F__INCLUDED_)
