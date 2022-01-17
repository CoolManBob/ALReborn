/*===========================================================================

	AgcmUITax.cpp

===========================================================================*/


#include "AgcmUITax.h"
#include "AuTimeStamp.h"

#include "AuRegionNameConvert.h"


/****************************************************/
/*		The Implementation of AgcmUITax class		*/
/****************************************************/
//
TCHAR AgcmUITax::s_szMessage[AGCMUITAX_MESSAGE_MAX][30] = 
	{
	_T("TAX_INTRO"),				// AGCMUITAX_MESSAGE_STEWARD_INTRO
	_T("TAX_MODIFY_CAPTION"),		// AGCMUITAX_MESSAGE_MODIFY_CAPTION
	_T("TAX_VIEW_CAPTION"),			// AGCMUITAX_MESSAGE_VIEW_CAPTION
	_T("TAX_MODIFY_CON_REG"),		// AGCMUITAX_MESSAGE_MODIFY_RATIO_CONFIRM_REG
	_T("TAX_MODIFY_CON_QUE"),		// AGCMUITAX_MESSAGE_MODIFY_RATIO_CONFIRM_QUE
	_T("TAX_MODIFY_SUCCESS"),		// AGCMUITAX_MESSAGE_MODIFY_RATIO_SUCCESS
	_T("TAX_MODIFY_F_NOT_OWNER"),	// AGCMUITAX_MESSAGE_MODIFY_RATIO_FAIL_NOT_OWNER
	_T("TAX_MODIFY_F_NOT_TIME"),	// AGCMUITAX_MESSAGE_MODIFY_RATIO_FAIL_NOT_TIME
	_T("TAX_MODIFY_F_EX_RANGE"),	// AGCMUITAX_MESSAGE_MODIFY_RATIO_EXCEED_RANGE
	};


TCHAR AgcmUITax::s_szEvent[AGCMUITAX_EVENT_MAX][30] =
	{
	_T("TAX_OPEN_STEWARD"),			//	AGCMUITAX_EVENT_OPEN_STEWARD
	_T("TAX_CLOSE_STEWARD"),		//	AGCMUITAX_EVENT_CLOSE_STEWARD
	_T("TAX_OPEN_RATIO"),			//	AGCMUITAX_EVENT_OPEN_RATIO
	_T("TAX_CLOSE_RATIO"),			//	AGCMUITAX_EVENT_CLOSE_RATIO
	_T("TAX_OPEN_INCOME"),			//	AGCMUITAX_EVENT_OPEN_INCOME
	_T("TAX_CLOSE_INCOME")			//	AGCMUITAX_EVENT_CLOSE_INCOME
	};


AgcmUITax::AgcmUITax()
	{
	SetModuleName(_T("AgcmUITax"));
	
	m_bModifyWindow = TRUE;
	m_lEventNPCID = 0;
	}


AgcmUITax::~AgcmUITax()
	{
	}




