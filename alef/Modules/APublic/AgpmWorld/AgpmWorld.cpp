#include "AgpmWorld.h"

INT16	g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_NUM] =	{
	/*
	-1,			// UNKNOWN
	300,		// GOOOOD
	500,		// AN
	1000,		// N
	2000,		// BN
	5000		// BAD
	*/

	0,			// UNKNOWN
	5,			// GOOOOD
	13,			// AN
	38,			// N
	94,			// BN
	95			// BAD

};

//--------------------------- AgpaWorld ---------------------------
AgpdWorld* AgpaWorld::AddWorld(AgpdWorld *pcsWorld, TCHAR* pszName)
{
	if( !pcsWorld || !AddObject( (PVOID) &pcsWorld, GetObjectCount() + 1, pcsWorld->m_szName )) 
		return NULL;

	return pcsWorld;
}

BOOL AgpaWorld::RemoveWorld(TCHAR* pszName)
{
	return GetWorld(pszName) ? RemoveObject(pszName) : FALSE;
}
AgpdWorld* AgpaWorld::GetWorld(TCHAR* pszName)
{
	AgpdWorld** ppcsWorld = (AgpdWorld**) GetObject(pszName);
	return ppcsWorld ? *ppcsWorld : NULL ;
}

AgpdWorld* AgpaWorld::GetWorld(INT32 lIndex)
{
	AgpdWorld** ppcsWorld = (AgpdWorld **) GetObject(lIndex + 1);
	return ppcsWorld ? *ppcsWorld : NULL;
}

//--------------------------- AgpmWorld ---------------------------
AgpmWorld::AgpmWorld()
{
	SetModuleName( _T("AgpmWorld") );
	SetModuleType( APMODULE_TYPE_PUBLIC );
	SetModuleData( sizeof(AgpdWorld), AGPMWORLD_DATA_TYPE_PUBLIC );
	SetPacketType( AGPMWORLD_PACKET_TYPE );

	// packet
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8, 1,							// operation
		AUTYPE_CHAR,			AGPDWORLD_MAX_WORLD_NAME,	// world
		AUTYPE_INT16,			1,							// status
		AUTYPE_MEMORY_BLOCK,	1,							// encoded world:status list
		AUTYPE_END,				0
		);
}

BOOL AgpmWorld::OnAddModule()
{
	return TRUE;
}	

BOOL AgpmWorld::OnInit()
{
	if ( !m_csAgpaWorld.InitializeObject( sizeof(AgpdWorld *), m_csAgpaWorld.GetCount(), NULL, NULL, this ) )
		return FALSE;

	return TRUE;
}

BOOL AgpmWorld::OnDestroy()
{
	INT32	lIndex = 0;
	AgpdWorld **ppcsAgpdWorld = NULL;
	while ( ppcsAgpdWorld = (AgpdWorld **) m_csAgpaWorld.GetObjectSequence(&lIndex) )
	{
		m_csAgpaWorld.RemoveWorld( (*ppcsAgpdWorld)->m_szName );
		_DestroyWorldData( *ppcsAgpdWorld );
	}
	return TRUE;
}

BOOL AgpmWorld::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8		cOperation = -1;
	TCHAR*		pszWorld = NULL;
	INT16		nStatus = AGPDWORLD_STATUS_UNKNOWN;
	BOOL		bResult = FALSE;
	TCHAR*		pszEncodedWorld = NULL;
	INT32		lEncodedWorldLength = 0;

	m_csPacket.GetField( TRUE, pvPacket, nSize, &cOperation, &pszWorld, &nStatus, &pszEncodedWorld, &lEncodedWorldLength );
	switch (cOperation)
	{
	case AGPMWORLD_PACKET_OPERATION_GETWOLRD:
		return EnumCallback(AGPMWORLD_CB_GET_WORLD, pszWorld, UintToPtr(ulNID));
	case AGPMWORLD_PACKET_OPERATION_RESULT_GETWORLD:
		return UpdateWorld(pszWorld, nStatus) ? TRUE : FALSE;
	case AGPMWORLD_PACKET_OPERATION_RESULT_GETWORLD_ALL:
		return _OnResultGetWorldAll(pszEncodedWorld, lEncodedWorldLength);
	case AGPMWORLD_PACKET_OPERATION_GET_CHAR_COUNT:
		return EnumCallback(AGPMWORLD_CB_GET_CHAR_COUNT, pszWorld, (PVOID) &ulNID);
	case AGPMWORLD_PACKET_OPERATION_RESULT_CHAR_COUNT:
		return EnumCallback(AGPMWORLD_CB_GET_RESULT_CHAR_COUNT, pszEncodedWorld, (PVOID) &lEncodedWorldLength);
	default:
		break;
	}

	return bResult;
}	

