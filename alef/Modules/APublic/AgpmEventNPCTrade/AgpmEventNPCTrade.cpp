#include "AgpmEventNPCTrade.h"
#include "ApMemoryTracker.h"
#include "ApModuleStream.h"

AgpmEventNPCTrade::AgpmEventNPCTrade()
{
	SetModuleName("AgpmEventNPCTrade");
	
	m_pcsApmEventManager	= NULL;
	m_pcsAgpmGrid			= NULL;
}

AgpmEventNPCTrade::~AgpmEventNPCTrade()
{
	//템플릿을 삭제한다.
	AgpdEventNPCTradeTemplate	**ppcsTemplate = NULL;

	INT32			lIndex = 0;

	// 등록된 모든 NPCTrade Template에 대해서...
	for( ppcsTemplate = (AgpdEventNPCTradeTemplate **) m_csNPCTradeTemplate.GetObjectSequence(&lIndex); ppcsTemplate; ppcsTemplate = (AgpdEventNPCTradeTemplate **) m_csNPCTradeTemplate.GetObjectSequence(&lIndex))
	{
		if( (*ppcsTemplate) != NULL )
		{
			(*ppcsTemplate)->m_csItemList.Destroy();

			// 마고자 (2004-06-17 오후 12:35:08) : 이거 넣어줘야하지 않나요오~?
			delete (*ppcsTemplate);
		}
	}
}

