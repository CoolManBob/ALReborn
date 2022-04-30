#ifndef _AULUA_H
#define _AULUA_H

//////////////////////////////////////////////////////////////////////////
//
#include <AuLua/Lua/lua.hpp>

#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment(lib, "lua5.1.5.Win64.static.lib")
	#else
	#pragma comment(lib, "lua5.1.5.Win64.static.lib")
	#endif
#elif _WIN32
	#ifdef _DEBUG
	#pragma comment(lib, "lua5.1.5.Win32.static.lib")
	#else
	#pragma comment(lib, "lua5.1.5.Win32.static.lib")
	#endif
#else
#endif

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

static int traceback(lua_State* L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

static void stackDump (lua_State *L) 
{
#ifdef _DEBUG
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++) { /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

			case LUA_TSTRING: /* strings */
				printf("`%s'", lua_tostring(L, i));
				break;

			case LUA_TBOOLEAN: /* booleans */
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;

			case LUA_TNUMBER: /* numbers */
				printf("%g", lua_tonumber(L, i));
				break;

			default: /* other values */
				printf("%s", lua_typename(L, t));
				break;

		}
		printf(" "); /* put a separator */
	}
	printf("\n"); /* end the listing */
#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// CLuaStreamPack - encryped script supporter
// arycoat - 2008.4.
// 
typedef int (*Decrypt_CFunction)(char*, unsigned);

class CLuaStreamPack
{
public:
	string filename;
	fstream file;
	char* buffer;
	size_t size;
	Decrypt_CFunction df;

	CLuaStreamPack()
		: buffer(0), df(0)
	{
	}

	CLuaStreamPack(const char* _filename, Decrypt_CFunction df = NULL)
		: buffer(0)
	{
		Open(_filename, df);
	}

	~CLuaStreamPack()
	{
		ClearBuffer();
	};

	char* Open(const char* _filename, Decrypt_CFunction _df = NULL)
	{
		df = _df;
		filename = _filename;
		
		file.open(filename.c_str(), ios::in | ios::binary);
		if(!file) return NULL;

		if(buffer)
			ClearBuffer();

		file.seekg(0, ios::end);
		size = (size_t)file.tellg();
		buffer = new char[size];

		file.seekg(0, ios::beg);
		file.read(buffer, size);
		file.close();

		if(df)
			df(buffer, (unsigned int)size);

		return buffer;
	};

	void ClearBuffer()
	{
		if(buffer)
			delete[] buffer;
	};

	bool dobuffer(lua_State* L)
	{
		if(!buffer && !Open(filename.c_str(), df))
			return false;
			
		int base = lua_gettop(L);
		lua_pushcfunction(L, traceback);
		lua_insert(L, base);
		
		const char* szName = filename.c_str();
		int status = ( luaL_loadbuffer(L, buffer, size, szName) || lua_pcall(L, 0, LUA_MULTRET, base) );
		
//		string strmsg = lua_tostring(L, -1);
		lua_remove(L, base);

		if (status != 0)
			lua_gc(L, LUA_GCCOLLECT, 0);
		
		return ( status == 0 ) ? true : false;
	}
};

//------------------------------ Client ----------------------------
struct lua_State;

#define	DECLARE_SINGLETON( classname )		\
public:										\
	static classname & GetInstance()		\
	{										\
		static classname _sStaticInstance;	\
		return _sStaticInstance;			\
	}										\
private:									\
	classname();							\
	classname( const classname &){}			\
public:

#define LuaGlue extern "C" int
extern "C" {
typedef int (*LuaFunctionType)(struct lua_State *pLuaState);
};

typedef void (*ErrorHandler) (const char *pError);

class AuLua
{
public:										
	static AuLua& GetInstance()		
	{										
		static AuLua _sStaticInstance;	
		return _sStaticInstance;			
	}
	static AuLua* GetPointer()
	{
		return &GetInstance();
	}

private:
	AuLua();
	AuLua( const AuLua & )	{		}

public:
	virtual ~AuLua();

	bool		RunScript( const char* pFilename );
	bool		RunString( const char* pCommand );
	const char*	GetErrorString();
	bool		AddFunction( const char *pFunctionName, LuaFunctionType pFunction );
	const char*	GetStringArgument( int num, const char *pDefault = NULL );
	double		GetNumberArgument( int num, double dDefault = 0.0 );
	void		PushString( const char *pString );
	void		PushNumber( double value );

	void		SetErrorHandler( ErrorHandler fucErrorHandler )		{	m_fucErrorHandler = fucErrorHandler;	}
	lua_State*	GetScriptContext()									{	return m_pScriptContext;	}

private:
	bool			m_bDuplicated;
	lua_State*		m_pScriptContext;
	ErrorHandler	m_fucErrorHandler;
};

//////////////////////////////////////////////////////////////////////////
//
#include <ApMutualEx.h>
#include <list>

typedef int (*OnRegister_CFunction)(lua_State* );

class AuLuaArray
{
	ApMutualEx			m_LuaLock;
	std::list<lua_State *> m_LuaList;
	OnRegister_CFunction pOnRegisterFunc;

public:
	AuLuaArray();
	virtual ~AuLuaArray();

	bool Initialize(int pDefaultHandle, OnRegister_CFunction pLua);
	void Destroy();
	lua_State* CreateHandle();
	lua_State* GetHandle();
	void RestoreHandle(lua_State *pLua);
};

#endif // _AULUA_H
