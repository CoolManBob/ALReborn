/*====================================================================

	AgsmRelay2AccountWorld.cpp

====================================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"


//
//	==========		Account World		==========
//
void AgsmRelay2::InitPacketAccountWorld()
	{
	m_csPacketAccountWorld.SetFlagLength(sizeof(INT8));
	m_csPacketAccountWorld.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
										AUTYPE_CHAR,		AGSMACCOUNT_MAX_ACCOUNT_NAME + 1,
										AUTYPE_CHAR,		_MAX_SERVERNAME_LENGTH + 1,		// Server Name
										AUTYPE_INT64,		1,
										AUTYPE_INT8,		1,
										AUTYPE_END,			0
										);
	}


BOOL AgsmRelay2::OnParamAccountWorld(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2AccountWorld* pcsAccount = new AgsdRelay2AccountWorld;

	CHAR	*pszAccountID	= NULL;
	CHAR	*pszServerName	= NULL;
	INT64	llBankMoney		= 0;
	INT8	cBankSize		= 0;

	m_csPacketAccountWorld.GetField(FALSE, pvPacket, 0,
									&pcsAccount->m_eOperation,		// Operation
									&pszAccountID,					// Account ID
									&pszServerName,
									&llBankMoney,					// Bank money
									&cBankSize
									);

	if (!pszAccountID || _T('\0') == *pszAccountID)
		{
		pcsAccount->Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : AccountID is NULL or Empty in OnParamAccountWorld()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);
		return FALSE;
		}

	pcsAccount->m_ulNID = ulNID;
	strncpy(pcsAccount->m_szAccountID, pszAccountID, AGSMACCOUNT_MAX_ACCOUNT_NAME);
	strncpy(pcsAccount->m_szServerName, pszServerName, _MAX_SERVERNAME_LENGTH);
	_i64toa(llBankMoney, pcsAccount->m_szBankMoney, 10);
	pcsAccount->m_cBankSize = cBankSize;

	return EnumCallback(AGSMRELAY_PARAM_ACCOUNT_WORLD, (PVOID)pcsAccount, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBAccountWorldUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && "Invalid Parameter for Update AccountWorldin AgsmRelay2::CBAccountWorldUpdate()");

	AgsmRelay2*	pThis			= (AgsmRelay2 *) pClass;
	AgpdCharacter*	pAgpdCharacter	= (AgpdCharacter *) pData;

	if (pThis->m_pAgpmCharacter->IsReturnToLoginServerStatus(pAgpdCharacter))
		return TRUE;
	
	return pThis->SendAccountWorldUpdate(pThis->m_pAgsmCharacter->GetAccountID(pAgpdCharacter),
										 pThis->m_pAgsmCharacter->GetServerName(pAgpdCharacter),
										 pAgpdCharacter->m_llBankMoney,
										 pAgpdCharacter->m_cBankSize);
	}


BOOL AgsmRelay2::SendAccountWorldUpdate(CHAR *pszAccountID, CHAR *pszServerName, INT64 llBankMoney, INT8 cBankSize)
	{
	if (!pszAccountID || !pszServerName || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	INT16 nPacketLength	= 0;
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketAccountWorld = m_csPacketAccountWorld.MakePacket(FALSE, &nPacketLength, 0,
																   &nOperation,
																   pszAccountID,
																   pszServerName,
																   &llBankMoney,
																   &cBankSize
																   );
	if (!pvPacketAccountWorld)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketAccountWorld, AGSMRELAY_PARAM_ACCOUNT_WORLD);
	m_csPacketAccountWorld.FreePacket(pvPacketAccountWorld);
	
	return bResult;
	}




/****************************************************************/
/*		The Implementation of AgsdRelay2AccountWorld class		*/
/****************************************************************/
//
AgsdRelay2AccountWorld::AgsdRelay2AccountWorld()
	{
	ZeroMemory(m_szAccountID, sizeof(m_szAccountID));
	ZeroMemory(m_szServerName, sizeof(m_szServerName));
	ZeroMemory(m_szBankMoney, sizeof(m_szBankMoney));
	m_cBankSize	= 0;
	}


BOOL AgsdRelay2AccountWorld::SetParamUpdate(AuStatement* pStatement)
	{
	INT16 i=0;
	pStatement->SetParam(i++, m_szBankMoney, sizeof(m_szBankMoney));
	pStatement->SetParam(i++, &m_cBankSize);
	pStatement->SetParam(i++, m_szAccountID, sizeof(m_szAccountID));
	pStatement->SetParam(i++, m_szServerName, sizeof(m_szServerName));

	return TRUE;
	}


BOOL AgsdRelay2AccountWorld::SetParamInsert(AuStatement *pStatement)
	{
	INT16 i=0;
	pStatement->SetParam(i++, m_szAccountID, sizeof(m_szAccountID));
	pStatement->SetParam(i++, m_szServerName, sizeof(m_szServerName));
	pStatement->SetParam(i++, m_szBankMoney, sizeof(m_szBankMoney));
	pStatement->SetParam(i++, &m_cBankSize);

	return TRUE;	
	}


void AgsdRelay2AccountWorld::Dump(CHAR *pszOp)
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, ":LOG\\%04d%02d%02d-%02d-RELAY_DUMP_BANK.log", st.wYear, st.wMonth, st.wDay, st.wHour);


	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s] "
						"Account[%s] Server[%s] BankMoney[%s] BankSize[%d]\n",
						st.wHour, st.wMinute, st.wSecond, pszOp,
						m_szAccountID, m_szServerName, m_szBankMoney, m_cBankSize);
	AuLogFile_s(szFile, strCharBuff);
	}

