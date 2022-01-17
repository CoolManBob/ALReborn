// AgcmShrineDlg.h : main header file for the AGCMSHRINEDLG DLL
//

#if !defined(AFX_AGCMSHRINEDLG_H__5ABC5580_C2F9_48CC_8E72_E14899CFC804__INCLUDED_)
#define AFX_AGCMSHRINEDLG_H__5ABC5580_C2F9_48CC_8E72_E14899CFC804__INCLUDED_

/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
*/

//#include "resource.h"		// main symbols
#include "ApModule.h"

#include "AgpmShrine.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CAgcmShrineDlgApp
// See AgcmShrineDlg.cpp for the implementation of this class
//

class AFX_EXT_CLASS AgcmShrineDlg : public ApmEventDialog
{
private:
	AgpmShrine			*m_pcsAgpmShrine;
	ApmEventManager		*m_pcsApmEventManager;
	ApmEventManagerDlg	*m_pcsApmEventManagerDlg;

	INT16				m_nCreatedShrineIndex;

public:
	AgcmShrineDlg();
	virtual ~AgcmShrineDlg();

	/*
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgcmShrineDlgApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAgcmShrineDlgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	*/

	// Virtual Function ต้
	BOOL	OnAddModule();

	BOOL	Open(ApdEvent *pcsEvent);
	BOOL	Close();

	static BOOL	CBAddShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMSHRINEDLG_H__5ABC5580_C2F9_48CC_8E72_E14899CFC804__INCLUDED_)
