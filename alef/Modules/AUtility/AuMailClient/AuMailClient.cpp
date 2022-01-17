// MailMan.cpp: implementation of the AuMailClient class.
//
//////////////////////////////////////////////////////////////////////
// This generic class uses MAPI to read and send emails via your application.
// This work is a clean class inspired by others that did a bit hera dn did a 
// bit there but never took it this far for its easy usage.No dll
// no nasty extra code.Just a simple class to use.
// As always I say:Keep It Simple Stupid
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AuMailClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

AuMailClient::AuMailClient()
{
	ZeroMemory(m_szServerAddress, sizeof(CHAR) * 128);
}

AuMailClient::~AuMailClient()
{
}

BOOL AuMailClient::SetSMTPLoginInfo(CHAR *szUserName, CHAR *szPassword, CHAR *szServerAddress)
{
	if (!szUserName || !szPassword || !szServerAddress ||
		!strlen(szUserName) || !strlen(szPassword) || !strlen(szServerAddress))
		return FALSE;

	m_csSmtp.m_strUser = _T(szUserName);
	m_csSmtp.m_strPass = _T(szPassword);

	strncpy(m_szServerAddress, szServerAddress, 128);

	return TRUE;
}

/*
BOOL AuMailClient::SetDefaultSenderInfo(CHAR *szSenderName, CHAR *szSenderAddress)
{
	return TRUE;
}

BOOL AuMailClient::SetDefaultRecipientInfo(CHAR *szRecipientName, CHAR *szRecipientAddress)
{
	return TRUE;
}
*/

BOOL AuMailClient::SendMail(LPTSTR pszAddrFrom, LPTSTR pszAddrTo, LPTSTR pszSubject, LPTSTR pszMessage, LPVOID pvAttachments, DWORD dwAttachmentCount)
{

	// Initialize winsock  
	//WSADATA wsa;
	//WSAStartup(MAKEWORD(2,0),&wsa);

	m_csSmtp.Connect(m_szServerAddress);

	m_csSmtp.SendMessage(pszAddrFrom, pszAddrTo, pszSubject, pszMessage, pvAttachments, dwAttachmentCount);

	m_csSmtp.Close();

	//WSACleanup();

	return TRUE;
}