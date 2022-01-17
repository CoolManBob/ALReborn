/*=============================================================

	AgsmAuctionRelay.cpp

=============================================================*/


#include "AgsmAuctionRelay.h"
#include "ApMemoryTracker.h"


/************************************************************/
/*		The Implementation of AgsmAuctionRelay class		*/
/************************************************************/
//
AgsmAuctionRelay::AgsmAuctionRelay()
	{
	m_pAgsmServerManager	= NULL;
	m_pAgsmDatabasePool		= NULL;
	m_pAgsmInterServerLink	= NULL;
	m_pAgsmCharacter		= NULL;

	SetModuleName("AgsmAuctionRelay");
	SetPacketType(AGSMAUCTION_PACKET_TYPE);

	InitPacket();
	InitPacketSelect();
	InitPacketSell();
	InitPacketCancel();
	InitPacketConfirm();
	InitPacketBuy();
	}


AgsmAuctionRelay::~AgsmAuctionRelay()
	{
	}




//	Packet Initialization
//========================================
//
void AgsmAuctionRelay::InitPacket()
	{
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT16,	1,		// nParam
							AUTYPE_PACKET,	1,		// Embedded Packet
							AUTYPE_END,		0
						    );
	}


void AgsmAuctionRelay::InitPacketSelect()
	{
	m_csPacketSelect.SetFlagLength(sizeof(INT8));
	m_csPacketSelect.SetFieldType(AUTYPE_INT16,		1,		// operation
								  AUTYPE_INT32,		1,		// CID
								  AUTYPE_INT16,		1,		// Code
								  AUTYPE_INT32,		1,		// item tid
								  AUTYPE_UINT64,	1,		// doc id.
								  AUTYPE_INT16,		1,		// flag
								  AUTYPE_PACKET,	1,		// embedded rowset packet
								  AUTYPE_END,		0
								  );

	m_csPacketRowset.SetFlagLength(sizeof(INT16));
	m_csPacketRowset.SetFieldType(//AUTYPE_INT16,		1,		// eAgsmRelay2Operation
								  AUTYPE_INT32,			1,		// Query Index
								  AUTYPE_MEMORY_BLOCK,	1,		// Query Text
								  AUTYPE_MEMORY_BLOCK,	1,		// Headers
								  AUTYPE_UINT32,		1,		// No. of Rows
								  AUTYPE_UINT32,		1,		// No. of Cols
								  AUTYPE_UINT32,		1,		// Row Buffer Size
								  AUTYPE_MEMORY_BLOCK,	1,		// Buffer
								  AUTYPE_MEMORY_BLOCK,	1,		// Offset
								  AUTYPE_END,			0
								  );

	m_csPacketSelectSales.SetFlagLength(sizeof(INT8));
	m_csPacketSelectSales.SetFieldType(AUTYPE_INT16,		1,							// operation
									   AUTYPE_INT32,		1,							// CID
									   AUTYPE_INT16,		1,							// Code
									   AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,	// character id
									   AUTYPE_PACKET,		1,							// embedded rowset packet
									   AUTYPE_END,			0
									   );

	m_csPacketSelectSalesCount.SetFlagLength(sizeof(INT8));
	m_csPacketSelectSalesCount.SetFieldType(AUTYPE_INT16,		1,		// operation
										    AUTYPE_INT32,		1,		// CID
										    AUTYPE_INT16,		1,		// Code
										    AUTYPE_INT16,		1,		// Total packet count
										    AUTYPE_PACKET,		1,		// embedded rowset packet
										    AUTYPE_END,			0
										    );
	}


void AgsmAuctionRelay::InitPacketSell()
	{
	m_csPacketSell.SetFlagLength(sizeof(INT16));
	m_csPacketSell.SetFieldType(AUTYPE_INT16,		1,							// operation
								AUTYPE_INT32,		1,							// CID
								AUTYPE_INT16,		1,							// code
								AUTYPE_UINT64,		1,							// item seq.
								AUTYPE_INT32,		1,							// unit price
								AUTYPE_INT16,		1,							// quantity
								AUTYPE_INT16,		1,							// item status
								AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,	// char id
								AUTYPE_INT32,		1,							// item id
								AUTYPE_UINT64,		1,							// doc. id
								AUTYPE_CHAR,		_MAX_DATETIME_LENGTH + 1,	// date
								AUTYPE_END,			0
								);
	}


void AgsmAuctionRelay::InitPacketCancel()
	{
	m_csPacketCancel.SetFlagLength(sizeof(INT16));
	m_csPacketCancel.SetFieldType(AUTYPE_INT16,		1,							// operation
								  AUTYPE_INT32,		1,							// CID
								  AUTYPE_INT16,		1,							// code
								  AUTYPE_INT32,		1,							// sales ID.
								  AUTYPE_UINT64,	1,							// doc. ID(auction id)
								  AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,	// character
								  AUTYPE_END,		0
								  );
	}


void AgsmAuctionRelay::InitPacketConfirm()
	{
	m_csPacketConfirm.SetFlagLength(sizeof(INT16));
	m_csPacketConfirm.SetFieldType(AUTYPE_INT16,		1,							// eAgsmDatabaseOperation
								   AUTYPE_INT32,		1,							// CID
								   AUTYPE_INT16,		1,							// Code
								   AUTYPE_INT32,		1,							// Sales ID.
								   AUTYPE_UINT64,		1,							// Doc. ID(auction id)
								   AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,	// Character ID
								   AUTYPE_END,			0
								   );
	}


void AgsmAuctionRelay::InitPacketBuy()
	{
	m_csPacketBuy.SetFlagLength(sizeof(INT32));
	m_csPacketBuy.SetFieldType(AUTYPE_INT16,	1,									// operation
							   AUTYPE_INT32,	1,									// CID
							   AUTYPE_INT16,	1,									// Code
							   AUTYPE_INT64,	1,									// doc. id
							   AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,			// buyer
							   AUTYPE_INT64,	1,									// buyer's money
							   AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,			// seller
							   AUTYPE_INT32,	1,									// unit price
							   AUTYPE_UINT64,	1,									// item seq
							   AUTYPE_INT32,	1,									// Item TID
							   AUTYPE_INT16,	1,									// quantity
							   AUTYPE_INT8,		1,									// Need Level
							   AUTYPE_CHAR,		_MAX_DATETIME_LENGTH + 1,			// Own Time
							   AUTYPE_CHAR,		_MAX_CONVERT_HISTORY_LENGTH + 1,	// Convert History
							   AUTYPE_INT32,	1,									// Durability
							   AUTYPE_INT32,	1,									// Max Durability
							   AUTYPE_CHAR,		_MAX_OPTION_LENGTH + 1,				// option
							   AUTYPE_CHAR,		_MAX_OPTION_LENGTH + 1,				// skill plus
							   AUTYPE_END,		0
							   );
	}




