/*===========================================================================

	AgpmWantedCriminal.cpp

===========================================================================*/


#include "AgpmWantedCriminal.h"
#include "ApMemoryTracker.h"


/************************************************************/
/*		The Implementation of AgpmWantedCriminal class		*/
/************************************************************/
//
AgpdWantedCriminalBaseBounty AgpmWantedCriminal::m_BaseBounty[AGPMWANTEDCRIMINAL_BASE_BOUNTY_MAX] =
	{
	{20, 50000},
	{40, 100000},
	{60, 200000},
	{80, 400000},
	{1000000, 1000000}
	};


AgpmWantedCriminal::AgpmWantedCriminal()
	{
	SetModuleName(_T("AgpmWantedCriminal"));
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMWANTEDCRIMINAL_PACKET_TYPE);
	
	SetModuleData(sizeof(AgpdWantedCriminal), AGPMWANTEDCRIMINAL_DATATYPE_WC);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1,	// operation
							AUTYPE_INT32,			1,	// cid
							AUTYPE_INT32,			1,	// page
							AUTYPE_INT32,			1,	// rank
							AUTYPE_INT32,			1,	// result
							AUTYPE_PACKET,			1,	// embedded packet(wanted criminal)
							AUTYPE_END,				0
							);

	m_csPacketWC.SetFlagLength(sizeof(INT16));
	m_csPacketWC.SetFieldType(AUTYPE_CHAR,			AGPDCHARACTER_NAME_LENGTH + 1,		// char id
							  AUTYPE_INT64,			1,									// bounty
							  AUTYPE_INT32,			1,									// level
							  AUTYPE_INT32,			1,									// class
							  AUTYPE_INT32,			1,									// on/offline
							  AUTYPE_END,			0
							  );

	}


