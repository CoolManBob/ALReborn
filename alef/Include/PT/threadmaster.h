#pragma once
#include "sdkconfig.h"
#include "stdafx.h"

#ifndef PTSDK_STUFF
#define PTSDK_STUFF
#endif

class PTSDK_STUFF CThreadMaster
{
public:
	CThreadMaster(void);
	~CThreadMaster(void);	

	typedef void (*WorkFuncType)(void * param);
	enum CycleTimeEnum{ AlwaysWait = -1, AlwaysWork = 0};

	void Start();
	void Stop();
	void WorkNow();

	void Set(WorkFuncType function, void * parameter, int cycleTime);
	void SetWorkFunction( WorkFuncType function );
	void SetWorkParameter( void * parameter);
	void SetCycleTime( int cycleTime );

	unsigned long GetThreadId();

	static unsigned __stdcall WorkThread( void * lpParameter );

protected:
	enum ThreadCmd{ nothing = 0, stop};

	ThreadCmd	m_ThreadCmd;	

	HANDLE	m_hThread;
	HANDLE	m_hEvent;

	unsigned int	m_ThreadId;

	int		m_CycleTime;
	void *	m_lpParameter;
	WorkFuncType	m_WorkFunction;		
};

class PTSDK_STUFF CMutiThreadMaster
{
public:
	CMutiThreadMaster(int count = 1);
	~CMutiThreadMaster();

	void Set(CThreadMaster::WorkFuncType function, void * parameter, int cycleTime);
	void SetWorkFunction( CThreadMaster::WorkFuncType function );
	void SetWorkParameter( void * parameter);
	void SetWorkParameter( void ** parameters, int count);
	void SetCycleTime( int cycleTime );

	void Start(int count = 1);
	void Stop();
	void WorkNow(int jobCount=1);
	void WorkIndex(int v_job = 0);

	void CreateThread(int count);
protected:
	int m_ThreadCount;
	CThreadMaster ** m_pThreads;
	void Clear();	

	int		m_CycleTime;
	void *	m_lpParameter;
	CThreadMaster::WorkFuncType	m_WorkFunction;		
private:
	int m_ThreadIndex;
};