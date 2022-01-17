#include "AgpmCashMall.h"
#include "AgppCashMall.h"

AgpmCashMall::AgpmCashMall()
{
	SetModuleName("AgpmCashMall");

	SetPacketType(AGPMCASHMALL_PACKET_TYPE);

	m_pcsAgpmCharacter	= NULL;
	m_pcsAgpmGrid		= NULL;
	m_pcsAgpmItem		= NULL;

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_INT8,	1,		// result
							AUTYPE_UINT8,	1,		// mall list version
							AUTYPE_INT32,	1,		// cid
							AUTYPE_INT32,	1,		// tab
							AUTYPE_INT32,	1,		// product id
							AUTYPE_MEMORY_BLOCK,	1,		// mall tab list
							AUTYPE_MEMORY_BLOCK,	1,		// item list
							AUTYPE_END,		0
							);

	m_lIndexAttachData	= 0;
	m_bUseCashMall = TRUE;
}

AgpmCashMall::~AgpmCashMall()
{
}

BOOL AgpmCashMall::OnAddModule()
{
	m_pcsAgpmCharacter	= (AgpmCharacter *)	GetModule("AgpmCharacter");
	m_pcsAgpmGrid		= (AgpmGrid *)		GetModule("AgpmGrid");
	m_pcsAgpmItem		= (AgpmItem *)		GetModule("AgpmItem");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmGrid ||
		!m_pcsAgpmItem)
		return FALSE;

	m_lIndexAttachData	= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdCashMall), NULL, NULL);
	if (m_lIndexAttachData < 0)
		return FALSE;

	return TRUE;
}

BOOL AgpmCashMall::OnInit()
{
	return TRUE;
}

BOOL AgpmCashMall::OnDestroy()
{
	return TRUE;
}

AgpdCashMall* AgpmCashMall::GetADCharacter(AgpdCharacter *pcsCharacter)
{
	if (m_pcsAgpmCharacter)
		return (AgpdCashMall *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexAttachData, (PVOID) pcsCharacter);

	return NULL;
}

