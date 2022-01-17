// AgcmEventQuestDlg.h : main header file for the AGCMEVENTQUESTDLG DLL
//

#if !defined(AFX_AGCMEVENTQUESTDLG_H__EBB066AE_2CE2_4ED6_8882_C01577785963__INCLUDED_)
#define AFX_AGCMEVENTQUESTDLG_H__EBB066AE_2CE2_4ED6_8882_C01577785963__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef	_WINDLL
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventQuestDlgD" )
#else
#pragma comment ( lib , "AgcmEventQuestDlg" )
#endif // 	_DEBUG
#endif
#endif //  _WINDLL

//#include "resource.h"		// main symbols

#include "ApModule.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
#include "AgpmQuest.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmEventQuestDlg
// See AgcmEventQuestDlg.cpp for the implementation of this class
//

class AFX_EXT_CLASS AgcmEventQuestDlg : public ApmEventDialog
{
public:
	AgcmEventQuestDlg();
	~AgcmEventQuestDlg();

	BOOL	OnAddModule();

	BOOL	Open(ApdEvent *pcsEvent);
	void	Close();

public:
	ApmEventManager			*m_pcsApmEventManager;
	ApmEventManagerDlg		*m_pcsApmEventManagerDlg;	
	AgpmQuest				*m_pcsAgpmQuest;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTQUESTDLG_H__EBB066AE_2CE2_4ED6_8882_C01577785963__INCLUDED_)