//	AgsModule inherited
//======================================
//
BOOL AgsmAuctionRelay::OnAddModule()
	{
	// get related modules
	m_pAgsmServerManager	= (AgsmServerManager *) GetModule("AgsmServerManager2");
	m_pAgsmInterServerLink	= (AgsmInterServerLink *) GetModule("AgsmInterServerLink");
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgsmCharacter		= (AgsmCharacter *) GetModule("AgsmCharacter");

	if (!m_pAgsmServerManager || !m_pAgsmInterServerLink)
		return FALSE;

	return TRUE;
	}


BOOL AgsmAuctionRelay::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	INT16		nParam = -1;
	PVOID		pvPacketEmb = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&nParam,
						&pvPacketEmb
						);

	if (!pvPacketEmb)
		return FALSE;

	switch (nParam)
		{
		case AGSMAUCTIONRELAY_PARAM_SELECT:
			OnParamSelect(nParam, pvPacketEmb, ulNID);
			break;
		
		case AGSMAUCTIONRELAY_PARAM_SELECT_SALES:
			OnParamSelectSales(nParam, pvPacketEmb, ulNID);
			break;
		
		case AGSMAUCTIONRELAY_PARAM_SELECT_SALES_COUNT:
			OnParamSelectSalesCount(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMAUCTIONRELAY_PARAM_SELL:
			OnParamSell(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMAUCTIONRELAY_PARAM_CANCEL:
			OnParamCancel(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMAUCTIONRELAY_PARAM_CONFIRM:
			OnParamConfirm(nParam, pvPacketEmb, ulNID);
			break;

		case AGSMAUCTIONRELAY_PARAM_BUY:
			OnParamBuy(nParam, pvPacketEmb, ulNID);
			break;

		default:
			TRACE("!!!Error : Invalid parameter in AgsmAuctionRelay::OnReceive()\n");
			break;
		}

	return TRUE;
	}




//	Packet processing
//======================================
//
BOOL AgsmAuctionRelay::OnParamSell(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdAuctionRelaySell	csParam;

	CHAR *pszChar = NULL;
	CHAR *pszDate = NULL;
	m_csPacketSell.GetField(FALSE, pvPacket, 0,
						    &csParam.m_eOperation,			// operation
							&csParam.m_lCID,				// CID
							&csParam.m_nCode,				// code
							&csParam.m_ullItemSeq,			// item seq.
							&csParam.m_lPrice,				// unit price
							&csParam.m_nQuantity,			// quantity
							&csParam.m_nItemStatus,			// item status
							&pszChar,						// char id
							&csParam.m_lItemID,				// item id
							&csParam.m_ullDocID,			// doc. id
							&pszDate						// date	
						    );								
	_tcsncpy(csParam.m_szChar, pszChar ? pszChar : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(csParam.m_szDate, pszDate ? pszDate : _T(""), _MAX_DATETIME_LENGTH);
	_i64toa(csParam.m_ullItemSeq, csParam.m_szItemSeq, 10);
	_i64toa(csParam.m_ullDocID, csParam.m_szDocID, 10);
	csParam.m_ulNID = ulNID;	

	BOOL bResult = EnumCallback(AGSMAUCTIONRELAY_CB_SELL, &csParam, (PVOID) nParam);
	if(bResult == FALSE)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuctionRelay::OnParamSell(), Callback returned FALSE. Operation:%d, ItemSeq:%I64d, DocID:%I64d, ulNID:%d\n",
									csParam.m_eOperation,
									csParam.m_ullItemSeq,
									csParam.m_ullDocID,
									ulNID);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		}
	return bResult;
	}


BOOL AgsmAuctionRelay::OnParamCancel(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdAuctionRelayCancel	csParam;

	CHAR *pszChar = NULL;
	m_csPacketCancel.GetField(FALSE, pvPacket, 0,
							  &csParam.m_eOperation,		// operation
							  &csParam.m_lCID,				// CID
							  &csParam.m_nCode,				// code
							  &csParam.m_lSalesID,			// sales ID.
							  &csParam.m_ullDocID,			// doc. ID(auction id)
							  &pszChar						// character
							  );
	_i64toa(csParam.m_ullDocID, csParam.m_szDocID, 10);
	_tcsncpy(csParam.m_szChar, pszChar ? pszChar : _T(""), _MAX_CHARNAME_LENGTH);
	csParam.m_ulNID = ulNID;	

	return EnumCallback(AGSMAUCTIONRELAY_CB_CANCEL, &csParam, (PVOID) nParam);
	}


BOOL AgsmAuctionRelay::OnParamConfirm(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdAuctionRelayConfirm	csParam;

	CHAR *pszChar = NULL;
	m_csPacketConfirm.GetField(FALSE, pvPacket, 0,
							   &csParam.m_eOperation,			// eAgsmDatabaseOperation
							   &csParam.m_lCID,					// CID
							   &csParam.m_nCode,				// Code
							   &csParam.m_lSalesID,				// Sales ID.
							   &csParam.m_ullDocID,				// Doc. ID(auction id)
							   &pszChar							// Character ID
							   );								
	_i64toa(csParam.m_ullDocID, csParam.m_szDocID, 10);
	_tcsncpy(csParam.m_szChar, pszChar ? pszChar : _T(""), _MAX_CHARNAME_LENGTH);
	csParam.m_ulNID = ulNID;	

	return EnumCallback(AGSMAUCTIONRELAY_CB_CONFIRM, &csParam, (PVOID) nParam);
	}


BOOL AgsmAuctionRelay::OnParamBuy(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdAuctionRelayBuy	csParam;
	
	CHAR *pszBuyer = NULL;
	CHAR *pszSeller = NULL;
	CHAR *pszPosition = NULL;
	CHAR *pszOwnTime = NULL;
	CHAR *pszConvHistory = NULL;
	CHAR *pszOption = NULL;
	CHAR *pszSkillPlus = NULL;
	m_csPacketBuy.GetField(FALSE, pvPacket, 0,
						   &csParam.m_eOperation,				// operation
						   &csParam.m_lCID,						// CID
						   &csParam.m_nCode,					// Code
						   &csParam.m_ullDocID,					// doc. id
						   &pszBuyer,							// buyer
						   &csParam.m_llMoney,					// buyer's money
						   &pszSeller,							// seller
						   &csParam.m_lPrice,					// unit price
						   &csParam.m_ullItemSeq,				// item seq
						   &csParam.m_lItemTID,					// Item TID
						   &csParam.m_nQuantity,				// quantity
						   &csParam.m_cNeedLevel,				// Need Level
						   &pszOwnTime,							// Own Time
						   &pszConvHistory,						// Convert History
						   &csParam.m_lDurability,				// Durability
						   &csParam.m_lMaxDurability,			// Max Durability
						   &pszOption,							// option
						   &pszSkillPlus						// skill plus
						   );
	_i64toa(csParam.m_ullDocID, csParam.m_szDocID, 10);
	_i64toa(csParam.m_llMoney, csParam.m_szMoney, 10);
	_i64toa(csParam.m_ullItemSeq, csParam.m_szItemSeq, 10);
	_tcsncpy(csParam.m_szBuyer, pszBuyer ? pszBuyer : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(csParam.m_szSeller, pszSeller ? pszSeller : _T(""), _MAX_CHARNAME_LENGTH);
	_tcsncpy(csParam.m_szOwnTime, pszOwnTime ? pszOwnTime : _T(""), _MAX_DATETIME_LENGTH);
	_tcsncpy(csParam.m_szConvHistory, pszConvHistory ? pszConvHistory : _T(""), _MAX_CONVERT_HISTORY_LENGTH);
	_tcsncpy(csParam.m_szOption, pszOption ? pszOption : _T(""), _MAX_OPTION_LENGTH);
	_tcsncpy(csParam.m_szSkillPlus, pszSkillPlus ? pszSkillPlus : _T(""), _MAX_OPTION_LENGTH);
	csParam.m_ulNID = ulNID;

	return EnumCallback(AGSMAUCTIONRELAY_CB_BUY, &csParam, (PVOID) nParam);
	}


BOOL AgsmAuctionRelay::OnParamSelect(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdAuctionRelaySelect	csParam;

	m_csPacketSelect.GetField(FALSE, pvPacket, 0,
							  &csParam.m_eOperation,			// operation
							  &csParam.m_lCID,					// CID
							  &csParam.m_nCode,					// Code
							  &csParam.m_lItemTID,				// item tid
							  &csParam.m_ullDocID,				// doc id.
							  &csParam.m_nFlag,					// flag
							  &csParam.m_pvPacket				// embedded rowset packet
							  );
	csParam.ullDocID = (csParam.m_ullDocID > UINT32(-1)) ? UINT32(-1) : UINT32(csParam.m_ullDocID);
	csParam.m_ulNID = ulNID;	

	return EnumCallback(AGSMAUCTIONRELAY_CB_SELECT, &csParam, (PVOID) nParam);
	}


BOOL AgsmAuctionRelay::OnParamSelectSales(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdAuctionRelaySelectSales	csParam;

	CHAR *pszChar = NULL;

	m_csPacketSelectSales.GetField(FALSE, pvPacket, 0,
							  &csParam.m_eOperation,			// operation
							  &csParam.m_lCID,					// CID
							  &csParam.m_nCode,					// Code
							  &pszChar,							// character id
							  &csParam.m_pvPacket				// embedded rowset packet
							  );
	if (!pszChar)
		return FALSE;
								
	_tcsncpy(csParam.m_szChar, pszChar, _MAX_CHARNAME_LENGTH);
	csParam.m_ulNID = ulNID;	

	return EnumCallback(AGSMAUCTIONRELAY_CB_SELECT_SALES, &csParam, (PVOID) nParam);
	}


BOOL AgsmAuctionRelay::OnParamSelectSalesCount(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdAuctionRelaySelectSalesCount csParam;

	m_csPacketSelectSalesCount.GetField(FALSE, pvPacket, 0,
							  &csParam.m_eOperation,				// operation
							  &csParam.m_lCID,						// CID
							  &csParam.m_nCode,						// Code
							  &csParam.m_nTotalPacket,				// Total packet count
							  &csParam.m_pvPacket					// embedded rowset packet
							  );
	csParam.m_ulNID = ulNID;

	return EnumCallback(AGSMAUCTIONRELAY_CB_SELECT_SALES_COUNT, &csParam, (PVOID) nParam);
	}




//		Send packet (External)
//======================================================
//
BOOL AgsmAuctionRelay::SendSelect(INT32 lCID, INT32 lItemTID, UINT64 ullDocID, INT16 nFlag)
	{
	AgsdServer* pAuctionServer = m_pAgsmServerManager->GetRelayServer();
	if (!pAuctionServer || 0 == pAuctionServer->m_dpnidServer)
		{
		return FALSE;
		}

	AgsdAuctionRelaySelect csAuctionRelay;

	csAuctionRelay.m_eOperation = AGSMDATABASE_OPERATION_SELECT;
	csAuctionRelay.m_lCID = lCID;
	csAuctionRelay.m_lItemTID = lItemTID;
	csAuctionRelay.m_ullDocID = ullDocID;
	csAuctionRelay.m_nFlag = nFlag;
	csAuctionRelay.m_pvPacket = NULL;	//	OUT
	
	return SendSelect(&csAuctionRelay, pAuctionServer->m_dpnidServer);
	}


BOOL AgsmAuctionRelay::SendSelectSales(INT32 lCID, CHAR *pszChar)
	{
	//AgsdServer* pAuctionServer = m_pAgsmServerManager->GetDealServer();
	AgsdServer* pAuctionServer = m_pAgsmServerManager->GetRelayServer();
	if (!pAuctionServer || 0 == pAuctionServer->m_dpnidServer || !pszChar)
		{
		return FALSE;
		}

	AgsdAuctionRelaySelectSales csAuctionRelay;

	csAuctionRelay.m_eOperation = AGSMDATABASE_OPERATION_SELECT;
	csAuctionRelay.m_lCID = lCID;
	_tcsncpy(csAuctionRelay.m_szChar, pszChar, _MAX_CHARNAME_LENGTH);
	csAuctionRelay.m_pvPacket = NULL;	//	OUT
	
	return SendSelectSales(&csAuctionRelay, pAuctionServer->m_dpnidServer);
	}


BOOL AgsmAuctionRelay::SendSelectSalesCount()
	{
	AgsdServer* pAuctionServer = m_pAgsmServerManager->GetRelayServer();
	if (!pAuctionServer || 0 == pAuctionServer->m_dpnidServer)
		{
		return FALSE;
		}

	AgsdAuctionRelaySelectSalesCount csAuctionRelay;

	csAuctionRelay.m_eOperation = AGSMDATABASE_OPERATION_SELECT;
	csAuctionRelay.m_lCID = 0;
	csAuctionRelay.m_nTotalPacket = 0;	// OUT
	csAuctionRelay.m_pvPacket = NULL;	// OUT
	
	return SendSelectSalesCount(&csAuctionRelay, pAuctionServer->m_dpnidServer);
	}


BOOL AgsmAuctionRelay::SendSell(INT32 lCID, UINT64 ullItemSeq, INT32 lPrice, INT16 nQuantity,
								INT16 nItemStatus, CHAR *pszChar, INT32 lItemID)
	{
	AgsdServer* pAuctionServer = m_pAgsmServerManager->GetRelayServer();
	if (!pAuctionServer || 0 == pAuctionServer->m_dpnidServer)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuctionRelay::SendSell(), Can't find the RelayServer\n");
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}

	AgsdAuctionRelaySell csAuctionRelay;
	
	csAuctionRelay.m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;
	csAuctionRelay.m_lCID = lCID;
	csAuctionRelay.m_ullItemSeq = ullItemSeq;
	csAuctionRelay.m_lPrice = lPrice;
	csAuctionRelay.m_nQuantity = nQuantity;	
	csAuctionRelay.m_nItemStatus = nItemStatus;
	if (pszChar)
		_tcsncpy(csAuctionRelay.m_szChar, pszChar, _MAX_CHARNAME_LENGTH);
	csAuctionRelay.m_lItemID = lItemID;

	INT16	nPacketLength = 0;
	BOOL	bResult = TRUE;
	INT16	nParam = AGSMAUCTIONRELAY_PARAM_SELL;

	PVOID pvPacketEmb = m_csPacketSell.MakePacket(FALSE, &nPacketLength, 0,
												&csAuctionRelay.m_eOperation,			// operation
												&csAuctionRelay.m_lCID,					// CID
												&csAuctionRelay.m_nCode,				// code
												&csAuctionRelay.m_ullItemSeq,			// item seq.
												&csAuctionRelay.m_lPrice,				// unit price
												&csAuctionRelay.m_nQuantity,			// quantity
												&csAuctionRelay.m_nItemStatus,			// item status
												csAuctionRelay.m_szChar,				// char id
												&csAuctionRelay.m_lItemID,				// item id
												&csAuctionRelay.m_ullDocID,				// doc. id
												&csAuctionRelay.m_szDate				// date
												);										
		
	if (NULL == pvPacketEmb)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuctionRelay::SendSell(), pvPacketEmb is NULL\n");
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}	

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
										   &nParam, pvPacketEmb);
	m_csPacketSell.FreePacket(pvPacketEmb);	

	if (FALSE == EnumCallback(AGSMAUCTIONRELAY_CB_BUFFERING_SEND, pvPacket, (PVOID) nPacketLength))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuctionRelay::SendSell(), EnumCallback(AGSMAUCTIONRELAY_CB_BUFFERING_SEND) is FALSE PacketLength:%d\n", nPacketLength);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		bResult = MakeAndSendPacket(pvPacketEmb, nParam, pAuctionServer->m_dpnidServer);
		}

	m_csPacket.FreePacket(pvPacket);

	if(bResult == FALSE)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuctionRelay::SendSell(), bResult is FALSE\n");
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		}

	return bResult;
	}