BOOL AgpmCashMall::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1 || ulNID == 0 || !pstCheckArg)
		return FALSE;

	INT8	cOperation			= (-1);
	INT8	cResult				= (-1);
	UINT8	ucMallListVersion	= 0;
	INT32	lCID				= AP_INVALID_CID;
	INT32	lTab				= AP_INVALID_IID;
	INT32	lProductID			= AP_INVALID_IID;

	TCHAR*	pszMallList			= NULL;
	UINT16	unMallListLength	= 0;

	TCHAR*	pszProductList		= NULL;
	UINT16	unProductListLength	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&cResult,
						&ucMallListVersion,
						&lCID,
						&lTab,
						&lProductID,
						&pszMallList, &unMallListLength,
						&pszProductList, &unProductListLength);

	if(IsEnableUseCashMall() == FALSE)
	{
		EnumCallback(AGPMCASH_CB_DISABLE_USE_FORAWHILE, &lCID, NULL);

		return FALSE;
	}

	switch (cOperation) {
		case AGPMCASH_OPERATION_REQUEST_MALL_PRODUCT_LIST:
			{
				if (pstCheckArg->lSocketOwnerID != lCID)
					return FALSE;

				return OnOperationRequestMallProductList(lCID, lTab);
			}
			break;

		case AGPMCASH_OPERATION_RESPONSE_MALL_LIST:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationResponseMallList(pszMallList, unMallListLength, ucMallListVersion);
			}
			break;

		case AGPMCASH_OPERATION_RESPONSE_PRODUCT_LIST:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationResponseProductList(pszProductList, unProductListLength, ucMallListVersion, lTab);
			}
			break;

		case AGPMCASH_OPERATION_REQUEST_BUY_ITEM:
			{
				if (pstCheckArg->lSocketOwnerID != lCID)
					return FALSE;

				return OnOperationRequestBuyItem(lCID, lProductID, ucMallListVersion);
			}
			break;

		case AGPMCASH_OPERATION_RESPONSE_BUY_RESULT:
			{
				if (!pstCheckArg->bReceivedFromServer)
					return FALSE;

				return OnOperationResponseBuyResult(cResult);
			}
			break;

		case AGPMCASH_OPERATION_REFRESH_CASH:
			{
				return OnOperationResponseRefreshCash(lCID);
			}
			break;

		case AGPMCASH_OPERATION_CHECK_LIST_VERSION:
			{
				return OnOperationCheckListVersion(lCID, ucMallListVersion);
			}
			break;
		//JK_À¥Á¨ºô¸µ
		case AGPMCASH_OPERATION_REQUEST_BUY_ITEM_WEBZEN:	//c->s
			{
				PACKET_CASHMALL_BUY_CSREQ* pPacket = (PACKET_CASHMALL_BUY_CSREQ*)pvPacket;

				return OnOperationRequestBuyItemWebzen(pPacket->lCID, pPacket->m_nPackageSeq, pPacket->m_nDisPlaySeq, pPacket->m_nPriceSeq);

			}
			break;
		//JK_À¥Á¨ºô¸µ
		case AGPMCASH_OPERATION_RESPONSE_BUY_RESULT_WEBZEN:	//s->c	
			{
				PACKET_CASHMALL_BUY_SCACK* pPacket = ( PACKET_CASHMALL_BUY_SCACK* )pvPacket;
				EnumCallback( AGPMCASH_CB_RESPONSE_BUY_RESULT, pPacket, NULL );
			}
			break;
		//JK_À¥Á¨ºô¸µ : 
		case AGPMCASH_OPERATION_USESTORAGE_CSREQ:	// C -> S
			{
				PACKET_CASHMALL_USESTORAGE_CSREQ* pPacket = (PACKET_CASHMALL_USESTORAGE_CSREQ*)pvPacket;

				return OnOperationRequestUseStorage(pPacket->lCID, pPacket->m_nStorageSeq, pPacket->m_nStorageItemSeq);

			}
			break;

		case AGPMCASH_OPERATION_USESTORAGE_SCACK:	//S -> C
			{
				PACKET_CASHMALL_USESTORAGE_SCACK* pPacket = ( PACKET_CASHMALL_USESTORAGE_SCACK* )pvPacket;
				EnumCallback( AGPMCASH_CB_RESPONSE_USESTORAGE, pPacket, NULL );
			}
			break;

		case AGPMCASH_OPERATION_INQUIRESTOAGELIST_CSREQ:  // C -> S
			{
				PACKET_CASHMALL_INQUIRESTORAGELIST_CSREQ* pPacket = (PACKET_CASHMALL_INQUIRESTORAGELIST_CSREQ*)pvPacket;

				return OnOperationRequestStorageList(pPacket->lCID, pPacket->m_nNowPage);

			}
			break;

		case AGPMCASH_OPERATION_INQUIRESTOAGELIST_SCACK:
			{
				PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK* pPacket = ( PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK* )pvPacket;
				EnumCallback( AGPMCASH_CB_RESPONSE_INQUIRESTORAGELIST, pPacket, NULL );
			}
			break;
	}

	return TRUE;
}

