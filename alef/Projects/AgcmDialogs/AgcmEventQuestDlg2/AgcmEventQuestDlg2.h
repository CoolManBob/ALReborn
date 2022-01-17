// AgcmEventQuestDlg2.h : main header file for the AGCMEVENTQUESTDLG2 DLL
//

#if !defined(AFX_AGCMEVENTQUESTDLG2_H__B5E5E7C6_6160_4DC8_96C9_C42BCBB3B36A__INCLUDED_)
#define AFX_AGCMEVENTQUESTDLG2_H__B5E5E7C6_6160_4DC8_96C9_C42BCBB3B36A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

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

#endif // !defined(AFX_AGCMEVENTQUESTDLG2_H__B5E5E7C6_6160_4DC8_96C9_C42BCBB3B36A__INCLUDED_)
