#include "AgcmUICustomizing.h"
#include "AuStrTable.h"

AgcmUICustomizing::AgcmUICustomizing()
{
	SetModuleName("AgcmUICustomizing");

	EnableIdle(TRUE);

	m_pcsEditFace		= NULL;
	m_pcsEditHair		= NULL;
	m_pcsEditSummary	= NULL;

	m_pcsUserDataFaceList	= NULL;
	m_pcsUserDataHairList	= NULL;

	m_pcsUserDataPriceMoney	= NULL;

	for (int i = 0; i < CHARCUSTOMIZE_MAX_LIST; ++i)
	{
		m_alFace[i]	= i;
		m_alHair[i]	= i;
	}

	m_bIsActiveBuy		= TRUE;

	m_lTotalPriceMoney	= 0;
	m_lTotalPriceSkull	= 0;

	m_lAddTextureID		= (-1);

	m_bIsOpenedUI		= FALSE;

	ZeroMemory(&m_stOpenedPosition, sizeof(m_stOpenedPosition));

	m_pcsEvent			= NULL;

	m_lPrevFaceIndex	= 0;
	m_lPrevHairIndex	= 0;	
}

AgcmUICustomizing::~AgcmUICustomizing()
{
}

BOOL AgcmUICustomizing::OnAddModule()
{
	m_pcsAgpmGrid				= (AgpmGrid *)					GetModule("AgpmGrid");
	m_pcsAgpmCharacter			= (AgpmCharacter *)				GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)					GetModule("AgpmItem");
	m_pcsAgpmEventCharCustomize	= (AgpmEventCharCustomize *)	GetModule("AgpmEventCharCustomize");

	m_pcsAgcmCharacter			= (AgcmCharacter *)				GetModule("AgcmCharacter");
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)			GetModule("AgcmUIManager2");
	m_pcsAgcmCustomizeRender	= (AgcmCustomizeRender *)		GetModule("AgcmCustomizeRender");
	m_pcsAgcmEventCharCustomize	= (AgcmEventCharCustomize *)	GetModule("AgcmEventCharCustomize");
	m_pcsAgcmChatting2			= (AgcmChatting2 *)				GetModule("AgcmChatting2");
	m_pcsAgcmUIChatting2		= (AgcmUIChatting2 *)			GetModule("AgcmUIChatting2");
	m_pcsAgcmUIMain				= (AgcmUIMain *)				GetModule("AgcmUIMain");

	if (!m_pcsAgpmGrid ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmEventCharCustomize ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmCustomizeRender ||
		!m_pcsAgcmEventCharCustomize ||
		!m_pcsAgcmChatting2 ||
		!m_pcsAgcmUIChatting2 ||
		!m_pcsAgcmUIMain)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateCustomize(CBSelfUpdateCustomize, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!m_pcsAgcmEventCharCustomize->SetCallbackGrantEvent(CBEventGrant, this))
		return FALSE;

	if (!m_pcsAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (!AddEvent() ||
		!AddFunction() ||
		!AddUserData() ||
		!AddDisplay())
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddCustomControl("CustomizePreview", &m_csCustomizePreview))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICustomizing::OnInit()
{	
	return TRUE;
}

BOOL AgcmUICustomizing::OnIdle(UINT32 ulClockCount)
{
	if (m_pcsAgcmCustomizeRender->IsStart())
	{
		m_pcsAgcmCustomizeRender->Render();
	}

	//@{ 2006/08/29 burumal
	if ( g_pEngine->IsMouseLeftBtnDown() == FALSE )
	{
		if ( m_pcsAgcmCustomizeRender->IsRightTurnOn() || m_pcsAgcmCustomizeRender->IsLeftTurnOn() )
		{			
			m_pcsAgcmCustomizeRender->LeftTurnOff();
			m_pcsAgcmCustomizeRender->RightTurnOff();
		}

		if ( m_pcsAgcmCustomizeRender->IsZoomInOn() || m_pcsAgcmCustomizeRender->IsZoomOutOn() )
		{			
			m_pcsAgcmCustomizeRender->ZoomInOff();
			m_pcsAgcmCustomizeRender->ZoomOutOff();
		}
	}
	//@}

	return TRUE;
}

