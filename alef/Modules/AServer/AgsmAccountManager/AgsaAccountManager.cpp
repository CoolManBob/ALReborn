/******************************************************************************
Module:  AgsaAccountManager.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 27
******************************************************************************/

#include "AgsaAccountManager.h"

AgsaAccount::AgsaAccount()
{
}

AgsaAccount::~AgsaAccount()
{
}

AgsdAccount* AgsaAccount::GetAccount(CHAR *szAccountName)
{
	AgsdAccount	**ppcsAccount = (AgsdAccount **) GetObject(szAccountName);

	if (!ppcsAccount)
		return NULL;

	return *ppcsAccount;
}

AgsdAccount* AgsaAccount::GetAccount(INT32 lAccountID)
{
	AgsdAccount	**ppcsAccount = (AgsdAccount **) GetObject(lAccountID);

	if (!ppcsAccount)
		return NULL;

	return *ppcsAccount;
}

AgsdAccount* AgsaAccount::AddAccount(AgsdAccount *pcsAccount, INT32 lAccountID, CHAR *szAccountName)
{
	if (!AddObject((PVOID *) &pcsAccount, lAccountID, szAccountName))
		return NULL;

	return pcsAccount;
}

BOOL AgsaAccount::RemoveAccount(CHAR *szAccountName)
{
	return RemoveObject(szAccountName);
}

BOOL AgsaAccount::RemoveAccount(INT32 lAccountID)
{
	return RemoveObject(lAccountID);
}
