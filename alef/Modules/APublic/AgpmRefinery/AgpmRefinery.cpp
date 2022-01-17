/*===================================================================

	AgpmRefinery.cpp

===================================================================*/

#include "AgpmRefinery.h"
#include "AuExcelTxtLib.h"
#include "ApUtil.h"
#include "AuExcelBinaryLib.h"

using namespace std;

/********************************************************/
/*		The Implementation of AgpmRefinery class		*/
/********************************************************/
//
AgpmRefinery::AgpmRefinery()
	{
	SetModuleName("AgpmRefinery");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMREFINERY_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1,	// Operation
							AUTYPE_INT32,			1,	// CID
							AUTYPE_INT32,			1,	// Item TID
							AUTYPE_INT32,			10, // Source Item IDs
							AUTYPE_INT32,			1,	// Result
							AUTYPE_INT32,			1,	// Result Item TID
							AUTYPE_END,				0
							);

	SetModuleData(sizeof(AgpdRefineTemplate), AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
	SetModuleData(sizeof(AgpdRefineItem), AGPMREFINERY_DATATYPE_REFINE_ITEM);

	m_pAgpmItem			= NULL;
	m_pAgpmGrid			= NULL;
	m_pAgpmFactors		= NULL;
	m_pAgpmCharacter	= NULL;
    m_pAgpdResultItem   = NULL;
	EnableIdle(FALSE);
	}


AgpmRefinery::~AgpmRefinery()
	{
	}




//	ApModule inherited
//============================================
//
BOOL AgpmRefinery::OnAddModule()
{
	m_pAgpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmGrid			= (AgpmGrid *) GetModule("AgpmGrid");
	m_pAgpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	
	if (!m_pAgpmItem || !m_pAgpmGrid || !m_pAgpmFactors || !m_pAgpmCharacter)
		return FALSE;

	return TRUE;
}


BOOL AgpmRefinery::OnInit()
{
	//if (!m_csAdminRefineTemplate.InitializeObject(sizeof(AgpdRefineTemplate *), m_csAdminRefineTemplate.GetCount()))
	//	return FALSE;

	if (!m_csAdminRefineItem.InitializeObject(sizeof(AgpdRefineItem *), m_csAdminRefineItem.GetCount()))
		return FALSE;

	return TRUE;
}


BOOL AgpmRefinery::OnDestroy()
{
	INT32 lIndex = 0;
	lIndex = 0;
	//AgpdRefineTemplate** ppAgpdRefineTemplate = NULL;

	//for (ppAgpdRefineTemplate = (AgpdRefineTemplate **) m_csAdminRefineTemplate.GetObjectSequence(&lIndex); ppAgpdRefineTemplate; 
	//	 ppAgpdRefineTemplate = (AgpdRefineTemplate **) m_csAdminRefineTemplate.GetObjectSequence(&lIndex))
	//{
	//	DestroyModuleData(*ppAgpdRefineTemplate, AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
	//}

	//m_csAdminRefineTemplate.Reset();

	AgpdRefineItem** ppAgpdRefineItem = NULL;

	for (ppAgpdRefineItem = (AgpdRefineItem **) m_csAdminRefineItem.GetObjectSequence(&lIndex); ppAgpdRefineItem; 
		 ppAgpdRefineItem = (AgpdRefineItem **) m_csAdminRefineItem.GetObjectSequence(&lIndex))
	{
		DestroyModuleData(*ppAgpdRefineItem, AGPMREFINERY_DATATYPE_REFINE_ITEM);
	}

	m_csAdminRefineItem.Reset();

	return TRUE;
}