BOOL AgsmAuctionRelay::SendCancel(INT32 lCID, INT32 lSalesID, UINT64 ullDocID, CHAR *pszChar)
	{
	AgsdServer* pAuctionServer = m_pAgsmServerManager->GetRelayServer();
	if (!pAuctionServer || 0 == pAuctionServer->m_dpnidServer)
		{
		return FALSE;
		}

	AgsdAuctionRelayCancel csAuctionRelay;

	csAuctionRelay.m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;
	csAuctionRelay.m_lCID = lCID;
	csAuctionRelay.m_lSalesID = lSalesID;
	csAuctionRelay.m_ullDocID = ullDocID;
	_tcsncpy(csAuctionRelay.m_szChar, pszChar, _MAX_CHARNAME_LENGTH);

	return SendCancel(&csAuctionRelay, pAuctionServer->m_dpnidServer);
	}


BOOL AgsmAuctionRelay::SendConfirm(INT32 lCID, INT32 lSalesID, UINT64 ullDocID, CHAR *pszChar)
	{
	AgsdServer* pAuctionServer = m_pAgsmServerManager->GetRelayServer();
	if (!pAuctionServer || 0 == pAuctionServer->m_dpnidServer)
		{
		return FALSE;
		}

	AgsdAuctionRelayConfirm csAuctionRelay;

	csAuctionRelay.m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;
	csAuctionRelay.m_lCID = lCID;
	csAuctionRelay.m_lSalesID = lSalesID;
	csAuctionRelay.m_ullDocID = ullDocID;
	_tcsncpy(csAuctionRelay.m_szChar, pszChar, _MAX_CHARNAME_LENGTH);

	return SendConfirm(&csAuctionRelay, pAuctionServer->m_dpnidServer);
	}