BOOL AgpmCashMall::OnOperationRequestMallProductList(INT32 lCID, INT32 lTab)
{
	if(lTab < 0 || lTab >= AGPMCASHMALL_MAX_TAB_COUNT)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMCASH_CB_REQUEST_MALL_PRODUCT_LIST, pcsCharacter, &lTab);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmCashMall::OnOperationResponseMallList(TCHAR *pszMallList, UINT16 unMallListLength, UINT8 ucMallListVersion)
{
	AuAutoLock Lock(m_csCashMallInfo.m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (!DecodeTabList(pszMallList, unMallListLength))
		return FALSE;

	m_csCashMallInfo.m_ucMallListVersion	= ucMallListVersion;

	return TRUE;
}

BOOL AgpmCashMall::OnOperationResponseProductList(TCHAR *pszProductList, UINT16 unProductListLength, UINT8 ucMallListVersion, INT32 lTab)
{
	if(lTab < 0 || lTab >= AGPMCASHMALL_MAX_TAB_COUNT)
		return FALSE;

	AuAutoLock Lock(m_csCashMallInfo.m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (!DecodeProductList(pszProductList, unProductListLength, lTab))
		return FALSE;

	m_csCashMallInfo.m_ucMallListVersion	= ucMallListVersion;

	return TRUE;
}

BOOL AgpmCashMall::OnOperationRequestBuyItem(INT32 lCID, INT32 lProductID, UINT8 ucMallListVersion)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	PVOID	pvBuffer[2];
	pvBuffer[0]	= IntToPtr(lProductID);
	pvBuffer[1]	= IntToPtr(ucMallListVersion);

	EnumCallback(AGPMCASH_CB_REQUEST_BUY_ITEM, pcsCharacter, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmCashMall::OnOperationResponseBuyResult(UINT8 cResult)
{
	return EnumCallback(AGPMCASH_CB_RESPONSE_BUY_RESULT, &cResult, NULL);
}
//JK_À¥Á¨ºô¸µ
BOOL AgpmCashMall::OnOperationRequestBuyItemWebzen(INT32 lCID, INT32 PackageSeq, INT32 DisplaySeq, INT32 PriceSeq)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	PVOID	pvBuffer[3];
	pvBuffer[0]	= IntToPtr(PackageSeq);
	pvBuffer[1]	= IntToPtr(DisplaySeq);
	pvBuffer[2]	= IntToPtr(PriceSeq);

	EnumCallback(AGPMCASH_CB_REQUEST_BUY_ITEM_WEBZEN, pcsCharacter, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

//JK_À¥Á¨ºô¸µ
BOOL AgpmCashMall::OnOperationRequestUseStorage(INT32 lCID, INT32 StorageSeq, INT32 StorageItemSeq)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	PVOID	pvBuffer[2];
	pvBuffer[0]	= IntToPtr(StorageSeq);
	pvBuffer[1]	= IntToPtr(StorageItemSeq);
	

	EnumCallback(AGPMCASH_CB_REQUEST_USESTORAGE, pcsCharacter, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

//JK_À¥Á¨ºô¸µ
BOOL AgpmCashMall::OnOperationRequestStorageList(INT32 lCID, INT32 NowPage)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMCASH_CB_REQUEST_INQUIRESTORAGELIST, pcsCharacter, &NowPage);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmCashMall::OnOperationResponseRefreshCash(INT32 lCID)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	EnumCallback(AGPMCASH_CB_REFRESH_CASH, pcsCharacter, NULL);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmCashMall::OnOperationCheckListVersion(INT32 lCID, UINT8 ucMallListVersion)
{
	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	EnumCallback(AGPMCASH_CB_CHECK_LIST_VERSION, pcsCharacter, &ucMallListVersion);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

CashMallInfo* AgpmCashMall::GetCashMallInfo()
{
	return &m_csCashMallInfo;
}

BOOL AgpmCashMall::SetMaxProduct(INT32 lMax)
{
	return m_csCashMallInfo.SetMaxProduct(lMax);
}

pAgpmCashItemInfo AgpmCashMall::GetCashItem(INT32 lProductID)
{
	pAgpmCashItemInfo	*ppCashItemInfo	= (pAgpmCashItemInfo *) m_csCashMallInfo.m_csAdminProduct.GetObject(lProductID);
	if (!ppCashItemInfo)
		return NULL;

	return *ppCashItemInfo;
}

BOOL AgpmCashMall::AddCashItem(pAgpmCashItemInfo pItemInfo)
{
	if (pItemInfo && m_csCashMallInfo.m_csAdminProduct.AddObject(&pItemInfo, pItemInfo->m_lProductID))
		return TRUE;

	return FALSE;
}

BOOL AgpmCashMall::RefreshMallList()
{
	CashMallInfo	*pcsCashMallInfo	= GetCashMallInfo();

	AuAutoLock Lock(pcsCashMallInfo->m_Mutex);
	if (!Lock.Result()) return FALSE;

	StreamReadCashMallTab(pcsCashMallInfo->m_szFileMallTab.GetBuffer(), FALSE);
	StreamReadCashMallItemList(pcsCashMallInfo->m_szFileMallItemList.GetBuffer(), FALSE);

	++pcsCashMallInfo->m_ucMallListVersion;

	return TRUE;
}

BOOL AgpmCashMall::SetCallbackRequestMallProductList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_REQUEST_MALL_PRODUCT_LIST, pfCallback, pClass);
}

BOOL AgpmCashMall::SetCallbackRequestBuyItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_REQUEST_BUY_ITEM, pfCallback, pClass);
}
//JK_À¥Á¨ºô¸µ
BOOL AgpmCashMall::SetCallbackRequestBuyItemWebzen(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_REQUEST_BUY_ITEM_WEBZEN, pfCallback, pClass);
}
//JK_À¥Á¨ºô¸µ
BOOL AgpmCashMall::SetCallbackRequestUseStorage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_REQUEST_USESTORAGE, pfCallback, pClass);
}
BOOL AgpmCashMall::SetCallbackResponseUseStorage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_RESPONSE_USESTORAGE, pfCallback, pClass);
}
//JK_À¥Á¨ºô¸µ
BOOL AgpmCashMall::SetCallbackRequestInquireStorageList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_REQUEST_INQUIRESTORAGELIST, pfCallback, pClass);
}