BOOL AgpmRefinery::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8		cOperation		= (-1);
	INT32		lCID			= 0;
	INT32		lID				= 0;
	INT32		lSourceItems[10] = {0, };
	INT32		*plSourceItems	= NULL;
	INT32		lResult			= 0;
	INT32		lResultItemTID	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lID,
						lSourceItems,
						&lResult,
						&lResultItemTID
						);

	AgpdCharacter* pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer && m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
	{
		pAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (cOperation)
	{
		case AGPMREFINERY_PACKET_OPERATION_REFINE:
	    	{
    	    	//OnOperationRefine(pAgpdCharacter, lID, lSourceItems);
		    }
		    break;

		case AGPMREFINERY_PACKET_OPERATION_RESULT:
		    {
			    EnumCallback(AGPMREFINERY_CB_RESULT, IntToPtr(lResultItemTID), IntToPtr(lResult));
		    }
		    break;
			
		case AGPMREFINERY_PACKET_OPERATION_REFINE_ITEM:
		    {
			    OnOperationRefineItem(pAgpdCharacter, lSourceItems);
		    }
			break;

        // 사용되지 않는 packet op 타입인듯.... @@@ tj@20071127
		//case AGPMREFINERY_PACKET_OPERATION_REFINE_ITEM_RESULT:
		//	{
		//	    EnumCallback(AGPMREFINERY_CB_REFINE_ITEM_RESULT, IntToPtr(lResultItemTID), IntToPtr(lResult));
		//	}
		//	break;		
		
		}

	pAgpdCharacter->m_Mutex.Release();
		
	return TRUE;
}


//	Admin
//==========================================
//
//BOOL AgpmRefinery::SetMaxRefineTemplate(INT32 lCount)
//	{
//	return m_csAdminRefineTemplate.SetCount(lCount);
//	}


//AgpdRefineTemplate* AgpmRefinery::GetRefineTemplate(INT32 lID)
//	{
//	return m_csAdminRefineTemplate.Get(lID);
//	}


BOOL AgpmRefinery::SetMaxRefineItem(INT32 lCount)
	{
	return m_csAdminRefineItem.SetCount(lCount);
	}

AgpdRefineItem2* AgpmRefinery::GetRefineItem(const char* strKey)
{
    return m_csAdminRefineItem.Get(strKey);
}


//	Refine Validation
//==========================================
//
//BOOL AgpmRefinery::IsValidStatus(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, AgpdItem *pSourceItems[], INT32 *plResult)
//{
//	INT32 lResult = AGPMREFINERY_RESULT_SUCCESS;
//	AgpdItemADChar* pAgpdItemADChar = NULL;
//
//	if (!pAgpdCharacter || !pAgpdRefineTemplate)
//	{
//		lResult = AGPMREFINERY_RESULT_NONE;
//		goto my_result1;
//	}
//		
//	// check inventory full
//	pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
//	if (!pAgpdItemADChar)
//	{
//		lResult = AGPMREFINERY_RESULT_NONE;
//		goto my_result1;
//	}
//		
//	if (m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
//	{
//		lResult = AGPMREFINERY_RESULT_FAIL_FULL_INVENTORY;
//		goto my_result1;
//	}
//	
//	// for all source item, check quantity
//	if (!IsValidItemStatus(pAgpdCharacter, pAgpdRefineTemplate->m_lItemTID, pAgpdRefineTemplate->m_lQuantity, pSourceItems))
//	{
//		lResult = AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_ITEM;
//		goto my_result1;
//	}
//
//	// check price
//	if (!IsValidMoney(pAgpdCharacter, pAgpdRefineTemplate))
//	{
//		lResult = AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_MONEY;
//		goto my_result1;
//	}
//
//  my_result1:
//	if (plResult)
//		*plResult = lResult;
//	
//	return (AGPMREFINERY_RESULT_SUCCESS == lResult);
//}


//BOOL AgpmRefinery::IsValidStatus(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, INT32 lSourceItems[], INT32 *plResult)
//{
//	AgpdItem *pSourceItems[AGPMREFINERY_MAX_GRID];
//	for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
//	{
//		if (lSourceItems[i])
//			pSourceItems[i] = m_pAgpmItem->GetItem(lSourceItems[i]);
//		else
//			pSourceItems[i] = NULL;
//	}
//	
//	return IsValidStatus(pAgpdCharacter, pAgpdRefineTemplate, pSourceItems, plResult);
//}

