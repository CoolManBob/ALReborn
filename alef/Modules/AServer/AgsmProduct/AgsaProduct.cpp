/*====================================================================

	AgsaProduct.cpp

====================================================================*/

#include "AgsaProduct.h"

/************************************************************/
/*		The Implementation of AgsaProductItemSet class		*/
/************************************************************/
//
AgsaProductItemSet::AgsaProductItemSet()
	{
	}

AgsaProductItemSet::~AgsaProductItemSet()
	{
	}

BOOL AgsaProductItemSet::Add(AgsdProductItemSet *pAgsdProductItemSet)
	{
	if (!AddObject((PVOID) &pAgsdProductItemSet, pAgsdProductItemSet->m_lID))
		return FALSE;

	return TRUE;	
	}

BOOL AgsaProductItemSet::Remove(AgsdProductItemSet *pAgsdProductItemSet)
	{
	return RemoveObject(pAgsdProductItemSet->m_lID);
	}

AgsdProductItemSet* AgsaProductItemSet::Get(INT32 lID)
	{
	AgsdProductItemSet **ppAgsdProductItemSet = (AgsdProductItemSet **) GetObject(lID);
	if (!ppAgsdProductItemSet)
		return NULL;	

	return *ppAgsdProductItemSet;
	}

