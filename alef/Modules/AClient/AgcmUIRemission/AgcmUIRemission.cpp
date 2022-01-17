/*===========================================================================

		AgcmUIRemission.cpp

===========================================================================*/


#include "AgcmUIRemission.h"


/********************************************************/
/*		The Implementation of AgcmUIRemission class		*/
/********************************************************/
//
//	Constants
//=================================
//
CHAR AgcmUIRemission::s_szEvent[AGCMUIREMISSION_EVENT_MAX][30] = 
	{
	_T("REMIT_EVENT_OPEN"),
	_T("REMIT_EVENT_CLOSE"),
	_T("REMIT_EVENT_SUCCESS")
	};


CHAR AgcmUIRemission::s_szMessage[AGCMUIREMISSION_MESSAGE_MAX][30] = 
	{
	_T("REMIT_MSG_NONE"),		// Unused
	_T("REMIT_MSG_DESC"),		// AGCMUIREMISSION_MESSAGE_DESCRIPTION = 0,
	_T("REMIT_MSG_CUR_PO"),		// AGCMUIREMISSION_MESSAGE_CURRENT_POINT,
	_T("REMIT_MSG_RM_PO"),		// AGCMUIREMISSION_MESSAGE_REMITABLE_POINT,
	_T("REMIT_MSG_IND_CNT"),	// AGCMUIREMISSION_MESSAGE_INDUL_COUNT,

	_T("REMIT_MSG_NOT_TP"),		// AGCMUIREMISSION_MESSAGE_NOT_TIME_POINT,
	_T("REMIT_MSG_CNF_IND"),	// AGCMUIREMISSION_MESSAGE_CONFIRM_INDUL,
	_T("REMIT_MSG_SUCCESS"),	// AGCMUIREMISSION_MESSAGE_SUCCESS,
	
	_T("FreeMurdererLv1"),		// AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV1,				
	_T("FreeMurdererLv2"),		// AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV2,
	_T("FreeMurdererLv3"),		// AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV3,				
	
	_T("REMIT_MSG_NEED_INDUL")	//AGCMUIREMISSION_MESSAGE_NEED_INDUL_COUNT
	};




//	Con/Des
//======================================
//
AgcmUIRemission::AgcmUIRemission()
	{
	SetModuleName(_T("AgcmUIRemission"));
	
	m_pAgpmRemission = NULL;
	m_pAgpmCharacter = NULL;
	m_pAgcmCharacter = NULL;
	m_pAgcmUIManager2 = NULL;
	m_pAgcmUIControl = NULL;
	m_pAgcmUIMain = NULL;
	m_pAgcmUICooldown = NULL;

	m_cType = AGPMREMISSION_TYPE_NONE;

	m_pstDummy = NULL;
	m_pstIndulEnable = NULL;
	m_lIndumEnable = 0;
	
	m_lCooldownID = AGCMUICOOLDOWN_INVALID_ID;
	m_lOldLevel = 0;
	}


AgcmUIRemission::~AgcmUIRemission()
	{
	}




