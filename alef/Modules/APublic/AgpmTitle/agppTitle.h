#ifndef _AGPP_TITLE_H
#define _AGPP_TITLE_H

#include "AgpmTitle.h"

typedef enum _eAgpmTitlePacketOperation
{
	AGPMTITLE_PACKET_OPERATION_TITLE_ADD	= 0,
	AGPMTITLE_PACKET_OPERATION_TITLE_ADD_RESUILT,
	AGPMTITLE_PACKET_OPERATION_TITLE_USE,
	AGPMTITLE_PACKET_OPERATION_TITLE_USE_RESULT,
	AGPMTITLE_PACKET_OPERATION_TITLE_LIST,
	AGPMTITLE_PACKET_OPERATION_TITLE_LIST_RESULT,
	AGPMTITLE_PACKET_OPERATION_TITLE_QUEST,
	AGPMTITLE_PACKET_OPERATION_TITLE_DELETE,
	AGPMTITLE_PACKET_OPERATION_TITLE_DELETE_RESULT,
	AGPMTITLE_PACKET_OPERATION_TITLE_USE_NEAR,
	AGPMTITLE_PACKET_OPERATION_TITLE_UI_OPEN,
}eAgpmTitlePacketOperation;

typedef enum _eAgpmTitleQuestOperation
{
	AGPMTITLE_QUEST_REQUEST	= 0,
	AGPMTITLE_QUEST_REQUEST_RESULT,
	AGPMTITLE_QUEST_CHECK,
	AGPMTITLE_QUEST_CHECK_RESULT,
	AGPMTITLE_QUEST_COMPLETE,
	AGPMTITLE_QUEST_COMPLETE_RESULT,
	AGPMTITLE_QUEST_LIST,
	AGPMTITLE_QUEST_LIST_RESULT,
}eAgpmTitleQuestOperation;

typedef enum _eAgpmTitleUseFailReason
{
	AGPMTITLE_USE_SUCCESS = 0,
	AGPMTITLE_USE_FAIL_DONT_HAVE_TITLE,
	AGPMTITLE_USE_FAIL_ALREADY_USE,
	AGPMTITLE_USE_FAIL_NOT_ENOUGH_TIME,
	AGPMTITLE_USE_FAIL_DONT_KNOW_REASON,
};

struct PACKET_AGPPTITLE : public PACKET_HEADER
{
	CHAR FlagLow;
	INT16 nParam;
	CHAR strCharName[AGPACHARACTER_MAX_ID_STRING+1];
	INT32 nTitleID;

	PACKET_AGPPTITLE() : nParam(0), FlagLow(1)
	{
		cType			= AGPM_TITLE_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE);
		memset(strCharName, 0, sizeof(strCharName));
	}
};

struct PACKET_AGPPTITLE_ADD : public PACKET_AGPPTITLE
{
	PACKET_AGPPTITLE_ADD(CHAR* CharName, INT32 TitleID)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_ADD;
		nTitleID = TitleID;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_ADD);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGPPTITLE_ADD_RESULT : public PACKET_AGPPTITLE
{
	BOOL bAddResult;

	PACKET_AGPPTITLE_ADD_RESULT(CHAR* CharName, INT32 TitleID, BOOL AddResult)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_ADD_RESUILT;
		nTitleID = TitleID;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_ADD_RESULT);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bAddResult = AddResult;
	}
};

struct PACKET_AGPPTITLE_USE : public PACKET_AGPPTITLE
{
	INT32 nOperation;
	BOOL bUse;

	PACKET_AGPPTITLE_USE(CHAR* CharName, INT32 TitleID,  INT32 Operation, BOOL Use)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_USE;
		nOperation = Operation;
		nTitleID = TitleID;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_USE);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bUse = Use;
	}
};

struct PACKET_AGPPTITLE_USE_RESULT : public PACKET_AGPPTITLE
{
	BOOL bUse;
	BOOL bUseResult;
	UINT32 lTimeStamp;
	UINT32 lFailReason;

	PACKET_AGPPTITLE_USE_RESULT(CHAR* CharName, INT32 TitleID,  BOOL UseResult, BOOL Use, UINT32 TimeStamp, UINT32 FailReason)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_USE_RESULT;
		nTitleID = TitleID;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_USE_RESULT);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bUse = Use;
		bUseResult = UseResult;
		lTimeStamp = TimeStamp;
		lFailReason = FailReason;
	}
};

struct PACKET_AGPPTITLE_LIST : public PACKET_AGPPTITLE
{
	PACKET_AGPPTITLE_LIST(CHAR* CharName)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_LIST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_LIST);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGPPTITLE_LIST_RESULT : public PACKET_AGPPTITLE
{
	UINT32 lTimeStamp;
	BOOL bUseTitle;
	BOOL bListResult;
	BOOL bTitleListEnd;

	PACKET_AGPPTITLE_LIST_RESULT(CHAR* CharName, INT32 TitleID, UINT32 TimeStamp, BOOL UseTitle, BOOL TitleListEnd, BOOL ListResult)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_LIST_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_LIST_RESULT);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		nTitleID = TitleID;
		lTimeStamp = TimeStamp;
		bUseTitle = UseTitle;
		bTitleListEnd = TitleListEnd;
		bListResult = ListResult;
	}
};

struct PACKET_AGPPTITLE_QUEST : public PACKET_AGPPTITLE
{
	INT32 nOperation;

	PACKET_AGPPTITLE_QUEST()
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_QUEST;
		nTitleID = 0;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST);
		nOperation = 0;
	}
};

