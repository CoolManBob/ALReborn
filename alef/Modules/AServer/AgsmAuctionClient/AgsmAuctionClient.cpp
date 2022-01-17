/******************************************************************************
Module:  AgsmAuctionClient.cpp
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#include <stdio.h>
#include "AgsmAuctionClient.h"

AgsmAuctionClient::AgsmAuctionClient()
{
	SetModuleName( "AgsmAuctionClient" );

	//만들어봐요~ 만들어봐요~ 자~자~ 어떤 패킷이 만들어졌을까나? by 치요스케~
	SetPacketType(AGPMAUCTION_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  연산
							AUTYPE_INT32,			1, // lControlServerID;
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1, // pstrSellerID;
							AUTYPE_INT32,			1, // lSellerID;
							AUTYPE_INT32,			1, // lBuyerID;
							AUTYPE_INT32,			1, // lIID       Item ID
							AUTYPE_UINT64,			1, // ullDBID	 Item DBID
							AUTYPE_INT32,			1, // lITID      Item Type ID
							AUTYPE_PACKET,			1, // csInvenPos
							AUTYPE_PACKET,			1, // csSalesBoxPos
							AUTYPE_PACKET,			1, // csItemInfo
		                    AUTYPE_PACKET,			1, // csSearchInfo
							AUTYPE_PACKET,			1, // csCategoryCount
							AUTYPE_INT32,			1, // lResult    Result
		                    AUTYPE_END,				0
							);

	//Inven에 넣고 뺄때 사용한다.
	m_csInvenPos.SetFlagLength(sizeof(INT8));
	m_csInvenPos.SetFieldType(
		                         AUTYPE_INT16,		1, //Layer
		                         AUTYPE_INT16,		1, //Row
		                         AUTYPE_INT16,		1, //Column
		                         AUTYPE_END,		0
								 );

	//SalesBox에 넣고 뺄때 사용한다.
	m_csSalesBoxPos.SetFlagLength(sizeof(INT8));
	m_csSalesBoxPos.SetFieldType(
		                         AUTYPE_INT16,		1, //Layer
		                         AUTYPE_INT16,		1, //Row
		                         AUTYPE_INT16,		1, //Column
		                         AUTYPE_END,		0
								 );

	m_csItemInfo.SetFlagLength(sizeof(INT8));
	m_csItemInfo.SetFieldType(
								 AUTYPE_INT32,		1,  //게시물 번호.
		                         AUTYPE_CHAR,		16, //게시물 올린놈ID(문자형이름이다.)
		                         AUTYPE_INT32,		1,  //게시Item 가격.
								 AUTYPE_PACKET,		1,  //게시아이템 Field(m_csFieldInfo) 정보.
		                         AUTYPE_CHAR,		30, //게시물 올린시간 
		                         AUTYPE_END,		0
								 );

	m_csSearchInfo.SetFlagLength(sizeof(INT8));
	m_csSearchInfo.SetFieldType(
								 AUTYPE_INT8,		1,  //내 레벨을 기준으로 찾는다?
								 AUTYPE_INT32,		1,  //몇번째 페이지 인가?
		                         AUTYPE_END,		0
								 );

	m_csCategoryCount.SetFlagLength(sizeof(INT8));
	m_csCategoryCount.SetFieldType(
								 AUTYPE_INT32,		1,  //Item TypeID
								 AUTYPE_INT32,		1,  //Item Count
		                         AUTYPE_END,		0
								 );
}

AgsmAuctionClient::~AgsmAuctionClient()
{
}


BOOL AgsmAuctionClient::OnAddModule()
{
	m_csCIDManager.InitializeObject( sizeof(INT32), MAX_AUCTION_CONNECTION_COUNT );

	m_pagsmAuctionServer = (AgsmAuctionServer *)GetModule( "AgsmAuctionServer" );

	if( !m_pagsmAuctionServer )
		return FALSE;

	m_pagsmAuctionServer->SetCallbackSell( CBAuctionSell, this );
	m_pagsmAuctionServer->SetCallbackSold( CBAuctionSold, this );
	m_pagsmAuctionServer->SetCallbackBuy( CBAuctionBuy, this );
	m_pagsmAuctionServer->SetCallbackBackOutToInventory( CBAuctionBackoutToInventory, this );
	m_pagsmAuctionServer->SetCallbackBackOutToSalesBox( CBAuctionBackOutToSalesBox, this );

	return TRUE;
}

//옥션서버에서 캐릭터ID와 dpnid를 등록한다. ondestroy때 CID를 받기 위해서이다.
BOOL AgsmAuctionClient::ProcessAddCIDToAuctionServer( INT32 lCID, UINT32 ulNID )
{
	BOOL			bResult;

	bResult = SetIDToPlayerContext( lCID, ulNID );

	if( bResult == TRUE )
	{
		m_csCIDManager.AddObject( &lCID, ulNID );
	}

	return bResult;
}

UINT32 AgsmAuctionClient::GetulNID( INT32 lCID )
{
	UINT32		*pulNID;
	
	pulNID = (UINT32 *)m_csCIDManager.GetObject( lCID );

	if( pulNID != NULL )
	{
		return *pulNID;
	}
	else
	{
		return 0;
	}
}

BOOL AgsmAuctionClient::ParseInvenPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol)
{
	BOOL			bResult;

	bResult = FALSE;

	if( pGridPos != NULL )
	{
		INT16  lTempIndex, lTempRow, lTempColumn;

		m_csInvenPos.GetField(FALSE, pGridPos, 0, NULL,
									&lTempIndex,
									&lTempRow,
									&lTempColumn);

		if(pnLayer)
		{
			*pnLayer = lTempIndex;
		}

		if(pnRow)
		{
			*pnRow = lTempRow;
		}

		if(pnCol)
		{
			*pnCol = lTempColumn;
		}

		bResult = TRUE;	
	}

	return bResult;
}

BOOL AgsmAuctionClient::ParseSalesBoxPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol)
{
	BOOL			bResult;

	bResult = FALSE;

	if( pGridPos != NULL )
	{
		INT16  lTempIndex, lTempRow, lTempColumn;

		m_csSalesBoxPos.GetField(FALSE, pGridPos, 0, NULL,
									&lTempIndex,
									&lTempRow,
									&lTempColumn);

		if(pnLayer)
		{
			*pnLayer = lTempIndex;
		}

		if(pnRow)
		{
			*pnRow = lTempRow;
		}

		if(pnCol)
		{
			*pnCol = lTempColumn;
		}

		bResult = TRUE;	
	}

	return bResult;
}

BOOL AgsmAuctionClient::ParseSearchInfo( PVOID pvSearchInfo, bool *pbSearchByMyLevel, INT32 *plPage )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvSearchInfo != NULL )
	{
		bool			bTempSearchByMyLevel;
		INT32			lTempPage;


		m_csSearchInfo.GetField(FALSE, pvSearchInfo, 0, NULL,
									&bTempSearchByMyLevel,
									&lTempPage	);

		if(pbSearchByMyLevel)
		{
			*pbSearchByMyLevel = bTempSearchByMyLevel;
		}

		if(plPage)
		{
			*plPage = lTempPage;
		}

		bResult = TRUE;	
	}

	return bResult;
}

BOOL AgsmAuctionClient::CBAuctionSell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;

	pThis->SendSellItem( *plCID, 0 );

	return TRUE;
}

BOOL AgsmAuctionClient::CBAuctionSold(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;

	pThis->SendSoldItem( *plCID, 0 );

	return TRUE;
}

BOOL AgsmAuctionClient::CBAuctionBuy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;

	pThis->SendSellItem( *plCID, 0 );

	return TRUE;
}

BOOL AgsmAuctionClient::CBAuctionBackoutToInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;
	INT8					*pnOperation;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;
	pnOperation = (INT8 *)pCustData;

	pThis->SendBackOutItem( *plCID, 0, *pnOperation );

	return TRUE;
}


BOOL AgsmAuctionClient::CBAuctionBackOutToSalesBox(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;
	INT8					*pnOperation;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;
	pnOperation = (INT8 *)pCustData;

	pThis->SendBackOutItem( *plCID, 0, *pnOperation );

	return TRUE;
}

/*BOOL AgsmAuctionClient::SendItemCount( INT32 lBuyerID, INT32 lItemTypeID, INT32 lItemCount )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_GET_ITEM_COUNT;
	INT16	nPacketLength;
	void	*pvCategoryCount;

	pvCategoryCount  = m_csCategoryCount.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
										&lItemTypeID,
										&lItemCount );

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,     //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													NULL,           //lSellerID
													&lBuyerID,      //lBuyerID
													NULL,           //lIID
													NULL,			//ullDBID
													NULL,           //lTID
													NULL,			//pvGridPos
													NULL,           //pvItemInfo
													NULL,			//pvSearchInfo
													pvCategoryCount,//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lBuyerID) );
		
		m_csCategoryCount.FreePacket(pvCategoryCount);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmAuctionClient::SendGetItemInfo()
{
	return TRUE;
}

//옥션에 아이템을 판다고 알린다.(Sales Box에 물건이 들어가는거다.)
BOOL AgsmAuctionClient::SendSellItem( INT32 lSellerID, INT32 lItemTypeID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_SELL_ITEM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													&lSellerID,      //lSellerID
													NULL,			//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lSellerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

//SalesBox에 올린 아이템이 팔렸다고 알린다.
BOOL AgsmAuctionClient::SendSoldItem( INT32 lSellerID, INT32 lItemTypeID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_SOLD_ITEM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													&lSellerID,      //lSellerID
													NULL,			//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lSellerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

///옥션에 올라온 아이템을 산다고 알린다.
BOOL AgsmAuctionClient::SendBuyItem( INT32 lBuyerID, INT32 lItemTypeID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_BUY_ITEM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													NULL,			//lSellerID
													&lBuyerID,		//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lBuyerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

//옥션에 올린 아이템을 유찰시킨다고 알린다.(Sales Box에서 물건을 뺀다.)
BOOL AgsmAuctionClient::SendBackOutItem( INT32 lSellerID, INT32 lItemTypeID, INT8 lOperation )
{
	BOOL			bResult;

	bResult = FALSE;

	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													&lSellerID,      //lSellerID
													NULL,			//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lSellerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmAuctionClient::SendSearchByPage( INT32 lBuyerID, INT32 lIndex, char *pstrSellerID, UINT64 ullDBID, INT32 lItemTypeID, INT32 lPrice, char *pstrDate )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_GET_ITEM_COUNT;
	INT16	nPacketLength;
	void	*pvItemInfo;

	pvItemInfo  = m_csItemInfo.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
										lIndex,
										pstrSellerID,
										lPrice,
										NULL,
										pstrDate );

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,     //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													NULL,           //lSellerID
													&lBuyerID,      //lBuyerID
													NULL,			//lIID
													&ullDBID,		//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													pvItemInfo,     //pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lBuyerID) );
		
		m_csItemInfo.FreePacket(pvItemInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

//TID에 해당하는 아이템 갯수를 확인하고 보내준다.
BOOL AgsmAuctionClient::ProcessGetItemCount( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID )
{
	//DB에 해당 아이템의 갯수를 얻는다.

	//갯수를 보내준다.
	//SendItemCount();

	return TRUE;
}

BOOL AgsmAuctionClient::ProcessGetItemInfo( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID )
{
	//DB에서 해당 아이템의 정보(Factor)를 얻어낸다.

	//SendItemInfo();
	
	return TRUE;
}

BOOL AgsmAuctionClient::ProcessSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos )
{
	BOOL			bResult;
	INT16			nInvenLayer, nInvenRow, nInvenColumn;
	INT16			nSaleLayer, nSaleRow, nSaleColumn;

	bResult = ParseInvenPos( pvInvenPos, &nInvenLayer, &nInvenRow, &nInvenColumn );
	
	if( bResult == TRUE )
	{
		bResult = ParseSalesBoxPos( pvSalesBoxPos, &nSaleLayer, &nSaleRow, &nSaleColumn );

		if( bResult == TRUE )
		{
			bResult = m_pagsmAuctionServer->PreprocessSellItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, nInvenLayer, nInvenRow, nInvenColumn, nSaleLayer, nSaleRow, nSaleColumn );
		}
	}
	
	return bResult;
}

BOOL AgsmAuctionClient::ProcessBuyItem( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID )
{
	return m_pagsmAuctionServer->PreprocessBuyItem( lControlServerID, lBuyerID, ullDBID, lTID );
}

BOOL AgsmAuctionClient::ProcessBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos, INT8 lOperation )
{
	BOOL			bResult;
	INT16			nInvenLayer, nInvenRow, nInvenColumn;
	INT16			nSaleLayer, nSaleRow, nSaleColumn;

	bResult = ParseInvenPos( pvInvenPos, &nInvenLayer, &nInvenRow, &nInvenColumn );
	
	if( bResult == TRUE )
	{
		bResult = ParseSalesBoxPos( pvSalesBoxPos, &nSaleLayer, &nSaleRow, &nSaleColumn );

		if( bResult == TRUE )
		{
			m_pagsmAuctionServer->PreprocessBackOutItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, nSaleLayer, nInvenLayer, nInvenRow, nInvenColumn, nSaleRow, nSaleColumn, lOperation );
		}
	}
	
	return bResult;
}

BOOL AgsmAuctionClient::ProcessSearchByPage( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID, void *pvSearchInfo )
{
	BOOL			bResult;
	bool			bSearchByMyLevel;
	INT32			lPage;

	bResult = ParseSearchInfo( pvSearchInfo, &bSearchByMyLevel, &lPage );

	if( bResult == TRUE )
	{
		//조건에 따라 검색한다.

		//검색 뒤 결과를 보내준다.
		//SendSearchByPage();
	}

	return bResult;
}

//패킷을 받고 파싱하자~
BOOL AgsmAuctionClient::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
{
	//거래결과 혹은 Error메시지를 출력해준다~ 출력해봐요~ 출력해봐요~
	BOOL			bResult;

	INT8			lOperation;
	INT32			lControlServerID;
	char			*pstrSellerID;
	INT32			lSellerID;
	INT32			lBuyerID;
	INT32			lIID;
	UINT64			ullDBID;
	INT32			lTID;
	void			*pvInvenPos;
	void			*pvSalesBoxPos;   //Sales Box 그리드 위치.
	void			*pvItemInfo;  //아이템 정보.
	void			*pvSearchInfo;
	void			*pvCategoryCount;
	INT32			lResult;

	bResult = FALSE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&lOperation,
						&lControlServerID,
						&pstrSellerID,
						&lSellerID,
						&lBuyerID,
						&lIID,
						&ullDBID,
						&lTID,
						&pvInvenPos,
						&pvSalesBoxPos,
						&pvItemInfo,
						&pvSearchInfo,
						&pvCategoryCount,
						&lResult );

	//나도 옥션 서버에 등록시켜줘~
	if( lOperation == AGPMAUCTION_ADD_TO_AUCTIONSERVER )
	{
		bResult = ProcessAddCIDToAuctionServer( lSellerID, ulNID );
	}
	//서버로부터 원하는 분류의 아이템 갯수를 얻어낸다.
	else if( lOperation == AGPMAUCTION_GET_ITEM_COUNT )
	{
		bResult = ProcessGetItemCount( lControlServerID, lBuyerID, lTID );
	}
	else if( lOperation == AGPMAUCTION_GET_ITEM_INFO )
	{
		bResult = ProcessGetItemInfo( lControlServerID, lBuyerID, lIID );
	}
	//아이템을 판다.(Sales Box)에 올린다.
	else if( lOperation == AGPMAUCTION_SELL_ITEM )
	{
		bResult = ProcessSellItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, pvInvenPos, pvSalesBoxPos );
	}
	//아이템을 산다.(게시판에 올라온 아이템을 산다.)
	else if( lOperation == AGPMAUCTION_BUY_ITEM )
	{
		bResult = ProcessBuyItem( lControlServerID, lBuyerID, ullDBID, lTID );
	}
	//입찰한다. (2차 이후부터 쓰인다. 아직은 안쓰임.)
	else if( lOperation == AGPMAUCTION_BIDDING_ITEM )
	{
	}
	//아이템을 유찰시킨다.
	else if( lOperation == AGPMAUCTION_BACK_OUT_TO_INVENTORY )
	{
		bResult = ProcessBackOutItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, pvInvenPos, pvSalesBoxPos, lOperation );
	}
	//아이템을 유찰시킨다.
	else if( lOperation == AGPMAUCTION_BACK_OUT_TO_SALESBOX )
	{
		bResult = ProcessBackOutItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, pvInvenPos, pvSalesBoxPos, lOperation );
	}
	//페이지 단위로 검색한다.
	else if( lOperation == AGPMAUCTION_SEARCH_BY_PAGE )
	{
		bResult = ProcessSearchByPage(lControlServerID, lBuyerID, lTID, pvSearchInfo );
	}
	//비정상적인 패킷을 받았다. 아햏햏~
	else
	{
	}

	return TRUE;
}

//접속이 끊기면 이리루온다. 아햏햏~
BOOL AgsmAuctionClient::OnDisconnect(INT32 lAccountID)
{
	//lAccountID는 AGPMAUCTION_ADD_TO_AUCTIONSERVER를 받고 CID, dpnid를 받은뒤에 
	//SetIDToPlayerContext의 조합되에 발생된 값이다. AccountID == CID이다.
	return m_csCIDManager.RemoveObject( lAccountID );
}
*/