// RKEntryManager.cpp
// 2003.07.30 steeple

#include "stdafx.h"
#include "RKInclude.h"
//#include "imagehlp.h"
// imagehlp.h -> dbghelp.h
#include "dbghelp.h"

BOOL MyCopyFile(LPCTSTR szLocal, LPCTSTR szTarget, BOOL bOverwrite);

CRKEntryManager::CRKEntryManager()
{
	m_szRemoteDrive = "";

	Init();
}

CRKEntryManager::~CRKEntryManager()
{
}

void CRKEntryManager::Init()
{
	ClearEntryInfo();
	ClearHistoryInfo();
}

void CRKEntryManager::ClearEntryInfo()
{
	m_stRKEntryInfo.szType = "";
	m_stRKEntryInfo.szFileName = "";
	m_stRKEntryInfo.szSize = "";
	m_stRKEntryInfo.szDate = "";
	m_stRKEntryInfo.szVer = "";
	m_stRKEntryInfo.szOwner = "";
}

void CRKEntryManager::ClearHistoryInfo()
{
	m_stRKHistoryInfo.szOwner = "";
	m_stRKHistoryInfo.szDate = "";
	m_stRKHistoryInfo.szVer = "";
}

void CRKEntryManager::SetEntryParserFileName(CString& szFileName)
{
	m_csRKEntryParser.SetFileName(szFileName);
}

void CRKEntryManager::SetHistoryParserFileName(CString& szFileName)
{
	m_csRKHistoryParser.SetFileName(szFileName);
}

void CRKEntryManager::SetEntryWriterFileName(CString& szFileName)
{
	m_csRKEntryWriter.SetFileName(szFileName);
}

void CRKEntryManager::SetHistoryWriterFileName(CString& szFileName)
{
	m_csRKHistoryWriter.SetFileName(szFileName);
}

void CRKEntryManager::GetFileNameByFilePath(CString& szFilePath, CString& szFileName)
{
	/* 2003.08.13 이거 때메 이상하게 되버려서 역시 주석처리.. -0-
	CFileFind csFileFind;
	if(csFileFind.FindFile((LPCTSTR)szFilePath))
	{
		csFileFind.FindNextFile();
		if(csFileFind.IsDirectory())	// 디렉토리라면 그냥 대입해주고 나간다.
		{
			szFileName = szFilePath;
			return;
		}
	}
	*/

	int iIndex = szFilePath.ReverseFind(TCHAR('\\'));
	if(iIndex >= 0 && iIndex < szFilePath.GetLength())
		szFileName = szFilePath.Mid(iIndex+1, szFilePath.GetLength() - iIndex);
	else
		szFileName = szFilePath;
}

BOOL CRKEntryManager::GetEntryFileInfo(CString& szFileName, stRKEntryInfo* pstRKEntryInfo)
{
	if(!m_csRKEntryParser.Open())
		return FALSE;

	BOOL bResult = FALSE;

	// 해당 파일의 엔트리 정보를 찾는다.
	if(m_csRKEntryParser.FindFileInfo(szFileName))
	{
		ClearEntryInfo();
		m_csRKEntryParser.GetEntryTokenStruct(m_stRKEntryInfo);

		bResult = TRUE;

		// 인자로 넘어온 게 있으면 값을 세팅해준다.
		if(pstRKEntryInfo)
		{
			pstRKEntryInfo->szType = m_stRKEntryInfo.szType;
			pstRKEntryInfo->szFileName = m_stRKEntryInfo.szFileName;
			pstRKEntryInfo->szSize = m_stRKEntryInfo.szSize;
			pstRKEntryInfo->szDate = m_stRKEntryInfo.szDate;
			pstRKEntryInfo->szVer = m_stRKEntryInfo.szVer;
			pstRKEntryInfo->szOwner = m_stRKEntryInfo.szOwner;
		}
	}

	m_csRKEntryParser.Close();

	return bResult;
}

