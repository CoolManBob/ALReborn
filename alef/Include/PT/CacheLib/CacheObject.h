#pragma once
#include "basecache.h"

class CCacheObject :
	public CBaseCache
{
public:
	CCacheObject(void);
	~CCacheObject(void);

	int Add(int v_policyGroup, string v_userkey, string v_value);
	int Remove(string v_userkey, string& v_value);
	int Remove(string v_userkey);

	int FindValue(string v_userkey, string& v_value);


	int Update(string v_userkey, string v_value);
};