BOOL AgpmEventNPCTrade::OnAddModule()
{
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmGrid = (AgpmGrid *) GetModule("AgpmGrid");

	if( !m_pcsApmEventManager || !m_pcsAgpmGrid )
	{
		return FALSE;
	}

	if( !m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_NPCTRADE, CBEventConstructor, CBEventDestructor, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
	{
		return FALSE;
	}

	if( !m_csNPCTradeTemplate.InitializeObject(sizeof(AgpdEventNPCTradeData *) , AGPMEVENT_NPCTRADE_MAX_NPC ) )
	{
		return FALSE;
	}

	if( !m_csNPCTradeItemGroup.InitializeObject( sizeof(AgpdEventNPCTradeItemGroupList *), AGPMEVENT_NPCTRADE_MAX_NPC ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL	AgpmEventNPCTrade::CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade		*pThis;
	ApdEvent				*pcsNPCTrade;
	BOOL			bResult;

	pThis = (AgpmEventNPCTrade *)pClass;
		
	bResult = FALSE;

	if( pThis->m_pcsAgpmGrid != NULL && pThis != NULL )
	{
		pcsNPCTrade = (ApdEvent	*)pData;			//실제 데이터를 받을 Object

		pcsNPCTrade->m_pvData = new AgpdEventNPCTradeData;
		((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid = new AgpdGrid;

		ZeroMemory(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcInitGridData, sizeof(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcInitGridData));
		ZeroMemory(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_alItemID, sizeof(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_alItemID));
		ZeroMemory(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcItemData, sizeof(((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_ppcItemData));

		if( pcsNPCTrade->m_pvData != NULL )
		{
			if( ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid != NULL )
			{
				bResult = pThis->m_pcsAgpmGrid->Init( ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid, AGPDGRID_TYPE_NPCTRADEBOX );
			}
		}
	}

	return bResult;
}

BOOL	AgpmEventNPCTrade::CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade		*pThis;
	ApdEvent				*pcsNPCTrade;
	BOOL			bResult;
		
	bResult = FALSE;

	pThis = (AgpmEventNPCTrade *)pClass;

	if( pThis->m_pcsAgpmGrid != NULL && pThis != NULL )
	{
		pcsNPCTrade = (ApdEvent	*)pData;			//실제 데이터를 받을 Object

		if( (AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData != NULL )
		{
			pThis->EnumCallback(AGPMEVENT_NPCTRADE_CB_ID_DELETE_GRID, ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid, NULL);

			pThis->m_pcsAgpmGrid->Remove( ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid );

			delete ((AgpdEventNPCTradeData *)pcsNPCTrade->m_pvData)->m_pcsGrid;
			delete pcsNPCTrade->m_pvData;
		}
	}

	return TRUE;
}

BOOL	AgpmEventNPCTrade::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade *		pThis		= (AgpmEventNPCTrade *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdEventNPCTradeData*	pstNPCTrade	= (AgpdEventNPCTradeData *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;

	//시작~
	if (!pstStream->WriteValue(AGPMEVENT_NPCTRADE_INI_NAME_START, 0))
		return FALSE;

	//템플릿이름 저장.
	if (!pstStream->WriteValue(AGPMEVENT_NPCTRADE_INI_TEMPLATE, pstNPCTrade->m_lNPCTradeTemplateID ))
		return FALSE;

	//끝~
	if (!pstStream->WriteValue(AGPMEVENT_NPCTRADE_INI_NAME_END, 0))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventNPCTrade::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventNPCTrade *		pThis		= (AgpmEventNPCTrade *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	AgpdEventNPCTradeData*	pstNPCTrade	= (AgpdEventNPCTradeData *) pstEvent->m_pvData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;
	const CHAR *			szValueName;

	if (!pstStream->ReadNextValue())
		return TRUE;

	szValueName = pstStream->GetValueName();
	if (_stricmp(szValueName, AGPMEVENT_NPCTRADE_INI_NAME_START))
		return TRUE;

	while (pstStream->ReadNextValue())
	{
		szValueName = pstStream->GetValueName();

		if( !_stricmp(szValueName, AGPMEVENT_NPCTRADE_INI_TEMPLATE) )
		{
			pstStream->GetValue( &pstNPCTrade->m_lNPCTradeTemplateID );
		}
		else if(!_stricmp(szValueName, AGPMEVENT_NPCTRADE_INI_NAME_END))
		{
			break;
		}
	}

	return TRUE;
}

/*BOOL AgpmEventNPCTrade::LoadNPCTradeItemGroupTemplate( char *pstrFileName )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, true ) )
	{
		INT32			lRow, lColumn;

		lRow = m_csExcelTxtLib.GetRow();
		lColumn = m_csExcelTxtLib.GetColumn();

		for( int lTempColumn=0; lTempColumn<lColumn; lTempColumn++ )
		{
			AgpdEventNPCTradeItemGroupList *pcsItemGroup;

			char			*pstrGroupName;

			pcsItemGroup = new AgpdEventNPCTradeItemGroupList;

			//2번째가 그룹 아이템 이름.
			pstrGroupName = m_csExcelTxtLib.GetData( lTempColumn, 2 );

			strcat( pcsItemGroup->m_strGroupName, pstrGroupName );

			//여기부터 아이템이름.
			for( int lTempRow=3; lTempRow<lRow; lTempRow++ )
			{
				char			*pstrTempItemTID;

				pstrTempItemTID = m_csExcelTxtLib.GetData( lTempColumn, lTempRow );

				if( pstrTempItemTID != NULL )
				{
					INT32			*plItemTID;

					plItemTID = new INT32;
					
					*plItemTID = atoi( pstrTempItemTID );

					pcsItemGroup->m_csNPCTradeItemList.Add( plItemTID );
				}
			}

			m_csNPCTradeItemGroup.AddObject( (void *)&pcsItemGroup, pstrGroupName );
		}

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgpmEventNPCTrade::LoadNPCTradeTemplate( char *pstrFileName )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, true ) )
	{
		INT32			lRow, lColumn;

		lRow = m_csExcelTxtLib.GetRow();
		lColumn = m_csExcelTxtLib.GetColumn();

		for( int lTempRow=3; lTempRow<lRow; lTempRow++ )
		{
			AgpdEventNPCTradeTeamplate	*pcsTemplate;
			char			*pstrData;

			pcsTemplate = new AgpdEventNPCTradeTeamplate;

			//Template 이름복사.
			pstrData = m_csExcelTxtLib.GetData( 0, lTempRow );
			pcsTemplate->m_lNPCTID = atoi( pstrData );

			//Town 이름복사
			pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );
			strcat( pcsTemplate->m_strTownName, pstrData );

			//매도함수.
			pstrData = m_csExcelTxtLib.GetData( 2, lTempRow );
			pcsTemplate->m_fSellFunc = atof( pstrData );

			//매입함수
			pstrData = m_csExcelTxtLib.GetData( 3, lTempRow );
			pcsTemplate->m_BuyFunc = atof( pstrData );

			//매입함수B(Other)
			pstrData = m_csExcelTxtLib.GetData( 4, lTempRow );
			pcsTemplate->m_BuyOtherFunc = atof( pstrData );

			for( int lTempColumn=5; lTempColumn<lColumn; lTempColumn++ )
			{
				pstrData = m_csExcelTxtLib.GetData( lTempColumn, lTempRow );

				if( pstrData != NULL )
				{
					AgpdEventNPCTradeItemGroupList		**ppcsGroupList;
					
					ppcsGroupList = (AgpdEventNPCTradeItemGroupList **)m_csNPCTradeItemGroup.GetObject( pstrData );

					if( ppcsGroupList != NULL )
					{
						pcsTemplate->m_csGroupList.Add( (*ppcsGroupList) );
					}
				}
			}

			m_csNPCTradeTemplate.AddObject( (void *)&pcsTemplate, pcsTemplate->m_lNPCTID );
		}

		bResult = TRUE;
	}

	return bResult;
}*/

BOOL AgpmEventNPCTrade::LoadNPCTradeRes( char *pstrFileName , BOOL bDecryption )
{
	BOOL				bResult;

	bResult = FALSE;

	if( m_csExcelTxtLib.OpenExcelFile( pstrFileName, TRUE , bDecryption ) )
	{
		AgpdEventNPCTradeTemplate	*pcsTemplate;

		INT32			lRow, lColumn;

		pcsTemplate = NULL;
		lRow = m_csExcelTxtLib.GetRow();
		lColumn = m_csExcelTxtLib.GetColumn();

		for( int lTempRow=0; lTempRow<lRow; lTempRow++ )
		{
			char			*pstrData;

			//Template 이름복사.
			pstrData = m_csExcelTxtLib.GetData( 0, lTempRow );

			if( pstrData != NULL )
			{
				if( !_stricmp( pstrData, "Npc" ) )
				{
					pcsTemplate = new AgpdEventNPCTradeTemplate;
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

					if( pstrData )
						pcsTemplate->m_lNPCTID = atoi(pstrData);
				}
				else if( !_stricmp( pstrData, "Sell" ) )
				{
					//매도함수.
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

					if( pstrData )
						pcsTemplate->m_fSellFunc = (float)atof( pstrData );
				}
				else if( !_stricmp( pstrData, "Buy" ) )
				{
					//매입함수
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );
					
					if( pstrData )
						pcsTemplate->m_fBuyFunc = (float)atof( pstrData );
				}
				else if( !_stricmp( pstrData, "BuyOther" ) )
				{
					//매입함수B(Other)
					pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

					if( pstrData )
						pcsTemplate->m_fBuyOtherFunc = (float)atof( pstrData );
				}
				else if( !_stricmp( pstrData, "Item" ) )
				{
					//pcsTemplate의 리스트에 직접 넣는다.
					AgpdEventNPCTradeItemListData		*pcsItemListData;

					pcsItemListData = new AgpdEventNPCTradeItemListData;

					if( pcsItemListData )
					{
						pstrData = m_csExcelTxtLib.GetData( 1, lTempRow );

						if( pstrData )
							pcsItemListData->m_lItemTID = atoi(pstrData);

						pstrData = m_csExcelTxtLib.GetData( 2, lTempRow );

						if( pstrData )
							pcsItemListData->m_lItemCount = atoi(pstrData);

						pcsTemplate->m_csItemList.Add( pcsItemListData );
					}
				}
				else if( !_stricmp( pstrData, "-End" ) )
				{
					m_csNPCTradeTemplate.AddObject( (void *)&pcsTemplate, pcsTemplate->m_lNPCTID );
				}
			}
		}

		bResult = TRUE;

		//열었으니 닫는다.
		m_csExcelTxtLib.CloseFile();
	}

	return bResult;
}

BOOL AgpmEventNPCTrade::IsItemInGrid( AgpdEventNPCTradeTemplate *pcsTemplate, INT32 lItemTID )
{
	CListNode< AgpdEventNPCTradeItemListData * >		*pcsNode;
	BOOL				bResult;

	bResult = FALSE;

	for( pcsNode = pcsTemplate->m_csItemList.GetStartNode(); pcsNode; pcsNode=pcsNode->m_pcNextNode )
	{
		if( lItemTID == ((AgpdEventNPCTradeItemListData *)pcsNode->m_tData)->m_lItemTID )
		{
			bResult = TRUE;
			break;
		}
	}

	return bResult;
}

BOOL AgpmEventNPCTrade::SetCallbackDeleteGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_NPCTRADE_CB_ID_DELETE_GRID, pfCallback, pClass);
}
