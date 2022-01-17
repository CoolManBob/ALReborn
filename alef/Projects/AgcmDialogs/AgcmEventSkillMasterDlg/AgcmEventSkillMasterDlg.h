// AgcmEventSkillMasterDlg.h : main header file for the AGCMEVENTSKILLMASTERDLG DLL
//

#if !defined(AFX_AGCMEVENTSKILLMASTERDLG_H__44F09752_E821_48F6_9C7A_65F7D5FEAB61__INCLUDED_)
#define AFX_AGCMEVENTSKILLMASTERDLG_H__44F09752_E821_48F6_9C7A_65F7D5FEAB61__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000
//
//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "ApModule.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
#include "AgpmEventSkillMaster.h"

/////////////////////////////////////////////////////////////////////////////
// CAgcmEventSkillMasterDlgApp
// See AgcmEventSkillMasterDlg.cpp for the implementation of this class
//

//class AgcmEventSkillMasterDlg : public CWinApp
class AFX_EXT_CLASS AgcmEventSkillMasterDlg : public ApmEventDialog  
{
public:
	AgcmEventSkillMasterDlg();
	virtual ~AgcmEventSkillMasterDlg();

	BOOL	OnAddModule();

	BOOL	Open(ApdEvent *pcsEvent);
	void	Close();

private:
	ApmEventManager			*m_pcsApmEventManager;
	ApmEventManagerDlg		*m_pcsApmEventManagerDlg;
	AgpmEventSkillMaster	*m_pcsAgpmEventSkillMaster;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgcmEventSkillMasterDlgApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAgcmEventSkillMasterDlgApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTSKILLMASTERDLG_H__44F09752_E821_48F6_9C7A_65F7D5FEAB61__INCLUDED_)
