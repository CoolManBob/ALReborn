/*=======================================================================

	AuOLEDB.h

=======================================================================*/

#ifndef _AU_OLEDB_H_
	#define _AU_OLEDB_H_


#include <oledb.h>
#include "AuDatabase.h"


/********************************************************/
/*		The Definition of OLEDB Environment class		*/
/********************************************************/
//
class AuEnvOLEDB : public AuEnvironment
	{
	friend class AuDatabaseOLEDB;

	protected:
		IUnknown*			m_pIEnumerator;
		ISourcesRowset*		m_pISourcesRowset;
		IParseDisplayName*	m_pIParseDisplayName;

	public:
		AuEnvOLEDB();
		AuEnvOLEDB(eAUDB_VENDER eVender);
		virtual ~AuEnvOLEDB();

		BOOL	Open();
		void	Close();

	private:
		IDBInitialize*		AllocConn();
		void				FreeConn(IUnknown* pIDBInitialize);
	};




//	Attribute(Property)
//=============================================
//
HRESULT	Set(IUnknown* pIUnknown, ULONG cPropertySets, DBPROPSET rgPropertySets[]);
HRESULT Get(IUnknown* pIUnknown, ULONG cPropertyIDSets, const DBPROPIDSET rgPropertyIDSets[],
								 ULONG* pcPropertySets, DBPROPSET** prgPropertySets);




/****************************************************/
/*		The Definition of OLEDB Database class		*/
/****************************************************/
//
class AuDatabaseOLEDB : public AuDatabase
	{
	friend class AuStmtOLEDB;

	protected:
		AuEnvOLEDB*				m_pEnvironment;
		IDBInitialize*			m_pIDBInitialize;
		IDBProperties*			m_pIDBProperties;
		ITransactionLocal*		m_pITransaction;
		IUnknown*				m_pISession;

		BOOL					m_bOpen;

	  #ifndef _UNICODE	
		WCHAR					m_wszUser[AUDB_MAX_DSN_SIZE];
		WCHAR					m_wszPassword[AUDB_MAX_DSN_SIZE];
		WCHAR					m_wszDatasource[AUDB_MAX_DSN_SIZE];
	  #endif

	public:
		AuDatabaseOLEDB(AuEnvironment& Environment);
		virtual ~AuDatabaseOLEDB();

		//	Open(Connect)
		BOOL	Open(LPCTSTR pszDatasource, LPCTSTR pszUser, LPCTSTR pszPassword);
		BOOL	Open(LPCTSTR pszConnectionString)	{ UNUSED_PARA(pszConnectionString); return FALSE; }
		void	Close();

		//	Transaction
		BOOL	BeginTransaction(eAUDB_ISOLATION_LEVEL eIsolation);
		BOOL	EndTransaction(BOOL fCommit = TRUE);

		//	Status
		BOOL	IsAlive();
	};




/************************************************************/
/*		The Definition of OLEDB Parameter Info. class		*/
/************************************************************/
//
class AuParamInfoOLEDB
	{
	public:
		DBLENGTH	m_ulParamSize;
		BYTE		m_bPrecision;
		BYTE		m_bScale;
		LPOLESTR	m_pwszDataSourceType;
		DBTYPE		m_wType;
		DBLENGTH	m_cbMaxLen;
	};




/****************************************************/
/*		The Definition of OLEDB Statement class		*/
/****************************************************/
//
class AuStmtOLEDB : public AuStatement
	{
	friend class AuRowsetOLEDB;

	protected:
		WCHAR					m_wszCommand[AUDB_MAX_QUERY_SIZE];
		AuDatabaseOLEDB*		m_pDatabase;
		IDBCreateCommand*		m_pISession;
		ICommandText*			m_pICommandText;
		IRowset*				m_pIRowset;

		//	parameterized
		ICommandWithParameters*	m_pICommandWithParameters;
		HACCESSOR				m_hAccessor;
		IAccessor*				m_pIAccessor;
		INT16					m_nParams;
		DBPARAMS				m_Params[AUDB_MAX_COLUMN_SIZE];
		AuParamInfoOLEDB		m_ParamInfo[AUDB_MAX_COLUMN_SIZE];

	public:
		AuStmtOLEDB(AuDatabase& Database);
		virtual ~AuStmtOLEDB();

		BOOL	Open();
		void	Close();

		//	Direct
		BOOL	Execute(LPCTSTR pszQuery);

		//	Parameterized;
		BOOL	Prepare(LPCTSTR pszQuery);
		BOOL	Execute();
		BOOL	SetParam(INT16 nIndex, PVOID pBuffer, INT32 lBufferSize = 0, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, CHAR* pszBuffer, INT32 lBufferSize = 0, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT8* pcBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT16* pnBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT32* plBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT64* plBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT8* pucBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT16* punBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT32* pulBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT64* plBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, FLOAT* pfBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, double* pfBuffer, BOOL bOut = FALSE)			{ UNUSED_PARA(nIndex); UNUSED_PARA(pfBuffer); UNUSED_PARA(bOut); return FALSE;}

		//	Result
		UINT32		GetAffectedRows();
		BOOL		HasResult();
	};




/****************************************************/
/*		The Definition of OLEDB Rowset class		*/
/****************************************************/
//
class AuRowsetOLEDB : public AuRowset
	{
	protected:
		IRowset*			m_pIRowset;

		//	Binding
		DBCOLUMNINFO*		m_pDBColumnInfo;
		DBBINDING*			m_pBindings;
		IAccessor*			m_pIAccessor;
		HACCESSOR			m_hAccessor;

		HROW				m_Rows[AUDB_MAX_FETCH_COUNT];
		DBBINDING			m_Bindings[AUDB_MAX_COLUMN_SIZE];

	public:
		AuRowsetOLEDB();
		AuRowsetOLEDB(AuStatement& Statement);

	protected:
		void	Close();
		virtual ~AuRowsetOLEDB();

	protected:
		void	BindPerType();
		void	BindToString();			// bind all types to string

	public:
		//	Bind and fetch method
		BOOL	Bind(BOOL bBindToString = TRUE);
		BOOL	Fetch(UINT16 uiFetchCount = AUDB_DEFAULT_FETCH_COUNT);

		//	Info. method
		BOOL	IsEnd();
		UINT32	GetOffset(UINT32 ulCol);

		//	Value method
		PVOID	Get(UINT32 ulRow, UINT32 ulCol);

		//	Extra
		void	Set(AuStatement& Statement);
	};




/****************************************************/
/*		The Definition of GetErrorInfo Function		*/
/****************************************************/
//
void AuGetErrorInfoOLEDB(LPTSTR pszError);


#endif