//
//BOOL AgpmRefinery::IsValidItemStatus(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lCount, AgpdItem *pSourceItems[])
//{
//	if (!pAgpdCharacter)
//		return FALSE;
//	
//	INT32 lOwned = 0;
//	AgpdItem *pAgpdItem = NULL;
//	for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
//	{
//		pAgpdItem = pSourceItems[i];
//		if (!pAgpdItem || lItemTID != pAgpdItem->m_lTID)
//			continue;
//		
//		if (pAgpdCharacter != pAgpdItem->m_pcsCharacter)
//			return FALSE;
//		
//		if (AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus &&
//			AGPDITEM_STATUS_CASH_INVENTORY != pAgpdItem->m_eStatus)
//		{
//			if (AGPDITEM_STATUS_SALESBOX_GRID == pAgpdItem->m_eStatus)
//				EnumCallback(AGPMREFINERY_CB_BADBOY, pAgpdCharacter, pAgpdItem);
//			return FALSE;
//		}
//
//		lOwned += pAgpdItem->m_nCount;
//		
//		if (lOwned >= lCount)
//			return TRUE;
//	}
//	
//	return FALSE;
//}
//
//
//BOOL AgpmRefinery::IsValidItemStatus(AgpdCharacter* pAgpdCharacter, INT32 lItemTID, INT32 lCount)
//{
//	INT32	lIndex	= 0;
//	INT32	lTotal	= 0;
//
//	AgpdGridItem *pAgpdGridItem;
//	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
//
//	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
//		 pAgpdGridItem;
//		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
//	{
//		// Check TID
//		if (lItemTID == pAgpdGridItem->m_lItemTID)
//		{
//			AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
//			if (!pAgpdItem)
//				return FALSE;
//
//			if (AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus &&
//				AGPDITEM_STATUS_CASH_INVENTORY != pAgpdItem->m_eStatus)
//			{
//				if (AGPDITEM_STATUS_SALESBOX_GRID == pAgpdItem->m_eStatus)
//					EnumCallback(AGPMREFINERY_CB_BADBOY, pAgpdCharacter, pAgpdItem);
//				return FALSE;
//			}
//
//			lTotal += pAgpdItem->m_nCount;
//			
//			if (lTotal >= lCount)
//				return TRUE;
//		}
//	}
//
//	return FALSE;
//}

//
//BOOL AgpmRefinery::IsValidMoney(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate)
//{
//	if (!pAgpdCharacter || !pAgpdRefineTemplate)
//		return FALSE;
//	}
//		
//	return (pAgpdCharacter->m_llMoney >= pAgpdRefineTemplate->m_lPrice);
//}

//	Item Refine Validation
//==========================================

