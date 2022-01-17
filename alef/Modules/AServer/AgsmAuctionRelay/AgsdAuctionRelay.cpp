/*=============================================================

	AgsdAuctionRelay.cpp

=============================================================*/

#include "AgsdAuctionRelay.h"

#define _LOG_FILENAME "LOG\\db_auction.log"


/************************************************/
/*		The Implementaiton of Parameters		*/
/************************************************/
//
//	==========		Select		==========
//
AgsdAuctionRelaySelect::AgsdAuctionRelaySelect()
	{
	m_lItemTID = 0;
	m_nStatus = AGPMAUCTION_SALES_STATUS_ONSALE;
	m_ullDocID = 0;
	m_nFlag = 0;
	m_pvPacket = NULL;
	ullDocID	= 0;
	}


AgsdAuctionRelaySelect::AgsdAuctionRelaySelect(const AgsdAuctionRelaySelect &r)
	{
	*this = r;
	}


AgsdAuctionRelaySelect::AgsdAuctionRelaySelect(const AgsdAuctionRelaySelect *p)
	{
	if (p)
		*this = *p;
	}


BOOL AgsdAuctionRelaySelect::SetParamSelect(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, &m_lItemTID);
	pStatement->SetParam(i++, &m_nStatus);
	pStatement->SetParam(i++, &ullDocID);
	pStatement->SetParam(i++, &m_nFlag);
	pStatement->SetParam(i++, &m_lItemTID);
	pStatement->SetParam(i++, &m_nStatus);
	pStatement->SetParam(i++, &ullDocID);
	pStatement->SetParam(i++, &m_nFlag);	
	return TRUE;
	}



//
//	==========		Select Sales		==========
//
AgsdAuctionRelaySelectSales::AgsdAuctionRelaySelectSales()
	{
	ZeroMemory(m_szChar, sizeof(m_szChar));
	m_pvPacket = NULL;
	}


AgsdAuctionRelaySelectSales::AgsdAuctionRelaySelectSales(const AgsdAuctionRelaySelectSales &r)
	{
	*this = r;
	}


AgsdAuctionRelaySelectSales::AgsdAuctionRelaySelectSales(const AgsdAuctionRelaySelectSales *p)
	{
	if (p)
		*this = *p;
	}


BOOL AgsdAuctionRelaySelectSales::SetParamSelect(AuStatement *pStatement)
	{
	INT16 i = 0;
	pStatement->SetParam(i++, m_szChar, sizeof(m_szChar));
	
	return TRUE;
	}


//
//	==========		Select Sales Count		==========
//
AgsdAuctionRelaySelectSalesCount::AgsdAuctionRelaySelectSalesCount()
	{
	m_pvPacket = NULL;
	}


AgsdAuctionRelaySelectSalesCount::AgsdAuctionRelaySelectSalesCount(const AgsdAuctionRelaySelectSalesCount &r)
	{
	*this = r;
	}


AgsdAuctionRelaySelectSalesCount::AgsdAuctionRelaySelectSalesCount(const AgsdAuctionRelaySelectSalesCount *p)
	{
	if (p)
		*this = *p;
	}


BOOL AgsdAuctionRelaySelectSalesCount::SetParamSelect(AuStatement *pStatement)
	{
	return TRUE;
	}


//
//	==========		Sell		==========
//
AgsdAuctionRelaySell::AgsdAuctionRelaySell()
	{
	// in
	m_ullItemSeq = 0;
	m_lPrice = 0;
	m_nQuantity = 0;
	m_nItemStatus = AGPDITEM_STATUS_NONE;
	m_nStatusTo = AGPMAUCTION_SALES_STATUS_ONSALE;	
	ZeroMemory(m_szChar, sizeof(m_szChar));	
	m_lItemID = 0;

	// out
	m_ullDocID = 0;
	ZeroMemory(m_szDate, sizeof(m_szDate));

	// dummy
	ZeroMemory(m_szItemSeq, sizeof(m_szItemSeq));
	ZeroMemory(m_szDocID, sizeof(m_szDocID));
	}


