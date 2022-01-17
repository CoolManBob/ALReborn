#include "AgpmCashMall.h"

BOOL AgpmCashMall::StreamReadCashMallTab(TCHAR *pszFile, BOOL bDecryption)
{
	if (!pszFile)
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmItemConvert::StreamReadCashMallTab() OpenExcelFile  Error!!!\n");
		return FALSE;
	}

	CashMallInfo	*pcsCashMallInfo	= GetCashMallInfo();

	INT32			lRow			= csExcelTxtLib.GetRow();
	INT32			lColumn			= csExcelTxtLib.GetColumn();

	if (lRow <= 0 || lColumn <= 0)
		return FALSE;

	if (pcsCashMallInfo->m_pstMallTabInfo)
	{
		delete [] pcsCashMallInfo->m_pstMallTabInfo;
		pcsCashMallInfo->m_pstMallTabInfo	= NULL;
	}

	pcsCashMallInfo->m_pstMallTabInfo	= new AgpmCashTabInfo[lColumn];
	if (!pcsCashMallInfo->m_pstMallTabInfo)
		return FALSE;

	pcsCashMallInfo->m_lNumTab			= lColumn - 1;
	pcsCashMallInfo->m_lNumTabListItem	= lRow - 1;

	int i = 0;
	for (i = 0; i < pcsCashMallInfo->m_lNumTab; ++i)
	{
		pcsCashMallInfo->m_pstMallTabInfo[i].m_plTabProductList	= new INT32[pcsCashMallInfo->m_lNumTabListItem];

		if (!pcsCashMallInfo->m_pstMallTabInfo[i].m_plTabProductList)
			return FALSE;

		ZeroMemory(pcsCashMallInfo->m_pstMallTabInfo[i].m_plTabProductList, sizeof(INT32) * pcsCashMallInfo->m_lNumTabListItem);

		pcsCashMallInfo->m_pstMallTabInfo[i].m_szTabName.SetText((TCHAR *) csExcelTxtLib.GetData(i + 1, 0));
	}

	for (i = 0; i < pcsCashMallInfo->m_lNumTabListItem; ++i)
	{
		for (int j = 0; j < pcsCashMallInfo->m_lNumTab; ++j)
		{
			TCHAR	*pszProductID	= csExcelTxtLib.GetData(j + 1, i + 1);
			if (pszProductID)
				pcsCashMallInfo->m_pstMallTabInfo[j].m_plTabProductList[i]	= _ttoi(pszProductID);
		}
	}

	for (i = 0; i < pcsCashMallInfo->m_lNumTab; ++i)
	{
		pcsCashMallInfo->m_pstMallTabInfo[i].m_lNumItem	= pcsCashMallInfo->m_lNumTabListItem;

		for (int j = 0; j < pcsCashMallInfo->m_lNumTabListItem; ++j)
		{
			if (pcsCashMallInfo->m_pstMallTabInfo[i].m_plTabProductList[j] == 0)
			{
				pcsCashMallInfo->m_pstMallTabInfo[i].m_lNumItem	= j;
				break;
			}
		}
	}

	pcsCashMallInfo->m_szFileMallTab.SetText(pszFile);

	return EncodeTabList();
}

