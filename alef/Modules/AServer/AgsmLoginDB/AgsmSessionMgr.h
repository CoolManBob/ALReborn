#ifndef __AGSM_SESSIONMGR_H__
#define __AGSM_SESSIONMGR_H__

// **************************************************************************

#include <string>
#include <map>
#include <windows.h>

#include "PT/SDKStuff.h"

// forward declaration
using std::string;
using std::map;

// class Lock
class Lock
{
public:
	Lock(CRITICAL_SECTION& lock) : m_lock(lock) { EnterCriticalSection(&m_lock); }
	~Lock() { LeaveCriticalSection(&m_lock); }

private:
	CRITICAL_SECTION& m_lock;
};

//
class AgsmLoginQueueInfo;

// class AgsmSessionMgr
class AgsmSessionMgr
{
public:
	AgsmSessionMgr();
	~AgsmSessionMgr();

	void NewSessionToQi(unsigned session, AgsmLoginQueueInfo* lqi);
	void NewNidToSession(unsigned nid, CSessionPtr* csp);
	void NewAccountToQi(char* account, AgsmLoginQueueInfo* lqi);

	CSessionPtr* GetSessionByNid(unsigned nid);
	AgsmLoginQueueInfo* GetQiBySession(unsigned sid);
	AgsmLoginQueueInfo* GetQiByAccount(const char* account);

	void RemoveSession(unsigned sid);
	void RemoveNid(unsigned nid);
	void RemoveAccount(char* account);

private:
	CRITICAL_SECTION m_csSTQ;
	CRITICAL_SECTION m_csNTS;
	CRITICAL_SECTION m_csATQ;

	typedef std::map<unsigned, AgsmLoginQueueInfo*> SessionToQi;
	typedef std::map<unsigned, CSessionPtr*> NidToSession;
	typedef std::map<string, AgsmLoginQueueInfo*> AccountToQi;

	SessionToQi	 m_stq;
	NidToSession m_nts;
	AccountToQi	 m_atq;
};

#endif