//	ApModule inherited
//=======================================
//
BOOL AgcmUIRemission::OnAddModule()
	{
	m_pAgpmRemission	= (AgpmRemission *) GetModule(_T("AgpmRemission"));
	m_pAgpmCharacter	= (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	
	m_pAgcmCharacter	= (AgcmCharacter *) GetModule(_T("AgcmCharacter"));
	m_pAgcmUIManager2	= (AgcmUIManager2 *) GetModule(_T("AgcmUIManager2"));
	m_pAgcmUIControl	= (AgcmUIControl *) GetModule(_T("AgcmUIControl"));
	m_pAgcmUIMain		= (AgcmUIMain *) GetModule(_T("AgcmUIMain"));
	m_pAgcmUICooldown	= (AgcmUICooldown *) GetModule("AgcmUICooldown");
	m_pAgcmEventManager	= (AgcmEventManager *) GetModule("AgcmEventManager");

	if (!m_pAgpmRemission || !m_pAgpmCharacter
		|| !m_pAgcmCharacter || !m_pAgcmUIManager2 || !m_pAgcmUIControl || !m_pAgcmUIMain
		|| !m_pAgcmEventManager
		)
		return FALSE;

	// remit result
	if (FALSE == m_pAgpmRemission->SetCallbackRemit(CBRemit, this))
		return FALSE;

	// event
	if (FALSE == m_pAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_REMISSION, CBRequestEvent, this))
		return FALSE;	
	
	if (FALSE == m_pAgpmRemission->SetCallbackGrant(CBGrant, this))
		return FALSE;
		
	if (FALSE == m_pAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	// Main UI callback
	if (FALSE == m_pAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (m_pAgcmUICooldown)
		{
		m_lCooldownID = m_pAgcmUICooldown->RegisterCooldown(CBAfterCooldown, this);
		if (AGCMUICOOLDOWN_INVALID_ID >= m_lCooldownID)
			return FALSE;
		}

	// UI
	if (!AddEvent() || !AddFunction() || !AddDisplay() || !AddUserData() || !AddBoolean())
		return FALSE;

	return TRUE;
	}




//	OnAddModule helper
//=================================================================
//
BOOL AgcmUIRemission::AddEvent()
	{
	for (int i=0; i < AGCMUIREMISSION_EVENT_MAX; ++i)
		{
		m_lEvent[i] = m_pAgcmUIManager2->AddEvent(s_szEvent[i]);
		if (m_lEvent[i] < 0)
			return FALSE;
		}

	return TRUE;	
	}


BOOL AgcmUIRemission::AddFunction()
	{
	if (!m_pAgcmUIManager2->AddFunction(this, _T("REMIT_OPEN"), CBOpenWindow, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("REMIT_CLOSE"), CBCloseWindow, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(this, _T("REMIT_INDUL_BTN_CLICK"), CBClickButtonIndul, 0))
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIRemission::AddDisplay()
	{
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("REMIT_DESC"), 0, CBDisplayDescription, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("REMIT_CURR_POINT"), 0, CBDisplayCurrentPoint, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("REMIT_ABLE_POINT"), 0, CBDisplayRemitablePoint, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;		
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("REMIT_INDUL_COUNT"), 0, CBDisplayIndulCount, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(this, _T("REMIT_NEED_INDUL"), 0, CBDisplayNeedIndulCount, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;	

	return TRUE;
	}


BOOL AgcmUIRemission::AddUserData()
	{
	m_pstDummy = m_pAgcmUIManager2->AddUserData(_T("REMIT_UD_DUMMY"), NULL, 0, 0, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstDummy)
		return FALSE;
	m_pstIndulEnable = m_pAgcmUIManager2->AddUserData(_T("REMIT_UD_INDUL_ENABLE"), &m_lIndumEnable, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (NULL == m_pstIndulEnable)
		return FALSE;

	return TRUE;
	}


BOOL AgcmUIRemission::AddBoolean()
	{
	if (!m_pAgcmUIManager2->AddBoolean(this, _T("REMIT_ACTIVE_INDUL_BTN"), CBIsActiveIndulButton, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
		
	return TRUE;
	}




//	Window
//===========================================================
//
BOOL AgcmUIRemission::Open()
	{
	AgpdCharacter *pAgpdCharacter = m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;
	
	m_stOpenPos	= pAgpdCharacter->m_stPos;	
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIREMISSION_EVENT_OPEN]);
	
	return TRUE;
	}


void AgcmUIRemission::Close()
	{
	m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIREMISSION_EVENT_CLOSE]);
	}




//	Result
//============================================
//
BOOL AgcmUIRemission::OnTest(INT32 lResult)
	{
	if (AGPMREMISSION_RESULT_REMITABLE == lResult)
		{
		// open UI
		Open();
		}
	else
		{
		CHAR *psz = GetMessageTxt(AGCMUIREMISSION_MESSAGE_NOT_TIME_POINT);
		if (psz)
			m_pAgcmUIManager2->ActionMessageOKDialog(psz);
		}
	
	return TRUE;
	}


BOOL AgcmUIRemission::OnResult(AgpdRemitArg *pArg)
	{
	if (NULL == pArg)
		return FALSE;
	
	if (AGPMREMISSION_RESULT_SUCCESS == pArg->m_lResult)
		{
		m_pAgcmUIManager2->ThrowEvent(m_lEvent[AGCMUIREMISSION_EVENT_SUCCESS]);

		// 원래는 AgcmUICharacter에서 뿌려주던 것이나 Message Dialog의 순서 문제와
		// 덮어씌워지는 현상 때문에 여기로 옮긴다.
		INT32 lNewLevel = m_pAgpmCharacter->GetMurdererLevel(m_pAgcmCharacter->GetSelfCharacter());
		CHAR *psz = NULL;
		if (m_lOldLevel > lNewLevel)
			{
			switch (lNewLevel)
				{
				case AGPMCHAR_MURDERER_LEVEL0_POINT	:
					psz = GetMessageTxt(AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV1);
					break;
					
				case AGPMCHAR_MURDERER_LEVEL1_POINT	:
					psz = GetMessageTxt(AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV2);
					break;
								
				case AGPMCHAR_MURDERER_LEVEL2_POINT	:
					psz = GetMessageTxt(AGCMUIREMISSION_MESSAGE_FREE_MURDERER_LV3);
					break;			
				}
			}

		if (psz)
			m_pAgcmUIManager2->ActionMessageOKDialog(psz);

		}

	// refresh
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstDummy);
	m_pAgcmUIManager2->SetUserDataRefresh(m_pstIndulEnable);
	
	return TRUE;
	}




//	Event callback
//============================================
//
BOOL AgcmUIRemission::CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIRemission	*pThis					= (AgcmUIRemission *)		pClass;
	ApdEvent		*pApdEvent				= (ApdEvent *)					pData;
	ApBase			*pcsGenerator			= (ApBase *)					pCustData;


	if (pcsGenerator->m_eType == APBASE_TYPE_CHARACTER &&
		pcsGenerator->m_lID	== pThis->m_pAgcmCharacter->GetSelfCID())
		{
		if (pThis->m_pAgpmCharacter->IsActionBlockCondition(pThis->m_pAgcmCharacter->m_pcsSelfCharacter))
			return FALSE;

		return pThis->SendRequestEvent(pThis->m_pAgcmCharacter->m_pcsSelfCharacter, pApdEvent);
		}

	return TRUE;	
	}
	

BOOL AgcmUIRemission::CBGrant(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pCustData || !pClass)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pCustData;
	
	if (pAgpdCharacter != pThis->m_pAgcmCharacter->GetSelfCharacter())
		return FALSE;
	
	// point check(>= 40), time check (>= 2hours)
	/*
	INT32 lPoint = pThis->m_pAgpmCharacter->GetMurdererPoint(pAgpdCharacter);
	if (AGPMCHAR_MURDERER_LEVEL1_POINT > lPoint)
		{
		CHAR *psz = pThis->GetMessageTxt(AGCMUIREMISSION_MESSAGE_NOT_TIME_POINT);
		if (psz)
			pThis->m_pAgcmUIManager2->ActionMessageOKDialog(psz);
		return FALSE;
		}
	*/

	INT8 cType = AGPMREMISSION_TYPE_TEST;
	pThis->SendRemit(pAgpdCharacter, cType);

	return TRUE;
	}


BOOL AgcmUIRemission::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pAgpdCharacter->m_stPos, pThis->m_stOpenPos);
	if ((INT32) fDistance < 150)
		return TRUE;

	pThis->Close();

	return TRUE;
	}




