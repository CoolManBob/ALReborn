// AuRKSingleton.cpp: implementation of the AuRKSingleton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AuResourceKeeper.h"
#include "AuRKSingleton.h"
#include "RKInclude.h"
#include "FolderDlg.h"
#include "AuHistoryDlg.h"
#include "AuRKMainDlg.h"
#include "ProgressDlg.h"

#include <afxdlgs.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuRKSingleton::AuRKSingleton()
{
	m_bAdmin = FALSE;
	m_bLogin = FALSE;
	m_iErrorCode = 0;

	Initialize();
}

AuRKSingleton::~AuRKSingleton()
{
}

AuRKSingleton* AuRKSingleton::_Instance = 0;
AuRKSingleton* AuRKSingleton::Instance()
{
	if(!_Instance)
		_Instance = new AuRKSingleton();

	return _Instance;
}

void AuRKSingleton::DeleteInstance()
{
	if( _Instance )
		delete _Instance;
}

void AuRKSingleton::InitCurrentPath()
{
	// 현재 실행 디렉토리 얻기
	CHAR pszPath[MAX_PATH];
	::GetModuleFileName(::GetModuleHandle(NULL), pszPath, MAX_PATH);

	m_strCurrentPath = pszPath;
	m_strCurrentPath.MakeUpper();

	int iPos = m_strCurrentPath.ReverseFind('\\');
	if (iPos)
	{
		m_strCurrentPath.Delete(iPos, m_strCurrentPath.GetLength() - iPos);
	}
}

void AuRKSingleton::InitUserIDPassword()
{
	// INI 파일에 저장된 User ID 얻기
	CHAR pszBuffer[MAX_PATH];
	::GetPrivateProfileString(INIAPP, USERID, "RK_USER", pszBuffer, MAX_PATH, m_strCurrentPath + PATH(INIFILE));
	m_strUserID = pszBuffer;
	
	// JNY TODO : Password도 저장된거 읽어오게 변경 필요
}

BOOL AuRKSingleton::Initialize()
{
	InitCurrentPath();

	// INIFILE 존재 여부 확인 -> INIFILE은 있어도 없어도 상관없음
	if (!IsFileExist(m_strCurrentPath + PATH(INIFILE)))
	{
		InitUserIDPassword();
	}

	return TRUE;
}

BOOL AuRKSingleton::IsFileExist(LPCSTR sFilePath)
{
	// 파일 존재 여부 확인
    if (!access(_T(sFilePath), 00))
        return true;	// 파일 존재
    else return false;	// 존재하지 않음
}

BOOL AuRKSingleton::InitRepositoryRootPath()
{
	// Repository 경로 얻기
	CHAR pszPathName[MAX_PATH];
	::GetPrivateProfileString(INIAPP, REPOROOT, INIFAILED, pszPathName, MAX_PATH, m_strCurrentPath + PATH(INIFILE));

	if (!strcmp(pszPathName, INIFAILED))
	{
		// Key값에 해당하는 문자를 읽어오지 못했을때
		m_strRepoRootPath = "";
		m_iErrorCode = EC_INIFILE;
		return FALSE;
	}
	else 
	{
		// Key값에 해당하는 문자를 읽어왔을때
		m_strRepoRootPath = pszPathName;
		m_iErrorCode = EC_EXECUTABLE;
		return TRUE;
	}
}

BOOL AuRKSingleton::GetRepoPath(CString &rRepoName, CString &rPathName)
{
	// Repository 경로를 얻어옴 -> 획득한 Repository 경로는 info.rk 에 저장
	CFileDialog FileDlg(TRUE, "*.ini", RKADMIN, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "RK Admin (RKAdmin.ini)|RKAdmin.ini||");
	if (IDOK == FileDlg.DoModal())
	{
		rPathName = FileDlg.GetPathName();
		int iPos = rPathName.ReverseFind('\\');
		rPathName.Delete(iPos, rPathName.GetLength() - iPos);

		CHAR pszRepoName[MAX_PATH];
		::GetPrivateProfileString(RKADMIN_APP, RKADMIN_REPONAME, "Default", pszRepoName, MAX_PATH, rPathName + PATH(RKADMIN));
		rRepoName = pszRepoName;

		return TRUE;
	}

	return FALSE;
}

