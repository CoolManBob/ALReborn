#include "AgcmUIEventNPCDialog.h"
#include "AgcmTargeting.h"
#include "AgcmSound.h"

#include "AuStrTable.h"

void AgcmUIEventNPCDialogButton::Clear()
{
	for( int i=0; i<AGCMUI_EVENT_MAX_NPCDIALOG; ++i )
	{
		m_csButton[i].Reset();
	}
}

INT32 AgcmUIEventNPCDialogButton::SetData(ApdEventFunction eFunctionType, char *pstrDisplay, bool bExit )
{
	INT32			lIndex;

	lIndex = -1;

	for( int i=0; i<AGCMUI_EVENT_MAX_NPCDIALOG; ++i )
	{
		if( m_csButton[i].m_eFunctionType == APDEVENT_FUNCTION_NONE )
		{
			lIndex = i;
			break;
		}
	}

	if( lIndex != -1 )
	{
		m_csButton[lIndex].m_eFunctionType = eFunctionType;
		strcpy( m_csButton[lIndex].m_strDisplayString, pstrDisplay );
		m_csButton[lIndex].m_bExit = bExit;
	}

	return lIndex;
}

AgcdUIEventNPCDialogButtonInfo *AgcmUIEventNPCDialogButton::GetData( INT32 lIndex )
{
	return &m_csButton[lIndex];
}

AgcmUIEventNPCDialog::AgcmUIEventNPCDialog()
{
	SetModuleName("AgcmUIEventDialog");

	m_pcsApBase				= NULL;
	m_pcsGenerator			= NULL;

	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmItem			= NULL;

	m_pcsAgcmCharacter		= NULL;
	m_pcsAgcmItem			= NULL;
	m_pcsAgcmEventNPCDialog		= NULL;
	m_pcsAgcmUIManager2		= NULL;

	m_lEventOpenNPCDialogUI = 0;
	m_lEventCloseNPCDialogUI = 0;

	ZeroMemory(m_apcsDialog, sizeof(AgcdUIEventNPCDialogButtonInfo *) * AGCMUI_EVENT_MAX_NPCDIALOG);

	ZeroMemory(&m_stEventPos, sizeof(AuPOS));

	m_bIsNPCDialogUIOpen	= FALSE;
	m_bNoNeedCloseSound		= FALSE;

	m_lLastOpenDialogNPCID	= AP_INVALID_CID;
}

AgcmUIEventNPCDialog::~AgcmUIEventNPCDialog()
{
}

