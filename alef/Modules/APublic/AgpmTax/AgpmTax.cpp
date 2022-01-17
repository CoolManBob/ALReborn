/*===========================================================================

	AgpmTax.cpp

===========================================================================*/


#include "AgpmTax.h"
#include "ApMemoryTracker.h"


/************************************************/
/*		The Implementation of AgpmTax class		*/
/************************************************/
//
const INT32 AgpmTax::s_lBaseRatio[AGPMTAX_RATIO_MAX] = 
	{
	0,
	10,
	20,
	30,
	40,
	50
	};

const INT32 AGPMTAX_ARCHLORD_CASTLE_RATIO = 20;


AgpmTax::AgpmTax()
	{
	SetModuleName(_T("AgpmTax"));
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMTAX_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT8,			1,	// operation
							AUTYPE_INT32,			1,	// cid
							AUTYPE_INT32,			1,	// result
							AUTYPE_PACKET,			1,	// embedded packet, castle tax or event
							AUTYPE_PACKET,			1,	// embedded packet, region tax 1
							AUTYPE_PACKET,			1,	// embedded packet, region tax 2
							AUTYPE_PACKET,			1,	// embedded packet, region tax 3
							AUTYPE_PACKET,			1,	// embedded packet, region tax 4
							AUTYPE_END,				0
							);

	m_csPacketCastleTax.SetFlagLength(sizeof(INT16));
	m_csPacketCastleTax.SetFieldType(AUTYPE_CHAR,		AGPMSIEGEWAR_MAX_CASTLE_NAME,	// castle
									 AUTYPE_INT64,		1,	// total income
									 AUTYPE_UINT32,		1,	// latest transfer date
									 AUTYPE_UINT32,		1,	// latest modify date
									 AUTYPE_END,		0
									 );

	m_csPacketRegionTax.SetFlagLength(sizeof(INT16));
	m_csPacketRegionTax.SetFieldType(AUTYPE_CHAR,		32 + 1,		// region
									 AUTYPE_INT64,		1,			// income
									 AUTYPE_INT32,		1,			// ratio
									 AUTYPE_END,		0
									 );

	}


AgpmTax::~AgpmTax()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgpmTax::OnAddModule()
	{
	m_pApmMap = (ApmMap *) GetModule(_T("ApmMap"));
	m_pApmEventManager = (ApmEventManager *) GetModule(_T("ApmEventManager"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmSiegeWar = (AgpmSiegeWar *) GetModule(_T("AgpmSiegeWar"));

	if (!m_pApmMap || !m_pApmEventManager || !m_pAgpmCharacter || !m_pAgpmSiegeWar)
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackActionEventTax(CBEventAction, this))
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackGetTaxRatio(CBGetTaxRatio, this))
		return FALSE;

	if (!m_pApmEventManager->RegisterEvent(APDEVENT_FUNCTION_TAX, NULL, NULL, NULL, NULL, NULL, this))
		return FALSE;

	return TRUE;
	}


