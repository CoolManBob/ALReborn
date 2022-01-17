/*===========================================================================

	AgpmMailBox.cpp

===========================================================================*/


#include "AgpmMailBox.h"
#include "ApMemoryTracker.h"


/****************************************************/
/*		The Implementation of AgpmMailBox class		*/
/****************************************************/
//
AgpmMailBox::AgpmMailBox()
	{
	SetModuleName(_T("AgpmMailBox"));
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMMAILBOX_PACKET_TYPE);

	SetModuleData(sizeof(AgpdMail), AGPMMAILBOX_DATATYPE_MAIL);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1, // operation
							AUTYPE_INT32,			1, // cid
							AUTYPE_PACKET,			1, // embedded packet(Mail)
							AUTYPE_INT32,			1, // item id(write only)
							AUTYPE_INT32,			1, // result
							AUTYPE_END,				0
							);

	m_csPacketMail.SetFlagLength(sizeof(INT16));
	m_csPacketMail.SetFieldType(AUTYPE_INT32,			1,									// mail id
								AUTYPE_CHAR,			AGPDCHARACTER_NAME_LENGTH + 1,		// from charid
								AUTYPE_CHAR,			AGPDCHARACTER_NAME_LENGTH + 1,		// to charid
								AUTYPE_CHAR,			AGPMMAILBOX_MAX_SUBJECT_LENGTH + 1,	// subject
								AUTYPE_INT32,			1,									// date(time_t)
								AUTYPE_INT32,			1,									// flag
								AUTYPE_INT32,			1,									// item tid
								AUTYPE_INT16,			1,									// item qty
								AUTYPE_MEMORY_BLOCK,	1,									// content
								AUTYPE_END,				0
								);

	m_pAgpmGrid = NULL;
	m_pApmMap = NULL;
	m_pAgpmCharacter = NULL;
	m_pAgpmFactors = NULL;
	m_pAgpmItem = NULL;

	m_nIndexCharacterAD = -1;
	}


AgpmMailBox::~AgpmMailBox()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgpmMailBox::OnAddModule()
	{
	m_pApmMap				= (ApmMap *) GetModule(_T("ApmMap"));
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmFactors			= (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmItem				= (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmGrid				= (AgpmGrid *) GetModule(_T("AgpmGrid"));

	if (!m_pApmMap || !m_pAgpmCharacter || !m_pAgpmFactors || !m_pAgpmItem || !m_pAgpmGrid)
		return FALSE;
	
	// mail character attached data
	m_nIndexCharacterAD = m_pAgpmCharacter->AttachCharacterData(this, sizeof(AgpdMailCAD),
																ConAgpdMailCAD,
																DesAgpdMailCAD
																);
	if (m_nIndexCharacterAD < 0)
		return FALSE;

	return TRUE;
	}


BOOL AgpmMailBox::OnInit()
	{
	if (!m_csAdmin.InitializeObject(sizeof(AgpdMail *), m_csAdmin.GetCount()))
		return FALSE;
	
	return TRUE;
	}

	
BOOL AgpmMailBox::OnDestroy()
	{
	INT32 lIndex = 0;
	AgpdMail **ppAgpdMail = NULL;

	for (ppAgpdMail = (AgpdMail **) m_csAdmin.GetObjectSequence(&lIndex); ppAgpdMail; 
		 ppAgpdMail = (AgpdMail **) m_csAdmin.GetObjectSequence(&lIndex))
		{
		DestroyModuleData(*ppAgpdMail, AGPMMAILBOX_DATATYPE_MAIL);
		}

	m_csAdmin.Reset();
	return TRUE;
	}


BOOL AgpmMailBox::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8	cOperation = AGPMMAILBOX_OPERATION_NONE;
	INT32	lCID = AP_INVALID_CID;
	INT32	lItemID = 0;
	INT32	lResult = AGPMMAILBOX_RESULT_NONE;
	PVOID	pvPacketEmb = NULL;
	AgpdMailArg	stArg;
	
	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&pvPacketEmb,
						&lItemID,
						&lResult
						);

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	switch (cOperation)
		{
		case AGPMMAILBOX_OPERATION_ADD_MAIL :
			{
			AgpdMail *pAgpdMail = CreateMail();
			if (NULL == pAgpdMail)
				return FALSE;
			
			if (FALSE == ParsePacketMail(pvPacketEmb, pAgpdMail))
				{
				DestroyMail(pAgpdMail);
				return FALSE;
				}
			
			stArg.m_pAgpdMail = pAgpdMail;
			stArg.m_lResult = lResult;
			
			EnumCallback(AGPMMAILBOX_CB_ADD_MAIL, &stArg, pAgpdCharacter);
			}
			break;
		
		case AGPMMAILBOX_OPERATION_REMOVE_MAIL :
			{
			AgpdMail csMail;
			
			if (FALSE == ParsePacketMail(pvPacketEmb, &csMail))
				return FALSE;
				
			stArg.m_pAgpdMail = &csMail;
			stArg.m_lResult = lResult;
			
			EnumCallback(AGPMMAILBOX_CB_REMOVE_MAIL, &stArg, pAgpdCharacter);
			}
			break;

		case AGPMMAILBOX_OPERATION_READ_MAIL :
			{
			AgpdMail csMail;
			
			if (FALSE == ParsePacketMail(pvPacketEmb, &csMail))
				return FALSE;
				
			stArg.m_pAgpdMail = &csMail;
			stArg.m_lResult = lResult;		
			
			EnumCallback(AGPMMAILBOX_CB_READ_MAIL, &stArg, pAgpdCharacter);
			}
			break;

		case AGPMMAILBOX_OPERATION_WRITE_MAIL :
			{
			AgpdMail csMail;

			// packet may have NULL value
			ParsePacketMail(pvPacketEmb, &csMail);

			stArg.m_pAgpdMail = &csMail;
			stArg.m_lItemID = lItemID;
			stArg.m_lResult = lResult;
			
			EnumCallback(AGPMMAILBOX_CB_WRITE_MAIL, &stArg, pAgpdCharacter);
			}
			break;

		case AGPMMAILBOX_OPERATION_ITEM_SAVE :
			{
			AgpdMail csMail;
			
			if (FALSE == ParsePacketMail(pvPacketEmb, &csMail))
				return FALSE;
				
			stArg.m_pAgpdMail = &csMail;
			stArg.m_lResult = lResult;		
			
			EnumCallback(AGPMMAILBOX_CB_ITEM_SAVE, &stArg, pAgpdCharacter);
			}
			break;

		case AGPMMAILBOX_OPERATION_REMOVE_ALL_MAIL :
			{
			if (pstCheckArg->bReceivedFromServer)
				RemoveAllMail(pAgpdCharacter);
			}
			break;

		default :
			break;
		}

	return TRUE;
	}