//	ApModule inherited
//======================================
//
BOOL AgcmUITax::OnAddModule()
	{
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmTax = (AgpmTax *) GetModule(_T("AgpmTax"));
	m_pAgpmSiegeWar = (AgpmSiegeWar*)GetModule(_T("AgpmSiegeWar"));
	m_pAgpmArchlord = (AgpmArchlord*)GetModule(_T("AgpmArchlord"));
	
	m_pAgcmCharacter = (AgcmCharacter *) GetModule(_T("AgcmCharacter"));
	m_pAgcmUIManager2 = (AgcmUIManager2 *) GetModule(_T("AgcmUIManager2"));
	m_pAgcmUIControl = (AgcmUIControl *) GetModule(_T("AgcmUIControl"));
	m_pAgcmUIMain = (AgcmUIMain *) GetModule(_T("AgcmUIMain"));
	m_pAgcmEventManager	= (AgcmEventManager *) GetModule(_T("AgcmEventManager"));
	m_pAgcmChatting2 = (AgcmChatting2 *) GetModule(_T("AgcmChatting2"));
	m_pAgcmUICharacter = (AgcmUICharacter *) GetModule(_T("AgcmUICharacter"));

	if (!m_pAgpmCharacter || !m_pAgpmTax || !m_pAgpmSiegeWar || !m_pAgpmArchlord
		|| !m_pAgcmCharacter || !m_pAgcmUIManager2 || !m_pAgcmUIControl || !m_pAgcmUIMain
		|| !m_pAgcmEventManager || !m_pAgcmChatting2 || !m_pAgcmUICharacter
		)
		return FALSE;	

	// module
	if (FALSE == m_pAgpmTax->SetCallbackModifyRatio(CBResultModifyRatio, this))
		return FALSE;
	if (FALSE == m_pAgpmTax->SetCallbackInquireTax(CBResultInquireTax, this))
		return FALSE;
	if (FALSE == m_pAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseCharacter, this))
		return FALSE;

	// event
	if (FALSE == m_pAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_TAX, CBRequestEvent, this))
		return FALSE;	
	if (FALSE == m_pAgpmTax->SetCallbackEventGrant(CBGrant, this))
		return FALSE;
	if (FALSE == m_pAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	// Main UI callback
	if (FALSE == m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	// UI
	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData())
		return FALSE;
	
	// Boolean callback
	if (!m_pAgcmUIManager2->AddBoolean(this, _T("TAX_CASTLE_OWNER"), CBIsCastleOwner, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUITax::OnInit()
	{
  #ifdef _DEBUG
	AS_REGISTER_TYPE_BEGIN(AgcmUITax, AgcmUITax);
		AS_REGISTER_METHOD0(void, TestOpenSteward);
		AS_REGISTER_METHOD0(void, TestOpenModifyRatio);
		AS_REGISTER_METHOD0(void, TestRequestIncome);
	AS_REGISTER_TYPE_END;
  #endif
  
	return TRUE;
	}




//	OnAddModule helper
//=================================================================
//
BOOL AgcmUITax::AddEvent()
	{
	for (int i=0; i < AGCMUITAX_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}


BOOL AgcmUITax::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, _T("TAX_MODIFY_RATIO"), CBClickModifyRatio, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, _T("TAX_VIEW_INCOME"), CBClickViewIncome, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, _T("TAX_RESET"), CBClickReset, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, _T("TAX_CONFIRM"), CBClickConfirm, 0))
		return FALSE;

	if (!m_pAgcmUIManager2->AddFunction(this, _T("TAX_BACK"), CBClickBack, 0))
		return FALSE;
	
	return TRUE;
	}


BOOL AgcmUITax::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_INTRO"), 0, CBDisplayStewardIntro, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_CAPTION"), 0, CBDisplayCaption, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_REGION1"), 0, CBDisplayRegion1, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_REGION2"), 0, CBDisplayRegion2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_REGION3"), 0, CBDisplayRegion3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_REGION4"), 0, CBDisplayRegion4, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_REGION5"), 0, CBDisplayRegion5, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_MODIFY_RATIO1"), 0, CBDisplayModifyRatio1, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_MODIFY_RATIO2"), 0, CBDisplayModifyRatio2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_MODIFY_RATIO3"), 0, CBDisplayModifyRatio3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_MODIFY_RATIO4"), 0, CBDisplayModifyRatio4, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_VIEW_RATIO1"), 0, CBDisplayViewRatio1, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_VIEW_RATIO2"), 0, CBDisplayViewRatio2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_VIEW_RATIO3"), 0, CBDisplayViewRatio3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_VIEW_RATIO4"), 0, CBDisplayViewRatio4, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_VIEW_RATIO5"), 0, CBDisplayViewRatio5, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_INCOME1"), 0, CBDisplayIncome1, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_INCOME2"), 0, CBDisplayIncome2, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_INCOME3"), 0, CBDisplayIncome3, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_INCOME4"), 0, CBDisplayIncome4, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_INCOME5"), 0, CBDisplayIncome5, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_REC_INCOME"), 0, CBDisplayRecentIncome, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_TOT_INCOME"), 0, CBDisplayTotalIncome, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("TAX_TR_DATE"), 0, CBDisplayLatestTransferDate, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
		
	return TRUE;
	}


BOOL AgcmUITax::AddUserData()
	{
	m_pstSteward =  m_pAgcmUIManager2->AddUserData(_T("TAX_UD_STEWARD"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstSteward)
		return FALSE;
	
	m_pstModifyRatio = m_pAgcmUIManager2->AddUserData(_T("TAX_UD_MODIFY_RATIO"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstModifyRatio)
		return FALSE;

	m_pstViewIncome = m_pAgcmUIManager2->AddUserData(_T("TAX_UD_VIEW_INCOME"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstViewIncome)
		return FALSE;
	
	return TRUE;
	}




//	UI
//===========================================================
//
BOOL AgcmUITax::OpenSteward()
	{
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;

	if (FALSE == m_pAgpmTax->IsCastleOwner(pAgpdCharacter))
		return FALSE;

	// 타겟을 세팅해준다. 2007.08.30. steeple
	m_pAgcmCharacter->SelectTarget(pAgpdCharacter, m_lEventNPCID);

	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_CLOSE_RATIO]);
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_CLOSE_INCOME]);
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_OPEN_STEWARD]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstSteward);
	m_pAgcmUICharacter->RefreshUserDataTargetCharacter();
	
	return TRUE;
	}


BOOL AgcmUITax::OpenModifyRatio()
	{
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_CLOSE_STEWARD]);
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_OPEN_RATIO]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstModifyRatio);
	
	return TRUE;
	}


BOOL AgcmUITax::RequestIncome()
	{
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	TCHAR *pszCastle = m_pAgpmTax->GetCastle(pAgpdCharacter);
	if (NULL == pszCastle || 0 >= _tcslen(pszCastle))
		return FALSE;
	
	return SendViewIncome(pAgpdCharacter, pszCastle);
	}


