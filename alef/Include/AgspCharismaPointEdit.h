#ifndef _AGSP_CHARISMA_POINT_EDIT_H
#define _AGSP_CHARISMA_POINT_EDIT_H

#include "AgsdRelay2.h"

struct PACKET_AGSMCHARISMAPOINTEDIT_RELAY : public PACKET_HEADER
{
	CHAR	FlagLow;
	INT16	nParam;
	INT16	nOperation;
	CHAR	strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32	nCharismaPoint;

	
	PACKET_AGSMCHARISMAPOINTEDIT_RELAY(CHAR *CharName, INT32 CharismaPoint)
		:nParam(0), FlagLow(1), nOperation(0)
	{
		cType			= AGSMRELAY_PACKET_TYPE;
		nParam			= AGSMRELAY_QUERY_CHARISMAPOINTEDIT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSMCHARISMAPOINTEDIT_RELAY);

		memset(strCharName, 0, sizeof(strCharName));
		strcpy(strCharName, CharName);
		
		nCharismaPoint = CharismaPoint;
	}
};

#endif //_AGSP_CHARISMA_POINT_EDIT_H