// 이 함수를 호출하기 전에, m_csRKEntryWriter 의 SetFileName 을 해주어야 한다. 그래야지 Open 이 됨.
// 여기서 szFileName 은 szOnlyFileName 이다.
BOOL CRKEntryManager::AddEntry(CString& szType, CString& szFileName, INT32 lFileSize, CString& szDate, INT32 lVer)
{
	RemoveDirectoryFinalSlash(szFileName);

	if(!m_csRKEntryWriter.Open())
		return FALSE;

	BOOL bResult = m_csRKEntryWriter.WriteEntry(szType, szFileName, lFileSize, szDate, lVer);
	m_csRKEntryWriter.Close();

	return bResult;
}


// szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::AddEntry(CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CFileStatus csStatus;
	if(CFile::GetStatus((LPCTSTR)szFileName, csStatus))
	{
		CString szType;
		CFileFind csFileFind;
		if(csFileFind.FindFile((LPCTSTR)szFileName))
		{
			csFileFind.FindNextFile();
			szType = csFileFind.IsDirectory() ? "D" : "F";
		}
		else
			szType = "_";

		CString szDate;
		szDate.Format("%04d-%02d-%02d %02d:%02d:%02d", csStatus.m_mtime.GetYear(), csStatus.m_mtime.GetMonth(),
																		csStatus.m_mtime.GetDay(), csStatus.m_mtime.GetHour(),
																		csStatus.m_mtime.GetMinute(), csStatus.m_mtime.GetSecond());
		INT32 lSize = (INT32)csStatus.m_size;
		INT32 lVer = 1;		// 등록할 때는 버전 1 이다.

		if(m_csRKEntryWriter.Open())
		{
			bResult = m_csRKEntryWriter.WriteEntry(szType, szOnlyFileName, lSize, szDate, lVer);
			m_csRKEntryWriter.Close();
		}
	}

	return bResult;
}

// szLocalFileName, szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::CheckLatestVersion(CString& szLocalFileName, CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szLocalFileName);
	RemoveDirectoryFinalSlash(szFileName);

	CFileStatus csLocalStatus, csRemoteStatus;
	if(CFile::GetStatus((LPCTSTR)szLocalFileName, csLocalStatus) && CFile::GetStatus((LPCTSTR)szFileName, csRemoteStatus))
	{
		// 디렉토리라면 그냥 return TRUE 해준다. 2004.11.23. steeple
		if(csRemoteStatus.m_attribute & CFile::directory)
		{
			bResult = TRUE;
		}
		else
		{
			// 파일이라면 검사하자.
			if(csLocalStatus.m_size == csRemoteStatus.m_size)	// 사이즈 같고..
			{
				CTimeSpan csSpan = csRemoteStatus.m_mtime - csLocalStatus.m_mtime;
				if(csSpan.GetTotalSeconds() == 0L)	// 수정시간도 같다.
				{
					bResult = TRUE;
				}
			}
		}
	}
	
	// 2003.10.21 대망의 CheckLatestVersion 방식이 바뀜.
	// M$ 날짜 계산의 압박으로 인해서, 현호형이 가르쳐준 새로운 방법으로 거시기함.
	/*
	DWORD dwLocalHeadSum, dwOriginHeadSum;
	DWORD dwLocalCheckSum, dwOriginCheckSum;

	char szLocalFileNameTmp[512];
	char szOriginFileNameTmp[512];
	strcpy(szLocalFileNameTmp, (LPCTSTR)szLocalFileName);
	strcpy(szOriginFileNameTmp, (LPCTSTR)szFileName);

	// 파일들의 CheckSum 을 읽어온다.
	if(MapFileAndCheckSum(szLocalFileNameTmp, &dwLocalHeadSum, &dwLocalCheckSum) == CHECKSUM_SUCCESS &&
		MapFileAndCheckSum(szOriginFileNameTmp, &dwOriginHeadSum, &dwOriginCheckSum) == CHECKSUM_SUCCESS)
	{
		if(dwLocalCheckSum == dwOriginCheckSum)
			bResult = TRUE;
	}
	*/

	return bResult;
}

