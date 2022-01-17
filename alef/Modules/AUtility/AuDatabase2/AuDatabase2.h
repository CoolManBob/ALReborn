/*=================================================================

	AuDatabase2.h
	
		AuDatabase의 database, statement, rowset을 하나의
		클래스로 래핑해 간단히 쓸 수 있게한 놈.

=================================================================*/

#ifndef _AU_DATABASE2_H_
	#define	_AU_DATABASE2_H_

#include "AuDatabase.h"
#include "ApModule.h"
#include "AgsmDatabaseConfig.h"
#include "zzThread.h"


/****************************************/
/*		The Definition Constants		*/
/****************************************/
//
enum	eAUDATABASE2_QR
	{
	AUDATABASE2_QR_FAIL			= 0,
	AUDATABASE2_QR_NO_RESULT,
	AUDATABASE2_QR_SUCCESS,
	AUDATABASE2_QR_NO_ROW_UPDATE,
	};




/****************************************************/
/*		The Definition of Simple Database class		*/
/****************************************************/
//
class AuDatabase2
	{
	protected:
		AuDatabaseManager	*m_pManager;
		AuDatabase			*m_pDatabase;
		AuStatement			*m_pStatement;
		AuRowset			*m_pRowset;
	
		UINT32				m_ulCurrentRow;

	public:
		HANDLE			m_hSemaphore;

	public:
		AuDatabase2();
		virtual ~AuDatabase2();

		//	Init/Term
		BOOL	Initialize(AuDatabaseManager *pManager);
		BOOL	Connect(CHAR *pszUser, CHAR *pszPassword, CHAR *pszDatasource);
		void	Terminate();

		//	Query
		BOOL	SetQueryText(CHAR *pszQuery);
		INT8	ExecuteQuery();
		BOOL	GetNextRow();
		CHAR*	GetQueryResult(UINT32 ulColumn);
		BOOL	GetQueryINT32(UINT32 ulColumn, INT32* desc);
		BOOL	GetQueryUINT32(UINT32 ulColumn, UINT32* desc);
		BOOL	GetQueryDWORD(UINT32 ulColumn, DWORD* desc);
		UINT32	GetCurrentColumns();
		UINT32	GetReadRows();
		void	EndQuery();

		//	Transaction
		BOOL	StartTranaction();
		BOOL	CommitTransaction();
		BOOL	AbortTransaction();

		//	Bind Variable
		BOOL	SetParam(INT16 nIndex, CHAR *psz, INT32 lSize, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, UINT32 *pul, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT32 *pl, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT8 *pc, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT16 *pn, BOOL bOut = FALSE);
		BOOL	SetParam(INT16 nIndex, INT64 *pn, BOOL bOut = FALSE);
		
		//
		void	InitializeSemaphore();
		void	ReleaseSemaphore();
		
		eAUDB_VENDER	GetVender();
	};



class AuAutoReleaseDS
	{
	private:
		AuDatabase2		*m_pDatabase;
		
	public:
		AuAutoReleaseDS(AuDatabase2 *pDatabase)
			: m_pDatabase(pDatabase)
			{
			}
		~AuAutoReleaseDS()
			{
			m_pDatabase->ReleaseSemaphore();
			}
	};


#endif