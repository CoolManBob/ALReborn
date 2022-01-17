/******************************************************************************
Module:  AgsaAccountManager.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 27
******************************************************************************/

#if !defined(__AGSAACCOUNTMANAGER_H__)
#define __AGSAACCOUNTMANAGER_H__

#include "ApAdmin.h"
#include "AgsdAccount.h"

class AgsaAccount : public ApAdmin {
public:
	AgsaAccount();
	~AgsaAccount();

	AgsdAccount*	GetAccount(CHAR *szAccountName);
	AgsdAccount*	GetAccount(INT32 lAccountID);

	AgsdAccount*	AddAccount(AgsdAccount *pcsAccount, INT32 lAccountID, CHAR *szAccountName);

	BOOL			RemoveAccount(CHAR *szAccountName);
	BOOL			RemoveAccount(INT32 lAccountID);
};

#endif //__AGSAACCOUNTMANAGER_H__