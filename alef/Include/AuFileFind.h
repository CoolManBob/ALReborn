#ifndef __AU_FILE_FIND_H__
#define __AU_FILE_FIND_H__

// 2006.02.16. steeple
// ignore deprecated warning message in VS2005
#if _MSC_VER >= 1400
#pragma warning (disable : 4996)
#endif

#include <windows.h>

/////////////////////////////////////////////////////
//	sample
/////////////////////////////////////////////////////
//
//	AuFileFind	csFinder;
//	CHAR		szFileName[256];

//	memset(szFileName, 0, sizeof (CHAR) * 256);

//	if (!csFinder.FindFile("D:/MODELTOOL/BIN/*"))
//		return FALSE;

//	for (; csFinder.FindFile();)
//	{
//		csFinder.GetFileName(szFileName);

//		무언가를 하시오!
//	}
//
/////////////////////////////////////////////////////

class AuFileFind
{
protected:
	BOOL			m_bSkipDirectory;
	HANDLE			m_hHandle;
	WIN32_FIND_DATA	m_stFindData;

public:
	AuFileFind();
	virtual ~AuFileFind();

	BOOL	FindFile(CHAR *pszName, BOOL bSkipDirectory = TRUE);
	BOOL	FindNextFile();

	BOOL	IsNormal();
	BOOL	IsArchive();
	BOOL	IsCompressed();
	BOOL	IsDirectory();
	BOOL	IsEncrypted();
	BOOL	IsHidden();
	BOOL	IsOffLine();
	BOOL	IsReadOnly();
	BOOL	IsReparsePoint();
	BOOL	IsSparseFile();
	BOOL	IsSystem();
	BOOL	IsTemporary();

	BOOL	GetCreationTime(LPSYSTEMTIME pTime);
	BOOL	GetLastAccessTime(LPSYSTEMTIME pTime);
	BOOL	GetLastWriteTime(LPSYSTEMTIME pTime);
	VOID	GetFileName(CHAR *pszFileName);

private:
	void	Close();

protected:
	VOID	Initialize();

};

BOOL DoesFileExist(char* fileName);

#endif // __AU_FILE_FIND_H__