AgpmWantedCriminal::~AgpmWantedCriminal()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgpmWantedCriminal::OnAddModule()
	{
	m_pApmEventManager = (ApmEventManager *) GetModule(_T("ApmEventManager"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));

	if (!m_pApmEventManager || !m_pAgpmCharacter)
		return FALSE;

	 m_nIndexCharacterAD = m_pAgpmCharacter->AttachCharacterData(this, sizeof(AgpdWantedCriminalCAD),
																 ConAgpdWantedCriminalCAD,
																 DesAgpdWantedCriminalCAD
																 );
	if (m_nIndexCharacterAD < 0)
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackActionEventWantedCriminal(CBEventAction, this))
		return FALSE;

	if (!m_pApmEventManager->RegisterEvent(APDEVENT_FUNCTION_WANTEDCRIMINAL, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
	}


BOOL AgpmWantedCriminal::OnInit()
	{
	if (!m_csAdmin.InitializeObject(sizeof(AgpdWantedCriminal *), m_csAdmin.GetCount()))
		return FALSE;
	
	return TRUE;
	}


BOOL AgpmWantedCriminal::OnDestroy()
	{
	INT32 lIndex = 0;
	AgpdWantedCriminal **ppAgpdWantedCriminal = NULL;

	for (ppAgpdWantedCriminal = (AgpdWantedCriminal **) m_csAdmin.GetObjectSequence(&lIndex); ppAgpdWantedCriminal; 
		 ppAgpdWantedCriminal = (AgpdWantedCriminal **) m_csAdmin.GetObjectSequence(&lIndex))
		{
		DestroyWantedCriminal(*ppAgpdWantedCriminal);
		}

	m_csAdmin.Reset();
	return TRUE;
	}


BOOL AgpmWantedCriminal::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8	cOperation = AGPMWANTEDCRIMINAL_OPERATION_NONE;
	INT32	lCID = AP_INVALID_CID;
	INT32	lPage = 0;
	INT32	lRank = 0;
	INT32	lResult = AGPMWANTEDCRIMINAL_RESULT_NONE;
	PVOID	pvPacketEmb = NULL;
	
	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lPage,
						&lRank,
						&lResult,
						&pvPacketEmb
						);

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	switch (cOperation)
		{
		case AGPMWANTEDCRIMINAL_OPERATION_EVENT_REQUEST :
			{
			ApdEvent* pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if(!pApdEvent)
				return FALSE;

			if (APDEVENT_FUNCTION_WANTEDCRIMINAL != pApdEvent->m_eFunction)
				return FALSE;
					
			OnOperationEventRequest(pApdEvent, pAgpdCharacter);
			}
			break;

		case AGPMWANTEDCRIMINAL_OPERATION_EVENT_GRANT :
			{
			ApdEvent* pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if(!pApdEvent)
				return FALSE;

			if (APDEVENT_FUNCTION_WANTEDCRIMINAL != pApdEvent->m_eFunction)
				return FALSE;
					
			OnOperationEventGrant(pApdEvent, pAgpdCharacter);
			}
			break;

		case AGPMWANTEDCRIMINAL_OPERATION_ARRANGE :
			{
			AgpdWantedCriminal csWC;
			if (ParsePacketWC(pvPacketEmb, &csWC))
				{
				AgpdWantedCriminalParam Param;
				Param.m_pAgpdWantedCriminal = &csWC;
				Param.m_lResult = lResult;

				EnumCallback(AGPMWANTEDCRIMINAL_CB_ARRANGE, pAgpdCharacter, &Param);
				}
			}
			break;

		case AGPMWANTEDCRIMINAL_OPERATION_REQ_LIST :
			EnumCallback(AGPMWANTEDCRIMINAL_CB_REQ_LIST, pAgpdCharacter, &lPage);
			break;
			
		case AGPMWANTEDCRIMINAL_OPERATION_RES_LIST :
			{
			// WC, page, rank
			AgpdWantedCriminal csWC;
			ParsePacketWC(pvPacketEmb, &csWC);

			AgpdWantedCriminalParam Param;
			Param.m_pAgpdWantedCriminal = &csWC;
			Param.m_lPage = lPage;
			Param.m_lRank = lRank;

			EnumCallback(AGPMWANTEDCRIMINAL_CB_RES_LIST, pAgpdCharacter, &Param);
			}
			break;
		
		case AGPMWANTEDCRIMINAL_OPERATION_MY_RANK :
			{
			AgpdWantedCriminal csWC;
			ParsePacketWC(pvPacketEmb, &csWC);

			AgpdWantedCriminalParam Param;
			Param.m_pAgpdWantedCriminal = &csWC;
			Param.m_lPage = lPage;
			Param.m_lRank = lRank;
			
			EnumCallback(AGPMWANTEDCRIMINAL_CB_MY_RANK, pAgpdCharacter, &Param);
			}
			break;

		case AGPMWANTEDCRIMINAL_OPERATION_UPDATE_KILLER :
			{
			AgpdWantedCriminal csWC;
			if (ParsePacketWC(pvPacketEmb, &csWC))
				EnumCallback(AGPMWANTEDCRIMINAL_CB_UPDATE_KILLER, pAgpdCharacter, csWC.m_szCharID);
			}
			break;

		case AGPMWANTEDCRIMINAL_OPERATION_UPDATE_FLAG :
			{
			BOOL bIsWantedCriminal = (BOOL) lResult;
			EnumCallback(AGPMWANTEDCRIMINAL_CB_UPDATE_FLAG, pAgpdCharacter, &bIsWantedCriminal);
			}
			break;
		
		case AGPMWANTEDCRIMINAL_OPERATION_NOTIFY :
			{
			AgpdWantedCriminal csWC;
			ParsePacketWC(pvPacketEmb, &csWC);
			
			AgpdWantedCriminalParam Param;
			Param.m_pAgpdWantedCriminal = &csWC;
			Param.m_lResult = lResult;
			EnumCallback(AGPMWANTEDCRIMINAL_CB_NOTIFY, pAgpdCharacter, &Param);
			}
			break;

		default :
			break;
		}

	return TRUE;
	}