struct PACKET_AGPPTITLE_QUEST_REQUEST : public PACKET_AGPPTITLE_QUEST
{
	PACKET_AGPPTITLE_QUEST_REQUEST(CHAR* CharName, INT32 TitleID)
	{
		nOperation = AGPMTITLE_QUEST_REQUEST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_REQUEST);
		nTitleID = TitleID;
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGPPTITLE_QUEST_REQUEST_RESULT : public PACKET_AGPPTITLE_QUEST
{
	UINT32 lTimeStamp;
	BOOL bQuestRequestResult;

	PACKET_AGPPTITLE_QUEST_REQUEST_RESULT(CHAR* CharName, INT32 TitleID, BOOL QuestRequestResult)
	{
		nOperation = AGPMTITLE_QUEST_REQUEST_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_REQUEST_RESULT);
		nTitleID = TitleID;
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bQuestRequestResult = QuestRequestResult;
	}
};

struct PACKET_AGPPTITLE_QUEST_CHECK : public PACKET_AGPPTITLE_QUEST
{
	INT32 nTitleCheckType;
	INT32 nTitleCheckSet;
	INT32 nTitleCurrentValue;

	PACKET_AGPPTITLE_QUEST_CHECK(CHAR* CharName, INT32 TitleID)
	{
		nOperation = AGPMTITLE_QUEST_CHECK;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_CHECK);
		nTitleID = TitleID;
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGPPTITLE_QUEST_CHECK_RESULT : public PACKET_AGPPTITLE_QUEST
{
	INT32 nTitleCurrentValue[AGPDTITLE_MAX_TITLE_CHECK];
	BOOL bCheckResult;

	PACKET_AGPPTITLE_QUEST_CHECK_RESULT(CHAR* CharName, INT32 TitleID, BOOL CheckResult)
	{
		nOperation = AGPMTITLE_QUEST_CHECK_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_CHECK_RESULT);
		nTitleID = TitleID;
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bCheckResult = CheckResult;
	}
};

struct PACKET_AGPPTITLE_QUEST_COMPLETE : public PACKET_AGPPTITLE_QUEST
{
	PACKET_AGPPTITLE_QUEST_COMPLETE(CHAR* CharName, INT32 TitleID)
	{
		nOperation = AGPMTITLE_QUEST_COMPLETE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_COMPLETE);
		nTitleID = TitleID;
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT : public PACKET_AGPPTITLE_QUEST
{
	UINT32 lTimeStamp;
	BOOL bCompleteResult;

	PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT(CHAR* CharName, INT32 TitleID, BOOL CompleteResult)
	{
		nOperation = AGPMTITLE_QUEST_COMPLETE_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_COMPLETE_RESULT);
		nTitleID = TitleID;
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bCompleteResult = CompleteResult;
	}
};

struct PACKET_AGPPTITLE_QUEST_LIST : public PACKET_AGPPTITLE_QUEST
{
	PACKET_AGPPTITLE_QUEST_LIST(CHAR* CharName)
	{
		nOperation = AGPMTITLE_QUEST_LIST;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_LIST);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGPPTITLE_QUEST_LIST_RESULT : public PACKET_AGPPTITLE_QUEST
{
	BOOL bTitleQuestListResult;
	BOOL bListEnd;
	BOOL bComplete;
	UINT32 lAcceptTime;
	UINT32 lCompleteTime;
	INT32 nTitleCurrentValue[AGPDTITLE_MAX_TITLE_CHECK];

	PACKET_AGPPTITLE_QUEST_LIST_RESULT(CHAR* CharName, INT32 TitleID, BOOL TitleQuestListResult)
	{
		nOperation = AGPMTITLE_QUEST_LIST_RESULT;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_QUEST_LIST_RESULT);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		nTitleID = TitleID;
		bTitleQuestListResult = TitleQuestListResult;
	}
};

struct PACKET_AGPPTITLE_DELETE : public PACKET_AGPPTITLE
{
	PACKET_AGPPTITLE_DELETE(CHAR* CharName, INT32 TitleID)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_DELETE;
		nTitleID = TitleID;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_DELETE);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};

struct PACKET_AGPPTITLE_DELETE_RESULT : public PACKET_AGPPTITLE
{
	BOOL bTitleDeleteResult;

	PACKET_AGPPTITLE_DELETE_RESULT(CHAR* CharName, INT32 TitleID, BOOL TitleDeleteResult)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_DELETE_RESULT;
		nTitleID = TitleID;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_DELETE_RESULT);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
		bTitleDeleteResult = TitleDeleteResult;
	}
};

struct PACKET_AGPPTITLE_USE_NEAR : public PACKET_AGPPTITLE
{
	BOOL bUse;
	PACKET_AGPPTITLE_USE_NEAR()
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_USE_NEAR;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_USE_NEAR);
		nTitleID = 0;
		bUse = FALSE;
	}
};

struct PACKET_AGPPTITLE_UI_OPEN : public PACKET_AGPPTITLE
{
	PACKET_AGPPTITLE_UI_OPEN(CHAR* CharName)
	{
		nParam = AGPMTITLE_PACKET_OPERATION_TITLE_UI_OPEN;
		unPacketLength	= (UINT16)sizeof(PACKET_AGPPTITLE_UI_OPEN);
		if(CharName)
			memcpy(strCharName, CharName, AGPACHARACTER_MAX_ID_STRING);
	}
};


#endif //_AGPP_TITLE_H