// szLocalFileName, szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::CheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	// 현재 최신 버전인지 확인한다.
	if(CheckLatestVersion(szLocalFileName, szFileName) == FALSE)
		return FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// 이전에 세팅된 값을 읽는다.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// 파일 이름이 맞고, 이전의 소유자가 없다면..
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
			m_stRKEntryInfo.szOwner.GetLength() == 0)
		{
			// 체크아웃 수행!!
			if(m_csRKEntryWriter.Open())
			{
				bResult = m_csRKEntryWriter.WriteOwner(szOnlyFileName, 0, CString(""), szOwner);	// Size 랑, 날짜는 여기에는 필요없으므로.
				m_csRKEntryWriter.Close();
			}

			if(bResult)	// CheckOut 성공했으면 LocalFile 의 ReadOnly 풀어준다. 2003.10.08 김태희
				SetReadOnly(szLocalFileName, FALSE);
		}
	}

	return bResult;
}

// szLocalFileName, szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::CheckIn(CString& szLocalFileName, CString& szFileName, CString& szOwner, BOOL bRealCopy)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// 이전에 세팅된 값을 읽는다.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// 파일 이름이 맞고, 이전의 소유자와 같다면!!!
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
//			m_stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) == 0)
			(m_stRKEntryInfo.szOwner.IsEmpty() || m_stRKEntryInfo.szOwner.CompareNoCase(LPCTSTR(szOwner)) == 0))	// Lock을 걸어놓은 사용자가 없어야 됨
		{
			INT32 lVer = atoi((LPCTSTR)m_stRKEntryInfo.szVer);

			CString szNewDate;
			INT32 lSize = 0;

			CFileStatus csLocalStatus;
			if(CFile::GetStatus((LPCTSTR)szLocalFileName, csLocalStatus))
			{
				szNewDate.Format("%04d-%02d-%02d %02d:%02d:%02d", csLocalStatus.m_mtime.GetYear(), csLocalStatus.m_mtime.GetMonth(),
																				csLocalStatus.m_mtime.GetDay(), csLocalStatus.m_mtime.GetHour(),
																				csLocalStatus.m_mtime.GetMinute(), csLocalStatus.m_mtime.GetSecond());
				lSize = (INT32)csLocalStatus.m_size;
			}
			else
			{
				// 파일 정보 못 얻으면 꼬인다.. -0-
				OutputDebugString("CRKEntryManager::CheckIn(...) => CFile::GetStatus((LPCTSTR)szLocalFileName, csLocalStatus) Failed!!!!\n");
				return FALSE;
			}

			// 파일을 서버에 올린다. (뒤에 '_' 를 붙여서 올린다.)
			CString szTmpFileName = szFileName + "_";
			if(bRealCopy)
				CopyFile((LPCTSTR)szLocalFileName, (LPCTSTR)szTmpFileName, FALSE);	// Overwrite

			// 체크인 수행!!
			if(m_csRKEntryWriter.Open())
			{
				bResult = m_csRKEntryWriter.WriteOwner(szOnlyFileName, lSize, szNewDate, CString(""), lVer+1);	// 새파일의 날짜를 기록

				if(bResult)
				{
					// BackUp 해준다.
					BackupFile(szFileName, lVer);

					// History 해준다.
					HistoryFile(szFileName, szNewDate, szOwner, lVer+1);	// 새로운 파일로 History 해준다. 2004.11.25.
					
					m_csRKEntryWriter.WriteOwner(szOnlyFileName, lSize, szNewDate, CString(""), lVer+1);	// 새파일의 날짜를 기록

					// 원래 Lock 유저로 돌려좀 - 2004.1.5. 김태희
					m_csRKEntryWriter.WriteOwner(szOnlyFileName, lSize, szNewDate, m_stRKEntryInfo.szOwner, lVer+1);	
				}

				m_csRKEntryWriter.Close();
			}

			if(bRealCopy)
			{
				// 이전 파일을 지운다. 아마 ReadOnly 일 것이다.
				RemoveFile(szFileName);

				// 올린 파일의 이름을 바꿔준다.
				MoveFileEx((LPCTSTR)szTmpFileName, (LPCTSTR)szFileName, MOVEFILE_REPLACE_EXISTING);
			}

			// ReadOnly 로 해준다.
//			SetReadOnly(szLocalFileName);
//			SetReadOnly(szFileName);
		}
	}

	return bResult;
}

