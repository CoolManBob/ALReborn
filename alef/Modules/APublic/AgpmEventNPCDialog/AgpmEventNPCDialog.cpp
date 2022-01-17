#include "AgpmEventNPCDialog.h"
#include "ApMemoryTracker.h"
#include "ApModuleStream.h"
#include <AgppEventNPCDialog.h>

AgpmEventNPCDialog::AgpmEventNPCDialog()
{
	SetModuleName("AgpmEventNPCDialog");
	
	m_pcsApmEventManager	= NULL;
	m_pcsAgpmCharacter		= NULL;

	SetPacketType(AGPMEVENT_NPCDIALOG_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,				// operation
							AUTYPE_PACKET,		1,				// event base packet
							AUTYPE_INT32,		1,				// CID
							AUTYPE_END,			0
							);

	m_csPacketEventData.SetFlagLength(sizeof(INT8));
	m_csPacketEventData.SetFieldType(
							AUTYPE_INT32,		1,				// NPCDialog TID;
							AUTYPE_END,			0
							);							
}

AgpmEventNPCDialog::~AgpmEventNPCDialog()
{

}

BOOL AgpmEventNPCDialog::OnAddModule()
{
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");

	if( !m_pcsApmEventManager || !m_pcsAgpmCharacter )
	{
		return FALSE;
	}

	if( !m_pcsAgpmCharacter->SetCallbackActionEventNPCDialog(CBActionNPCDialog, this))
		return FALSE;

	if( !m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_NPCDAILOG, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
	{
		return FALSE;
	}

	if( !m_pcsApmEventManager->RegisterPacketFunction( CBMakePacketEventData, CBParsePacketEventData, this, APDEVENT_FUNCTION_NPCDAILOG ) )
	{
		return FALSE;
	}

	if( !m_csNPCDialogTemplate.InitializeObject( sizeof(AgpdEventNPCDialogTemplate *), AGPMEVENT_NPCTDIALOG_MAX_TEMPLATE ) )
	{
		return FALSE;
	}

	if( !m_csMobDialogKeyTemplate.InitializeObject( sizeof(AgpdEventMobDialogKeyTemplate *), AGPMEVENT_NPCTDIALOG_MAX_TEMPLATE ) )
	{
		return FALSE;
	}

	if( !m_csMobDialogTemplate.InitializeObject( sizeof(AgpdEventMobDialogData *), AGPMEVENT_NPCTDIALOG_MAX_TEMPLATE ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL	AgpmEventNPCDialog::OnDestroy()
{
	//템플릿을 삭제한다.
	AgpdEventNPCDialogTemplate	**ppcsTemplate = NULL;

	INT32			lIndex = 0;

	// 등록된 모든 NPCTrade Template에 대해서...
	for( ppcsTemplate = (AgpdEventNPCDialogTemplate **) m_csNPCDialogTemplate.GetObjectSequence(&lIndex); ppcsTemplate; ppcsTemplate = (AgpdEventNPCDialogTemplate **) m_csNPCDialogTemplate.GetObjectSequence(&lIndex))
	{
		if( (*ppcsTemplate) != NULL )
		{
			for( int i=0; i<(*ppcsTemplate)->m_iDialogTextNum; i++ )
			{
				delete [] (*ppcsTemplate)->m_ppstrDialogText[i];
			}

			delete [] (*ppcsTemplate)->m_ppstrDialogText;

			delete (*ppcsTemplate);
		}
	}

	m_csNPCDialogTemplate.RemoveObjectAll();

	lIndex	= 0;

	for( AgpdEventMobDialogTemplate **ppcsMobTemplate = (AgpdEventMobDialogTemplate **) m_csMobDialogTemplate.GetObjectSequence(&lIndex); ppcsMobTemplate; ppcsMobTemplate = (AgpdEventMobDialogTemplate **) m_csMobDialogTemplate.GetObjectSequence(&lIndex))
	{
		if( (*ppcsMobTemplate) != NULL )
		{
			for (int j = 0; j < AGPD_NPCDIALOG_MOB_MAX; ++j)
			{
				for( int i=0; i<(*ppcsMobTemplate)->m_csEventMobDialog[j].m_iDialogTextNum; i++ )
				{
					delete [] (*ppcsMobTemplate)->m_csEventMobDialog[j].m_ppstrDialogText[i];
				}

				delete [] (*ppcsMobTemplate)->m_csEventMobDialog[j].m_ppstrDialogText;
			}

			delete (*ppcsMobTemplate);
		}
	}

	m_csMobDialogTemplate.RemoveObjectAll();

	AgpdEventMobDialogKeyTemplate	**ppcsKeyTemplate	= NULL;

	lIndex	= 0;

	for( ppcsKeyTemplate = (AgpdEventMobDialogKeyTemplate **) m_csMobDialogKeyTemplate.GetObjectSequence(&lIndex); ppcsKeyTemplate; ppcsKeyTemplate = (AgpdEventMobDialogKeyTemplate **) m_csMobDialogKeyTemplate.GetObjectSequence(&lIndex))
	{
		if( (*ppcsKeyTemplate) != NULL )
		{
			delete (*ppcsKeyTemplate);
		}
	}

	m_csMobDialogKeyTemplate.RemoveObjectAll();

	return TRUE;
}

BOOL	AgpmEventNPCDialog::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCDialog		*pThis;
	ApdEvent				*pcsNPCDialog;
	BOOL			bResult;

	pThis = (AgpmEventNPCDialog *)pClass;
		
	bResult = FALSE;

	if( pThis != NULL )
	{
		pcsNPCDialog = (ApdEvent	*)pData;			//실제 데이터를 받을 Object

		pcsNPCDialog->m_pvData = new AgpdEventNPCDialogData;

		bResult = TRUE;
	}

	return bResult;
}

BOOL	AgpmEventNPCDialog::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCDialog		*pThis;
	ApdEvent				*pcsNPCDialog;
	BOOL			bResult;
		
	bResult = FALSE;

	pThis = (AgpmEventNPCDialog *)pClass;

	if( pThis != NULL )
	{
		pcsNPCDialog = (ApdEvent	*)pData;			//실제 데이터를 받을 Object

		if( (AgpdEventNPCDialogData *)pcsNPCDialog->m_pvData != NULL )
		{
			delete pcsNPCDialog->m_pvData;
		}
	}

	return TRUE;
}

BOOL	AgpmEventNPCDialog::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCDialog *	pThis		= (AgpmEventNPCDialog *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdEventNPCDialogData*	pstNPCDialog	= (AgpdEventNPCDialogData *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;

	//시작~
	if (!pstStream->WriteValue(AGPMEVENT_NPCDIALOG_INI_NAME_START, 0))
		return FALSE;

	//템플릿ID 쓰기.
	if (!pstStream->WriteValue(AGPMEVENT_NPCDIALOG_INI_TEMPLATE, pstNPCDialog->m_lNPCDialogTextID ))
		return FALSE;

	//끝~
	if (!pstStream->WriteValue(AGPMEVENT_NPCDIALOG_INI_NAME_END, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventNPCDialog::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCDialog *	pThis		= (AgpmEventNPCDialog *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdEventNPCDialogData*	pstNPCDialog	= (AgpdEventNPCDialogData *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;
	const CHAR *			szValueName;

	if (!pstStream->ReadNextValue())
		return TRUE;

	szValueName = pstStream->GetValueName();
	if (strcmp(szValueName, AGPMEVENT_NPCDIALOG_INI_NAME_START))
		return TRUE;

	while (pstStream->ReadNextValue())
	{
		szValueName = pstStream->GetValueName();

		if( !strcmp(szValueName, AGPMEVENT_NPCDIALOG_INI_TEMPLATE) )
		{
			//몇번 NPC TID인지를 본다.
			pstStream->GetValue( &pstNPCDialog->m_lNPCDialogTextID );
		}
		else if(!strcmp(szValueName, AGPMEVENT_NPCDIALOG_INI_NAME_END))
		{
			break;
		}
	}

	return TRUE;
}

PVOID AgpmEventNPCDialog::MakePacketEventData(ApdEvent *pcsEvent)
{
	if (!pcsEvent || !pcsEvent->m_pvData)
		return NULL;

	AgpdEventNPCDialogData	*pcsAttachData	= (AgpdEventNPCDialogData *)	pcsEvent->m_pvData;

	INT32				lNPCDialogTID;

	lNPCDialogTID = pcsAttachData->m_lNPCDialogTextID;

	return m_csPacketEventData.MakePacket(FALSE, NULL, 0,
											&lNPCDialogTID);
}

BOOL AgpmEventNPCDialog::ParseEventDataPacket(ApdEvent *pcsEvent, PVOID pvPacketCustomData)
{
	if (!pcsEvent || !pcsEvent->m_pvData || !pvPacketCustomData)
		return FALSE;

	INT32				lNPCDialogTID;

	lNPCDialogTID = 0;
	
	m_csPacketEventData.GetField(FALSE, pvPacketCustomData, 0,
											&lNPCDialogTID 
											);

	AgpdEventNPCDialogData *pcsAttachData	= (AgpdEventNPCDialogData *)pcsEvent->m_pvData;

	if( pcsAttachData )
		pcsAttachData->m_lNPCDialogTextID	= lNPCDialogTID;

	return TRUE;
}

BOOL AgpmEventNPCDialog::CBMakePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventNPCDialog		*pThis					= (AgpmEventNPCDialog *)	pClass;
	ApdEvent				*pcsEvent				= (ApdEvent *)				pData;
	PVOID					*ppvPacketCustomData	= (PVOID *)					pCustData;

	*ppvPacketCustomData	= pThis->MakePacketEventData(pcsEvent);

	return TRUE;
}

BOOL AgpmEventNPCDialog::CBParsePacketEventData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventNPCDialog		*pThis					= (AgpmEventNPCDialog *)	pClass;
	ApdEvent				*pcsEvent				= (ApdEvent *)				pData;
	PVOID					pvPacketCustomData		= (PVOID)					pCustData;

	return pThis->ParseEventDataPacket(pcsEvent, pvPacketCustomData);
}

BOOL AgpmEventNPCDialog::LoadMobDialogRes( char *pstrFileName , BOOL bDecryption )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName , TRUE , bDecryption ) )
	{
		INT32				lMaxRow, lMaxColumn;
		INT32				lStartRow, lEndRow;
		INT32				lTemplateCount;
		INT32				lReadTemplateCount;
		char				*pstrTempBuffer;

		lTemplateCount = 0;
		lReadTemplateCount = 0;

		//0번째줄은 컬럼네임이 있고 1번 row부터 데이터가 들어간다.
		lStartRow = 1;
		lEndRow = 0;

		lMaxRow = m_csExcelTxtLib.GetRow();
		lMaxColumn = m_csExcelTxtLib.GetColumn();

		//실제 갯수를 세어둔다.
		for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
		{
			pstrTempBuffer = m_csExcelTxtLib.GetData( 0, lRow );

			if( pstrTempBuffer )
			{
				if( !strcmp( pstrTempBuffer, "-" ) )
				{
					lTemplateCount++;
				}
			}
		}

		while( 1 )
		{
			if( lReadTemplateCount >= lTemplateCount )
				break;

			for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
			{
				pstrTempBuffer = m_csExcelTxtLib.GetData( 0, lRow );

				if( pstrTempBuffer )
				{
					if( !strcmp( pstrTempBuffer, "-" ) )
					{
						lEndRow = lRow;
						break;
					}
				}
			}

			char			*pstrData;
			char			*pstrData2;

			INT32			lRowCounter;
			INT32			lDialogRows;
			INT32			lMobDialogTID;

			lDialogRows = lEndRow - lStartRow;

			pstrData = m_csExcelTxtLib.GetData( 0, lStartRow );
			pstrData2 = m_csExcelTxtLib.GetData( 1, lStartRow );

			if( pstrData && pstrData2  )
			{
				lMobDialogTID = atoi( pstrData );

				//등록된 ID인지본다.
				if( GetMobDialogTemplate( lMobDialogTID ) )
				{
				}
				//등록되지 않았다면 추가한다.
				else
				{
					AgpdEventMobDialogTemplate	*pcsTemplate;

					pcsTemplate = new AgpdEventMobDialogTemplate;

					//TID복사
					pcsTemplate->m_lMobDialogTID = lMobDialogTID;
					//템플릿 이름복사.
					memset( pcsTemplate->m_strTemplateName, 0, sizeof(pcsTemplate->m_strTemplateName) );
					strcat( pcsTemplate->m_strTemplateName, pstrData2 );
					ASSERT(strlen(pcsTemplate->m_strTemplateName) < sizeof(pcsTemplate->m_strTemplateName));

					//대사를 읊을 몹ID 읽어들이기.
					for( lRowCounter=lStartRow; lRowCounter<lEndRow; lRowCounter++ )
					{
						pstrData = m_csExcelTxtLib.GetData( 2, lRowCounter );

						if( pstrData )
						{
							AgpdEventMobDialogKeyTemplate	*pcsKeyTemplate;
							
							pcsKeyTemplate = new AgpdEventMobDialogKeyTemplate;

							pcsKeyTemplate->m_lDialogID = lMobDialogTID;
							pcsKeyTemplate->m_lMobTID = atoi( pstrData );

							//몹 ID를 넣으면 Dialog TID를 뱉어낸다.
							if (!m_csMobDialogKeyTemplate.AddObject((void *)&pcsKeyTemplate, pcsKeyTemplate->m_lMobTID ))
							{
								//ASSERT(!"AgpmEventNPCDialog::MobDialogKeyTemplate 추가 실패");
								delete pcsKeyTemplate;
							}
						}
						else
						{
							break;
						}
					}

					for( int lDialogType=0; lDialogType<AGPD_NPCDIALOG_MOB_MAX; lDialogType++ )
					{
						char				**ppstrDialogText;

						INT32				lMaxRows;
						float				fProbable;
						
						lMaxRows = 0;
						fProbable = 0.0f;

						//초기화!
						pcsTemplate->m_csEventMobDialog[lDialogType].m_fProbable = fProbable;
						pcsTemplate->m_csEventMobDialog[lDialogType].m_iDialogTextNum = lMaxRows;
						pcsTemplate->m_csEventMobDialog[lDialogType].m_ppstrDialogText = NULL;

						//대사가 몇개인지 확인해본다.
						for( lRowCounter =lStartRow; lRowCounter<lEndRow; lRowCounter++ )
						{
							pstrData = m_csExcelTxtLib.GetData( 4+lDialogType*2, lRowCounter );

							if( pstrData )
							{
								lMaxRows++;
							}
							else
							{
								break;
							}
						}

						if( lMaxRows == 0 )
							continue;

						//메모리를 세팅하자!
						ppstrDialogText = new char*[lMaxRows];

						for( int iDialogTextRow=0; iDialogTextRow<lMaxRows; iDialogTextRow++ )
						{
							ppstrDialogText[iDialogTextRow] = new char [AGPMEVENT_NPCDIALOG_TEXT_MAX_SIZE];
						}

						//대사를 읽어들인다.
						for( lRowCounter =0; lRowCounter<lMaxRows; lRowCounter++ )
						{
							pstrData = m_csExcelTxtLib.GetData( 4+lDialogType*2, lStartRow+lRowCounter );

							//대사를 복사한다.
							if( pstrData )
							{
								memset( ppstrDialogText[lRowCounter], 0, AGPMEVENT_NPCDIALOG_TEXT_MAX_SIZE );
								strcat( ppstrDialogText[lRowCounter], pstrData );
								ASSERT(strlen(ppstrDialogText[lRowCounter]) < AGPMEVENT_NPCDIALOG_TEXT_MAX_SIZE);
							}
						}

						//대사에따른 확률을 복사한다.
						pstrData = m_csExcelTxtLib.GetData( 4+lDialogType*2+1, lStartRow );

						if( pstrData )
						{
							fProbable = (float)atof( pstrData );
						}

						//대사를 템플릿에 세팅한다.
						pcsTemplate->m_csEventMobDialog[lDialogType].m_iDialogTextNum = lMaxRows;
						pcsTemplate->m_csEventMobDialog[lDialogType].m_ppstrDialogText = ppstrDialogText;
						pcsTemplate->m_csEventMobDialog[lDialogType].m_fProbable = fProbable;
					}

					if (!m_csMobDialogTemplate.AddObject( (void *)&pcsTemplate, pcsTemplate->m_lMobDialogTID ))
						ASSERT(!"AgpmEventNPCDialog::Template 추가 실패");
				}
			}

			lReadTemplateCount++;

			lStartRow = lEndRow+1;

			if( lStartRow >=lMaxRow )
			{
				break;
			}
		}

		bResult = TRUE;

		//열었으니 닫는다.
		m_csExcelTxtLib.CloseFile();
	}

	return bResult;
}

BOOL AgpmEventNPCDialog::LoadNPCDialogRes( char *pstrFileName , BOOL bDecryption )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, TRUE, bDecryption ) )
	{
		INT32				lMaxRow, lMaxColumn;
		INT32				lStartRow, lEndRow;
		INT32				lTemplateCount;
		INT32				lReadTemplateCount;
		char				*pstrTempBuffer;

		lTemplateCount = 0;
		lReadTemplateCount = 0;

		//0번째줄은 컬럼네임이 있고 1번 row부터 데이터가 들어간다.
		lStartRow = 1;
		lEndRow = 0;

		lMaxRow = m_csExcelTxtLib.GetRow();
		lMaxColumn = m_csExcelTxtLib.GetColumn();

		//실제 갯수를 세어둔다.
		for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
		{
			pstrTempBuffer = m_csExcelTxtLib.GetData( 0, lRow );

			if( pstrTempBuffer )
			{
				if( !strcmp( pstrTempBuffer, "-" ) )
				{
					lTemplateCount++;
				}
			}
		}

		while( 1 )
		{
			if( lReadTemplateCount >= lTemplateCount )
				break;

			lEndRow = lMaxRow;

			for( INT32 lRow=lStartRow; lRow<lMaxRow; lRow++ )
			{
				pstrTempBuffer = m_csExcelTxtLib.GetData( 0, lRow );

				if( pstrTempBuffer )
				{
					if( !strcmp( pstrTempBuffer, "-" ) )
					{
						lEndRow = lRow;
						break;
					}
				}
			}

			char			**ppstrDialogText;
			char			*pstrData;
			char			*pstrData2;
			char			*pstrData3;
			char			*pstrData4;

			INT32			lRowCounter;
			INT32			lDialogRows;
			INT32			lNPCDialogTID;

			lDialogRows = lEndRow - lStartRow;

			pstrData = m_csExcelTxtLib.GetData( 0, lStartRow );
			pstrData2 = m_csExcelTxtLib.GetData( 1, lStartRow );
			pstrData3 = m_csExcelTxtLib.GetData( 2, lStartRow );
			pstrData4 = m_csExcelTxtLib.GetData( 3, lStartRow );

			if( pstrData  )
			{
				lNPCDialogTID = atoi( pstrData );

				if( GetNPCDialogTemplate( lNPCDialogTID ) )
				{
					ASSERT(!"AgpmEventNPCDialog::LoadNPCDialogRes() Already Exist !!!");
				}
				else
				{
					AgpdEventNPCDialogTemplate	*pcsTemplate;

					pcsTemplate = new AgpdEventNPCDialogTemplate;

					pcsTemplate->m_lNPCDialogTID = lNPCDialogTID;
					pcsTemplate->m_iDialogTextNum = lDialogRows;

					ppstrDialogText = new char*[lDialogRows];

					//대사를 읽는다.
					for( lRowCounter=0; lRowCounter<lDialogRows; lRowCounter++ )
					{
						pstrData = m_csExcelTxtLib.GetData( 5, lStartRow + lRowCounter );

						if( pstrData )
						{
							size_t dlgSize = strnlen(pstrData, AGPMEVENT_NPCDIALOG_TEXT_MAX_SIZE);
							ppstrDialogText[lRowCounter] = new char [dlgSize];
							//ppstrDialogText[lRowCounter] = new char [AGPMEVENT_NPCDIALOG_TEXT_MAX_SIZE];

							memset( ppstrDialogText[lRowCounter], 0, dlgSize );
							strncpy_s( ppstrDialogText[lRowCounter], dlgSize, pstrData, _TRUNCATE );
							//strcat( ppstrDialogText[lRowCounter], pstrData );
							//ASSERT(strlen(ppstrDialogText[lRowCounter]) < AGPMEVENT_NPCDIALOG_TEXT_MAX_SIZE);
						}
						else
						{
							break;
						}
					}

					pcsTemplate->m_iDialogTextNum = lRowCounter;
					pcsTemplate->m_ppstrDialogText = ppstrDialogText;

					VERIFY(m_csNPCDialogTemplate.AddObject( (void *)&pcsTemplate, pcsTemplate->m_lNPCDialogTID ));
				}
			}

			lReadTemplateCount++;

			lStartRow = lEndRow+1;

			if( lStartRow >=lMaxRow )
			{
				break;
			}
		}

		bResult = TRUE;

		//열었으니 닫는다.
		m_csExcelTxtLib.CloseFile();
	}

	return bResult;
}