BOOL AgpmTax::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8	cOperation = AGPMTAX_OPERATION_NONE;
	INT32	lCID = AP_INVALID_CID;
	INT32	lResult = AGPMTAX_RESULT_NONE;
	PVOID	pvPacketEmb = NULL;
	PVOID	pvPacketRT[AGPDTAX_REGION_MAX];
	
	ZeroMemory(pvPacketRT, sizeof(pvPacketRT));

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lResult,
						&pvPacketEmb,
						&pvPacketRT[0],
						&pvPacketRT[1],
						&pvPacketRT[2],
						&pvPacketRT[3]
						);

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(lCID);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	switch (cOperation)
		{
		case AGPMTAX_OPERATION_EVENT_REQUEST :
			{
			ApdEvent *pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if (!pApdEvent)
				return FALSE;

			if (APDEVENT_FUNCTION_TAX != pApdEvent->m_eFunction)
				return FALSE;
					
			OnOperationEventRequest(pApdEvent, pAgpdCharacter);
			}
			break;

		case AGPMTAX_OPERATION_EVENT_GRANT :
			{
			ApdEvent *pApdEvent = m_pApmEventManager->GetEventFromBasePacket(pvPacketEmb);
			if (!pApdEvent)
				return FALSE;

			if (APDEVENT_FUNCTION_TAX != pApdEvent->m_eFunction)
				return FALSE;
					
			OnOperationEventGrant(pApdEvent, pAgpdCharacter);
			}
			break;
		
		case AGPMTAX_OPERATION_MODIFY_RATIO :
		case AGPMTAX_OPERATION_INQUIRE_TAX:
			{
			AgpdTaxParam Param;
			
			AgpdTax Tax;
			AgpdRegionTax RegionTax[AGPDTAX_REGION_MAX];
			
			Tax.Reset();
			Param.m_bCastle = ParseCastleTax(pvPacketEmb, &Tax);
			
			for (INT32 i = 0; i < AGPDTAX_REGION_MAX; i++)
				{
				RegionTax[i].Reset();
				if (ParseRegionTax(pvPacketRT[i], &RegionTax[i]))
					Tax.Add(&RegionTax[i]);
				}
			
			Param.m_pAgpdTax = &Tax;
			Param.m_lResult = lResult;

			EnumCallback(AGPMTAX_OPERATION_MODIFY_RATIO == cOperation ?
						 AGPMTAX_CB_MODIFY_RATIO : AGPMTAX_CB_INQUIRE_TAX,
						 pAgpdCharacter, &Param);
			}
			break;
		
		case AGPMTAX_OPERATION_NOTIFY_RATIO :
			{
			AgpdRegionTax RegionTax;
			RegionTax.Reset();
			
			if (ParseRegionTax(pvPacketRT[0], &RegionTax))
				{
				UpdateRegionTax(RegionTax.m_szRegionName, RegionTax.m_llIncome, RegionTax.m_lRatio);
				}
			}
		
		default :
			break;
		}

	return TRUE;
	}




//	Operation
//==================================================
//
BOOL AgpmTax::OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	AuPOS stTargetPos;
	ZeroMemory(&stTargetPos, sizeof(stTargetPos));
	
	if (m_pApmEventManager->CheckValidRange(pApdEvent, &pAgpdCharacter->m_stPos, AGPMTAX_MAX_USE_RANGE, &stTargetPos))
		{
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

		if(pAgpdCharacter->m_bMove)
			m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

		EnumCallback(AGPMTAX_CB_EVENT_REQUEST, pAgpdCharacter, pApdEvent);
		}
	else
		{
		// move to target position
		pAgpdCharacter->m_stNextAction.m_bForceAction = FALSE;
		pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_TAX;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType = pApdEvent->m_pcsSource->m_eType;
		pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID = pApdEvent->m_pcsSource->m_lID;

		m_pAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_FORWARD, FALSE, TRUE, FALSE, TRUE, FALSE);
		}

	return TRUE;
	}


BOOL AgpmTax::OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter)
	{
	if (!pApdEvent || !pAgpdCharacter)
		return FALSE;

	EnumCallback(AGPMTAX_CB_EVENT_GRANT, pAgpdCharacter, pApdEvent);

	return TRUE;
	}




//	Tax
//===================================================
//
AgpdTax* AgpmTax::GetTax(TCHAR *pszCastle)
	{
	if (!pszCastle || 0 == _tcslen(pszCastle))
		return FALSE;
	
	AgpdTax *pAgpdTax = NULL;	
	CastleTaxIter Iter = m_CastleTaxMap.find(pszCastle);
	if (Iter != m_CastleTaxMap.end())
		{
		pAgpdTax = Iter->second;
		}
	
	return pAgpdTax;
	}


AgpdTax* AgpmTax::GetTax(INT32 *plIndex)
	{
	if (!plIndex)
		return NULL;
	
	AgpdTax *pAgpdTax = NULL;
	CastleTaxIter Iter = m_CastleTaxMap.begin();
	advance(Iter, *plIndex);
	if (Iter != m_CastleTaxMap.end())
		{
		pAgpdTax = Iter->second;
		(*plIndex) = (*plIndex) + 1;
		}
	
	return pAgpdTax;
	}