//	Module Data
//============================
//
AgpdWantedCriminal* AgpmWantedCriminal::CreateWantedCriminal()
	{
	AgpdWantedCriminal *pAgpdWantedCriminal =
				(AgpdWantedCriminal *) CreateModuleData(AGPMWANTEDCRIMINAL_DATATYPE_WC);
	if (pAgpdWantedCriminal)
		{
		pAgpdWantedCriminal->Init();
		}
	
	return pAgpdWantedCriminal;
	}


void AgpmWantedCriminal::DestroyWantedCriminal(AgpdWantedCriminal *pAgpdWantedCriminal)
	{
	if (pAgpdWantedCriminal)
		{
		pAgpdWantedCriminal->Destroy();
		DestroyModuleData(pAgpdWantedCriminal, AGPMWANTEDCRIMINAL_DATATYPE_WC);
		}
	}




//	Map
//============================
//
BOOL AgpmWantedCriminal::SetMaxWantedCriminal(INT32 lCount)
	{
	return m_csAdmin.SetCount(lCount);
	}


BOOL AgpmWantedCriminal::AddWantedCriminal(AgpdWantedCriminal *pAgpdWantedCriminal)
	{
	if (!pAgpdWantedCriminal || !m_csAdmin.AddObject((PVOID) &pAgpdWantedCriminal, pAgpdWantedCriminal->m_szCharID))
		return FALSE;
	
	return TRUE;
	}


BOOL AgpmWantedCriminal::RemoveWantedCriminal(AgpdWantedCriminal *pAgpdWantedCriminal)
	{
	if (!pAgpdWantedCriminal || !m_csAdmin.RemoveObject(pAgpdWantedCriminal->m_szCharID))
		return FALSE;
	
	return TRUE;
	}


AgpdWantedCriminal* AgpmWantedCriminal::GetWantedCriminal(TCHAR *pszCharID)
	{
	AgpdWantedCriminal **ppAgpdWantedCriminal = (AgpdWantedCriminal **) m_csAdmin.GetObject(pszCharID);
	if (ppAgpdWantedCriminal)
		return *ppAgpdWantedCriminal;
	
	return NULL;
	}


AgpdWantedCriminal* AgpmWantedCriminal::GetWantedCriminal(AgpdCharacter *pAgpdCharacter)
	{
	if (NULL == pAgpdCharacter)
		return NULL;
	
	return GetWantedCriminal(pAgpdCharacter->m_szID);
	}




//	CAD
//==================================================
//
BOOL AgpmWantedCriminal::ConAgpdWantedCriminalCAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmWantedCriminal	*pThis		= (AgpmWantedCriminal *) pClass;
	ApBase				*pApBase	= (ApBase *) pData;

	AgpdWantedCriminalCAD *pAgpdWantedCriminalCAD = pThis->GetCAD(pApBase);
	if (pAgpdWantedCriminalCAD)
		{
		pAgpdWantedCriminalCAD->Reset();
		pAgpdWantedCriminalCAD->m_Mutex.Init();
		}
	else
		return FALSE;

	return TRUE;
	}


BOOL AgpmWantedCriminal::DesAgpdWantedCriminalCAD(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgpmWantedCriminal	*pThis		= (AgpmWantedCriminal *) pClass;
	ApBase				*pApBase	= (ApBase *) pData;

	AgpdWantedCriminalCAD *pAgpdWantedCriminalCAD = pThis->GetCAD(pApBase);
	if (pAgpdWantedCriminalCAD)
		{
		pAgpdWantedCriminalCAD->Reset();
		pAgpdWantedCriminalCAD->m_Mutex.Destroy();
		}
	else
		return FALSE;

	return TRUE;
	}


