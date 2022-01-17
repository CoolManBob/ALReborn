/*============================================================================

	AuDatabase.h

============================================================================*/

#ifndef _AU_DATABASE_H_
	#define _AU_DATABASE_H_

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment (lib, "AuDatabaseD")
#else 
	#pragma comment (lib, "AuDatabase")
#endif
#endif

#include <sqlext.h>

#ifndef UNUSED_PARA
	#define UNUSED_PARA(p)		p;
#endif


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
//	==========	Internal API type	==========
//
typedef enum _eAUDB_API
	{
	AUDB_API_OLEDB = 0,
	AUDB_API_ODBC,
	AUDB_API_OCI,			// Oracle only
	AUDB_API_NUM,
	} eAUDB_API;


//
//	==========	Vender	==========
//
typedef enum _eAUDB_VENDER
	{
	AUDB_VENDER_ORACLE = 0,
	AUDB_VENDER_MYSQL,
	AUDB_VENDER_SYBASE,
	AUDB_VENDER_INFORMIX,
	AUDB_VENDER_MSSQL,
	AUDB_VENDER_MAX,
	} eAUDB_VENDER;


//
//	==========	Column Type	==========
//
typedef enum _eAUDB_TYPE
	{
	AUDB_TYPE_UNSUPPORT = 0,
	AUDB_TYPE_STRING,
	AUDB_TYPE_INT8,
	AUDB_TYPE_INT16,
	AUDB_TYPE_INT32,
	AUDB_TYPE_INT64,
	AUDB_TYPE_UINT8,
	AUDB_TYPE_UINT16,
	AUDB_TYPE_UINT32,
	AUDB_TYPE_UINT64,
	AUDB_TYPE_FLOAT,
	AUDB_TYPE_DOUBLE,
	AUDB_TYPE_DATE,
	} eAUDB_TYPE;


//
//	==========	Transaction Isolation Level	==========
//
typedef enum _eAUDB_ISOLATION_LEVEL
	{
	AUDB_ISOLATION_LEVEL_UNDEFINED = 0,
	AUDB_ISOLATION_LEVEL_CHAOS,
	AUDB_ISOLATION_LEVEL_READ_UNCOMMITED,
	AUDB_ISOLATION_LEVEL_READ_COMMITED,
	AUDB_ISOLATION_LEVEL_REPEATABLE_READ,
	AUDB_ISOLATION_LEVEL_SERIALIZABLE,
	AUDB_ISOLATION_LEVEL_ISOLATED,
	AUDB_ISOLATION_LEVEL_NUM,
	} eAUDB_ISOLATION_LEVEL;


//
//	==========	Others	==========
//
#define AUDB_MAX_DSN_SIZE					128			// Maximum dsn, user, pwd length
#define AUDB_DEFAULT_FETCH_COUNT			16			// Default row fetch count
#define AUDB_MAX_FETCH_COUNT				32			// Max row fetch count
#define AUDB_DEFAULT_ERROR_STRING_SIZE		512
#define AUDB_DEFAULT_HEAP_SIZE				1			// Default Heap Size, MB
#define AUDB_MAX_COLUMN_SIZE				32			// Maximum no. of column in table
#define AUDB_MAX_COLUMN_NAME_SIZE			32			// Maximum column name length
#define AUDB_MAX_COLUMN_VALUE_SIZE			256			// Maximum column value length
#define	AUDB_MAX_QUERY_SIZE					4096		// Maximun query string length
#define AUDB_MAX_DATETIME_SIZE				32
#define AUDB_MAX_NUMERIC_SIZE				22

extern BOOL		g_bNotUseOCI;


class AuEnvironment;
class AuDatabase;
class AuStatement;
class AuRowset;
class AuDatabaseBuffer;
class AuDatabaseManager;




/********************************************************/
/*		The Definition of Environment Base class		*/
/********************************************************/
//
class AuEnvironment
	{
	protected:
		TCHAR			m_szError[AUDB_DEFAULT_ERROR_STRING_SIZE];
		eAUDB_VENDER	m_eVender;
		static	BOOL	m_bCaching;

	public:
		AuEnvironment(eAUDB_VENDER eVender);
		virtual ~AuEnvironment() {}

		virtual BOOL	Open() = 0;
		virtual void	Close() = 0;

		LPCTSTR			GetError();
		
		static int IsCaching()
			{
			return m_bCaching;
			}
	};


inline LPCTSTR AuEnvironment::GetError()
	{
	return m_szError;
	}




