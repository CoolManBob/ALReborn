/*===========================================================================

	AgsmTax.cpp

===========================================================================*/


#include "AgsmTax.h"
#include "ApMemoryTracker.h"
#include "AuTimeStamp.h"

#include "AuTranslateRegionName.h"


const int AGSMTAX_TRANSFER_WEEK			= 1;	// monday
const int AGSMTAX_TRANSFER_HOUR			= 0;	// 00H

/************************************************/
/*		The Implementation of AgsmTax class		*/
/************************************************/
//
AgsmTax::AgsmTax()
	{
	SetModuleName(_T("AgsmTax"));
	SetModuleType(APMODULE_TYPE_SERVER);
	EnableIdle(TRUE);
	
	m_pAgpmLog = NULL;
	
	m_ulPrevClockCount = 0;
	m_bLoad = FALSE;
	}


AgsmTax::~AgsmTax()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgsmTax::OnAddModule()
	{
	m_pAgpmCharacter		= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmTax				= (AgpmTax *) GetModule(_T("AgpmTax"));
	m_pAgpmSiegeWar			= (AgpmSiegeWar *) GetModule(_T("AgpmSiegeWar"));
	m_pAgpmGuildWarehouse	= (AgpmGuildWarehouse *) GetModule(_T("AgpmGuildWarehouse"));
	m_pAgpmArchlord			= (AgpmArchlord *) GetModule(_T("AgpmArchlord"));
	m_pAgsmCharacter		= (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmSiegeWar			= (AgsmSiegeWar *) GetModule(_T("AgsmSiegeWar"));

	m_pAgpmLog				= (AgpmLog *) GetModule(_T("AgpmLog"));

	if (!m_pAgpmCharacter || !m_pAgpmTax || !m_pAgpmSiegeWar || !m_pAgpmArchlord
		|| !m_pAgsmCharacter || !m_pAgsmSiegeWar)
		return FALSE;

	if (!m_pAgsmSiegeWar->SetCallbackDBSelectCastleEnd(CBLoadCastle, this))
		return FALSE;

	if (!m_pAgsmCharacter->SetCallbackEnterGameworld(CBEnterGame, this))
		return FALSE;
	if(!m_pAgpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
		return FALSE;
	if (!m_pAgpmCharacter->SetCallbackPayTax(CBPayTax, this))
		return FALSE;

	if (!m_pAgpmTax->SetCallbackEventRequest(CBEventRequest, this))
		return FALSE;
	if (!m_pAgpmTax->SetCallbackModifyRatio(CBModifyRatio, this))
		return FALSE;
	if (!m_pAgpmTax->SetCallbackInquireTax(CBInquireTax, this))
		return FALSE;
	
	if (!m_pAgpmSiegeWar->SetCallbackSetNewOwner(CBCastleOwnerChange, this))
		return FALSE;
	
	return TRUE;
	}


BOOL AgsmTax::OnIdle(UINT32 ulClockCount)
	{
	if (m_bLoad &&
		ulClockCount > m_ulPrevClockCount + AGSMTAX_INTERVAL_5MINUTE)
		{
		UpdateDBAll();
		TransferAll();

		m_ulPrevClockCount = ulClockCount;
		}
	
	return TRUE;
	}




//	Operations
//==========================================
//
BOOL AgsmTax::OnRegionChange(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter
		|| !m_pAgpmCharacter->IsPC(pAgpdCharacter)		// monster
		|| 0 == _tcslen(pAgpdCharacter->m_szID))		// guard
		return TRUE;

	// get region tax;
	AgpdRegionTax *pAgpdRegionTax = m_pAgpmTax->GetRegionTax(m_pAgpmTax->GetRegion(pAgpdCharacter));
	if (!pAgpdRegionTax)
		return FALSE;

	AuAutoLock Lock(pAgpdRegionTax->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	// notify to client
	return SendNotifyRegionTax(pAgpdCharacter, pAgpdRegionTax);
	}


BOOL AgsmTax::OnModifyRatio(AgpdCharacter *pAgpdCharacter, AgpdTaxParam *pParam)
	{
	AgpdTax *pAgpdTax = NULL;
	
	if (NULL == pAgpdCharacter || NULL == pParam || NULL == pParam->m_pAgpdTax)
		return FALSE;

	pAgpdTax = m_pAgpmTax->GetTax(pParam->m_pAgpdTax->m_szCastle);
	if (NULL == pAgpdTax)
		return FALSE;

	AuAutoLock Lock(pAgpdTax->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	// owner check
	if (FALSE == m_pAgpmTax->IsCastleOwner(pAgpdCharacter, pAgpdTax->m_szCastle))
		return SendResultModifyTaxRatio(pAgpdCharacter, NULL, AGPMTAX_RESULT_FAIL_NOT_OWNER);
	
	// parameter check
	if (FALSE == m_pAgpmTax->IsValidRatio(pParam->m_pAgpdTax))
		return SendResultModifyTaxRatio(pAgpdCharacter, NULL, AGPMTAX_RESULT_FAIL_EXCEED_RANGE);
	if (pAgpdTax->m_lRegionTaxes != pParam->m_pAgpdTax->m_lRegionTaxes)
		return FALSE;				// invalid parameter so we answer nothing
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		AgpdRegionTax *pDest = pAgpdTax->m_RegionTaxes[i];
		AgpdRegionTax *pSrc = pParam->m_pAgpdTax->m_RegionTaxes[i];
		if (0 != _tcsnicmp(pDest->m_szRegionName, pSrc->m_szRegionName, 32))
			return FALSE;			// invalid parameter so we answer nothing
		}
	
	if (FALSE == m_pAgpmTax->IsValidDate(pAgpdTax->m_ulLatestModifyDate))
		return SendResultModifyTaxRatio(pAgpdCharacter, NULL, AGPMTAX_RESULT_FAIL_NOT_TIME);
	
	// update
	TCHAR szDescription[200]; ZeroMemory(szDescription, sizeof(szDescription));
	TCHAR *pszDescription = szDescription;
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		AgpdRegionTax *pDest = pAgpdTax->m_RegionTaxes[i];
		AgpdRegionTax *pSrc = pParam->m_pAgpdTax->m_RegionTaxes[i];

		pDest->m_Mutex.Lock();
		pDest->m_lRatio = pSrc->m_lRatio;
		pszDescription += _stprintf(pszDescription, _T("%s(I64%d)"), pDest->m_szRegionName, pDest->m_lRatio);
		pDest->m_Mutex.Unlock();
		}

	// modify date
	time_t date;
	time(&date);
	pAgpdTax->m_ulLatestModifyDate = (UINT32) date;
	
	UpdateDB(pAgpdTax);															// Database
	WriteUpdateRatioLog(pAgpdCharacter, pAgpdTax->m_szCastle, szDescription);	// Log
	
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		NotifyRatioAllCharactersInRegion(pAgpdTax->m_RegionTaxes[i]);
		}
	
	return SendResultModifyTaxRatio(pAgpdCharacter, pAgpdTax, AGPMTAX_RESULT_SUCCESS);
	}


