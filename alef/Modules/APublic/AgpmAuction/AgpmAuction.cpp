/*===========================================================================

	AgpmAuction.cpp

===========================================================================*/


#include "AgpmAuction.h"
#include "ApMemoryTracker.h"


/****************************************************/
/*		The Implementation of AgpmAuction class		*/
/****************************************************/
//
AgpmAuction::AgpmAuction()
	{
	SetModuleName(_T("AgpmAuction"));
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMAUCTION_PACKET_TYPE);

	SetModuleData(sizeof(AgpdAuctionSales), AGPMAUCTION_DATATYPE_SALES);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1, // operation
							AUTYPE_INT32,			1, // cid
							AUTYPE_PACKET,			1, // embedded packet
							AUTYPE_INT32,			1, // result
							AUTYPE_END,				0
							);

	m_csPacketSales.SetFlagLength(sizeof(INT16));
	m_csPacketSales.SetFieldType(AUTYPE_INT32,		1,	// sales id
								 AUTYPE_UINT64,		1,	// doc id.
								 AUTYPE_UINT64,		1,	// item seq
								 AUTYPE_INT32,		1,	// item id
								 AUTYPE_INT32,		1,	// price
								 AUTYPE_INT16,		1,	// quantity
								 AUTYPE_INT16,		1,	// status
								 AUTYPE_CHAR,		33,	// reg date
								 AUTYPE_INT32,		1,	// item tid
								 AUTYPE_END,		0
							   );

	m_csPacketSelect.SetFlagLength(sizeof(INT8));
	m_csPacketSelect.SetFieldType(AUTYPE_INT32,			1,	// item tid
								  AUTYPE_UINT32,		1,	// page
								  AUTYPE_PACKET,		1,	// item info.(rowset)
								  AUTYPE_END,			0
								  );

	m_csPacketSelect2.SetFlagLength(sizeof(INT8));
	m_csPacketSelect2.SetFieldType(AUTYPE_INT32,		1,	// item tid
								  AUTYPE_UINT64,		1,	// doc id.(max, min)
								  AUTYPE_INT16,			1,	// flag(forward, backward)
								  AUTYPE_INT32,			1,	// total no. of sales of Item TID
								  AUTYPE_PACKET,		1,	// item info.(rowset)
								  AUTYPE_END,			0
								  );

	m_csPacketSell.SetFlagLength(sizeof(INT8));
	m_csPacketSell.SetFieldType(AUTYPE_INT32,			1,	// item id
								AUTYPE_INT16,			1,	// quantity
								AUTYPE_INT32,			1,	// price
								AUTYPE_CHAR,			33,	// date
								AUTYPE_END,				0
								);


	m_csPacketCancel.SetFlagLength(sizeof(INT8));
	m_csPacketCancel.SetFieldType(AUTYPE_INT32,			1,	// sales id
								  AUTYPE_INT32,			1,	// item tid
								  AUTYPE_INT16,			1,	// quantity remained
								  AUTYPE_END,			0
								  );


	m_csPacketConfirm.SetFlagLength(sizeof(INT8));
	m_csPacketConfirm.SetFieldType(AUTYPE_INT32,		1,	// sales id
								   AUTYPE_INT32,		1,	// item tid
								   AUTYPE_INT16,		1,	// quantity selled
								   AUTYPE_INT32,		1,	// income
								   AUTYPE_END,			0
								   );

	m_csPacketBuy.SetFlagLength(sizeof(INT8));
	m_csPacketBuy.SetFieldType(AUTYPE_INT64,			1,	// doc id
							   AUTYPE_INT16,			1,	// quantity
							   AUTYPE_INT32,			1,	// item tid
							   AUTYPE_INT32,			1,	// price
							   AUTYPE_END,				0
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
								  
	m_pAgpmGrid = NULL;
	m_pApmMap = NULL;
	m_pApmEventManager = NULL;
	m_pAgpmCharacter = NULL;
	m_pAgpmFactors = NULL;
	m_pAgpmItem = NULL;

	m_nIndexCharacterAD = -1;

	m_Mutex.Init();
	}

