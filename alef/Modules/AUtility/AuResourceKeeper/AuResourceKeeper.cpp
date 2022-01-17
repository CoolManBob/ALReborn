// AuResourceKeeper.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AuResourceKeeper.h"

#include "AuRKMainDlg.h"
#include "AuLoginDlg.h"
#include "AuHistoryDlg.h"
#include "AuRKSingleton.h"
#include "AuSelectDlg.h"
#include "AuRK_API.h"
#include "ProgressDlg.h"

#include <winnetwk.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CAuResourceKeeperApp

BEGIN_MESSAGE_MAP(CAuResourceKeeperApp, CWinApp)
	//{{AFX_MSG_MAP(CAuResourceKeeperApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuResourceKeeperApp construction

CAuResourceKeeperApp::CAuResourceKeeperApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	
	// singleton 초기화
	AuRKSingleton::Instance();
}

CAuResourceKeeperApp::~CAuResourceKeeperApp()
{
	AuRKSingleton::DeleteInstance();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAuResourceKeeperApp object

CAuResourceKeeperApp theApp;

RK_DLL BOOL OpenMainDlg()
{
	if (AuRKSingleton::Instance()->m_bLogin)
	{
//		AuRKSingleton::Instance()->Initialize();

		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		CHAR szBuffer[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, szBuffer);

		AuRKMainDlg MainDlg;
		MainDlg.DoModal();

		::SetCurrentDirectory(szBuffer);
		return TRUE;
	}
	else return FALSE;
}

RK_DLL BOOL OpenLoginDlg()
{
	// JNY TODO : Login 된 상태에서 다시 로그인 했을경우의 처리는???
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AuLoginDlg LoginDlg;

	AuRKSingleton::Instance()->Initialize();

	CHAR szBuffer[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szBuffer);

	if (IDOK == LoginDlg.DoModal())
	{
		::SetCurrentDirectory(szBuffer);
		return LoginDlg.m_bLoginSuccess;
	}

	::SetCurrentDirectory(szBuffer);
	return FALSE;
}

RK_DLL BOOL RKLogout()
{
	AuRKSingleton::Instance()->m_bAdmin = FALSE;
	AuRKSingleton::Instance()->m_bLogin = FALSE;
	AuRKSingleton::Instance()->m_strCurrentPath = "";

	return TRUE;
}

RK_DLL BOOL OpenHistoryDlg(CList<FileInfo, FileInfo>* pList /* = NULL */)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AuHistoryDlg HistoryDlg;
	if(pList)
		HistoryDlg.SetHistoryList(pList);

	HistoryDlg.DoModal();

	return TRUE;
}

RK_DLL BOOL DeleteInstance()
{
	AuRKSingleton::DeleteInstance();
	return TRUE;
}

RK_DLL BOOL OpenFolderDlg()
{
	if (AuRKSingleton::Instance()->m_bLogin)
	{
//		AFX_MANAGE_STATE(AfxGetStaticModuleState());
//		AuFolderDlg FolderDlg;
//		FolderDlg.DoModal();
		return TRUE;
	}
	else return FALSE;
}

RK_DLL LPCTSTR GetCurrentPath()
{
	return AuRKSingleton::Instance()->m_strCurrentPath;
}

RK_DLL BOOL SetRepoRootPath()
{
	if (AuRKSingleton::Instance()->m_bLogin)
	{
//		AuRKSingleton::Instance()->GetRepoPath(
		return TRUE;
	}
	else return FALSE;
}

RK_DLL BOOL CreateRepository()
{
	if (AuRKSingleton::Instance()->m_bLogin)
	{
		if (AuRKSingleton::Instance()->m_bAdmin)
		{
			CHAR szBuffer[MAX_PATH];
			::GetCurrentDirectory(MAX_PATH, szBuffer);
			AuRKSingleton::Instance()->CreateRepository();

			::SetCurrentDirectory(szBuffer);
			return TRUE;
		}
	}
	return FALSE;
}

