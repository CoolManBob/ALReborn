#ifndef _AGSP_BANK_MONEYEDIT_H
#define _AGSP_BANK_MONEYEDIT_H

#include "AgsdRelay2.h"

struct PACKET_AGSMBANKMONEYEDIT_RELAY : public PACKET_HEADER
{
	CHAR	FlagLow;
	INT16	nParam;
	INT16	nOperation;
	CHAR	strAccountName[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR	strWorldName[AGSM_MAX_SERVER_NAME+1];
	INT64	nBankMoney;

	
	PACKET_AGSMBANKMONEYEDIT_RELAY(CHAR *AccountName, CHAR *WorldName, INT64 BankMoney)
		:nParam(0), FlagLow(1), nOperation(0)
	{
		cType			= AGSMRELAY_PACKET_TYPE;
		nParam			= AGSMRELAY_PARAM_ADMIN_EDIT_BANK_MONEY;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSMBANKMONEYEDIT_RELAY);

		memset(strAccountName, 0, sizeof(strAccountName));
		strcpy(strAccountName, AccountName);

		memset(strWorldName, 0, sizeof(strWorldName));
		strcpy(strWorldName, WorldName);
		
		nBankMoney = BankMoney;
	}
};

#endif //_AGSP_BANK_MONEYEDIT_H