// szLocalFileName, szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::UndoCheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// 이전에 세팅된 값을 읽는다.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// 파일 이름이 맞고, szOwner 가 Check-Out 중이었다면!!!
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
			m_stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) == 0)
		{
			if(m_csRKEntryWriter.Open())
			{
				// 아래 함수 호출할 때 인자에 주의 한다.
				bResult = m_csRKEntryWriter.WriteOwner(szOnlyFileName, atoi((LPCTSTR)m_stRKEntryInfo.szSize), m_stRKEntryInfo.szDate,
																				CString(""), atoi((LPCTSTR)m_stRKEntryInfo.szVer));
				m_csRKEntryWriter.Close();
			}
/*
			if(bResult)
				if(::AfxMessageBox(CONFIRM_REPLACE_FILE, MB_YESNO) == IDYES)
					CopyFile((LPCTSTR)szFileName, (LPCTSTR)szLocalFileName, FALSE);	// Overwrite
*/
		}
	}

	return bResult;
}

// szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::RemoveEntry(CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	CString szType;
	CFileFind csFileFind;
	if(csFileFind.FindFile((LPCTSTR)szFileName))
	{
		csFileFind.FindNextFile();
		szType = csFileFind.IsDirectory() ? "D" : "F";
	}

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// 이전에 세팅된 값을 읽는다.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// 파일 이름 맞고, 현재 소유주가 없을 때 가능이다.
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
			// 현재 체크아웃 중이거나, 소유자가 없을 때 가능하다.
			(m_stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) == 0 || m_stRKEntryInfo.szOwner.GetLength() == 0))
		{
			if(m_csRKEntryWriter.Open())
			{
				bResult = m_csRKEntryWriter.RemoveEntry(szType, szOnlyFileName);
				m_csRKEntryWriter.Close();
			}

			if(bResult)
			{
				// 백업 쪽을 먼저 지우고..
				RemoveBackupEntry(szFileName);

				// 원본을 지운다.
				if(!DeleteFile((LPCTSTR)szFileName))
					RemoveFile(szFileName);
			}
		}
	}

	return bResult;
}

// szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::RemoveDirectoryEntry(CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	// 서브 디렉토리를 돌면서 임시 Check-Out 을 건다.
	bResult = TemporaryCheckOut(szFileName, szOwner);

	// 성공하면 디렉토리를 날린다~
	if(bResult)
	{
		// 지우자~
		RemoveDirectoryNAllFiles(szFileName);

		int iIndex = szFileName.ReverseFind('\\');
		CString szUpDirectoryEntryFileName = szFileName.Left(iIndex + 1) + ENTRY_FILENAME;
		CString szOnlyName = szFileName.Right(szFileName.GetLength() - iIndex - 1);
		
		CRKWriter csRKWriter;
		csRKWriter.SetFileName(szUpDirectoryEntryFileName);
		if(csRKWriter.Open())
		{
			bResult = csRKWriter.RemoveEntry(CString("D"), szOnlyName);
			csRKWriter.Close();
		}

		if(bResult)
		{
			// 백업 쪽을 먼저 지운 후
			RemoveBackupDirectoryEntry(szFileName);

			// 원본도 지운다.
			if(!RemoveDirectory((LPCTSTR)szFileName))
				RemoveFile(szFileName);
		}
	}
	// 실패하면 임시 Check-Out 건 거를 풀어준다.
	else
	{
		UndoTemporaryCheckOut(szFileName, szOwner);
	}

	return bResult;
}

