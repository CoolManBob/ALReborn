/*===============================================================

	AgsmRelay2UIStatus.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"


//
//	==========		UI Status		==========
//
void AgsmRelay2::InitPacketUIStatus()
	{
	m_csPacketUIStatus.SetFlagLength(sizeof(INT8));
	m_csPacketUIStatus.SetFieldType(AUTYPE_INT16,			1,							// eAgsmRelay2Operation
									AUTYPE_CHAR,			_MAX_CHARNAME_LENGTH + 1,	// Owner ID
									AUTYPE_MEMORY_BLOCK,	1,							// encoded QBelt string
									AUTYPE_MEMORY_BLOCK,	1,							// encoded Cooldown string
									AUTYPE_END,				0
									);
	}


BOOL AgsmRelay2::OnParamUIStatus(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2UIStatus *pcsRelayUIStatus = new AgsdRelay2UIStatus;

	CHAR	*pszCharName = NULL;
	CHAR	*pQuickBelt = NULL;
	INT16	nQuickBeltLength = 0;

	CHAR	*pCooldown = NULL;
	INT16	nCooldownLength = 0;

	m_csPacketUIStatus.GetField(FALSE, pvPacket, 0,
								&pcsRelayUIStatus->m_eOperation,
								&pszCharName,
								&pQuickBelt,
								&nQuickBeltLength,
								&pCooldown,
								&nCooldownLength);

	if (!pszCharName || _T('\0') == *pszCharName)
		{
		pcsRelayUIStatus->Release();

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "!!!Error : CharID is NULL in OnParamUIStatus()\n");
		AuLogFile_s(AGMSRELAY2_LOG_FILENAME1, strCharBuff);
		
		return FALSE;
		}


	pcsRelayUIStatus->m_ulNID = ulNID;
	_tcsncpy(pcsRelayUIStatus->m_szCharName, pszCharName, _MAX_CHARNAME_LENGTH);
	if(pQuickBelt && nQuickBeltLength > 0 && nQuickBeltLength <= _MAX_QUICKBELT_LENGTH)
		_tcsncpy(pcsRelayUIStatus->m_szQuickBelt, pQuickBelt, nQuickBeltLength);
	if(pCooldown && nCooldownLength > 0 && nCooldownLength <= _MAX_COOLDOWN_LENGTH)
		_tcsncpy(pcsRelayUIStatus->m_szCooldown, pCooldown, nCooldownLength);

	return EnumCallback(AGSMRELAY_PARAM_UI_STATUS, (PVOID)pcsRelayUIStatus, (PVOID)nParam);
	}


BOOL AgsmRelay2::CBUIStatusUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmRelay2		*pThis			= (AgsmRelay2 *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->SendUIStatusUpdate(pcsCharacter);
	}


BOOL AgsmRelay2::SendUIStatusUpdate(AgpdCharacter *pcsCharacter)
	{
	if (!pcsCharacter || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("SendUIStatusUpdate"));

	CHAR	szStringBuffer[AGPMUISTATUS_MAX_QUICKBELT_STRING + 1];
	ZeroMemory(szStringBuffer, sizeof(szStringBuffer));

	if (!m_pAgsmUIStatus->EncodingQBeltString(pcsCharacter, szStringBuffer, AGPMUISTATUS_MAX_QUICKBELT_STRING))
		return FALSE;

	INT16	nStringLength	= (INT16)_tcslen(szStringBuffer);
	if (nStringLength > AGPMUISTATUS_MAX_QUICKBELT_STRING)
		return FALSE;

	CHAR szCooldown[_MAX_COOLDOWN_LENGTH + 1];
	ZeroMemory(szCooldown, sizeof(szCooldown));
	if(!m_pAgsmItem->EncodingCooldown(pcsCharacter, szCooldown, _MAX_COOLDOWN_LENGTH))
		return FALSE;

	INT16 nCooldownLength = (INT16)_tcslen(szCooldown);
	if(nCooldownLength > _MAX_COOLDOWN_LENGTH)
		return FALSE;

	INT16	nOperation = AGSMDATABASE_OPERATION_UPDATE;
	
	INT16	nPacketLength = 0;
	PVOID	pvPacketUIStatus = m_csPacketUIStatus.MakePacket(FALSE, &nPacketLength, 0,
															 &nOperation,
															 pcsCharacter->m_szID,
															 szStringBuffer,
															 &nStringLength,
															 szCooldown,
															 &nCooldownLength
															 );

	if (!pvPacketUIStatus || nPacketLength < 1)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketUIStatus, AGSMRELAY_PARAM_UI_STATUS);	
	m_csPacketUIStatus.FreePacket(pvPacketUIStatus);

	return bResult;
	}




/************************************************************/
/*		The Implementation of AgsdRelay2UIStatus class		*/
/************************************************************/
//
AgsdRelay2UIStatus::AgsdRelay2UIStatus()
	{
	ZeroMemory(m_szCharName, sizeof(m_szCharName));
	ZeroMemory(m_szQuickBelt, sizeof(m_szQuickBelt));
	ZeroMemory(m_szCooldown, sizeof(m_szCooldown));
	}


BOOL AgsdRelay2UIStatus::SetParamUpdate(AuStatement* pStatement)
	{
	pStatement->SetParam(0, m_szQuickBelt, sizeof(m_szQuickBelt));
	pStatement->SetParam(1, m_szCooldown, sizeof(m_szCooldown));
	pStatement->SetParam(2, m_szCharName, sizeof(m_szCharName));

	return TRUE;
	}

