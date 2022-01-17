/*============================================================

	AgpaAuction.cpp
	
============================================================*/

#include "AgpaAuction.h"

/************************************************************/
/*		The Implementation of AgpaAuctionSales class		*/
/************************************************************/
//
AgpaAuctionSales::AgpaAuctionSales()
	{
	}

	
AgpaAuctionSales::~AgpaAuctionSales()
	{
	}
	

BOOL AgpaAuctionSales::Add(AgpdAuctionSales *pAgpdAuctionSales)
	{
	if (!pAgpdAuctionSales || !AddObject((PVOID) &pAgpdAuctionSales, pAgpdAuctionSales->m_lID))
		return FALSE;

	return TRUE;		
	}

	
BOOL AgpaAuctionSales::Remove(AgpdAuctionSales *pAgpdAuctionSales)
	{
	if (!pAgpdAuctionSales)
		return FALSE;

	return RemoveObject(pAgpdAuctionSales->m_lID);
	}


BOOL AgpaAuctionSales::Remove(INT32 lID)
	{
	return RemoveObject(lID);
	}


AgpdAuctionSales* AgpaAuctionSales::Get(INT32 lID)
	{
	AgpdAuctionSales **ppAgpdAuctionSales = (AgpdAuctionSales **) GetObject(lID);
	if (!ppAgpdAuctionSales)
		return NULL;	

	return *ppAgpdAuctionSales;	
	}

