/*===================================================================

	AuDatabaseOCI.h

===================================================================*/

#ifndef _AUDATABASE_OCI_H_
	#define _AUDATABASE_OCI_H_

#ifdef ORACLE_THROUGH_OCI

#include "AuDatabase.h"
#include "oci.h"


class AuEnvOCI;
class AuDatabaseOCI;
class AuStmtOCI;
class AuRowsetOCI;


/****************************************************/
/*		The Definition of OCI Environment class		*/
/****************************************************/
//
class AuEnvOCI : public AuEnvironment
	{
	friend class AuDatabaseOCI;

	public:
		AuEnvOCI(eAUDB_VENDER eVender);
		virtual ~AuEnvOCI();

		BOOL	Open();
		void	Close();
	
	private:
		BOOL	_AllocEnv(OCIEnv **pphEnvironment, OCIError **pphError);
		void	_FreeEnv(OCIEnv *phEnvironment, OCIError *phError);
	};




/****************************************************/
/*		The Definition of OCI Database class		*/
/****************************************************/
//
class AuDatabaseOCI : public AuDatabase
	{
	friend class AuStmtOCI;

	protected:
		AuEnvOCI		*m_pEnvironment;

		OCIEnv			*m_phEnvironment;
		OCIError		*m_phError;
		OCIServer		*m_phServer;
		OCISvcCtx		*m_phServiceContext;
		OCISession		*m_phSession;

	public:
		AuDatabaseOCI(AuEnvironment& Environment);
		virtual ~AuDatabaseOCI();

		//	Connection
		BOOL	Open(LPCTSTR pszDatasource, LPCTSTR pszUser, LPCTSTR pszPassword);
		BOOL	Open(LPCTSTR pszConnectionString);
		void	Close();

		//	Transaction
		BOOL	BeginTransaction(eAUDB_ISOLATION_LEVEL eIsolation);
		BOOL	EndTransaction(BOOL fCommit = TRUE);

		//	Status
		BOOL	IsAlive();

	private:
		//	Pre-connection
		BOOL	_AllocServer(CHAR *pszDatasource);
		void	_FreeServer();
		BOOL	_AllocContext();
		void	_FreeContext();
		BOOL	_AllocSession();
		void	_FreeSession();
		
		//	Statement
		BOOL	_AllocSTMT(OCIStmt **pphStatement);
		void	_FreeSTMT(OCIStmt *phStatement);
	};




/****************************************************/
/*		The Definition of OCI Parameter class		*/
/****************************************************/
//
class AuParamOCI
	{
	public:
		// handle
		OCIStmt		*m_phStatement;
		OCIError	*m_phError;
		OCIBind		*m_phParam;
		
		// position
		INT16		m_nIndex;

	protected:
		void	_Initialize();

	public:
		AuParamOCI()	{ _Initialize(); }

		void	Set(OCIStmt *phStatement, OCIError *phError, INT16 nIndex);
		BOOL	Describe();
		BOOL	Bind(PVOID pBuffer, sb4 lBufferSize, ub2 unType);
	};

		


/****************************************************/
/*		The Definition of OCI Statement class		*/
/****************************************************/
//
class AuStmtOCI : public AuStatement
	{
	friend class AuRowsetOCI;

	protected:
		AuDatabaseOCI	*m_pDatabase;
		OCIStmt			*m_phStatement;
		OCIError		*m_phError;

		UINT32			m_ulCols;			// result column count(0 if DML)
		INT16			m_nParams;			// No. of parameter

		AuParamOCI		m_Params[AUDB_MAX_COLUMN_SIZE];
		// dummy for bind variable
		text			*m_pszNames[AUDB_MAX_COLUMN_SIZE];
		ub1				m_ucNameSizes[AUDB_MAX_COLUMN_SIZE];
		text			*m_pszInd[AUDB_MAX_COLUMN_SIZE];
		ub1				m_ucIndSizes[AUDB_MAX_COLUMN_SIZE];
		ub1				m_ucDupl[AUDB_MAX_COLUMN_SIZE];
		OCIBind*		m_phBinds[AUDB_MAX_COLUMN_SIZE];

		ub4				m_ulMode;
	protected:
		BOOL	_DescribeParams();
		//BOOL	_Execute

	public:
		AuStmtOCI(AuDatabase& Database);
		virtual ~AuStmtOCI();

		BOOL	Open();
		void	Close();

		//	direct
		BOOL	Execute(LPCTSTR pszQuery);

		//	paramterized
		BOOL	Prepare(LPCTSTR pszQuery);
		//BOOL	Prepare2(LPCTSTR pszQuery, LPCTSTR pszKey);
		BOOL	Execute();
		BOOL	SetParam(INT16 nIndex, PVOID pBuffer, INT32 lBufferSize = 0, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, CHAR* pBuffer, INT32 lBufferSize = 0, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT8* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT16* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT32* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT64* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT8* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT16* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT32* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT64* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, FLOAT* pBuffer, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, double* pBuffer, BOOL bOut = FALSE);
		BOOL	ResetParameters();
		INT16	GetParamCount()		{ return m_nParams; }

		//	Result
		UINT32		GetAffectedRows();
		BOOL		HasResult();
		
		//	Extra
		void		SetTransactionMode(BOOL bInstantCommit);
	};




