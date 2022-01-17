#include "AuTimeStamp.h"
#include <sys/timeb.h>

UINT32 AuTimeStamp::GetCurrentTimeStamp()
{
	time_t timeval;
	time(&timeval);

	return (UINT32) timeval;
}

UINT32 AuTimeStamp::GetCurrentTimeStampString(TCHAR *szTimeBuf, UINT32 ulLen)
{
	if(!szTimeBuf || ulLen < AUTIMESTAMP_SIZE_TIME_STRING + 1)
		return 0;

	time_t timeval;
	struct tm* tm_ptr;

	time(&timeval);
	tm_ptr = localtime(&timeval);

	_tcsftime(szTimeBuf, ulLen, AUTIMESTAMP_TIME_FORMAT, tm_ptr);

	return (UINT32)_tcslen(szTimeBuf);
}

UINT32 AuTimeStamp::AddTime(UINT32 ulTimeStamp, UINT32 ulDay, UINT32 ulHour, UINT32 ulMin, UINT32 ulSec)
{
	return (ulTimeStamp + ulDay*AUTIMESTAMP_DAY + ulHour*AUTIMESTAMP_HOUR + ulMin*AUTIMESTAMP_MINUTE + ulSec);
}

UINT32 AuTimeStamp::ConvertTimeStampToOracleTime(UINT32 ulTimeStamp, TCHAR *szOracleTime, UINT32 ulLen)
{
	return FormatTimeString(ulTimeStamp, AUTIMESTAMP_ORACLE_TIME_FORMAT, szOracleTime, ulLen);
}

UINT32 AuTimeStamp::FormatTimeString(UINT32 ulTimeStamp, const TCHAR *szFormat, TCHAR *szTimeString, UINT32 ulLen)
{
	if(!szTimeString || ulLen < AUTIMESTAMP_SIZE_TIME_STRING + 1)
	{
		return 0;
	}

	// 0인 경우는 무시함.
	if (ulTimeStamp == 0)
	{
		_tcscpy(szTimeString, _T("1900-01-01 00:00:00"));
		return (UINT32)_tcslen(szTimeString);
	}

	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)ulTimeStamp;
	tm_ptr = localtime(&timeval);

	if(!tm_ptr)
		return 0;

	_tcsftime(szTimeString, ulLen, szFormat, tm_ptr);

	return (UINT32)_tcslen(szTimeString);
}

UINT32 AuTimeStamp::ConvertOracleTimeToTimeStamp(TCHAR *szOracleTime)
{
	if(!szOracleTime)
		return 0;

	// 졸래 하드코딩 해준다.
	// 포맷은 ####/##/## ##:##:## 이거다. 고로 삑살나면 return 0;
	if(strlen(szOracleTime) != AUTIMESTAMP_SIZE_ORACLE_TIME_STRING)
		return 0;

	TCHAR szTmp[32];
	memset(szTmp, 0, sizeof(szTmp));

	struct tm timeptr;
	memset(&timeptr, 0, sizeof(timeptr));

	memcpy(szTmp, szOracleTime, 4);
	szTmp[4] = '\0';
	timeptr.tm_year = atoi(szTmp) - 1900;		// 1900 Base

	memcpy(szTmp, &szOracleTime[5], 2);
	szTmp[2] = '\0';
	timeptr.tm_mon = atoi(szTmp) - 1;			// January == 0 이므로 1 빼줌.

	memcpy(szTmp, &szOracleTime[8], 2);
	szTmp[2] = '\0';
	timeptr.tm_mday = atoi(szTmp);

	memcpy(szTmp, &szOracleTime[11], 2);
	szTmp[2] = '\0';
	timeptr.tm_hour = atoi(szTmp);

	memcpy(szTmp, &szOracleTime[14], 2);
	szTmp[2] = '\0';
	timeptr.tm_min = atoi(szTmp);

	memcpy(szTmp, &szOracleTime[17], 2);
	szTmp[2] = '\0';
	timeptr.tm_sec = atoi(szTmp);

	timeptr.tm_isdst = -1;

	UINT32 timeval = (UINT32)mktime(&timeptr);

	return (timeval != -1) ? timeval : 0;
}