BOOL AgcmUICustomizing::OnDestroy()
{
	if (m_pcsAgcmCustomizeRender->IsStart())
	{
		m_csCustomizePreview.m_csTextureList.DeleteImage_ID(m_lAddTextureID, FALSE);
		m_pcsAgcmCustomizeRender->End();
	}

	return TRUE;
}

BOOL AgcmUICustomizing::AddEvent()
{
	m_lEventUIOpen	= m_pcsAgcmUIManager2->AddEvent("OpenCustomizeUI");
	if (m_lEventUIOpen < 0)
		return FALSE;

	m_lEventUIClose	= m_pcsAgcmUIManager2->AddEvent("CloseCustomizeUI");
	if (m_lEventUIClose < 0)
		return FALSE;

	m_lEventBuyConfirm	= m_pcsAgcmUIManager2->AddEvent("CustomizeBuyConfirm");
	if (m_lEventBuyConfirm < 0)
		return FALSE;

	m_lEventNeedDismountRide = m_pcsAgcmUIManager2->AddEvent("CustomizeNeedDismountRide");
	if (m_lEventNeedDismountRide < 0)
		return FALSE;

	m_lEventNeedRestoreTransform = m_pcsAgcmUIManager2->AddEvent("CustomizeNeedRestoreTransform");
	if (m_lEventNeedRestoreTransform < 0)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICustomizing::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeGetEdit", CBGetEditControl, 3, "face edit control", "hair edit control", "total edit control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeBuy", CBBuy, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeInit", CBInit, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeConfirmOK", CBConfirmOK, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeSelectFace", CBSelectFace, 1, "user data"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeSelectHair", CBSelectHair, 1, "user data"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeRotateLeft", CBRotateLeft, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeRotateRight", CBRotateRight, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeRotateEnd", CBRotateEnd, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeZoomIn", CBZoomIn, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeZoomOut", CBZoomOut, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeZoomEnd", CBZoomEnd, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeInitialize", CBInitialize, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CustomizeCloseUI", CBCloseUI, 0))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICustomizing::AddUserData()
{
	m_pcsUserDataFaceList = m_pcsAgcmUIManager2->AddUserData("CustomizeFaceList", m_alFace, sizeof(INT32), CHARCUSTOMIZE_MAX_LIST, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataFaceList)
		return FALSE;

	m_pcsUserDataHairList = m_pcsAgcmUIManager2->AddUserData("CustomizeHairList", m_alHair, sizeof(INT32), CHARCUSTOMIZE_MAX_LIST, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataHairList)
		return FALSE;

	m_pcsUserDataPriceMoney = m_pcsAgcmUIManager2->AddUserData("CustomizeBuyPrice", &m_lTotalPriceMoney, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataPriceMoney)
		return FALSE;

	m_pcsUserDataIsActiveBuy = m_pcsAgcmUIManager2->AddUserData("CustomizeIsActiveBuy", &m_bIsActiveBuy, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pcsUserDataIsActiveBuy)
		return FALSE;

	return TRUE;
}

BOOL AgcmUICustomizing::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CustomizeFaceName", 0, CBDisplayFaceName, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CustomizeHairName", 0, CBDisplayHairName, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "CustomizeBuyPrice", 0, CBDisplayBuyPrice, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

BOOL AgcmUICustomizing::CBGetEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1 || !pData2 || !pData3)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	AgcdUIControl	*pcsFaceEditControl		= (AgcdUIControl *)	pData1;
	AgcdUIControl	*pcsHairEditControl		= (AgcdUIControl *)	pData2;
	AgcdUIControl	*pcsTotalEditControl	= (AgcdUIControl *)	pData3;

	pThis->m_pcsEditFace	= (AcUIEdit *) pcsFaceEditControl->m_pcsBase;
	pThis->m_pcsEditHair	= (AcUIEdit *) pcsHairEditControl->m_pcsBase;
	pThis->m_pcsEditSummary	= (AcUIEdit *) pcsTotalEditControl->m_pcsBase;

	return TRUE;
}