BOOL AgsmAuctionRelay::SendBuy(INT32 lCID, UINT64 ullDocID, CHAR *pszBuyer, INT64 llMoney)
	{
	AgsdServer* pAuctionServer = m_pAgsmServerManager->GetRelayServer();
	if (!pAuctionServer || 0 == pAuctionServer->m_dpnidServer)
		{
		return FALSE;
		}

	AgsdAuctionRelayBuy csAuctionRelay;

	csAuctionRelay.m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;
	csAuctionRelay.m_lCID = lCID;
	csAuctionRelay.m_ullDocID = ullDocID;
	_tcsncpy(csAuctionRelay.m_szBuyer, pszBuyer, _MAX_CHARNAME_LENGTH);
	csAuctionRelay.m_llMoney = llMoney;

	return SendBuy(&csAuctionRelay, pAuctionServer->m_dpnidServer);
	}


//	Send Packet (Internal)
//===========================================================
//
BOOL AgsmAuctionRelay::SendSelect(AgsdAuctionRelaySelect *pAgsdAuctionRelay, UINT32 ulNID)
	{
	ASSERT(NULL != pAgsdAuctionRelay);

	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacketEmb = m_csPacketSelect.MakePacket(FALSE, &nPacketLength, 0,
													&pAgsdAuctionRelay->m_eOperation,			// operation
													&pAgsdAuctionRelay->m_lCID,					// CID
													&pAgsdAuctionRelay->m_nCode,				// Code
													&pAgsdAuctionRelay->m_lItemTID,				// item tid
													&pAgsdAuctionRelay->m_ullDocID,				// doc id.
													&pAgsdAuctionRelay->m_nFlag,				// flag
													pAgsdAuctionRelay->m_pvPacket				// embedded rowset packet
													);
	
	if (pvPacketEmb)
		{
		bResult = MakeAndSendPacket(pvPacketEmb, AGSMAUCTIONRELAY_PARAM_SELECT, ulNID);
		m_csPacketSelect.FreePacket(pvPacketEmb);
		}

	return bResult;
	}