ApAdmin *AgpmEventNPCDialog::GetNPCTemplate()
{
	return &m_csNPCDialogTemplate;
}

ApAdmin *AgpmEventNPCDialog::GetMobTemplate()
{
	return &m_csMobDialogTemplate;
}

char *AgpmEventNPCDialog::GetMobDialog( INT32 lMobTID, eAgpdEventNPCDialogMob lDialogType )
{
	// 일본의 경우 monster dialog를 임시로 막는다.
	if ( g_eServiceArea == AP_SERVICE_AREA_JAPAN ) 
		return NULL;

	AgpdEventMobDialogKeyTemplate *pcsKeyTemplate;
	char			*pstrMobDialog;

	pstrMobDialog = NULL;

	pcsKeyTemplate = GetMobKeyDialogTemplate( lMobTID );

	if( pcsKeyTemplate )
	{
		AgpdEventMobDialogTemplate		*pcsTemplate;

		pcsTemplate = GetMobDialogTemplate( pcsKeyTemplate->m_lDialogID );

		if( pcsTemplate )
		{
			//대사를 말할 확률이 있는지본다.
			INT32					lProbable;

			lProbable = (INT32)(pcsTemplate->m_csEventMobDialog[lDialogType].m_fProbable*1000000);

			if( m_csRand.rand(100000000) <= lProbable )
			{
				INT32			lDialogIndex;
				INT32			lTextNum;

				//아래의 randInt는 rand와 달리 10을 넣으면 0~10이 나옴. 따라서 1을 줄였음.
				lTextNum = pcsTemplate->m_csEventMobDialog[lDialogType].m_iDialogTextNum - 1;

				if( lTextNum > 0 )
				{
					lDialogIndex = m_csRand.randInt(lTextNum);

					pstrMobDialog = pcsTemplate->m_csEventMobDialog[lDialogType].m_ppstrDialogText[lDialogIndex];
				}
			}
		}
	}
	
	return pstrMobDialog;
}