AgpdTax* AgpmTax::GetTax(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter)
		return NULL;
	
	AgpdTax *pAgpdTax = NULL;
	AgpdSiegeWar *pAgpdSiegeWar = m_pAgpmSiegeWar->GetSiegeWarInfo(pAgpdCharacter);
	if (pAgpdSiegeWar)
		pAgpdTax = GetTax(pAgpdSiegeWar->m_strCastleName.GetBuffer());
	
	return pAgpdTax;
	}


AgpdRegionTax* AgpmTax::GetRegionTax(TCHAR *pszRegion)
	{
	if (!pszRegion || 0 == _tcslen(pszRegion))
		return NULL;

	AgpdRegionTax *pAgpdRegionTax = NULL;
	RegionTaxIter Iter = m_RegionTaxMap.find(pszRegion);
	if (Iter != m_RegionTaxMap.end())		// already exist
		{
		pAgpdRegionTax = Iter->second;
		}
	
	return pAgpdRegionTax;
	}


BOOL AgpmTax::UpdateCastleTax(TCHAR *pszCastle, INT64 llTotalIncome, UINT32 ulLatestTransferDate, UINT32 ulLatestModifyDate)
	{
	AgpdTax *pAgpdTax = GetTax(pszCastle);
	if (NULL == pAgpdTax)
		{
		pAgpdTax = new AgpdTax;
		pAgpdTax->Init();
		_tcsncpy(pAgpdTax->m_szCastle, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
		m_CastleTaxMap.insert(CastleTaxPair(pszCastle, pAgpdTax));
		}
	
	// modify
	AuAutoLock Lock(pAgpdTax->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	pAgpdTax->m_llTotalIncome = llTotalIncome;
	pAgpdTax->m_ulLatestTransferDate = ulLatestTransferDate;
	pAgpdTax->m_ulLatestModifyDate = ulLatestModifyDate;
	
	return TRUE;
	}


BOOL AgpmTax::UpdateRegionTax(TCHAR *pszRegion, INT64 llIncome, INT32 lRatio, TCHAR *pszCastle)
	{
	AgpdRegionTax *pAgpdRegionTax = GetRegionTax(pszRegion);
	if (NULL == pAgpdRegionTax)
		{
		pAgpdRegionTax = new AgpdRegionTax;
		pAgpdRegionTax->Init();

		///일본어 윈도우에서 strcpy가 말썽을 부려서 memcpy로 수정
		memcpy(pAgpdRegionTax->m_szRegionName, pszRegion, sizeof(pAgpdRegionTax->m_szRegionName)-1);

		m_RegionTaxMap.insert(RegionTaxPair(pszRegion, pAgpdRegionTax));
		}

	if (pszCastle && _tcslen(pszCastle) > 0)
		{
		AgpdTax *pAgpdTax = GetTax(pszCastle);				// add to castle.
		if (!pAgpdTax || !pAgpdTax->Add(pAgpdRegionTax))
			{
			m_RegionTaxMap.erase(pszRegion);
			pAgpdRegionTax->Destroy();
			delete pAgpdRegionTax;
			return FALSE;
			}
		}
	
	// modify
	pAgpdRegionTax->m_Mutex.Lock();
	pAgpdRegionTax->m_llIncome = llIncome;
	pAgpdRegionTax->m_lRatio = lRatio;
	pAgpdRegionTax->m_Mutex.Unlock();

	return TRUE;
	}


INT32 AgpmTax::GetTaxRatio(TCHAR *pszRegion)
	{
	INT32 lRatio = 0;
	
	// find AgpdRegionTax
	RegionTaxIter Iter = m_RegionTaxMap.find(pszRegion);
	if (Iter != m_RegionTaxMap.end())
		{
		AgpdRegionTax *pAgpdRegionTax = Iter->second;
		if (pAgpdRegionTax)
			lRatio = pAgpdRegionTax->m_lRatio;
		}
		
	return lRatio;
	}


INT32 AgpmTax::GetTaxRatio(AgpdCharacter *pAgpdCharacter)
	{
	INT32 lRatio = 0;
	TCHAR *pszRegion = GetRegion(pAgpdCharacter);
	if (pszRegion)
		lRatio = GetTaxRatio(pszRegion);
	
	return lRatio;
	}

// 2007.09.10. steeple
// 아크로드 성의 세율
INT32 AgpmTax::GetTaxRatioArchlordCastle()
{
	return AGPMTAX_ARCHLORD_CASTLE_RATIO;
}


TCHAR* AgpmTax::GetRegion(AgpdCharacter *pAgpdCharacter)
	{
	if (pAgpdCharacter)
		{
		INT16 nBindingRegion = m_pApmMap->GetRegion(pAgpdCharacter->m_stPos.x ,pAgpdCharacter->m_stPos.z);
		ApmMap::RegionTemplate *pRegionTemplate	= m_pApmMap->GetTemplate(nBindingRegion);
		if (pRegionTemplate)
			return pRegionTemplate->pStrName;
		}
	
	return NULL;
	}

INT16 AgpmTax::GetRegionIndex(TCHAR *pszRegion)
	{
	INT16 nIndex = -1;
	if (pszRegion)
		{
		ApmMap::RegionTemplate *pRegionTemplate	= m_pApmMap->GetTemplate(pszRegion);
		if (pRegionTemplate)
			nIndex = pRegionTemplate->nIndex;
		}
		
	return nIndex;
	}


TCHAR* AgpmTax::GetCastle(AgpdCharacter *pAgpdCharacter)
	{
	if (pAgpdCharacter)
		{
		AgpdSiegeWar *pAgpdSiegeWar = m_pAgpmSiegeWar->GetSiegeWarInfo(pAgpdCharacter);
		if (pAgpdSiegeWar)
			return pAgpdSiegeWar->m_strCastleName.GetBuffer();
		}
	
	return NULL;
	}




//	Validation
//===================================================
//
BOOL AgpmTax::IsValidRatio(AgpdTax *pAgpdTax)
	{
	if (NULL == pAgpdTax)
		return FALSE;
	
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		if (pAgpdTax->m_RegionTaxes[i])
			{
			INT32 lRatio = pAgpdTax->m_RegionTaxes[i]->m_lRatio;
			if (s_lBaseRatio[AGPMTAX_RATIO_0] > lRatio
				|| lRatio > s_lBaseRatio[AGPMTAX_RATIO_5])
				return FALSE;
			}
		}

	return TRUE;
	}


BOOL AgpmTax::IsValidDate(UINT32 ulLatestModifyDate)
	{
	if (0 == ulLatestModifyDate)
		return TRUE;

	time_t today, modify;
	tm *pToday, *pModify;
	
	time(&today);
	modify = (time_t) ulLatestModifyDate;
	pToday = localtime(&today);
	if (!pToday)
		return FALSE;
	
	INT32 lTodayYear = pToday->tm_year;
	INT32 lTodayYDay = pToday->tm_yday;
	INT32 lTodayWDay = pToday->tm_wday;	
	
	pModify = localtime(&modify);
	if (!pModify)
		return FALSE;
	
	if (lTodayYear != pModify->tm_year)
		return TRUE;

	// 7일이 지나면 어떤 경우든 새로운 한주이므로 수정 가능하다.
	// 새로 시작되는 월요일에 수정 가능하므로 
	// 오늘 - 월요일부터 지난 날짜가 최종 수정일 보다 크면 수정 가능하다.

	if (lTodayYDay - pModify->tm_yday >= 7)	// 1 week elapsed
		return TRUE;

	UINT32 ulDistanceFromMonday = 0;
	if (0 == lTodayWDay)				// sunday
		ulDistanceFromMonday = 6;
	else
		ulDistanceFromMonday = lTodayWDay - 1;

	if ((INT32)(lTodayYDay - ulDistanceFromMonday) > pModify->tm_yday)
		return TRUE;
	
	return FALSE;
	}


BOOL AgpmTax::IsCastleOwner(AgpdCharacter *pAgpdCharacter, TCHAR *pszCastle)
	{
	if (NULL == pAgpdCharacter)
		return FALSE;
		
	AgpdSiegeWar *pAgpdSiegeWar = m_pAgpmSiegeWar->GetSiegeWarInfo(pAgpdCharacter);
	if (NULL == pAgpdSiegeWar)
		return FALSE;
		
	if (pszCastle && COMPARE_EQUAL != pAgpdSiegeWar->m_strCastleName.Compare(pszCastle))
		return FALSE;

	return m_pAgpmSiegeWar->IsCastleOwner(pAgpdSiegeWar, pAgpdCharacter->m_szID);
	}




//	Packet Parsing
//===================================
//
BOOL AgpmTax::ParseCastleTax(PVOID pvPacket, AgpdTax *pAgpdTax)
	{
	if (!pvPacket || !pAgpdTax)
		return FALSE;
	
	TCHAR *pszCastle = NULL;
	m_csPacketCastleTax.GetField(FALSE, pvPacket, 0,
								 &pszCastle,
								 &pAgpdTax->m_llTotalIncome,
								 &pAgpdTax->m_ulLatestTransferDate,
								 &pAgpdTax->m_ulLatestModifyDate
								 );
	
	if (!pszCastle)
		return FALSE;
	
	_tcsncpy(pAgpdTax->m_szCastle, pszCastle, AGPMSIEGEWAR_MAX_CASTLE_NAME);
	
	return TRUE;
	}


BOOL AgpmTax::ParseRegionTax(PVOID pvPacket, AgpdRegionTax *pAgpdRegionTax)
	{
	if (!pvPacket || !pAgpdRegionTax)
		return FALSE;
	
	TCHAR *pszRegion = NULL;
	m_csPacketRegionTax.GetField(FALSE, pvPacket, 0,
								 &pszRegion,
								 &pAgpdRegionTax->m_llIncome,
								 &pAgpdRegionTax->m_lRatio
								 );
	
	// 일본어 윈도우에서 strncpy가 말썽을 부려서 memcpy로 수정
	if ( pszRegion )
		{
		ZeroMemory(pAgpdRegionTax->m_szRegionName, sizeof(pAgpdRegionTax->m_szRegionName));
		memcpy(pAgpdRegionTax->m_szRegionName, pszRegion, sizeof(pAgpdRegionTax->m_szRegionName)-1);
		}

	return NULL != pszRegion;
	}




//	Event Packet
//===================================
//
PVOID AgpmTax::MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMTAX_OPERATION_EVENT_REQUEST;
	
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMTAX_PACKET_TYPE,
										   &cOperation,			// operation
										   &lCID,				// cid
										   NULL,				// result
										   pvPacketEvent,		// embedded packet, castle tax or event
										   NULL,
										   NULL,
										   NULL,
										   NULL
										   );

	return pvPacket;
	}