BOOL AgsmAuctionRelay::SendSelectSales(AgsdAuctionRelaySelectSales *pAgsdAuctionRelay, UINT32 ulNID)
	{
	ASSERT(NULL != pAgsdAuctionRelay);

	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacketEmb = m_csPacketSelectSales.MakePacket(FALSE, &nPacketLength, 0,
													&pAgsdAuctionRelay->m_eOperation,		// operation
													&pAgsdAuctionRelay->m_lCID,				// CID
													&pAgsdAuctionRelay->m_nCode,			// Code
													pAgsdAuctionRelay->m_szChar,			// character id
													pAgsdAuctionRelay->m_pvPacket			// embedded rowset packet
													);
	
	if (pvPacketEmb)
		{
		bResult = MakeAndSendPacket(pvPacketEmb, AGSMAUCTIONRELAY_PARAM_SELECT_SALES, ulNID);
		m_csPacketSelectSales.FreePacket(pvPacketEmb);
		}

	return bResult;
	}


BOOL AgsmAuctionRelay::SendSelectSalesCount(AgsdAuctionRelaySelectSalesCount *pAgsdAuctionRelay, UINT32 ulNID)
	{
	ASSERT(NULL != pAgsdAuctionRelay);

	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacketEmb = m_csPacketSelectSalesCount.MakePacket(FALSE, &nPacketLength, 0,
													&pAgsdAuctionRelay->m_eOperation,			// operation
													&pAgsdAuctionRelay->m_lCID,					// CID
													&pAgsdAuctionRelay->m_nCode,				// Code
													&pAgsdAuctionRelay->m_nTotalPacket,			// Total packet count
													pAgsdAuctionRelay->m_pvPacket				// embedded rowset packet
													);
	
	if (pvPacketEmb)
		{
		bResult = MakeAndSendPacket(pvPacketEmb, AGSMAUCTIONRELAY_PARAM_SELECT_SALES_COUNT, ulNID);
		m_csPacketSelectSalesCount.FreePacket(pvPacketEmb);
		}

	return bResult;
	}


BOOL AgsmAuctionRelay::SendSell(AgsdAuctionRelaySell *pAgsdAuctionRelay, UINT32 ulNID)
	{
	ASSERT(NULL != pAgsdAuctionRelay);

	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacketEmb = m_csPacketSell.MakePacket(FALSE, &nPacketLength, 0,
												  &pAgsdAuctionRelay->m_eOperation,			// operation
												  &pAgsdAuctionRelay->m_lCID,				// CID
												  &pAgsdAuctionRelay->m_nCode,				// code
												  &pAgsdAuctionRelay->m_ullItemSeq,			// item seq.
												  &pAgsdAuctionRelay->m_lPrice,				// unit price
												  &pAgsdAuctionRelay->m_nQuantity,			// quantity
												  &pAgsdAuctionRelay->m_nItemStatus,		// item status
												  pAgsdAuctionRelay->m_szChar,				// char id
												  &pAgsdAuctionRelay->m_lItemID,			// item id
												  &pAgsdAuctionRelay->m_ullDocID,			// doc. id
												  &pAgsdAuctionRelay->m_szDate				// date
												  );										
	
	if (pvPacketEmb)
		{
		bResult = MakeAndSendPacket(pvPacketEmb, AGSMAUCTIONRELAY_PARAM_SELL, ulNID);
		m_csPacketSell.FreePacket(pvPacketEmb);
		}

	if(bResult == FALSE)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuctionRelay::SendSell(), bResult is FALSE. Operation:%d, ItemSeq:%I64d, DocID:%I64d, ulNID:%d\n",
									(INT32)pAgsdAuctionRelay->m_eOperation,
									pAgsdAuctionRelay->m_ullItemSeq,
									pAgsdAuctionRelay->m_ullDocID,
									ulNID);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		}

	return bResult;
	}


BOOL AgsmAuctionRelay::SendCancel(AgsdAuctionRelayCancel *pAgsdAuctionRelay, UINT32 ulNID)
	{
	ASSERT(NULL != pAgsdAuctionRelay);

	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacketEmb = m_csPacketCancel.MakePacket(FALSE, &nPacketLength, 0,
													&pAgsdAuctionRelay->m_eOperation,			// operation
													&pAgsdAuctionRelay->m_lCID,					// CID
													&pAgsdAuctionRelay->m_nCode,				// code
													&pAgsdAuctionRelay->m_lSalesID,				// sales ID.
													&pAgsdAuctionRelay->m_ullDocID,				// doc. ID(auction id)
													pAgsdAuctionRelay->m_szChar					// character
													);
	
	if (pvPacketEmb)
		{
		bResult = MakeAndSendPacket(pvPacketEmb, AGSMAUCTIONRELAY_PARAM_CANCEL, ulNID);
		m_csPacketCancel.FreePacket(pvPacketEmb);
		}

	return bResult;
	}