BOOL AgpmEventNPCDialog::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8	cOperation			= (-1);
	PVOID	pvPacketEventBase	= NULL;
	INT32	lCID				= AP_INVALID_CID;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvPacketEventBase,
						&lCID);

	switch (cOperation) 
	{
		case AGPMEVENT_NPCDIALOG_PACKET_OPERATION_REQUEST:
			{
				if (!pvPacketEventBase)
					return FALSE;

				ApdEvent		*pcsEvent		= m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
				if (!pcsEvent)
					return FALSE;

				if (pcsEvent->m_eFunction != APDEVENT_FUNCTION_NPCDAILOG)
					return FALSE;

				AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
				if (!pcsCharacter)
					return FALSE;

				if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
				{
					pcsCharacter->m_Mutex.Release();
					return FALSE;
				}

				OnOperationRequest(pcsEvent, pcsCharacter);

				pcsCharacter->m_Mutex.Release();
			} break;

		case AGPMEVENT_NPCDIALOG_PACKET_OPERATION_GRANT:
			{
				if (!pvPacketEventBase)
					return FALSE;

				ApdEvent		*pcsEvent		= m_pcsApmEventManager->GetEventFromBasePacket(pvPacketEventBase);
				if (!pcsEvent)
					return FALSE;

				if (pcsEvent->m_eFunction != APDEVENT_FUNCTION_NPCDAILOG)
					return FALSE;

				AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
				if (!pcsCharacter)
					return FALSE;

				if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
				{
					pcsCharacter->m_Mutex.Release();
					return FALSE;
				}

				OnOperationGrant(pcsEvent, pcsCharacter);

				pcsCharacter->m_Mutex.Release();
			} break;

		case AGPMEVENT_NPCDIALOG_PACKET_OPERATION_REQUEST_EX:
			{
				OnOperationRequestEx(pvPacket);
			} break;
	
		case AGPMEVENT_NPCDIALOG_PACKET_OPERATION_GRANT_EX:
			{
				OnOperationGrantEx(pvPacket);
			} break;
	}

	return TRUE;
}

