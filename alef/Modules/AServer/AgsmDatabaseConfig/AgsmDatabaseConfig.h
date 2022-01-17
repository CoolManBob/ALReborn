/*===================================================================

	AgsmDatabaseConfig.h

====================================================================*/

#ifndef _AGSM_DB_CONFIG_H_
	#define _AGSM_DB_CONFIG_H_


#include "ApDefine.h"
#include "ApBase.h"
#include "ApAdmin.h"
#include "AgsEngine.h"
#include "AgsmServerManager2.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment (lib , "AgsmDatabaseConfigD")
#else
	#pragma comment (lib , "AgsmDatabaseConfig")
#endif
#endif



/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGSMDATABASECONFIG_DATA_TYPE
	{
	AGSMDATABASECONFIG_DATA_TYPE_QUERYCONFIG = 0,
	};

#define		AGSMDBCONFIG_MAX_DB							20
#define		AGSMDBCONFIG_MAX_QUERY_STRING_LENGTH		1024
#define		AGSMDBCONFIG_MAX_DSN_LENGTH					128

#define _AGSMDATABASECONFIG_DSN			"DSN"
#define _AGSMDATABASECONFIG_USER		"USER"
#define _AGSMDATABASECONFIG_PWD			"PWD"
#define _AGSMDATABASECONFIG_CONNECTION	"CONNECTION"
#define _AGSMDATABASECONFIG_CONNSTR		"CONNSTR"
#define _AGSMDATABASECONFIG_FORCED		"FORCED"
#define _AGSMDATABASECONFIG_LIB			"LIB"

#define _AGSMDATABASECONFIG_DELIM1		'('
#define _AGSMDATABASECONFIG_DELIM2		')'

typedef enum _eAGSMDATABASECONFIG_DBTYPE
	{
	AGSMDATABASECONFIG_UNKNOWN = -1,
	AGSMDATABASECONFIG_DSN = 0,
	AGSMDATABASECONFIG_CONNSTR,
	} eAGSMDATABASECONFIG_DBTYPE;


/****************************************************/
/*		The Definition of AgsdQueryConfig class		*/
/****************************************************/
//
class AgsdQueryConfig
	{
	public:
		CHAR		m_szQuery[AGSMDBCONFIG_MAX_QUERY_STRING_LENGTH];
		INT16		m_nDatabase;

	public:
		void Init()
			{
			m_nDatabase = 0;
			ZeroMemory(m_szQuery, sizeof(CHAR) * AGSMDBCONFIG_MAX_QUERY_STRING_LENGTH);
			}
	};


/****************************************************/
/*		The Definition of AgsaQuery(Map) class		*/
/****************************************************/
//
class AgsaQuery : public ApAdmin
	{
	private:
		INT32				m_nNumQuerys;

	public:
		AgsaQuery();
		virtual ~AgsaQuery();

		//	add/remove
		AgsdQueryConfig*	AddQuery(AgsdQueryConfig* pcsQuery, INT32 lQueryIndex);
		BOOL				RemoveQuery(INT32 lQueryIndex);

		//	get(find)
		AgsdQueryConfig*	GetQuery(INT32 lQueryIndex);
	};


/********************************************************/
/*		The Definition of AgsdDatabaseConfig class		*/
/********************************************************/
//
class AgsdDatabaseConfig
	{
	public:
		CHAR	m_szDSN[AGSMDBCONFIG_MAX_DSN_LENGTH];
		CHAR	m_szUser[AGSMDBCONFIG_MAX_DSN_LENGTH];
		CHAR	m_szPwd[AGSMDBCONFIG_MAX_DSN_LENGTH];
		INT16	m_nConnection;
		INT8	m_cType;
		BOOL	m_bForced;
		BOOL	m_bIsLogDB;
		CHAR	m_szLib[10];

	public:
		AgsdDatabaseConfig()	{}
		virtual ~AgsdDatabaseConfig()	{}
	};


/********************************************************/
/*		The Definition of AgsmDatabaseConfig class		*/
/********************************************************/
//
class AgsmDatabaseConfig : public AgsModule
	{
	private:
		AgsmServerManager2	*m_pAgsmServerManager;
		
		ApSafeArray<AgsdDatabaseConfig, AGSMDBCONFIG_MAX_DB>	m_DatabaseConfig;
		INT16				m_nDatabase;
		AgsaQuery			m_csQueryMap;

		INT16				m_nLogPooIndex;
		
	public:
		AgsmDatabaseConfig();
		virtual ~AgsmDatabaseConfig();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();

		//	Read configuration
		BOOL	ReadConfig(CHAR *pszDatabase, CHAR *pszQuery);

	protected:
		//	Configuration
		BOOL	_ReadQuery(CHAR *pszFile);
		BOOL	_ReadDatabase(CHAR *pszFile);
		BOOL	_ParseDatabaseString(INT16 nIndex, CHAR *psz);
		BOOL	_AddQuery(INT16 nIndex, INT16 nDatabase, CHAR* pszQuery);

		//	Module data
		AgsdQueryConfig*	_CreateQueryConfig();
		BOOL				_DestroyQueryConfig(AgsdQueryConfig *pcsQuery);

	public:
		CHAR*				GetQuery(INT32 lIndex, INT16* pnDatabaseIndex);
		AgsdQueryConfig*	GetQuery(INT32 lIndex);
		AgsdDatabaseConfig*	GetDatabase(INT16 nIndex);
		INT16				GetDatabaseCount();
	};


#endif