void AuRKSingleton::CreateRepository()
{
	// 폴더 생성 가능한 다이얼로그 
	LPCTSTR lpszTitle = _T( "Repository를 생성할 폴더를 선택하세요" );
	UINT	uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	CString strFolderPath, strFolderName;
	
	CFolderDialog* lpDlg = NULL;
	lpDlg = new CFolderDialog( lpszTitle, strFolderPath, ::AfxGetMainWnd(), uFlags );

	if( lpDlg != NULL )
	{	
		if( lpDlg->DoModal() == IDOK )
		{
			strFolderName = lpDlg->GetFolderName();
			strFolderPath = lpDlg->GetFolderPath();		

			WORD  wIcon = 0;
			TCHAR szPath[ MAX_PATH ] = { 0 };
			::lstrcpy( szPath,  strFolderPath );
		}

		delete lpDlg;
		lpDlg = NULL;
	}

	// Repository 폴더 생성 및 RK, RK_BAK폴더, 관리파일 생성
	::CreateDirectory(strFolderPath, NULL);
	::CreateDirectory(strFolderPath + PATH(REMOTE_ROOT_PATH), NULL);
	::CreateDirectory(strFolderPath + PATH(REMOTE_BACKUP_PATH), NULL);

	::WritePrivateProfileString(RKADMIN_APP, RKADMIN_REPONAME, strFolderName, strFolderPath + PATH(RKADMIN));

	// User ID를 추가할수 있는 환경까지는 고려해 줘야 됨
	m_strRepoRootPath = strFolderPath;
	CRKRepository csRepository;
	csRepository.SetInfoFileName(m_strCurrentPath + PATH(INFO_FILENAME));
	csRepository.AddRepository(strFolderName, strFolderPath, CString(""));
}

BOOL AuRKSingleton::GetFolderNameDlg(LPCTSTR lpTitle, CHAR* pszPath, INT32 iLength)
{
	// 다이얼로그로부터 선택된 폴더의 Path를 돌려준다.
	ITEMIDLIST      *pidlBrowse;

    BROWSEINFO BrInfo;
    BrInfo.hwndOwner = ::AfxGetMainWnd()->GetSafeHwnd();
    BrInfo.pidlRoot = NULL;

    ::ZeroMemory(&BrInfo, sizeof(BrInfo));
    BrInfo.pszDisplayName = pszPath;
    BrInfo.lpszTitle = lpTitle;
    BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    pidlBrowse = ::SHBrowseForFolder(&BrInfo);    
    if(NULL != pidlBrowse)
    {
        ::SHGetPathFromIDList(pidlBrowse, pszPath);  // 폴더명 얻기 pszPathName
		return TRUE;
	}
	else return FALSE;
}

void AuRKSingleton::SetWorkingFolder()
{
	// Client의 루트 디렉토리 설정
    CHAR pszPathName[MAX_PATH];

	if (m_strRepoName.IsEmpty()) return;

	if (GetFolderNameDlg("Working Folder 선택", pszPathName, MAX_PATH))
	{
		CRKRepository cRepository;

		// INFO_FILENAME 파일 설정
		cRepository.SetInfoFileName(m_strCurrentPath + PATH(INFO_FILENAME));

		// INFO_FILENAME 파일 열기
		if (cRepository.LoadInfoFile()) 
		{
			stRepositoryData* pRepoData = cRepository.GetRepositoryData(m_strRepoName);
			m_strRepoRootPath = pRepoData->szPath;
			m_strWorkingFolder = pszPathName;
			m_strRepoName = pRepoData->szName;

			m_strRepoRootPath.MakeUpper();
			m_strWorkingFolder.MakeUpper();
			m_strRepoName.MakeUpper();
			
			cRepository.SetWorkingFolder(m_strRepoName, m_strWorkingFolder);
		}
	}
}

BOOL AuRKSingleton::AddFolder(LPCTSTR lpExistPath, LPCTSTR lpNewPath)
{
	CProgressDlg ProgressDlg;
	ProgressDlg.AddFolder(lpExistPath, lpNewPath);
	if (IDOK == ProgressDlg.DoModal()) return TRUE;
	else return FALSE;
}

BOOL AuRKSingleton::IsAvailableRepository(LPCTSTR lpRepositoryPath)
{
	// Open 가능한 Repository인지 확인 -> 확인 기준은 해당 디렉토리에 RKAdmin.ini 파일의 존재 유무로 확인
	return IsFileExist(CString(lpRepositoryPath) + PATH(RKADMIN));
}