// class AuTimeStamp2
///////////////////////////////////////////////////////////////////

UINT64 AuTimeStamp2::GetCurrentTimeStamp()
{
	SYSTEMTIME	system_time;
	FILETIME	file_time;

	::GetLocalTime(&system_time);
	if (::SystemTimeToFileTime(&system_time, &file_time) == 0)
		return 0;

	UINT64	ullCurrentTime;

	CopyMemory(&ullCurrentTime, &file_time, sizeof(ULARGE_INTEGER));

	return ullCurrentTime;
}

UINT64 AuTimeStamp2::ConvertTimeStampToOracleTime(UINT64 ullTimeStamp, TCHAR *szOracleTime, UINT32 ulLen)
{
	return FormatTimeString(ullTimeStamp, szOracleTime, ulLen);
}

UINT64 AuTimeStamp2::FormatTimeString(UINT64 ullTimeStamp, TCHAR *szTimeString, UINT32 ulLen)
{
	if(!szTimeString || ulLen < AUTIMESTAMP_SIZE_TIME_STRING + 1)
	{
		return 0;
	}

	// 0인 경우는 무시함.
	if (ullTimeStamp == 0)
	{
		_tcscpy(szTimeString, _T("1900-01-01 00:00:00"));
		return (UINT32)_tcslen(szTimeString);
	}

	FILETIME	file_time;
	SYSTEMTIME	system_time;

	CopyMemory(&file_time, &ullTimeStamp, sizeof(ULARGE_INTEGER));

	if (::FileTimeToSystemTime(&file_time, &system_time) == 0)
	{
		_tcscpy(szTimeString, _T("1900-01-01 00:00:00"));
		return (UINT32)_tcslen(szTimeString);
	}

	sprintf(szTimeString,
			"%04d-%02d-%02d %02d:%02d:%02d",
			system_time.wYear,
			system_time.wMonth,
			system_time.wDay,
			system_time.wHour,
			system_time.wMinute,
			system_time.wSecond);

	return (UINT32)_tcslen(szTimeString);
}

UINT64 AuTimeStamp2::ConvertOracleTimeToTimeStamp(TCHAR *szOracleTime)
{
	if(!szOracleTime)
		return 0;

	// 졸래 하드코딩 해준다.
	// 포맷은 ####/##/## ##:##:## 이거다. 고로 삑살나면 return 0;
	if(strlen(szOracleTime) != AUTIMESTAMP_SIZE_ORACLE_TIME_STRING)
		return 0;

	TCHAR szTmp[32];
	memset(szTmp, 0, sizeof(szTmp));

	SYSTEMTIME	system_time;
	ZeroMemory(&system_time, sizeof(system_time));

	memcpy(szTmp, szOracleTime, 4);
	szTmp[4] = '\0';
	system_time.wYear = atoi(szTmp);		// 1900 Base

	memcpy(szTmp, &szOracleTime[5], 2);
	szTmp[2] = '\0';
	system_time.wMonth = atoi(szTmp);			// January == 0 이므로 1 빼줌.

	memcpy(szTmp, &szOracleTime[8], 2);
	szTmp[2] = '\0';
	system_time.wDay = atoi(szTmp);

	memcpy(szTmp, &szOracleTime[11], 2);
	szTmp[2] = '\0';
	system_time.wHour = atoi(szTmp);

	memcpy(szTmp, &szOracleTime[14], 2);
	szTmp[2] = '\0';
	system_time.wMinute = atoi(szTmp);

	memcpy(szTmp, &szOracleTime[17], 2);
	szTmp[2] = '\0';
	system_time.wSecond = atoi(szTmp);

	FILETIME	file_time;

	if (::SystemTimeToFileTime(&system_time, &file_time) == 0)
		return 0;

	UINT64	ullCurrentTime	= 0;

	CopyMemory(&ullCurrentTime, &file_time, sizeof(ULARGE_INTEGER));

	return ullCurrentTime;
}
