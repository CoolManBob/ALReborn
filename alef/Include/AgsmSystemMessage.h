// AgpmSystemMessage.h
// (C) NHN Games - ArchLord Development Team
// kelovon, 200501212

#ifndef _AGSM_SYSTEM_MESSAGE_H_
#define _AGSM_SYSTEM_MESSAGE_H_

#include "AgsEngine.h"
#include "AgpmSystemMessage.h"

#include <AuXmlParser.h>

typedef enum _eAgsmSystemMessageCallback
{
	AGSMSYSTEMMESSAGE_CB_GET_CHAR_DPNID = 0,
	AGSMSYSTEMMESSAGE_CB_MAX,
} eAgsmSystemMessageCallback;

class AgsmSystemMessage : public AgsModule
{
private:
	AgpmSystemMessage	*m_pcsAgpmSystemMessage;

	AuXmlDocument		m_pXmlDoc;
	AuXmlNode*			m_pXmlRoot;

public:
	AgsmSystemMessage();
	virtual ~AgsmSystemMessage();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle2(UINT32 ulClockCount);
	BOOL OnDestroy();

	AuXmlNode* GetRootNode();

	// Callback Registration
	BOOL SetCallbackGetDPNID(ApModuleDefaultCallBack, PVOID pClass);

	// Send Packet
	BOOL SendSystemMessage(UINT32 ulNID, INT32 lCode, INT32 lIntParam1 = -1, INT32 lIntParam2 = -1, CHAR* szStringParam1 = NULL, CHAR* szStringParam2 = NULL, AgpdCharacter* pcsCharacter = NULL);
	BOOL SendSystemGeneralString(UINT32 ulNID, CHAR* szStringParam, AgpdCharacter* pcsCharacter = NULL);
	BOOL SendSystemDebugString(UINT32 ulNID, CHAR* szStringParam, AgpdCharacter* pcsCharacter = NULL);

	BOOL SendSystemMessage(AgpdCharacter* pcsCharacter, INT32 lCode, INT32 lIntParam1 = -1, INT32 lIntParam2 = -1, CHAR* szStringParam1 = NULL, CHAR* szStringParam2 = NULL);
	BOOL SendSystemGeneralString(AgpdCharacter* pcsCharacter, CHAR* szStringParam);
	BOOL SendSystemDebugString(AgpdCharacter* pcsCharacter, CHAR* szStringParam);

	static BOOL CBProcessSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL SendSystemMessageAllUser(INT32 lCode, INT32 lIntParam1, INT32 lIntParam2, CHAR* szStringParam1, CHAR* szStringParam2);
	//JK_½É¾ß¼¦´Ù¿î
	BOOL SendSystemMessageWithType(UINT32 ulNID, INT8 cMsgType, INT32 lCode, INT32 lIntParam1  = -1 , INT32 lIntParam2  = -1 , CHAR* szStringParam1  = NULL , CHAR* szStringParam2  = NULL , AgpdCharacter* pcsCharacter  = NULL );

};

#endif // _AGSM_SYSTEM_MESSAGE_H_