BOOL AuRKSingleton::IsDirectoryInRepository(LPCTSTR szPath)
{
	if(!IsFileExist(szPath))
		return FALSE;

	CString szFullPath = szPath;
	CFileFind csFileFind;
	if(csFileFind.FindFile(szFullPath))
	{
		csFileFind.FindNextFile();
		return csFileFind.IsDirectory();
	}

	return FALSE;
}

BOOL AuRKSingleton::ViewHistory(LPCTSTR lpFilePath)
{
//	if (!IsFileExist(lpFilePath)) return FALSE;

	AuHistoryDlg HistoryDlg;
	HistoryDlg.m_strFileName = lpFilePath;
	HistoryDlg.DoModal();

	return TRUE;
}

BOOL AuRKSingleton::GetLatestFolder(LPCTSTR lpExistPath)
{
	CProgressDlg dlg;
	dlg.GetLatestFolder(lpExistPath);
	if (IDOK == dlg.DoModal()) return TRUE;
	else return FALSE;
}

BOOL AuRKSingleton::GetLatestFile(LPCTSTR lpExistFile)
{
	CProgressDlg dlg;
	dlg.GetLatestFile(lpExistFile);
	if (IDOK == dlg.DoModal()) return TRUE;
	else return FALSE;

/*
	CRKEntryManager cFileVersion;
	if (!cFileVersion.CheckLatestVersion(CString(lpExistingFile), CString(lpNewFile)))
	{
		// 최신 버젼이 아니라면
		CreateFolder(lpNewFile);	// 해당 폴더가 없을 경우 폴더 경로를 완성시켜줌
		::CopyFile(lpExistingFile, lpNewFile, FALSE);
		return TRUE;
	}

	return FALSE;
*/
}

void AuRKSingleton::CreateFolder(LPCTSTR lpPath)
{
	// 폴더를 생성
	CString strPath = lpPath;
	INT32 iPos = strPath.ReverseFind('\\');
	strPath.Delete(iPos, strPath.GetLength() - iPos);

	INT32 iIndex = strPath.Find('\\');
	if (-1 != iIndex) 
	{
		iIndex = strPath.Find('\\', iIndex);
		if (-1 == iIndex) return;
	}
	else return;
	

	if (!IsFileExist(lpPath))
	{
		CreateFolder(strPath);
	}

	::CreateDirectory(strPath, NULL);
}

// 2004.11.17. steeple
BOOL AuRKSingleton::Login(LPCTSTR szUserID, LPCTSTR szPassword, LPCTSTR szWorkingFolderPath, LPCTSTR szReporitoryPath)
{
	if(!szUserID || !szPassword || !szWorkingFolderPath || !szReporitoryPath)
		return FALSE;

	CString strUserID, strPassword, strWorkingFolder, strRepositoryPath;
	strUserID = szUserID;
	strPassword = szPassword;
	strWorkingFolder = szWorkingFolderPath;
	strRepositoryPath = szReporitoryPath;

	if(strUserID.IsEmpty() || strPassword.IsEmpty() || strWorkingFolder.IsEmpty() || strRepositoryPath.IsEmpty())
		return FALSE;

	// Hard-Coded Admin 일 경우
	if(strUserID.CompareNoCase(ADMIN_ID) == 0 && strPassword.CompareNoCase(ADMIN_PASSWORD) == 0)
	{
		m_bLogin = TRUE;
		m_bAdmin = TRUE;
	}
	else
	{
		CRKUserManager csUserManager;
		csUserManager.SetRemotePath(strRepositoryPath);
		
		m_bLogin = csUserManager.Login(strUserID, strPassword);
		if(m_bLogin)
			m_bAdmin = csUserManager.IsAdmin();
	}

	if(m_bLogin)
	{
		m_strUserID = strUserID;
		m_strRepoRootPath = strRepositoryPath;
		m_strRepoName = REPONAME;
		m_strWorkingFolder = strWorkingFolder;

		m_strRepoRootPath.MakeUpper();
		m_strRepoName.MakeUpper();
		m_strWorkingFolder.MakeUpper();
	}

	return m_bLogin;
}

// 2004.11.17. steeple
BOOL AuRKSingleton::Logout(LPCTSTR szUserID /* = NULL */)
{
	if(!m_bLogin)
		return FALSE;

	CString strUserID;
	if(szUserID)
		strUserID = szUserID;
	else
		strUserID = m_strUserID;

	m_bLogin = FALSE;
	m_bAdmin = FALSE;
	m_strUserID.Empty();

	return TRUE;
}

