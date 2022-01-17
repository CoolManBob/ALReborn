#include "AgsmRelay2.h"
#include "AgspBankMoneyEdit.h"

BOOL AgsmRelay2::OnParamBankMoneyEdit(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	CHAR	strBankMoney[_I64_STR_LENGTH + 1];

	PACKET_AGSMBANKMONEYEDIT_RELAY* pPacket = (PACKET_AGSMBANKMONEYEDIT_RELAY*)pvPacket;

	AgsdRelay2BankMoneyEdit *pcsBankMoneyEdit = new AgsdRelay2BankMoneyEdit(ulNID, pPacket->nParam);

	strncpy(pcsBankMoneyEdit->strAccountName, pPacket->strAccountName, AGSMACCOUNT_MAX_ACCOUNT_NAME);
	strncpy(pcsBankMoneyEdit->strServerName, pPacket->strWorldName, _MAX_SERVERNAME_LENGTH);
	_i64toa(pPacket->nBankMoney, strBankMoney, 10);
	strncpy(pcsBankMoneyEdit->strBankMoney, strBankMoney, _I64_STR_LENGTH);

	AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
	pQuery->m_nIndex = AGSMRELAY_QUERY_BANKMONEYEDIT;
	pQuery->m_pParam = pcsBankMoneyEdit;

	m_pAgsmDatabasePool->Execute(pQuery);
	
	return TRUE;
}

AgsdRelay2BankMoneyEdit::AgsdRelay2BankMoneyEdit(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;

	memset(strServerName, 0, sizeof(strServerName));
	memset(strAccountName, 0, sizeof(strAccountName));
	memset(strBankMoney, 0, sizeof(strBankMoney));

}

AgsdRelay2BankMoneyEdit::~AgsdRelay2BankMoneyEdit()
{
	
}

void AgsdRelay2BankMoneyEdit::Release()
{ 
	delete this;
}

BOOL AgsdRelay2BankMoneyEdit::SetParamExecute(AuStatement* pStatement)
{
	pStatement->SetParam( 0, strBankMoney, sizeof(strBankMoney) );
	pStatement->SetParam( 1, strAccountName, sizeof(strAccountName) );
	pStatement->SetParam( 2, strServerName, sizeof(strServerName) );

	return TRUE;
}