BOOL AgsmAuctionRelay::SendConfirm(AgsdAuctionRelayConfirm *pAgsdAuctionRelay, UINT32 ulNID)
	{
	ASSERT(NULL != pAgsdAuctionRelay);

	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacketEmb = m_csPacketConfirm.MakePacket(FALSE, &nPacketLength, 0,
													 &pAgsdAuctionRelay->m_eOperation,			// eAgsmDatabaseOperation
													 &pAgsdAuctionRelay->m_lCID,				// CID
													 &pAgsdAuctionRelay->m_nCode,				// Code
													 &pAgsdAuctionRelay->m_lSalesID,			// Sales ID.
													 &pAgsdAuctionRelay->m_ullDocID,			// Doc. ID(auction id)
													 pAgsdAuctionRelay->m_szChar				// Character ID
													 );											
	
	if (pvPacketEmb)
		{
		bResult = MakeAndSendPacket(pvPacketEmb, AGSMAUCTIONRELAY_PARAM_CONFIRM, ulNID);
		m_csPacketConfirm.FreePacket(pvPacketEmb);
		}

	return bResult;
	}


BOOL AgsmAuctionRelay::SendBuy(AgsdAuctionRelayBuy *pAgsdAuctionRelay, UINT32 ulNID)
	{
	ASSERT(NULL != pAgsdAuctionRelay);

	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacketEmb = m_csPacketBuy.MakePacket(FALSE, &nPacketLength, 0,
												 &pAgsdAuctionRelay->m_eOperation,			// operation
												 &pAgsdAuctionRelay->m_lCID,				// CID
												 &pAgsdAuctionRelay->m_nCode,				// Code
												 &pAgsdAuctionRelay->m_ullDocID,			// doc. id
												 pAgsdAuctionRelay->m_szBuyer,				// buyer
												 &pAgsdAuctionRelay->m_llMoney,				// buyer's money
												 pAgsdAuctionRelay->m_szSeller,				// seller
												 &pAgsdAuctionRelay->m_lPrice,				// unit price
												 &pAgsdAuctionRelay->m_ullItemSeq,			// item seq
												 &pAgsdAuctionRelay->m_lItemTID,			// Item TID
												 &pAgsdAuctionRelay->m_nQuantity,			// quantity
												 &pAgsdAuctionRelay->m_cNeedLevel,			// Need Level
												 pAgsdAuctionRelay->m_szOwnTime,			// Own Time
												 pAgsdAuctionRelay->m_szConvHistory,		// Convert History
												 &pAgsdAuctionRelay->m_lDurability,			// Durability
												 &pAgsdAuctionRelay->m_lMaxDurability,		// Max Durability
												 pAgsdAuctionRelay->m_szOption,				// option
												 pAgsdAuctionRelay->m_szSkillPlus			// skill plus
												 );
	
	if (pvPacketEmb)
		{
		bResult = MakeAndSendPacket(pvPacketEmb, AGSMAUCTIONRELAY_PARAM_BUY, ulNID);
		m_csPacketBuy.FreePacket(pvPacketEmb);
		}

	return bResult;
	}


BOOL AgsmAuctionRelay::MakeAndSendPacket(PVOID pvPacketEmb, INT16 nParam, UINT32 ulNID)
	{
	INT16	nPacketLength = 0;
	BOOL	bResult = FALSE;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
										   &nParam, pvPacketEmb);
	if (pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}




//	Callback setting
//========================================================
//
BOOL AgsmAuctionRelay::SetCallbackSell(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_SELL, pfCallback, pClass);
	}


BOOL AgsmAuctionRelay::SetCallbackCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_CANCEL, pfCallback, pClass);
	}


BOOL AgsmAuctionRelay::SetCallbackConfirm(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_CONFIRM, pfCallback, pClass);
	}


BOOL AgsmAuctionRelay::SetCallbackBuy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_BUY, pfCallback, pClass);
	}


BOOL AgsmAuctionRelay::SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_SELECT, pfCallback, pClass);
	}


BOOL AgsmAuctionRelay::SetCallbackSelectSales(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_SELECT_SALES, pfCallback, pClass);
	}


BOOL AgsmAuctionRelay::SetCallbackSelectSalesCount(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_SELECT_SALES_COUNT, pfCallback, pClass);
	}


BOOL AgsmAuctionRelay::SetCallbackBufferingSend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMAUCTIONRELAY_CB_BUFFERING_SEND, pfCallback, pClass);
	}