BOOL AgpmCashMall::SetCallbackResponseInquireStorageList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_RESPONSE_INQUIRESTORAGELIST, pfCallback, pClass);
}

BOOL AgpmCashMall::SetCallbackResponseBuyResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_RESPONSE_BUY_RESULT, pfCallback, pClass);
}

BOOL AgpmCashMall::SetCallbackUpdateMallList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_UPDATE_MALL_LIST, pfCallback, pClass);
}

BOOL AgpmCashMall::SetCallbackRefreshCash(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_REFRESH_CASH, pfCallback, pClass);
}

BOOL AgpmCashMall::SetCallbackCheckListVersion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_CHECK_LIST_VERSION, pfCallback, pClass);
}

BOOL AgpmCashMall::SetCallbackDisableForaWhile(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCASH_CB_DISABLE_USE_FORAWHILE, pfCallback, pClass);
}

BOOL AgpmCashMall::EncodeTabList()
{
	if (!m_csCashMallInfo.m_pstMallTabInfo)
		return FALSE;

	m_strMallEncodeString.Clear();

	for (int nNumTab = 0; nNumTab < m_csCashMallInfo.m_lNumTab; ++nNumTab)
	{
		if (m_csCashMallInfo.m_pstMallTabInfo[nNumTab].m_plTabProductList)
		{
			m_strMallEncodeString.AppendFormat("%s;", m_csCashMallInfo.m_pstMallTabInfo[nNumTab].m_szTabName.GetBuffer());

			for (int nListItem = 0; nListItem < m_csCashMallInfo.m_lNumTabListItem; ++nListItem)
			{
				if (m_csCashMallInfo.m_pstMallTabInfo[nNumTab].m_plTabProductList[nListItem] == 0)
					break;

				m_strMallEncodeString.AppendFormat("%d,", m_csCashMallInfo.m_pstMallTabInfo[nNumTab].m_plTabProductList[nListItem]);
			}

			m_strMallEncodeString.AppendFormat(":");
		}
	}

	return TRUE;
}