AgpmAuction::~AgpmAuction()
	{
		m_Mutex.Destroy();
	}




//	ApModule inherited
//======================================
//
BOOL AgpmAuction::OnAddModule()
	{
	m_pAgpmGrid				= (AgpmGrid *) GetModule(_T("AgpmGrid"));
	m_pApmMap				= (ApmMap *) GetModule(_T("ApmMap"));
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmFactors			= (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmItem				= (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pApmEventManager		= (ApmEventManager *) GetModule(_T("ApmEventManager"));

	// register event
	if (!m_pApmEventManager->RegisterEvent(APDEVENT_FUNCTION_AUCTION, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	if (!m_pApmMap || !m_pAgpmCharacter || !m_pAgpmFactors || !m_pAgpmItem)
		return FALSE;
	
	 m_nIndexCharacterAD = m_pAgpmCharacter->AttachCharacterData(this, sizeof(AgpdAuctionCAD),
																 ConAgpdAuctionCAD,
																 DesAgpdAuctionCAD
																 );
	if (m_nIndexCharacterAD < 0)
		return FALSE;
		
	return TRUE;
	}


BOOL AgpmAuction::OnInit()
	{
	if (!m_csAdmin.InitializeObject(sizeof(AgpdAuctionSales *), m_csAdmin.GetCount()))
		return FALSE;
	
	return TRUE;
	}

	
BOOL AgpmAuction::OnDestroy()
	{
	INT32 lIndex = 0;
	AgpdAuctionSales **ppAgpdAuctionSales = NULL;

	for (ppAgpdAuctionSales = (AgpdAuctionSales **) m_csAdmin.GetObjectSequence(&lIndex); ppAgpdAuctionSales; 
		 ppAgpdAuctionSales = (AgpdAuctionSales **) m_csAdmin.GetObjectSequence(&lIndex))
		{
		DestroyModuleData(*ppAgpdAuctionSales, AGPMAUCTION_DATATYPE_SALES);
		}

	m_csAdmin.Reset();
	return TRUE;
	}


BOOL AgpmAuction::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize == 0)
		return FALSE;

	AgpmAuctionArg	cAuctionArg;
	cAuctionArg.Init();
	INT8			cOperation = AGPMAUCTION_OPERATION_NONE;
	INT32			lCID;
	INT32			lResult;
	PVOID			pvPacketEmb = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&pvPacketEmb,
						&lResult
						);

	cAuctionArg.m_cOperation = cOperation;
	cAuctionArg.m_lCID = lCID;
	cAuctionArg.m_lResult = lResult;
	
	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result())
		return FALSE;
	
	if (!pstCheckArg->bReceivedFromServer && m_pAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
		return FALSE;
	
	switch (cOperation)
		{
		case AGPMAUCTION_OPERATION_ADD_SALES :
			{
			if (!pvPacketEmb)
				return FALSE;
			
			AgpdAuctionSales *pAgpdAuctionSales = CreateSales();
			if (!pAgpdAuctionSales)
				return FALSE;
		
			TCHAR *pszDate = NULL;
			m_csPacketSales.GetField(FALSE, pvPacketEmb, 0,
									 &pAgpdAuctionSales->m_lID,
								     &pAgpdAuctionSales->m_ullDocID,
								     &pAgpdAuctionSales->m_ullItemSeq,
								     &pAgpdAuctionSales->m_lItemID,
								     &pAgpdAuctionSales->m_lPrice,
								     &pAgpdAuctionSales->m_nQuantity,
								     &pAgpdAuctionSales->m_nStatus,
								     &pszDate,
								     &pAgpdAuctionSales->m_lItemTID
								     );
			_tcsncpy(pAgpdAuctionSales->m_szDate, pszDate ? pszDate : _T(""), 32);
			
			if (AddSalesToCAD(pAgpdCharacter, pAgpdAuctionSales))
				EnumCallback(AGPMAUCTION_CB_ADD_SALES, pAgpdAuctionSales, pAgpdCharacter);
			else
				DestroySales(pAgpdAuctionSales);
			}
			break;
		
		case AGPMAUCTION_OPERATION_REMOVE_SALES :
			{
			if (!pvPacketEmb)
				return FALSE;
			
			INT32 lSalesID = 0;

			m_csPacketSales.GetField(FALSE, pvPacketEmb, 0,
									 &lSalesID,
								     NULL,
								     NULL,
								     NULL,
								     NULL,
								     NULL,
								     NULL,
								     NULL,
								     NULL
								     );

			// !!! Caution : Sales of lSaleID can't find from Admin. it already removed
			
			AgpdAuctionSales *pAgpdAuctionSales = GetSales(lSalesID);
			if (pAgpdAuctionSales)
				{
				if (RemoveSalesFromCAD(pAgpdCharacter, pAgpdAuctionSales))
					{
					EnumCallback(AGPMAUCTION_CB_REMOVE_SALES, pAgpdAuctionSales, pAgpdCharacter);
					DestroySales(pAgpdAuctionSales);
					}
				}
			}

			break;

		case AGPMAUCTION_OPERATION_UPDATE_SALES :
			{
			if (!pvPacketEmb)
				return FALSE;
			
			INT32 lSalesID = 0;
			INT16 nStatus = 0;			
		
			TCHAR *pszDate = NULL;
			m_csPacketSales.GetField(FALSE, pvPacketEmb, 0,
									 &lSalesID,
								     NULL,
								     NULL,
								     NULL,
								     NULL,
								     NULL,
								     &nStatus,
								     NULL,
								     NULL
								     );

			PVOID pvBuffer[2];
			pvBuffer[0] = IntToPtr(lSalesID);
			pvBuffer[1] = IntToPtr(nStatus);
						
			if (UpdateSalesStatus(lSalesID, nStatus))
				EnumCallback(AGPMAUCTION_CB_UPDATE_SALES, pvBuffer, pAgpdCharacter);
			}
			break;

		case AGPMAUCTION_OPERATION_SELECT :
			return FALSE;
			break;

		case AGPMAUCTION_OPERATION_SELECT2 :
			{
			if (!pvPacketEmb)
				return FALSE;
			
			m_csPacketSelect2.GetField(FALSE, pvPacketEmb, 0,
									  &cAuctionArg.m_lItemTID,
									  &cAuctionArg.m_ullDocID,
									  &cAuctionArg.m_nFlag,
									  &cAuctionArg.m_lTotalSales,
									  &cAuctionArg.m_pvRowsetPacket
									  );

			EnumCallback(AGPMAUCTION_CB_SELECT2, &cAuctionArg, pAgpdCharacter);
			}
			break;
			
		case AGPMAUCTION_OPERATION_SELL :
			{
			if (!pvPacketEmb)
				return FALSE;
			
			TCHAR *pszDate = NULL;
			
			m_csPacketSell.GetField(FALSE, pvPacketEmb, 0,
									&cAuctionArg.m_lItemID,
									&cAuctionArg.m_nQuantity,
									&cAuctionArg.m_lMoney,
									&pszDate
									);

			_tcsncpy(cAuctionArg.m_szDate, pszDate ? pszDate : _T(""), 32);

			EnumCallback(AGPMAUCTION_CB_SELL, &cAuctionArg, pAgpdCharacter);
			}
			break;

		case AGPMAUCTION_OPERATION_CANCEL :
			{
			if (!pvPacketEmb)
				return FALSE;
							
			m_csPacketCancel.GetField(FALSE, pvPacketEmb, 0,
									  &cAuctionArg.m_lSalesID,
									  &cAuctionArg.m_lItemTID,
									  &cAuctionArg.m_nQuantity
									  );

			EnumCallback(AGPMAUCTION_CB_CANCEL, &cAuctionArg, pAgpdCharacter);
			}
			break;

		case AGPMAUCTION_OPERATION_CONFIRM :
			{
			if (!pvPacketEmb)
				return FALSE;

			m_csPacketConfirm.GetField(FALSE, pvPacketEmb, 0,
									   &cAuctionArg.m_lSalesID,
									   &cAuctionArg.m_lItemTID,
									   &cAuctionArg.m_nQuantity,
									   &cAuctionArg.m_lMoney
									   );

			EnumCallback(AGPMAUCTION_CB_CONFIRM, &cAuctionArg, pAgpdCharacter);
			}
			break;			

		case AGPMAUCTION_OPERATION_BUY :
			{
			if (!pvPacketEmb)
				return FALSE;

			m_csPacketBuy.GetField(FALSE, pvPacketEmb, 0,
								   &cAuctionArg.m_ullDocID,
								   &cAuctionArg.m_nQuantity,
								   &cAuctionArg.m_lItemTID,
								   &cAuctionArg.m_lMoney
								   );
					
			EnumCallback(AGPMAUCTION_CB_BUY, &cAuctionArg, pAgpdCharacter);
			}
			break;
		
		case AGPMAUCTION_OPERATION_NOTIFY :
			{
			EnumCallback(AGPMAUCTION_CB_NOTIFY, &cAuctionArg, pAgpdCharacter);
			}
			break;
		
		case AGPMAUCTION_OPERATION_EVENT_REQUEST :
			{
			ApdEvent *pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if(!pApdEvent || !pAgpdCharacter)
				return FALSE;

			AuPOS stTargetPos;
			memset(&stTargetPos, 0, sizeof(stTargetPos));
			
			if (m_pApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, 1600, &stTargetPos))
				{
				pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
				if (pAgpdCharacter->m_bMove)
					m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

				if (m_pAgpmCharacter->HasPenalty(pAgpdCharacter, AGPMCHAR_PENALTY_AUCTION))
					return FALSE;

				if( m_pAgpmCharacter->HasPenalty( pAgpdCharacter , AGPMCHAR_PENALTY_PRVTRADE ))
					return FALSE;

				EnumCallback(AGPMAUCTION_CB_OPEN_AUCTION, pApdEvent, pAgpdCharacter);
				}
			else
				{
				// move to <stTargetPos>
				pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
				pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_PRODUCT;
				pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
				pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

				m_pAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
				}
			}
			break;
		
		case AGPMAUCTION_OPERATION_EVENT_GRANT :
			{
			ApdEvent *pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if(!pApdEvent)
				return FALSE;
				
			EnumCallback(AGPMAUCTION_CB_OPEN_AUCTION, pApdEvent, pAgpdCharacter);
			}
			break;

		case AGPMAUCTION_OPERATION_LOGIN :
			{
			EnumCallback(AGPMAUCTION_CB_LOGIN, pAgpdCharacter, NULL);
			}
			break;

		case AGPMAUCTION_OPERATION_REQUEST_ALL_SALES :
			{
			
			EnumCallback(AGPMAUCTION_CB_REQUEST_ALL_SALES, pAgpdCharacter, &ulNID);
			}
			break;
			
		case AGPMAUCTION_OPERATION_OPEN_ANYWHERE :
			{
			EnumCallback(AGPMAUCTION_CB_OPEN_ANYWHERE, pAgpdCharacter, &lResult);
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
BOOL AgpmAuction::SetMaxSales(INT32 lCount)
	{
	return m_csAdmin.SetCount(lCount);
	}


AgpdAuctionSales* AgpmAuction::CreateSales()
	{
	AgpdAuctionSales* pAgpdAuctionSales = (AgpdAuctionSales *) CreateModuleData(AGPMAUCTION_DATATYPE_SALES);
	if (pAgpdAuctionSales)
		{
		ZeroMemory(pAgpdAuctionSales, sizeof(AgpdAuctionSales));
		//pAgpdAuctionSales->m_Mutex.Init();
		}
		
	return pAgpdAuctionSales;
	}


void AgpmAuction::DestroySales(AgpdAuctionSales* pAgpdAuctionSales)
	{
	if (pAgpdAuctionSales)
		{
		//pAgpdAuctionSales->m_Mutex.Destroy();
		DestroyModuleData(pAgpdAuctionSales, AGPMAUCTION_DATATYPE_SALES);
		}
	}
	

AgpdAuctionSales* AgpmAuction::GetSales(INT32 lID)
	{
	return m_csAdmin.Get(lID);
	}


BOOL AgpmAuction::AddSales(AgpdAuctionSales *pAgpdAuctionSales)
	{
	return m_csAdmin.Add(pAgpdAuctionSales);
	}


BOOL AgpmAuction::RemoveSales(AgpdAuctionSales *pAgpdAuctionSales)
	{
	return m_csAdmin.Remove(pAgpdAuctionSales);
	}




//	AD
//====================================
//
BOOL AgpmAuction::ConAgpdAuctionCAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmAuction	*pThis		= (AgpmAuction *) pClass;
	ApBase		*pApBase	= (ApBase *) pData;

	AgpdAuctionCAD	*pAgpdAuctionCAD = pThis->GetCAD(pApBase);
	if (pAgpdAuctionCAD)
		{
		pAgpdAuctionCAD->m_ulClock = 0;
		pAgpdAuctionCAD->m_nCount = 0;
		pAgpdAuctionCAD->m_Sales.MemSetAll();
		pAgpdAuctionCAD->m_Mutex.Init();
		}
	else
		return FALSE;

	return TRUE;
	}


BOOL AgpmAuction::DesAgpdAuctionCAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmAuction	*pThis		= (AgpmAuction *) pClass;
	ApBase		*pApBase	= (ApBase *) pData;

	AgpdAuctionCAD	*pAgpdAuctionCAD = pThis->GetCAD(pApBase);
	if (pAgpdAuctionCAD)
		{
		pAgpdAuctionCAD->m_ulClock = 0;
		pAgpdAuctionCAD->m_nCount = 0;
		pAgpdAuctionCAD->m_Sales.MemSetAll();
		pAgpdAuctionCAD->m_Mutex.Destroy();
		}
	else
		return FALSE;

	return TRUE;
	}


AgpdAuctionCAD*	AgpmAuction::GetCAD(ApBase *pApBase)
	{
	if (!pApBase || APBASE_TYPE_CHARACTER != pApBase->m_eType)
		return NULL;

	return (AgpdAuctionCAD *) m_pAgpmCharacter->GetAttachedModuleData(m_nIndexCharacterAD, (PVOID) pApBase);
	}


PVOID AgpmAuction::MakeEventPacket(ApdEvent *pApdEvent)
	{
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	return pvPacketEvent;
	}




//	Sales
//=========================================
//
BOOL AgpmAuction::UpdateSalesStatus(INT32 lSalesID, INT16 nStatus)
	{
	return UpdateSalesStatus(GetSales(lSalesID), nStatus);
	}


BOOL AgpmAuction::UpdateSalesStatus(AgpdAuctionSales *pAgpdAuctionSales, INT16 nStatus)
	{
	if (!pAgpdAuctionSales)
		return FALSE;
	
	//pAgpdAuctionSales->m_Mutex.WLock();
	if (m_Mutex.WLock())
		{
		pAgpdAuctionSales->m_nStatus = nStatus;
		m_Mutex.Release();
		}
	//pAgpdAuctionSales->m_Mutex.Release();
	
	return TRUE;
	}


BOOL AgpmAuction::AddSalesToCAD(AgpdCharacter *pAgpdCharacter, AgpdAuctionSales *pAgpdAuctionSales)
	{
	if (!pAgpdCharacter || !pAgpdAuctionSales)
		return FALSE;
	
	BOOL bResult = FALSE;
	if (m_csAdmin.Add(pAgpdAuctionSales))
		{	
		AgpdAuctionCAD *pAgpdAuctionCAD = GetCAD(pAgpdCharacter);
		if (pAgpdAuctionCAD)
			{
			if (pAgpdAuctionCAD->m_Mutex.WLock())
				{
				bResult = pAgpdAuctionCAD->Add(pAgpdAuctionSales->m_lID);
				pAgpdAuctionCAD->m_Mutex.Release();
				}
			
			if (FALSE == bResult)
				m_csAdmin.Remove(pAgpdAuctionSales);
			
			return bResult;
			}
		}
	
	return bResult;
	}


BOOL AgpmAuction::RemoveSalesFromCAD(AgpdCharacter *pAgpdCharacter, AgpdAuctionSales *pAgpdAuctionSales)
	{
	if (!pAgpdCharacter || !pAgpdAuctionSales)
		return FALSE;
	
	BOOL bResult = FALSE;
	m_csAdmin.Remove(pAgpdAuctionSales);

	AgpdAuctionCAD *pAgpdAuctionCAD = GetCAD(pAgpdCharacter);
	if (pAgpdAuctionCAD)
		{
		if (pAgpdAuctionCAD->m_Mutex.WLock())
			{
			bResult = pAgpdAuctionCAD->Remove(pAgpdAuctionSales->m_lID);
			pAgpdAuctionCAD->m_Mutex.Release();
			}
		}
	
	return bResult;
	}


BOOL AgpmAuction::RemoveSalesFromCAD(AgpdCharacter *pAgpdCharacter, INT32 lSalesID)
	{
	return RemoveSalesFromCAD(pAgpdCharacter, GetSales(lSalesID));
	}


BOOL AgpmAuction::RemoveAllSales(AgpdCharacter *pAgpdCharacter, AuGenerateID *pGenerateID)
	{
	AgpdAuctionCAD *pAgpdAuctionCAD = GetCAD(pAgpdCharacter);
	if (!pAgpdAuctionCAD)
		return FALSE;

	// 2006.06.17. steeple
	// 여기서 계속 죽길래, DestorySales 순서를 변경해봤다.
	INT16 nCount = 0;
	AgpdAuctionSales* ppcsAgpdAuctionSales[AGPMAUCTION_MAX_REGISTER];
	memset(ppcsAgpdAuctionSales, 0, sizeof(AgpdAuctionSales*) * AGPMAUCTION_MAX_REGISTER);
	
	//AuAutoLock Lock(pAgpdAuctionCAD->m_Mutex);
	if (pAgpdAuctionCAD->m_Mutex.WLock())
		{
	
		for (INT16 i=0; i<pAgpdAuctionCAD->GetCount(); ++i)
			{
			INT32 lSalesID = pAgpdAuctionCAD->m_Sales[i];
			AgpdAuctionSales *pAgpdAuctionSales = GetSales(lSalesID);
			if (pAgpdAuctionSales)
				{
				ppcsAgpdAuctionSales[nCount++] = pAgpdAuctionSales;
				RemoveSales(pAgpdAuctionSales);
				if (pGenerateID)
					pGenerateID->AddRemoveID(pAgpdAuctionSales->m_lID);
				//DestroySales(pAgpdAuctionSales);
				}
			}
		
		pAgpdAuctionCAD->m_nCount = 0;
		pAgpdAuctionCAD->m_Sales.MemSetAll();	

		// Lock 을 풀고 Destory 해준다.
		pAgpdAuctionCAD->m_Mutex.Release();
		}

	for (INT16 i=0; i<nCount && i<AGPMAUCTION_MAX_REGISTER; ++i)
		DestroySales(ppcsAgpdAuctionSales[i]);
	
	return TRUE;
	}


AgpdAuctionSales* AgpmAuction::FindSalesFromCAD(AgpdCharacter *pAgpdCharacter, UINT64 ullDocID)
	{
	// get AD
	AgpdAuctionCAD *pAgpdAuctionCAD = GetCAD(pAgpdCharacter);
	if (!pAgpdAuctionCAD)
		return NULL;
		
	AuAutoLock LockSales(pAgpdAuctionCAD->m_Mutex);
	if (!LockSales.Result())
		return NULL;

	for (INT16 i=0; i<pAgpdAuctionCAD->m_nCount; ++i)
		{
		AgpdAuctionSales *pAgpdAuctionSales = GetSales(pAgpdAuctionCAD->m_Sales[i]);
		if (pAgpdAuctionSales)
			{
			if (ullDocID == pAgpdAuctionSales->m_ullDocID)
				{
				return pAgpdAuctionSales;
				}
			}
		}
	
	return NULL;
	}




//	Requirements
//======================================
//
BOOL AgpmAuction::CheckTicket(AgpdCharacter *pAgpdCharacter, BOOL bSub)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;
		
	AgpdGridItem *pAgpdGridItem = NULL;
	AgpdItemADChar *pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	INT32 lIndex = 0;
	
	// hard-coded
	INT32 lID = 984;
	
	for (pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex);
		 pAgpdGridItem;
		 pAgpdGridItem = m_pAgpmGrid->GetItemSequence(&pAgpdItemADChar->m_csInventoryGrid, &lIndex))
		{
		// Check TID
		if (lID == pAgpdGridItem->m_lItemTID)
			{
			AgpdItem* pAgpdItem = m_pAgpmItem->GetItem(pAgpdGridItem);
			if (!pAgpdItem)
				return FALSE;

			// stackable
			if (pAgpdItem->m_pcsItemTemplate->m_bStackable && 1 > pAgpdItem->m_nCount)
				return FALSE;
			
			if (bSub)
				m_pAgpmItem->SubItemStackCount(pAgpdItem, 1);

			return TRUE;
			}
		}

	return FALSE;
	}


