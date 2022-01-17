// AgcmEventQuestDlg2.h : main header file for the AGCMEVENTQUESTDLG2 DLL
//

#ifndef _AGCMEVENT_PRODUCT_DLG_H_
	#define _AGCMEVENT_PRODUCT_DLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ApModule.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
#include "AgpmProduct.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmEventProductDlg
// See AgcmEventProductDlg.cpp for the implementation of this class
//

class AFX_EXT_CLASS AgcmEventProductDlg : public ApmEventDialog
{
public:
	AgcmEventProductDlg();
	~AgcmEventProductDlg();

	BOOL	OnAddModule();

	BOOL	Open(ApdEvent *pcsEvent);
	void	Close();

public:
	ApmEventManager			*m_pApmEventManager;
	ApmEventManagerDlg		*m_pApmEventManagerDlg;	
	AgpmProduct				*m_pAgpmProduct;
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
