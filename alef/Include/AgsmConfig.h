// AgsmConfig.h
// (C) NHN Games - ArchLord Development Team
// kelovon, 20051010

#ifndef _AGSM_CONFIG_H_
#define _AGSM_CONFIG_H_

#include "AgpmConfig.h"
#include "AgsmServerManager2.h"
#include <AuLua.h>
#include <AuXmlParser.h>

///////////////////////////////////////////////////////////
// TPack°ü·Ã Define
const string TPACKFILENAME = "ini\\TPack.xml";

//////////////////////////////////////////////////////////

class AgsmSystemMessage;
class AgsmCharacter;
class AgpmCharacter;

class AgsmConfig : public AgsModule
{
	static AgsmConfig*	m_pInstance;
	
public:
	AgsmConfig();
	virtual ~AgsmConfig();

	ApMutualEx m_Mutex;

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnTimer(UINT32 ulClockCount);
	BOOL OnDestroy();

	// load server configurations
	BOOL LoadConfig();
	AuXmlNode* GetNoticeNodeDoubleEvent();
	BOOL LoadConfigLua();
	BOOL LoadTPackConfig();

	BOOL m_bReloadConfigLua;

private:
	static AgsmConfig* GetInstance()
	{
		return AgsmConfig::m_pInstance;
	}
	
	BOOL ParseConfigString(TCHAR *szConfigString);

	BOOL bDoubleEventState;
	static int GetDoubleEventState(lua_State* L);
	static int SetDoubleEventState(lua_State* L);
	static int GetEventNumber(lua_State* L);

	static BOOL CBSendEventState(PVOID pData, PVOID pClass, PVOID pCustData);
	
	AgpmConfig			*m_pcsAgpmConfig;
	AgsmServerManager2	*m_pcsAgsmServerManager2;
	AgsmSystemMessage*	m_pagsmSystemMessage;
	AgsmCharacter*		m_pagsmCharacter;
	AgpmCharacter*		m_pagpmCharacter;
	
	lua_State* pLuaState;
	CLuaStreamPack pluaPack;
	UINT32 m_ulClockCount;
};

#endif // _AGSM_CONFIG_H_