BOOL AgpmAuction::IsAbleToOpenAnywhere(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return FALSE;
	
	return FALSE;
	}




//	Event
//======================================
//
BOOL AgpmAuction::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
		
	AgpmAuction		*pThis = (AgpmAuction *) pClass;
	ApdEvent		*pApdEvent = (ApdEvent *) pData;

	pApdEvent->m_pvData = new AgpdEventAuction;
	((AgpdEventAuction *)pApdEvent->m_pvData)->m_bHaveAuctionEvent = FALSE;

	return TRUE;
	}


BOOL AgpmAuction::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
		
	AgpmAuction		*pThis = (AgpmAuction *) pClass;
	ApdEvent		*pApdEvent = (ApdEvent *) pData;
		
	delete pApdEvent->m_pvData;

	return TRUE;
	}


BOOL AgpmAuction::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgpmAuction			*pThis		= (AgpmAuction *) pClass;
	ApdEvent			*pstEvent	= (ApdEvent *) pData;
	AgpdEventAuction	*pstAuction	= (AgpdEventAuction *) pstEvent->m_pvData;
	ApModuleStream		*pStream	= (ApModuleStream *) pCustData;

	//시작~
	if (!pStream->WriteValue(AGPMEVENT_AUCTION_INI_NAME_START, 0))
		return FALSE;

	//템플릿이름 저장.
	if (!pStream->WriteValue(AGPMEVENT_AUCTION_INI_HAVE_EVENT, pstAuction->m_bHaveAuctionEvent))
		return FALSE;

	//끝~
	if (!pStream->WriteValue(AGPMEVENT_AUCTION_INI_NAME_END, 0))
		return FALSE;

	return TRUE;
	}

