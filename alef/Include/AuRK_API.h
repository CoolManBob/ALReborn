#pragma once

#ifdef RK_IMPORT
#define RK_DLL extern "C" __declspec(dllimport)
#else
#define RK_DLL extern "C" __declspec(dllexport)
#endif

#include "RKDefine.h"

//////////////////////////////////////////////////////////////////////////
//
// 2004.11.16. steeple
//
// 경로 포함 이라는 주석이 달린 놈은
//
// $RK\모시깽이\모시깽이.ini
//
// 이런식으로 경로를 넘겨주면 된다.
//
//


typedef struct _FileInfo
{
	CString m_strFileName;
	CString m_strDate;
	CString m_strOwner;
	INT32	m_iVersion;
} FileInfo;

//////////////////////////////////////////////////////////////////////////
// 2004.11.24. steeple
// 
// 새롭게 추가되거나 검증된 API. (앞으로 주로 쓰일 놈들)
// 이놈들 위주로 안의 소스들도 수정하였기 때문에,
// 쪼~~ 밑에 있는 오래된 API 는 혹 동작 안할 수도 있음. -_-;;
// 뭐 그렇습니다. 캬하핫!
//

// ex : ConnectRemoteDrive("\\\\alefserver\\ResourceKeeper", "R:", "alef", "archlord");
RK_DLL VOID ConnectRemoteDrive(LPCTSTR lpRemotePCName, LPCTSTR lpLocalDriveName, LPCTSTR lpUserName, LPCTSTR lpPassword);

// 2004.11.17. steeple. 경로는 드라이브 절대 경로가 와야함.
RK_DLL BOOL Login(LPCTSTR szUserID, LPCTSTR szPassword, LPCTSTR szWorkingFolderPath, LPCTSTR szRepositoryPath);

RK_DLL BOOL Logout(LPCTSTR szUserID = NULL);

RK_DLL BOOL IsLogin();

RK_DLL BOOL IsAdmin();

RK_DLL LPCTSTR GetUserID();

RK_DLL BOOL AddUser(LPCTSTR lpUserID, LPCTSTR lpPassword, BOOL bAdmin = FALSE, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL DeleteUser(LPCTSTR lpUserID, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL ChangePassword(LPCTSTR lpUserID, LPCTSTR lpNewPassword, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL SetAdmin(LPCTSTR lpUserID, BOOL bAdmin, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL Lock(LPCTSTR lpFileName);

RK_DLL BOOL UnLock(LPCTSTR lpFileName);

RK_DLL BOOL IsLock(LPCTSTR lpFileName);

RK_DLL BOOL GetLockOwner(LPCTSTR lpFileName, CString &rOwnerName);

RK_DLL BOOL LoadInfoFile(LPTSTR lpBuffer);	// Local 에 있는 info.rk 를 읽는다. lpBuffer 에 읽은 WorkingFolder 를 넣어준다.

RK_DLL BOOL SaveInfoFile(LPCTSTR lpWorkingFolder);	// Local 에 info.rk 를 만든다. WorkingFolder 저장용.

RK_DLL BOOL GetUpdatedFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive = TRUE);	// 넘겨준 폴더안을 검사. 폴더는 경로 포함

RK_DLL BOOL GetNewFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive = TRUE);	// 넘겨준 폴더안을 검사. 폴더는 경로 포함

RK_DLL BOOL OpenSelectDlg(CList<FileInfo, FileInfo> *pUpdatedFileList, CList<FileInfo, FileInfo> *pSelectedFileList);

RK_DLL BOOL FileDownLoad(LPCTSTR lpFileName);	// 경로 포함. 2004.11.18. steeple 

RK_DLL BOOL FileUpload(LPCTSTR lpFileName);		// 경로 포함

RK_DLL BOOL FolderUpload(LPCTSTR lpFolderName);	// 경로 포함. 2004.11.16. steeple

RK_DLL BOOL DeleteFileInRepository(LPCTSTR lpFileName);	// 경로 포함. 폴더도 된다. 2004.11.16. steeple

RK_DLL BOOL IsLatestFile(LPCTSTR lpFileName);

RK_DLL BOOL GetLatestFile(LPCTSTR lpExistFilePath);	// 경로 포함

RK_DLL BOOL GetFileList(CList<FileInfo, FileInfo> *pList);

RK_DLL BOOL IsExistFileToRemote(LPCTSTR lpFileName);

RK_DLL LPCTSTR GetWorkingFolderString();

RK_DLL BOOL GetHistoryList(LPCTSTR lpFileName, CList<FileInfo, FileInfo>* pList);

RK_DLL BOOL GetHistoryFile(LPCTSTR lpFileName, INT32 lVersion);

RK_DLL INT32 GetCurrentVersion(LPCTSTR lpFileName);

RK_DLL BOOL OpenHistoryDlg(CList<FileInfo, FileInfo>* pList = NULL);

RK_DLL BOOL DirectoryRollBack(CString strRemotePathName, CString strLocalPathName, CString szUser, CString szDate);

RK_DLL BOOL FileRollBack(CString strRemotePathName, CString strLocalPathName, CString szUser, CString szDate);

RK_DLL BOOL DeleteInstance();





//////////////////////////////////////////////////////////////////////////
// 내부적으로 쓰이는 API
// 원래는 외부용으로 제작하였으나, 세월이 지나면서 내부적으로 쓰이기 때문에
// 외부에서 부르면 낭패가 될 수도 있음.
RK_DLL BOOL CheckIn(LPCTSTR lpFilePath);		// 경로 포함

RK_DLL BOOL CheckOut(LPCTSTR lpFileName);		// 경로 포함







//////////////////////////////////////////////////////////////////////////
// 오래된 API
// 보통 작동할테지만... 작동안할 수도 있으니 주의 요망!!!
RK_DLL BOOL OpenMainDlg();

RK_DLL BOOL OpenLoginDlg();

RK_DLL BOOL RKLogout();

RK_DLL BOOL OpenFolderDlg();

RK_DLL LPCTSTR GetCurrentPath();

RK_DLL BOOL GetWorkingFolder();

RK_DLL BOOL SetRepoRootPath();

RK_DLL LPCTSTR GetRepoRootPath();

RK_DLL BOOL CreateRepository();

RK_DLL BOOL SetWorkingFolder();

RK_DLL BOOL GetLatestFolder(LPCTSTR lpExistPath);

RK_DLL BOOL AddFile(LPCTSTR lpExistFilePath, LPCTSTR lpNewFilePath = NULL);		// 경로 포함

RK_DLL BOOL AddFolder(LPCTSTR lpExistPath, LPCTSTR lpNewPath = NULL);	// 경로 포함

RK_DLL BOOL GetUpdatedFileList(CList<FileInfo, FileInfo> *pList);

RK_DLL BOOL GetNewFileList(CList<FileInfo, FileInfo> *pList);

RK_DLL BOOL GetLocalName(CString &rLocalName);