BOOL AgcmUIEventNPCDialog::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmEventNPCDialog = (AgpmEventNPCDialog *)GetModule("AgpmEventNPCDialog" );
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgcmItem			= (AgcmItem *)			GetModule("AgcmItem");
	m_pcsAgcmEventManager	= (AgcmEventManager *)  GetModule("AgcmEventManager");
	m_pcsAgcmEventNPCDialog		= (AgcmEventNPCDialog *)		GetModule("AgcmEventNPCDialog");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmEventNPCDialog ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmItem ||
		!m_pcsAgcmEventManager ||
		!m_pcsAgcmEventNPCDialog ||
		!m_pcsAgcmUIManager2)
		return FALSE;

	if (!m_pcsAgcmEventNPCDialog->SetCallbackAriseNPCDialogEvent(CBAriseNPCDialogUI, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(CBSelfUpdatePosition, this))
		return FALSE;

	if (!AddEvent())
		return FALSE;
	if (!AddFunction())
		return FALSE;
	if (!AddDisplay())
		return FALSE;
	if (!AddUserData())
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::AddEvent()
{
	m_lEventOpenNPCDialogUI = m_pcsAgcmUIManager2->AddEvent("NPCDialog_OpenUI");
	if (m_lEventOpenNPCDialogUI < 0)
		return FALSE;

	m_lEventCloseNPCDialogUI = m_pcsAgcmUIManager2->AddEvent("NPCDialog_CloseUI");
	if (m_lEventCloseNPCDialogUI < 0)
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCDialog_DisplayDialog", CBNPCDialogDisplayDialog, 1 ))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "NPCDialog_SelectButton", CBNPCDailogSelectButton, 1))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::AddDisplay()
{
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "NPCDialog_Button", 0, CBDisplayNPCDialog, AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::AddUserData()
{
	m_pstDialogUD = m_pcsAgcmUIManager2->AddUserData("NPCDialog_Buttons", m_apcsDialog, sizeof(AgcdUIEventNPCDialogButtonInfo *), AGCMUI_EVENT_MAX_NPCDIALOG, AGCDUI_USERDATA_TYPE_STRING );

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::CBAriseNPCDialogUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventNPCDialog *pThis	= (AgcmUIEventNPCDialog *)	pClass;
	ApdEvent			*pcsEvent = (ApdEvent *)pData;
	ApBase				*pcsGenerator = (ApBase *)pCustData;

	// 마고자 (2005-05-27 오전 11:40:29) : 
	// 종료시 음성출력 코드..
	pThis->m_bNoNeedCloseSound = FALSE;

	//pThis는 당연히 있겠지?
	if( pThis && pcsEvent )
	{
		ApdEventAttachData	*pcsAttachData = pThis->m_pcsApmEventManager->GetEventData(pcsEvent->m_pcsSource);

		if (pcsAttachData)
		{
			INT32			lIndex;
			INT32			lDisplayButtonCount;

			lDisplayButtonCount = 0;
			pThis->m_pcsApBase = pcsEvent->m_pcsSource;
			pThis->m_pcsGenerator = pcsGenerator;
			pThis->m_csDialogButtons.Clear();

			//해당 오브젝트가 가지고있는 이벤트를 쭈욱~ 본다.
			for (int i = 0; i < pcsAttachData->m_unFunction; ++i)
			{
				//뭔가 이벤트를 가지고 있는경우.
				if (pcsAttachData->m_astEvent[i].m_eFunction > APDEVENT_FUNCTION_NONE && pcsAttachData->m_astEvent[i].m_eFunction < APDEVENT_MAX_FUNCTION)
				{
					//NPC대사 이벤트는 뺀다.
					//					if( pcsAttachData->m_astEvent[i].m_eFunction != APDEVENT_FUNCTION_NPCDAILOG &&
					//						pcsAttachData->m_astEvent[i].m_eFunction != APDEVENT_FUNCTION_AUCTION)
					if( pcsAttachData->m_astEvent[i].m_eFunction != APDEVENT_FUNCTION_NPCDAILOG )
					{
						char			strDisplayData[80];

						sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_CANT_USE) );

						if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_NPCTRADE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_BUY_SELL) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_ITEM_REPAIR )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_REPAIR) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_BANK )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_WAREHOUSE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_ITEMCONVERT )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_ENHANCE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_GUILD )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_GUILD) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_PRODUCT )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_PRODUCE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_TELEPORT )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_TELEPORT) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_QUEST )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_QUEST) );
						}						
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_SKILLMASTER )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_SKILL) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_AUCTION )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_AUCTION) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_REFINERY )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_REFINE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_CHAR_CUSTOMIZE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_CUSTOMIZE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_REMISSION )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_FORGIVE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_WANTEDCRIMINAL )
						{
							sprintf( strDisplayData, "%s",  ClientStr().GetStr(STI_WANTED));
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_SIEGEWAR_NPC )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_SIEGEWAR_NPC));
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_TAX )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_TAX));
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_GUILD_WAREHOUSE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_GUILD_WAREHOUSE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_ARCHLORD )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_EVENT_ARCHLORD) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_GAMBLE )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_EVENT_GAMBLE) );
						}
						else if( pcsAttachData->m_astEvent[i].m_eFunction == APDEVENT_FUNCTION_WORLD_CHAMPIONSHIP )
						{
							sprintf( strDisplayData, "%s", ClientStr().GetStr(STI_EVENT_WORLD_CHAMPIONSHIP) );
						}

						//NPC대사 이벤트외의 이벤트들을 보여준다.
						lIndex = pThis->m_csDialogButtons.SetData( pcsAttachData->m_astEvent[i].m_eFunction, strDisplayData );

						if( lIndex != -1 )
						{
							pThis->m_apcsDialog[lIndex] = pThis->m_csDialogButtons.GetData(lIndex);
							lDisplayButtonCount++;
						}
					}
				}
			}

			//마지막에 Exit를 붙여준다.
			lIndex = pThis->m_csDialogButtons.SetData( APDEVENT_MAX_FUNCTION, "Exit", true );
			if( lIndex != -1 )
			{
				pThis->m_apcsDialog[lIndex] = pThis->m_csDialogButtons.GetData(lIndex);
				++lDisplayButtonCount;
			}

			pThis->m_pstDialogUD->m_stUserData.m_lCount = lDisplayButtonCount;

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstDialogUD);

			pThis->m_lNPCDailogTextID = ((AgpdEventNPCDialogData *)(pcsEvent->m_pvData))->m_lNPCDialogTextID;

			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenNPCDialogUI , 0 , FALSE , pcsEvent->m_pcsSource );

			if (pcsGenerator)
				pThis->m_stEventPos	= ((AgpdCharacter *) pcsGenerator)->m_stPos;

			// 마고자 (2005-05-17 오후 7:07:33) : 
			// 사운드 출력
			if (!pThis->m_bIsNPCDialogUIOpen)
				pThis->PlayNPCSound( NS_WELCOME , pcsEvent->m_pcsSource );

			pThis->m_bIsNPCDialogUIOpen	= TRUE;

			pThis->m_lLastOpenDialogNPCID	= pThis->m_pcsApBase->m_lID;
		}
	}

	return TRUE;
}

