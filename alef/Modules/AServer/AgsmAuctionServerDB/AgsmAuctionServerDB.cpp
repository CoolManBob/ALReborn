#include <stdio.h>
#include "AgsmAuctionServerDB.h"

AgsmAuctionServerDB::AgsmAuctionServerDB()
{
	SetModuleName( "AgsmAuctionServerDB" );
}

AgsmAuctionServerDB::~AgsmAuctionServerDB()
{
}

//어디서나 볼 수 있는 OnAddModule()
BOOL AgsmAuctionServerDB::OnAddModule()
{
	m_paAuOLEDBManager = (AuOLEDBManager *)GetModule( "AuOLEDBManager" );
	m_pagsmAuctionServer = (AgsmAuctionServer *)GetModule( "AgsmAuctionServer" );
	m_pagsmAuctionClient = (AgsmAuctionClient *)GetModule( "AgsmAuctionClient" );

	if( !m_paAuOLEDBManager || !m_pagsmAuctionServer || !m_pagsmAuctionServer )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmAuctionServerDB::ProcessDBOperation( COLEDB *pcOLEDB, INT8 nOperation, INT32 lSellerID, INT32 lBuyerID, INT32 lErrorCode )
{
	//쿼리를 실행한다.
	pcOLEDB->StartTranaction();

	//1. 성공한경우.
	if( pcOLEDB->ExecuteQuery() == OLEDB_SQL_SUCCESS )
	{
		//팔때.......
		if( nOperation == AGPMAUCTION_SELL_ITEM )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_SELL, &lSellerID, NULL );
		}
		//팔렸을때.......
		else if( nOperation == AGPMAUCTION_SOLD_ITEM )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_SOLD, &lSellerID, NULL );
		}
		//살때~
		else if( nOperation == AGPMAUCTION_BUY_ITEM )
		{
			//여기서는 하는일 없다~ AgsmAuctionServer에서 이미 CallBack SendBuyItem을 보낸다.
		}
		else if( nOperation == AGPMAUCTION_BIDDING_ITEM )
		{
			//2차이후에나 쓰인다.
		}
		else if( nOperation == AGPMAUCTION_BACK_OUT_TO_INVENTORY )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_BACKOUT_TO_INVENTORY, &lSellerID, NULL );
		}
		else if( nOperation == AGPMAUCTION_BACK_OUT_TO_SALESBOX )
		{
			m_pagsmAuctionServer->EnumCallback( AUCTION_CB_ID_BACKOUT_TO_SALESBOX, &lSellerID, NULL );
		}
		else if( nOperation == AGPMAUCTION_SEARCH_BY_PAGE )
		{
		}
		//Error~
		else
		{
		}

		pcOLEDB->EndQuery();
	}
	//2.실패한 경우.
	else
	{
	}

	pcOLEDB->CommitTransaction();

	return FALSE;
}

unsigned int WINAPI AgsmAuctionServerDB::AuctionDBProcessThread( void *pvArg )
{
	COLEDBManagerArg	*pcsOLEDBManagerArg;

	AuOLEDBManager		*pcOLEDBManager;
	AgsmAuctionServerDB	*pThis;
	COLEDB				*pcOLEDB;
	void				*pvQuery;

	INT32				lSellerID;
	INT32				lBuyerID;
	INT32				lErrorCode;
	INT8				nOperation;
	char				*pstrQuery;

	int				iIndex;

	//캐스팅.
	pcsOLEDBManagerArg = (COLEDBManagerArg *)pvArg;

	//필요한 정보를 얻는다.
	pcOLEDBManager = pcsOLEDBManagerArg->m_pcOLEDBManager;
	pThis = (AgsmAuctionServerDB *)pcsOLEDBManagerArg->pvClassPointer;
	iIndex = pcsOLEDBManagerArg->m_iIndex;

	//필요한 정보를 얻었으니 지운다.
	delete pvArg;

	//DB서버에 접속한다.
	pcOLEDB = pcOLEDBManager->GetOLEDB( iIndex );

	if( pcOLEDB->Initialize() == true )
	{
		printf( "OLEDB Init succeeded!!\n" );
	}
	else
	{
		printf( "OLEDBInit Failed!!\n" );
	}

	if( pcOLEDB->ConnectToDB( (LPOLESTR)L"scott", (LPOLESTR)L"tiger", (LPOLESTR)L"ora81" ) == true )
	{
		printf( "DB Connetion Succeeded!\n" );
	}
	else
	{
		printf( "DB Connection Failed!\n" );
	}

	while( 1 )
	{
		//Deactive면 Break!!
		if( pcOLEDBManager->GetStatus() == DBTHREAD_DEACTIVE )
		{
			break;
		}

		pvQuery = pcOLEDBManager->PopFromQueue();

		if( pvQuery != NULL )
		{
			AgsmAuctionQueueInfo		*pcsQueryInfo;
			pcsQueryInfo = (AgsmAuctionQueueInfo *)pvQuery;

			//CID, Operation, Query를 얻는다.
			nOperation = pcsQueryInfo->m_nOperation;
			lBuyerID = pcsQueryInfo->m_lBuyerID;
			lSellerID = pcsQueryInfo->m_lSellerID;
			lErrorCode = pcsQueryInfo->m_lErrorCode;
			pstrQuery = pcsQueryInfo->m_pstrQueury;

			if( pstrQuery != NULL )
			{
				//쿼리를 UniCode로 만든다.
				pcOLEDB->SetQueryText( pstrQuery );

				pThis->ProcessDBOperation( pcOLEDB, nOperation, lBuyerID, lSellerID, lErrorCode );
			}

			//쿼리를 실행했으니 지우자~ 아햏햏~
			delete pcsQueryInfo;
		}
		else
		{
			//1ms씩 쉰다.
			Sleep( 1 );
		}
	}

	return 1;
}
