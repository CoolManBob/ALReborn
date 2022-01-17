#include "AgpmEventGacha.h"
#include "AuIniManager.h"
#include "AgpmItem.h"
#include "AgpmGrid.h"
#include "AgpmDropItem2.h"
#include "AuRandomNumber.h"

static AgpmEventGacha	* g_pcsAgpmEventGacha = NULL;
static AgpmItem			* g_pcsAgpmItem		= NULL;
static AgpmGrid			* g_pcsAgpmGrid		= NULL;

AgpmEventGacha::AgpmEventGacha():
	m_pcsAgpmCharacter		( NULL ) ,
	m_pcsApmEventManager	( NULL ) ,
	m_pcsAgpmFactor			( NULL )

{
	SetModuleName("AgpmEventGacha");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMEVENT_GACHA_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT16));
			// INT8 은 인자 8개 까지 가능..
			// INT16 은 인자 16개까지 가능..
	m_csPacket.SetFieldType(AUTYPE_INT8,		1,	// Operation
							AUTYPE_PACKET,		1,	// Event Base Packet
							AUTYPE_INT32,		1,	// CID
							AUTYPE_INT32,		1,	// Result ( Operation에 의해서 달라짐. )

							AUTYPE_INT32,		1,	// 지불 비용 TID
							AUTYPE_INT32,		1,	// 위에 TID 필요 갯수
							AUTYPE_INT32,		1,	// 돈이 필요할경우 비용.
							AUTYPE_INT32,		1,	// charisma

							AUTYPE_MEMORY_BLOCK, 1,  // 가능한 TID벡터 

							AUTYPE_END,			0
							);

	EnableIdle(FALSE);

	if( g_pcsAgpmEventGacha )
	{
		PostQuitMessage( 0 );
	}

	g_pcsAgpmEventGacha = this;
}

AgpmEventGacha::~AgpmEventGacha()
{

}