// 2004.11.18. steeple
BOOL AuRKSingleton::FileDownLoad(LPCTSTR lpFileName)
{
	if(!lpFileName)
		return FALSE;

	CString szLocalPath = lpFileName;
	CString szRepositoryPath = lpFileName;

	ConvertLocalPath(szLocalPath);
	ConvertRemotePath(szRepositoryPath);

	// Repostiory 에 파일이 없으면 GG
	if(!IsFileExist((LPCTSTR)szRepositoryPath))
		return FALSE;

	// Local 에 없으면 걍 복사한다.
	if(!IsFileExist((LPCTSTR)szLocalPath))
	{
		// Local 에 경로를 만들고
		CreateFolder(szLocalPath);
		return CopyFile((LPCTSTR)szRepositoryPath, (LPCTSTR)szLocalPath, FALSE);	// Overwrite
	}

	// Local 에 있으면 최신버전인지 비교해서 아니면 복사
	CRKEntryManager csEntryManager;
	if(!csEntryManager.CheckLatestVersion(szLocalPath, szRepositoryPath))
	{
		// Local 에 있는 걸 지우고
		csEntryManager.RemoveFile(szLocalPath);
		
		// Local 에 경로를 만들고
		CreateFolder(szLocalPath);
		return CopyFile((LPCTSTR)szRepositoryPath, (LPCTSTR)szLocalPath, FALSE);	// Overwrite
	}

	return FALSE;
}

void AuRKSingleton::ConvertRemotePath(CString &rPath)
{
	if (rPath.Find(RKROOT) != -1)
	{
		rPath.Delete(0, ::strlen(RKROOT));
		if (rPath.GetLength() && (rPath[0] == '\\')) rPath.Delete(0, 1);
		rPath = m_strRepoRootPath + PATH(REMOTE_ROOT_PATH) + PATH(rPath);
		if (rPath[rPath.GetLength() - 1] == '\\') rPath.Delete(rPath.GetLength() - 1, 1);
	}
}

void AuRKSingleton::ConvertLocalPath(CString &rPath)
{
	if (rPath.Find(RKROOT) != -1)
	{
		rPath.Delete(0, ::strlen(RKROOT));
		if (rPath.GetLength() && (rPath[0] == '\\')) rPath.Delete(0, 1);
		if (m_strWorkingFolder.Right(1) != '\\')
			rPath = m_strWorkingFolder + PATH(rPath);
		else
			rPath = m_strWorkingFolder + rPath;
		if (rPath[rPath.GetLength() - 1] == '\\') rPath.Delete(rPath.GetLength() - 1, 1);
	}
}

CString AuRKSingleton::GetFolderPath(CString &rFilePath)
{
	if(rFilePath.GetLength() == 0) return rFilePath;

	// 2004.11.23. steeple
	// $RK 가 없으면 맨 앞에 붙여준다.
	CString strFolderPath = _T("");
	if(rFilePath.Find(RKROOT) == -1)
		strFolderPath = RKROOT"\\";

	strFolderPath += rFilePath;
	
	// 맨 마지막 \ 를 지워준다.
	if(strFolderPath.GetAt(strFolderPath.GetLength() - 1) == '\\')
		strFolderPath = strFolderPath.Left(strFolderPath.GetLength() - 1);

	INT32 iPos = strFolderPath.ReverseFind('\\');
	strFolderPath.Delete(iPos, strFolderPath.GetLength() - iPos);

	return strFolderPath;
}

CString AuRKSingleton::GetFileName(CString &rFilePath)
{
	CString strFileName = rFilePath;
	// 맨 마지막 \ 를 지워준다.
	if(strFileName.GetAt(strFileName.GetLength() - 1) == '\\')
		strFileName = strFileName.Left(strFileName.GetLength() - 1);

	INT32 iPos = strFileName.ReverseFind('\\');
	strFileName.Delete(0, iPos + 1);

	return strFileName;
}