BOOL AgcmUITax::OpenViewIncome()
	{
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_CLOSE_STEWARD]);
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_OPEN_INCOME]);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstViewIncome);
	
	return TRUE;
	}


void AgcmUITax::CloseAll()
	{
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_CLOSE_STEWARD]);
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_CLOSE_RATIO]);
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUITAX_EVENT_CLOSE_INCOME]);
	}


#ifdef _DEBUG

void AgcmUITax::TestOpenSteward()
	{
	OpenSteward();
	}


void AgcmUITax::TestOpenModifyRatio()
	{
	OpenModifyRatio();
	}


void AgcmUITax::TestRequestIncome()
	{
	RequestIncome();
	}

#endif




//	Tax Data
//============================================
//
AgpdTax* AgcmUITax::GetTax()
	{
	AgpdTax *pAgpdTax = NULL;
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return pAgpdTax;
	
	if (m_pAgpmTax->IsCastleOwner(pAgpdCharacter))
		{
		pAgpdTax = m_pAgpmTax->GetTax(pAgpdCharacter);
		}
	
	return pAgpdTax;
	}


AgpdRegionTax* AgcmUITax::GetRegionTax(INT32 lIndex)
	{
	AgpdTax *pAgpdTax = GetTax();
	if (!pAgpdTax)
		return NULL;
	
	if (0 > lIndex || lIndex >= pAgpdTax->m_lRegionTaxes)
		return NULL;
	
	return pAgpdTax->m_RegionTaxes[lIndex];
	}


BOOL AgcmUITax::UpdateTax(AgpdTax *pAgpdTax)
	{
	if (!pAgpdTax || 0 >= _tcslen(pAgpdTax->m_szCastle))
		return FALSE;
	
	// update AgpdTax & ...
	if (!m_pAgpmTax->UpdateCastleTax(pAgpdTax->m_szCastle, pAgpdTax->m_llTotalIncome,
									 pAgpdTax->m_ulLatestTransferDate, pAgpdTax->m_ulLatestModifyDate)
		)
		return FALSE;									
	
	for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		AgpdRegionTax *pAgpdRegionTax = pAgpdTax->m_RegionTaxes[i];
		if (pAgpdRegionTax)
			m_pAgpmTax->UpdateRegionTax(pAgpdRegionTax->m_szRegionName, pAgpdRegionTax->m_llIncome,
										pAgpdRegionTax->m_lRatio, pAgpdTax->m_szCastle);
		}

	return TRUE;
	}




//	Packet Send
//============================================
//
BOOL AgcmUITax::SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent)
	{
	if (NULL == pAgpdCharacter || NULL == pApdEvent)
		return FALSE;

	INT16 nPacketLength	= 0;
	PVOID pvPacket = m_pAgpmTax->MakePacketEventRequest(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmTax->m_csPacket.FreePacket(pvPacket);

	return	bResult;
	}


BOOL AgcmUITax::SendModifyRatio(AgpdCharacter *pAgpdCharacter, AgpdTax *pAgpdTax)
	{
	if (NULL == pAgpdCharacter)
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
																 NULL,
																 NULL,
																 NULL
																 );
		
		for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
			{
			AgpdRegionTax *pAgpdRegionTax = pAgpdTax->m_RegionTaxes[i];
			if (pAgpdRegionTax)
				pvPacketRT[i] = m_pAgpmTax->m_csPacketRegionTax.MakePacket(FALSE, &nPacketLength, AGPMTAX_PACKET_TYPE,
																		   pAgpdRegionTax->m_szRegionName,
																		   NULL,
																		   &pAgpdRegionTax->m_lRatio
																		   );
			}
		}
	
	PVOID pvPacket = m_pAgpmTax->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMTAX_PACKET_TYPE,
													   &cOperation,
													   &pAgpdCharacter->m_lID,
													   NULL,
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
		
	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmTax->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgcmUITax::SendViewIncome(AgpdCharacter *pAgpdCharacter, TCHAR *pszCastle)
	{
	if (NULL == pAgpdCharacter || NULL == pszCastle)
		return FALSE;

	INT8 cOperation = AGPMTAX_OPERATION_INQUIRE_TAX;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = NULL;
	pvPacketEmb = m_pAgpmTax->m_csPacketCastleTax.MakePacket(FALSE, &nPacketLength, AGPMTAX_PACKET_TYPE,
															 pszCastle,
															 NULL,
															 NULL,
															 NULL
															 );

	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;

	PVOID pvPacket = m_pAgpmTax->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMTAX_PACKET_TYPE,
													   &cOperation,
													   &pAgpdCharacter->m_lID,
													   NULL,
													   pvPacketEmb,
													   NULL,
													   NULL,
													   NULL,
													   NULL
													   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	m_pAgpmTax->m_csPacketCastleTax.FreePacket(pvPacketEmb);

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmTax->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}



