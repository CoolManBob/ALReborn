#include "AgsmRelay2.h"
#include "AgspCharismaPointEdit.h"

BOOL AgsmRelay2::OnParamCharismaPointEdit(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	PACKET_AGSMCHARISMAPOINTEDIT_RELAY* pPacket = (PACKET_AGSMCHARISMAPOINTEDIT_RELAY*)pvPacket;

	AgsdRelay2CharismaPointEdit *pcsCharismaPointEdit = new AgsdRelay2CharismaPointEdit(ulNID, pPacket->nParam);

	strncpy(pcsCharismaPointEdit->strCharName, pPacket->strCharName, AGPACHARACTER_MAX_ID_STRING);
	pcsCharismaPointEdit->nCharismaPoint = pPacket->nCharismaPoint;

	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = AGSMRELAY_QUERY_CHARISMAPOINTEDIT;
	pQuery->m_pParam = pcsCharismaPointEdit;

	m_pAgsmDatabasePool->Execute(pQuery);

	return TRUE;
}

AgsdRelay2CharismaPointEdit::AgsdRelay2CharismaPointEdit(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

}

AgsdRelay2CharismaPointEdit::~AgsdRelay2CharismaPointEdit()
{
	
}

void AgsdRelay2CharismaPointEdit::Release()
{ 
	delete this;
}

BOOL AgsdRelay2CharismaPointEdit::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, &nCharismaPoint);
	pStatement->SetParam( 1, strCharName, sizeof(strCharName) );

	return TRUE;
}