RK_DLL BOOL SetWorkingFolder()
{
	if (AuRKSingleton::Instance()->m_bLogin)
	{
		CHAR szBuffer[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, szBuffer);

		AuRKSingleton::Instance()->SetWorkingFolder();

		::SetCurrentDirectory(szBuffer);
		return TRUE;
	}
	else return FALSE;
}
/*
RK_DLL BOOL AddFolder()
{
	if (AuRKSingleton::Instance()->m_bLogin)
	{
		AuRKSingleton::Instance()->AddFolder(NULL);
		return TRUE;
	}
	else return FALSE;
}
*/

RK_DLL BOOL CheckIn(LPCTSTR lpFileName)
{
/*
	CProgressDlg dlg;
	dlg.CheckIn(lpFilePath);
	dlg.DoModal();

	return TRUE;
*/

	if (AuRKSingleton::Instance()->m_bLogin || !AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty())
	{
		//CString strPath = lpFileName;
		//strPath.MakeUpper();

		//if (strPath.GetLength() >= AuRKSingleton::Instance()->m_strWorkingFolder.GetLength())
		//{
		//	INT32 iWorkFolder, iPos;
		//	iWorkFolder = AuRKSingleton::Instance()->m_strWorkingFolder.GetLength();
		//	iPos = strPath.GetLength();
		//	strPath.Delete(iWorkFolder, iPos - iWorkFolder);
		//}
		//else return FALSE;

		//if (strPath == AuRKSingleton::Instance()->m_strWorkingFolder)
		//{
			//CString strRootPath = lpFileName;
			//strRootPath.Delete(0,  AuRKSingleton::Instance()->m_strWorkingFolder.GetLength());
			//strRootPath = RKROOT + strRootPath;

			//strRootPath = AuRKSingleton::Instance()->GetFolderPath(strRootPath);
//			INT32 iPos = strRootPath.ReverseFind('\\');
//			strRootPath.Delete(iPos, strRootPath.GetLength() - iPos);

			CString strRepoPath, strLocalPath;
			strRepoPath = strLocalPath = lpFileName;

			strRepoPath = AuRKSingleton::Instance()->GetFolderPath(strRepoPath);
			strLocalPath = AuRKSingleton::Instance()->GetFolderPath(strLocalPath);

			AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
			AuRKSingleton::Instance()->ConvertLocalPath(strLocalPath);

			CRKEntryManager cRKEntryManager;
			cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
			cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));

			CString strFileName = AuRKSingleton::Instance()->GetFileName(CString(lpFileName));

			BOOL bCheckIn = cRKEntryManager.UndoCheckOut(strLocalPath + PATH(strFileName),
				strRepoPath + PATH(strFileName), AuRKSingleton::Instance()->m_strUserID);
			if (bCheckIn) return TRUE;
		//}
	}

	return FALSE;
}

