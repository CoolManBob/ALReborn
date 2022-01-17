// FILE		: XmlLogger.cpp
// AUTHOR	: Christian Richardt (cr@whizer.net)
// MODIFIER	: burumal
// DATE		: 2006/08/03

#include "../StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>

#include "XmlLogger.h"

CXmlLogger::CXmlLogger(char* szFileName, char* szAppName)
{
	m_szFileName = new char[ strlen(szFileName) + 1];
	strcpy(m_szFileName, szFileName);

	// Try to open log file
	try
	{
		m_pFile = fopen(szFileName, "w");
	}
	catch (...)
	{
		char buf[256];
		sprintf(buf, "CXmlLogger: Error opening file \"%s\".\n", szFileName);
		perror(buf);
		return;
	}

	InitializeCriticalSection(&m_csSyncObj);

	MakeStyleSheet();

	m_nDepth = 0; // root

	// Header
	WriteData("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n");
	WriteData("<?xml-stylesheet href=\"logss.xsl\" type=\"text/xsl\" ?>");
	WriteLine("<LogFile>");

	m_nDepth++; // one level down
	WriteLine("AppName", szAppName);

	Log("Start");

	WriteTimestamp();
	WriteDate();
	WriteTime();
}


CXmlLogger::~CXmlLogger()
{
	int nCount = m_nDepth;
	while ( nCount > 1 )
	{ 
		GoUp();
	}

	//Log("Log File closed", GetFileName(__FILE__));
	Log("End.");

	Lock();
	m_nDepth--; // up
	UnLock();

	WriteLine("</LogFile>"); // Last Line

	Lock();

	// Try to close log file
	try
	{ 
		fclose(m_pFile);
	}
	catch (...)
	{
		char buf[256];
		sprintf(buf, "CXmlLogger: Error closing file \"%s\".\n", m_szFileName);
		perror(buf);
	}

	if ( m_szFileName )
	{
		delete[] m_szFileName;
		m_szFileName = NULL;
	}

	UnLock();

	DeleteCriticalSection(&m_csSyncObj);
}

bool CXmlLogger::GoDown(char* szMsg, char* szFileName, char* szFuncName)
{
	bool bResult = true;

	bResult &= WriteLine("<LogItem>");

	Lock();
	m_nDepth++;
	UnLock();

	bResult &= WriteTime();
	
	if ( szFileName != 0 ) { bResult &= WriteLine("FileName", szFileName); }
	if ( szFuncName != 0 ) { bResult &= WriteLine("Location", szFuncName); }
	if ( szMsg != 0 ) { bResult &= WriteLine("Message", szMsg); }

	return bResult;
}

void CXmlLogger::GoUp()
{
	if ( m_nDepth > 1 )
	{
		Lock();
		m_nDepth--;
		UnLock();

		WriteLine("</LogItem>");
	}
}

bool CXmlLogger::Log(char* szMsg, char* szFileName, char* szFuncName)
{
	bool bResult = true;
	
	GoDown();

	if ( szFileName != 0 ) { bResult &= WriteLine("FileName", szFileName); }
	if ( szFuncName != 0 ) { bResult &= WriteLine("Location", szFuncName); }
	bResult &= WriteLine("Message", szMsg);

	GoUp();

	return bResult;
}

bool CXmlLogger::WriteDate()
{
	char buf[256];	

	time_t ltime; time( &ltime );
	strftime( buf, 255, "<Date>%Y,%m,%d</Date>", localtime(&ltime));
	bool bResult = WriteLine(buf); // write

	return bResult;
}

bool CXmlLogger::WriteTime()
{	
	char buf1[256]; // for strftime
	char buf2[256]; // result

	time_t ltime; time( &ltime );

	sprintf(buf1, "<Time>%%H:%%M:%%S</Time>");
	strftime( buf2, 255, buf1, localtime(&ltime));
	bool bResult = WriteLine(buf2);

	return true;
}

bool CXmlLogger::WriteTimestamp()
{
	char buf[256];
	sprintf(buf, "<TimeStamp>%.3f</TimeStamp>", mtime());
	return WriteLine(buf);
}

bool CXmlLogger::WriteLine(char* pData)
{
	bool bResult = true;

	bResult &= WriteData("\n");

	int nCount = m_nDepth;
	for(int i = 0; i < nCount; i++ )
	{ 
		bResult &= WriteData("  ");
	}

	bResult &= WriteData(pData);

	return bResult;
}

bool CXmlLogger::WriteLine(char* pTagName, char* pContents)
{
	char* buf = new char[ 2 * strlen(pTagName) + strlen(pContents) + 10 ];
	sprintf(buf, "<%s>%s</%s>", pTagName, pContents, pTagName);

	bool bResult = WriteLine(buf);
	delete[] buf;

	return bResult;
}

bool CXmlLogger::WriteData(char* pData)
{
	try
	{ 
		Lock();
		fwrite(pData, strlen(pData), 1, m_pFile);
		UnLock();
	}
	catch (...)
	{		
		char buf[256];
		sprintf("CXmlLogger: Could not write to file \"%s\".\n", m_szFileName);
		perror(buf);
		return false; // Error
	}

	return true; // Successful
}

char* CXmlLogger::GetFileName(char* szFile)
{
	char* pName = strrchr(szFile, '\\');
	if ( pName ) { return (pName+1); }
	
	return "Error";
}

