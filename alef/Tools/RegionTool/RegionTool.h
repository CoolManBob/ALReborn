// RegionTool.h : main header file for the REGIONTOOL application
//

#if !defined(AFX_REGIONTOOL_H__D5B607F5_28D5_4E11_8985_B2781F52B554__INCLUDED_)
#define AFX_REGIONTOOL_H__D5B607F5_28D5_4E11_8985_B2781F52B554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRegionToolApp:
// See RegionTool.cpp for the implementation of this class
//

class CRegionToolApp : public CWinApp
{
public:
	CRegionToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionToolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRegionToolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONTOOL_H__D5B607F5_28D5_4E11_8985_B2781F52B554__INCLUDED_)