//	Result callback
//================================================
//
BOOL AgcmUIRemission::CBRemit(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
	
	AgcmUIRemission *pThis = (AgcmUIRemission *) pClass;
	AgpdRemitArg	*pArg = (AgpdRemitArg *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	if (pAgpdCharacter != pThis->m_pAgcmCharacter->GetSelfCharacter())
		return FALSE;
	
	switch (pArg->m_cType)
		{
		case AGPMREMISSION_TYPE_TEST :
			pThis->OnTest(pArg->m_lResult);
			break;
		
		case AGPMREMISSION_TYPE_INDULGENCE :
			pThis->OnResult(pArg);
			break;
		
		case AGPMREMISSION_TYPE_GHELD :
		case AGPMREMISSION_TYPE_SKELETON :
		default :
			break;
		}

	return TRUE;
	}




//	Main UI callback
//================================================
//
BOOL AgcmUIRemission::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmUIRemission *pThis = (AgcmUIRemission *) pClass;
	
	return TRUE;
	}




//	UI Function callback
//===========================================================
//
BOOL AgcmUIRemission::CBOpenWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;
	AgcdUIControl	*pcsControl = (AgcdUIControl *) pData1;

	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstDummy);
	pThis->m_pAgcmUIManager2->SetUserDataRefresh(pThis->m_pstIndulEnable);

	return TRUE;	
	}