AgpdWantedCriminalCAD* AgpmWantedCriminal::GetCAD(ApBase *pApBase)
	{
	if (!pApBase || APBASE_TYPE_CHARACTER != pApBase->m_eType)
		return NULL;

	return (AgpdWantedCriminalCAD *) m_pAgpmCharacter->GetAttachedModuleData(m_nIndexCharacterAD, (PVOID) pApBase);
	}




//	Operation
//==================================================
//
BOOL AgpmWantedCriminal::OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	AuPOS stTargetPos;
	memset(&stTargetPos, 0, sizeof(stTargetPos));
	
	if (m_pApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, AGPMWANTEDCRIMINAL_MAX_USE_RANGE, &stTargetPos))
		{
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pAgpdCharacter->m_bMove)
			m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

		EnumCallback(AGPMWANTEDCRIMINAL_CB_EVENT_REQUEST, pAgpdCharacter, pApdEvent);
		}
	else
		{
		// move to target position
		pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_WANTEDCRIMINAL;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}


BOOL AgpmWantedCriminal::OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	EnumCallback(AGPMWANTEDCRIMINAL_CB_EVENT_GRANT, pAgpdCharacter, pApdEvent);

	return TRUE;
	}




//	Validation
//===================================================
//
BOOL AgpmWantedCriminal::IsEnoughBaseBounty(AgpdCharacter *pAgpdCharacter)
	{
	return IsEnoughBounty(pAgpdCharacter, GetBaseBounty(pAgpdCharacter));
	}


BOOL AgpmWantedCriminal::IsEnoughBounty(AgpdCharacter *pAgpdCharacter, INT64 llMoney)
	{
	if (NULL == pAgpdCharacter || 0 >= llMoney)
		return FALSE;
	
	INT64 llTotalMoney = pAgpdCharacter->m_llMoney + pAgpdCharacter->m_llBankMoney;
	
	if (llTotalMoney >= llMoney)
		return TRUE;
	
	return FALSE;
	}


TCHAR* AgpmWantedCriminal::GetFinalAttacker(AgpdCharacter *pAgpdCharacter)
	{
	TCHAR *pszKiller = NULL;
	AgpdWantedCriminalCAD *pAgpdWantedCriminalCAD = GetCAD(pAgpdCharacter);
	if (pAgpdWantedCriminalCAD)
		pszKiller = pAgpdWantedCriminalCAD->m_szKiller;
	
	return pszKiller;
	}


void AgpmWantedCriminal::SetFinalAttacker(AgpdCharacter *pAgpdCharacter, TCHAR *pszKiller)
	{
	AgpdWantedCriminalCAD *pAgpdWantedCriminalCAD = GetCAD(pAgpdCharacter);
	if (pAgpdWantedCriminalCAD)
		_tcscpy(pAgpdWantedCriminalCAD->m_szKiller, pszKiller ? pszKiller : _T(""));
	}


BOOL AgpmWantedCriminal::IsFinalAttacker(AgpdCharacter *pAgpdCharacter, TCHAR *pszKiller)
	{
	BOOL bResult = FALSE;
	AgpdWantedCriminalCAD *pAgpdWantedCriminalCAD = GetCAD(pAgpdCharacter);
	if (pAgpdWantedCriminalCAD)
		{
		if (0 == _tcscmp(pAgpdWantedCriminalCAD->m_szKiller, pszKiller))
			bResult = TRUE;
		}
	
	return bResult;
	}


INT64 AgpmWantedCriminal::GetBaseBounty(AgpdCharacter *pAgpdCharacter)
	{
	INT32 lLevel = m_pAgpmCharacter->GetLevel(pAgpdCharacter);
	return GetBaseBounty(lLevel);
	}