BOOL AgcmUICustomizing::IsEnoughCost()
{
	m_lTotalPriceMoney	= 0;
	m_lTotalPriceSkull	= 0;

	bool byItem        = false;
	int  itemTid       = 0;
	int  minLevelToBuy = 0;

	const INT32 hairIndex = m_pcsUserDataHairList->m_lSelectedIndex;
	const INT32 faceIndex = m_pcsUserDataFaceList->m_lSelectedIndex;

	if (hairIndex >= 0)
	{
		if ( m_acsCustomizeHairList[hairIndex].m_lPriceSkull == 0 )
			m_lTotalPriceMoney = ( INT32 ) PriceWithTax( ( INT64 ) m_acsCustomizeHairList[hairIndex].m_lPriceMoney );
		else
		{
			m_lTotalPriceMoney = m_acsCustomizeHairList[hairIndex].m_lPriceMoney;
			byItem = true;
			itemTid = m_acsCustomizeHairList[hairIndex].m_lPriceSkull;
		}

		minLevelToBuy = m_acsCustomizeHairList[hairIndex].m_lUseLevel;
	}

	if (faceIndex >= 0)
	{
		if ( m_acsCustomizeFaceList[faceIndex].m_lPriceSkull == 0 )
			m_lTotalPriceMoney += ( INT32 ) PriceWithTax( ( INT64 )m_acsCustomizeFaceList[faceIndex].m_lPriceMoney );
		else
		{
			m_lTotalPriceMoney += m_acsCustomizeFaceList[faceIndex].m_lPriceMoney;
			byItem = true;
			itemTid = m_acsCustomizeFaceList[faceIndex].m_lPriceSkull;
		}

		minLevelToBuy = MIN(minLevelToBuy, m_acsCustomizeFaceList[faceIndex].m_lUseLevel);
	}
	
	if ( minLevelToBuy > m_pcsAgpmCharacter->GetLevel(m_pcsAgcmCharacter->GetSelfCharacter()) )
		return FALSE;

	INT64 countAtInven = 0;
	if ( false == byItem )
		countAtInven = GetInvenMoney();
	else
		countAtInven = m_pcsAgpmEventCharCustomize->GetItemCount(m_pcsAgcmCharacter->GetSelfCharacter(), itemTid);

	return (m_lTotalPriceMoney > countAtInven) ? FALSE : TRUE;
}

BOOL AgcmUICustomizing::CBBuy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	if (pThis->m_pcsUserDataHairList->m_lSelectedIndex < 0 &&
		pThis->m_pcsUserDataFaceList->m_lSelectedIndex < 0)
		return TRUE;

	if (pThis->IsEnoughCost())
	{
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataPriceMoney);

		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventBuyConfirm);
	}

	return TRUE;
}

BOOL AgcmUICustomizing::CBInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	return pThis->InitializeList();
}

BOOL AgcmUICustomizing::CBConfirmOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	// 산다고 서버로 보낸다.

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventCharCustomize->MakePacketRequestCustomize(pThis->m_pcsAgcmCharacter->GetSelfCID(),
																							pThis->m_pcsEvent,
																							(pThis->m_pcsUserDataFaceList->m_lSelectedIndex >= 0) ? pThis->m_acsCustomizeFaceList[pThis->m_pcsUserDataFaceList->m_lSelectedIndex].m_lNumber : (-1),
																							(pThis->m_pcsUserDataHairList->m_lSelectedIndex >= 0) ? pThis->m_acsCustomizeHairList[pThis->m_pcsUserDataHairList->m_lSelectedIndex].m_lNumber : (-1),
																							&nPacketLength);



	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength);

	pThis->m_pcsAgpmEventCharCustomize->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmUICustomizing::CBSelectFace(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsUserDataFaceList->m_lSelectedIndex	= pcsSourceControl->m_lUserDataIndex;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataFaceList);

	pThis->SetFaceDescription();
	pThis->SetSummaryDescription();

	if (pThis->m_pcsUserDataFaceList->m_lSelectedIndex >= 0)
		pThis->m_pcsAgcmCustomizeRender->SetFace(pThis->m_acsCustomizeFaceList[pThis->m_pcsUserDataFaceList->m_lSelectedIndex].m_lNumber);

	if (pThis->IsEnoughCost())
		pThis->m_bIsActiveBuy	= TRUE;
	else
		pThis->m_bIsActiveBuy	= FALSE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataIsActiveBuy);
	
	return TRUE;
}