//	Module Callback
//============================================
//
BOOL AgcmUITax::CBResultModifyRatio(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdTaxParam *pParam = (AgpdTaxParam *) pCustData;

	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf || pAgpdCharacter != pAgpdCharacterSelf)
		return FALSE;

	TCHAR *pszMessage = NULL;

	switch (pParam->m_lResult)
		{
		case AGPMTAX_RESULT_SUCCESS :
			{
			pszMessage = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_MODIFY_RATIO_SUCCESS);
			// update AgpdTax & ...
			pThis->UpdateTax(pParam->m_pAgpdTax);
			}
			break;
		case AGPMTAX_RESULT_FAIL_NOT_OWNER :
			pszMessage = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_MODIFY_RATIO_FAIL_NOT_OWNER);
			break;
		case AGPMTAX_RESULT_FAIL_NOT_TIME :
			pszMessage = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_MODIFY_RATIO_FAIL_NOT_TIME);
			break;
		case AGPMTAX_RESULT_FAIL_EXCEED_RANGE :
			pszMessage = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_MODIFY_RATIO_EXCEED_RANGE);
			break;
		}
	
	if (pszMessage && _tcslen(pszMessage) > 0)
		pThis->m_pAgcmUIManager2->ActionMessageOKDialog(pszMessage);

	return TRUE;
	}


BOOL AgcmUITax::CBResultInquireTax(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdTaxParam *pParam = (AgpdTaxParam *) pCustData;

	AgpdCharacter *pAgpdCharacterSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacterSelf || pAgpdCharacter != pAgpdCharacterSelf)
		return FALSE;

	switch (pParam->m_lResult)
		{
		case AGPMTAX_RESULT_SUCCESS :
			{
			// update AgpdTax & ...
			if (!pThis->UpdateTax(pParam->m_pAgpdTax))
				return FALSE;
			
			if (pThis->m_bModifyWindow)
				pThis->OpenModifyRatio();
			else
				pThis->OpenViewIncome();
			}
			break;
		
		default :
			break;
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBReleaseCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUITax *pThis = (AgcmUITax *) pClass;

	return TRUE;
	}




//	Event Callback
//============================================
//
BOOL AgcmUITax::CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUITax	*pThis			= (AgcmUITax *)		pClass;
	ApdEvent	*pApdEvent		= (ApdEvent *)		pData;
	ApBase		*pcsGenerator	= (ApBase *)		pCustData;

	if (pcsGenerator->m_eType == APBASE_TYPE_CHARACTER &&
		pcsGenerator->m_lID	== pThis->m_pAgcmCharacter->GetSelfCID())
		{
		if (pThis->m_pAgpmCharacter->IsActionBlockCondition(pThis->m_pAgcmCharacter->m_pcsSelfCharacter))
			return FALSE;

		// 2007.08.30. steeple
		if(pApdEvent->m_pcsSource)
			pThis->m_lEventNPCID = pApdEvent->m_pcsSource->m_lID;

		return pThis->SendRequestEvent(pThis->m_pAgcmCharacter->m_pcsSelfCharacter, pApdEvent);
		}

	return TRUE;	
	}


BOOL AgcmUITax::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pCustData || !pClass)
		return FALSE;

	AgcmUITax		*pThis = (AgcmUITax *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	
	if (pAgpdCharacter != pThis->m_pAgcmCharacter->GetSelfCharacter())
		return FALSE;
	
	// open
	return pThis->OpenSteward();
	}


BOOL AgcmUITax::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUITax		*pThis = (AgcmUITax *) pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pAgpdCharacter->m_stPos, pThis->m_stOpenPos);
	if ((INT32) fDistance < 150)
		return TRUE;

	pThis->CloseAll();

	return TRUE;
	}




//	Main UI Callback
//================================================
//
BOOL AgcmUITax::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUITax *pThis = (AgcmUITax *) pClass;

	// reset
	
	return TRUE;
	}




//	UI Function Callback
//================================================
//
BOOL AgcmUITax::CBClickModifyRatio(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUITax	*pThis = (AgcmUITax *) pClass;
	pThis->m_bModifyWindow = TRUE;
	return pThis->RequestIncome();
	}


BOOL AgcmUITax::CBClickViewIncome(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUITax	*pThis = (AgcmUITax *) pClass;
	pThis->m_bModifyWindow = FALSE;
	return pThis->RequestIncome();
	}