BOOL AgsmTax::OnInquireTax(AgpdCharacter *pAgpdCharacter, AgpdTaxParam *pParam)
	{
	if (NULL == pAgpdCharacter || NULL == pParam || NULL == pParam->m_pAgpdTax)
		return FALSE;

	if (!m_pAgpmTax->IsCastleOwner(pAgpdCharacter, pParam->m_pAgpdTax->m_szCastle))
		return SendResultInquireTax(pAgpdCharacter, NULL, AGPMTAX_RESULT_FAIL_NOT_OWNER);
	
	AgpdTax *pAgpdTax = m_pAgpmTax->GetTax(pParam->m_pAgpdTax->m_szCastle);
	if (NULL == pAgpdTax)
		return FALSE;
	
	AuAutoLock Lock(pAgpdTax->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	return SendResultInquireTax(pAgpdCharacter, pAgpdTax, AGPMTAX_RESULT_SUCCESS);
	}


BOOL AgsmTax::OnAddIncome(TCHAR *pszRegion, INT64 llIncome)
	{
	if (NULL == pszRegion || 0 >= llIncome)
		return FALSE;

	// apply income to region income
	AgpdRegionTax *pAgpdRegionTax = m_pAgpmTax->GetRegionTax(pszRegion);
	if (NULL == pAgpdRegionTax)
		return FALSE;
	
	pAgpdRegionTax->m_Mutex.Lock();
	pAgpdRegionTax->m_llIncome += llIncome;
	pAgpdRegionTax->m_Mutex.Unlock();
	
	return TRUE;
	}


BOOL AgsmTax::NotifyRatioAllCharactersInRegion(AgpdRegionTax *pAgpdRegionTax)
	{
	if (!pAgpdRegionTax)
		return FALSE;
	
	AuAutoLock Lock(pAgpdRegionTax->m_Mutex);
	if (!Lock.Result()) return FALSE;

	INT16 nRegionIndex = m_pAgpmTax->GetRegionIndex(pAgpdRegionTax->m_szRegionName);
	if (-1 == nRegionIndex)
		return FALSE;
	
	INT32 lIndex = 0;
	for (AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharSequence(&lIndex);
		 pAgpdCharacter;
		 pAgpdCharacter = m_pAgpmCharacter->GetCharSequence(&lIndex))
		{
		if (m_pAgpmCharacter->IsPC(pAgpdCharacter) &&
			nRegionIndex == pAgpdCharacter->m_nBindingRegionIndex)
			{
			SendNotifyRegionTax(pAgpdCharacter, pAgpdRegionTax);
			}
		}	
	
	return TRUE;
	}


BOOL AgsmTax::TransferAll()
	{
	INT32 lIndex = 0;
	AgpdTax *pAgpdTax = NULL;

	time_t today;
	tm *pToday;

	time(&today);
	pToday = localtime(&today);
	
	if (AGSMTAX_TRANSFER_WEEK != pToday->tm_wday	// if not monday
		|| AGSMTAX_TRANSFER_HOUR != pToday->tm_hour	// if not 00H
		)
		return FALSE;
	
	while (NULL != (pAgpdTax = m_pAgpmTax->GetTax(&lIndex)))
		Transfer(pAgpdTax);
	
	return TRUE;
	}


BOOL AgsmTax::Transfer(AgpdTax *pAgpdTax)
	{
	if (!pAgpdTax)
		return FALSE;
	
	AuAutoLock Lock(pAgpdTax->m_Mutex);
	if (!Lock.Result()) return FALSE;

	time_t today;
	time(&today);
	tm *pToday = localtime(&today);
	if (!pToday)
		return FALSE;
	
	INT32 lToday_WDay = pToday->tm_wday;
	INT32 lToday_Hour = pToday->tm_hour;
	INT32 lToday_Year = pToday->tm_year;
	INT32 lToday_YDay = pToday->tm_yday;
	
	time_t transfer = (time_t) pAgpdTax->m_ulLatestTransferDate;
	tm *pTransfer = localtime(&transfer);	// this call issues pToday's change
	if (!pTransfer)
		return FALSE;
	
	if (AGSMTAX_TRANSFER_WEEK != lToday_WDay	// if not monday
		|| AGSMTAX_TRANSFER_HOUR != lToday_Hour	// if not 00H
		)
		return FALSE;
	
	if (lToday_Year == pTransfer->tm_year
		&&  lToday_YDay == pTransfer->tm_yday)		// already transferred
		return FALSE;
		
	// sum all region's income	
	INT64 llSum = 0;
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		if (pAgpdTax->m_RegionTaxes[i])
			{
			llSum += pAgpdTax->m_RegionTaxes[i]->m_llIncome;
			pAgpdTax->m_RegionTaxes[i]->m_llIncome = 0;			// reset
			}
		}

	// 2007.09.10. steeple
	// 아크로드가 존재하면 아크로드에게 세금 내야 한다.
	if(!m_pAgpmArchlord->m_szArchlord.IsEmpty())
	{
		AgpdSiegeWar* pcsArchlordSiegeWar = m_pAgpmSiegeWar->GetArchlordCastle();
		if(m_pAgpmGuildWarehouse && pcsArchlordSiegeWar)
		{
			INT32 lRatio = m_pAgpmTax->GetTaxRatioArchlordCastle();
			INT64 llArchlordTax = (INT64)(llSum * (double)lRatio / (double)100);
			llSum -= llArchlordTax;

			// 아크로드 길드에다가 넣어준다.
			m_pAgpmGuildWarehouse->AddMoney(pcsArchlordSiegeWar->m_strOwnerGuildName, llArchlordTax);

			AgpdTax* pcsArchlordTax = m_pAgpmTax->GetTax(pcsArchlordSiegeWar->m_strCastleName);
			if(pcsArchlordTax)
			{
				pcsArchlordTax->m_llTotalIncome += llArchlordTax;

				UpdateDB(pcsArchlordTax);
				WriteTransferLog(pcsArchlordTax->m_szCastle, llArchlordTax, pcsArchlordTax->m_llTotalIncome);
			}
		}
	}

	// add to guild
	AgpdSiegeWar *pAgpdSiegeWar = m_pAgpmSiegeWar->GetSiegeWarInfo(pAgpdTax->m_szCastle);
	// 길드로 이체된 금액에 대한 DB 저장은 GuidWarehouse에 위임한다. 난몰라~~ 아~~ 아~~
	if (m_pAgpmGuildWarehouse && pAgpdSiegeWar)
		m_pAgpmGuildWarehouse->AddMoney(pAgpdSiegeWar->m_strOwnerGuildName, llSum);
	
	pAgpdTax->m_llTotalIncome += llSum;
	time_t date;
	time(&date);
	pAgpdTax->m_ulLatestTransferDate = (UINT32) date;

	UpdateDB(pAgpdTax);
	WriteTransferLog(pAgpdTax->m_szCastle, llSum, pAgpdTax->m_llTotalIncome);

	return TRUE;
	}