BOOL	AgpmEventGacha::StreamReadGachaTypeTable( const CHAR *szFile, BOOL bDecryption )
{
	if( m_mapGachaType.size() )
	{
		//이미 로딩되어있다?
		return FALSE;
	}

	const char strName			[]	= "Name"		;
	const char strComment		[]	= "Comment"		;
	const char strRaceCheck		[]	= "RaceCheck"	;
	const char strClassCheck	[]	= "ClassCheck"	;
	const char strLevelCheck	[]	= "LevelCheck"	;
	const char strLevelLimit	[]	= "LevelLimit"	;
	const char strDropTable		[]	= "DropTable"	;

	AuIniManagerA	iniManager;

	iniManager.SetPath(	szFile	);

	if( iniManager.ReadFile(0, bDecryption) )
	{
		int		nSectionCount	;
		int		nIndex			;

		int		nKeyName		;
		int		nKeyComment		;
		int		nKeyRaceCheck	;
		int		nKeyClassCheck	;
		int		nKeyLevelCheck	;
		int		nKeyLevelLimit	;
		int		nKeyDropTable	;

		nSectionCount	= iniManager.GetNumSection();	

		// 테긋쳐 추가함..

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			AgpdGachaType	stType;

			nIndex			= atoi( iniManager.GetSectionName( i ) );

			nKeyName		= iniManager.FindKey( i , ( char * ) strName		);
			nKeyComment		= iniManager.FindKey( i , ( char * ) strComment		);
			nKeyRaceCheck	= iniManager.FindKey( i , ( char * ) strRaceCheck	);
			nKeyClassCheck	= iniManager.FindKey( i , ( char * ) strClassCheck	);
			nKeyLevelCheck	= iniManager.FindKey( i , ( char * ) strLevelCheck	);
			nKeyLevelLimit	= iniManager.FindKey( i , ( char * ) strLevelLimit	);
			nKeyDropTable	= iniManager.FindKey( i , ( char * ) strDropTable	);

			stType.nID			= nIndex;

			stType.strName		= iniManager.GetValue	( i , nKeyName );
			stType.strComment	= iniManager.GetValue	( i , nKeyComment );

			string	strDropTable = "./ini/";
			strDropTable += iniManager.GetValue	( i , nKeyDropTable );

			stType.bRaceCheck	= atoi( iniManager.GetValue	( i , nKeyRaceCheck		) ) ? true : false;
			stType.bClassCheck	= atoi( iniManager.GetValue	( i , nKeyClassCheck	) ) ? true : false;
			stType.bLevelCheck	= atoi( iniManager.GetValue	( i , nKeyLevelCheck	) ) ? true : false;
			stType.nLevelLimit	= atoi( iniManager.GetValue	( i , nKeyLevelLimit	) );

			//데이타 확인하고 읽는거 확인..
			if( !stType.stDropTable.StreamRead( strDropTable.c_str() , bDecryption ) )
			{
				// 드롭테이블 읽기 실패
				return FALSE;
			}

			m_mapGachaType[ nIndex ] = stType;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL	AgpmEventGacha::OnAddModule()
{
	m_pcsApmEventManager	= (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmCharacter		= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmFactor			= (AgpmFactors *) GetModule("AgpmFactors");

	if (!m_pcsApmEventManager || !m_pcsAgpmCharacter || !m_pcsAgpmFactor )
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackActionEventGacha(CBAction, this))
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_GACHA, NULL, NULL, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	g_pcsAgpmItem = ( AgpmItem * ) GetModule( "AgpmItem" );
	g_pcsAgpmGrid = ( AgpmGrid * ) GetModule( "AgpmGrid" );

	return TRUE;
}

BOOL	AgpmEventGacha::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8		cOperation		= (-1);
	PVOID		pvPacketBase	= NULL;

	GachaInfo	stGachaInfo;

	CHAR*		szMessage		= NULL;
	UINT16		unMessageLength	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation					,
						&pvPacketBase				,
						&stGachaInfo.lCID			,
						&stGachaInfo.lResult		,
						&stGachaInfo.lTID			,
						&stGachaInfo.lItemCount		,
						&stGachaInfo.lMoney			,
						&stGachaInfo.lCharisma,
						&szMessage					,
						&unMessageLength
						);
	/*
	// 이놈은 뭐하는거지
	if (!pstCheckArg->bReceivedFromServer && m_pcsAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
	{
		pAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}
	*/


	if (pvPacketBase)
	{
		stGachaInfo.pcsEvent = m_pcsApmEventManager->GetEventFromBasePacket(pvPacketBase);
		if (!stGachaInfo.pcsEvent)
			return FALSE;
	}

	AgpdCharacter *pAgpdCharacter = m_pcsAgpmCharacter->GetCharacterLock(stGachaInfo.lCID);
	if (!pAgpdCharacter)
		return FALSE;

	switch( cOperation )
	{
	case	OP_REQUEST	:	// 가챠 요청 Client->Server
		{
			AuPOS	stTargetPos;
			ZeroMemory(&stTargetPos, sizeof(AuPOS));

			const INT32 AGPMEVENT_GACHA_MAX_USE_RANGE	= 600;

			if (m_pcsApmEventManager->CheckValidRange(stGachaInfo.pcsEvent, &pAgpdCharacter->m_stPos, AGPMEVENT_GACHA_MAX_USE_RANGE, &stTargetPos))
			{
				pAgpdCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

				if (pAgpdCharacter->m_bMove)
					m_pcsAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);

				EnumCallback( CB_EV_REQUEST , &stGachaInfo , pAgpdCharacter );
			}
			else
			{
				// stTargetPos 까지 이동
				pAgpdCharacter->m_stNextAction.m_bForceAction			= FALSE;
				pAgpdCharacter->m_stNextAction.m_eActionType			= AGPDCHAR_ACTION_TYPE_EVENT_GACHA;
				pAgpdCharacter->m_stNextAction.m_csTargetBase.m_eType	= stGachaInfo.pcsEvent->m_pcsSource->m_eType;
				pAgpdCharacter->m_stNextAction.m_csTargetBase.m_lID		= stGachaInfo.pcsEvent->m_pcsSource->m_lID;

				/*
				GachaInfo * pGachaInfo = new GachaInfo;
				*pGachaInfo = stGachaInfo;
				pAgpdCharacter->m_stNextAction.m_lUserData[ 0 ]			= ( INT_PTR ) pGachaInfo;
				*/

				m_pcsAgpmCharacter->MoveCharacter(pAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE, FALSE, TRUE, FALSE);
			}
		}
		break;
	case	OP_REQUESTGRANTED	:	// 가차 요청 결과 Server->Client
		{
			unMessageLength = unMessageLength / sizeof INT32;

			INT32 *		pTIDArray	= ( INT32 * ) szMessage;

			for( UINT16 i = 0 ; i < unMessageLength ; i ++ )
			{
				stGachaInfo.vecTID.push_back( pTIDArray[ i ] );
			}

			EnumCallback( CB_EV_REQUESTGRANTED , &stGachaInfo , pAgpdCharacter );
		}
		break;
	case	OP_GACHA	:	// 굴리기 Client->Server
		{
			EnumCallback( CB_EV_GACHA , &stGachaInfo , pAgpdCharacter);
		}
		break;
	case	OP_RESULT	:	// 결과물 Server->Client
		{
			// lCID
			INT32 nTID = stGachaInfo.lResult;
			INT32 eResult = stGachaInfo.lTID;
			EnumCallback( CB_EV_RESULT , &stGachaInfo, pAgpdCharacter );
		}
		break;
	}

	// 언제 락을 했나?
	pAgpdCharacter->m_Mutex.Release();

	return TRUE;
}