RK_DLL BOOL CheckOut(LPCTSTR lpFileName)
{
	if (AuRKSingleton::Instance()->m_bLogin &&
		!AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() &&
		!AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
	{
		//CString strPath = lpFileName;
		//strPath.MakeUpper();
		//AuRKSingleton::Instance()->ConvertLocalPath(strPath);

		/*
		if (strPath.GetLength() >= AuRKSingleton::Instance()->m_strWorkingFolder.GetLength())
		{
			INT32 iWorkFolder, iPos;
			iWorkFolder = AuRKSingleton::Instance()->m_strWorkingFolder.GetLength();
			iPos = strPath.GetLength();
			strPath.Delete(iWorkFolder, iPos - iWorkFolder);
		}
		else return FALSE;
		*/

		//if (strPath == AuRKSingleton::Instance()->m_strWorkingFolder)
		//{
			//CString strRootPath = lpFileName;
			//strRootPath.Delete(0,  AuRKSingleton::Instance()->m_strWorkingFolder.GetLength());
			//strRootPath = RKROOT + strRootPath;

			//strRootPath = AuRKSingleton::Instance()->GetFolderPath(strRootPath);

//			INT32 iPos = strRootPath.ReverseFind('\\');
//			strRootPath.Delete(iPos, strRootPath.GetLength() - iPos);

			CString strRepoPath, strLocalPath;
			strRepoPath = strLocalPath = lpFileName;

			strRepoPath = AuRKSingleton::Instance()->GetFolderPath(strRepoPath);
			strLocalPath = AuRKSingleton::Instance()->GetFolderPath(strLocalPath);

			AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
			AuRKSingleton::Instance()->ConvertLocalPath(strLocalPath);

			CRKEntryManager cRKEntryManager;
			cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
			cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));

			CString strFileName = AuRKSingleton::Instance()->GetFileName(CString(lpFileName));
//			strFileName.Delete(0, strFileName.ReverseFind('\\') + 1);

			BOOL bCheckOut = cRKEntryManager.CheckOut(strLocalPath + PATH(strFileName),
				strRepoPath + PATH(strFileName), AuRKSingleton::Instance()->m_strUserID);

			if (bCheckOut) return TRUE;
		//}
		//else return FALSE;
	}

	return FALSE;
}

RK_DLL BOOL GetUpdatedFileList(CList<FileInfo, FileInfo> *pList)
{
	if ((AuRKSingleton::Instance()->m_bLogin) &&
//		(!AuRKSingleton::Instance()->m_bAdmin) &&
		(!AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty()) &&
		(!AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()))
	{
		AuRKSingleton::Instance()->GetUpdatedFileList(pList);
		return pList->GetCount();
	}

	return FALSE;
}

RK_DLL BOOL GetNewFileList(CList<FileInfo, FileInfo> *pList)
{
	if ((AuRKSingleton::Instance()->m_bLogin) &&
//		(!AuRKSingleton::Instance()->m_bAdmin) &&
		(!AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty()) &&
		(!AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()))
	{
		AuRKSingleton::Instance()->GetNewFileList(pList);
		return pList->GetCount();
	}

	return FALSE;
}

// 2004.11.16. steeple
RK_DLL BOOL GetUpdatedFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive)
{
	if ((AuRKSingleton::Instance()->m_bLogin) &&
//		(!AuRKSingleton::Instance()->m_bAdmin) &&
		(!AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty()) &&
		(!AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()) &&
		(lpFolderPath))
	{
		AuRKSingleton::Instance()->GetUpdatedFileList2(pList, lpFolderPath, bRecursive);
		return pList->GetCount();
	}

	return FALSE;
}

// 2004.11.16. steeple
RK_DLL BOOL GetNewFileList2(CList<FileInfo, FileInfo> *pList, LPCTSTR lpFolderPath, BOOL bRecursive)
{
	if ((AuRKSingleton::Instance()->m_bLogin) &&
//		(!AuRKSingleton::Instance()->m_bAdmin) &&
		(!AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty()) &&
		(!AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()) &&
		(lpFolderPath))
	{
		AuRKSingleton::Instance()->GetNewFileList2(pList, lpFolderPath, bRecursive);
		return pList->GetCount();
	}

	return FALSE;
}

RK_DLL BOOL GetLatestFile(LPCTSTR lpExistFilePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AuRKSingleton::Instance()->GetLatestFile(lpExistFilePath);
}

RK_DLL BOOL GetLatestFolder(LPCTSTR lpExistPath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AuRKSingleton::Instance()->GetLatestFolder(lpExistPath);
}