AgsdAuctionRelaySell::AgsdAuctionRelaySell(const AgsdAuctionRelaySell &r)
	{
	*this = r;
	}


AgsdAuctionRelaySell::AgsdAuctionRelaySell(const AgsdAuctionRelaySell *p)
	{
	if (p)
		*this = *p;
	}


BOOL AgsdAuctionRelaySell::SetParamExecute(AuStatement *pStatement)
	{
	// In
	INT16 i = 0;
	pStatement->SetParam(i++, m_szItemSeq, sizeof(m_szItemSeq));
	pStatement->SetParam(i++, &m_lPrice);
	pStatement->SetParam(i++, &m_nQuantity);
	pStatement->SetParam(i++, &m_nItemStatus);
	pStatement->SetParam(i++, &m_nStatusTo);
	pStatement->SetParam(i++, m_szChar, sizeof(m_szChar));

	// Out
	pStatement->SetParam(i++, m_szDocID, sizeof(m_szDocID), TRUE);
	pStatement->SetParam(i++, m_szDate, sizeof(m_szDate), TRUE);
	pStatement->SetParam(i++, &m_nCode, TRUE);

	return TRUE;
	}


void AgsdAuctionRelaySell::Dump(CHAR *pszOp)
	{
	if (AGPMAUCTION_EXCPT_SUCCESS == m_nCode)
		return;

	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "%04d%02d%02d-%02d-RELAY_DUMP_AUCT.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	FILE *pFile = fopen(szFile, "a+");
	if (pFile)
		{
		fprintf(pFile, "[%02d:%02d:%02d][SEL]"
						"CHA[%s] SEQ[%s] PRC[%d] QTY[%d] IST[%d] STA[%d] O_COD[%d] O_DOC[%s]\n",
						st.wHour, st.wMinute, st.wSecond,
						m_szChar, m_szItemSeq, m_lPrice, m_nQuantity, m_nItemStatus, m_nStatusTo,
						m_nCode, m_szDocID
						);
		
		fclose(pFile);
		}	
	}




//
//	==========		Cancel		==========
//
AgsdAuctionRelayCancel::AgsdAuctionRelayCancel()
	{
	m_lSalesID = 0;
	
	// in
	m_ullDocID = 0;
	m_nStatusFrom = AGPMAUCTION_SALES_STATUS_ONSALE;
	m_nStatusTo = AGPMAUCTION_SALES_STATUS_CANCEL;
	ZeroMemory(m_szChar, sizeof(m_szChar));

	ZeroMemory(m_szDocID, sizeof(m_szDocID));
	}


AgsdAuctionRelayCancel::AgsdAuctionRelayCancel(const AgsdAuctionRelayCancel &r)
	{
	*this = r;
	}


AgsdAuctionRelayCancel::AgsdAuctionRelayCancel(const AgsdAuctionRelayCancel *p)
	{
	if (p)
		*this = *p;
	}


BOOL AgsdAuctionRelayCancel::SetParamExecute(AuStatement *pStatement)
	{
	// in
	INT16 i = 0;
	pStatement->SetParam(i++, m_szDocID, sizeof(m_szDocID));
	pStatement->SetParam(i++, m_szChar, sizeof(m_szChar));
	pStatement->SetParam(i++, &m_nStatusFrom);
	pStatement->SetParam(i++, &m_nStatusTo);
	
	// out
	pStatement->SetParam(i++, &m_nCode, TRUE);

	return TRUE;
	};


void AgsdAuctionRelayCancel::Dump(CHAR *pszOp)
	{
	if (AGPMAUCTION_EXCPT_SUCCESS == m_nCode)
		return;

	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "%04d%02d%02d-%02d-RELAY_DUMP_AUCT.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	FILE *pFile = fopen(szFile, "a+");
	if (pFile)
		{
		fprintf(pFile, "[%02d:%02d:%02d][CAN]"
						"DOC[%s] CHA[%s] O_COD[%d]\n",
						st.wHour, st.wMinute, st.wSecond,
						m_szDocID, m_szChar, m_nCode);
	
		fclose(pFile);
		}		
	}