BOOL AgcmUITax::CBClickReset(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1 || !pData2 || !pData3 || !pData4)
		return FALSE;

	AgcmUITax	*pThis = (AgcmUITax *) pClass;
	AgcdUIControl *pControl1 = (AgcdUIControl *) pData1;
	AgcdUIControl *pControl2 = (AgcdUIControl *) pData2;
	AgcdUIControl *pControl3 = (AgcdUIControl *) pData3;
	AgcdUIControl *pControl4 = (AgcdUIControl *) pData4;
	
	if (AcUIBase::TYPE_EDIT != pControl1->m_lType || NULL == pControl1->m_pcsBase
		|| AcUIBase::TYPE_EDIT != pControl2->m_lType || NULL == pControl2->m_pcsBase
		|| AcUIBase::TYPE_EDIT != pControl3->m_lType || NULL == pControl3->m_pcsBase
		|| AcUIBase::TYPE_EDIT != pControl4->m_lType || NULL == pControl4->m_pcsBase
		)
		return FALSE;

	// we have variable number of region tax(2 ~ 4)
	// so... get tax and if not exist set control value to empty string
	// otherwise set control(as number of tax's region tax) value to 0
	AgpdTax *pAgpdTax = pThis->GetTax();
	if (!pAgpdTax)
		{
		((AcUIEdit *)(pControl1->m_pcsBase))->SetText("");
		((AcUIEdit *)(pControl2->m_pcsBase))->SetText("");
		((AcUIEdit *)(pControl3->m_pcsBase))->SetText("");
		((AcUIEdit *)(pControl4->m_pcsBase))->SetText("");
		}
	else
		{
		AcUIEdit * pArray[4];
		pArray[0] = (AcUIEdit *)(pControl1->m_pcsBase);
		pArray[1] = (AcUIEdit *)(pControl2->m_pcsBase);
		pArray[2] = (AcUIEdit *)(pControl3->m_pcsBase);
		pArray[3] = (AcUIEdit *)(pControl4->m_pcsBase);
		
		for (INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
			pArray[i]->SetText("0");
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBClickConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pData1 || !pData2 || !pData3 || !pData4)
		return FALSE;

	AgcmUITax	*pThis = (AgcmUITax *) pClass;
	AgcdUIControl *pControl1 = (AgcdUIControl *) pData1;
	AgcdUIControl *pControl2 = (AgcdUIControl *) pData2;
	AgcdUIControl *pControl3 = (AgcdUIControl *) pData3;
	AgcdUIControl *pControl4 = (AgcdUIControl *) pData4;
	
	if (AcUIBase::TYPE_EDIT != pControl1->m_lType || NULL == pControl1->m_pcsBase
		|| AcUIBase::TYPE_EDIT != pControl2->m_lType || NULL == pControl2->m_pcsBase
		|| AcUIBase::TYPE_EDIT != pControl3->m_lType || NULL == pControl3->m_pcsBase
		|| AcUIBase::TYPE_EDIT != pControl4->m_lType || NULL == pControl4->m_pcsBase
		)
		return FALSE;
	
	TCHAR szConfirm[500] = {0, };
	TCHAR *pszConfirm = szConfirm;

	TCHAR *pszFormatRegion = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_MODIFY_RATIO_CONFIRM_REG);
	TCHAR *pszFormatQuestion = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_MODIFY_RATIO_CONFIRM_QUE);
	if (!pszFormatRegion || !pszFormatQuestion)
		return FALSE;
	
	AgpdTax *pAgpdTax = pThis->GetTax();
	if (!pAgpdTax)
		return FALSE;

	// get ratio
	const TCHAR * pszRatios[4];
	pszRatios[0] = ((AcUIEdit *)(pControl1->m_pcsBase))->GetText();
	pszRatios[1] = ((AcUIEdit *)(pControl2->m_pcsBase))->GetText();
	pszRatios[2] = ((AcUIEdit *)(pControl3->m_pcsBase))->GetText();
	pszRatios[3] = ((AcUIEdit *)(pControl4->m_pcsBase))->GetText();

	INT32 lRatios[4];
	for (INT32 x = 0; x < 4; x++)
		lRatios[x] = pszRatios[x] ? _ttoi(pszRatios[x]) : 0;

	for	(INT32 i = 0; i < pAgpdTax->m_lRegionTaxes; i++)
		{
		if (0 > lRatios[i] ||										// range check
			lRatios[i] > AgpmTax::s_lBaseRatio[AGPMTAX_RATIO_5])
			{
			TCHAR *pszExceed = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_MODIFY_RATIO_EXCEED_RANGE);
			pThis->m_pAgcmUIManager2->ActionMessageOKDialog(pszExceed);
			return TRUE;
			}
			
		std::string regionName = RegionLocalName().GetStr( pAgpdTax->m_RegionTaxes[i]->m_szRegionName );
		if ( regionName == "" ) regionName = pAgpdTax->m_RegionTaxes[i]->m_szRegionName;

		pszConfirm += _stprintf(pszConfirm, pszFormatRegion, regionName.c_str(), lRatios[i]);
		}

	_tcscat(szConfirm, pszFormatQuestion);
	
	if (IDOK != pThis->m_pAgcmUIManager2->ActionMessageOKCancelDialog(szConfirm))
		return FALSE;

	// copy region tax and tax - if server fail, tax and region tax stored in AgpmTax module must not be changed.
	AgpdTax Tax;
	AgpdRegionTax RegionTax[4];
	
	Tax.Reset();
	_tcscpy(Tax.m_szCastle, pAgpdTax->m_szCastle);
	
	for	(INT32 k = 0; k < pAgpdTax->m_lRegionTaxes; k++)
		{
		RegionTax[k].Reset();

		//일본어 윈도우에서 strncpy가 말썽을 부려 memcpy로 수정
		memcpy(RegionTax[k].m_szRegionName,
			   pAgpdTax->m_RegionTaxes[k]->m_szRegionName,
			   sizeof(RegionTax[k].m_szRegionName)-1);
		
		RegionTax[k].m_lRatio = lRatios[k];
		Tax.Add(&RegionTax[k]);
		}

	return pThis->SendModifyRatio(pThis->m_pAgcmCharacter->GetSelfCharacter(), &Tax);
	}


