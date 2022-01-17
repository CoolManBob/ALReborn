#pragma once
#include "sdkconfig.h"
#include "ClientAgent.h"
#include "CacheLib/CacheObject.h"
#include "AuthenComand.h"
#include "ConfigInterface.h"

class CCacheInfo
{
public:
	vector<string> m_Info;
	const char * operator[](int);
	void ToString(string & str);
	void FromString(string str);
};

class CAuthProcess: public CClientAgent
{
public:
	CAuthProcess();
	~CAuthProcess();

	enum ProcessResult{processed = 0, unprocessed};	

	virtual void OperateResult	( CSessionPtr &p_session, int operation, int result);
	
	int Process(CAuthenCommand & command);

	void Init( CConfigInterface & config);

protected:
	bool m_bFrequencyCtrl, m_bAuthCache;
	int m_Frequency;
	int m_CacheTime;
	int m_CacheSize;

	CCacheObject m_AuthCache;
	CCacheObject m_FrequencyCache;
	CCacheObject m_EkeyErrorLock;
};