//
//	==========		Confirm		==========
//
AgsdAuctionRelayConfirm::AgsdAuctionRelayConfirm()
	{
	m_lSalesID = 0;

	// in	
	m_ullDocID = 0;
	ZeroMemory(m_szChar, sizeof(m_szChar));
	m_nStatusFrom = AGPMAUCTION_SALES_STATUS_COMPLETE;
	m_nStatusTo = AGPMAUCTION_SALES_STATUS_CONFIRM_COMPLETE;

	ZeroMemory(m_szDocID, sizeof(m_szDocID));
	}


AgsdAuctionRelayConfirm::AgsdAuctionRelayConfirm(const AgsdAuctionRelayConfirm &r)
	{
	*this = r;
	}


AgsdAuctionRelayConfirm::AgsdAuctionRelayConfirm(const AgsdAuctionRelayConfirm *p)
	{
	if (p)
		*this = *p;
	}


BOOL AgsdAuctionRelayConfirm::SetParamExecute(AuStatement *pStatement)
	{
	// in
	INT16 i = 0;
	pStatement->SetParam(i++, m_szDocID, sizeof(m_szDocID));
	pStatement->SetParam(i++, m_szChar, sizeof(m_szChar));
	pStatement->SetParam(i++, &m_nStatusFrom);
	pStatement->SetParam(i++, &m_nStatusTo);
	
	// out
	pStatement->SetParam(i++, &m_nCode, TRUE);

	return TRUE;
	};


void AgsdAuctionRelayConfirm::Dump(CHAR *pszOp)
	{
	if (AGPMAUCTION_EXCPT_SUCCESS == m_nCode)
		return;

	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "%04d%02d%02d-%02d-RELAY_DUMP_AUCT.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	FILE *pFile = fopen(szFile, "a+");
	if (pFile)
		{
		fprintf(pFile, "[%02d:%02d:%02d][CNF]"
						"DOC[%s] CHA[%s] O_COD[%d]\n",
						st.wHour, st.wMinute, st.wSecond,
						m_szDocID, m_szChar, m_nCode
						);
		
		fclose(pFile);
		}		
	}




//
//	==========		Buy		==========
//
AgsdAuctionRelayBuy::AgsdAuctionRelayBuy()
	{
	// in
	m_ullDocID = 0;
	ZeroMemory(m_szBuyer, sizeof(m_szBuyer));
	m_llMoney = 0;
	m_nStatusFrom = AGPMAUCTION_SALES_STATUS_ONSALE;
	m_nStatusTo = AGPMAUCTION_SALES_STATUS_COMPLETE;

	// out
	ZeroMemory(m_szSeller, sizeof(m_szSeller));
	m_lPrice = 0;
	m_ullItemSeq = 0;
	m_lItemTID = 0;
	m_nQuantity = 0;
	m_cNeedLevel = 0;
	ZeroMemory(m_szOwnTime, sizeof(m_szOwnTime));
	ZeroMemory(m_szConvHistory, sizeof(m_szConvHistory));
	m_lDurability = 0;
	m_lMaxDurability = 0;
	ZeroMemory(m_szOption, sizeof(m_szOption));
	ZeroMemory(m_szSkillPlus, sizeof(m_szSkillPlus));
	m_lInUse = 0;
	m_lUseCount = 0;
	m_lRemainTime = 0;
	m_lExpireDate = 0;
	m_llStaminaRemainTime = 0;
	
	// dummy
	ZeroMemory(m_szDocID, sizeof(m_szDocID));
	ZeroMemory(m_szMoney, sizeof(m_szMoney));
	ZeroMemory(m_szItemSeq, sizeof(m_szItemSeq));
	ZeroMemory(m_szExpireDate, sizeof(m_szExpireDate));
	}