BOOL AgcmUITax::CBClickBack(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUITax	*pThis = (AgcmUITax *) pClass;
	
	return pThis->OpenSteward();
	}




//	Display Callback
//=========================================================
//
//	==========	Steward	==========
//
BOOL AgcmUITax::CBDisplayStewardIntro(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	AgcdUIControl *pControl = (AgcdUIControl *) pcsSourceControl;
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();

	if (AcUIBase::TYPE_EDIT != pControl->m_lType || NULL == pControl->m_pcsBase
		|| NULL == pAgpdCharacter)
		return FALSE;

	TCHAR sz[100];
	TCHAR *psz = pThis->GetMessageTxt(AGCMUITAX_MESSAGE_STEWARD_INTRO);
	TCHAR *pszCastle = pThis->m_pAgpmTax->GetRegion(pAgpdCharacter);

	if (psz && pszCastle)
	{
		std::string point = RegionLocalName().GetStr( pszCastle );
		//sprintf(szDisplay, "%s", point == "" ? (*ppcsPoint)->m_szPointName : point.c_str() );

		if ( AP_SERVICE_AREA_WESTERN == g_eServiceArea )
			strncpy_s( sz, 100, psz, _TRUNCATE );
		else
			_sntprintf_s( sz, 100, _TRUNCATE, psz, point == "" ? pszCastle : point.c_str() );
		((AcUIEdit *)(pControl->m_pcsBase))->SetText(sz);
	}	
	
	return TRUE;
}


//
//	==========	Common	==========
//
BOOL AgcmUITax::CBDisplayCaption(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	TCHAR *pszCastle = pThis->m_pAgpmTax->GetCastle(pAgpdCharacter);
	
	if (pszCastle && _tcslen(pszCastle) > 0)
		{
		TCHAR *psz = pThis->GetMessageTxt(pThis->m_bModifyWindow ? AGCMUITAX_MESSAGE_MODIFY_CAPTION : AGCMUITAX_MESSAGE_VIEW_CAPTION);
		if (psz)
			_stprintf(szDisplay, psz, pszCastle);
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayRegion1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	
	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(0);
	if (pAgpdRegionTax)
		{
		std::string localRegion = RegionLocalName().GetStr( pAgpdRegionTax->m_szRegionName );
		_tcscpy(szDisplay, localRegion == "" ? pAgpdRegionTax->m_szRegionName : localRegion.c_str());
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayRegion2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	
	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(1);
	if (pAgpdRegionTax)
		{
		std::string localRegion = RegionLocalName().GetStr( pAgpdRegionTax->m_szRegionName );
		_tcscpy(szDisplay, localRegion == "" ? pAgpdRegionTax->m_szRegionName : localRegion.c_str());
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayRegion3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		_stprintf(szDisplay, _T(""));
		//AgpdSiegeWar* pcsSiegeWarInfo = pThis->m_pAgpmSiegeWar->GetSiegeWarInfo(AGPMSIEGEWAR_HUMAN_CASTLE_INDEX);
		//if(pcsSiegeWarInfo)
		//{
		//	std::string castle = RegionLocalName().GetStr(pcsSiegeWarInfo->m_strCastleName);
		//	_tcscpy(szDisplay, castle == "" ? pcsSiegeWarInfo->m_strCastleName : castle.c_str());
		//}
	}
	else
	{
		AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(2);
		if (pAgpdRegionTax)
		{
			std::string localRegion = RegionLocalName().GetStr( pAgpdRegionTax->m_szRegionName );
			_tcscpy(szDisplay, localRegion == "" ? pAgpdRegionTax->m_szRegionName : localRegion.c_str());
		}
	}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayRegion4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		_stprintf(szDisplay, _T(""));
		//AgpdSiegeWar* pcsSiegeWarInfo = pThis->m_pAgpmSiegeWar->GetSiegeWarInfo(AGPMSIEGEWAR_ORC_CASTLE_INDEX);
		//if(pcsSiegeWarInfo)
		//{
		//	std::string castle = RegionLocalName().GetStr(pcsSiegeWarInfo->m_strCastleName);
		//	_tcscpy(szDisplay, castle == "" ? pcsSiegeWarInfo->m_strCastleName : castle.c_str());
		//}
	}
	else
	{
		AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(3);
		if (pAgpdRegionTax)
		{
			std::string localRegion = RegionLocalName().GetStr( pAgpdRegionTax->m_szRegionName );
			_tcscpy(szDisplay, localRegion == "" ? pAgpdRegionTax->m_szRegionName : localRegion.c_str());
		}
	}
		
	return TRUE;
	}

BOOL AgcmUITax::CBDisplayRegion5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	AgpdSiegeWar* pcsSiegeWarInfo = NULL;
	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		//pcsSiegeWarInfo = pThis->m_pAgpmSiegeWar->GetSiegeWarInfo(AGPMSIEGEWAR_MOONELF_CASTLE_INDEX);
	}
	else
	{
		pcsSiegeWarInfo = pThis->m_pAgpmSiegeWar->GetArchlordCastle();
	}

	if(!pcsSiegeWarInfo)
	{
		_stprintf(szDisplay, _T(""));
		return FALSE;
	}

	std::string castle = RegionLocalName().GetStr(pcsSiegeWarInfo->m_strCastleName);
	_tcscpy(szDisplay, castle == "" ? pcsSiegeWarInfo->m_strCastleName : castle.c_str());

	return TRUE;
}


