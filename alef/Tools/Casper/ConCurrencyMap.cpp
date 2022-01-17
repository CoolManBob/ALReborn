// ConCurrencyMap.cpp: implementation of the CConCurrencyMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Casper.h"
#include "ConCurrencyMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConCurrencyMap::CConCurrencyMap()
{
	SetCount(300);
	InitializeObject(sizeof(ConCurrencyUser), 300);
}

CConCurrencyMap::~CConCurrencyMap()
{

}

BOOL CConCurrencyMap::Add(ConCurrencyUser *pstUser)
{
	if (AddObject( (PVOID*) &pstUser, pstUser->m_lNID ))
		return TRUE;
	else return FALSE;
}

ConCurrencyUser* CConCurrencyMap::Get(INT16 lNID)
{
	ConCurrencyUser **pvRetVal = (ConCurrencyUser**)GetObject(lNID);

	if (pvRetVal) return *pvRetVal;

	return NULL;
}

ConCurrencyUser* CConCurrencyMap::Get(LPCTSTR lpUserName)
{
	return NULL;
}

BOOL CConCurrencyMap::Remove(INT16 lNID)
{
	return RemoveObject(lNID);
}