RK_DLL BOOL AddFile(LPCTSTR lpFileName, LPCTSTR lpNewFilePath)
{
/*
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return AuRKSingleton::Instance()->AddFile(lpExistFilePath, lpNewFilePath);
*/

	// 경로 얻기
	CString strPathName = AuRKSingleton::Instance()->GetFolderPath(CString(lpFileName));
	
	// 파일 이름 얻기
	CString strFileName = AuRKSingleton::Instance()->GetFileName(CString(lpFileName));
	CString strFileName2 = strFileName;

	// Repository 경로 얻기
	CString strRepoPath = strPathName;
	strRepoPath.MakeUpper();
	if (!strRepoPath.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT)) return FALSE;
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

//	CProgressDlg csProgressDlg;
//	csProgressDlg.CopyFile(

	CRKEntryManager cAddFile;
	cAddFile.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));
	if (!cAddFile.GetEntryFileInfo(strRepoPath + PATH(strFileName)))
	{
		// 같은 파일이 존재하지 않을때만 등록
		cAddFile.AddEntry(strPathName + PATH(strFileName2));

		return ::CopyFileEx(strPathName + PATH(strFileName2), strRepoPath + PATH(strFileName2), NULL, NULL, NULL, 0);
	}

	return FALSE;
}

RK_DLL BOOL AddFolder(LPCTSTR lpExistPath, LPCTSTR lpNewPath)
{
	// Repository에 등록할 폴더 선택
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CHAR szBuffer[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szBuffer);

	BOOL bResult = AuRKSingleton::Instance()->AddFile(lpExistPath, lpNewPath);

	::SetCurrentDirectory(szBuffer);

	return bResult;
}

RK_DLL BOOL GetWorkingFolder()
{
	return !AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty();
}

RK_DLL LPCTSTR GetWorkingFolderString()
{
	return (LPCTSTR)(AuRKSingleton::Instance()->m_strWorkingFolder);
}

RK_DLL BOOL OpenSelectDlg(CList<FileInfo, FileInfo> *pUpdatedFileList, CList<FileInfo, FileInfo> *pSelectedFileList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AuSelectDlg SelectDlg;	
	SelectDlg.SetFileList(pUpdatedFileList, pSelectedFileList);
	
	if (IDOK == SelectDlg.DoModal())
	{
		return TRUE;
	}

	return FALSE;
}

// 2004.11.18. steeple
// lpFileName 경로는 $RK 로 시작한다.
RK_DLL BOOL FileDownLoad(LPCTSTR lpFileName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty() ||
		!lpFileName)
		return FALSE;

	return AuRKSingleton::Instance()->FileDownLoad(lpFileName);
}

RK_DLL BOOL FileUpload(LPCTSTR lpFileName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strLocalFileName = lpFileName;
	CString strRepoFileName = lpFileName;

	strLocalFileName.MakeUpper();
	strRepoFileName.MakeUpper();
	AuRKSingleton::Instance()->ConvertLocalPath(strLocalFileName);
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoFileName);

	CString strRepoPath = strRepoFileName;
	INT32 iPos = strRepoPath.ReverseFind('\\');
	strRepoPath.Delete(iPos, strRepoPath.GetLength() - iPos);

	CRKEntryManager cRKEntryManager;
	cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
	cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));

	CString strFileName = lpFileName;
	strFileName.Delete(0, strFileName.ReverseFind('\\') + 1);

	if (AuRKSingleton::Instance()->IsFileExist(strRepoFileName))
		return cRKEntryManager.CheckIn(strLocalFileName, strRepoFileName, AuRKSingleton::Instance()->m_strUserID);
	else
	{
		// lpFileName 을 넘겨주는 걸로 바꿈.. -_-;;
		return AuRKSingleton::Instance()->AddFile(lpFileName, lpFileName);
	}
}

// 2004.11.16. steeple
// 넘겨받는 경로는 $RK 로 시작하면 된다.
RK_DLL BOOL FolderUpload(LPCTSTR lpFolderName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty() ||
		!lpFolderName)
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString szLocalPath = lpFolderName;
	AuRKSingleton::Instance()->ConvertLocalPath(szLocalPath);

	return AuRKSingleton::Instance()->AddFolder((LPCTSTR)szLocalPath, NULL);
}

