#include "AgcmUIEventTeleport.h"
#include "AuRegionNameConvert.h"

AgcmUIEventTeleport::AgcmUIEventTeleport()
{
	SetModuleName("AgcmUIEventTeleport");
	EnableIdle(TRUE);

	m_bIsStartedMapLoading			= FALSE;
	m_bIsOpenedTeleportListWindow	= FALSE;

	ZeroMemory(m_apcsTargetPoint, sizeof(AgpdTeleportPoint *) * AGCMUI_TELEPORT_MAX_TARGET_POINT);
	ZeroMemory(&m_stTeleportOpenPos, sizeof(AuPOS));

	m_szTeleportSelfName			= NULL;

	m_pcsEvent						= NULL;
	m_pcsPvPZonePoint				= NULL;
	m_bIsDisablePoint				= FALSE;
}

AgcmUIEventTeleport::~AgcmUIEventTeleport()
{
}

BOOL AgcmUIEventTeleport::OnAddModule()
{
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)		GetModule("AgcmUIManager2");
	m_pcsApmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pcsAgcmEventTeleport		= (AgcmEventTeleport *)		GetModule("AgcmEventTeleport");
	m_pcsAgpmEventTeleport		= (AgpmEventTeleport *)		GetModule("AgpmEventTeleport");
	m_pcsAgcmMap				= (AgcmMap *)				GetModule("AgcmMap");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmChatting2			= (AgcmChatting2 *)			GetModule("AgcmChatting2");
	m_pcsAgpmSiegeWar			= (AgpmSiegeWar *)			GetModule("AgpmSiegeWar");

	if (!m_pcsAgcmUIManager2 ||
		!m_pcsApmEventManager ||
		!m_pcsAgcmEventTeleport ||
		!m_pcsAgpmEventTeleport ||
		!m_pcsAgcmMap ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmChatting2 ||
		!m_pcsAgpmSiegeWar)
		return FALSE;

	if (!m_pcsAgcmEventTeleport->SetCallbackSelectTeleportPoint(CBSelectTeleportPoint, this))
		return FALSE;
	if (!m_pcsAgcmEventTeleport->SetCallbackCannotUseForTransforming(CBCannotUseForTransforming, this))
		return FALSE;
	if (!m_pcsAgcmEventTeleport->SetCallbackCannotUseForCombatMode(CBCannotUseForCombatMode, this))
		return FALSE;

	if (!m_pcsAgpmEventTeleport->SetCallbackReceivedTeleportStart(CBReceivedTeleportStart, this))
		return FALSE;
	if (!m_pcsAgpmEventTeleport->SetCallbackReceivedTeleportCanceled(CBReceivedTeleportCanceled, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!m_pcsAgcmMap->SetCallbackMapLoadEnd(CBMapLoadEnd, this))
		return FALSE;

	if (!AddFunctions())	return FALSE;
	if (!AddEvents())		return FALSE;
	if (!AddUserData())		return FALSE;
	if (!AddDisplay())		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventTeleport::OnInit()
{
	return TRUE;
}

BOOL AgcmUIEventTeleport::OnDestroy()
{
	return TRUE;
}

BOOL AgcmUIEventTeleport::OnIdle(UINT32 ulClockCount)
{
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstTargetUD );
	RefreshTeleportList();
	return TRUE;
}