/****************************************************/
/*		The Definition of Database Base class		*/
/****************************************************/
//
class AuDatabase
	{
	protected:
		TCHAR		m_szError[AUDB_DEFAULT_ERROR_STRING_SIZE];

	public:
		AuDatabase();
		virtual ~AuDatabase() {}

		//	Open/Close(Connect/Disconnect)
		virtual BOOL	Open(LPCTSTR pszDatasource, LPCTSTR pszUser, LPCTSTR pszPassword) = 0;
		virtual BOOL	Open(LPCTSTR pszConnectionString) = 0;
		virtual void	Close() = 0;

		//	Transaction
		virtual BOOL	BeginTransaction(eAUDB_ISOLATION_LEVEL eIsolation = AUDB_ISOLATION_LEVEL_READ_COMMITED) = 0;
		virtual BOOL	EndTransaction(BOOL fCommit = TRUE) = 0;

		//	Status
		virtual BOOL	IsAlive() = 0;		// is connection alive?

		LPCTSTR			GetError();
	};


inline LPCTSTR AuDatabase::GetError()
	{
	return m_szError;
	}




/****************************************************/
/*		The Definition of Statement Base class		*/
/****************************************************/
//
class AuStatement
	{
	protected:
		TCHAR		m_szError[AUDB_DEFAULT_ERROR_STRING_SIZE];
		SQLLEN		m_lAffectedRows;

	public:
		AuStatement();
		virtual ~AuStatement() {}

		virtual BOOL	Open() = 0;
		virtual void	Close() = 0;

		//	direct
		virtual BOOL	Execute(LPCTSTR pszQuery) = 0;

		//	paramterized
		virtual BOOL	Prepare(LPCTSTR pszQuery)					{ UNUSED_PARA(pszQuery); return FALSE; }
		virtual BOOL	Execute()									{ return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, PVOID pBuffer, INT32 lBufferSize = 0, BOOL bOut = FALSE)		{ UNUSED_PARA(nIndex); UNUSED_PARA(pBuffer); UNUSED_PARA(lBufferSize); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, CHAR* pszBuffer, INT32 lBufferSize = 0, BOOL bOut = FALSE)	{ UNUSED_PARA(nIndex); UNUSED_PARA(pszBuffer); UNUSED_PARA(lBufferSize); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, INT8* pcBuffer, BOOL bOut = FALSE)							{ UNUSED_PARA(nIndex); UNUSED_PARA(pcBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, INT16* pnBuffer, BOOL bOut = FALSE)							{ UNUSED_PARA(nIndex); UNUSED_PARA(pnBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, INT32* plBuffer, BOOL bOut = FALSE)							{ UNUSED_PARA(nIndex); UNUSED_PARA(plBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, INT64* pllBuffer, BOOL bOut = FALSE)							{ UNUSED_PARA(nIndex); UNUSED_PARA(pllBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, UINT8* pucBuffer, BOOL bOut = FALSE)							{ UNUSED_PARA(nIndex); UNUSED_PARA(pucBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, UINT16* punBuffer, BOOL bOut = FALSE)						{ UNUSED_PARA(nIndex); UNUSED_PARA(punBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, UINT32* pulBuffer, BOOL bOut = FALSE)						{ UNUSED_PARA(nIndex); UNUSED_PARA(pulBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, UINT64* pullBuffer, BOOL bOut = FALSE)						{ UNUSED_PARA(nIndex); UNUSED_PARA(pullBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, FLOAT* pfBuffer, BOOL bOut = FALSE)							{ UNUSED_PARA(nIndex); UNUSED_PARA(pfBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	SetParam(INT16 nIndex, double* pfBuffer, BOOL bOut = FALSE)							{ UNUSED_PARA(nIndex); UNUSED_PARA(pfBuffer); UNUSED_PARA(bOut); return FALSE; }
		virtual BOOL	ResetParameters()							{ return FALSE; }
		virtual INT16	GetParamCount()								{ return 0; }

		//	info.
		INT32			GetAffectedRows();
		virtual BOOL	HasResult() = 0;

		LPCTSTR			GetError();
		
		// Extra
		virtual void		SetTransactionMode(BOOL bInstantCommit)			{ return; }
	};


inline LPCTSTR AuStatement::GetError()
	{
	return m_szError;
	}




/************************************************/
/*		The Definition of Heap Buffer class		*/
/************************************************/
//
class AuDatabaseBuffer
	{
	protected:
		static HANDLE	s_hHeap;

		LPBYTE			m_pBuffer;
		DWORD			m_dwAlloc;
		DWORD			m_dwUsed;

	public:
		static BOOL	Initialize(DWORD dwSize=0); // nSize is M-byte unit. 0 for growable.
		static void	Cleanup();

		AuDatabaseBuffer();
		~AuDatabaseBuffer();

		BOOL	Alloc(DWORD dwSize);	// total allocated buffer size(total buffer object)
		void	Free();

		LPBYTE	GetHead();
		LPBYTE	GetTail();
		DWORD	GetSize();

		void	Reset();				// reset used size, not free
		void	AddUsed(DWORD dwUsed);	// add used size of allocated buffer

	protected:
		BOOL	RealAlloc(DWORD dwSize);

  #ifdef _DEBUG
	DWORD			m_dwAllocCount;
	static DWORD	m_dwInitialized;
  #endif
	};




/********************************************/
/*		The Definition of Column struct		*/
/********************************************/
//
struct stColumn
	{
	TCHAR				m_szName[AUDB_MAX_COLUMN_NAME_SIZE];
	eAUDB_TYPE			m_eType;
	eAUDB_TYPE			m_eBindType;
//	INT32				m_nPrecision;
//	INT32				m_nScale;

	stColumn()
		{
		m_szName[0] = _T('\0');
		}
	};




/************************************************/
/*		The Definition of Rowset Base class		*/
/************************************************/
//
class AuRowset
	{
	private:
		//	lifetime
		UINT8	m_cRef;

	protected:
		AuStatement*		m_pStatement;

		//	info.
		stColumn	m_stColumns[AUDB_MAX_COLUMN_SIZE];	// column info.
		ULONG		m_ulRows;
		ULONG		m_ulCols;
		UINT32		m_ulRowBufferSize;					// buffer size of 1 row
		BOOL		m_bEnd;
		TCHAR		m_szError[AUDB_DEFAULT_ERROR_STRING_SIZE];

		//	data buffer
		AuDatabaseBuffer	m_Buffer;

	protected:
		virtual ~AuRowset();

	public:
		AuRowset();
		UINT8	AddRef();
		UINT8	Release();

		//	bind and fetch
		virtual		BOOL	Bind(BOOL bBindToString) = 0;
		virtual		BOOL	Fetch(UINT16 uiFetchCount = AUDB_DEFAULT_FETCH_COUNT) = 0;

		//	value
		virtual		PVOID	Get(UINT32 ulRow, UINT32 ulCol) = 0;
		virtual		PVOID	GetHeadPtr();

		//	info.
		virtual	stColumn&	GetColumnInfo(UINT32 ulCol);
		virtual BOOL		IsEnd();			// No More Fetched Data
		UINT32				GetRowCount();		// Total Fetched Rows
		UINT32				GetColCount();		// Total Columns
		UINT32				GetRowBufferSize();
		virtual UINT32		GetOffset(UINT32 ulCol) = 0;

		//	extra
		void			InitializeBuffer(DWORD dwBufferSizeK);	// K-Byte
		LPCTSTR			GetError();
		virtual void	Set(AuStatement& Statement);
		virtual void	Close();
	};


inline LPCTSTR AuRowset::GetError()
	{
	return m_szError;
	}


inline BOOL AuRowset::IsEnd()
	{
	return m_bEnd;
	}


inline UINT32 AuRowset::GetColCount()
	{
	return m_ulCols;
	}


inline UINT32 AuRowset::GetRowCount()
	{
	return m_ulRows;
	}


inline UINT32 AuRowset::GetRowBufferSize()
	{
	return m_ulRowBufferSize;
	}




/********************************************************/
/*		The Definition of Manager(Factory) class		*/
/********************************************************/
//
typedef AuDatabase*		(*PRIVATE_DATABASE)(void);
typedef AuStatement*	(*PRIVATE_STATEMENT)(AuDatabase*);
typedef AuRowset*		(*PRIVATE_ROWSET)(AuStatement*);

class AuDatabaseManager
	{
	protected :
		AuEnvironment*				m_pEnvironment;
		BOOL						m_bInit;

	public :
		eAUDB_API					m_eApi;
		eAUDB_VENDER				m_eVender;

	public :
		AuDatabaseManager();
		virtual ~AuDatabaseManager();

		//	Init/Term
		BOOL		Initialize(eAUDB_API eApi = AUDB_API_ODBC, eAUDB_VENDER eVender = AUDB_VENDER_ORACLE);
		void		Terminate();

		//	Database
		AuDatabase*		CreateDatabase();
		AuStatement*	CreateStatement(AuDatabase *pDatabase);
		AuRowset*		CreateRowset(AuStatement *pStatement);

		//	Heap
		//		If ya don't use this method, Buffer uses process' default heap area
		BOOL		InitializeHeap(DWORD dwSize = AUDB_DEFAULT_HEAP_SIZE);
		void		TerminateHeap();

	private :
		AuDatabase*		CreateDatabaseOLEDB();
		AuStatement*	CreateStatementOLEDB(AuDatabase *pDatabase);
		AuRowset*		CreateRowsetOLEDB(AuStatement *pStatement);

		AuDatabase*		CreateDatabaseODBC();
		AuStatement*	CreateStatementODBC(AuDatabase *pDatabase);
		AuRowset*		CreateRowsetODBC(AuStatement *pStatement);

		AuDatabase*		CreateDatabaseOCI();
		AuStatement*	CreateStatementOCI(AuDatabase *pDatabase);
		AuRowset*		CreateRowsetOCI(AuStatement *pStatement);
	};


#endif	// _AU_DATABASE_H_