BOOL AgsmTax::OnCastleOwnerChange(AgpdSiegeWar *pAgpdSiegeWar, AgpdGuild *pAgpdGuildPrev)
	{
	if (!pAgpdSiegeWar)
		return FALSE;
	
	AgpdTax *pAgpdTax = m_pAgpmTax->GetTax(pAgpdSiegeWar->m_strCastleName.GetBuffer());
	if (!pAgpdTax)
		return FALSE;
	
	AuAutoLock Lock(pAgpdTax->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	// sum all region's income	
	INT64 llSum = 0;
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		if (pAgpdTax->m_RegionTaxes[i])
			{
			llSum += pAgpdTax->m_RegionTaxes[i]->m_llIncome;
			pAgpdTax->m_RegionTaxes[i]->m_llIncome = 0;			// reset
			//pAgpdTax->m_RegionTaxes[i]->m_lRatio = 0;			// reset ratio 4 new owner
			}
		}

	// add to previous guild
	// 길드로 이체된 금액에 대한 DB 저장은 GuidWarehouse에 위임한다. 난몰라~~ 아~~ 아~~
	if (m_pAgpmGuildWarehouse && pAgpdGuildPrev)
		m_pAgpmGuildWarehouse->AddMoney(pAgpdGuildPrev->m_szID, llSum);
	
	pAgpdTax->m_llTotalIncome = 0;	// reset total income(for new owner)
	time_t date;
	time(&date);
	pAgpdTax->m_ulLatestTransferDate = (UINT32) date;

	UpdateDB(pAgpdTax);
	WriteTransferLog(pAgpdTax->m_szCastle, llSum, pAgpdTax->m_llTotalIncome);

	return TRUE;	
	}


