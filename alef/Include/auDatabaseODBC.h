/*===================================================================

	zzODBC.h

===================================================================*/

#ifndef _ZZ_ODBC_H_
	#define _ZZ_ODBC_H_

#include <sqlext.h>
#include "AuDatabase.h"


class AuEnvODBC;
class AuDatabaseODBC;
class AuStmtODBC;
class AuRowsetODBC;


#define SQL_STRING_LENGTH	256


/********************************************************/
/*		The Definition of ODBC Environment class		*/
/********************************************************/
//
class AuEnvODBC : public AuEnvironment
	{
	friend class AuDatabaseODBC;

	protected:
		SQLHENV			m_hEnvironment;

	public:
		AuEnvODBC(eAUDB_VENDER eVender);
		virtual ~AuEnvODBC();

		BOOL	Open();
		void	Close();

	private:
		SQLHDBC		_AllocConn();
		void		_FreeConn(SQLHDBC hConnection);
	};




/****************************************************/
/*		The Definition of ODBC Database class		*/
/****************************************************/
//
class AuDatabaseODBC : public AuDatabase
	{
	friend class AuStmtODBC;

	protected:
		AuEnvODBC*			m_pEnvironment;
		SQLHDBC				m_hConnection;

	public:
		AuDatabaseODBC(AuEnvironment& Environment);
		virtual ~AuDatabaseODBC();

		//	Open(Connect)
		BOOL	Open(LPCTSTR pszDatasource, LPCTSTR pszUser, LPCTSTR pszPassword);
		BOOL	Open(LPCTSTR pszConnectionString);
		void	Close();

		//	Transaction
		BOOL	BeginTransaction(eAUDB_ISOLATION_LEVEL eIsolation);
		BOOL	EndTransaction(BOOL fCommit = TRUE);

		//	Status
		BOOL	IsAlive();

	protected:
		//	Attribute
		SQLRETURN	_Get(SQLINTEGER Attribute, SQLPOINTER pValue, SQLINTEGER BufferLength, SQLINTEGER* pStringLength);
		SQLRETURN	_Set(SQLINTEGER Attribute, SQLPOINTER pValue, SQLINTEGER StringLength);

	private:
		SQLHSTMT	_AllocSTMT();
		void		_FreeSTMT(SQLHDBC hConnection);
	};




/****************************************************/
/*		The Definition of ODBC Parameter class		*/
/****************************************************/
//
class AuParamODBC
	{
	//private:
	public:
		SQLHSTMT		m_hStatement;
		SQLUSMALLINT	m_nIndex;
		SQLSMALLINT		m_nType;
		SQLULEN			m_nSize;
		SQLSMALLINT		m_nDecimalDigits;
		SQLLEN			m_nInd;
		BOOL			m_bNull;

	protected:
		void	_Initialize();

	public:
		AuParamODBC()	{ _Initialize(); }

		void	Set(SQLHSTMT hStatement, SQLUSMALLINT nIndex);
		BOOL	Describe();
		BOOL	Bind(PVOID pBuffer, INT32 lBufferSize, INT16 nValueType, INT16 nInOut = SQL_PARAM_INPUT);
	};




/****************************************************/
/*		The Definition of ODBC Statement class		*/
/****************************************************/
//
class AuStmtODBC : public AuStatement
	{
	friend class AuRowsetODBC;

	protected:
		AuDatabaseODBC*		m_pDatabase;
		AuParamODBC			m_Params[AUDB_MAX_COLUMN_SIZE];

		SQLHSTMT			m_hStatement;
		UINT32				m_ulCols;			// result column count(0 if DML)

		INT16				m_nParams;			// No. of parameter

	protected:
		BOOL	_DescribeParams();
		//BOOL	_Execute

	public:
		AuStmtODBC(AuDatabase& Database);
		virtual ~AuStmtODBC();

		BOOL	Open();
		void	Close();

		//	direct
		BOOL	Execute(LPCTSTR pszQuery);

		//	paramterized
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
		BOOL	SetParam(INT16 nIndex, UINT64* pullBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, FLOAT* pfBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, double* pfBuffer, BOOL bOut = FALSE);
		BOOL	ResetParameters();
		INT16	GetParamCount()		{ return m_nParams; }

		//	Result
		UINT32		GetAffectedRows();
		BOOL		HasResult();
	};




/****************************************************/
/*		The Definition of AuColumnODBC class		*/
/****************************************************/
//
class AuColumnODBC
	{
	protected:
		TCHAR			m_szName[SQL_STRING_LENGTH];
		SQLSMALLINT		m_nType;
		SQLULEN			m_nColumnSize;
		SQLSMALLINT		m_nDecimalDigits;
		BOOL			m_bIsRequired;

	public:
		AuColumnODBC();

		//	Get Column Info.
		BOOL		GetInfo(SQLHSTMT hStatement, short nIndex);

		//	Value Method
		LPCTSTR		Name();
		SQLSMALLINT	Type();
		SQLINTEGER	ColumnSize();
		SQLSMALLINT	DecimalDigits();
		BOOL		IsRequired();

	private:
		void	Initialize();
	};




/****************************************************/
/*		The Definition of AuBindingODBC class		*/
/****************************************************/
//
class AuBindingODBC
	{
	protected:
		SQLSMALLINT		m_TargetType;		// target type. not original type
		ULONG			m_ulOffset;			// offset
		ULONG			m_ulMaxLen;			// maximum length
	
	public:
		AuBindingODBC()
			{
			m_TargetType = SQL_C_TCHAR;
			m_ulOffset = 0;
			m_ulMaxLen = 0;
			}

		ULONG		Set(AuColumnODBC* pColumnInfo, ULONG ulOffset);		// string binding
		ULONG		Set2(AuColumnODBC* pColumnInfo, ULONG ulOffset);	// column oriented binding
		void*		Value(LPBYTE pBuffer);
		ULONG		ValueOffset();
		SQLLEN*		Ind(LPBYTE pBuffer);
		ULONG		MaxLen();
		SQLSMALLINT	Type();
	};




/************************************************/
/*		The Definition of ODBC Rowset class		*/
/************************************************/
//
class AuRowsetODBC : public AuRowset
	{
	protected:
		AuStmtODBC*			m_pStatement;

		//	Column
		AuColumnODBC*		m_pDBColumnInfo;
		AuColumnODBC		m_ColumnODBC[AUDB_MAX_COLUMN_SIZE];

		//	Binding
		AuBindingODBC*		m_pBindings;
		AuBindingODBC		m_BindingODBC[AUDB_MAX_COLUMN_SIZE];

	public:
		AuRowsetODBC();
		AuRowsetODBC(AuStatement& Statement);
	
	protected:
		virtual		~AuRowsetODBC();
		void		Close();

	public:
		//	bind and fetch
		BOOL		Bind(BOOL bBindToString = TRUE);
		BOOL		Fetch(UINT16 uiFetchCount = AUDB_DEFAULT_FETCH_COUNT);

		//	info.
		BOOL		IsEnd();
		UINT32		GetOffset(UINT32 ulCol);

		//	value
		PVOID		Get(UINT32 ulRow, UINT32 ulCol);

		//	extra
		void		Set(AuStatement& Statement);
	};




/****************************************************/
/*		The Definition of GetErrorInfo Function		*/
/****************************************************/
//
void AuGetErrorInfoODBC(LPTSTR pszError, SQLSMALLINT HandleType, SQLHANDLE hHandle);


#endif