BOOL AgcmUIEventTeleport::AddFunctions()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "Teleport", CBRequestTeleport, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "TeleportRefreshList", CBRefreshList, 0))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventTeleport::AddEvents()
{
	m_lEventSelectTeleportWindowClose	= m_pcsAgcmUIManager2->AddEvent(AGCMUITELEPORT_EVENT_SELECT_WINDOW_CLOSE);
	if (m_lEventSelectTeleportWindowClose < 0)	return FALSE;

	m_lEventIDSelectTeleportPoint	= m_pcsAgcmUIManager2->AddEvent("need_select_teleport_point");
	if (m_lEventIDSelectTeleportPoint < 0)		return FALSE;

	m_lEventLoadingWindowOpen = m_pcsAgcmUIManager2->AddEvent(AGCMUITELEPORT_EVENT_LOADING_WINDOW_OPEN);
	if (m_lEventLoadingWindowOpen < 0)			return FALSE;

	m_lEventLoadingWindowClose = m_pcsAgcmUIManager2->AddEvent(AGCMUITELEPORT_EVENT_LOADING_WINDOW_CLOSE);
	if (m_lEventLoadingWindowClose < 0)			return FALSE;

	m_lEventConfirmPvPZone = m_pcsAgcmUIManager2->AddEvent("TELEPORT_ConfirmPvPZone", CBConfirmPvPZone, this);
	if (m_lEventConfirmPvPZone < 0)				return FALSE;

	m_lEventNotEnoughMoney = m_pcsAgcmUIManager2->AddEvent("TELEPORT_NotEnoughMOney", CBConfirmPvPZone, this);
	if (m_lEventNotEnoughMoney < 0)				return FALSE;

	m_lEventDisplayNormal = m_pcsAgcmUIManager2->AddEvent("TELEPORT_DisplayNormal");
	if (m_lEventDisplayNormal < 0)				return FALSE;

	m_lEventDisplayNoMoney = m_pcsAgcmUIManager2->AddEvent("TELEPORT_DisplayNoMoney");
	if (m_lEventDisplayNoMoney < 0)				return FALSE;

	m_lEventDisplayDisable = m_pcsAgcmUIManager2->AddEvent("TELEPORT_DisplayDisable");
	if (m_lEventDisplayDisable < 0)				return FALSE;
		
	return TRUE;
}

BOOL AgcmUIEventTeleport::AddUserData()
{
	m_pstTargetUD = m_pcsAgcmUIManager2->AddUserData("Teleport_Targets", m_apcsTargetPoint, sizeof(AgpdTeleportPoint *), AGCMUI_TELEPORT_MAX_TARGET_POINT, AGCDUI_USERDATA_TYPE_TELEPORT_POINT);
	if (!m_pstTargetUD)		return FALSE;
		
	return TRUE;
}