BOOL AgpmEventNPCDialog::OnOperationRequest(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	AuPOS	stTargetPos;
	ZeroMemory(&stTargetPos, sizeof(AuPOS));

	if (m_pcsApmEventManager->CheckValidRange(pcsEvent, &pcsCharacter->m_stPos, AGPMEVENT_NPCDIALOG_MAX_USE_RANGE, &stTargetPos))
	{
		pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

		if (pcsCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

		EnumCallback(AGPMEVENT_NPCDIALOG_CB_REQUEST, pcsEvent, pcsCharacter);
	}
	else
	{
		// stTargetPos 까지 이동
		pcsCharacter->m_stNextAction.m_bForceAction = FALSE;
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_EVENT_NPC_DIALOG;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsEvent->m_pcsSource->m_eType;
		pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsEvent->m_pcsSource->m_lID;

		m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
	}

	return TRUE;
}

BOOL AgpmEventNPCDialog::OnOperationGrant(ApdEvent *pcsEvent, AgpdCharacter *pcsCharacter)
{
	if (!pcsEvent || !pcsCharacter)
		return FALSE;

	EnumCallback(AGPMEVENT_NPCDIALOG_CB_GRANT, pcsEvent, pcsCharacter);

	return TRUE;
}

BOOL AgpmEventNPCDialog::SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_NPCDIALOG_CB_REQUEST, pfCallback, pClass);
}