/************************************************/
/*		The Definition of OCI Column class		*/
/************************************************/
//
class AuColumnOCI
	{
	protected:
		TCHAR			m_szName[AUDB_MAX_COLUMN_NAME_SIZE];
		OCIParam		*m_phColumn;
		ub2				m_unType;
		ub2				m_unColumnSize;
		ub1				m_ucPrecision;
		sb1				m_cScale;
		BOOL			m_bIsRequired;

	public:
		AuColumnOCI();

		//	Get Column Info.
		BOOL		GetInfo(OCIStmt *phStatement, OCIError *phError, ub4 lIndex);

		//	Value Method
		LPCTSTR		Name();
		ub2			Type();
		ub2			ColumnSize();
		ub1			Precision();
		sb1			Scale();
		BOOL		IsRequired();

	private:
		void	Initialize();
	};



/************************************************/
/*		The Definition of OCI Binding class		*/
/************************************************/
//
class AuBindingOCI
	{
	protected:
		ub2			m_unTargetType;		//	target type. not original type
		ULONG		m_ulOffset;			//	offset
		ULONG		m_ulMaxLen;			//	maximum length

	public:
		OCIDefine		*m_pDefine;
	
	public:
		AuBindingOCI()
			{
			m_pDefine = NULL;
			m_unTargetType = 0;
			m_ulOffset = 0;
			m_ulMaxLen = 0;
			}

		ULONG		Set(AuColumnOCI *pColumn, ULONG ulOffset);	// string binding
		ULONG		Set2(AuColumnOCI *pColumn, ULONG ulOffset);	// column type oriented binding
		void*		Value(LPBYTE pBuffer);
		ULONG		ValueOffset();
		sb2*		Ind(LPBYTE pBuffer);
		ULONG		MaxLen();
		sb2			Type();
	};




/************************************************/
/*		The Definition of OCI Rowset class		*/
/************************************************/
//
class AuRowsetOCI : public AuRowset
	{
	protected:
		AuStmtOCI			*m_pStatement;

		//	Column
		AuColumnOCI			*m_pDBColumnInfo;
		AuColumnOCI			m_Columns[AUDB_MAX_COLUMN_SIZE];

		//	Binding
		AuBindingOCI		*m_pBindings;
		AuBindingOCI		m_Bindings[AUDB_MAX_COLUMN_SIZE];

	public:
		AuRowsetOCI();
		AuRowsetOCI(AuStatement& Statement);
	
	protected:
		virtual		~AuRowsetOCI();
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



/********************************************/
/*		The Definition Helper Function		*/
/********************************************/
//
sword	_Get(dvoid *phHandle, ub4 HandleType, dvoid *pAttribute, ub4 *pSize, ub4 Attribute, OCIError *phError);
sword	_Set(dvoid *phHandle, ub4 HandleType, dvoid *pAttribute, ub4 Size, ub4 Attribute, OCIError *phError);
void	AuGetErrorInfoOCI(OCIError *phError, CHAR *pszError, size_t size);


#endif				// ORACLE_THROUGH_OCI

#endif				//_AUDATABASE_OCI_H_