//	Admin
//============================
//
BOOL AgpmMailBox::SetMaxMail(INT32 lCount)
	{
	return m_csAdmin.SetCount(lCount);
	}


AgpdMail* AgpmMailBox::CreateMail()
	{
	AgpdMail* pAgpdMail = (AgpdMail *) CreateModuleData(AGPMMAILBOX_DATATYPE_MAIL);
	if (pAgpdMail)
		{
		pAgpdMail->Init();
		}
		
	return pAgpdMail;
	}


void AgpmMailBox::DestroyMail(AgpdMail* pAgpdMail)
	{
	if (pAgpdMail)
		{
		DestroyModuleData(pAgpdMail, AGPMMAILBOX_DATATYPE_MAIL);
		}
	}


AgpdMail* AgpmMailBox::GetMail(INT32 lID)
	{
	return m_csAdmin.Get(lID);
	}


BOOL AgpmMailBox::AddMail(AgpdMail *pAgpdMail)
	{
	return m_csAdmin.Add(pAgpdMail);
	}


BOOL AgpmMailBox::RemoveMail(AgpdMail *pAgpdMail)
	{
	return m_csAdmin.Remove(pAgpdMail);
	}




//	CAD
//====================================
//
BOOL AgpmMailBox::ConAgpdMailCAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmMailBox	*pThis		= (AgpmMailBox *) pClass;
	ApBase		*pApBase	= (ApBase *) pData;

	AgpdMailCAD	*pAgpdMailCAD = pThis->GetCAD(pApBase);
	if (pAgpdMailCAD)
		{
		pAgpdMailCAD->Init();
		pAgpdMailCAD->m_Mutex.Init();
		}
	else
		return FALSE;

	return TRUE;
	}


BOOL AgpmMailBox::DesAgpdMailCAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmMailBox	*pThis		= (AgpmMailBox *) pClass;
	ApBase		*pApBase	= (ApBase *) pData;

	AgpdMailCAD	*pAgpdMailCAD = pThis->GetCAD(pApBase);
	if (pAgpdMailCAD)
		{
		pAgpdMailCAD->Init();
		pAgpdMailCAD->m_Mutex.Destroy();
		}
	else
		return FALSE;

	return TRUE;
	}