//
//	==========	Modify Ratio	==========
//
BOOL AgcmUITax::CBDisplayModifyRatio1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	AgcdUIControl *pControl = (AgcdUIControl *) pcsSourceControl;

	if (AcUIBase::TYPE_EDIT != pControl->m_lType || NULL == pControl->m_pcsBase)
		return FALSE;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(0);
	if (pAgpdRegionTax)
		{
		TCHAR sz[100];
		_stprintf(sz, _T("%d"), pAgpdRegionTax->m_lRatio);
		((AcUIEdit *)(pControl->m_pcsBase))->SetText(sz);
		}	

	return TRUE;
	}


BOOL AgcmUITax::CBDisplayModifyRatio2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	AgcdUIControl *pControl = (AgcdUIControl *) pcsSourceControl;

	if (AcUIBase::TYPE_EDIT != pControl->m_lType || NULL == pControl->m_pcsBase)
		return FALSE;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(1);
	if (pAgpdRegionTax)
		{
		TCHAR sz[100];
		_stprintf(sz, _T("%d"), pAgpdRegionTax->m_lRatio);
		((AcUIEdit *)(pControl->m_pcsBase))->SetText(sz);
		}	

	return TRUE;
	}


BOOL AgcmUITax::CBDisplayModifyRatio3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	AgcdUIControl *pControl = (AgcdUIControl *) pcsSourceControl;

	if (AcUIBase::TYPE_EDIT != pControl->m_lType || NULL == pControl->m_pcsBase)
		return FALSE;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(2);
	if (pAgpdRegionTax)
		{
		TCHAR sz[100];
		_stprintf(sz, _T("%d"), pAgpdRegionTax->m_lRatio);
		((AcUIEdit *)(pControl->m_pcsBase))->SetText(sz);
		}	

	return TRUE;
	}


BOOL AgcmUITax::CBDisplayModifyRatio4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || !pcsSourceControl)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	AgcdUIControl *pControl = (AgcdUIControl *) pcsSourceControl;

	if (AcUIBase::TYPE_EDIT != pControl->m_lType || NULL == pControl->m_pcsBase)
		return FALSE;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(3);
	if (pAgpdRegionTax)
		{
		TCHAR sz[100];
		_stprintf(sz, _T("%d"), pAgpdRegionTax->m_lRatio);
		((AcUIEdit *)(pControl->m_pcsBase))->SetText(sz);
		}	

	return TRUE;
	}


//
//	==========	View Income	==========
//
BOOL AgcmUITax::CBDisplayViewRatio1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(0);
	if (pAgpdRegionTax)
		{
		_stprintf(szDisplay, _T("%d"), pAgpdRegionTax->m_lRatio);
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayViewRatio2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(1);
	if (pAgpdRegionTax)
		{
		_stprintf(szDisplay, _T("%d"), pAgpdRegionTax->m_lRatio);
		}
		
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayViewRatio3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		_stprintf(szDisplay, _T(""));
		//_stprintf(szDisplay, _T("%d"), pThis->m_pAgpmTax->GetTaxRatioArchlordCastle());
	}
	else
	{
		AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(2);
		if (pAgpdRegionTax)
		{
			_stprintf(szDisplay, _T("%d"), pAgpdRegionTax->m_lRatio);
		}
	}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayViewRatio4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		_stprintf(szDisplay, _T(""));
		//_stprintf(szDisplay, _T("%d"), pThis->m_pAgpmTax->GetTaxRatioArchlordCastle());
	}
	else
	{
		AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(3);
		if (pAgpdRegionTax)
		{
			_stprintf(szDisplay, _T("%d"), pAgpdRegionTax->m_lRatio);
		}
	}
	
	return TRUE;
	}