BOOL AgcmUIRemission::CBCloseWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *)	pClass;
	
	return TRUE;
	}


BOOL AgcmUIRemission::CBClickButtonIndul(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission*)pClass;

	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;

	CHAR sz[512];
	CHAR *psz = pThis->GetMessageTxt(AGCMUIREMISSION_MESSAGE_CONFIRM_INDUL);
	if (psz)
	{
		if (AP_SERVICE_AREA_WESTERN == g_eServiceArea)
			_stprintf(sz, psz, AGPMREMISSION_REMIT_POINT_PER_TIME, AGPMREMISSION_PAY_INDULGENCE_QTY);
		else
			_stprintf(sz, psz, AGPMREMISSION_PAY_INDULGENCE_QTY, AGPMREMISSION_REMIT_POINT_PER_TIME);

		if (IDOK == pThis->m_pAgcmUIManager2->ActionMessageOKCancelDialog(sz))
		{
			pThis->m_cType = AGPMREMISSION_TYPE_INDULGENCE;
			pThis->m_lOldLevel = pThis->m_pAgpmCharacter->GetMurdererLevel(pAgpdCharacter);

			if (pThis->m_pAgcmUICooldown && AGCMUICOOLDOWN_INVALID_ID != pThis->m_lCooldownID)
				return pThis->m_pAgcmUICooldown->StartCooldown(pThis->m_lCooldownID, 3000, NULL);
			else
				pThis->SendRemit(pAgpdCharacter, pThis->m_cType);
		}
	}

	return TRUE;
}




//	Cooldown callback
//===========================================================
//
BOOL AgcmUIRemission::CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;
		
	AgcmUIRemission *pThis = (AgcmUIRemission *) pClass;
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;

	if (AGPMREMISSION_TYPE_NONE >= pThis->m_cType || pThis->m_cType >= AGPMREMISSION_TYPE_MAX)
		return FALSE;

	return pThis->SendRemit(pAgpdCharacter, pThis->m_cType);
	}




//	Display callback
//============================================================
//
BOOL AgcmUIRemission::CBDisplayDescription(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || AGCDUI_USERDATA_TYPE_INT32 != eType || !pcsSourceControl)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;
	
	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || !pcsSourceControl->m_pcsBase)
		return FALSE;	

	CHAR *psz = pThis->GetMessageTxt(AGCMUIREMISSION_MESSAGE_DESCRIPTION);
	if (psz)
		{
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(psz);
		}

	return TRUE;	
	}


BOOL AgcmUIRemission::CBDisplayCurrentPoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	
	CHAR *psz = pThis->GetMessageTxt(AGCMUIREMISSION_MESSAGE_CURRENT_POINT);
	if (psz)
		{
		INT32 lPoint = pAgpdCharacter ? pThis->m_pAgpmCharacter->GetMurdererPoint(pAgpdCharacter) : 0;
		_stprintf(szDisplay, psz, lPoint);
		}

	return TRUE;
	}


BOOL AgcmUIRemission::CBDisplayRemitablePoint(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;

	CHAR *psz = pThis->GetMessageTxt(AGCMUIREMISSION_MESSAGE_REMITABLE_POINT);
	if (psz)
		_stprintf(szDisplay, psz, AGPMREMISSION_REMIT_POINT_PER_TIME);

	return TRUE;
	}