BOOL AuRKSingleton::RecursiveUpdateFileList(LPCTSTR lpPath, CList<FileInfo, FileInfo> *pList, BOOL bRecursive)
{
	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	CString strPath = lpPath;
	ConvertRemotePath(strPath);

	CString strLocalPath = lpPath;
	ConvertLocalPath(strLocalPath);

	cRKEntryManager.GetEntryList(strPath + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if(stRKEntryInfo.szFileName.IsEmpty())
			continue;

		if (RK_FILE == stRKEntryInfo.szType)
		{
			// 존재하는 파일만 비교
			if (AuRKSingleton::Instance()->IsFileExist(strLocalPath + PATH(stRKEntryInfo.szFileName)))
			{
				CRKEntryManager cFileVersion;
				if ((!cFileVersion.CheckLatestVersion(strPath + PATH(stRKEntryInfo.szFileName), strLocalPath + PATH(stRKEntryInfo.szFileName))))
				{
					FileInfo stFileInfo;
					stFileInfo.m_strFileName = CString(lpPath) + PATH(stRKEntryInfo.szFileName);
					stFileInfo.m_strDate = stRKEntryInfo.szDate;
					pList->AddTail(stFileInfo);
				}
			}
		}
		else
		{
			if(bRecursive)
				RecursiveUpdateFileList(CString(lpPath) + PATH(stRKEntryInfo.szFileName), pList);
		}
	}

	return TRUE;
}

BOOL AuRKSingleton::RecursiveNewFileList(LPCTSTR lpPath, CList<FileInfo, FileInfo> *pList, BOOL bRecursive)
{
	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	CString strPath = lpPath;
	ConvertRemotePath(strPath);

	CString strLocalPath = lpPath;
	ConvertLocalPath(strLocalPath);

	cRKEntryManager.GetEntryList(strPath + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();

	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if (RK_FILE == stRKEntryInfo.szType)
		{
			// 존재하는 파일만 비교
			if (!AuRKSingleton::Instance()->IsFileExist(strLocalPath + PATH(stRKEntryInfo.szFileName)))
			{
				FileInfo stFileInfo;
				stFileInfo.m_strFileName = CString(lpPath) + PATH(stRKEntryInfo.szFileName);
				stFileInfo.m_strDate = stRKEntryInfo.szDate;
				pList->AddTail(stFileInfo);
			}
		}
		else
		{
			if(bRecursive)
				RecursiveNewFileList(CString(lpPath) + PATH(stRKEntryInfo.szFileName), pList);
		}
	}

	return TRUE;
}

BOOL AuRKSingleton::GetUpdatedFileList(CList<FileInfo, FileInfo> *pList)
{
	return RecursiveUpdateFileList(RKROOT, pList);
}

BOOL AuRKSingleton::GetNewFileList(CList<FileInfo, FileInfo> *pList)
{
	return RecursiveNewFileList(RKROOT, pList);
}

// 2004.11.16. steeple
BOOL AuRKSingleton::GetUpdatedFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive)
{
	if(!pList || !lpFolderPath)
		return FALSE;

	return RecursiveUpdateFileList(lpFolderPath, pList, bRecursive);
}

// 2004.11.16. steeple
BOOL AuRKSingleton::GetNewFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive)
{
	if(!pList || !lpFolderPath)
		return FALSE;

	return RecursiveNewFileList(lpFolderPath, pList, bRecursive);
}

void AuRKSingleton::EnableReadOnly(LPCTSTR lpExistingFile, BOOL bEnable)
{
	CFileStatus csStatus;
	if(CFile::GetStatus((LPCTSTR)lpExistingFile, csStatus))
	{
		csStatus.m_mtime = 0;	// 수정 시간에는 변화를 주지 않는다.

		if(bEnable)
			csStatus.m_attribute |= CFile::readOnly;	// ReadOnly 준다.
		else
			csStatus.m_attribute &= ~CFile::readOnly;	// ReadOnly 푼다.			

		if(csStatus.m_attribute & CFile::directory)
			Sleep(0);

		CFile::SetStatus((LPCTSTR)lpExistingFile, csStatus);
	}

	::Sleep(0);
}

BOOL AuRKSingleton::AddFile(LPCTSTR lpExistFilePath, LPCTSTR lpNewFilePath)
{
	CProgressDlg csProgressDlg;
	csProgressDlg.AddFile(lpExistFilePath, lpNewFilePath);

	if (IDOK == csProgressDlg.DoModal()) return TRUE;
	else return FALSE;
}