BOOL AgcmUICustomizing::CBSelectHair(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsUserDataHairList->m_lSelectedIndex	= pcsSourceControl->m_lUserDataIndex;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataHairList);

	pThis->SetHairDescription();
	pThis->SetSummaryDescription();

	if (pThis->m_pcsUserDataHairList->m_lSelectedIndex >= 0)
		pThis->m_pcsAgcmCustomizeRender->SetHair(pThis->m_acsCustomizeHairList[pThis->m_pcsUserDataHairList->m_lSelectedIndex].m_lNumber);

	if (pThis->IsEnoughCost())
		pThis->m_bIsActiveBuy	= TRUE;
	else
		pThis->m_bIsActiveBuy	= FALSE;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsUserDataIsActiveBuy);
	
	return TRUE;
}

BOOL AgcmUICustomizing::CBDisplayFaceName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;
	INT32	lListIndex	= *(INT32 *) pData;

	if (lListIndex >= pThis->m_lNumFaceList)
		return TRUE;

	sprintf(szDisplay, "%s", pThis->m_acsCustomizeFaceList[lListIndex].m_szName);

	return TRUE;
}

BOOL AgcmUICustomizing::CBDisplayHairName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;
	INT32	lListIndex	= *(INT32 *) pData;

	if (lListIndex >= pThis->m_lNumHairList)
		return TRUE;

	sprintf(szDisplay, "%s", pThis->m_acsCustomizeHairList[lListIndex].m_szName);

	return TRUE;
}

BOOL AgcmUICustomizing::CBDisplayBuyPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	sprintf(szDisplay, pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_BUY_PRICE_FMT), pThis->m_lTotalPriceMoney);

	return TRUE;
}

BOOL AgcmUICustomizing::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)		pData;
	ApdEvent			*pcsEvent		= (ApdEvent *)			pCustData;

	if (pcsCharacter->m_bRidable)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNeedDismountRide);
		return TRUE;
	}

	if (pcsCharacter->m_bIsTrasform || pcsCharacter->m_bIsEvolution)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNeedRestoreTransform);
		return TRUE;
	}

	pThis->m_pcsEvent	= pcsEvent;

	if (!pThis->InitializeList())
		return FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUIOpen);

	return TRUE;
}

BOOL AgcmUICustomizing::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	pThis->m_bIsOpenedUI	= FALSE;

	if (pThis->m_pcsAgcmCustomizeRender->IsStart())
	{
		pThis->m_csCustomizePreview.m_csTextureList.DeleteImage_ID(pThis->m_lAddTextureID, FALSE);
		pThis->m_pcsAgcmCustomizeRender->End();
	}

	return TRUE;
}

BOOL AgcmUICustomizing::CBRotateLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsAgcmCustomizeRender->LeftTurnOn();
	
	return TRUE;
}

BOOL AgcmUICustomizing::CBRotateRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsAgcmCustomizeRender->RightTurnOn();
	
	return TRUE;
}

BOOL AgcmUICustomizing::CBRotateEnd(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsAgcmCustomizeRender->LeftTurnOff();
	pThis->m_pcsAgcmCustomizeRender->RightTurnOff();	

	return TRUE;
}

BOOL AgcmUICustomizing::CBZoomIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsAgcmCustomizeRender->ZoomInOn();
	
	return TRUE;
}

BOOL AgcmUICustomizing::CBZoomOut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsAgcmCustomizeRender->ZoomOutOn();

	return TRUE;
}

