
/**************************************************************************************************

작성일: 2009.10.06
작성자: 진혜진

**************************************************************************************************/

#pragma once
#include "include.h"
#include "FTPFileDownLoader.h"

class CListManager  
{
public:
	CListManager();	
	virtual ~CListManager();

	//				샵 리스트 다운로드 & 관리 를 위한 정보를 설정한다.
	void			SetListManagerInfo(DownloaderType type, 
									   TCHAR* ServerIP, 
									   TCHAR* UserID, 
									   TCHAR* Pwd, 
									   TCHAR* RemotePath, 
									   TCHAR* LocalPath, 
									   CListVersionInfo Version,
									   DWORD dwDownloadMaxTime = 0);

	void			SetListManagerInfo(DownloaderType type, 
									   TCHAR* ServerIP, 
									   unsigned short PortNum, 
									   TCHAR* UserID, 
									   TCHAR* Pwd, 
									   TCHAR* RemotePath, 
									   TCHAR* LocalPath, 
									   FTP_SERVICE_MODE ftpMode, 
									   CListVersionInfo Version,
									   DWORD dwDownloadMaxTime = 0);

	WZResult		LoadScriptList(bool bDonwLoad);

protected:
	bool			IsScriptFileExist();
	tstring			GetScriptPath();
	void			DeleteScriptFiles();

	WZResult		FileDownLoad();
	WZResult		FileDownLoadImpl();
	static unsigned int __stdcall RunFileDownLoadThread(LPVOID pParam);

	virtual WZResult LoadScript(bool bDonwLoad) = 0;

	CListManagerInfo			m_ListManagerInfo;
	std::vector<tstring>		m_vScriptFiles;
	WZResult					m_Result;
	CFTPFileDownLoader*			m_pFTPDownLoader;
};
