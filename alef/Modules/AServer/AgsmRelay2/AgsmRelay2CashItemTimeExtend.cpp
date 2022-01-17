#include "AgsmRelay2.h"
#include "AgspCashItemTimeExtend.h"

BOOL AgsmRelay2::OnParamCashItemTimeExtend(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMCASHITEMTIMEEXTEND_RELAY* pPacket = (PACKET_AGSMCASHITEMTIMEEXTEND_RELAY*)pvPacket;

	switch(pPacket->nOperation)
	{
	case AGSMRELAY_PARAM_EXTEND_REQUEST:
		{
			PACKET_AGSMCASHITEMTIMEEXTEND_REQUEST_RELAY* pPacket2 = (PACKET_AGSMCASHITEMTIMEEXTEND_REQUEST_RELAY*)pPacket;

			AgsdRelay2CashItemTimeExtend* pcsCashItemTimeExtend = new AgsdRelay2CashItemTimeExtend(ulNID, pPacket->nOperation);
			pcsCashItemTimeExtend->ulExtendTime = pPacket2->ulExtendTime;

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;			
			pQuery->m_nIndex = AGSMRELAY_QUERY_CASHITEMTIMEEXTEND;
			pQuery->m_pParam = pcsCashItemTimeExtend;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultCashItemTimeExtend, NULL, this, pcsCashItemTimeExtend);

			m_pAgsmDatabasePool->Execute(pQuery);
		}
		break;

	case AGSMRELAY_PARAM_EXTEND_RESULT:
		{
			PACKET_AGSMCASHITEMTIMEEXTEND_RESULT_RELAY* pPacket2 = (PACKET_AGSMCASHITEMTIMEEXTEND_RESULT_RELAY*)pPacket;

			char strMessage[ASSM_MAX_GENERAL_PARAM_STRING];

			itoa(pPacket2->ulExtendTime, strMessage, 10);

			if(pPacket2->nResult)
				strcat(strMessage, "분 연장에 성공하였습니다. - ");
			else
				strcat(strMessage, "분 연장에 실패하였습니다. - ");

			if(m_pAgsmServerManager)
				strcat(strMessage, m_pAgsmServerManager->GetThisServer()->m_szGroupName);

			if(m_pAgsmServerStatus)
				m_pAgsmServerStatus->SendGeneralParamStringPacket(m_pAgsmServerStatus->GetConnectedSocketIndex(), NULL, strMessage);

		}
		break;
	}
	return TRUE;
}

BOOL AgsmRelay2::CBOperationResultCashItemTimeExtend(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmRelay2 *pThis = (AgsmRelay2 *) pClass;
	AgsdRelay2CashItemTimeExtend *pAgsdRelay2 = (AgsdRelay2CashItemTimeExtend *) pCustData;
	AuRowset *pRowset = (AuRowset *) pData;

	PACKET_AGSMCASHITEMTIMEEXTEND_RESULT_RELAY pPacket;
	pPacket.ulExtendTime = pAgsdRelay2->ulExtendTimeResult;
	pPacket.nResult = pAgsdRelay2->m_nCode;

	AgsEngine::GetInstance()->SendPacket(pPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}

AgsdRelay2CashItemTimeExtend::AgsdRelay2CashItemTimeExtend(UINT32 ulNID, INT16 Operation)
	:m_nCode(-1)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	ulExtendTime = 0;
	ulExtendTimeResult = 0;
}

AgsdRelay2CashItemTimeExtend::~AgsdRelay2CashItemTimeExtend()
{

}

void AgsdRelay2CashItemTimeExtend::Release()
{ 
	delete this;
}

BOOL AgsdRelay2CashItemTimeExtend::SetParamExecute(AuStatement* pStatement)
{
	switch(nOperation)
	{
	case AGSMRELAY_PARAM_EXTEND_REQUEST:
		{
			// in
			pStatement->SetParam( 0, &ulExtendTime );

			// out
			pStatement->SetParam( 1, &ulExtendTimeResult, TRUE);
			pStatement->SetParam( 2, &m_nCode, TRUE);
		} break;
	}

	return TRUE;
}