BOOL AgcmUICustomizing::CBZoomEnd(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	pThis->m_pcsAgcmCustomizeRender->ZoomInOff();
	pThis->m_pcsAgcmCustomizeRender->ZoomOutOff();

	return TRUE;
}

BOOL AgcmUICustomizing::CBInitialize(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis			= (AgcmUICustomizing *)	pClass;

	return pThis->InitializeList();
}

BOOL AgcmUICustomizing::InitializeList()
{
	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	m_bIsOpenedUI	= TRUE;

	m_stOpenedPosition	= pcsSelfCharacter->m_stPos;

	if (!m_pcsAgcmCustomizeRender->IsStart())
	{
		AgcdCharacter	*pcsAgcdCharacter	= m_pcsAgcmCharacter->GetCharacterData(pcsSelfCharacter);

		m_pcsAgcmCustomizeRender->Start(pcsAgcdCharacter);

		RwTexture *	pstTexture	= m_pcsAgcmCustomizeRender->GetTexture();

		m_lAddTextureID	= m_csCustomizePreview.m_csTextureList.AddTexture( pstTexture );
		m_csCustomizePreview.SetRednerTexture(m_lAddTextureID);
	}

	// initialize hair and face list
	
	ZeroMemory(m_acsCustomizeFaceList, sizeof(m_acsCustomizeFaceList));
	ZeroMemory(m_acsCustomizeHairList, sizeof(m_acsCustomizeHairList));

	m_lNumFaceList	= 0;
	m_lNumHairList	= 0;

	for (int i = 0; i < CHARCUSTOMIZE_MAX_LIST; ++i)
	{
		if (m_pcsAgpmEventCharCustomize->m_astCustomizeList[i].m_pcsCharacterTemplate &&
			m_pcsAgpmEventCharCustomize->m_astCustomizeList[i].m_pcsCharacterTemplate == pcsSelfCharacter->m_pcsCharacterTemplate &&
			m_pcsAgpmEventCharCustomize->m_astCustomizeList[i].m_eCase != CHARCUSTOMIZE_CHAR_LOGIN)
		{
			if (m_pcsAgpmEventCharCustomize->m_astCustomizeList[i].m_eType == CHARCUSTOMIZE_TYPE_FACE &&
				m_pcsAgpmEventCharCustomize->m_astCustomizeList[i].m_lNumber != pcsSelfCharacter->m_lFaceIndex)
			{
				m_acsCustomizeFaceList[m_lNumFaceList]	= m_pcsAgpmEventCharCustomize->m_astCustomizeList[i];
				++m_lNumFaceList;
			}
			else if (m_pcsAgpmEventCharCustomize->m_astCustomizeList[i].m_eType == CHARCUSTOMIZE_TYPE_HAIR &&
				m_pcsAgpmEventCharCustomize->m_astCustomizeList[i].m_lNumber != pcsSelfCharacter->m_lHairIndex)
			{
				m_acsCustomizeHairList[m_lNumHairList]	= m_pcsAgpmEventCharCustomize->m_astCustomizeList[i];
				++m_lNumHairList;
			}
		}
	}

	m_pcsUserDataFaceList->m_lSelectedIndex		= (-1);
	m_pcsUserDataHairList->m_lSelectedIndex		= (-1);

	m_pcsUserDataFaceList->m_stUserData.m_lCount	= m_lNumFaceList;
	m_pcsUserDataHairList->m_stUserData.m_lCount	= m_lNumHairList;

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataFaceList);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataHairList);

	SetHairDescription();
	SetFaceDescription();
	SetSummaryDescription();

	m_pcsAgcmCustomizeRender->SetFace(pcsSelfCharacter->m_lFaceIndex);
	m_pcsAgcmCustomizeRender->SetHair(pcsSelfCharacter->m_lHairIndex);

	m_lTotalPriceMoney	= 0;
	m_lTotalPriceSkull	= 0;

	m_bIsActiveBuy		= TRUE;

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataIsActiveBuy);
	
	return TRUE;
}

