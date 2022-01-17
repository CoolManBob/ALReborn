// FILE		: XmlLogger.h
// AUTHOR	: Christian Richardt (cr@whizer.net)
// MODIFIER	: burumal
// DATE		: 2006/08/03

#ifndef __XML_LOGGER_H__
#define __XML_LOGGER_H__

#include <stdio.h>

class CXmlLogger
{
public :
	CXmlLogger(char* szFileName, char* szAppName = "NONE");
	~CXmlLogger();

	bool	GoDown(char* szMsg=0, char* szFileName=0, char* szFuncName=0);
	void	GoUp(void);

	bool	Log(char* szMsg, char* szFileName=0, char* szFuncName=0);

	bool	WriteLine(char* pData);
	bool	WriteLine(char* pTagName, char* pContents);
	bool	WriteTimestamp();
	bool	WriteTime();
	bool	WriteDate();

	char*	GetFileName(char* szFile);

	void	Flush();


private :

	bool	WriteData(char* pData);

	bool	MakeStyleSheet();

	void	Lock();
	void	UnLock();

  /// UNIX timestamp: seconds from 1970-01-01 00:00:00 (UTC)
	double	mtime(void);

	char* m_szFileName;		// log file name	
	FILE* m_pFile;			// file handle of log file
	int   m_nDepth;			// current depth in document

	CRITICAL_SECTION m_csSyncObj;
};

#endif // __XML_LOGGER_H__