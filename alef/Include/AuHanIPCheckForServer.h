// AuHanIPCheckForServer.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2007.11.05.

#ifndef _AU_HANIPCHECK_FOR_SERVER_
#define _AU_HANIPCHECK_FOR_SERVER_

#include <windows.h>
#include <string>

class AuHanIPCheckForServer
{
private:
	BOOL m_bInit;

public:
	AuHanIPCheckForServer();
	virtual ~AuHanIPCheckForServer();

	int AuHanIPCheckInit(BOOL bTestServer = FALSE);
	int AuHanIPCheckRequest(const char* szIP, char* szCRMCode = NULL, char* szGrade = NULL);

	int AuHanIPCheckGetLastError();

	std::string GetGameString(BOOL bTestServer = FALSE);
};

#endif
