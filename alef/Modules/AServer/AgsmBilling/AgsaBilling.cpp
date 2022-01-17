/*============================================================

	AgsaBilling.cpp
	
============================================================*/


#include "AgsaBilling.h"


/****************************************************/
/*		The Implementation of AgsaBilling class		*/
/****************************************************/
//
AgsaBilling::AgsaBilling()
	{
	}

	
AgsaBilling::~AgsaBilling()
	{
	}
	

BOOL AgsaBilling::Add(AgsdBilling *pAgsdBilling)
	{
	if (!pAgsdBilling || !AddObject((PVOID) &pAgsdBilling, pAgsdBilling->m_lID))
		return FALSE;

	return TRUE;		
	}

BOOL AgsaBilling::Add(AgsdBilling *pAgsdBilling, CHAR* AccountID)//JK_ºô¸µ
{
	if (!pAgsdBilling || !AddObject((PVOID) &pAgsdBilling, AccountID))
		return FALSE;

	return TRUE;		
}

BOOL AgsaBilling::Remove(AgsdBilling *pAgsdBilling)
	{
	if (!pAgsdBilling)
		return FALSE;

	return RemoveObject(pAgsdBilling->m_lID);
	}


BOOL AgsaBilling::Remove(INT32 lID)
	{
	return RemoveObject(lID);
	}

BOOL AgsaBilling::Remove(CHAR* AccountID)//JK_ºô¸µ
{
	return RemoveObject(AccountID);
}


AgsdBilling* AgsaBilling::Get(INT32 lID)
	{
	AgsdBilling **ppAgsdBilling = (AgsdBilling **) GetObject(lID);
	if (!ppAgsdBilling)
		return NULL;

	return *ppAgsdBilling;	
	}

AgsdBilling* AgsaBilling::Get(CHAR* szAccountID)//JK_ºô¸µ
{
	AgsdBilling **ppAgsdBilling = (AgsdBilling **) GetObject(szAccountID);
	if (!ppAgsdBilling)
		return NULL;

	return *ppAgsdBilling;	
}