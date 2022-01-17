/*============================================================

	AgsdDatabase.h

=============================================================*/

#ifndef _AGSD_DATABASE_H_
	#define _AGSD_DATABASE_H_

#include "ApBase.h"
#include "ApDefine.h"
#include "AgsEngine.h"
#include "AuDatabase.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
typedef enum _eAgsmDatabaseOperation
	{
	AGSMDATABASE_OPERATION_UNKNOWN = 0,
	AGSMDATABASE_OPERATION_EXECUTE = 100,
	AGSMDATABASE_OPERATION_INSERT = 200,
	AGSMDATABASE_OPERATION_UPDATE = 300,
	AGSMDATABASE_OPERATION_DELETE = 400,
	AGSMDATABASE_OPERATION_SELECT = 500,
	} eAgsmDatabaseOperation;

typedef enum _eAgsmDatabaseParam
	{
	AGSMDATABASE_PARAM_CUSTOM = 0,
	AGSMDATABASE_PARAM_RELAY = 1,
	AGSMDATABASE_PARAM_AUCTION = 40,
	AGSMDATABASE_PARAM_RECRUIT = 50,
	} eAgsmDatabaseParam;

#define _MAX_QUERY_BUFFER_SIZE			1024 * 2		// 2K
#define _MAX_CONN_PARAM_STRING_LENGTH	128
#define	_MAX_QUERY_LENGTH				512
#define	_MAX_HEADER_STRING_LENGTH		512
#define	_MAX_COLUMN_SIZE				AUDB_MAX_COLUMN_SIZE

/************************************************/
/*		The Definition of Parameter class		*/
/************************************************/
//
//	==========		Base		==========
//
class AgsdDBParam
	{
	public:
		INT16	m_nParam;
		INT16	m_eOperation;
		UINT32	m_ulNID;
		INT16	m_nForcedPoolIndex;

	public:
		AgsdDBParam();
		virtual ~AgsdDBParam();
		virtual void Release();

		BOOL	SetParam(AuStatement* pStatement);
		void	DumpParam();
		
		virtual void	SetStaticQuery(CHAR *pszDest, CHAR *pszFmt);
		virtual void	Dump(CHAR *pszOp);

	protected:
		virtual BOOL	SetParamExecute(AuStatement* pStatement);
		virtual BOOL	SetParamInsert(AuStatement* pStatement);
		virtual BOOL	SetParamUpdate(AuStatement* pStatement);
		virtual BOOL	SetParamDelete(AuStatement* pStatement);
		virtual BOOL	SetParamSelect(AuStatement* pStatement);
	};

//
//	==========		Custom		==========
//
class AgsdDBParamCustom : public AgsdDBParam, public ApMemory<AgsdDBParamCustom, 10000>
	{
	public:
		INT32	m_lQueryIndex;						// query index, if ne 0
		CHAR	m_szQuery[_MAX_QUERY_LENGTH+1];		// custom query string

		CHAR	m_szHeaders[_MAX_HEADER_STRING_LENGTH+1];		// column headers
		UINT32	m_ulRows;									// row count
		UINT32	m_ulCols;									// column count

		PVOID	m_pBuffer;							// data buffer ptr.
		UINT32	m_ulRowBufferSize;					// size of 1 row
		UINT32	m_ulTotalBufferSize;				// total buffer size. m_ulRowBufferSize * m_ulCols
		//INT32	m_lOffsets[_MAX_COLUMN_SIZE];		// offsets between columns
		ApSafeArray<INT32, _MAX_COLUMN_SIZE>	m_lOffsets;
		
	public:
		AgsdDBParamCustom()	{}
		virtual ~AgsdDBParamCustom()	{}
		virtual void Release()	{ delete this; }

		CHAR*	Get(UINT32 ulRow, UINT32 ulCol);

	protected:
		BOOL	SetParamExecute(AuStatement* pStatement);
		BOOL	SetParamInsert(AuStatement* pStatement);
		BOOL	SetParamUpdate(AuStatement* pStatement);
		BOOL	SetParamDelete(AuStatement* pStatement);
		BOOL	SetParamSelect(AuStatement* pStatement);
		BOOL	SetParamCustom(AuStatement* pStatement);
	};


/********************************************/
/*		The Definition of Query class		*/
/********************************************/
//
//	==========		Base		==========
//
class AgsdQuery
	{
	public:
		INT16	m_nIndex;

		// ... Callback
		ApModuleDefaultCallBack	m_pfCallback;
		ApModuleDefaultCallBack	m_pfCallbackFail;
		PVOID	m_pClass;
		PVOID	m_pData;

	protected:
		virtual ~AgsdQuery();

	public:
		AgsdQuery();
		virtual void Release();

		INT16			GetIndex();
		virtual CHAR*	Get() = 0;
		virtual void	Set(CHAR* pszQuery) = 0;
		virtual BOOL	BindParam(AuStatement *pStatement);

		inline void SetCallback(ApModuleDefaultCallBack pfCallback,
								ApModuleDefaultCallBack pfCallbackFail,
								PVOID pClass, PVOID pData)
			{
			m_pfCallback = pfCallback;
			m_pfCallbackFail = pfCallbackFail;
			m_pClass = pClass;
			m_pData = pData;
			}

		virtual void	Dump();
	};


//
//	==========		Parameterized		==========
//
class AgsdQueryWithParam : public AgsdQuery, public ApMemory<AgsdQueryWithParam, 200000>
	{
	public:
		CHAR*			m_pszQuery;
		AgsdDBParam*	m_pParam;

	protected:
		virtual ~AgsdQueryWithParam();

	public:
		AgsdQueryWithParam();
		virtual void Release();

		CHAR*	Get();
		void	Set(CHAR* pszQuery);
		BOOL	BindParam(AuStatement *pStatement);

		virtual void	Dump();
	};


//
//	==========		Static		==========
//
class AgsdQueryStatic : public AgsdQuery, public ApMemory<AgsdQueryStatic, 20000>	// Pooling = 20000
	{
	public:
		CHAR	m_szQuery[_MAX_QUERY_BUFFER_SIZE];		// 2K

	protected:
		virtual ~AgsdQueryStatic();

	public:
		AgsdQueryStatic();
		virtual void Release();

		CHAR*	Get();
		void	Set(CHAR* pszQuery);
		inline void Init()
			{
			ZeroMemory(m_szQuery, sizeof(m_szQuery));
			}

		virtual void	Dump();
	};

#endif