// 2004.11.25. steeple
BOOL AuRKSingleton::GetHistoryList(LPCTSTR lpFileName, CList<FileInfo, FileInfo>* pList)
{
	if(!lpFileName || !pList)
		return FALSE;

	CRKEntryManager csEntryManager;
	CList<stRKHistoryInfo, stRKHistoryInfo> csList;
	
	CString szRemoteFileName = lpFileName;
	ConvertRemotePath(szRemoteFileName);

	if(!csEntryManager.GetHistoryList(szRemoteFileName, csList))
		return FALSE;

	CString szOnlyFileName = GetFileName(szRemoteFileName);
	CString szFolderPath = GetFolderPath(CString(lpFileName));
	
	POSITION p = csList.GetHeadPosition();
	while(p)
	{
		stRKHistoryInfo stRKHistoryInfo = csList.GetNext(p);
		
		FileInfo stFileInfo;
		stFileInfo.m_strFileName = szFolderPath + PATH(szOnlyFileName);
		stFileInfo.m_strDate = stRKHistoryInfo.szDate;
		stFileInfo.m_strOwner = stRKHistoryInfo.szOwner;
		stFileInfo.m_iVersion = atoi((LPCTSTR)stRKHistoryInfo.szVer);

		pList->AddTail(stFileInfo);
	}

	return TRUE;
}

// 2004.11.25. steeple
// 이걸 하게 되면 Repository -> Local 로 덮어 쓰게 되므로, 미리 덮어쓸건지 물어보면 좋을 듯.
BOOL AuRKSingleton::GetHistoryFile(LPCTSTR lpFileName, INT32 lVersion)
{
	if(!lpFileName || lVersion < 1)
		return FALSE;
	
	CString szRemoteFileName = lpFileName;
	ConvertRemotePath(szRemoteFileName);

	CString szRemoteFilePath;
	CString szOnlyFileName = GetFileName(szRemoteFileName);
	CRKEntryManager csEntryManager;
	csEntryManager.GetDirectoryPath(szRemoteFileName, szRemoteFilePath);
	csEntryManager.SetEntryParserFileName(szRemoteFilePath + PATH(ENTRIES));

	// 현재 정보를 얻어본다.
	stRKEntryInfo stRKEntryInfo;
	if(!csEntryManager.GetEntryFileInfo(szOnlyFileName, &stRKEntryInfo))
		return FALSE;

	// 버젼 검사를 한다.
	INT32 lLatestVersion = atoi((LPCTSTR)stRKEntryInfo.szVer);
	if(lVersion > lLatestVersion)
		return FALSE;

	CString szLocalFileName = lpFileName;
	ConvertLocalPath(szLocalFileName);

	BOOL bResult = FALSE;

	// 현재 버전과 같은 놈을 요구했다면
	if(lVersion == lLatestVersion)
	{
		if(!csEntryManager.CheckLatestVersion(szLocalFileName, szRemoteFileName))
		{
			csEntryManager.SetReadOnly(szLocalFileName, FALSE);
			bResult = ::CopyFile(szRemoteFileName, szLocalFileName, FALSE);
		}
		else
			bResult = TRUE;
	}
	else
	{
		// 백업 파일 경로를 만든다.
		CString szHistoryFileName = csEntryManager.GetBackupFilePath(szRemoteFileName, lVersion);
		csEntryManager.SetReadOnly(szLocalFileName, FALSE);
		
		CString csLocalPath;
		csEntryManager.GetDirectoryPath(szLocalFileName, csLocalPath);
		csEntryManager.CreateSubDirectory(csLocalPath);
		
		bResult = ::CopyFile(szHistoryFileName, szLocalFileName, FALSE);
	}

	return bResult;
}

INT32 AuRKSingleton::GetCurrentVersion(LPCTSTR lpFileName)
{
	if(!lpFileName)
		return -1;

	CString szRemoteFileName = lpFileName;
	ConvertRemotePath(szRemoteFileName);

	CString szEntryFileName;
	CRKEntryManager csEntryManager;
	csEntryManager.GetDirectoryPath(szRemoteFileName, szEntryFileName);
	csEntryManager.SetEntryParserFileName(szEntryFileName + PATH(ENTRIES));
	
	INT32 lVersion = 0;
	CString szOnlyFileName = GetFileName(szRemoteFileName);
	stRKEntryInfo stRKEntryInfo;
	if(csEntryManager.GetEntryFileInfo(szOnlyFileName, &stRKEntryInfo))
		lVersion = atoi((LPCTSTR)stRKEntryInfo.szVer);
	
	return lVersion;
}