BOOL AgpmEventNPCDialog::SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_NPCDIALOG_CB_GRANT, pfCallback, pClass);
}

PVOID AgpmEventNPCDialog::MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation		= AGPMEVENT_NPCDIALOG_PACKET_OPERATION_REQUEST;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_NPCDIALOG_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID AgpmEventNPCDialog::MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pcsEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8	cOperation		= AGPMEVENT_NPCDIALOG_PACKET_OPERATION_GRANT;

	PVOID	pvPacketBase	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_NPCDIALOG_PACKET_TYPE,
													&cOperation,
													pvPacketBase,
													&lCID);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

BOOL AgpmEventNPCDialog::CBActionNPCDialog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventNPCDialog		*pThis				= (AgpmEventNPCDialog *)	pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;
	AgpdCharacterAction		*pstAction			= (AgpdCharacterAction *)	pCustData;

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

	if (pcsCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);

	ApdEvent	*pcsEvent	= pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType, pstAction->m_csTargetBase.m_lID, APDEVENT_FUNCTION_NPCDAILOG);

	pThis->EnumCallback(AGPMEVENT_NPCDIALOG_CB_REQUEST, pcsEvent, pcsCharacter);

	return TRUE;
}

BOOL AgpmEventNPCDialog::OnOperationRequestEx( PVOID pvPacket )
{
	PACKET_EVENTNPCDIALOG_REQUEST_EX* pPacket = (PACKET_EVENTNPCDIALOG_REQUEST_EX*)pvPacket;

	switch(pPacket->pcOperation2)
	{
		case AGPMEVENTNPCDIALOG_PACKET_REQUEST_MESSAGEBOX:
			{
				EnumCallback(AGPMEVENT_NPCDIALOG_CB_REQUEST_MESSAGEBOX, pvPacket, NULL);
			} break;
		case AGPMEVENTNPCDIALOG_PACKET_REQUEST_MENU:
			{
				EnumCallback(AGPMEVENT_NPCDIALOG_CB_REQUEST_MENU, pvPacket, NULL);
			} break;
	}

	return TRUE;
}

BOOL AgpmEventNPCDialog::OnOperationGrantEx( PVOID pvPacket )
{
	PACKET_EVENTNPCDIALOG_GRANT_EX* pPacket = (PACKET_EVENTNPCDIALOG_GRANT_EX*)pvPacket;

	switch(pPacket->pcOperation2)
	{
		case AGPMEVENTNPCDIALOG_PACKET_GRANT_MESSAGEBOX:
			{
				EnumCallback(AGPMEVENT_NPCDIALOG_CB_GRANT_MESSAGEBOX, pvPacket, NULL);
			} break;

		case AGPMEVENTNPCDIALOG_PACKET_GRANT_MENU:
			{
				EnumCallback(AGPMEVENT_NPCDIALOG_CB_GRANT_MENU, pvPacket, NULL);
			} break;
	}

	return TRUE;
}