//	Callback
//=============================================================
//
BOOL AgsmAuctionRelay::CBOperation(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	INT16 nParam = (INT16) pCustData;

	AgsdAuctionRelay *pAuctionRelay = NULL;
	AgsdQueryWithParam *pQuery = new AgsdQueryWithParam;

	switch (nParam)
		{
		case AGSMAUCTIONRELAY_PARAM_SELECT :
			{
			AgsdAuctionRelaySelect *pAgsdAuctionRelay = (AgsdAuctionRelaySelect *) pData;
			pAuctionRelay = new AgsdAuctionRelaySelect(pAgsdAuctionRelay);

			INT16 nIndex = nParam + pAuctionRelay->m_eOperation;
			pQuery->m_pParam = pAuctionRelay;
			pQuery->m_nIndex = nIndex;
			
			pQuery->SetCallback(AgsmAuctionRelay::CBSelectResult,
								AgsmAuctionRelay::CBSelectResult,
								pThis, pAuctionRelay);
			}
			break;

		case AGSMAUCTIONRELAY_PARAM_SELECT_SALES :
			{
			AgsdAuctionRelaySelectSales *pAgsdAuctionRelay = (AgsdAuctionRelaySelectSales *) pData;
			pAuctionRelay = new AgsdAuctionRelaySelectSales(pAgsdAuctionRelay);

			INT16 nIndex = nParam + pAuctionRelay->m_eOperation;
			pQuery->m_pParam = pAuctionRelay;
			pQuery->m_nIndex = nIndex;
			
			pQuery->SetCallback(AgsmAuctionRelay::CBSelectSalesResult,
								AgsmAuctionRelay::CBSelectSalesResult,
								pThis, pAuctionRelay);
			}
			break;

		case AGSMAUCTIONRELAY_PARAM_SELECT_SALES_COUNT :
			{
			printf("\nSales count request received...\n");
			
			AgsdAuctionRelaySelectSalesCount *pAgsdAuctionRelay = (AgsdAuctionRelaySelectSalesCount *) pData;
			pAuctionRelay = new AgsdAuctionRelaySelectSalesCount(pAgsdAuctionRelay);

			INT16 nIndex = nParam + pAuctionRelay->m_eOperation;
			pQuery->m_pParam = pAuctionRelay;
			pQuery->m_nIndex = nIndex;
			
			pQuery->SetCallback(AgsmAuctionRelay::CBSelectSalesCountResult,
								AgsmAuctionRelay::CBSelectSalesCountResult,
								pThis, pAuctionRelay);
			}
			break;

		case AGSMAUCTIONRELAY_PARAM_SELL :
			{
			AgsdAuctionRelaySell *pAgsdAuctionRelay = (AgsdAuctionRelaySell *) pData;
			pAuctionRelay = new AgsdAuctionRelaySell(pAgsdAuctionRelay);
			
			INT16 nIndex = nParam + pAuctionRelay->m_eOperation;
			pQuery->m_pParam = pAuctionRelay;
			pQuery->m_nIndex = nIndex;

			if (pThis->m_pAgsmDatabasePool)
				{
				pAuctionRelay->m_nForcedPoolIndex =
					(INT16) (pAgsdAuctionRelay->m_ullItemSeq % pThis->m_pAgsmDatabasePool->m_nTotalForced) +
					pThis->m_pAgsmDatabasePool->m_nForcedStart;
				}
			
			pQuery->SetCallback(AgsmAuctionRelay::CBSellResult,
								AgsmAuctionRelay::CBSellResult,
								pThis, pAuctionRelay);
			}
			break;

		case AGSMAUCTIONRELAY_PARAM_CANCEL :
			{
			AgsdAuctionRelayCancel *pAgsdAuctionRelay = (AgsdAuctionRelayCancel *) pData;
			pAuctionRelay = new AgsdAuctionRelayCancel(pAgsdAuctionRelay);

			INT16 nIndex = nParam + pAuctionRelay->m_eOperation;
			pQuery->m_pParam = pAuctionRelay;
			pQuery->m_nIndex = nIndex;

			pQuery->SetCallback(AgsmAuctionRelay::CBCancelResult,
								AgsmAuctionRelay::CBCancelResult,
								pThis, pAuctionRelay);
			}
			break;

		case AGSMAUCTIONRELAY_PARAM_CONFIRM :
			{
			AgsdAuctionRelayConfirm *pAgsdAuctionRelay = (AgsdAuctionRelayConfirm *) pData;
			pAuctionRelay = new AgsdAuctionRelayConfirm(pAgsdAuctionRelay);

			INT16 nIndex = nParam + pAuctionRelay->m_eOperation;
			pQuery->m_pParam = pAuctionRelay;
			pQuery->m_nIndex = nIndex;

			pQuery->SetCallback(AgsmAuctionRelay::CBConfirmResult,
								AgsmAuctionRelay::CBConfirmResult,
								pThis, pAuctionRelay);
			}
			break;

		case AGSMAUCTIONRELAY_PARAM_BUY :
			{
			AgsdAuctionRelayBuy *pAgsdAuctionRelay = (AgsdAuctionRelayBuy *) pData;
			pAuctionRelay = new AgsdAuctionRelayBuy((AgsdAuctionRelayBuy *)pAgsdAuctionRelay);

			INT16 nIndex = nParam + pAuctionRelay->m_eOperation;
			pQuery->m_pParam = pAuctionRelay;
			pQuery->m_nIndex = nIndex;

			pQuery->SetCallback(AgsmAuctionRelay::CBBuyResult,
								AgsmAuctionRelay::CBBuyResult,
								pThis, pAuctionRelay);
			}
			break;
		
		default:
			return FALSE;
		}

	//#############
	pAuctionRelay->m_nParam = nParam;

	BOOL bExecute = pThis->m_pAgsmDatabasePool->Execute(pQuery, pAuctionRelay->m_nForcedPoolIndex);
	if(nParam == AGSMAUCTIONRELAY_PARAM_SELL && bExecute == FALSE)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuctionRelay::CBOperation(), Execute Error. ForcedIndex:%d\n", pAuctionRelay->m_nForcedPoolIndex);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		}
	return bExecute;
	}


BOOL AgsmAuctionRelay::CBSellResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	AgsdAuctionRelaySell *pAgsdAuctionRelay = (AgsdAuctionRelaySell *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	// epilog
	pAgsdAuctionRelay->m_ullDocID = _atoi64(pAgsdAuctionRelay->m_szDocID);

	BOOL bResult = pThis->SendSell(pAgsdAuctionRelay, pAgsdAuctionRelay->m_ulNID);

	pAgsdAuctionRelay->Release();
	return bResult;
	}


BOOL AgsmAuctionRelay::CBCancelResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	AgsdAuctionRelayCancel *pAgsdAuctionRelay = (AgsdAuctionRelayCancel *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	// epilog
	pAgsdAuctionRelay->m_ullDocID = _atoi64(pAgsdAuctionRelay->m_szDocID);
	
	BOOL bResult = pThis->SendCancel(pAgsdAuctionRelay, pAgsdAuctionRelay->m_ulNID);
	
	pAgsdAuctionRelay->Release();
	return bResult;
	}


BOOL AgsmAuctionRelay::CBConfirmResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	AgsdAuctionRelayConfirm *pAgsdAuctionRelay = (AgsdAuctionRelayConfirm *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	// epilog
	pAgsdAuctionRelay->m_ullDocID = _atoi64(pAgsdAuctionRelay->m_szDocID);
	
	BOOL bResult = pThis->SendConfirm(pAgsdAuctionRelay, pAgsdAuctionRelay->m_ulNID);
	
	pAgsdAuctionRelay->Release();
	return bResult;
	}


BOOL AgsmAuctionRelay::CBBuyResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	AgsdAuctionRelayBuy *pAgsdAuctionRelay = (AgsdAuctionRelayBuy *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;
	
	// epilog
	pAgsdAuctionRelay->m_ullItemSeq = _atoi64(pAgsdAuctionRelay->m_szItemSeq);

	BOOL bResult = pThis->SendBuy(pAgsdAuctionRelay, pAgsdAuctionRelay->m_ulNID);

	pAgsdAuctionRelay->Release();
	return bResult;
	}


BOOL AgsmAuctionRelay::CBSelectResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData || !pData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	AgsdAuctionRelaySelect *pAgsdAuctionRelay = (AgsdAuctionRelaySelect *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	PVOID pvPacket = NULL;
	pThis->MakeRowsetPakcet(pRowset, &pvPacket);
	pAgsdAuctionRelay->m_pvPacket = pvPacket;

	BOOL bResult = pThis->SendSelect(pAgsdAuctionRelay, pAgsdAuctionRelay->m_ulNID);

	pAgsdAuctionRelay->Release();
	return bResult;
	}


