#ifndef _AGSP_TITLE_RELAY_PACKET
#define _AGSP_TITLE_RELAY_PACKET

#include "AgsmTitle.h"
#include "AgsdRelay2.h"

struct PACKET_AGSP_TITLE_RELAY : public PACKET_HEADER
{
	CHAR FlagLow;
	INT16 nParam;
	INT16 nOperation;
	CHAR strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 nTitleID;

	PACKET_AGSP_TITLE_RELAY()
		:nOperation(0), nTitleID(0), FlagLow(1)
	{
		cType			= AGSMRELAY_PACKET_TYPE;
		nParam			= AGSMRELAY_PARAM_TITLE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_RELAY);
	}
};

struct PACKET_AGSP_TITLE_ADD_RELAY : public PACKET_AGSP_TITLE_RELAY
{
	PACKET_AGSP_TITLE_ADD_RELAY(CHAR *CharName, INT32 TitleID)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_ADD;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_ADD_RELAY);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGSP_TITLE_ADD_RESULT_RELAY : public PACKET_AGSP_TITLE_RELAY
{
	BOOL nAddResult;
	PACKET_AGSP_TITLE_ADD_RESULT_RELAY(CHAR *CharName, INT32 TitleID, BOOL AddResult)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_ADD_RESUILT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_ADD_RESULT_RELAY);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		nAddResult = AddResult;
	}
};

struct PACKET_AGSP_TITLE_USE_RELAY : public PACKET_AGSP_TITLE_RELAY
{
	CHAR m_szDate[_MAX_DATETIME_LENGTH + 1];
	INT32 nPauseTitleTid;

	PACKET_AGSP_TITLE_USE_RELAY(CHAR *CharName, INT32 TitleID, CHAR *Date)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_USE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_USE_RELAY);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		strncpy(m_szDate, Date, _MAX_DATETIME_LENGTH);
	}
};

struct PACKET_AGSP_TITLE_USE_RESULT_RELAY : public PACKET_AGSP_TITLE_RELAY
{
	CHAR m_szDate[_MAX_DATETIME_LENGTH + 1];
	BOOL bUse;
	BOOL nUseResult;

	PACKET_AGSP_TITLE_USE_RESULT_RELAY(CHAR *CharName, INT32 TitleID, CHAR *Date, BOOL Use, BOOL UseResult)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_USE_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_USE_RESULT_RELAY);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		strncpy(m_szDate, Date, _MAX_DATETIME_LENGTH);
		bUse = Use;
		nUseResult = UseResult;
	}
};

struct PACKET_AGSP_TITLE_LIST_RELAY : public PACKET_AGSP_TITLE_RELAY
{
	PACKET_AGSP_TITLE_LIST_RELAY(CHAR *CharName)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_LIST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_LIST_RELAY);
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGSP_TITLE_LIST_RESULT_RELAY : public PACKET_AGSP_TITLE_RELAY
{
	CHAR m_szDate[_MAX_DATETIME_LENGTH + 1];
	BOOL bUseTitle;
	BOOL bListResult;
	BOOL bTitleListEnd;

	PACKET_AGSP_TITLE_LIST_RESULT_RELAY(CHAR *CharName, BOOL ListResult, BOOL TitleListEnd)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_LIST_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_LIST_RESULT_RELAY);
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bListResult = ListResult;
		bTitleListEnd = TitleListEnd;
		memset(m_szDate, 0, sizeof(m_szDate));
		bUseTitle = FALSE;
	}
};

struct PACKET_AGSP_TITLE_QUEST : public PACKET_AGSP_TITLE_RELAY
{
	INT16 nOperation2;

	PACKET_AGSP_TITLE_QUEST() : nOperation2(0)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_QUEST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST);
		memset(strCharName, 0, sizeof(strCharName));
	}
};

struct PACKET_AGSP_TITLE_QUEST_REQUEST : public PACKET_AGSP_TITLE_QUEST
{
	CHAR m_szDate[_MAX_DATETIME_LENGTH + 1];
	PACKET_AGSP_TITLE_QUEST_REQUEST(CHAR *CharName, INT32 TitleID)
	{
		nOperation2 = AGPMTITLE_QUEST_REQUEST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_REQUEST);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT : public PACKET_AGSP_TITLE_QUEST
{
	CHAR m_szDate[_MAX_DATETIME_LENGTH + 1];
	BOOL bTitleQuestRequestResult;

	PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT(CHAR *CharName, INT32 TitleID, BOOL TitleQuestRequestResult)
	{
		nOperation2 = AGPMTITLE_QUEST_REQUEST_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_REQUEST_RESULT);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bTitleQuestRequestResult = TitleQuestRequestResult;
	}
};

