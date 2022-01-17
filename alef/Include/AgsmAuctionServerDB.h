/******************************************************************************
Module:  AgsmAuctionServerDB.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 2. 17
******************************************************************************/

#ifndef __AGSM_AUCTION_SERVERDB_H__
#define __AGSM_AUCTION_SERVERDB_H__

#include "AgsEngine.h"
#include "AuOLEDB.h"
#include "AgsmAuctionServer.h"
#include "AgsmAuctionClient.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAuctionServerDBD" )
#else
#pragma comment ( lib , "AgsmAuctuonServerDB" )
#endif

class AgsmAuctionServerDB : public AgsModule
{
	AuOLEDBManager		*m_paAuOLEDBManager;
	AgsmAuctionServer	*m_pagsmAuctionServer;
	AgsmAuctionClient	*m_pagsmAuctionClient;

public:

	AgsmAuctionServerDB();
	~AgsmAuctionServerDB();

	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();

	BOOL ProcessDBOperation( COLEDB *pcOLEDB, INT8 nOperation, INT32 lSellerID, INT32 lBuyerID, INT32 lErrorCode );
	static unsigned WINAPI AuctionDBProcessThread( void *pvArg );
};

#endif
