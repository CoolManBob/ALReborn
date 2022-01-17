// AgcmEventNPCTradeDlg.h : main header file for the AGCMEVENTNPCTRADEDLG DLL
//

#if !defined(AFX_AGCMEVENTAUCTIONDLG_H__A01A2F0E_E742_4073_8C48_9017FE77BCF5__INCLUDED_)
#define AFX_AGCMEVENTAUCTIONDLG_H__A01A2F0E_E742_4073_8C48_9017FE77BCF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ApModule.h"
#include "ApmObject.h"
#include "AgpmCharacter.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
//#include "AgpmEventNPCTrade.h"
//#include "AgcmEventNPCTrade.h"

/////////////////////////////////////////////////////////////////////////////
// CAgcmEventNPCTradeDlgApp
// See AgcmEventNPCTradeDlg.cpp for the implementation of this class
//

class AFX_EXT_CLASS AgcmEventAuctionDlg : public ApmEventDialog  
{
private:
	ApmObject				*m_pcsApmObject;
	AgpmCharacter			*m_pcsAgpmCharacter;
	ApmEventManager *		m_pcsApmEventManager;
	ApmEventManagerDlg *	m_pcsApmEventManagerDlg;

public:
	AgcmEventAuctionDlg();
	virtual ~AgcmEventAuctionDlg();

	// Virtual Function ต้
	BOOL	OnAddModule();
	BOOL	Open(ApdEvent *pstEvent);
	void	Close();
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTAUCTIONDLG_H__A01A2F0E_E742_4073_8C48_9017FE77BCF5__INCLUDED_)