BOOL AgsmTax::UpdateDBAll()
	{
	INT32 lIndex = 0;
	AgpdTax *pAgpdTax = NULL;
	
	while (NULL != (pAgpdTax = m_pAgpmTax->GetTax(&lIndex)))
		UpdateDB(pAgpdTax);
	
	return TRUE;
	}


BOOL AgsmTax::UpdateDB(AgpdTax *pAgpdTax)
	{
	if (!pAgpdTax)
		return FALSE;
	
	AuAutoLock Lock(pAgpdTax->m_Mutex);
	if (!Lock.Result()) return FALSE;

	// log before update
	WriteBackupLog(pAgpdTax);

	EnumCallback(AGSMTAX_CB_DBUPDATE, pAgpdTax, NULL);

	return TRUE;
	}




//	Callbacks
//======================================
//
BOOL AgsmTax::CBLoadCastle(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;
	
	AgsmTax *pThis = (AgsmTax *) pClass;
	stBuddyRowset *pRowset = (stBuddyRowset *) pData;
	BOOL bEnd = *((BOOL *) pCustData);
	
	if (bEnd)
		{
		pThis->m_bLoad = TRUE;
		pThis->m_ulPrevClockCount = pThis->GetClockCount();
		return TRUE;
		}

	if (NULL == pRowset)
		return FALSE;

	for (UINT32 ulRow = 0; ulRow < pRowset->m_ulRows; ulRow++)
		{
		TCHAR *psz = NULL;
		
		// set AgpdTax
		TCHAR *pszCastle = pRowset->Get(ulRow, 0);			// castle name
		if (NULL == pszCastle || 0 >= _tcslen(pszCastle))
			return FALSE;

		INT64 llTotalIncome = 0;							// total income
		psz = pRowset->Get(ulRow, 2);
		if (psz)	llTotalIncome = _ttoi64(psz);
		
		UINT32 ulLatestTransferDate = 0;					// latest transfer date
		psz = pRowset->Get(ulRow, 3);
		if (psz)	ulLatestTransferDate = AuTimeStamp::ConvertOracleTimeToTimeStamp(psz);
		
		UINT32 ulLatestModifyDate = 0;						// latest modify date
		psz = pRowset->Get(ulRow, 4);
		if (psz)	ulLatestModifyDate = AuTimeStamp::ConvertOracleTimeToTimeStamp(psz);
		
		if (FALSE == pThis->m_pAgpmTax->UpdateCastleTax(pszCastle, llTotalIncome, ulLatestTransferDate, ulLatestModifyDate))
			return FALSE;
		
		// for all region
		for (INT32 i = 0; i < AGPDTAX_REGION_MAX; i++)
			{
			TCHAR *pszRegion = pRowset->Get(ulRow, 5 + i * 3);	// base + i * xx + 0	// region name
			if (NULL == pszRegion || 0 >= _tcslen(pszRegion))
				continue;
			
			INT32 lRatio = 0;					// ratio
			psz = pRowset->Get(ulRow,  6 + i * 3);
			if (psz)	lRatio = _ttoi(psz);

			INT64 llIncome = 0;					// income
			psz = pRowset->Get(ulRow,  7 + i * 3);
			if (psz)	llIncome = _ttoi64(psz);

			std::string regionName;
			if (g_eServiceArea != AP_SERVICE_AREA_KOREA)
				{
				regionName = GetKorName(pszRegion);
				if (regionName == "")
					regionName = pszRegion;

				char tempRegion[128] = {0, 0};
				strcpy(tempRegion, regionName.c_str());

				pThis->m_pAgpmTax->UpdateRegionTax(tempRegion, llIncome, lRatio, pszCastle);
				}
			else
				{
				pThis->m_pAgpmTax->UpdateRegionTax(pszRegion, llIncome, lRatio, pszCastle);
				}
			}
		}

	return TRUE;
	}