//-----------------------------------------------------------------------------------------------//
/*서버일 경우에만 필요한 함수인가?? */
//-----------------------------------------------------------------------------------------------//
BOOL AgpmRefinery::IsValidStatusRefine(AgpdCharacter *pAgpdCharacter, INT32 lSourceItems[], INT32 *plResult, BOOL *pbSources)
{
	AgpdItem *pSourceItems[AGPMREFINERY_MAX_GRID];
	for (INT16 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
	{
		if (lSourceItems[i])
			pSourceItems[i] = m_pAgpmItem->GetItem(lSourceItems[i]);
		else
			pSourceItems[i] = NULL;
	}
	return IsValidStatusRefine(pAgpdCharacter, pSourceItems, plResult, pbSources);
}

//-----------------------------------------------------------------------------------------------//
/*
DESC: 재료 조합 가능 여부 확인
*/
//-----------------------------------------------------------------------------------------------//
BOOL AgpmRefinery::IsValidStatusRefine(AgpdCharacter *pAgpdCharacter, AgpdItem *pSourceItems[], 
                                 INT32 *plResult, BOOL *pbSources)
{   
    INT32 IResult = AGPMREFINERY_RESULT_SUCCESS;
    AgpdItemADChar* pAgpdItemADChar = NULL;

    if (!pAgpdCharacter)
    {
        IResult = AGPMREFINERY_RESULT_NONE;
        goto my_result1;
    }

    // check inventory full
	pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar)
	{
		IResult = AGPMREFINERY_RESULT_NONE;
		goto my_result1;
	}
    if (m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
	{
		IResult = AGPMREFINERY_RESULT_FAIL_FULL_INVENTORY;
		goto my_result1;
	}

    // check required item and quantity
	// for all source item, check quantity
	BOOL bNotFound = FALSE;
	BOOL bSources[AGPMREFINERY_MAX_GRID] = {FALSE, };

	//source item 검사
    //정제비용값을 위하여 결과 아이템 데이터 포인터 셋팅.
    m_pAgpdResultItem = GetRefineResultItems(pAgpdCharacter, pSourceItems);
    if(NULL == m_pAgpdResultItem)
	{
	    IResult = AGPMREFINERY_RESULT_FAIL;
		goto my_result1;
	}

    my_result1:
	    if (NULL != plResult)
		    *plResult = IResult;
		
	if (NULL != pbSources)
		CopyMemory(pbSources, bSources, sizeof(BOOL) * AGPMREFINERY_MAX_GRID);

    return (AGPMREFINERY_RESULT_SUCCESS == IResult);
}

//-----------------------------------------------------------------------------------------------//
/*     
desc: 정제기 인벤토리에 있는 유저가 넣은 재료와 시스템의 조합리스트와 비교하여 정제결과아이템을 얻어 온다. 
*/
//-----------------------------------------------------------------------------------------------//
AgpdRefineItem2* AgpmRefinery::GetRefineResultItems(AgpdCharacter *pAgpdCharacter, AgpdItem *pSourceItems[])
{
    if (!pAgpdCharacter)
	    return NULL;

    AgpdItem *pAgpdItem = NULL;
    list<AgpdRefineItemRes>    listRes;
    AgpdRefineItemRes res;

    for (INT32 i=0; i<AGPMREFINERY_MAX_GRID; ++i)
	{
	    pAgpdItem = pSourceItems[i];
	    if (!pAgpdItem)
		    continue;
    	
	    if (pAgpdCharacter != pAgpdItem->m_pcsCharacter)
		    return NULL;

	    if (AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus &&
		    AGPDITEM_STATUS_CASH_INVENTORY != pAgpdItem->m_eStatus)
	    {
		    if (AGPDITEM_STATUS_SALESBOX_GRID == pAgpdItem->m_eStatus)
			    EnumCallback(AGPMREFINERY_CB_BADBOY, pAgpdCharacter, pAgpdItem);
		    return NULL;
	    }

		// 2008.06.25. steeple
		// If the item uses stamina value, you can use it to refine.
		// 2008.01.28. steeple
		// Item 을 한번이라도 사용했다면 재료가 될 수 없다.
		if(pAgpdItem->m_lCashItemUseCount > 0 && m_pAgpmItem->IsUsingStamina(pAgpdItem->m_pcsItemTemplate) == FALSE)
			return NULL;

		// 2008.06.25. steeple
		// It's not enable to refine using items.
		if(IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType) && pAgpdItem->m_nInUseItem != AGPDITEM_CASH_ITEM_UNUSE)
			return NULL;

        // 유저의 정제 재료를 키로 만들기.
        res.m_lItemTID = pAgpdItem->m_lTID;
        if ( !((AgpdItemTemplate *)pAgpdItem->m_pcsItemTemplate)->m_bStackable )
            res.m_lQuantity = 1;
        else
            res.m_lQuantity = pAgpdItem->m_nCount;

        listRes.push_back(res);
	}
    // 유저의 정재 재료로 만든키를 시스템 재료키와 비교
    string strKey;
    
    MakeResKey(listRes, strKey);

    return m_csAdminRefineItem.Get(strKey.c_str());
}

//-----------------------------------------------------------------------------------------------//
/**/
//-----------------------------------------------------------------------------------------------//
//BOOL AgpmRefinery::IsValidMoney(AgpdCharacter *pAgpdCharacter, AgpdRefineItem2 *pAgpdRefineItem)
//{
//    return true;
//}

//	Callback setting
//==================================================
//
//BOOL AgpmRefinery::SetCallbackRefine(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//	{
//	return SetCallback(AGPMREFINERY_CB_REFINE, pfCallback, pClass);
//	}


BOOL AgpmRefinery::SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMREFINERY_CB_RESULT, pfCallback, pClass);
	}


BOOL AgpmRefinery::SetCallbackRefineItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMREFINERY_CB_REFINE_ITEM, pfCallback, pClass);
	}


//BOOL AgpmRefinery::SetCallbackRefineItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//	{
//	return SetCallback(AGPMREFINERY_CB_REFINE_ITEM_RESULT, pfCallback, pClass);
//	}

BOOL AgpmRefinery::SetCallbackBadboy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMREFINERY_CB_BADBOY, pfCallback, pClass);
	}



