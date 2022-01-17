#include "AgcmWorld.h"

AgcmWorld::AgcmWorld()
{
	SetModuleName(_T("AgcmWorld"));
	SetModuleType(APMODULE_TYPE_CLIENT);

	m_pAgpdWorldSelected	= NULL;
	m_nIndexAD				= 0;

	ZeroMemory( m_szWorldCharCount, sizeof(m_szWorldCharCount) );
	m_bReceiveWorld			= FALSE;

	m_vecWorldGroup.clear();
}

AgcmWorld::~AgcmWorld()
{
	m_vecWorldGroup.clear();
}

BOOL AgcmWorld::OnAddModule()
{
	m_pAgpmWorld = (AgpmWorld *) GetModule(_T("AgpmWorld"));
	if ( !m_pAgpmWorld )		return FALSE;

	m_nIndexAD = m_pAgpmWorld->SetAttachedModuleData(this, AGPMWORLD_DATA_TYPE_PUBLIC/*AGPMWORLD_DATA_TYPE_CLIENT*/, sizeof(AgcdWorld), ConAgcdWorld, DesAgcdWorld);
	if( m_nIndexAD < 0 )		return FALSE;

	if( !m_pAgpmWorld->SetCallbackResultGetWorldAll(CBGetWorldAll, this) )
		return FALSE;

	if( !m_pAgpmWorld->SetCallbackResultGetCharacterCount(CBGetCharCount, this) )
		return FALSE;

	return TRUE;
}	

BOOL AgcmWorld::ConAgcdWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmWorld*	pThis	= (AgcmWorld *) pClass;
	ApBase*		pApBase	= (ApBase *) pData;

	AgcdWorld*	pAgcdWorld = pThis->GetAD(pApBase);
	if( !pAgcdWorld )	return FALSE;

	pAgcdWorld->m_lCharacterCount = 0;
	return TRUE;	
}

BOOL AgcmWorld::DesAgcdWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmWorld	*pThis		= (AgcmWorld *) pClass;
	ApBase		*pApBase	= (ApBase *) pData;

	AgcdWorld *pAgcdWorld = pThis->GetAD(pApBase);
	if( !pAgcdWorld )		return FALSE;

	pAgcdWorld->m_lCharacterCount = 0;
	return TRUE;
}

AgcdWorld* AgcmWorld::GetAD(ApBase *pApBase)
{
	return pApBase ? (AgcdWorld *) m_pAgpmWorld->GetAttachedModuleData(m_nIndexAD, (PVOID) pApBase) : NULL;
}

BOOL AgcmWorld::SendPacketGetWorld(TCHAR* pszWorld, UINT32 ulNID)
{
	BOOL	bResult = FALSE;
	INT16	nLength;
	INT8	cOperation = AGPMWORLD_PACKET_OPERATION_GETWOLRD;
	INT16	nStatus = AGPDWORLD_STATUS_UNKNOWN;

	PVOID	pvPacket = m_pAgpmWorld->m_csPacket.MakePacket(TRUE, &nLength, AGPMWORLD_PACKET_TYPE, &cOperation, pszWorld, &nStatus, NULL );
	if( pvPacket && SendPacket(pvPacket, nLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, ulNID) )
		bResult = TRUE;
	m_pAgpmWorld->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmWorld::SendPacketGetCharCount(TCHAR *pszAccount, UINT32 ulNID)
{
	BOOL	bResult = FALSE;
	INT16	nLength;
	INT8	cOperation = AGPMWORLD_PACKET_OPERATION_GET_CHAR_COUNT;

	PVOID	pvPacket = m_pAgpmWorld->m_csPacket.MakePacket(TRUE, &nLength, AGPMWORLD_PACKET_TYPE, &cOperation, pszAccount, NULL, NULL );
	if( pvPacket && SendPacket(pvPacket, nLength, ACDP_SEND_LOGINSERVER, PACKET_PRIORITY_NONE, ulNID) )
		bResult = TRUE;
	m_pAgpmWorld->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmWorld::CBGetWorldAll(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass )		return FALSE;

	AgcmWorld *pThis = (AgcmWorld *) pClass;	

	// if first receiveing of world list
	// and already received character count of each worlds
	if (FALSE == pThis->m_bReceiveWorld && _tcslen(pThis->m_szWorldCharCount) > 0)
		pThis->_DecodeWorldCharCount();

	pThis->m_bReceiveWorld = TRUE;

	return TRUE;
}

BOOL AgcmWorld::CBGetCharCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgcmWorld *pThis = (AgcmWorld *) pClass;
	TCHAR *pszEncoded = (TCHAR *) pData;
	INT32 lLength = *((INT32 *) pCustData);

	INT32 lCopyLength = min(2048, lLength);

	_tcsncpy(pThis->m_szWorldCharCount, pszEncoded, lCopyLength);

	if (pThis->m_bReceiveWorld)
		pThis->_DecodeWorldCharCount();

	return TRUE;
}

BOOL AgcmWorld::SetCallbackGetCharCount(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMWORLD_CB_GET_CHAR_COUNT, pfCallback, pClass);
}

void	AgcmWorld::ResetWorldGroup()
{
	for( WorldGroupVecItr Itr = m_vecWorldGroup.begin(); Itr != m_vecWorldGroup.end(); ++Itr )
		(*Itr).m_vecWorld.clear();

	for(int i=0; i<s_dwWorldMax; ++i )
	{
		AgpdWorld* pWorld = m_pAgpmWorld->GetWorld(i);
		if( !pWorld )	continue;
		
		WorldGroupVecItr Itr = find( m_vecWorldGroup.begin(), m_vecWorldGroup.end(), pWorld->m_szGroupName );
		if( Itr != m_vecWorldGroup.end() )
		{
			(*Itr).m_vecWorld.push_back( pWorld );
		}
		else
		{
			m_vecWorldGroup.push_back( AgcdWorldGroup( pWorld ) );
		}
	}

	sort( m_vecWorldGroup.begin(), m_vecWorldGroup.end(), greater<AgcdWorldGroup>() );
}

#define _ctextend				_T('\0')

BOOL AgcmWorld::_DecodeWorldCharCount()
{
	TCHAR*	psz = m_szWorldCharCount;
	TCHAR	szWorld[AGPDWORLD_MAX_WORLD_NAME + 1];
	TCHAR	szCount[21];
	TCHAR*	pszBuf;

	INT32	lCount = 0;

	while (_ctextend != *psz)
	{
		// world
		pszBuf = szWorld;
		while (_ctextend != *psz && AGPMWORLD_DELIM1 != *psz)
			*pszBuf++ = *psz++;

		if (AGPMWORLD_DELIM1 == *psz)
			psz++; // skip

		*pszBuf = _ctextend;

		// status
		pszBuf = szCount;
		while (_ctextend != *psz && AGPMWORLD_DELIM2 != *psz)
			*pszBuf++ = *psz++;

		if (AGPMWORLD_DELIM2 == *psz)
			psz++; // skip

		*pszBuf = _ctextend;

		lCount = _ttoi(szCount);

		AgpdWorld *pAgpdWorld = m_pAgpmWorld->GetWorld(szWorld);
		if (pAgpdWorld)
		{
			AgcdWorld *pAgcdWorld = GetAD(pAgpdWorld);
			if (pAgcdWorld)
				pAgcdWorld->m_lCharacterCount = lCount;
		}
	}	

	return EnumCallback(AGCMWORLD_CB_GET_CHAR_COUNT, NULL, NULL);
}

#undef _ctextend

