// Patcher2.h : main header file for the PATCHER2 application
//

#if !defined(AFX_PATCHER2_H__E7539A97_5205_49BB_991E_B4801D85078D__INCLUDED_)
#define AFX_PATCHER2_H__E7539A97_5205_49BB_991E_B4801D85078D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPatcher2App:
// See Patcher2.cpp for the implementation of this class
//

class CPatcher2App : public CWinApp
{
public:
	CPatcher2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatcher2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPatcher2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHER2_H__E7539A97_5205_49BB_991E_B4801D85078D__INCLUDED_)
