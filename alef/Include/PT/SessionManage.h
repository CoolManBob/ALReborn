#pragma once
#include "sdkconfig.h"
#include "SessionInfo.h"
#include "ThreadMaster.h"
#include <deque>
#include "Lock\Lock.h"

using namespace CleverLock;

class PTSDK_STUFF CSessionOwner
{
public:
	virtual void SessionTimeOut(CSessionPtr &session) =0;
};

class PTSDK_STUFF CSessionManage: public CWithLock  
{
public:
	CSessionManage(void);
	~CSessionManage(void);

	unsigned int PutSession(CSessionPtr& session);
	void RemoveSession(unsigned int key);
	void RemoveSession(unsigned int key, CSessionPtr& ptr);
	void GetSession(unsigned int key, CSessionPtr& ptr);

	void StartGhost(int life = 0);
	void StopGhost();

	static void  TakeLife( void * lpParameter );

	void Clear();
	//unsigned int GetNextSid() { return GetKey(); };

    CSessionOwner * m_pSessionOwn;
protected:
	typedef map<unsigned int, CSessionPtr * > TSessionType;
	TSessionType m_SessionMap;
	
	typedef pair<unsigned long,unsigned int>	TDeadRecord;
	typedef deque<TDeadRecord>				TDeadBook;
	TDeadBook	m_LifeBook;	
	
	int m_SessionLife;

	CThreadMaster m_GhostJob;

private:	
	unsigned int m_Key;

	//unsigned int GetKey();

};

