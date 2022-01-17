/******************************************************************************
Module:  AgsmAuctionClient.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#ifndef __AGSM_AUCTION_CLIENT_H__
#define __AGSM_AUCTION_CLIENT_H__

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgsEngine.h"
#include "AgpmAuction.h"
#include "AgsmAuctionServer.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAuctionClientD" )
#else
#pragma comment ( lib , "AgsmAuctionClient" )
#endif

class AgsmAuctionClient : public AgsModule
{
	AuPacket			m_csPacket;   //Auction 패킷.
	AuPacket			m_csInvenPos;
	AuPacket			m_csSalesBoxPos;  //SalesBox Grid 위치
	AuPacket			m_csItemInfo;  //Item정보.
	AuPacket			m_csSearchInfo; //검색 조건.
	AuPacket			m_csCategoryCount; //카테고리 갯수

	AgsmAuctionServer	*m_pagsmAuctionServer;

	ApAdmin				m_csCIDManager;

public:
	AgsmAuctionClient();
	~AgsmAuctionClient();

	//옥션서버에서 캐릭터ID와 dpnid를 등록한다. ondestroy때 CID를 받기 위해서이다.
	BOOL ProcessAddCIDToAuctionServer( INT32 lSellerID, UINT32 ulNID );

	UINT32 GetulNID( INT32 lCID );

	static BOOL	CBAuctionSell(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionSold(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionBuy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionBackoutToInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionBackOutToSalesBox(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL ProcessGetItemCount( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID );
	BOOL ProcessGetItemInfo( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID );
	BOOL ProcessSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos );
	BOOL ProcessBuyItem( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID );
	BOOL ProcessBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos, INT8 lOperation );
	BOOL ProcessSearchByPage( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID, void *pvSearchInfo );

	BOOL SendItemCount( INT32 lBuyerID, INT32 lItemTypeID, INT32 lItemCount );
	BOOL SendGetItemInfo();

	//옥션에 아이템을 판다고 Client에 알린다.(Sales Box에 물건이 들어가는거다.)
	BOOL SendSellItem( INT32 lSellerID, INT32 lItemTypeID );
	//옥션에 올린 아이템이 팔렸다고 Client에 알린다
	BOOL SendSoldItem( INT32 lSellerID, INT32 lItemTypeID );
	///옥션에 올라온 아이템을 샀다고 클라이언트에 알린다.
	BOOL SendBuyItem( INT32 lBuyerID, INT32 lItemTypeID );
	//옥션에 올린 아이템을 유찰시킨다고 Client에 알린다.(Sales Box에서 물건을 뺀다.)
	BOOL SendBackOutItem( INT32 lSellerID, INT32 lItemTypeID, INT8 lOperation );
	BOOL SendSearchByPage( INT32 lBuyerID, INT32 lIndex, char *pstrSellerID, UINT64 ullDBID, INT32 lItemTypeID, INT32 lPrice, char *pstrDate );
	
	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();

	BOOL ParseInvenPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol);
	BOOL ParseSalesBoxPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol);
	BOOL ParseSearchInfo( PVOID pvSearchInfo, bool *pbSearchByMyLevel, INT32 *plPage );

	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);
	BOOL OnDisconnect(INT32 lAccountID);
};

#endif
