#ifndef _AU_TIME_STAMP_H_
#define _AU_TIME_STAMP_H_

#include "time.h"
#include "ApDefine.h"
#include "tchar.h"
#include <atltime.h>
#include <iostream>
static int AUTIMESTAMP_SECOND	= 1;
static int AUTIMESTAMP_MINUTE	= 60;
static int AUTIMESTAMP_HOUR		= 3600;
static int AUTIMESTAMP_DAY		= 86400;

#define AUTIMESTAMP_TIME_FORMAT				_T("%Y%m%d%H%M")
#define AUTIMESTAMP_ORACLE_TIME_FORMAT		_T("%Y-%m-%d %H:%M:%S")
#define AUTIMESTAMP_SIZE_TIME_STRING		12		//_tcslen(AUTIMESTAMP_TIME_FORMAT)		// 197706202312
#define AUTIMESTAMP_SIZE_ORACLE_TIME_STRING	19		// _tcslen(AUTIMESTAMP_ORACLE_TIME_FORMAT)	// 1977/06/20 12:35:23

class AuTimeStamp
{
public:
	static UINT32 GetCurrentTimeStamp();
	static UINT32 GetCurrentTimeStampString(TCHAR *szTimeBuf, UINT32 ulLen);
	static UINT32 AddTime(UINT32 ulTimeStamp, UINT32 ulDay, UINT32 ulHour, UINT32 ulMin, UINT32 ulSec);

	static UINT32 ConvertTimeStampToOracleTime(UINT32 ulTimeStamp, TCHAR *szOracleTime, UINT32 ulLen);
	static UINT32 ConvertOracleTimeToTimeStamp(TCHAR *szOracleTime);
	static UINT32 FormatTimeString(UINT32 ulTimeStamp, const TCHAR *szFormat, TCHAR *szTimeString, UINT32 ulLen);


	static CTime ConvertOracleTimeToCTime(CHAR* szOracleTime)
	{
		if(!szOracleTime)
			return 0;

		struct tm s_tm;
		if(sscanf(szOracleTime, "%04d-%02d-%02d %02d:%02d:%02d", &s_tm.tm_year, &s_tm.tm_mon, &s_tm.tm_mday, &s_tm.tm_hour, &s_tm.tm_min, &s_tm.tm_sec) != 6 )
			return ((CTime)0);

		return CTime(s_tm.tm_year, s_tm.tm_mon, s_tm.tm_mday, s_tm.tm_hour, s_tm.tm_min, s_tm.tm_sec);
	};
};

class AuTimeStamp2
{
public:
	static UINT64 GetCurrentTimeStamp();

	static UINT64 ConvertTimeStampToOracleTime(UINT64 ulTimeStamp, TCHAR *szOracleTime, UINT32 ulLen);
	static UINT64 ConvertOracleTimeToTimeStamp(TCHAR *szOracleTime);
	static UINT64 FormatTimeString(UINT64 ulTimeStamp, TCHAR *szTimeString, UINT32 ulLen);
	

};

using namespace std;
inline void PrintTimeStamp(const char* szArg, UINT64 utime )
{
	SYSTEMTIME	tmp_time;
	FILETIME	file_time;
	CopyMemory(&file_time, &utime, sizeof(UINT64));
	::FileTimeToSystemTime(&file_time, &tmp_time);
	cout << "["<< szArg << "]"<< tmp_time.wYear << "." << tmp_time.wMonth << "." << tmp_time.wDay << "(" << tmp_time.wDayOfWeek << ")" << tmp_time.wHour << ":" << tmp_time.wMinute << ":" << tmp_time.wSecond <<endl;
}
#endif // _AU_TIME_STAMP_H_