PVOID	AgpmEventGacha::MakePacketEventRequest	(ApdEvent *pApdEvent, INT32 lCID, INT32 nGachaType, INT16 *pnPacketLength)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AgpmEventGacha::OP_REQUEST;

	INT32		lTID			= 0;
	INT32		lItemCount		= 0;
	INT32		lMoney			= 0;
	INT32		lCharisma		= 0;

	UINT16		lDummy			= 0;

	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GACHA_PACKET_TYPE,
										   &cOperation	,
										   pvPacketBase	,
										   &lCID		,
										   &nGachaType	,
										   &lTID		,
										   &lItemCount	,
										   &lMoney		,
										   &lCharisma,
										   NULL			,
										   &lDummy		
										   );

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID	AgpmEventGacha::MakePacketEventRequestGranted	(ApdEvent *pApdEvent, INT32 lCID, INT32 nErrorCode , vector< INT32 > * pVector , INT16 *pnPacketLength)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AgpmEventGacha::OP_REQUESTGRANTED;

	INT32		lTID			= 0;
	INT32		lItemCount		= 0;
	INT32		lMoney			= 0;
	INT32		lCharisma		= 0;
	INT32		lByteCount		= 0;

	//if(nErrorCode == EC_NOERROR)
	{
		INT32	nGachaType = PtrToInt( pApdEvent->m_pvData );
		AgpdCharacter * pcsCharacter = m_pcsAgpmCharacter->GetCharacter( lCID );
		if(!pcsCharacter)
			return NULL;

		AgpdGachaType * pGachaType = GetGachaTypeInfo( nGachaType );
		ASSERT( pGachaType );
		if( !nGachaType ) return NULL;

		AgpdGachaItemTable	* pItemTable = pGachaType->GetGachItemTable( pcsCharacter );
		ASSERT( pItemTable );
		if( !pItemTable ) return NULL;

		INT32 nLevel = m_pcsAgpmCharacter->GetLevel( pcsCharacter );

		// 확율 정보
		AgpdGachaDropTable::DropInfo * pDropInfo = &pGachaType->stDropTable.mapGacha[ nLevel ];

		lTID		= pDropInfo->nItemTID	;
		lItemCount	= pDropInfo->nItemNeeded;
		lMoney		= pDropInfo->nCost		;
		lCharisma	= pDropInfo->nNeedCharisma;
		lByteCount	= ( INT32 ) ( pVector->size() * sizeof INT32 );
	}

	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	// Gacha 가능한 TID를 스트리밍 해주는 코드 필요.

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GACHA_PACKET_TYPE,
										   &cOperation	,
										   pvPacketBase	,
										   &lCID		,
										   &nErrorCode	,
										   &lTID		,
										   &lItemCount	,
										   &lMoney		,
										   &lCharisma,
										   ( char * ) &(*pVector)[ 0 ]			,
										   &lByteCount		
										   );

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID	AgpmEventGacha::MakePacketEventGacha			(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AgpmEventGacha::OP_GACHA;
	
	INT32		lTID			= 0;
	INT32		lItemCount		= 0;
	INT32		lMoney			= 0;
	INT32		lCharisma		= 0;

	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	INT32	 nDummy = 0;
	UINT16	 lDummy = 0;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GACHA_PACKET_TYPE,
										   &cOperation	,
										   pvPacketBase	,
										   &lCID		,
										   &nDummy		,
										   &lTID		,
										   &lItemCount	,
										   &lMoney		,
										   &lCharisma,
										   NULL			,
										   &lDummy		
										   );

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

PVOID	AgpmEventGacha::MakePacketEventResult			(ApdEvent *pApdEvent, INT32 lCID, INT32 eErrorCode ,  INT32 nTID , INT16 *pnPacketLength)
{
	if (!pApdEvent || !pnPacketLength || lCID == AP_INVALID_CID)
		return NULL;

	INT8 cOperation = AgpmEventGacha::OP_RESULT;
	
	INT32		lTID			= eErrorCode;
	INT32		lItemCount		= 0;
	INT32		lMoney			= 0;
	INT32		lCharisma		= 0;

	UINT16		 nDummy			= 0;

	PVOID pvPacketBase = m_pcsApmEventManager->MakeBasePacket(pApdEvent);
	if (!pvPacketBase)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_GACHA_PACKET_TYPE,
										   &cOperation	,
										   pvPacketBase	,
										   &lCID		,
										   &nTID		,
										   &lTID		,	// 에러코드 삽입..
										   &lItemCount	,
										   &lMoney		,
										   &lCharisma,
										   NULL			,
										   &nDummy		
										   );

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

BOOL	AgpmEventGacha::CBAction(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmEventGacha *pThis = (AgpmEventGacha *) pClass;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pData;
	AgpdCharacterAction *pstAction = (AgpdCharacterAction *) pCustData;

	//GachaInfo * pGachaInfo = ( GachaInfo *  ) pstAction->m_lUserData[ 0 ];

	if (pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pAgpdCharacter))
		return FALSE;

	pAgpdCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;
	
	if(pAgpdCharacter->m_bMove)
		pThis->m_pcsAgpmCharacter->StopCharacter(pAgpdCharacter, NULL);
	
	ApdEvent *pApdEvent = pThis->m_pcsApmEventManager->GetEvent(pstAction->m_csTargetBase.m_eType,
															  pstAction->m_csTargetBase.m_lID,
															  APDEVENT_FUNCTION_GACHA);

	GachaInfo	stGachaInfo;
	stGachaInfo.pcsEvent	= pApdEvent;

	// pThis->EnumCallback(AGPMEVENTREFINERY_CB_REQUEST, pApdEvent, pAgpdCharacter);
	pThis->EnumCallback( CB_EV_REQUEST ,&stGachaInfo , pAgpdCharacter );

	return TRUE;
}

