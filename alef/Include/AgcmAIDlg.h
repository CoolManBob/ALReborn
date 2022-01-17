// AgcmAIDlg.h : main header file for the AGCMAIDLG DLL
//

#if !defined(AFX_AGCMAIDLG_H__CF37B569_6CCC_4956_B102_FC1ED07CABD4__INCLUDED_)
#define AFX_AGCMAIDLG_H__CF37B569_6CCC_4956_B102_FC1ED07CABD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "ApModule.h"
#include "AgpmCharacter.h"
#include "AgcmFactorsDlg.h"

#include "AgpmAI.h"

#include "resource.h"		// main symbols

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmFactorsDlgD" )
#else
#pragma comment ( lib , "AgcmFactorsDlg" )
#endif // 	_DEBUG
#endif

class AFX_EXT_CLASS AgcmAIDlg : public ApModule
{
public:
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmItem *				m_pcsAgpmItem;
	AgpmAI *				m_pcsAgpmAI;
	AgcmFactorsDlg *		m_pcsAgcmFactorsDlg;

public:
	AgcmAIDlg();
	virtual ~AgcmAIDlg();

	// Virtual Function ต้
	BOOL	OnAddModule();
	BOOL	OpenTemplates(BOOL bSelect = FALSE);
	BOOL	OpenTemplate(AgpdAITemplate *pstTemplate);

	BOOL	SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	void	Close();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMAIDLG_H__CF37B569_6CCC_4956_B102_FC1ED07CABD4__INCLUDED_)
