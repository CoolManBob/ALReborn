#pragma once
#include "stdafx.h"
#include "ConfigInterface.h"

class CIniConfig:public CConfigInterface
{
public:
	CIniConfig(const char * iniFileName);
	char m_iniFile[MAX_PATH];

	virtual int GetConfigInt(
		const char * lpAppName,
		const char * lpKeyName,
		int nDefault );

	virtual int GetConfigString(
		const char * lpAppName,
		const char * lpKeyName,
		const char * lpDefault,
		char *		 lpReturnedString,
		int			 nSize );
};