BOOL	AgpmEventGacha::BuildGachaItemTable()
{
	// 아이템을 다 긁어서 케릭터 종족 , 클래스별로 테이블 구성한다.

	// not implemented yet;

	// 각 아이템 별로 , 각 가차 타입별로..종족별로.. 체크하여 테이블을 완성한다.

	AgpmDropItem2	* pcsAgpmDropItem2	= ( AgpmDropItem2 *) GetModule( "AgpmDropItem2" );

	ASSERT( g_pcsAgpmItem );
	ASSERT( g_pcsAgpmGrid );
	ASSERT( pcsAgpmDropItem2 ); // 클라이언트에선 이게 없다.

	if( ! g_pcsAgpmItem || ! g_pcsAgpmGrid || ! pcsAgpmDropItem2 )
	{
		//헉 이건 뭔 시츄에이션?
		return FALSE;
	}

	INT32	lIndex;
	AgpdItemTemplate		*pcsItemTemplate;

	lIndex = 0;

	vector< AuRace >	vecRace;

	// 이중에 몇개는 쓸모없는게 있을것이니라.
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN,		AUCHARCLASS_TYPE_KNIGHT	) );
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN,		AUCHARCLASS_TYPE_RANGER	) );
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN,		AUCHARCLASS_TYPE_SCION ) );
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN,		AUCHARCLASS_TYPE_MAGE ) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC,			AUCHARCLASS_TYPE_KNIGHT	) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC,			AUCHARCLASS_TYPE_RANGER	) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC,			AUCHARCLASS_TYPE_SCION ) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC,			AUCHARCLASS_TYPE_MAGE ) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF,		AUCHARCLASS_TYPE_KNIGHT	) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF,		AUCHARCLASS_TYPE_RANGER	) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF,		AUCHARCLASS_TYPE_SCION ) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF,		AUCHARCLASS_TYPE_MAGE ) );
	vecRace.push_back( AuRace( AURACE_TYPE_DRAGONSCION, AUCHARCLASS_TYPE_KNIGHT	) );
	vecRace.push_back( AuRace( AURACE_TYPE_DRAGONSCION, AUCHARCLASS_TYPE_RANGER	) );
	vecRace.push_back( AuRace( AURACE_TYPE_DRAGONSCION, AUCHARCLASS_TYPE_SCION ) );
	vecRace.push_back( AuRace( AURACE_TYPE_DRAGONSCION,	AUCHARCLASS_TYPE_MAGE ) );

	for(AgpaItemTemplate::iterator it = g_pcsAgpmItem->csTemplateAdmin.begin(); it != g_pcsAgpmItem->csTemplateAdmin.end(); ++it)
	{
		pcsItemTemplate = it->second;

		if( !pcsItemTemplate->m_nGachaType ) continue;

		AgpdGachaType	* pGachaType	= GetGachaTypeInfo( pcsItemTemplate->m_nGachaType );

		if( !pGachaType )
		{
			// 일단 컨티뉴..
			continue;

			// 에러!!!!!!!!!
			return FALSE;
		}

		// 랭크 얻어냄.
		INT32		lRank				= 0;
		m_pcsAgpmFactor->GetValue(&pcsItemTemplate->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_GACHA);
	
		if( lRank < 1 || lRank > 4 )
		{
			char	str[ 256 ];
			sprintf( str , "AgpmEventGacha::BuildGachaItemTable() '%s' 아이템 랭크 이상\n" , pcsItemTemplate->m_szName );
			OutputDebugString( str );

			continue;
		}

		vector< AuRace >::iterator	iter2;
		for( iter2 = vecRace.begin();
			iter2 != vecRace.end() ;
			iter2++ )
		{
			AuRace	stRace = *iter2;
			BOOL	bValid = TRUE;

			if( pGachaType->bRaceCheck )
			{
				// 아이템의 종족제한을 확인 , 해당 종족에 장비 가능한지 확인..
				// 실패할경우 bValid = FALSE;
				// 실패할경우 bValid = FALSE;
				INT32	lItemRace = m_pcsAgpmFactor->GetRace(&pcsItemTemplate->m_csRestrictFactor);

				if( lItemRace > 0 &&
					lItemRace != (INT32) AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE &&
					!m_pcsAgpmFactor->CheckRace((AuRaceType) stRace.detail.nRace, &pcsItemTemplate->m_csRestrictFactor) )
				{
					bValid = FALSE;
				}
			}
			if( pGachaType->bClassCheck )
			{
				INT32	lItemClass = m_pcsAgpmFactor->GetClass(&pcsItemTemplate->m_csRestrictFactor);

				if( lItemClass > 0 &&
					lItemClass != (INT32) AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE &&
					!m_pcsAgpmFactor->CheckClass((AuCharClassType) stRace.detail.nClass, &pcsItemTemplate->m_csRestrictFactor) )
				{
					bValid = FALSE;
				}			
			}

			if( bValid )
			{
				int	nMin = 1 , nMax = 99;

				if( pGachaType->bLevelCheck )
				{
					AgpdDropItemADItemTemplate *	pcsADItemTemplate	= pcsAgpmDropItem2->GetItemTemplateData(pcsItemTemplate);

					if( pcsADItemTemplate )
					{
						nMin = pcsADItemTemplate->m_lGachaLevelMin;
						nMax = pcsADItemTemplate->m_lGachaLevelMax;
					}
					else
					{
						// 먼소리냐.
						DebugBreak();
					}
				}
			// min level , max level 참고해서
				// 해당 레벨에
				AgpdGachaItemTable	*pcsGachaItemTable = &pGachaType->mapGachaTable[ stRace ];

				for( int nLevel = nMin ; nLevel <= nMax ; nLevel++ )
				{
					AgpdGachaItemTable::Table * pTable = &pcsGachaItemTable->mapItemTable[ nLevel ];
					vector< INT32 >	* pVector = &( (*pTable)[ lRank ] );
					pVector->push_back( pcsItemTemplate->m_lID );
				}
			}
		}
	}

	return TRUE;
}

