#pragma once
#include "sdkconfig.h"
#include "Lock\lock.h"
#include <vector>
using namespace CleverLock;
using namespace std;

class CInfoViewer;

#define ANALYZE_FORMAT(buf, format) \
{	\
	va_list log;	\
	va_start(log, ##format);	\
	_vsnprintf(buf+(int)strlen(buf), 1000, ##format, log);	\
	va_end(log);	\
	\
	int len = (int)strlen(buf);	\
	if (len > 0 && buf[len-1] != '\n')	\
	strcat(buf, "\n");	\
}


class PTSDK_STUFF CInfoNode: public CWithLock
{
public:
	CInfoNode();
	~CInfoNode();
	
	void SetViewer(CInfoViewer * p_viewer);
	void RemoveViewer(CInfoViewer * p_viewer, bool b_backCall=true);
	void SelfOut();

	void PostInfo(int intInfo, const char * strInfo);
	void PostInfo(const char* v_format, ...);

	void DebugError(const char* v_format, ...);
	void DebugProc(const char* v_format, ...);
	void DebugSession(const char* v_format, ...);

protected:
	vector<CInfoViewer *> m_List;

private:
	static int m_tick;
	static int m_lastNow;
};

class PTSDK_STUFF CInfoViewer: public CWithLock
{
public:
	CInfoViewer();
	~CInfoViewer();
	void RegistSource(CInfoNode * p_source);
	void UnRegistSource(CInfoNode * p_source, bool b_backCall=true);
	void SelfOut();
	virtual void GotInfo(int intInfo, const char * strInfo);
protected:
	vector<CInfoNode *> m_List;
private:
};

namespace InfoType
{
//enum { work, result, state};
enum {LOGTYPE_WORK=0, LOGTYPE_ERROR/*1*/, LOGTYPE_PROC/*2*/, LOGTYPE_OTHER/*3*/, LOGTYPE_SESSION/*4*/, LOGTYPE_LAST};
};