// 2004.11.16. steeple
// 넘겨받는 경로는 $RK 로 시작하면 된다.
RK_DLL BOOL DeleteFileInRepository(LPCTSTR lpFileName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty() ||
		!lpFileName)
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString szRepositoryFileName = lpFileName;
	AuRKSingleton::Instance()->ConvertRemotePath(szRepositoryFileName);

	if(!AuRKSingleton::Instance()->IsFileExist((LPCSTR)szRepositoryFileName))
		return FALSE;

	// 디렉토리라면
	if(AuRKSingleton::Instance()->IsDirectoryInRepository((LPCTSTR)szRepositoryFileName))
	{
		CProgressDlg dlg;
		dlg.DeleteFolder(szRepositoryFileName);
		if(IDOK == dlg.DoModal())
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		CString szRepositoryPath = szRepositoryFileName;
		INT32 iPos = szRepositoryPath.ReverseFind('\\');
		szRepositoryPath.Delete(iPos, szRepositoryPath.GetLength() - iPos);

		CRKEntryManager cRKEntryManager;
		cRKEntryManager.SetEntryParserFileName(szRepositoryPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(szRepositoryPath + PATH(ENTRIES));

		return cRKEntryManager.RemoveEntry(szRepositoryFileName, AuRKSingleton::Instance()->m_strUserID);
	}

	return FALSE;
}

RK_DLL BOOL Lock(LPCTSTR lpFileName)
{
	return CheckOut(lpFileName);
}

RK_DLL BOOL UnLock(LPCTSTR lpFileName)
{
	return CheckIn(lpFileName);
}

RK_DLL BOOL Login(LPCTSTR szUserID, LPCTSTR szPassword, LPCTSTR szWorkingFolderPath, LPCTSTR szRepositoryPath)
{
	if(!szUserID || !szPassword || !szWorkingFolderPath || !szRepositoryPath)
		return FALSE;

	return AuRKSingleton::Instance()->Login(szUserID, szPassword, szWorkingFolderPath, szRepositoryPath);
}

RK_DLL BOOL IsLogin()
{
	return (AuRKSingleton::Instance()->m_bLogin || AuRKSingleton::Instance()->m_bAdmin);
}

RK_DLL BOOL IsAdmin()
{
	return (AuRKSingleton::Instance()->m_bLogin && AuRKSingleton::Instance()->m_bAdmin);
}

RK_DLL LPCTSTR GetUserID()
{
	return (LPCTSTR)(AuRKSingleton::Instance()->m_strUserID);
}

RK_DLL BOOL Logout(LPCTSTR szUserID /* = NULL */)
{
	return AuRKSingleton::Instance()->Logout(szUserID);
}

// 2004.11.19. steeple
RK_DLL BOOL AddUser(LPCTSTR lpUserID, LPCTSTR lpPassword, BOOL bAdmin, LPCTSTR lpRepositoryPath)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		!AuRKSingleton::Instance()->m_bAdmin ||
		!lpUserID || !lpPassword)
		return FALSE;

	CString szUserID = lpUserID;
	CString szPassword = lpPassword;
	CString szRepository = lpRepositoryPath ? lpRepositoryPath : AuRKSingleton::Instance()->m_strRepoRootPath;

	if(szUserID.IsEmpty() || szPassword.IsEmpty() || szRepository.IsEmpty())
		return FALSE;

	CRKUserManager csUserManager;
	csUserManager.SetRemotePath(szRepository);

	// 기존아이디가 있으면 낭패
	CList<stUserInfo, stUserInfo> csUserList;
	csUserManager.GetUserList(csUserList, szRepository);

	POSITION p = csUserList.GetHeadPosition();
	while(p)
	{
		stUserInfo stInfo = csUserList.GetNext(p);
		if(szUserID.Compare((LPCTSTR)stInfo.szUserName) == 0)
			return FALSE;
	}

	return csUserManager.AddUser(szUserID, szPassword, szRepository, bAdmin);
}