double CXmlLogger::mtime()
{
	_timeb ts;
	_ftime( &ts );

	return (int) ts.time + (ts.millitm / 1000.0);
}


bool CXmlLogger::MakeStyleSheet()
{	
	char pXslFileName[MAX_PATH];
	ZeroMemory(pXslFileName, sizeof(pXslFileName));

	if ( m_szFileName )
	{
		strcpy(pXslFileName, m_szFileName);

		char* pDestMark1 = strrchr(pXslFileName, '\\');
		char* pDestMark2 = strrchr(pXslFileName, '/');
		
		if ( pDestMark1 == NULL && pDestMark2 == NULL )
		{
			strcpy(pXslFileName, "logss.xsl");
		}
		else
		if ( pDestMark1 != NULL && pDestMark2 != NULL )
		{
			if ( pDestMark1 > pDestMark2 )
				strcpy(pDestMark1 + 1, "logss.xsl");
			else
				strcpy(pDestMark2 + 1, "logss.xsl");
		}
		else
		{
			if ( pDestMark1 )
				strcpy(pDestMark1 + 1, "logss.xsl");
			else
				strcpy(pDestMark2 + 1, "logss.xsl");
		}
	}
	else
	{
		strcpy(pXslFileName, "logss.xsl");
	}

	FILE* pSsFile = fopen(pXslFileName, "wb");
	if ( pSsFile == NULL )
		return false;

	char* pStyleSheetString = new char[1024*32];
	if ( pStyleSheetString == NULL )
		return false;
	ZeroMemory(pStyleSheetString, sizeof(pStyleSheetString));

	strcat(pStyleSheetString, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\r\n");
	strcat(pStyleSheetString, "<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\r\n");
	strcat(pStyleSheetString, "<xsl:template match=\"/\">\r\n");
	strcat(pStyleSheetString, "\r\n");
	strcat(pStyleSheetString, "<html>\r\n");
	strcat(pStyleSheetString, "<head>\r\n");
	strcat(pStyleSheetString, "<title>LogXML - Log File for <xsl:value-of select=\"LogFile/AppName\" /></title>\r\n");
	strcat(pStyleSheetString, "</head>\r\n");
	strcat(pStyleSheetString, "<body>\r\n");
	strcat(pStyleSheetString, "<h2>Log File for <xsl:value-of select=\"LogFile/AppName\" /></h2>\r\n");
	strcat(pStyleSheetString, "\r\n");
	strcat(pStyleSheetString, "<table border=\"1\" cellspacing=\"1\" cellpadding=\"0\">\r\n");
	strcat(pStyleSheetString, "<tr>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#F0F0F0\">\r\n");
	strcat(pStyleSheetString, "<table border=\"0\" cellpadding=\"3\" cellspacing=\"1\">\r\n");
	strcat(pStyleSheetString, "<tr>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#b0c4de\"> </td>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#b0c4de\" align=\"center\" width=\"100\"><b>Time</b></td>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#b0c4de\" align=\"center\" width=\"250\"><b>Message</b></td>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#b0c4de\" align=\"center\" width=\"450\"><b>Location</b></td>\r\n");
	strcat(pStyleSheetString, "</tr>\r\n");
	strcat(pStyleSheetString, "\r\n");
	strcat(pStyleSheetString, "<xsl:for-each select=\"LogFile/LogItem\">\r\n");
	strcat(pStyleSheetString, "<tr onMouseover=\"\">\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#FFFFFF\" valign=\"top\"><xsl:number count=\"LogItem\" />.</td>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#E0E0E0\" valign=\"top\"><nobr><xsl:value-of select=\"Time\" /></nobr></td>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#F0F0F0\" valign=\"top\"><xsl:value-of select=\"Message\" /></td>\r\n");
	strcat(pStyleSheetString, "<td bgcolor=\"#E0E0E0\" valign=\"top\"><nobr><xsl:value-of select=\"FileName\" /><br /><xsl:value-of select=\"Location\" /></nobr></td>\r\n");
	strcat(pStyleSheetString, "</tr>\r\n");
	strcat(pStyleSheetString, "</xsl:for-each>\r\n");
	strcat(pStyleSheetString, "</table>\r\n");
	strcat(pStyleSheetString, "</td>\r\n");
	strcat(pStyleSheetString, "</tr>\r\n");
	strcat(pStyleSheetString, "</table>\r\n");
	strcat(pStyleSheetString, "\r\n");
	strcat(pStyleSheetString, "</body>\r\n");
	strcat(pStyleSheetString, "</html>\r\n");
	strcat(pStyleSheetString, "\r\n");
	strcat(pStyleSheetString, "</xsl:template>\r\n");
	strcat(pStyleSheetString, "</xsl:stylesheet>\r\n");
	
	fwrite(pStyleSheetString, strlen(pStyleSheetString), 1, pSsFile);
	fclose(pSsFile);

	delete [] pStyleSheetString;

	return true;
}

void CXmlLogger::Lock()
{
	EnterCriticalSection(&m_csSyncObj);
}

void CXmlLogger::UnLock()
{
	LeaveCriticalSection(&m_csSyncObj);
}

void CXmlLogger::Flush()
{
	Lock();

	if ( m_pFile )
		fflush(m_pFile);

	UnLock();
}