PVOID AgpmTax::MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
	{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AGPMTAX_OPERATION_EVENT_GRANT;
	PVOID pvPacketEvent = m_pApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMTAX_PACKET_TYPE,
										   &cOperation,			// operation
										   &lCID,				// cid
										   NULL,				// result
										   pvPacketEvent,		// embedded packet, castle tax or event
										   NULL,
										   NULL,
										   NULL,
										   NULL
										   );

	return pvPacket;
	}




//	Event Callbacks
//======================================================
//
BOOL AgpmTax::CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmTax *pThis = (AgpmTax *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pAgpdCharacter->m_bMove)
		pThis->m_pAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType,
															  pstAction->m_csTargetBase.m_lID,
															  APDEVENT_FUNCTION_TAX
															  );

	pThis->EnumCallback(AGPMTAX_CB_EVENT_REQUEST, pApdEvent, pAgpdCharacter);
	
	return TRUE;
	}




//	Module Callbacks
//===================================================
//
BOOL AgpmTax::CBGetTaxRatio(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgpmTax *pThis = (AgpmTax *) pClass;
	TCHAR	*pszRegion = (TCHAR *) pData;
	INT32	*plRatio = (INT32 *) pCustData;
	
	*plRatio = pThis->GetTaxRatio(pszRegion);
	
	return TRUE;
	}




//	Callback setting
//===================================================
//
BOOL AgpmTax::SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMTAX_CB_EVENT_REQUEST, pfCallback, pClass);
	}


BOOL AgpmTax::SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMTAX_CB_EVENT_GRANT, pfCallback, pClass);
	}


BOOL AgpmTax::SetCallbackModifyRatio(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMTAX_CB_MODIFY_RATIO, pfCallback, pClass);
	}


BOOL AgpmTax::SetCallbackInquireTax(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGPMTAX_CB_INQUIRE_TAX, pfCallback, pClass);
	}