BOOL AgsmTax::CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmTax *pThis = (AgsmTax *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;

	if (pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		pThis->OnRegionChange(pAgpdCharacter);

	return TRUE;
	}


BOOL AgsmTax::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmTax *pThis = (AgsmTax *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;

	if (pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		pThis->OnRegionChange(pAgpdCharacter);
	
	return TRUE;
	}


BOOL AgsmTax::CBPayTax(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmTax *pThis = (AgsmTax *) pClass;
	TCHAR *pszRegion = (TCHAR *) pData;
	INT64 llIncome = *((INT64 *) pCustData);

	return pThis->OnAddIncome(pszRegion, llIncome);
	}


BOOL AgsmTax::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmTax *pThis = (AgsmTax *)pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	ApdEvent *pApdEvent = (ApdEvent *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmTax->MakePacketEventGrant(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->_GetCharacterNID(pAgpdCharacter));
	pThis->m_pAgpmTax->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmTax::CBModifyRatio(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmTax *pThis = (AgsmTax *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdTaxParam *pAgpdTaxParam = (AgpdTaxParam *) pCustData;
		
	return pThis->OnModifyRatio(pAgpdCharacter, pAgpdTaxParam);
	}


BOOL AgsmTax::CBInquireTax(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgsmTax *pThis = (AgsmTax *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdTaxParam *pAgpdTaxParam = (AgpdTaxParam *) pCustData;

	return pThis->OnInquireTax(pAgpdCharacter, pAgpdTaxParam);
	}


BOOL AgsmTax::CBCastleOwnerChange(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmTax *pThis = (AgsmTax *) pClass;
	AgpdSiegeWar *pAgpdSiegeWar = (AgpdSiegeWar *) pData;
	AgpdGuild *pAgpdGuildPrev = (AgpdGuild *) pCustData;	
	
	return pThis->OnCastleOwnerChange(pAgpdSiegeWar, pAgpdGuildPrev);
	}




//	Packet Send
//===================================================
//
BOOL AgsmTax::SendNotifyRegionTax(AgpdCharacter *pAgpdCharacter, AgpdRegionTax *pAgpdRegionTax)
	{
	if (!pAgpdCharacter || !pAgpdRegionTax)
		return FALSE;

	INT8 cOperation = AGPMTAX_OPERATION_NOTIFY_RATIO;
	INT16 nPacketLength = 0;
	PVOID pvPacketRT = NULL;
	pvPacketRT = m_pAgpmTax->m_csPacketRegionTax.MakePacket(FALSE, &nPacketLength, AGPMTAX_PACKET_TYPE,
															pAgpdRegionTax->m_szRegionName,
															NULL,
															&pAgpdRegionTax->m_lRatio
															);

	if (!pvPacketRT || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmTax->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMTAX_PACKET_TYPE,
													   &cOperation,
													   &pAgpdCharacter->m_lID,
													   NULL,
													   NULL,
													   pvPacketRT,
													   NULL,
													   NULL,
													   NULL
													   );

	if (pvPacketRT)
		m_pAgpmTax->m_csPacketRegionTax.FreePacket(pvPacketRT);
		
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmTax->m_csPacket.FreePacket(pvPacket);

	return bResult;	
	}


BOOL AgsmTax::SendResultModifyTaxRatio(AgpdCharacter *pAgpdCharacter, AgpdTax *pAgpdTax, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMTAX_OPERATION_MODIFY_RATIO;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = NULL;
	PVOID pvPacketRT[AGPDTAX_REGION_MAX];
	ZeroMemory(pvPacketRT, sizeof(pvPacketRT));
	if (pAgpdTax)
		{
		pvPacketEmb = m_pAgpmTax->m_csPacketCastleTax.MakePacket(FALSE, &nPacketLength, AGPMTAX_PACKET_TYPE,
																 pAgpdTax->m_szCastle,
																 &pAgpdTax->m_llTotalIncome,
																 &pAgpdTax->m_ulLatestTransferDate,
																 &pAgpdTax->m_ulLatestModifyDate
																 );
		
		for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
			{
			AgpdRegionTax *pAgpdRegionTax = pAgpdTax->m_RegionTaxes[i];
			if (pAgpdRegionTax)
				pvPacketRT[i] = m_pAgpmTax->m_csPacketRegionTax.MakePacket(FALSE, &nPacketLength, AGPMTAX_PACKET_TYPE,
																		   pAgpdRegionTax->m_szRegionName,
																		   &pAgpdRegionTax->m_llIncome,
																		   &pAgpdRegionTax->m_lRatio
																		   );
			}
		}
	
	PVOID pvPacket = m_pAgpmTax->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMTAX_PACKET_TYPE,
													   &cOperation,
													   &pAgpdCharacter->m_lID,
													   &lResult,
													   pvPacketEmb,
													   pvPacketRT[0],
													   pvPacketRT[1],
													   pvPacketRT[2],
													   pvPacketRT[3]
													   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	if (pvPacketEmb)
		m_pAgpmTax->m_csPacketCastleTax.FreePacket(pvPacketEmb);

	for (INT32 j = 0; j < AGPDTAX_REGION_MAX; j++)
		{
		if (pvPacketRT[j])
			m_pAgpmTax->m_csPacketRegionTax.FreePacket(pvPacketRT[j]);
		}
		
	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmTax->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmTax::SendResultInquireTax(AgpdCharacter *pAgpdCharacter, AgpdTax *pAgpdTax, INT32 lResult)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMTAX_OPERATION_INQUIRE_TAX;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = NULL;
	PVOID pvPacketRT[AGPDTAX_REGION_MAX];
	ZeroMemory(pvPacketRT, sizeof(pvPacketRT));
	if (pAgpdTax)
		{
		pvPacketEmb = m_pAgpmTax->m_csPacketCastleTax.MakePacket(FALSE, &nPacketLength, AGPMTAX_PACKET_TYPE,
																 pAgpdTax->m_szCastle,
																 &pAgpdTax->m_llTotalIncome,
																 &pAgpdTax->m_ulLatestTransferDate,
																 &pAgpdTax->m_ulLatestModifyDate
																 );
		
		for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
			{
			AgpdRegionTax *pAgpdRegionTax = pAgpdTax->m_RegionTaxes[i];
			if (pAgpdRegionTax)
				pvPacketRT[i] = m_pAgpmTax->m_csPacketRegionTax.MakePacket(FALSE, &nPacketLength, AGPMTAX_PACKET_TYPE,
																		   pAgpdRegionTax->m_szRegionName,
																		   &pAgpdRegionTax->m_llIncome,
																		   &pAgpdRegionTax->m_lRatio
																		   );
			}
		}
	
	PVOID pvPacket = m_pAgpmTax->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMTAX_PACKET_TYPE,
													   &cOperation,
													   &pAgpdCharacter->m_lID,
													   &lResult,
													   pvPacketEmb,
													   pvPacketRT[0],
													   pvPacketRT[1],
													   pvPacketRT[2],
													   pvPacketRT[3]
													   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	if (pvPacketEmb)
		m_pAgpmTax->m_csPacketCastleTax.FreePacket(pvPacketEmb);

	for (INT32 j = 0; j < AGPDTAX_REGION_MAX; j++)
		{
		if (pvPacketRT[j])
			m_pAgpmTax->m_csPacketRegionTax.FreePacket(pvPacketRT[j]);
		}
		
	BOOL bResult = SendPacket(pvPacket, nPacketLength, _GetCharacterNID(pAgpdCharacter));
	m_pAgpmTax->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}




//	Callback Setting
//===============================================
//
BOOL AgsmTax::SetCallbackDBUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMTAX_CB_DBUPDATE, pfCallback, pClass);
	}




//	Helper
//===============================================
//
UINT32 AgsmTax::_GetCharacterNID(INT32 lCID)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(lCID);
	}


UINT32 AgsmTax::_GetCharacterNID(AgpdCharacter *pAgpdCharacter)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter);
	}