// 2004.11.19. steeple
RK_DLL BOOL DeleteUser(LPCTSTR lpUserID, LPCTSTR lpRepositoryPath)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		!AuRKSingleton::Instance()->m_bAdmin ||
		!lpUserID)
		return FALSE;

	CString szUserID = lpUserID;
	CString szRepository = lpRepositoryPath ? lpRepositoryPath : AuRKSingleton::Instance()->m_strRepoRootPath;

	if(szUserID.IsEmpty() || szRepository.IsEmpty())
		return FALSE;

	CRKUserManager csUserManager;
	return csUserManager.RemoveUser(szUserID, szRepository);
}

// 2004.11.19. steeple
RK_DLL BOOL ChangePassword(LPCTSTR lpUserID, LPCTSTR lpNewPassword, LPCTSTR lpRepositoryPath)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		!lpUserID || !lpNewPassword)
		return FALSE;

	CString szUserID = lpUserID;
	CString szPassword = lpNewPassword;
	CString szRepository = lpRepositoryPath ? lpRepositoryPath : AuRKSingleton::Instance()->m_strRepoRootPath;

	if(szUserID.IsEmpty() || szPassword.IsEmpty() || szRepository.IsEmpty())
		return FALSE;

	CRKUserManager csUserManager;
	csUserManager.SetRemotePath(szRepository);

	// 기존유저가 Admin 인지 알아내야 함.
	CList<stUserInfo, stUserInfo> csUserList;
	if(!csUserManager.GetUserList(csUserList, szRepository))
		return FALSE;

	POSITION p = csUserList.GetHeadPosition();
	while(p)
	{
		stUserInfo stInfo = csUserList.GetNext(p);
		if(szUserID.Compare((LPCTSTR)stInfo.szUserName) == 0)
		{
			// 암호를 바꿔준다.
			return csUserManager.AddUser(szUserID, szPassword, szRepository, stInfo.bAdmin);

			// 지우고 새로 추가 -_-;;
			//csUserManager.RemoveUser(szUserID, szRepository);
			//csUserManager.AddUser(szUserID, szPassword, szRepository, stInfo.bAdmin);
		}
	}

	return FALSE;
}

// 2004.11.19. steeple
RK_DLL BOOL SetAdmin(LPCTSTR lpUserID, BOOL bAdmin, LPCTSTR lpRepositoryPath)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		!AuRKSingleton::Instance()->m_bAdmin ||
		!lpUserID)
		return FALSE;

	CString szUserID = lpUserID;
	CString szRepository = lpRepositoryPath ? lpRepositoryPath : AuRKSingleton::Instance()->m_strRepoRootPath;

	if(szUserID.IsEmpty() || szRepository.IsEmpty())
		return FALSE;

	CRKUserManager csUserManager;
	csUserManager.SetRemotePath(szRepository);

	// 기존유저가 Admin 인지 알아내야 함.
	CList<stUserInfo, stUserInfo> csUserList;
	if(!csUserManager.GetUserList(csUserList, szRepository))
		return FALSE;

	POSITION p = csUserList.GetHeadPosition();
	while(p)
	{
		stUserInfo stInfo = csUserList.GetNext(p);
		if(szUserID.Compare((LPCTSTR)stInfo.szUserName) == 0)
		{
			// Admin 을 세팅해준다.
			return csUserManager.SetAdmin(szUserID, bAdmin, szRepository);
		}
	}

	return FALSE;
}