void	AgcmUIEventNPCDialog::PlayNPCSound( NS_TYPE eType , ApBase * pcsOwnerBase )
{
	if ( g_eServiceArea == AP_SERVICE_AREA_CHINA || g_eServiceArea == AP_SERVICE_AREA_JAPAN )
		return;		// 중국/일본은 NPC 음성을 안들려준다

	AgcmTargeting* pcsAgcmTargeting = ( AgcmTargeting* ) GetModule( "AgcmTargeting"	);
	AgcmSound* pcsAgcmSound = ( AgcmSound* ) GetModule( "AgcmSound" );
	ASSERT( pcsAgcmTargeting	);
	ASSERT( pcsAgcmSound		);
	if( !pcsAgcmTargeting	) return;
	if( !pcsAgcmSound		) return;

	AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacter( pcsOwnerBase ? pcsOwnerBase->m_lID : ( eType == NS_FAREWELL ? m_lLastOpenDialogNPCID : pcsAgcmTargeting->GetLastNPCID() ) );
	if( !pcsCharacter )		return;

	// 마지막 플레이한정보를 저장함
	static	INT32	_snCID		= -1;
	static	INT32	_snType		= -1;
	static	UINT32	_suEndtime	= GetTickCount();
	if( pcsCharacter->m_pcsCharacterTemplate->m_lID	== _snCID && _snType == eType && _suEndtime > GetTickCount() )
		return;

	char	strFileName[ 1024 ];
	if( pcsCharacter->m_pcsCharacterTemplate->m_lID < 1000 )	//4자리수 미만
	{
		sprintf( strFileName, "Sound\\Effect\\NP%03d%d.wav", pcsCharacter->m_pcsCharacterTemplate->m_lID, (int)eType );
	}
	else
	{
		char szID[16];
		itoa( pcsCharacter->m_pcsCharacterTemplate->m_lID, szID, 10 );
		sprintf( strFileName, "Sound\\Effect\\NP%s%d.wav", szID, (int)eType );
	}

	pcsAgcmSound->PlaySampleSound( strFileName );

	_snCID		= pcsCharacter->m_pcsCharacterTemplate->m_lID;
	_snType		= eType;
	_suEndtime	= GetTickCount() + pcsAgcmSound->GetSampleSoundLength( SOUND_TYPE_3D_SOUND, strFileName );
}


