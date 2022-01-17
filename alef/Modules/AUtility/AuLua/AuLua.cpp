#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <string>

#include "AuLua.h"

AuLua::AuLua() : 
 m_bDuplicated(false),
 m_fucErrorHandler(NULL)
{
	m_pScriptContext = lua_open();
	if( m_pScriptContext )
	{
		luaopen_base( m_pScriptContext );
		luaL_openlibs( m_pScriptContext );
		luaopen_string( m_pScriptContext );
		luaopen_math( m_pScriptContext );
		luaopen_debug (m_pScriptContext );
		luaopen_table( m_pScriptContext );
	}
}

AuLua::~AuLua()
{
	if( m_pScriptContext )
	{
		lua_close( m_pScriptContext );
		m_pScriptContext = NULL;
	}
}

static std::string findScript(const char *pFname)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( pFname, drive, dir, fname, ext );

	std::string strTestFile = (std::string) drive + dir + "Scripts\\" + fname + ".LUB";
	FILE* fTest = fopen(strTestFile.c_str(), "r");
	if( !fTest )
	{
		strTestFile = (std::string) drive + dir + "Scripts\\" + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if( !fTest )
	{
		strTestFile = (std::string) drive + dir + fname + ".LUB";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if( !fTest )
	{
		strTestFile = (std::string) drive + dir + fname + ".LUA";
		fTest = fopen(strTestFile.c_str(), "r");
	}

	if( fTest )
		fclose( fTest );

	return strTestFile;
}



bool AuLua::RunScript(const char *pFname)
{
	std::string strFilename = findScript(pFname);
	const char* pFilename = strFilename.c_str();

	if( luaL_loadfile( m_pScriptContext, pFilename ) )
	{
		if( m_fucErrorHandler )
		{
			char buf[65535];
			sprintf(buf, "Lua Error - Script Load\nScript Name:%s\nError Message:%s\n", pFilename, luaL_checkstring(m_pScriptContext, -1));
			m_fucErrorHandler(buf);
		}

		return false;
	}
	if( lua_pcall( m_pScriptContext, 0, LUA_MULTRET, 0 ) )
	{
		if(m_fucErrorHandler)
		{
			char buf[65535];
			sprintf(buf, "Lua Error - Script Run\nScript Name:%s\nError Message:%s\n", pFilename, luaL_checkstring(m_pScriptContext, -1));
			m_fucErrorHandler(buf);
		}

		return false;
	}
	return true;

}

bool AuLua::RunString(const char *pCommand)
{
	if( luaL_loadbuffer( m_pScriptContext, pCommand, strlen(pCommand), NULL ) )
	{
		if(m_fucErrorHandler)
		{
			char buf[65535];
			sprintf(buf, "Lua Error - String Load\nString:%s\nError Message:%s\n", pCommand, luaL_checkstring(m_pScriptContext, -1));
			m_fucErrorHandler(buf);
		}

		return false;
	}

	if ( lua_pcall( m_pScriptContext, 0, LUA_MULTRET, 0 ) )
	{
		if(m_fucErrorHandler)
		{
			char buf[65535];
			sprintf(buf, "Lua Error - String Run\nString:%s\nError Message:%s\n", pCommand, luaL_checkstring(m_pScriptContext, -1));
			m_fucErrorHandler(buf);
		}

		return false;
	}

	return true;
}

const char* AuLua::GetErrorString(void)
{
	return luaL_checkstring( m_pScriptContext, -1 );
}


bool AuLua::AddFunction(const char *pFunctionName, LuaFunctionType pFunction)
{
	lua_register( m_pScriptContext, pFunctionName, pFunction );
	return true;
}

const char* AuLua::GetStringArgument(int num, const char *pDefault)
{
	return luaL_optstring( m_pScriptContext, num, pDefault );
}

double AuLua::GetNumberArgument(int num, double dDefault)
{
	return luaL_optnumber( m_pScriptContext, num, dDefault );
}

void AuLua::PushString(const char *pString)
{
	lua_pushstring( m_pScriptContext, pString );
}

void AuLua::PushNumber(double value)
{
	lua_pushnumber( m_pScriptContext, value );
}

//////////////////////////////////////////////////////////////////////////
//
#include <ApMutualEx.h>

AuLuaArray::AuLuaArray()
	: pOnRegisterFunc(0)
{

}

AuLuaArray::~AuLuaArray()
{

}

bool AuLuaArray::Initialize(int pDefaultHandle, OnRegister_CFunction pLua)
{
	pOnRegisterFunc = pLua;

	for(int i = 0; i < pDefaultHandle; i++) 
	{
		lua_State* pLuaState = CreateHandle();

		m_LuaList.push_back(pLuaState);
	}

	return true;
}

void AuLuaArray::Destroy()
{
	lua_State* pLua = NULL;

	while(!m_LuaList.empty()) 
	{
		pLua = m_LuaList.front();
		lua_close(pLua);
		
		m_LuaList.pop_front();
	}
}

lua_State* AuLuaArray::CreateHandle()
{
	lua_State* pLuaState = lua_open();
	luaL_openlibs(pLuaState);

	if( pOnRegisterFunc )
		pOnRegisterFunc(pLuaState);

	return pLuaState;
};

lua_State* AuLuaArray::GetHandle()
{
	AuAutoLock pLock(m_LuaLock);
	if(!pLock.Result())
		return NULL;

	lua_State* pLua = NULL;
	if(!m_LuaList.empty()) 
	{
		pLua = m_LuaList.front();
		m_LuaList.pop_front();
	}
	else 
	{
		pLua = CreateHandle();
	}

	return pLua;
}

void AuLuaArray::RestoreHandle( lua_State *pLua )
{
	AuAutoLock pLock(m_LuaLock);
	if(!pLock.Result())
	{
		lua_close(pLua);
	}
	else
	{
		m_LuaList.push_back(pLua);
	}
}