//	Operation
//===============================================
//
//BOOL AgpmRefinery::OnOperationRefine(AgpdCharacter* pAgpdCharacter, INT32 lItemTID, INT32 lSourceItems[])
//	{
//	if (!pAgpdCharacter)
//		return FALSE;
//
//	AgpdRefineTemplate *pAgpdRefineTemplate = m_csAdminRefineTemplate.Get(lItemTID);
//	if (!pAgpdRefineTemplate)
//		return FALSE;
//
//	// 
//	PVOID pvBuffer[2];
//	pvBuffer[0] = pAgpdRefineTemplate;
//	pvBuffer[1] = lSourceItems;
//	EnumCallback(AGPMREFINERY_CB_REFINE, pvBuffer, pAgpdCharacter);
//
//	return TRUE;
//	}

// 서버에서만 쓰는 함수?
BOOL AgpmRefinery::OnOperationRefineItem(AgpdCharacter* pAgpdCharacter, INT32 lSourceItems[])
{
	if (!pAgpdCharacter)
		return FALSE;

	PVOID pvBuffer = lSourceItems;

	EnumCallback(AGPMREFINERY_CB_REFINE_ITEM, pvBuffer, pAgpdCharacter);

	return TRUE;
}

//	Stream
//=============================================
//
//BOOL AgpmRefinery::StreamReadRefineTemplate(CHAR *pszFile, BOOL bDecryption)
//	{
//	if (!pszFile || !strlen(pszFile))
//		return FALSE;
//
//	AuExcelTxtLib	csExcelTxtLib;
//
//	if (!csExcelTxtLib.OpenExcelFile(pszFile, TRUE, bDecryption))
//		return FALSE;
//
//	for (INT32 lRow = 1; lRow < csExcelTxtLib.GetRow(); ++lRow)
//		{
//		AgpdRefineTemplate *pAgpdRefineTemplate = (AgpdRefineTemplate *) CreateModuleData(AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
//		ZeroMemory(pAgpdRefineTemplate, sizeof(AgpdRefineTemplate));
//
//		CHAR *psz = NULL;
//
//		// category
//		psz = csExcelTxtLib.GetData(AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_CATEGORY, lRow);
//		if (!psz || _T('\0') == *psz)
//			{
//			DestroyModuleData(pAgpdRefineTemplate, AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
//			continue;
//			}
//		pAgpdRefineTemplate->m_eCategory = atoi(psz);
//
//		// item tid
//		psz = csExcelTxtLib.GetData(AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_ITEMTID, lRow);
//		if (!psz || _T('\0') == *psz)
//			{
//			DestroyModuleData(pAgpdRefineTemplate, AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
//			return FALSE;
//			}
//		pAgpdRefineTemplate->m_lItemTID = atoi(psz);
//		
//		// result tid
//		psz = csExcelTxtLib.GetData(AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_RESULT_ITEMTID, lRow);
//		if (!psz || _T('\0') == *psz)
//			{
//			DestroyModuleData(pAgpdRefineTemplate, AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
//			return FALSE;
//			}
//		pAgpdRefineTemplate->m_lResultItemTID = atoi(psz);
//		
//		// quantity required 
//		psz = csExcelTxtLib.GetData(AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_QUANTITY, lRow);
//		if (!psz || _T('\0') == *psz)
//			{
//			DestroyModuleData(pAgpdRefineTemplate, AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
//			continue;
//			}
//		pAgpdRefineTemplate->m_lQuantity = atoi(psz);
//		
//		// price
//		psz = csExcelTxtLib.GetData(AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_PRICE, lRow);
//		if (!psz || _T('\0') == *psz)
//			{
//			DestroyModuleData(pAgpdRefineTemplate, AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
//			continue;
//			}
//		pAgpdRefineTemplate->m_lPrice = atoi(psz);		
//		
//		// probability
//		psz = csExcelTxtLib.GetData(AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_SUCCESS_PROB, lRow);
//		if (!psz || _T('\0') == *psz)
//			{
//			DestroyModuleData(pAgpdRefineTemplate, AGPMREFINERY_DATATYPE_REFINE_TEMPLATE);
//			continue;
//			}
//		pAgpdRefineTemplate->m_lSuccessProb = atoi(psz);
//	
//		m_csAdminRefineTemplate.Add(pAgpdRefineTemplate);
//		}
//
//	return TRUE;
//	}

