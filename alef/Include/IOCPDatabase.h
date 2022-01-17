/*===================================================================

	IOCPDatabase.h

===================================================================*/

#ifndef _IOCP_DATABASE_H_
	#define _IOCP_DATABASE_H_

#include "ApIOCP.h"
#include "ApModule.h"
#include "AuDatabase.h"
#include "ApMemory.h"
#include "zzThread.h"
#include "AgsdDatabase.h"


/************************************************/
/*		The Definition of Simple IOCP class		*/
/************************************************/
//
class zzIOCP
	{
	protected:
		HANDLE		m_hIOCP;
		LONG		m_lPosted;
		LONG		m_lQueued;
		LONG		m_lExecuted;
		LONG		m_lFailed;

	public:
		zzIOCP();
		virtual ~zzIOCP();

		BOOL	Create(DWORD dwThreadCount);
		void	Destroy();
		
		BOOL	PostStatus(ULONG_PTR dwKey, DWORD dwBytes = 0, LPOVERLAPPED pOverlapped = NULL);
		BOOL	GetStatus(PULONG_PTR pdwKey, DWORD *pdwBytes, LPOVERLAPPED *ppOverlapped, DWORD dwTime);

		LONG	Executed();
		LONG	Failed();
		
		LONG	GetExecuted()	{ return m_lExecuted; }
		LONG	GetPosted()	{ return m_lPosted; }
		LONG	GetFailed()	{ return m_lFailed; }
		
		void	WriteStatus();
	};


/************************************************/
/*		The Definition of DBWorker class		*/
/************************************************/
//
class DBWorker : public zzThread
	{
	friend class IOCPDatabase;

	protected:
		INT16				m_nIndex;
		BOOL				m_bIgnoreFail;
		IOCPDatabase*		m_pManager;
		AuDatabase*			m_pConnection;
		AuStatement*		m_pStatement;
		AuRowset*			m_pRowset;
		zzIOCP*				m_pIOCP;

		BOOL				m_bStop;

		BOOL				m_bBufferingCommit;
		INT32				m_lNowBufferingCount;
		INT32				m_lMaxBufferingCount;
		CRITICAL_SECTION	m_csBufferingLock;

		typedef std::vector<AgsdQuery*>				BufferingQueryVector;
		typedef std::vector<AgsdQuery*>::iterator	BufferingQueryIter;

		BufferingQueryVector	m_BufferingQueryVector;

	public:
		DBWorker();
		virtual ~DBWorker();

		void	Stop();

		// 2008.01.06. steeple
		void	SetBufferingCommit(BOOL bValue) { m_bBufferingCommit = bValue; }
		BOOL	GetBufferingCommit() { return m_bBufferingCommit; }
		void	SetMaxBufferingCount(INT32 lValue) { m_lMaxBufferingCount = lValue; }

		void	InitializeBufferingLock() { ::InitializeCriticalSection(&m_csBufferingLock); }
		void	DeleteBufferingLock() { ::DeleteCriticalSection(&m_csBufferingLock); }
		void	BufferingLock() { ::EnterCriticalSection(&m_csBufferingLock); }
		void	BufferingUnLock() { ::LeaveCriticalSection(&m_csBufferingLock); }

		BOOL	ProcessBufferingQuery(AgsdQuery* pcsQuery, BOOL bFlush = FALSE);
		BOOL	FlushBufferingCommit();

	protected:
		// ... Working Method(zzThread inherited)
		DWORD	Do();
		BOOL	OnCreate();
		void	OnTerminate();

		BOOL	Execute(AgsdQuery* pQuery);
		BOOL	ExecuteQuery(AgsdQuery* pQuery);
		BOOL	Connect();
		void	Disconnect();
	};

/****************************************************/
/*		The Definition of IOCPDatabase class		*/
/****************************************************/
//
class IOCPDatabase : public AuDatabaseManager
	{
	private:
		zzIOCP		m_IOCP;
		DBWorker*	m_pWorkers;
		INT16		m_nWorkers;

	public:
		INT8		m_cType;
		CHAR		m_szDSN[_MAX_CONN_PARAM_STRING_LENGTH];
		CHAR		m_szUser[_MAX_CONN_PARAM_STRING_LENGTH];
		CHAR		m_szPwd[_MAX_CONN_PARAM_STRING_LENGTH];
		eAUDB_API	m_eApi;
		BOOL		m_bIgnoreFail;

		BOOL		m_bBufferingCommit;

	public:
		IOCPDatabase();
		virtual ~IOCPDatabase();

		BOOL	Create(INT16 nWorkers);
		void	Destroy();

		BOOL	PostQuery(AgsdQuery *pQuery);
		BOOL	PostStatus(INT32 lStatus);

		BOOL	FlushAll();

		//	Status
		INT32	GetPosted();
		INT32	GetExecuted();
		INT32	GetFailed();
	};

#endif