RK_DLL BOOL GetLockOwner(LPCTSTR lpFileName, CString &rOwnerName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	CString strRepoPath = lpFileName;
	strRepoPath = AuRKSingleton::Instance()->GetFolderPath(strRepoPath);
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

	CRKEntryManager cRKEntryManager;
	cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));

	CString strFileName = AuRKSingleton::Instance()->GetFileName(CString(lpFileName));
	stRKEntryInfo stLockOnwer;
	if (cRKEntryManager.GetEntryFileInfo(strFileName, &stLockOnwer))
	{
		if (stLockOnwer.szOwner.IsEmpty()) return FALSE;
		rOwnerName = stLockOnwer.szOwner;
		return TRUE;
	}
	
	return FALSE;
}

RK_DLL BOOL IsLock(LPCTSTR lpFileName)
{
	return GetLockOwner(lpFileName, CString());
}

RK_DLL BOOL GetLocalName(CString &rLocalName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	rLocalName = AuRKSingleton::Instance()->m_strUserID;

	return TRUE;
}

RK_DLL BOOL IsLatestFile(LPCTSTR lpFileName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	CString strRepoPath = lpFileName;
	strRepoPath.MakeUpper();
	//if (!strRepoPath.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT)) return FALSE;
	strRepoPath.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT);
	CString strLocalPath = strRepoPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
	AuRKSingleton::Instance()->ConvertLocalPath(strLocalPath);

	CRKEntryManager cRKEntryManager;
	return cRKEntryManager.CheckLatestVersion(strLocalPath, strRepoPath);
}

RK_DLL VOID ConnectRemoteDrive(LPCTSTR lpRemotePCName, LPCTSTR lpLocalDriveName, LPCTSTR lpUserName, LPCTSTR lpPassword)
{
	NETRESOURCE NetInfo;
	NetInfo.dwType = RESOURCETYPE_ANY;
	NetInfo.lpRemoteName = const_cast<CHAR*>(lpRemotePCName);
	NetInfo.lpLocalName = const_cast<CHAR*>(lpLocalDriveName);
	NetInfo.lpProvider = NULL;
	DWORD dwResult = WNetAddConnection2(&NetInfo, lpPassword, lpUserName, CONNECT_UPDATE_PROFILE);
	switch(dwResult)
	{
	case NO_ERROR:
		::AfxMessageBox("아싸 성공");
		break;
	default:
		::AfxMessageBox("뷁!!");
		break;
	}
}

RK_DLL LPCTSTR GetRepoRootPath()
{
	return (AuRKSingleton::Instance()->m_strRepoRootPath + PATH("RK"));
}

RK_DLL BOOL GetFileList(CList<FileInfo, FileInfo> *pList)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CProgressDlg dlg;
	dlg.GetFileList(pList);
	if (IDOK == dlg.DoModal()) return TRUE;
	else return FALSE;
}

RK_DLL BOOL IsExistFileToRemote(LPCTSTR lpFileName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	CString strRepoPath = lpFileName;
	strRepoPath.MakeUpper();
	//if (!strRepoPath.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT)) return FALSE;
	strRepoPath.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT);
	CString strLocalPath = strRepoPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

	return AuRKSingleton::Instance()->IsFileExist(strRepoPath);
}

// 2004.11.18. steeple
// Local 에 있는 info.rk 를 읽는다. lpBuffer 가 NULL 이 아니면 WorkingFolder 를 넣어준다.
RK_DLL BOOL LoadInfoFile(LPTSTR lpBuffer)
{
	CHAR szBuffer[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szBuffer);

	CString szInfoFilePath;
	szInfoFilePath.Format("%s\\%s", szBuffer, INFO_FILENAME);

	CRKRepository csRepository;
	csRepository.SetInfoFileName(szInfoFilePath);
	if(!csRepository.LoadInfoFile())
		return FALSE;

	stRepositoryData* pstRepository = csRepository.GetRepositoryData(CString(REPONAME));
	if(!pstRepository)
		return FALSE;

	if(lpBuffer)
		strcpy(lpBuffer, (LPCTSTR)pstRepository->szWorkingFolder);

	return TRUE;
}