void	AgpmEventGacha::ReportGachaTable()
{
	// 테이블 생성한걸 로그로 남긴다..

	vector< AuRace >	vecRace;

	// 이중에 몇개는 쓸모없는게 있을것이니라.
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN	, AUCHARCLASS_TYPE_KNIGHT	) );
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN	, AUCHARCLASS_TYPE_RANGER	) );
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN	, AUCHARCLASS_TYPE_SCION		) );
	vecRace.push_back( AuRace( AURACE_TYPE_HUMAN	, AUCHARCLASS_TYPE_MAGE		) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC		, AUCHARCLASS_TYPE_KNIGHT	) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC		, AUCHARCLASS_TYPE_RANGER	) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC		, AUCHARCLASS_TYPE_SCION		) );
	vecRace.push_back( AuRace( AURACE_TYPE_ORC		, AUCHARCLASS_TYPE_MAGE		) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF	, AUCHARCLASS_TYPE_KNIGHT	) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF	, AUCHARCLASS_TYPE_RANGER	) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF	, AUCHARCLASS_TYPE_SCION		) );
	vecRace.push_back( AuRace( AURACE_TYPE_MOONELF	, AUCHARCLASS_TYPE_MAGE		) );

	map< INT32	, AgpdGachaType			>::iterator	iter;

	for( iter = m_mapGachaType.begin() ; 
		iter != m_mapGachaType.end();
		iter ++ )
	{
		AgpdGachaType	* pGachaType	= &iter->second;

		char	strGachaTypeLog[ 256 ];
		sprintf_s( strGachaTypeLog , sizeof(strGachaTypeLog), "log\\Log_GachaType_%02d.txt" , iter->first );
		FILE	* pFile = fopen( strGachaTypeLog , "wt" );

		fprintf( pFile , "Gacha Type %d Item Log\n\n" , iter->first );

		vector< AuRace >::iterator	iter2;
		for( iter2 = vecRace.begin();
			iter2 != vecRace.end() ;
			iter2++ )
		{
			AuRace	stRace = *iter2;

			switch( stRace.detail.nRace )
			{
			case AURACE_TYPE_HUMAN:			fprintf( pFile , "AURACE_TYPE_HUMAN	," );		break;
			case AURACE_TYPE_ORC:			fprintf( pFile , "AURACE_TYPE_ORC		," );	break;
			case AURACE_TYPE_MOONELF:		fprintf( pFile , "AURACE_TYPE_MOONELF	," );	break;
			case AURACE_TYPE_DRAGONSCION:	fprintf( pFile , "AURACE_TYPE_MOONELF	," );	break;
			default:						fprintf( pFile , "unknown_race		," );		break;
			}
			switch( stRace.detail.nClass )
			{
			case AUCHARCLASS_TYPE_KNIGHT	: fprintf( pFile , "AUCHARCLASS_TYPE_KNIGHT	\n\n" ); break;
			case AUCHARCLASS_TYPE_RANGER	: fprintf( pFile , "AUCHARCLASS_TYPE_RANGER	\n\n" ); break;
			case AUCHARCLASS_TYPE_SCION		: fprintf( pFile , "AUCHARCLASS_TYPE_SCION	\n\n" ); break;
			case AUCHARCLASS_TYPE_MAGE		: fprintf( pFile , "AUCHARCLASS_TYPE_MAGE	\n\n" ); break;
			default							: fprintf( pFile , "unknown_class			\n\n" ); break;
			}

			int	nMin = 1 , nMax = 99;

			// 해당 레벨에
			AgpdGachaItemTable	*pcsGachaItemTable = &pGachaType->mapGachaTable[ stRace ];

			for( int nLevel = nMin ; nLevel <= nMax ; nLevel++ )
			{
				AgpdGachaItemTable::Table * pTable = &pcsGachaItemTable->mapItemTable[ nLevel ];
				string	strItem;
	
				AgpdGachaItemTable::Table::iterator iter3;

				for( int nRank = 1 ; nRank <= 4 ; nRank ++ )
				{
					vector< INT32 > * pItemVector = &( *pTable )[ nRank ];

					if( pItemVector->size() )
					{
						for( vector< INT32 >::iterator iter4 = pItemVector->begin();
							iter4 != pItemVector->end();
							iter4 ++ )
						{
							char	str[ 256 ];
							sprintf( str , "%d," , *iter4 );
							strItem += str;
						}
					}
					else
					{
						strItem += "<Empty>";
					}

					strItem += "\t";
				}
				
				/*
				for( iter3 = pTable->begin();
					iter3 != pTable->end();
					iter3++)
				{
					INT32	nRank = iter3->first;
					vector< INT32 > * pItemVector = &iter3->second;

					vector< INT32 >::iterator	iter4;
					char	str[ 256 ];

					sprintf( str , "(Rank %d) " , nRank );
					strItem += str;

					for( iter4 = pItemVector->begin();
						iter4 != pItemVector->end();
						iter4 ++ )
					{
						sprintf( str , "%d," , *iter4 );
						strItem += str;
					}
				}
				*/
			
				char	strLevel[ 256 ];
				sprintf( strLevel , "%02d\t" , nLevel );

				string	strLevelItemInfo;
				strLevelItemInfo = strLevel;
				strLevelItemInfo += strItem;
				strLevelItemInfo += "\n";

				fprintf( pFile , "%s" , strLevelItemInfo.c_str() );
			}

			fprintf( pFile , "\n\n" );
		}

		fclose( pFile );
	}
}