BOOL AgpmRefinery::StreamReadRefineItem2(CHAR *pszFile, BOOL bDecryption)
{
	using namespace profile;
	using namespace AuExcel;

	Timer t;
	#define CHECK()	t.Log( __FILE__ , __LINE__ );

	if (!pszFile || !strlen(pszFile))
        return FALSE;

	AuExcelLib * pExcel = LoadExcelFile( pszFile , bDecryption );
	AuAutoPtr< AuExcelLib >	ptrExcel = pExcel;

	if( pExcel == NULL)
	{
		TRACE("AgpmRefinery::StreamReadRefineItem2() Error (1) !!!\n");
		return FALSE;
	}

    CHAR *psz = NULL;
    
    INT32 iResItemCnt=0;
    INT32 iMakeItemCnt=0;

    AgpdRefineItem2 *pAgpdRefineItem = NULL;
#ifdef _DEBUG
	CHECK();
#endif
	for (INT32 lRow = 1; lRow < pExcel->GetRow(); )
    {
        pAgpdRefineItem = new AgpdRefineItem2;

        //-- Read resources Item data
        // no. of resources
        iResItemCnt = pExcel->GetDataToInt( AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_TOTAL_RES, lRow);

		if( iResItemCnt <= 0)
		{
			delete pAgpdRefineItem;
			continue;
		}
        list<AgpdRefineItemRes>    listRes;
        AgpdRefineItemRes res;

        for (INT32 i=0; i< iResItemCnt; ++i)
        {
            // resource tid
            res.m_lItemTID = pExcel->GetDataToInt(i*3 + AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_RES_TID_A, lRow);

            // required quantity
            res.m_lQuantity = pExcel->GetDataToInt(i*3 + AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_RES_QTY_A, lRow);

            listRes.push_back(res);
        }

        string strKey;

        MakeResKey(listRes, strKey);

        pAgpdRefineItem->m_RefineKey = strKey;

        //-- Read result item data
        // result item Count
        iMakeItemCnt = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_MAKEITEM_TOTAL, lRow);

        for( INT32 j = 0; j < iMakeItemCnt; j++)
        {
			AgpdRefineMakeItem MakeItem;
            memset(&MakeItem,0,sizeof(MakeItem));

            // result item tid
            MakeItem.m_lMakeItemTID = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_MAKEITEM_ITEMTID, lRow);

			// result item count
			MakeItem.m_IMakeItemCount = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_MAKEITEM_ITEM_COUNT, lRow);

			// valid time
			MakeItem.m_lValidTime = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_VALID_TIME, lRow);

            // bonus item tid
            MakeItem.m_IBonusItemTID = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_BONUS_ITEMTID, lRow);

            // option group MIN
            MakeItem.m_lOptionMIN = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_OPTION_MIN, lRow);

            // option group MAX
            MakeItem.m_lOptionMAX = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_OPTION_MAX, lRow);

            // socket inchant MIN
            MakeItem.m_lSocketInchantMIN = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_SOCKET_INCHANT_MIN, lRow);

            // socket inchant MAX
            MakeItem.m_lSocketInchantMAX = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_SOCKET_INCHANT_MAX, lRow);

            // price
            pAgpdRefineItem->m_lPrice = pExcel->GetDataToInt(AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_PRICE, lRow);

			pAgpdRefineItem->m_RefineMakeItemVec.push_back(MakeItem);

			lRow++;
        }

        m_csAdminRefineItem.Add(pAgpdRefineItem);
/*
#ifdef _DEBUG
        char refineItemLog[256]={0,};
        sprintf(refineItemLog,"[%d행] %s \n",lRow,strKey.c_str());
        TRACE(refineItemLog);
#endif
*/
    }
#ifdef _DEBUG
	CHECK();
#endif
	pExcel->CloseFile();
    return TRUE;
}

void AgpmRefinery::MakeResKey(list<AgpdRefineItemRes>& reslist, string& strKey)
{
    char strElemRes[256]={0,};

    reslist.sort();

    list<AgpdRefineItemRes>::iterator iter = reslist.begin();

    while(iter != reslist.end())
    {
        AgpdRefineItemRes* pcsItemRes = &( *iter );

        sprintf(strElemRes, "%d:%d|",pcsItemRes->m_lItemTID, pcsItemRes->m_lQuantity);
        strKey += strElemRes;
        iter++;
    }
}