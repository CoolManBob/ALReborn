/*====================================================================

	AgsmRelay2ConcurrentUser.cpp

====================================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"

//
//	==========		Concurrent User		==========
//
void AgsmRelay2::InitPacketConcurrentUser()
	{
	m_csPacketConcurrentUser.SetFlagLength(sizeof(INT8));
	m_csPacketConcurrentUser.SetFieldType(AUTYPE_INT16,		1,			// eAgsmRelay2Operation
										  AUTYPE_CHAR,		21,			// server
										  AUTYPE_CHAR,		21,			// class
										  AUTYPE_INT32,		1,			// current
										  AUTYPE_INT32,		1,			// max
										  AUTYPE_END,		0
										  );
	}


BOOL AgsmRelay2::OnParamConcurrentUser(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2ConcurrentUser *pcsUser = new AgsdRelay2ConcurrentUser;

	CHAR	*pszServer = NULL;
	CHAR	*pszType = NULL;

	m_csPacketConcurrentUser.GetField(FALSE, pvPacket, 0,
									&pcsUser->m_eOperation,		// Operation
									&pszServer,
									&pszType,
									&pcsUser->m_lCurrent,
									&pcsUser->m_lMax
									);
    
	pcsUser->m_ulNID = ulNID;
	strncpy(pcsUser->m_szServer, pszServer, 20);
	strncpy(pcsUser->m_szType, pszType, 20);

	return EnumCallback(AGSMRELAY_PARAM_CONCURRENT_USER, (PVOID)pcsUser, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBConcurrentUser(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2		*pThis			= (AgsmRelay2 *) pClass;
	stAgsdAdminCurrentUser* pCurrent = (stAgsdAdminCurrentUser *) pData;


	AgsdServer *pAgsdServer = NULL;
	CHAR szType[21];
	INT32 lCurrent = 0;
	INT32 lMax = 0;

	pAgsdServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pAgsdServer)
		return FALSE;

	
	strcpy(szType, _T("0"));
	lCurrent = pThis->m_pAgsmCharacter->GetNumOfPlayers(); //pCurrent->m_lTotalUserCount;
	lMax = pCurrent->m_lHighestUserCount;
	pThis->SendConcurrentUser(pAgsdServer->m_szWorld, szType, lCurrent, lMax);


	for (INT16 i=0; i<AGSMADMIN_CHARTID_MAX; ++i)
		{
		sprintf(szType, "%d", AGSDADMIN_CHARTID[i]);
		lCurrent = pCurrent->m_lUserCountTID[i];
		lCurrent = pCurrent->m_lUserCountTID[i];
		pThis->SendConcurrentUser(pAgsdServer->m_szWorld, szType, lCurrent, lMax);
		}

	return TRUE;
	}


BOOL AgsmRelay2::SendConcurrentUser(CHAR *pszServer, CHAR *pszType, INT32 lCurrent, INT32 lMax)
	{
	if (!pszType)
		return FALSE;

	INT16 nPacketLength	= 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketConcurrentUser = m_csPacketConcurrentUser.MakePacket(FALSE, &nPacketLength, 0,
																   &nOperation,
																   pszServer,
																   pszType,
																   &lCurrent,
																   &lMax
																   );
	if (!pvPacketConcurrentUser)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketConcurrentUser, AGSMRELAY_PARAM_CONCURRENT_USER);
	m_csPacketConcurrentUser.FreePacket(pvPacketConcurrentUser);
	
	return bResult;
	}




/********************************************************************/
/*		The Implementation of AgsdRelay2ConcurrentUser class		*/
/********************************************************************/
//
AgsdRelay2ConcurrentUser::AgsdRelay2ConcurrentUser()
	{
	ZeroMemory(m_szServer, sizeof(m_szServer));
	ZeroMemory(m_szType, sizeof(m_szType));
	m_lCurrent = 0;
	m_lMax = 0;
	}


BOOL AgsdRelay2ConcurrentUser::SetParamUpdate(AuStatement* pStatement)
	{
	INT16 i=0;
	pStatement->SetParam(i++, &m_lCurrent);
	pStatement->SetParam(i++, &m_lMax);
	pStatement->SetParam(i++, m_szServer, sizeof(m_szServer));
	pStatement->SetParam(i++, m_szType, sizeof(m_szType));

	return TRUE;
	}