AgpdGachaType		*	AgpmEventGacha::GetGachaTypeInfo( INT32 nGachaIndex	)
{
	map< INT32 , AgpdGachaType >::iterator	iter;
	iter = m_mapGachaType.find( nGachaIndex );
	if( iter == m_mapGachaType.end() )
	{
		return NULL;
	}
	else
	{
		AgpdGachaType * pGachaType = &iter->second;
		return pGachaType;
	}
}

AgpdGachaItemTable	*	AgpdGachaType::GetGachItemTable( AuRace stRace		)
{
	map< AuRace , AgpdGachaItemTable >::iterator	iter;
	iter = mapGachaTable.find( stRace );
	if( iter == mapGachaTable.end() )
	{
		return NULL;
	}
	else
	{
		AgpdGachaItemTable * pGachaItemTable = &iter->second;
		return pGachaItemTable;
	}
}

AgpdGachaItemTable	*	AgpdGachaType::GetGachItemTable( AgpdCharacter * pcsCharacter	)
{
	AuRace pAuRace;
	if(pcsCharacter->m_bIsTrasform && g_pcsAgpmEventGacha->m_pcsAgpmCharacter->IsCreature(pcsCharacter->m_pcsCharacterTemplate))
	{
		pAuRace.detail.nRace = g_pcsAgpmEventGacha->m_pcsAgpmFactor->GetRace( &pcsCharacter->m_csFactorOriginal );
		pAuRace.detail.nClass = g_pcsAgpmEventGacha->m_pcsAgpmFactor->GetClass( &pcsCharacter->m_csFactorOriginal );
	}
	else
	{
		pAuRace.detail.nRace = g_pcsAgpmEventGacha->m_pcsAgpmFactor->GetRace( &pcsCharacter->m_csFactor );
		pAuRace.detail.nClass = g_pcsAgpmEventGacha->m_pcsAgpmFactor->GetClass( &pcsCharacter->m_csFactor );
	}

	return GetGachItemTable(pAuRace);
}

BOOL    AgpdGachaDropTable::StreamRead( const char * pFilename , BOOL bDecryption )
{
	if (!pFilename )
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(pFilename, TRUE, bDecryption))
	{
		OutputDebugString("AgpdGachaDropTable::StreamRead Error (1) !!!\n");
		csExcelTxtLib.CloseFile();
		return FALSE;
	}

	const INT16				nKeyRow						= 0;
	const INT16				nTNameColumn				= 0;	
	char					*pszData					= NULL;

	enum	IMPORT_VALUE
	{
		IV_LEVEL		,
		IV_ITEMTID		,
		IV_ITEMCOUNT	,
		IV_COST			,
		IV_NEEDCHARISMA,
		IV_RANK1		,
		IV_RANK2		,
		IV_RANK3		,
		IV_RANK4		,

		IV_MAX
	};

	const	char	strIP_Level		[]	= "Character_Lv"		;
	const	char	strIP_ItemTID	[]	= "Gacha_ItemTID"		;
	const	char	strIP_ItemCount	[]	= "Gacha_Item_Number"	;
	const	char	strIP_Cost		[]	= "Gacha_Cost"			;
	const	char	strIP_NeedCharisma[]	= "CharismaPoint";
	const	char	strIP_Rank1		[]	= "Rank1"				;
	const	char	strIP_Rank2		[]	= "Rank2"				;
	const	char	strIP_Rank3		[]	= "Rank3"				;
	const	char	strIP_Rank4		[]	= "Rank4"				;

	int	aColumn[ IV_MAX ] = { 0 , };

	for(INT16 nCol = 0; nCol < csExcelTxtLib.GetColumn(); ++nCol)
	{
		pszData = csExcelTxtLib.GetData(nCol, nKeyRow);
		if(!pszData)
			continue;
		if(!strcmp(pszData,			strIP_Level		)) aColumn[ nCol ] = IV_LEVEL		;
		else if(!strcmp(pszData,	strIP_ItemTID	)) aColumn[ nCol ] = IV_ITEMTID		;
		else if(!strcmp(pszData,	strIP_ItemCount	)) aColumn[ nCol ] = IV_ITEMCOUNT	;
		else if(!strcmp(pszData,	strIP_Cost		)) aColumn[ nCol ] = IV_COST		;
		else if(!strcmp(pszData,	strIP_Rank1		)) aColumn[ nCol ] = IV_RANK1		;
		else if(!strcmp(pszData,	strIP_Rank2		)) aColumn[ nCol ] = IV_RANK2		;
		else if(!strcmp(pszData,	strIP_Rank3		)) aColumn[ nCol ] = IV_RANK3		;
		else if(!strcmp(pszData,	strIP_Rank4		)) aColumn[ nCol ] = IV_RANK4		;
		else if(!strcmp(pszData,	strIP_NeedCharisma))	aColumn[ nCol ] = IV_NEEDCHARISMA;
	}


	for(INT16 nRow = 1; nRow < csExcelTxtLib.GetRow(); ++nRow)
	{
		pszData = csExcelTxtLib.GetData(nTNameColumn, nRow);
		if(!pszData)
		{
			continue;
		}

		AgpdGachaDropTable::DropInfo	stInfo;
		INT32	nLevel = 0;

		FLOAT	fRank1 = 0.0f , fRank2 = 0.0f, fRank3 = 0.0f, fRank4= 0.0f;

		for(INT16 nCol = 0; nCol < csExcelTxtLib.GetColumn(); ++nCol)
		{
			CHAR* pszValue = csExcelTxtLib.GetData(nCol, nRow);

			INT32	 nValue = 0 ;
			if( pszValue )
			{
				nValue = atoi( pszValue );
			}

			switch( aColumn[ nCol] )
			{
			case IV_LEVEL		: nLevel = nValue; break;
			case IV_ITEMTID		: stInfo.nItemTID = nValue; break;
			case IV_ITEMCOUNT	: stInfo.nItemNeeded = nValue; break;
			case IV_COST		: stInfo.nCost = nValue; break;
			case IV_NEEDCHARISMA: stInfo.nNeedCharisma = nValue; break;
			case IV_RANK1		: fRank1 = ( float )nValue / 10000.0f; break;
			case IV_RANK2		: fRank2 = ( float )nValue / 10000.0f; break;
			case IV_RANK3		: fRank3 = ( float )nValue / 10000.0f; break;
			case IV_RANK4		: fRank4 = ( float )nValue / 10000.0f; break;
			}
		}

		if( nLevel )
		{
			// 확율 미리 계산해둠..
			stInfo.fRank4 = fRank4;
			stInfo.fRank3 = fRank4 + fRank3;
			stInfo.fRank2 = fRank4 + fRank3 + fRank2;

			mapGacha[ nLevel ] = stInfo;
		}
		else
		{
			// 레벨이 이상하다?..
			csExcelTxtLib.CloseFile();
			return FALSE;
		}
	}

	csExcelTxtLib.CloseFile();

	return TRUE;
}