BOOL AgcmUITax::CBDisplayViewRatio5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		_stprintf(szDisplay, _T(""));
		//_stprintf(szDisplay, _T("%d"), pThis->m_pAgpmTax->GetTaxRatioArchlordCastle());
	}
	else
	{
		_stprintf(szDisplay, _T("-%d"), pThis->m_pAgpmTax->GetTaxRatioArchlordCastle());
	}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayIncome1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(0);
	if (pAgpdRegionTax)
		{
		_stprintf(szDisplay, _T("%I64d"), pAgpdRegionTax->m_llIncome);
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayIncome2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(1);
	if (pAgpdRegionTax)
		{
		_stprintf(szDisplay, _T("%I64d"), pAgpdRegionTax->m_llIncome);
		}
		
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayIncome3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		// 2007.09.10. steeple
		// 현재 문제가 있다....
		_stprintf(szDisplay, _T(""));
	}
	else
	{
		AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(2);
		if (pAgpdRegionTax)
		{
			_stprintf(szDisplay, _T("%I64d"), pAgpdRegionTax->m_llIncome);
		}
	}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayIncome4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		// 2007.09.10. steeple
		// 현재 문제가 있다....
		_stprintf(szDisplay, _T(""));
	}
	else
	{
		AgpdRegionTax *pAgpdRegionTax = pThis->GetRegionTax(3);
		if (pAgpdRegionTax)
		{
			_stprintf(szDisplay, _T("%I64d"), pAgpdRegionTax->m_llIncome);
		}
	}
	
	return TRUE;
	}

BOOL AgcmUITax::CBDisplayIncome5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if(pThis->m_pAgpmSiegeWar->IsInArchlordCastle(pcsSelfCharacter))
	{
		// 2007.09.10. steeple
		// 현재 문제가 있다....
		_stprintf(szDisplay, _T(""));
	}
	else
	{
		// 총 합에 세금이 적용된 액수를 보여줘야 한다.
		AgpdTax* pAgpdTax = pThis->GetTax();
		if(pAgpdTax)
		{
			INT64 llRecent = pAgpdTax->GetRecentIncome();
			INT64 llIncome = (INT64)(llRecent * pThis->m_pAgpmTax->GetTaxRatioArchlordCastle() / (double)100);

			if(llIncome == 0)
				_stprintf(szDisplay, _T("0"));
			else
				_stprintf(szDisplay, _T("-%I64d"), llIncome);
		}
	}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayRecentIncome(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdTax *pAgpdTax = pThis->GetTax();
	if (pAgpdTax)
		{
		INT64 llRecent = pAgpdTax->GetRecentIncome();
		_stprintf(szDisplay, _T("%I64d"), llRecent);
		}
	
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayTotalIncome(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;
	
	AgpdTax *pAgpdTax = pThis->GetTax();
	if (pAgpdTax)
		_stprintf(szDisplay, _T("%I64d"), pAgpdTax->m_llTotalIncome);
		
	return TRUE;
	}


BOOL AgcmUITax::CBDisplayLatestTransferDate(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUITax *pThis = (AgcmUITax *) pClass;

	AgpdTax *pAgpdTax = pThis->GetTax();
	if (pAgpdTax)
		{
		TCHAR szDate[35];
		ZeroMemory(szDate, sizeof(szDate));
		AuTimeStamp::ConvertTimeStampToOracleTime(pAgpdTax->m_ulLatestTransferDate, szDate, sizeof(szDate));
		szDate[16] = _T('\0');		// truncate seconds
		_tcscpy(szDisplay, szDate);
		}
	
	return TRUE;
	}




//	Boolean Callback
//================================================
//
BOOL AgcmUITax::CBIsCastleOwner(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	
	AgcmUITax *pThis	= (AgcmUITax *)	pClass;	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;

	return pThis->m_pAgpmTax->IsCastleOwner(pAgpdCharacter);
	}




//	Helper
//================================================
//
TCHAR* AgcmUITax::GetMessageTxt(eAGCMUITAX_MESSAGE eMessage)
	{
	if (0 > eMessage || eMessage > AGCMUITAX_MESSAGE_MAX)
		return NULL;

	return m_pAgcmUIManager2->GetUIMessage(s_szMessage[eMessage]);
	}


void AgcmUITax::AddSystemMessage(TCHAR *pszMessage)
	{
	if (!pszMessage)
		return;

	AgpdChatData	stChatData;
	ZeroMemory(&stChatData, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = pszMessage;

	stChatData.lMessageLength = _tcslen(stChatData.szMessage);
		
	m_pAgcmChatting2->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
	}