// 2004.11.18. steeple
// Local 에 info.rk 를 만든다. WorkingFolder 저장용
RK_DLL BOOL SaveInfoFile(LPCTSTR lpWorkingFolder)
{
	if(!lpWorkingFolder || strlen(lpWorkingFolder) == 0)
		return FALSE;

	CHAR szBuffer[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szBuffer);

	CString szInfoFilePath;
	szInfoFilePath.Format("%s\\%s", szBuffer, INFO_FILENAME);

	CRKRepository csRepository;
	csRepository.SetInfoFileName(szInfoFilePath);

	return csRepository.AddRepository(CString(REPONAME), CString(REPONAME), CString(lpWorkingFolder));
}

// 2004.11.25. steeple
// History List 를 받는다.
RK_DLL BOOL GetHistoryList(LPCTSTR lpFileName, CList<FileInfo, FileInfo>* pList)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	if(!lpFileName || !pList || strlen(lpFileName) == 0)
		return FALSE;

	return AuRKSingleton::Instance()->GetHistoryList(lpFileName, pList);
}

// 2004.11.25. steeple
// History File 을 받는다.
RK_DLL BOOL GetHistoryFile(LPCTSTR lpFileName, INT32 lVersion)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return FALSE;

	if(!lpFileName)
		return FALSE;

	return AuRKSingleton::Instance()->GetHistoryFile(lpFileName, lVersion);
}

// 2004.11.25. steeple
// 해당 파일의 현재 버전을 리턴한다.
RK_DLL INT32 GetCurrentVersion(LPCTSTR lpFileName)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return -1;

	if(!lpFileName)
		return -1;

	return AuRKSingleton::Instance()->GetCurrentVersion(lpFileName);
}

// 2005.03.01. steeple
RK_DLL BOOL DirectoryRollBack(CString strRemotePathName, CString strLocalPathName, CString szUser, CString szDate)
{
	if (!AuRKSingleton::Instance()->m_bLogin || 
		AuRKSingleton::Instance()->m_strRepoRootPath.IsEmpty() ||
		AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty())
		return -1;

	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	AuRKSingleton::Instance()->ConvertRemotePath(strRemotePathName);
	cRKEntryManager.GetEntryList(strRemotePathName + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);
		CString strExist = strRemotePathName + PATH(stRKEntryInfo.szFileName);
		CString strNew = strLocalPathName + PATH(stRKEntryInfo.szFileName);

		if (RK_FOLDER == stRKEntryInfo.szType)
		{
			// 로컬에 디렉토리 생성해준다.
			cRKEntryManager.CreateSubDirectory(strNew);

			DirectoryRollBack(strExist, strNew, szUser, szDate);
		}
		else
		{
			FileRollBack(strExist, strNew, szUser, szDate);
		}
	}

	return TRUE;
}

RK_DLL BOOL FileRollBack(CString strRemotePathName, CString strLocalPathName, CString szUser, CString szDate)
{
	INT32 lCurrentVer = GetCurrentVersion((LPCTSTR)strRemotePathName);
	if(lCurrentVer <= 0)
		return FALSE;

	CList<FileInfo, FileInfo> cList;
	GetHistoryList((LPCTSTR)strRemotePathName, &cList);

	CRKEntryManager cRKEntryManager;

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		FileInfo fileInfo = cList.GetNext(p);

		// History 파일에서 버전과 현재 버전을 비교
		if(fileInfo.m_iVersion == lCurrentVer)
		{
			// 올린이와 날짜를 비교
			if(szUser.Compare(fileInfo.m_strOwner) == 0 &&
				szDate.Compare(fileInfo.m_strDate.Left(szDate.GetLength())) == 0)
			{
				// 로컬로 가져온다.
				strRemotePathName.Replace("F:\\RM\\RK", "$RK");
				GetHistoryFile((LPCTSTR)strRemotePathName, lCurrentVer - 1);
				break;
			}
		}
	}

    return TRUE;
}