AgsdAuctionRelayBuy::AgsdAuctionRelayBuy(const AgsdAuctionRelayBuy &r)
	{
	*this = r;
	}


AgsdAuctionRelayBuy::AgsdAuctionRelayBuy(const AgsdAuctionRelayBuy *p)
	{
	if (p)
		*this = *p;
	}


BOOL AgsdAuctionRelayBuy::SetParamExecute(AuStatement *pStatement)
	{
	INT16 i = 0;

	// in
	pStatement->SetParam(i++, m_szDocID, sizeof(m_szDocID));
	pStatement->SetParam(i++, m_szBuyer, sizeof(m_szBuyer));
	pStatement->SetParam(i++, m_szMoney, sizeof(m_szMoney));
	pStatement->SetParam(i++, &m_nStatusFrom);
	pStatement->SetParam(i++, &m_nStatusTo);
	
	// out
	pStatement->SetParam(i++, m_szSeller, sizeof(m_szSeller), TRUE);
	pStatement->SetParam(i++, &m_lPrice, TRUE);
	pStatement->SetParam(i++, m_szItemSeq, sizeof(m_szItemSeq), TRUE);
	pStatement->SetParam(i++, &m_lItemTID, TRUE);
	pStatement->SetParam(i++, &m_nQuantity, TRUE);
	pStatement->SetParam(i++, &m_cNeedLevel, TRUE);
	pStatement->SetParam(i++, m_szOwnTime, sizeof(m_szOwnTime), TRUE);
	pStatement->SetParam(i++, m_szConvHistory, sizeof(m_szConvHistory), TRUE);
	pStatement->SetParam(i++, &m_lDurability, TRUE);
	pStatement->SetParam(i++, &m_lMaxDurability, TRUE);
	pStatement->SetParam(i++, m_szOption, sizeof(m_szOption), TRUE);
	pStatement->SetParam(i++, m_szSkillPlus, sizeof(m_szSkillPlus), TRUE);
	pStatement->SetParam(i++, &m_lInUse, TRUE);
	pStatement->SetParam(i++, &m_lUseCount, TRUE);
	pStatement->SetParam(i++, (UINT32*)&m_lRemainTime, TRUE);
	pStatement->SetParam(i++, m_szExpireDate, sizeof(m_szExpireDate), TRUE);
	pStatement->SetParam(i++, (UINT32*)&m_llStaminaRemainTime, TRUE);
	pStatement->SetParam(i++, &m_nCode, TRUE);
	

	return TRUE;
	};


void AgsdAuctionRelayBuy::Dump(CHAR *pszOP)
	{
	if (AGPMAUCTION_EXCPT_SUCCESS == m_nCode)
		return;

	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "%04d%02d%02d-%02d-RELAY_DUMP_AUCT.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	FILE *pFile = fopen(szFile, "a+");
	if (pFile)
		{
		fprintf(pFile, "[%02d:%02d:%02d][BUY]"
						"DOC[%s] BUY[%s]"
						"O_COD[%d] O_SEL[%s] O_PRC[%d] O_SEQ[%s] O_TID[%d] O_QTY[%d] O_LVL[%d] O_OWN[%s] O_O_CNV[%s] O_DUR[%d] O_OPT[%s]\n",
						st.wHour, st.wMinute, st.wSecond,
						m_szDocID, m_szBuyer,
						m_nCode, m_szSeller, m_lPrice, m_szItemSeq, m_lItemTID, m_nQuantity, m_cNeedLevel,
						m_szOwnTime, m_szConvHistory, m_lDurability, m_szOption
						);
		
		fclose(pFile);
		}
	}




//
//	==========		Character		==========
//
AgsdAuctionRelayCharacter::AgsdAuctionRelayCharacter()
	{
	//ZeroMemory(m_szChar, sizeof(m_szChar));
	m_lServerID = 0;
	}


AgsdAuctionRelayCharacter::~AgsdAuctionRelayCharacter()
	{
	}