BOOL AgcmUIRemission::CBDisplayIndulCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();

	CHAR *psz = pThis->GetMessageTxt(AGCMUIREMISSION_MESSAGE_INDUL_COUNT);
	if (psz)
		{
		INT32 lCount = 0;
		if (pAgpdCharacter)
			pThis->m_pAgpmRemission->IsValidItem(pAgpdCharacter, 0, AGPMITEM_TYPE_OTHER, AGPMITEM_OTHER_TYPE_REMISSION, AGPMREMISSION_PAY_INDULGENCE_QTY, &lCount);
		_stprintf(szDisplay, psz, lCount);
		}

	return TRUE;
	}


BOOL AgcmUIRemission::CBDisplayNeedIndulCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIRemission	*pThis = (AgcmUIRemission *) pClass;

	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType || !pcsSourceControl->m_pcsBase)
		return FALSE;	

	CHAR szMessage[200], szCount[100];
	ZeroMemory(szMessage, sizeof(szMessage));
	ZeroMemory(szCount, sizeof(szCount));

	CHAR *pszFormat = pThis->GetMessageTxt(AGCMUIREMISSION_MESSAGE_NEED_INDUL_COUNT);
	sprintf(szCount, "<C16776960>%d<C16777215>", AGPMREMISSION_PAY_INDULGENCE_QTY);
	
	if (pszFormat)
		{
		sprintf(szMessage, pszFormat, szCount);
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pAgcmUIManager2->GetLineDelimiter());
		((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(szMessage);
		}

	return TRUE;
	}




//	Boolean callback
//========================================================
//
BOOL AgcmUIRemission::CBIsActiveIndulButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
	{
	if (!pClass)
		return FALSE;
	
	AgcmUIRemission *pThis	= (AgcmUIRemission *)	pClass;	
	AgpdCharacter *pAgpdCharacter = pThis->m_pAgcmCharacter->GetSelfCharacter();
	if (NULL == pAgpdCharacter)
		return FALSE;

	return pThis->m_pAgpmRemission->IsValidStatus(pAgpdCharacter, AGPMREMISSION_TYPE_INDULGENCE);
	}




//	Packet
//=========================================================
BOOL AgcmUIRemission::SendRemit(AgpdCharacter *pAgpdCharacter, INT8 cType)
	{
	BOOL bResult = FALSE;
	if (NULL == pAgpdCharacter)
		return bResult;
		
	INT16 nPacketLength = 0;
	INT8 cSendType = cType;
	PVOID pvPacketEmb = m_pAgpmRemission->m_csPacketRemit.MakePacket(FALSE, &nPacketLength, NULL,
													&cSendType,			// type
													NULL			// result
													);
	
	if (NULL == pvPacketEmb)
		return bResult;
	
	INT8 cOperation = AGPMREMISSION_OPERATION_REMIT;
	nPacketLength = 0;
	PVOID pvPacket = m_pAgpmRemission->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMREMISSION_PACKET_TYPE,
															&cOperation,
															&pAgpdCharacter->m_lID,
															pvPacketEmb
															);
	m_pAgpmRemission->m_csPacketRemit.FreePacket(pvPacketEmb);
	
	if (pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength);
		m_pAgpmRemission->m_csPacket.FreePacket(pvPacket);
		}
	
	return bResult;
	}


BOOL AgcmUIRemission::SendRequestEvent(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent)
	{
	if (!pApdEvent)
		return FALSE;

	INT16 nPacketLength	= 0;
	PVOID pvPacket = m_pAgpmRemission->MakePacketEventRequest(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);

	m_pAgpmRemission->m_csPacket.FreePacket(pvPacket);

	return	bResult;
	}



//
//	Helper
//=========================================================
//
CHAR* AgcmUIRemission::GetMessageTxt(eAGCMUIREMISSION_MESSAGE eMessage)
	{
	if (0 > eMessage || eMessage > AGCMUIREMISSION_MESSAGE_MAX)
		return NULL;

	return m_pAgcmUIManager2->GetUIMessage(s_szMessage[eMessage]);
	}