// 서브 디렉토리를 돌면서 모든 파일에 임시 Check-Out 을 건다.
// 하나라도 실패하면 바로 FALSE 를 리턴한다.
BOOL CRKEntryManager::TemporaryCheckOut(CString& szFileName, CString& szOwner)
{
	BOOL bResult = TRUE;

	CString szFindFileName = szFileName + "\\*.*";

	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile(szFindFileName);
	if(!bWorking)
		return FALSE;

	CString szOnlyFileName;
	CString szEntryFileName;
	CRKParser csRKParser;
	CRKWriter csRKWriter;
	CList<stRKEntryInfo, stRKEntryInfo> csEntryList;

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())	// . .. 은 continue
			continue;

		if(csFileFind.IsDirectory())	// 디렉토리라면, 재귀 돈다.
		{
			bResult = TemporaryCheckOut(csFileFind.GetFilePath(), szOwner);
			if(!bResult)	// 이게 실패하면 바로 리턴이다.
				return FALSE;

			continue;
		}

		szOnlyFileName = csFileFind.GetFileName();	// 해당 파일의 이름을 얻고..

		// Entries.rk 파일을 읽는다.
		if(szOnlyFileName.CompareNoCase(ENTRY_FILENAME) == 0)
		{
			szEntryFileName = szFileName + "\\" + ENTRY_FILENAME;
			csRKParser.SetFileName(szEntryFileName);
			csRKWriter.SetFileName(szEntryFileName);

			// 이전의 Entry 리스트를 비운다.
			csEntryList.RemoveAll();
			if(csRKParser.Open())
			{
				// 엔트리 정보를 List 에 넣는다.
				while(csRKParser.NewLine())
				{
					stRKEntryInfo stRKEntryInfo;
					csRKParser.GetEntryTokenStruct(stRKEntryInfo);
					csEntryList.AddTail(stRKEntryInfo);
				}
				csRKParser.Close();
			}

			int iCount = csEntryList.GetCount();
			POSITION p = csEntryList.GetHeadPosition();
			for(int i = 0; i < iCount; i++)
			{
				stRKEntryInfo stRKEntryInfo = csEntryList.GetNext(p);

				// 해당 파일의 정보를 분석한다.
				if(stRKEntryInfo.szOwner.GetLength() != 0 &&
					stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) != 0)	// 기존에 체크아웃 하고 있는 파일이라면 중단!!!
					return FALSE;	// 바로 리턴

				if(csRKWriter.Open())
				{
					bResult = csRKWriter.WriteOwner(stRKEntryInfo.szFileName, atoi((LPCTSTR)stRKEntryInfo.szSize),
														stRKEntryInfo.szDate, szOwner, atoi((LPCTSTR)stRKEntryInfo.szVer));
					csRKWriter.Close();
				}

				if(!bResult)	// 역시 실패하면 바로 나간다.
					return FALSE;
			}
		}	// Entries.rk 파일 읽음
	}	// while(bWorking)

	return bResult;
}

// 서브 디렉토리를 돌면서 임시로 Check-Out 걸려있던 모든 파일을 푼다.
// 중간에 실패하더라도 끝까지 돈다.
BOOL CRKEntryManager::UndoTemporaryCheckOut(CString& szFileName, CString& szOwner)
{
	BOOL bResult = TRUE;

	CString szFindFileName = szFileName + "\\*.*";

	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile(szFindFileName);
	if(!bWorking)
		return FALSE;

	CString szOnlyFileName;
	CString szEntryFileName;
	CRKParser csRKParser;
	CRKWriter csRKWriter;
	CList<stRKEntryInfo, stRKEntryInfo> csEntryList;

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())	// . .. 은 continue
			continue;

		if(csFileFind.IsDirectory())	// 디렉토리라면, 재귀 돈다.
		{
			UndoTemporaryCheckOut(csFileFind.GetFilePath(), szOwner);
			continue;
		}

		szOnlyFileName = csFileFind.GetFileName();	// 해당 파일의 이름을 얻고..

		// Entries.rk 파일을 읽는다.
		if(szOnlyFileName.CompareNoCase(ENTRY_FILENAME) == 0)
		{
			szEntryFileName = szFileName + "\\" + ENTRY_FILENAME;
			csRKParser.SetFileName(szEntryFileName);
			csRKWriter.SetFileName(szEntryFileName);

			// 이전의 Entry 리스트를 비운다.
			csEntryList.RemoveAll();
			if(csRKParser.Open())
			{
				// 엔트리 정보를 List 에 넣는다.
				while(csRKParser.NewLine())
				{
					stRKEntryInfo stRKEntryInfo;
					csRKParser.GetEntryTokenStruct(stRKEntryInfo);
					csEntryList.AddTail(stRKEntryInfo);
				}
				csRKParser.Close();
			}

			int iCount = csEntryList.GetCount();
			POSITION p = csEntryList.GetHeadPosition();
			for(int i = 0; i < iCount; i++)
			{
				stRKEntryInfo stRKEntryInfo = csEntryList.GetNext(p);

				// 해당 파일의 정보를 분석한다.
				if(stRKEntryInfo.szOwner.GetLength() != 0 &&
					stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) != 0)	// 다른 사람이 Check-Out 하고 있으면 Skip
					continue;

				if(csRKWriter.Open())
				{
					bResult = csRKWriter.WriteOwner(stRKEntryInfo.szFileName, atoi((LPCTSTR)stRKEntryInfo.szSize),
														stRKEntryInfo.szDate, CString(""), atoi((LPCTSTR)stRKEntryInfo.szVer));
					csRKWriter.Close();
				}
			}
		}	// Entries.rk 파일 읽음
	}	// while(bWorking)

	return bResult;
}

