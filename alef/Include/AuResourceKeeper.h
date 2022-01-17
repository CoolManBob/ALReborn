// AuResourceKeeper.h : main header file for the AURESOURCEKEEPER DLL
//

#if !defined(AFX_AURESOURCEKEEPER_H__98E4729C_D697_4BE3_AE7B_4699D5E5190C__INCLUDED_)
#define AFX_AURESOURCEKEEPER_H__98E4729C_D697_4BE3_AE7B_4699D5E5190C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAuResourceKeeperApp
// See AuResourceKeeper.cpp for the implementation of this class
//

class CAuResourceKeeperApp : public CWinApp
{
public:
	CAuResourceKeeperApp();
	~CAuResourceKeeperApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuResourceKeeperApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAuResourceKeeperApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AURESOURCEKEEPER_H__98E4729C_D697_4BE3_AE7B_4699D5E5190C__INCLUDED_)
