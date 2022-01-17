#include "AuFileFind.h"

// Global function(s)
BOOL DoesFileExist(char* fileName)
{
	AuFileFind fileFind;
	return fileFind.FindFile(fileName);
}

//
// Implementation of class AuFileFind
//
AuFileFind::AuFileFind()
{
}

AuFileFind::~AuFileFind()
{
	Close();
}

VOID AuFileFind::Initialize()
{
	m_bSkipDirectory	= FALSE;
	m_hHandle			= NULL;

	memset(&m_stFindData, 0, sizeof (WIN32_FIND_DATA));
}

BOOL AuFileFind::FindFile(CHAR *pszName, BOOL bSkipDirectory)
{
	Initialize();

	m_bSkipDirectory	= bSkipDirectory;

	m_hHandle			= FindFirstFile(pszName, &m_stFindData);

	return (m_hHandle == INVALID_HANDLE_VALUE) ? (FALSE) : (TRUE);
}

BOOL AuFileFind::FindNextFile()
{
	if ((!m_hHandle) || (m_hHandle == INVALID_HANDLE_VALUE))
		return FALSE;

	if (!::FindNextFile(m_hHandle, &m_stFindData))
		return FALSE;

	if (m_bSkipDirectory)
	{
		while (IsDirectory())
		{
			if (!::FindNextFile(m_hHandle, &m_stFindData))
				return FALSE;
		}
	}

	return TRUE;
}

void AuFileFind::Close()
{
	FindClose(m_hHandle);
	m_hHandle = INVALID_HANDLE_VALUE;
}

BOOL AuFileFind::IsNormal()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsArchive()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsCompressed()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsDirectory()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsEncrypted()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsHidden()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsOffLine()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsReadOnly()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsReparsePoint()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsSparseFile()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsSystem()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::IsTemporary()
{
	return (m_stFindData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) ? (TRUE) : (FALSE);
}

BOOL AuFileFind::GetCreationTime(LPSYSTEMTIME pTime)
{
	return FileTimeToSystemTime(&m_stFindData.ftCreationTime, pTime);
}

BOOL AuFileFind::GetLastAccessTime(LPSYSTEMTIME pTime)
{
	return FileTimeToSystemTime(&m_stFindData.ftLastAccessTime, pTime);
}

BOOL AuFileFind::GetLastWriteTime(LPSYSTEMTIME pTime)
{
	return FileTimeToSystemTime(&m_stFindData.ftLastWriteTime, pTime);
}

VOID AuFileFind::GetFileName(CHAR *pszFileName)
{
	strcpy(pszFileName, m_stFindData.cFileName);
}