AgpdMailCAD* AgpmMailBox::GetCAD(ApBase *pApBase)
	{
	if (!pApBase || APBASE_TYPE_CHARACTER != pApBase->m_eType)
		return NULL;

	return (AgpdMailCAD *) m_pAgpmCharacter->GetAttachedModuleData(m_nIndexCharacterAD, (PVOID) pApBase);
	}




//	CAD
//=========================================
//
BOOL AgpmMailBox::AddMailToCAD(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail, BOOL bHead, INT32 *plRemovedID)
	{
	if (!pAgpdCharacter || !pAgpdMail)
		return FALSE;
	
	if (0 != _tcscmp(pAgpdCharacter->m_szID, pAgpdMail->m_szToCharID))
		return FALSE;
	
	BOOL bResult = FALSE;
	AgpdMailCAD *pAgpdMailCAD = GetCAD(pAgpdCharacter);
	if (pAgpdMailCAD)
		{
		pAgpdMailCAD->m_Mutex.WLock();
		if (bHead)
			bResult = pAgpdMailCAD->AddHead(pAgpdMail->m_lID);
		else
			bResult = pAgpdMailCAD->AddTail(pAgpdMail->m_lID);
		
		if (!bResult)
			{
			INT32 lID = pAgpdMailCAD->RemoveTail();
			AgpdMail *pAgpdMailRemove = GetMail(lID);
			if (NULL != pAgpdMailRemove)
				{
				if (plRemovedID)
					*plRemovedID = lID;
				
				RemoveMail(pAgpdMailRemove);
				DestroyMail(pAgpdMailRemove);
				}

			if (bHead)
				bResult = pAgpdMailCAD->AddHead(pAgpdMail->m_lID);
			else
				bResult = pAgpdMailCAD->AddTail(pAgpdMail->m_lID);
			}
		
		pAgpdMailCAD->m_Mutex.Release();
		}

	return bResult;
	}


BOOL AgpmMailBox::RemoveMailFromCAD(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail)
	{
	if (!pAgpdCharacter || !pAgpdMail)
		return FALSE;

	BOOL bResult = FALSE;
	
	if (0 != _tcscmp(pAgpdCharacter->m_szID, pAgpdMail->m_szToCharID))
		return bResult;	

	AgpdMailCAD *pAgpdMailCAD = GetCAD(pAgpdCharacter);
	if (pAgpdMailCAD)
		{
		pAgpdMailCAD->m_Mutex.WLock();
		bResult = pAgpdMailCAD->Remove(pAgpdMail->m_lID);
		pAgpdMailCAD->m_Mutex.Release();
		}
	
	return bResult;
	}


BOOL AgpmMailBox::RemoveMailFromCAD(AgpdCharacter *pAgpdCharacter, INT32 lMailID)
	{
	return RemoveMailFromCAD(pAgpdCharacter, GetMail(lMailID));
	}