// szFileName 에는 완전한 PATH 가 넘어와야 된다.
// Entries.rk 의 전체 경로가 오는 것이다.
BOOL CRKEntryManager::GetEntryList(CString& szFileName, CList<stRKEntryInfo, stRKEntryInfo>& csList)
{
	BOOL bResult = FALSE;

	m_csRKEntryParser.SetFileName(szFileName);
	if(m_csRKEntryParser.Open())
	{
		while(m_csRKEntryParser.NewLine())
		{
			stRKEntryInfo stRKEntryInfo;
			m_csRKEntryParser.GetEntryTokenStruct(stRKEntryInfo);
			csList.AddTail(stRKEntryInfo);
		}

		m_csRKEntryParser.Close();
		bResult = TRUE;
	}

	return bResult;
}

// szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::GetHistoryList(CString& szFileName, CList<stRKHistoryInfo, stRKHistoryInfo>& csList)
{
	BOOL bResult = FALSE;

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// 현재 버전을 넣는다.
	/*
	CString szEntryFileName;
	GetDirectoryPath(szFileName, szEntryFileName);
	szEntryFileName += "\\"ENTRY_FILENAME;
	m_csRKEntryParser.SetFileName(szEntryFileName);

	stRKEntryInfo stRKEntryInfo;
	if(GetEntryFileInfo(szOnlyFileName, &stRKEntryInfo))
	{
		stRKHistoryInfo stHistoryInfo;
		stHistoryInfo.szDate = stRKEntryInfo.szDate;
		stHistoryInfo.szOwner = stRKEntryInfo.szOwner;
		stHistoryInfo.szVer = stRKEntryInfo.szVer;

		csList.AddTail(stHistoryInfo);

		bResult = TRUE;
	}
	else
	{
		// 현재 정보를 못 얻었으면 나간다.
		return FALSE;
	}
	*/

	// 히스토리에서 찾아서 넣는다.
	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	CString szHistoryFileName;
	szHistoryFileName.Format("%s_%s", szOnlyFileName, HISTORY_SUBNAME);
	szTargetPath += "\\" + szHistoryFileName;

	m_csRKHistoryParser.SetFileName(szTargetPath);
	if(m_csRKHistoryParser.Open())
	{
		while(m_csRKHistoryParser.NewLine())
		{
			stRKHistoryInfo stRKHistoryInfo;
			m_csRKHistoryParser.GetHistoryTokenStruct(stRKHistoryInfo);
			csList.AddTail(stRKHistoryInfo);
		}
		m_csRKHistoryParser.Close();
		bResult = TRUE;
	}

	return bResult;
}

// szFileName 에는 완전한 PATH 가 넘어와야 된다.
CString CRKEntryManager::GetBackupFilePath(CString& szFileName, INT32 lVer)
{
	CString szNewPath;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	GetBackupFilePathByFileName(szFileName, szNewPath);

	CString szVer;
	szVer.Format("%d", lVer);
	szNewPath += CString("\\") + szOnlyFileName + CString("_") + szVer + CString("_") + CString(BACKUP_SUBNAME);

	return szNewPath;
}

// szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::BackupFile(CString& szFileName, INT32 lVer)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	CreateSubDirectory(szTargetPath);

	CString szBackupFileName;
	szBackupFileName.Format("%s_%d_%s", szOnlyFileName, lVer, BACKUP_SUBNAME);
	szTargetPath += "\\" + szBackupFileName;

	bResult = CopyFile((LPCTSTR)szFileName, (LPCTSTR)szTargetPath, FALSE);	// Overwrite

	return bResult;
}

// szFileName 에는 완전한 PATH 가 넘어와야 된다.
BOOL CRKEntryManager::HistoryFile(CString szFileName, CString szDate, CString szOwner, INT32 lVer)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	CreateSubDirectory(szTargetPath);

	CString szHistoryFileName;
	szHistoryFileName.Format("%s_%s", szOnlyFileName, HISTORY_SUBNAME);
	szTargetPath += "\\" + szHistoryFileName;

	m_csRKHistoryWriter.SetFileName(szTargetPath);
	if(m_csRKHistoryWriter.Open())
	{
		m_csRKHistoryWriter.WriteHistory(szOnlyFileName, szDate, szOwner, lVer);
		m_csRKHistoryWriter.Close();

		bResult = TRUE;
	}

	return bResult;
}

BOOL CRKEntryManager::RemoveBackupEntry(CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	szTargetPath += "\\*.*";
	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile((LPCTSTR)szTargetPath);

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())
			continue;

		if(csFileFind.IsDirectory())
			continue;

		// 해당 파일의 백업 파일이라면...
		if(csFileFind.GetFileName().Left(szOnlyFileName.GetLength()).CompareNoCase((LPCTSTR)szOnlyFileName) == 0)
		{
			// 지우자~
			if(!DeleteFile((LPCTSTR)csFileFind.GetFilePath()))
				RemoveFile(csFileFind.GetFilePath());

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL CRKEntryManager::RemoveBackupDirectoryEntry(CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	RemoveDirectoryNAllFiles(szTargetPath);

	bResult = RemoveDirectory(szTargetPath);
	if(!bResult)
	{
		RemoveFile(szTargetPath);
		bResult = TRUE;
	}

	return bResult;
}

// szFileName 에는 원본 PATH 가 온다. - 디렉토리가 올 수도 있고, 파일 이름 풀 경로가 올 수도 있다.
// szNewPath 에는 PATH 만 넘어간다. 
void CRKEntryManager::GetBackupFilePathByFileName(CString& szFileName, CString& szNewPath)
{
	szNewPath = szFileName;
	int iIndex = 0;

	CFileFind csFileFind;
	if(csFileFind.FindFile(szNewPath))
	{
		csFileFind.FindNextFile();
		if(!csFileFind.IsDirectory())	// 파일이라면...
		{
			iIndex = szNewPath.ReverseFind(TCHAR('\\'));
			if(iIndex > 0)
				szNewPath.Delete(iIndex, szNewPath.GetLength() - iIndex);
		}
	}

	// Root 디렉토리에서는 Find 가 안되는 경우가 발생해서 긴급 수정!!
	if(szNewPath.GetLength() == 0) return;
	if(szNewPath.GetAt(szNewPath.GetLength() - 1) != '\\')
		szNewPath += "\\";

	CString szReplace1 = "\\" + CString(REMOTE_ROOT_PATH) + "\\";
	CString szReplace2 = "\\" + CString(REMOTE_BACKUP_PATH) + "\\";

	iIndex = szNewPath.Find(szReplace1);
	szNewPath.Delete(iIndex, szReplace1.GetLength());
	szNewPath.Insert(iIndex, (LPCTSTR)szReplace2);

	// 한번 더 지워준다.
	RemoveDirectoryFinalSlash(szNewPath);
}

// 읽기 전용 파일이라면, 풀고 지운다.
void CRKEntryManager::RemoveFile(CString& szFileName)
{
	CFileStatus csStatus;
	if(CFile::GetStatus((LPCTSTR)szFileName, csStatus))
	{
		if(csStatus.m_attribute & CFile::readOnly)
		{
			OutputDebugString("RemoveFile 에서 읽기 전용 풀고 지움\n");

			csStatus.m_mtime = 0;	// 수정 시간에는 변화를 주지 않는다.
			csStatus.m_attribute &= ~CFile::readOnly;

			if(csStatus.m_attribute & CFile::directory)
			{
				Sleep(0);
				CFile::SetStatus((LPCTSTR)szFileName, csStatus);
				RemoveDirectory((LPCTSTR)szFileName);
			}
			else
			{
				CFile::SetStatus((LPCTSTR)szFileName, csStatus);
				DeleteFile((LPCTSTR)szFileName);
			}
		}
	}
}

void CRKEntryManager::RemoveDirectoryNAllFiles(CString& szDirectoryName)
{
	RemoveDirectoryFinalSlash(szDirectoryName);

	CString szFind = szDirectoryName + "\\*.*";
	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile(szFind);

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())
			continue;

		if(csFileFind.IsDirectory())
		{
			RemoveDirectoryNAllFiles(csFileFind.GetFilePath());

			Sleep(0);	// 이거 안주면.. 디렉토리가 안지워지는 경우가 있다.

			BOOL bResult = RemoveDirectory((LPCTSTR)csFileFind.GetFilePath());
			if(!bResult)
			{
				RemoveFile(csFileFind.GetFilePath());
				bResult = TRUE;
			}

			continue;
		}

		if(!DeleteFile((LPCTSTR)csFileFind.GetFilePath()))
			RemoveFile(csFileFind.GetFilePath());
	}
}