BOOL AgcmUIEventTeleport::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Teleport_Name", 0, CBDisplayTeleport, AGCDUI_USERDATA_TYPE_TELEPORT_POINT))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Teleport_Self", 0, CBDisplaySelfTeleport, AGCDUI_USERDATA_TYPE_TELEPORT_POINT))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddDisplay(this, "Teleport_Fee", 0, CBDisplayTeleportFee, AGCDUI_USERDATA_TYPE_TELEPORT_POINT))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBSelectTeleportPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)	return FALSE;
		
	AgcmUIEventTeleport		*pThis				= (AgcmUIEventTeleport *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pCustData;
	AgpdTeleportAttach		*pcsAttachData		= (AgpdTeleportAttach *)	pcsEvent->m_pvData;

	PVOID					pvDisableTargetID		= (PVOID)	((PVOID *)  pData)[2];
	INT16					unDisableTargetIDLength	= (UINT16)	((PVOID *)  pData)[3];

	if( !pcsAttachData->m_pcsPoint )	return FALSE;
		
	// 2007.01.19. steeple
	// 공성후에 성주가 포탈 이용이 안된다.
	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter )				return FALSE;

	// 얘는 NULL 일수도 있다.
	AgpdSiegeWar* pcsSiegeWarInfo = pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pcsSelfCharacter);

	// 공성 전용 포탈은 수성길드만 이용할 수 있다.
	if( pcsAttachData->m_pcsPoint->m_eSpecialType == AGPDTELEPORT_SPECIAL_TYPE_SIEGEWAR &&
		(!pThis->m_pcsAgpmSiegeWar->IsDefenseGuild(pcsSelfCharacter, pcsSiegeWarInfo) &&
		!pThis->m_pcsAgpmSiegeWar->IsOwnerGuild(pcsSelfCharacter, pcsSiegeWarInfo) ) )
		return TRUE;

	pThis->m_pcsEvent			= pcsEvent;
	pThis->m_ePointSpecialType	= AGPDTELEPORT_SPECIAL_TYPE_NORMAL;
	pThis->m_szTeleportSelfName	= NULL;

	ZeroMemory( pThis->m_lDisableTargetPoint, sizeof( pThis->m_lDisableTargetPoint ) );
	if (unDisableTargetIDLength > 0)
	{
		if (unDisableTargetIDLength < AGCMUI_TELEPORT_MAX_TARGET_POINT * sizeof(INT32))
			CopyMemory(pThis->m_lDisableTargetPoint, pvDisableTargetID, unDisableTargetIDLength);
		else
			CopyMemory(pThis->m_lDisableTargetPoint, pvDisableTargetID, sizeof(pThis->m_lDisableTargetPoint));
	}

	pThis->m_bIsDisablePoint	= pThis->IsDisablePoint(pcsAttachData->m_pcsPoint);
	if (pcsAttachData->m_pcsPoint->m_eRegionType == AGPDTELEPORT_REGION_TYPE_PVP)
	{
		pThis->m_lTargetPoint		= 1;
		pThis->m_apcsTargetPoint[0]	= NULL;
		pThis->m_ePointSpecialType	= AGPDTELEPORT_SPECIAL_TYPE_RETURN_ONLY;
		pThis->m_szTeleportSelfName	= pcsAttachData->m_pcsPoint->m_szPointName;
	}
	else
	{
		pThis->m_lTargetPoint = pThis->m_pcsAgpmEventTeleport->GetTargetPointList(pcsAttachData->m_pcsPoint, pThis->m_apcsTargetPoint, AGCMUI_TELEPORT_MAX_TARGET_POINT);
		if (pThis->m_lTargetPoint > AGCMUI_TELEPORT_MAX_TARGET_POINT)
			pThis->m_lTargetPoint	= AGCMUI_TELEPORT_MAX_TARGET_POINT;

		for (int i = 0; i < pThis->m_lTargetPoint; ++i)
		{
			if (strncmp(((AgpdTeleportAttach *) pcsEvent->m_pvData)->m_pcsPoint->m_szPointName, pThis->m_apcsTargetPoint[i]->m_szPointName, AGPD_TELEPORT_MAX_POINT_NAME) == 0)
			{
				pThis->m_szTeleportSelfName	= ((AgpdTeleportAttach *) pcsEvent->m_pvData)->m_pcsPoint->m_szPointName;

				CopyMemory(pThis->m_apcsTargetPoint + i,
						pThis->m_apcsTargetPoint + i + 1,
						sizeof(AgpdTeleportPoint *) * (pThis->m_lTargetPoint - i - 1));
				
				pThis->m_apcsTargetPoint[pThis->m_lTargetPoint - 1] = NULL;

				--pThis->m_lTargetPoint;
				--i;
			}
			else
			{
				for (int j = 0; j < i; ++ j)
				{
					if (strncmp(pThis->m_apcsTargetPoint[j]->m_szPointName, pThis->m_apcsTargetPoint[i]->m_szPointName, AGPD_TELEPORT_MAX_POINT_NAME) == 0)
					{
						CopyMemory(pThis->m_apcsTargetPoint + i,
								pThis->m_apcsTargetPoint + i + 1,
								sizeof(AgpdTeleportPoint *) * (pThis->m_lTargetPoint - i - 1));
						
						pThis->m_apcsTargetPoint[pThis->m_lTargetPoint - 1] = NULL;

						--pThis->m_lTargetPoint;
						--i;

						break;
					}
				}
			}
		}
	}

	if (pThis->m_szTeleportSelfName	== NULL)
		pThis->m_szTeleportSelfName = pcsAttachData->m_pcsPoint->m_szPointName;

	pThis->m_pstTargetUD->m_stUserData.m_lCount	= pThis->m_lTargetPoint;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstTargetUD);

	pThis->m_bIsOpenedTeleportListWindow	= TRUE;

	if (pcsSelfCharacter)
	{
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsSelfCharacter, NULL);

		pThis->m_stTeleportOpenPos	= pcsSelfCharacter->m_stPos;
	}

	pThis->RefreshTeleportList();

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_Teleport" );
	if( !pcdUI ) return FALSE;

	AgcdUIControl* pcdControl = pcmUIManager->GetControl( pcdUI, "CONTROL_699" );
	if( !pcdControl ) return FALSE;

	AcUIList* pList = ( AcUIList* )pcdControl->m_pcsBase;
	if( !pList ) return FALSE;

	pList->OnMouseWheel( 0 );
	pList->SetListItemWindowStartRow(0);

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventIDSelectTeleportPoint);
}