BOOL AgpmMailBox::RemoveAllMail(AgpdCharacter *pAgpdCharacter, AuGenerateID *pGenerateID)
	{
	AgpdMailCAD *pAgpdMailCAD = GetCAD(pAgpdCharacter);
	if (!pAgpdMailCAD)
		return FALSE;
	
	AuAutoLock Lock(pAgpdMailCAD->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	for (INT16 i=0; i<pAgpdMailCAD->GetCount(); ++i)
		{
		INT32 lMailID = pAgpdMailCAD->Get(i);
		AgpdMail *pAgpdMail = GetMail(lMailID);
		if (pAgpdMail)
			{
			RemoveMail(pAgpdMail);
			if (pGenerateID)
				pGenerateID->AddRemoveID(pAgpdMail->m_lID);
			DestroyMail(pAgpdMail);
			}
		}
	
	pAgpdMailCAD->Init();
	
	return TRUE;
	}


AgpdMail* AgpmMailBox::FindMailFromCAD(AgpdCharacter *pAgpdCharacter, UINT64 ullDBID)
	{
	// get AD
	AgpdMailCAD *pAgpdMailCAD = GetCAD(pAgpdCharacter);
	if (!pAgpdMailCAD)
		return NULL;

	AuAutoLock LockMailCAD(pAgpdMailCAD->m_Mutex);
	if (!LockMailCAD.Result()) return NULL;

	for (INT16 i=0; i<pAgpdMailCAD->GetCount(); ++i)
		{
		AgpdMail *pAgpdMail = GetMail(pAgpdMailCAD->Get(i));
		if (pAgpdMail)
			{
			if (ullDBID == pAgpdMail->m_ullDBID)
				{
				return pAgpdMail;
				}
			}
		}
	
	return NULL;
	}




//	Validation, ...
//===================================================
//
static const INT32 s_lNormalTID = 3601;
static const INT32 s_lPremiumTID = 3602;

BOOL AgpmMailBox::GetLetterCount(AgpdCharacter *pAgpdCharacter, INT32 &lNormal, INT32 &lPremium)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;

	lNormal = 0;
	lPremium = 0;

	INT32	lIndex	= 0;
	AgpdGridItem *pAgpdGridItem;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);

	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
		{
		// Check TID
		if (s_lNormalTID == pAgpdGridItem->m_lItemTID
			|| s_lPremiumTID == pAgpdGridItem->m_lItemTID)
			{
			AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if (!pAgpdItem || AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
				continue;

			INT32 lStack = pAgpdItem->m_pcsItemTemplate->m_bStackable ? pAgpdItem->m_nCount : 1;
			if (s_lNormalTID == pAgpdGridItem->m_lItemTID)
				lNormal += lStack;
			else
				lPremium += lStack;
			}
		}
	
	return TRUE;
	}


BOOL AgpmMailBox::SubLetter(AgpdCharacter *pAgpdCharacter, BOOL bNormal)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;

	INT32	lIndex	= 0;
	AgpdGridItem *pAgpdGridItem;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);

	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
		{
		if ((bNormal && s_lNormalTID == pAgpdGridItem->m_lItemTID)
			|| (!bNormal && s_lPremiumTID == pAgpdGridItem->m_lItemTID)
			)
			{
			AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem->m_lItemID);
			if (!pAgpdItem || AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
				continue;
			
			m_pAgpmItem->SubItemStackCount(pAgpdItem, 1);
			return TRUE;
			}
		}
	
	return FALSE;
	}


BOOL AgpmMailBox::IsValidAttachItem(AgpdItem *pAgpdItem, INT32 *plResult)
	{
	INT32 lResult = AGPMMAILBOX_RESULT_SUCCESS;

	if (NULL == pAgpdItem)
		{
		lResult = AGPMMAILBOX_RESULT_FAIL;
		goto my_result;
		}

	// can't attach cash item
	if (IS_CASH_ITEM(pAgpdItem->m_pcsItemTemplate->m_eCashItemType))
		{
		lResult = AGPMMAILBOX_RESULT_CANT_ATT_CASH_ITEM;
		goto my_result;
		}

	// must be in inventory or subinventory
	if (AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus &&
		AGPDITEM_STATUS_SUB_INVENTORY != pAgpdItem->m_eStatus)
		{
		lResult = AGPMMAILBOX_RESULT_NOT_IN_INVEN;
		goto my_result;
		}

	// bound type check
	if (m_pAgpmItem->GetBoundType(pAgpdItem) != E_AGPMITEM_NOT_BOUND)
		{
		lResult = AGPMMAILBOX_RESULT_REVERTED_ITEM;
		goto my_result;
		}

	// can't attach letter item - 2007.07.20. steeple 추가 작업
	if (pAgpdItem->m_pcsItemTemplate->m_lID == s_lNormalTID ||
		pAgpdItem->m_pcsItemTemplate->m_lID == s_lPremiumTID)
		{
		lResult = AGPMMAILBOX_RESULT_INVALID_ITEM;
		goto my_result;
		}

	// 북미 pre-order pack에 들어 있는 아이템이 첨부시 이상하게 변한다고 해서
	// 일단 메일 첨부 금지
	if (IsWesterPreOrderItem(pAgpdItem->m_pcsItemTemplate->m_lID))
		{
		lResult = AGPMMAILBOX_RESULT_INVALID_ITEM;
		goto my_result;
		}
	//#######
	
  my_result :
	if (plResult)
		*plResult = lResult;
  
	return (AGPMMAILBOX_RESULT_SUCCESS == lResult);
	}

BOOL AgpmMailBox::IsWesterPreOrderItem(INT32 lTID)
{
	BOOL ret = FALSE;
	switch (lTID)
	{
	case 4609:
	case 4610:
	case 4611:
	case 4612:
	case 4613:
	case 4614:
	case 4615:
	case 4616:
	case 4617:
	case 4618:
	case 4619:
	case 4620:
	case 4621:
	case 4622:
	case 4623:
		ret = TRUE;
	}

	return ret;
}




