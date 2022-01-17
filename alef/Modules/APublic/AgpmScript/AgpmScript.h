#pragma once

#include "ApModule.h"
#include "AuPacket.h"
#include "AuAngelScriptEngine.h"

#include <vector>

#define AGPMSCRIPT_REGISTER_TYPE_BEGIN(type, name, classPtr)											\
	do																									\
	{																									\
		typedef type myType;																			\
		std::string typeString = #type;																	\
		std::string nameString = #name;																	\
		std::string tmpString;																			\
		bool result;																					\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectType(typeString, sizeof(myType));		\
		ASSERT(result);																					\
		result = g_pcsAgpmScript->GetScriptEngine().registerGlobalVariable(#type " " #name, classPtr);	\
		ASSERT(result);																					\
		g_pcsAgpmScript->RegisterKeyword(#name ".")

#define AGPMSCRIPT_REGISTER_VARIABLE(type, name)																			\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectVariable(typeString, #type " " #name, offsetof(myType,name));	\
		ASSERT(result);																										\
		tmpString = nameString + "." + #name;																				\
		g_pcsAgpmScript->RegisterKeyword(tmpString)

#define AGPMSCRIPT_REGISTER_METHOD0(type, name)																					\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectMethod(typeString, #type " " #name "()", asMETHOD(myType,name));	\
		ASSERT(result);																											\
		tmpString = nameString + "." + #name + "()";																			\
		g_pcsAgpmScript->RegisterKeyword(tmpString)

#define AGPMSCRIPT_REGISTER_METHOD1(type, name, param0)																						\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 ")", asMETHOD(myType,name));	\
		ASSERT(result);																														\
		tmpString = nameString + "." + #name + "( )";																						\
		g_pcsAgpmScript->RegisterKeyword(tmpString)

#define AGPMSCRIPT_REGISTER_METHOD2(type, name, param0, param1)																							\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 ")", asMETHOD(myType,name));	\
		ASSERT(result);																																	\
		tmpString = nameString + "." + #name + "( , )";																									\
		g_pcsAgpmScript->RegisterKeyword(tmpString)

#define AGPMSCRIPT_REGISTER_METHOD3(type, name, param0, param1, param2)																								\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 "," #param2 ")", asMETHOD(myType,name));	\
		ASSERT(result);																																				\
		tmpString = nameString + "." + #name + "( , , )";																											\
		g_pcsAgpmScript->RegisterKeyword(tmpString)

#define AGPMSCRIPT_REGISTER_METHOD4(type, name, param0, param1, param2, param3)																										\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 "," #param2 "," #param3 ")", asMETHOD(myType,name));	\
		ASSERT(result);																																							\
		tmpString = nameString + "." + #name + "( , , , )";																														\
		g_pcsAgpmScript->RegisterKeyword(tmpString)

#define AGPMSCRIPT_REGISTER_METHOD5(type, name, param0, param1, param2, param3, param4)																											\
		result = g_pcsAgpmScript->GetScriptEngine().registerObjectMethod(typeString, #type " " #name "(" #param0 "," #param1 "," #param2 "," #param3 "," #param4 ")", asMETHOD(myType,name));	\
		ASSERT(result);																																										\
		tmpString = nameString + "." + #name + "( , , , , )";																																\
		g_pcsAgpmScript->RegisterKeyword(tmpString)

#define AGPMSCRIPT_REGISTER_TYPE_END	\
	} while(0)
		
		

enum EnumAgpmScriptCallback
{
	AGPMSCRIPT_CALLBACK_MESSAGE = 0,
	AGPMSCRIPT_CALLBACK_KEYWORDS,
	AGPMSCRIPT_CALLBACK_IS_ADMIN,
};

enum EnumAgpmScriptOperation
{
	AGPMSCRIPT_OPERATION_COMMAND = 0,		// script 명령어로 사용
	AGPMSCRIPT_OPERATION_MESSAGE,			// 단순한 문자열(결과값등에서 사용)
	AGPMSCRIPT_OPERATIION_KEYWORDS,			// 키워드 전송
};

class AgpmScript : public ApModule
{
private:
	enum {AGPMSCRIPT_MAX_MEMORY_LENGTH = 1024 * 5};	// 5KB
	AuAngelScriptEngine			m_ScriptEngine;
	std::vector<std::string>	m_Keywords;
	ApCriticalSection			m_CriticalSection;

	CHAR m_Buffer[AGPMSCRIPT_MAX_MEMORY_LENGTH];

public:
	AuPacket m_csPacket;

public:
	AgpmScript(void);
	~AgpmScript(void);

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();

	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL SetCallbackMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackKeywords(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackIsAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	AuAngelScriptEngine& GetScriptEngine()
	{
		return m_ScriptEngine;
	}

	void RegisterKeyword(std::string keyword)
	{
		m_Keywords.push_back(keyword);
	}

	PVOID MakeScriptPacket(INT16 *pPacketLength, EnumAgpmScriptOperation eOperation, CHAR* szMessage, INT32 lCID);
	PVOID MakeScriptKeywordPacket(INT16 *pPacketLength, INT32 lCID);
	void TestMessage();
};