BOOL AgcmUICustomizing::SetFaceDescription()
{
	if (!m_pcsEditFace || !m_pcsUserDataFaceList || m_pcsUserDataFaceList->m_lSelectedIndex < 0)
		return FALSE;

	CHAR szBuffer[1024] = {0, };

	const int index     = m_pcsUserDataFaceList->m_lSelectedIndex;
	const int minLevel  = m_acsCustomizeFaceList[index].m_lUseLevel;
	const int userLevel = m_pcsAgpmCharacter->GetLevel( m_pcsAgcmCharacter->GetSelfCharacter() );

	sprintf(szBuffer,
			"%s : <C%s>%d<C16773320>\n",
			m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_USELEVEL),
			(userLevel >= minLevel) ? "16773320" : "16711680",
			minLevel);

	// 해골 개수가 0보다 크면 겔드가 아닌 해골 TID로 비용을 계산한다. 이 경우에는 가격이 해골 개수다.

	int   price		 = 0;	// 겔드 혹은 아이템 개수
	INT64 invenMoney = 0;	// 인벤에 소지중인 겔드 혹은 아이템 개수

	// 겔드로 구입 가능?
	if ( m_acsCustomizeFaceList[index].m_lPriceSkull == 0 )
	{
		price = ( INT32 ) PriceWithTax(m_acsCustomizeFaceList[index].m_lPriceMoney);
		invenMoney = GetInvenMoney();
	}
	// 특정 아이템이 필요하다
	else
	{
		price = m_acsCustomizeFaceList[index].m_lPriceMoney;
		invenMoney = m_pcsAgpmEventCharCustomize->GetItemCount(m_pcsAgcmCharacter->GetSelfCharacter(), m_acsCustomizeFaceList[index].m_lPriceSkull);
	}

	sprintf( szBuffer + strlen(szBuffer),
			 "%s : <C%s>%d <C16773320>",
			 m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_PRICEMONEY),
			 ( invenMoney >= price ) ? "16773320" : "16711680",
			 price);

	if (AP_SERVICE_AREA_JAPAN == g_eServiceArea)
		sprintf(szBuffer + strlen(szBuffer), "%s\n", ClientStr().GetStr(STI_NUMBER_UNIT));
	else if( AP_SERVICE_AREA_CHINA == g_eServiceArea )
	{
		sprintf(szBuffer + strlen(szBuffer), "%s\n", m_pcsAgcmUIManager2->GetUIMessage("Customize_Ticket"));
	}
	else
		sprintf(szBuffer + strlen(szBuffer), "%s\n", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));

	m_pcsEditFace->SetLineDelimiter("\n");
	m_pcsEditFace->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUICustomizing::SetHairDescription()
{
	if (!m_pcsEditHair || !m_pcsUserDataHairList || m_pcsUserDataHairList->m_lSelectedIndex < 0)
		return FALSE;

	CHAR szBuffer[1024] = {0, };

	const int index     = m_pcsUserDataHairList->m_lSelectedIndex;
	const int minLevel  = m_acsCustomizeHairList[index].m_lUseLevel;
	const int userLevel = m_pcsAgpmCharacter->GetLevel(m_pcsAgcmCharacter->GetSelfCharacter());

	sprintf( szBuffer,
			 "%s : <C%s>%d<C16773320>\n",
			 m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_USELEVEL),
			 (userLevel >= minLevel) ? "16773320" : "16711680",
			 minLevel);

	int   price		 = 0;	// 겔드 혹은 아이템 개수
	INT64 invenMoney = 0;	// 인벤에 소지중인 겔드 혹은 아이템 개수

	// 겔드로 구입 가능?
	if ( m_acsCustomizeHairList[index].m_lPriceSkull == 0 )
	{
		price = ( INT32 ) PriceWithTax(m_acsCustomizeHairList[index].m_lPriceMoney);
		invenMoney = GetInvenMoney();
	}
	// 특정 아이템이 필요하다
	else
	{
		price = m_acsCustomizeHairList[index].m_lPriceMoney;
		invenMoney = m_pcsAgpmEventCharCustomize->GetItemCount(m_pcsAgcmCharacter->GetSelfCharacter(), m_acsCustomizeHairList[index].m_lPriceSkull);
	}

	sprintf(szBuffer + strlen(szBuffer),
			"%s : <C%s>%d <C16773320>",
			m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_PRICEMONEY),
			(invenMoney >= price) ? "16773320" : "16711680",
			price);

	if (AP_SERVICE_AREA_JAPAN == g_eServiceArea)
		sprintf(szBuffer + strlen(szBuffer), "%s\n", ClientStr().GetStr(STI_NUMBER_UNIT));
	else if( AP_SERVICE_AREA_CHINA == g_eServiceArea )
	{
		sprintf(szBuffer + strlen(szBuffer), "%s\n", m_pcsAgcmUIManager2->GetUIMessage("Customize_Ticket"));
	}
	else
		sprintf(szBuffer + strlen(szBuffer), "%s\n", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));

	m_pcsEditHair->SetLineDelimiter("\n");
	m_pcsEditHair->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUICustomizing::SetSummaryDescription()
{
	if (!m_pcsEditSummary || !m_pcsUserDataHairList || !m_pcsUserDataFaceList)
		return FALSE;

	CHAR  szBuffer[1024] = {0, };
	
	INT64 totalCost = 0;
	INT32 itemTid	= 0;

	const INT32 hairIndex = m_pcsUserDataHairList->m_lSelectedIndex;
	const INT32 faceIndex = m_pcsUserDataFaceList->m_lSelectedIndex;

	// 아이템을 이용해서 성형 술사를 이용하나?
	const bool byItem = m_acsCustomizeFaceList[faceIndex].m_lPriceSkull || m_acsCustomizeHairList[hairIndex].m_lPriceSkull;

	// 머리 비용 계산
	if (hairIndex >= 0)
	{
		sprintf(szBuffer,
				"%s : %s\n",
				m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_HAIRSTYLE),
				m_acsCustomizeHairList[hairIndex].m_szName);

		if ( false == byItem )
			totalCost = PriceWithTax(m_acsCustomizeHairList[hairIndex].m_lPriceMoney);
		else
		{
			totalCost = m_acsCustomizeHairList[hairIndex].m_lPriceMoney;
			itemTid   = m_acsCustomizeHairList[hairIndex].m_lPriceSkull;
		}
	}

	// 얼굴 비용 계산
	if (faceIndex >= 0)
	{
		sprintf(szBuffer + strlen(szBuffer),
				"%s : %s\n",
				m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_FACESTYLE),
				m_acsCustomizeFaceList[faceIndex].m_szName);

		if ( false == byItem )
			totalCost += PriceWithTax( m_acsCustomizeFaceList[faceIndex].m_lPriceMoney );
		else
		{
			totalCost += m_acsCustomizeFaceList[faceIndex].m_lPriceMoney;
			itemTid    = m_acsCustomizeFaceList[faceIndex].m_lPriceSkull;
		}
	}

	if ( totalCost > 0 )
	{
		INT64 countOnInven = 0;

		if ( false == byItem )
			countOnInven = GetInvenMoney();
		else
			countOnInven = m_pcsAgpmEventCharCustomize->GetItemCount( m_pcsAgcmCharacter->GetSelfCharacter(), itemTid );

		
		sprintf(szBuffer + strlen(szBuffer),
				"%s : <C%s>%d <C16773320>",
				m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_TOTALPRICEMONEY),
				(countOnInven >= totalCost) ? "16773320" : "16711680",
				totalCost);

		if (AP_SERVICE_AREA_JAPAN == g_eServiceArea)
		{
			sprintf(szBuffer + strlen(szBuffer), "%s\n",
					ClientStr().GetStr(STI_NUMBER_UNIT));
		}
		else if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
		{
			sprintf(szBuffer + strlen(szBuffer), "%s\n",
					m_pcsAgcmUIManager2->GetUIMessage("Customize_Ticket"));
		}
		else
		{
			sprintf(szBuffer + strlen(szBuffer), "%s\n",
					m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_MONEY_NAME));
		}
	}

	m_pcsEditSummary->SetLineDelimiter("\n");
	m_pcsEditSummary->SetText(szBuffer);

	return TRUE;
}