BOOL AgcmUIEventTeleport::CBCannotUseForTransforming(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventTeleport	*pThis		= (AgcmUIEventTeleport *)	pClass;

	// UI에 변신중이라 사용못한다고 날려준다.
	pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TELEPORT_TRANSFORM_CHAR));

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBCannotUseForCombatMode(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventTeleport		*pThis				= (AgcmUIEventTeleport *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TELEPORT_COMBAT_MODE));

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBReceivedTeleportStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIEventTeleport		*pThis				= (AgcmUIEventTeleport *)	pClass;
	INT32					lCID				= *(INT32 *)				pData;
	AuPOS					*pstPos				= (AuPOS *)					pCustData;

	pThis->m_bIsStartedMapLoading				= TRUE;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventLoadingWindowOpen);
}

BOOL AgcmUIEventTeleport::CBReceivedTeleportCanceled(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventTeleport		*pThis				= (AgcmUIEventTeleport *)	pClass;

	if (pThis->m_bIsStartedMapLoading)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventLoadingWindowClose);
		pThis->m_bIsStartedMapLoading			= FALSE;
	}

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBMapLoadEnd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIEventTeleport		*pThis				= (AgcmUIEventTeleport *)	pClass;

	if (pThis->m_bIsStartedMapLoading)
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventLoadingWindowClose);
		pThis->m_bIsStartedMapLoading			= FALSE;
	}

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmUIEventTeleport::CBSelfUpdatePosition");

	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventTeleport		*pThis				= (AgcmUIEventTeleport *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	if (pThis->m_bIsOpenedTeleportListWindow)
	{
		FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stTeleportOpenPos);

		if ((INT32) fDistance < AGCMUITELEPORT_CLOSE_UI_DISTANCE)
			return TRUE;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventSelectTeleportWindowClose);

		pThis->m_bIsOpenedTeleportListWindow	= FALSE;
	}

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBRequestTeleport(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIEventTeleport		*pThis			= (AgcmUIEventTeleport *)	pClass;
	AgcdUIUserData			*pcsUserData	= pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);
	AcUIButton				*pcsUIButton	= (AcUIButton *)			pcsSourceControl->m_pcsBase;
	if (!pcsUIButton) 
		return FALSE;

	if (!pThis->m_pcsEvent || pThis->m_bIsDisablePoint)
		return FALSE;

	AgpdTeleportAttach* pcsAttachData = (AgpdTeleportAttach *)pThis->m_pcsEvent->m_pvData;
	AgpdTeleportPoint* pcsTargetPoint = NULL;

	if (pcsUserData && pcsUserData->m_eType == AGCDUI_USERDATA_TYPE_TELEPORT_POINT)
		pcsTargetPoint = ((AgpdTeleportPoint **) pcsUserData->m_stUserData.m_pvData)[pcsSourceControl->m_lUserDataIndex];

	if (pcsTargetPoint)
	{
		INT32	lFee	= pThis->m_pcsAgpmEventTeleport->GetFee(pThis->m_szTeleportSelfName, pcsTargetPoint->m_szPointName, pThis->m_pcsAgcmCharacter->GetSelfCharacter());
		INT64	llMoney	= 0;

		pThis->m_pcsAgpmCharacter->GetMoney(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), &llMoney);
		if (llMoney < (INT64) lFee)
			return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventNotEnoughMoney);
	}

	if (pcsTargetPoint && pcsTargetPoint->m_eRegionType == AGPDTELEPORT_REGION_TYPE_PVP)
	{
		pThis->m_pcsPvPZonePoint	= pcsTargetPoint;
		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventConfirmPvPZone);
	}
	else
	{
		if (pThis->m_ePointSpecialType == AGPDTELEPORT_SPECIAL_TYPE_RETURN_ONLY)
			return pThis->m_pcsAgcmEventTeleport->RequestTeleportReturnOnly();
		else if (pcsTargetPoint)
			return pThis->m_pcsAgcmEventTeleport->RequestTeleport(pcsTargetPoint->m_szPointName);
	}

	return FALSE;
}

BOOL AgcmUIEventTeleport::CBRefreshList(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if ( !pClass )		return FALSE;
		
	AgcmUIEventTeleport* pThis = (AgcmUIEventTeleport*)pClass;
	pThis->RefreshTeleportList();
	return TRUE;
}