//	Log
//===================================================
//
BOOL AgsmTax::WriteBackupLog(AgpdTax *pAgpdTax)
	{
	if (NULL == m_pAgpmLog || NULL == pAgpdTax)
		return FALSE;

	TCHAR szCastle2[20];
	ZeroMemory(szCastle2, sizeof(szCastle2));
	_stprintf(szCastle2, _T("#@%s"), pAgpdTax->m_szCastle);

	TCHAR szDescription[200]; ZeroMemory(szDescription, sizeof(szDescription));
	TCHAR *pszDescription = szDescription;
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		AgpdRegionTax *pRegionTax = pAgpdTax->m_RegionTaxes[i];
		TCHAR szRegion[3]; ZeroMemory(szRegion, sizeof(szRegion));
		strncpy(szRegion, pRegionTax->m_szRegionName, 2);
		pszDescription += _stprintf(pszDescription, _T("%s(%I64d)"), szRegion, pRegionTax->m_llIncome);
		}
	
	return m_pAgpmLog->WriteLog_ETC(AGPDLOGTYPE_ETC_TAX_BACKUP,
									0,
									NULL,	// ip
									NULL,	// account
									NULL,	// server name
									szCastle2,	// char id
									0,	// char tid
									0,	// char level
									0,	// char exp
									0,	// inven money
									pAgpdTax->m_llTotalIncome,	// bank money
									0,	// num id
									pAgpdTax->m_szCastle,	// str id
									szDescription,	// desc
									0			//
									);
	}