BOOL AgpmCashMall::StreamReadCashMallItemList(TCHAR *pszFile, BOOL bDecryption)
{
	if (!pszFile)
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmItemConvert::StreamReadCashMallItemList() OpenExcelFile  Error!!!\n");
		return FALSE;
	}

	CashMallInfo	*pcsCashMallInfo	= GetCashMallInfo();

	INT32			lRow			= csExcelTxtLib.GetRow();
	INT32			lColumn			= csExcelTxtLib.GetColumn();

	if (lRow <= 0 || lColumn <= 0)
		return FALSE;

	if (pcsCashMallInfo->m_pstMallItemInfo)
	{
		delete [] pcsCashMallInfo->m_pstMallItemInfo;
		pcsCashMallInfo->m_pstMallItemInfo	= NULL;

		pcsCashMallInfo->m_csAdminProduct.RemoveObjectAll();
	}

	pcsCashMallInfo->m_pstMallItemInfo	= new AgpmCashItemInfo[lRow - 1];
	if (!pcsCashMallInfo->m_pstMallItemInfo)
		return FALSE;

	pcsCashMallInfo->m_lNumMallItemInfo	= lRow - 1;

	for (int i = 0; i < pcsCashMallInfo->m_lNumMallItemInfo; ++i)
	{
		for (int j = 0; j < lColumn; ++j)
		{
			TCHAR	*pszColumnName	= csExcelTxtLib.GetData(j, 0);
			TCHAR	*pszValue		= csExcelTxtLib.GetData(j, i + 1);
			if (pszColumnName && pszValue)
			{
				if (_tcscmp(pszColumnName, AGPMCASHMALL_STREAM_PRODUCTID) == 0)
				{
					pcsCashMallInfo->m_pstMallItemInfo[i].m_lProductID	= _ttoi(pszValue);
				}
				else if (_tcsncmp(pszColumnName, AGPMCASHMALL_STREAM_ITEMTID, _tcslen(AGPMCASHMALL_STREAM_ITEMTID)) == 0)
				{
					INT32 nIndex = atoi(pszColumnName + strlen(AGPMCASHMALL_STREAM_ITEMTID));
					ASSERT(nIndex == pcsCashMallInfo->m_pstMallItemInfo[i].m_lNumTotalItems);
					pcsCashMallInfo->m_pstMallItemInfo[i].m_alItemTID[nIndex]	= _ttoi(pszValue);
					pcsCashMallInfo->m_pstMallItemInfo[i].m_lNumTotalItems++;
				}
				else if (_tcsncmp(pszColumnName, AGPMCASHMALL_STREAM_COUNT, _tcslen(AGPMCASHMALL_STREAM_COUNT)) == 0)
				{
					INT32 nIndex = atoi(pszColumnName + strlen(AGPMCASHMALL_STREAM_COUNT));
					pcsCashMallInfo->m_pstMallItemInfo[i].m_alItemQty[nIndex]	= _ttoi(pszValue);
				}
				else if (_tcscmp(pszColumnName, AGPMCASHMALL_STREAM_PRICE) == 0)
				{
					pcsCashMallInfo->m_pstMallItemInfo[i].m_llPrice	= _ttoi64(pszValue);
				}
				else if (_tcscmp(pszColumnName, AGPMCASHMALL_STREAM_SPECIALFLAG) == 0)
				{
					if (pszValue)
					{
						if (_tcscmp(pszValue, AGPMCASHMALL_STREAM_FLAG_NEW) == 0)
							pcsCashMallInfo->m_pstMallItemInfo[i].m_lSpecialFlag	= AGPMCASH_SPECIALFLAG_NEW;
						else if (_tcscmp(pszValue, AGPMCASHMALL_STREAM_FLAG_HOT) == 0)
							pcsCashMallInfo->m_pstMallItemInfo[i].m_lSpecialFlag	= AGPMCASH_SPECIALFLAG_HOT;
						else if (_tcscmp(pszValue, AGPMCASHMALL_STREAM_FLAG_BEST) == 0)
							pcsCashMallInfo->m_pstMallItemInfo[i].m_lSpecialFlag	= AGPMCASH_SPECIALFLAG_BEST;
						else if (_tcscmp(pszValue, AGPMCASHMALL_STREAM_FLAG_EVENT) == 0)
							pcsCashMallInfo->m_pstMallItemInfo[i].m_lSpecialFlag	= AGPMCASH_SPECIALFLAG_EVENT;
						else if (_tcscmp(pszValue, AGPMCASHMALL_STREAM_FLAG_SALE) == 0)
							pcsCashMallInfo->m_pstMallItemInfo[i].m_lSpecialFlag	= AGPMCASH_SPECIALFLAG_SALE;
						else if (_tcscmp(pszValue, AGPMCASHMALL_STREAM_FLAG_SPC) == 0)
							pcsCashMallInfo->m_pstMallItemInfo[i].m_lSpecialFlag	= AGPMCASH_SPECIALFLAG_SPC;
						else if (_tcscmp(pszValue, AGPMCASHMALL_STREAM_FLAG_TPACK) == 0)
							pcsCashMallInfo->m_pstMallItemInfo[i].m_lSpecialFlag	= AGPMCASH_SPECIALFLAG_TPACK;
					}
				}
				else if (_tcscmp(pszColumnName, AGPMCASHMALL_STREAM_DESCRIPTION) == 0)
				{
					pcsCashMallInfo->m_pstMallItemInfo[i].m_szDescription.SetText(pszValue);
				}
				else if (_tcscmp(pszColumnName, AGPMCASHMALL_STREAM_BUY_MESSAGE) == 0)
				{
					pcsCashMallInfo->m_pstMallItemInfo[i].m_szBuyMessage.SetText(pszValue);
				}
				else if (_tcscmp(pszColumnName, AGPMCASHMALL_STREAM_PCBANGBUY) == 0)
				{
					pcsCashMallInfo->m_pstMallItemInfo[i].m_lPCBangBuy = _ttoi(pszValue);
				}
			}
		}

		AddCashItem(&pcsCashMallInfo->m_pstMallItemInfo[i]);
	}

	pcsCashMallInfo->m_szFileMallItemList.SetText(pszFile);
	
	// validation
	for (int i = 0; i < pcsCashMallInfo->m_lNumMallItemInfo; ++i)
	{
		if (0 == pcsCashMallInfo->m_pstMallItemInfo[i].m_lProductID ||
			0 == pcsCashMallInfo->m_pstMallItemInfo[i].m_lNumTotalItems
			)
		{
			ASSERT(!"!!! Error : Item info. validation failed in StreamReadCashMallItemList()");
			return FALSE;
		}

		for (int j = 0; j < pcsCashMallInfo->m_pstMallItemInfo[i].m_lNumTotalItems; j++)
		{
			if (0 == pcsCashMallInfo->m_pstMallItemInfo[i].m_alItemTID[j] ||
				0 == pcsCashMallInfo->m_pstMallItemInfo[i].m_alItemQty[j]
				)
			{
				ASSERT(!"!!! Error : Item validation failed in StreamReadCashMallItemList()");
				return FALSE;
			}
		}
	}
	

	return EncodeProductList();
}