BOOL AgsmAuctionRelay::CBSelectSalesResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData || !pData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	AgsdAuctionRelaySelectSales *pAgsdAuctionRelay = (AgsdAuctionRelaySelectSales *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	PVOID pvPacket = NULL;
	pThis->MakeRowsetPakcet(pRowset, &pvPacket);

	pAgsdAuctionRelay->m_pvPacket = pvPacket;

	BOOL bResult = pThis->SendSelectSales(pAgsdAuctionRelay, pAgsdAuctionRelay->m_ulNID);

	pAgsdAuctionRelay->Release();
	return bResult;
	}


BOOL AgsmAuctionRelay::CBSelectSalesCountResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData || !pData)
		return FALSE;

	AgsmAuctionRelay *pThis = (AgsmAuctionRelay *) pClass;
	AgsdAuctionRelaySelectSalesCount *pAgsdAuctionRelay = (AgsdAuctionRelaySelectSalesCount *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	// Rowset을 Custom parameter로 맹글고 이걸 Embedded Packet으로 맹근다.
	INT32 lTotalSize = pRowset->GetRowBufferSize() * pRowset->GetRowCount();
	INT32 lTotalStep = 1 + (INT32) (lTotalSize / 20000);
	pAgsdAuctionRelay->m_nTotalPacket = (INT16) lTotalStep;
	INT32 lRowsPerStep = 20000 / pRowset->GetRowBufferSize();
	for (INT32 lStep = 0; lStep < lTotalStep; lStep++)
		{
		PVOID pvPacket = NULL;
		pThis->MakeRowsetPakcet2(pRowset, &pvPacket, lStep, lRowsPerStep);

		pAgsdAuctionRelay->m_pvPacket = pvPacket;
		BOOL bResult = pThis->SendSelectSalesCount(pAgsdAuctionRelay, pAgsdAuctionRelay->m_ulNID);

		printf("\nSales count result sended[%d/%d][%s]\n", lStep + 1, lTotalStep,  bResult ? _T("TRUE") : _T("FALSE"));
		}

	pAgsdAuctionRelay->Release();
	return TRUE;
	}




//	Auction-side callbacks setting method
//=====================================================
//
BOOL AgsmAuctionRelay::SetCallbackForAuctionServer()
	{
	m_pAgsmDatabasePool = (AgsmDatabasePool *) GetModule("AgsmDatabasePool");
	if (!m_pAgsmDatabasePool)
		return FALSE;

	if (!SetCallbackSell(CBOperation, this) ||
		!SetCallbackCancel(CBOperation, this) ||
		!SetCallbackConfirm(CBOperation, this) ||
		!SetCallbackBuy(CBOperation, this) ||
		!SetCallbackSelect(CBOperation, this) ||
		!SetCallbackSelectSales(CBOperation, this) ||
		!SetCallbackSelectSalesCount(CBOperation, this)
		)
		return FALSE;

	return TRUE;
	}




//	Helper
//============================================
//
BOOL AgsmAuctionRelay::MakeRowsetPakcet(AuRowset *pRowset, PVOID *ppvPacket)
	{
	ASSERT(NULL != pRowset);

	stRowset csCustom;

	csCustom.m_lQueryIndex = 0;
	// query, header 나중에
	ZeroMemory(csCustom.m_szQuery, sizeof(csCustom.m_szQuery));
	ZeroMemory(csCustom.m_szHeaders, sizeof(csCustom.m_szHeaders));

	csCustom.m_ulRows = pRowset->GetRowCount();
	csCustom.m_ulCols = pRowset->GetColCount();
	
	csCustom.m_pBuffer = pRowset->GetHeadPtr();
	csCustom.m_ulRowBufferSize = pRowset->GetRowBufferSize();
	
	for (UINT32 i=0; i<pRowset->GetColCount(); i++)
		csCustom.m_lOffsets[i] = pRowset->GetOffset(i);

	INT16 nBufferSize = csCustom.m_ulRowBufferSize * csCustom.m_ulRows;
	INT16 nOffsetsSize = (INT16)csCustom.m_ulCols * sizeof(INT32);

	INT16	nPacketLength = 0;
	
	*ppvPacket = m_csPacketRowset.MakePacket(FALSE, &nPacketLength, 0,
											//&csCustom.m_eOperation,
											&csCustom.m_lQueryIndex,
											NULL,
											NULL,
											&csCustom.m_ulRows,
											&csCustom.m_ulCols,
											&csCustom.m_ulRowBufferSize,
											csCustom.m_pBuffer,
											&nBufferSize,
											&csCustom.m_lOffsets[0],
											&nOffsetsSize
											);

	return TRUE;
	}


BOOL AgsmAuctionRelay::MakeRowsetPakcet2(AuRowset *pRowset, PVOID *ppvPacket, INT32 lStep, INT32 lRowsPerStep)
	{
	ASSERT(NULL != pRowset);

	stRowset csCustom;

	csCustom.m_lQueryIndex = 0;
	// query, header 나중에
	ZeroMemory(csCustom.m_szQuery, sizeof(csCustom.m_szQuery));
	ZeroMemory(csCustom.m_szHeaders, sizeof(csCustom.m_szHeaders));

	csCustom.m_ulRows = min((INT32) (pRowset->GetRowCount() - (lStep * lRowsPerStep)), lRowsPerStep);
	csCustom.m_ulCols = pRowset->GetColCount();
	
	csCustom.m_pBuffer = pRowset->Get(lStep * lRowsPerStep, 0);
	csCustom.m_ulRowBufferSize = pRowset->GetRowBufferSize();
	
	for (UINT32 i=0; i<pRowset->GetColCount(); i++)
		csCustom.m_lOffsets[i] = pRowset->GetOffset(i);

	INT16 nBufferSize = csCustom.m_ulRowBufferSize * csCustom.m_ulRows;
	INT16 nOffsetsSize = (INT16)csCustom.m_ulCols * sizeof(INT32);

	INT16	nPacketLength = 0;
	
	*ppvPacket = m_csPacketRowset.MakePacket(FALSE, &nPacketLength, 0,
											//&csCustom.m_eOperation,
											&csCustom.m_lQueryIndex,
											NULL,
											NULL,
											&csCustom.m_ulRows,
											&csCustom.m_ulCols,
											&csCustom.m_ulRowBufferSize,
											csCustom.m_pBuffer,
											&nBufferSize,
											&csCustom.m_lOffsets[0],
											&nOffsetsSize
											);

	return TRUE;
	}