BOOL AgpmCashMall::DecodeTabList(TCHAR *pszMallList, UINT16 unMallListLength)
{
	if (!pszMallList || unMallListLength < 1)
		return FALSE;

	CashMallInfo	*pcsCashMallInfo	= GetCashMallInfo();

	if (pcsCashMallInfo->m_pstMallTabInfo)
	{
		delete [] pcsCashMallInfo->m_pstMallTabInfo;
		pcsCashMallInfo->m_pstMallTabInfo	= NULL;
	}

	INT32	lNumTab			= 0;
	INT32	lNumTabListItem	= 0;

	INT32	lMaxTabListItem	= 0;
	for (int i = 0; i < unMallListLength; ++i)
	{
		if (pszMallList[i] == ':')
		{
			++lNumTab;

			if (lMaxTabListItem < lNumTabListItem)
				lMaxTabListItem	= lNumTabListItem;

			lNumTabListItem	= 0;
		}
		else if (pszMallList[i] == ',')
			++lNumTabListItem;
	}

	pcsCashMallInfo->m_pstMallTabInfo	= new AgpmCashTabInfo[lNumTab];
	if (!pcsCashMallInfo->m_pstMallTabInfo)
		return FALSE;

	pcsCashMallInfo->m_lNumTab			= lNumTab;
	pcsCashMallInfo->m_lNumTabListItem	= lMaxTabListItem;

	INT32	lStartPos	= 0;
	INT32	lCurrentPos	= 0;

	for (int i = 0; i < lNumTab; ++i)
	{
		pcsCashMallInfo->m_pstMallTabInfo[i].m_plTabProductList	= new INT32[lMaxTabListItem];

		if (!pcsCashMallInfo->m_pstMallTabInfo[i].m_plTabProductList)
			return FALSE;

		for ( ; lCurrentPos < unMallListLength; ++lCurrentPos)
			if (pszMallList[lCurrentPos] == ';')
				break;

		TCHAR	szBuffer[128];
		ZeroMemory(szBuffer, sizeof(szBuffer));

		_tcsncpy(szBuffer, pszMallList + lStartPos, lCurrentPos - lStartPos);

		pcsCashMallInfo->m_pstMallTabInfo[i].m_szTabName.SetText(szBuffer);

		++lCurrentPos;

		lStartPos	= lCurrentPos;
		
		INT32	lCurrentTab	= 0;

		for ( ; lCurrentPos < unMallListLength; ++lCurrentPos)
		{
			if (pszMallList[lCurrentPos] == ',')
			{
				ZeroMemory(szBuffer, sizeof(szBuffer));

				_tcsncpy(szBuffer, pszMallList + lStartPos, lCurrentPos - lStartPos);

				pcsCashMallInfo->m_pstMallTabInfo[i].m_plTabProductList[lCurrentTab++]	= _ttoi(szBuffer);

				++lCurrentPos;

				lStartPos	= lCurrentPos;

				if (pszMallList[lCurrentPos] == ':')
				{
					++lCurrentPos;
					++lStartPos;
					break;
				}
			}
		}

		pcsCashMallInfo->m_pstMallTabInfo[i].m_lNumItem	= lCurrentTab;
	}

	return TRUE;
}

BOOL AgpmCashMall::EncodeProductList()
{
	if (!m_csCashMallInfo.m_pstMallItemInfo)
		return FALSE;

	//m_strProductListEncodeString.Clear();

	//for (int nNumProduct = 0; nNumProduct < m_csCashMallInfo.m_lNumMallItemInfo; ++nNumProduct)
	//{
	//	m_strProductListEncodeString.AppendFormat("%d,%d,%d,%I64d,%d,%s:",
	//											  m_csCashMallInfo.m_pstMallItemInfo[nNumProduct].m_lProductID,
	//											  m_csCashMallInfo.m_pstMallItemInfo[nNumProduct].m_alItemTID[0],
	//											  m_csCashMallInfo.m_pstMallItemInfo[nNumProduct].m_alItemQty[0],
	//											  m_csCashMallInfo.m_pstMallItemInfo[nNumProduct].m_llPrice,
	//											  m_csCashMallInfo.m_pstMallItemInfo[nNumProduct].m_lSpecialFlag,
	//											  m_csCashMallInfo.m_pstMallItemInfo[nNumProduct].m_szDescription.GetBuffer());
	//}

	for(int lNumTab = 0; lNumTab < m_csCashMallInfo.m_lNumTab; ++lNumTab)
	{
		m_strProductListEncodeString[lNumTab].Clear();

		for(int lNumProduct = 0; lNumProduct < m_csCashMallInfo.m_pstMallTabInfo[lNumTab].m_lNumItem; ++lNumProduct)
		{
			pAgpmCashItemInfo pCashItemInfo = GetCashItem(m_csCashMallInfo.m_pstMallTabInfo[lNumTab].m_plTabProductList[lNumProduct]);
			if(!pCashItemInfo)
				continue;

			m_strProductListEncodeString[lNumTab].AppendFormat("%d,%d,%d,%I64d,%d,%s:",
																pCashItemInfo->m_lProductID,
																pCashItemInfo->m_alItemTID[0],
																pCashItemInfo->m_alItemQty[0],
																pCashItemInfo->m_llPrice,
																pCashItemInfo->m_lSpecialFlag,
																pCashItemInfo->m_szDescription.GetBuffer());
		}
	}

	return TRUE;
}