struct PACKET_AGSP_TITLE_QUEST_CHECK : public PACKET_AGSP_TITLE_QUEST
{
	INT32 nTitleCurrentValue[AGPDTITLE_MAX_TITLE_CHECK];

	PACKET_AGSP_TITLE_QUEST_CHECK(CHAR *CharName, INT32 TitleID)
	{
		nOperation2 = AGPMTITLE_QUEST_CHECK;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_CHECK);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGSP_TITLE_QUEST_CHECK_RESULT : public PACKET_AGSP_TITLE_QUEST
{
	INT32 nTitleCurrentValue[AGPDTITLE_MAX_TITLE_CHECK];
	BOOL bTitleQuestCheckResult;

	PACKET_AGSP_TITLE_QUEST_CHECK_RESULT(CHAR *CharName, INT32 TitleID, BOOL TitleQuestCheckResult)
	{
		nOperation2 = AGPMTITLE_QUEST_CHECK_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_CHECK_RESULT);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bTitleQuestCheckResult = TitleQuestCheckResult;
	}
};

struct PACKET_AGSP_TITLE_QUEST_COMPLETE : public PACKET_AGSP_TITLE_QUEST
{
	CHAR m_szDate[_MAX_DATETIME_LENGTH + 1];

	PACKET_AGSP_TITLE_QUEST_COMPLETE(CHAR *CharName, INT32 TitleID)
	{
		nOperation2 = AGPMTITLE_QUEST_COMPLETE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_COMPLETE);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT : public PACKET_AGSP_TITLE_QUEST
{
	CHAR m_szDate[_MAX_DATETIME_LENGTH + 1];
	BOOL bTitleQuestCompleteResult;

	PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT(CHAR *CharName, INT32 TitleID, BOOL TitleQuestCompleteResult)
	{
		nOperation2 = AGPMTITLE_QUEST_COMPLETE_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_COMPLETE_RESULT);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bTitleQuestCompleteResult = TitleQuestCompleteResult;
	}
};

struct PACKET_AGSP_TITLE_QUEST_LIST : public PACKET_AGSP_TITLE_QUEST
{
	PACKET_AGSP_TITLE_QUEST_LIST(CHAR *CharName)
	{
		nOperation2 = AGPMTITLE_QUEST_LIST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_LIST);
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGSP_TITLE_QUEST_LIST_RESULT : public PACKET_AGSP_TITLE_QUEST
{
	BOOL bTitleQuestComplete;
	INT32 nTitleQuestCheckValue[AGPDTITLE_MAX_TITLE_CHECK];
	BOOL bListEnd;
	CHAR m_szDateAccept[_MAX_DATETIME_LENGTH + 1];
	CHAR m_szDateComplete[_MAX_DATETIME_LENGTH + 1];
	BOOL bTitleQuestListResult;
	PACKET_AGSP_TITLE_QUEST_LIST_RESULT(CHAR *CharName, BOOL TitleQuestListResult, BOOL ListEnd)
	{
		nOperation2 = AGPMTITLE_QUEST_LIST_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_QUEST_LIST_RESULT);
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bTitleQuestListResult = TitleQuestListResult;
		bListEnd = ListEnd;
	}
};

struct PACKET_AGSP_TITLE_DELETE : public PACKET_AGSP_TITLE_RELAY
{
	PACKET_AGSP_TITLE_DELETE(CHAR *CharName, INT32 TitleID)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_DELETE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_DELETE);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGSP_TITLE_DELETE_RESULT : public PACKET_AGSP_TITLE_RELAY
{
	BOOL bTitleDeleteResult;

	PACKET_AGSP_TITLE_DELETE_RESULT(CHAR *CharName, INT32 TitleID, BOOL TitleDeleteResult)
	{
		nOperation = AGPMTITLE_PACKET_OPERATION_TITLE_DELETE_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_TITLE_DELETE_RESULT);
		nTitleID = TitleID;
		strncpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bTitleDeleteResult = TitleDeleteResult;
	}
};

#endif //_AGSP_TITLE_RELAY_PACKET