//	Mail packet
//===================================================
//
BOOL AgpmMailBox::ParsePacketMail(PVOID pvPacketMail, AgpdMail *pAgpdMail)
	{
	if (NULL == pvPacketMail || NULL == pAgpdMail)
		return FALSE;
	
	TCHAR *pszFromCharID = NULL;
	TCHAR *pszToCharID = NULL;
	TCHAR *pszSubject = NULL;
	PVOID pvContent = NULL;
	INT16 nContentLength = 0;
			
	m_csPacketMail.GetField(FALSE, pvPacketMail, 0,
						&pAgpdMail->m_lID,				// mail id
						&pszFromCharID,					// from charid
						&pszToCharID,					// to charid
						&pszSubject,					// subject
						&pAgpdMail->m_lDate,			// date
						&pAgpdMail->m_ulFlag,			// flag
						&pAgpdMail->m_lItemTID,			// item tid
						&pAgpdMail->m_nItemQty,			// item qty
						&pvContent,						// content
						&nContentLength
						);

	_tcscpy(pAgpdMail->m_szFromCharID, pszFromCharID ? pszFromCharID : _T(""));
	_tcscpy(pAgpdMail->m_szToCharID, pszToCharID ? pszToCharID : _T(""));
	_tcscpy(pAgpdMail->m_szSubject, pszSubject ? pszSubject : _T(""));
	pAgpdMail->SetContent(pvContent, nContentLength);
	
	return TRUE;
	}


PVOID AgpmMailBox::MakePacketMail(AgpdMail *pAgpdMail, INT16 *pnPacketLength)
	{
	if (NULL == pAgpdMail || NULL == pnPacketLength)
		return NULL;

	PVOID pvContent = pAgpdMail->GetContent();
	INT16 nContentSize = pAgpdMail->GetContentSize();
	
	PVOID pvPacketMail = NULL;
	if (NULL == pvContent || 0 >= nContentSize)
		{
		pvPacketMail = m_csPacketMail.MakePacket(FALSE, pnPacketLength, 0,
								&pAgpdMail->m_lID,
								pAgpdMail->m_szFromCharID,
								pAgpdMail->m_szToCharID,
								pAgpdMail->m_szSubject,
								&pAgpdMail->m_lDate,
								&pAgpdMail->m_ulFlag,
								&pAgpdMail->m_lItemTID,
								&pAgpdMail->m_nItemQty,
								NULL						// content is null
								);
		}
	else
		{
		pvPacketMail = m_csPacketMail.MakePacket(FALSE, pnPacketLength, 0,
								&pAgpdMail->m_lID,
								pAgpdMail->m_szFromCharID,
								pAgpdMail->m_szToCharID,
								pAgpdMail->m_szSubject,
								&pAgpdMail->m_lDate,
								&pAgpdMail->m_ulFlag,
								&pAgpdMail->m_lItemTID,
								&pAgpdMail->m_nItemQty,
								pvContent,
								&nContentSize
								);
		}

	return pvPacketMail;
	}


PVOID AgpmMailBox::MakePacketMailOnlyID(INT32 lMailID, INT16 *pnPacketLength)
	{
	if (NULL == pnPacketLength)
		return NULL;

	PVOID pvPacketMail = NULL;
	pvPacketMail = m_csPacketMail.MakePacket(FALSE, pnPacketLength, 0,
								&lMailID,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL						// content is null
								);
	
	return pvPacketMail;
	}




//	Callback setting
//===================================================
//
BOOL AgpmMailBox::SetCallbackAddMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMMAILBOX_CB_ADD_MAIL, pfCallback, pClass);
	}


BOOL AgpmMailBox::SetCallbackRemoveMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMMAILBOX_CB_REMOVE_MAIL, pfCallback, pClass);
	}


BOOL AgpmMailBox::SetCallbackReadMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMMAILBOX_CB_READ_MAIL, pfCallback, pClass);
	}


BOOL AgpmMailBox::SetCallbackWriteMail(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMMAILBOX_CB_WRITE_MAIL, pfCallback, pClass);
	}


BOOL AgpmMailBox::SetCallbackItemSave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMMAILBOX_CB_ITEM_SAVE, pfCallback, pClass);
	}