BOOL AgpmCashMall::DecodeProductList(TCHAR *pszProductList, UINT16 unProductListLength, INT32 lTab)
{
	if (!pszProductList || unProductListLength < 1)
		return FALSE;

	if (m_csCashMallInfo.m_pstMallItemInfo)
	{
		delete [] m_csCashMallInfo.m_pstMallItemInfo;
		m_csCashMallInfo.m_pstMallItemInfo	= NULL;

		m_csCashMallInfo.m_csAdminProduct.RemoveObjectAll();
	}

	INT32	lNumItemInfo	= 0;
	for (int i = 0; i < unProductListLength; ++i)
	{
		if (pszProductList[i] == ':')
			++lNumItemInfo;
	}

	if (lNumItemInfo <= 0)
		return FALSE;

	m_csCashMallInfo.m_pstMallItemInfo	= new AgpmCashItemInfo[lNumItemInfo];
	if (!m_csCashMallInfo.m_pstMallItemInfo)
		return FALSE;

	m_csCashMallInfo.m_lNumMallItemInfo	= lNumItemInfo;

	INT32	lStartPos	= 0;
	INT32	lIndex		= 0;
	for (int i = 0; i < unProductListLength; ++i)
	{
		if (pszProductList[i] == ':')
		{
			_stscanf(pszProductList + lStartPos, "%d,%d,%d,%I64d,%d,",
				&m_csCashMallInfo.m_pstMallItemInfo[lIndex].m_lProductID,
				&m_csCashMallInfo.m_pstMallItemInfo[lIndex].m_alItemTID[0],
				&m_csCashMallInfo.m_pstMallItemInfo[lIndex].m_alItemQty[0],
				&m_csCashMallInfo.m_pstMallItemInfo[lIndex].m_llPrice,
				&m_csCashMallInfo.m_pstMallItemInfo[lIndex].m_lSpecialFlag);

			for (int j = 0; j < unProductListLength; ++j)
			{
				if (pszProductList[i - j] == ',')
				{
					CopyMemory(m_csCashMallInfo.m_pstMallItemInfo[lIndex].m_szDescription.GetBuffer(), &pszProductList[i - j + 1], sizeof(TCHAR) * (j - 1));
					break;
				}
			}

			AddCashItem(&m_csCashMallInfo.m_pstMallItemInfo[lIndex]);

			++lIndex;
			lStartPos	= i + 1;
		}
	}

	return EnumCallback(AGPMCASH_CB_UPDATE_MALL_LIST, NULL, &lTab);
}

CashMallInfo::CashMallInfo()
{
	m_lNumTab			= 0;
	m_lNumTabListItem	= 0;

	m_pstMallTabInfo	= NULL;

	m_lNumMallItemInfo	= 0;

	m_pstMallItemInfo	= NULL;

	m_ucMallListVersion	= 1;

	m_Mutex.Init();
}

CashMallInfo::~CashMallInfo()
{
	if (m_pstMallTabInfo)
	{
		delete [] m_pstMallTabInfo;

		m_pstMallTabInfo	= NULL;
	}

	if (m_pstMallItemInfo)
	{
		delete [] m_pstMallItemInfo;

		m_pstMallItemInfo	= NULL;
	}

	m_csAdminProduct.RemoveObjectAll();

	m_Mutex.Destroy();
}