BOOL AgsmTax::WriteTransferLog(TCHAR *pszCastle, INT64 llSumOfWeek, INT64 llTotal)
	{
	if (NULL == m_pAgpmLog)
		return FALSE;

	TCHAR szCastle2[20];
	ZeroMemory(szCastle2, sizeof(szCastle2));
	_stprintf(szCastle2, _T("#@%s"), pszCastle);
	
	TCHAR szValue[30];
	ZeroMemory(szValue, sizeof(szValue));
	_stprintf(szValue, _T("Week(%I64d) Total(%I64d)"), llSumOfWeek, llTotal);
	
	return m_pAgpmLog->WriteLog_ETC(AGPDLOGTYPE_ETC_TAX_TRANSFER,
									0,
									NULL,	// ip
									NULL,	// account
									NULL,	// server name
									szCastle2,	// char id
									0,	// char tid
									0,	// char level
									0,	// char exp
									0,	// inven money
									0,	// bank money
									0,	// num id
									pszCastle,	// str id
									szValue,	// desc
									0			//
									);
	}


BOOL AgsmTax::WriteUpdateRatioLog(AgpdCharacter *pAgpdCharacter, TCHAR *pszCastle, TCHAR *pszDesc)
	{
	if (NULL == m_pAgpmLog || NULL == pAgpdCharacter)
		return FALSE;
	
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	return m_pAgpmLog->WriteLog_ETC(AGPDLOGTYPE_ETC_TAX_UPDATE,
									0,
									&pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
									m_pAgpmCharacter->GetLevel(pAgpdCharacter),
									m_pAgpmCharacter->GetExp(pAgpdCharacter),
									pAgpdCharacter->m_llMoney,
									pAgpdCharacter->m_llBankMoney,
									0,
									pszCastle,
									pszDesc,
									0
									);
	}