BOOL AgcmUIEventTeleport::CBDisplayTeleport(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_TELEPORT_POINT)
		return FALSE;

	AgcmUIEventTeleport		*pThis				= (AgcmUIEventTeleport *)	pClass;
	AgpdTeleportPoint		**ppcsPoint			= (AgpdTeleportPoint **)	pData;

	if (*ppcsPoint)
	{
		std::string point = RegionLocalName().GetStr( (*ppcsPoint)->m_szPointName );
		sprintf(szDisplay, "%s", point == "" ? (*ppcsPoint)->m_szPointName : point.c_str() );
	}
	else
	{
		if (pThis->m_ePointSpecialType == AGPDTELEPORT_SPECIAL_TYPE_RETURN_ONLY)
			sprintf(szDisplay, "%s", pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TELEPORT_NAME_RETURN_ONLY));
		else
			szDisplay[0] = NULL;
	}

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBDisplaySelfTeleport(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventTeleport* pThis	= (AgcmUIEventTeleport *)pClass;

	if ( pThis->m_szTeleportSelfName && pThis->m_szTeleportSelfName[0] )
	{
		std::string point = RegionLocalName().GetStr( pThis->m_szTeleportSelfName );
		sprintf( szDisplay, "%s", point == "" ? pThis->m_szTeleportSelfName : point.c_str() );
	}
	else
		szDisplay[0] = NULL;

	return TRUE;
}

BOOL AgcmUIEventTeleport::CBDisplayTeleportFee(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pData || !pClass )		return FALSE;

	*plValue = 0;

	AgcmUIEventTeleport* pThis		= (AgcmUIEventTeleport *)	pClass;
	AgpdTeleportPoint** ppcsPoint	= (AgpdTeleportPoint **)	pData;

	if( *ppcsPoint && pThis->m_szTeleportSelfName && pThis->m_szTeleportSelfName[0] )
	{
		*plValue = pThis->m_pcsAgpmEventTeleport->GetFee( pThis->m_szTeleportSelfName, (*ppcsPoint)->m_szPointName, pThis->m_pcsAgcmCharacter->GetSelfCharacter() );
		CHAR* szGheld = pThis->m_pcsAgcmUIManager2->GetUIMessage( "Money_Name" );
		sprintf( szDisplay, "%d %s", *plValue, szGheld ? szGheld : "" );
		return TRUE;
	}
	
	return FALSE;
}

BOOL AgcmUIEventTeleport::CBConfirmPvPZone(PVOID pClass, INT32 lTrueCancel, CHAR *szInputMessage)
{
	if( !pClass )		return FALSE;
	if( !lTrueCancel )	return TRUE;
		
	AgcmUIEventTeleport *pThis	= (AgcmUIEventTeleport *)	pClass;
	if ( pThis->m_pcsPvPZonePoint )
		pThis->m_pcsAgcmEventTeleport->RequestTeleport( pThis->m_pcsPvPZonePoint->m_szPointName );

	return TRUE;
}

BOOL AgcmUIEventTeleport::RefreshTeleportList()
{
	if( !m_pcsAgcmCharacter->GetSelfCharacter() || !m_szTeleportSelfName )
		return FALSE;

	INT64	llInvenMoney;
	if( m_pcsAgpmCharacter->GetMoney( m_pcsAgcmCharacter->GetSelfCharacter(), &llInvenMoney ) )
	{
		for ( INT32 lIndex = 0; lIndex < m_lTargetPoint; ++lIndex )
		{
			if( m_bIsDisablePoint || IsDisablePoint(m_apcsTargetPoint[lIndex]))
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventDisplayDisable, lIndex);
			else if( !m_apcsTargetPoint[lIndex] || llInvenMoney >= m_pcsAgpmEventTeleport->GetFee(m_szTeleportSelfName, m_apcsTargetPoint[lIndex]->m_szPointName, m_pcsAgcmCharacter->GetSelfCharacter()) )
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventDisplayNormal, lIndex);
			else
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventDisplayNoMoney, lIndex);
		}
	}

	return TRUE;
}

BOOL AgcmUIEventTeleport::IsDisablePoint(AgpdTeleportPoint *pcsTeleportPoint)
{
	if( !pcsTeleportPoint )		return FALSE;
		
	for (int i = 0; i < AGCMUI_TELEPORT_MAX_TARGET_POINT; ++i)
	{
		if( !m_lDisableTargetPoint[i] )		break;
		if ( m_lDisableTargetPoint[i] == pcsTeleportPoint->m_lID )
			return TRUE;
	}

	return FALSE;
}