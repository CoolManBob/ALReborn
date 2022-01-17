#ifndef _AGSPSKILL_H
#define _AGSPSKILL_H

#include "AgsdRelay2.h"

struct PACKET_AGSMSKILL : public PACKET_HEADER
{
	CHAR	FlagLow;
	CHAR	FlagHigh;
	
	INT8	pcOperation;

	PACKET_AGSMSKILL()
		: pcOperation(0), FlagLow(1), FlagHigh(0)
	{
		cType			= AGSMSKILL_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSMSKILL);
	}
};

struct PACKET_AGSMSKILL_EQUIPLIST : public PACKET_AGSMSKILL
{
	INT32	lCID;
	ApSafeArray<AgsdEquipSkill, AGPMSKILL_MAX_SKILL_BUFF> m_csEquipSkillArray;
	INT32	nSkillCount;

	PACKET_AGSMSKILL_EQUIPLIST(INT32 CID)
		: nSkillCount(0)
	{
		pcOperation		= AGSMSKILL_PACKET_OPERATION_EQIP_LIST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSMSKILL_EQUIPLIST);

		lCID = CID;
	}
};

struct PACKET_AGSMSKILL_RELAY : public PACKET_HEADER
{
	CHAR	FlagLow;

	INT16	nParam;

	PACKET_AGSMSKILL_RELAY()
		: nParam(0), FlagLow(1)
	{
		cType			= AGSMRELAY_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSMSKILL_RELAY);
	}
};

struct PACKET_AGSMSKILL_RELAY_SKILLSAVE : public PACKET_AGSMSKILL_RELAY
{
	INT16 nOperation;

	CHAR	strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	AgsdEquipSkill m_pSaveData;

	PACKET_AGSMSKILL_RELAY_SKILLSAVE(CHAR* CharName)
	{
		nParam = AGSMRELAY_PARAM_SKILL_SAVE;
		unPacketLength = (UINT16)sizeof(PACKET_AGSMSKILL_RELAY_SKILLSAVE);

		memset(strCharName, 0, sizeof(strCharName));
		strcpy(strCharName, CharName);
	}
};

#endif //_AGSPSKILL_H