BOOL	AgpmEventGacha::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpmEventGacha *		pThis		= (AgpmEventGacha *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	INT32					nGachaType	= PtrToInt( pstEvent->m_pvData );
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;

	const string strGachaStart	= "GachaStart"	;
	const string strGachaEnd	= "GachaEnd"	;
	const string strGachaType	= "Type"		;

	//시작~
	if (!pstStream->WriteValue(strGachaStart.c_str(), 0))
		return FALSE;

	//템플릿이름 저장.
	if (!pstStream->WriteValue(strGachaType.c_str(), nGachaType ))
		return FALSE;

	//끝~
	if (!pstStream->WriteValue(strGachaEnd.c_str(), 0))
		return FALSE;

	return TRUE;
}

BOOL	AgpmEventGacha::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	const string strGachaStart	= "GachaStart"	;
	const string strGachaEnd	= "GachaEnd"	;
	const string strGachaType	= "Type"		;

	AgpmEventGacha *		pThis		= (AgpmEventGacha *) pClass;
	ApdEvent *				pstEvent	= (ApdEvent *) pData;
	ApModuleStream *		pstStream	= (ApModuleStream *) pCustData;

	const CHAR *			szValueName;

	if (!pstStream->ReadNextValue())
		return TRUE;

	szValueName = pstStream->GetValueName();
	if (_stricmp(szValueName, strGachaStart.c_str()))
		return TRUE;

	while (pstStream->ReadNextValue())
	{
		szValueName = pstStream->GetValueName();

		if( !_stricmp(szValueName, strGachaType.c_str()) )
		{
			INT32	nType;
			pstStream->GetValue( &nType );
			pstEvent->m_pvData = IntToPtr( nType );
		}
		else if(!_stricmp(szValueName, strGachaEnd.c_str()))
		{
			break;
		}
	}

	return TRUE;
}

INT32	AgpmEventGacha::GetGachaItem( AgpdCharacter * pcsCharacter , INT32 nGachaType )
{
	if( !pcsCharacter ) return 0;

	// 가차 타입..
	// 드롭테이블을 참고해서 확율 계산..

	AgpdGachaType * pGachaType = GetGachaTypeInfo( nGachaType );
	AuRace stRace = m_pcsAgpmCharacter->GetCharacterRace( pcsCharacter );

	ASSERT( pGachaType );
	if( !pGachaType ) return 0;

	AgpdGachaItemTable	* pItemTable = pGachaType->GetGachItemTable( pcsCharacter );

	ASSERT( pItemTable );
	if( !pItemTable ) return 0;

	INT32 nLevel = m_pcsAgpmCharacter->GetLevel( pcsCharacter );


	// 확율 정보
	AgpdGachaDropTable::DropInfo * pDropInfo = &pGachaType->stDropTable.mapGacha[ nLevel ];

	AgpdGachaItemTable::Table * pTable = &pItemTable->mapItemTable[ nLevel ];

	// 랜덤 확율 뽑아냄.

	MTRand	mtRnd;
	// Rank 결정

	FLOAT	fRandValue = ( FLOAT ) mtRnd.rand( 1.0 );

	INT32	nRank = 1;

	if( fRandValue < pDropInfo->fRank4 )		nRank = 4;
	else if( fRandValue < pDropInfo->fRank3 )	nRank = 3;
	else if( fRandValue < pDropInfo->fRank2 )	nRank = 2;
	else										nRank = 1;

	// 아이템 벡터
	vector< INT32 > * pItemVector = &(*pTable)[ nRank ];

	// 랜덤 확율 뽑아냄

	INT32	nTID;
	INT32	nItemCount = ( INT32 ) pItemVector->size();

	if( nItemCount )
	{
		nTID	= (*pItemVector)[ mtRnd.randInt( nItemCount ) ];
		return nTID;
	}
	else
	{
		ASSERT( !"해당 레벨& 해당 랭크에 적당한 아이템이 없다. 기획 문제" );
		return 0;
	}
}