BOOL AgpmAuction::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	AgpmAuction			*pThis		= (AgpmAuction *) pClass;
	ApdEvent			*pstEvent	= (ApdEvent *) pData;
	AgpdEventAuction	*pstAuction	= (AgpdEventAuction *) pstEvent->m_pvData;
	ApModuleStream		*pStream	= (ApModuleStream *) pCustData;
	const TCHAR			*pszValueName;

	if (!pStream->ReadNextValue())
		return TRUE;

	pszValueName = pStream->GetValueName();
	if (_tcscmp(pszValueName, AGPMEVENT_AUCTION_INI_NAME_START))
		return TRUE;

	while (pStream->ReadNextValue())
		{
		pszValueName = pStream->GetValueName();

		if (!_tcscmp(pszValueName, AGPMEVENT_AUCTION_INI_HAVE_EVENT))
			{
			pStream->GetValue( &pstAuction->m_bHaveAuctionEvent );
			}
		else if (!_tcscmp(pszValueName, AGPMEVENT_AUCTION_INI_NAME_END))
			{
			break;
			}
		}

	return TRUE;
	}




//	Callback setting
//===================================================
//
BOOL AgpmAuction::SetCallbackAddSales(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_ADD_SALES, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackRemoveSales(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_REMOVE_SALES, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackUpdateSales(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_UPDATE_SALES, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_SELECT, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackSelect2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_SELECT2, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackSell(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_SELL, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_CANCEL, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackConfirm(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_CONFIRM, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackBuy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_BUY, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackNotify(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_NOTIFY, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackOpenAuction(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_OPEN_AUCTION, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackLogin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_LOGIN, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackRequestAllSales(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_REQUEST_ALL_SALES, pfCallback, pClass);
	}


BOOL AgpmAuction::SetCallbackOpenAnywhere(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMAUCTION_CB_OPEN_ANYWHERE, pfCallback, pClass);
	}




//
//==============================================
//
BOOL AgpmAuction::ParseRowsetPacket(PVOID pvPacket, stRowset* pstRowset)
	{
	if (!pvPacket || !pstRowset)
		return FALSE;

	CHAR*	pszQuery		= NULL;
	INT16	nQueryLength	= 0;
	PVOID	pHeaders		= NULL;
	INT16	nHeadersLength	= 0;
	//UINT32	ulRows			= 0;
	//UINT32	ulCols			= 0;
	//PVOID		pBuffer			= NULL;
	//UINT32	ulRowBufferSize	= 0;
	//UINT32	ulTotalBufferSize	= 0;
	INT16	nTotalBufferSize	= 0;
	PVOID	pOffsets		= NULL;
	INT16	nOffsetsLength	=0;

	m_csPacketRowset.GetField(FALSE, pvPacket, 0,
							  //&pstRowset->m_eOperation,		// op.
							  &pstRowset->m_lQueryIndex,		// query index
							  &pszQuery,							// query text
							  &nQueryLength,						// query text length
							  &pHeaders,							// headers text
							  &nHeadersLength,						//  headers text length
							  &pstRowset->m_ulRows,			// no. of rows
							  &pstRowset->m_ulCols,			// no. of cols
							  &pstRowset->m_ulRowBufferSize,	// 1 row's buffer size
							  &pstRowset->m_pBuffer,			// buffer
							  &nTotalBufferSize,					// total buffer size
							  &pOffsets,
							  &nOffsetsLength
							  );

	ZeroMemory(pstRowset->m_szQuery, sizeof(pstRowset->m_szQuery));
	_tcsncpy(pstRowset->m_szQuery, pszQuery ? pszQuery : _T(""), MAX_QUERY_LENGTH - 1);
	ASSERT(nHeadersLength < MAX_HEADER_STRING_LENGTH);
	memcpy(pstRowset->m_szHeaders, pHeaders, nHeadersLength);
	pstRowset->m_ulTotalBufferSize = nTotalBufferSize;
	pstRowset->m_lOffsets.MemSetAll();
	pstRowset->m_lOffsets.MemCopy(0, (INT32 *) pOffsets, nOffsetsLength / sizeof(INT32));

	return TRUE;
	}