AgpdWorld* AgpmWorld::_CreateWorldData()
{
	AgpdWorld* pcsAgpdWorld = (AgpdWorld *)CreateModuleData(AGPMWORLD_DATA_TYPE_PUBLIC);
	if ( pcsAgpdWorld )
		pcsAgpdWorld->Initialize();

	return pcsAgpdWorld;
}

BOOL AgpmWorld::_DestroyWorldData(AgpdWorld* pcsAgpdWorld)
{
	pcsAgpdWorld->Cleanup();
	return DestroyModuleData((PVOID) pcsAgpdWorld, AGPMWORLD_DATA_TYPE_PUBLIC);
}

AgpdWorld* AgpmWorld::UpdateWorld(TCHAR* pszWorld, INT16 nStatus, TCHAR* pszGroup, UINT32 m_ulPriority, UINT32 ulFlag, BOOL bEnum)
{
	// if exist then update, else insert
	AgpdWorld* pcsAgpdWorld = m_csAgpaWorld.GetWorld(pszWorld);
	if ( pcsAgpdWorld )
	{
		pcsAgpdWorld->m_Mutex.WLock();
		pcsAgpdWorld->m_nStatus = nStatus;
		if( AGPMWORLD_FLAG_NONE != ulFlag )
			pcsAgpdWorld->m_ulFlag = ulFlag;
		pcsAgpdWorld->m_Mutex.Release();
	}
	else
	{
		pcsAgpdWorld = _CreateWorldData();
		if( pszWorld )	
			_tcsncpy( pcsAgpdWorld->m_szName, pszWorld, AGPDWORLD_MAX_WORLD_NAME );
		pcsAgpdWorld->m_nStatus		= nStatus;
		if( pszGroup )
			_tcsncpy( pcsAgpdWorld->m_szGroupName, pszGroup, AGPDWORLD_MAX_WORLD_NAME );
		pcsAgpdWorld->m_ulPriority	= m_ulPriority;
		if (AGPMWORLD_FLAG_NONE != ulFlag)
			pcsAgpdWorld->m_ulFlag = ulFlag;
			
		m_csAgpaWorld.AddWorld(pcsAgpdWorld, pszWorld);
	}

	if ( bEnum )
		EnumCallback(AGPMWORLD_CB_UPDATE_WORLD, pcsAgpdWorld, NULL);

	return pcsAgpdWorld;
}

BOOL AgpmWorld::SetCallbackUpdateWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMWORLD_CB_UPDATE_WORLD, pfCallback, pClass);
}

BOOL AgpmWorld::SetCallbackResultGetWorldAll(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMWORLD_CB_RESULT_GET_WORLD_ALL, pfCallback, pClass);
}

BOOL AgpmWorld::SetCallbackGetWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMWORLD_CB_GET_WORLD, pfCallback, pClass);
}

BOOL AgpmWorld::SetCallbackGetCharacterCount(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMWORLD_CB_GET_CHAR_COUNT, pfCallback, pClass);
}

BOOL AgpmWorld::SetCallbackResultGetCharacterCount(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMWORLD_CB_GET_RESULT_CHAR_COUNT, pfCallback, pClass);
}

typedef vector< string >	StringVec;
typedef StringVec::iterator	StringVecItr;
void	MyStrTok( StringVec& vecString, char* szText, char* szDetermine )
{
	if( !szText || !szDetermine )	return;

	vecString.clear();
	char* szTok = strtok( szText, szDetermine );
	while( szTok )
	{
		vecString.push_back( szTok );
		szTok = strtok( NULL, szDetermine );
	}
}

//Name, GroupName, Priority, Status, Flag
BOOL AgpmWorld::_OnResultGetWorldAll(TCHAR* pszEncodedWorld, INT32 lEncodedWorldLength)
{
	if ( !pszEncodedWorld )		return FALSE;
	ASSERT( (_tcslen(pszEncodedWorld)+1) == lEncodedWorldLength );

	StringVec vecWorldInfo;
	MyStrTok( vecWorldInfo, pszEncodedWorld, ";" );
	for( StringVecItr Itr = vecWorldInfo.begin(); Itr != vecWorldInfo.end(); ++Itr )
	{
		StringVec	vecDetailInfo;
		MyStrTok( vecDetailInfo, (char*)(*Itr).c_str(), "=" );

		if( vecDetailInfo.size() != 5 )	continue;

		////이걸 가지고 셋팅하면 될꺼야.. ^^
		UpdateWorld( (char*)vecDetailInfo[0].c_str(), 
					 atoi( vecDetailInfo[3].c_str() ),
					 (char*)vecDetailInfo[1].c_str(),
					 atoi( vecDetailInfo[2].c_str() ),
					 atoi( vecDetailInfo[4].c_str() ),
					 FALSE );
	}

	return EnumCallback(AGPMWORLD_CB_RESULT_GET_WORLD_ALL, NULL, NULL);
}