AgpmEventGacha::ERROR_CODE	AgpmEventGacha::CheckProperGacha( AgpdCharacter * pcsCharacter , INT32 nGachaType , vector< INT32 > * pVectorItem )
{
// 가차가 가능한지 확인함.
	ASSERT( pcsCharacter );

	if(!pcsCharacter)
		return EC_GACHA_ERROR;

	AgpmEventGacha::ERROR_CODE eErrorCode = EC_NOERROR;

	AgpdGachaType * pGachaType = GetGachaTypeInfo( nGachaType );
	AuRace stRace = m_pcsAgpmCharacter->GetCharacterRace( pcsCharacter );

	ASSERT( pGachaType );
	if( !nGachaType ) return EC_GACHA_ERROR;

	AgpdGachaItemTable	* pItemTable = pGachaType->GetGachItemTable( pcsCharacter );
	ASSERT( pItemTable );
	if( !pItemTable ) return EC_GACHA_ERROR;

	INT32 nLevel = m_pcsAgpmCharacter->GetLevel( pcsCharacter );

	// 확율 정보
	AgpdGachaDropTable::DropInfo * pDropInfo = &pGachaType->stDropTable.mapGacha[ nLevel ];
	// pGachaType

	// 비용확인
	// 1, 아이템이 있으면 인벤에 아이템이 있는지 확인
	// 2, 물론 갯수도 확인
	// 3, 돈이 있으면 돈을 충분히 가지고 있는지 확인.
	// 4, 그리고 인벤에 여유가 있는지 확인.

	// 가챠 아이템 가능 확인부
	// 우선 필요한 아이템 - pDropInfo->nItemTID
	// 그 아이템이 필요갯수 - pDropInfo->nItemNeeded
	// 필요 금액 - pDropInfo->nCost

	if( pDropInfo->nItemTID && !eErrorCode )
	{
		// 인벤토리에
		// pDropInfo->nItemTID 이것이 pDropInfo->nItemNeeded개 이상인지 확인
		// 이상있을경우 return EC_NOT_ENOUGH_ITEM;

		AgpdItemStatus eItemStatus = AGPDITEM_STATUS_INVENTORY; //국내의 경우 그냥 인벤에서 가챠이용권 조회
		
		if (g_eServiceArea == AP_SERVICE_AREA_CHINA || g_eServiceArea == AP_SERVICE_AREA_JAPAN)
			eItemStatus = AGPDITEM_STATUS_CASH_INVENTORY; //일본 /중국의 경우 캐쉬 인벤에서 가챠이용권 조회

		if(pDropInfo->nItemNeeded > g_pcsAgpmItem->GetItemTotalCountFromGrid(pcsCharacter, pDropInfo->nItemTID, eItemStatus))
		{
			eErrorCode = EC_NOT_ENOUGH_ITEM;
		}
	}

	if( pDropInfo->nCost && !eErrorCode )
	{
		// 비용이 있는 경우
		// 돈을 충분이 가지고 있는지 확인.
		// 이상있을경우 return EC_NOT_ENOUGH_MONEY;

		INT64 llMoney = 0;
		m_pcsAgpmCharacter->GetMoney(pcsCharacter, &llMoney);
		if((INT64)pDropInfo->nCost > llMoney)
			eErrorCode = EC_NOT_ENOUGH_MONEY;
	}
	
	if( pDropInfo->nNeedCharisma && !eErrorCode )
	{
		INT64 llCharisma = 0;
		llCharisma = m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter);
		if((INT64)pDropInfo->nNeedCharisma > llCharisma)
			eErrorCode = EC_NOT_ENOUGH_CHARISMA;
	}

	// 인벤토리가 여유가 있는지 확인.
	AgpdItemADChar* pcsItemADChar = g_pcsAgpmItem->GetADCharacter(pcsCharacter);
	if(!pcsItemADChar || g_pcsAgpmGrid->IsFullGrid(&pcsItemADChar->m_csInventoryGrid)  && !eErrorCode )
		eErrorCode = EC_NOT_ENOUGH_INVENTORY;

	// 가능한 TID를 리스팅함
	if( pVectorItem )
	{
		pVectorItem->clear();

		AgpdGachaItemTable::Table * pTable = &pItemTable->mapItemTable[ nLevel ];

		AgpdGachaItemTable::Table::iterator iter3;
		for( iter3 = pTable->begin();
			iter3 != pTable->end();
			iter3++)
		{
			INT32	nRank = iter3->first;
			vector< INT32 > * pItemVector = &iter3->second;

			//*pVectorItem += *pItemVector;

			//TODO:
			// 뒤에 추가로 넣는 걸 하는 작업 같음; 2008.02.01. steeple
			// 아래가 되는 지는 모르겠음;; 테스트 필요
			pVectorItem->insert( pVectorItem->end(), pItemVector->begin() , pItemVector->end() );
		}


		/*
	#ifdef _DEBUG
		// 디버그 아이템 추가

		pVectorItem->push_back( 3034 );
		pVectorItem->push_back( 4624 );
		pVectorItem->push_back( 4625 );
		pVectorItem->push_back( 11 );
		pVectorItem->push_back( 1965 );
		pVectorItem->push_back( 18 );
		pVectorItem->push_back( 488 );
		pVectorItem->push_back( 1895 );
		pVectorItem->push_back( 51 );
		pVectorItem->push_back( 4001 );
	#endif
	*/
		if( pVectorItem->size() == 0 )
		{
			return EC_GACHA_ERROR;
		}
	}

	return eErrorCode;
}