INT64 AgpmWantedCriminal::GetBaseBounty(INT32 lLevel)
{
	// 일본의 경우에는 현상금 최저를 1겔드로 한다.
	if ( AP_SERVICE_AREA_JAPAN == g_eServiceArea )
		return 1;

	INT64 llMoney = 0;
	for (INT32 i =0; i < AGPMWANTEDCRIMINAL_BASE_BOUNTY_MAX; i++)
	{
		if (m_BaseBounty[i].m_lMaxLevel >= lLevel)
		{
			llMoney = m_BaseBounty[i].m_llMoney;
			break;
		}
	}

	return llMoney;
}




//	Packet
//===================================================
//
PVOID AgpmWantedCriminal::MakePacketWC(AgpdWantedCriminal *pAgpdWantedCriminal, INT16 *pnPacketLength)
	{
	if (NULL == pAgpdWantedCriminal || NULL == pnPacketLength)
		return NULL;
	
	PVOID pvPacketEmb = m_csPacketWC.MakePacket(FALSE, pnPacketLength, 0,
											pAgpdWantedCriminal->m_szCharID,	// char id
											&pAgpdWantedCriminal->m_llMoney,		// bounty
											&pAgpdWantedCriminal->m_lLevel,		// level
											&pAgpdWantedCriminal->m_lClass,		// class
											&pAgpdWantedCriminal->m_bOnline		// on/offline
											);
	return pvPacketEmb;
	}


BOOL AgpmWantedCriminal::ParsePacketWC(PVOID pvPacketEmb, AgpdWantedCriminal *pAgpdWantedCriminal)
	{
	if (NULL == pvPacketEmb || NULL == pAgpdWantedCriminal)
		return FALSE;

	TCHAR *pszCharID = NULL;
	pAgpdWantedCriminal->Reset();
	
	m_csPacketWC.GetField(FALSE, pvPacketEmb, 0,
						&pszCharID,
						&pAgpdWantedCriminal->m_llMoney,
						&pAgpdWantedCriminal->m_lLevel,
						&pAgpdWantedCriminal->m_lClass,
						&pAgpdWantedCriminal->m_bOnline
						);
	
	if (NULL == pszCharID)
		return FALSE;
	
	_tcsncpy(pAgpdWantedCriminal->m_szCharID, pszCharID, AGPDCHARACTER_NAME_LENGTH);
	
	return TRUE;
	}




//	Event Packet
//===================================
//
PVOID AgpmWantedCriminal::MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_EVENT_REQUEST;
	
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
										   &cOperation,
										   &lCID,
										   NULL,
										   NULL,
										   NULL,
										   pvPacketEvent
										   );

	return pvPacket;
	}


PVOID AgpmWantedCriminal::MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMWANTEDCRIMINAL_OPERATION_EVENT_GRANT;
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMWANTEDCRIMINAL_PACKET_TYPE,
										   &cOperation,
										   &lCID,
										   NULL,
										   NULL,
										   NULL,
										   pvPacketEvent
										   );

	return pvPacket;
	}




//	Event Callbacks
//======================================================
//
BOOL AgpmWantedCriminal::CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmWantedCriminal *pThis = (AgpmWantedCriminal *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pAgpdCharacter->m_bMove)
		pThis->m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType,
															  pstAction->m_csTargetBase.m_lID,
															  APDEVENT_FUNCTION_WANTEDCRIMINAL);

	pThis->EnumCallback(AGPMWANTEDCRIMINAL_CB_EVENT_REQUEST, pApdEvent, pAgpdCharacter);
	
	return TRUE;
	}




//	Callback setting
//===================================================
//
BOOL AgpmWantedCriminal::SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_EVENT_REQUEST, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_EVENT_GRANT, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackArrange(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_ARRANGE, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackRequestList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_REQ_LIST, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackResponseList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_RES_LIST, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackMyRank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_MY_RANK, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackUpdateKiller(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_UPDATE_KILLER, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackUpdateFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_UPDATE_FLAG, pfCallback, pClass);
	}


BOOL AgpmWantedCriminal::SetCallbackNotify(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMWANTEDCRIMINAL_CB_NOTIFY, pfCallback, pClass);
	}