BOOL AgcmUIEventNPCDialog::CBNPCDialogDisplayDialog(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIEventNPCDialog *pThis = (AgcmUIEventNPCDialog *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;
	BOOL				bResult;

	bResult = FALSE;

	if( pcsControl != NULL )
	{
		AgpdEventNPCDialogTemplate *pcsNPCDialogTemplate;

		char			*pstrNPCText;

		if (pcsControl->m_lType != AcUIBase::TYPE_EDIT || !pcsControl->m_pcsBase)
			return FALSE;

		pcsNPCDialogTemplate = pThis->m_pcsAgpmEventNPCDialog->GetNPCDialogTemplate( pThis->m_lNPCDailogTextID );

		if( pcsNPCDialogTemplate )
		{
			INT32			lDialogIndex;
			
			srand((unsigned int) time(NULL));

			lDialogIndex = rand()%pcsNPCDialogTemplate->m_iDialogTextNum;

			pstrNPCText = pcsNPCDialogTemplate->m_ppstrDialogText[lDialogIndex];

			if( pstrNPCText )
			{
				((AcUIEdit *)pcsControl->m_pcsBase)->SetLineDelimiter( "\\n" );
				bResult = ((AcUIEdit *)pcsControl->m_pcsBase)->SetText( pstrNPCText );
			}
		}
	}

	return bResult;
}

BOOL AgcmUIEventNPCDialog::CBDisplayNPCDialog(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING)
		return FALSE;
	AgcdUIEventNPCDialogButtonInfo		**ppcsButton;

	ppcsButton = (AgcdUIEventNPCDialogButtonInfo **)pData;

	if( (*ppcsButton) == NULL )
		return FALSE;

	AgcmUIEventNPCDialog	*pThis	= (AgcmUIEventNPCDialog *) pClass;

	sprintf(szDisplay, "%s", (*ppcsButton)->m_strDisplayString );

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::CBNPCDailogSelectButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIEventNPCDialog	*pThis			= (AgcmUIEventNPCDialog *)	pClass;

	AcUIButton				*pcsUIButton	= (AcUIButton *)			pcsSourceControl->m_pcsBase;
	if (!pcsUIButton)
		return FALSE;

	AgcdUIEventNPCDialogButtonInfo	*pcsButtonInfo;

	pcsButtonInfo = pThis->m_csDialogButtons.GetData(pcsSourceControl->m_lUserDataIndex);

	if( pcsButtonInfo )
	{
		//만약 종료 버튼이면~
		if( pcsButtonInfo->m_bExit )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseNPCDialogUI );

			pThis->m_bIsNPCDialogUIOpen	= FALSE;
		}
		else
		{
			// 마고자 (2005-05-27 오전 11:40:29) : 
			// 종료시 음성출력 코드..
			pThis->m_bNoNeedCloseSound = TRUE;

			//현재는 대화창이 닫히고 다른 UI가 열린다.
			//차후 Function ID에 따라 대화 UI가 닫힐수도 열릴수도 있다.
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseNPCDialogUI );

			pThis->m_bIsNPCDialogUIOpen	= FALSE;

			pThis->m_pcsAgcmEventManager->CheckEvent( pThis->m_pcsApBase, pThis->m_pcsGenerator, pcsButtonInfo->m_eFunctionType );
		}
	}

	return TRUE;
}

BOOL AgcmUIEventNPCDialog::CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIEventNPCDialog	*pThis				= (AgcmUIEventNPCDialog *)		pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)				pData;

	if (!pThis->m_bIsNPCDialogUIOpen)
		return TRUE;

	FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stEventPos);

	if ((INT32) fDistance < AGCMUIEVENTNPCDIALOG_CLOSE_UI_DISTANCE)
		return TRUE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseNPCDialogUI);

	pThis->m_bIsNPCDialogUIOpen	= FALSE;

	return TRUE;
}

void AgcmUIEventNPCDialog::CloseNPCDialog()
{
	if (!m_pcsApBase || !m_bIsNPCDialogUIOpen)
		return;

	PlayNPCSound( NS_FAREWELL , m_pcsApBase );
	m_bIsNPCDialogUIOpen	= FALSE;
}
