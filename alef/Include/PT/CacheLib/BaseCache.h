#pragma once

#include <string>
#include <map>
#include <vector>
using namespace std;

#include "CommonDefine.h"
#include "../Lock/Lock.h"
#include "../ThreadMaster.h"
using namespace CleverLock;

typedef struct CACHE_VALUE
{
	string	strValue;
	int		nAccess;
	int		nGroup;

	__int64 llTime;

	string	strKey;
}
TCacheValue, *pTCacheValue;

typedef struct POLICY_GROUP
{
	int	 nExpireTime;
	bool bRecordFindTime;
}
TPolicyGroup, *pTPolicyGroup;

typedef multimap<string, string>		USERMAP;
typedef map		<string, TCacheValue>	UIDMAP;
typedef multimap<int ,	 string>		GROUPMAP;
typedef multimap<__int64, string>		TIMEMAP;


typedef pair<string,	string> PAIR_STRING_STRING;
typedef pair<int,		string> PAIR_INT_STRING;
typedef pair<unsigned,	string> PAIR_ULONG_STRING;
typedef pair<__int64, string> PAIR_LONGLONG_STRING;

class CBaseCache
{
public:
	CBaseCache(void);
	~CBaseCache(void);

	void SetParameters(int v_checkInterval, int v_mapCapacity, int v_escapePercent);
	void Clear();
	int  GetSize();

	int SetPolicy(int& v_policyGroup, enum POLICYNAME v_policyName, string v_propertyValue);
	int GetPolicy(int v_policyGroup,  enum POLICYNAME v_policyName, string& v_propertyValue);
	int GetPolicyGroupSize();

protected:
	int Add(int v_policyGroup, string v_userkey, string v_value, string& v_uid, bool v_multi);
	int Remove(string v_uid, string& v_value);
	int Find(string v_uid, TCacheValue& v_cacheValue);
	int Update(string v_uid, string v_value);

	int SearchUidByUserkey(string v_userkey, string& v_uid, int v_index = 0);

private:
	USERMAP		m_mapUser;
	UIDMAP		m_mapUid;
	TIMEMAP		m_mapTime;
	GROUPMAP	m_mapGroup;

	union DOUBLE_TICK
	{
		__int64 llTick;
		struct DWORD_TICK
		{
			DWORD dwLow;
			DWORD dwHigh;
		}dwTick;
	}m_LongLongTick;

	__int64 GetMyTickCount();

	vector<TPolicyGroup> m_vecPolicyGroup;

	CWithLock	m_lock;

	int		m_nCacheCap;
	int		m_nEscapeNumber;
	bool	m_bFull;

	int CreateGuid(char* v_guid);

	CThreadMaster	m_CheckThread;
	static void		CheckThread(LPVOID lpParameter);

    int DeleteExpireRecords();
	int DeleteEarlyRecords();
	int DeleteElementsByUserkey(string v_userkey);

	USERMAP::iterator	FindUserMap( string v_key, string v_uid);
	TIMEMAP::iterator	FindTimeMap( __int64 v_llTime, string v_uid);
	GROUPMAP::iterator	FindGroupMap(int v_group, string v_uid);
};
