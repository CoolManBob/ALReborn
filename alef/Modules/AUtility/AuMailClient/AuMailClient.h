// AuMailClient.h : main header file for the AUMAILCLIENT DLL
//

#if !defined(AFX_AUMAILCLIENT_H__586AA6C8_443B_4D87_A91A_D5312D88FA8C__INCLUDED_)
#define AFX_AUMAILCLIENT_H__586AA6C8_443B_4D87_A91A_D5312D88FA8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
*/

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAuMailClientApp
// See AuMailClient.cpp for the implementation of this class
//

#include "Smtp.h"

class AuMailClient
{
private:
	CSmtp	m_csSmtp;

	CHAR	m_szServerAddress[128];

public:
	AuMailClient();
	virtual ~AuMailClient();

	BOOL	SetSMTPLoginInfo(CHAR *szUserName, CHAR *szPassword, CHAR *szServerAddress);

	/*
	BOOL	SetDefaultSenderInfo(CHAR *szSenderName, CHAR *szSenderAddress);
	BOOL	SetDefaultRecipientInfo(CHAR *szRecipientName, CHAR *szRecipientAddress);
	*/

	BOOL	SendMail(LPTSTR pszAddrFrom, LPTSTR pszAddrTo, LPTSTR pszSubject, LPTSTR pszMessage, LPVOID pvAttachments = NULL, DWORD dwAttachmentCount = 0);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUMAILCLIENT_H__586AA6C8_443B_4D87_A91A_D5312D88FA8C__INCLUDED_)