void CRKEntryManager::RemoveDirectoryFinalSlash(CString& szFileName)
{
	if(szFileName.GetLength() == 0) return;

	if(szFileName.GetAt(szFileName.GetLength() - 1) == '\\')
		szFileName = szFileName.Left(szFileName.GetLength() - 1);
}

void CRKEntryManager::CreateSubDirectory(CString& szPath)
{
	int iIndex = 0;
	if(szPath.GetAt(0) == '\\')
		iIndex = szPath.Find("\\", 2);
	else
		iIndex = szPath.Find("\\", 0);

	while((iIndex = szPath.Find("\\", iIndex + 1)) >= 0)
	{
		if(_access((LPCTSTR)szPath.Left(iIndex), 0) == -1)
		{
			CreateDirectory(szPath.Left(iIndex), NULL);
		}
	}

	// Full Path 도 검사
	if(_access((LPCTSTR)szPath, 0) == -1)
		CreateDirectory(szPath, NULL);
}

void CRKEntryManager::GetDirectoryPath(CString& szFileName, CString& szPath)
{
	if(szFileName.IsEmpty())
		return;

	RemoveDirectoryFinalSlash(szFileName);

	INT32 iPos = szFileName.ReverseFind('\\');
	szPath = szFileName.Left(iPos);
}

void CRKEntryManager::SetReadOnly(CString& szFileName, BOOL bFlag)
{
	CFileStatus csStatus;
	if(CFile::GetStatus((LPCTSTR)szFileName, csStatus))
	{
		csStatus.m_mtime = 0;	// 수정 시간에는 변화를 주지 않는다.

		if(bFlag)
			csStatus.m_attribute |= CFile::readOnly;	// ReadOnly 준다.
		else
			csStatus.m_attribute &= ~CFile::readOnly;	// ReadOnly 푼다.			

		if(csStatus.m_attribute & CFile::directory)
			Sleep(0);

		CFile::SetStatus((LPCTSTR)szFileName, csStatus);
	}
}

BOOL CRKEntryManager::SetModifyTime(CString& szOriginFileName, CString& szTargetFileName)
{
	BOOL bResult = FALSE;
	CFileStatus csOriginStatus, csTargetStatus;
	if(CFile::GetStatus((LPCTSTR)szOriginFileName, csOriginStatus) && CFile::GetStatus((LPCTSTR)szTargetFileName, csTargetStatus))
	{
		csTargetStatus.m_mtime = csOriginStatus.m_mtime;
		CFile::SetStatus((LPCTSTR)szTargetFileName, csTargetStatus);

		bResult = TRUE;
	}

	return bResult;
}