BOOL AgcmUICustomizing::CBCloseUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	pThis->m_bIsOpenedUI	= FALSE;

	if (pThis->m_pcsAgcmCustomizeRender->IsStart())
	{
		pThis->m_csCustomizePreview.m_csTextureList.DeleteImage_ID(pThis->m_lAddTextureID, FALSE);
		pThis->m_pcsAgcmCustomizeRender->End();
	}

	return TRUE;
}

BOOL AgcmUICustomizing::CBSelfUpdateCustomize(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;
	AgpdCharacter *pcsCharacter	= (AgpdCharacter *)	pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	if (pThis->m_lPrevFaceIndex != pcsCharacter->m_lFaceIndex)
	{
		for (int i = 0; i < CHARCUSTOMIZE_MAX_LIST; ++i)
		{
			if (pThis->m_acsCustomizeFaceList[i].m_lNumber == pcsCharacter->m_lFaceIndex)
			{
				CHAR	szBuffer[128];
				ZeroMemory(szBuffer, sizeof(szBuffer));

				sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_FACECHANGE_FMT), pThis->m_acsCustomizeFaceList[i].m_szName);

				pThis->m_pcsAgcmChatting2->AddSystemMessage(szBuffer, 0xffffff00);

				pThis->m_pcsAgcmUIChatting2->SetSystemChatMessage();

				break;
			}
		}
	}

	if (pThis->m_lPrevHairIndex != pcsCharacter->m_lHairIndex)
	{
		for (int i = 0; i < CHARCUSTOMIZE_MAX_LIST; ++i)
		{
			if (pThis->m_acsCustomizeHairList[i].m_lNumber == pcsCharacter->m_lHairIndex)
			{
				CHAR	szBuffer[128];
				ZeroMemory(szBuffer, sizeof(szBuffer));

				sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCMUIMESSAGE_CUSTOMIZE_HAIRCHANGE_FMT), pThis->m_acsCustomizeHairList[i].m_szName);

				pThis->m_pcsAgcmChatting2->AddSystemMessage(szBuffer, 0xffffff00);

				pThis->m_pcsAgcmUIChatting2->SetSystemChatMessage();

				break;
			}
		}
	}

	pThis->m_lPrevFaceIndex	= pcsCharacter->m_lFaceIndex;
	pThis->m_lPrevHairIndex	= pcsCharacter->m_lHairIndex;

	return TRUE;
}

