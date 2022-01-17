/*============================================================

	AgpaAuction.h
	
============================================================*/

#ifndef _AGPA_AUCTION_H_
	#define _AGPA_AUCTION_H_

#include "ApBase.h"
#include "ApAdmin.h"
#include "ApModule.h"
#include "AgpdAuction.h"

/********************************************************/
/*		The Definition of AgpaAuctionSales class		*/
/********************************************************/
//
class AgpaAuctionSales : public ApAdmin
	{
	public:
		AgpaAuctionSales();
		~AgpaAuctionSales();
		
		BOOL Add(AgpdAuctionSales *pAgpdAuctionSales);
		BOOL Remove(AgpdAuctionSales *pAgpdAuctionSales);
		BOOL Remove(INT32 lID);
		AgpdAuctionSales* Get(INT32 lID);
	};

#endif