BOOL AgcmUICustomizing::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;
	AgpdCharacter *pcsCharacter	= (AgpdCharacter *)	pData;

	pThis->m_lPrevFaceIndex	= pcsCharacter->m_lFaceIndex;
	pThis->m_lPrevHairIndex	= pcsCharacter->m_lHairIndex;

	return TRUE;
}

BOOL AgcmUICustomizing::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUICustomizing	*pThis	= (AgcmUICustomizing *)	pClass;

	if (!pThis->m_bIsOpenedUI)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_stPos, pThis->m_stOpenedPosition);

	if ((INT32) fDistance < 150)
		return TRUE;

	pThis->m_bIsOpenedUI	= FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventUIClose);

	return TRUE;
}

INT64 AgcmUICustomizing::PriceWithTax(INT64 originPrice)
{
	int taxRatio = m_pcsAgpmCharacter->GetTaxRatio(m_pcsAgcmCharacter->GetSelfCharacter());
	return originPrice + (int)( taxRatio * originPrice / 100);
}

INT64 AgcmUICustomizing::GetInvenMoney()
{
	INT64 invenMoney = 0;
	m_pcsAgpmCharacter->GetMoney(m_pcsAgcmCharacter->GetSelfCharacter(), &invenMoney);

	return invenMoney;
}
